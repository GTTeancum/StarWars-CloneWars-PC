/*
 * xiso.c -- first run: unpack the player's Xbox disc image beside the .exe.
 *
 * The game reads default.xbe, data.zwp and the Addon/Bins/Data/Movies folders
 * from the folder the .exe sits in (resolve_dirs in main.c). When default.xbe
 * is not there, cw_xiso_prepare looks for a disc image in that folder (*.iso,
 * *.xiso) or asks for one, checks that it is Star Wars: The Clone Wars, and
 * unpacks it there once, with a progress window. Later runs find default.xbe
 * and start straight away; the image itself is left alone.
 *
 * The image's file system is XDVDFS: 2048-byte sectors, the volume descriptor
 * at sector 32 ("MICROSOFT*XBOX*MEDIA"), each directory a binary tree of
 * entries (left/right child offsets in dwords, start sector, size, attributes,
 * name). Trimmed images ("xiso", extract-xiso) start the volume at byte 0;
 * full disc dumps (redump) carry it at a game-partition offset per disc type.
 * default.xbe is written last, so an interrupted unpack is redone next run.
 */
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int cw_headless_muted(void);

#define SECTOR        2048ull
#define XDVD_MAGIC    "MICROSOFT*XBOX*MEDIA"
#define TITLE_ID      0x4C410004u            /* SW: The Clone Wars */
#define XBE_BUILD     0x3E8CED27u            /* USA release: the build this .exe is translated from */
#define MAX_DEPTH     32

typedef struct {
    HANDLE   f;
    uint64_t base;                           /* byte offset of the XDVDFS volume */
    const char *dest;
    uint64_t total, done;                    /* bytes, for the progress bar */
    int      files;
    char     err[512];
    uint32_t root_sector, root_size;         /* from the volume descriptor */
    uint32_t xbe_sector, xbe_size;           /* default.xbe in the root */
} xiso_t;

static int read_at(xiso_t *x, uint64_t off, void *buf, DWORD n)
{
    LARGE_INTEGER li;
    DWORD got = 0;
    li.QuadPart = (LONGLONG)(x->base + off);
    if (!SetFilePointerEx(x->f, li, NULL, FILE_BEGIN)) return 0;
    return ReadFile(x->f, buf, n, &got, NULL) && got == n;
}

/* A name from the image is used as a path component: no separators, no
 * device letters, no "." / "..". */
static int safe_name(const char *s)
{
    if (!*s || !strcmp(s, ".") || !strcmp(s, "..")) return 0;
    for (; *s; s++)
        if (*s == '\\' || *s == '/' || *s == ':' || (unsigned char)*s < 0x20) return 0;
    return 1;
}

/* Walk one directory. pass 0 counts bytes and finds default.xbe in the root;
 * pass 1 writes everything except default.xbe. */
static int walk(xiso_t *x, uint32_t sector, uint32_t size, const char *rel, int depth, int pass);

static int extract_file(xiso_t *x, const char *path, uint32_t sector, uint32_t size)
{
    static unsigned char buf[1 << 20];
    HANDLE out = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    uint64_t off = (uint64_t)sector * SECTOR;
    uint32_t left = size;
    if (out == INVALID_HANDLE_VALUE) {
        snprintf(x->err, sizeof x->err, "Could not create\n%s\n\n(error %lu)", path, GetLastError());
        return 0;
    }
    while (left) {
        DWORD n = left < sizeof buf ? left : (DWORD)sizeof buf, w = 0;
        if (!read_at(x, off, buf, n)) {
            snprintf(x->err, sizeof x->err, "The disc image ended early or could not be read.");
            CloseHandle(out);
            return 0;
        }
        if (!WriteFile(out, buf, n, &w, NULL) || w != n) {
            snprintf(x->err, sizeof x->err, "Could not write\n%s\n\nIs the disk full?", path);
            CloseHandle(out);
            return 0;
        }
        off += n; left -= n; x->done += n;
    }
    CloseHandle(out);
    x->files++;
    return 1;
}

