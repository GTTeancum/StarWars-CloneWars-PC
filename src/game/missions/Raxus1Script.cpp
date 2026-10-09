// Raxus1Script.cpp -- reconstruction of a shipped mission script.
//
// The smallest of the eighteen campaign missions, and the first one attempted: 11,588 bytes,
// of which `Execute` alone is 10,800. Written to test whether the procedure that took the two
// multiplayer wave scripts to byte-exact carries over to the campaign tier -- see
// analysis/phase5_status.md.
//
// STATE: the destructor, `Setup` and `Raxus1BuildMission` are byte-exact (788 B). `Execute`
// is 10,800 bytes of polled state machine -- 69 one-shot blocks -- and **2 of those 69 are
// written**. It does not compile to the shipped size yet and is not expected to until all 69
// are in; the meter in the meantime is tools/pool_check.py, which reports how many of the
// 152 string literals land in the shipped order. Currently 16.
//
// The campaign missions are a different shape from the multiplayer ones and the difference is
// good news. `Multi5Script` was 9,552 bytes of code over 39 KB of tuning tables that had to be
// reproduced byte for byte. `Raxus1Script` has **no tables at all**: 2,544 bytes of .rodata,
// every byte of it a string literal, and 64 bytes of .data that is nothing but the vtable and
// the RTTI records. There is no data layout to recover -- only the pool order, which follows
// from writing the code in the right order.
//
// The state is 81 boolean latches, three floats, 64 handles and sixteen `Timer`s. That is the
// polled state machine the ABI document predicted, and it is what `Execute` walks every tick.

// Every MissionUtility entry point this translation unit calls, with the parameter list
// taken from the CodeWarrior mangling. The return types are inferred from the naming
// convention -- Is*/Has* are bool, the float queries are float, Get*/Add*/Create*/Run*
// return a handle -- and are the only part of this block that is not read off the binary.
namespace MissionUtility
{
    int   AddBonusObjective(const char*);
    int   AddObjective(const char*);
    void  BonusObjectiveComplete(int, bool);
    void  BonusObjectiveFailed(int);
    int   CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    void  DamageObject(int, float, float);
    void  DisplayText(const char*, float, float);
    void  FlushSoundQueue();
    int   GetCinId(int);
    float GetCurHealth(int);
    float GetDistance(int, const char*, int);
    float GetDistance(int, int);
    int   GetHandle(const char*);
    int   GetPlayerHandle(int);
    float GetTime();
    void  Goto(int, const char*, bool);
    void  GotoDirect(int, const char*, bool);
    void  GotoFire(int, const char*, bool, bool, bool, bool);
    bool  IsAlive(int);
    bool  IsCinRunning(int);
    bool  IsInsideRegion(int, const char*);
    void  Land(int, const char*, int, float);
    void  MissionFailure();
    void  MissionSuccess();
    void  MoveObject(int, const char*, int, bool);
    void  MoveObjectWithRotation(int, const char*, int, bool);
    void  Objectify(int, const char*, bool, bool, float);
    void  ObjectiveComplete(int);
    void  OverrideSoundRange(int, bool);
    void  PlayMusic(const char*, bool);
    void  PreloadConfig(const char*);
    int   QueueSound(const char*, float, float, float, const char*, int, const char*);
    void  RemoveObject(int);
    void  RemoveObjectify(int);
    void  RemoveText();
    int   RunCin(const char*, bool, bool);
    void  SetAccelThrust(int, float);
    void  SetAlliance(int, int);
    void  SetApplyDynamics(int, bool);
    void  SetCollidable(int, bool);
    void  SetCurHealth(int, float);
    void  SetCurShield(int, float);
    void  SetEnemies(int, int);
    void  SetEnemiesOneWay(int, int);
    void  SetFogRange(float, float, float);
    void  SetMaxHealth(int, float);
    void  SetMusicLooping(bool);
    void  SetNeutralOneWay(int, int);
    void  SetOriginalFog(float);
    void  SetQueueFlag(bool);
    void  SetTeamNum(int, int);
    void  SetVelocForward(int, float);
    void  SetVelocMaximumFly(int, float);
    void  SetVelocMinimumFly(int, float);
    void  SetVelocNeutralFly(int, float);
    void  SetVisible(int, bool);
    void  StartAmbiences(const char*, const char*, float, float);
    void  Stop(int);
}


// Twelve bytes, constructed out of line -- pinned by the sixteen `__ct__5TimerFv` calls in
// Raxus1BuildMission.
class Timer
{
public:
    Timer();
    operator float();
    float operator=(float v);   // NOT Timer& -- retail returns in f1 (0x801886f0)

    int mState[3];
};

// Free functions, not members: the shipped symbols are `BeginTimer__FR5Timer` and
// `StopTimer__FR5Timer`.
void BeginTimer(Timer &t);
void StopTimer(Timer &t);

// The RTTI class names. CodeWarrior emits RTTI at end-of-TU but the shipped pool has them
// first; two file-scope statics reproduce that. Everything else in the pool is created by
// Execute, in the order Execute creates it -- check with tools/pool_check.py.
// All three levels of the hierarchy, in the shipped order. "DLLBase" is eight bytes with its
// terminator so it goes to .sdata2 and takes no .rodata space -- but it still occupies a place
// in the numbering, and tools/pool_check.py catches its absence immediately.
static const char *const kClassName = "Raxus1Script";
static const char *const kRootName  = "DLLBase";
static const char *const kBaseName  = "SPMission";

// ---------------------------------------------------------------------------------------
// The classes. DLLBase declares no data, so its vptr sits at offset 0; SPMission's four
// (base, count) pairs occupy +0x04..+0x23 and the derived members start at +0x24. See
// analysis/mission_script_abi.md.
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

    bool  *mBools;    int mBoolCount;    // +0x04 +0x08
    int   *mIntsB;    int mCountB;       // +0x0c +0x10
    int   *mIntsC;    int mCountC;       // +0x14 +0x18
    void  *mBlockD;   int mCountD;       // +0x1c +0x20
};

// The four registered blocks are the mission's whole state, and their counts come straight
// out of Raxus1BuildMission: 81 latches, 3 floats, 50 handles, 14 more ints. Nothing here is
// named, because nothing in the three recovered functions names it -- `Execute` would.
class Raxus1Script : public SPMission
{
public:
    virtual ~Raxus1Script();

