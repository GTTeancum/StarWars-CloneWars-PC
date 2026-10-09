// Geonosis2Script.cpp -- reconstruction of a shipped mission script. 34,396 bytes, byte-exact.
//
// Hand corrections (analysis/mission_batch_b.md):
//
//   * a compare-chain `switch (mInts2[1])` the generator cannot see -- it reads jump tables
//     out of .rodata, and this one is CodeWarrior's binary-search form. The PIVOT pins the
//     number of case labels: `cmpwi 2` before `cmpwi 0` means four, not three, so there is a
//     fourth label with an empty body, emitted before case 0. Its value is pinned only to
//     "greater than 2";
//   * the two guard chains inside cases 1 and 2 are `||` chains whose body is the block that
//     follows -- the last term branches PAST the body, every earlier one INTO it;
//   * `Quat::Quat(float,float,float,float)` is out of line for the same reason Kashyyyk2's
//     copy constructor is: it is defined after `Execute`;
//   * the two `GetCurHealth` sums are written the other way round. `((t1+t2)+t3)+...` emits
//     t2, t1, t3, ... -- only the innermost pair is visibly swapped;
//   * `mFlags[19]` is tested by the assignment that sets it (`if (mFlags[19] = true)`), which
//     is why the shipped code tests a constant it never reloads;
//   * `DidPlayerShootMe` returns bool, and the block at +0xa8 is seven floats.

class Quat
{
public:
    Quat();
    Quat(float, float, float, float);

    float s, x, y, z;
};

class Vector
{
public:
    Vector(float, float, float);

    float x, y, z;
};

// Twelve bytes, constructed out of line -- pinned by the `__ct__5TimerFv` calls in the
// constructor.
class Timer
{
public:
    Timer();
    operator float();
    float operator=(float v);   // NOT Timer& -- retail returns in f1 (0x801886f0)

    int mState[3];
};

void BeginTimer(Timer &t);
void ResumeTimer(Timer &t);
void StopTimer(Timer &t);

enum Formation { kFormation0 };        // an enum in the API: only the mangling matters