static int walk_entry(xiso_t *x, const unsigned char *dir, uint32_t size, uint32_t off,
                      const char *rel, int depth, int pass, int guard)
{
    uint16_t left, right;
    uint32_t sector, len;
    uint8_t attr, nlen;
    char name[256], path[MAX_PATH * 2], sub[MAX_PATH];
    if (guard > 4096) return 1;                          /* malformed tree */
    if (off + 14 > size) return 1;
    left  = (uint16_t)(dir[off] | dir[off + 1] << 8);
    right = (uint16_t)(dir[off + 2] | dir[off + 3] << 8);
    if (left == 0xFFFF && right == 0xFFFF) return 1;     /* sector padding */
    memcpy(&sector, dir + off + 4, 4);
    memcpy(&len, dir + off + 8, 4);
    attr = dir[off + 12];
    nlen = dir[off + 13];
    if (off + 14 + nlen > size) return 1;
    memcpy(name, dir + off + 14, nlen);
    name[nlen] = 0;

    if (left && !walk_entry(x, dir, size, (uint32_t)left * 4, rel, depth, pass, guard + 1)) return 0;
    if (safe_name(name)) {
        snprintf(sub, sizeof sub, "%s%s%s", rel, *rel ? "\\" : "", name);
        snprintf(path, sizeof path, "%s\\%s", x->dest, sub);
        if (attr & 0x10) {                               /* directory */
            if (pass == 1 && !CreateDirectoryA(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
                snprintf(x->err, sizeof x->err, "Could not create the folder\n%s", path);
                return 0;
            }
            if (len && !walk(x, sector, len, sub, depth + 1, pass)) return 0;
        } else if (depth == 0 && !_stricmp(name, "default.xbe")) {
            x->xbe_sector = sector; x->xbe_size = len;
            if (pass == 0) x->total += len;
        } else if (pass == 0) {
            x->total += len;
        } else if (!extract_file(x, path, sector, len)) {
            return 0;
        }
    }
    if (right && !walk_entry(x, dir, size, (uint32_t)right * 4, rel, depth, pass, guard + 1)) return 0;
    return 1;
}

static int walk(xiso_t *x, uint32_t sector, uint32_t size, const char *rel, int depth, int pass)
{
    unsigned char *dir;
    int ok;
    if (depth > MAX_DEPTH || size > (16u << 20)) {
        snprintf(x->err, sizeof x->err, "The disc image's folder structure is damaged.");
        return 0;
    }
    dir = (unsigned char *)malloc(size);
    if (!dir || !read_at(x, (uint64_t)sector * SECTOR, dir, size)) {
        free(dir);
        snprintf(x->err, sizeof x->err, "The disc image could not be read.");
        return 0;
    }
    ok = walk_entry(x, dir, size, 0, rel, depth, pass, 0);
    free(dir);
    return ok;
}

/* ── image checks ──────────────────────────────────────────── */

static int open_image(xiso_t *x, const char *iso)
{
    static const uint64_t bases[] = { 0, 0x18300000ull, 0x0FD90000ull, 0x02080000ull };
    char magic[20];
    unsigned i;
    x->f = CreateFileA(iso, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (x->f == INVALID_HANDLE_VALUE) {
        snprintf(x->err, sizeof x->err, "Could not open\n%s", iso);
        return 0;
    }
    for (i = 0; i < sizeof bases / sizeof bases[0]; i++) {
        x->base = bases[i];
        if (read_at(x, 32 * SECTOR, magic, 20) && !memcmp(magic, XDVD_MAGIC, 20)
            && read_at(x, 32 * SECTOR + 0x14, &x->root_sector, 4)
            && read_at(x, 32 * SECTOR + 0x18, &x->root_size, 4))
            return 1;
    }
    snprintf(x->err, sizeof x->err, "%s\n\nis not an Xbox disc image.", iso);
    return 0;
}

/* default.xbe's certificate: the title ID says which game the disc is, and
 * the header's link time which release -- the .exe carries the code of one
 * build, so another region's default.xbe would not match it. Returns 1 for
 * this release, -1 for another release of the game, 0 for another game. */
static int check_title(xiso_t *x, char *title, size_t n)
{
    unsigned char hdr[0x1000];
    uint32_t base, cert, id, stamp;
    int k;
    title[0] = 0;
    if (!x->xbe_size || x->xbe_size < sizeof hdr ||
        !read_at(x, (uint64_t)x->xbe_sector * SECTOR, hdr, sizeof hdr) || memcmp(hdr, "XBEH", 4))
        return 0;
    memcpy(&base, hdr + 0x104, 4);
    memcpy(&cert, hdr + 0x118, 4);
    if (cert < base || cert - base + 0x0C + 80 > sizeof hdr) return 0;
    for (k = 0; k < 40 && (size_t)k < n - 1; k++) {
        uint16_t c = (uint16_t)(hdr[cert - base + 0x0C + 2 * k] | hdr[cert - base + 0x0D + 2 * k] << 8);
        if (!c) break;
        title[k] = c < 0x80 ? (char)c : '?';
    }
    title[k] = 0;
    memcpy(&id, hdr + cert - base + 8, 4);
    memcpy(&stamp, hdr + 0x114, 4);
    if (id != TITLE_ID) return 0;
    return stamp == XBE_BUILD ? 1 : -1;
}

/* ── progress window ───────────────────────────────────────── */

static volatile LONG s_state;               /* 0 running, 1 done, 2 failed */
static xiso_t s_x;

static DWORD WINAPI unpack_thread(LPVOID p)
{
    char path[MAX_PATH * 2];
    (void)p;
    snprintf(path, sizeof path, "%s\\default.xbe", s_x.dest);
    s_state = walk(&s_x, s_x.root_sector, s_x.root_size, "", 0, 1)
              && extract_file(&s_x, path, s_x.xbe_sector, s_x.xbe_size) ? 1 : 2;
    return 0;
}

static LRESULT CALLBACK progress_proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == WM_TIMER) { InvalidateRect(h, NULL, TRUE); return 0; }
    if (m == WM_CLOSE) return 0;                 /* runs to completion */
    if (m == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        RECT r, bar;
        char text[160];
        int pct = s_x.total ? (int)(s_x.done * 100 / s_x.total) : 0;
        GetClientRect(h, &r);
        SetBkMode(dc, TRANSPARENT);
        snprintf(text, sizeof text, "Unpacking your disc image (first run only)... %d%%", pct);
        r.top += 16; r.left += 16;
        DrawTextA(dc, text, -1, &r, DT_LEFT | DT_TOP | DT_SINGLELINE);
        GetClientRect(h, &r);
        bar.left = 16; bar.right = r.right - 16; bar.top = 48; bar.bottom = 68;
        FrameRect(dc, &bar, (HBRUSH)GetStockObject(BLACK_BRUSH));
        bar.left += 2; bar.top += 2; bar.bottom -= 2;
        bar.right = bar.left + (r.right - 36) * pct / 100;
        FillRect(dc, &bar, (HBRUSH)(COLOR_HIGHLIGHT + 1));
        EndPaint(h, &ps);
        return 0;
    }
    return DefWindowProcA(h, m, w, l);
}