    Raxus1Script()
    {
        mBoolCount = 81;   mBools  = mFlags;
        mCountB    = 3;    mIntsB  = (int *)mFloats;
        mCountC    = 50;   mIntsC  = mHandles;
        mCountD    = 14;   mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    char   mPad24;                           // +0x024
    bool   mFlags[81];                       // +0x025  the polled state machine's latches
    char   mPad76[6];                        // +0x076
    float  mFloats[3];                       // +0x07c
    int    mPad88[2];                        // +0x088
    int    mHandles[50];                     // +0x090
    int    mPad158[2];                       // +0x158
    int    mInts[14];                        // +0x160
    int    mPad198;                          // +0x198
    // Sixteen named members, not `Timer mTimers[16]`: an array of a class with a
    // constructor compiles to one `__construct_array` call, and the shipped
    // BuildMission has sixteen separate `bl __ct__5TimerFv`.
    Timer  mTimer0;                          // +0x19c
    Timer  mTimer1;                          // +0x1a8
    Timer  mTimer2;                          // +0x1b4
    Timer  mTimer3;                          // +0x1c0
    Timer  mTimer4;                          // +0x1cc
    Timer  mTimer5;                          // +0x1d8
    Timer  mTimer6;                          // +0x1e4
    Timer  mTimer7;                          // +0x1f0
    Timer  mTimer8;                          // +0x1fc
    Timer  mTimer9;                          // +0x208
    Timer  mTimer10;                         // +0x214
    Timer  mTimer11;                         // +0x220
    Timer  mTimer12;                         // +0x22c
    Timer  mTimer13;                         // +0x238
    Timer  mTimer14;                         // +0x244
    Timer  mTimer15;                         // +0x250
                                             // -> sizeof == 0x25c
};

// ---------------------------------------------------------------------------------------

Raxus1Script::~Raxus1Script()
{
}

// 10,800 bytes: 69 one-shot blocks, each guarded by its own latch, sharing 59 distinct API
// calls. The blocks are independent -- a guard, a run of calls, a latch flip -- so they were
// generated with tools/gen_block.py and checked one at a time with tools/pool_check.py, which
// is the only signal available until the last one lands and the function reaches the shipped
// size.
void Raxus1Script::Execute()
{
    // ---- +0x000c  476 bytes ----
    mHandles[3] = MissionUtility::GetPlayerHandle(0);
    if (mFlags[5]) {
    MissionUtility::SetMusicLooping(true);
    MissionUtility::SetEnemies(1, 2);
    MissionUtility::SetAlliance(1, 0);
    MissionUtility::SetAlliance(2, 0);
    MissionUtility::SetEnemiesOneWay(3, 1);
    MissionUtility::SetNeutralOneWay(1, 3);
    MissionUtility::SetAlliance(2, 3);
    MissionUtility::SetAlliance(3, 0);
    mFlags[6] = true;
    MissionUtility::SetVelocForward(mHandles[4], 160.0f);
    MissionUtility::SetVelocForward(mHandles[5], 160.0f);
    MissionUtility::SetVelocForward(mHandles[6], 160.0f);
    MissionUtility::SetVelocForward(mHandles[7], 160.0f);
    MissionUtility::SetVelocForward(mHandles[8], 160.0f);
    MissionUtility::SetAccelThrust(mHandles[4], 150.0f);
    MissionUtility::SetAccelThrust(mHandles[5], 150.0f);
    MissionUtility::SetAccelThrust(mHandles[6], 150.0f);
    MissionUtility::SetAccelThrust(mHandles[7], 150.0f);
    MissionUtility::SetAccelThrust(mHandles[8], 150.0f);
    mFloats[0] = 1.0f;
    mFloats[1] = 128.0f;
    mFloats[2] = 5.0f;
    MissionUtility::StartAmbiences("AmbRaxus_rain01_pl2", "AmbRaxus_desert_stinger01", 10.0f, 30.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[0], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[0], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[0], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[1], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[1], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[1], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[2], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[2], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[2], 0.0f);
    mInts[0] = MissionUtility::AddObjective("missions.Raxus1.objective.str0000");
    mHandles[17] = MissionUtility::AddBonusObjective("missions.Raxus1.bonus.str0004");
    mHandles[15] = MissionUtility::AddBonusObjective("missions.Raxus1.bonus.str0003");
    mHandles[16] = MissionUtility::AddBonusObjective("missions.Raxus1.bonus.str0001");
    mFlags[5] = false;
    }

    // ---- +0x01e8  312 bytes ----
    if (mFlags[6]) {
        MissionUtility::SetVisible(mHandles[47], false);
        MissionUtility::SetFogRange(200.0f, 1800.0f, 2.0f);
        mHandles[31] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinFighterPath2", 0, "OpenCinFighter2", 0, -1, -1);
        mHandles[32] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinFighterPath3", 0, "OpenCinFighter3", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[31], true);
        MissionUtility::OverrideSoundRange(mHandles[32], true);
        MissionUtility::SetVelocMinimumFly(mHandles[31], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[31], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[31], 200.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[32], 202.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[32], 202.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[32], 202.0f);
        MissionUtility::Goto(mHandles[31], "OpenCinFighterPath2", true);
        MissionUtility::Goto(mHandles[32], "OpenCinFighterPath3", true);
        mHandles[11] = MissionUtility::RunCin("opencin", true, true);
        MissionUtility::PlayMusic("EP6_V2_T05_01", true);
        MissionUtility::SetVisible(mHandles[14], false);
        MissionUtility::SetCollidable(mHandles[14], false);
        mFlags[6] = false;
        mFlags[7] = true;
    }

    // ---- +0x0320  380 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[70]) {
        if (MissionUtility::GetCinId(mHandles[11]) == 2) {
        MissionUtility::QueueSound("OBR08_10", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("ASR08_08", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("OBR08_10A", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObject(mHandles[31]);
        MissionUtility::RemoveObject(mHandles[32]);
        BeginTimer(mTimer4);
        mFlags[70] = true;
        mHandles[29] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinObiWanFighterPath", 0, "OpenCinFighter", 0, -1, -1);
        mHandles[30] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinAnakinFighterPath", 0, "OpenCinFighter1", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[29], true);
        MissionUtility::OverrideSoundRange(mHandles[30], true);
        MissionUtility::SetVelocMinimumFly(mHandles[29], 150.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[29], 150.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[29], 150.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[30], 152.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[30], 152.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[30], 152.0f);
        MissionUtility::Goto(mHandles[29], "OpenCinObiWanFighterPath", true);
        MissionUtility::Goto(mHandles[30], "OpenCinAnakinFighterPath", true);
        }
    }

    // ---- +0x049c  324 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[79]) {
        if (MissionUtility::GetCinId(mHandles[11]) == 3) {
        MissionUtility::SetVelocMinimumFly(mHandles[29], 100.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[29], 100.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[29], 100.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[30], 102.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[30], 102.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[30], 102.0f);
        mHandles[18] = MissionUtility::CreateObjectWithRotation("REP_bike_speeder_player_small", "OpenCinObiWanBikePath", 0, "OpenCinObiWan", 0, -1, -1);
        mHandles[19] = MissionUtility::CreateObjectWithRotation("REP_bike_speeder_anakin", "OpenCinAnakinBikePath", 0, "OpenCinAnakin", 0, -1, -1);
        mHandles[20] = MissionUtility::CreateObjectWithRotation("REP_bike_speeder_player_small", "OpenCinNewBikePath", 0, "OpenCinObiWan1", 0, -1, -1);
        mHandles[21] = MissionUtility::CreateObjectWithRotation("REP_bike_speeder_anakin", "OpenCinNewBikePath1", 0, "OpenCinAnakin1", 0, -1, -1);
        MissionUtility::SetCollidable(mHandles[20], false);
        MissionUtility::SetCollidable(mHandles[21], false);
        MissionUtility::SetCollidable(mHandles[18], false);
        MissionUtility::SetCollidable(mHandles[19], false);
        mFlags[79] = true;
        }
    }

    // ---- +0x05e0  132 bytes ----
    if (mFlags[7] && mFlags[79] && !mFlags[14] && !mFlags[78]) {
        if (MissionUtility::GetDistance(mHandles[29], "OpenCinObiWanFighterPath", 7) < 40.0f) {
        MissionUtility::Land(mHandles[29], 0, 0, 80.0f);
        MissionUtility::Land(mHandles[30], 0, 0, 80.0f);
        mFlags[78] = true;
        BeginTimer(mTimer5);
        }
    }

    // ---- +0x0664  96 bytes ----
    if (!mFlags[14] && mFlags[7]) {
        if (mTimer4 > 18.0f) {
        MissionUtility::Goto(mHandles[18], "OpenCinObiWanBikePath", true);
        MissionUtility::Goto(mHandles[19], "OpenCinAnakinBikePath", true);
        StopTimer(mTimer4);
        mTimer4 = 0.0f;
        }
    }

    // ---- +0x06c4  120 bytes ----
    if (mFlags[7] && !mFlags[14]) {
        if (mTimer5 > 10.0f) {
        StopTimer(mTimer5);
        mTimer5 = 0.0f;
        MissionUtility::SetVelocForward(mHandles[20], 200.0f);
        MissionUtility::SetVelocForward(mHandles[21], 200.0f);
        MissionUtility::Goto(mHandles[20], "OpenCinNewBikePath", false);
        MissionUtility::Goto(mHandles[21], "OpenCinNewBikePath1", false);
        }
    }

    // ---- +0x073c  152 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[71]) {
        if (MissionUtility::GetCinId(mHandles[11]) == 4) {
        if (mFlags[7]) {
        MissionUtility::RemoveObject(mHandles[18]);
        MissionUtility::RemoveObject(mHandles[19]);
        MissionUtility::QueueSound("ASR08_01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("OBR08_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[71] = true;
        }
        }
    }

    // ---- +0x07d4  240 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[72]) {
        if (MissionUtility::GetCinId(mHandles[11]) == 5) {
        if (mFlags[7]) {
        MissionUtility::SetVelocForward(mHandles[20], 70.0f);
        MissionUtility::SetVelocForward(mHandles[21], 70.0f);
        MissionUtility::MoveObjectWithRotation(mHandles[20], "OpenCinObiWanBikePath1", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[21], "OpenCinAnakinBikePath1", 0, true);
        mHandles[33] = MissionUtility::CreateObjectWithRotation("RAX_bldg_crane", "OpenCinCranePath", 0, "OpenCinCrane", 0, -1, -1);
        mHandles[22] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder_large", "OpenCinDroidPath1", 0, "OpenCinDroid1", 0, -1, -1);
        MissionUtility::Goto(mHandles[20], "OpenCinObiWanBikePath1", false);
        MissionUtility::Goto(mHandles[21], "OpenCinAnakinBikePath1", false);
        mFlags[72] = true;
        }
        }
    }

    // ---- +0x08c4  84 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[80]) {
        if (MissionUtility::GetDistance(mHandles[22], mHandles[20]) < 200.0f) {
        MissionUtility::Goto(mHandles[22], "OpenCinDroidPath1", true);
        mFlags[80] = true;
        }
    }