namespace MissionUtility
{
    int    AddBonusObjective(const char*);
    void   AddFlockMember(int, int);
    int    AddHealthBar(int, const char*, float);
    int    AddObjective(const char*);
    void   AddTurnAroundRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    void   AttackTarget(int, int, bool, bool, bool, bool);
    void   BeginWave(const char*);
    void   BonusObjectiveComplete(int, bool);
    void   BonusObjectiveFailed(int);
    void   CarrierAddCargo(int, const char*, int, const char*, bool);
    void   CarrierDropoff(int, const char*, int, float);
    int    CreateFlock(int, Formation);
    int    CreateObject(const char*, const Vector&, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObject(const char*, const char*, const char*, int, int, int);
    int    CreateObject(const char*, const char*, int, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    void   DamageObject(int, float, float);
    bool   DidPlayerShootMe(int);
    void   DisplayText(const char*, float, float);
    void   EvictConfig(const char*);
    void   FadeInDust(float);
    void   FadeOutDust(float);
    void   Fire(int, bool, bool, bool);
    void   FlushSoundQueue();
    void   Follow(int, int);
    int    GetCinId(int);
    float  GetCurHealth(int);
    float  GetDistance(int, const char*, int);
    float  GetDistance(int, int);
    float  GetGameClock();
    int    GetHandle(const char*);
    int    GetPlayerHandle(int);
    int    GetPlayerKillCount();
    float  GetTime();
    int    GetWhoShotMe(int);
    void   Goto(int, const char*, bool);
    void   Goto(int, const char*, int);
    void   GotoDirect(int, const char*, bool);
    void   GotoFire(int, const char*, bool, bool, bool, bool);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsDropped(int);
    bool   IsFlockAlive(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsWaveSpawned(const char*);
    void   Land(int, const char*, int, float);
    int    MidMissionGetSavePoint();
    void   MidMissionLoad(bool&);
    void   MidMissionLoad(float&);
    void   MidMissionLoad(int&);
    void   MidMissionLoadPlayer();
    void   MidMissionSave(bool);
    void   MidMissionSave(float);
    void   MidMissionSave(int);
    void   MidMissionSavePlayer(int);
    void   MissionFailure();
    void   MissionSuccess();
    void   MoveObject(int, const char*, int, bool);
    void   MoveObjectWithRotation(int, const char*, int, bool);
    void   Objectify(const char*, int, const char*, bool, bool, float, float);
    void   Objectify(int, const char*, bool, bool, float);
    void   ObjectiveComplete(int);
    void   OverrideSoundRange(int, bool);
    void   Patrol(int, const char*, float, bool);
    void   PlayMusic(const char*, bool);
    void   PreloadConfig(const char*);
    int    QueueSound(const char*, float, float, float, const char*, int, const char*);
    void   RemoveHealthBar(int);
    void   RemoveObject(int);
    void   RemoveObjectify(const char*, int);
    void   RemoveObjectify(int);
    void   RemoveTurnAroundRegion(const char*);
    int    RunCin(const char*, bool, bool);
    void   SetAccelThrust(int, float);
    void   SetAlliance(int, int);
    void   SetAltitude(int, float);
    void   SetAnimation(int, const char*, float, int);
    void   SetApplyDynamics(int, bool);
    void   SetAsPlayer(int, int);
    void   SetAttackRange(int, int);
    void   SetCollidable(int, bool);
    void   SetCurHealth(int, float);
    void   SetEnemies(int, int);
    void   SetEnemiesOneWay(int, int);
    void   SetFOV(float);
    void   SetFlockSeparation(int, float);
    void   SetFogRange(float, float, float);
    void   SetMapZoom(float, float);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetNeutral(int, int);
    void   SetNeutralOneWay(int, int);
    void   SetOnRadar(int, bool);
    void   SetOriginalFog(float);
    void   SetPlayerKillCount(int);
    void   SetQueueFlag(bool);
    void   SetTeamNum(int, int);
    void   SetTurnAroundMessage(const char*, const char*);
    void   SetVelocForward(int, float);
    void   SetVelocMaximumFly(int, float);
    void   SetVelocMinimumFly(int, float);
    void   SetVelocNeutralFly(int, float);
    void   SetVisible(int, bool);
    void   SetWeaponOrd(int, const char*, const char*);
    void   StartAmbiences(const char*, const char*, float, float);
    int    StartSound(const char*, bool, float, float, float, const char*, int, const char*);
    void   Stop(int);
    void   StopSound(int);
    void   Wait(int);
}

static const char *const kClassName = "Geonosis2Script";
static const char *const kRootName  = "DLLBase";
static const char *const kBaseName  = "SPMission";

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

class Geonosis2Script : public SPMission
{
public:
    virtual ~Geonosis2Script();
    Geonosis2Script()
    {
        mBoolCount = 124;   mBools = mFlags;
        mCountB    = 7;     mIntsB = (int *)mFloatsB;
        mCountC    = 381;   mIntsC = mHandles;
        mCountD    = 7;     mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    char   mPad24[1];
    bool   mFlags[124];            // +0x025  the one-shot latches
    char   mPadA1[7];                  // +0x0a1
    float  mFloatsB[7];            // +0x0a8
    char   mPadC4[8];                  // +0x0c4
    int    mHandles[381];          // +0x0cc
    char   mPad6C0[8];                  // +0x6c0
    int    mInts[7];             // +0x6c8
    char   mPad6E4[4];                  // +0x6e4
    Timer  mTimer0;                      // +0x6e8
    Timer  mTimer1;                      // +0x6f4
    Timer  mTimer2;                      // +0x700
    Timer  mTimer3;                      // +0x70c
    Timer  mTimer4;                      // +0x718
    Timer  mTimer5;                      // +0x724
    Timer  mTimer6;                      // +0x730
    Timer  mTimer7;                      // +0x73c
    Timer  mTimer8;                      // +0x748
    Timer  mTimer9;                      // +0x754
    Timer  mTimer10;                      // +0x760
    Timer  mTimer11;                      // +0x76c
    Timer  mTimer12;                      // +0x778
    Timer  mTimer13;                      // +0x784
    Timer  mTimer14;                      // +0x790
    Timer  mTimer15;                      // +0x79c
    Timer  mTimer16;                      // +0x7a8
    Timer  mTimer17;                      // +0x7b4
    Timer  mTimer18;                      // +0x7c0
    Timer  mTimer19;                      // +0x7cc
    Timer  mTimer20;                      // +0x7d8
    Timer  mTimer21;                      // +0x7e4
    Timer  mTimer22;                      // +0x7f0
    int    mInts2[2];               // +0x7fc
};

Geonosis2Script::~Geonosis2Script()
{
}

void Geonosis2Script::Execute()
{
    // The two mid-mission restore blocks read the mission clock into a stack float; slots
    // 0xc then 0x8, so declaration order is the textual order of the two blocks.
    float mLoadTA, mLoadTB;

    // ---- +0x000c  1692 bytes ----
    if (mFlags[3]) {
    MissionUtility::SetMusicLooping(true);
    MissionUtility::SetEnemies(1, 2);
    MissionUtility::SetAlliance(1, 0);
    MissionUtility::SetAlliance(2, 0);
    MissionUtility::SetNeutral(2, 3);
    MissionUtility::SetAlliance(1, 3);
    MissionUtility::SetEnemiesOneWay(1, 4);
    MissionUtility::SetNeutralOneWay(4, 1);
    MissionUtility::SetAlliance(2, 4);
    MissionUtility::SetAlliance(5, 1);
    MissionUtility::SetNeutral(5, 4);
    MissionUtility::SetEnemies(5, 2);
    MissionUtility::SetAlliance(5, 3);
    MissionUtility::SetEnemies(8, 9);
    MissionUtility::SetAlliance(1, 9);
    MissionUtility::SetAlliance(2, 9);
    MissionUtility::SetAlliance(3, 9);
    MissionUtility::SetAlliance(4, 9);
    MissionUtility::SetAlliance(5, 9);
    MissionUtility::SetNeutral(1, 8);
    MissionUtility::SetAlliance(5, 8);
    MissionUtility::SetAlliance(4, 8);
    MissionUtility::SetAlliance(2, 8);
    MissionUtility::SetAlliance(3, 8);
    MissionUtility::SetEnemies(10, 11);
    MissionUtility::SetAlliance(10, 1);
    MissionUtility::SetAlliance(10, 2);
    MissionUtility::SetAlliance(10, 8);
    MissionUtility::SetAlliance(10, 9);
    MissionUtility::SetAlliance(11, 1);
    MissionUtility::SetAlliance(11, 2);
    MissionUtility::SetAlliance(11, 8);
    MissionUtility::SetAlliance(11, 9);
    MissionUtility::Stop(mHandles[13]);
    MissionUtility::SetCurHealth(mHandles[13], 2100.0f);
    MissionUtility::SetMaxHealth(mHandles[13], 2100.0f);
    mFloatsB[5] = 2100.0f;
    mFloatsB[6] = 2100.0f;
    MissionUtility::SetCurHealth(mHandles[106], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[92], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[102], 99999.0f);
    MissionUtility::SetWeaponOrd(mHandles[13], "REP_blaster_fighter1", "REP_blaster_fighter1_ord_luminara");
    MissionUtility::SetAttackRange(mHandles[13], 300);
    MissionUtility::SetVelocForward(mHandles[13], 125.0f);
    MissionUtility::SetAccelThrust(mHandles[13], 90.0f);
    MissionUtility::SetAttackRange(mHandles[112], 350);
    MissionUtility::Stop(mHandles[140]);
    MissionUtility::Stop(mHandles[141]);
    MissionUtility::Stop(mHandles[142]);
    MissionUtility::Stop(mHandles[143]);
    MissionUtility::Stop(mHandles[144]);
    MissionUtility::Stop(mHandles[145]);
    MissionUtility::Stop(mHandles[154]);
    MissionUtility::Stop(mHandles[155]);
    MissionUtility::Stop(mHandles[189]);
    MissionUtility::Stop(mHandles[190]);
    MissionUtility::Stop(mHandles[156]);
    MissionUtility::Stop(mHandles[157]);
    MissionUtility::Stop(mHandles[158]);
    MissionUtility::Stop(mHandles[159]);
    MissionUtility::Stop(mHandles[160]);
    MissionUtility::Stop(mHandles[161]);
    MissionUtility::Stop(mHandles[162]);
    MissionUtility::Stop(mHandles[163]);
    MissionUtility::Stop(mHandles[164]);
    MissionUtility::Stop(mHandles[165]);
    MissionUtility::Stop(mHandles[166]);
    MissionUtility::Stop(mHandles[167]);
    MissionUtility::Stop(mHandles[168]);
    MissionUtility::Stop(mHandles[169]);
    MissionUtility::Stop(mHandles[170]);
    MissionUtility::Stop(mHandles[171]);
    MissionUtility::Stop(mHandles[172]);
    MissionUtility::Stop(mHandles[173]);
    MissionUtility::Stop(mHandles[174]);
    MissionUtility::Stop(mHandles[176]);
    MissionUtility::Stop(mHandles[177]);
    MissionUtility::Stop(mHandles[178]);
    MissionUtility::Stop(mHandles[179]);
    MissionUtility::Stop(mHandles[180]);
    MissionUtility::Stop(mHandles[181]);
    MissionUtility::Stop(mHandles[182]);
    MissionUtility::Stop(mHandles[183]);
    MissionUtility::Stop(mHandles[184]);
    MissionUtility::Stop(mHandles[175]);
    MissionUtility::SetVelocNeutralFly(mHandles[41], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[42], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[43], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[44], 0.0f);
    MissionUtility::Stop(mHandles[41]);
    MissionUtility::Stop(mHandles[42]);
    MissionUtility::Stop(mHandles[43]);
    MissionUtility::Stop(mHandles[44]);
    MissionUtility::SetAttackRange(mHandles[304], 1);
    MissionUtility::SetAttackRange(mHandles[305], 1);
    MissionUtility::Stop(mHandles[304]);
    MissionUtility::Stop(mHandles[305]);
    MissionUtility::Stop(mHandles[306]);
    MissionUtility::Stop(mHandles[307]);
    MissionUtility::Stop(mHandles[308]);
    MissionUtility::Stop(mHandles[309]);
    MissionUtility::Stop(mHandles[310]);
    MissionUtility::Stop(mHandles[311]);
    MissionUtility::Stop(mHandles[312]);
    MissionUtility::Stop(mHandles[313]);
    MissionUtility::Stop(mHandles[314]);
    MissionUtility::Stop(mHandles[315]);
    MissionUtility::Stop(mHandles[290]);
    MissionUtility::Stop(mHandles[291]);
    MissionUtility::Stop(mHandles[292]);
    MissionUtility::Stop(mHandles[293]);
    MissionUtility::Stop(mHandles[294]);
    MissionUtility::Stop(mHandles[295]);
    MissionUtility::Stop(mHandles[296]);
    MissionUtility::Stop(mHandles[297]);
    MissionUtility::Stop(mHandles[298]);
    MissionUtility::Stop(mHandles[299]);
    mHandles[252] = 0;
    MissionUtility::SetCurHealth(MissionUtility::GetHandle("stagingarea"), 999999.0f);
    MissionUtility::StartAmbiences("AmbGeon_plains01_pl2", "AmbGeon_plains_stinger01", 10.0f, 30.0f);
    MissionUtility::FadeOutDust(0.0f);
    MissionUtility::Goto(mHandles[6], "r41path", true);
    MissionUtility::Goto(mHandles[7], "r42path", true);
    MissionUtility::Goto(mHandles[8], "r43path", true);
    MissionUtility::SetAltitude(mHandles[8], 1.2f);
    MissionUtility::SetTurnAroundMessage("", "LMG02_30");
    MissionUtility::AddTurnAroundRegion("turnaround1", "turnaround1pt", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("turnaround2", "turnaround2pt", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("turnaround3", "turnaround3pta", "turnaround3ptb", "turnaround3ptc", "turnaround3ptd", "turnaround3pte");
    MissionUtility::AddTurnAroundRegion("turnaround4", "turnaround4pta", "turnaround4ptb", "turnaround4ptc", "turnaround4ptd", "turnaround4pte");
    MissionUtility::AddTurnAroundRegion("turnaround5", "turnaround5pta", "turnaround5ptb", "turnaround5ptc", "turnaround5ptd", "turnaround5pte");
    MissionUtility::AddTurnAroundRegion("turnaround6", "turnaround6pt", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("turnaround7", "turnaround7pta", "turnaround7ptb", "turnaround7ptc", "turnaround7ptd", "turnaround7pte");
    MissionUtility::AddTurnAroundRegion("turnaround8", "turnaround8pta", "turnaround8ptb", "turnaround8ptc", "turnaround8ptd", "turnaround8pte");
    MissionUtility::AddTurnAroundRegion("powerupcintrigger", "destroygun1turnpt", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("platformdelete", "oldplatformdeletept", 0, 0, 0, 0);
    mInts2[0] = MissionUtility::MidMissionGetSavePoint();
    MissionUtility::Fire(mHandles[103], true, true, false);
    MissionUtility::Fire(mHandles[104], true, true, false);
    MissionUtility::Fire(mHandles[102], true, true, false);
    mHandles[31] = MissionUtility::AddObjective("missions.Geonosis2.objective.str0011");
    mHandles[38] = MissionUtility::AddBonusObjective("missions.Geonosis2.bonus.str0009");
    mHandles[40] = MissionUtility::AddBonusObjective("missions.Geonosis2.bonus.str0008");
    mHandles[39] = MissionUtility::AddBonusObjective("missions.Geonosis2.bonus.str0005");
    mFlags[3] = false;
    }

    // ---- +0x06a8  80 bytes ----
    mHandles[9] = MissionUtility::GetPlayerHandle(0);
    if (!mFlags[16]) {
    if (MissionUtility::DidPlayerShootMe(mHandles[13])) {
    MissionUtility::QueueSound("LMG03_27", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[16] = true;
    }
    }

    // ---- +0x06f8  64 bytes ----
    if (!mFlags[17]) {
        if (!MissionUtility::IsAlive(mHandles[9])) {
        BeginTimer(mTimer14);
        MissionUtility::SetTeamNum(mHandles[13], 0);
        mInts2[0] = 11;
        mFlags[17] = true;
        }
    }

    // ---- +0x0738  132 bytes ----
    if (!mFlags[18] && !mFlags[24]) {
        if (!MissionUtility::IsAlive(mHandles[13])) {
        mInts[1] = MissionUtility::AddObjective("missions.Geonosis2.objective.str0004");
        MissionUtility::BonusObjectiveFailed(mInts[1]);
        MissionUtility::StartSound("LMDS004R", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        BeginTimer(mTimer14);
        MissionUtility::SetTeamNum(mHandles[9], 0);
        mInts2[0] = 11;
        mFlags[18] = true;
        }
    }

    // ---- +0x07bc  312 bytes ----
    if (!mFlags[24]) {
        if (!mFlags[26]) {
        if (MissionUtility::GetCurHealth(mHandles[13]) < 150.0f) {
        MissionUtility::QueueSound("LMG02_31", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[27] = true;
        mFlags[28] = true;
        mFlags[26] = true;
        }
        }
        if (!mFlags[27]) {
        mFloatsB[5] = MissionUtility::GetCurHealth(mHandles[13]);
        BeginTimer(mTimer3);
        mFlags[28] = false;
        mFlags[27] = true;
        }
        if (!mFlags[28]) {
        if (mTimer3 > 5.0f) {
        mFloatsB[6] = MissionUtility::GetCurHealth(mHandles[13]);
        mFlags[27] = false;
        mFlags[28] = true;
        }
        }
        if (!mFlags[32]) {
        if (mFloatsB[5] - mFloatsB[6] > 110.0f) {
        MissionUtility::QueueSound("LMG02_06A", 1.0f, 0.0f, 0.0f, "", 0, "");
        BeginTimer(mTimer4);
        mFlags[32] = true;
        }
        }
        if (mTimer4 > 9.0f) {
        mFlags[32] = false;
        }
    }

    // ---- +0x08f4  96 bytes ----
    if (!mFlags[6]) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[6]) < 50.0f) {
        MissionUtility::StartSound("Ifc_LowShield01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::DisplayText("missions.Geonosis2.text.str0024", 5.0f, -1.0f);
        mFlags[6] = true;
        }
    }

    // ---- +0x0954  96 bytes ----
    if (!mFlags[7]) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[7]) < 50.0f) {
        MissionUtility::StartSound("Ifc_LowShield01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::DisplayText("missions.Geonosis2.text.str0024", 5.0f, -1.0f);
        mFlags[7] = true;
        }
    }

    // ---- +0x09b4  96 bytes ----
    if (!mFlags[8]) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[8]) < 20.0f) {
        MissionUtility::StartSound("Ifc_LowShield01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::DisplayText("missions.Geonosis2.text.str0024", 5.0f, -1.0f);
        mFlags[8] = true;
        }
    }

    // ---- +0x0a14  84 bytes ----
    if (!mFlags[9] && mFlags[6] && mFlags[7] && mFlags[8]) {
        MissionUtility::DisplayText("missions.Geonosis2.text.str0021", 5.0f, -1.0f);
        MissionUtility::BonusObjectiveComplete(mHandles[40], true);
        mFlags[9] = true;
    }

    // ---- +0x0a68  60 bytes ----
    if (!mFlags[10]) {
        if (MissionUtility::GetPlayerKillCount() >= 100) {
        MissionUtility::DisplayText("missions.Geonosis2.text.str0021", 5.0f, -1.0f);
        MissionUtility::BonusObjectiveComplete(mHandles[39], true);
        mFlags[10] = true;
        }
    }

    // ---- +0x0aa4  48 bytes ----
    if (!mFlags[11]) {
        if (MissionUtility::GetGameClock() >= 540.0f) {
        MissionUtility::BonusObjectiveFailed(mHandles[38]);
        mFlags[11] = true;
        }
    }

    switch (mInts2[0]) {
    case 0:
    // ---- +0x0af8  552 bytes ----
    mHandles[370] = MissionUtility::CreateObjectWithRotation("rep_inf_mace", "OpenCinMaceTempPath", 0, "OpenCinMace", 0, -1, -1);
    mHandles[369] = MissionUtility::CreateObjectWithRotation("rep_inf_luminara", "OpenCinLuminaraTempPath", 0, "OpenCinLuminara", 0, -1, -1);
    MissionUtility::MoveObjectWithRotation(mHandles[370], "OpenCinMaceA", 0, true);
    MissionUtility::MoveObjectWithRotation(mHandles[369], "OpenCinLuminaraA", 0, true);
    MissionUtility::SetCollidable(mHandles[370], false);
    MissionUtility::SetCollidable(mHandles[369], false);
    MissionUtility::SetVisible(mHandles[370], false);
    MissionUtility::SetVisible(mHandles[369], false);
    MissionUtility::SetFogRange(1200.0f, 3500.0f, 0.0f);
    mHandles[377] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter_cin", "OpenCinFighterPathB", 0, "OpenCinFighterB", 0, -1, -1);
    mHandles[378] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinFighter1PathB", 0, "OpenCinFighter1B", 0, -1, -1);
    MissionUtility::OverrideSoundRange(mHandles[377], true);
    MissionUtility::OverrideSoundRange(mHandles[378], true);
    MissionUtility::SetApplyDynamics(mHandles[377], true);
    MissionUtility::SetApplyDynamics(mHandles[378], true);
    MissionUtility::SetVelocMaximumFly(mHandles[377], 180.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[377], 180.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[377], 180.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[378], 170.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[378], 170.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[378], 170.0f);
    MissionUtility::SetCurHealth(mHandles[371], 10.0f);
    MissionUtility::SetCurHealth(mHandles[372], 10.0f);
    MissionUtility::SetCurHealth(mHandles[373], 10.0f);
    MissionUtility::SetCurHealth(mHandles[374], 10.0f);
    MissionUtility::Goto(mHandles[377], "OpenCinFighterPathB", false);
    MissionUtility::Goto(mHandles[378], "OpenCinFighter1PathB", false);
    MissionUtility::Fire(mHandles[103], true, true, false);
    MissionUtility::Fire(mHandles[104], true, true, false);
    MissionUtility::Fire(mHandles[102], true, true, false);
    mHandles[45] = MissionUtility::RunCin("fopencin", true, true);
    MissionUtility::PlayMusic("EP1_V1_T05", true);
    mInts2[0] = 1;

        break;
    case 1:
    // ---- +0x0d20  556 bytes ----
    if (!mFlags[120] && !mFlags[119]) {
        if (MissionUtility::GetCinId(mHandles[45]) == 2) {
        MissionUtility::SetOriginalFog(2.0f);
        MissionUtility::Fire(mHandles[103], true, true, false);
        mHandles[363] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter_cin", "OpenCinFighterPath", 0, "OpenCinFighter", 0, -1, -1);
        mHandles[364] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinFighter1Path", 0, "OpenCinFighter1", 0, -1, -1);
        mHandles[375] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinFighterPathA", 0, "OpenCinFighterA", 0, -1, -1);
        mHandles[376] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinFighter1PathA", 0, "OpenCinFighter1A", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[375], true);
        MissionUtility::OverrideSoundRange(mHandles[376], true);
        MissionUtility::SetApplyDynamics(mHandles[363], true);
        MissionUtility::SetApplyDynamics(mHandles[364], true);
        MissionUtility::SetApplyDynamics(mHandles[375], true);
        MissionUtility::SetApplyDynamics(mHandles[376], true);
        MissionUtility::SetVelocMaximumFly(mHandles[363], 180.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[363], 180.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[363], 180.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[364], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[364], 170.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[364], 170.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[375], 180.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[375], 180.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[375], 180.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[376], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[376], 170.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[376], 170.0f);
        MissionUtility::Goto(mHandles[363], "OpenCinFighterPath", false);
        MissionUtility::Goto(mHandles[364], "OpenCinFighter1Path", false);
        MissionUtility::Goto(mHandles[375], "OpenCinFighterPathA", false);
        MissionUtility::Goto(mHandles[376], "OpenCinFighter1PathA", false);
        BeginTimer(mTimer21);
        BeginTimer(mTimer20);
        MissionUtility::SetVisible(mHandles[363], false);
        MissionUtility::SetVisible(mHandles[364], false);
        MissionUtility::RemoveObject(mHandles[377]);
        MissionUtility::RemoveObject(mHandles[378]);
        mFlags[119] = true;
        }
    }

    // ---- +0x0f4c  120 bytes ----
    if (!mFlags[120]) {
        if (mTimer20 > 6.0f) {
        StopTimer(mTimer20);
        mTimer20 = 0.0f;
        mHandles[52] = MissionUtility::QueueSound("MWG02_14", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("LMG02_13", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
    }

    // ---- +0x0fc4  108 bytes ----
    if (!mFlags[120]) {
        if (mTimer21 > 17.0f) {
        StopTimer(mTimer21);
        mTimer21 = 0.0f;
        MissionUtility::GotoFire(mHandles[363], "OpenCinFighterPath", false, true, true, false);
        MissionUtility::GotoFire(mHandles[364], "OpenCinFighter1Path", false, true, true, false);
        }
    }

    // ---- +0x1030  100 bytes ----
    if (!mFlags[120] && mFlags[119]) {
        if (MissionUtility::IsAlive(mHandles[371])) {
        if (MissionUtility::GetDistance(mHandles[363], mHandles[371]) < 100.0f
            || mHandles[363] == MissionUtility::GetWhoShotMe(mHandles[371])) {
        MissionUtility::DamageObject(mHandles[371], 10000.0f, 10000.0f);
        }
        }
    }

    // ---- +0x1094  100 bytes ----
    if (!mFlags[120] && mFlags[119]) {
        if (MissionUtility::IsAlive(mHandles[372])) {
        if (MissionUtility::GetDistance(mHandles[363], mHandles[372]) < 100.0f
            || mHandles[363] == MissionUtility::GetWhoShotMe(mHandles[372])) {
        MissionUtility::DamageObject(mHandles[372], 10000.0f, 10000.0f);
        }
        }
    }

    // ---- +0x10f8  100 bytes ----
    if (!mFlags[120] && mFlags[119]) {
        if (MissionUtility::IsAlive(mHandles[373])) {
        if (MissionUtility::GetDistance(mHandles[364], mHandles[373]) < 100.0f
            || mHandles[364] == MissionUtility::GetWhoShotMe(mHandles[373])) {
        MissionUtility::DamageObject(mHandles[373], 10000.0f, 10000.0f);
        }
        }
    }

    // ---- +0x115c  224 bytes ----
    if (!mFlags[120] && mFlags[119]) {
        if (MissionUtility::IsAlive(mHandles[374])) {
        if (MissionUtility::GetDistance(mHandles[364], mHandles[374]) < 100.0f
            || mHandles[364] == MissionUtility::GetWhoShotMe(mHandles[374])) {
        MissionUtility::DamageObject(mHandles[374], 10000.0f, 10000.0f);
        MissionUtility::SetCollidable(mHandles[363], false);
        MissionUtility::Goto(mHandles[363], "OpenCinFighterPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Land(mHandles[363], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[364], "OpenCinFighter1Path", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Land(mHandles[364], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        mFlags[123] = true;
        }
        }
    }

    // ---- +0x123c  112 bytes ----
    if (!mFlags[120] && !mFlags[36]) {
        if (MissionUtility::GetCinId(mHandles[45]) == 3) {
        MissionUtility::RemoveObject(mHandles[375]);
        MissionUtility::RemoveObject(mHandles[376]);
        MissionUtility::OverrideSoundRange(mHandles[363], true);
        MissionUtility::OverrideSoundRange(mHandles[364], true);
        MissionUtility::SetVisible(mHandles[363], true);
        MissionUtility::SetVisible(mHandles[364], true);
        mFlags[36] = true;
        }
    }

    // ---- +0x12ac  480 bytes ----
    if (!mFlags[120] && !mFlags[113]) {
        if (MissionUtility::GetCinId(mHandles[45]) == 3) {
        mFlags[113] = true;
        mHandles[365] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "OpenCinCarrierPath", 0, "OpenCinCarrier", 0, -1, -1);
        mHandles[367] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "OpenCinDropOff", 0, "OpenCinTank", 0, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[365], "hp_link_1", mHandles[367], "hp_link_1", true);
        mHandles[366] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "OpenCinCarrier1Path", 0, "OpenCinCarrier1", 0, -1, -1);
        mHandles[368] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "OpenCinDropOff1", 0, "OpenCinTank1", 0, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[366], "hp_link_1", mHandles[368], "hp_link_1", true);
        MissionUtility::OverrideSoundRange(mHandles[365], true);
        MissionUtility::OverrideSoundRange(mHandles[367], true);
        MissionUtility::OverrideSoundRange(mHandles[366], true);
        MissionUtility::OverrideSoundRange(mHandles[368], true);
        MissionUtility::SetApplyDynamics(mHandles[365], true);
        MissionUtility::SetApplyDynamics(mHandles[366], true);
        MissionUtility::Goto(mHandles[365], "OpenCinCarrierPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[365], "OpenCinDropOff", 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[365], "OpenCinCarrierLeave", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[366], "OpenCinCarrier1Path", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[366], "OpenCinDropOff1", 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[366], "OpenCinCarrierLeave", false);
        MissionUtility::SetQueueFlag(false);
        }
    }

    // ---- +0x148c  20 bytes ----
    if (!mFlags[35]) {
        mFlags[35] = true;
    }

    // ---- +0x14a0  64 bytes ----
    if (!mFlags[120] && !mFlags[114]) {
        if (MissionUtility::IsDropped(mHandles[367])) {
        if (MissionUtility::IsDropped(mHandles[368])) {
        mFlags[114] = true;
        }
        }
    }

    // ---- +0x14e0  256 bytes ----
    if (!mFlags[120] && !mFlags[37]) {
        if (MissionUtility::GetCinId(mHandles[45]) == 6) {
        mFlags[37] = true;
        MissionUtility::RemoveObject(mHandles[363]);
        MissionUtility::RemoveObject(mHandles[364]);
        mHandles[363] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinFighterLand", 0, "OpenCinFighter", 0, -1, -1);
        mHandles[364] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinFighterLand1", 0, "OpenCinFighter1", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[363], true);
        MissionUtility::OverrideSoundRange(mHandles[364], true);
        MissionUtility::Land(mHandles[363], 0, 0, 80.0f);
        MissionUtility::Land(mHandles[364], 0, 0, 80.0f);
        mFlags[118] = true;
        MissionUtility::SetVisible(mHandles[370], true);
        MissionUtility::SetVisible(mHandles[369], true);
        MissionUtility::OverrideSoundRange(mHandles[370], true);
        MissionUtility::OverrideSoundRange(mHandles[369], true);
        }
    }

    // ---- +0x15e0  116 bytes ----
    if (!mFlags[120]) {
        if (mTimer19 > 3.0f) {
        MissionUtility::QueueSound("MWG02_15", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("LMG02_14", 1.0f, 0.0f, 0.0f, "", 0, "");
        StopTimer(mTimer19);
        mTimer19 = 0.0f;
        }
    }

    // ---- +0x1654  80 bytes ----
    if (!mFlags[120] && !mFlags[39]) {
        if (MissionUtility::GetCinId(mHandles[45]) == 6) {
        MissionUtility::Goto(mHandles[370], "OpenCinDropOff", false);
        MissionUtility::Goto(mHandles[369], "OpenCinDropOff1", false);
        mFlags[39] = true;
        }
    }

    // ---- +0x16a4  40 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[45])) {
    if (MissionUtility::IsAlive(mHandles[5])) {
    MissionUtility::RemoveObject(mHandles[5]);
    }

    // ---- +0x16cc  228 bytes ----
    mFlags[120] = true;
    MissionUtility::MoveObjectWithRotation(mHandles[10], "OpenCinDropOff", 0, true);
    MissionUtility::MoveObjectWithRotation(mHandles[13], "OpenCinDropOff1", 0, true);
    mInts2[0] = 12;
    MissionUtility::RemoveObject(mHandles[370]);
    MissionUtility::RemoveObject(mHandles[369]);
    MissionUtility::RemoveObject(mHandles[367]);
    MissionUtility::RemoveObject(mHandles[368]);
    MissionUtility::RemoveObject(mHandles[365]);
    MissionUtility::RemoveObject(mHandles[366]);
    MissionUtility::RemoveObject(mHandles[375]);
    MissionUtility::RemoveObject(mHandles[376]);
    MissionUtility::RemoveObject(mHandles[363]);
    MissionUtility::RemoveObject(mHandles[364]);
    MissionUtility::RemoveObject(mHandles[377]);
    MissionUtility::RemoveObject(mHandles[378]);
    MissionUtility::RemoveObject(mHandles[371]);
    MissionUtility::RemoveObject(mHandles[372]);
    MissionUtility::RemoveObject(mHandles[373]);
    MissionUtility::RemoveObject(mHandles[374]);
    MissionUtility::EvictConfig("REP_inf_luminara");
    MissionUtility::EvictConfig("REP_fly_vcarrier");
    MissionUtility::EvictConfig("REP_fly_fighter");
    MissionUtility::EvictConfig("REP_fly_fighter_cin");
    MissionUtility::SetOriginalFog(0.1f);

        break;
    case 6:
    // ---- +0x17b0  124 bytes ----
    MissionUtility::FadeOutDust(0.0f);
    MissionUtility::QueueSound("LMG03_40", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("MWG03_18B", 1.0f, 0.0f, 0.0f, "", 0, "");
    mHandles[53] = MissionUtility::RunCin("midcin1", true, true);
    MissionUtility::PlayMusic("EP2_V1_T13_01", true);
    MissionUtility::BeginWave("jedi");
    mInts2[0] = 7;

        break;
    case 7:
    // ---- +0x182c  176 bytes ----
    if (!mFlags[121] && !mFlags[43]) {
        if (MissionUtility::GetCinId(mHandles[53]) == 1) {
        mHandles[54] = MissionUtility::CreateObject("REP_tank_fighter1", "midcin1macepath", "midcin1macetank", 0, -1, -1);
        MissionUtility::SetVelocForward(mHandles[54], 90.0f);
        MissionUtility::Goto(mHandles[54], "midcin1macepath", true);
        mHandles[55] = MissionUtility::CreateObject("REP_tank_fighter1", "midcin1taospath", "midcin1taostank", 0, -1, -1);
        MissionUtility::SetVelocForward(mHandles[55], 90.0f);
        MissionUtility::Goto(mHandles[55], "midcin1taospath", true);
        BeginTimer(mTimer15);
        mFlags[43] = true;
        }
    }

    // ---- +0x18dc  160 bytes ----
    if (!mFlags[121] && !mFlags[44] && mFlags[43]) {
        if (mTimer15 > 7.0f) {
        mHandles[56] = MissionUtility::CreateObject("REP_inf_mace", "midcin1macespawn", 0, "midcin1macejedi", 0, -1);
        MissionUtility::GotoDirect(mHandles[56], "midcin1macespawn", true);
        mFlags[44] = true;
        }
    }

    // ---- +0x197c  56 bytes ----
    if (!mFlags[121]) {
        if (MissionUtility::GetCinId(mHandles[53]) == 2) {
        if (!mFlags[116]) {
        BeginTimer(mTimer18);
        mFlags[116] = true;
        }
        }
    }

    // ---- +0x19b4  68 bytes ----
    if (!mFlags[121]) {
        if (mTimer18 > 4.5f) {
        MissionUtility::Goto(mHandles[56], "MidCinMacePath", false);
        StopTimer(mTimer18);
        mTimer18 = 0.0f;
        }
    }

    // ---- +0x19f8  100 bytes ----
    if (!mFlags[121]) {
        if (MissionUtility::GetCinId(mHandles[53]) == 3) {
        if (!mFlags[115]) {
        MissionUtility::MoveObjectWithRotation(mHandles[56], "MidCinMaceJumpPath", 0, true);
        MissionUtility::SetApplyDynamics(mHandles[56], true);
        MissionUtility::SetAnimation(mHandles[56], "jumppose", 1.0f, -1);
        mFlags[115] = true;
        }
        }
    }

    // ---- +0x1a5c  300 bytes ----
    if (!mFlags[83]) {
        if (MissionUtility::IsWaveSpawned("jedi")) {
        mHandles[12] = MissionUtility::GetHandle("playerjedi1");
        mHandles[240] = MissionUtility::GetHandle("stopdroid1");
        mHandles[241] = MissionUtility::GetHandle("stopdroid2");
        mHandles[242] = MissionUtility::GetHandle("stopdroid3");
        mHandles[243] = MissionUtility::GetHandle("stopdroid4");
        mHandles[244] = MissionUtility::GetHandle("stopdroid5");
        mHandles[300] = MissionUtility::GetHandle("unstablecrate");
        mHandles[239] = MissionUtility::GetHandle("walkercincrate");
        mHandles[247] = MissionUtility::GetHandle("smallwalker1");
        mHandles[248] = MissionUtility::GetHandle("smallwalker2");
        mHandles[249] = MissionUtility::GetHandle("smallwalker3");
        mHandles[250] = MissionUtility::GetHandle("smallwalker4");
        mHandles[290] = MissionUtility::GetHandle("geo36");
        mHandles[291] = MissionUtility::GetHandle("geo37");
        mHandles[292] = MissionUtility::GetHandle("geo38");
        mHandles[293] = MissionUtility::GetHandle("geo39");
        mHandles[294] = MissionUtility::GetHandle("geo40");
        mHandles[295] = MissionUtility::GetHandle("geo41");
        mHandles[296] = MissionUtility::GetHandle("geo42");
        mHandles[297] = MissionUtility::GetHandle("geo43");
        mHandles[298] = MissionUtility::GetHandle("geo44");
        mHandles[299] = MissionUtility::GetHandle("geo45");
        mFlags[83] = true;
        }
    }

    // ---- +0x1b88  76 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[53])) {
    if (mFlags[83]) {
    mFlags[121] = true;
    MissionUtility::RemoveObject(mHandles[56]);
    MissionUtility::RemoveObject(mHandles[54]);
    MissionUtility::RemoveObject(mHandles[55]);
    MissionUtility::FlushSoundQueue();
    mInts2[0] = 21;

        break;
    case 8:
    // ---- +0x1bd4  144 bytes ----
    MissionUtility::SetTeamNum(mHandles[9], 0);
    MissionUtility::SetFogRange(1500.0f, 1600.0f, 0.5f);
    MissionUtility::QueueSound("lmg03_45", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("mwg03_26", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::PlayMusic("EP4_V2_T09_05", true);
    mHandles[60] = MissionUtility::RunCin("endcin", true, true);
    BeginTimer(mTimer5);
    mInts2[0] = 9;

        break;
    case 9:
    // ---- +0x1c64  172 bytes ----
    if (!mFlags[117]) {
        mHandles[54] = MissionUtility::CreateObjectWithRotation("REP_tank_fighter1", "EndCinTankPath", 0, "midcin1macetank", 0, -1, -1);
        mHandles[55] = MissionUtility::CreateObjectWithRotation("REP_tank_fighter1", "EndCinTankPath1", 0, "midcin1taostank", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[54], true);
        MissionUtility::OverrideSoundRange(mHandles[55], true);
        MissionUtility::SetVelocForward(mHandles[54], 250.0f);
        MissionUtility::SetVelocForward(mHandles[55], 200.0f);
        MissionUtility::Goto(mHandles[54], "EndCinTankPath", false);
        MissionUtility::Goto(mHandles[55], "EndCinTankPath1", false);
        mFlags[117] = true;
    }

    // ---- +0x1d10  48 bytes ----
    if (mTimer5 > 4.0f) {
    MissionUtility::DamageObject(mHandles[102], 999999.0f, 999999.0f);
    mInts2[0] = 23;

        break;
    case 11:
    // ---- +0x1d40  48 bytes ----
    if (mTimer14 > 2.0f) {
    if (MissionUtility::IsAlive(mHandles[9])) {
    MissionUtility::SetTeamNum(mHandles[9], 0);
    }

    // ---- +0x1d70  28 bytes ----
    if (MissionUtility::IsAlive(mHandles[13])) {
    MissionUtility::SetTeamNum(mHandles[13], 0);
    }

    // ---- +0x1d8c  12 bytes ----
    mInts2[0] = 10;

        break;
    case 10:
    // ---- +0x1d98  108 bytes ----
    MissionUtility::SetFogRange(1500.0f, 1600.0f, 0.5f);
    MissionUtility::Fire(mHandles[102], true, true, false);
    mHandles[61] = MissionUtility::RunCin("flosecin", true, true);
    if (!MissionUtility::IsAlive(mHandles[9])) {
    MissionUtility::StartSound("LMG03_31", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    }

    // ---- +0x1e04  64 bytes ----
    if (!MissionUtility::IsAlive(mHandles[13])) {
    if (!mFlags[24]) {
    MissionUtility::StartSound("MWG03_44", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    }
    }

    // ---- +0x1e44  12 bytes ----
    mInts2[0] = 24;

        break;
    case 24:
    // ---- +0x1e50  44 bytes ----
    if (!mFlags[1]) {
    if (!MissionUtility::IsCinRunning(mHandles[61])) {
    MissionUtility::MissionFailure();
    mFlags[1] = true;

        break;
    case 23:
    // ---- +0x1e7c  44 bytes ----
    if (!mFlags[0]) {
    if (!MissionUtility::IsCinRunning(mHandles[60])) {
    MissionUtility::MissionSuccess();
    mFlags[0] = true;

        break;
    case 30:
    // ---- +0x1ea8  36 bytes ----
    if (!mFlags[2]) {
    MissionUtility::SetAsPlayer(mHandles[0], 0);
    mFlags[2] = true;

        break;
    case 12:
    // ---- +0x1ecc  40 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[45])) {
    if (MissionUtility::IsAlive(mHandles[49])) {
    MissionUtility::RemoveObject(mHandles[49]);
    }

    // ---- +0x1ef4  52 bytes ----
    MissionUtility::RemoveObject(mHandles[44]);
    MissionUtility::RemoveObject(MissionUtility::GetHandle("opencintanktaos"));
    MissionUtility::RemoveObject(MissionUtility::GetHandle("opencinplayer"));
    MissionUtility::StopSound(mHandles[52]);
    mInts2[0] = 13;

        break;
    case 13:
    // ---- +0x1f28  200 bytes ----
    if (!mFlags[33]) {
        if (!MissionUtility::IsCinRunning(mHandles[45])) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::QueueSound("LMG03_25", 1.0f, 0.0f, 0.0f, "", 0, "");
        BeginTimer(mTimer1);
        BeginTimer(mTimer13);
        MissionUtility::DisplayText("missions.Geonosis2.text.str0012", 7.0f, -1.0f);
        MissionUtility::Objectify(mHandles[13], "missions.Geonosis2.marker.str0009", true, false, 200.0f);
        MissionUtility::Objectify(mHandles[13], "missions.Geonosis2.marker.str0009", false, true, 0.0f);
        MissionUtility::AddHealthBar(mHandles[13], "", 65.0f);
        MissionUtility::Fire(mHandles[103], true, true, false);
        MissionUtility::PlayMusic("EP6_V1_T07", true);
        mFlags[33] = true;
        }
    }

    // ---- +0x1ff0  104 bytes ----
    if (mFlags[33]) {
    if (mTimer1 > 1.25f) {
    MissionUtility::Patrol(mHandles[13], "taoscynpath1", 300.0f, false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[13]);
    MissionUtility::SetQueueFlag(false);
    mFlags[19] = true;
    mFlags[20] = false;
    mInts2[0] = 14;

        break;
    case 14:
    // ---- +0x2058  124 bytes ----
    if (!mFlags[20]) {
        if (MissionUtility::IsAlive(mHandles[9])) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[13]) > 400.0f) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath1", 8) > MissionUtility::GetDistance(mHandles[13], "taoscynpath1", 8)) {
        MissionUtility::SetVelocForward(mHandles[13], 0.0f);
        mFlags[19] = false;
        mFlags[20] = true;
        }
        }
        }
    }

    // ---- +0x20d4  164 bytes ----
    if (!mFlags[19]) {
        if (MissionUtility::IsAlive(mHandles[9])) {
        if (!mFlags[21]) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[13]) < 400.0f) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath1", 8) > MissionUtility::GetDistance(mHandles[13], "taoscynpath1", 8)) {
        MissionUtility::SetVelocForward(mHandles[13], 125.0f);
        StopTimer(mTimer2);
        mTimer2 = 0.0f;
        mFlags[21] = false;
        mFlags[22] = false;
        mFlags[19] = true;
        mFlags[20] = false;
        }
        }
        }
        }
    }

    // ---- +0x2178  684 bytes ----
    if (!mFlags[47]) {
    if (MissionUtility::GetDistance(mHandles[9], mHandles[92]) < 600.0f) {
    MissionUtility::AddTurnAroundRegion("destroygun1turn", "destroygun1turnpt", 0, 0, 0, 0);
    MissionUtility::Objectify(mHandles[92], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[93], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[94], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[95], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[96], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[97], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
    MissionUtility::SetTeamNum(mHandles[92], 2);
    MissionUtility::SetTeamNum(mHandles[93], 2);
    MissionUtility::SetTeamNum(mHandles[94], 2);
    MissionUtility::SetTeamNum(mHandles[95], 2);
    MissionUtility::SetTeamNum(mHandles[96], 2);
    MissionUtility::SetTeamNum(mHandles[97], 2);
    MissionUtility::DisplayText("missions.Geonosis2.text.str0013", 7.0f, -1.0f);
    MissionUtility::SetCurHealth(mHandles[92], 733.0f);
    MissionUtility::SetMaxHealth(mHandles[92], 733.0f);
    MissionUtility::SetCurHealth(mHandles[93], 733.0f);
    MissionUtility::SetMaxHealth(mHandles[93], 733.0f);
    MissionUtility::SetCurHealth(mHandles[94], 733.0f);
    MissionUtility::SetMaxHealth(mHandles[94], 733.0f);
    MissionUtility::SetCurHealth(mHandles[95], 733.0f);
    MissionUtility::SetMaxHealth(mHandles[95], 733.0f);
    MissionUtility::SetCurHealth(mHandles[96], 733.0f);
    MissionUtility::SetMaxHealth(mHandles[96], 733.0f);
    MissionUtility::SetCurHealth(mHandles[97], 733.0f);
    MissionUtility::SetMaxHealth(mHandles[97], 733.0f);
    MissionUtility::AddHealthBar(mHandles[92], 0, 400.0f);
    MissionUtility::AddHealthBar(mHandles[93], 0, 400.0f);
    MissionUtility::AddHealthBar(mHandles[94], 0, 400.0f);
    MissionUtility::AddHealthBar(mHandles[95], 0, 400.0f);
    MissionUtility::AddHealthBar(mHandles[96], 0, 400.0f);
    MissionUtility::AddHealthBar(mHandles[97], 0, 400.0f);
    MissionUtility::QueueSound("lmg03_26", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::SetVelocForward(mHandles[13], 125.0f);
    MissionUtility::Goto(mHandles[13], "taoscynpath1", false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[13]);
    MissionUtility::SetQueueFlag(false);
    mFlags[19] = true;
    mFlags[20] = false;
    mFlags[21] = false;
    mFlags[22] = false;
    StopTimer(mTimer2);
    mTimer2 = 0.0f;
    mFlags[47] = true;
    mInts2[0] = 15;

        break;
    case 15:
    // ---- +0x2424  136 bytes ----
    if (!mFlags[50]) {
        if (MissionUtility::GetCurHealth(mHandles[92]) + MissionUtility::GetCurHealth(mHandles[93]) + MissionUtility::GetCurHealth(mHandles[94]) + MissionUtility::GetCurHealth(mHandles[95]) + MissionUtility::GetCurHealth(mHandles[96]) + MissionUtility::GetCurHealth(mHandles[97]) < 700.0f) {
        MissionUtility::QueueSound("LMG03_32", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[50] = true;
        }
    }

    // ---- +0x24ac  276 bytes ----
    if (!MissionUtility::IsAlive(mHandles[92])) {
    if (!MissionUtility::IsAlive(mHandles[93])) {
    if (!MissionUtility::IsAlive(mHandles[94])) {
    if (!MissionUtility::IsAlive(mHandles[95])) {
    if (!MissionUtility::IsAlive(mHandles[96])) {
    if (!MissionUtility::IsAlive(mHandles[97])) {
    if (MissionUtility::IsInsideRegion(mHandles[9], "destroygun1area")) {
    MissionUtility::RemoveTurnAroundRegion("powerupcintrigger");
    MissionUtility::ObjectiveComplete(mHandles[31]);
    mHandles[33] = MissionUtility::AddObjective("missions.Geonosis2.objective.str0014");
    MissionUtility::DisplayText("missions.Geonosis2.text.str0014", 7.0f, -1.0f);
    MissionUtility::QueueSound("LMG03_33", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("MWG03_07", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::SetCurHealth(mHandles[103], 99999.0f);
    MissionUtility::DamageObject(mHandles[103], 999999.0f, 999999.0f);
    MissionUtility::Goto(mHandles[139], "attack1landing1", true);
    mInts2[0] = 16;
    }
    }
    }
    }
    }
    }
    }

    // ---- +0x25c0  16 bytes ----
    switch (mInts2[1]) {
    case 3:
        break;
    case 0:

    // ---- +0x25d0  548 bytes ----
    if (!mFlags[52]) {
    mHandles[113] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder", "newstapspawn", 0, "attack1alpha1", 2, -1, -1);
    BeginTimer(mTimer6);
    mFlags[52] = true;
    }
    if (!mFlags[53]) {
    mHandles[114] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder", "newstapspawn", 1, "attack1alpha2", 2, -1, -1);
    mFlags[53] = true;
    }
    if (!mFlags[54]) {
    mHandles[115] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder", "newstapspawn", 2, "attack1alpha3", 2, -1, -1);
    mHandles[118] = MissionUtility::CreateFlock(mHandles[113], (Formation)1);
    MissionUtility::SetFlockSeparation(mHandles[118], 0.5f);
    MissionUtility::AddFlockMember(mHandles[118], mHandles[114]);
    MissionUtility::AddFlockMember(mHandles[118], mHandles[115]);
    MissionUtility::SetAttackRange(mHandles[113], 150);
    MissionUtility::SetAttackRange(mHandles[114], 150);
    MissionUtility::SetAttackRange(mHandles[115], 150);
    MissionUtility::Goto(mHandles[118], "newattackpath", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[118], mHandles[9], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    mHandles[116] = MissionUtility::CreateObjectWithRotation("CIS_tank_fighter", "newtankspawn", 0, "attack1alpha4", 2, -1, -1);
    mHandles[117] = MissionUtility::CreateObjectWithRotation("CIS_tank_fighter", "newtankspawn", 2, "attack1alpha5", 2, -1, -1);
    MissionUtility::Goto(mHandles[116], "newattackpath", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[116], mHandles[9], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[117], "newattackpath", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[117], mHandles[9], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    mFlags[54] = true;
    mInts2[1] = 2;
    }

        break;
    case 1:
    // ---- +0x27f4  200 bytes ----
    if (mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[125])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[126])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[127])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[128])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[129])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[125])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[126])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[127])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[128])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[129])) {

    // ---- +0x28bc  296 bytes ----
    mHandles[119] = MissionUtility::CreateObjectWithRotation("CIS_tank_fighter", "attack1path5", 0, "attack1beta1", 2, -1, -1);
    mHandles[120] = MissionUtility::CreateObjectWithRotation("CIS_tank_fighter", "attack1path5", 0, "attack1beta2", 2, -1, -1);
    mHandles[121] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder", "attack1path5", 0, "attack1beta3", 2, -1, -1);
    mHandles[122] = MissionUtility::CreateObjectWithRotation("CIS_bike_speeder", "attack1path5", 0, "attack1beta4", 2, -1, -1);
    MissionUtility::SetAttackRange(mHandles[119], 300);
    MissionUtility::SetAttackRange(mHandles[120], 300);
    MissionUtility::SetAttackRange(mHandles[121], 300);
    MissionUtility::Goto(mHandles[119], "attack1path5", true);
    MissionUtility::Goto(mHandles[120], "attack1path5", true);
    mHandles[124] = MissionUtility::CreateFlock(mHandles[121], (Formation)4);
    MissionUtility::AddFlockMember(mHandles[124], mHandles[122]);
    MissionUtility::SetFlockSeparation(mHandles[124], 0.5f);
    MissionUtility::Goto(mHandles[124], "attack1path5", true);
    mFlags[52] = false;
    mFlags[53] = false;
    mFlags[54] = false;
    mInts2[1] = 2;

    }

        break;
    case 2:
    // ---- +0x29e4  400 bytes ----
    if (mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[113])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[114])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[115])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[116])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[117])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[113])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[114])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[115])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[116])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[117])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[119])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[120])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[121])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[122])
        || mHandles[9] == MissionUtility::GetWhoShotMe(mHandles[123])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[119])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[120])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[121])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[122])
        || mHandles[13] == MissionUtility::GetWhoShotMe(mHandles[123])) {

    // ---- +0x2b74  240 bytes ----
    mHandles[125] = MissionUtility::CreateObject("CIS_tank_fighter", "attack1path2", 0, "attack1gamma1", 2, -1);
    mHandles[126] = MissionUtility::CreateObject("CIS_tank_fighter", "attack1path2spawn", 0, "attack1gamma2", 2, -1);
    MissionUtility::SetAttackRange(mHandles[125], 300);
    MissionUtility::SetAttackRange(mHandles[126], 300);
    MissionUtility::SetAttackRange(mHandles[127], 300);
    MissionUtility::Goto(mHandles[125], "attack1path2", true);
    MissionUtility::Goto(mHandles[126], "attack1path2", true);
    mInts2[1] = 1;
    }
    }

        break;
    case 16:
    // ---- +0x2c64  116 bytes ----
    if (!mFlags[58]) {
        MissionUtility::Patrol(mHandles[13], "taoscynpath2", 600.0f, true);
        MissionUtility::SetAttackRange(mHandles[13], 600);
        MissionUtility::SetVelocForward(mHandles[13], 125.0f);
        StopTimer(mTimer14);
        mTimer14 = 0.0f;
        MissionUtility::PlayMusic("EP2_V1_T12_01", true);
        mFlags[19] = true;
        mFlags[21] = false;
        mFlags[22] = false;
        mFlags[20] = false;
        mFlags[58] = true;
    }

    // ---- +0x2cd8  108 bytes ----
    if (!mFlags[45]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "powerupcintrigger")
            || MissionUtility::IsInsideRegion(mHandles[13], "powerupcintrigger")) {
        MissionUtility::SetTeamNum(mHandles[9], 0);
        MissionUtility::SetTeamNum(mHandles[13], 0);
        MissionUtility::Stop(mHandles[13]);
        StopTimer(mTimer13);
        mFlags[45] = true;
        mInts2[0] = 25;
        }
    }

    // ---- +0x2d44  128 bytes ----
    if (!mFlags[56]) {
        mHandles[131] = MissionUtility::CreateObject("CIS_tank_fighter", "attack1path7", "attack1delta1", 2, -1, -1);
        mHandles[133] = MissionUtility::CreateObject("CIS_tank_fighter", "attack1path7", "attack1delta3", 2, -1, -1);
        mHandles[134] = MissionUtility::CreateFlock(mHandles[131], (Formation)4);
        MissionUtility::AddFlockMember(mHandles[134], mHandles[133]);
        MissionUtility::Goto(mHandles[134], "attack1path7", true);
        mFlags[56] = true;
    }

    // ---- +0x2dc4  2276 bytes ----
    if (!mFlags[57] && mFlags[56]) {
        if (!MissionUtility::IsFlockAlive(mHandles[134])
            || MissionUtility::GetDistance(mHandles[13], "taoscynpath2", 12) < 20.0f
            || MissionUtility::IsInsideRegion(mHandles[9], "spawntrigger")) {
        mHandles[135] = MissionUtility::CreateObject("CIS_tank_fighter", "attack1epath", "attack1epsilon1", 2, -1, -1);
        mHandles[136] = MissionUtility::CreateObject("CIS_tank_fighter", "attack1epath", "attack1epsilon2", 2, -1, -1);
        mHandles[137] = MissionUtility::CreateObject("CIS_tank_fighter", "attack1epath", "attack1epsilon3", 2, -1, -1);
        mHandles[138] = MissionUtility::CreateFlock(mHandles[135], (Formation)4);
        MissionUtility::AddFlockMember(mHandles[138], mHandles[136]);
        MissionUtility::AddFlockMember(mHandles[138], mHandles[137]);
        MissionUtility::Patrol(mHandles[138], "attack1epath", 500.0f, true);
        mHandles[193] = MissionUtility::CreateObject("CIS_bike_speeder", Vector(-1331.1604f, -83.11004f, -444.79102f), "attack2alpha2", 2, -1, Quat(0.998794f, -0.04909f, 0.0f, 0.0f), -1);
        mHandles[194] = MissionUtility::CreateObject("CIS_bike_speeder", Vector(-1219.6606f, -91.588264f, -657.4937f), "attack2alpha3", 2, -1, Quat(0.994158f, 0.096596f, -0.001474f, 0.048135f), -1);
        mHandles[196] = MissionUtility::CreateObject("CIS_bike_speeder", Vector(-1219.6606f, -91.588264f, -657.4937f), "attack2alpha4", 2, -1, Quat(0.994158f, 0.096596f, -0.001474f, 0.048135f), -1);
        mHandles[197] = MissionUtility::CreateObject("CIS_bike_speeder", Vector(-1059.658f, -89.797386f, -726.4414f), "attack2alpha5", 2, -1, Quat(0.998794f, -0.04909f, 0.0f, 0.0f), -1);
        mHandles[198] = MissionUtility::CreateObject("CIS_bike_speeder", Vector(-746.3131f, -75.42386f, -784.5476f), "attack2alpha6", 2, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[199] = MissionUtility::CreateObject("CIS_bike_speeder", Vector(-864.0918f, -84.96534f, -778.4294f), "attack2alpha7", 2, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[201] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(-785.6686f, -81.79578f, -883.31256f), "attack2alpha8", 2, -1, Quat(0.791602f, 0.02292f, -0.609882f, 0.029766f), -1);
        mHandles[202] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(-437.48077f, -91.40543f, -1085.4294f), "attack2alpha9", 2, -1, Quat(0.998559f, 0.0f, 0.0f, 0.053664f), -1);
        mHandles[203] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(-398.72305f, -92.95338f, -1117.6808f), "attack2alpha10", 2, -1, Quat(0.99935f, 0.0f, 0.0f, 0.036041f), -1);
        mHandles[205] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(-62.169117f, -89.37378f, -1170.3777f), "attack2alpha13", 2, -1, Quat(0.717887f, -0.001463f, -0.691926f, -0.076641f), -1);
        mHandles[204] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(-141.27238f, -79.822716f, -1136.843f), "attack2alpha12", 2, -1, Quat(0.994158f, 0.096596f, -0.001474f, 0.048135f), -1);
        mHandles[209] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(218.2563f, -78.74708f, -1215.068f), "attack2alpha99", 2, -1, Quat(0.63673f, 0.041356f, -0.769216f, -0.034233f), -1);
        mHandles[206] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(281.52124f, -90.61395f, -1155.6676f), "attack2alpha14", 2, -1, Quat(0.707649f, 0.0f, -0.706564f, 0.0f), -1);
        mHandles[207] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(698.9905f, -87.669624f, -1307.6787f), "attack2alpha15", 2, -1, Quat(0.825612f, -0.014544f, -0.559007f, -0.075262f), -1);
        mHandles[208] = MissionUtility::CreateObject("CIS_tank_fighter", Vector(724.90826f, -85.19826f, -1235.1201f), "attack2alpha16", 2, -1, Quat(0.766789f, -0.075211f, -0.632228f, 0.081642f), -1);
        mHandles[211] = MissionUtility::CreateObject("CIS_tank_wheeled", Vector(-1342.7352f, -67.02995f, -576.51855f), "attack2alpha97", 2, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[210] = MissionUtility::CreateObject("CIS_tank_wheeled", Vector(-953.08405f, -88.45332f, -667.30096f), "attack2alpha98", 2, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[195] = MissionUtility::CreateFlock(mHandles[194], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[195], mHandles[193]);
        MissionUtility::AddFlockMember(mHandles[195], mHandles[196]);
        MissionUtility::SetFlockSeparation(mHandles[195], 0.5f);
        mHandles[200] = MissionUtility::CreateFlock(mHandles[199], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[200], mHandles[197]);
        MissionUtility::AddFlockMember(mHandles[200], mHandles[198]);
        MissionUtility::SetFlockSeparation(mHandles[200], 0.5f);
        mFlags[12] = true;
        mFlags[14] = true;
        mFlags[57] = true;
        mInts2[0] = 17;
        }
    }

    // ---- +0x36a8  44 bytes ----
    if (!mFlags[55]) {
    if (MissionUtility::IsInsideRegion(mHandles[13], "gotogun2check")) {
    mFlags[55] = true;

        break;
    case 25:
    // ---- +0x36d4  320 bytes ----
    if (!mFlags[46]) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::MoveObjectWithRotation(mHandles[9], "powerupcinmove1", 0, true);
        mHandles[64] = MissionUtility::CreateObject("NEU_prop_ammo_dummy", Vector(-636.94476f, -58.690098f, 224.84851f), "dummyammo", 0, -1, Quat(-0.315503f, 0.0f, -0.948926f, 0.0f), -1);
        mHandles[65] = MissionUtility::CreateObject("NEU_prop_fullhealth_dummy", Vector(-632.25653f, -58.545834f, 217.33401f), "dummyhealth", 0, -1, Quat(1.000002f, 0.0f, 0.0f, 0.0f), 0);
        mHandles[22] = MissionUtility::RunCin("powerupcin", true, true);
        MissionUtility::QueueSound("LMG03_35", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[46] = true;
    }

    // ---- +0x3814  128 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[22])) {
    MissionUtility::SetTeamNum(mHandles[9], 1);
    MissionUtility::SetTeamNum(mHandles[13], 1);
    MissionUtility::Patrol(mHandles[13], "taoscynpath2", 300.0f, false);
    MissionUtility::SetAttackRange(mHandles[13], 300);
    ResumeTimer(mTimer13);
    MissionUtility::MoveObjectWithRotation(mHandles[9], "powerupcinmove2", 0, true);
    MissionUtility::RemoveObject(mHandles[64]);
    MissionUtility::RemoveObject(mHandles[65]);
    mInts2[0] = 16;

        break;
    case 17:
    // ---- +0x3894  100 bytes ----
    if (!mFlags[45]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "powerupcintrigger")
            || MissionUtility::IsInsideRegion(mHandles[13], "powerupcintrigger")) {
        MissionUtility::SetTeamNum(mHandles[9], 0);
        MissionUtility::SetTeamNum(mHandles[13], 0);
        MissionUtility::Stop(mHandles[13]);
        mFlags[45] = true;
        mInts2[0] = 25;
        }
    }

    // ---- +0x38f8  40 bytes ----
    if (!mFlags[55]) {
        if (MissionUtility::IsInsideRegion(mHandles[13], "gotogun2check")) {
        mFlags[55] = true;
        }
    }

    // ---- +0x3920  136 bytes ----
    if (!mFlags[20]) {
        if (MissionUtility::IsAlive(mHandles[9])) {
        if (mFlags[55]) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[13]) > 400.0f) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath2", 39) > MissionUtility::GetDistance(mHandles[13], "taoscynpath2", 39)) {
        MissionUtility::SetVelocForward(mHandles[13], 0.0f);
        mFlags[19] = false;
        mFlags[20] = true;
        }
        }
        }
        }
    }

    // ---- +0x39a8  164 bytes ----
    if (!mFlags[19]) {
        if (MissionUtility::IsAlive(mHandles[9])) {
        if (!mFlags[21]) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[13]) < 400.0f) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath2", 39) > MissionUtility::GetDistance(mHandles[13], "taoscynpath2", 39)) {
        MissionUtility::SetVelocForward(mHandles[13], 125.0f);
        StopTimer(mTimer2);
        mTimer2 = 0.0f;
        mFlags[21] = false;
        mFlags[22] = false;
        mFlags[19] = true;
        mFlags[20] = false;
        }
        }
        }
        }
    }

    // ---- +0x3a4c  256 bytes ----
    if (!mFlags[59]) {
        if (MissionUtility::IsInsideRegion(mHandles[13], "taosplatformarea")
            || MissionUtility::IsInsideRegion(mHandles[9], "platformentry")) {
        MissionUtility::Goto(mHandles[13], "taoscynpath2", 40);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[13]);
        MissionUtility::SetQueueFlag(false);
        StopTimer(mTimer2);
        mTimer2 = 0.0f;
        mFlags[19] = true;
        mFlags[20] = false;
        mFlags[21] = false;
        mFlags[22] = false;
        MissionUtility::DisplayText("missions.Geonosis2.text.str0015", 7.0f, -1.0f);
        MissionUtility::QueueSound("LMG03_34", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Objectify("platformentry", 0, "missions.Geonosis2.marker.str0006", true, false, 0.0f, 2.0f);
        BeginTimer(mTimer9);
        MissionUtility::RemoveObjectify(mHandles[13]);
        MissionUtility::RemoveObjectify(mHandles[13]);
        mInts2[0] = 18;
        mFlags[59] = true;
        }
    }

    // ---- +0x3b4c  160 bytes ----
    if (!mFlags[63]) {
        if (MissionUtility::IsInsideRegion(mHandles[13], "gotogun2check")
            || MissionUtility::IsInsideRegion(mHandles[9], "gotogun2check")) {
        StopTimer(mTimer13);
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon1"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon1a"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon1b"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon1c"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon1d"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon1e"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitbody1"));
        mInts2[0] = 2;
        mFlags[63] = true;
        }
    }

    // ---- +0x3bec  1060 bytes ----
    if (!mFlags[66]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "platformobjectify")
            || MissionUtility::IsInsideRegion(mHandles[9], "platformentry")) {
        MissionUtility::BeginWave("platform");
        mHandles[146] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(542.6251f, -18.426008f, -1169.9052f), "platformturret1", 0, -1, Quat(-0.41643f, 0.0f, -0.909168f, 0.0f), -1);
        mHandles[147] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(308.52304f, -18.426012f, -1291.4547f), "platformturret2", 0, -1, Quat(0.807979f, 0.0f, 0.589212f, 0.0f), -1);
        mHandles[148] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(143.71259f, -18.426004f, -1292.2195f), "platformturret3", 0, -1, Quat(0.85256f, 0.0f, 0.522629f, 0.0f), -1);
        mHandles[149] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(-171.40508f, 39.573997f, -1031.1848f), "platformturret4", 0, -1, Quat(0.71787f, 0.0f, 0.696177f, 0.0f), -1);
        mHandles[150] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(-180.56216f, 66.57387f, -1280.8348f), "platformturret5", 0, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[151] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(170.67348f, 66.57387f, -1312.0787f), "platformturret6", 0, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[152] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(564.3753f, 124.57303f, -1449.3162f), "platformturret7", 0, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[153] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(711.9123f, 124.57303f, -1455.5774f), "platformturret8", 0, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        MissionUtility::SetOnRadar(mHandles[146], false);
        MissionUtility::SetOnRadar(mHandles[147], false);
        MissionUtility::SetOnRadar(mHandles[148], false);
        MissionUtility::SetOnRadar(mHandles[149], false);
        MissionUtility::SetOnRadar(mHandles[150], false);
        MissionUtility::SetOnRadar(mHandles[151], false);
        MissionUtility::SetOnRadar(mHandles[152], false);
        MissionUtility::SetOnRadar(mHandles[153], false);
        mFlags[66] = true;
        }
    }

    // ---- +0x4010  776 bytes ----
    if (!mFlags[67]) {
    if (MissionUtility::IsWaveSpawned("platform")) {
    mHandles[189] = MissionUtility::GetHandle("gun2walker1");
    mHandles[190] = MissionUtility::GetHandle("gun2walker2");
    mHandles[104] = MissionUtility::GetHandle("orbitbody2");
    mHandles[98] = MissionUtility::GetHandle("orbitalcannon2");
    mHandles[99] = MissionUtility::GetHandle("orbitalcannon2a");
    mHandles[100] = MissionUtility::GetHandle("orbitalcannon2b");
    mHandles[101] = MissionUtility::GetHandle("orbitalcannon2c");
    mHandles[140] = MissionUtility::GetHandle("platformfighter1");
    mHandles[141] = MissionUtility::GetHandle("platformfighter2");
    mHandles[142] = MissionUtility::GetHandle("platformfighter3");
    mHandles[143] = MissionUtility::GetHandle("platformfighter4");
    mHandles[144] = MissionUtility::GetHandle("platformfighter5");
    mHandles[145] = MissionUtility::GetHandle("platformfighter6");
    mHandles[154] = MissionUtility::GetHandle("platformaat1");
    mHandles[155] = MissionUtility::GetHandle("platformaat2");
    mHandles[2] = MissionUtility::GetHandle("bridgespider1");
    mHandles[3] = MissionUtility::GetHandle("bridgespider2");
    mHandles[4] = MissionUtility::GetHandle("bridgespider3");
    MissionUtility::SetOnRadar(mHandles[140], false);
    MissionUtility::SetOnRadar(mHandles[141], false);
    MissionUtility::SetOnRadar(mHandles[142], false);
    MissionUtility::SetOnRadar(mHandles[143], false);
    MissionUtility::SetOnRadar(mHandles[144], false);
    MissionUtility::SetOnRadar(mHandles[145], false);
    MissionUtility::SetTeamNum(mHandles[140], 2);
    MissionUtility::SetTeamNum(mHandles[141], 2);
    MissionUtility::SetTeamNum(mHandles[142], 2);
    MissionUtility::SetTeamNum(mHandles[143], 2);
    MissionUtility::SetTeamNum(mHandles[144], 2);
    MissionUtility::SetTeamNum(mHandles[145], 2);
    MissionUtility::SetTeamNum(mHandles[2], 2);
    MissionUtility::SetTeamNum(mHandles[3], 2);
    MissionUtility::SetTeamNum(mHandles[4], 2);
    MissionUtility::SetOnRadar(mHandles[154], false);
    MissionUtility::SetOnRadar(mHandles[155], false);
    MissionUtility::SetOnRadar(mHandles[2], false);
    MissionUtility::SetOnRadar(mHandles[3], false);
    MissionUtility::SetOnRadar(mHandles[4], false);
    MissionUtility::Stop(mHandles[140]);
    MissionUtility::Stop(mHandles[141]);
    MissionUtility::Stop(mHandles[142]);
    MissionUtility::Stop(mHandles[143]);
    MissionUtility::Stop(mHandles[144]);
    MissionUtility::Stop(mHandles[145]);
    MissionUtility::Stop(mHandles[2]);
    MissionUtility::Stop(mHandles[3]);
    MissionUtility::Stop(mHandles[4]);
    MissionUtility::SetAttackRange(mHandles[2], 130);
    MissionUtility::SetAttackRange(mHandles[3], 130);
    MissionUtility::SetAttackRange(mHandles[4], 130);
    MissionUtility::Stop(mHandles[154]);
    MissionUtility::Stop(mHandles[155]);
    MissionUtility::SetTeamNum(mHandles[154], 2);
    MissionUtility::SetTeamNum(mHandles[155], 2);
    MissionUtility::Stop(mHandles[189]);
    MissionUtility::Stop(mHandles[190]);
    MissionUtility::SetCurHealth(mHandles[98], 600.0f);
    MissionUtility::SetCurHealth(mHandles[99], 600.0f);
    MissionUtility::SetCurHealth(mHandles[100], 600.0f);
    MissionUtility::SetCurHealth(mHandles[101], 600.0f);
    MissionUtility::SetMaxHealth(mHandles[98], 600.0f);
    MissionUtility::SetMaxHealth(mHandles[99], 600.0f);
    MissionUtility::SetMaxHealth(mHandles[100], 600.0f);
    MissionUtility::SetMaxHealth(mHandles[101], 600.0f);
    MissionUtility::Fire(mHandles[104], true, true, false);
    mFlags[67] = true;

        break;
    case 2:
    // ---- +0x4318  116 bytes ----
    MissionUtility::SetTeamNum(mHandles[13], 0);
    MissionUtility::SetTeamNum(mHandles[9], 0);
    MissionUtility::SetVelocForward(mHandles[13], 0.0f);
    MissionUtility::FlushSoundQueue();
    mHandles[62] = MissionUtility::RunCin("newwheelcin", true, true);
    MissionUtility::QueueSound("lmg03_36", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::PlayMusic("EP5_V2_T07", true);
    mInts2[0] = 3;

        break;
    case 3:
    // ---- +0x438c  112 bytes ----
    if (!mFlags[64]) {
        MissionUtility::SetTeamNum(mHandles[59], 10);
        MissionUtility::OverrideSoundRange(mHandles[57], true);
        MissionUtility::OverrideSoundRange(mHandles[58], true);
        MissionUtility::SetVelocForward(mHandles[57], 200.0f);
        MissionUtility::SetVelocForward(mHandles[58], 200.0f);
        MissionUtility::Goto(mHandles[57], "wheelcinpath3", true);
        MissionUtility::Goto(mHandles[58], "wheelcinpath3", 1);
        mFlags[64] = true;
    }

    // ---- +0x43fc  84 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[62])) {
    MissionUtility::RemoveObject(mHandles[57]);
    MissionUtility::RemoveObject(mHandles[58]);
    MissionUtility::SetTeamNum(mHandles[9], 1);
    MissionUtility::SetTeamNum(mHandles[13], 1);
    if (mFlags[19] = true) {
    MissionUtility::SetVelocForward(mHandles[13], 125.0f);
    }

    // ---- +0x4450  80 bytes ----
    MissionUtility::MoveObjectWithRotation(mHandles[9], "wheelcinmove", 0, true);
    MissionUtility::PlayMusic("EP2_V1_T12_01", true);
    MissionUtility::AddTurnAroundRegion("preplatformturn", "preplatformturnpt1", "preplatformturnpt2", "preplatformturnpt3", 0, 0);
    ResumeTimer(mTimer13);
    mInts2[0] = 17;

        break;
    case 18:
    // ---- +0x44a0  72 bytes ----
    if (!mFlags[65]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "platformobjectify")
            || MissionUtility::IsInsideRegion(mHandles[9], "platformentry")) {
        MissionUtility::RemoveObjectify("platformentry", 0);
        mFlags[65] = true;
        }
    }

    // ---- +0x44e8  1072 bytes ----
    if (!mFlags[66]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "platformobjectify")
            || MissionUtility::IsInsideRegion(mHandles[9], "platformentry")) {
        MissionUtility::RemoveObjectify("platformentry", 0);
        MissionUtility::BeginWave("platform");
        mHandles[146] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(542.6251f, -18.426008f, -1169.9052f), "platformturret1", 0, -1, Quat(-0.41643f, 0.0f, -0.909168f, 0.0f), -1);
        mHandles[147] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(308.52304f, -18.426012f, -1291.4547f), "platformturret2", 0, -1, Quat(0.807979f, 0.0f, 0.589212f, 0.0f), -1);
        mHandles[148] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(143.71259f, -18.426004f, -1292.2195f), "platformturret3", 0, -1, Quat(0.85256f, 0.0f, 0.522629f, 0.0f), -1);
        mHandles[149] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(-171.40508f, 39.573997f, -1031.1848f), "platformturret4", 0, -1, Quat(0.71787f, 0.0f, 0.696177f, 0.0f), -1);
        mHandles[150] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(-180.56216f, 66.57387f, -1280.8348f), "platformturret5", 0, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[151] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(170.67348f, 66.57387f, -1312.0787f), "platformturret6", 0, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[152] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(564.3753f, 124.57303f, -1449.3162f), "platformturret7", 0, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        mHandles[153] = MissionUtility::CreateObject("GEO_bldg_turret", Vector(711.9123f, 124.57303f, -1455.5774f), "platformturret8", 0, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
        MissionUtility::SetOnRadar(mHandles[146], false);
        MissionUtility::SetOnRadar(mHandles[147], false);
        MissionUtility::SetOnRadar(mHandles[148], false);
        MissionUtility::SetOnRadar(mHandles[149], false);
        MissionUtility::SetOnRadar(mHandles[150], false);
        MissionUtility::SetOnRadar(mHandles[151], false);
        MissionUtility::SetOnRadar(mHandles[152], false);
        MissionUtility::SetOnRadar(mHandles[153], false);
        mFlags[66] = true;
        }
    }

    // ---- +0x4918  772 bytes ----
    if (!mFlags[67]) {
        if (MissionUtility::IsWaveSpawned("platform")) {
        mHandles[189] = MissionUtility::GetHandle("gun2walker1");
        mHandles[190] = MissionUtility::GetHandle("gun2walker2");
        mHandles[104] = MissionUtility::GetHandle("orbitbody2");
        mHandles[98] = MissionUtility::GetHandle("orbitalcannon2");
        mHandles[99] = MissionUtility::GetHandle("orbitalcannon2a");
        mHandles[100] = MissionUtility::GetHandle("orbitalcannon2b");
        mHandles[101] = MissionUtility::GetHandle("orbitalcannon2c");
        mHandles[140] = MissionUtility::GetHandle("platformfighter1");
        mHandles[141] = MissionUtility::GetHandle("platformfighter2");
        mHandles[142] = MissionUtility::GetHandle("platformfighter3");
        mHandles[143] = MissionUtility::GetHandle("platformfighter4");
        mHandles[144] = MissionUtility::GetHandle("platformfighter5");
        mHandles[145] = MissionUtility::GetHandle("platformfighter6");
        mHandles[154] = MissionUtility::GetHandle("platformaat1");
        mHandles[155] = MissionUtility::GetHandle("platformaat2");
        mHandles[2] = MissionUtility::GetHandle("bridgespider1");
        mHandles[3] = MissionUtility::GetHandle("bridgespider2");
        mHandles[4] = MissionUtility::GetHandle("bridgespider3");
        MissionUtility::Stop(mHandles[140]);
        MissionUtility::Stop(mHandles[141]);
        MissionUtility::Stop(mHandles[142]);
        MissionUtility::Stop(mHandles[143]);
        MissionUtility::Stop(mHandles[144]);
        MissionUtility::Stop(mHandles[145]);
        MissionUtility::Stop(mHandles[154]);
        MissionUtility::Stop(mHandles[155]);
        MissionUtility::SetTeamNum(mHandles[154], 2);
        MissionUtility::SetTeamNum(mHandles[155], 2);
        MissionUtility::Stop(mHandles[189]);
        MissionUtility::Stop(mHandles[190]);
        MissionUtility::Stop(mHandles[2]);
        MissionUtility::Stop(mHandles[3]);
        MissionUtility::Stop(mHandles[4]);
        MissionUtility::SetAttackRange(mHandles[2], 130);
        MissionUtility::SetAttackRange(mHandles[3], 130);
        MissionUtility::SetAttackRange(mHandles[4], 130);
        MissionUtility::SetTeamNum(mHandles[2], 2);
        MissionUtility::SetTeamNum(mHandles[3], 2);
        MissionUtility::SetTeamNum(mHandles[4], 2);
        MissionUtility::SetCurHealth(mHandles[98], 500.0f);
        MissionUtility::SetCurHealth(mHandles[99], 500.0f);
        MissionUtility::SetCurHealth(mHandles[100], 500.0f);
        MissionUtility::SetCurHealth(mHandles[101], 500.0f);
        MissionUtility::SetMaxHealth(mHandles[98], 500.0f);
        MissionUtility::SetMaxHealth(mHandles[99], 500.0f);
        MissionUtility::SetMaxHealth(mHandles[100], 500.0f);
        MissionUtility::SetMaxHealth(mHandles[101], 500.0f);
        MissionUtility::SetOnRadar(mHandles[140], false);
        MissionUtility::SetOnRadar(mHandles[141], false);
        MissionUtility::SetOnRadar(mHandles[142], false);
        MissionUtility::SetOnRadar(mHandles[143], false);
        MissionUtility::SetOnRadar(mHandles[144], false);
        MissionUtility::SetOnRadar(mHandles[145], false);
        MissionUtility::SetTeamNum(mHandles[140], 2);
        MissionUtility::SetTeamNum(mHandles[141], 2);
        MissionUtility::SetTeamNum(mHandles[142], 2);
        MissionUtility::SetTeamNum(mHandles[143], 2);
        MissionUtility::SetTeamNum(mHandles[144], 2);
        MissionUtility::SetTeamNum(mHandles[145], 2);
        MissionUtility::SetOnRadar(mHandles[154], false);
        MissionUtility::SetOnRadar(mHandles[155], false);
        MissionUtility::SetOnRadar(mHandles[2], false);
        MissionUtility::SetOnRadar(mHandles[3], false);
        MissionUtility::SetOnRadar(mHandles[4], false);
        MissionUtility::Fire(mHandles[104], true, true, false);
        mFlags[67] = true;
        }
    }

    // ---- +0x4c1c  72 bytes ----
    if (!mFlags[68]) {
        if (mTimer9 > 15.0f) {
        MissionUtility::QueueSound("LMG03_34", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[68] = true;
        }
    }

    // ---- +0x4c64  72 bytes ----
    if (!mFlags[69]) {
        if (mTimer9 > 30.0f) {
        MissionUtility::QueueSound("LMG03_34", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[69] = true;
        }
    }

    // ---- +0x4cac  952 bytes ----
    if (!mFlags[29] && mFlags[67]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "platformentry")) {
        MissionUtility::AddTurnAroundRegion("platformbeginturn", "platformbeginturnpt", 0, 0, 0, 0);
        MissionUtility::SetVelocForward(mHandles[13], 200.0f);
        MissionUtility::RemoveHealthBar(mHandles[13]);
        MissionUtility::Goto(mHandles[13], "taoscynreversepath", false);
        StopTimer(mTimer9);
        mTimer9 = 0.0f;
        mFlags[68] = true;
        mFlags[69] = true;
        MissionUtility::SetTeamNum(mHandles[140], 2);
        MissionUtility::SetTeamNum(mHandles[141], 2);
        MissionUtility::SetTeamNum(mHandles[142], 2);
        MissionUtility::SetTeamNum(mHandles[143], 2);
        MissionUtility::SetTeamNum(mHandles[144], 2);
        MissionUtility::SetTeamNum(mHandles[145], 2);
        MissionUtility::SetTeamNum(mHandles[2], 2);
        MissionUtility::SetTeamNum(mHandles[3], 2);
        MissionUtility::SetTeamNum(mHandles[4], 2);
        MissionUtility::SetTeamNum(mHandles[146], 2);
        MissionUtility::SetTeamNum(mHandles[147], 2);
        MissionUtility::SetTeamNum(mHandles[148], 2);
        MissionUtility::SetTeamNum(mHandles[149], 2);
        MissionUtility::SetTeamNum(mHandles[150], 2);
        MissionUtility::SetTeamNum(mHandles[151], 2);
        MissionUtility::SetTeamNum(mHandles[152], 2);
        MissionUtility::SetTeamNum(mHandles[153], 2);
        MissionUtility::SetTeamNum(mHandles[154], 2);
        MissionUtility::SetTeamNum(mHandles[155], 2);
        MissionUtility::SetTeamNum(mHandles[156], 2);
        MissionUtility::SetTeamNum(mHandles[157], 2);
        MissionUtility::SetTeamNum(mHandles[158], 2);
        MissionUtility::SetTeamNum(mHandles[159], 2);
        MissionUtility::SetTeamNum(mHandles[160], 2);
        MissionUtility::SetTeamNum(mHandles[161], 2);
        MissionUtility::SetTeamNum(mHandles[162], 2);
        MissionUtility::SetTeamNum(mHandles[163], 2);
        MissionUtility::SetTeamNum(mHandles[164], 2);
        MissionUtility::SetTeamNum(mHandles[165], 2);
        MissionUtility::SetTeamNum(mHandles[166], 2);
        MissionUtility::SetTeamNum(mHandles[167], 2);
        MissionUtility::SetTeamNum(mHandles[168], 2);
        MissionUtility::SetTeamNum(mHandles[169], 2);
        MissionUtility::SetTeamNum(mHandles[170], 2);
        MissionUtility::SetTeamNum(mHandles[171], 2);
        MissionUtility::SetTeamNum(mHandles[172], 2);
        MissionUtility::SetTeamNum(mHandles[173], 2);
        MissionUtility::SetTeamNum(mHandles[174], 2);
        MissionUtility::SetTeamNum(mHandles[175], 2);
        MissionUtility::SetTeamNum(mHandles[176], 2);
        MissionUtility::SetTeamNum(mHandles[177], 2);
        MissionUtility::SetTeamNum(mHandles[178], 2);
        MissionUtility::SetTeamNum(mHandles[179], 2);
        MissionUtility::SetTeamNum(mHandles[180], 2);
        MissionUtility::SetTeamNum(mHandles[181], 2);
        MissionUtility::SetTeamNum(mHandles[182], 2);
        MissionUtility::SetTeamNum(mHandles[183], 2);
        MissionUtility::SetTeamNum(mHandles[184], 2);
        MissionUtility::SetOnRadar(mHandles[140], true);
        MissionUtility::SetOnRadar(mHandles[141], true);
        MissionUtility::SetOnRadar(mHandles[142], true);
        MissionUtility::SetOnRadar(mHandles[143], true);
        MissionUtility::SetOnRadar(mHandles[144], true);
        MissionUtility::SetOnRadar(mHandles[145], true);
        MissionUtility::SetOnRadar(mHandles[2], true);
        MissionUtility::SetOnRadar(mHandles[3], true);
        MissionUtility::SetOnRadar(mHandles[4], true);
        MissionUtility::SetOnRadar(mHandles[146], true);
        MissionUtility::SetOnRadar(mHandles[147], true);
        MissionUtility::SetOnRadar(mHandles[148], true);
        MissionUtility::SetOnRadar(mHandles[149], true);
        MissionUtility::SetOnRadar(mHandles[150], true);
        MissionUtility::SetOnRadar(mHandles[151], true);
        MissionUtility::SetOnRadar(mHandles[152], true);
        MissionUtility::SetOnRadar(mHandles[153], true);
        MissionUtility::SetOnRadar(mHandles[154], true);
        MissionUtility::SetOnRadar(mHandles[155], true);
        mFlags[29] = true;
        }
    }

    // ---- +0x5064  164 bytes ----
    if (!mFlags[5] && mFlags[67]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "bridgedestroyarea")
            || !MissionUtility::IsAlive(mHandles[2])
            || !MissionUtility::IsAlive(mHandles[3])
            || !MissionUtility::IsAlive(mHandles[4])) {
        MissionUtility::DamageObject(mHandles[1], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[2], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[3], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[4], 999999.0f, 999999.0f);
        mFlags[5] = true;
        }
    }

    // ---- +0x5108  88 bytes ----
    if (!mFlags[30]) {
        if (MissionUtility::GetDistance(mHandles[13], "taoscynreversepath", 6) < 50.0f) {
        MissionUtility::SetTeamNum(mHandles[13], 0);
        MissionUtility::MoveObject(mHandles[13], "taoscynrescuepath", 0, true);
        MissionUtility::Stop(mHandles[13]);
        mFlags[30] = true;
        }
    }

    // ---- +0x5160  80 bytes ----
    if (!mFlags[30]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "taosrescuearea")) {
        MissionUtility::SetTeamNum(mHandles[13], 0);
        MissionUtility::MoveObject(mHandles[13], "taoscynrescuepath", 0, true);
        MissionUtility::Stop(mHandles[13]);
        mFlags[30] = true;
        }
    }

    // ---- +0x51b0  476 bytes ----
    if (!mFlags[31] && mFlags[30]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "taosrescuearea")) {
        MissionUtility::SetTeamNum(mHandles[13], 1);
        MissionUtility::SetVelocForward(mHandles[13], 90.0f);
        MissionUtility::MoveObjectWithRotation(mHandles[13], "taosrescuepath", 1, true);
        MissionUtility::Goto(mHandles[13], "taoscynrescuepath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[13]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::RemoveObject(mHandles[206]);
        MissionUtility::RemoveObject(mHandles[207]);
        MissionUtility::RemoveObject(mHandles[208]);
        MissionUtility::RemoveObject(mHandles[209]);
        MissionUtility::RemoveObject(mHandles[201]);
        MissionUtility::RemoveObject(mHandles[202]);
        MissionUtility::RemoveObject(mHandles[203]);
        MissionUtility::RemoveObject(mHandles[204]);
        MissionUtility::RemoveObject(mHandles[205]);
        MissionUtility::RemoveObject(mHandles[211]);
        MissionUtility::RemoveObject(mHandles[210]);
        MissionUtility::SetFogRange(800.0f, 1550.0f, 4.0f);
        MissionUtility::Fire(mHandles[102], true, true, false);
        MissionUtility::SetTeamNum(mHandles[98], 2);
        MissionUtility::SetTeamNum(mHandles[99], 2);
        MissionUtility::SetTeamNum(mHandles[100], 2);
        MissionUtility::SetTeamNum(mHandles[101], 2);
        MissionUtility::AddHealthBar(mHandles[98], 0, 400.0f);
        MissionUtility::AddHealthBar(mHandles[99], 0, 400.0f);
        MissionUtility::AddHealthBar(mHandles[100], 0, 400.0f);
        MissionUtility::AddHealthBar(mHandles[101], 0, 400.0f);
        MissionUtility::Objectify(mHandles[98], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[99], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[100], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[101], "missions.Geonosis2.marker.str0008", false, true, 0.0f);
        mFlags[13] = true;
        mFlags[15] = true;
        mFlags[31] = true;
        }
    }

    // ---- +0x538c  76 bytes ----
    if (!mFlags[70]) {
        if (MissionUtility::GetDistance(mHandles[9], "platformfighterpath1", 3) < 30.0f) {
        MissionUtility::SetVelocForward(mHandles[140], 80.0f);
        MissionUtility::Goto(mHandles[140], "platformfighterpath1", true);
        mFlags[70] = true;
        }
    }

    // ---- +0x53d8  76 bytes ----
    if (!mFlags[71]) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath3", 13) < 30.0f) {
        MissionUtility::SetVelocForward(mHandles[141], 80.0f);
        MissionUtility::Goto(mHandles[141], "platformfighterpath2", true);
        mFlags[71] = true;
        }
    }

    // ---- +0x5424  100 bytes ----
    if (!mFlags[72]) {
        if (MissionUtility::GetDistance(mHandles[9], "platformfighterpath2", 0) < 30.0f) {
        MissionUtility::SetVelocForward(mHandles[142], 80.0f);
        MissionUtility::Goto(mHandles[142], "platformfighterpath3", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[142]);
        MissionUtility::SetQueueFlag(false);
        mFlags[72] = true;
        }
    }

    // ---- +0x5488  100 bytes ----
    if (!mFlags[73]) {
        if (MissionUtility::GetDistance(mHandles[9], "platformfighterpath3", 0) < 30.0f) {
        MissionUtility::SetVelocForward(mHandles[143], 80.0f);
        MissionUtility::Goto(mHandles[143], "platformfighterpath4", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[143]);
        MissionUtility::SetQueueFlag(false);
        mFlags[72] = true;
        }
    }

    // ---- +0x54ec  100 bytes ----
    if (!mFlags[74]) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath3", 21) < 30.0f) {
        MissionUtility::SetVelocForward(mHandles[144], 80.0f);
        MissionUtility::Goto(mHandles[144], "platformfighterpath5", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[144]);
        MissionUtility::SetQueueFlag(false);
        mFlags[72] = true;
        }
    }

    // ---- +0x5550  100 bytes ----
    if (!mFlags[75]) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath3", 21) < 30.0f) {
        MissionUtility::SetVelocForward(mHandles[145], 80.0f);
        MissionUtility::Goto(mHandles[145], "platformfighterpath6", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[145]);
        MissionUtility::SetQueueFlag(false);
        mFlags[72] = true;
        }
    }

    // ---- +0x55b4  188 bytes ----
    if (!mFlags[62]) {
    if (MissionUtility::IsInsideRegion(mHandles[13], "taosrescuearea")) {
    MissionUtility::Objectify(mHandles[13], "missions.Geonosis2.marker.str0009", true, false, 200.0f);
    MissionUtility::Objectify(mHandles[13], "missions.Geonosis2.marker.str0009", false, true, 0.0f);
    MissionUtility::AddHealthBar(mHandles[13], "", 65.0f);
    MissionUtility::DisplayText("missions.Geonosis2.text.str0016", 5.0f, -1.0f);
    MissionUtility::QueueSound("LMG03_11", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("MWG03_12", 1.0f, 0.0f, 0.0f, "", 0, "");
    mInts2[0] = 19;

        break;
    case 19:
    // ---- +0x5670  140 bytes ----
    if (!mFlags[79]) {
        if (MissionUtility::IsInsideRegion(mHandles[13], "gun2")
            || MissionUtility::IsInsideRegion(mHandles[9], "gun2")) {
        MissionUtility::Goto(mHandles[189], "gun2walker1path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[189]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[190], "gun2walker2path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[190]);
        MissionUtility::SetQueueFlag(false);
        mFlags[79] = true;
        }
    }

    // ---- +0x56fc  112 bytes ----
    if (!mFlags[51]) {
        if (MissionUtility::GetCurHealth(mHandles[98]) + MissionUtility::GetCurHealth(mHandles[99]) + MissionUtility::GetCurHealth(mHandles[100]) + MissionUtility::GetCurHealth(mHandles[101]) < 800.0f) {
        MissionUtility::QueueSound("lmg03_37", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[51] = true;
        }
    }

    // ---- +0x576c  172 bytes ----
    if (!mFlags[82]) {
        if (!MissionUtility::IsAlive(mHandles[98])) {
        if (!MissionUtility::IsAlive(mHandles[99])) {
        if (!MissionUtility::IsAlive(mHandles[100])) {
        if (!MissionUtility::IsAlive(mHandles[101])) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::ObjectiveComplete(mHandles[33]);
        MissionUtility::SetCurHealth(mHandles[104], 99999.0f);
        MissionUtility::DamageObject(mHandles[104], 999999.0f, 999999.0f);
        MissionUtility::Goto(mHandles[191], "attack2biglanding", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[191]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::BeginWave("area3");
        mFlags[82] = true;
        }
        }
        }
        }
    }

    // ---- +0x5818  232 bytes ----
    if (mFlags[82]) {
    if (MissionUtility::IsWaveSpawned("area3")) {
    mHandles[234] = MissionUtility::GetHandle("attack3wheel1");
    mHandles[235] = MissionUtility::GetHandle("attack3wheel2");
    mHandles[226] = MissionUtility::GetHandle("attack3beta3");
    mHandles[227] = MissionUtility::GetHandle("attack3beta4");
    mHandles[228] = MissionUtility::GetHandle("attack3beta5");
    mHandles[229] = MissionUtility::GetHandle("attack3beta6");
    mHandles[230] = MissionUtility::GetHandle("attack3beta7");
    mHandles[231] = MissionUtility::GetHandle("attack3gamma1");
    mHandles[232] = MissionUtility::GetHandle("attack3gamma2");
    mHandles[233] = MissionUtility::GetHandle("attack3gamma3");
    mHandles[216] = MissionUtility::GetHandle("attack3alpha4");
    mHandles[218] = MissionUtility::GetHandle("attack3alpha5");
    mHandles[220] = MissionUtility::GetHandle("attack3alpha6");
    mHandles[221] = MissionUtility::GetHandle("attack3alpha7");
    mHandles[222] = MissionUtility::GetHandle("attack3alpha8");
    mHandles[223] = MissionUtility::GetHandle("attack3alpha9");
    mInts2[0] = 26;

        break;
    case 26:
    // ---- +0x5900  100 bytes ----
    MissionUtility::MidMissionSavePlayer(27);
    mFloatsB[3] = MissionUtility::GetCurHealth(mHandles[13]);
    MissionUtility::MidMissionSave(mFloatsB[3]);
    MissionUtility::MidMissionSave(mTimer13);
    MissionUtility::MidMissionSave((bool)mFlags[78]);
    mInts[6] = MissionUtility::GetPlayerKillCount();
    MissionUtility::MidMissionSave(mInts[6]);
    MissionUtility::MidMissionSave((bool)mFlags[6]);
    MissionUtility::MidMissionSave((bool)mFlags[7]);
    MissionUtility::MidMissionSave((bool)mFlags[8]);
    mInts2[0] = 20;

        break;
    case 27:
    // ---- +0x5964  764 bytes ----
    if (!mFlags[34]) {
        mFlags[24] = true;
        mFloatsB[5] = 0.0f;
        mFloatsB[6] = 0.0f;
        MissionUtility::ObjectiveComplete(mHandles[31]);
        mHandles[33] = MissionUtility::AddObjective("missions.Geonosis2.objective.str0014");
        MissionUtility::ObjectiveComplete(mHandles[33]);
        MissionUtility::MidMissionLoadPlayer();
        MissionUtility::MidMissionLoad(mFloatsB[3]);
        MissionUtility::SetCurHealth(mHandles[13], mFloatsB[3]);
        if (MissionUtility::GetCurHealth(mHandles[13]) < 150.0f) {
        mFlags[26] = true;
        }
        MissionUtility::MidMissionLoad(mLoadTA);
        StopTimer(mTimer13);
        mTimer13 = mLoadTA;
        ResumeTimer(mTimer13);
        MissionUtility::MidMissionLoad(mFlags[78]);
        MissionUtility::MidMissionLoad(mInts[6]);
        MissionUtility::SetPlayerKillCount(mInts[6]);
        MissionUtility::MidMissionLoad(mFlags[6]);
        MissionUtility::MidMissionLoad(mFlags[7]);
        MissionUtility::MidMissionLoad(mFlags[8]);
        MissionUtility::RemoveObject(mHandles[23]);
        MissionUtility::RemoveObject(mHandles[24]);
        MissionUtility::RemoveObject(mHandles[25]);
        MissionUtility::RemoveObject(mHandles[26]);
        MissionUtility::RemoveObject(mHandles[27]);
        MissionUtility::RemoveObject(mHandles[28]);
        MissionUtility::RemoveObject(mHandles[29]);
        MissionUtility::RemoveObject(mHandles[57]);
        MissionUtility::RemoveObject(mHandles[58]);
        MissionUtility::RemoveObject(mHandles[140]);
        MissionUtility::RemoveObject(mHandles[141]);
        MissionUtility::RemoveObject(mHandles[142]);
        MissionUtility::RemoveObject(mHandles[143]);
        MissionUtility::RemoveObject(mHandles[144]);
        MissionUtility::RemoveObject(mHandles[145]);
        MissionUtility::RemoveObject(mHandles[154]);
        MissionUtility::RemoveObject(mHandles[155]);
        MissionUtility::RemoveObject(mHandles[189]);
        MissionUtility::RemoveObject(mHandles[190]);
        MissionUtility::RemoveObject(mHandles[2]);
        MissionUtility::RemoveObject(mHandles[3]);
        MissionUtility::RemoveObject(mHandles[4]);
        MissionUtility::DamageObject(mHandles[103], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[92], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[93], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[94], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[95], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[96], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[97], 999999.0f, 999999.0f);
        MissionUtility::RemoveObject(mHandles[112]);
        MissionUtility::RemoveObject(mHandles[23]);
        MissionUtility::DamageObject(mHandles[104], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[98], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[99], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[100], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[101], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[1], 999999.0f, 999999.0f);
        MissionUtility::MoveObjectWithRotation(mHandles[9], "playerloadpoint", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[13], "taosloadpoint", 0, true);
        MissionUtility::BeginWave("area3");
        MissionUtility::Fire(mHandles[102], true, true, false);
        MissionUtility::Objectify(mHandles[13], "missions.Geonosis2.marker.str0009", true, false, 200.0f);
        MissionUtility::Objectify(mHandles[13], "missions.Geonosis2.marker.str0009", false, true, 0.0f);
        MissionUtility::AddHealthBar(mHandles[13], "", 65.0f);
        MissionUtility::AddTurnAroundRegion("platformbeginturn", "platformbeginturnpt", 0, 0, 0, 0);
        mFlags[34] = true;
    }

    // ---- +0x5c60  232 bytes ----
    if (mFlags[34]) {
    if (MissionUtility::IsWaveSpawned("area3")) {
    mHandles[234] = MissionUtility::GetHandle("attack3wheel1");
    mHandles[235] = MissionUtility::GetHandle("attack3wheel2");
    mHandles[226] = MissionUtility::GetHandle("attack3beta3");
    mHandles[227] = MissionUtility::GetHandle("attack3beta4");
    mHandles[228] = MissionUtility::GetHandle("attack3beta5");
    mHandles[229] = MissionUtility::GetHandle("attack3beta6");
    mHandles[230] = MissionUtility::GetHandle("attack3beta7");
    mHandles[231] = MissionUtility::GetHandle("attack3gamma1");
    mHandles[232] = MissionUtility::GetHandle("attack3gamma2");
    mHandles[233] = MissionUtility::GetHandle("attack3gamma3");
    mHandles[216] = MissionUtility::GetHandle("attack3alpha4");
    mHandles[218] = MissionUtility::GetHandle("attack3alpha5");
    mHandles[220] = MissionUtility::GetHandle("attack3alpha6");
    mHandles[221] = MissionUtility::GetHandle("attack3alpha7");
    mHandles[222] = MissionUtility::GetHandle("attack3alpha8");
    mHandles[223] = MissionUtility::GetHandle("attack3alpha9");
    mInts2[0] = 20;

        break;
    case 20:
    // ---- +0x5d48  884 bytes ----
    if (!mFlags[85]) {
        MissionUtility::PlayMusic("EP6_V2_T05_02", true);
        MissionUtility::QueueSound("MWG03_13", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("LMG03_14", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveTurnAroundRegion("platformdelete");
        mHandles[35] = MissionUtility::AddObjective("missions.Geonosis2.objective.str0020");
        MissionUtility::DisplayText("missions.Geonosis2.text.str0017", 5.0f, -1.0f);
        MissionUtility::AttackTarget(mHandles[13], mHandles[189], true, true, false, false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[13], mHandles[190], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[13], "taosgotopath", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[13], "taoscynpath4", 300.0f, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[13], "taospatrolpoint", 300.0f, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocForward(mHandles[13], 125.0f);
        mFlags[19] = true;
        mFlags[21] = false;
        mFlags[22] = false;
        mFlags[20] = false;
        StopTimer(mTimer14);
        mTimer14 = 0.0f;
        mHandles[236] = MissionUtility::CreateFlock(mHandles[235], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[224]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[225]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[226]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[227]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[228]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[229]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[230]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[231]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[232]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[233]);
        MissionUtility::AddFlockMember(mHandles[236], mHandles[234]);
        MissionUtility::SetAttackRange(mHandles[212], 650);
        MissionUtility::SetAttackRange(mHandles[213], 650);
        MissionUtility::SetAttackRange(mHandles[215], 650);
        MissionUtility::SetAttackRange(mHandles[216], 400);
        MissionUtility::SetAttackRange(mHandles[218], 400);
        MissionUtility::SetAttackRange(mHandles[220], 450);
        MissionUtility::SetAttackRange(mHandles[221], 400);
        MissionUtility::SetAttackRange(mHandles[222], 450);
        MissionUtility::SetAttackRange(mHandles[223], 450);
        MissionUtility::SetAttackRange(mHandles[224], 600);
        MissionUtility::SetAttackRange(mHandles[225], 600);
        MissionUtility::SetAttackRange(mHandles[226], 450);
        MissionUtility::SetAttackRange(mHandles[227], 400);
        MissionUtility::SetAttackRange(mHandles[228], 400);
        MissionUtility::SetAttackRange(mHandles[229], 400);
        MissionUtility::SetAttackRange(mHandles[230], 400);
        mHandles[214] = MissionUtility::CreateFlock(mHandles[213], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[214], mHandles[212]);
        MissionUtility::AddFlockMember(mHandles[214], mHandles[215]);
        MissionUtility::SetFlockSeparation(mHandles[214], 0.5f);
        mHandles[217] = MissionUtility::CreateFlock(mHandles[216], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[217], mHandles[220]);
        MissionUtility::AddFlockMember(mHandles[217], mHandles[223]);
        MissionUtility::SetFlockSeparation(mHandles[217], 0.5f);
        mHandles[219] = MissionUtility::CreateFlock(mHandles[218], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[219], mHandles[221]);
        MissionUtility::AddFlockMember(mHandles[219], mHandles[222]);
        MissionUtility::SetFlockSeparation(mHandles[219], 0.5f);
        MissionUtility::SetFogRange(600.0f, 1000.0f, 4.0f);
        mFlags[24] = false;
        mFlags[85] = true;
    }

    // ---- +0x60bc  124 bytes ----
    if (!mFlags[20]) {
        if (MissionUtility::IsAlive(mHandles[9])) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[13]) > 400.0f) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath4", 12) > MissionUtility::GetDistance(mHandles[13], "taoscynpath4", 12)) {
        MissionUtility::SetVelocForward(mHandles[13], 0.0f);
        mFlags[19] = false;
        mFlags[20] = true;
        }
        }
        }
    }

    // ---- +0x6138  164 bytes ----
    if (!mFlags[19]) {
        if (MissionUtility::IsAlive(mHandles[9])) {
        if (!mFlags[21]) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[13]) < 400.0f) {
        if (MissionUtility::GetDistance(mHandles[9], "taoscynpath4", 12) > MissionUtility::GetDistance(mHandles[13], "taoscynpath4", 12)) {
        MissionUtility::SetVelocForward(mHandles[13], 125.0f);
        StopTimer(mTimer2);
        mTimer2 = 0.0f;
        mFlags[21] = false;
        mFlags[22] = false;
        mFlags[19] = true;
        mFlags[20] = false;
        }
        }
        }
        }
    }

    // ---- +0x61dc  156 bytes ----
    if (!mFlags[76]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "platformdeletetrigger")) {
        MissionUtility::RemoveObject(mHandles[105]);
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon2"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon2a"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon2b"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitalcannon2c"));
        MissionUtility::RemoveObject(MissionUtility::GetHandle("orbitbody2"));
        MissionUtility::EvictConfig("GEO_bldg_platform");
        MissionUtility::EvictConfig("GEO_bldg_orbitgun");
        MissionUtility::QueueSound("LMG03_38", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[76] = true;
        }
    }

    // ---- +0x6278  76 bytes ----
    if (!mFlags[77]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "platformdeletetrigger")) {
        MissionUtility::AddTurnAroundRegion("platformdelete", "platformdeletept", 0, 0, 0, 0);
        MissionUtility::FadeInDust(5.0f);
        mFlags[77] = true;
        }
    }

    // ---- +0x62c4  300 bytes ----
    if (!mFlags[83]) {
        if (MissionUtility::IsWaveSpawned("jedi")) {
        mHandles[12] = MissionUtility::GetHandle("playerjedi1");
        mHandles[240] = MissionUtility::GetHandle("stopdroid1");
        mHandles[241] = MissionUtility::GetHandle("stopdroid2");
        mHandles[242] = MissionUtility::GetHandle("stopdroid3");
        mHandles[243] = MissionUtility::GetHandle("stopdroid4");
        mHandles[244] = MissionUtility::GetHandle("stopdroid5");
        mHandles[300] = MissionUtility::GetHandle("unstablecrate");
        mHandles[239] = MissionUtility::GetHandle("walkercincrate");
        mHandles[247] = MissionUtility::GetHandle("smallwalker1");
        mHandles[248] = MissionUtility::GetHandle("smallwalker2");
        mHandles[249] = MissionUtility::GetHandle("smallwalker3");
        mHandles[250] = MissionUtility::GetHandle("smallwalker4");
        mHandles[290] = MissionUtility::GetHandle("geo36");
        mHandles[291] = MissionUtility::GetHandle("geo37");
        mHandles[292] = MissionUtility::GetHandle("geo38");
        mHandles[293] = MissionUtility::GetHandle("geo39");
        mHandles[294] = MissionUtility::GetHandle("geo40");
        mHandles[295] = MissionUtility::GetHandle("geo41");
        mHandles[296] = MissionUtility::GetHandle("geo42");
        mHandles[297] = MissionUtility::GetHandle("geo43");
        mHandles[298] = MissionUtility::GetHandle("geo44");
        mHandles[299] = MissionUtility::GetHandle("geo45");
        mFlags[83] = true;
        }
    }

    // ---- +0x63f0  208 bytes ----
    if (!mFlags[86] && mFlags[85]) {
        if (!MissionUtility::IsFlockAlive(mHandles[236])) {
        MissionUtility::QueueSound("LMG02_03", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("LMG02_35", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::BeginWave("jedi");
        MissionUtility::SetTeamNum(mHandles[9], 0);
        MissionUtility::SetTeamNum(mHandles[13], 0);
        mFlags[19] = true;
        mFlags[20] = true;
        MissionUtility::SetVelocForward(mHandles[13], 125.0f);
        MissionUtility::SetAttackRange(mHandles[13], 1);
        MissionUtility::Follow(mHandles[13], mHandles[9]);
        BeginTimer(mTimer11);
        MissionUtility::RemoveTurnAroundRegion("turnaround8");
        mFlags[86] = true;
        }
    }

    // ---- +0x64c0  72 bytes ----
    if (!mFlags[88]) {
        if (MissionUtility::GetDistance(mHandles[9], mHandles[13]) < 75.0f) {
        if (mFlags[86]) {
        MissionUtility::Goto(mHandles[13], "taoslastpath", false);
        mFlags[88] = true;
        }
        }
    }

    // ---- +0x6508  64 bytes ----
    if (!mFlags[87]) {
    if (mFlags[86]) {
    if (MissionUtility::IsInsideRegion(mHandles[9], "yardentry")
        || MissionUtility::IsInsideRegion(mHandles[13], "yardentry")) {

    // ---- +0x6548  44 bytes ----
    StopTimer(mTimer13);
    mFlags[24] = true;
    MissionUtility::ObjectiveComplete(mHandles[35]);
    mInts2[0] = 28;
    mFlags[87] = true;

        break;
    case 28:
    // ---- +0x6574  72 bytes ----
    MissionUtility::MidMissionSavePlayer(29);
    MissionUtility::MidMissionSave(mTimer13);
    mInts[6] = MissionUtility::GetPlayerKillCount();
    MissionUtility::MidMissionSave(mInts[6]);
    MissionUtility::MidMissionSave((bool)mFlags[6]);
    MissionUtility::MidMissionSave((bool)mFlags[7]);
    MissionUtility::MidMissionSave((bool)mFlags[8]);
    mInts2[0] = 6;

        break;
    case 29:
    // ---- +0x65bc  484 bytes ----
    if (!mFlags[4]) {
    mFlags[24] = true;
    mFloatsB[5] = 0.0f;
    mFloatsB[6] = 0.0f;
    MissionUtility::ObjectiveComplete(mHandles[31]);
    mHandles[33] = MissionUtility::AddObjective("missions.Geonosis2.objective.str0014");
    MissionUtility::ObjectiveComplete(mHandles[33]);
    mHandles[35] = MissionUtility::AddObjective("missions.Geonosis2.objective.str0020");
    MissionUtility::ObjectiveComplete(mHandles[35]);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mLoadTB);
    StopTimer(mTimer13);
    mTimer13 = mLoadTB;
    ResumeTimer(mTimer13);
    MissionUtility::MidMissionLoad(mInts[6]);
    MissionUtility::SetPlayerKillCount(mInts[6]);
    MissionUtility::MidMissionLoad(mFlags[6]);
    MissionUtility::MidMissionLoad(mFlags[7]);
    MissionUtility::MidMissionLoad(mFlags[8]);
    MissionUtility::RemoveObject(mHandles[23]);
    MissionUtility::RemoveObject(mHandles[24]);
    MissionUtility::RemoveObject(mHandles[25]);
    MissionUtility::RemoveObject(mHandles[26]);
    MissionUtility::RemoveObject(mHandles[27]);
    MissionUtility::RemoveObject(mHandles[28]);
    MissionUtility::RemoveObject(mHandles[29]);
    MissionUtility::RemoveObject(mHandles[57]);
    MissionUtility::RemoveObject(mHandles[58]);
    MissionUtility::RemoveObject(mHandles[105]);
    MissionUtility::RemoveObject(mHandles[140]);
    MissionUtility::RemoveObject(mHandles[141]);
    MissionUtility::RemoveObject(mHandles[142]);
    MissionUtility::RemoveObject(mHandles[143]);
    MissionUtility::RemoveObject(mHandles[144]);
    MissionUtility::RemoveObject(mHandles[145]);
    MissionUtility::RemoveObject(mHandles[154]);
    MissionUtility::RemoveObject(mHandles[155]);
    MissionUtility::RemoveObject(mHandles[189]);
    MissionUtility::RemoveObject(mHandles[190]);
    MissionUtility::RemoveObject(mHandles[2]);
    MissionUtility::RemoveObject(mHandles[3]);
    MissionUtility::RemoveObject(mHandles[4]);
    MissionUtility::RemoveObject(mHandles[103]);
    MissionUtility::RemoveObject(mHandles[92]);
    MissionUtility::RemoveObject(mHandles[93]);
    MissionUtility::RemoveObject(mHandles[94]);
    MissionUtility::RemoveObject(mHandles[95]);
    MissionUtility::RemoveObject(mHandles[96]);
    MissionUtility::RemoveObject(mHandles[97]);
    MissionUtility::RemoveObject(mHandles[112]);
    MissionUtility::RemoveObject(mHandles[23]);
    MissionUtility::RemoveObject(mHandles[104]);
    MissionUtility::RemoveObject(mHandles[98]);
    MissionUtility::RemoveObject(mHandles[99]);
    MissionUtility::RemoveObject(mHandles[100]);
    MissionUtility::RemoveObject(mHandles[101]);
    MissionUtility::RemoveObject(mHandles[1]);
    mInts2[0] = 6;
    mFlags[4] = true;

        break;
    case 21:
    // ---- +0x67a0  92 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[53])) {
    mFlags[24] = true;
    MissionUtility::SetMapZoom(85.0f, 999999.0f);
    ResumeTimer(mTimer13);
    MissionUtility::FlushSoundQueue();
    MissionUtility::RemoveHealthBar(mHandles[49]);
    MissionUtility::PlayMusic("EP4_V1_T03_02", true);
    if (MissionUtility::IsAlive(mHandles[56])) {
    MissionUtility::RemoveObject(mHandles[56]);
    }

    // ---- +0x67fc  24 bytes ----
    if (MissionUtility::IsAlive(mHandles[55])) {
    MissionUtility::RemoveObject(mHandles[55]);
    }

    // ---- +0x6814  24 bytes ----
    if (MissionUtility::IsAlive(mHandles[54])) {
    MissionUtility::RemoveObject(mHandles[54]);
    }

    // ---- +0x682c  324 bytes ----
    MissionUtility::DisplayText("missions.Geonosis2.text.str0011", 7.0f, -1.0f);
    MissionUtility::SetAsPlayer(mHandles[12], 0);
    MissionUtility::SetTeamNum(mHandles[12], 1);
    MissionUtility::SetFogRange(200.0f, 300.0f, 4.0f);
    MissionUtility::RemoveObject(mHandles[10]);
    MissionUtility::RemoveObject(mHandles[13]);
    MissionUtility::Stop(mHandles[300]);
    MissionUtility::Stop(mHandles[247]);
    MissionUtility::Stop(mHandles[248]);
    MissionUtility::Stop(mHandles[249]);
    MissionUtility::Stop(mHandles[250]);
    MissionUtility::Stop(mHandles[240]);
    MissionUtility::Stop(mHandles[241]);
    MissionUtility::Stop(mHandles[242]);
    MissionUtility::Stop(mHandles[243]);
    MissionUtility::Stop(mHandles[244]);
    MissionUtility::Stop(mHandles[245]);
    MissionUtility::Stop(mHandles[246]);
    mHandles[36] = MissionUtility::AddObjective("missions.Geonosis2.objective.str0021");
    MissionUtility::DisplayText("missions.Geonosis2.text.str0023", 5.0f, -1.0f);
    MissionUtility::EvictConfig("CIS_tank_fighter");
    MissionUtility::EvictConfig("CIS_tank_wheeled");
    MissionUtility::EvictConfig("GEO_bldg_turret");
    MissionUtility::SetFOV(64.3f);
    mHandles[251] = MissionUtility::StartSound("klaxon1", true, 0.88f, 0.0f, 0.0f, "", 0, "");
    mFloatsB[0] = 10.0f + MissionUtility::GetTime();
    if (mFloatsB[0] < MissionUtility::GetTime()) {
    MissionUtility::StopSound(mHandles[251]);
    mFloatsB[0] = 999999.9f + MissionUtility::GetTime();
    }

    // ---- +0x6970  12 bytes ----
    mInts2[0] = 22;

        break;
    case 4:
    // ---- +0x697c  96 bytes ----
    MissionUtility::SetTeamNum(mHandles[12], 0);
    MissionUtility::SetTeamNum(mHandles[9], 0);
    mHandles[237] = MissionUtility::RunCin("walkercin", true, true);
    mHandles[238] = MissionUtility::StartSound("lmg03_43", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mInts2[0] = 5;

        break;
    case 5:
    // ---- +0x69dc  96 bytes ----
    if (!mFlags[84]) {
        MissionUtility::Stop(mHandles[240]);
        MissionUtility::Stop(mHandles[241]);
        MissionUtility::Stop(mHandles[242]);
        MissionUtility::SetVelocForward(mHandles[300], 3.0f);
        MissionUtility::Goto(mHandles[300], "unstablecratepath", 1);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[300]);
        MissionUtility::SetQueueFlag(false);
        mFlags[84] = true;
    }

    // ---- +0x6a3c  96 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[237])) {
    MissionUtility::SetTeamNum(mHandles[9], 1);
    MissionUtility::SetTeamNum(mHandles[300], 2);
    MissionUtility::SetTeamNum(mHandles[240], 2);
    MissionUtility::SetTeamNum(mHandles[241], 2);
    MissionUtility::SetTeamNum(mHandles[242], 2);
    ResumeTimer(mTimer13);
    mInts2[0] = 22;

        break;
    case 22:
    // ---- +0x6a9c  40 bytes ----
    if (mFloatsB[0] < MissionUtility::GetTime()) {
    MissionUtility::StopSound(mHandles[251]);
    mFloatsB[0] = 999999.9f + MissionUtility::GetTime();
    }

    // ---- +0x6ac4  536 bytes ----
    if (!mFlags[89]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "explodecrate")) {
        if (MissionUtility::IsAlive(mHandles[300])) {
        mHandles[253] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath2", 0, "geo1", 2, -1);
        mHandles[254] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath2", 1, "geo2", 2, -1);
        mHandles[255] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath2", 2, "geo3", 2, -1);
        mHandles[256] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath2", 3, "geo4", 2, -1);
        mHandles[257] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath2", 4, "geo5", 2, -1);
        MissionUtility::Stop(mHandles[253]);
        MissionUtility::Stop(mHandles[254]);
        MissionUtility::Stop(mHandles[255]);
        MissionUtility::Stop(mHandles[256]);
        MissionUtility::Stop(mHandles[257]);
        MissionUtility::SetTeamNum(mHandles[12], 0);
        MissionUtility::SetTeamNum(mHandles[9], 0);
        StopTimer(mTimer13);
        mInts2[0] = 4;
        mFlags[89] = true;
        }
        }
    }

    // ---- +0x6cdc  180 bytes ----
    if (!mFlags[93]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "yardarea1")) {
        MissionUtility::AttackTarget(mHandles[253], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[254], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[255], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[256], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[257], mHandles[9], true, true, false, false);
        mFlags[93] = true;
        }
    }

    // ---- +0x6d90  440 bytes ----
    if (!mFlags[95]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "geospawnarea1")) {
        mHandles[265] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath3", 0, "geo13", 2, -1);
        mHandles[266] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath3", 1, "geo14", 2, -1);
        mHandles[267] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath3", 2, "geo15", 2, -1);
        mHandles[268] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath3", 3, "geo16", 2, -1);
        mHandles[269] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath3", 4, "geo17", 2, -1);
        mFlags[95] = true;
        }
    }

    // ---- +0x6f48  280 bytes ----
    if (!mFlags[96]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "geospawnarea2")) {
        mHandles[270] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath4", 0, "geo18", 2, -1);
        mHandles[271] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath4", 1, "geo19", 2, -1);
        mHandles[272] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath4", 2, "geo20", 2, -1);
        mFlags[96] = true;
        }
    }

    // ---- +0x7060  468 bytes ----
    if (!mFlags[97] && mFlags[84]) {
        if (!MissionUtility::IsCinRunning(mHandles[237])) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "explodecrate")) {
        mHandles[273] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath5", 0, "geo21", 2, -1);
        mHandles[274] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath5", 1, "geo22", 2, -1);
        mHandles[275] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath5", 2, "geo23", 2, -1);
        mHandles[276] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath5", 3, "geo24", 2, -1);
        mHandles[277] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath5", 4, "geo25", 2, -1);
        mFlags[97] = true;
        }
        }
    }

    // ---- +0x7234  632 bytes ----
    if (!mFlags[112]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "yardarea2")) {
        MissionUtility::SetCurHealth(mHandles[106], 10.0f);
        MissionUtility::SetCurHealth(mHandles[107], 10.0f);
        MissionUtility::SetCurHealth(mHandles[108], 10.0f);
        MissionUtility::SetCurHealth(mHandles[109], 10.0f);
        MissionUtility::SetCurHealth(mHandles[110], 10.0f);
        MissionUtility::SetCurHealth(mHandles[111], 10.0f);
        MissionUtility::Objectify(mHandles[106], "missions.Geonosis2.marker.str0007", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[107], "missions.Geonosis2.marker.str0009", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[108], "missions.Geonosis2.marker.str0009", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[109], "missions.Geonosis2.marker.str0009", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[110], "missions.Geonosis2.marker.str0009", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[111], "missions.Geonosis2.marker.str0009", false, true, 0.0f);
        MissionUtility::QueueSound("lmg03_41", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::AttackTarget(mHandles[290], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[291], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[292], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[293], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[294], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[295], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[296], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[297], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[298], mHandles[9], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[299], mHandles[9], true, true, false, false);
        MissionUtility::Wait(mHandles[247]);
        MissionUtility::Wait(mHandles[248]);
        MissionUtility::Wait(mHandles[249]);
        MissionUtility::SetAttackRange(mHandles[247], 40);
        MissionUtility::SetAttackRange(mHandles[248], 40);
        MissionUtility::SetAttackRange(mHandles[249], 40);
        mFlags[111] = true;
        mFlags[112] = true;
        }
    }

    // ---- +0x74ac  1072 bytes ----
    if (mFlags[111]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "geodroparea")) {
        if (!MissionUtility::IsFlockAlive(mHandles[283])) {
        mHandles[278] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath8", 0, "geo26", 2, -1, Quat(), 0);
        mHandles[279] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath8", 1, "geo27", 2, -1, Quat(), 0);
        mHandles[280] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath8", 2, "geo28", 2, -1, Quat(), 0);
        mHandles[281] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath8", 3, "geo29", 2, -1, Quat(), 0);
        mHandles[282] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath8", 4, "geo30", 2, -1, Quat(), 0);
        mHandles[252] = mHandles[252] + 5;
        mHandles[283] = MissionUtility::CreateFlock(mHandles[278], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[283], mHandles[279]);
        MissionUtility::AddFlockMember(mHandles[283], mHandles[280]);
        MissionUtility::AddFlockMember(mHandles[283], mHandles[281]);
        MissionUtility::AddFlockMember(mHandles[283], mHandles[282]);
        MissionUtility::AttackTarget(mHandles[283], mHandles[9], true, true, false, false);
        }
        if (!MissionUtility::IsFlockAlive(mHandles[289])) {
        mHandles[284] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath9", 0, "geo31", 2, -1, Quat(), 0);
        mHandles[285] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath9", 1, "geo32", 2, -1, Quat(), 0);
        mHandles[286] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath9", 2, "geo33", 2, -1, Quat(), 0);
        mHandles[287] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath9", 3, "geo34", 2, -1, Quat(), 0);
        mHandles[288] = MissionUtility::CreateObject("GEO_inf_geonosian", "geospawnpath9", 4, "geo35", 2, -1, Quat(), 0);
        mHandles[252] = mHandles[252] + 5;
        mHandles[289] = MissionUtility::CreateFlock(mHandles[284], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[289], mHandles[285]);
        MissionUtility::AddFlockMember(mHandles[289], mHandles[286]);
        MissionUtility::AddFlockMember(mHandles[289], mHandles[287]);
        MissionUtility::AddFlockMember(mHandles[289], mHandles[288]);
        MissionUtility::AttackTarget(mHandles[289], mHandles[9], true, true, false, false);
        }
        }
    }

    // ---- +0x78dc  152 bytes ----
    if (!mFlags[90]) {
        if (!MissionUtility::IsAlive(mHandles[106])) {
        if (!MissionUtility::IsAlive(mHandles[107])) {
        if (!MissionUtility::IsAlive(mHandles[108])) {
        if (!MissionUtility::IsAlive(mHandles[109])) {
        if (!MissionUtility::IsAlive(mHandles[110])) {
        if (!MissionUtility::IsAlive(mHandles[111])) {
        MissionUtility::SetTeamNum(mHandles[9], 0);
        BeginTimer(mTimer12);
        MissionUtility::ObjectiveComplete(mHandles[35]);
        MissionUtility::ObjectiveComplete(mHandles[36]);
        mFlags[90] = true;
        }
        }
        }
        }
        }
        }
    }

    // ---- +0x7974  68 bytes ----
    if (mTimer12 > 3.0f) {
    StopTimer(mTimer13);
    if (MissionUtility::GetGameClock() < 540.0f) {
    MissionUtility::BonusObjectiveComplete(mHandles[38], true);
    } else {
    MissionUtility::BonusObjectiveFailed(mHandles[38]);
    }

    // ---- +0x79b8  20 bytes ----
    if (MissionUtility::GetPlayerKillCount() < 100) {
    MissionUtility::BonusObjectiveFailed(mHandles[39]);
    }

    // ---- +0x79cc  20 bytes ----
    if (!mFlags[9]) {
        MissionUtility::BonusObjectiveFailed(mHandles[40]);
    }

    // ---- +0x79e0  8 bytes ----
    mInts2[0] = 8;

        break;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
    }
}