/* Runs pass 1 on a worker thread while this thread shows progress. */
static int unpack_with_progress(void)
{
    WNDCLASSA wc;
    HWND h = NULL;
    HANDLE t;
    MSG msg;
    s_state = 0;
    if (!cw_headless_muted()) {
        RECT r = { 0, 0, 460, 88 };
        memset(&wc, 0, sizeof wc);
        wc.lpfnWndProc = progress_proc;
        wc.hInstance = GetModuleHandleA(NULL);
        wc.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_WAIT);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = "CloneWarsUnpack";
        RegisterClassA(&wc);
        AdjustWindowRect(&r, WS_CAPTION, FALSE);
        h = CreateWindowExA(0, wc.lpszClassName, "Star Wars: The Clone Wars", WS_CAPTION | WS_VISIBLE,
                            (GetSystemMetrics(SM_CXSCREEN) - (r.right - r.left)) / 2,
                            (GetSystemMetrics(SM_CYSCREEN) - (r.bottom - r.top)) / 2,
                            r.right - r.left, r.bottom - r.top, NULL, NULL, wc.hInstance, NULL);
        if (h) SetTimer(h, 1, 100, NULL);
        else fprintf(stderr, "[XISO] no progress window (error %lu)\n", GetLastError());
    }
    t = CreateThread(NULL, 0, unpack_thread, NULL, 0, NULL);
    if (!t) return 0;
    while (WaitForSingleObject(t, 50) == WAIT_TIMEOUT)
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageA(&msg); }
    CloseHandle(t);
    if (h) DestroyWindow(h);
    return s_state == 1;
}

/* ── entry ─────────────────────────────────────────────────── */

static void fail(const char *msg)
{
    fprintf(stderr, "[XISO] %s\n", msg);
    if (!cw_headless_muted())
        MessageBoxA(NULL, msg, "Star Wars: The Clone Wars", MB_ICONERROR);
}