    // ---- +0x0918  88 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[68]) {
        if (MissionUtility::GetDistance(mHandles[20], "OpenCinObiWanBikePath1", 5) < 30.0f) {
        BeginTimer(mTimer13);
        mFlags[68] = true;
        MissionUtility::Stop(mHandles[20]);
        }
    }

    // ---- +0x0970  472 bytes ----
    if (mFlags[7] && !mFlags[14]) {
        if (mTimer13 > 0.2f) {
        StopTimer(mTimer13);
        mTimer13 = 0.0f;
        mHandles[23] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder_large", "OpenCinDroidPath2", 0, "OpenCinDroid2", 0, -1, -1);
        mHandles[24] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder_large", "OpenCinDroidPath3", 0, "OpenCinDroid3", 0, -1, -1);
        mHandles[25] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder_large", "OpenCinDroidPath4", 0, "OpenCinDroid4", 0, -1, -1);
        mHandles[26] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder_large", "OpenCinDroidPath5", 0, "OpenCinDroid5", 0, -1, -1);
        mHandles[27] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder_large", "OpenCinDroidPath6", 0, "OpenCinDroid6", 0, -1, -1);
        mHandles[28] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder_large", "OpenCinDroidPath7", 0, "OpenCinDroid7", 0, -1, -1);
        MissionUtility::SetVelocForward(mHandles[23], 130.0f);
        MissionUtility::SetVelocForward(mHandles[24], 130.0f);
        MissionUtility::SetVelocForward(mHandles[25], 130.0f);
        MissionUtility::SetVelocForward(mHandles[26], 130.0f);
        MissionUtility::SetVelocForward(mHandles[27], 130.0f);
        MissionUtility::SetVelocForward(mHandles[28], 130.0f);
        MissionUtility::Goto(mHandles[23], "OpenCinDroidPath2", true);
        MissionUtility::Goto(mHandles[24], "OpenCinDroidPath3", true);
        MissionUtility::Goto(mHandles[25], "OpenCinDroidPath4", true);
        MissionUtility::Goto(mHandles[26], "OpenCinDroidPath5", true);
        MissionUtility::Goto(mHandles[27], "OpenCinDroidPath6", true);
        MissionUtility::Goto(mHandles[28], "OpenCinDroidPath7", true);
        BeginTimer(mTimer6);
        BeginTimer(mTimer7);
        BeginTimer(mTimer8);
        }
    }

    // ---- +0x0b48  80 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[69]) {
        if (MissionUtility::GetDistance(mHandles[21], "OpenCinAnakinBikePath1", 6) < 20.0f) {
        mFlags[69] = true;
        MissionUtility::Stop(mHandles[21]);
        }
    }

    // ---- +0x0b98  232 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[73]) {
        if (mTimer6 > 5.5f) {
        MissionUtility::QueueSound("BSR08_03", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("OBR08_04", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("OBR08_05", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("ASR08_06", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Goto(mHandles[23], "OpenCinDroidPath2a", true);
        MissionUtility::Goto(mHandles[24], "OpenCinDroidPath3a", true);
        mFlags[73] = true;
        StopTimer(mTimer6);
        }
    }

    // ---- +0x0c80  104 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[74]) {
        if (mTimer7 > 6.0f) {
        MissionUtility::Goto(mHandles[25], "OpenCinDroidPath4a", true);
        MissionUtility::Goto(mHandles[26], "OpenCinDroidPath5a", true);
        mFlags[74] = true;
        StopTimer(mTimer7);
        }
    }

    // ---- +0x0ce8  120 bytes ----
    if (mFlags[7] && !mFlags[14] && !mFlags[75]) {
        if (mTimer8 > 6.5f) {
        MissionUtility::Goto(mHandles[27], "OpenCinDroidPath6a", true);
        MissionUtility::Goto(mHandles[28], "OpenCinDroidPath7a", true);
        mFlags[75] = true;
        StopTimer(mTimer8);
        BeginTimer(mTimer9);
        BeginTimer(mTimer12);
        }
    }

    // ---- +0x0d60  92 bytes ----
    if (mFlags[7] && !mFlags[14] && mFlags[75]) {
        if (mTimer9 > 7.0f) {
        MissionUtility::Goto(mHandles[21], "OpenCinAnakinBikePath2", false);
        StopTimer(mTimer9);
        mTimer9 = 0.0f;
        }
    }

    // ---- +0x0dbc  92 bytes ----
    if (mFlags[7] && !mFlags[14] && mFlags[75]) {
        if (mTimer12 > 8.0f) {
        MissionUtility::Goto(mHandles[20], "OpenCinObiWanBikePath2", false);
        StopTimer(mTimer12);
        mTimer12 = 0.0f;
        }
    }

    // ---- +0x0e18  44 bytes ----
    if (mFlags[32]) {
        if (mFloats[0] < MissionUtility::GetTime()) {
        mFloats[0] = 195.0f + MissionUtility::GetTime();
        }
    }

    // ---- +0x0e44  44 bytes ----
    if (mFlags[32]) {
        if (mFloats[1] < MissionUtility::GetTime()) {
        mFloats[1] = 195.0f + MissionUtility::GetTime();
        }
    }

    // ---- +0x0e70  40 bytes ----
    if (!mFlags[65]) {
        if (MissionUtility::IsInsideRegion(mHandles[3], "shortcut1")) {
        mFlags[65] = true;
        }
    }

    // ---- +0x0e98  40 bytes ----
    if (!mFlags[66]) {
        if (MissionUtility::IsInsideRegion(mHandles[3], "shortcut2")) {
        mFlags[66] = true;
        }
    }

    // ---- +0x0ec0  76 bytes ----
    if (!mFlags[67] && mFlags[65] && mFlags[66]) {
        MissionUtility::BonusObjectiveComplete(mHandles[17], true);
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0011", 5.0f, -1.0f);
        mFlags[67] = true;
    }

    // ---- +0x0f0c  700 bytes ----
    if (mFlags[8] && mFlags[7]) {
        if (!MissionUtility::IsCinRunning(mHandles[11])) {
        MissionUtility::SetVisible(mHandles[14], true);
        MissionUtility::SetCollidable(mHandles[14], true);
        MissionUtility::SetVisible(mHandles[47], true);
        MissionUtility::FlushSoundQueue();
        MissionUtility::GotoDirect(mHandles[4], "stap1path", false);
        MissionUtility::GotoDirect(mHandles[5], "stap1path", false);
        MissionUtility::GotoDirect(mHandles[6], "stap1path", false);
        MissionUtility::GotoDirect(mHandles[7], "stap1path", false);
        MissionUtility::GotoDirect(mHandles[8], "stap1path", false);
        MissionUtility::SetMaxHealth(mHandles[4], 45.0f);
        MissionUtility::SetMaxHealth(mHandles[5], 45.0f);
        MissionUtility::SetMaxHealth(mHandles[6], 45.0f);
        MissionUtility::SetMaxHealth(mHandles[7], 45.0f);
        MissionUtility::SetMaxHealth(mHandles[8], 45.0f);
        MissionUtility::SetCurHealth(mHandles[4], 45.0f);
        MissionUtility::SetCurHealth(mHandles[5], 45.0f);
        MissionUtility::SetCurHealth(mHandles[6], 45.0f);
        MissionUtility::SetCurHealth(mHandles[7], 45.0f);
        MissionUtility::SetCurHealth(mHandles[8], 45.0f);
        MissionUtility::DisplayText("missions.Raxus1.text.str0000", 6.0f, -1.0f);
        MissionUtility::SetTeamNum(mHandles[3], 1);
        MissionUtility::SetTeamNum(mHandles[4], 2);
        MissionUtility::SetTeamNum(mHandles[5], 2);
        MissionUtility::SetTeamNum(mHandles[6], 2);
        MissionUtility::SetTeamNum(mHandles[7], 2);
        MissionUtility::SetTeamNum(mHandles[8], 2);
        MissionUtility::MoveObject(mHandles[5], "stap1path", 3, true);
        MissionUtility::MoveObject(mHandles[6], "stap1path", 5, true);
        MissionUtility::MoveObject(mHandles[7], "stap1path", 7, true);
        MissionUtility::MoveObject(mHandles[8], "stap1path", 9, true);
        MissionUtility::SetVelocForward(mHandles[4], 190.0f);
        MissionUtility::SetVelocForward(mHandles[5], 190.0f);
        MissionUtility::SetVelocForward(mHandles[6], 190.0f);
        MissionUtility::SetVelocForward(mHandles[7], 190.0f);
        MissionUtility::SetVelocForward(mHandles[8], 190.0f);
        MissionUtility::SetVelocForward(mHandles[3], 150.0f);
        MissionUtility::RemoveObject(mHandles[22]);
        MissionUtility::RemoveObject(mHandles[23]);
        MissionUtility::RemoveObject(mHandles[24]);
        MissionUtility::RemoveObject(mHandles[25]);
        MissionUtility::RemoveObject(mHandles[26]);
        MissionUtility::RemoveObject(mHandles[27]);
        MissionUtility::RemoveObject(mHandles[28]);
        MissionUtility::RemoveObject(mHandles[21]);
        MissionUtility::RemoveObject(mHandles[20]);
        MissionUtility::RemoveObject(mHandles[29]);
        MissionUtility::RemoveObject(mHandles[30]);
        MissionUtility::RemoveObject(mHandles[31]);
        MissionUtility::RemoveObject(mHandles[32]);
        MissionUtility::RemoveObject(mHandles[33]);
        MissionUtility::SetOriginalFog(0.0f);
        MissionUtility::PlayMusic("EP2_V1_T03_01", true);
        mFlags[8] = false;
        BeginTimer(mTimer2);
        mFlags[11] = true;
        mFlags[10] = true;
        mFlags[13] = true;
        mFlags[10] = true;
        mFlags[14] = false;
        mFlags[15] = false;
        }
    }

    // ---- +0x11c8  52 bytes ----
    if (!mFlags[0]) {
        if (mTimer2 >= 141.0f) {
        MissionUtility::BonusObjectiveFailed(mHandles[15]);
        mFlags[0] = true;
        }
    }

    // ---- +0x11fc  240 bytes ----
    if (!mFlags[33]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[4]) < MissionUtility::GetDistance(mHandles[3], mHandles[5])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[4]) < MissionUtility::GetDistance(mHandles[3], mHandles[6])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[4]) < MissionUtility::GetDistance(mHandles[3], mHandles[7])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[4]) < MissionUtility::GetDistance(mHandles[3], mHandles[8])) {
        MissionUtility::Objectify(mHandles[4], "missions.Raxus1.marker.str0000", false, true, 0.0f);
        MissionUtility::RemoveObjectify(mHandles[5]);
        MissionUtility::RemoveObjectify(mHandles[6]);
        MissionUtility::RemoveObjectify(mHandles[7]);
        MissionUtility::RemoveObjectify(mHandles[8]);
        mFlags[34] = false;
        mFlags[35] = false;
        mFlags[36] = false;
        mFlags[37] = false;
        mFlags[33] = true;
        }
        }
        }
        }
    }

    // ---- +0x12ec  240 bytes ----
    if (!mFlags[34]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[5]) < MissionUtility::GetDistance(mHandles[3], mHandles[4])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[5]) < MissionUtility::GetDistance(mHandles[3], mHandles[6])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[5]) < MissionUtility::GetDistance(mHandles[3], mHandles[7])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[5]) < MissionUtility::GetDistance(mHandles[3], mHandles[8])) {
        MissionUtility::Objectify(mHandles[5], "missions.Raxus1.marker.str0000", false, true, 0.0f);
        MissionUtility::RemoveObjectify(mHandles[4]);
        MissionUtility::RemoveObjectify(mHandles[6]);
        MissionUtility::RemoveObjectify(mHandles[7]);
        MissionUtility::RemoveObjectify(mHandles[8]);
        mFlags[33] = false;
        mFlags[35] = false;
        mFlags[36] = false;
        mFlags[37] = false;
        mFlags[34] = true;
        }
        }
        }
        }
    }

    // ---- +0x13dc  240 bytes ----
    if (!mFlags[35]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[6]) < MissionUtility::GetDistance(mHandles[3], mHandles[4])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[6]) < MissionUtility::GetDistance(mHandles[3], mHandles[5])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[6]) < MissionUtility::GetDistance(mHandles[3], mHandles[7])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[6]) < MissionUtility::GetDistance(mHandles[3], mHandles[8])) {
        MissionUtility::Objectify(mHandles[6], "missions.Raxus1.marker.str0000", false, true, 0.0f);
        MissionUtility::RemoveObjectify(mHandles[4]);
        MissionUtility::RemoveObjectify(mHandles[5]);
        MissionUtility::RemoveObjectify(mHandles[7]);
        MissionUtility::RemoveObjectify(mHandles[8]);
        mFlags[33] = false;
        mFlags[34] = false;
        mFlags[36] = false;
        mFlags[37] = false;
        mFlags[35] = true;
        }
        }
        }
        }
    }

    // ---- +0x14cc  240 bytes ----
    if (!mFlags[36]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[7]) < MissionUtility::GetDistance(mHandles[3], mHandles[4])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[7]) < MissionUtility::GetDistance(mHandles[3], mHandles[5])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[7]) < MissionUtility::GetDistance(mHandles[3], mHandles[6])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[7]) < MissionUtility::GetDistance(mHandles[3], mHandles[8])) {
        MissionUtility::Objectify(mHandles[7], "missions.Raxus1.marker.str0000", false, true, 0.0f);
        MissionUtility::RemoveObjectify(mHandles[4]);
        MissionUtility::RemoveObjectify(mHandles[5]);
        MissionUtility::RemoveObjectify(mHandles[6]);
        MissionUtility::RemoveObjectify(mHandles[8]);
        mFlags[33] = false;
        mFlags[34] = false;
        mFlags[35] = false;
        mFlags[37] = false;
        mFlags[36] = true;
        }
        }
        }
        }
    }

    // ---- +0x15bc  240 bytes ----
    if (!mFlags[37]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[8]) < MissionUtility::GetDistance(mHandles[3], mHandles[4])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[8]) < MissionUtility::GetDistance(mHandles[3], mHandles[5])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[8]) < MissionUtility::GetDistance(mHandles[3], mHandles[6])) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[8]) < MissionUtility::GetDistance(mHandles[3], mHandles[7])) {
        MissionUtility::Objectify(mHandles[8], "missions.Raxus1.marker.str0000", false, true, 0.0f);
        MissionUtility::RemoveObjectify(mHandles[4]);
        MissionUtility::RemoveObjectify(mHandles[5]);
        MissionUtility::RemoveObjectify(mHandles[6]);
        MissionUtility::RemoveObjectify(mHandles[7]);
        mFlags[33] = false;
        mFlags[34] = false;
        mFlags[35] = false;
        mFlags[36] = false;
        mFlags[37] = true;
        }
        }
        }
        }
    }

    // ---- +0x16ac  108 bytes ----
    if (!mFlags[1]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[1]) < 1000.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocMinimumFly(mHandles[1], 20.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[1], 20.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[1], 20.0f);
        MissionUtility::Goto(mHandles[1], "collector2path", true);
        mFlags[1] = true;
        }
        }
    }

    // ---- +0x1718  108 bytes ----
    if (!mFlags[2]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[2]) < 1000.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocMinimumFly(mHandles[2], 20.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[2], 20.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[2], 20.0f);
        MissionUtility::Goto(mHandles[2], "collector3path", true);
        mFlags[2] = true;
        }
        }
    }

    // ---- +0x1784  84 bytes ----
    if (!mFlags[3] && mFlags[1]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[1]) < 1000.0f) {
        MissionUtility::SetVelocMinimumFly(mHandles[1], 20.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[1], 20.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[1], 20.0f);
        }
    }

    // ---- +0x17d8  84 bytes ----
    if (!mFlags[4] && mFlags[2]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[2]) < 1000.0f) {
        MissionUtility::SetVelocMinimumFly(mHandles[2], 20.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[2], 20.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[2], 20.0f);
        }
    }

    // ---- +0x182c  76 bytes ----
    if (!mFlags[21]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[4]) > 300.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[4], 190.0f);
        mFlags[21] = true;
        mFlags[22] = false;
        }
        }
    }

    // ---- +0x1878  76 bytes ----
    if (!mFlags[22]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[4]) < 300.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[4], 200.0f);
        mFlags[22] = true;
        mFlags[21] = false;
        }
        }
    }

    // ---- +0x18c4  76 bytes ----
    if (!mFlags[23]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[5]) > 300.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[5], 190.0f);
        mFlags[23] = true;
        mFlags[24] = false;
        }
        }
    }

    // ---- +0x1910  76 bytes ----
    if (!mFlags[24]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[5]) < 300.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[5], 205.0f);
        mFlags[24] = true;
        mFlags[23] = false;
        }
        }
    }

    // ---- +0x195c  76 bytes ----
    if (!mFlags[17]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[6]) > 325.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[6], 190.0f);
        mFlags[18] = false;
        mFlags[17] = true;
        }
        }
    }

    // ---- +0x19a8  76 bytes ----
    if (!mFlags[18]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[6]) < 325.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[6], 210.0f);
        mFlags[17] = false;
        mFlags[18] = true;
        }
        }
    }

    // ---- +0x19f4  76 bytes ----
    if (!mFlags[29]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[7]) > 350.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[7], 190.0f);
        mFlags[19] = false;
        mFlags[29] = true;
        }
        }
    }

    // ---- +0x1a40  76 bytes ----
    if (!mFlags[19]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[7]) < 350.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[7], 215.0f);
        mFlags[29] = false;
        mFlags[19] = true;
        }
        }
    }

    // ---- +0x1a8c  76 bytes ----
    if (!mFlags[28]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[8]) > 375.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[8], 190.0f);
        mFlags[26] = false;
        mFlags[28] = true;
        }
        }
    }

    // ---- +0x1ad8  76 bytes ----
    if (!mFlags[26]) {
        if (MissionUtility::GetDistance(mHandles[3], mHandles[8]) < 375.0f) {
        if (!mFlags[8]) {
        MissionUtility::SetVelocForward(mHandles[8], 220.0f);
        mFlags[28] = false;
        mFlags[26] = true;
        }
        }
    }

    // ---- +0x1b24  152 bytes ----
    if (!mFlags[48] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "20000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "20000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "20000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "20000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "20000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0012", 7.0f, -1.0f);
        mFlags[48] = true;
        }
    }

    // ---- +0x1bbc  152 bytes ----
    if (!mFlags[49] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "18000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "18000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "18000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "18000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "18000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0013", 7.0f, -1.0f);
        mFlags[49] = true;
        }
    }

    // ---- +0x1c54  152 bytes ----
    if (!mFlags[50] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "16000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "16000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "16000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "16000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "16000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0014", 7.0f, -1.0f);
        mFlags[50] = true;
        }
    }

    // ---- +0x1cec  152 bytes ----
    if (!mFlags[51] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "14000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "14000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "14000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "14000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "14000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0015", 7.0f, -1.0f);
        mFlags[51] = true;
        }
    }

    // ---- +0x1d84  152 bytes ----
    if (!mFlags[52] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "12000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "12000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "12000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "12000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "12000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0016", 7.0f, -1.0f);
        mFlags[52] = true;
        }
    }

    // ---- +0x1e1c  152 bytes ----
    if (!mFlags[53] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "10000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "10000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "10000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "10000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "10000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0017", 7.0f, -1.0f);
        mFlags[53] = true;
        }
    }

    // ---- +0x1eb4  152 bytes ----
    if (!mFlags[54] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "8000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "8000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "8000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "8000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "8000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0018", 7.0f, -1.0f);
        mFlags[54] = true;
        }
    }

    // ---- +0x1f4c  152 bytes ----
    if (!mFlags[55] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "6000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "6000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "6000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "6000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "6000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0019", 7.0f, -1.0f);
        mFlags[55] = true;
        }
    }

    // ---- +0x1fe4  152 bytes ----
    if (!mFlags[56] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "4000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "4000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "4000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "4000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "4000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0020", 7.0f, -1.0f);
        mFlags[56] = true;
        }
    }

    // ---- +0x207c  152 bytes ----
    if (!mFlags[57] && !mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "2000area")
            || MissionUtility::IsInsideRegion(mHandles[5], "2000area")
            || MissionUtility::IsInsideRegion(mHandles[6], "2000area")
            || MissionUtility::IsInsideRegion(mHandles[7], "2000area")
            || MissionUtility::IsInsideRegion(mHandles[8], "2000area")) {
        MissionUtility::RemoveText();
        MissionUtility::DisplayText("missions.Raxus1.text.str0021", 7.0f, -1.0f);
        mFlags[57] = true;
        }
    }

    // ---- +0x2114  52 bytes ----
    if (!mFlags[58]) {
        if (!MissionUtility::IsAlive(mHandles[8])) {
        mFloats[2] = mFloats[2] - 1.0f;
        mFlags[58] = true;
        }
    }

    // ---- +0x2148  52 bytes ----
    if (!mFlags[59]) {
        if (!MissionUtility::IsAlive(mHandles[7])) {
        mFloats[2] = mFloats[2] - 1.0f;
        mFlags[59] = true;
        }
    }

    // ---- +0x217c  52 bytes ----
    if (!mFlags[60]) {
        if (!MissionUtility::IsAlive(mHandles[6])) {
        mFloats[2] = mFloats[2] - 1.0f;
        mFlags[60] = true;
        }
    }

    // ---- +0x21b0  52 bytes ----
    if (!mFlags[61]) {
        if (!MissionUtility::IsAlive(mHandles[5])) {
        mFloats[2] = mFloats[2] - 1.0f;
        mFlags[61] = true;
        }
    }

    // ---- +0x21e4  52 bytes ----
    if (!mFlags[62]) {
        if (!MissionUtility::IsAlive(mHandles[4])) {
        mFloats[2] = mFloats[2] - 1.0f;
        mFlags[62] = true;
        }
    }

    // ---- +0x2218  64 bytes ----
    if (!mFlags[64]) {
        if (1.0f == mFloats[2]) {
        MissionUtility::QueueSound("OBR08_15", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[64] = true;
        }
    }

    // ---- +0x2258  68 bytes ----
    if (!mFlags[63]) {
        if (2.0f == mFloats[2]) {
        MissionUtility::QueueSound("OBR08_14", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[63] = true;
        }
    }

    // ---- +0x229c  116 bytes ----
    if (mFlags[13]) {
        if (!MissionUtility::IsAlive(mHandles[4])) {
        if (!MissionUtility::IsAlive(mHandles[5])) {
        if (!MissionUtility::IsAlive(mHandles[6])) {
        if (!MissionUtility::IsAlive(mHandles[7])) {
        if (!MissionUtility::IsAlive(mHandles[8])) {
        StopTimer(mTimer2);
        BeginTimer(mTimer1);
        mFlags[13] = false;
        }
        }
        }
        }
        }
    }

    // ---- +0x2310  420 bytes ----
    if (!mFlags[12]) {
        if (mTimer1 > 4.0f) {
        if (MissionUtility::GetCurHealth(mHandles[3]) > 190.0f) {
        MissionUtility::BonusObjectiveComplete(mHandles[16], true);
        } else {
        MissionUtility::BonusObjectiveFailed(mHandles[16]);
        }
        if (mTimer2 < 141.0f) {
        MissionUtility::BonusObjectiveComplete(mHandles[15], true);
        } else {
        MissionUtility::BonusObjectiveFailed(mHandles[15]);
        }
        if (!mFlags[67]) {
        MissionUtility::BonusObjectiveFailed(mHandles[17]);
        }
        mHandles[35] = MissionUtility::CreateObjectWithRotation("REP_bike_speeder_anakin", "WinCinObiWanBikePath", 0, "WinCinAnakin", 0, -1, -1);
        mHandles[39] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder_large", "WinCinDroidBikePath", 0, "WinCinDroid", 0, -1, -1);
        mHandles[34] = MissionUtility::CreateObjectWithRotation("REP_inf_obiwan_cin", "WinCinObiWanEndPath", 0, "WinCinObiWan", 0, -1, -1);
        MissionUtility::Goto(mHandles[39], "WinCinDroidBikePath", true);
        MissionUtility::GotoFire(mHandles[35], "WinCinObiWanBikePath", true, true, true, false);
        MissionUtility::SetVelocForward(mHandles[39], 125.0f);
        MissionUtility::SetCurHealth(mHandles[39], 1.0f);
        MissionUtility::SetCurShield(mHandles[39], 0.0f);
        MissionUtility::SetFogRange(200.0f, 1800.0f, 2.0f);
        MissionUtility::RemoveText();
        mHandles[13] = MissionUtility::RunCin("wincin", true, true);
        MissionUtility::PlayMusic("EP4_V1_T03_04", true);
        MissionUtility::ObjectiveComplete(mInts[0]);
        mFlags[12] = true;
        mFlags[14] = true;
        BeginTimer(mTimer10);
        MissionUtility::SetVisible(mHandles[3], false);
        }
    }

    // ---- +0x24b4  168 bytes ----
    if (mTimer10 > 3.0f) {
    BeginTimer(mTimer11);
    MissionUtility::DamageObject(mHandles[39], 1000.0f, 1000.0f);
    StopTimer(mTimer10);
    mTimer10 = 0.0f;
    MissionUtility::Goto(mHandles[35], "WinCinObiWanBikePath", false);
    mHandles[36] = MissionUtility::CreateObjectWithRotation("REP_bike_speeder_anakin", "WinCinObiWanPath1", 0, "WinCinObiWan1", 0, -1, -1);
    MissionUtility::Goto(mHandles[36], "WinCinObiWanPath1", true);
    mHandles[41] = MissionUtility::CreateObjectWithRotation("rax_bldg_crane", "WinCinCranePath", 0, "WinCinCrane", 0, -1, -1);
    }

    // ---- +0x255c  136 bytes ----
    if (mTimer11 > 4.0f) {
    StopTimer(mTimer11);
    mTimer11 = 0.0f;
    MissionUtility::QueueSound("ASR08_07", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("OBR08_08", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("OBR08_09", 1.0f, 0.0f, 0.0f, "", 0, "");
    }

    // ---- +0x25e4  520 bytes ----
    if (mFlags[14] && !mFlags[77]) {
        if (MissionUtility::GetCinId(mHandles[13]) == 3) {
        mHandles[37] = MissionUtility::CreateObjectWithRotation("REP_inf_anakin_cin", "WinCinObiWanPath2", 0, "WinCinObiWan2", 0, -1, -1);
        MissionUtility::RemoveObject(mHandles[36]);
        MissionUtility::RemoveObject(mHandles[35]);
        MissionUtility::Goto(mHandles[37], "WinCinObiWanPath2", true);
        mHandles[42] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "WinCinShipPath1", 0, "WinCinShip1", 0, -1, -1);
        mHandles[43] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "WinCinShipPath2", 0, "WinCinShip2", 0, -1, -1);
        mHandles[44] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "WinCinShipPath3", 0, "WinCinShip3", 0, -1, -1);
        mHandles[45] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "WinCinShipPath4", 0, "WinCinShip4", 0, -1, -1);
        mHandles[46] = MissionUtility::CreateObjectWithRotation("CIS_fly_technounion_rax3", "WinCinShipPath5", 0, "WinCinShip5", 0, -1, -1);
        MissionUtility::SetApplyDynamics(mHandles[46], true);
        MissionUtility::Land(mHandles[46], "WinCinShipLandPath", 0, 80.0f);
        MissionUtility::Goto(mHandles[42], "WinCinShipPath1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[42], "WinCinShipPath1", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[43], "WinCinShipPath2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[43], "WinCinShipPath2", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[44], "WinCinShipPath3", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[44], "WinCinShipPath3", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[45], "WinCinShipPath4", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[45], "WinCinShipPath4", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[77] = true;
        }
    }

    // ---- +0x27ec  44 bytes ----
    if (mFlags[10]) {
        if (!MissionUtility::IsAlive(mHandles[3])) {
        BeginTimer(mTimer3);
        mFlags[10] = false;
        }
    }

    // ---- +0x2818  40 bytes ----
    if (!mFlags[10]) {
        if (mTimer3 > 4.0f) {
        mFlags[15] = true;
        }
    }

    // ---- +0x2840  316 bytes ----
    if (mFlags[11]) {
        if ((MissionUtility::IsAlive(mHandles[4]) && MissionUtility::IsInsideRegion(mHandles[4], "endarea"))
            || (MissionUtility::IsAlive(mHandles[5]) && MissionUtility::IsInsideRegion(mHandles[5], "endarea"))
            || (MissionUtility::IsAlive(mHandles[6]) && MissionUtility::IsInsideRegion(mHandles[6], "endarea"))
            || (MissionUtility::IsAlive(mHandles[7]) && MissionUtility::IsInsideRegion(mHandles[7], "endarea"))
            || (MissionUtility::IsAlive(mHandles[8]) && MissionUtility::IsInsideRegion(mHandles[8], "endarea"))) {
        MissionUtility::SetTeamNum(mHandles[4], 0);
        MissionUtility::SetTeamNum(mHandles[5], 0);
        MissionUtility::SetTeamNum(mHandles[6], 0);
        MissionUtility::SetTeamNum(mHandles[7], 0);
        MissionUtility::SetTeamNum(mHandles[8], 0);
        MissionUtility::RemoveText();
        MissionUtility::SetFogRange(200.0f, 1800.0f, 2.0f);
        mHandles[12] = MissionUtility::RunCin("losecin", true, true);
        BeginTimer(mTimer14);
        mFlags[11] = false;
        mFlags[15] = true;
        }
    }

    // ---- +0x297c  72 bytes ----
    if (mTimer14 > 3.0f) {
    MissionUtility::QueueSound("asr08_06a", 1.0f, 0.0f, 0.0f, "", 0, "");
    StopTimer(mTimer14);
    mTimer14 = 0.0f;
    }

    // ---- +0x29c4  40 bytes ----
    if (mFlags[14]) {
        if (!MissionUtility::IsCinRunning(mHandles[13])) {
        MissionUtility::MissionSuccess();
        mFlags[14] = false;
        }
    }

    // ---- +0x29ec  40 bytes ----
    if (mFlags[15]) {
        if (!MissionUtility::IsCinRunning(mHandles[12])) {
        MissionUtility::MissionFailure();
        mFlags[15] = false;
        }
    }
}


