// Multi13Script.cpp -- reconstruction of a shipped mission script.
//
// Second application of the recipe proved on Multi12Script (see that file's header for the
// three traps: build flags, .text ordering, and string-pool creation order).
//
//   ~Multi13Script        108 B
//   Execute               144 B
//   Setup                  12 B
//   Multi13BuildMission   128 B
//                         -----
//                         392 B  == the TU's shipped size
//
// Build: -proc gekko -O4 -nodefaults -fp hard -Cpp_exceptions on -enum int -RTTI on
//        -inline all -use_lmw_stmw on -wchar_t off

static const char *const kClassName = "Multi13Script";
static const char *const kBaseName  = "SPMission";

namespace MissionUtility
{
    void  SetMusicLooping(bool loop);
    void  PlayMusic(const char *name, bool loop);
    void  StartAmbiences(const char *ambience, const char *stinger, float fadeIn, float fadeOut);
    int   AddObjective(const char *text);
    void  SetMapZoom(float in, float out);
}

class DLLBase
{
public:
    // Six vtable slots, read out of __vt__7DLLBase (0x8035ae5c, 24 B) and
    // __vt__9SPMission (0x8035ae3c, 32 B) -- see analysis/mission_script_abi.md.
    // A script never calls its own virtuals, so getting this wrong left every byte of
    // .text intact; it is .data that catches it -- tools/verify_vtables.py.
    virtual ~DLLBase() {}                 // +0x08
    virtual void InitialSetup();          // +0x0c  DLLBase::InitialSetup, 4 B, blr
    virtual bool Load();                  // +0x10  DLLBase::Load, 8 B, `return true`
    virtual void Execute();               // +0x14  DLLBase::Execute, 4 B, blr
};

class SPMission : public DLLBase
{
public:
    SPMission();
    virtual ~SPMission() {}                       // +0x08
    // SPMission::InitialSetup (0x801887b8, 200 B) zeroes the bool block, fills the float
    // block with 99999.0f, zeroes both int blocks, and only then calls slot +0x18 -- this
    // script's Setup. So a mission's start state is defined: every latch Setup does not
    // mention is false on the first tick.
    virtual void InitialSetup();                  // +0x0c  overrides DLLBase's
    virtual void Execute() = 0;                   // +0x14  re-declared pure; slot is null
    virtual void Setup() = 0;                     // +0x18  first slot SPMission adds
    virtual void RestartMission(int);             // +0x1c  0x801885bc, 4 B, blr

    bool  *mBools;    int mBoolCount;    // +0x04 +0x08
    int   *mIntsB;    int mCountB;       // +0x0c +0x10
    int   *mIntsC;    int mCountC;       // +0x14 +0x18
    void  *mBlockD;   int mCountD;       // +0x1c +0x20
};

class Multi13Script : public SPMission
{
public:
    virtual ~Multi13Script();

    Multi13Script()
    {
        mBoolCount = 1;   mBools  = &mDone;
        mCountB    = 0;   mIntsB  = mIntsBStore;
        mCountC    = 0;   mIntsC  = mIntsCStore;
        mCountD    = 0;   mBlockD = &mBlockDStore;
    }

    virtual void Setup();
    virtual void Execute();

    char  mPad24;                  // +0x24
    bool  mDone;                   // +0x25
    char  mPad26[6];               // +0x26
    int   mIntsBStore[2];          // +0x2c  registered with count 0
    int   mIntsCStore[2];          // +0x34  registered with count 0
    int   mBlockDStore;            // +0x3c  registered with count 0  -> sizeof == 0x40
};

Multi13Script::~Multi13Script()
{
}

void Multi13Script::Execute()
{
    // Unlike Multi12 this really is an early return -- the shipped `bne` targets the `lmw`
    // in the epilogue. King of the Hill has no per-tick work in the script.
    if (mDone) {
        return;
    }

    MissionUtility::SetMusicLooping(true);
    MissionUtility::PlayMusic("EP2_V1_T03_01", true);
    MissionUtility::StartAmbiences("AmbRaxus_desert01",
                                   "AmbRaxus_desert_stinger01", 10.0f, 30.0f);

    MissionUtility::AddObjective("multiplayer.types.king.rule01");
    MissionUtility::AddObjective("multiplayer.types.king.rule02");
    MissionUtility::AddObjective("multiplayer.types.king.rule03");

    mDone = true;

    MissionUtility::SetMapZoom(900.0f, 10000000.0f);
}

void Multi13Script::Setup()
{
    mDone = false;
}

SPMission *Multi13BuildMission()
{
    return new Multi13Script();
}