static int find_image(const char *dir, char *out, size_t n)
{
    static const char *pats[] = { "*.iso", "*.xiso" };
    WIN32_FIND_DATAA fd;
    unsigned i;
    for (i = 0; i < 2; i++) {
        char pat[MAX_PATH];
        HANDLE h;
        snprintf(pat, sizeof pat, "%s\\%s", dir, pats[i]);
        h = FindFirstFileA(pat, &fd);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                snprintf(out, n, "%s\\%s", dir, fd.cFileName);
                FindClose(h);
                return 1;
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    return 0;
}

static int ask_image(char *out, size_t n)
{
    OPENFILENAMEA ofn;
    if (cw_headless_muted()) return 0;
    if (MessageBoxA(NULL,
            "The game files are not set up yet.\n\n"
            "Choose your Star Wars: The Clone Wars Xbox disc image (.iso). It is unpacked "
            "into the game's folder once; after that the game starts straight away.",
            "Star Wars: The Clone Wars", MB_OKCANCEL | MB_ICONINFORMATION) != IDOK)
        return 0;
    memset(&ofn, 0, sizeof ofn);
    out[0] = 0;
    ofn.lStructSize = sizeof ofn;
    ofn.lpstrFilter = "Xbox disc images (*.iso, *.xiso)\0*.iso;*.xiso\0All files\0*.*\0";
    ofn.lpstrFile = out;
    ofn.nMaxFile = (DWORD)n;
    ofn.lpstrTitle = "Choose your Star Wars: The Clone Wars disc image";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    return GetOpenFileNameA(&ofn) != 0;
}

/* Returns 1 when default.xbe is in game_dir (already, or after unpacking). */
int cw_xiso_prepare(const char *game_dir)
{
    char xbe[MAX_PATH], iso[MAX_PATH], title[64], msg[768];
    ULARGE_INTEGER freeb;
    DWORD t0;
    snprintf(xbe, sizeof xbe, "%s\\default.xbe", game_dir);
    if (GetFileAttributesA(xbe) != INVALID_FILE_ATTRIBUTES)
        return 1;
    if (!find_image(game_dir, iso, sizeof iso) && !ask_image(iso, sizeof iso)) {
        fail("No disc image found.\n\nPut your Star Wars: The Clone Wars disc image (.iso) "
             "in the same folder as CloneWars32.exe and start the game again.");
        return 0;
    }
    fprintf(stderr, "[XISO] unpacking %s into %s\n", iso, game_dir);
    t0 = GetTickCount();

    memset(&s_x, 0, sizeof s_x);
    s_x.dest = game_dir;
    if (!open_image(&s_x, iso) || !walk(&s_x, s_x.root_sector, s_x.root_size, "", 0, 0)) {
        fail(s_x.err);
        goto out;
    }
    switch (check_title(&s_x, title, sizeof title)) {
    case 1:
        break;
    case -1:
        snprintf(msg, sizeof msg, "%s\n\nis a different release of Star Wars: The Clone Wars "
                 "(another region or version). Only the USA release is supported.", iso);
        fail(msg);
        goto out;
    default:
        snprintf(msg, sizeof msg, "%s\n\nis not Star Wars: The Clone Wars%s%s%s.", iso,
                 title[0] ? " (it is \"" : "", title, title[0] ? "\")" : "");
        fail(msg);
        goto out;
    }
    s_x.total += s_x.xbe_size;
    if (GetDiskFreeSpaceExA(game_dir, &freeb, NULL, NULL) && freeb.QuadPart < s_x.total + (64u << 20)) {
        snprintf(msg, sizeof msg, "Not enough free disk space to unpack the game.\n\n"
                 "Needed: %.1f GB\nFree: %.1f GB", s_x.total / 1e9, freeb.QuadPart / 1e9);
        fail(msg);
        goto out;
    }
    if (!unpack_with_progress()) {
        fail(s_x.err[0] ? s_x.err : "Unpacking the disc image failed.");
        goto out;
    }
    fprintf(stderr, "[XISO] %d files, %.1f MB in %.1f s\n", s_x.files, s_x.done / 1e6,
            (GetTickCount() - t0) / 1000.0);
out:
    if (s_x.f && s_x.f != INVALID_HANDLE_VALUE) CloseHandle(s_x.f);
    return GetFileAttributesA(xbe) != INVALID_FILE_ATTRIBUTES;
}