void Raxus1Script::Setup()
{
    mFlags[5]  = true;
    mFlags[6]  = false;
    mFlags[7]  = false;
    mFlags[8]  = true;
    mFlags[9]  = false;
    mFlags[11] = false;
    mFlags[10] = false;
    mFlags[13] = false;
    mFlags[14] = false;
    mFlags[15] = false;
    mFlags[16] = false;
    mFlags[17] = false;
    mFlags[18] = false;
    mFlags[20] = false;
    mFlags[23] = false;
    mFlags[24] = false;
    mFlags[25] = false;
    mFlags[19] = false;
    mFlags[26] = false;
    mFlags[27] = false;
    mFlags[30] = false;
    mFlags[31] = false;
    mFlags[32] = true;

    mFloats[0] = 999999.0f;
    mFloats[1] = 999999.0f;

    mHandles[0]  = MissionUtility::GetHandle("collector1");
    mHandles[1]  = MissionUtility::GetHandle("collector2");
    mHandles[2]  = MissionUtility::GetHandle("collector3");
    mHandles[47] = MissionUtility::GetHandle("Prop1");
    mHandles[48] = MissionUtility::GetHandle("Prop2");
    mHandles[49] = MissionUtility::GetHandle("Prop3");
    mHandles[4]  = MissionUtility::GetHandle("stap1");
    mHandles[5]  = MissionUtility::GetHandle("stap2");
    mHandles[6]  = MissionUtility::GetHandle("stap3");
    mHandles[7]  = MissionUtility::GetHandle("stap4");
    mHandles[8]  = MissionUtility::GetHandle("stap5");
    mHandles[9]  = MissionUtility::GetHandle("endspot");
    mHandles[14] = MissionUtility::GetHandle("Wall");

    mHandles[10] = 0;
    mHandles[11] = 0;
    mHandles[12] = 0;
    mHandles[13] = 0;

    MissionUtility::PreloadConfig("REP_bike_speeder_anakin");
    MissionUtility::PreloadConfig("REP_inf_obiwan_cin");
    MissionUtility::PreloadConfig("REP_inf_anakin_cin");
    MissionUtility::PreloadConfig("CIS_fly_fighter");
    MissionUtility::PreloadConfig("REP_bike_speeder_player_small");
    MissionUtility::PreloadConfig("REP_fly_gunship");
    MissionUtility::PreloadConfig("REP_fly_fighter");
    MissionUtility::PreloadConfig("RAX_bldg_crane");
    MissionUtility::PreloadConfig("REP_bike_speeder_empty");
    MissionUtility::PreloadConfig("CIS_fly_technounion_rax3");
}

SPMission *Raxus1BuildMission()
{
    return new Raxus1Script();
}
