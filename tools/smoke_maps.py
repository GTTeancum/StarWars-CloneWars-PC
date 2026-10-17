#!/usr/bin/env python3
"""Every-map smoke test: boot the game straight into each map, headless and
muted, and check that it gets into gameplay and keeps running.

One game at a time. For each map the game is started with
  CloneWars32.exe --headlessMuted
and RECOMP_CMDLINE="<map>.wld" (plus "-multiplayer" for the splitscreen and
online maps): the title's own command-line parser takes the world name and
skips the front end (see the xboxrecomp kernel bridge, LaunchDataPage).

--headlessMuted makes the game log a "[SMOKE]" status line every second
(src/host/input.c): frames presented, mission state key, running flag. A map
passes when the state reaches RUN and frames keep advancing for --play
seconds; a screenshot is saved at the end of the run for a look. Nobody
plays, so a map whose mission fails on its own (FAILURE state) is reported.

Results: <game>/logs/smoke/<timestamp>/ -- one .log and .png per map,
summary.txt and summary.json.

Usage: python tools/smoke_maps.py [map ...] [--play 60] [--load-timeout 240]
"""
import argparse, json, os, re, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(HERE)
GAME = os.path.abspath(os.path.join(PROJECT, "..", ".."))
EXE = os.path.join(GAME, "CloneWars32.exe")

# Campaign in mission order (missionbuttons.cfg), then splitscreen
# (multiplayerbuttons.cfg), then online / system link (onlinebuttons.cfg).
CAMPAIGN = ["geonosis2", "geonosis1", "geonosis3", "ossus1", "raxus1", "raxus2", "raxus3",
            "kashyyyk1", "kashyyyk2", "kashyyyk3", "ossus2", "ossus3", "asteroid1",
            "thule1", "thule2", "thule3"]
SPLITSCREEN = ["multi4", "multi3", "multi1", "multi15", "multi2", "multi14", "multi13", "multi9",
               "multi8", "multi12", "multi10", "multi6", "multi5", "multi17"]
ONLINE = ["cz2", "cz4", "dm1", "dm2", "dm3", "cq1", "cq2", "cq3", "cq4", "ac1"]
ALL = CAMPAIGN + SPLITSCREEN + ONLINE

STATES = {0x6D3CE530: "RUN", 0x37345010: "PAUSE", 0x4CCBDEC7: "SUCCESS", 0xA90C322D: "FAILURE"}
SMOKE = re.compile(r"\[SMOKE\] t=(\d+) frames=(\d+) state=([0-9A-F]{8}) run=(\d+)")


def running_games():
    r = subprocess.run(["tasklist", "/FI", "IMAGENAME eq CloneWars32.exe", "/NH"],
                       capture_output=True, text=True)
    return "CloneWars32.exe" in r.stdout


def kill(p):
    if p.poll() is None:
        subprocess.run(["taskkill", "/F", "/T", "/PID", str(p.pid)], capture_output=True)
        try:
            p.wait(10)
        except subprocess.TimeoutExpired:
            pass


def to_png(bmp, png):
    try:
        from PIL import Image
        Image.open(bmp).convert("RGB").resize((640, 360)).save(png)
        os.remove(bmp)
        return png
    except Exception:
        return bmp if os.path.exists(bmp) else None


def tail(path, n=15):
    try:
        with open(path, errors="replace") as f:
            lines = [l.rstrip() for l in f if not l.startswith("[SMOKE]")]
        return lines[-n:]
    except OSError:
        return []