// `inline` because the shipped symbol is WEAK (`__ct__4QuatFffff`, 0x801a5ed0, 20 B) -- the
// binding CodeWarrior gives an inline function it had to emit out of line, not the GLOBAL a
// namespace-scope definition would get. It still has to be written AFTER `Execute` so the call
// sites above are calls; see the file header. CodeWarrior does not encode the binding in
// .text, so `verify_missions.py` cannot see this either way -- but MSVC needs the `inline` to
// merge this COMDAT with Multi17Script.cpp's class-body definition instead of LNK2005.
inline Quat::Quat(float a, float b, float c, float d)
{
    s = a;
    x = b;
    y = c;
    z = d;
}

void Geonosis2Script::Setup()
{
        mFlags[3] = true;
        mFlags[122] = false;
        mFlags[19] = true;
        mFlags[20] = false;
        mFlags[21] = false;
        mFlags[29] = false;
        mFlags[30] = false;
        mFlags[31] = false;
        mFlags[37] = false;
        mFlags[38] = false;
        mFlags[39] = false;
        mFlags[40] = false;
        mFlags[41] = false;
        mFlags[42] = false;
        mFlags[43] = false;
        mFlags[44] = false;
        mFlags[33] = false;
        mFlags[48] = false;
        mFlags[49] = false;
        mFlags[47] = false;
        mFlags[52] = false;
        mFlags[53] = false;
        mFlags[54] = false;
        mFlags[56] = false;
        mFlags[57] = false;
        mFlags[58] = false;
        mFlags[59] = false;
        mFlags[60] = false;
        mFlags[61] = false;
        mFlags[62] = false;
        mFlags[70] = false;
        mFlags[71] = false;
        mFlags[72] = false;
        mFlags[73] = false;
        mFlags[74] = false;
        mFlags[75] = false;
        mFlags[79] = false;
        mFlags[80] = false;
        mFlags[86] = false;
        mFlags[89] = false;
        mFlags[90] = false;
        mFlags[91] = false;
        mFlags[92] = false;
        mFlags[98] = false;
        mFlags[99] = false;
        mFlags[100] = false;
        mFlags[101] = false;
        mFlags[103] = false;
        mFlags[104] = false;
        mFlags[105] = false;
        mFlags[106] = false;
        mFlags[107] = false;
        mFlags[108] = false;
        mFlags[109] = false;
        mFlags[110] = false;
        mFlags[115] = false;
        mFloatsB[2] = 1.0f;
        mHandles[0] = MissionUtility::GetHandle("testplayer");
        mHandles[1] = MissionUtility::GetHandle("platformbridge");
        mHandles[2] = MissionUtility::GetHandle("bridgespider1");
        mHandles[3] = MissionUtility::GetHandle("bridgespider2");
        mHandles[4] = MissionUtility::GetHandle("bridgespider3");
        mHandles[6] = MissionUtility::GetHandle("r41");
        mHandles[7] = MissionUtility::GetHandle("r42");
        mHandles[8] = MissionUtility::GetHandle("r43");
        mHandles[5] = MissionUtility::GetHandle("fopencinlandingship");
        mHandles[11] = MissionUtility::GetHandle("playerjedi");
        mHandles[10] = MissionUtility::GetHandle("playertank");
        mHandles[13] = MissionUtility::GetHandle("taoscyn");
        mHandles[23] = MissionUtility::GetHandle("loadturret1");
        mHandles[24] = MissionUtility::GetHandle("loadturret2");
        mHandles[25] = MissionUtility::GetHandle("loadturret3");
        mHandles[26] = MissionUtility::GetHandle("loadturret4");
        mHandles[27] = MissionUtility::GetHandle("loadturret5");
        mHandles[28] = MissionUtility::GetHandle("loadturret6");
        mHandles[29] = MissionUtility::GetHandle("loadturret7");
        mHandles[41] = MissionUtility::GetHandle("opencingunship1");
        mHandles[42] = MissionUtility::GetHandle("opencingunship2");
        mHandles[43] = MissionUtility::GetHandle("opencingunship3");
        mHandles[44] = MissionUtility::GetHandle("opencingunship4");
        mHandles[57] = MissionUtility::GetHandle("cinwheel1");
        mHandles[58] = MissionUtility::GetHandle("cinwheel2");
        mHandles[59] = MissionUtility::GetHandle("cinjunk");
        mHandles[49] = MissionUtility::GetHandle("luminara");
        mHandles[112] = MissionUtility::GetHandle("firstturret");
        mHandles[92] = MissionUtility::GetHandle("orbitalcannon1");
        mHandles[93] = MissionUtility::GetHandle("orbitalcannon1a");
        mHandles[94] = MissionUtility::GetHandle("orbitalcannon1b");
        mHandles[95] = MissionUtility::GetHandle("orbitalcannon1c");
        mHandles[96] = MissionUtility::GetHandle("orbitalcannon1d");
        mHandles[97] = MissionUtility::GetHandle("orbitalcannon1e");
        mHandles[98] = MissionUtility::GetHandle("orbitalcannon2");
        mHandles[99] = MissionUtility::GetHandle("orbitalcannon2a");
        mHandles[100] = MissionUtility::GetHandle("orbitalcannon2b");
        mHandles[101] = MissionUtility::GetHandle("orbitalcannon2c");
        mHandles[102] = MissionUtility::GetHandle("orbitalcannon3");
        mHandles[103] = MissionUtility::GetHandle("orbitbody1");
        mHandles[104] = MissionUtility::GetHandle("orbitbody2");
        mHandles[106] = MissionUtility::GetHandle("powersupply");
        mHandles[107] = MissionUtility::GetHandle("powersupplya");
        mHandles[108] = MissionUtility::GetHandle("powersupplyb");
        mHandles[109] = MissionUtility::GetHandle("powersupplyc");
        mHandles[110] = MissionUtility::GetHandle("powersupplyd");
        mHandles[111] = MissionUtility::GetHandle("powersupplye");
        mHandles[105] = MissionUtility::GetHandle("platform");
        mHandles[140] = MissionUtility::GetHandle("platformfighter1");
        mHandles[141] = MissionUtility::GetHandle("platformfighter2");
        mHandles[142] = MissionUtility::GetHandle("platformfighter3");
        mHandles[143] = MissionUtility::GetHandle("platformfighter4");
        mHandles[144] = MissionUtility::GetHandle("platformfighter5");
        mHandles[145] = MissionUtility::GetHandle("platformfighter6");
        mHandles[146] = MissionUtility::GetHandle("platformturret1");
        mHandles[147] = MissionUtility::GetHandle("platformturret2");
        mHandles[148] = MissionUtility::GetHandle("platformturret3");
        mHandles[149] = MissionUtility::GetHandle("platformturret4");
        mHandles[150] = MissionUtility::GetHandle("platformturret5");
        mHandles[151] = MissionUtility::GetHandle("platformturret6");
        mHandles[152] = MissionUtility::GetHandle("platformturret7");
        mHandles[153] = MissionUtility::GetHandle("platformturret8");
        mHandles[154] = MissionUtility::GetHandle("platformaat1");
        mHandles[155] = MissionUtility::GetHandle("platformaat2");
        mHandles[156] = MissionUtility::GetHandle("sb1");
        mHandles[157] = MissionUtility::GetHandle("sb2");
        mHandles[158] = MissionUtility::GetHandle("sb3");
        mHandles[159] = MissionUtility::GetHandle("sb4");
        mHandles[160] = MissionUtility::GetHandle("sb5");
        mHandles[161] = MissionUtility::GetHandle("sb6");
        mHandles[162] = MissionUtility::GetHandle("sb7");
        mHandles[163] = MissionUtility::GetHandle("sb8");
        mHandles[164] = MissionUtility::GetHandle("sb9");
        mHandles[165] = MissionUtility::GetHandle("sb11");
        mHandles[166] = MissionUtility::GetHandle("sb12");
        mHandles[167] = MissionUtility::GetHandle("sb13");
        mHandles[168] = MissionUtility::GetHandle("sb14");
        mHandles[169] = MissionUtility::GetHandle("sb15");
        mHandles[170] = MissionUtility::GetHandle("sb16");
        mHandles[171] = MissionUtility::GetHandle("sb17");
        mHandles[172] = MissionUtility::GetHandle("sb18");
        mHandles[173] = MissionUtility::GetHandle("sb19");
        mHandles[174] = MissionUtility::GetHandle("sb10");
        mHandles[175] = MissionUtility::GetHandle("sb20");
        mHandles[176] = MissionUtility::GetHandle("sb21");
        mHandles[177] = MissionUtility::GetHandle("sb22");
        mHandles[178] = MissionUtility::GetHandle("sb23");
        mHandles[179] = MissionUtility::GetHandle("sb24");
        mHandles[180] = MissionUtility::GetHandle("sb25");
        mHandles[181] = MissionUtility::GetHandle("sb26");
        mHandles[182] = MissionUtility::GetHandle("sb27");
        mHandles[183] = MissionUtility::GetHandle("sb28");
        mHandles[184] = MissionUtility::GetHandle("sb29");
        mHandles[189] = MissionUtility::GetHandle("gun2walker1");
        mHandles[190] = MissionUtility::GetHandle("gun2walker2");
        mHandles[191] = MissionUtility::GetHandle("attack2biglanding");
        mHandles[192] = MissionUtility::GetHandle("attack2biglanding2");
        mHandles[185] = MissionUtility::GetHandle("attack2defender1");
        mHandles[186] = MissionUtility::GetHandle("attack2defender2");
        mHandles[187] = MissionUtility::GetHandle("attack2defender3");
        mHandles[212] = MissionUtility::GetHandle("attack3alpha1");
        mHandles[213] = MissionUtility::GetHandle("attack3alpha2");
        mHandles[215] = MissionUtility::GetHandle("attack3alpha3");
        mHandles[224] = MissionUtility::GetHandle("attack3beta1");
        mHandles[225] = MissionUtility::GetHandle("attack3beta2");
        mHandles[234] = MissionUtility::GetHandle("attack3wheel1");
        mHandles[235] = MissionUtility::GetHandle("attack3wheel2");
        mHandles[226] = MissionUtility::GetHandle("attack3beta3");
        mHandles[227] = MissionUtility::GetHandle("attack3beta4");
        mHandles[228] = MissionUtility::GetHandle("attack3beta5");
        mHandles[229] = MissionUtility::GetHandle("attack3beta6");
        mHandles[230] = MissionUtility::GetHandle("attack3beta7");
        mHandles[231] = MissionUtility::GetHandle("attack3gamma1");
        mHandles[232] = MissionUtility::GetHandle("attack3gamma2");
        mHandles[233] = MissionUtility::GetHandle("attack3gamma3");
        mHandles[216] = MissionUtility::GetHandle("attack3alpha4");
        mHandles[218] = MissionUtility::GetHandle("attack3alpha5");
        mHandles[220] = MissionUtility::GetHandle("attack3alpha6");
        mHandles[221] = MissionUtility::GetHandle("attack3alpha7");
        mHandles[222] = MissionUtility::GetHandle("attack3alpha8");
        mHandles[223] = MissionUtility::GetHandle("attack3alpha9");
        mHandles[245] = MissionUtility::GetHandle("stopdroid6");
        mHandles[246] = MissionUtility::GetHandle("stopdroid7");
        mHandles[253] = MissionUtility::GetHandle("geo1");
        mHandles[254] = MissionUtility::GetHandle("geo2");
        mHandles[255] = MissionUtility::GetHandle("geo3");
        mHandles[256] = MissionUtility::GetHandle("geo4");
        mHandles[257] = MissionUtility::GetHandle("geo5");
        mHandles[258] = MissionUtility::GetHandle("geo6");
        mHandles[259] = MissionUtility::GetHandle("geo7");
        mHandles[260] = MissionUtility::GetHandle("geo8");
        mHandles[261] = MissionUtility::GetHandle("geo9");
        mHandles[262] = MissionUtility::GetHandle("geo10");
        mHandles[263] = MissionUtility::GetHandle("geo11");
        mHandles[264] = MissionUtility::GetHandle("geo12");
        mHandles[301] = MissionUtility::GetHandle("explodecrate3");
        mHandles[302] = MissionUtility::GetHandle("explodecrate4");
        mHandles[303] = MissionUtility::GetHandle("explodecrate5");
        mHandles[304] = MissionUtility::GetHandle("explodefighter1");
        mHandles[305] = MissionUtility::GetHandle("explodefighter2");
        mHandles[306] = MissionUtility::GetHandle("rundroid1");
        mHandles[307] = MissionUtility::GetHandle("rundroid2");
        mHandles[308] = MissionUtility::GetHandle("rundroid3");
        mHandles[309] = MissionUtility::GetHandle("rundroid4");
        mHandles[310] = MissionUtility::GetHandle("rundroid5");
        mHandles[311] = MissionUtility::GetHandle("rundroid6");
        mHandles[312] = MissionUtility::GetHandle("rundroid7");
        mHandles[313] = MissionUtility::GetHandle("rundroid8");
        mHandles[314] = MissionUtility::GetHandle("rundroid9");
        mHandles[315] = MissionUtility::GetHandle("rundroid10");
        mHandles[316] = MissionUtility::GetHandle("hidefighter1");
        mHandles[317] = MissionUtility::GetHandle("hidefighter2");
        mHandles[318] = MissionUtility::GetHandle("explodeaat1");
        mHandles[319] = MissionUtility::GetHandle("explodeaat2");
        mHandles[320] = MissionUtility::GetHandle("explodeaat3");
        mHandles[321] = MissionUtility::GetHandle("explodeaat4");
        mHandles[322] = MissionUtility::GetHandle("explodeaat5");
        mHandles[323] = MissionUtility::GetHandle("explodeaat6");
        mHandles[324] = MissionUtility::GetHandle("explodeaat7");
        mHandles[325] = MissionUtility::GetHandle("explodeaat8");
        mHandles[326] = MissionUtility::GetHandle("destroyaatcrate");
        mHandles[327] = MissionUtility::GetHandle("a1");
        mHandles[328] = MissionUtility::GetHandle("a2");
        mHandles[329] = MissionUtility::GetHandle("a3");
        mHandles[330] = MissionUtility::GetHandle("b1");
        mHandles[331] = MissionUtility::GetHandle("b2");
        mHandles[332] = MissionUtility::GetHandle("b3");
        mHandles[333] = MissionUtility::GetHandle("c1");
        mHandles[334] = MissionUtility::GetHandle("c2");
        mHandles[335] = MissionUtility::GetHandle("c3");
        mHandles[336] = MissionUtility::GetHandle("d1");
        mHandles[337] = MissionUtility::GetHandle("d2");
        mHandles[338] = MissionUtility::GetHandle("d3");
        mHandles[339] = MissionUtility::GetHandle("e1");
        mHandles[340] = MissionUtility::GetHandle("e2");
        mHandles[341] = MissionUtility::GetHandle("e3");
        mHandles[342] = MissionUtility::GetHandle("f1");
        mHandles[343] = MissionUtility::GetHandle("f2");
        mHandles[344] = MissionUtility::GetHandle("f3");
        mHandles[345] = MissionUtility::GetHandle("g1");
        mHandles[346] = MissionUtility::GetHandle("g2");
        mHandles[347] = MissionUtility::GetHandle("g3");
        mHandles[348] = MissionUtility::GetHandle("h1");
        mHandles[349] = MissionUtility::GetHandle("h2");
        mHandles[350] = MissionUtility::GetHandle("h3");
        mHandles[351] = MissionUtility::GetHandle("j1");
        mHandles[352] = MissionUtility::GetHandle("j2");
        mHandles[353] = MissionUtility::GetHandle("j3");
        mHandles[354] = MissionUtility::GetHandle("k1");
        mHandles[355] = MissionUtility::GetHandle("k2");
        mHandles[356] = MissionUtility::GetHandle("k3");
        mHandles[371] = MissionUtility::GetHandle("OpenCinTurret");
        mHandles[372] = MissionUtility::GetHandle("OpenCinTurret1");
        mHandles[373] = MissionUtility::GetHandle("OpenCinTurret2");
        mHandles[374] = MissionUtility::GetHandle("OpenCinTurret3");
        MissionUtility::PreloadConfig("REP_blaster_fighter1_ord_luminara");
        MissionUtility::PreloadConfig("NEU_prop_fullhealth_dummy");
        MissionUtility::PreloadConfig("NEU_prop_ammo_dummy");
        MissionUtility::PreloadConfig("REP_fly_fighter");
        MissionUtility::PreloadConfig("REP_fly_fighter_cin");
        MissionUtility::PreloadConfig("GEO_bldg_orbitgun2_dest");
        MissionUtility::PreloadConfig("GEO_bldg_powgen_dest");
        MissionUtility::PreloadConfig("REP_fly_vcarrier");
        MissionUtility::PreloadConfig("REP_inf_luminara");
        MissionUtility::PreloadConfig("hugeexp");
        mInts2[1] = 0;
}

SPMission *Geonosis2BuildMission()
{
    return new Geonosis2Script();
}
