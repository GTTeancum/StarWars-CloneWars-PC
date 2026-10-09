// Ossus3Script.cpp -- reconstruction of a shipped mission script.  BYTE-EXACT, all 4 functions.
//
// Head from tools/gen_mission_head.py, bodies from tools/gen_block.py, assembled by
// tools/gen_mission_tu.py.  Eight things in `Execute` were not generated -- see
// analysis/mission_batch_a.md.  The two that are easiest to miss:
//   * `case 19:` is empty and invisible in the jump table, but the shipped range check is
//     `cmplwi r0, 0x13` and only an explicit label reproduces it;
//   * `mFlags[28] & !IsSoundPlaying(...)` is a non-branching `&`, which the reach scan
//     cannot see -- it came out as a plain `if (mFlags[28])` with the call discarded.

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
    int    AddFlockMember(int, int, Vector);
    int    AddFlyerArmy(char, const char*, int, int, float);
    int    AddHealthBar(int, const char*, float);
    int    AddObjective(const char*);
    int    AddPitchRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    int    AddPropArmy(char, const char*, int, int, float);
    void   AddTurnAroundRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    void   AttackTarget(int, int, bool, bool, bool, bool);
    void   BeginWave(const char*);
    void   BonusObjectiveComplete(int, bool);
    void   BonusObjectiveFailed(int);
    void   CarrierAddCargo(int, const char*, int, const char*, bool);
    void   CarrierDropoff(int, const char*, int, float);
    int    CreateFlock();
    int    CreateFlock(int, Formation);
    int    CreateObject(const char*, const Vector&, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObject(const char*, const char*, int, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    int    CreateRegionList(const char*, bool, bool);
    void   DamageObject(int, float, float);
    void   DestroyRegionList(int);
    void   DisbandFlockMember(int, int);
    void   DisplayText(const char*, float, float);
    void   EvictConfig(const char*);
    void   ExcludeObject(int, int);
    void   FlushSoundQueue();
    int    GetCinId(int);
    float  GetCurHealth(int);
    float  GetDistance(int, int);
    int    GetFlockCount(int);
    float  GetGameClock();
    int    GetHandle(const char*);
    int    GetPlayerHandle(int);
    int    GetRandomInt(int, int);
    int    GetRegionNewMember(int, int);
    int    GetRegionNewMemberCount(int);
    int    GetWhoShotMe(int);
    void   Goto(int, const char*, bool);
    void   Goto(int, const char*, int);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsDropped(int);
    bool   IsFlockAlive(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsQueuedSoundPlaying(int);
    bool   IsSoundPlaying(int);
    bool   IsWaveSpawned(const char*);
    void   Land(int, const char*, int, float);
    int    MidMissionGetSavePoint();
    void   MidMissionLoad(bool&);
    void   MidMissionLoad(float&);
    void   MidMissionLoadPlayer();
    void   MidMissionSave(bool);
    void   MidMissionSave(float);
    void   MidMissionSavePlayer(int);
    void   MissionFailure();
    void   MissionSuccess();
    void   MoveObjectWithRotation(int, const char*, int, bool);
    void   Objectify(const char*, int, const char*, bool, bool, float, float);
    void   Objectify(int, const char*, bool, bool, float);
    void   ObjectiveComplete(int);
    void   OverrideSoundRange(int, bool);
    void   Patrol(int, const char*, float, bool);
    void   PlayMusic(const char*, bool);
    void   PreloadConfig(const char*);
    int    QueueSound(const char*, float, float, float, const char*, int, const char*);
    void   RemoveObject(int);
    void   RemoveObjectify(const char*, int);
    void   RemoveTurnAroundRegion(const char*);
    int    RunCin(const char*, bool, bool);
    void   SetAlliance(int, int);
    void   SetAllianceOneWay(int, int);
    void   SetAltitude(int, float);
    void   SetAnimation(int, const char*, float, int);
    void   SetApplyDynamics(int, bool);
    void   SetAsPlayer(int, int);
    void   SetAttackRange(int, int);
    void   SetCollidable(int, bool);
    void   SetCurHealth(int, float);
    void   SetCurShield(int, float);
    void   SetEnemies(int, int);
    void   SetEnemiesOneWay(int, int);
    void   SetFOV(float);
    void   SetFiringRange(int, int);
    void   SetFogRange(float, float, float);
    void   SetMapZoom(float, float);
    void   SetMaxAltitude(int, float);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetNeutralOneWay(int, int);
    void   SetOriginalFog(float);
    void   SetQueueFlag(bool);
    void   SetSky(const char*);
    void   SetTeamNum(int, int);
    void   SetVelocForward(int, float);
    void   SetVelocMaximumFly(int, float);
    void   SetVelocMinimumFly(int, float);
    void   SetVelocNeutralFly(int, float);
    void   StartAmbiences(const char*, const char*, float, float);
    int    StartSound(const char*, bool, float, float, float, const char*, int, const char*);
    void   StartSoundAtObject(const char*, int, bool, float);
    void   Stop(int);
    void   StopAmbiences();
    void   StopAnimation(int);
    void   TakeOff(int);
}

static const char *const kClassName = "Ossus3Script";
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

class Ossus3Script : public SPMission
{
public:
    virtual ~Ossus3Script();

    Ossus3Script()
    {
        mBoolCount  = 147;           mBools = mFlags;
        mCountB     = 0;             mIntsB = mIntsB_;
        mCountC     = 321;           mIntsC = mHandles;
        mCountD     = 5;             mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    char   mPad24[1];
    bool   mFlags[147];            // +0x025  the one-shot latches
    char   mPadB8[8];                  // +0x0b8
    int    mIntsB_[1];            // +0x0c0
    char   mPadC4[4];                  // +0x0c4
    int    mHandles[321];          // +0x0c8
    char   mPad5CC[8];                  // +0x5cc
    int    mInts[5];             // +0x5d4
    char   mPad5E8[4];                  // +0x5e8
    Timer  mTimer0;                      // +0x5ec
    Timer  mTimer1;                      // +0x5f8
    Timer  mTimer2;                      // +0x604
    Timer  mTimer3;                      // +0x610
    Timer  mTimer4;                      // +0x61c
    Timer  mTimer5;                      // +0x628
    Timer  mTimer6;                      // +0x634
    Timer  mTimer7;                      // +0x640
    Timer  mTimer8;                      // +0x64c
    Timer  mTimer9;                      // +0x658
    Timer  mTimer10;                      // +0x664
    Timer  mTimer11;                      // +0x670
    Timer  mTimer12;                      // +0x67c
    Timer  mTimer13;                      // +0x688
    Timer  mTimer14;                      // +0x694
    Timer  mTimer15;                      // +0x6a0
    Timer  mTimer16;                      // +0x6ac
    Timer  mTimer17;                      // +0x6b8
    Timer  mTimer18;                      // +0x6c4
    Timer  mTimer19;                      // +0x6d0
    Timer  mTimer20;                      // +0x6dc
    Timer  mTimer21;                      // +0x6e8
    Timer  mTimer22;                      // +0x6f4
    Timer  mTimer23;                      // +0x700
    Timer  mTimer24;                      // +0x70c
    Timer  mTimer25;                      // +0x718
    Timer  mTimer26;                      // +0x724
    Timer  mTimer27;                      // +0x730
    int    mPhase;                      // +0x73c  -> sizeof == 0x740
};

Ossus3Script::~Ossus3Script()
{
}

void Ossus3Script::Execute()
{
    // ---- +0x0008  1968 bytes ----
    mHandles[4] = MissionUtility::GetPlayerHandle(0);
    if (mFlags[8]) {
    MissionUtility::SetMusicLooping(true);
    MissionUtility::Stop(mHandles[303]);
    MissionUtility::Stop(mHandles[304]);
    MissionUtility::Stop(mHandles[305]);
    MissionUtility::Stop(mHandles[306]);
    MissionUtility::Stop(mHandles[307]);
    MissionUtility::Stop(mHandles[308]);
    MissionUtility::Stop(mHandles[309]);
    MissionUtility::Stop(mHandles[310]);
    MissionUtility::Stop(mHandles[311]);
    MissionUtility::Stop(mHandles[312]);
    MissionUtility::SetVelocMaximumFly(mHandles[303], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[304], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[305], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[306], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[307], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[308], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[309], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[310], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[311], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[312], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[303], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[304], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[305], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[306], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[307], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[308], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[309], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[310], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[311], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[312], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[303], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[304], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[305], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[306], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[307], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[308], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[309], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[310], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[311], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[312], 0.0f);
    MissionUtility::SetAlliance(1, 0);
    MissionUtility::SetAlliance(2, 0);
    MissionUtility::SetEnemies(1, 2);
    MissionUtility::SetAlliance(3, 0);
    MissionUtility::SetAlliance(4, 0);
    MissionUtility::SetEnemies(3, 4);
    MissionUtility::SetAlliance(3, 1);
    MissionUtility::SetAlliance(3, 2);
    MissionUtility::SetEnemiesOneWay(1, 4);
    MissionUtility::SetAllianceOneWay(4, 1);
    MissionUtility::SetAlliance(2, 4);
    MissionUtility::SetEnemies(5, 6);
    MissionUtility::SetEnemies(9, 6);
    MissionUtility::SetAlliance(1, 6);
    MissionUtility::SetAlliance(2, 6);
    MissionUtility::SetAlliance(3, 6);
    MissionUtility::SetAlliance(4, 6);
    MissionUtility::SetAlliance(1, 5);
    MissionUtility::SetEnemiesOneWay(2, 5);
    MissionUtility::SetNeutralOneWay(5, 2);
    MissionUtility::SetAlliance(3, 5);
    MissionUtility::SetAlliance(4, 5);
    MissionUtility::SetAlliance(1, 9);
    MissionUtility::SetAlliance(2, 9);
    MissionUtility::SetAlliance(3, 9);
    MissionUtility::SetAlliance(4, 9);
    MissionUtility::SetAlliance(5, 9);
    MissionUtility::SetEnemies(1, 7);
    MissionUtility::SetEnemies(2, 7);
    MissionUtility::SetAlliance(3, 7);
    MissionUtility::SetAlliance(4, 7);
    MissionUtility::SetAlliance(5, 7);
    MissionUtility::SetAlliance(6, 7);
    MissionUtility::SetEnemies(1, 8);
    MissionUtility::SetAlliance(2, 8);
    MissionUtility::SetAlliance(3, 8);
    MissionUtility::SetAlliance(4, 8);
    MissionUtility::SetAlliance(5, 8);
    MissionUtility::SetAlliance(6, 8);
    MissionUtility::SetAlliance(7, 8);
    MissionUtility::SetEnemies(10, 11);
    MissionUtility::SetAlliance(2, 10);
    MissionUtility::SetAlliance(2, 11);
    MissionUtility::SetEnemies(1, 10);
    MissionUtility::SetAlliance(1, 11);
    MissionUtility::Land(mHandles[58], 0, 0, 80.0f);
    MissionUtility::Land(mHandles[59], 0, 0, 80.0f);
    MissionUtility::SetCurHealth(mHandles[12], 1000.0f);
    MissionUtility::SetCurHealth(mHandles[13], 1000.0f);
    MissionUtility::SetCurHealth(mHandles[14], 1000.0f);
    mHandles[15] = MissionUtility::CreateFlock(mHandles[14], (Formation)8);
    MissionUtility::AddFlockMember(mHandles[15], mHandles[13], Vector(-75.0f, 0.0f, 150.0f));
    MissionUtility::AddFlockMember(mHandles[15], mHandles[12], Vector(0.0f, 0.0f, 330.0f));
    MissionUtility::Goto(mHandles[15], "asixlegpath", true);
    MissionUtility::SetVelocForward(mHandles[15], 12.0f);
    mHandles[7] = MissionUtility::AddBonusObjective("missions.Ossus3.bonus.str0003");
    mHandles[8] = MissionUtility::AddBonusObjective("missions.Ossus3.bonus.str0007");
    mHandles[9] = MissionUtility::AddBonusObjective("missions.Ossus3.bonus.str0006");
    mTimer16 = 0.0f;
    MissionUtility::SetCurHealth(mHandles[122], 100.0f);
    MissionUtility::SetMaxHealth(mHandles[122], 100.0f);
    MissionUtility::SetCurHealth(mHandles[123], 100.0f);
    MissionUtility::SetMaxHealth(mHandles[123], 100.0f);
    MissionUtility::SetCurHealth(mHandles[124], 100.0f);
    MissionUtility::SetMaxHealth(mHandles[124], 100.0f);
    MissionUtility::SetCurHealth(mHandles[125], 20.0f);
    MissionUtility::SetMaxHealth(mHandles[125], 20.0f);
    MissionUtility::SetCurHealth(mHandles[126], 10.0f);
    MissionUtility::SetMaxHealth(mHandles[126], 10.0f);
    MissionUtility::SetCurHealth(mHandles[127], 100.0f);
    MissionUtility::SetMaxHealth(mHandles[127], 100.0f);
    mTimer17 = 0.0f;
    mTimer18 = 0.0f;
    MissionUtility::AddTurnAroundRegion("beginturnaround", "beginturnaroundpoint", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("outpostturnaround", "outpostturnaroundpoint", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("basetrigger", "basetriggerpoint", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("turretturnaround", "turretturnaroundpoint", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("holeturnaround", "holeturnaroundpoint", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("transportturnaround", "transportturnpoint", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("buildingturn", "buildingturnpt", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("backleftwallturn", "backleftwallturnpt", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("backrightwallturn", "backrightwallturnpt", 0, 0, 0, 0);
    MissionUtility::AddPitchRegion("outpostpitchturn", 0, 0, 0, 0, 0);
    MissionUtility::AddPitchRegion("transportpitchturn", 0, 0, 0, 0, 0);
    MissionUtility::AddPitchRegion("commandpitchturn", 0, 0, 0, 0, 0);
    MissionUtility::AddPitchRegion("convoypitchturn", 0, 0, 0, 0, 0);
    MissionUtility::AddPitchRegion("templepitchturn", 0, 0, 0, 0, 0);
    mInts[2] = 10;
    mPhase = MissionUtility::MidMissionGetSavePoint();
    MissionUtility::StartAmbiences("AmbRhen_blizzard_pl2", "AmbRhen_stinger_blizzard01", 10.0f, 30.0f);
    mHandles[47] = MissionUtility::AddObjective("missions.Ossus3.objective.str0010");
    MissionUtility::SetMapZoom(2000.0f, 999999.0f);
    mTimer25 = 0.0f;
    mFlags[3] = true;
    mFlags[8] = false;
    }

    // ---- +0x07b8  44 bytes ----
    if (!mFlags[5]) {
        if (!MissionUtility::IsAlive(mHandles[4])) {
        BeginTimer(mTimer2);
        mFlags[5] = true;
        }
    }

    // ---- +0x07e4  44 bytes ----
    if (!mFlags[6]) {
        if (mTimer2 > 4.0f) {
        MissionUtility::MissionFailure();
        mFlags[6] = true;
        }
    }

    // ---- +0x0810  48 bytes ----
    if (!mFlags[4]) {
        if (MissionUtility::GetGameClock() >= 720.0f) {
        MissionUtility::BonusObjectiveFailed(mHandles[8]);
        mFlags[4] = true;
        }
    }

    switch (mPhase) {
    default:
    // ---- +0x0850  24 bytes ----

        break;
    case 0:
    // ---- +0x0868  380 bytes ----
    if (mFlags[9]) {
        if (!MissionUtility::IsCinRunning(mHandles[10])) {
        mFlags[142] = true;
        MissionUtility::MoveObjectWithRotation(mHandles[6], "moveplayer1", 0, true);
        MissionUtility::SetVelocMinimumFly(mHandles[6], 70.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[6], 170.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[6], 220.0f);
        MissionUtility::RemoveObject(mHandles[278]);
        MissionUtility::SetTeamNum(mHandles[6], 1);
        MissionUtility::SetAsPlayer(mHandles[6], 0);
        MissionUtility::FlushSoundQueue();
        MissionUtility::RemoveObject(mHandles[279]);
        MissionUtility::RemoveObject(mHandles[280]);
        MissionUtility::RemoveObject(mHandles[281]);
        MissionUtility::RemoveObject(mHandles[282]);
        MissionUtility::RemoveObject(mHandles[283]);
        MissionUtility::RemoveObject(mHandles[284]);
        MissionUtility::RemoveObject(mHandles[285]);
        MissionUtility::RemoveObject(mHandles[286]);
        MissionUtility::RemoveObject(mHandles[287]);
        MissionUtility::RemoveObject(mHandles[288]);
        MissionUtility::RemoveObject(mHandles[289]);
        MissionUtility::RemoveObject(mHandles[290]);
        MissionUtility::RemoveObject(mHandles[291]);
        MissionUtility::RemoveObject(mHandles[292]);
        MissionUtility::RemoveObject(mHandles[293]);
        MissionUtility::RemoveObject(mHandles[294]);
        MissionUtility::RemoveObject(mHandles[295]);
        MissionUtility::RemoveObject(mHandles[296]);
        MissionUtility::RemoveObject(mHandles[297]);
        MissionUtility::RemoveObject(mHandles[298]);
        MissionUtility::RemoveObject(mHandles[299]);
        MissionUtility::RemoveObject(mHandles[300]);
        MissionUtility::RemoveObject(mHandles[301]);
        MissionUtility::RemoveObject(mHandles[302]);
        MissionUtility::SetTeamNum(mHandles[12], 1);
        MissionUtility::SetTeamNum(mHandles[13], 1);
        MissionUtility::SetTeamNum(mHandles[14], 1);
        BeginTimer(mTimer16);
        MissionUtility::RemoveObject(mHandles[29]);
        mPhase = 1;
        }
    }

    // ---- +0x09e4  400 bytes ----
    if (!mFlags[9]) {
        MissionUtility::SetTeamNum(mHandles[4], 0);
        MissionUtility::SetAsPlayer(mHandles[5], 0);
        mHandles[300] = MissionUtility::CreateObjectWithRotation("cis_fly_technounion", "OpenCinTechnoPath", 0, "OpenCinTechno", 0, -1, -1);
        mHandles[301] = MissionUtility::CreateObjectWithRotation("cis_fly_technounion", "OpenCinTechnoPath1", 0, "OpenCinTechno1", 0, -1, -1);
        mHandles[302] = MissionUtility::CreateObjectWithRotation("cis_fly_technounion", "OpenCinTechnoPath2", 0, "OpenCinTechno2", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[300], true);
        MissionUtility::OverrideSoundRange(mHandles[301], true);
        MissionUtility::OverrideSoundRange(mHandles[302], true);
        MissionUtility::SetApplyDynamics(mHandles[300], true);
        MissionUtility::Land(mHandles[300], 0, 0, 80.0f);
        MissionUtility::SetApplyDynamics(mHandles[301], true);
        MissionUtility::Land(mHandles[301], 0, 0, 80.0f);
        MissionUtility::SetApplyDynamics(mHandles[302], true);
        MissionUtility::Land(mHandles[302], 0, 0, 80.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[278], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[278], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[278], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[6], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[6], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[6], 0.0f);
        MissionUtility::PlayMusic("EP1_V2_T22", true);
        mHandles[10] = MissionUtility::RunCin("opencin", true, true);
        mFlags[9] = true;
        mFlags[140] = true;
        BeginTimer(mTimer23);
    }

    // ---- +0x0b74  84 bytes ----
    if (!mFlags[142]) {
    if (mFlags[140]) {
    if (mTimer23 > 2.0f) {
    if (MissionUtility::IsCinRunning(mHandles[10])) {
    mInts[4] = mInts[4] + 1;
    mTimer23 = 0.0f;

    switch (mInts[4]) {
    case 1:
    // ---- +0x0bec  196 bytes ----
    mHandles[279] = MissionUtility::CreateObjectWithRotation("CIS_tank_assault", "OpenCinAATPath", 0, "OpenCinEnemy1", 0, -1, -1);
    mHandles[280] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath", 0, "OpenCinEnemy2", 0, -1, -1);
    mHandles[281] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath1", 0, "OpenCinEnemy3", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[279], 50.0f);
    MissionUtility::SetVelocForward(mHandles[280], 60.0f);
    MissionUtility::SetVelocForward(mHandles[281], 55.0f);
    MissionUtility::Goto(mHandles[279], "OpenCinAATPath", false);
    MissionUtility::Goto(mHandles[280], "OpenCinWheelPath", false);
    MissionUtility::Goto(mHandles[281], "OpenCinWheelPath1", false);

        break;
    case 2:
    // ---- +0x0cb0  196 bytes ----
    mHandles[282] = MissionUtility::CreateObjectWithRotation("CIS_tank_assault", "OpenCinAATPath", 0, "OpenCinEnemy1a", 0, -1, -1);
    mHandles[283] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath", 0, "OpenCinEnemy2a", 0, -1, -1);
    mHandles[284] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath1", 0, "OpenCinEnemy3a", 0, -1, -1);
    MissionUtility::Goto(mHandles[282], "OpenCinAATPath", false);
    MissionUtility::Goto(mHandles[283], "OpenCinWheelPath", false);
    MissionUtility::Goto(mHandles[284], "OpenCinWheelPath1", false);
    MissionUtility::SetVelocForward(mHandles[282], 50.0f);
    MissionUtility::SetVelocForward(mHandles[283], 60.0f);
    MissionUtility::SetVelocForward(mHandles[284], 55.0f);

        break;
    case 3:
    // ---- +0x0d74  196 bytes ----
    mHandles[285] = MissionUtility::CreateObjectWithRotation("CIS_tank_assault", "OpenCinAATPath", 0, "OpenCinEnemy1b", 0, -1, -1);
    mHandles[286] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath", 0, "OpenCinEnemy2b", 0, -1, -1);
    mHandles[287] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath1", 0, "OpenCinEnemy3b", 0, -1, -1);
    MissionUtility::Goto(mHandles[285], "OpenCinAATPath", false);
    MissionUtility::Goto(mHandles[286], "OpenCinWheelPath", false);
    MissionUtility::Goto(mHandles[287], "OpenCinWheelPath1", false);
    MissionUtility::SetVelocForward(mHandles[285], 50.0f);
    MissionUtility::SetVelocForward(mHandles[286], 60.0f);
    MissionUtility::SetVelocForward(mHandles[287], 55.0f);

        break;
    case 4:
    // ---- +0x0e38  196 bytes ----
    mHandles[288] = MissionUtility::CreateObjectWithRotation("CIS_tank_assault", "OpenCinAATPath", 0, "OpenCinEnemy1c", 0, -1, -1);
    mHandles[289] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath", 0, "OpenCinEnemy2c", 0, -1, -1);
    mHandles[290] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath1", 0, "OpenCinEnemy3c", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[288], 50.0f);
    MissionUtility::SetVelocForward(mHandles[289], 60.0f);
    MissionUtility::SetVelocForward(mHandles[290], 55.0f);
    MissionUtility::Goto(mHandles[288], "OpenCinAATPath", false);
    MissionUtility::Goto(mHandles[289], "OpenCinWheelPath", false);
    MissionUtility::Goto(mHandles[290], "OpenCinWheelPath1", false);

        break;
    case 5:
    // ---- +0x0efc  196 bytes ----
    mHandles[291] = MissionUtility::CreateObjectWithRotation("CIS_tank_assault", "OpenCinAATPath", 0, "OpenCinEnemy1d", 0, -1, -1);
    mHandles[292] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath", 0, "OpenCinEnemy2d", 0, -1, -1);
    mHandles[293] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath1", 0, "OpenCinEnemy3d", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[291], 50.0f);
    MissionUtility::SetVelocForward(mHandles[292], 60.0f);
    MissionUtility::SetVelocForward(mHandles[293], 55.0f);
    MissionUtility::Goto(mHandles[291], "OpenCinAATPath", false);
    MissionUtility::Goto(mHandles[292], "OpenCinWheelPath", false);
    MissionUtility::Goto(mHandles[293], "OpenCinWheelPath1", false);

        break;
    case 6:
    // ---- +0x0fc0  196 bytes ----
    mHandles[294] = MissionUtility::CreateObjectWithRotation("CIS_tank_assault", "OpenCinAATPath", 0, "OpenCinEnemy1e", 0, -1, -1);
    mHandles[295] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath", 0, "OpenCinEnemy2e", 0, -1, -1);
    mHandles[296] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath1", 0, "OpenCinEnemy3e", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[294], 50.0f);
    MissionUtility::SetVelocForward(mHandles[295], 60.0f);
    MissionUtility::SetVelocForward(mHandles[296], 55.0f);
    MissionUtility::Goto(mHandles[294], "OpenCinAATPath", false);
    MissionUtility::Goto(mHandles[295], "OpenCinWheelPath", false);
    MissionUtility::Goto(mHandles[296], "OpenCinWheelPath1", false);

        break;
    case 7:
    // ---- +0x1084  192 bytes ----
    mHandles[297] = MissionUtility::CreateObjectWithRotation("CIS_tank_assault", "OpenCinAATPath", 0, "OpenCinEnemy1f", 0, -1, -1);
    mHandles[298] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath", 0, "OpenCinEnemy2f", 0, -1, -1);
    mHandles[299] = MissionUtility::CreateObjectWithRotation("CIS_tank_wheeled_ossus3", "OpenCinWheelPath1", 0, "OpenCinEnemy3f", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[297], 50.0f);
    MissionUtility::SetVelocForward(mHandles[298], 60.0f);
    MissionUtility::SetVelocForward(mHandles[299], 55.0f);
    MissionUtility::Goto(mHandles[297], "OpenCinAATPath", false);
    MissionUtility::Goto(mHandles[298], "OpenCinWheelPath", false);
    MissionUtility::Goto(mHandles[299], "OpenCinWheelPath1", false);

    }
    }
    }
    }
    }
    // ---- +0x1144  364 bytes ----
    if (!mFlags[142] && !mFlags[138]) {
        if (MissionUtility::GetCinId(mHandles[10]) == 2) {
        MissionUtility::OverrideSoundRange(mHandles[278], true);
        MissionUtility::RemoveObject(mHandles[279]);
        MissionUtility::RemoveObject(mHandles[280]);
        MissionUtility::RemoveObject(mHandles[281]);
        MissionUtility::RemoveObject(mHandles[282]);
        MissionUtility::RemoveObject(mHandles[283]);
        MissionUtility::RemoveObject(mHandles[284]);
        MissionUtility::RemoveObject(mHandles[285]);
        MissionUtility::RemoveObject(mHandles[286]);
        MissionUtility::RemoveObject(mHandles[287]);
        MissionUtility::RemoveObject(mHandles[288]);
        MissionUtility::RemoveObject(mHandles[289]);
        MissionUtility::RemoveObject(mHandles[290]);
        MissionUtility::RemoveObject(mHandles[291]);
        MissionUtility::RemoveObject(mHandles[292]);
        MissionUtility::RemoveObject(mHandles[293]);
        MissionUtility::RemoveObject(mHandles[294]);
        MissionUtility::RemoveObject(mHandles[295]);
        MissionUtility::RemoveObject(mHandles[296]);
        MissionUtility::RemoveObject(mHandles[297]);
        MissionUtility::RemoveObject(mHandles[298]);
        MissionUtility::RemoveObject(mHandles[299]);
        MissionUtility::RemoveObject(mHandles[300]);
        MissionUtility::RemoveObject(mHandles[301]);
        MissionUtility::RemoveObject(mHandles[302]);
        mFlags[138] = true;
        MissionUtility::SetTeamNum(mHandles[12], 1);
        MissionUtility::SetTeamNum(mHandles[13], 1);
        MissionUtility::SetTeamNum(mHandles[14], 1);
        MissionUtility::AttackTarget(mHandles[278], mHandles[29], true, true, false, false);
        MissionUtility::SetCurHealth(mHandles[29], 30.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[278], 100.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[278], 100.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[278], 100.0f);
        }
    }

    // ---- +0x12b0  128 bytes ----
    if (!mFlags[142] && !mFlags[132]) {
        if (!MissionUtility::IsAlive(mHandles[29])) {
        mFlags[132] = true;
        MissionUtility::Goto(mHandles[278], "OpenCinGunshipPath", false);
        MissionUtility::QueueSound("OBR17_18", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("ASR17_16", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
    }

    // ---- +0x1330  64 bytes ----
    if (!mFlags[142]) {
    if (!mFlags[133]) {
    if (MissionUtility::GetCinId(mHandles[10]) == 3) {
    mFlags[133] = true;
    }
    }
    }

        break;
    case 1:
    // ---- +0x1370  348 bytes ----
    if (!mFlags[10]) {
        MissionUtility::PlayMusic("EP2_V1_T07", true);
        mHandles[67] = MissionUtility::AddPropArmy(2, "reparmyspawn5", 0, 50, 2.0f);
        mHandles[70] = MissionUtility::AddPropArmy(2, "reparmyspawn8", 0, 50, 2.0f);
        mHandles[69] = MissionUtility::AddPropArmy(2, "reparmyspawn7", 0, 50, 2.0f);
        mHandles[75] = MissionUtility::AddPropArmy(3, "cisarmyspawn5", 0, 50, 2.0f);
        mHandles[76] = MissionUtility::AddPropArmy(3, "cisarmyspawn6", 0, 50, 2.0f);
        mHandles[77] = MissionUtility::AddPropArmy(3, "cisarmyspawn7", 0, 50, 2.0f);
        mHandles[78] = MissionUtility::AddPropArmy(3, "cisarmyspawn8", 0, 50, 2.0f);
        mHandles[79] = MissionUtility::AddPropArmy(3, "cisarmyspawn9", 0, 50, 2.0f);
        mHandles[3] = MissionUtility::QueueSound("OBR17_18A", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Objectify("outpostobjectifypt", 0, "missions.Ossus3.marker.str0006", true, false, 750.0f, 2.0f);
        MissionUtility::Stop(mHandles[23]);
        MissionUtility::Stop(mHandles[24]);
        MissionUtility::Stop(mHandles[25]);
        mFlags[10] = true;
    }

    // ---- +0x14cc  52 bytes ----
    if (!mFlags[11]) {
        if (!MissionUtility::IsQueuedSoundPlaying(mHandles[3])) {
        MissionUtility::DisplayText("missions.Ossus3.text.str0014", 7.0f, -1.0f);
        mFlags[11] = true;
        }
    }

    // ---- +0x1500  428 bytes ----
    if (!mFlags[12]) {
        if (!MissionUtility::IsAlive(mHandles[23])) {
        if (!MissionUtility::IsAlive(mHandles[24])) {
        if (!MissionUtility::IsAlive(mHandles[25])) {
        if (!MissionUtility::IsAlive(mHandles[26])) {
        if (!MissionUtility::IsAlive(mHandles[27])) {
        if (!MissionUtility::IsAlive(mHandles[28])) {
        if (!MissionUtility::IsAlive(mHandles[35])) {
        if (!MissionUtility::IsAlive(mHandles[36])) {
        if (!MissionUtility::IsAlive(mHandles[37])) {
        if (!MissionUtility::IsAlive(mHandles[38])) {
        if (!MissionUtility::IsAlive(mHandles[39])) {
        if (!MissionUtility::IsAlive(mHandles[40])) {
        if (!MissionUtility::IsAlive(mHandles[41])) {
        if (!MissionUtility::IsAlive(mHandles[42])) {
        if (!MissionUtility::IsAlive(mHandles[43])) {
        if (!MissionUtility::IsAlive(mHandles[44])) {
        if (!MissionUtility::IsAlive(mHandles[45])) {
        if (!MissionUtility::IsAlive(mHandles[46])) {
        MissionUtility::RemoveObjectify("outpostobjectifypt", 0);
        MissionUtility::RemoveTurnAroundRegion("outpostturnaround");
        MissionUtility::Objectify("marker1", 0, "missions.Ossus3.marker.str0000", true, false, 0.0f, 2.0f);
        MissionUtility::ObjectiveComplete(mHandles[47]);
        MissionUtility::QueueSound("OBR17_19", 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[48] = MissionUtility::AddObjective("missions.Ossus3.objective.str0007");
        MissionUtility::DisplayText("missions.Ossus3.text.str0011", 7.0f, -1.0f);
        mFlags[12] = true;
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

    // ---- +0x16ac  68 bytes ----
    if (!mFlags[13]) {
    if (mFlags[12]) {
    if (MissionUtility::IsInsideRegion(mHandles[4], "vcarriertrigger")) {
    mPhase = 2;
    }
    }
    }

        break;
    case 2:
    // ---- +0x16f0  372 bytes ----
    if (!mFlags[14]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "vcarriertrigger")) {
        MissionUtility::RemoveObjectify("marker1", 0);
        MissionUtility::Objectify("marker2", 0, "missions.Ossus3.marker.str0001", true, false, 0.0f, 2.0f);
        MissionUtility::QueueSound("OBR17_20", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetTeamNum(mHandles[58], 2);
        MissionUtility::SetTeamNum(mHandles[59], 2);
        MissionUtility::SetMaxHealth(mHandles[58], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[58], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[59], 3000.0f);
        MissionUtility::SetCurHealth(mHandles[59], 3000.0f);
        MissionUtility::TakeOff(mHandles[58]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[58], "vcarrier1path", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::TakeOff(mHandles[59]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[59], "vcarrier1path", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[14] = true;
        MissionUtility::SetVelocMinimumFly(mHandles[58], 215.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[59], 215.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[58], 215.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[59], 215.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[58], 215.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[59], 215.0f);
        MissionUtility::SetMaxAltitude(mHandles[58], 999999.0f);
        MissionUtility::SetMaxAltitude(mHandles[59], 999999.0f);
        mFlags[14] = true;
        }
    }

    // ---- +0x1864  108 bytes ----
    if (!mFlags[16]) {
        if (!MissionUtility::IsAlive(mHandles[58])) {
        if (!MissionUtility::IsAlive(mHandles[59])) {
        if (!mFlags[19]) {
        if (!mFlags[20]) {
        MissionUtility::QueueSound("OBR17_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[16] = true;
        }
        }
        }
        }
    }

    // ---- +0x18d0  92 bytes ----
    if (!mFlags[15]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "marker2area")) {
        MissionUtility::RemoveObjectify("marker2", 0);
        MissionUtility::Objectify("marker3", 0, "missions.Ossus3.marker.str0002", true, false, 0.0f, 2.0f);
        MissionUtility::BeginWave("turret");
        mFlags[15] = true;
        }
    }

    // ---- +0x192c  372 bytes ----
    if (mFlags[15]) {
        if (MissionUtility::IsWaveSpawned("turret")) {
        mHandles[83] = MissionUtility::GetHandle("a2turret1");
        mHandles[84] = MissionUtility::GetHandle("a2turret2");
        mHandles[85] = MissionUtility::GetHandle("a2turret3");
        mHandles[86] = MissionUtility::GetHandle("a2turret4");
        mHandles[87] = MissionUtility::GetHandle("a2turret5");
        mHandles[88] = MissionUtility::GetHandle("a2turret6");
        mHandles[89] = MissionUtility::GetHandle("a2turret7");
        mHandles[90] = MissionUtility::GetHandle("a2turret8");
        mHandles[91] = MissionUtility::GetHandle("a2turret9");
        mHandles[92] = MissionUtility::GetHandle("a2turret10");
        mHandles[93] = MissionUtility::GetHandle("a2turret11");
        mHandles[94] = MissionUtility::GetHandle("a2turret12");
        mHandles[95] = MissionUtility::GetHandle("a2turret13");
        mHandles[96] = MissionUtility::GetHandle("a2turret14");
        mHandles[97] = MissionUtility::GetHandle("a2turret15");
        mHandles[98] = MissionUtility::GetHandle("a2turret16");
        mHandles[99] = MissionUtility::GetHandle("a2turret17");
        mHandles[100] = MissionUtility::GetHandle("a2turret18");
        mHandles[101] = MissionUtility::GetHandle("a2turret19");
        mHandles[102] = MissionUtility::GetHandle("a2turret20");
        mHandles[103] = MissionUtility::GetHandle("a2turret21");
        mHandles[104] = MissionUtility::GetHandle("a2turret22");
        mHandles[105] = MissionUtility::GetHandle("a2turret23");
        mHandles[106] = MissionUtility::GetHandle("a2turret24");
        mHandles[107] = MissionUtility::GetHandle("a2turret25");
        mHandles[108] = MissionUtility::GetHandle("a2turret26");
        mHandles[109] = MissionUtility::GetHandle("a2turret27");
        mHandles[110] = MissionUtility::GetHandle("a2turret28");
        mPhase = 3;
        }
    }

    // ---- +0x1aa0  64 bytes ----
    if (!mFlags[17]) {
        if (MissionUtility::IsInsideRegion(mHandles[58], "transportstakeoff")) {
        MissionUtility::Goto(mHandles[58], "vcarrier1path", false);
        BeginTimer(mTimer17);
        mFlags[17] = true;
        }
    }

    // ---- +0x1ae0  64 bytes ----
    if (!mFlags[18]) {
        if (MissionUtility::IsInsideRegion(mHandles[59], "transportstakeoff")) {
        MissionUtility::Goto(mHandles[59], "vcarrier1path", false);
        BeginTimer(mTimer18);
        mFlags[18] = true;
        }
    }

    // ---- +0x1b20  68 bytes ----
    if (!mFlags[19] && mFlags[17]) {
        if (mTimer17 > 5.0f) {
        MissionUtility::RemoveObject(mHandles[58]);
        MissionUtility::BonusObjectiveFailed(mHandles[7]);
        mFlags[19] = true;
        }
    }

    // ---- +0x1b64  72 bytes ----
    if (!mFlags[20]) {
    if (mFlags[18]) {
    if (mTimer18 > 5.0f) {
    MissionUtility::BonusObjectiveFailed(mHandles[7]);
    MissionUtility::RemoveObject(mHandles[59]);
    mFlags[20] = true;

        break;
    case 3:
    // ---- +0x1bac  108 bytes ----
    if (!mFlags[16]) {
        if (!MissionUtility::IsAlive(mHandles[58])) {
        if (!MissionUtility::IsAlive(mHandles[59])) {
        if (!mFlags[19]) {
        if (!mFlags[20]) {
        MissionUtility::QueueSound("OBR17_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[16] = true;
        }
        }
        }
        }
    }

    // ---- +0x1c18  64 bytes ----
    if (!mFlags[17]) {
        if (MissionUtility::IsInsideRegion(mHandles[58], "transportstakeoff")) {
        MissionUtility::Goto(mHandles[58], "vcarrier1path", false);
        BeginTimer(mTimer17);
        mFlags[17] = true;
        }
    }

    // ---- +0x1c58  64 bytes ----
    if (!mFlags[18]) {
        if (MissionUtility::IsInsideRegion(mHandles[59], "transportstakeoff")) {
        MissionUtility::Goto(mHandles[59], "vcarrier1path", false);
        BeginTimer(mTimer18);
        mFlags[18] = true;
        }
    }

    // ---- +0x1c98  68 bytes ----
    if (!mFlags[19] && mFlags[17]) {
        if (mTimer17 > 5.0f) {
        MissionUtility::BonusObjectiveFailed(mHandles[7]);
        MissionUtility::RemoveObject(mHandles[58]);
        mFlags[19] = true;
        }
    }

    // ---- +0x1cdc  68 bytes ----
    if (!mFlags[20] && mFlags[18]) {
        if (mTimer18 > 5.0f) {
        MissionUtility::BonusObjectiveFailed(mHandles[7]);
        MissionUtility::RemoveObject(mHandles[59]);
        mFlags[20] = true;
        }
    }

    // ---- +0x1d20  844 bytes ----
    if (!mFlags[25]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "flyertrigger")) {
        MissionUtility::RemoveObjectify("marker3", 0);
        mHandles[82] = MissionUtility::AddObjective("missions.Ossus3.objective.str0011");
        MissionUtility::DisplayText("missions.Ossus3.text.str0015", 7.0f, -1.0f);
        MissionUtility::ObjectiveComplete(mHandles[48]);
        MissionUtility::QueueSound("OBR17_04F", 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[80] = MissionUtility::AddFlyerArmy(0, "repflyerspawn1", 0, 6, 1.0f);
        mHandles[81] = MissionUtility::AddFlyerArmy(0, "repflyerspawn2", 0, 6, 1.0f);
        mHandles[112] = MissionUtility::CreateObject("REP_fly_fighter", "flyerpath1", 0, "repflyer1", 3, -1);
        mHandles[113] = MissionUtility::CreateObject("REP_fly_fighter", "flyerpath1", 1, "repflyer2", 3, -1);
        mHandles[114] = MissionUtility::CreateObject("REP_fly_fighter", "flyerpath1", 2, "repflyer3", 3, -1);
        MissionUtility::SetCurHealth(mHandles[112], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[113], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[114], 999999.0f);
        MissionUtility::Goto(mHandles[112], "flyerpath1", false);
        MissionUtility::Goto(mHandles[113], "flyerpath1", false);
        MissionUtility::Goto(mHandles[114], "flyerpath1", false);
        mHandles[111] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[111], mHandles[83]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[84]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[85]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[86]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[87]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[88]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[89]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[90]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[91]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[92]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[93]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[94]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[95]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[96]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[97]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[98]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[99]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[100]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[101]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[102]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[103]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[104]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[105]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[106]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[107]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[108]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[109]);
        MissionUtility::AddFlockMember(mHandles[111], mHandles[110]);
        mFlags[25] = true;
        }
    }

    // ---- +0x206c  80 bytes ----
    if (!mFlags[24] && mFlags[25]) {
        if (MissionUtility::GetFlockCount(mHandles[111]) < 6) {
        MissionUtility::QueueSound("OBR17_04E", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[24] = true;
        }
    }

    // ---- +0x20bc  400 bytes ----
    if (!mFlags[23]) {
    if (mFlags[25]) {
    if (!MissionUtility::IsFlockAlive(mHandles[111])) {
    MissionUtility::ObjectiveComplete(mHandles[82]);
    MissionUtility::QueueSound("OBR17_26", 1.0f, 0.0f, 0.0f, "", 0, "");
    mHandles[60] = MissionUtility::CreateObjectWithRotation("REP_fly_gunship", "turretgunship1path", 0, "turretgunship1", 1, -1, -1);
    MissionUtility::Goto(mHandles[60], "turretgunship1path", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Land(mHandles[60], "turretgunship1land", 0, 10.0f);
    MissionUtility::SetQueueFlag(false);
    mHandles[61] = MissionUtility::CreateObjectWithRotation("REP_fly_gunship", "turretgunship2path", 0, "turretgunship2", 1, -1, -1);
    MissionUtility::Goto(mHandles[61], "turretgunship2path", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Land(mHandles[61], "turretgunship2land", 0, 10.0f);
    MissionUtility::SetQueueFlag(false);
    mHandles[62] = MissionUtility::CreateObjectWithRotation("REP_fly_gunship", "turretgunship3path", 0, "turretgunship3", 1, -1, -1);
    MissionUtility::Goto(mHandles[62], "turretgunship3path", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Land(mHandles[62], "turretgunship3land", 0, 10.0f);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::RemoveTurnAroundRegion("turretturnaround");
    mHandles[49] = MissionUtility::AddObjective("missions.Ossus3.objective.str0008");
    MissionUtility::DisplayText("missions.Ossus3.text.str0012", 7.0f, -1.0f);
    mPhase = 4;
    mFlags[23] = true;

        break;
    case 4:
    // ---- +0x224c  52 bytes ----
    if (!mFlags[26]) {
        MissionUtility::Objectify("marker4", 0, "missions.Ossus3.marker.str0003", true, false, 0.0f, 2.0f);
        mFlags[26] = true;
    }

    // ---- +0x2280  56 bytes ----
    if (!mFlags[27]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "marker4area")) {
        MissionUtility::BeginWave("convoy");
        MissionUtility::BeginWave("techno");
        mFlags[27] = true;
        }
    }

    // ---- +0x22b8  1220 bytes ----
    if (mFlags[27]) {
    if (MissionUtility::IsWaveSpawned("convoy")) {
    if (MissionUtility::IsWaveSpawned("techno")) {
    mHandles[129] = MissionUtility::GetHandle("convoy1");
    mHandles[130] = MissionUtility::GetHandle("convoy2");
    mHandles[131] = MissionUtility::GetHandle("convoy3");
    mHandles[132] = MissionUtility::GetHandle("convoy4");
    mHandles[133] = MissionUtility::GetHandle("convoy5");
    mHandles[134] = MissionUtility::GetHandle("convoy6");
    mHandles[135] = MissionUtility::GetHandle("convoy7");
    mHandles[136] = MissionUtility::GetHandle("convoy8");
    mHandles[137] = MissionUtility::GetHandle("cassault1");
    mHandles[138] = MissionUtility::GetHandle("cassault2");
    mHandles[139] = MissionUtility::GetHandle("cassault3");
    mHandles[140] = MissionUtility::GetHandle("cassault4");
    mHandles[141] = MissionUtility::GetHandle("cassault5");
    mHandles[142] = MissionUtility::GetHandle("cassault6");
    mHandles[143] = MissionUtility::GetHandle("cassault7");
    mHandles[144] = MissionUtility::GetHandle("cassault8");
    mHandles[158] = MissionUtility::GetHandle("techno1");
    mHandles[159] = MissionUtility::GetHandle("techno2");
    mHandles[160] = MissionUtility::GetHandle("techno3");
    mHandles[145] = MissionUtility::CreateFlock(mHandles[129], (Formation)8);
    MissionUtility::AddFlockMember(mHandles[145], mHandles[130], Vector(0.0f, 0.0f, 110.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[131], Vector(0.0f, 0.0f, 220.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[135], Vector(0.0f, 0.0f, 330.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[132], Vector(70.0f, 0.0f, 0.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[133], Vector(70.0f, 0.0f, 110.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[134], Vector(70.0f, 0.0f, 220.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[136], Vector(70.0f, 0.0f, 330.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[137], Vector(-70.0f, 0.0f, 0.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[138], Vector(-70.0f, 0.0f, 110.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[139], Vector(-70.0f, 0.0f, 220.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[143], Vector(-70.0f, 0.0f, 320.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[140], Vector(140.0f, 0.0f, 0.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[141], Vector(140.0f, 0.0f, 110.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[142], Vector(140.0f, 0.0f, 220.0f));
    MissionUtility::AddFlockMember(mHandles[145], mHandles[144], Vector(140.0f, 0.0f, 330.0f));
    MissionUtility::Stop(mHandles[145]);
    MissionUtility::SetVelocForward(mHandles[145], 70.0f);
    mPhase = 5;

        break;
    case 5:
    // ---- +0x277c  192 bytes ----
    if (!mFlags[30]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "convoytrigger")) {
        MissionUtility::ObjectiveComplete(mHandles[49]);
        MissionUtility::RemoveObjectify("marker4", 0);
        MissionUtility::Objectify("marker5", 0, "missions.Ossus3.marker.str0004", true, false, 0.0f, 2.0f);
        mHandles[128] = MissionUtility::AddObjective("missions.Ossus3.objective.str0012");
        MissionUtility::DisplayText("missions.Ossus3.text.str0016", 7.0f, -1.0f);
        MissionUtility::QueueSound("OBR17_04I", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Goto(mHandles[145], "convoypath1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[145]);
        MissionUtility::SetQueueFlag(false);
        mFlags[30] = true;
        }
    }

    // ---- +0x283c  52 bytes ----
    if (!mFlags[32]) {
        if (MissionUtility::IsInsideRegion(mHandles[4], "obj4area")) {
        MissionUtility::RemoveObjectify("marker5", 0);
        mFlags[32] = true;
        }
    }

    // ---- +0x2870  104 bytes ----
    if (!mFlags[28] && mFlags[32]) {
        if (MissionUtility::IsInsideRegion(mHandles[145], "icesheet6break")) {
        MissionUtility::FlushSoundQueue();
        mHandles[146] = MissionUtility::StartSound("OBR17_04K", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::BonusObjectiveFailed(mHandles[128]);
        mFlags[28] = true;
        }
    }

    // ---- +0x28d8  56 bytes ----
    if (!mFlags[33]) {
        if (MissionUtility::IsInsideRegion(mHandles[145], "icesheet4break")) {
        MissionUtility::DamageObject(mHandles[125], 999999.0f, 999999.0f);
        mFlags[33] = true;
        }
    }

    // ---- +0x2910  56 bytes ----
    if (!mFlags[34]) {
        if (MissionUtility::IsInsideRegion(mHandles[145], "icesheet5break")) {
        MissionUtility::DamageObject(mHandles[126], 999999.0f, 999999.0f);
        mFlags[34] = true;
        }
    }

    // ---- +0x2948  56 bytes ----
    if (!mFlags[35]) {
        if (MissionUtility::IsInsideRegion(mHandles[145], "icesheet6break")) {
        MissionUtility::DamageObject(mHandles[127], 999999.0f, 999999.0f);
        mFlags[35] = true;
        }
    }

    // ---- +0x2980  40 bytes ----
    if (mFlags[28] & !MissionUtility::IsSoundPlaying(mHandles[146])) {
    mPhase = 11;
    }

    // ---- +0x29a8  168 bytes ----
    if (!mFlags[31] && mFlags[30] && mFlags[32]) {
        if (!MissionUtility::IsFlockAlive(mHandles[145])) {
        MissionUtility::ObjectiveComplete(mHandles[128]);
        mHandles[50] = MissionUtility::AddObjective("missions.Ossus3.objective.str0013");
        MissionUtility::DisplayText("missions.Ossus3.text.str0017", 7.0f, -1.0f);
        MissionUtility::Objectify("marker6", 0, "missions.Ossus3.marker.str0005", true, false, 0.0f, 2.0f);
        MissionUtility::QueueSound("OBR17_04J", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveTurnAroundRegion("basetrigger");
        mFlags[31] = true;
        }
    }

    // ---- +0x2a50  44 bytes ----
    if (mFlags[31]) {
    if (MissionUtility::IsInsideRegion(mHandles[4], "basetrigger")) {
    mPhase = 14;

        break;
    case 14:
    // ---- +0x2a7c  56 bytes ----
    StopTimer(mTimer16);
    MissionUtility::MidMissionSavePlayer(15);
    MissionUtility::MidMissionSave(mTimer16);
    MissionUtility::MidMissionSave((bool)mFlags[19]);
    MissionUtility::MidMissionSave((bool)mFlags[20]);
    mPhase = 6;

        break;
    case 15:
    // ---- +0x2ab4  156 bytes ----
    mHandles[48] = MissionUtility::AddObjective("missions.Ossus3.objective.str0009");
    mHandles[82] = MissionUtility::AddObjective("missions.Ossus3.objective.str0015");
    mHandles[49] = MissionUtility::AddObjective("missions.Ossus3.objective.str0008");
    mHandles[128] = MissionUtility::AddObjective("missions.Ossus3.objective.str0012");
    MissionUtility::ObjectiveComplete(mHandles[47]);
    MissionUtility::ObjectiveComplete(mHandles[48]);
    MissionUtility::ObjectiveComplete(mHandles[82]);
    MissionUtility::ObjectiveComplete(mHandles[49]);
    MissionUtility::ObjectiveComplete(mHandles[128]);
    MissionUtility::MidMissionLoadPlayer();
    StopTimer(mTimer16);
    float savedTimeA;
    MissionUtility::MidMissionLoad(savedTimeA);
    mTimer16 = savedTimeA;
    MissionUtility::MidMissionLoad(mFlags[19]);
    if (mFlags[19]) {
    MissionUtility::BonusObjectiveFailed(mHandles[7]);
    mFlags[22] = true;
    }

    // ---- +0x2b50  36 bytes ----
    MissionUtility::MidMissionLoad(mFlags[20]);
    if (mFlags[20]) {
    MissionUtility::BonusObjectiveFailed(mHandles[7]);
    mFlags[22] = true;
    }

    // ---- +0x2b74  708 bytes ----
    MissionUtility::RemoveObject(mHandles[16]);
    MissionUtility::RemoveObject(mHandles[17]);
    MissionUtility::RemoveObject(mHandles[18]);
    MissionUtility::RemoveObject(mHandles[19]);
    MissionUtility::RemoveObject(mHandles[20]);
    MissionUtility::RemoveObject(mHandles[21]);
    MissionUtility::RemoveObject(mHandles[22]);
    MissionUtility::RemoveObject(mHandles[23]);
    MissionUtility::RemoveObject(mHandles[24]);
    MissionUtility::RemoveObject(mHandles[25]);
    MissionUtility::RemoveObject(mHandles[26]);
    MissionUtility::RemoveObject(mHandles[27]);
    MissionUtility::RemoveObject(mHandles[28]);
    MissionUtility::RemoveObject(mHandles[29]);
    MissionUtility::RemoveObject(mHandles[30]);
    MissionUtility::RemoveObject(mHandles[31]);
    MissionUtility::RemoveObject(mHandles[32]);
    MissionUtility::RemoveObject(mHandles[33]);
    MissionUtility::RemoveObject(mHandles[34]);
    MissionUtility::RemoveObject(mHandles[35]);
    MissionUtility::RemoveObject(mHandles[36]);
    MissionUtility::RemoveObject(mHandles[37]);
    MissionUtility::RemoveObject(mHandles[38]);
    MissionUtility::RemoveObject(mHandles[39]);
    MissionUtility::RemoveObject(mHandles[40]);
    MissionUtility::RemoveObject(mHandles[41]);
    MissionUtility::RemoveObject(mHandles[42]);
    MissionUtility::RemoveObject(mHandles[43]);
    MissionUtility::RemoveObject(mHandles[44]);
    MissionUtility::RemoveObject(mHandles[45]);
    MissionUtility::RemoveObject(mHandles[46]);
    MissionUtility::RemoveObject(mHandles[12]);
    MissionUtility::RemoveObject(mHandles[13]);
    MissionUtility::RemoveObject(mHandles[14]);
    MissionUtility::RemoveObject(mHandles[58]);
    MissionUtility::RemoveObject(mHandles[59]);
    MissionUtility::RemoveObject(mHandles[83]);
    MissionUtility::RemoveObject(mHandles[84]);
    MissionUtility::RemoveObject(mHandles[85]);
    MissionUtility::RemoveObject(mHandles[86]);
    MissionUtility::RemoveObject(mHandles[87]);
    MissionUtility::RemoveObject(mHandles[88]);
    MissionUtility::RemoveObject(mHandles[89]);
    MissionUtility::RemoveObject(mHandles[90]);
    MissionUtility::RemoveObject(mHandles[91]);
    MissionUtility::RemoveObject(mHandles[92]);
    MissionUtility::RemoveObject(mHandles[93]);
    MissionUtility::RemoveObject(mHandles[94]);
    MissionUtility::RemoveObject(mHandles[95]);
    MissionUtility::RemoveObject(mHandles[96]);
    MissionUtility::RemoveObject(mHandles[97]);
    MissionUtility::RemoveObject(mHandles[98]);
    MissionUtility::RemoveObject(mHandles[99]);
    MissionUtility::RemoveObject(mHandles[100]);
    MissionUtility::RemoveObject(mHandles[101]);
    MissionUtility::RemoveObject(mHandles[102]);
    MissionUtility::RemoveObject(mHandles[103]);
    MissionUtility::RemoveObject(mHandles[104]);
    MissionUtility::RemoveObject(mHandles[105]);
    MissionUtility::RemoveObject(mHandles[106]);
    MissionUtility::RemoveObject(mHandles[107]);
    MissionUtility::RemoveObject(mHandles[108]);
    MissionUtility::RemoveObject(mHandles[109]);
    MissionUtility::RemoveObject(mHandles[110]);
    MissionUtility::RemoveObject(mHandles[122]);
    MissionUtility::RemoveObject(mHandles[123]);
    MissionUtility::RemoveObject(mHandles[124]);
    MissionUtility::RemoveObject(mHandles[125]);
    MissionUtility::RemoveObject(mHandles[126]);
    MissionUtility::RemoveObject(mHandles[127]);
    MissionUtility::RemoveObject(mHandles[129]);
    MissionUtility::RemoveObject(mHandles[130]);
    MissionUtility::RemoveObject(mHandles[131]);
    MissionUtility::RemoveObject(mHandles[132]);
    MissionUtility::RemoveObject(mHandles[133]);
    MissionUtility::RemoveObject(mHandles[134]);
    MissionUtility::RemoveObject(mHandles[135]);
    MissionUtility::RemoveObject(mHandles[136]);
    MissionUtility::RemoveObject(mHandles[137]);
    MissionUtility::RemoveObject(mHandles[138]);
    MissionUtility::RemoveObject(mHandles[139]);
    MissionUtility::RemoveObject(mHandles[140]);
    MissionUtility::RemoveObject(mHandles[141]);
    MissionUtility::RemoveObject(mHandles[142]);
    MissionUtility::RemoveObject(mHandles[143]);
    MissionUtility::RemoveObject(mHandles[144]);
    mFlags[36] = true;
    mPhase = 6;

        break;
    case 6:
    // ---- +0x2e38  48 bytes ----
    if (!mFlags[39]) {
        MissionUtility::BeginWave("temple");
        if (mFlags[36]) {
        MissionUtility::BeginWave("techno");
        }
        mFlags[39] = true;
    }

    // ---- +0x2e68  668 bytes ----
    if (!mFlags[40] && mFlags[39]) {
        if (MissionUtility::IsWaveSpawned("temple")) {
        if (MissionUtility::IsWaveSpawned("techno")) {
        StopTimer(mTimer16);
        MissionUtility::RemoveTurnAroundRegion("basetrigger");
        mHandles[165] = MissionUtility::GetHandle("lconvoy1cin");
        mHandles[166] = MissionUtility::GetHandle("lconvoy2cin");
        mHandles[167] = MissionUtility::GetHandle("lconvoy3cin");
        mHandles[168] = MissionUtility::GetHandle("lconvoy4cin");
        mHandles[169] = MissionUtility::GetHandle("lconvoy5cin");
        mHandles[170] = MissionUtility::GetHandle("lconvoy1");
        mHandles[171] = MissionUtility::GetHandle("lconvoy2");
        mHandles[172] = MissionUtility::GetHandle("lconvoy3");
        mHandles[173] = MissionUtility::GetHandle("lconvoy4");
        mHandles[174] = MissionUtility::GetHandle("lconvoy5");
        mHandles[175] = MissionUtility::GetHandle("lbeam1");
        mHandles[178] = MissionUtility::GetHandle("rconvoy1cin");
        mHandles[179] = MissionUtility::GetHandle("rconvoy2cin");
        mHandles[180] = MissionUtility::GetHandle("rconvoy3cin");
        mHandles[181] = MissionUtility::GetHandle("rconvoy4cin");
        mHandles[182] = MissionUtility::GetHandle("rconvoy5cin");
        mHandles[183] = MissionUtility::GetHandle("rconvoy1");
        mHandles[184] = MissionUtility::GetHandle("rconvoy2");
        mHandles[185] = MissionUtility::GetHandle("rconvoy3");
        mHandles[186] = MissionUtility::GetHandle("rconvoy4");
        mHandles[187] = MissionUtility::GetHandle("rconvoy5");
        mHandles[188] = MissionUtility::GetHandle("rbeam1");
        mHandles[209] = MissionUtility::GetHandle("rassaulta1");
        mHandles[210] = MissionUtility::GetHandle("rassaulta2");
        mHandles[211] = MissionUtility::GetHandle("rassaulta3");
        mHandles[213] = MissionUtility::GetHandle("rassaultb1");
        mHandles[214] = MissionUtility::GetHandle("rassaultb2");
        mHandles[215] = MissionUtility::GetHandle("rassaultb3");
        mHandles[217] = MissionUtility::GetHandle("rassaultc1");
        mHandles[218] = MissionUtility::GetHandle("rassaultc2");
        mHandles[219] = MissionUtility::GetHandle("rassaultc3");
        mHandles[221] = MissionUtility::GetHandle("rassaultd1");
        mHandles[222] = MissionUtility::GetHandle("rassaultd2");
        mHandles[223] = MissionUtility::GetHandle("rassaultd3");
        mHandles[193] = MissionUtility::GetHandle("lassaulta1");
        mHandles[194] = MissionUtility::GetHandle("lassaulta2");
        mHandles[195] = MissionUtility::GetHandle("lassaulta3");
        mHandles[197] = MissionUtility::GetHandle("lassaultb1");
        mHandles[198] = MissionUtility::GetHandle("lassaultb2");
        mHandles[199] = MissionUtility::GetHandle("lassaultb3");
        mHandles[201] = MissionUtility::GetHandle("lassaultc1");
        mHandles[202] = MissionUtility::GetHandle("lassaultc2");
        mHandles[203] = MissionUtility::GetHandle("lassaultc3");
        mHandles[205] = MissionUtility::GetHandle("lassaultd1");
        mHandles[206] = MissionUtility::GetHandle("lassaultd2");
        mHandles[207] = MissionUtility::GetHandle("lassaultd3");
        mHandles[158] = MissionUtility::GetHandle("techno1");
        mHandles[159] = MissionUtility::GetHandle("techno2");
        mHandles[160] = MissionUtility::GetHandle("techno3");
        mFlags[40] = true;
        }
        }
    }

    // ---- +0x3104  536 bytes ----
    if (!mFlags[54] && mFlags[40]) {
        MissionUtility::SetCurHealth(mHandles[188], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[175], 999999.0f);
        MissionUtility::SetVelocForward(mHandles[209], 20.0f);
        mHandles[212] = MissionUtility::CreateFlock(mHandles[209], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[212], mHandles[210]);
        MissionUtility::AddFlockMember(mHandles[212], mHandles[211]);
        MissionUtility::Stop(mHandles[212]);
        MissionUtility::SetVelocForward(mHandles[213], 20.0f);
        mHandles[216] = MissionUtility::CreateFlock(mHandles[213], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[216], mHandles[214]);
        MissionUtility::AddFlockMember(mHandles[216], mHandles[215]);
        MissionUtility::Stop(mHandles[216]);
        MissionUtility::SetVelocForward(mHandles[217], 20.0f);
        mHandles[220] = MissionUtility::CreateFlock(mHandles[217], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[220], mHandles[218]);
        MissionUtility::AddFlockMember(mHandles[220], mHandles[219]);
        MissionUtility::Stop(mHandles[220]);
        MissionUtility::SetVelocForward(mHandles[221], 20.0f);
        mHandles[224] = MissionUtility::CreateFlock(mHandles[221], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[224], mHandles[222]);
        MissionUtility::AddFlockMember(mHandles[224], mHandles[223]);
        MissionUtility::Stop(mHandles[224]);
        MissionUtility::SetVelocForward(mHandles[193], 20.0f);
        mHandles[196] = MissionUtility::CreateFlock(mHandles[193], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[196], mHandles[194]);
        MissionUtility::AddFlockMember(mHandles[196], mHandles[195]);
        MissionUtility::Stop(mHandles[196]);
        MissionUtility::SetVelocForward(mHandles[197], 20.0f);
        mHandles[200] = MissionUtility::CreateFlock(mHandles[197], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[200], mHandles[198]);
        MissionUtility::AddFlockMember(mHandles[200], mHandles[199]);
        MissionUtility::Stop(mHandles[200]);
        MissionUtility::SetVelocForward(mHandles[201], 20.0f);
        mHandles[204] = MissionUtility::CreateFlock(mHandles[201], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[204], mHandles[202]);
        MissionUtility::AddFlockMember(mHandles[204], mHandles[203]);
        MissionUtility::Stop(mHandles[204]);
        MissionUtility::SetVelocForward(mHandles[205], 20.0f);
        mHandles[208] = MissionUtility::CreateFlock(mHandles[205], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[208], mHandles[206]);
        MissionUtility::AddFlockMember(mHandles[208], mHandles[207]);
        MissionUtility::Stop(mHandles[208]);
        mPhase = 7;
    }

    // ---- +0x331c  552 bytes ----
    if (!mFlags[54]) {
    if (mFlags[40]) {
    if (mFlags[36]) {
    MissionUtility::SetCurHealth(mHandles[188], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[175], 999999.0f);
    MissionUtility::SetVelocForward(mHandles[209], 20.0f);
    mHandles[212] = MissionUtility::CreateFlock(mHandles[209], (Formation)1);
    MissionUtility::AddFlockMember(mHandles[212], mHandles[210]);
    MissionUtility::AddFlockMember(mHandles[212], mHandles[211]);
    MissionUtility::Stop(mHandles[212]);
    MissionUtility::SetVelocForward(mHandles[213], 20.0f);
    mHandles[216] = MissionUtility::CreateFlock(mHandles[213], (Formation)1);
    MissionUtility::AddFlockMember(mHandles[216], mHandles[214]);
    MissionUtility::AddFlockMember(mHandles[216], mHandles[215]);
    MissionUtility::Stop(mHandles[216]);
    MissionUtility::SetVelocForward(mHandles[217], 20.0f);
    mHandles[220] = MissionUtility::CreateFlock(mHandles[217], (Formation)1);
    MissionUtility::AddFlockMember(mHandles[220], mHandles[218]);
    MissionUtility::AddFlockMember(mHandles[220], mHandles[219]);
    MissionUtility::Stop(mHandles[220]);
    MissionUtility::SetVelocForward(mHandles[221], 20.0f);
    mHandles[224] = MissionUtility::CreateFlock(mHandles[221], (Formation)1);
    MissionUtility::AddFlockMember(mHandles[224], mHandles[222]);
    MissionUtility::AddFlockMember(mHandles[224], mHandles[223]);
    MissionUtility::Stop(mHandles[224]);
    MissionUtility::SetVelocForward(mHandles[193], 20.0f);
    mHandles[196] = MissionUtility::CreateFlock(mHandles[193], (Formation)1);
    MissionUtility::AddFlockMember(mHandles[196], mHandles[194]);
    MissionUtility::AddFlockMember(mHandles[196], mHandles[195]);
    MissionUtility::Stop(mHandles[196]);
    MissionUtility::SetVelocForward(mHandles[197], 20.0f);
    mHandles[200] = MissionUtility::CreateFlock(mHandles[197], (Formation)1);
    MissionUtility::AddFlockMember(mHandles[200], mHandles[198]);
    MissionUtility::AddFlockMember(mHandles[200], mHandles[199]);
    MissionUtility::Stop(mHandles[200]);
    MissionUtility::SetVelocForward(mHandles[201], 20.0f);
    mHandles[204] = MissionUtility::CreateFlock(mHandles[201], (Formation)1);
    MissionUtility::AddFlockMember(mHandles[204], mHandles[202]);
    MissionUtility::AddFlockMember(mHandles[204], mHandles[203]);
    MissionUtility::Stop(mHandles[204]);
    MissionUtility::SetVelocForward(mHandles[205], 20.0f);
    mHandles[208] = MissionUtility::CreateFlock(mHandles[205], (Formation)1);
    MissionUtility::AddFlockMember(mHandles[208], mHandles[206]);
    MissionUtility::AddFlockMember(mHandles[208], mHandles[207]);
    MissionUtility::Stop(mHandles[208]);
    mPhase = 7;

        break;
    case 7:
    // ---- +0x3544  1704 bytes ----
    if (!mFlags[134]) {
        MissionUtility::MoveObjectWithRotation(mHandles[4], "midcin1moveplayer1", 0, true);
        MissionUtility::SetTeamNum(mHandles[4], 0);
        MissionUtility::SetAsPlayer(mHandles[5], 0);
        MissionUtility::SetVelocMinimumFly(mHandles[6], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[6], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[6], 0.0f);
        MissionUtility::ObjectiveComplete(mHandles[50]);
        MissionUtility::SetFogRange(500.0f, 1250.0f, 0.0f);
        MissionUtility::SetTeamNum(mHandles[165], 1);
        MissionUtility::SetTeamNum(mHandles[166], 1);
        MissionUtility::SetTeamNum(mHandles[167], 1);
        MissionUtility::SetTeamNum(mHandles[168], 1);
        MissionUtility::SetTeamNum(mHandles[169], 1);
        MissionUtility::SetCurHealth(mHandles[165], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[166], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[167], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[168], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[169], 999999.0f);
        MissionUtility::SetTeamNum(mHandles[178], 1);
        MissionUtility::SetTeamNum(mHandles[179], 1);
        MissionUtility::SetTeamNum(mHandles[180], 1);
        MissionUtility::SetTeamNum(mHandles[181], 1);
        MissionUtility::SetTeamNum(mHandles[182], 1);
        MissionUtility::SetCurHealth(mHandles[178], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[179], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[180], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[181], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[182], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[303], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[304], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[305], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[306], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[307], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[308], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[309], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[310], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[311], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[312], 999999.0f);
        MissionUtility::CarrierAddCargo(mHandles[303], "hp_link_1", mHandles[165], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[304], "hp_link_1", mHandles[166], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[305], "hp_link_1", mHandles[167], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[306], "hp_link_1", mHandles[168], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[307], "hp_link_1", mHandles[169], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[308], "hp_link_1", mHandles[178], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[309], "hp_link_1", mHandles[179], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[310], "hp_link_1", mHandles[180], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[311], "hp_link_1", mHandles[181], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[312], "hp_link_1", mHandles[182], "hp_link_1", true);
        MissionUtility::SetVelocMinimumFly(mHandles[308], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[308], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[308], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[309], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[309], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[309], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[310], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[310], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[310], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[311], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[311], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[311], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[312], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[312], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[312], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[303], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[303], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[303], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[304], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[304], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[304], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[305], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[305], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[305], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[306], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[306], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[306], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[307], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[307], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[307], 170.0f);
        MissionUtility::Goto(mHandles[303], "MidCinLeftCarrierDropoff1", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[303], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[303], "MidCinLeftCarrierLeave1", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[304], "MidCinLeftCarrierDropoff2", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[304], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[304], "MidCinLeftCarrierLeave2", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[305], "MidCinLeftCarrierDropoff3", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[305], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[305], "MidCinLeftCarrierLeave3", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[306], "MidCinLeftCarrierDropoff4", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[306], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[306], "MidCinLeftCarrierLeave4", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[307], "MidCinLeftCarrierDropoff5", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[307], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[307], "MidCinLeftCarrierLeave5", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::OverrideSoundRange(mHandles[303], true);
        MissionUtility::OverrideSoundRange(mHandles[304], true);
        MissionUtility::OverrideSoundRange(mHandles[305], true);
        MissionUtility::OverrideSoundRange(mHandles[306], true);
        MissionUtility::OverrideSoundRange(mHandles[307], true);
        MissionUtility::OverrideSoundRange(mHandles[165], true);
        MissionUtility::OverrideSoundRange(mHandles[166], true);
        MissionUtility::OverrideSoundRange(mHandles[167], true);
        MissionUtility::OverrideSoundRange(mHandles[168], true);
        MissionUtility::OverrideSoundRange(mHandles[169], true);
        mFlags[134] = true;
        mHandles[147] = MissionUtility::RunCin("midcin1", true, true);
        MissionUtility::PlayMusic("EP6_V2_T03", true);
        MissionUtility::QueueSound("obr17_28", 1.0f, 0.0f, 0.0f, "", 0, "");
        BeginTimer(mTimer19);
        BeginTimer(mTimer20);
    }

    // ---- +0x3bec  52 bytes ----
    if (!mFlags[143]) {
        if (mTimer20 > 4.0f) {
        StopTimer(mTimer20);
        mTimer20 = 0.0f;
        }
    }

    // ---- +0x3c20  732 bytes ----
    if (!mFlags[143]) {
        if (mTimer19 > 10.0f) {
        StopTimer(mTimer19);
        mTimer19 = 0.0f;
        MissionUtility::SetVelocMinimumFly(mHandles[308], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[308], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[308], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[309], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[309], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[309], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[310], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[310], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[310], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[311], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[311], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[311], 170.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[312], 10.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[312], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[312], 170.0f);
        MissionUtility::CarrierDropoff(mHandles[308], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[308], "MidCinRightCarrierLeave1", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::CarrierDropoff(mHandles[309], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[309], "MidCinRightCarrierLeave2", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::CarrierDropoff(mHandles[310], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[310], "MidCinRightCarrierLeave3", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::CarrierDropoff(mHandles[311], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[311], "MidCinRightCarrierLeave4", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::CarrierDropoff(mHandles[312], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[312], "MidCinRightCarrierLeave5", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::OverrideSoundRange(mHandles[303], false);
        MissionUtility::OverrideSoundRange(mHandles[304], false);
        MissionUtility::OverrideSoundRange(mHandles[305], false);
        MissionUtility::OverrideSoundRange(mHandles[306], false);
        MissionUtility::OverrideSoundRange(mHandles[307], false);
        MissionUtility::OverrideSoundRange(mHandles[165], false);
        MissionUtility::OverrideSoundRange(mHandles[166], false);
        MissionUtility::OverrideSoundRange(mHandles[167], false);
        MissionUtility::OverrideSoundRange(mHandles[168], false);
        MissionUtility::OverrideSoundRange(mHandles[169], false);
        MissionUtility::OverrideSoundRange(mHandles[308], true);
        MissionUtility::OverrideSoundRange(mHandles[309], true);
        MissionUtility::OverrideSoundRange(mHandles[310], true);
        MissionUtility::OverrideSoundRange(mHandles[311], true);
        MissionUtility::OverrideSoundRange(mHandles[312], true);
        MissionUtility::OverrideSoundRange(mHandles[178], true);
        MissionUtility::OverrideSoundRange(mHandles[179], true);
        MissionUtility::OverrideSoundRange(mHandles[180], true);
        MissionUtility::OverrideSoundRange(mHandles[181], true);
        MissionUtility::OverrideSoundRange(mHandles[182], true);
        }
    }

    // ---- +0x3efc  3020 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[147])) {
    mFlags[143] = true;
    MissionUtility::FlushSoundQueue();
    MissionUtility::SetOriginalFog(0.0f);
    MissionUtility::RemoveObjectify("marker6", 0);
    MissionUtility::RemoveTurnAroundRegion("basetrigger");
    MissionUtility::RemoveObject(mHandles[303]);
    MissionUtility::RemoveObject(mHandles[304]);
    MissionUtility::RemoveObject(mHandles[305]);
    MissionUtility::RemoveObject(mHandles[306]);
    MissionUtility::RemoveObject(mHandles[307]);
    MissionUtility::RemoveObject(mHandles[308]);
    MissionUtility::RemoveObject(mHandles[309]);
    MissionUtility::RemoveObject(mHandles[310]);
    MissionUtility::RemoveObject(mHandles[311]);
    MissionUtility::RemoveObject(mHandles[312]);
    MissionUtility::RemoveObject(mHandles[178]);
    MissionUtility::RemoveObject(mHandles[179]);
    MissionUtility::RemoveObject(mHandles[180]);
    MissionUtility::RemoveObject(mHandles[181]);
    MissionUtility::RemoveObject(mHandles[182]);
    MissionUtility::RemoveObject(mHandles[165]);
    MissionUtility::RemoveObject(mHandles[166]);
    MissionUtility::RemoveObject(mHandles[167]);
    MissionUtility::RemoveObject(mHandles[168]);
    MissionUtility::RemoveObject(mHandles[169]);
    mHandles[183] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(2285.103f, -304.7462f, 2837.0396f), "rconvoy1", 0, -1, Quat(0.409107f, 0.0f, -0.912486f, 0.0f), -1);
    mHandles[184] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(2270.3188f, -295.81665f, 2876.1118f), "rconvoy2", 0, -1, Quat(0.390794f, 0.0f, -0.920478f, 0.0f), -1);
    mHandles[185] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(2324.0952f, -307.22272f, 2820.8762f), "rconvoy3", 0, -1, Quat(0.390794f, 0.0f, -0.920478f, 0.0f), -1);
    mHandles[186] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(2416.551f, -306.8968f, 2918.3655f), "rconvoy4", 0, -1, Quat(0.399981f, 0.0f, -0.916524f, 0.0f), -1);
    mHandles[187] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(2371.784f, -297.957f, 2968.741f), "rconvoy5", 0, -1, Quat(0.390797f, 0.0f, -0.920477f, 0.0f), -1);
    mHandles[170] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(521.6839f, -307.9935f, 3055.2961f), "lconvoy1", 0, -1, Quat(-0.372095f, 0.0f, -0.928195f, 0.0f), -1);
    mHandles[172] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(522.73004f, -304.3679f, 3102.1028f), "lconvoy3", 0, -1, Quat(-0.381339f, 0.0f, -0.924435f, 0.0f), -1);
    mHandles[171] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(467.0359f, -307.59738f, 3050.4814f), "lconvoy2", 0, -1, Quat(-0.399726f, 0.0f, -0.916635f, 0.0f), -1);
    mHandles[173] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(427.1785f, -294.51935f, 3185.9788f), "lconvoy4", 0, -1, Quat(-0.362813f, 0.0f, -0.931862f, 0.0f), -1);
    mHandles[174] = MissionUtility::CreateObject("REP_walk_sixleg", Vector(382.3483f, -307.6467f, 3140.6768f), "lconvoy5", 0, -1, Quat(-0.343772f, 0.0f, -0.939053f, 0.0f), -1);
    MissionUtility::SetTeamNum(mHandles[183], 1);
    MissionUtility::SetTeamNum(mHandles[184], 1);
    MissionUtility::SetTeamNum(mHandles[185], 1);
    MissionUtility::SetTeamNum(mHandles[186], 1);
    MissionUtility::SetTeamNum(mHandles[187], 1);
    MissionUtility::SetAttackRange(mHandles[186], 100);
    MissionUtility::SetAttackRange(mHandles[187], 100);
    MissionUtility::SetFiringRange(mHandles[186], 100);
    MissionUtility::SetFiringRange(mHandles[187], 100);
    MissionUtility::SetCurHealth(mHandles[183], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[184], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[185], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[186], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[187], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[183], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[184], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[185], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[186], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[187], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[188], 999999.0f);
    MissionUtility::SetVelocForward(mHandles[183], 13.0f);
    mHandles[189] = MissionUtility::CreateFlock(mHandles[183], (Formation)8);
    MissionUtility::AddFlockMember(mHandles[189], mHandles[184], Vector(40.0f, 0.0f, 15.0f));
    MissionUtility::AddFlockMember(mHandles[189], mHandles[185], Vector(-40.0f, 0.0f, 15.0f));
    MissionUtility::AddFlockMember(mHandles[189], mHandles[186], Vector(-40.0f, 0.0f, 145.0f));
    MissionUtility::AddFlockMember(mHandles[189], mHandles[187], Vector(40.0f, 0.0f, 145.0f));
    MissionUtility::Objectify(mHandles[183], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[184], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[185], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[186], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[187], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Goto(mHandles[189], "rconvoypath", false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[189]);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::AddHealthBar(mHandles[183], "", 400.0f);
    MissionUtility::AddHealthBar(mHandles[184], "", 400.0f);
    MissionUtility::AddHealthBar(mHandles[185], "", 400.0f);
    MissionUtility::AddHealthBar(mHandles[186], "", 400.0f);
    MissionUtility::AddHealthBar(mHandles[187], "", 400.0f);
    MissionUtility::SetTeamNum(mHandles[170], 1);
    MissionUtility::SetTeamNum(mHandles[171], 1);
    MissionUtility::SetTeamNum(mHandles[172], 1);
    MissionUtility::SetTeamNum(mHandles[173], 1);
    MissionUtility::SetTeamNum(mHandles[174], 1);
    MissionUtility::SetAttackRange(mHandles[173], 100);
    MissionUtility::SetAttackRange(mHandles[174], 100);
    MissionUtility::SetFiringRange(mHandles[173], 100);
    MissionUtility::SetFiringRange(mHandles[174], 100);
    MissionUtility::SetCurHealth(mHandles[170], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[171], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[172], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[173], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[174], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[170], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[171], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[172], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[173], 4100.0f);
    MissionUtility::SetMaxHealth(mHandles[174], 4100.0f);
    MissionUtility::SetCurHealth(mHandles[175], 999999.0f);
    MissionUtility::SetVelocForward(mHandles[170], 13.0f);
    MissionUtility::AddHealthBar(mHandles[170], "", 400.0f);
    MissionUtility::AddHealthBar(mHandles[171], "", 400.0f);
    MissionUtility::AddHealthBar(mHandles[172], "", 400.0f);
    MissionUtility::AddHealthBar(mHandles[173], "", 400.0f);
    MissionUtility::AddHealthBar(mHandles[174], "", 400.0f);
    mHandles[176] = MissionUtility::CreateFlock(mHandles[170], (Formation)8);
    MissionUtility::AddFlockMember(mHandles[176], mHandles[171], Vector(40.0f, 0.0f, 15.0f));
    MissionUtility::AddFlockMember(mHandles[176], mHandles[172], Vector(-40.0f, 0.0f, 15.0f));
    MissionUtility::AddFlockMember(mHandles[176], mHandles[173], Vector(-40.0f, 0.0f, 145.0f));
    MissionUtility::AddFlockMember(mHandles[176], mHandles[174], Vector(40.0f, 0.0f, 145.0f));
    MissionUtility::Objectify(mHandles[170], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[171], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[172], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[173], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[174], "missions.Ossus3.marker.str0007", false, true, 0.0f);
    MissionUtility::Goto(mHandles[176], "lconvoypath", false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[176]);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Patrol(mHandles[212], "rattackpath", 100.0f, false);
    MissionUtility::Patrol(mHandles[216], "rattackpath", 100.0f, false);
    MissionUtility::Patrol(mHandles[220], "rattackpath", 100.0f, false);
    MissionUtility::Patrol(mHandles[224], "rattackpath", 100.0f, false);
    MissionUtility::Patrol(mHandles[196], "lattackpath", 100.0f, false);
    MissionUtility::Patrol(mHandles[200], "lattackpath", 100.0f, false);
    MissionUtility::Patrol(mHandles[204], "lattackpath", 100.0f, false);
    MissionUtility::Patrol(mHandles[208], "lattackpath", 100.0f, false);
    ResumeTimer(mTimer16);
    mPhase = 12;
    mFlags[54] = true;

        break;
    case 18:
    // ---- +0x4ac8  652 bytes ----
    if (!mFlags[7]) {
        StopTimer(mTimer16);
        MissionUtility::MoveObjectWithRotation(mHandles[4], "midcin1moveplayer1", 0, true);
        MissionUtility::SetTeamNum(mHandles[4], 0);
        MissionUtility::SetAsPlayer(mHandles[5], 0);
        MissionUtility::SetVelocMinimumFly(mHandles[6], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[6], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[6], 0.0f);
        MissionUtility::QueueSound("OBR17_04Q", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObject(mHandles[188]);
        MissionUtility::RemoveObject(mHandles[175]);
        mHandles[192] = MissionUtility::CreateObject("REP_tank_beam", Vector(2026.8297f, -328.11133f, 2068.1616f), "rtempbeam", 9, -1, Quat(0.581453f, 0.0f, -0.813582f, 0.0f), -1);
        MissionUtility::AddHealthBar(mHandles[192], "", 400.0f);
        MissionUtility::Objectify(mHandles[192], "missions.Ossus3.marker.str0007", false, true, 0.0f);
        mHandles[191] = MissionUtility::CreateObject("REP_tank_beam", Vector(947.43585f, -351.87793f, 2294.5383f), "ltempbeam", 9, -1, Quat(-0.453706f, 0.0f, -0.891152f, 0.0f), -1);
        MissionUtility::AddHealthBar(mHandles[191], "", 400.0f);
        MissionUtility::Objectify(mHandles[191], "missions.Ossus3.marker.str0007", false, true, 0.0f);
        MissionUtility::SetTeamNum(mHandles[157], 6);
        MissionUtility::SetCurHealth(mHandles[157], 7500.0f);
        MissionUtility::SetTeamNum(mHandles[183], 0);
        MissionUtility::SetTeamNum(mHandles[184], 0);
        MissionUtility::SetTeamNum(mHandles[185], 0);
        MissionUtility::SetTeamNum(mHandles[186], 0);
        MissionUtility::SetTeamNum(mHandles[187], 0);
        MissionUtility::SetTeamNum(mHandles[170], 0);
        MissionUtility::SetTeamNum(mHandles[171], 0);
        MissionUtility::SetTeamNum(mHandles[172], 0);
        MissionUtility::SetTeamNum(mHandles[173], 0);
        MissionUtility::SetTeamNum(mHandles[174], 0);
        mHandles[1] = MissionUtility::RunCin("maccin", true, true);
        mInts[2] = MissionUtility::GetFlockCount(mHandles[177]) + MissionUtility::GetFlockCount(mHandles[190]);
        mFlags[7] = true;
    }

    // ---- +0x4d54  128 bytes ----
    if (mFlags[7]) {
    if (!MissionUtility::IsCinRunning(mHandles[1])) {
    MissionUtility::MoveObjectWithRotation(mHandles[6], "midcin1moveplayer2", 0, true);
    MissionUtility::SetVelocMinimumFly(mHandles[6], 70.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[6], 140.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[6], 220.0f);
    MissionUtility::SetTeamNum(mHandles[6], 1);
    MissionUtility::SetAsPlayer(mHandles[6], 0);
    if (mFlags[49]) {
    MissionUtility::RemoveObject(mHandles[192]);
    }

    // ---- +0x4dd4  20 bytes ----
    if (mFlags[50]) {
        MissionUtility::RemoveObject(mHandles[191]);
    }

    // ---- +0x4de8  140 bytes ----
    MissionUtility::SetTeamNum(mHandles[183], 1);
    MissionUtility::SetTeamNum(mHandles[184], 1);
    MissionUtility::SetTeamNum(mHandles[185], 1);
    MissionUtility::SetTeamNum(mHandles[186], 1);
    MissionUtility::SetTeamNum(mHandles[187], 1);
    MissionUtility::SetTeamNum(mHandles[170], 1);
    MissionUtility::SetTeamNum(mHandles[171], 1);
    MissionUtility::SetTeamNum(mHandles[172], 1);
    MissionUtility::SetTeamNum(mHandles[173], 1);
    MissionUtility::SetTeamNum(mHandles[174], 1);
    ResumeTimer(mTimer16);
    mPhase = 12;

        break;
    case 12:
    // ---- +0x4e74  96 bytes ----
    MissionUtility::ExcludeObject(mHandles[161], mHandles[4]);
    MissionUtility::ExcludeObject(mHandles[161], mHandles[157]);
    MissionUtility::ExcludeObject(mHandles[161], mHandles[176]);
    MissionUtility::ExcludeObject(mHandles[161], mHandles[189]);
    MissionUtility::ExcludeObject(mHandles[161], mHandles[158]);
    int i;
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[161]); i++)
        MissionUtility::DamageObject(MissionUtility::GetRegionNewMember(mHandles[161], i), 999999.0f, 999999.0f);

    // ---- +0x4ed4  112 bytes ----
    MissionUtility::ExcludeObject(mHandles[162], mHandles[4]);
    MissionUtility::ExcludeObject(mHandles[162], mHandles[157]);
    MissionUtility::ExcludeObject(mHandles[162], mHandles[176]);
    MissionUtility::ExcludeObject(mHandles[162], mHandles[189]);
    MissionUtility::ExcludeObject(mHandles[162], mHandles[159]);
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[162]); i++)
        MissionUtility::DamageObject(MissionUtility::GetRegionNewMember(mHandles[162], i), 999999.0f, 999999.0f);

    // ---- +0x4f44  112 bytes ----
    MissionUtility::ExcludeObject(mHandles[163], mHandles[4]);
    MissionUtility::ExcludeObject(mHandles[163], mHandles[157]);
    MissionUtility::ExcludeObject(mHandles[163], mHandles[176]);
    MissionUtility::ExcludeObject(mHandles[163], mHandles[189]);
    MissionUtility::ExcludeObject(mHandles[163], mHandles[160]);
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[163]); i++)
        MissionUtility::DamageObject(MissionUtility::GetRegionNewMember(mHandles[163], i), 999999.0f, 999999.0f);

    // ---- +0x4fb4  100 bytes ----
    MissionUtility::ExcludeObject(mHandles[164], mHandles[4]);
    MissionUtility::ExcludeObject(mHandles[164], mHandles[157]);
    MissionUtility::ExcludeObject(mHandles[164], mHandles[176]);
    MissionUtility::ExcludeObject(mHandles[164], mHandles[189]);
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[164]); i++)
        MissionUtility::DamageObject(MissionUtility::GetRegionNewMember(mHandles[164], i), 999999.0f, 999999.0f);

    // ---- +0x5018  84 bytes ----
    if (!mFlags[41]) {
    if (MissionUtility::IsInsideRegion(mHandles[4], "InsideTempleRegion")) {
    MissionUtility::AddTurnAroundRegion("basetrigger", "cisflyerpath1", 0, 0, 0, 0);
    mFlags[41] = true;
    }
    }

    // ---- +0x506c  212 bytes ----
    if (!mFlags[42]) {
        if (!MissionUtility::IsCinRunning(mHandles[147])) {
        MissionUtility::PlayMusic("EP4_V1_T03_02", true);
        MissionUtility::RemoveObject(mHandles[12]);
        MissionUtility::RemoveObject(mHandles[13]);
        MissionUtility::RemoveObject(mHandles[14]);
        MissionUtility::SetCurHealth(mHandles[6], 3500.0f);
        MissionUtility::SetCurShield(mHandles[6], 500.0f);
        MissionUtility::MoveObjectWithRotation(mHandles[6], "midcin1moveplayer2", 0, true);
        MissionUtility::SetVelocMinimumFly(mHandles[6], 70.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[6], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[6], 220.0f);
        MissionUtility::SetTeamNum(mHandles[6], 1);
        MissionUtility::SetAsPlayer(mHandles[6], 0);
        mHandles[148] = MissionUtility::AddObjective("missions.Ossus3.objective.str0016");
        MissionUtility::ObjectiveComplete(mHandles[48]);
        MissionUtility::DisplayText("missions.Ossus3.text.str0018", 10.0f, -1.0f);
        mFlags[42] = true;
        }
    }

    // ---- +0x5140  60 bytes ----
    if (!mFlags[44]) {
        if (!MissionUtility::IsAlive(mHandles[185])) {
        MissionUtility::SetAttackRange(mHandles[186], 450);
        MissionUtility::SetFiringRange(mHandles[186], 450);
        mFlags[44] = true;
        }
    }

    // ---- +0x517c  60 bytes ----
    if (!mFlags[43]) {
        if (!MissionUtility::IsAlive(mHandles[184])) {
        MissionUtility::SetAttackRange(mHandles[187], 450);
        MissionUtility::SetFiringRange(mHandles[187], 450);
        mFlags[43] = true;
        }
    }

    // ---- +0x51b8  60 bytes ----
    if (!mFlags[47]) {
        if (!MissionUtility::IsAlive(mHandles[172])) {
        MissionUtility::SetAttackRange(mHandles[173], 450);
        MissionUtility::SetFiringRange(mHandles[173], 450);
        mFlags[47] = true;
        }
    }

    // ---- +0x51f4  60 bytes ----
    if (!mFlags[46]) {
        if (!MissionUtility::IsAlive(mHandles[171])) {
        MissionUtility::SetAttackRange(mHandles[174], 450);
        MissionUtility::SetFiringRange(mHandles[174], 450);
        mFlags[46] = true;
        }
    }

    // ---- +0x5230  240 bytes ----
    if (!mFlags[7]) {
        if (MissionUtility::GetFlockCount(mHandles[176]) + MissionUtility::GetFlockCount(mHandles[189]) < mInts[2]) {
        mFlags[38] = true;
        }
        if (mFlags[38]) {
        mInts[3] = MissionUtility::GetRandomInt(1, 2);
        if (mInts[3] == 1) {
        MissionUtility::QueueSound("CTR06_09", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        if (mInts[3] == 2) {
        MissionUtility::QueueSound("CTINT_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        MissionUtility::QueueSound("CTR10_17", 1.0f, 0.0f, 0.0f, "", 0, "");
        mInts[2] = MissionUtility::GetFlockCount(mHandles[189]) + MissionUtility::GetFlockCount(mHandles[176]);
        mFlags[38] = false;
        }
    }

    // ---- +0x5320  240 bytes ----
    if (mFlags[7]) {
        if (MissionUtility::GetFlockCount(mHandles[177]) + MissionUtility::GetFlockCount(mHandles[190]) < mInts[2]) {
        mFlags[38] = true;
        }
        if (mFlags[38]) {
        mInts[3] = MissionUtility::GetRandomInt(1, 2);
        if (mInts[3] == 1) {
        MissionUtility::QueueSound("CTR06_09", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        if (mInts[3] == 2) {
        MissionUtility::QueueSound("CTINT_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        MissionUtility::QueueSound("CTR10_17", 1.0f, 0.0f, 0.0f, "", 0, "");
        mInts[2] = MissionUtility::GetFlockCount(mHandles[190]) + MissionUtility::GetFlockCount(mHandles[177]);
        mFlags[38] = false;
        }
    }

    // ---- +0x5410  112 bytes ----
    if (!mFlags[50] && !mFlags[7]) {
        if (!MissionUtility::IsAlive(mHandles[170])) {
        if (!MissionUtility::IsAlive(mHandles[172])) {
        if (!MissionUtility::IsAlive(mHandles[173])) {
        if (!MissionUtility::IsAlive(mHandles[174])) {
        if (!MissionUtility::IsAlive(mHandles[174])) {
        mFlags[50] = true;
        }
        }
        }
        }
        }
    }

    // ---- +0x5480  112 bytes ----
    if (!mFlags[49] && !mFlags[7]) {
        if (!MissionUtility::IsAlive(mHandles[183])) {
        if (!MissionUtility::IsAlive(mHandles[185])) {
        if (!MissionUtility::IsAlive(mHandles[186])) {
        if (!MissionUtility::IsAlive(mHandles[187])) {
        if (!MissionUtility::IsAlive(mHandles[187])) {
        mFlags[49] = true;
        }
        }
        }
        }
        }
    }

    // ---- +0x54f0  32 bytes ----
    if (mFlags[49]
        || mFlags[50]) {
    mPhase = 11;
    }

    // ---- +0x5510  124 bytes ----
    if (!mFlags[48] && mFlags[7]) {
        if (!MissionUtility::IsAlive(mHandles[170])) {
        if (!MissionUtility::IsAlive(mHandles[172])) {
        if (!MissionUtility::IsAlive(mHandles[173])) {
        if (!MissionUtility::IsAlive(mHandles[174])) {
        if (!MissionUtility::IsAlive(mHandles[174])) {
        MissionUtility::SetTeamNum(mHandles[191], 5);
        mFlags[48] = true;
        }
        }
        }
        }
        }
    }

    // ---- +0x558c  124 bytes ----
    if (!mFlags[45] && mFlags[7]) {
        if (!MissionUtility::IsAlive(mHandles[183])) {
        if (!MissionUtility::IsAlive(mHandles[185])) {
        if (!MissionUtility::IsAlive(mHandles[186])) {
        if (!MissionUtility::IsAlive(mHandles[187])) {
        if (!MissionUtility::IsAlive(mHandles[187])) {
        MissionUtility::SetTeamNum(mHandles[192], 5);
        mFlags[45] = true;
        }
        }
        }
        }
        }
    }

    // ---- +0x5608  208 bytes ----
    if (!mFlags[56]) {
        if (MissionUtility::IsInsideRegion(mHandles[176], "lconvoystop")) {
        MissionUtility::DisbandFlockMember(mHandles[176], mHandles[170]);
        MissionUtility::DisbandFlockMember(mHandles[176], mHandles[171]);
        MissionUtility::DisbandFlockMember(mHandles[176], mHandles[172]);
        MissionUtility::DisbandFlockMember(mHandles[176], mHandles[173]);
        MissionUtility::DisbandFlockMember(mHandles[176], mHandles[174]);
        MissionUtility::Stop(mHandles[170]);
        MissionUtility::Stop(mHandles[171]);
        MissionUtility::Stop(mHandles[172]);
        MissionUtility::Stop(mHandles[173]);
        MissionUtility::Stop(mHandles[174]);
        mHandles[177] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[177], mHandles[170]);
        MissionUtility::AddFlockMember(mHandles[177], mHandles[171]);
        MissionUtility::AddFlockMember(mHandles[177], mHandles[172]);
        MissionUtility::AddFlockMember(mHandles[177], mHandles[173]);
        MissionUtility::AddFlockMember(mHandles[177], mHandles[174]);
        mFlags[56] = true;
        }
    }

    // ---- +0x56d8  208 bytes ----
    if (!mFlags[57]) {
        if (MissionUtility::IsInsideRegion(mHandles[189], "rconvoystop")) {
        MissionUtility::DisbandFlockMember(mHandles[189], mHandles[183]);
        MissionUtility::DisbandFlockMember(mHandles[189], mHandles[184]);
        MissionUtility::DisbandFlockMember(mHandles[189], mHandles[185]);
        MissionUtility::DisbandFlockMember(mHandles[189], mHandles[186]);
        MissionUtility::DisbandFlockMember(mHandles[189], mHandles[187]);
        MissionUtility::Stop(mHandles[183]);
        MissionUtility::Stop(mHandles[184]);
        MissionUtility::Stop(mHandles[185]);
        MissionUtility::Stop(mHandles[186]);
        MissionUtility::Stop(mHandles[187]);
        mHandles[190] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[190], mHandles[183]);
        MissionUtility::AddFlockMember(mHandles[190], mHandles[184]);
        MissionUtility::AddFlockMember(mHandles[190], mHandles[185]);
        MissionUtility::AddFlockMember(mHandles[190], mHandles[186]);
        MissionUtility::AddFlockMember(mHandles[190], mHandles[187]);
        mFlags[57] = true;
        }
    }

    // ---- +0x57a8  160 bytes ----
    if (!mFlags[51]) {
        if (!MissionUtility::IsAlive(mHandles[192])) {
        if (mFlags[7]) {
        mInts[3] = MissionUtility::GetRandomInt(1, 2);
        if (mInts[3] == 1) {
        MissionUtility::StartSound("CTR06_09", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        if (mInts[3] == 2) {
        MissionUtility::StartSound("CTINT_21", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        mFlags[51] = true;
        }
        }
    }

    // ---- +0x5848  160 bytes ----
    if (!mFlags[52]) {
        if (!MissionUtility::IsAlive(mHandles[191])) {
        if (mFlags[7]) {
        mInts[3] = MissionUtility::GetRandomInt(1, 2);
        if (mInts[3] == 1) {
        MissionUtility::StartSound("CTR06_09", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        if (mInts[3] == 2) {
        MissionUtility::StartSound("CTINT_21", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        mFlags[52] = true;
        }
        }
    }

    // ---- +0x58e8  100 bytes ----
    if (!mFlags[53]) {
        if (mFlags[51]
            || mFlags[52]) {
        MissionUtility::DisplayText("missions.Ossus3.text.str0021", 5.0f, -1.0f);
        MissionUtility::QueueSound("CTG04_37", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::BonusObjectiveFailed(mHandles[9]);
        mFlags[53] = true;
        }
    }

    // ---- +0x594c  32 bytes ----
    if (mFlags[51] && mFlags[52]) {
        mPhase = 11;
    }

    // ---- +0x596c  68 bytes ----
    if (!mFlags[55] && mFlags[54] && mFlags[57] && mFlags[56]) {
        MissionUtility::FlushSoundQueue();
        mFlags[55] = true;
        mPhase = 18;
    }

    // ---- +0x59b0  3472 bytes ----
    if (!mFlags[75] && mFlags[54]) {
        if (MissionUtility::IsAlive(mHandles[157])) {
        if (!MissionUtility::IsFlockAlive(mHandles[196])) {
        if (!MissionUtility::IsInsideRegion(mHandles[200], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[204], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[208], "lshipspawnarea")) {
        mInts[0] = mInts[0] + 1;
        mHandles[193] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 1, "lassaulta1", 2, -1);
        mHandles[194] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 0, "lassaulta2", 2, -1);
        mHandles[195] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 2, "lassaulta3", 2, -1);
        MissionUtility::SetVelocForward(mHandles[193], 40.0f);
        mHandles[196] = MissionUtility::CreateFlock(mHandles[193], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[196], mHandles[194]);
        MissionUtility::AddFlockMember(mHandles[196], mHandles[195]);
        MissionUtility::Goto(mHandles[196], "lspawngoto", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[196], "lattackpath", 50.0f, true);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
        }
        if (!MissionUtility::IsFlockAlive(mHandles[200])) {
        if (!MissionUtility::IsInsideRegion(mHandles[196], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[204], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[208], "lshipspawnarea")) {
        mInts[0] = mInts[0] + 1;
        mHandles[197] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 1, "lassaultb1", 2, -1);
        mHandles[198] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 0, "lassaultb2", 2, -1);
        mHandles[199] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 2, "lassaultb3", 2, -1);
        MissionUtility::SetVelocForward(mHandles[197], 40.0f);
        mHandles[200] = MissionUtility::CreateFlock(mHandles[197], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[200], mHandles[198]);
        MissionUtility::AddFlockMember(mHandles[200], mHandles[199]);
        MissionUtility::Goto(mHandles[200], "lspawngoto", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[200], "lattackpath", 50.0f, true);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
        }
        if (!MissionUtility::IsFlockAlive(mHandles[204])) {
        if (!MissionUtility::IsInsideRegion(mHandles[196], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[200], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[208], "lshipspawnarea")) {
        mInts[0] = mInts[0] + 1;
        mHandles[201] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 1, "lassaultc1", 2, -1);
        mHandles[202] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 0, "lassaultc2", 2, -1);
        mHandles[203] = MissionUtility::CreateObject("CIS_tank_assault", "lspawnpath", 2, "lassaultc3", 2, -1);
        MissionUtility::SetVelocForward(mHandles[201], 40.0f);
        mHandles[204] = MissionUtility::CreateFlock(mHandles[201], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[204], mHandles[202]);
        MissionUtility::AddFlockMember(mHandles[204], mHandles[203]);
        MissionUtility::Goto(mHandles[204], "lspawngoto", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[204], "lattackpath", 50.0f, true);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
        }
        if (!MissionUtility::IsFlockAlive(mHandles[208])) {
        if (!MissionUtility::IsInsideRegion(mHandles[196], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[200], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[204], "lshipspawnarea")) {
        mInts[0] = mInts[0] + 1;
        mHandles[205] = MissionUtility::CreateObject("CIS_tank_wheeled_ossus3", "newwheelspawnpath", 1, "lassaultd1", 2, -1);
        mHandles[206] = MissionUtility::CreateObject("CIS_tank_wheeled_ossus3", "newwheelspawnpath", 0, "lassaultd2", 2, -1);
        mHandles[207] = MissionUtility::CreateObject("CIS_tank_wheeled_ossus3", "newwheelspawnpath", 2, "lassaultd3", 2, -1);
        mHandles[208] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[208], mHandles[205]);
        MissionUtility::AddFlockMember(mHandles[208], mHandles[206]);
        MissionUtility::AddFlockMember(mHandles[208], mHandles[207]);
        MissionUtility::Goto(mHandles[205], "newwheelpath1", true);
        MissionUtility::Goto(mHandles[206], "newwheelpath0", true);
        MissionUtility::Goto(mHandles[207], "newwheelpath2", true);
        }
        }
        }
        }
        if (!MissionUtility::IsFlockAlive(mHandles[212])) {
        if (!MissionUtility::IsInsideRegion(mHandles[216], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[220], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[224], "lshipspawnarea")) {
        mInts[0] = mInts[0] + 1;
        mHandles[209] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 1, "rassaulta1", 2, -1);
        mHandles[210] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 0, "rassaulta2", 2, -1);
        mHandles[211] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 2, "rassaulta3", 2, -1);
        MissionUtility::SetVelocForward(mHandles[209], 40.0f);
        mHandles[212] = MissionUtility::CreateFlock(mHandles[209], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[212], mHandles[210]);
        MissionUtility::AddFlockMember(mHandles[212], mHandles[211]);
        MissionUtility::Goto(mHandles[212], "rspawngoto", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[212], "rattackpath", 50.0f, true);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
        }
        if (!MissionUtility::IsFlockAlive(mHandles[216])) {
        if (!MissionUtility::IsInsideRegion(mHandles[212], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[220], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[224], "lshipspawnarea")) {
        mInts[0] = mInts[0] + 1;
        mHandles[213] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 1, "rassaultb1", 2, -1);
        mHandles[214] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 0, "rassaultb2", 2, -1);
        mHandles[215] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 2, "rassaultb3", 2, -1);
        MissionUtility::SetVelocForward(mHandles[213], 40.0f);
        mHandles[216] = MissionUtility::CreateFlock(mHandles[213], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[216], mHandles[214]);
        MissionUtility::AddFlockMember(mHandles[216], mHandles[215]);
        MissionUtility::Goto(mHandles[216], "rspawngoto", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[216], "rattackpath", 50.0f, true);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
        }
        if (!MissionUtility::IsFlockAlive(mHandles[220])) {
        if (!MissionUtility::IsInsideRegion(mHandles[212], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[216], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[224], "lshipspawnarea")) {
        mInts[0] = mInts[0] + 1;
        mHandles[217] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 1, "rassaultc1", 2, -1);
        mHandles[218] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 0, "rassaultc2", 2, -1);
        mHandles[219] = MissionUtility::CreateObject("CIS_tank_assault", "rspawnpath", 2, "rassaultc3", 2, -1);
        MissionUtility::SetVelocForward(mHandles[217], 40.0f);
        mHandles[220] = MissionUtility::CreateFlock(mHandles[217], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[220], mHandles[218]);
        MissionUtility::AddFlockMember(mHandles[220], mHandles[219]);
        MissionUtility::Goto(mHandles[220], "rspawngoto", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[220], "rattackpath", 50.0f, true);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
        }
        if (!MissionUtility::IsFlockAlive(mHandles[224])) {
        if (!MissionUtility::IsInsideRegion(mHandles[212], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[216], "lshipspawnarea")) {
        if (!MissionUtility::IsInsideRegion(mHandles[220], "lshipspawnarea")) {
        mInts[0] = mInts[0] + 1;
        mHandles[221] = MissionUtility::CreateObject("CIS_tank_wheeled_ossus3", "newwheelspawnpath", 1, "rassaultd1", 2, -1);
        mHandles[222] = MissionUtility::CreateObject("CIS_tank_wheeled_ossus3", "newwheelspawnpath", 0, "rassaultd2", 2, -1);
        mHandles[223] = MissionUtility::CreateObject("CIS_tank_wheeled_ossus3", "newwheelspawnpath", 2, "rassaultd3", 2, -1);
        mHandles[224] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[224], mHandles[221]);
        MissionUtility::AddFlockMember(mHandles[224], mHandles[222]);
        MissionUtility::AddFlockMember(mHandles[224], mHandles[223]);
        MissionUtility::Goto(mHandles[221], "newwheelpath1", true);
        MissionUtility::Goto(mHandles[222], "newwheelpath0", true);
        MissionUtility::Goto(mHandles[223], "newwheelpath2", true);
        }
        }
        }
        }
        }
    }

    // ---- +0x6740  644 bytes ----
    if (!mFlags[76] && mFlags[54]) {
        if (mInts[0] > 6) {
        MissionUtility::QueueSound("OBR17_04N", 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[225] = MissionUtility::CreateObjectWithRotation("CIS_fly_vcarrier", "vcarrierbasepath2", 0, "vcarrierbase1", 2, -1, -1);
        mHandles[234] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnleft", 0, "rforcetankl1", 2, -1);
        mHandles[235] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnleft", 1, "rforcetankl2", 2, -1);
        mHandles[236] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnleft", 1, "rforcetankl3", 2, -1);
        MissionUtility::CarrierAddCargo(mHandles[225], "hp_link_1", mHandles[234], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[225], "hp_link_11", mHandles[235], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[225], "hp_link_18", mHandles[236], "hp_link_1", true);
        MissionUtility::SetVelocMaximumFly(mHandles[225], 100.0f);
        MissionUtility::SetAttackRange(mHandles[225], 1);
        MissionUtility::SetMaxAltitude(mHandles[225], 999999.0f);
        MissionUtility::Goto(mHandles[225], "vcarrierbasepath2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[225], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[225], "vcarrierbasepath1", 3);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[225], "vcarrierbasepath1", 2);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[225], "vcarrierbasepath1", 1);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Land(mHandles[225], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        mFlags[76] = true;
        }
    }

    // ---- +0x69c4  128 bytes ----
    if (!mFlags[77]) {
        if (MissionUtility::IsDropped(mHandles[234])) {
        if (MissionUtility::IsDropped(mHandles[235])) {
        if (MissionUtility::IsDropped(mHandles[236])) {
        mHandles[237] = MissionUtility::CreateFlock(mHandles[234], (Formation)4);
        MissionUtility::AddFlockMember(mHandles[237], mHandles[235]);
        MissionUtility::AddFlockMember(mHandles[237], mHandles[236]);
        MissionUtility::Patrol(mHandles[237], "rforcepathleft", 150.0f, false);
        mFlags[77] = true;
        }
        }
        }
    }

    // ---- +0x6a44  56 bytes ----
    if (!mFlags[78]) {
        if (MissionUtility::IsInsideRegion(mHandles[225], "vcarrierremove")) {
        MissionUtility::RemoveObject(mHandles[225]);
        MissionUtility::BonusObjectiveFailed(mHandles[7]);
        mFlags[78] = true;
        }
    }

    // ---- +0x6a7c  656 bytes ----
    if (!mFlags[82] && mFlags[54] && mFlags[76]) {
        mHandles[227] = MissionUtility::CreateObject("CIS_fly_vcarrier", "vcarrierbasepath3", 0, "vcarrierbase3", 2, -1);
        mHandles[245] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnright", 0, "rforcetankr1", 2, -1);
        mHandles[246] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnright", 1, "rforcetankr2", 2, -1);
        mHandles[247] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnright", 1, "rforcetankr3", 2, -1);
        MissionUtility::CarrierAddCargo(mHandles[227], "hp_link_1", mHandles[245], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[227], "hp_link_11", mHandles[246], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[227], "hp_link_18", mHandles[247], "hp_link_1", true);
        MissionUtility::SetAttackRange(mHandles[227], 1);
        MissionUtility::SetVelocMaximumFly(mHandles[227], 100.0f);
        MissionUtility::SetMaxAltitude(mHandles[226], 999999.0f);
        MissionUtility::Goto(mHandles[227], "vcarrierbasepath3", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[227], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[227], "vcarrierbasepath1", 3);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[227], "vcarrierbasepath1", 2);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[227], "vcarrierbasepath1", 1);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Land(mHandles[227], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        mFlags[82] = true;
    }

    // ---- +0x6d0c  128 bytes ----
    if (!mFlags[83]) {
        if (MissionUtility::IsDropped(mHandles[245])) {
        if (MissionUtility::IsDropped(mHandles[246])) {
        if (MissionUtility::IsDropped(mHandles[247])) {
        mHandles[248] = MissionUtility::CreateFlock(mHandles[245], (Formation)4);
        MissionUtility::AddFlockMember(mHandles[248], mHandles[246]);
        MissionUtility::AddFlockMember(mHandles[248], mHandles[247]);
        MissionUtility::Patrol(mHandles[248], "rforcepathright", 150.0f, false);
        mFlags[83] = true;
        }
        }
        }
    }

    // ---- +0x6d8c  56 bytes ----
    if (!mFlags[84]) {
        if (MissionUtility::IsInsideRegion(mHandles[227], "vcarrierremove")) {
        MissionUtility::RemoveObject(mHandles[227]);
        MissionUtility::BonusObjectiveFailed(mHandles[7]);
        mFlags[84] = true;
        }
    }

    // ---- +0x6dc4  720 bytes ----
    if (!mFlags[79] && mFlags[54] && mFlags[76]) {
        if (MissionUtility::IsDropped(mHandles[234])
            || !MissionUtility::IsAlive(mHandles[225])) {
        if (MissionUtility::IsDropped(mHandles[245])
            || !MissionUtility::IsAlive(mHandles[227])) {
        mHandles[226] = MissionUtility::CreateObject("CIS_fly_vcarrier", "vcarrierbasepath2", 0, "vcarrierbase2", 2, -1);
        mHandles[238] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnleft", 0, "rforcetankl4", 2, -1);
        mHandles[239] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnleft", 1, "rforcetankl5", 2, -1);
        mHandles[240] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnleft", 1, "rforcetankl6", 2, -1);
        MissionUtility::CarrierAddCargo(mHandles[226], "hp_link_1", mHandles[238], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[226], "hp_link_11", mHandles[239], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[226], "hp_link_18", mHandles[240], "hp_link_1", true);
        MissionUtility::SetVelocMaximumFly(mHandles[226], 100.0f);
        MissionUtility::SetAttackRange(mHandles[226], 1);
        MissionUtility::SetMaxAltitude(mHandles[226], 999999.0f);
        MissionUtility::Goto(mHandles[226], "vcarrierbasepath2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[226], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[226], "vcarrierbasepath1", 3);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[226], "vcarrierbasepath1", 2);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[226], "vcarrierbasepath1", 1);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Land(mHandles[226], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        mFlags[79] = true;
        }
        }
    }

    // ---- +0x7094  128 bytes ----
    if (!mFlags[80]) {
        if (MissionUtility::IsDropped(mHandles[238])) {
        if (MissionUtility::IsDropped(mHandles[239])) {
        if (MissionUtility::IsDropped(mHandles[240])) {
        mHandles[241] = MissionUtility::CreateFlock(mHandles[238], (Formation)4);
        MissionUtility::AddFlockMember(mHandles[241], mHandles[239]);
        MissionUtility::AddFlockMember(mHandles[241], mHandles[240]);
        MissionUtility::Patrol(mHandles[241], "rforcepathleft", 150.0f, false);
        mFlags[80] = true;
        }
        }
        }
    }

    // ---- +0x7114  56 bytes ----
    if (!mFlags[81]) {
        if (MissionUtility::IsInsideRegion(mHandles[226], "vcarrierremove")) {
        MissionUtility::RemoveObject(mHandles[226]);
        MissionUtility::BonusObjectiveFailed(mHandles[7]);
        mFlags[81] = true;
        }
    }

    // ---- +0x714c  720 bytes ----
    if (!mFlags[85] && mFlags[54] && mFlags[76]) {
        if (MissionUtility::IsDropped(mHandles[234])
            || !MissionUtility::IsAlive(mHandles[225])) {
        if (MissionUtility::IsDropped(mHandles[245])
            || !MissionUtility::IsAlive(mHandles[227])) {
        mHandles[228] = MissionUtility::CreateObject("CIS_fly_vcarrier", "vcarrierbasepath3", 0, "vcarrierbase4", 2, -1);
        mHandles[249] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnright", 0, "rforcetankr4", 2, -1);
        mHandles[250] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnright", 1, "rforcetankr5", 2, -1);
        mHandles[251] = MissionUtility::CreateObject("CIS_tank_fighter", "rforcespawnright", 1, "rforcetankr6", 2, -1);
        MissionUtility::CarrierAddCargo(mHandles[228], "hp_link_1", mHandles[249], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[228], "hp_link_11", mHandles[250], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[228], "hp_link_18", mHandles[251], "hp_link_1", true);
        MissionUtility::SetAttackRange(mHandles[228], 1);
        MissionUtility::SetVelocMaximumFly(mHandles[228], 100.0f);
        MissionUtility::SetMaxAltitude(mHandles[228], 999999.0f);
        MissionUtility::Goto(mHandles[228], "vcarrierbasepath3", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[228], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[228], "vcarrierbasepath1", 3);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[228], "vcarrierbasepath1", 2);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[228], "vcarrierbasepath1", 1);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Land(mHandles[228], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        mFlags[85] = true;
        }
        }
    }

    // ---- +0x741c  128 bytes ----
    if (!mFlags[86]) {
        if (MissionUtility::IsDropped(mHandles[249])) {
        if (MissionUtility::IsDropped(mHandles[250])) {
        if (MissionUtility::IsDropped(mHandles[251])) {
        mHandles[252] = MissionUtility::CreateFlock(mHandles[249], (Formation)4);
        MissionUtility::AddFlockMember(mHandles[252], mHandles[250]);
        MissionUtility::AddFlockMember(mHandles[252], mHandles[251]);
        MissionUtility::Patrol(mHandles[252], "rforcepathright", 150.0f, false);
        mFlags[86] = true;
        }
        }
        }
    }

    // ---- +0x749c  56 bytes ----
    if (!mFlags[87]) {
        if (MissionUtility::IsInsideRegion(mHandles[228], "vcarrierremove")) {
        MissionUtility::RemoveObject(mHandles[228]);
        MissionUtility::BonusObjectiveFailed(mHandles[7]);
        mFlags[87] = true;
        }
    }

    // ---- +0x74d4  204 bytes ----
    if (!mFlags[21] && !mFlags[22] && mFlags[76] && mFlags[79] && mFlags[82] && mFlags[85]) {
        if (!MissionUtility::IsAlive(mHandles[225])) {
        if (!MissionUtility::IsAlive(mHandles[226])) {
        if (!MissionUtility::IsAlive(mHandles[227])) {
        if (!MissionUtility::IsAlive(mHandles[228])) {
        if (!mFlags[78]) {
        if (!mFlags[81]) {
        if (!mFlags[84]) {
        if (!mFlags[87]) {
        MissionUtility::BonusObjectiveComplete(mHandles[7], true);
        mFlags[21] = true;
        }
        }
        }
        }
        }
        }
        }
        }
    }

    // ---- +0x75a0  64 bytes ----
    if (!mFlags[58]) {
        if (!MissionUtility::IsAlive(mHandles[158])) {
        MissionUtility::StartSoundAtObject("C_TechnoUnion_fall01", mHandles[158], false, 400.0f);
        BeginTimer(mTimer5);
        mFlags[58] = true;
        }
    }

    // ---- +0x75e0  40 bytes ----
    if (!mFlags[60] && mFlags[59]) {
        MissionUtility::DestroyRegionList(mHandles[161]);
        mFlags[60] = true;
    }

    // ---- +0x7608  60 bytes ----
    if (!mFlags[59]) {
        if (mTimer5 > 1.0f) {
        mHandles[161] = MissionUtility::CreateRegionList("techno1area", true, false);
        mFlags[59] = true;
        }
    }

    // ---- +0x7644  64 bytes ----
    if (!mFlags[61]) {
        if (!MissionUtility::IsAlive(mHandles[159])) {
        MissionUtility::StartSoundAtObject("C_TechnoUnion_fall01", mHandles[159], false, 400.0f);
        BeginTimer(mTimer6);
        mFlags[61] = true;
        }
    }

    // ---- +0x7684  40 bytes ----
    if (!mFlags[63] && mFlags[62]) {
        MissionUtility::DestroyRegionList(mHandles[162]);
        mFlags[63] = true;
    }

    // ---- +0x76ac  60 bytes ----
    if (!mFlags[62]) {
        if (mTimer6 > 1.0f) {
        mHandles[162] = MissionUtility::CreateRegionList("techno2area", true, false);
        mFlags[62] = true;
        }
    }

    // ---- +0x76e8  64 bytes ----
    if (!mFlags[64]) {
        if (!MissionUtility::IsAlive(mHandles[160])) {
        MissionUtility::StartSoundAtObject("C_TechnoUnion_fall01", mHandles[160], false, 400.0f);
        BeginTimer(mTimer7);
        mFlags[64] = true;
        }
    }

    // ---- +0x7728  40 bytes ----
    if (!mFlags[66] && mFlags[65]) {
        MissionUtility::DestroyRegionList(mHandles[163]);
        mFlags[66] = true;
    }

    // ---- +0x7750  60 bytes ----
    if (!mFlags[65]) {
        if (mTimer7 > 1.0f) {
        mHandles[163] = MissionUtility::CreateRegionList("techno3area", true, false);
        mFlags[65] = true;
        }
    }

    // ---- +0x778c  136 bytes ----
    if (!mFlags[69]) {
        if (!MissionUtility::IsAlive(mHandles[157])) {
        MissionUtility::ObjectiveComplete(mHandles[148]);
        if (MissionUtility::IsAlive(mHandles[192]) && MissionUtility::IsAlive(mHandles[191])) {
        MissionUtility::BonusObjectiveComplete(mHandles[9], true);
        mFlags[37] = true;
        } else {
        MissionUtility::BonusObjectiveFailed(mHandles[9]);
        }
        StopTimer(mTimer16);
        MissionUtility::FlushSoundQueue();
        mPhase = 16;
        BeginTimer(mTimer8);
        mFlags[69] = true;
        }
    }

    // ---- +0x7814  84 bytes ----
    if (!mFlags[67]) {
        if (MissionUtility::GetCurHealth(mHandles[157]) < 3750.0f) {
        if (mFlags[7]) {
        MissionUtility::QueueSound("CTR17_30", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[67] = true;
        }
        }
    }

    // ---- +0x7868  88 bytes ----
    if (!mFlags[68]) {
    if (MissionUtility::GetCurHealth(mHandles[157]) < 750.0f) {
    if (mFlags[7]) {
    MissionUtility::QueueSound("CTR17_31", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[68] = true;

        break;
    case 16:
    // ---- +0x78c0  56 bytes ----
    StopTimer(mTimer16);
    MissionUtility::MidMissionSavePlayer(17);
    MissionUtility::MidMissionSave(mTimer16);
    MissionUtility::MidMissionSave((bool)mFlags[37]);
    MissionUtility::MidMissionSave((bool)mFlags[21]);
    mPhase = 9;

        break;
    case 17:
    // ---- +0x78f8  204 bytes ----
    mHandles[48] = MissionUtility::AddObjective("missions.Ossus3.objective.str0009");
    mHandles[82] = MissionUtility::AddObjective("missions.Ossus3.objective.str0015");
    mHandles[49] = MissionUtility::AddObjective("missions.Ossus3.objective.str0008");
    mHandles[128] = MissionUtility::AddObjective("missions.Ossus3.objective.str0012");
    mHandles[50] = MissionUtility::AddObjective("missions.Ossus3.objective.str0013");
    mHandles[148] = MissionUtility::AddObjective("missions.Ossus3.objective.str0016");
    MissionUtility::ObjectiveComplete(mHandles[47]);
    MissionUtility::ObjectiveComplete(mHandles[48]);
    MissionUtility::ObjectiveComplete(mHandles[82]);
    MissionUtility::ObjectiveComplete(mHandles[49]);
    MissionUtility::ObjectiveComplete(mHandles[128]);
    MissionUtility::ObjectiveComplete(mHandles[50]);
    MissionUtility::ObjectiveComplete(mHandles[148]);
    MissionUtility::MidMissionLoadPlayer();
    StopTimer(mTimer16);
    float savedTimeB;
    MissionUtility::MidMissionLoad(savedTimeB);
    mTimer16 = savedTimeB;
    MissionUtility::MidMissionLoad(mFlags[37]);
    if (mFlags[37]) {
    MissionUtility::BonusObjectiveComplete(mHandles[9], false);
    } else {
    MissionUtility::BonusObjectiveFailed(mHandles[9]);
    }

    // ---- +0x79c4  44 bytes ----
    MissionUtility::MidMissionLoad(mFlags[21]);
    if (mFlags[21]) {
    MissionUtility::BonusObjectiveComplete(mHandles[7], false);
    } else {
    MissionUtility::BonusObjectiveFailed(mHandles[7]);
    }

    // ---- +0x79f0  12 bytes ----
    mPhase = 9;

        break;
    case 11:
    // ---- +0x79fc  212 bytes ----
    if (!mFlags[130]) {
        MissionUtility::FlushSoundQueue();
        mHandles[276] = MissionUtility::RunCin("losecin", true, true);
        MissionUtility::StartSound("OBR17_32", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[130] = true;
        if (mFlags[42]) {
        mHandles[318] = MissionUtility::CreateObjectWithRotation("rep_tank_beam", "LoseCinMacPath", 0, "LoseCinMac", 0, -1, -1);
        mHandles[319] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "LoseCinWalkerPath", 0, "LoseCinWalker", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[318], true);
        MissionUtility::OverrideSoundRange(mHandles[319], true);
        MissionUtility::Goto(mHandles[319], "LoseCinWalkerPath", false);
        BeginTimer(mTimer25);
        }
    }

    // ---- +0x7ad0  56 bytes ----
    if (mTimer25 > 2.0f) {
    if (!mFlags[145]) {
    mFlags[145] = true;
    MissionUtility::DamageObject(mHandles[319], 2000.0f, 2000.0f);
    }
    }

    // ---- +0x7b08  56 bytes ----
    if (mTimer25 > 3.0f) {
    if (!mFlags[146]) {
    mFlags[146] = true;
    MissionUtility::DamageObject(mHandles[318], 12000.0f, 12000.0f);
    }
    }

    // ---- +0x7b40  44 bytes ----
    if (!mFlags[1]) {
    if (!MissionUtility::IsCinRunning(mHandles[276])) {
    MissionUtility::MissionFailure();
    mFlags[1] = true;

        break;
    case 10:
    // ---- +0x7b6c  192 bytes ----
    if (!mFlags[131]) {
        MissionUtility::MoveObjectWithRotation(mHandles[259], "EndCinTankPath1", 0, true);
        mHandles[317] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_player", "EndCinTankPath", 0, "EndCinTank", 0, -1, -1);
        MissionUtility::SetAltitude(mHandles[317], 7.0f);
        MissionUtility::Goto(mHandles[317], "EndCinTankPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[317]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::OverrideSoundRange(mHandles[317], true);
        MissionUtility::OverrideSoundRange(mHandles[320], true);
        BeginTimer(mTimer22);
        MissionUtility::PlayMusic("EP6_V2_T04_02", true);
        mFlags[131] = true;
        mHandles[277] = MissionUtility::RunCin("endcin", true, true);
    }

    // ---- +0x7c2c  140 bytes ----
    if (mTimer22 > 6.0f) {
    StopTimer(mTimer22);
    mTimer22 = 0.0f;
    mHandles[316] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin_cin", "EndCinAnakinPath", 0, "EndCinAnakin", 0, -1, -1);
    MissionUtility::OverrideSoundRange(mHandles[316], true);
    MissionUtility::SetApplyDynamics(mHandles[320], true);
    MissionUtility::SetCollidable(mHandles[320], false);
    MissionUtility::SetAnimation(mHandles[320], "fullanimation", 1.0f, 1);
    BeginTimer(mTimer26);
    }

    // ---- +0x7cb8  56 bytes ----
    if (mTimer26 > 3.0f) {
    StopTimer(mTimer26);
    mTimer26 = 0.0f;
    MissionUtility::Goto(mHandles[316], "EndCinAnakinPath", false);
    }

    // ---- +0x7cf0  56 bytes ----
    if (!mFlags[0]) {
    if (mFlags[131]) {
    if (!MissionUtility::IsCinRunning(mHandles[277])) {
    MissionUtility::MissionSuccess();
    mFlags[0] = true;

        break;
    case 9:
    // ---- +0x7d28  36 bytes ----
    if (!mFlags[96]) {
        StopTimer(mTimer16);
        MissionUtility::BeginWave("tomb");
        mFlags[96] = true;
    }

    // ---- +0x7d4c  344 bytes ----
    if (!mFlags[97]) {
        if (MissionUtility::IsWaveSpawned("tomb")) {
        mHandles[263] = MissionUtility::GetHandle("guardian1");
        mHandles[264] = MissionUtility::GetHandle("guardian2");
        mHandles[265] = MissionUtility::GetHandle("guardian3");
        mHandles[259] = MissionUtility::GetHandle("playertank");
        mHandles[266] = MissionUtility::GetHandle("tombmound1");
        mHandles[267] = MissionUtility::GetHandle("tombmound2");
        mHandles[268] = MissionUtility::GetHandle("tombmound3");
        mHandles[260] = MissionUtility::GetHandle("tomb1");
        mHandles[261] = MissionUtility::GetHandle("tomb2");
        mHandles[262] = MissionUtility::GetHandle("tomb3");
        mHandles[269] = MissionUtility::GetHandle("tomb1fake1");
        mHandles[270] = MissionUtility::GetHandle("tomb1fake2");
        mHandles[271] = MissionUtility::GetHandle("tomb1fake3");
        mHandles[272] = MissionUtility::GetHandle("tomb2fake1");
        mHandles[273] = MissionUtility::GetHandle("tomb2fake2");
        mHandles[274] = MissionUtility::GetHandle("tomb3fake1");
        mHandles[275] = MissionUtility::GetHandle("tomb3fake2");
        MissionUtility::SetCurHealth(mHandles[263], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[264], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[265], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[263], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[264], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[265], 999999.0f);
        MissionUtility::EvictConfig("CIS_tank_fighter");
        MissionUtility::EvictConfig("OSS_bldg_turret");
        MissionUtility::EvictConfig("REP_walk_sixleg");
        MissionUtility::EvictConfig("REP_tank_beam");
        mFlags[97] = true;
        }
    }

    // ---- +0x7ea4  252 bytes ----
    if (mHandles[257] == 0) {
    MissionUtility::MoveObjectWithRotation(mHandles[4], "midcin1moveplayer1", 0, true);
    MissionUtility::SetTeamNum(mHandles[4], 0);
    MissionUtility::SetAsPlayer(mHandles[259], 0);
    mHandles[314] = MissionUtility::CreateObjectWithRotation("REP_fly_gunship", "EndCinTempleGunshipPath", 0, "MidCinTempleGunship", 0, -1, -1);
    MissionUtility::OverrideSoundRange(mHandles[314], true);
    MissionUtility::SetVelocMaximumFly(mHandles[314], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[314], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[314], 0.0f);
    mHandles[256] = MissionUtility::RunCin("midcin2", true, true);
    MissionUtility::PlayMusic("EP5_V2_T11", false);
    MissionUtility::SetCurHealth(mHandles[157], 9999.0f);
    MissionUtility::DamageObject(mHandles[157], 999999.0f, 999999.0f);
    MissionUtility::StartSound("C_DrdCoreSh_die01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mHandles[257] = 1;
    BeginTimer(mTimer24);
    }

    // ---- +0x7fa0  424 bytes ----
    if (mTimer24 > 4.0f) {
    StopTimer(mTimer24);
    mTimer24 = 0.0f;
    MissionUtility::DamageObject(mHandles[193], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[194], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[195], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[197], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[198], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[199], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[201], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[202], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[203], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[205], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[206], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[207], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[209], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[210], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[211], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[213], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[214], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[215], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[217], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[218], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[219], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[221], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[222], 999999.0f, 999999.0f);
    MissionUtility::DamageObject(mHandles[223], 999999.0f, 999999.0f);
    }

    // ---- +0x8148  184 bytes ----
    if (!mFlags[144] && !mFlags[139]) {
        if (MissionUtility::GetCinId(mHandles[256]) == 2) {
        mFlags[139] = true;
        MissionUtility::SetVelocMaximumFly(mHandles[314], 150.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[314], 150.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[314], 150.0f);
        mHandles[313] = MissionUtility::CreateObjectWithRotation("REP_tank_fighter1_player", "MidCinTempleTankPath", 0, "MidCInTempleTank", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[313], true);
        MissionUtility::Goto(mHandles[314], "EndCinTempleGunshipPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Land(mHandles[314], "TempleLandPath", 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        }
    }

    // ---- +0x8200  144 bytes ----
    if (!mFlags[144] && !mFlags[137]) {
        if (MissionUtility::GetCinId(mHandles[256]) == 3) {
        mFlags[137] = true;
        MissionUtility::QueueSound("obr17_12", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("asr17_13", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("obr17_33", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
    }

    // ---- +0x8290  168 bytes ----
    if (!mFlags[144] && !mFlags[135]) {
        if (MissionUtility::GetCinId(mHandles[256]) == 4) {
        mFlags[135] = true;
        mHandles[315] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin_cin", "MidCinTempleAnakinPath", 0, "MidCinTempleAnakin", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[315], true);
        MissionUtility::SetCollidable(mHandles[315], false);
        MissionUtility::SetCollidable(mHandles[313], false);
        MissionUtility::Goto(mHandles[315], "MidCinTempleAnakinPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[315]);
        MissionUtility::SetQueueFlag(false);
        BeginTimer(mTimer21);
        }
    }

    // ---- +0x8338  76 bytes ----
    if (!mFlags[144]) {
        if (mTimer21 > 5.0f) {
        MissionUtility::RemoveObject(mHandles[315]);
        MissionUtility::Goto(mHandles[313], "MidCinTempleTankGotoPath", false);
        StopTimer(mTimer21);
        mTimer21 = 0.0f;
        }
    }

    // ---- +0x8384  140 bytes ----
    if (!mFlags[144] && !mFlags[136]) {
        if (MissionUtility::GetCinId(mHandles[256]) == 5) {
        mFlags[136] = true;
        MissionUtility::PlayMusic("EP6_V2_T04_01", false);
        MissionUtility::SetSky("ossus3_CAVE.sky");
        MissionUtility::MoveObjectWithRotation(mHandles[313], "MidCinTempleTankPath1", 0, true);
        MissionUtility::Goto(mHandles[313], "MidCinTempleTankPath1", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[313]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocForward(mHandles[313], 200.0f);
        }
    }

    // ---- +0x8410  120 bytes ----
    if (!mFlags[144] && !mFlags[141]) {
        if (MissionUtility::GetCinId(mHandles[256]) == 6) {
        mFlags[141] = true;
        MissionUtility::QueueSound("asr17_35", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("obr17_36", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObject(mHandles[313]);
        }
    }

    // ---- +0x8488  92 bytes ----
    if (!MissionUtility::IsCinRunning(mHandles[256])) {
    MissionUtility::FlushSoundQueue();
    mFlags[144] = true;
    MissionUtility::SetSky("ossus3_CAVE.sky");
    MissionUtility::MoveObjectWithRotation(mHandles[259], "tombplayermove", 0, true);
    MissionUtility::RemoveObject(mHandles[313]);
    MissionUtility::RemoveObject(mHandles[314]);
    ResumeTimer(mTimer16);
    mPhase = 13;

        break;
    case 13:
    // ---- +0x84e4  288 bytes ----
    if (!mFlags[98]) {
        MissionUtility::SetMapZoom(800.0f, 999999.0f);
        mHandles[258] = MissionUtility::AddObjective("missions.Ossus3.objective.str0017");
        MissionUtility::PlayMusic("EP2_V1_T12_03", false);
        MissionUtility::SetFOV(55.4f);
        MissionUtility::SetAsPlayer(mHandles[259], 0);
        MissionUtility::SetTeamNum(mHandles[259], 1);
        MissionUtility::SetMaxHealth(mHandles[260], 2200.0f);
        MissionUtility::SetMaxHealth(mHandles[261], 2200.0f);
        MissionUtility::SetMaxHealth(mHandles[262], 2200.0f);
        MissionUtility::SetCurHealth(mHandles[260], 2200.0f);
        MissionUtility::SetCurHealth(mHandles[261], 2200.0f);
        MissionUtility::SetCurHealth(mHandles[262], 2200.0f);
        MissionUtility::Stop(mHandles[263]);
        MissionUtility::Stop(mHandles[264]);
        MissionUtility::Stop(mHandles[265]);
        MissionUtility::SetAttackRange(mHandles[263], 0);
        MissionUtility::SetAttackRange(mHandles[264], 0);
        MissionUtility::SetAttackRange(mHandles[265], 0);
        MissionUtility::SetAnimation(mHandles[263], "skel", 1.0f, -1);
        MissionUtility::SetAnimation(mHandles[264], "skel", 1.0f, -1);
        MissionUtility::SetAnimation(mHandles[265], "skel", 1.0f, -1);
        MissionUtility::StopAmbiences();
        mFlags[100] = true;
        mFlags[98] = true;
    }

    // ---- +0x8604  48 bytes ----
    if (!mFlags[117]) {
        if (MissionUtility::GetCurHealth(mHandles[266]) < 1450.0f) {
        MissionUtility::StopAnimation(mHandles[263]);
        mFlags[117] = true;
        }
    }

    // ---- +0x8634  48 bytes ----
    if (!mFlags[118]) {
        if (MissionUtility::GetCurHealth(mHandles[267]) < 1450.0f) {
        MissionUtility::StopAnimation(mHandles[264]);
        mFlags[118] = true;
        }
    }

    // ---- +0x8664  48 bytes ----
    if (!mFlags[119]) {
        if (MissionUtility::GetCurHealth(mHandles[268]) < 1450.0f) {
        MissionUtility::StopAnimation(mHandles[265]);
        mFlags[119] = true;
        }
    }

    // ---- +0x8694  92 bytes ----
    if (!mFlags[120]) {
        if (MissionUtility::GetCurHealth(mHandles[266]) < 1000.0f) {
        MissionUtility::SetAttackRange(mHandles[263], 1000);
        MissionUtility::SetVelocForward(mHandles[263], 0.0f);
        MissionUtility::AttackTarget(mHandles[263], mHandles[4], false, false, false, false);
        mFlags[120] = true;
        }
    }

    // ---- +0x86f0  92 bytes ----
    if (!mFlags[121]) {
        if (MissionUtility::GetCurHealth(mHandles[267]) < 1000.0f) {
        MissionUtility::SetAttackRange(mHandles[264], 1000);
        MissionUtility::SetVelocForward(mHandles[264], 0.0f);
        MissionUtility::AttackTarget(mHandles[264], mHandles[4], false, false, false, false);
        mFlags[121] = true;
        }
    }

    // ---- +0x874c  92 bytes ----
    if (!mFlags[122]) {
        if (MissionUtility::GetCurHealth(mHandles[268]) < 1000.0f) {
        MissionUtility::SetAttackRange(mHandles[265], 1000);
        MissionUtility::SetVelocForward(mHandles[265], 0.0f);
        MissionUtility::AttackTarget(mHandles[265], mHandles[4], false, false, false, false);
        mFlags[122] = true;
        }
    }

    // ---- +0x87a8  132 bytes ----
    if (!mFlags[123]) {
        if (!MissionUtility::IsAlive(mHandles[266])) {
        MissionUtility::SetTeamNum(mHandles[263], 11);
        MissionUtility::SetAttackRange(mHandles[263], 1000);
        MissionUtility::AttackTarget(mHandles[263], mHandles[267], true, false, true, false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[263], mHandles[268], true, false, true, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[123] = true;
        }
    }

    // ---- +0x882c  132 bytes ----
    if (!mFlags[124]) {
        if (!MissionUtility::IsAlive(mHandles[267])) {
        MissionUtility::SetTeamNum(mHandles[264], 11);
        MissionUtility::SetAttackRange(mHandles[264], 1000);
        MissionUtility::AttackTarget(mHandles[264], mHandles[268], true, false, true, false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[264], mHandles[266], true, false, true, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[124] = true;
        }
    }

    // ---- +0x88b0  132 bytes ----
    if (!mFlags[125]) {
        if (!MissionUtility::IsAlive(mHandles[268])) {
        MissionUtility::SetTeamNum(mHandles[265], 11);
        MissionUtility::SetAttackRange(mHandles[265], 1000);
        MissionUtility::AttackTarget(mHandles[265], mHandles[266], true, false, true, false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[265], mHandles[267], true, false, true, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[125] = true;
        }
    }

    // ---- +0x8934  212 bytes ----
    if (!mFlags[126] && mFlags[123] && mFlags[124] && mFlags[125]) {
        MissionUtility::SetTeamNum(mHandles[263], 2);
        MissionUtility::SetTeamNum(mHandles[264], 2);
        MissionUtility::SetTeamNum(mHandles[265], 2);
        MissionUtility::AttackTarget(mHandles[263], mHandles[4], true, false, true, false);
        MissionUtility::SetVelocForward(mHandles[263], 160.0f);
        MissionUtility::AttackTarget(mHandles[264], mHandles[4], true, false, true, false);
        MissionUtility::SetVelocForward(mHandles[264], 200.0f);
        MissionUtility::AttackTarget(mHandles[265], mHandles[4], true, false, true, false);
        MissionUtility::SetVelocForward(mHandles[265], 240.0f);
        mFlags[126] = true;
    }

    // ---- +0x8a08  132 bytes ----
    if (!mFlags[2]) {
        if (mHandles[4] == MissionUtility::GetWhoShotMe(mHandles[260])
            || mHandles[4] == MissionUtility::GetWhoShotMe(mHandles[261])
            || mHandles[4] == MissionUtility::GetWhoShotMe(mHandles[262])) {
        MissionUtility::StartSound("C_PhantGuard_pain01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        BeginTimer(mTimer1);
        mFlags[3] = false;
        mFlags[2] = true;
        }
    }

    // ---- +0x8a8c  228 bytes ----
    if (!mFlags[99]) {
        if (mInts[1] < 3) {
        if (MissionUtility::GetCurHealth(mHandles[260]) < 1900.0f
            || MissionUtility::GetCurHealth(mHandles[261]) < 1900.0f
            || MissionUtility::GetCurHealth(mHandles[262]) < 1900.0f) {
        MissionUtility::QueueSound("ASR17_38", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::StartSound("C_PhantGuard_pain01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::DamageObject(mHandles[266], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[267], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[268], 999999.0f, 999999.0f);
        mInts[1] = mInts[1] + 1;
        BeginTimer(mTimer15);
        mFlags[99] = true;
        }
        }
    }

    // ---- +0x8b70  28 bytes ----
    if (mTimer15 > 3.0f) {
    mFlags[99] = true;
    }

    // ---- +0x8b8c  128 bytes ----
    if (!mFlags[112]) {
        if (MissionUtility::GetCurHealth(mHandles[260]) < 1900.0f
            || MissionUtility::GetCurHealth(mHandles[261]) < 1900.0f
            || MissionUtility::GetCurHealth(mHandles[262]) < 1900.0f) {
        MissionUtility::AddHealthBar(mHandles[260], "Tomb 1", 400.0f);
        MissionUtility::AddHealthBar(mHandles[261], "Tomb 2", 400.0f);
        MissionUtility::AddHealthBar(mHandles[262], "Tomb 3", 400.0f);
        mFlags[112] = true;
        }
    }

    // ---- +0x8c0c  88 bytes ----
    if (!mFlags[113]) {
        if (mFlags[127]
            || mFlags[128]
            || mFlags[129]) {
        MissionUtility::QueueSound("ASR17_39", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[113] = true;
        }
    }

    // ---- +0x8c64  44 bytes ----
    if (!mFlags[114]) {
        if (!MissionUtility::IsAlive(mHandles[260])) {
        BeginTimer(mTimer12);
        mFlags[114] = true;
        }
    }

    // ---- +0x8c90  104 bytes ----
    if (!mFlags[127]) {
        if (mTimer12 > 2.0f) {
        MissionUtility::SetCurHealth(mHandles[263], 99999.0f);
        MissionUtility::DamageObject(mHandles[263], 999999.0f, 999999.0f);
        MissionUtility::StartSound("C_PhantGuard_pain01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[127] = true;
        }
    }

    // ---- +0x8cf8  44 bytes ----
    if (!mFlags[115]) {
        if (!MissionUtility::IsAlive(mHandles[261])) {
        BeginTimer(mTimer13);
        mFlags[115] = true;
        }
    }

    // ---- +0x8d24  148 bytes ----
    if (!mFlags[128]) {
        if (mTimer13 > 2.0f) {
        MissionUtility::SetCurHealth(mHandles[264], 99999.0f);
        MissionUtility::DamageObject(mHandles[264], 999999.0f, 999999.0f);
        MissionUtility::StartSound("C_PhantGuard_pain01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        if (MissionUtility::IsAlive(mHandles[265])) {
        MissionUtility::SetVelocForward(mHandles[263], 200.0f);
        } else {
        MissionUtility::SetVelocForward(mHandles[263], 240.0f);
        }
        mFlags[128] = true;
        }
    }

    // ---- +0x8db8  44 bytes ----
    if (!mFlags[116]) {
        if (!MissionUtility::IsAlive(mHandles[262])) {
        BeginTimer(mTimer14);
        mFlags[116] = true;
        }
    }

    // ---- +0x8de4  160 bytes ----
    if (!mFlags[129]) {
        if (mTimer14 > 2.0f) {
        MissionUtility::SetCurHealth(mHandles[265], 99999.0f);
        MissionUtility::DamageObject(mHandles[265], 999999.0f, 999999.0f);
        MissionUtility::StartSound("C_PhantGuard_pain01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        if (MissionUtility::IsAlive(mHandles[264])) {
        MissionUtility::SetVelocForward(mHandles[264], 240.0f);
        MissionUtility::SetVelocForward(mHandles[263], 200.0f);
        } else {
        MissionUtility::SetVelocForward(mHandles[263], 200.0f);
        }
        mFlags[129] = true;
        }
    }

    // ---- +0x8e84  504 bytes ----
    if (mFlags[126] && mFlags[113]) {
        if (!mFlags[101]) {
        if (MissionUtility::GetDistance(mHandles[4], mHandles[263]) < 50.0f) {
        MissionUtility::AttackTarget(mHandles[263], mHandles[4], false, true, false, true);
        mFlags[100] = false;
        mFlags[101] = true;
        }
        }
        if (!mFlags[100]) {
        if (MissionUtility::GetDistance(mHandles[4], mHandles[263]) > 50.0f) {
        MissionUtility::AttackTarget(mHandles[263], mHandles[4], true, false, true, false);
        mFlags[101] = false;
        mFlags[100] = true;
        }
        }
        if (!mFlags[103]) {
        if (MissionUtility::GetDistance(mHandles[4], mHandles[264]) < 50.0f) {
        MissionUtility::AttackTarget(mHandles[264], mHandles[4], false, true, false, true);
        mFlags[102] = false;
        mFlags[103] = true;
        }
        }
        if (!mFlags[102]) {
        if (MissionUtility::GetDistance(mHandles[4], mHandles[264]) > 50.0f) {
        MissionUtility::AttackTarget(mHandles[264], mHandles[4], true, false, true, false);
        mFlags[103] = false;
        mFlags[102] = true;
        }
        }
        if (!mFlags[105]) {
        if (MissionUtility::GetDistance(mHandles[4], mHandles[265]) < 50.0f) {
        MissionUtility::AttackTarget(mHandles[265], mHandles[4], false, true, false, true);
        mFlags[104] = false;
        mFlags[105] = true;
        }
        }
        if (!mFlags[104]) {
        if (MissionUtility::GetDistance(mHandles[4], mHandles[265]) > 50.0f) {
        MissionUtility::AttackTarget(mHandles[265], mHandles[4], true, false, true, false);
        mFlags[105] = false;
        mFlags[104] = true;
        }
        }
    }

    // ---- +0x907c  96 bytes ----
    if (!MissionUtility::IsAlive(mHandles[263])) {
    if (!MissionUtility::IsAlive(mHandles[264])) {
    if (!MissionUtility::IsAlive(mHandles[265])) {
    StopTimer(mTimer16);
    if (MissionUtility::GetGameClock() < 720.0f) {
    MissionUtility::BonusObjectiveComplete(mHandles[8], true);
    } else {
    MissionUtility::BonusObjectiveFailed(mHandles[8]);
    }

    // ---- +0x90dc  16 bytes ----
    MissionUtility::ObjectiveComplete(mHandles[258]);
    mPhase = 10;

        break;
    case 19:
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

void Ossus3Script::Setup()
{
        mFlags[8] = true;
        mFlags[107] = true;
        mFlags[109] = true;
        mFlags[111] = true;
        mFlags[3] = true;
        mInts[4] = 0;
        mHandles[6] = MissionUtility::GetHandle("playergunship");
        mHandles[5] = MissionUtility::GetHandle("tempplayer");
        mHandles[12] = MissionUtility::GetHandle("asixleg1");
        mHandles[13] = MissionUtility::GetHandle("asixleg2");
        mHandles[14] = MissionUtility::GetHandle("asixleg3");
        mHandles[16] = MissionUtility::GetHandle("a1fighter1");
        mHandles[17] = MissionUtility::GetHandle("a1fighter2");
        mHandles[18] = MissionUtility::GetHandle("a1fighter3");
        mHandles[19] = MissionUtility::GetHandle("a1fighter4");
        mHandles[20] = MissionUtility::GetHandle("a1fighter5");
        mHandles[21] = MissionUtility::GetHandle("a1fighter6");
        mHandles[22] = MissionUtility::GetHandle("a1fighter7");
        mHandles[23] = MissionUtility::GetHandle("a1assault1");
        mHandles[24] = MissionUtility::GetHandle("a1assault2");
        mHandles[25] = MissionUtility::GetHandle("a1assault3");
        mHandles[26] = MissionUtility::GetHandle("a1wheel1");
        mHandles[27] = MissionUtility::GetHandle("a1wheel2");
        mHandles[28] = MissionUtility::GetHandle("a1wheel3");
        mHandles[29] = MissionUtility::GetHandle("a1turret1");
        mHandles[30] = MissionUtility::GetHandle("a1turret2");
        mHandles[31] = MissionUtility::GetHandle("a1turret3");
        mHandles[32] = MissionUtility::GetHandle("a1turret4");
        mHandles[33] = MissionUtility::GetHandle("a1turret5");
        mHandles[34] = MissionUtility::GetHandle("a1turret6");
        mHandles[35] = MissionUtility::GetHandle("a1turret7");
        mHandles[36] = MissionUtility::GetHandle("a1turret8");
        mHandles[37] = MissionUtility::GetHandle("a1turret9");
        mHandles[38] = MissionUtility::GetHandle("a1turret10");
        mHandles[39] = MissionUtility::GetHandle("a1turret11");
        mHandles[40] = MissionUtility::GetHandle("a1turret12");
        mHandles[41] = MissionUtility::GetHandle("a1turret13");
        mHandles[42] = MissionUtility::GetHandle("a1turret14");
        mHandles[43] = MissionUtility::GetHandle("a1turret15");
        mHandles[44] = MissionUtility::GetHandle("a1turret16");
        mHandles[45] = MissionUtility::GetHandle("a1turret17");
        mHandles[46] = MissionUtility::GetHandle("a1turret18");
        mHandles[57] = MissionUtility::GetHandle("vcarrier1");
        mHandles[58] = MissionUtility::GetHandle("vcarrier2");
        mHandles[59] = MissionUtility::GetHandle("vcarrier3");
        mHandles[2] = MissionUtility::GetHandle("testwheel");
        mHandles[83] = MissionUtility::GetHandle("a2turret1");
        mHandles[84] = MissionUtility::GetHandle("a2turret2");
        mHandles[85] = MissionUtility::GetHandle("a2turret3");
        mHandles[86] = MissionUtility::GetHandle("a2turret4");
        mHandles[87] = MissionUtility::GetHandle("a2turret5");
        mHandles[88] = MissionUtility::GetHandle("a2turret6");
        mHandles[89] = MissionUtility::GetHandle("a2turret7");
        mHandles[90] = MissionUtility::GetHandle("a2turret8");
        mHandles[91] = MissionUtility::GetHandle("a2turret9");
        mHandles[92] = MissionUtility::GetHandle("a2turret10");
        mHandles[93] = MissionUtility::GetHandle("a2turret11");
        mHandles[94] = MissionUtility::GetHandle("a2turret12");
        mHandles[95] = MissionUtility::GetHandle("a2turret13");
        mHandles[96] = MissionUtility::GetHandle("a2turret14");
        mHandles[97] = MissionUtility::GetHandle("a2turret15");
        mHandles[98] = MissionUtility::GetHandle("a2turret16");
        mHandles[99] = MissionUtility::GetHandle("a2turret17");
        mHandles[100] = MissionUtility::GetHandle("a2turret18");
        mHandles[101] = MissionUtility::GetHandle("a2turret19");
        mHandles[102] = MissionUtility::GetHandle("a2turret20");
        mHandles[103] = MissionUtility::GetHandle("a2turret21");
        mHandles[104] = MissionUtility::GetHandle("a2turret22");
        mHandles[105] = MissionUtility::GetHandle("a2turret23");
        mHandles[106] = MissionUtility::GetHandle("a2turret24");
        mHandles[107] = MissionUtility::GetHandle("a2turret25");
        mHandles[108] = MissionUtility::GetHandle("a2turret26");
        mHandles[109] = MissionUtility::GetHandle("a2turret27");
        mHandles[110] = MissionUtility::GetHandle("a2turret28");
        mHandles[122] = MissionUtility::GetHandle("icesheet1");
        mHandles[123] = MissionUtility::GetHandle("icesheet2");
        mHandles[124] = MissionUtility::GetHandle("icesheet3");
        mHandles[125] = MissionUtility::GetHandle("icesheet4");
        mHandles[126] = MissionUtility::GetHandle("icesheet5");
        mHandles[127] = MissionUtility::GetHandle("icesheet6");
        mHandles[129] = MissionUtility::GetHandle("convoy1");
        mHandles[130] = MissionUtility::GetHandle("convoy2");
        mHandles[131] = MissionUtility::GetHandle("convoy3");
        mHandles[132] = MissionUtility::GetHandle("convoy4");
        mHandles[133] = MissionUtility::GetHandle("convoy5");
        mHandles[134] = MissionUtility::GetHandle("convoy6");
        mHandles[135] = MissionUtility::GetHandle("convoy7");
        mHandles[136] = MissionUtility::GetHandle("convoy8");
        mHandles[137] = MissionUtility::GetHandle("cassault1");
        mHandles[138] = MissionUtility::GetHandle("cassault2");
        mHandles[139] = MissionUtility::GetHandle("cassault3");
        mHandles[140] = MissionUtility::GetHandle("cassault4");
        mHandles[141] = MissionUtility::GetHandle("cassault5");
        mHandles[142] = MissionUtility::GetHandle("cassault6");
        mHandles[143] = MissionUtility::GetHandle("cassault7");
        mHandles[144] = MissionUtility::GetHandle("cassault8");
        mHandles[157] = MissionUtility::GetHandle("controlpod");
        mHandles[158] = MissionUtility::GetHandle("techno1");
        mHandles[159] = MissionUtility::GetHandle("techno2");
        mHandles[160] = MissionUtility::GetHandle("techno3");
        mHandles[165] = MissionUtility::GetHandle("lconvoy1cin");
        mHandles[166] = MissionUtility::GetHandle("lconvoy2cin");
        mHandles[167] = MissionUtility::GetHandle("lconvoy3cin");
        mHandles[168] = MissionUtility::GetHandle("lconvoy4cin");
        mHandles[169] = MissionUtility::GetHandle("lconvoy5cin");
        mHandles[170] = MissionUtility::GetHandle("lconvoy1");
        mHandles[171] = MissionUtility::GetHandle("lconvoy2");
        mHandles[172] = MissionUtility::GetHandle("lconvoy3");
        mHandles[173] = MissionUtility::GetHandle("lconvoy4");
        mHandles[174] = MissionUtility::GetHandle("lconvoy5");
        mHandles[175] = MissionUtility::GetHandle("lbeam1");
        mHandles[178] = MissionUtility::GetHandle("rconvoy1cin");
        mHandles[179] = MissionUtility::GetHandle("rconvoy2cin");
        mHandles[180] = MissionUtility::GetHandle("rconvoy3cin");
        mHandles[181] = MissionUtility::GetHandle("rconvoy4cin");
        mHandles[182] = MissionUtility::GetHandle("rconvoy5cin");
        mHandles[183] = MissionUtility::GetHandle("rconvoy1");
        mHandles[184] = MissionUtility::GetHandle("rconvoy2");
        mHandles[185] = MissionUtility::GetHandle("rconvoy3");
        mHandles[186] = MissionUtility::GetHandle("rconvoy4");
        mHandles[187] = MissionUtility::GetHandle("rconvoy5");
        mHandles[188] = MissionUtility::GetHandle("rbeam1");
        mHandles[209] = MissionUtility::GetHandle("rassaulta1");
        mHandles[210] = MissionUtility::GetHandle("rassaulta2");
        mHandles[211] = MissionUtility::GetHandle("rassaulta3");
        mHandles[213] = MissionUtility::GetHandle("rassaultb1");
        mHandles[214] = MissionUtility::GetHandle("rassaultb2");
        mHandles[215] = MissionUtility::GetHandle("rassaultb3");
        mHandles[217] = MissionUtility::GetHandle("rassaultc1");
        mHandles[218] = MissionUtility::GetHandle("rassaultc2");
        mHandles[219] = MissionUtility::GetHandle("rassaultc3");
        mHandles[221] = MissionUtility::GetHandle("rassaultd1");
        mHandles[222] = MissionUtility::GetHandle("rassaultd2");
        mHandles[223] = MissionUtility::GetHandle("rassaultd3");
        mHandles[193] = MissionUtility::GetHandle("lassaulta1");
        mHandles[194] = MissionUtility::GetHandle("lassaulta2");
        mHandles[195] = MissionUtility::GetHandle("lassaulta3");
        mHandles[197] = MissionUtility::GetHandle("lassaultb1");
        mHandles[198] = MissionUtility::GetHandle("lassaultb2");
        mHandles[199] = MissionUtility::GetHandle("lassaultb3");
        mHandles[201] = MissionUtility::GetHandle("lassaultc1");
        mHandles[202] = MissionUtility::GetHandle("lassaultc2");
        mHandles[203] = MissionUtility::GetHandle("lassaultc3");
        mHandles[205] = MissionUtility::GetHandle("lassaultd1");
        mHandles[206] = MissionUtility::GetHandle("lassaultd2");
        mHandles[207] = MissionUtility::GetHandle("lassaultd3");
        mHandles[320] = MissionUtility::GetHandle("EndCinTombDoor");
        mHandles[260] = MissionUtility::GetHandle("tomb1");
        mHandles[261] = MissionUtility::GetHandle("tomb2");
        mHandles[262] = MissionUtility::GetHandle("tomb3");
        mHandles[263] = MissionUtility::GetHandle("guardian1");
        mHandles[264] = MissionUtility::GetHandle("guardian2");
        mHandles[265] = MissionUtility::GetHandle("guardian3");
        mHandles[259] = MissionUtility::GetHandle("playertank");
        mHandles[278] = MissionUtility::GetHandle("OpenCinGunship");
        mHandles[303] = MissionUtility::GetHandle("MidCinLeftCarrier1");
        mHandles[304] = MissionUtility::GetHandle("MidCinLeftCarrier2");
        mHandles[305] = MissionUtility::GetHandle("MidCinLeftCarrier3");
        mHandles[306] = MissionUtility::GetHandle("MidCinLeftCarrier4");
        mHandles[307] = MissionUtility::GetHandle("MidCinLeftCarrier5");
        mHandles[308] = MissionUtility::GetHandle("MidCinRightCarrier1");
        mHandles[309] = MissionUtility::GetHandle("MidCinRightCarrier2");
        mHandles[310] = MissionUtility::GetHandle("MidCinRightCarrier3");
        mHandles[311] = MissionUtility::GetHandle("MidCinRightCarrier4");
        mHandles[312] = MissionUtility::GetHandle("MidCinRightCarrier5");
        MissionUtility::PreloadConfig("REP_fly_fighter");
        MissionUtility::PreloadConfig("CIS_fly_technounion_dest");
        MissionUtility::PreloadConfig("CIS_fly_fighter_combat");
        MissionUtility::PreloadConfig("CIS_fly_landed_dest");
        MissionUtility::PreloadConfig("CIS_tank_wheeled_ossus3");
        MissionUtility::PreloadConfig("rep_inf_anakin_cin");
        MissionUtility::PreloadConfig("REP_fly_gunship");
}

SPMission *Ossus3BuildMission()
{
    return new Ossus3Script();
}