def run_map(name, out, play, load_timeout, stall):
    log = os.path.join(out, name + ".log")
    inp = os.path.join(out, name + ".input")
    bmp = os.path.join(out, name + ".bmp")
    open(inp, "w").close()
    env = dict(os.environ)
    multi = name not in CAMPAIGN
    env["RECOMP_CMDLINE"] = name + ".wld" + (" -multiplayer" if multi else "")
    env["CW_HOST_LOG"] = log
    env["CW_INPUT_FILE"] = inp
    env["CW_SHOT_FILE"] = bmp
    if multi:
        env.setdefault("CW_PADS", env.get("CW_MP_PLAYERS", "2"))
    for k in ("CW_WARP", "CW_INPUT_SCRIPT", "CW_INPUT_REPLAY"):
        env.pop(k, None)

    t0 = time.time()
    p = subprocess.Popen([EXE, "--headlessMuted"], cwd=GAME, env=env)
    res = {"map": name, "cmdline": env["RECOMP_CMDLINE"]}
    run_since = None          # wall time the state first read RUN
    last_frames, frames_at = -1, time.time()
    states_seen = []
    pos = 0
    try:
        while True:
            time.sleep(1)
            now = time.time()
            if p.poll() is not None:
                res.update(result="CRASH", detail="game exited (code %s)" % p.returncode)
                break
            try:
                with open(log, errors="replace") as f:
                    f.seek(pos)
                    chunk = f.read()
                    pos = f.tell()
            except OSError:
                chunk = ""
            if "Mission load failed" in chunk:
                res.update(result="LOAD FAILED", detail="Mission load failed")
                break
            for m in SMOKE.finditer(chunk):
                frames, key = int(m.group(2)), int(m.group(3), 16)
                st = STATES.get(key, "%08X" % key)
                if not states_seen or states_seen[-1] != st:
                    states_seen.append(st)
                if frames != last_frames:
                    last_frames, frames_at = frames, now
                if st == "RUN" and run_since is None:
                    run_since = now
                    res["load_seconds"] = round(now - t0, 1)
            if "FAILURE" in states_seen:
                res.update(result="MISSION FAILED", detail="the mission's own failure screen came up")
                break
            if now - frames_at > stall and last_frames >= 0:
                res.update(result="FROZEN", detail="no new frame for %d s" % stall)
                break
            if run_since is None and now - t0 > load_timeout:
                res.update(result="NO GAMEPLAY", detail="never reached RUN in %d s" % load_timeout)
                break
            if run_since is not None and now - run_since >= play:
                if now - frames_at > 5:
                    res.update(result="FROZEN", detail="no new frame for %d s" % (now - frames_at))
                    break
                res.update(result="PASS", detail="%d s in gameplay" % play)
                break
        res["frames"] = last_frames
        res["states"] = states_seen
        if p.poll() is None:
            if os.path.exists(bmp):
                os.remove(bmp)
            with open(inp, "a") as f:
                f.write("SHOT\n")
            for _ in range(10):
                time.sleep(1)
                if os.path.exists(bmp) and os.path.getsize(bmp) > 0:
                    time.sleep(1)
                    break
            res["screenshot"] = to_png(bmp, os.path.join(out, name + ".png"))
    finally:
        kill(p)
    os.remove(inp)
    if res["result"] != "PASS":
        res["log_tail"] = tail(log)
    res["seconds"] = round(time.time() - t0, 1)
    return res


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("maps", nargs="*", help="map names (default: all %d)" % len(ALL))
    ap.add_argument("--play", type=int, default=60, help="seconds of gameplay a map must survive")
    ap.add_argument("--load-timeout", type=int, default=240, help="seconds allowed to reach gameplay")
    ap.add_argument("--stall", type=int, default=45, help="seconds without a new frame counted as frozen")
    a = ap.parse_args()
    maps = [m.lower() for m in a.maps] or ALL
    bad = [m for m in maps if m not in ALL]
    if bad:
        sys.exit("unknown map(s): %s" % ", ".join(bad))
    if running_games():
        sys.exit("CloneWars32.exe is already running; close it first (one game at a time).")

    out = os.path.join(GAME, "logs", "smoke", time.strftime("%Y%m%d-%H%M%S"))
    os.makedirs(out)
    print("results in", out, flush=True)
    results = []
    for i, m in enumerate(maps, 1):
        r = run_map(m, out, a.play, a.load_timeout, a.stall)
        results.append(r)
        print("[%2d/%d] %-10s %-11s %s%s" % (i, len(maps), m, r["result"], r.get("detail", ""),
              "  (loaded in %ss)" % r["load_seconds"] if "load_seconds" in r else ""), flush=True)
        with open(os.path.join(out, "summary.json"), "w") as f:
            json.dump(results, f, indent=1)

    passed = sum(r["result"] == "PASS" for r in results)
    with open(os.path.join(out, "summary.txt"), "w") as f:
        f.write("%d of %d maps passed\n\n" % (passed, len(results)))
        for r in results:
            f.write("%-10s %-11s %s\n" % (r["map"], r["result"], r.get("detail", "")))
            for l in r.get("log_tail", []):
                f.write("    | %s\n" % l)
    print("%d of %d maps passed" % (passed, len(results)))
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    sys.exit(main())
