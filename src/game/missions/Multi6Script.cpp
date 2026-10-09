// Multi6Script.cpp -- reconstruction of a shipped mission script.
//
// A Rhen Var blizzard Conquest map with two breakable ice sheets. The most involved of the
// multiplayer scripts: a non-trivial Setup, per-tick work outside the one-shot guard, and
// two independent region traps.
//
// Started from tools/gen_mission.py, then finished by hand -- the generator emits the
// straight-line call sequence and marks control flow it cannot model with /* TODO */.
//
//   ~Multi6Script        108 B
//   Execute              456 B
//   Setup                 80 B
//   Multi6BuildMission   132 B
//                        -----
//                        776 B  == the TU's shipped size
//
// Build: -proc gekko -O4 -nodefaults -fp hard -Cpp_exceptions on -enum int -RTTI on
//        -inline all -use_lmw_stmw on -wchar_t off

static const char *const kClassName = "Multi6Script";
static const char *const kBaseName  = "SPMission";

namespace MissionUtility
{
    int   AddObjective(const char*);
    int   CreateRegionList(const char*, bool, bool);
    void  DamageObject(int, float, float);
    void  ExcludeTeam(int, int);
    int   GetHandle(const char*);
    int   GetRegionNewMember(int, int);
    int   GetRegionNewMemberCount(int);
    bool  IsAlive(int);
    void  PlayMusic(const char*, bool);
    void  SetEnemies(int, int);
    void  SetMapZoom(float, float);
    void  SetMusicLooping(bool);
    void  StartAmbiences(const char*, const char*, float, float);
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

class Multi6Script : public SPMission
{
public:
    virtual ~Multi6Script();

    Multi6Script()
    {
        mBoolCount = 1;   mBools    = &mDone;
        mCountB    = 0;   mIntsB    = mIntsBStore;
        mCountC    = 4;   mIntsC    = &mRegion1;
        mCountD    = 0;   mBlockD   = &mBlockDStore;
    }

    virtual void Setup();
    virtual void Execute();

    char  mPad24;
    bool  mDone;                     // +0x25
    char  mPad26[6];
    int   mIntsBStore[2];           // +0x2c
    // The four registered ints, in order: the two region lists and the two ice sheets.
    int   mRegion1;                 // +0x34
    int   mRegion2;                 // +0x38
    int   mIceSheet1;               // +0x3c
    int   mIceSheet2;               // +0x40
    char  mPad44[8];
    int   mBlockDStore;             // +0x4c  -> sizeof == 0x50
};

Multi6Script::~Multi6Script()
{
}

void Multi6Script::Execute()
{
    if (!mDone) {
        MissionUtility::SetMusicLooping(true);
        MissionUtility::PlayMusic("EP5_V1_T05_02", true);
        MissionUtility::StartAmbiences("AmbRhen_blizzard",
                                       "AmbRhen_stinger_blizzard01", 10.0f, 30.0f);
        MissionUtility::AddObjective("multiplayer.types.conquest.rule01");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule02");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule03");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule04");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule05");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule06");
        MissionUtility::AddObjective("multiplayer.types.conquest.rule07");
        MissionUtility::SetMapZoom(1500.0f, 9999999.0f);

        MissionUtility::SetEnemies(1, 5);
        MissionUtility::SetEnemies(2, 5);

        mRegion1 = MissionUtility::CreateRegionList("wall_tester1_region", true, false);
        mRegion2 = MissionUtility::CreateRegionList("wall_tester2_region", true, false);

        mDone = true;
    }

    // Each ice sheet takes damage while anything is walking onto it, so it eventually gives
    // way. Note the shipped code calls GetRegionNewMember and throws the result away --
    // DamageObject is applied to the sheet, not to whoever stepped on it. Reproduced as
    // written; `lwz r3, 0x3c(r30)` overwrites the return value one instruction before the
    // call, so this is the shipped behaviour and not a reading error.
    if (MissionUtility::IsAlive(mIceSheet1)) {
        MissionUtility::ExcludeTeam(mRegion1, 1);
        MissionUtility::ExcludeTeam(mRegion1, 2);
        MissionUtility::ExcludeTeam(mRegion1, 5);

        for (int i = 0; i < MissionUtility::GetRegionNewMemberCount(mRegion1); i++) {
            MissionUtility::GetRegionNewMember(mRegion1, i);
            MissionUtility::DamageObject(mIceSheet1, 999999.0f, 999999.0f);
        }
    }

    if (MissionUtility::IsAlive(mIceSheet2)) {
        MissionUtility::ExcludeTeam(mRegion2, 1);
        MissionUtility::ExcludeTeam(mRegion2, 2);
        MissionUtility::ExcludeTeam(mRegion2, 5);

        for (int i = 0; i < MissionUtility::GetRegionNewMemberCount(mRegion2); i++) {
            MissionUtility::GetRegionNewMember(mRegion2, i);
            MissionUtility::DamageObject(mIceSheet2, 999999.0f, 999999.0f);
        }
    }
}

void Multi6Script::Setup()
{
    mDone = false;

    mIceSheet1 = MissionUtility::GetHandle("ice_sheet_handle1");
    mIceSheet2 = MissionUtility::GetHandle("ice_sheet_handle2");
}

SPMission *Multi6BuildMission()
{
    return new Multi6Script();
}
