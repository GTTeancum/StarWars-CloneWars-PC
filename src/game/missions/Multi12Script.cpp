// Multi12Script.cpp -- reconstruction of a shipped mission script.
//
// There is no source for any of the 32 <World>Script translation units. This one was
// rebuilt from the disassembly alone (tools/dis_func.py) and checked against the shipped
// bytes (tools/sbs_diff.py). It is the proof that the Phase 5 loop works end to end.
//
// Build with -O4, NOT -O4,p: the mission scripts were a separate project from the Zero
// engine and were not built for speed. -O4,p costs the stmw/lmw prologue and shifts the
// whole function. See analysis/mission_script_abi.md.
//
// All four functions match the shipped bytes exactly, and .text comes out in the shipped
// order at the shipped size:
//
//   ~Multi12Script        108 B   27/27 words
//   Execute               248 B   62/62 words, 23/23 relocs
//   Setup                  12 B    3/3  words
//   Multi12BuildMission   132 B   33/33 words, 4/4 relocs
//                         -----
//                         500 B   == the TU's shipped size

// The shipped TU creates these two literals before any function body, so they sit at the
// head of the string pool ("Multi12Script" +0, "SPMission" +16) and every literal Execute
// references is a bare offset off that base -- with no relocation, so the order is load-
// bearing. They are the RTTI class names, but CodeWarrior emits RTTI at end-of-TU, which
// puts them last. Something in the original header created them first; a DECLARE/register
// macro in Multi12Script.h is the obvious candidate. These two statics are a stand-in that
// reproduces the creation order exactly. Without them Execute drops to 52/62 words, every
// miss being the same +0x1c shift.
static const char *const kClassName = "Multi12Script";
static const char *const kBaseName  = "SPMission";

namespace MissionUtility
{
    void  SetMusicLooping(bool loop);
    void  PlayMusic(const char *name, bool loop);
    void  StartAmbiences(const char *ambience, const char *stinger, float fadeIn, float fadeOut);
    int   AddObjective(const char *text);
    int   CreateRegionList(const char *name, bool a, bool b);
    void  SetMapZoom(float in, float out);
    int   GetRegionNewMember(int list, int index);
    int   GetRegionNewMemberCount(int list);
    void  DamageObject(int handle, float damage, float shield);
}

// ---------------------------------------------------------------------------------------
// The two engine-side base classes. Both are external -- only their layout and vtable
// shape matter here, and both are pinned by Multi12BuildMission and the destructor.
//
// DLLBase declares no data, so its vptr sits at offset 0 (CodeWarrior otherwise places the
// vptr *after* the data members -- see analysis/phase2_status.md). SPMission's members
// therefore start at +4, and the derived script's at +0x24.
// ---------------------------------------------------------------------------------------

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

    // Four (base, count) pairs the host walks to serialise the script's state. Every one
    // of the 32 missions fills in exactly these four, always pointing back into its own
    // storage -- so they are the save/restore descriptor for the mission's variables.
    bool  *mBools;    int mBoolCount;    // +0x04 +0x08
    int   *mIntsB;    int mCountB;       // +0x0c +0x10
    int   *mIntsC;    int mCountC;       // +0x14 +0x18
    void  *mBlockD;   int mCountD;       // +0x1c +0x20
};

class Multi12Script : public SPMission
{
public:
    virtual ~Multi12Script();

    Multi12Script()
    {
        mBoolCount = 1;   mBools  = &mDone;
        mCountB    = 0;   mIntsB  = mIntsBStore;
        mCountC    = 2;   mIntsC  = &mRegionList;
        mCountD    = 0;   mBlockD = &mBlockDStore;
    }

    virtual void Setup();
    virtual void Execute();

    char  mPad24;                  // +0x24  unread by any of the four functions
    bool  mDone;                   // +0x25  the one registered bool
    char  mPad26[6];               // +0x26  six more byte-sized members, unread
    int   mIntsBStore[2];          // +0x2c  registered with count 0
    int   mRegionList;             // +0x34  the two registered ints ...
    int   mIntsCStore2;            // +0x38  ... second one unread
    int   mPad3c[2];               // +0x3c
    int   mBlockDStore;            // +0x44  registered with count 0   -> sizeof == 0x48
};

// ---------------------------------------------------------------------------------------

// Defined first on purpose. Emitting the destructor here forces the vtable and its two
// RTTI name strings out ahead of the string literals, which is the layout the shipped
// .rodata pool has ("Multi12Script" +0, "SPMission" +16, "EP2_V1_T12_01" +28). Every
// literal reference in Execute is a bare offset off that base with no relocation, so the
// pool order has to be reproduced exactly or nothing in Execute matches.
Multi12Script::~Multi12Script()
{
}

void Multi12Script::Execute()
{
    // `mDone` guards the one-shot setup only -- the region sweep below runs every tick.
    // The shipped `bne` jumps to the loop head, not to the epilogue.
    if (!mDone) {
        MissionUtility::SetMusicLooping(true);
        MissionUtility::PlayMusic("EP2_V1_T12_01", true);
        MissionUtility::StartAmbiences("AmbKshyk_forestday01",
                                       "amb_kashyyk_day_stinger01", 10.0f, 30.0f);

        MissionUtility::AddObjective("multiplayer.types.conquest.rule01");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule02");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule03");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule04");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule05");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule06");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule07");

        mDone = true;

        mRegionList = MissionUtility::CreateRegionList("Region0", false, true);
        MissionUtility::SetMapZoom(2000.0f, 10000000.0f);
    }

    // Anything that wanders into Region0 is killed outright -- the same 1e7 constant for
    // damage and shield (the shipped code loads it once and `fmr`s it into f2).
    for (int i = 0; i < MissionUtility::GetRegionNewMemberCount(mRegionList); i++) {
        MissionUtility::DamageObject(MissionUtility::GetRegionNewMember(mRegionList, i),
                                     10000000.0f, 10000000.0f);
    }
}

void Multi12Script::Setup()
{
    mDone = false;
}

SPMission *Multi12BuildMission()
{
    return new Multi12Script();
}
