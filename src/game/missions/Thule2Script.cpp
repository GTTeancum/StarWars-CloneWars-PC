// Thule2Script.cpp -- reconstruction of a shipped mission script.  BYTE-EXACT, all 4 functions.
//
// Head from tools/gen_mission_head.py, bodies from tools/gen_block.py, assembled by
// tools/gen_mission_tu.py.  Five things in `Execute` were not generated -- see
// analysis/mission_batch_a.md:
//   * `savePoint` is one call held in r29, not three calls (the shipped prologue is `stmw r29`);
//   * two `switch`es on a cinematic step counter, compiled as CodeWarrior compare chains;
//   * thirteen explicit `Vector` arguments, each with its own argument slot.

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
void StopTimer(Timer &t);

enum Formation { kFormation0 };        // an enum in the API: only the mangling matters

namespace MissionUtility
{
    int    AddAmmoBox(const char*, int, float);
    int    AddBonusObjective(const char*);
    void   AddFlockMember(int, int);
    int    AddFlockMember(int, int, Vector);
    int    AddFlyerArmy(char, const char*, int, int, float);
    int    AddHealthBar(int, const char*, float);
    int    AddHealthBox(const char*, int, float);
    int    AddObjective(const char*);
    int    AddPropArmy(char, const char*, int, int, float);
    void   AddSquadMember(int, int);
    void   AddTurnAroundRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    void   AttackTarget(int, int, bool, bool, bool, bool);
    void   BeginWave(const char*);
    void   BonusObjectiveComplete(int, bool);
    void   BonusObjectiveFailed(int);
    void   CarrierAddCargo(int, const char*, int, const char*, bool);
    void   CarrierDropoff(int, const char*, int, float);
    int    CountUnitsNearObject(int, float, int, const char*);
    int    CountUnitsNearPoint(const char*, int, float, int, const char*);
    int    CreateFlock(int, Formation);
    int    CreateObject(const char*, const Vector&, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObject(const char*, const char*, const char*, int, int, int);
    int    CreateObject(const char*, const char*, int, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObject(const char*, int, const char*, const char*, int, int, int);
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    int    CreateSquad(int, Formation);
    void   DamageObject(int, float, float);
    void   DisbandSquad(int);
    void   DisplayText(const char*, float, float);
    int    DropAmmoBox(int);
    int    DropHealthBox(int);
    void   EvictConfig(const char*);
    void   FlushSoundQueue();
    int    GetCinId(int);
    float  GetCurHealth(int);
    float  GetDistance(int, const char*);
    float  GetDistance(int, const char*, int);
    float  GetDistance(int, int);
    int    GetHandle(const char*);
    int    GetPlayerHandle(int);
    int    GetRandomInt(int, int);
    float  GetTime();
    int    GetWhoShotMe(int);
    void   Goto(int, const char*, bool);
    void   Goto(int, int);
    void   GotoFire(int, const char*, bool, bool, bool, bool);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsDropped(int);
    bool   IsFlockAlive(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsLanded(int);
    bool   IsPowerupAlive(unsigned int);
    bool   IsSoundPlaying(int);
    bool   IsWaveSpawned(const char*);
    void   Land(int, const char*, int, float);
    int    MidMissionGetSavePoint();
    void   MidMissionLoad(bool&);
    void   MidMissionLoadPlayer();
    void   MidMissionSave(bool);
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
    void   RemoveArmy(int);
    void   RemoveFlock(int, bool);
    void   RemoveFlyerArmy(int);
    void   RemoveHealthBar(int);
    void   RemoveObject(int);
    void   RemoveObjectify(const char*, int);
    void   RemoveObjectify(int);
    void   RemoveTurnAroundRegion(const char*);
    int    RunCin(const char*, bool, bool);
    void   SetAlliance(int, int);
    void   SetAnimation(int, const char*, float, int);
    void   SetApplyDynamics(int, bool);
    void   SetAsPlayer(int, int);
    void   SetAttackRange(int, int);
    void   SetCollidable(int, bool);
    void   SetCurHealth(int, float);
    void   SetCurPrimaryAmmo(int, float);
    void   SetCurSecondaryAmmo(int, float);
    void   SetCurShield(int, float);
    void   SetCurSpecialAmmo(int, float);
    void   SetEnemies(int, int);
    void   SetEnemiesOneWay(int, int);
    void   SetFOV(float);
    void   SetFireSpecial(int, bool);
    void   SetFiringRange(int, int);
    void   SetFogRange(float, float, float);
    void   SetMaxAltitude(int, float);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetNeutral(int, int);
    void   SetNeutralOneWay(int, int);
    void   SetOnRadar(int, bool);
    void   SetPropArmyWayPoints(int, const char*, bool);
    void   SetQueueFlag(bool);
    void   SetSquadAttackSound(const char*);
    void   SetSquadBreakSound(const char*);
    void   SetSquadHoldSound(const char*);
    void   SetSquadInvalidSound(const char*);
    void   SetSquadRegroupSound(const char*);
    void   SetTakeoffAltitude(int, float);
    void   SetTeamNum(int, int);
    void   SetVelocForward(int, float);
    void   SetVelocMaximumFly(int, float);
    void   SetVelocMinimumFly(int, float);
    void   SetVelocNeutralFly(int, float);
    void   SetVelocVertical(int, float);
    void   SetVelocVerticalFly(int, float);
    void   SetVisible(int, bool);
    void   SetWeaponPitch(int, const char*, float);
    void   SetWeaponYaw(int, const char*, float);
    void   ShakeCamera(float, float, float);
    void   StartAmbiences(const char*, const char*, float, float);
    int    StartSound(const char*, bool, float, float, float, const char*, int, const char*);
    void   Stop(int);
    void   StopCameraShake(float);
    void   Wait(int);
}

static const char *const kClassName = "Thule2Script";
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
    float *mIntsB;    int mCountB;       // +0x0c +0x10
    int   *mIntsC;    int mCountC;       // +0x14 +0x18
    void  *mBlockD;   int mCountD;       // +0x1c +0x20
};

class Thule2Script : public SPMission
{
public:
    virtual ~Thule2Script();

    Thule2Script()
    {
        mBoolCount  = 237;           mBools = mFlags;
        mCountB     = 48;            mIntsB = mTimes;
        mCountC     = 529;           mIntsC = mHandles;
        mCountD     = 14;            mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    char   mPad24[13];
    bool   mFlags[237];            // +0x031  the one-shot latches
    char   mPad11E[6];                  // +0x11e
    float    mTimes[48];            // +0x124
    char   mPad1E4[8];                  // +0x1e4
    int    mHandles[529];          // +0x1ec
    char   mPadA30[8];                  // +0xa30
    int    mInts[14];             // +0xa38
    char   mPadA70[4];                  // +0xa70
    Timer  mTimer0;                      // +0xa74
    Timer  mTimer1;                      // +0xa80
    Timer  mTimer2;                      // +0xa8c
    Timer  mTimer3;                      // +0xa98
    Timer  mTimer4;                      // +0xaa4
    Timer  mTimer5;                      // +0xab0
    Timer  mTimer6;                      // +0xabc
    Timer  mTimer7;                      // +0xac8
    Timer  mTimer8;                      // +0xad4
    Timer  mTimer9;                      // +0xae0
    Timer  mTimer10;                      // +0xaec
    Timer  mTimer11;                      // +0xaf8
    Timer  mTimer12;                      // +0xb04
    Timer  mTimer13;                      // +0xb10
    Timer  mTimer14;                      // +0xb1c
    Timer  mTimer15;                      // +0xb28
    Timer  mTimer16;                      // +0xb34
    Timer  mTimer17;                      // +0xb40
    Timer  mTimer18;                      // +0xb4c
};

Thule2Script::~Thule2Script()
{
}

void Thule2Script::Execute()
{
    // ---- +0x0008  4356 bytes ----
    mHandles[7] = MissionUtility::GetPlayerHandle(0);
    if (!mFlags[15]) {
    MissionUtility::SetMusicLooping(true);
    MissionUtility::SetEnemies(1, 2);
    MissionUtility::SetEnemies(3, 4);
    MissionUtility::SetNeutral(1, 0);
    MissionUtility::SetAlliance(2, 0);
    MissionUtility::SetAlliance(1, 5);
    MissionUtility::SetEnemies(5, 6);
    MissionUtility::SetAlliance(1, 6);
    MissionUtility::SetEnemies(5, 2);
    MissionUtility::SetEnemies(1, 7);
    MissionUtility::SetEnemies(5, 8);
    MissionUtility::SetEnemiesOneWay(1, 8);
    MissionUtility::SetNeutralOneWay(8, 1);
    MissionUtility::SetEnemiesOneWay(1, 11);
    MissionUtility::SetNeutralOneWay(11, 1);
    MissionUtility::SetAlliance(1, 9);
    MissionUtility::SetNeutral(2, 9);
    MissionUtility::SetEnemies(8, 9);
    MissionUtility::SetNeutralOneWay(8, 10);
    MissionUtility::SetEnemiesOneWay(10, 8);
    mInts[6] = MissionUtility::AddBonusObjective("missions.Thule2.bonus.str0000");
    mInts[7] = MissionUtility::AddBonusObjective("missions.Thule2.bonus.str0001");
    mInts[8] = MissionUtility::AddBonusObjective("missions.Thule2.bonus.str0004");
    MissionUtility::SetTeamNum(mHandles[61], 0);
    MissionUtility::SetTeamNum(mHandles[63], 0);
    int savePoint = MissionUtility::MidMissionGetSavePoint();
    if (savePoint == 1) {
    mFlags[167] = true;
    mFlags[170] = true;
    mInts[0] = MissionUtility::AddObjective("missions.Thule2.objective.str0000");
    MissionUtility::ObjectiveComplete(mInts[0]);
    mFlags[80] = true;
    MissionUtility::RemoveObject(mHandles[191]);
    MissionUtility::RemoveObject(mHandles[192]);
    MissionUtility::RemoveObject(mHandles[193]);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mFlags[113]);
    MissionUtility::MidMissionLoad(mFlags[116]);
    MissionUtility::MidMissionLoad(mFlags[114]);
    MissionUtility::MidMissionLoad(mFlags[117]);
    MissionUtility::MidMissionLoad(mFlags[115]);
    MissionUtility::MidMissionLoad(mFlags[118]);
    if (mFlags[113]) {
    MissionUtility::BonusObjectiveComplete(mInts[6], false);
    }
    if (mFlags[116]) {
    MissionUtility::BonusObjectiveFailed(mInts[6]);
    }
    if (mFlags[114]) {
    MissionUtility::BonusObjectiveComplete(mInts[7], false);
    }
    if (mFlags[117]) {
    MissionUtility::BonusObjectiveFailed(mInts[7]);
    }
    if (mFlags[115]) {
    MissionUtility::BonusObjectiveComplete(mInts[8], false);
    }
    if (mFlags[118]) {
    MissionUtility::BonusObjectiveFailed(mInts[8]);
    }
    MissionUtility::RemoveObject(mHandles[164]);
    MissionUtility::RemoveObject(mHandles[397]);
    MissionUtility::RemoveObject(mHandles[398]);
    MissionUtility::RemoveObject(mHandles[399]);
    MissionUtility::RemoveObject(mHandles[57]);
    MissionUtility::RemoveObject(mHandles[58]);
    MissionUtility::RemoveObject(mHandles[59]);
    MissionUtility::RemoveObject(mHandles[60]);
    MissionUtility::RemoveObject(mHandles[61]);
    MissionUtility::RemoveObject(mHandles[62]);
    MissionUtility::RemoveObject(mHandles[63]);
    MissionUtility::RemoveObject(mHandles[64]);
    MissionUtility::RemoveObject(mHandles[65]);
    MissionUtility::RemoveObject(mHandles[66]);
    MissionUtility::RemoveObject(mHandles[403]);
    MissionUtility::RemoveObject(mHandles[404]);
    MissionUtility::RemoveObject(mHandles[405]);
    MissionUtility::RemoveObject(mHandles[406]);
    MissionUtility::RemoveObject(mHandles[407]);
    MissionUtility::RemoveObject(mHandles[408]);
    MissionUtility::RemoveObject(mHandles[409]);
    MissionUtility::RemoveObject(mHandles[410]);
    MissionUtility::RemoveObject(mHandles[411]);
    MissionUtility::RemoveObject(mHandles[412]);
    MissionUtility::RemoveObject(mHandles[413]);
    MissionUtility::RemoveObject(mHandles[414]);
    MissionUtility::RemoveObject(mHandles[95]);
    MissionUtility::RemoveObject(mHandles[96]);
    MissionUtility::RemoveObject(mHandles[92]);
    MissionUtility::RemoveObject(mHandles[93]);
    MissionUtility::RemoveObject(mHandles[94]);
    MissionUtility::RemoveObject(mHandles[415]);
    MissionUtility::RemoveObject(mHandles[416]);
    MissionUtility::RemoveObject(mHandles[417]);
    MissionUtility::RemoveObject(mHandles[418]);
    MissionUtility::RemoveObject(mHandles[103]);
    MissionUtility::RemoveObject(mHandles[104]);
    MissionUtility::RemoveObject(mHandles[105]);
    MissionUtility::RemoveObject(mHandles[106]);
    MissionUtility::RemoveObject(mHandles[107]);
    MissionUtility::RemoveObject(mHandles[108]);
    MissionUtility::RemoveObject(mHandles[5]);
    MissionUtility::RemoveObject(mHandles[6]);
    MissionUtility::EvictConfig("thu_bldg_hangar");
    }
    if (savePoint == 2) {
    mFlags[167] = true;
    mFlags[168] = true;
    mFlags[171] = true;
    mFlags[80] = true;
    MissionUtility::RemoveObject(mHandles[191]);
    MissionUtility::RemoveObject(mHandles[192]);
    MissionUtility::RemoveObject(mHandles[193]);
    mInts[0] = MissionUtility::AddObjective("missions.Thule2.objective.str0000");
    MissionUtility::ObjectiveComplete(mInts[0]);
    mInts[1] = MissionUtility::AddObjective("missions.Thule2.objective.str0004");
    MissionUtility::ObjectiveComplete(mInts[1]);
    MissionUtility::Objectify("go", 0, "missions.Thule2.marker.str0002", true, false, 0.0f, 2.0f);
    mInts[2] = MissionUtility::AddObjective("missions.Thule2.objective.str0005");
    MissionUtility::RemoveObject(mHandles[123]);
    MissionUtility::RemoveObject(mHandles[124]);
    MissionUtility::RemoveObject(mHandles[125]);
    MissionUtility::SetCurHealth(mHandles[69], 10.0f);
    MissionUtility::SetCurHealth(mHandles[70], 10.0f);
    MissionUtility::DamageObject(mHandles[69], 100.0f, 100.0f);
    MissionUtility::DamageObject(mHandles[70], 100.0f, 100.0f);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mFlags[113]);
    MissionUtility::MidMissionLoad(mFlags[116]);
    MissionUtility::MidMissionLoad(mFlags[114]);
    MissionUtility::MidMissionLoad(mFlags[117]);
    MissionUtility::MidMissionLoad(mFlags[115]);
    MissionUtility::MidMissionLoad(mFlags[118]);
    MissionUtility::MidMissionLoad(mFlags[173]);
    MissionUtility::MidMissionLoad(mFlags[174]);
    if (mFlags[113]) {
    MissionUtility::BonusObjectiveComplete(mInts[6], true);
    }
    if (mFlags[116]) {
    MissionUtility::BonusObjectiveFailed(mInts[6]);
    }
    if (mFlags[114]) {
    MissionUtility::BonusObjectiveComplete(mInts[7], true);
    }
    if (mFlags[117]) {
    MissionUtility::BonusObjectiveFailed(mInts[7]);
    }
    if (mFlags[115]) {
    MissionUtility::BonusObjectiveComplete(mInts[8], true);
    }
    if (mFlags[118]) {
    MissionUtility::BonusObjectiveFailed(mInts[8]);
    }
    mHandles[13] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_player", "section3_player_spawn", 0, "player_tank", 1, -1, -1);
    MissionUtility::SetAsPlayer(mHandles[13], 0);
    mHandles[7] = MissionUtility::GetPlayerHandle(0);
    MissionUtility::SetTeamNum(mHandles[7], 1);
    mHandles[16] = MissionUtility::CreateSquad(mHandles[7], (Formation)1);
    if (mFlags[173]) {
    mHandles[14] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_squadmate", "section3_squadmate1_spawn", 0, "tank_squadmate1", 1, -1, -1);
    MissionUtility::AddSquadMember(mHandles[16], mHandles[14]);
    }
    if (mFlags[174]) {
    mHandles[15] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_squadmate", "section3_squadmate2_spawn", 0, "tank_squadmate2", 1, -1, -1);
    MissionUtility::AddSquadMember(mHandles[16], mHandles[15]);
    }
    MissionUtility::RemoveObject(mHandles[10]);
    MissionUtility::RemoveObject(mHandles[55]);
    MissionUtility::RemoveObject(mHandles[56]);
    MissionUtility::EvictConfig("rep_walk_assault_player");
    MissionUtility::EvictConfig("rep_walk_assault_squadmate");
    MissionUtility::RemoveObject(mHandles[164]);
    MissionUtility::RemoveObject(mHandles[397]);
    MissionUtility::RemoveObject(mHandles[398]);
    MissionUtility::RemoveObject(mHandles[399]);
    MissionUtility::RemoveObject(mHandles[57]);
    MissionUtility::RemoveObject(mHandles[58]);
    MissionUtility::RemoveObject(mHandles[59]);
    MissionUtility::RemoveObject(mHandles[60]);
    MissionUtility::RemoveObject(mHandles[61]);
    MissionUtility::RemoveObject(mHandles[62]);
    MissionUtility::RemoveObject(mHandles[63]);
    MissionUtility::RemoveObject(mHandles[64]);
    MissionUtility::RemoveObject(mHandles[65]);
    MissionUtility::RemoveObject(mHandles[66]);
    MissionUtility::RemoveObject(mHandles[403]);
    MissionUtility::RemoveObject(mHandles[404]);
    MissionUtility::RemoveObject(mHandles[405]);
    MissionUtility::RemoveObject(mHandles[406]);
    MissionUtility::RemoveObject(mHandles[407]);
    MissionUtility::RemoveObject(mHandles[408]);
    MissionUtility::RemoveObject(mHandles[409]);
    MissionUtility::RemoveObject(mHandles[410]);
    MissionUtility::RemoveObject(mHandles[411]);
    MissionUtility::RemoveObject(mHandles[412]);
    MissionUtility::RemoveObject(mHandles[413]);
    MissionUtility::RemoveObject(mHandles[414]);
    MissionUtility::RemoveObject(mHandles[95]);
    MissionUtility::RemoveObject(mHandles[96]);
    MissionUtility::RemoveObject(mHandles[92]);
    MissionUtility::RemoveObject(mHandles[93]);
    MissionUtility::RemoveObject(mHandles[94]);
    MissionUtility::RemoveObject(mHandles[415]);
    MissionUtility::RemoveObject(mHandles[416]);
    MissionUtility::RemoveObject(mHandles[417]);
    MissionUtility::RemoveObject(mHandles[418]);
    MissionUtility::RemoveObject(mHandles[103]);
    MissionUtility::RemoveObject(mHandles[104]);
    MissionUtility::RemoveObject(mHandles[105]);
    MissionUtility::RemoveObject(mHandles[106]);
    MissionUtility::RemoveObject(mHandles[107]);
    MissionUtility::RemoveObject(mHandles[108]);
    MissionUtility::RemoveObject(mHandles[5]);
    MissionUtility::RemoveObject(mHandles[6]);
    MissionUtility::EvictConfig("thu_bldg_hangar");
    MissionUtility::AddHealthBox("add_health_here", 0, -1.0f);
    MissionUtility::AddAmmoBox("add_ammo_here", 0, -1.0f);
    MissionUtility::PlayMusic("EP4_V2_T09_02", true);
    }
    if (savePoint == 3) {
    mFlags[167] = true;
    mFlags[168] = true;
    mFlags[169] = true;
    mFlags[80] = true;
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mFlags[113]);
    MissionUtility::MidMissionLoad(mFlags[116]);
    MissionUtility::MidMissionLoad(mFlags[114]);
    MissionUtility::MidMissionLoad(mFlags[117]);
    MissionUtility::MidMissionLoad(mFlags[115]);
    MissionUtility::MidMissionLoad(mFlags[118]);
    if (mFlags[113]) {
    MissionUtility::BonusObjectiveComplete(mInts[6], false);
    }
    if (mFlags[116]) {
    MissionUtility::BonusObjectiveFailed(mInts[6]);
    }
    if (mFlags[114]) {
    MissionUtility::BonusObjectiveComplete(mInts[7], false);
    }
    if (mFlags[117]) {
    MissionUtility::BonusObjectiveFailed(mInts[7]);
    }
    if (mFlags[115]) {
    MissionUtility::BonusObjectiveComplete(mInts[8], false);
    }
    if (mFlags[118]) {
    MissionUtility::BonusObjectiveFailed(mInts[8]);
    }
    MissionUtility::SetMaxHealth(mHandles[384], 9999999.0f);
    MissionUtility::SetCurHealth(mHandles[384], 9999999.0f);
    MissionUtility::BeginWave("jedi_wave");
    mHandles[276] = MissionUtility::CreateObject("rep_fly_assault", Vector(1988.8434f, 0.953394f, -1967.729f), "", 0, -1, Quat(0.777612f, 0.0f, 0.628744f, 0.0f), -1);
    MissionUtility::SetApplyDynamics(mHandles[276], true);
    MissionUtility::Land(mHandles[276], 0, 0, 80.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[276], 0.001f);
    MissionUtility::SetVelocNeutralFly(mHandles[276], 0.001f);
    MissionUtility::SetVelocMaximumFly(mHandles[276], 0.001f);
    mHandles[150] = MissionUtility::CreateObject("cis_tank_assault", "gate_aat1_spawn", "", 8, -1, -1);
    mHandles[151] = MissionUtility::CreateObject("cis_tank_assault", "gate_aat2_spawn", "", 8, -1, -1);
    mHandles[152] = MissionUtility::CreateObject("cis_tank_assault", "gate_aat3_spawn", "", 8, -1, -1);
    mHandles[153] = MissionUtility::CreateObject("cis_tank_assault", "gate_aat4_spawn", "", 8, -1, -1);
    mHandles[154] = MissionUtility::CreateObject("cis_tank_fighter", "gate_treaded1_spawn", "", 8, -1, -1);
    mHandles[155] = MissionUtility::CreateObject("cis_tank_fighter", "gate_treaded2_spawn", "", 8, -1, -1);
    mHandles[156] = MissionUtility::CreateObject("cis_tank_assault", "gate_walker1_spawn", "", 8, -1, -1);
    mHandles[157] = MissionUtility::CreateObject("cis_tank_assault", "gate_walker2_spawn", "", 8, -1, -1);
    mHandles[158] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "rep_initial_spawn1", 0, "", 5, -1, -1);
    mHandles[159] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "rep_initial_spawn2", 0, "", 5, -1, -1);
    MissionUtility::AttackTarget(mHandles[158], mHandles[154], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[159], mHandles[155], true, true, false, false);
    MissionUtility::Stop(mHandles[150]);
    MissionUtility::Stop(mHandles[151]);
    MissionUtility::Stop(mHandles[152]);
    MissionUtility::Stop(mHandles[153]);
    MissionUtility::Stop(mHandles[154]);
    MissionUtility::Stop(mHandles[155]);
    MissionUtility::Stop(mHandles[156]);
    MissionUtility::Stop(mHandles[157]);
    MissionUtility::SetMaxHealth(mHandles[150], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[150], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[151], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[151], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[152], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[152], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[153], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[153], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[154], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[154], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[155], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[155], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[156], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[156], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[157], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[157], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[109], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[109], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[110], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[110], 999999.0f);
    mHandles[287] = MissionUtility::AddPropArmy(0, "end_clone_army1", 0, 50, 2.0f);
    mHandles[295] = MissionUtility::AddPropArmy(1, "end_droid_army1", 0, 50, 2.0f);
    mHandles[288] = MissionUtility::AddPropArmy(0, "end_clone_army2", 0, 50, 2.0f);
    mHandles[296] = MissionUtility::AddPropArmy(1, "end_droid_army2", 0, 50, 2.0f);
    mHandles[289] = MissionUtility::AddPropArmy(0, "end_clone_army3", 0, 50, 2.0f);
    mHandles[297] = MissionUtility::AddPropArmy(1, "end_droid_army3", 0, 50, 2.0f);
    mHandles[290] = MissionUtility::AddPropArmy(0, "end_clone_army4", 0, 50, 2.0f);
    mHandles[298] = MissionUtility::AddPropArmy(1, "end_droid_army4", 0, 50, 2.0f);
    mHandles[291] = MissionUtility::AddPropArmy(0, "end_clone_army5", 0, 50, 2.0f);
    mHandles[299] = MissionUtility::AddPropArmy(1, "end_droid_army5", 0, 50, 2.0f);
    mHandles[292] = MissionUtility::AddPropArmy(0, "end_clone_army6", 0, 50, 2.0f);
    mHandles[300] = MissionUtility::AddPropArmy(1, "end_droid_army6", 0, 50, 2.0f);
    mHandles[293] = MissionUtility::AddPropArmy(0, "end_clone_army7", 0, 50, 2.0f);
    mHandles[301] = MissionUtility::AddPropArmy(1, "end_droid_army7", 0, 50, 2.0f);
    mHandles[294] = MissionUtility::AddPropArmy(0, "end_clone_army8", 0, 50, 2.0f);
    mHandles[302] = MissionUtility::AddPropArmy(1, "end_droid_army8", 0, 50, 2.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[287], "end_clone_army1", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[288], "end_clone_army2", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[289], "end_clone_army3", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[290], "end_clone_army4", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[291], "end_clone_army5", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[292], "end_clone_army6", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[293], "end_clone_army7", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[294], "end_clone_army8", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[295], "end_droid_army1", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[296], "end_droid_army2", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[297], "end_droid_army3", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[298], "end_droid_army4", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[299], "end_droid_army5", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[300], "end_droid_army6", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[301], "end_droid_army7", true);
    MissionUtility::SetPropArmyWayPoints(mHandles[302], "end_droid_army8", true);
    MissionUtility::RemoveObject(mHandles[164]);
    MissionUtility::RemoveObject(mHandles[397]);
    MissionUtility::RemoveObject(mHandles[398]);
    MissionUtility::RemoveObject(mHandles[399]);
    MissionUtility::RemoveObject(mHandles[57]);
    MissionUtility::RemoveObject(mHandles[58]);
    MissionUtility::RemoveObject(mHandles[59]);
    MissionUtility::RemoveObject(mHandles[60]);
    MissionUtility::RemoveObject(mHandles[61]);
    MissionUtility::RemoveObject(mHandles[62]);
    MissionUtility::RemoveObject(mHandles[63]);
    MissionUtility::RemoveObject(mHandles[64]);
    MissionUtility::RemoveObject(mHandles[65]);
    MissionUtility::RemoveObject(mHandles[66]);
    MissionUtility::RemoveObject(mHandles[403]);
    MissionUtility::RemoveObject(mHandles[404]);
    MissionUtility::RemoveObject(mHandles[405]);
    MissionUtility::RemoveObject(mHandles[406]);
    MissionUtility::RemoveObject(mHandles[407]);
    MissionUtility::RemoveObject(mHandles[408]);
    MissionUtility::RemoveObject(mHandles[409]);
    MissionUtility::RemoveObject(mHandles[410]);
    MissionUtility::RemoveObject(mHandles[411]);
    MissionUtility::RemoveObject(mHandles[412]);
    MissionUtility::RemoveObject(mHandles[413]);
    MissionUtility::RemoveObject(mHandles[414]);
    MissionUtility::RemoveObject(mHandles[95]);
    MissionUtility::RemoveObject(mHandles[96]);
    MissionUtility::RemoveObject(mHandles[92]);
    MissionUtility::RemoveObject(mHandles[93]);
    MissionUtility::RemoveObject(mHandles[94]);
    MissionUtility::RemoveObject(mHandles[415]);
    MissionUtility::RemoveObject(mHandles[416]);
    MissionUtility::RemoveObject(mHandles[417]);
    MissionUtility::RemoveObject(mHandles[418]);
    MissionUtility::RemoveObject(mHandles[103]);
    MissionUtility::RemoveObject(mHandles[104]);
    MissionUtility::RemoveObject(mHandles[105]);
    MissionUtility::RemoveObject(mHandles[106]);
    MissionUtility::RemoveObject(mHandles[107]);
    MissionUtility::RemoveObject(mHandles[108]);
    MissionUtility::RemoveObject(mHandles[5]);
    MissionUtility::RemoveObject(mHandles[6]);
    MissionUtility::EvictConfig("thu_bldg_hangar");
    mTimes[18] = 5.0f + MissionUtility::GetTime();
    mInts[0] = MissionUtility::AddObjective("missions.Thule2.objective.str0000");
    MissionUtility::ObjectiveComplete(mInts[0]);
    mInts[1] = MissionUtility::AddObjective("missions.Thule2.objective.str0004");
    MissionUtility::ObjectiveComplete(mInts[1]);
    mInts[2] = MissionUtility::AddObjective("missions.Thule2.objective.str0005");
    MissionUtility::ObjectiveComplete(mInts[2]);
    mInts[3] = MissionUtility::AddObjective("missions.Thule2.objective.str0007");
    MissionUtility::ObjectiveComplete(mInts[3]);
    mFlags[209] = true;
    }
    MissionUtility::StartAmbiences("AmbThule_Main01_pl2", "PropGen_thunderLt01", 10.0f, 30.0f);
    MissionUtility::SetSquadAttackSound("squad_attacktarget");
    MissionUtility::SetSquadBreakSound("squad_breakandattack");
    MissionUtility::SetSquadHoldSound("squad_holdposition");
    MissionUtility::SetSquadRegroupSound("squad_regroup");
    MissionUtility::SetSquadInvalidSound("squad_invalidtarget");
    mFlags[15] = true;
    }

    // ---- +0x110c  16612 bytes ----
    if (!mFlags[167]) {
        if (!mFlags[92]) {
        if (!mFlags[93]) {
        MissionUtility::PlayMusic("EP2_V1_T13_01", true);
        mHandles[429] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap1_spawn", 0, "", 2, -1, -1);
        mHandles[430] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap1_spawn", 1, "", 2, -1, -1);
        mHandles[431] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap1_spawn", 2, "", 2, -1, -1);
        mHandles[423] = MissionUtility::CreateFlock(mHandles[429], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[423], mHandles[430]);
        MissionUtility::AddFlockMember(mHandles[423], mHandles[431]);
        mHandles[432] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap2_spawn", 0, "", 2, -1, -1);
        mHandles[433] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap2_spawn", 1, "", 2, -1, -1);
        mHandles[434] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap2_spawn", 2, "", 2, -1, -1);
        mHandles[424] = MissionUtility::CreateFlock(mHandles[432], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[424], mHandles[433]);
        MissionUtility::AddFlockMember(mHandles[424], mHandles[434]);
        mHandles[435] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap3_spawn", 0, "", 2, -1, -1);
        mHandles[436] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap3_spawn", 1, "", 2, -1, -1);
        mHandles[437] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap3_spawn", 2, "", 2, -1, -1);
        mHandles[425] = MissionUtility::CreateFlock(mHandles[435], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[425], mHandles[436]);
        MissionUtility::AddFlockMember(mHandles[425], mHandles[437]);
        mHandles[438] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap4_spawn", 0, "", 2, -1, -1);
        mHandles[439] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap4_spawn", 1, "", 2, -1, -1);
        mHandles[440] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap4_spawn", 2, "", 2, -1, -1);
        mHandles[426] = MissionUtility::CreateFlock(mHandles[438], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[426], mHandles[439]);
        MissionUtility::AddFlockMember(mHandles[426], mHandles[440]);
        mHandles[441] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap5_spawn", 0, "", 2, -1, -1);
        mHandles[442] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap5_spawn", 1, "", 2, -1, -1);
        mHandles[443] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap5_spawn", 2, "", 2, -1, -1);
        mHandles[427] = MissionUtility::CreateFlock(mHandles[441], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[427], mHandles[442]);
        MissionUtility::AddFlockMember(mHandles[427], mHandles[443]);
        mHandles[444] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap6_spawn", 0, "", 2, -1, -1);
        mHandles[445] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap6_spawn", 1, "", 2, -1, -1);
        mHandles[446] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap6_spawn", 2, "", 2, -1, -1);
        mHandles[428] = MissionUtility::CreateFlock(mHandles[444], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[428], mHandles[445]);
        MissionUtility::AddFlockMember(mHandles[428], mHandles[446]);
        MissionUtility::Stop(mHandles[95]);
        MissionUtility::Stop(mHandles[96]);
        MissionUtility::Stop(mHandles[92]);
        MissionUtility::Stop(mHandles[93]);
        MissionUtility::Stop(mHandles[94]);
        MissionUtility::Stop(mHandles[103]);
        MissionUtility::Stop(mHandles[104]);
        MissionUtility::Stop(mHandles[105]);
        MissionUtility::Stop(mHandles[106]);
        MissionUtility::Stop(mHandles[107]);
        MissionUtility::Stop(mHandles[108]);
        MissionUtility::SetMaxHealth(mHandles[5], 4000.0f);
        MissionUtility::SetCurHealth(mHandles[5], 4000.0f);
        MissionUtility::SetMaxHealth(mHandles[6], 4000.0f);
        MissionUtility::SetCurHealth(mHandles[6], 4000.0f);
        MissionUtility::Stop(mHandles[57]);
        MissionUtility::Stop(mHandles[58]);
        MissionUtility::Stop(mHandles[59]);
        MissionUtility::Stop(mHandles[60]);
        MissionUtility::Stop(mHandles[61]);
        MissionUtility::Stop(mHandles[62]);
        MissionUtility::Stop(mHandles[63]);
        MissionUtility::Stop(mHandles[64]);
        MissionUtility::Stop(mHandles[65]);
        MissionUtility::Stop(mHandles[66]);
        MissionUtility::SetWeaponPitch(mHandles[57], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[57], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[58], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[58], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[59], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[59], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[60], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[60], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[61], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[61], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[62], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[62], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[63], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[63], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[64], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[64], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[65], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[65], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponPitch(mHandles[66], "cis_mortar_assault", 0.2f);
        MissionUtility::SetWeaponYaw(mHandles[66], "cis_mortar_assault", 0.2f);
        mHandles[54] = MissionUtility::CreateSquad(mHandles[7], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[54], mHandles[55]);
        MissionUtility::AddSquadMember(mHandles[54], mHandles[56]);
        MissionUtility::Land(mHandles[164], 0, 0, 80.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[164], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[164], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[164], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[191], 270.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[191], 270.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[191], 270.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[192], 270.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[192], 270.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[192], 270.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[193], 270.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[193], 270.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[193], 270.0f);
        MissionUtility::SetCollidable(mHandles[191], false);
        MissionUtility::SetCollidable(mHandles[192], false);
        MissionUtility::SetCollidable(mHandles[193], false);
        mHandles[51] = MissionUtility::RunCin("OpenCin", true, true);
        MissionUtility::SetVisible(mHandles[55], false);
        MissionUtility::SetVisible(mHandles[56], false);
        MissionUtility::SetApplyDynamics(mHandles[510], true);
        MissionUtility::Land(mHandles[510], 0, 0, 80.0f);
        mHandles[511] = MissionUtility::CreateObjectWithRotation("rep_walk_assault", "OpenCinWalkerPath", 0, "OpenCinWalker", 1, -1, -1);
        mHandles[512] = MissionUtility::CreateObjectWithRotation("rep_walk_assault", "OpenCinWalkerPath1", 0, "OpenCinWalker1", 1, -1, -1);
        MissionUtility::Goto(mHandles[511], "OpenCinWalkerPath", false);
        MissionUtility::Goto(mHandles[512], "OpenCinWalkerPath1", false);
        MissionUtility::MoveObjectWithRotation(mHandles[191], "OpenCinFighter1Path", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[192], "OpenCinFighter2Path", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[193], "OpenCinFighter3Path", 0, true);
        MissionUtility::GotoFire(mHandles[191], "OpenCinFighter1Path", false, true, true, false);
        MissionUtility::GotoFire(mHandles[192], "OpenCinFighter2Path", false, true, true, false);
        MissionUtility::GotoFire(mHandles[193], "OpenCinFighter3Path", false, true, true, false);
        mTimes[40] = 3.0f + MissionUtility::GetTime();
        mHandles[513] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinNewFighterPath", 0, "OpenCinNewFighter", 1, -1, -1);
        MissionUtility::Goto(mHandles[513], "OpenCinNewFighterPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[513], "OpenCinNewFighterPath", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::OverrideSoundRange(mHandles[510], true);
        MissionUtility::OverrideSoundRange(mHandles[511], true);
        MissionUtility::OverrideSoundRange(mHandles[512], true);
        MissionUtility::OverrideSoundRange(mHandles[191], true);
        MissionUtility::OverrideSoundRange(mHandles[192], true);
        MissionUtility::OverrideSoundRange(mHandles[193], true);
        MissionUtility::OverrideSoundRange(mHandles[513], true);
        mFlags[93] = true;
        mHandles[490] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath1a", 0, 20, 2.0f);
        mHandles[491] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath2a", 0, 20, 2.0f);
        mHandles[492] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath3a", 0, 30, 2.0f);
        mHandles[493] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath4a", 0, 25, 2.0f);
        mHandles[486] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath1", 0, 100, 1.0f);
        mHandles[487] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath2", 0, 100, 1.0f);
        mHandles[488] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath3", 0, 100, 1.0f);
        mHandles[489] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath4", 0, 100, 1.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[490], "OpenCinCloneArmyPath1c", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[491], "OpenCinCloneArmyPath2c", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[492], "OpenCinCloneArmyPath3c", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[493], "OpenCinCloneArmyPath4c", true);
        BeginTimer(mTimer16);
        }
        if (!mFlags[222]) {
        if (mTimer14 > 0.2f) {
        MissionUtility::QueueSound("lmt22_02a", 1.0f, 0.0f, 0.0f, "", 0, "");
        StopTimer(mTimer14);
        mTimer14 = 0.0f;
        }
        }
        if (!mFlags[222]) {
        if (!mFlags[217]) {
        if (MissionUtility::GetCinId(mHandles[51]) == 3) {
        MissionUtility::MoveObjectWithRotation(mHandles[513], "OpenCinNewFighterPath", 0, true);
        mHandles[514] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinNewFighterPath1", 0, "OpenCinNewFighter1", 1, -1, -1);
        mHandles[515] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "OpenCinNewFighterPath2", 0, "OpenCinNewFighter2", 1, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[514], true);
        MissionUtility::OverrideSoundRange(mHandles[515], true);
        MissionUtility::SetVelocMinimumFly(mHandles[514], 250.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[514], 250.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[514], 250.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[515], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[515], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[515], 200.0f);
        MissionUtility::Goto(mHandles[513], "OpenCinNewFighterPath", false);
        MissionUtility::Goto(mHandles[514], "OpenCinNewFighterPath1", false);
        MissionUtility::Goto(mHandles[515], "OpenCinNewFighterPath2", false);
        mTimes[23] = 1.0f + MissionUtility::GetTime();
        mFlags[217] = true;
        }
        }
        }
        if (!mFlags[222]) {
        if (MissionUtility::IsAlive(mHandles[514])) {
        if (MissionUtility::GetDistance(mHandles[514], "OpenCinNewFighterPath1", 3) < 40.0f) {
        MissionUtility::DamageObject(mHandles[514], 5000.0f, 5000.0f);
        }
        }
        }
        if (!mFlags[222]) {
        if (MissionUtility::IsAlive(mHandles[515])) {
        if (MissionUtility::GetDistance(mHandles[515], "OpenCinNewFighterPath2", 3) < 40.0f) {
        MissionUtility::DamageObject(mHandles[515], 5000.0f, 5000.0f);
        }
        }
        }
        if (!mFlags[222]) {
        if (!mFlags[95]) {
        if (mTimes[23] < MissionUtility::GetTime()) {
        mHandles[194] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "cin_flyer_spawn1", 0, "cin_flyer1", 3, -1, -1);
        mHandles[195] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "cin_flyer_spawn2", 0, "cin_flyer2", 3, -1, -1);
        MissionUtility::Goto(mHandles[194], "cin_flyers_go", true);
        MissionUtility::Goto(mHandles[195], "cin_flyers_go", true);
        mTimes[24] = 3.0f + MissionUtility::GetTime();
        mFlags[95] = true;
        }
        }
        }
        if (!mFlags[222]) {
        if (!mFlags[96]) {
        if (mTimes[24] < MissionUtility::GetTime()) {
        mHandles[196] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "cin_flyer_spawn1", 0, "cin_flyer3", 3, -1, -1);
        mHandles[197] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "cin_flyer_spawn2", 0, "cin_flyer4", 3, -1, -1);
        MissionUtility::Goto(mHandles[196], "cin_flyers_go", true);
        MissionUtility::Goto(mHandles[197], "cin_flyers_go", true);
        mTimes[25] = 3.0f + MissionUtility::GetTime();
        mFlags[96] = true;
        MissionUtility::MoveObjectWithRotation(mHandles[191], "OpenCinFighter1Patha", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[192], "OpenCinFighter2Patha", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[193], "OpenCinFighter3Patha", 0, true);
        MissionUtility::GotoFire(mHandles[191], "OpenCinFighter1Patha", false, true, true, false);
        MissionUtility::GotoFire(mHandles[192], "OpenCinFighter2Patha", false, true, true, false);
        MissionUtility::GotoFire(mHandles[193], "OpenCinFighter3Patha", false, true, true, false);
        }
        }
        }
        if (!mFlags[222]) {
        if (!mFlags[97]) {
        if (mTimes[25] < MissionUtility::GetTime()) {
        mHandles[198] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "cin_flyer_spawn1", 0, "cin_flyer5", 3, -1, -1);
        mHandles[199] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "cin_flyer_spawn2", 0, "cin_flyer6", 3, -1, -1);
        MissionUtility::Goto(mHandles[198], "cin_flyers_go", true);
        MissionUtility::Goto(mHandles[199], "cin_flyers_go", true);
        mFlags[97] = true;
        }
        }
        }
        if (!mFlags[222]) {
        if (mTimer11 > 10.0f) {
        StopTimer(mTimer11);
        mTimer11 = 0.0f;
        }
        }
        if (!mFlags[222]) {
        if (!mFlags[213]) {
        if (MissionUtility::GetCinId(mHandles[51]) == 4) {
        MissionUtility::RemoveObject(mHandles[191]);
        MissionUtility::RemoveObject(mHandles[192]);
        MissionUtility::RemoveObject(mHandles[193]);
        MissionUtility::DamageObject(mHandles[513], 5000.0f, 5000.0f);
        MissionUtility::Goto(mHandles[513], "OpenCinNewFighterPath", false);
        mFlags[213] = true;
        }
        }
        }
        if (!mFlags[222]) {
        if (mTimer10 > 4.0f) {
        StopTimer(mTimer10);
        mTimer10 = 0.0f;
        }
        }
        if (!mFlags[222]) {
        if (!mFlags[215]) {
        if (MissionUtility::GetCinId(mHandles[51]) == 5) {
        mHandles[494] = MissionUtility::CreateObjectWithRotation("rep_inf_mace", "OpenCinMacePath", 0, "OpenCinMace", 0, -1, -1);
        BeginTimer(mTimer10);
        MissionUtility::QueueSound("mwt22_09", 1.0f, 0.0f, 0.0f, "", mHandles[494], "talk01");
        MissionUtility::MoveObjectWithRotation(mHandles[495], "OpenCinTankPath", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[496], "OpenCinTankPath", 1, true);
        MissionUtility::MoveObjectWithRotation(mHandles[497], "OpenCinTankPath", 2, true);
        MissionUtility::MoveObjectWithRotation(mHandles[498], "OpenCinTankPath1", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[499], "OpenCinTankPath1", 1, true);
        MissionUtility::MoveObjectWithRotation(mHandles[500], "OpenCinTankPath1", 2, true);
        mFlags[215] = true;
        }
        }
        }
        if (!mFlags[222]) {
        if (!mFlags[216]) {
        if (MissionUtility::GetCinId(mHandles[51]) == 7) {
        MissionUtility::Goto(mHandles[494], "OpenCinMacePath", false);
        MissionUtility::QueueSound("mwt22_10", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetPropArmyWayPoints(mHandles[486], "OpenCinCloneArmyPath1b", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[487], "OpenCinCloneArmyPath1b", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[488], "OpenCinCloneArmyPath1b", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[489], "OpenCinCloneArmyPath1b", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[490], "OpenCinCloneArmyPath1b", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[491], "OpenCinCloneArmyPath1b", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[492], "OpenCinCloneArmyPath1b", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[493], "OpenCinCloneArmyPath1b", true);
        MissionUtility::Goto(mHandles[495], "OpenCinTankPath", false);
        MissionUtility::Goto(mHandles[496], "OpenCinTankPath", false);
        MissionUtility::Goto(mHandles[497], "OpenCinTankPath", false);
        MissionUtility::Goto(mHandles[498], "OpenCinTankPath1", false);
        MissionUtility::Goto(mHandles[499], "OpenCinTankPath1", false);
        MissionUtility::Goto(mHandles[500], "OpenCinTankPath1", false);
        MissionUtility::Goto(mHandles[511], mHandles[55]);
        MissionUtility::Goto(mHandles[512], mHandles[56]);
        mFlags[216] = true;
        }
        }
        }
        if (mTimes[40] < MissionUtility::GetTime()) {
        MissionUtility::Goto(mHandles[191], "OpenCinFighter1Path", false);
        MissionUtility::Goto(mHandles[192], "OpenCinFighter2Path", false);
        MissionUtility::Goto(mHandles[193], "OpenCinFighter3Path", false);
        MissionUtility::AttackTarget(mHandles[192], mHandles[164], true, true, false, false);
        mTimes[40] = 999999.9f;
        }
        if (!mFlags[222]) {
        if (!mFlags[94]) {
        if (mFlags[93]) {
        if (MissionUtility::GetCinId(mHandles[51]) == 2) {
        MissionUtility::QueueSound("lmt22_02a", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::MoveObjectWithRotation(mHandles[191], "OpenCinFighter1Patha", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[192], "OpenCinFighter2Patha", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[193], "OpenCinFighter3Patha", 0, true);
        MissionUtility::Goto(mHandles[191], "OpenCinFighter1Patha", false);
        MissionUtility::Goto(mHandles[192], "OpenCinFighter2Patha", false);
        MissionUtility::Goto(mHandles[193], "OpenCinFighter3Patha", false);
        mFlags[94] = true;
        }
        }
        }
        }
        if (!mFlags[222]) {
        if (mTimer16 > 2.0f) {
        mInts[13] = mInts[13] + 1;
        switch (mInts[13]) {
        case 1:
        mHandles[495] = MissionUtility::CreateObject("rep_tank_fighter1", mHandles[164], "hp_spawnpoint_r", "OpenCinTank", 1, -1, -1);
        MissionUtility::Goto(mHandles[495], "OpenCinTankRampPath", false);
        break;
        case 2:
        mHandles[496] = MissionUtility::CreateObject("rep_tank_fighter1", mHandles[164], "hp_spawnpoint_r", "", 1, -1, -1);
        MissionUtility::Goto(mHandles[496], "OpenCinTankRampPath", false);
        break;
        case 3:
        mHandles[497] = MissionUtility::CreateObject("rep_tank_fighter1", mHandles[164], "hp_spawnpoint_r", "", 1, -1, -1);
        MissionUtility::Goto(mHandles[497], "OpenCinTankRampPath", false);
        break;
        case 4:
        mHandles[498] = MissionUtility::CreateObject("rep_tank_fighter1", mHandles[164], "hp_spawnpoint_r", "", 1, -1, -1);
        MissionUtility::Goto(mHandles[498], "OpenCinTankRampPath", false);
        break;
        case 5:
        mHandles[499] = MissionUtility::CreateObject("rep_tank_fighter1", mHandles[164], "hp_spawnpoint_r", "", 1, -1, -1);
        MissionUtility::Goto(mHandles[499], "OpenCinTankRampPath", false);
        break;
        case 6:
        mHandles[500] = MissionUtility::CreateObject("rep_tank_fighter1", mHandles[164], "hp_spawnpoint_r", "", 1, -1, -1);
        MissionUtility::Goto(mHandles[500], "OpenCinTankRampPath", false);
        StopTimer(mTimer16);
        break;
        }
        mTimer16 = 0.0f;
        }
        }
        if (mFlags[93]) {
        if (!MissionUtility::IsCinRunning(mHandles[51])) {
        mFlags[222] = true;
        MissionUtility::FlushSoundQueue();
        MissionUtility::SetVisible(mHandles[55], true);
        MissionUtility::SetVisible(mHandles[56], true);
        MissionUtility::RemoveObject(mHandles[495]);
        MissionUtility::RemoveObject(mHandles[496]);
        MissionUtility::RemoveObject(mHandles[497]);
        MissionUtility::RemoveObject(mHandles[498]);
        MissionUtility::RemoveObject(mHandles[499]);
        MissionUtility::RemoveObject(mHandles[500]);
        MissionUtility::RemoveObject(mHandles[511]);
        MissionUtility::RemoveObject(mHandles[512]);
        MissionUtility::RemoveObject(mHandles[513]);
        MissionUtility::RemoveArmy(mHandles[486]);
        MissionUtility::RemoveArmy(mHandles[487]);
        MissionUtility::RemoveArmy(mHandles[488]);
        MissionUtility::RemoveArmy(mHandles[489]);
        MissionUtility::RemoveArmy(mHandles[490]);
        MissionUtility::RemoveArmy(mHandles[491]);
        MissionUtility::RemoveArmy(mHandles[492]);
        MissionUtility::RemoveArmy(mHandles[493]);
        MissionUtility::RemoveObject(mHandles[494]);
        MissionUtility::Objectify(mHandles[6], "missions.Thule2.marker.str0000", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[5], "missions.Thule2.marker.str0000", false, true, 0.0f);
        mInts[0] = MissionUtility::AddObjective("missions.Thule2.objective.str0000");
        MissionUtility::DisplayText("missions.Thule2.text.str0000", 6.0f, -1.0f);
        MissionUtility::RemoveObject(mHandles[191]);
        MissionUtility::RemoveObject(mHandles[192]);
        MissionUtility::RemoveObject(mHandles[193]);
        MissionUtility::RemoveObject(mHandles[194]);
        MissionUtility::RemoveObject(mHandles[195]);
        MissionUtility::RemoveObject(mHandles[196]);
        MissionUtility::RemoveObject(mHandles[197]);
        MissionUtility::RemoveObject(mHandles[198]);
        MissionUtility::RemoveObject(mHandles[199]);
        MissionUtility::FlushSoundQueue();
        MissionUtility::SetTeamNum(mHandles[227], 2);
        MissionUtility::SetTeamNum(mHandles[228], 2);
        mHandles[117] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "looping_flyer7_spawn", 0, "looping_flyer7", 3, -1, -1);
        mHandles[118] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "looping_flyer8_spawn", 0, "looping_flyer8", 4, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[117], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[117], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[117], 120.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[118], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[118], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[118], 120.0f);
        MissionUtility::SetMaxHealth(mHandles[117], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[117], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[118], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[118], 999999.0f);
        mHandles[270] = MissionUtility::CreateFlock(mHandles[117], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[270], mHandles[118], Vector(0.0f, 0.0f, 40.0f));
        MissionUtility::Goto(mHandles[270], "looping_flyer78_path", true);
        MissionUtility::RemoveObject(mHandles[164]);
        MissionUtility::PlayMusic("demomusic2", true);
        mHandles[303] = MissionUtility::AddPropArmy(0, "clone_army1", 0, 50, 1.5f);
        mHandles[305] = MissionUtility::AddPropArmy(0, "clone_army2", 0, 50, 2.0f);
        mHandles[304] = MissionUtility::AddPropArmy(1, "droid_army1", 0, 50, 1.0f);
        mHandles[306] = MissionUtility::AddPropArmy(1, "droid_army2", 0, 50, 0.7f);
        mHandles[307] = MissionUtility::AddPropArmy(0, "clone_army3", 0, 30, 1.0f);
        mHandles[308] = MissionUtility::AddPropArmy(1, "droid_army3", 0, 30, 1.0f);
        mHandles[312] = MissionUtility::AddPropArmy(1, "droid_army5", 0, 50, 3.0f);
        mHandles[309] = MissionUtility::AddPropArmy(0, "clone_army4", 0, 50, 2.0f);
        mHandles[311] = MissionUtility::AddPropArmy(0, "clone_army5", 0, 50, 2.0f);
        mHandles[314] = MissionUtility::AddPropArmy(1, "droid_army6", 0, 30, 0.5f);
        mHandles[313] = MissionUtility::AddPropArmy(0, "clone_army6", 0, 30, 0.5f);
        mHandles[316] = MissionUtility::AddPropArmy(1, "droid_army7", 0, 50, 2.0f);
        mHandles[318] = MissionUtility::AddPropArmy(1, "droid_army8", 0, 50, 2.0f);
        mHandles[315] = MissionUtility::AddPropArmy(0, "clone_army7", 0, 50, 2.0f);
        mHandles[317] = MissionUtility::AddPropArmy(0, "clone_army8", 0, 50, 2.0f);
        mHandles[319] = MissionUtility::AddPropArmy(0, "clone_army9_patrol", 0, 50, 1.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[303], "clone_army1_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[305], "clone_army2_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[307], "clone_army3_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[309], "clone_army4_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[311], "clone_army5_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[313], "clone_army6_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[315], "clone_army7_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[317], "clone_army8_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[319], "clone_army9_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[304], "droid_army1_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[306], "droid_army2_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[308], "droid_army3_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[312], "droid_army5_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[314], "droid_army6_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[316], "droid_army7_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[318], "droid_army8_patrol", true);
        mHandles[320] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn1", 0, 3, 0.01f);
        mHandles[321] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn2", 0, 3, 0.01f);
        mHandles[322] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn3", 0, 3, 0.01f);
        mHandles[323] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn4", 0, 3, 0.01f);
        mHandles[324] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn5", 0, 3, 0.01f);
        mHandles[325] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn6", 0, 3, 0.01f);
        mHandles[326] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn7", 0, 3, 0.01f);
        mHandles[327] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn8", 0, 3, 0.01f);
        mHandles[328] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn9", 0, 3, 0.01f);
        mHandles[329] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn10", 0, 3, 0.01f);
        mHandles[330] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn11", 0, 3, 0.01f);
        mHandles[331] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn12", 0, 3, 0.01f);
        MissionUtility::PlayMusic("EP5_V2_T01", true);
        MissionUtility::Stop(mHandles[405]);
        MissionUtility::Stop(mHandles[406]);
        MissionUtility::Stop(mHandles[407]);
        mFlags[92] = true;
        }
        }
        }
        if (!mFlags[53]) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        MissionUtility::Wait(mHandles[405]);
        MissionUtility::Wait(mHandles[406]);
        MissionUtility::Wait(mHandles[407]);
        MissionUtility::SetTeamNum(mHandles[61], 2);
        MissionUtility::SetTeamNum(mHandles[63], 2);
        mFlags[53] = true;
        }
        }
        if (!mFlags[54]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "walker_fall")) {
        MissionUtility::DamageObject(mHandles[7], 999999.0f, 999999.0f);
        if (!MissionUtility::IsAlive(mHandles[7])) {
        mFlags[54] = true;
        }
        }
        }
        if (!mFlags[5]) {
        if (MissionUtility::IsInsideRegion(mHandles[55], "walker_fall")) {
        MissionUtility::DamageObject(mHandles[55], 999999.0f, 999999.0f);
        if (!MissionUtility::IsAlive(mHandles[55])) {
        mFlags[5] = true;
        }
        }
        }
        if (!mFlags[6]) {
        if (MissionUtility::IsInsideRegion(mHandles[56], "walker_fall")) {
        MissionUtility::DamageObject(mHandles[56], 999999.0f, 999999.0f);
        if (!MissionUtility::IsAlive(mHandles[56])) {
        mFlags[6] = true;
        }
        }
        }
        if (!mFlags[164]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "flyer_trigger1")) {
        MissionUtility::Goto(mHandles[397], "spider1_go", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[397]);
        MissionUtility::SetQueueFlag(false);
        mFlags[164] = true;
        }
        }
        if (mFlags[92]) {
        if (!mFlags[81]) {
        if (!mFlags[74]) {
        if (!MissionUtility::IsAlive(mHandles[165])) {
        mHandles[165] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship1_path", 0, "landingship1", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[165], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[165], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[165], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[165], true);
        MissionUtility::Goto(mHandles[165], "landingship1_path", true);
        mFlags[74] = true;
        }
        }
        if (MissionUtility::IsAlive(mHandles[165])) {
        if (MissionUtility::GetDistance(mHandles[165], "landingship1_path", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[165]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[166])) {
        mHandles[166] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship2_path", 0, "landingship2a", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[166], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[166], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[166], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[166], true);
        MissionUtility::Goto(mHandles[166], "landingship2_path", true);
        }
        if (MissionUtility::IsAlive(mHandles[166])) {
        if (MissionUtility::GetDistance(mHandles[166], "landingship2_path", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[166]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[167])) {
        mHandles[167] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship3_path", 0, "landingship3a", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[167], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[167], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[167], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[167], true);
        MissionUtility::Goto(mHandles[167], "landingship3_path", true);
        }
        if (MissionUtility::IsAlive(mHandles[167])) {
        if (MissionUtility::GetDistance(mHandles[167], "landingship3_path", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[167]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[168])) {
        mHandles[168] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship4_path", 0, "landingship4a", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[168], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[168], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[168], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[168], true);
        MissionUtility::Goto(mHandles[168], "landingship4_path", true);
        }
        if (MissionUtility::IsAlive(mHandles[168])) {
        if (MissionUtility::GetDistance(mHandles[168], "landingship4_path", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[168]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[169])) {
        mHandles[169] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship5_path", 0, "landingship5a", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[169], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[169], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[169], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[169], true);
        MissionUtility::Goto(mHandles[169], "landingship5_path", true);
        }
        if (MissionUtility::IsAlive(mHandles[169])) {
        if (MissionUtility::GetDistance(mHandles[169], "landingship5_path", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[169]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[170])) {
        mHandles[170] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship6_path", 0, "landingship6a", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[170], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[170], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[170], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[170], true);
        MissionUtility::Goto(mHandles[170], "landingship6_path", true);
        }
        if (MissionUtility::IsAlive(mHandles[170])) {
        if (MissionUtility::GetDistance(mHandles[170], "landingship6_path", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[170]);
        }
        }
        if (MissionUtility::IsAlive(mHandles[5])) {
        if (!MissionUtility::IsAlive(mHandles[340])) {
        mHandles[340] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "hangar1_spawn1", 0, "hangar1_flyer1", 0, -1, -1);
        MissionUtility::Goto(mHandles[340], "hangar1_go1", true);
        }
        if (MissionUtility::IsAlive(mHandles[340])) {
        if (MissionUtility::GetDistance(mHandles[340], "hangar1_go1", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[340]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[341])) {
        mHandles[341] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "hangar1_spawn2", 0, "hangar1_flyer2", 0, -1, -1);
        MissionUtility::Goto(mHandles[341], "hangar1_go2", true);
        }
        if (MissionUtility::IsAlive(mHandles[341])) {
        if (MissionUtility::GetDistance(mHandles[341], "hangar1_go2", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[341]);
        }
        }
        }
        if (MissionUtility::IsAlive(mHandles[6])) {
        if (!MissionUtility::IsAlive(mHandles[342])) {
        mHandles[342] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "hangar2_spawn1", 0, "hangar2_flyer1", 0, -1, -1);
        MissionUtility::Goto(mHandles[342], "hangar2_go1", true);
        }
        if (MissionUtility::IsAlive(mHandles[342])) {
        if (MissionUtility::GetDistance(mHandles[342], "hangar2_go1", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[342]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[343])) {
        mHandles[343] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "hangar2_spawn2", 0, "hangar2_flyer2", 0, -1, -1);
        MissionUtility::Goto(mHandles[343], "hangar2_go2", true);
        }
        if (MissionUtility::IsAlive(mHandles[343])) {
        if (MissionUtility::GetDistance(mHandles[343], "hangar2_go2", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[343]);
        }
        }
        }
        }
        }
        if (!mFlags[62]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "flyer_trigger1")) {
        mHandles[82] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "flyer_path1", 0, "cis_flyer1", 3, -1, -1);
        mHandles[77] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "flyer_path1", 1, "rep_flyer1", 4, -1, -1);
        mHandles[78] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "flyer_path2", 0, "rep_flyer2", 4, -1, -1);
        MissionUtility::SetMaxHealth(mHandles[82], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[82], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[77], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[77], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[78], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[78], 999999.0f);
        MissionUtility::Goto(mHandles[82], "flyer_path1", true);
        MissionUtility::Goto(mHandles[77], "flyer_path1", false);
        MissionUtility::Goto(mHandles[78], "flyer_path2", true);
        MissionUtility::SetVelocMinimumFly(mHandles[82], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[82], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[82], 120.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[77], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[77], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[77], 120.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[78], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[78], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[78], 120.0f);
        mFlags[62] = true;
        }
        }
        if (!mFlags[63]) {
        if (MissionUtility::IsInsideRegion(mHandles[77], "flyer_remove_trigger1")) {
        MissionUtility::RemoveObject(mHandles[77]);
        MissionUtility::RemoveObject(mHandles[78]);
        MissionUtility::RemoveObject(mHandles[82]);
        mFlags[63] = true;
        }
        }
        if (!mFlags[65]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "flyer_trigger2")) {
        mHandles[83] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "flyer_spawn1", 0, "cis_flyer2", 3, -1, -1);
        mHandles[79] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "flyer_spawn2", 0, "rep_flyer3", 4, -1, -1);
        mHandles[80] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "flyer_spawn3", 0, "rep_flyer4", 4, -1, -1);
        MissionUtility::SetMaxHealth(mHandles[83], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[83], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[79], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[79], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[80], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[80], 999999.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[83], 140.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[83], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[83], 140.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[79], 140.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[79], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[79], 140.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[80], 140.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[80], 140.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[80], 140.0f);
        mHandles[87] = MissionUtility::CreateFlock(mHandles[79], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[87], mHandles[80], Vector(40.0f, 0.0f, 0.0f));
        MissionUtility::Goto(mHandles[83], "flyer_path3", true);
        MissionUtility::Goto(mHandles[87], "flyer_path3", true);
        mFlags[65] = true;
        }
        }
        if (mFlags[65]) {
        if (!mFlags[67]) {
        if (MissionUtility::IsInsideRegion(mHandles[83], "flyer_die_trigger1")) {
        MissionUtility::SetCurShield(mHandles[83], 0.0f);
        MissionUtility::SetMaxHealth(mHandles[83], 1.0f);
        MissionUtility::SetCurHealth(mHandles[83], 1.0f);
        mFlags[67] = true;
        }
        }
        }
        if (!mFlags[66]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "flyer_die_trigger1")) {
        mHandles[111] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "flyer_spawn4", 0, "looping_flyer1", 3, -1, -1);
        mHandles[112] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "flyer_spawn5", 0, "looping_flyer2", 4, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[111], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[111], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[111], 120.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[112], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[112], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[112], 120.0f);
        MissionUtility::SetMaxHealth(mHandles[111], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[111], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[112], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[112], 999999.0f);
        mHandles[88] = MissionUtility::CreateFlock(mHandles[111], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[88], mHandles[112], Vector(0.0f, 0.0f, 40.0f));
        MissionUtility::Goto(mHandles[88], "flyer_path4", true);
        mHandles[113] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "flyer_spawn6", 0, "looping_flyer3", 3, -1, -1);
        mHandles[114] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "flyer_spawn7", 0, "looping_flyer4", 4, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[113], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[113], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[113], 120.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[114], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[114], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[114], 120.0f);
        MissionUtility::SetMaxHealth(mHandles[113], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[113], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[114], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[114], 999999.0f);
        mHandles[89] = MissionUtility::CreateFlock(mHandles[113], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[89], mHandles[114], Vector(0.0f, 0.0f, 40.0f));
        MissionUtility::Goto(mHandles[89], "flyer_path5", true);
        mHandles[115] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "flyer_spawn8", 0, "looping_flyer5", 3, -1, -1);
        mHandles[116] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "flyer_spawn9", 0, "looping_flyer6", 4, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[115], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[115], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[115], 120.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[116], 120.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[116], 120.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[116], 120.0f);
        MissionUtility::SetMaxHealth(mHandles[115], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[115], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[116], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[116], 999999.0f);
        mHandles[265] = MissionUtility::CreateFlock(mHandles[115], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[265], mHandles[116], Vector(0.0f, 0.0f, 40.0f));
        MissionUtility::Goto(mHandles[265], "flyer_path6", true);
        mFlags[66] = true;
        }
        }
        if (!mFlags[64]) {
        if (mFlags[65]) {
        if (MissionUtility::IsInsideRegion(mHandles[79], "flyer_remove_trigger2")
            || MissionUtility::IsInsideRegion(mHandles[80], "flyer_remove_trigger2")) {
        MissionUtility::RemoveObject(mHandles[79]);
        MissionUtility::RemoveObject(mHandles[80]);
        if (MissionUtility::IsAlive(mHandles[83])) {
        MissionUtility::RemoveObject(mHandles[83]);
        }
        mFlags[64] = true;
        }
        }
        }
        if (!mFlags[210]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "vcarrier_trigger1")) {
        mHandles[1] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier1", 0, "enemy_vcarrier1", 0, -1, -1);
        mHandles[2] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier2", 0, "enemy_vcarrier2", 0, -1, -1);
        mHandles[3] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier1", 0, "dropped_walker1", 7, -1, -1);
        mHandles[4] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier2", 0, "dropped_walker2", 7, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[1], "hp_link_1", mHandles[3], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[2], "hp_link_1", mHandles[4], "hp_link_1", true);
        MissionUtility::SetVelocVertical(mHandles[1], 40.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[1], 40.0f);
        MissionUtility::SetMaxAltitude(mHandles[1], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[1], 350.0f);
        MissionUtility::CarrierDropoff(mHandles[1], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[1], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocVertical(mHandles[2], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[2], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[2], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[2], 350.0f);
        MissionUtility::CarrierDropoff(mHandles[2], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[2], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[210] = true;
        }
        }
        if (!mFlags[211]) {
        if (mFlags[210]) {
        if (MissionUtility::GetDistance(mHandles[1], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[1]);
        mFlags[211] = true;
        }
        }
        }
        if (!mFlags[212]) {
        if (mFlags[210]) {
        if (MissionUtility::GetDistance(mHandles[2], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[2]);
        mFlags[212] = true;
        }
        }
        }
        if (!mFlags[1]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "vcarrier_trigger2")) {
        mHandles[474] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier3", 0, "enemy_vcarrier3", 0, -1, -1);
        mHandles[475] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier4", 0, "enemy_vcarrier4", 0, -1, -1);
        mHandles[480] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier3", 0, "dropped_walker3", 2, -1, -1);
        mHandles[481] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier4", 0, "dropped_walker4", 2, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[474], "hp_link_1", mHandles[480], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[475], "hp_link_1", mHandles[481], "hp_link_1", true);
        MissionUtility::SetVelocVertical(mHandles[474], 40.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[474], 40.0f);
        MissionUtility::SetMaxAltitude(mHandles[474], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[474], 350.0f);
        MissionUtility::CarrierDropoff(mHandles[474], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[474], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocVertical(mHandles[475], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[475], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[475], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[475], 350.0f);
        MissionUtility::CarrierDropoff(mHandles[475], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[475], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[1] = true;
        }
        }
        if (!mFlags[26]) {
        if (mFlags[1]) {
        if (MissionUtility::GetDistance(mHandles[474], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[474]);
        mFlags[26] = true;
        }
        }
        }
        if (!mFlags[27]) {
        if (mFlags[1]) {
        if (MissionUtility::GetDistance(mHandles[475], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[475]);
        mFlags[27] = true;
        }
        }
        }
        if (!mFlags[68]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "flyer_die_trigger1")) {
        mHandles[97] = MissionUtility::CreateObject("cis_tank_fighter", "gat_spawn1", "gat1", 2, -1, -1);
        mHandles[99] = MissionUtility::CreateObject("cis_tank_fighter", "gat_spawn3", "gat3", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[97], 1000);
        MissionUtility::SetAttackRange(mHandles[98], 1000);
        MissionUtility::SetAttackRange(mHandles[99], 1000);
        MissionUtility::SetAttackRange(mHandles[100], 1000);
        MissionUtility::AttackTarget(mHandles[97], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[98], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[99], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[100], mHandles[7], true, true, false, false);
        mFlags[68] = true;
        }
        }
        if (!mFlags[7]) {
        if (MissionUtility::GetCurHealth(mHandles[5]) < 3000.0f) {
        MissionUtility::Wait(mHandles[103]);
        MissionUtility::Wait(mHandles[104]);
        MissionUtility::Wait(mHandles[105]);
        MissionUtility::AttackTarget(mHandles[103], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[104], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[105], mHandles[7], true, true, false, false);
        mFlags[7] = true;
        }
        }
        if (!mFlags[8]) {
        if (MissionUtility::GetCurHealth(mHandles[6]) < 3000.0f) {
        MissionUtility::Wait(mHandles[106]);
        MissionUtility::Wait(mHandles[107]);
        MissionUtility::Wait(mHandles[108]);
        MissionUtility::AttackTarget(mHandles[106], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[107], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[108], mHandles[7], true, true, false, false);
        mFlags[8] = true;
        }
        }
        if (!mFlags[37]) {
        if (!MissionUtility::IsAlive(mHandles[5])) {
        MissionUtility::StartSound("explosion", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[37] = true;
        }
        }
        if (!mFlags[38]) {
        if (!MissionUtility::IsAlive(mHandles[6])) {
        MissionUtility::StartSound("explosion", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[38] = true;
        }
        }
        if (!mFlags[98]) {
        if (!MissionUtility::IsAlive(mHandles[5])
            || !MissionUtility::IsAlive(mHandles[6])) {
        mTimes[28] = 2.0f + MissionUtility::GetTime();
        mFlags[98] = true;
        }
        }
        if (!mFlags[101]) {
        if (mTimes[28] < MissionUtility::GetTime()) {
        mHandles[202] = MissionUtility::StartSound("LMT22_04B", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[101] = true;
        }
        }
        if (!mFlags[34]) {
        if (mFlags[37]) {
        if (mFlags[38]) {
        MissionUtility::ObjectiveComplete(mInts[0]);
        mFlags[34] = true;
        }
        }
        }
        if (!mFlags[9]) {
        if (mFlags[37]) {
        if (mFlags[38]) {
        if (MissionUtility::CountUnitsNearObject(mHandles[7], 500.0f, 7, 0) == 0) {
        mTimes[17] = 5.0f + MissionUtility::GetTime();
        mTimes[29] = 2.0f + MissionUtility::GetTime();
        mFlags[9] = true;
        }
        }
        }
        }
        if (!mFlags[176]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "hangar1_sneak_attack")) {
        mHandles[419] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "hangar1_sneak_attack_spawn1", 0, "", 2, -1, -1);
        mHandles[420] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "hangar1_sneak_attack_spawn2", 0, "", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[419], 3000);
        MissionUtility::SetAttackRange(mHandles[420], 3000);
        MissionUtility::AttackTarget(mHandles[419], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[420], mHandles[7], true, true, false, false);
        mFlags[176] = true;
        }
        }
        if (!mFlags[177]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "hangar2_sneak_attack")) {
        mHandles[421] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "hangar2_sneak_attack_spawn1", 0, "", 2, -1, -1);
        mHandles[422] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "hangar2_sneak_attack_spawn2", 0, "", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[421], 3000);
        MissionUtility::SetAttackRange(mHandles[422], 3000);
        MissionUtility::AttackTarget(mHandles[421], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[422], mHandles[7], true, true, false, false);
        mFlags[177] = true;
        }
        }
        if (!mFlags[80]) {
        if (mTimes[17] < MissionUtility::GetTime()) {
        if (!mFlags[81]) {
        MissionUtility::MoveObject(mHandles[7], "safe_spot", 0, true);
        MissionUtility::MoveObject(mHandles[55], "safe_spot", 0, true);
        MissionUtility::MoveObject(mHandles[56], "safe_spot", 0, true);
        MissionUtility::RemoveObject(mHandles[111]);
        MissionUtility::RemoveObject(mHandles[112]);
        MissionUtility::RemoveObject(mHandles[113]);
        MissionUtility::RemoveObject(mHandles[114]);
        MissionUtility::RemoveObject(mHandles[115]);
        MissionUtility::RemoveObject(mHandles[116]);
        MissionUtility::RemoveObject(mHandles[117]);
        MissionUtility::RemoveObject(mHandles[118]);
        MissionUtility::RemoveObject(mHandles[165]);
        MissionUtility::RemoveObject(mHandles[166]);
        MissionUtility::RemoveObject(mHandles[167]);
        MissionUtility::RemoveObject(mHandles[168]);
        MissionUtility::RemoveObject(mHandles[169]);
        MissionUtility::RemoveObject(mHandles[170]);
        MissionUtility::RemoveArmy(mHandles[304]);
        MissionUtility::RemoveArmy(mHandles[306]);
        MissionUtility::RemoveArmy(mHandles[308]);
        MissionUtility::RemoveArmy(mHandles[312]);
        MissionUtility::RemoveArmy(mHandles[314]);
        MissionUtility::RemoveArmy(mHandles[316]);
        MissionUtility::RemoveArmy(mHandles[318]);
        MissionUtility::RemoveArmy(mHandles[303]);
        MissionUtility::RemoveArmy(mHandles[305]);
        MissionUtility::RemoveArmy(mHandles[307]);
        MissionUtility::RemoveArmy(mHandles[309]);
        MissionUtility::RemoveArmy(mHandles[311]);
        MissionUtility::RemoveArmy(mHandles[313]);
        MissionUtility::RemoveArmy(mHandles[315]);
        MissionUtility::RemoveArmy(mHandles[317]);
        MissionUtility::RemoveFlyerArmy(mHandles[320]);
        MissionUtility::RemoveFlyerArmy(mHandles[321]);
        MissionUtility::RemoveFlyerArmy(mHandles[322]);
        MissionUtility::RemoveFlyerArmy(mHandles[323]);
        MissionUtility::RemoveFlyerArmy(mHandles[324]);
        MissionUtility::RemoveFlyerArmy(mHandles[325]);
        MissionUtility::RemoveFlyerArmy(mHandles[326]);
        MissionUtility::RemoveFlyerArmy(mHandles[327]);
        MissionUtility::RemoveFlyerArmy(mHandles[328]);
        MissionUtility::RemoveFlyerArmy(mHandles[329]);
        MissionUtility::RemoveFlyerArmy(mHandles[330]);
        MissionUtility::RemoveFlyerArmy(mHandles[331]);
        MissionUtility::EvictConfig("thu_bldg_hangar");
        MissionUtility::SetTeamNum(mHandles[7], 0);
        MissionUtility::MoveObject(mHandles[7], "move_player", 0, true);
        if (MissionUtility::IsAlive(mHandles[18])) {
        MissionUtility::MoveObject(mHandles[18], "move_squadmate1", 0, true);
        }
        if (MissionUtility::IsAlive(mHandles[19])) {
        MissionUtility::MoveObject(mHandles[19], "move_squadmate2", 0, true);
        }
        if (MissionUtility::IsAlive(mHandles[147])) {
        MissionUtility::RemoveObject(mHandles[147]);
        }
        if (MissionUtility::IsAlive(mHandles[148])) {
        MissionUtility::RemoveObject(mHandles[148]);
        }
        if (MissionUtility::IsAlive(mHandles[106])) {
        MissionUtility::RemoveObject(mHandles[106]);
        }
        if (MissionUtility::IsAlive(mHandles[107])) {
        MissionUtility::RemoveObject(mHandles[107]);
        }
        mHandles[187] = MissionUtility::CreateObjectWithRotation("rep_fly_assault", "mid_cin1_landingship_spawn2", 0, "mid_cin1_landingship2", 1, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[187], true);
        MissionUtility::SetApplyDynamics(mHandles[187], true);
        MissionUtility::Land(mHandles[187], 0, 0, 80.0f);
        mHandles[146] = MissionUtility::RunCin("mid_cin1", true, true);
        MissionUtility::PlayMusic("Ep2_v1_t12_05", true);
        mFlags[81] = true;
        }
        if (!mFlags[223]) {
        if (!mFlags[102]) {
        if (mFlags[81]) {
        if (MissionUtility::GetCinId(mHandles[146]) == 1) {
        MissionUtility::QueueSound("LMT22_04C", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[102] = true;
        }
        }
        }
        }
        if (!mFlags[223]) {
        if (!mFlags[139]) {
        if (mFlags[81]) {
        if (MissionUtility::GetCinId(mHandles[146]) == 2) {
        mHandles[501] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "MidCinTank1Path", 0, "MidCinTank1", 2, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[501], true);
        MissionUtility::Goto(mHandles[501], "MidCinTank1Path", false);
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        BeginTimer(mTimer12);
        MissionUtility::QueueSound("LMT22_05A", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[139] = true;
        }
        }
        }
        }
        if (!mFlags[223]) {
        if (mTimer12 > 6.0f) {
        if (MissionUtility::GetCinId(mHandles[146]) == 2) {
        mTimer12 = 0.0f;
        mInts[12] = mInts[12] + 1;
        switch (mInts[12]) {
        case 1:
        mHandles[387] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg1_spawn", 0, "cin_sixleg1", 1, -1, -1);
        mHandles[388] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg2_spawn", 0, "cin_sixleg2", 1, -1, -1);
        mHandles[389] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg3_spawn", 0, "cin_sixleg3", 1, -1, -1);
        mHandles[390] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg4_spawn", 0, "cin_sixleg4", 1, -1, -1);
        mHandles[391] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg5_spawn", 0, "cin_sixleg5", 1, -1, -1);
        mHandles[392] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg6_spawn", 0, "cin_sixleg6", 1, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[387], true);
        MissionUtility::OverrideSoundRange(mHandles[388], true);
        MissionUtility::OverrideSoundRange(mHandles[389], true);
        MissionUtility::OverrideSoundRange(mHandles[390], true);
        MissionUtility::OverrideSoundRange(mHandles[391], true);
        MissionUtility::OverrideSoundRange(mHandles[392], true);
        mHandles[502] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "MidCinTank2Path", 0, "MidCinTank2", 2, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[502], true);
        MissionUtility::Goto(mHandles[502], "MidCinTank2Path", false);
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        break;
        case 2:
        mHandles[503] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "MidCinTank1Path", 0, "MidCinTank3", 2, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[503], true);
        MissionUtility::Goto(mHandles[503], "MidCinTank1Path", false);
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        break;
        case 3:
        mHandles[504] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "MidCinTank2Path", 0, "MidCinTank4", 2, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[504], true);
        MissionUtility::Goto(mHandles[504], "MidCinTank2Path", false);
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        break;
        case 4:
        mHandles[505] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "MidCinTank1Path", 0, "MidCinTank5", 2, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[505], true);
        MissionUtility::Goto(mHandles[505], "MidCinTank1Path", false);
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        break;
        }
        }
        }
        }
        if (!mFlags[223]) {
        if (!mFlags[140]) {
        if (mFlags[81]) {
        if (MissionUtility::GetCinId(mHandles[146]) == 3) {
        mHandles[393] = MissionUtility::CreateObject("rep_tank_fighter1_player", "player_tank_spawn", 0, "cin_player_tank", 1, -1, Quat(0.009204f, 0.0f, -0.999958f, 0.0f), -1);
        mHandles[394] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "squadmate1_spawn", 0, "cin_tank_squadmate1", 1, -1, Quat(0.009204f, 0.0f, -0.999958f, 0.0f), -1);
        mHandles[395] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "squadmate2_spawn", 0, "cin_tank_squadmate2", 1, -1, Quat(0.009204f, 0.0f, -0.999958f, 0.0f), -1);
        MissionUtility::OverrideSoundRange(mHandles[393], true);
        MissionUtility::OverrideSoundRange(mHandles[394], true);
        MissionUtility::OverrideSoundRange(mHandles[395], true);
        MissionUtility::Goto(mHandles[387], "sixleg1_spawn", false);
        MissionUtility::Goto(mHandles[388], "sixleg2_spawn", false);
        MissionUtility::Goto(mHandles[389], "sixleg3_spawn", false);
        MissionUtility::Goto(mHandles[390], "sixleg4_spawn", false);
        MissionUtility::Goto(mHandles[391], "sixleg5_spawn", false);
        MissionUtility::Goto(mHandles[392], "sixleg6_spawn", false);
        mHandles[385] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "dropper_test", 0, "", 1, -1, 0);
        MissionUtility::SetVelocMinimumFly(mHandles[385], 75.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[385], 75.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[385], 75.0f);
        MissionUtility::Goto(mHandles[385], "dropper_test", true);
        mFlags[158] = true;
        mFlags[140] = true;
        }
        }
        }
        }
        if (!mFlags[159]) {
        if (MissionUtility::IsInsideRegion(mHandles[385], "dropper_region")) {
        mHandles[468] = MissionUtility::DropHealthBox(mHandles[385]);
        mTimes[44] = 1.0f + MissionUtility::GetTime();
        mFlags[159] = true;
        }
        }
        if (mTimes[44] < MissionUtility::GetTime()) {
        mHandles[469] = MissionUtility::DropAmmoBox(mHandles[385]);
        mTimes[44] = 999999.9f;
        }
        if (mFlags[81]) {
        if (!MissionUtility::IsCinRunning(mHandles[146])) {
        MissionUtility::FlushSoundQueue();
        mFlags[223] = true;
        MissionUtility::RemoveObject(mHandles[387]);
        MissionUtility::RemoveObject(mHandles[388]);
        MissionUtility::RemoveObject(mHandles[389]);
        MissionUtility::RemoveObject(mHandles[390]);
        MissionUtility::RemoveObject(mHandles[391]);
        MissionUtility::RemoveObject(mHandles[392]);
        MissionUtility::RemoveObject(mHandles[393]);
        MissionUtility::RemoveObject(mHandles[394]);
        MissionUtility::RemoveObject(mHandles[395]);
        MissionUtility::RemoveObject(mHandles[149]);
        MissionUtility::RemoveObject(mHandles[187]);
        MissionUtility::RemoveObject(mHandles[385]);
        MissionUtility::RemoveObject(mHandles[11]);
        MissionUtility::RemoveObject(mHandles[386]);
        MissionUtility::RemoveObject(mHandles[501]);
        MissionUtility::RemoveObject(mHandles[502]);
        MissionUtility::RemoveObject(mHandles[503]);
        MissionUtility::RemoveObject(mHandles[504]);
        MissionUtility::RemoveObject(mHandles[505]);
        MissionUtility::FlushSoundQueue();
        mFlags[170] = true;
        mFlags[167] = true;
        MissionUtility::MidMissionSavePlayer(1);
        MissionUtility::MidMissionSave((bool)mFlags[113]);
        MissionUtility::MidMissionSave((bool)mFlags[116]);
        MissionUtility::MidMissionSave((bool)mFlags[114]);
        MissionUtility::MidMissionSave((bool)mFlags[117]);
        MissionUtility::MidMissionSave((bool)mFlags[115]);
        MissionUtility::MidMissionSave((bool)mFlags[118]);
        MissionUtility::RemoveObject(mHandles[164]);
        MissionUtility::RemoveObject(mHandles[397]);
        MissionUtility::RemoveObject(mHandles[398]);
        MissionUtility::RemoveObject(mHandles[399]);
        MissionUtility::RemoveObject(mHandles[57]);
        MissionUtility::RemoveObject(mHandles[58]);
        MissionUtility::RemoveObject(mHandles[59]);
        MissionUtility::RemoveObject(mHandles[60]);
        MissionUtility::RemoveObject(mHandles[61]);
        MissionUtility::RemoveObject(mHandles[62]);
        MissionUtility::RemoveObject(mHandles[63]);
        MissionUtility::RemoveObject(mHandles[64]);
        MissionUtility::RemoveObject(mHandles[65]);
        MissionUtility::RemoveObject(mHandles[66]);
        MissionUtility::RemoveObject(mHandles[403]);
        MissionUtility::RemoveObject(mHandles[404]);
        MissionUtility::RemoveObject(mHandles[405]);
        MissionUtility::RemoveObject(mHandles[406]);
        MissionUtility::RemoveObject(mHandles[407]);
        MissionUtility::RemoveObject(mHandles[408]);
        MissionUtility::RemoveObject(mHandles[409]);
        MissionUtility::RemoveObject(mHandles[410]);
        MissionUtility::RemoveObject(mHandles[411]);
        MissionUtility::RemoveObject(mHandles[412]);
        MissionUtility::RemoveObject(mHandles[413]);
        MissionUtility::RemoveObject(mHandles[414]);
        MissionUtility::RemoveObject(mHandles[95]);
        MissionUtility::RemoveObject(mHandles[96]);
        MissionUtility::RemoveObject(mHandles[92]);
        MissionUtility::RemoveObject(mHandles[93]);
        MissionUtility::RemoveObject(mHandles[94]);
        MissionUtility::RemoveObject(mHandles[415]);
        MissionUtility::RemoveObject(mHandles[416]);
        MissionUtility::RemoveObject(mHandles[417]);
        MissionUtility::RemoveObject(mHandles[418]);
        MissionUtility::RemoveObject(mHandles[103]);
        MissionUtility::RemoveObject(mHandles[104]);
        MissionUtility::RemoveObject(mHandles[105]);
        MissionUtility::RemoveObject(mHandles[106]);
        MissionUtility::RemoveObject(mHandles[107]);
        MissionUtility::RemoveObject(mHandles[108]);
        MissionUtility::RemoveObject(mHandles[5]);
        MissionUtility::RemoveObject(mHandles[6]);
        if (!mFlags[159]) {
        MissionUtility::AddHealthBox("add_health_here", 0, -1.0f);
        MissionUtility::AddAmmoBox("add_ammo_here", 0, -1.0f);
        }
        MissionUtility::EvictConfig("thu_bldg_hangar");
        mFlags[80] = true;
        }
        }
        }
        }
    }

    // ---- +0x51f0  12460 bytes ----
    if (!mFlags[168] && mFlags[170]) {
        if (!mFlags[166]) {
        MissionUtility::EvictConfig("thu_bldg_hangar");
        MissionUtility::EvictConfig("rep_walk_assault_squadmate");
        MissionUtility::EvictConfig("cis_bike_speeder");
        MissionUtility::EvictConfig("THU_bldg_hangar_dest");
        MissionUtility::EvictConfig("thu_bldg_rwall_tunnel");
        MissionUtility::EvictConfig("thu_bldg_bridge");
        MissionUtility::EvictConfig("thu_bldg_rwall_tunnel_door");
        MissionUtility::EvictConfig("thu_bldg_rwall_tunnel_door_dest");
        mHandles[13] = MissionUtility::CreateObject("rep_tank_fighter1_player", "player_tank_spawn", 0, "player", 1, -1, Quat(0.009204f, 0.0f, -0.999958f, 0.0f), -1);
        MissionUtility::SetAsPlayer(mHandles[13], 0);
        mHandles[7] = MissionUtility::GetPlayerHandle(0);
        MissionUtility::SetTeamNum(mHandles[7], 1);
        MissionUtility::EvictConfig("rep_walk_assault_player");
        mHandles[14] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "squadmate1_spawn", 0, "tank_squadmate1", 1, -1, Quat(0.009204f, 0.0f, -0.999958f, 0.0f), -1);
        mHandles[15] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "squadmate2_spawn", 0, "tank_squadmate2", 1, -1, Quat(0.009204f, 0.0f, -0.999958f, 0.0f), -1);
        mHandles[16] = MissionUtility::CreateSquad(mHandles[7], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[16], mHandles[14]);
        MissionUtility::AddSquadMember(mHandles[16], mHandles[15]);
        MissionUtility::PlayMusic("EP1_V1_T15", true);
        MissionUtility::SetMaxHealth(mHandles[69], 1800.0f);
        MissionUtility::SetCurHealth(mHandles[69], 1800.0f);
        MissionUtility::SetMaxHealth(mHandles[70], 1800.0f);
        MissionUtility::SetCurHealth(mHandles[70], 1800.0f);
        MissionUtility::SetOnRadar(mHandles[69], false);
        MissionUtility::SetOnRadar(mHandles[70], false);
        MissionUtility::SetWeaponPitch(mHandles[67], "cis_mortar_assault", 0.5f);
        MissionUtility::SetWeaponYaw(mHandles[67], "cis_mortar_assault", 0.5f);
        MissionUtility::SetWeaponPitch(mHandles[68], "cis_mortar_assault", 0.5f);
        MissionUtility::SetWeaponYaw(mHandles[68], "cis_mortar_assault", 0.5f);
        mInts[1] = MissionUtility::AddObjective("missions.Thule2.objective.str0004");
        MissionUtility::DisplayText("missions.Thule2.text.str0005", 6.0f, -1.0f);
        MissionUtility::BeginWave("sixleg_section_wave1");
        MissionUtility::RemoveObject(mHandles[10]);
        MissionUtility::RemoveObject(mHandles[55]);
        MissionUtility::RemoveObject(mHandles[56]);
        mHandles[71] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg1_spawn", 0, "sixleg1", 5, -1, -1);
        mHandles[72] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg2_spawn", 0, "sixleg2", 5, -1, -1);
        mHandles[73] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg3_spawn", 0, "sixleg3", 5, -1, -1);
        mHandles[74] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg4_spawn", 0, "sixleg4", 5, -1, -1);
        mHandles[75] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg5_spawn", 0, "sixleg5", 5, -1, -1);
        mHandles[76] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "sixleg6_spawn", 0, "sixleg6", 5, -1, -1);
        MissionUtility::SetMaxHealth(mHandles[71], 2500.0f);
        MissionUtility::SetCurHealth(mHandles[71], 2500.0f);
        MissionUtility::SetMaxHealth(mHandles[72], 3000.0f);
        MissionUtility::SetCurHealth(mHandles[72], 3000.0f);
        MissionUtility::SetMaxHealth(mHandles[73], 3000.0f);
        MissionUtility::SetCurHealth(mHandles[73], 3000.0f);
        MissionUtility::SetMaxHealth(mHandles[74], 2500.0f);
        MissionUtility::SetCurHealth(mHandles[74], 2500.0f);
        MissionUtility::SetMaxHealth(mHandles[75], 3000.0f);
        MissionUtility::SetCurHealth(mHandles[75], 3000.0f);
        MissionUtility::SetMaxHealth(mHandles[76], 3000.0f);
        MissionUtility::SetCurHealth(mHandles[76], 3000.0f);
        mHandles[119] = MissionUtility::CreateFlock(mHandles[71], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[119], mHandles[72], Vector(0.0f, 0.0f, 50.0f));
        MissionUtility::AddFlockMember(mHandles[119], mHandles[73], Vector(0.0f, 0.0f, 100.0f));
        mHandles[120] = MissionUtility::CreateFlock(mHandles[74], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[120], mHandles[75], Vector(0.0f, 0.0f, 50.0f));
        MissionUtility::AddFlockMember(mHandles[120], mHandles[76], Vector(0.0f, 0.0f, 100.0f));
        MissionUtility::Goto(mHandles[119], "sixleg_path1", true);
        MissionUtility::Goto(mHandles[120], "sixleg_path2", true);
        MissionUtility::AddHealthBar(mHandles[71], "", 400.0f);
        MissionUtility::AddHealthBar(mHandles[72], "", 400.0f);
        MissionUtility::AddHealthBar(mHandles[73], "", 400.0f);
        MissionUtility::AddHealthBar(mHandles[74], "", 400.0f);
        MissionUtility::AddHealthBar(mHandles[75], "", 400.0f);
        MissionUtility::AddHealthBar(mHandles[76], "", 400.0f);
        MissionUtility::Objectify(mHandles[74], "missions.Thule2.marker.str0004", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[71], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mTimes[7] = 1.0f + MissionUtility::GetTime();
        mTimes[8] = 60.0f + MissionUtility::GetTime();
        mTimes[9] = 10.0f + MissionUtility::GetTime();
        mTimes[10] = 80.0f + MissionUtility::GetTime();
        mTimes[39] = 1.0f + MissionUtility::GetTime();
        mHandles[256] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "window_gunship1_spawn", 0, "window_gunship1", 10, -1, -1);
        mHandles[257] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "window_gunship1_spawn", 0, "window_gunship2", 10, -1, -1);
        mHandles[258] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "window_gunship2_spawn", 0, "window_gunship3", 10, -1, -1);
        mHandles[259] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "window_gunship2_spawn", 0, "window_gunship4", 10, -1, -1);
        mHandles[260] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "window_gunship3_spawn", 0, "window_gunship5", 10, -1, -1);
        mHandles[261] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "window_gunship3_spawn", 0, "window_gunship6", 10, -1, -1);
        mHandles[262] = MissionUtility::CreateFlock(mHandles[256], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[262], mHandles[257], Vector(-5.0f, 0.0f, 5.0f));
        mHandles[263] = MissionUtility::CreateFlock(mHandles[258], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[263], mHandles[259], Vector(-5.0f, 0.0f, 5.0f));
        mHandles[264] = MissionUtility::CreateFlock(mHandles[260], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[264], mHandles[261], Vector(-5.0f, 0.0f, 5.0f));
        MissionUtility::Patrol(mHandles[262], "window_gunship1_path", 500.0f, true);
        MissionUtility::Patrol(mHandles[263], "window_gunship2_path", 500.0f, true);
        MissionUtility::Patrol(mHandles[264], "window_gunship3_path", 500.0f, true);
        MissionUtility::SetMaxHealth(mHandles[256], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[256], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[257], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[257], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[258], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[258], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[259], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[259], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[260], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[260], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[261], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[261], 999999.0f);
        mHandles[396] = MissionUtility::CreateObject("rep_fly_assault", Vector(2046.1324f, -62.06877f, 2867.6206f), "", 1, -1, Quat(0.682399f, 0.0f, 0.730983f, 0.0f), -1);
        MissionUtility::EvictConfig("rep_walk_assault_player");
        MissionUtility::EvictConfig("rep_walk_assault_squadmate");
        MissionUtility::AddTurnAroundRegion("turnaround1", "turnaround1_point1", 0, 0, 0, 0);
        mHandles[332] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn13", 0, 3, 1.0f);
        mHandles[333] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn14", 0, 3, 1.0f);
        mHandles[334] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn15", 0, 3, 1.0f);
        mHandles[335] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn16", 0, 3, 1.0f);
        mHandles[336] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn17", 0, 3, 1.0f);
        mHandles[337] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn18", 0, 3, 1.0f);
        mHandles[338] = MissionUtility::AddFlyerArmy(0, "flyer_army_spawn19", 0, 3, 1.0f);
        mHandles[339] = MissionUtility::AddFlyerArmy(2, "flyer_army_spawn20", 0, 3, 1.0f);
        mHandles[451] = MissionUtility::AddPropArmy(0, "sixleg_army1", 0, 45, 2.0f);
        mHandles[452] = MissionUtility::AddPropArmy(1, "sixleg_army2", 0, 45, 2.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[451], "sixleg_army1_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[452], "sixleg_army2_patrol", true);
        mHandles[453] = MissionUtility::AddPropArmy(0, "sixleg_army3", 0, 45, 2.0f);
        mHandles[454] = MissionUtility::AddPropArmy(1, "sixleg_army4", 0, 45, 2.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[453], "sixleg_army3_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[454], "sixleg_army4_patrol", true);
        mHandles[455] = MissionUtility::AddPropArmy(0, "sixleg_army5", 0, 45, 2.0f);
        mHandles[456] = MissionUtility::AddPropArmy(1, "sixleg_army6", 0, 45, 2.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[455], "sixleg_army5_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[456], "sixleg_army6_patrol", true);
        mHandles[457] = MissionUtility::AddPropArmy(0, "sixleg_army7", 0, 45, 2.0f);
        mHandles[458] = MissionUtility::AddPropArmy(1, "sixleg_army8", 0, 45, 2.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[457], "sixleg_army7_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[458], "sixleg_army8_patrol", true);
        mFlags[166] = true;
        }
        if (!mFlags[13]) {
        if (MissionUtility::IsInsideRegion(mHandles[119], "sixleg_drop1")
            || MissionUtility::IsInsideRegion(mHandles[120], "sixleg_drop1")) {
        mHandles[476] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier5", 0, "enemy_vcarrier5", 0, -1, -1);
        mHandles[477] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier6", 0, "enemy_vcarrier6", 0, -1, -1);
        mHandles[482] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier5", 0, "dropped_walker5", 7, -1, -1);
        mHandles[483] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier6", 0, "dropped_walker6", 7, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[476], "hp_link_1", mHandles[482], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[477], "hp_link_1", mHandles[483], "hp_link_1", true);
        MissionUtility::SetVelocVertical(mHandles[476], 40.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[476], 40.0f);
        MissionUtility::SetMaxAltitude(mHandles[476], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[476], 350.0f);
        MissionUtility::CarrierDropoff(mHandles[476], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[476], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocVertical(mHandles[477], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[477], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[477], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[477], 350.0f);
        MissionUtility::CarrierDropoff(mHandles[477], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[477], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[13] = true;
        }
        }
        if (!mFlags[28]) {
        if (mFlags[13]) {
        if (MissionUtility::GetDistance(mHandles[476], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[476]);
        mFlags[28] = true;
        }
        }
        }
        if (!mFlags[29]) {
        if (mFlags[13]) {
        if (MissionUtility::GetDistance(mHandles[477], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[477]);
        mFlags[29] = true;
        }
        }
        }
        if (!mFlags[14]) {
        if (MissionUtility::IsInsideRegion(mHandles[119], "sixleg_drop2")
            || MissionUtility::IsInsideRegion(mHandles[120], "sixleg_drop2")) {
        mHandles[478] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier7", 0, "enemy_vcarrier7", 0, -1, -1);
        mHandles[479] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier8", 0, "enemy_vcarrier8", 0, -1, -1);
        mHandles[484] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier7", 0, "dropped_walker7", 2, -1, -1);
        mHandles[485] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier8", 0, "dropped_walker8", 2, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[478], "hp_link_1", mHandles[484], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[479], "hp_link_1", mHandles[485], "hp_link_1", true);
        MissionUtility::SetVelocVertical(mHandles[478], 40.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[478], 40.0f);
        MissionUtility::SetMaxAltitude(mHandles[478], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[478], 350.0f);
        MissionUtility::CarrierDropoff(mHandles[478], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[478], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocVertical(mHandles[479], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[479], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[479], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[479], 350.0f);
        MissionUtility::CarrierDropoff(mHandles[479], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[479], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[14] = true;
        }
        }
        if (!mFlags[30]) {
        if (mFlags[14]) {
        if (MissionUtility::GetDistance(mHandles[478], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[478]);
        mFlags[30] = true;
        }
        }
        }
        if (!mFlags[31]) {
        if (mFlags[14]) {
        if (MissionUtility::GetDistance(mHandles[479], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[479]);
        mFlags[31] = true;
        }
        }
        }
        if (!mFlags[145]) {
        if (!mFlags[55]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "turnaround1")) {
        MissionUtility::StartSound("LMG02_30", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[145] = true;
        }
        }
        }
        if (!mFlags[165]) {
        mHandles[400] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "fighters1_spawn1", 0, "fighter1a", 3, -1, -1);
        mHandles[401] = MissionUtility::CreateObjectWithRotation("cis_fly_fighter", "fighters1_spawn2", 0, "fighter1b", 4, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[400], 90.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[400], 90.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[400], 90.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[401], 90.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[401], 90.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[401], 90.0f);
        MissionUtility::SetMaxHealth(mHandles[400], 800.0f);
        MissionUtility::SetCurHealth(mHandles[400], 800.0f);
        MissionUtility::SetMaxHealth(mHandles[401], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[401], 999999.0f);
        mHandles[402] = MissionUtility::CreateFlock(mHandles[400], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[402], mHandles[401], Vector(0.0f, 0.0f, 100.0f));
        MissionUtility::Goto(mHandles[402], "fighters1_path", true);
        mFlags[165] = true;
        }
        if (MissionUtility::IsWaveSpawned("sixleg_section_wave1")) {
        if (!mFlags[162]) {
        mHandles[140] = MissionUtility::GetHandle("initial_sixleg_attacker1");
        mHandles[141] = MissionUtility::GetHandle("initial_sixleg_attacker2");
        mHandles[142] = MissionUtility::GetHandle("initial_sixleg_attacker3");
        mHandles[143] = MissionUtility::GetHandle("initial_sixleg_attacker4");
        mHandles[144] = MissionUtility::GetHandle("initial_sixleg_attacker5");
        mHandles[145] = MissionUtility::GetHandle("initial_sixleg_attacker6");
        mHandles[67] = MissionUtility::GetHandle("mortartank11");
        mHandles[68] = MissionUtility::GetHandle("mortartank12");
        mHandles[447] = MissionUtility::GetHandle("wheeled1");
        mHandles[448] = MissionUtility::GetHandle("wheeled2");
        mHandles[449] = MissionUtility::GetHandle("wheeled3");
        mHandles[450] = MissionUtility::GetHandle("wheeled4");
        mHandles[123] = MissionUtility::GetHandle("chokepoint_guard1");
        mHandles[124] = MissionUtility::GetHandle("chokepoint_guard3");
        mHandles[125] = MissionUtility::GetHandle("chokepoint_guard5");
        MissionUtility::Stop(mHandles[123]);
        MissionUtility::Stop(mHandles[124]);
        MissionUtility::Stop(mHandles[125]);
        MissionUtility::SetAttackRange(mHandles[140], 2000);
        MissionUtility::SetAttackRange(mHandles[141], 2000);
        MissionUtility::SetAttackRange(mHandles[142], 2000);
        MissionUtility::SetAttackRange(mHandles[143], 2000);
        MissionUtility::SetAttackRange(mHandles[144], 2000);
        MissionUtility::SetAttackRange(mHandles[145], 2000);
        MissionUtility::AttackTarget(mHandles[140], mHandles[119], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[141], mHandles[119], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[142], mHandles[119], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[143], mHandles[120], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[144], mHandles[120], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[145], mHandles[120], true, true, false, false);
        MissionUtility::Stop(mHandles[67]);
        MissionUtility::Stop(mHandles[68]);
        mFlags[162] = true;
        }
        if (!mFlags[131]) {
        if (!MissionUtility::IsAlive(mHandles[71])) {
        if (MissionUtility::IsAlive(mHandles[72])) {
        MissionUtility::Objectify(mHandles[72], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mFlags[133] = true;
        }
        if (!mFlags[133]) {
        if (MissionUtility::IsAlive(mHandles[73])) {
        MissionUtility::Objectify(mHandles[73], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mFlags[134] = true;
        }
        }
        mFlags[131] = true;
        }
        }
        if (!mFlags[132]) {
        if (mFlags[131]) {
        if (mFlags[133]) {
        if (!MissionUtility::IsAlive(mHandles[72])) {
        MissionUtility::Objectify(mHandles[73], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mFlags[132] = true;
        }
        }
        if (mFlags[134]) {
        if (!MissionUtility::IsAlive(mHandles[73])) {
        MissionUtility::Objectify(mHandles[72], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mFlags[132] = true;
        }
        }
        }
        }
        if (!mFlags[135]) {
        if (!MissionUtility::IsAlive(mHandles[74])) {
        if (MissionUtility::IsAlive(mHandles[75])) {
        MissionUtility::Objectify(mHandles[75], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mFlags[137] = true;
        }
        if (!mFlags[137]) {
        if (MissionUtility::IsAlive(mHandles[76])) {
        MissionUtility::Objectify(mHandles[76], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mFlags[138] = true;
        }
        }
        mFlags[135] = true;
        }
        }
        if (!mFlags[136]) {
        if (mFlags[135]) {
        if (mFlags[137]) {
        if (!MissionUtility::IsAlive(mHandles[75])) {
        MissionUtility::Objectify(mHandles[76], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mFlags[136] = true;
        }
        }
        if (mFlags[138]) {
        if (!MissionUtility::IsAlive(mHandles[76])) {
        MissionUtility::Objectify(mHandles[75], "missions.Thule2.marker.str0004", false, true, 0.0f);
        mFlags[136] = true;
        }
        }
        }
        }
        if (mHandles[121] == 0) {
        if (MissionUtility::GetDistance(mHandles[119], mHandles[69]) < 500.0f) {
        MissionUtility::SetAttackRange(mHandles[119], 1000);
        MissionUtility::AttackTarget(mHandles[119], mHandles[69], true, true, false, false);
        mHandles[121] = 1;
        }
        }
        if (!mFlags[71]) {
        if (!MissionUtility::IsAlive(mHandles[69])) {
        if (MissionUtility::IsAlive(mHandles[70])) {
        MissionUtility::SetAttackRange(mHandles[71], 2000);
        MissionUtility::SetAttackRange(mHandles[71], 2000);
        MissionUtility::SetAttackRange(mHandles[71], 2000);
        MissionUtility::SetVelocForward(mHandles[71], 7.0f);
        MissionUtility::SetVelocForward(mHandles[72], 7.0f);
        MissionUtility::SetVelocForward(mHandles[73], 7.0f);
        MissionUtility::AttackTarget(mHandles[71], mHandles[70], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[72], mHandles[70], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[73], mHandles[70], true, true, false, false);
        mFlags[71] = true;
        }
        }
        }
        if (mHandles[122] == 0) {
        if (MissionUtility::GetDistance(mHandles[120], mHandles[70]) < 500.0f) {
        MissionUtility::SetAttackRange(mHandles[120], 1000);
        MissionUtility::AttackTarget(mHandles[120], mHandles[70], true, true, false, false);
        mHandles[122] = 1;
        }
        }
        if (!mFlags[72]) {
        if (!MissionUtility::IsAlive(mHandles[70])) {
        if (MissionUtility::IsAlive(mHandles[69])) {
        MissionUtility::SetAttackRange(mHandles[74], 2000);
        MissionUtility::SetAttackRange(mHandles[75], 2000);
        MissionUtility::SetAttackRange(mHandles[76], 2000);
        MissionUtility::SetVelocForward(mHandles[74], 7.0f);
        MissionUtility::SetVelocForward(mHandles[75], 7.0f);
        MissionUtility::SetVelocForward(mHandles[76], 7.0f);
        MissionUtility::AttackTarget(mHandles[74], mHandles[69], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[75], mHandles[69], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[76], mHandles[69], true, true, false, false);
        mFlags[72] = true;
        }
        }
        }
        }
        if (MissionUtility::IsAlive(mHandles[69])) {
        if (!MissionUtility::IsAlive(mHandles[123])) {
        if (!mFlags[73]) {
        mTimes[12] = 20.0f + MissionUtility::GetTime();
        mFlags[73] = true;
        }
        if (mTimes[12] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[69], true);
        MissionUtility::SetAnimation(mHandles[69], "fullanimation", 1.0f, 1);
        mHandles[123] = MissionUtility::CreateObjectWithRotation("cis_tank_mortar", "factory1_spawn1", 0, "chokepoint_guard1", 2, -1, -1);
        MissionUtility::Goto(mHandles[123], "factory1_door1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[123], "chokepoint_guard1_spot", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[123]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[123], mHandles[7], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[73] = false;
        }
        }
        if (!MissionUtility::IsAlive(mHandles[124])) {
        if (!mFlags[75]) {
        mTimes[13] = 20.0f + MissionUtility::GetTime();
        mFlags[75] = true;
        }
        if (mTimes[13] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[69], true);
        MissionUtility::SetAnimation(mHandles[69], "fullanimation", 1.0f, 1);
        mHandles[124] = MissionUtility::CreateObjectWithRotation("cis_tank_mortar", "factory1_spawn1", 0, "chokepoint_guard3", 2, -1, -1);
        MissionUtility::Goto(mHandles[124], "factory1_door1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[124], "chokepoint_guard3_spot", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[124]);
        MissionUtility::SetQueueFlag(false);
        mFlags[75] = false;
        }
        }
        if (!mFlags[119]) {
        if (mTimes[7] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[69], true);
        MissionUtility::SetAnimation(mHandles[69], "fullanimation", 1.0f, 1);
        mHandles[126] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory1_spawn1", 0, "factory1_attacker1", 2, -1, -1);
        mHandles[127] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory1_spawn2", 0, "factory1_attacker2", 2, -1, -1);
        mHandles[128] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory1_spawn3", 0, "factory1_attacker3", 2, -1, -1);
        mHandles[136] = MissionUtility::CreateFlock(mHandles[126], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[136], mHandles[127]);
        MissionUtility::AddFlockMember(mHandles[136], mHandles[128]);
        MissionUtility::SetAttackRange(mHandles[136], 2500);
        MissionUtility::Goto(mHandles[136], "factory1_door1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[136], mHandles[119], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mTimes[7] = 70.0f + MissionUtility::GetTime();
        }
        }
        if (mTimes[8] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[69], true);
        MissionUtility::SetAnimation(mHandles[69], "fullanimation", 1.0f, 1);
        mHandles[129] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory1_spawn4", 0, "factory1_attacker4", 2, -1, -1);
        mHandles[130] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory1_spawn5", 0, "factory1_attacker5", 2, -1, -1);
        mHandles[138] = MissionUtility::CreateFlock(mHandles[129], (Formation)6);
        MissionUtility::AddFlockMember(mHandles[138], mHandles[130]);
        MissionUtility::SetAttackRange(mHandles[138], 2000);
        MissionUtility::Goto(mHandles[138], "factory1_door1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[138], mHandles[7], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mTimes[8] = 90.0f + MissionUtility::GetTime();
        }
        if (!mFlags[16]) {
        if (mTimes[45] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[69], true);
        MissionUtility::SetAnimation(mHandles[69], "fullanimation", 1.0f, 1);
        mHandles[240] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory1_spawn4", 0, "factory1_cwindowtank1", 8, -1, -1);
        mHandles[241] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory1_spawn5", 0, "factory1_cwindowtank2", 8, -1, -1);
        mHandles[242] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory1_spawn6", 0, "factory1_cwindowtank3", 8, -1, -1);
        mHandles[243] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory1_spawn7", 0, "factory1_cwindowtank4", 8, -1, -1);
        mHandles[245] = MissionUtility::CreateFlock(mHandles[240], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[245], mHandles[241]);
        MissionUtility::AddFlockMember(mHandles[245], mHandles[242]);
        MissionUtility::AddFlockMember(mHandles[245], mHandles[243]);
        MissionUtility::Goto(mHandles[245], "factory1_door2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[245], "window_path1", 500.0f, true);
        MissionUtility::SetQueueFlag(false);
        mFlags[16] = true;
        mFlags[17] = false;
        }
        }
        if (!mFlags[17]) {
        if (!MissionUtility::IsFlockAlive(mHandles[245])) {
        mTimes[45] = 10.0f + MissionUtility::GetTime();
        mFlags[16] = false;
        mFlags[17] = true;
        }
        }
        if (!mFlags[21]) {
        if (mTimes[5] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[69], true);
        MissionUtility::SetAnimation(mHandles[69], "fullanimation", 1.0f, 1);
        mHandles[266] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory1_spawn4", 0, "factory1_window_cwalker1", 8, -1, -1);
        MissionUtility::Goto(mHandles[266], "factory1_door2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[266], "window_path1", 500.0f, true);
        MissionUtility::SetQueueFlag(false);
        mFlags[22] = false;
        mFlags[21] = true;
        }
        }
        if (!mFlags[22]) {
        if (!MissionUtility::IsAlive(mHandles[266])) {
        if (!MissionUtility::IsAlive(mHandles[267])) {
        mTimes[5] = 10.0f + MissionUtility::GetTime();
        mFlags[21] = false;
        mFlags[22] = true;
        }
        }
        }
        if (!mFlags[32]) {
        if (mTimes[2] < MissionUtility::GetTime()) {
        mHandles[236] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_window", "rep_landingship_spawn1", 0, "factory1_rwindowtank1", 9, -1, -1);
        mHandles[237] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_window", "rep_landingship_spawn1", 0, "factory1_rwindowtank2", 9, -1, -1);
        mHandles[238] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_window", "rep_landingship_spawn1", 0, "factory1_rwindowtank3", 9, -1, -1);
        mHandles[239] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_window", "rep_landingship_spawn1", 0, "factory1_rwindowtank4", 9, -1, -1);
        mHandles[244] = MissionUtility::CreateFlock(mHandles[236], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[244], mHandles[237]);
        MissionUtility::AddFlockMember(mHandles[244], mHandles[238]);
        MissionUtility::AddFlockMember(mHandles[244], mHandles[239]);
        MissionUtility::Patrol(mHandles[244], "window_path1b", 500.0f, true);
        mFlags[33] = false;
        mFlags[32] = true;
        }
        }
        if (!mFlags[33]) {
        if (!MissionUtility::IsFlockAlive(mHandles[244])) {
        mTimes[2] = 10.0f + MissionUtility::GetTime();
        mFlags[32] = false;
        mFlags[33] = true;
        }
        }
        }
        if (MissionUtility::IsAlive(mHandles[70])) {
        if (!MissionUtility::IsAlive(mHandles[125])) {
        if (!mFlags[76]) {
        mTimes[14] = 20.0f + MissionUtility::GetTime();
        mFlags[76] = true;
        }
        if (mTimes[14] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        mHandles[125] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory2_spawn4", 0, "chokepoint_guard5", 2, -1, -1);
        MissionUtility::Goto(mHandles[125], "factory2_door2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[125], "chokepoint_guard5_spot", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[125]);
        MissionUtility::SetQueueFlag(false);
        mFlags[76] = false;
        }
        }
        if (!mFlags[120]) {
        if (mTimes[9] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        mHandles[131] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory2_spawn1", 0, "factory2_attacker1", 2, -1, -1);
        mHandles[132] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory2_spawn2", 0, "factory2_attacker2", 2, -1, -1);
        mHandles[133] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory2_spawn3", 0, "factory2_attacker3", 2, -1, -1);
        mHandles[137] = MissionUtility::CreateFlock(mHandles[131], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[137], mHandles[132]);
        MissionUtility::AddFlockMember(mHandles[137], mHandles[133]);
        MissionUtility::SetAttackRange(mHandles[137], 2000);
        MissionUtility::Goto(mHandles[137], "factory2_door1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[137], mHandles[120], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mTimes[9] = 90.0f + MissionUtility::GetTime();
        }
        }
        if (mTimes[10] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        mHandles[134] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory2_spawn4", 0, "factory2_attacker4", 2, -1, -1);
        mHandles[135] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory2_spawn5", 0, "factory2_attacker5", 2, -1, -1);
        mHandles[139] = MissionUtility::CreateFlock(mHandles[134], (Formation)6);
        MissionUtility::AddFlockMember(mHandles[139], mHandles[135]);
        MissionUtility::SetAttackRange(mHandles[139], 2000);
        MissionUtility::Goto(mHandles[139], "factory2_door2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[139], mHandles[7], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mTimes[10] = 120.0f + MissionUtility::GetTime();
        }
        if (!mFlags[18]) {
        if (mTimes[1] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        mHandles[250] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory2_spawn4", 0, "factory2_cwindowtank1", 8, -1, -1);
        mHandles[251] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory2_spawn5", 0, "factory2_cwindowtank2", 8, -1, -1);
        mHandles[252] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory2_spawn6", 0, "factory2_cwindowtank3", 8, -1, -1);
        mHandles[253] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "factory2_spawn7", 0, "factory2_cwindowtank4", 8, -1, -1);
        mHandles[255] = MissionUtility::CreateFlock(mHandles[250], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[255], mHandles[251]);
        MissionUtility::AddFlockMember(mHandles[255], mHandles[252]);
        MissionUtility::AddFlockMember(mHandles[255], mHandles[253]);
        MissionUtility::Goto(mHandles[255], "factory2_door2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[255], "window_path2", 500.0f, true);
        MissionUtility::SetQueueFlag(false);
        mFlags[19] = false;
        mFlags[18] = true;
        }
        }
        if (!mFlags[19]) {
        if (!MissionUtility::IsFlockAlive(mHandles[255])) {
        mTimes[1] = 10.0f + MissionUtility::GetTime();
        mFlags[18] = false;
        mFlags[19] = true;
        }
        }
        if (!mFlags[207]) {
        if (mTimes[46] < MissionUtility::GetTime()) {
        mHandles[246] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_window", "rep_landingship_spawn2", 0, "factory2_rwindowtank1", 9, -1, -1);
        mHandles[247] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_window", "rep_landingship_spawn2", 0, "factory2_rwindowtank2", 9, -1, -1);
        mHandles[248] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_window", "rep_landingship_spawn2", 0, "factory2_rwindowtank3", 9, -1, -1);
        mHandles[249] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_window", "rep_landingship_spawn2", 0, "factory2_rwindowtank4", 9, -1, -1);
        mHandles[254] = MissionUtility::CreateFlock(mHandles[246], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[254], mHandles[247]);
        MissionUtility::AddFlockMember(mHandles[254], mHandles[248]);
        MissionUtility::AddFlockMember(mHandles[254], mHandles[249]);
        MissionUtility::Patrol(mHandles[254], "window_path2b", 500.0f, true);
        mFlags[208] = false;
        mFlags[207] = true;
        }
        }
        if (!mFlags[208]) {
        if (!MissionUtility::IsFlockAlive(mHandles[254])) {
        mTimes[46] = 10.0f + MissionUtility::GetTime();
        mFlags[207] = false;
        mFlags[208] = true;
        }
        }
        if (!mFlags[24]) {
        if (mTimes[3] < MissionUtility::GetTime()) {
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::SetAnimation(mHandles[70], "fullanimation", 1.0f, 1);
        mHandles[268] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "factory2_spawn3", 0, "factory2_window_cwalker1", 8, -1, -1);
        MissionUtility::Goto(mHandles[268], "factory2_door1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[268], "window_path2", 500.0f, true);
        MissionUtility::SetQueueFlag(false);
        mFlags[25] = false;
        mFlags[24] = true;
        }
        }
        if (!mFlags[25]) {
        if (!MissionUtility::IsAlive(mHandles[268])) {
        if (!MissionUtility::IsAlive(mHandles[269])) {
        mTimes[3] = 10.0f + MissionUtility::GetTime();
        mFlags[24] = false;
        mFlags[25] = true;
        }
        }
        }
        }
        if (!mFlags[56]) {
        if (MissionUtility::IsInsideRegion(mHandles[119], "sixleg_drop1")
            || MissionUtility::IsInsideRegion(mHandles[120], "sixleg_drop1")) {
        mHandles[463] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "sixleg_droppath_1", 0, "sixleg_dropper1", 1, -1, -1);
        MissionUtility::Goto(mHandles[463], "sixleg_droppath_1", true);
        mHandles[470] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "side_attack_left1", 0, "side_attacker1", 2, -1, -1);
        mHandles[471] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "side_attack_left2", 0, "side_attacker2", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[470], 2000);
        MissionUtility::SetAttackRange(mHandles[471], 2000);
        MissionUtility::AttackTarget(mHandles[470], mHandles[119], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[471], mHandles[119], true, true, false, false);
        mHandles[472] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "side_attack_right1", 0, "side_attacker3", 2, -1, -1);
        mHandles[473] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "side_attack_right2", 0, "side_attacker4", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[472], 2000);
        MissionUtility::SetAttackRange(mHandles[473], 2000);
        MissionUtility::AttackTarget(mHandles[472], mHandles[120], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[473], mHandles[120], true, true, false, false);
        mFlags[56] = true;
        }
        }
        if (!mFlags[57]) {
        if (mFlags[56]) {
        if (MissionUtility::GetDistance(mHandles[463], "sixleg_droppath_1", 1) < 75.0f) {
        MissionUtility::DropAmmoBox(mHandles[463]);
        MissionUtility::QueueSound("CTT22_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[57] = true;
        }
        }
        }
        if (!mFlags[58]) {
        if (mFlags[56]) {
        if (MissionUtility::GetDistance(mHandles[463], "sixleg_droppath_1", 2) < 75.0f) {
        MissionUtility::DropHealthBox(mHandles[463]);
        mFlags[58] = true;
        }
        }
        }
        if (!mFlags[59]) {
        if (mFlags[56]) {
        if (MissionUtility::GetDistance(mHandles[463], "sixleg_droppath_1", 3) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[463]);
        mFlags[59] = true;
        }
        }
        }
        if (!mFlags[191]) {
        if (MissionUtility::IsInsideRegion(mHandles[119], "sixleg_drop2")
            || MissionUtility::IsInsideRegion(mHandles[120], "sixleg_drop2")) {
        mHandles[464] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "sixleg_droppath_2", 0, "sixleg_dropper2", 1, -1, -1);
        MissionUtility::Goto(mHandles[464], "sixleg_droppath_2", true);
        mFlags[191] = true;
        }
        }
        if (!mFlags[192]) {
        if (mFlags[191]) {
        if (MissionUtility::GetDistance(mHandles[464], "sixleg_droppath_2", 1) < 75.0f) {
        MissionUtility::DropAmmoBox(mHandles[464]);
        MissionUtility::QueueSound("CTT22_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[192] = true;
        }
        }
        }
        if (!mFlags[193]) {
        if (mFlags[191]) {
        if (MissionUtility::GetDistance(mHandles[464], "sixleg_droppath_2", 2) < 75.0f) {
        MissionUtility::DropHealthBox(mHandles[464]);
        mFlags[193] = true;
        }
        }
        }
        if (!mFlags[194]) {
        if (mFlags[191]) {
        if (MissionUtility::GetDistance(mHandles[464], "sixleg_droppath_2", 3) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[464]);
        mFlags[194] = true;
        }
        }
        }
        if (!mFlags[195]) {
        if (MissionUtility::IsInsideRegion(mHandles[119], "sixleg_drop3")
            || MissionUtility::IsInsideRegion(mHandles[120], "sixleg_drop3")) {
        mHandles[465] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "sixleg_droppath_3", 0, "sixleg_dropper3", 1, -1, -1);
        MissionUtility::Goto(mHandles[465], "sixleg_droppath_3", true);
        mFlags[195] = true;
        }
        }
        if (!mFlags[197]) {
        if (mFlags[195]) {
        if (MissionUtility::GetDistance(mHandles[465], "sixleg_droppath_3", 1) < 75.0f) {
        MissionUtility::DropAmmoBox(mHandles[465]);
        MissionUtility::QueueSound("CTT22_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[197] = true;
        }
        }
        }
        if (!mFlags[196]) {
        if (mFlags[195]) {
        if (MissionUtility::GetDistance(mHandles[465], "sixleg_droppath_3", 2) < 75.0f) {
        MissionUtility::DropHealthBox(mHandles[465]);
        mFlags[196] = true;
        }
        }
        }
        if (!mFlags[198]) {
        if (mFlags[195]) {
        if (MissionUtility::GetDistance(mHandles[465], "sixleg_droppath_3", 3) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[465]);
        mFlags[198] = true;
        }
        }
        }
        if (!mFlags[203]) {
        if (mFlags[55]) {
        mHandles[53] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "sixleg_droppath_4", 0, "sixleg_dropper4", 1, -1, -1);
        MissionUtility::Goto(mHandles[53], "sixleg_droppath_4", true);
        mFlags[203] = true;
        }
        }
        if (!mFlags[204]) {
        if (mFlags[203]) {
        if (MissionUtility::GetDistance(mHandles[53], "sixleg_droppath_4", 1) < 75.0f) {
        MissionUtility::DropAmmoBox(mHandles[53]);
        MissionUtility::QueueSound("CTT22_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[204] = true;
        }
        }
        }
        if (!mFlags[205]) {
        if (mFlags[203]) {
        if (MissionUtility::GetDistance(mHandles[53], "sixleg_droppath_4", 2) < 75.0f) {
        MissionUtility::DropHealthBox(mHandles[53]);
        mFlags[205] = true;
        }
        }
        }
        if (!mFlags[206]) {
        if (mFlags[203]) {
        if (MissionUtility::GetDistance(mHandles[53], "sixleg_droppath_4", 3) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[53]);
        mFlags[206] = true;
        }
        }
        }
        if (!mFlags[110]) {
        if (!MissionUtility::IsAlive(mHandles[69])
            || !MissionUtility::IsAlive(mHandles[70])) {
        mTimes[30] = 2.0f + MissionUtility::GetTime();
        mFlags[110] = true;
        }
        }
        if (!mFlags[103]) {
        if (mTimes[30] < MissionUtility::GetTime()) {
        mHandles[204] = MissionUtility::StartSound("LMT22_05C", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[103] = true;
        }
        }
        if (!mFlags[55]) {
        if (!mFlags[127]) {
        if (!MissionUtility::IsAlive(mHandles[71])) {
        if (!MissionUtility::IsAlive(mHandles[72])) {
        if (!MissionUtility::IsAlive(mHandles[73])) {
        mFlags[119] = true;
        mFlags[127] = true;
        }
        }
        }
        }
        if (!mFlags[128]) {
        if (!MissionUtility::IsAlive(mHandles[74])) {
        if (!MissionUtility::IsAlive(mHandles[75])) {
        if (!MissionUtility::IsAlive(mHandles[76])) {
        mFlags[120] = true;
        mFlags[128] = true;
        }
        }
        }
        }
        if (!mFlags[129]) {
        if (mFlags[127]
            || mFlags[128]) {
        mHandles[207] = MissionUtility::StartSound("LMT22_05G", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[129] = true;
        }
        }
        if (!mFlags[70]) {
        if (mFlags[127]) {
        if (mFlags[128]) {
        mTimes[11] = 5.0f + MissionUtility::GetTime();
        mHandles[206] = MissionUtility::StartSound("LMT22_05E", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[70] = true;
        }
        }
        }
        }
        if (!mFlags[55]) {
        if (!MissionUtility::IsAlive(mHandles[69])) {
        if (!MissionUtility::IsAlive(mHandles[70])) {
        MissionUtility::RemoveObjectify(mHandles[71]);
        MissionUtility::RemoveObjectify(mHandles[72]);
        MissionUtility::RemoveObjectify(mHandles[73]);
        MissionUtility::RemoveObjectify(mHandles[74]);
        MissionUtility::RemoveObjectify(mHandles[75]);
        MissionUtility::RemoveObjectify(mHandles[76]);
        MissionUtility::RemoveHealthBar(mHandles[71]);
        MissionUtility::RemoveHealthBar(mHandles[72]);
        MissionUtility::RemoveHealthBar(mHandles[73]);
        MissionUtility::RemoveHealthBar(mHandles[74]);
        MissionUtility::RemoveHealthBar(mHandles[75]);
        MissionUtility::RemoveHealthBar(mHandles[76]);
        mTimes[31] = 2.0f + MissionUtility::GetTime();
        mFlags[55] = true;
        }
        }
        }
        if (!mFlags[104]) {
        if (mTimes[31] < MissionUtility::GetTime()) {
        mHandles[205] = MissionUtility::QueueSound("LMT22_05D", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::ObjectiveComplete(mInts[1]);
        MissionUtility::Objectify("go", 0, "missions.Thule2.marker.str0002", true, false, 0.0f, 2.0f);
        MissionUtility::RemoveTurnAroundRegion("turnaround1");
        mInts[2] = MissionUtility::AddObjective("missions.Thule2.objective.str0005");
        MissionUtility::DisplayText("missions.Thule2.text.str0006", 6.0f, -1.0f);
        mFlags[171] = true;
        mFlags[168] = true;
        if (!mFlags[173]) {
        if (MissionUtility::IsAlive(mHandles[14])) {
        mFlags[173] = true;
        }
        }
        if (!mFlags[174]) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        mFlags[174] = true;
        }
        }
        MissionUtility::MidMissionSavePlayer(2);
        MissionUtility::MidMissionSave((bool)mFlags[113]);
        MissionUtility::MidMissionSave((bool)mFlags[116]);
        MissionUtility::MidMissionSave((bool)mFlags[114]);
        MissionUtility::MidMissionSave((bool)mFlags[117]);
        MissionUtility::MidMissionSave((bool)mFlags[115]);
        MissionUtility::MidMissionSave((bool)mFlags[118]);
        MissionUtility::MidMissionSave((bool)mFlags[173]);
        MissionUtility::MidMissionSave((bool)mFlags[174]);
        mFlags[104] = true;
        }
        }
    }

    // ---- +0x829c  6916 bytes ----
    if (!mFlags[169] && mFlags[171]) {
        if (!mFlags[51]) {
        MissionUtility::AddTurnAroundRegion("turnaround2", "turnaround2_point1", 0, 0, 0, 0);
        MissionUtility::SetMaxHealth(mHandles[384], 9999999.0f);
        MissionUtility::SetCurHealth(mHandles[384], 9999999.0f);
        MissionUtility::BeginWave("jedi_wave");
        mFlags[51] = true;
        }
        if (!mFlags[157]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "turnaround1")) {
        MissionUtility::AddTurnAroundRegion("reverse_turnaround1", "reverse_turnaround1_point1", 0, 0, 0, 0);
        mFlags[157] = true;
        }
        }
        if (!mFlags[146]) {
        if (!mFlags[45]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "turnaround2")) {
        MissionUtility::StartSound("LMG02_30", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[146] = true;
        }
        }
        }
        if (!mFlags[161]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "fed_landing_trigger")) {
        MissionUtility::RemoveFlock(mHandles[262], true);
        MissionUtility::RemoveFlock(mHandles[263], true);
        MissionUtility::RemoveFlock(mHandles[264], true);
        MissionUtility::RemoveFlock(mHandles[119], true);
        MissionUtility::RemoveFlock(mHandles[120], true);
        MissionUtility::RemoveFlock(mHandles[244], true);
        MissionUtility::RemoveFlock(mHandles[254], true);
        MissionUtility::RemoveFlock(mHandles[245], true);
        MissionUtility::RemoveFlock(mHandles[255], true);
        MissionUtility::RemoveFlock(mHandles[136], true);
        MissionUtility::RemoveFlock(mHandles[138], true);
        MissionUtility::RemoveFlock(mHandles[137], true);
        MissionUtility::RemoveFlock(mHandles[139], true);
        MissionUtility::RemoveObject(mHandles[396]);
        MissionUtility::RemoveObject(mHandles[67]);
        MissionUtility::RemoveObject(mHandles[68]);
        MissionUtility::RemoveObject(mHandles[268]);
        MissionUtility::RemoveObject(mHandles[266]);
        MissionUtility::RemoveObject(mHandles[140]);
        MissionUtility::RemoveObject(mHandles[141]);
        MissionUtility::RemoveObject(mHandles[142]);
        MissionUtility::RemoveObject(mHandles[143]);
        MissionUtility::RemoveObject(mHandles[144]);
        MissionUtility::RemoveObject(mHandles[145]);
        MissionUtility::RemoveFlyerArmy(mHandles[332]);
        MissionUtility::RemoveFlyerArmy(mHandles[333]);
        MissionUtility::RemoveFlyerArmy(mHandles[334]);
        MissionUtility::RemoveFlyerArmy(mHandles[335]);
        MissionUtility::RemoveFlyerArmy(mHandles[336]);
        MissionUtility::RemoveFlyerArmy(mHandles[337]);
        MissionUtility::RemoveFlyerArmy(mHandles[338]);
        MissionUtility::RemoveFlyerArmy(mHandles[339]);
        MissionUtility::RemoveArmy(mHandles[451]);
        MissionUtility::RemoveArmy(mHandles[452]);
        MissionUtility::RemoveArmy(mHandles[453]);
        MissionUtility::RemoveArmy(mHandles[454]);
        MissionUtility::RemoveArmy(mHandles[455]);
        MissionUtility::RemoveArmy(mHandles[456]);
        MissionUtility::RemoveArmy(mHandles[457]);
        MissionUtility::RemoveArmy(mHandles[458]);
        mFlags[161] = true;
        }
        }
        if (!mFlags[163]) {
        MissionUtility::BeginWave("protodekas");
        mFlags[163] = true;
        }
        if (MissionUtility::IsWaveSpawned("protodekas")) {
        if (!mFlags[35]) {
        mHandles[20] = MissionUtility::GetHandle("jugg1");
        MissionUtility::Stop(mHandles[20]);
        MissionUtility::SetAttackRange(mHandles[20], 1);
        MissionUtility::SetMaxHealth(mHandles[20], 12000.0f);
        MissionUtility::SetCurHealth(mHandles[20], 12000.0f);
        mHandles[21] = MissionUtility::GetHandle("jugg2");
        MissionUtility::Stop(mHandles[21]);
        MissionUtility::SetAttackRange(mHandles[21], 1);
        MissionUtility::SetMaxHealth(mHandles[21], 12000.0f);
        MissionUtility::SetCurHealth(mHandles[21], 12000.0f);
        mFlags[35] = true;
        }
        if (!mFlags[175]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[20]) < 900.0f) {
        mFlags[175] = true;
        }
        }
        if (!mFlags[82]) {
        if (mFlags[175]) {
        if (!mFlags[83]) {
        MissionUtility::PlayMusic("EP4_V2_T09_02", true);
        mHandles[211] = MissionUtility::StartSound("LMT22_05I", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[12] = MissionUtility::RunCin("protodeka_intro_cin", true, true);
        mFlags[83] = true;
        }
        if (mFlags[83]) {
        if (!MissionUtility::IsCinRunning(mHandles[12])) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::SetTeamNum(mHandles[20], 2);
        MissionUtility::SetTeamNum(mHandles[21], 2);
        mFlags[82] = true;
        }
        }
        }
        }
        if (!mFlags[43]) {
        if (mFlags[82]) {
        MissionUtility::DisplayText("missions.Thule2.text.str0011", 6.0f, -1.0f);
        MissionUtility::PlayMusic("EP4_V2_T09_02", true);
        mInts[3] = MissionUtility::AddObjective("missions.Thule2.objective.str0007");
        MissionUtility::RemoveObjectify("go", 0);
        MissionUtility::AddHealthBar(mHandles[20], "Protodeka", 400.0f);
        MissionUtility::AddHealthBar(mHandles[21], "Protodeka", 400.0f);
        MissionUtility::Wait(mHandles[20]);
        MissionUtility::SetAttackRange(mHandles[20], 1000);
        MissionUtility::Wait(mHandles[21]);
        MissionUtility::SetAttackRange(mHandles[21], 1000);
        BeginTimer(mTimer2);
        mFlags[187] = true;
        mFlags[190] = true;
        mFlags[188] = true;
        mFlags[189] = true;
        mFlags[184] = true;
        BeginTimer(mTimer6);
        mFlags[225] = true;
        mFlags[226] = true;
        mFlags[227] = true;
        mFlags[228] = true;
        mFlags[234] = true;
        mTimes[0] = 20.0f + MissionUtility::GetTime();
        mFlags[43] = true;
        }
        }
        if (mFlags[43]) {
        if (!mFlags[45]) {
        if (!mFlags[181]) {
        if (mTimer2 > 10.0f) {
        MissionUtility::SetAttackRange(mHandles[20], 1000);
        BeginTimer(mTimer2);
        MissionUtility::AttackTarget(mHandles[20], mHandles[7], false, true, false, true);
        mFlags[182] = false;
        mFlags[181] = true;
        }
        }
        if (!mFlags[182]) {
        if (mTimer2 > 20.0f) {
        BeginTimer(mTimer2);
        MissionUtility::AttackTarget(mHandles[20], mHandles[7], true, false, true, false);
        mFlags[181] = false;
        mFlags[182] = true;
        }
        }
        if (!mFlags[183]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[20]) < 125.0f) {
        BeginTimer(mTimer3);
        mFlags[185] = false;
        mFlags[184] = false;
        mFlags[183] = true;
        }
        }
        if (!mFlags[184]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[20]) > 125.0f) {
        StopTimer(mTimer3);
        mFlags[183] = false;
        mFlags[184] = true;
        }
        }
        if (!mFlags[185]) {
        if (mTimer3 > 1.0f) {
        StopTimer(mTimer3);
        mTimer3 = 0.0f;
        mFlags[183] = true;
        mFlags[184] = true;
        MissionUtility::SetFireSpecial(mHandles[20], true);
        mFlags[187] = false;
        BeginTimer(mTimer4);
        mFlags[186] = false;
        mFlags[185] = true;
        }
        }
        if (!mFlags[186]) {
        if (mFlags[185]) {
        if (mTimer4 > 7.0f) {
        MissionUtility::SetFireSpecial(mHandles[20], false);
        MissionUtility::StopCameraShake(1.0f);
        mFlags[183] = false;
        mFlags[184] = true;
        mFlags[188] = true;
        mFlags[189] = true;
        mFlags[187] = true;
        mFlags[190] = true;
        mFlags[185] = false;
        mFlags[186] = true;
        }
        }
        }
        if (!mFlags[187]) {
        if (mFlags[185]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[20]) < 150.0f) {
        mFlags[188] = false;
        mFlags[189] = true;
        mFlags[190] = false;
        mFlags[187] = true;
        }
        }
        }
        if (!mFlags[190]) {
        if (mFlags[187]) {
        if (mFlags[185]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[20]) > 150.0f) {
        MissionUtility::StopCameraShake(1.0f);
        mFlags[188] = true;
        mFlags[189] = true;
        mFlags[187] = false;
        mFlags[190] = true;
        }
        }
        }
        }
        if (!mFlags[188]) {
        MissionUtility::DamageObject(mHandles[7], 4.0f, 4.0f);
        BeginTimer(mTimer5);
        MissionUtility::ShakeCamera(90.0f, 0.0f, 0.3f);
        mFlags[189] = false;
        mFlags[188] = true;
        }
        if (!mFlags[189]) {
        if (mTimer5 > 0.1f) {
        MissionUtility::DamageObject(mHandles[7], 4.0f, 4.0f);
        mFlags[188] = false;
        mFlags[189] = true;
        }
        }
        if (!mFlags[229]) {
        if (mTimer6 > 10.0f) {
        MissionUtility::SetAttackRange(mHandles[21], 1000);
        BeginTimer(mTimer6);
        MissionUtility::AttackTarget(mHandles[21], mHandles[7], false, true, false, true);
        mFlags[230] = false;
        mFlags[229] = true;
        }
        }
        if (!mFlags[230]) {
        if (mTimer6 > 20.0f) {
        BeginTimer(mTimer6);
        MissionUtility::AttackTarget(mHandles[21], mHandles[7], true, false, true, false);
        mFlags[229] = false;
        mFlags[230] = true;
        }
        }
        if (!mFlags[231]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[21]) < 125.0f) {
        BeginTimer(mTimer7);
        mFlags[232] = false;
        mFlags[234] = false;
        mFlags[231] = true;
        }
        }
        if (!mFlags[234]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[21]) > 125.0f) {
        StopTimer(mTimer7);
        mFlags[231] = false;
        mFlags[234] = true;
        }
        }
        if (!mFlags[232]) {
        if (mTimer7 > 1.0f) {
        StopTimer(mTimer7);
        mTimer7 = 0.0f;
        mFlags[231] = true;
        mFlags[234] = true;
        MissionUtility::SetFireSpecial(mHandles[21], true);
        mFlags[225] = false;
        BeginTimer(mTimer8);
        mFlags[233] = false;
        mFlags[232] = true;
        }
        }
        if (!mFlags[233]) {
        if (mFlags[232]) {
        if (mTimer8 > 7.0f) {
        MissionUtility::SetFireSpecial(mHandles[21], false);
        MissionUtility::StopCameraShake(1.0f);
        mFlags[231] = false;
        mFlags[234] = true;
        mFlags[227] = true;
        mFlags[228] = true;
        mFlags[225] = true;
        mFlags[226] = true;
        mFlags[232] = false;
        mFlags[233] = true;
        }
        }
        }
        if (!mFlags[225]) {
        if (mFlags[232]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[21]) < 150.0f) {
        mFlags[227] = false;
        mFlags[228] = true;
        mFlags[226] = false;
        mFlags[225] = true;
        }
        }
        }
        if (!mFlags[226]) {
        if (mFlags[225]) {
        if (mFlags[232]) {
        if (MissionUtility::GetDistance(mHandles[7], mHandles[21]) > 150.0f) {
        MissionUtility::StopCameraShake(1.0f);
        mFlags[227] = true;
        mFlags[228] = true;
        mFlags[225] = false;
        mFlags[226] = true;
        }
        }
        }
        }
        if (!mFlags[227]) {
        MissionUtility::DamageObject(mHandles[7], 4.0f, 4.0f);
        BeginTimer(mTimer9);
        MissionUtility::ShakeCamera(90.0f, 0.0f, 0.3f);
        mFlags[228] = false;
        mFlags[227] = true;
        }
        if (!mFlags[228]) {
        if (mTimer9 > 0.1f) {
        MissionUtility::DamageObject(mHandles[7], 4.0f, 4.0f);
        mFlags[227] = false;
        mFlags[228] = true;
        }
        }
        }
        }
        if (!mFlags[45]) {
        if (!mFlags[178]) {
        if (mTimes[0] < MissionUtility::GetTime()) {
        mHandles[46] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "dropper1_spawn", 0, "powerup_dropper1", 1, -1, -1);
        MissionUtility::Goto(mHandles[46], "dropperpath", true);
        mFlags[178] = true;
        }
        }
        if (!mFlags[179]) {
        if (mFlags[178]) {
        if (MissionUtility::GetDistance(mHandles[46], "dropperpath", 3) < 85.0f) {
        if (!MissionUtility::IsPowerupAlive(mHandles[101])) {
        mHandles[101] = MissionUtility::DropAmmoBox(mHandles[46]);
        MissionUtility::QueueSound("CTT22_03", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[179] = true;
        }
        }
        }
        }
        if (!mFlags[180]) {
        if (mFlags[178]) {
        if (MissionUtility::GetDistance(mHandles[46], "dropperpath", 4) < 85.0f) {
        if (!MissionUtility::IsPowerupAlive(mHandles[102])) {
        mHandles[102] = MissionUtility::DropHealthBox(mHandles[46]);
        if (!mFlags[179]) {
        MissionUtility::QueueSound("CTT22_03", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        mFlags[180] = true;
        }
        }
        }
        }
        if (!mFlags[44]) {
        if (mFlags[178]) {
        if (MissionUtility::GetDistance(mHandles[46], "dropperpath", 5) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[46]);
        mFlags[39] = false;
        mFlags[44] = true;
        }
        }
        }
        if (!mFlags[39]) {
        if (!MissionUtility::IsPowerupAlive(mHandles[101])
            || !MissionUtility::IsPowerupAlive(mHandles[102])) {
        mFlags[178] = false;
        mFlags[179] = false;
        mFlags[180] = false;
        mFlags[44] = false;
        mTimes[0] = 15.0f + MissionUtility::GetTime();
        mFlags[39] = true;
        }
        }
        }
        if (!mFlags[45]) {
        if (mFlags[43]) {
        if (!MissionUtility::IsAlive(mHandles[20])) {
        if (!MissionUtility::IsAlive(mHandles[21])) {
        MissionUtility::ObjectiveComplete(mInts[3]);
        MissionUtility::Objectify("cheese_cin_cam1", 0, "missions.Thule2.marker.str0002", true, false, 0.0f, 2.0f);
        mTimes[41] = 3.0f + MissionUtility::GetTime();
        MissionUtility::RemoveTurnAroundRegion("turnaround2");
        mHandles[209] = MissionUtility::StartSound("LMT22_21", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::EvictConfig("CIS_boss_protodeka");
        mFlags[45] = true;
        }
        }
        }
        }
        }
        if (!mFlags[143]) {
        if (mFlags[45]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "trigger_landingship")) {
        if (!mFlags[142]) {
        mHandles[275] = MissionUtility::CreateObjectWithRotation("rep_fly_assault", "cin_landingship2_spawn", 0, "cin_landingship2", 1, -1, -1);
        MissionUtility::SetApplyDynamics(mHandles[275], true);
        MissionUtility::Land(mHandles[275], 0, 0, 80.0f);
        mHandles[52] = MissionUtility::StartSound("LMT22_23", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[142] = true;
        }
        }
        }
        }
        if (!mFlags[84]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "city_trigger")) {
        mHandles[150] = MissionUtility::CreateObject("cis_tank_assault", "gate_aat1_spawn", "", 8, -1, -1);
        mHandles[151] = MissionUtility::CreateObject("cis_tank_assault", "gate_aat2_spawn", "", 8, -1, -1);
        mHandles[152] = MissionUtility::CreateObject("cis_tank_assault", "gate_aat3_spawn", "", 8, -1, -1);
        mHandles[153] = MissionUtility::CreateObject("cis_tank_assault", "gate_aat4_spawn", "", 8, -1, -1);
        mHandles[154] = MissionUtility::CreateObject("cis_tank_fighter", "gate_treaded1_spawn", "", 8, -1, -1);
        mHandles[155] = MissionUtility::CreateObject("cis_tank_fighter", "gate_treaded2_spawn", "", 8, -1, -1);
        mHandles[156] = MissionUtility::CreateObject("cis_tank_assault", "gate_walker1_spawn", "", 8, -1, -1);
        mHandles[157] = MissionUtility::CreateObject("cis_tank_assault", "gate_walker2_spawn", "", 8, -1, -1);
        mHandles[158] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "rep_initial_spawn1", 0, "", 5, -1, -1);
        mHandles[159] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "rep_initial_spawn2", 0, "", 5, -1, -1);
        MissionUtility::AttackTarget(mHandles[158], mHandles[154], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[159], mHandles[155], true, true, false, false);
        MissionUtility::Stop(mHandles[150]);
        MissionUtility::Stop(mHandles[151]);
        MissionUtility::Stop(mHandles[152]);
        MissionUtility::Stop(mHandles[153]);
        MissionUtility::Stop(mHandles[154]);
        MissionUtility::Stop(mHandles[155]);
        MissionUtility::Stop(mHandles[156]);
        MissionUtility::Stop(mHandles[157]);
        MissionUtility::SetMaxHealth(mHandles[150], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[150], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[151], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[151], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[152], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[152], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[153], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[153], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[154], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[154], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[155], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[155], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[156], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[156], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[157], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[157], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[109], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[109], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[110], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[110], 999999.0f);
        mHandles[287] = MissionUtility::AddPropArmy(0, "end_clone_army1", 0, 50, 2.0f);
        mHandles[295] = MissionUtility::AddPropArmy(1, "end_droid_army1", 0, 50, 2.0f);
        mHandles[288] = MissionUtility::AddPropArmy(0, "end_clone_army2", 0, 50, 2.0f);
        mHandles[296] = MissionUtility::AddPropArmy(1, "end_droid_army2", 0, 50, 2.0f);
        mHandles[289] = MissionUtility::AddPropArmy(0, "end_clone_army3", 0, 50, 2.0f);
        mHandles[297] = MissionUtility::AddPropArmy(1, "end_droid_army3", 0, 50, 2.0f);
        mHandles[290] = MissionUtility::AddPropArmy(0, "end_clone_army4", 0, 50, 2.0f);
        mHandles[298] = MissionUtility::AddPropArmy(1, "end_droid_army4", 0, 50, 2.0f);
        mHandles[291] = MissionUtility::AddPropArmy(0, "end_clone_army5", 0, 50, 2.0f);
        mHandles[299] = MissionUtility::AddPropArmy(1, "end_droid_army5", 0, 50, 2.0f);
        mHandles[292] = MissionUtility::AddPropArmy(0, "end_clone_army6", 0, 50, 2.0f);
        mHandles[300] = MissionUtility::AddPropArmy(1, "end_droid_army6", 0, 50, 2.0f);
        mHandles[293] = MissionUtility::AddPropArmy(0, "end_clone_army7", 0, 50, 2.0f);
        mHandles[301] = MissionUtility::AddPropArmy(1, "end_droid_army7", 0, 50, 2.0f);
        mHandles[294] = MissionUtility::AddPropArmy(0, "end_clone_army8", 0, 50, 2.0f);
        mHandles[302] = MissionUtility::AddPropArmy(1, "end_droid_army8", 0, 50, 2.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[287], "end_clone_army1", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[288], "end_clone_army2", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[289], "end_clone_army3", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[290], "end_clone_army4", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[291], "end_clone_army5", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[292], "end_clone_army6", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[293], "end_clone_army7", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[294], "end_clone_army8", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[295], "end_droid_army1", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[296], "end_droid_army2", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[297], "end_droid_army3", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[298], "end_droid_army4", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[299], "end_droid_army5", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[300], "end_droid_army6", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[301], "end_droid_army7", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[302], "end_droid_army8", true);
        mTimes[18] = 5.0f + MissionUtility::GetTime();
        MissionUtility::AddTurnAroundRegion("city_turnaround1", "city_turnaround1_point", 0, 0, 0, 0);
        MissionUtility::AddTurnAroundRegion("city_turnaround2", "city_turnaround2_point", 0, 0, 0, 0);
        mFlags[84] = true;
        }
        }
        if (!mFlags[141]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "cydon_attack_trigger")) {
        mTimes[21] = 0.5f + MissionUtility::GetTime();
        mFlags[141] = true;
        }
        }
        if (!mFlags[88]) {
        if (mTimes[21] < MissionUtility::GetTime()) {
        if (!mFlags[89]) {
        MissionUtility::SetTeamNum(mHandles[14], 0);
        MissionUtility::SetTeamNum(mHandles[15], 0);
        MissionUtility::PlayMusic("EP1_V1_T12", true);
        mHandles[7] = MissionUtility::GetPlayerHandle(0);
        MissionUtility::MoveObject(mHandles[7], "cheese_move_player2", 0, true);
        MissionUtility::MoveObject(mHandles[14], "cheese_move_squadmate1", 0, true);
        MissionUtility::MoveObject(mHandles[15], "cheese_move_squadmate2", 0, true);
        mHandles[189] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "cheese_move_player", 0, "cheese_cin_player", 1, -1, -1);
        MissionUtility::SetVelocForward(mHandles[189], 150.0f);
        MissionUtility::Goto(mHandles[189], "cheese_move_player", false);
        MissionUtility::SetMaxHealth(mHandles[189], 50000.0f);
        MissionUtility::SetCurHealth(mHandles[189], 50000.0f);
        mHandles[507] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "CheeseCinTankPath", 0, "CheeseCinTank", 2, -1, -1);
        mHandles[508] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "CheeseCinTankPath", 1, "CheeseCinTank1", 2, -1, -1);
        mHandles[509] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "CheeseCinTankPath", 2, "CheeseCinTank2", 2, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[189], true);
        MissionUtility::OverrideSoundRange(mHandles[507], true);
        MissionUtility::OverrideSoundRange(mHandles[508], true);
        MissionUtility::OverrideSoundRange(mHandles[509], true);
        MissionUtility::SetCurHealth(mHandles[507], 70.0f);
        MissionUtility::SetCurHealth(mHandles[508], 70.0f);
        MissionUtility::SetCurHealth(mHandles[509], 70.0f);
        mHandles[190] = MissionUtility::CreateObjectWithRotation("cis_boss_cydon_thule2", "cheese_cydon_spawn", 0, "cydon_prax", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[190], 0);
        MissionUtility::SetCurPrimaryAmmo(mHandles[190], 1000.0f);
        MissionUtility::SetCurSecondaryAmmo(mHandles[190], 0.0f);
        MissionUtility::SetCurSpecialAmmo(mHandles[190], 0.0f);
        mHandles[188] = MissionUtility::RunCin("cheese_cin", true, true);
        mFlags[89] = true;
        }
        if (!mFlags[0]) {
        if (!MissionUtility::IsAlive(mHandles[507])) {
        if (!MissionUtility::IsAlive(mHandles[508])) {
        if (!MissionUtility::IsAlive(mHandles[509])) {
        MissionUtility::SetAttackRange(mHandles[189], 1);
        mFlags[0] = true;
        }
        }
        }
        }
        if (!mFlags[224]) {
        if (!mFlags[221]) {
        if (MissionUtility::GetCinId(mHandles[188]) == 2) {
        mFlags[221] = true;
        MissionUtility::AttackTarget(mHandles[507], mHandles[189], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[508], mHandles[189], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[509], mHandles[189], true, true, false, false);
        }
        }
        }
        if (!mFlags[224]) {
        if (!mFlags[91]) {
        if (mFlags[89]) {
        if (MissionUtility::GetCinId(mHandles[188]) == 4) {
        MissionUtility::OverrideSoundRange(mHandles[190], true);
        MissionUtility::QueueSound("CPT22_05J", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("LMT22_22", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[91] = true;
        }
        }
        }
        }
        if (!mFlags[224]) {
        if (!mFlags[220]) {
        if (MissionUtility::GetCinId(mHandles[188]) == 5) {
        MissionUtility::SetAttackRange(mHandles[189], 0);
        mTimes[22] = 3.0f + MissionUtility::GetTime();
        mFlags[220] = true;
        }
        }
        }
        if (!mFlags[224]) {
        if (!mFlags[90]) {
        if (mTimes[22] < MissionUtility::GetTime()) {
        MissionUtility::StartSound("generic_powerup_med01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Goto(mHandles[190], "cheese_move_player", false);
        mFlags[90] = true;
        }
        }
        }
        if (!mFlags[224]) {
        if (!mFlags[219]) {
        if (MissionUtility::GetCinId(mHandles[188]) == 6) {
        mFlags[219] = true;
        MissionUtility::MoveObjectWithRotation(mHandles[189], "cheese_move_player", 5, true);
        MissionUtility::Stop(mHandles[189]);
        MissionUtility::SetVelocForward(mHandles[189], 0.0f);
        BeginTimer(mTimer13);
        MissionUtility::SetAttackRange(mHandles[190], 50);
        MissionUtility::AttackTarget(mHandles[190], mHandles[189], true, true, false, false);
        MissionUtility::SetAttackRange(mHandles[189], 100);
        }
        }
        }
        if (!mFlags[224]) {
        if (mTimer13 > 3.0f) {
        StopTimer(mTimer13);
        mTimer13 = 0.0f;
        mHandles[506] = MissionUtility::CreateObjectWithRotation("rep_inf_mace", "cheese_move_player", 5, "CheeseCinMace", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[506], true);
        MissionUtility::SetCollidable(mHandles[506], false);
        MissionUtility::Goto(mHandles[506], "CheeseCinMacePath", false);
        MissionUtility::DamageObject(mHandles[189], 999999.0f, 999999.0f);
        MissionUtility::Goto(mHandles[190], "cheese_cydon_spawn", true);
        }
        }
        if (mFlags[89]) {
        if (!MissionUtility::IsCinRunning(mHandles[188])) {
        MissionUtility::FlushSoundQueue();
        mFlags[224] = true;
        MissionUtility::RemoveObject(mHandles[507]);
        MissionUtility::RemoveObject(mHandles[508]);
        MissionUtility::RemoveObject(mHandles[509]);
        MissionUtility::RemoveObject(mHandles[190]);
        MissionUtility::EvictConfig("cis_boss_cydon");
        MissionUtility::RemoveObject(mHandles[189]);
        MissionUtility::RemoveObject(mHandles[506]);
        MissionUtility::MidMissionSavePlayer(3);
        MissionUtility::MidMissionSave((bool)mFlags[113]);
        MissionUtility::MidMissionSave((bool)mFlags[116]);
        MissionUtility::MidMissionSave((bool)mFlags[114]);
        MissionUtility::MidMissionSave((bool)mFlags[117]);
        MissionUtility::MidMissionSave((bool)mFlags[115]);
        MissionUtility::MidMissionSave((bool)mFlags[118]);
        mFlags[209] = true;
        mFlags[88] = true;
        }
        }
        }
        }
    }

    // ---- +0x9da0  9576 bytes ----
    if (!mFlags[52] && mFlags[209]) {
        if (!mFlags[20]) {
        mHandles[526] = MissionUtility::AddFlyerArmy(0, "jedi_flyers_spawn1", 0, 6, 1.0f);
        mHandles[527] = MissionUtility::AddFlyerArmy(2, "jedi_flyers_spawn2", 0, 6, 1.0f);
        MissionUtility::EvictConfig("THU_bldg_factory_dest");
        MissionUtility::EvictConfig("cis_walk_assault");
        MissionUtility::EvictConfig("cis_fly_fighter");
        mTimes[19] = 0.1f + MissionUtility::GetTime();
        mTimes[42] = 1.0f + MissionUtility::GetTime();
        mHandles[229] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "duct_guards_spawn", 0, "", 2, -1, -1);
        mHandles[230] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "duct_guards_spawn", 1, "", 2, -1, -1);
        mHandles[231] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "duct_guards_spawn", 2, "", 2, -1, -1);
        mHandles[232] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "duct_guards_spawn", 3, "", 2, -1, -1);
        MissionUtility::Stop(mHandles[229]);
        MissionUtility::Stop(mHandles[230]);
        MissionUtility::Stop(mHandles[231]);
        MissionUtility::Stop(mHandles[232]);
        MissionUtility::RemoveObjectify("cheese_cin_cam1", 0);
        MissionUtility::AddTurnAroundRegion("jedi_turnaround1", "jedi_turnaround1_point1", "jedi_turnaround1_point2", "jedi_turnaround1_point3", 0, 0);
        MissionUtility::AddTurnAroundRegion("jedi_turnaround2", "jedi_turnaround2_point1", "jedi_turnaround2_point2", "jedi_turnaround2_point3", 0, 0);
        MissionUtility::AddTurnAroundRegion("jedi_turnaround3", "jedi_turnaround3_point1", "jedi_turnaround3_point2", "jedi_turnaround3_point3", 0, 0);
        MissionUtility::ObjectiveComplete(mInts[2]);
        MissionUtility::PlayMusic("EP5_V1_T05_01", true);
        mTimes[36] = 3.0f + MissionUtility::GetTime();
        mHandles[17] = MissionUtility::CreateObjectWithRotation("rep_inf_mace", "cheese_move_player", 5, "", 1, -1, -1);
        MissionUtility::SetAsPlayer(mHandles[17], 0);
        MissionUtility::SetFOV(64.3f);
        mHandles[7] = MissionUtility::GetPlayerHandle(0);
        MissionUtility::DisbandSquad(mHandles[16]);
        MissionUtility::RemoveObject(mHandles[13]);
        MissionUtility::RemoveObject(mHandles[14]);
        MissionUtility::RemoveObject(mHandles[15]);
        MissionUtility::SetFogRange(375.0f, 500.0f, 0.1f);
        MissionUtility::RemoveObject(mHandles[10]);
        MissionUtility::RemoveObject(mHandles[55]);
        MissionUtility::RemoveObject(mHandles[56]);
        MissionUtility::EvictConfig("rep_walk_assault_player");
        MissionUtility::EvictConfig("rep_walk_assault_squadmate");
        BeginTimer(mTimer15);
        mFlags[20] = true;
        }
        if (!mFlags[23]) {
        if (mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[150])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[150])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[151])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[152])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[153])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[154])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[155])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[156])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[157])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[109])
            || mHandles[7] == MissionUtility::GetWhoShotMe(mHandles[110])) {
        MissionUtility::SetEnemies(1, 11);
        MissionUtility::AttackTarget(mHandles[109], mHandles[7], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[110], mHandles[7], true, true, false, false);
        mFlags[23] = true;
        }
        }
        if (!mFlags[85]) {
        if (mTimes[18] < MissionUtility::GetTime()) {
        mHandles[235] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "end_vcarrier_spawn1", 0, "rep_vcarrier1", 0, -1, -1);
        mHandles[160] = MissionUtility::CreateObject("rep_walk_sixleg", "end_vcarrier_spawn1", 0, "city_attacker1", 5, -1);
        MissionUtility::CarrierAddCargo(mHandles[235], "hp_link_1", mHandles[160], "hp_link_1", true);
        MissionUtility::Goto(mHandles[235], "rep_spawn1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[235], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[235], "rep_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mHandles[466] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "end_vcarrier_spawn2", 0, "rep_vcarrier2", 0, -1, -1);
        mHandles[161] = MissionUtility::CreateObject("rep_walk_sixleg", "end_vcarrier_spawn2", 0, "city_attacker2", 5, -1);
        MissionUtility::CarrierAddCargo(mHandles[466], "hp_link_1", mHandles[161], "hp_link_1", true);
        MissionUtility::Goto(mHandles[466], "rep_spawn2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[466], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[466], "rep_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mHandles[467] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "end_vcarrier_spawn3", 0, "rep_vcarrier3", 0, -1, -1);
        mHandles[162] = MissionUtility::CreateObject("rep_walk_sixleg", "end_vcarrier_spawn3", 0, "city_attacker3", 5, -1);
        MissionUtility::CarrierAddCargo(mHandles[467], "hp_link_1", mHandles[162], "hp_link_1", true);
        MissionUtility::Goto(mHandles[467], "rep_spawn3", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[467], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[467], "rep_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[41] = false;
        mFlags[199] = false;
        mFlags[200] = false;
        mFlags[36] = false;
        mFlags[201] = false;
        mFlags[202] = false;
        mTimes[18] = 50.0f + MissionUtility::GetTime();
        }
        }
        if (!mFlags[41]) {
        if (MissionUtility::IsAlive(mHandles[160])) {
        if (MissionUtility::IsDropped(mHandles[160])) {
        MissionUtility::AttackTarget(mHandles[160], mHandles[154], true, true, false, false);
        mFlags[41] = true;
        }
        }
        }
        if (!mFlags[199]) {
        if (MissionUtility::IsAlive(mHandles[161])) {
        if (MissionUtility::IsDropped(mHandles[161])) {
        MissionUtility::AttackTarget(mHandles[161], mHandles[155], true, true, false, false);
        mFlags[199] = true;
        }
        }
        }
        if (!mFlags[200]) {
        if (MissionUtility::IsAlive(mHandles[162])) {
        if (MissionUtility::IsDropped(mHandles[162])) {
        MissionUtility::AttackTarget(mHandles[162], mHandles[154], true, true, false, false);
        mFlags[200] = true;
        }
        }
        }
        if (!mFlags[36]) {
        if (MissionUtility::IsAlive(mHandles[235])) {
        if (MissionUtility::GetDistance(mHandles[235], "rep_vcarrier1_go") < 100.0f) {
        MissionUtility::RemoveObject(mHandles[235]);
        mFlags[36] = true;
        }
        }
        }
        if (!mFlags[201]) {
        if (MissionUtility::IsAlive(mHandles[466])) {
        if (MissionUtility::GetDistance(mHandles[466], "rep_vcarrier1_go") < 100.0f) {
        MissionUtility::RemoveObject(mHandles[466]);
        mFlags[201] = true;
        }
        }
        }
        if (!mFlags[202]) {
        if (MissionUtility::IsAlive(mHandles[467])) {
        if (MissionUtility::GetDistance(mHandles[467], "rep_vcarrier1_go") < 100.0f) {
        MissionUtility::RemoveObject(mHandles[467]);
        mFlags[202] = true;
        }
        }
        }
        if (!mFlags[109]) {
        if (mTimes[36] < MissionUtility::GetTime()) {
        mHandles[214] = MissionUtility::QueueSound("LMT22_05K", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[109] = true;
        }
        }
        if (!mFlags[111]) {
        if (mFlags[109]) {
        if (!MissionUtility::IsSoundPlaying(mHandles[214])) {
        MissionUtility::DisplayText("missions.Thule2.text.str0012", 6.0f, -1.0f);
        MissionUtility::Objectify("service_duct", 0, "missions.Thule2.marker.str0003", true, false, 0.0f, 2.0f);
        mInts[4] = MissionUtility::AddObjective("missions.Thule2.objective.str0008");
        mFlags[111] = true;
        }
        }
        }
        if (!mFlags[2]) {
        if (MissionUtility::GetDistance(mHandles[7], "service_duct") < 40.0f) {
        MissionUtility::RemoveObjectify("service_duct", 0);
        mFlags[2] = true;
        }
        }
        if (!mFlags[86]) {
        if (mTimes[19] < MissionUtility::GetTime()) {
        mTimes[19] = 30.0f + MissionUtility::GetTime();
        }
        }
        if (!mFlags[153]) {
        if (!mFlags[154]) {
        if (mTimes[42] < MissionUtility::GetTime()) {
        mInts[10] = MissionUtility::GetRandomInt(1, 3);
        if (mInts[10] == 1) {
        mHandles[277] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn1", 0, "", 2, -1, -1);
        mHandles[278] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn2", 0, "", 2, -1, -1);
        mHandles[181] = MissionUtility::CreateFlock(mHandles[278], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[181], mHandles[277]);
        }
        if (mInts[10] == 2) {
        mHandles[277] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn1", 0, "", 2, -1, -1);
        mHandles[278] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn2", 0, "", 2, -1, -1);
        mHandles[279] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn3", 0, "", 2, -1, -1);
        mHandles[181] = MissionUtility::CreateFlock(mHandles[278], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[181], mHandles[277]);
        MissionUtility::AddFlockMember(mHandles[181], mHandles[279]);
        }
        if (mInts[10] == 3) {
        mHandles[277] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn1", 0, "", 2, -1, -1);
        mHandles[278] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn2", 0, "", 2, -1, -1);
        mHandles[279] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn3", 0, "", 2, -1, -1);
        mHandles[280] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "end_droid_spawn3", 0, "", 2, -1, -1);
        mHandles[181] = MissionUtility::CreateFlock(mHandles[278], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[181], mHandles[277]);
        MissionUtility::AddFlockMember(mHandles[181], mHandles[279]);
        MissionUtility::AddFlockMember(mHandles[181], mHandles[280]);
        }
        MissionUtility::SetAttackRange(mHandles[181], 5000);
        MissionUtility::AttackTarget(mHandles[181], mHandles[7], true, true, false, false);
        if (!mFlags[144]) {
        mHandles[281] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "pre_droids1", 1, "", 2, -1, -1);
        mHandles[282] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "pre_droids1", 0, "", 2, -1, -1);
        mHandles[283] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "pre_droids1", 2, "", 2, -1, -1);
        mHandles[182] = MissionUtility::CreateFlock(mHandles[281], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[182], mHandles[282]);
        MissionUtility::AddFlockMember(mHandles[182], mHandles[283]);
        MissionUtility::SetAttackRange(mHandles[182], 5000);
        MissionUtility::AttackTarget(mHandles[182], mHandles[7], true, true, false, false);
        mHandles[284] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "pre_droids2", 1, "", 2, -1, -1);
        mHandles[285] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "pre_droids2", 0, "", 2, -1, -1);
        mHandles[286] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "pre_droids2", 2, "", 2, -1, -1);
        mHandles[183] = MissionUtility::CreateFlock(mHandles[284], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[183], mHandles[285]);
        MissionUtility::AddFlockMember(mHandles[183], mHandles[286]);
        MissionUtility::SetAttackRange(mHandles[183], 5000);
        MissionUtility::AttackTarget(mHandles[183], mHandles[7], true, true, false, false);
        mHandles[374] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "end_mini_spider1", 0, "end_mini_spider1", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[374], 80);
        MissionUtility::SetFiringRange(mHandles[374], 80);
        mHandles[375] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "end_mini_spider2", 0, "end_mini_spider2", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[375], 2000);
        MissionUtility::AttackTarget(mHandles[375], mHandles[7], true, true, false, false);
        mHandles[376] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "end_mini_spider3", 0, "end_mini_spider3", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[376], 2000);
        MissionUtility::AttackTarget(mHandles[376], mHandles[7], true, true, false, false);
        mHandles[377] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "end_mini_spider4", 0, "end_mini_spider4", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[377], 2000);
        MissionUtility::AttackTarget(mHandles[377], mHandles[7], true, true, false, false);
        mHandles[378] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "end_mini_spider5", 0, "end_mini_spider5", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[378], 2000);
        MissionUtility::AttackTarget(mHandles[378], mHandles[7], true, true, false, false);
        mHandles[379] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "end_mini_spider6", 0, "end_mini_spider6", 2, -1, -1);
        MissionUtility::SetAttackRange(mHandles[379], 2000);
        MissionUtility::AttackTarget(mHandles[379], mHandles[7], true, true, false, false);
        mFlags[144] = true;
        }
        mTimes[42] = 8.0f + MissionUtility::GetTime();
        }
        }
        if (!mFlags[154]) {
        if (MissionUtility::CountUnitsNearPoint("droid_test", 0, 2000.0f, 2, "cis_inf_droid") > 16) {
        mFlags[155] = false;
        mFlags[154] = true;
        }
        }
        if (!mFlags[155]) {
        if (mFlags[154]) {
        if (MissionUtility::CountUnitsNearPoint("droid_test", 0, 2000.0f, 2, "cis_inf_droid") < 9) {
        mFlags[155] = true;
        mFlags[154] = false;
        }
        }
        }
        if (!mFlags[153]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "stop_droids")) {
        mFlags[153] = true;
        }
        }
        }
        if (!mFlags[156]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "popup_region")) {
        mHandles[380] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "popup_spawn", 3, "popup_droid1", 2, -1, -1);
        mHandles[381] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "popup_spawn", 2, "popup_droid2", 2, -1, -1);
        mHandles[382] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "popup_spawn", 1, "popup_droid3", 2, -1, -1);
        mHandles[383] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "popup_spawn", 0, "popup_droid4", 2, -1, -1);
        MissionUtility::Patrol(mHandles[380], "popup_path", 0.0f, true);
        MissionUtility::Patrol(mHandles[381], "popup_path", 1.0f, true);
        MissionUtility::Patrol(mHandles[382], "popup_path", 2.0f, true);
        MissionUtility::Patrol(mHandles[383], "popup_path", 3.0f, true);
        mFlags[156] = true;
        }
        }
        if (!mFlags[4]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "spawn_hallway_droids")) {
        mFlags[4] = true;
        }
        }
        if (mTimer15 > 1.0f) {
        if (!mFlags[4]) {
        mInts[11] = MissionUtility::GetRandomInt(1, 14);
        if (mInts[11] == 1) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 0, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 2) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 1, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 3) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 2, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 4) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 3, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 5) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 4, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 6) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 5, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 7) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 6, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 8) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 7, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 9) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 8, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 10) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 9, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 11) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 10, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 12) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 11, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 13) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 12, "", 0, -1);
        BeginTimer(mTimer15);
        }
        if (mInts[11] == 14) {
        MissionUtility::CreateObject("REP_mortar_assault_xpl_soft", "area1explosions", 13, "", 0, -1);
        BeginTimer(mTimer15);
        }
        }
        }
        if (!mFlags[147]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "spawn_hallway_droids")) {
        mHandles[344] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid1", 0, "hallway_droid1", 2, -1, -1);
        mHandles[345] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid2", 0, "hallway_droid2", 2, -1, -1);
        MissionUtility::Stop(mHandles[344]);
        MissionUtility::Stop(mHandles[345]);
        mFlags[147] = true;
        }
        }
        if (!mFlags[148]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "hallway_trigger1")) {
        mHandles[346] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid3", 0, "hallway_droid3", 2, -1, -1);
        mHandles[347] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid4", 0, "hallway_droid4", 2, -1, -1);
        mHandles[349] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid6", 0, "hallway_droid6", 2, -1, -1);
        mHandles[351] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid8", 0, "hallway_droid8", 2, -1, -1);
        MissionUtility::Goto(mHandles[347], "hallway_go1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[347]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[346], "hallway_go3", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[346]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[351], "hallway_go2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[351]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[349], "hallway_go4", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[349]);
        MissionUtility::SetQueueFlag(false);
        mFlags[148] = true;
        }
        }
        if (!mFlags[149]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "hallway_trigger2")) {
        mHandles[352] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "hallway_droid9", 0, "hallway_droid9", 2, -1, -1);
        mHandles[353] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid10", 0, "hallway_droid10", 2, -1, -1);
        mHandles[354] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid11", 0, "hallway_droid11", 2, -1, -1);
        mFlags[149] = true;
        }
        }
        if (!mFlags[150]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "hallway_trigger3")) {
        mHandles[355] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "hallway_droid12", 0, "hallway_droid12", 2, -1, -1);
        mHandles[356] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid13", 0, "hallway_droid13", 2, -1, -1);
        mHandles[357] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid14", 0, "hallway_droid14", 2, -1, -1);
        mHandles[368] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid30", 0, "hallway_droid30", 2, -1, -1);
        mHandles[369] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid31", 0, "hallway_droid31", 2, -1, -1);
        mHandles[370] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid32", 0, "hallway_droid32", 2, -1, -1);
        mHandles[371] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid33", 0, "hallway_droid33", 2, -1, -1);
        mHandles[372] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid34", 0, "hallway_droid34", 2, -1, -1);
        mHandles[373] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid35", 0, "hallway_droid35", 2, -1, -1);
        MissionUtility::Stop(mHandles[368]);
        MissionUtility::Stop(mHandles[369]);
        MissionUtility::Stop(mHandles[370]);
        MissionUtility::Stop(mHandles[371]);
        MissionUtility::Stop(mHandles[372]);
        MissionUtility::Stop(mHandles[373]);
        mFlags[150] = true;
        }
        }
        if (!mFlags[151]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "hallway_trigger4")) {
        mHandles[358] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid15", 0, "hallway_droid15", 2, -1, -1);
        mHandles[359] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid16", 0, "hallway_droid16", 2, -1, -1);
        MissionUtility::Stop(mHandles[358]);
        MissionUtility::Stop(mHandles[359]);
        mHandles[360] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid17", 0, "hallway_droid17", 2, -1, -1);
        mHandles[361] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid18", 0, "hallway_droid18", 2, -1, -1);
        MissionUtility::Stop(mHandles[360]);
        MissionUtility::Stop(mHandles[361]);
        mFlags[151] = true;
        }
        }
        if (!mFlags[152]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "hallway_trigger5")) {
        mHandles[362] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid19", 0, "hallway_droid19", 2, -1, -1);
        mHandles[363] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid20", 0, "hallway_droid20", 2, -1, -1);
        MissionUtility::Stop(mHandles[362]);
        MissionUtility::Stop(mHandles[363]);
        mHandles[364] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid21", 0, "hallway_droid21", 2, -1, -1);
        mHandles[365] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "hallway_droid22", 0, "hallway_droid22", 2, -1, -1);
        MissionUtility::Stop(mHandles[364]);
        MissionUtility::Stop(mHandles[365]);
        mHandles[366] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "hallway_droid23", 0, "hallway_droid23", 2, -1, -1);
        mHandles[367] = MissionUtility::CreateObjectWithRotation("cis_walk_small_jedi", "hallway_droid24", 0, "hallway_droid24", 2, -1, -1);
        MissionUtility::Stop(mHandles[366]);
        MissionUtility::Stop(mHandles[367]);
        mFlags[152] = true;
        }
        }
        if (!mFlags[46]) {
        if (!MissionUtility::IsAlive(mHandles[384])) {
        MissionUtility::ObjectiveComplete(mInts[4]);
        mFlags[46] = true;
        }
        }
        if (!mFlags[48]) {
        if (mFlags[46]) {
        if (!mFlags[49]) {
        MissionUtility::PlayMusic("EP2_V1_T13_01", true);
        MissionUtility::Wait(mHandles[150]);
        MissionUtility::Wait(mHandles[151]);
        MissionUtility::Wait(mHandles[152]);
        MissionUtility::Wait(mHandles[153]);
        MissionUtility::Wait(mHandles[154]);
        MissionUtility::Wait(mHandles[155]);
        MissionUtility::Wait(mHandles[156]);
        MissionUtility::Wait(mHandles[156]);
        MissionUtility::RemoveObject(mHandles[32]);
        MissionUtility::RemoveObject(mHandles[33]);
        MissionUtility::RemoveObject(mHandles[34]);
        MissionUtility::RemoveObject(mHandles[35]);
        MissionUtility::RemoveObject(mHandles[36]);
        MissionUtility::RemoveObject(mHandles[37]);
        MissionUtility::RemoveObject(mHandles[38]);
        MissionUtility::RemoveObject(mHandles[39]);
        MissionUtility::RemoveObject(mHandles[150]);
        MissionUtility::RemoveObject(mHandles[151]);
        MissionUtility::RemoveObject(mHandles[152]);
        MissionUtility::RemoveObject(mHandles[153]);
        MissionUtility::RemoveObject(mHandles[154]);
        MissionUtility::RemoveObject(mHandles[155]);
        MissionUtility::RemoveObject(mHandles[156]);
        MissionUtility::RemoveObject(mHandles[157]);
        mFlags[86] = true;
        MissionUtility::RemoveObject(mHandles[160]);
        MissionUtility::RemoveObject(mHandles[161]);
        mFlags[85] = true;
        mHandles[49] = MissionUtility::RunCin("gate_cin", true, true);
        mTimes[6] = 2.0f + MissionUtility::GetTime();
        mTimes[38] = 4.0f + MissionUtility::GetTime();
        mHandles[516] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath1", 0, "EndCinTank1", 0, -1, -1);
        mHandles[517] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath2", 0, "EndCinTank2", 0, -1, -1);
        mHandles[518] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath3", 0, "EndCinTank3", 0, -1, -1);
        mHandles[519] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath4", 0, "EndCinTank4", 0, -1, -1);
        MissionUtility::RemoveArmy(mHandles[287]);
        MissionUtility::RemoveArmy(mHandles[295]);
        MissionUtility::RemoveArmy(mHandles[288]);
        MissionUtility::RemoveArmy(mHandles[296]);
        MissionUtility::RemoveArmy(mHandles[289]);
        MissionUtility::RemoveArmy(mHandles[297]);
        MissionUtility::RemoveArmy(mHandles[290]);
        MissionUtility::RemoveArmy(mHandles[298]);
        MissionUtility::RemoveArmy(mHandles[291]);
        MissionUtility::RemoveArmy(mHandles[299]);
        MissionUtility::RemoveArmy(mHandles[292]);
        MissionUtility::RemoveArmy(mHandles[300]);
        MissionUtility::RemoveArmy(mHandles[293]);
        MissionUtility::RemoveArmy(mHandles[301]);
        MissionUtility::RemoveArmy(mHandles[294]);
        MissionUtility::RemoveArmy(mHandles[302]);
        mHandles[32] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath1", 0, "clone1", 0, -1, -1);
        mHandles[33] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath2", 0, "clone2", 0, -1, -1);
        mHandles[34] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath3", 0, "clone3", 0, -1, -1);
        mHandles[35] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath4", 0, "clone4", 0, -1, -1);
        mHandles[36] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath5", 0, "clone5", 0, -1, -1);
        mHandles[37] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath6", 0, "clone6", 0, -1, -1);
        mHandles[38] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath7", 0, "clone7", 0, -1, -1);
        mHandles[39] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath8", 0, "clone8", 0, -1, -1);
        mHandles[225] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "cin_clonetank_spawn1", 0, "cin_clonetank1", 1, -1, -1);
        mHandles[226] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "cin_clonetank_spawn2", 0, "cin_clonetank2", 1, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[516], true);
        MissionUtility::OverrideSoundRange(mHandles[517], true);
        MissionUtility::OverrideSoundRange(mHandles[518], true);
        MissionUtility::OverrideSoundRange(mHandles[519], true);
        MissionUtility::OverrideSoundRange(mHandles[32], true);
        MissionUtility::OverrideSoundRange(mHandles[33], true);
        MissionUtility::OverrideSoundRange(mHandles[34], true);
        MissionUtility::OverrideSoundRange(mHandles[35], true);
        MissionUtility::OverrideSoundRange(mHandles[36], true);
        MissionUtility::OverrideSoundRange(mHandles[37], true);
        MissionUtility::OverrideSoundRange(mHandles[38], true);
        MissionUtility::OverrideSoundRange(mHandles[39], true);
        MissionUtility::OverrideSoundRange(mHandles[225], true);
        MissionUtility::OverrideSoundRange(mHandles[226], true);
        mHandles[525] = MissionUtility::AddPropArmy(0, "EndCinCloneArmyPath", 0, 100, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[525], "EndCinCloneArmyPath1", true);
        mFlags[49] = true;
        }
        if (!mFlags[50]) {
        if (mTimes[6] < MissionUtility::GetTime()) {
        mHandles[50] = MissionUtility::GetHandle("gate1");
        MissionUtility::QueueSound("MWT22_08", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("AST22_08", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetCollidable(mHandles[50], false);
        MissionUtility::SetApplyDynamics(mHandles[50], true);
        MissionUtility::SetAnimation(mHandles[50], "fullanimation", 1.0f, 1);
        mFlags[50] = true;
        BeginTimer(mTimer17);
        }
        }
        if (mTimer17 > 1.0f) {
        MissionUtility::SetVelocForward(mHandles[32], 15.0f);
        MissionUtility::SetVelocForward(mHandles[33], 15.0f);
        MissionUtility::SetVelocForward(mHandles[34], 15.0f);
        MissionUtility::SetVelocForward(mHandles[35], 15.0f);
        MissionUtility::SetVelocForward(mHandles[36], 15.0f);
        MissionUtility::SetVelocForward(mHandles[37], 15.0f);
        MissionUtility::SetVelocForward(mHandles[38], 15.0f);
        MissionUtility::SetVelocForward(mHandles[39], 15.0f);
        StopTimer(mTimer17);
        mTimer17 = 0.0f;
        mHandles[520] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "EndCinGunshipPath1", 0, "EndCinGunship1", 1, -1, -1);
        mHandles[521] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "EndCinGunshipPath2", 0, "EndCinGunship2", 1, -1, -1);
        mHandles[522] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "EndCinFighterPath1", 0, "EndCinGunship1", 1, -1, -1);
        mHandles[523] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "EndCinFighterPath2", 0, "EndCinGunship2", 1, -1, -1);
        mHandles[524] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "EndCinFighterPath3", 0, "EndCinGunship3", 1, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[520], true);
        MissionUtility::OverrideSoundRange(mHandles[521], true);
        MissionUtility::OverrideSoundRange(mHandles[522], true);
        MissionUtility::OverrideSoundRange(mHandles[523], true);
        MissionUtility::OverrideSoundRange(mHandles[524], true);
        MissionUtility::SetApplyDynamics(mHandles[522], true);
        MissionUtility::SetApplyDynamics(mHandles[523], true);
        MissionUtility::SetApplyDynamics(mHandles[522], true);
        MissionUtility::SetApplyDynamics(mHandles[520], true);
        MissionUtility::SetApplyDynamics(mHandles[521], true);
        MissionUtility::Goto(mHandles[32], "EndCinClonePath1", false);
        MissionUtility::Goto(mHandles[33], "EndCinClonePath2", false);
        MissionUtility::Goto(mHandles[34], "EndCinClonePath3", false);
        MissionUtility::Goto(mHandles[35], "EndCinClonePath4", false);
        MissionUtility::Goto(mHandles[36], "EndCinClonePath5", false);
        MissionUtility::Goto(mHandles[37], "EndCinClonePath6", false);
        MissionUtility::Goto(mHandles[38], "EndCinClonePath7", false);
        MissionUtility::Goto(mHandles[39], "EndCinClonePath8", false);
        MissionUtility::Goto(mHandles[225], "cin_clonetank1_go", true);
        MissionUtility::Goto(mHandles[226], "cin_clonetank2_go", true);
        MissionUtility::Goto(mHandles[516], "EndCinTankPath1", false);
        MissionUtility::Goto(mHandles[517], "EndCinTankPath2", false);
        MissionUtility::Goto(mHandles[518], "EndCinTankPath3", false);
        MissionUtility::Goto(mHandles[519], "EndCinTankPath4", false);
        MissionUtility::Goto(mHandles[522], "EndCinFighterPath1", false);
        MissionUtility::Goto(mHandles[523], "EndCinFighterPath2", false);
        MissionUtility::Goto(mHandles[524], "EndCinFighterPath3", false);
        MissionUtility::Goto(mHandles[520], "EndCinGunshipPath1", false);
        MissionUtility::Goto(mHandles[521], "EndCinGunshipPath2", false);
        }
        if (!mFlags[112]) {
        if (mTimes[38] < MissionUtility::GetTime()) {
        MissionUtility::DamageObject(mHandles[150], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[151], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[152], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[153], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[154], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[155], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[156], 999999.0f, 999999.0f);
        MissionUtility::DamageObject(mHandles[157], 999999.0f, 999999.0f);
        mFlags[112] = true;
        }
        }
        if (mFlags[49]) {
        if (!MissionUtility::IsCinRunning(mHandles[49])) {
        mFlags[48] = true;
        }
        }
        }
        }
    }

    // ---- +0xc308  64 bytes ----
    if (!mFlags[236]) {
        if (MissionUtility::IsInsideRegion(mHandles[7], "gate_mech_trigger")) {
        MissionUtility::SetMaxHealth(mHandles[384], 100.0f);
        MissionUtility::SetCurHealth(mHandles[384], 100.0f);
        mFlags[236] = true;
        }
    }

    // ---- +0xc348  88 bytes ----
    if (!mFlags[113] && !mFlags[116] && mFlags[9]) {
        if (MissionUtility::IsAlive(mHandles[55])) {
        if (MissionUtility::IsAlive(mHandles[56])) {
        MissionUtility::BonusObjectiveComplete(mInts[6], true);
        mFlags[113] = true;
        }
        }
    }

    // ---- +0xc3a0  84 bytes ----
    if (!mFlags[116] && !mFlags[113] && !mFlags[9]) {
        if (!MissionUtility::IsAlive(mHandles[55])
            || !MissionUtility::IsAlive(mHandles[56])) {
        MissionUtility::BonusObjectiveFailed(mInts[6]);
        mFlags[116] = true;
        }
    }

    // ---- +0xc3f4  88 bytes ----
    if (!mFlags[114] && !mFlags[117] && mFlags[89]) {
        if (MissionUtility::IsAlive(mHandles[14])) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::BonusObjectiveComplete(mInts[7], true);
        mFlags[114] = true;
        }
        }
    }

    // ---- +0xc44c  96 bytes ----
    if (!mFlags[117] && !mFlags[114] && mFlags[80] && !mFlags[89]) {
        if (!MissionUtility::IsAlive(mHandles[14])
            || !MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::BonusObjectiveFailed(mInts[7]);
        mFlags[117] = true;
        }
    }

    // ---- +0xc4ac  312 bytes ----
    if (mFlags[80] && !mFlags[55]) {
        if (!mFlags[121]) {
        if (!MissionUtility::IsAlive(mHandles[71])) {
        mInts[9] = mInts[9] + 1;
        mFlags[121] = true;
        }
        }
        if (!mFlags[122]) {
        if (!MissionUtility::IsAlive(mHandles[72])) {
        mInts[9] = mInts[9] + 1;
        mFlags[122] = true;
        }
        }
        if (!mFlags[123]) {
        if (!MissionUtility::IsAlive(mHandles[73])) {
        mInts[9] = mInts[9] + 1;
        mFlags[123] = true;
        }
        }
        if (!mFlags[124]) {
        if (!MissionUtility::IsAlive(mHandles[74])) {
        mInts[9] = mInts[9] + 1;
        mFlags[124] = true;
        }
        }
        if (!mFlags[125]) {
        if (!MissionUtility::IsAlive(mHandles[75])) {
        mInts[9] = mInts[9] + 1;
        mFlags[125] = true;
        }
        }
        if (!mFlags[126]) {
        if (!MissionUtility::IsAlive(mHandles[76])) {
        mInts[9] = mInts[9] + 1;
        mFlags[126] = true;
        }
        }
    }

    // ---- +0xc5e4  68 bytes ----
    if (!mFlags[115] && !mFlags[118] && mFlags[55]) {
        if (mInts[9] < 3) {
        MissionUtility::BonusObjectiveComplete(mInts[8], true);
        mFlags[115] = true;
        }
    }

    // ---- +0xc628  64 bytes ----
    if (!mFlags[118] && !mFlags[115] && !mFlags[55]) {
        if (mInts[9] > 2) {
        MissionUtility::BonusObjectiveFailed(mInts[8]);
        mFlags[118] = true;
        }
    }

    // ---- +0xc668  52 bytes ----
    if (!mFlags[130]) {
        if (!MissionUtility::IsAlive(mHandles[7])) {
        mTimes[11] = 2.0f + MissionUtility::GetTime();
        mFlags[130] = true;
        }
    }

    // ---- +0xc69c  40 bytes ----
    if (!mFlags[69]) {
        if (mTimes[11] < MissionUtility::GetTime()) {
        MissionUtility::MissionFailure();
        mFlags[69] = true;
        }
    }

    // ---- +0xc6c4  36 bytes ----
    if (!mFlags[47] && mFlags[48]) {
        MissionUtility::MissionSuccess();
        mFlags[47] = true;
    }

    // ---- +0xc6e8  64 bytes ----
    if (MissionUtility::IsAlive(mHandles[275])) {
    if (!mFlags[10]) {
    if (MissionUtility::IsLanded(mHandles[275])) {
    MissionUtility::SetApplyDynamics(mHandles[275], false);
    mFlags[10] = true;
    }
    }
    }

    // ---- +0xc728  64 bytes ----
    if (MissionUtility::IsAlive(mHandles[276])) {
    if (!mFlags[11]) {
    if (MissionUtility::IsLanded(mHandles[276])) {
    MissionUtility::SetApplyDynamics(mHandles[276], false);
    mFlags[11] = true;
    }
    }
    }

    // ---- +0xc768  64 bytes ----
    if (MissionUtility::IsAlive(mHandles[187])) {
    if (!mFlags[12]) {
    if (MissionUtility::IsLanded(mHandles[187])) {
    MissionUtility::SetApplyDynamics(mHandles[187], false);
    mFlags[12] = true;
    }
    }
    }

}

void Thule2Script::Setup()
{
        mTimes[0] = 999999.9f;
        mTimes[4] = 999999.9f;
        mTimes[7] = 999999.9f;
        mTimes[8] = 999999.9f;
        mTimes[9] = 999999.9f;
        mTimes[10] = 999999.9f;
        mTimes[12] = 999999.9f;
        mTimes[13] = 999999.9f;
        mTimes[14] = 999999.9f;
        mTimes[15] = 999999.9f;
        mTimes[16] = 999999.9f;
        mTimes[17] = 999999.9f;
        mTimes[11] = 999999.9f;
        mTimes[18] = 999999.9f;
        mTimes[19] = 999999.9f;
        mTimes[20] = 999999.9f;
        mTimes[21] = 999999.9f;
        mTimes[39] = 999999.9f;
        mTimes[22] = 999999.9f;
        mTimes[47] = 999999.9f;
        mTimes[26] = 999999.9f;
        mTimes[27] = 999999.9f;
        mTimes[28] = 999999.9f;
        mTimes[29] = 999999.9f;
        mTimes[30] = 999999.9f;
        mTimes[31] = 999999.9f;
        mTimes[32] = 999999.9f;
        mTimes[33] = 999999.9f;
        mTimes[34] = 999999.9f;
        mTimes[35] = 999999.9f;
        mTimes[36] = 999999.9f;
        mTimes[23] = 999999.9f;
        mTimes[24] = 999999.9f;
        mTimes[25] = 999999.9f;
        mTimes[38] = 999999.9f;
        mTimes[40] = 999999.9f;
        mTimes[41] = 999999.9f;
        mTimes[42] = 999999.9f;
        mTimes[43] = 999999.9f;
        mTimes[44] = 999999.9f;
        mTimes[45] = 1.0f;
        mTimes[5] = 1.0f;
        mTimes[2] = 1.0f;
        mTimes[1] = 1.0f;
        mTimes[46] = 1.0f;
        mTimes[3] = 1.0f;
        mHandles[5] = MissionUtility::GetHandle("ltower1");
        mHandles[6] = MissionUtility::GetHandle("ltower2");
        mHandles[0] = MissionUtility::GetHandle("door1");
        mHandles[10] = MissionUtility::GetHandle("player_gunship");
        mHandles[22] = MissionUtility::GetHandle("end_turret1");
        mHandles[23] = MissionUtility::GetHandle("end_turret2");
        mHandles[24] = MissionUtility::GetHandle("end_turret3");
        mHandles[25] = MissionUtility::GetHandle("end_turret4");
        mHandles[26] = MissionUtility::GetHandle("end_tank1");
        mHandles[27] = MissionUtility::GetHandle("end_tank2");
        mHandles[28] = MissionUtility::GetHandle("end_tank3");
        mHandles[29] = MissionUtility::GetHandle("end_tank4");
        mHandles[30] = MissionUtility::GetHandle("end_tank5");
        mHandles[31] = MissionUtility::GetHandle("end_tank6");
        mHandles[109] = MissionUtility::GetHandle("gate_gtower1");
        mHandles[110] = MissionUtility::GetHandle("gate_gtower2");
        mHandles[50] = MissionUtility::GetHandle("gate1");
        mHandles[55] = MissionUtility::GetHandle("squadmate1");
        mHandles[56] = MissionUtility::GetHandle("squadmate2");
        mHandles[57] = MissionUtility::GetHandle("mortartank1");
        mHandles[58] = MissionUtility::GetHandle("mortartank2");
        mHandles[59] = MissionUtility::GetHandle("mortartank3");
        mHandles[60] = MissionUtility::GetHandle("mortartank4");
        mHandles[61] = MissionUtility::GetHandle("mortartank5");
        mHandles[62] = MissionUtility::GetHandle("mortartank6");
        mHandles[63] = MissionUtility::GetHandle("mortartank7");
        mHandles[64] = MissionUtility::GetHandle("mortartank8");
        mHandles[65] = MissionUtility::GetHandle("mortartank9");
        mHandles[66] = MissionUtility::GetHandle("mortartank10");
        mHandles[69] = MissionUtility::GetHandle("factory1");
        mHandles[70] = MissionUtility::GetHandle("factory2");
        mHandles[164] = MissionUtility::GetHandle("open_cin_assaultship");
        mHandles[147] = MissionUtility::GetHandle("ltower2_turret1");
        mHandles[148] = MissionUtility::GetHandle("ltower2_turret2");
        mHandles[90] = MissionUtility::GetHandle("delete_him1");
        mHandles[91] = MissionUtility::GetHandle("delete_him2");
        mHandles[92] = MissionUtility::GetHandle("asstank1");
        mHandles[93] = MissionUtility::GetHandle("asstank2");
        mHandles[94] = MissionUtility::GetHandle("asstank3");
        mHandles[95] = MissionUtility::GetHandle("sneaky_walker1");
        mHandles[96] = MissionUtility::GetHandle("sneaky_walker2");
        mHandles[103] = MissionUtility::GetHandle("tower1_defender1");
        mHandles[104] = MissionUtility::GetHandle("tower1_defender2");
        mHandles[105] = MissionUtility::GetHandle("tower1_defender4");
        mHandles[106] = MissionUtility::GetHandle("tower2_defender1");
        mHandles[107] = MissionUtility::GetHandle("tower2_defender3");
        mHandles[108] = MissionUtility::GetHandle("tower2_defender4");
        mHandles[191] = MissionUtility::GetHandle("landingship1_attacker1");
        mHandles[192] = MissionUtility::GetHandle("landingship1_attacker2");
        mHandles[193] = MissionUtility::GetHandle("landingship1_attacker3");
        mHandles[227] = MissionUtility::GetHandle("initial_walker1");
        mHandles[228] = MissionUtility::GetHandle("initial_walker2");
        mHandles[384] = MissionUtility::GetHandle("gate_mechanism");
        mHandles[397] = MissionUtility::GetHandle("spider1");
        mHandles[398] = MissionUtility::GetHandle("spider5");
        mHandles[399] = MissionUtility::GetHandle("spider6");
        mHandles[403] = MissionUtility::GetHandle("walker_tank1");
        mHandles[404] = MissionUtility::GetHandle("walker_tank2");
        mHandles[405] = MissionUtility::GetHandle("walker_tank3");
        mHandles[406] = MissionUtility::GetHandle("walker_tank4");
        mHandles[407] = MissionUtility::GetHandle("walker_tank5");
        mHandles[408] = MissionUtility::GetHandle("walker_tank6");
        mHandles[409] = MissionUtility::GetHandle("walker_tank7");
        mHandles[410] = MissionUtility::GetHandle("walker_tank8");
        mHandles[411] = MissionUtility::GetHandle("walker_tank9");
        mHandles[412] = MissionUtility::GetHandle("walker_tank10");
        mHandles[413] = MissionUtility::GetHandle("walker_tank11");
        mHandles[414] = MissionUtility::GetHandle("walker_tank12");
        mHandles[415] = MissionUtility::GetHandle("gtower1");
        mHandles[416] = MissionUtility::GetHandle("gtower2");
        mHandles[417] = MissionUtility::GetHandle("gtower3");
        mHandles[418] = MissionUtility::GetHandle("gtower4");
        mHandles[510] = MissionUtility::GetHandle("OpenCinTransport");
        MissionUtility::PreloadConfig("rep_walk_assault");
        MissionUtility::PreloadConfig("rep_walk_sixleg");
        MissionUtility::PreloadConfig("rep_fly_assault");
        MissionUtility::PreloadConfig("rep_fly_assault_far");
        MissionUtility::PreloadConfig("rep_tank_fighter1_player");
        MissionUtility::PreloadConfig("rep_tank_fighter1_squadmate");
        MissionUtility::PreloadConfig("cis_boss_cydon_thule2");
        MissionUtility::PreloadConfig("cis_fly_fighter");
        MissionUtility::PreloadConfig("rep_fly_fighter");
        MissionUtility::PreloadConfig("cis_tank_assault");
        MissionUtility::PreloadConfig("cis_walk_assault");
        MissionUtility::PreloadConfig("cis_inf_droid");
        MissionUtility::PreloadConfig("thu_bldg_factory_dest");
        MissionUtility::PreloadConfig("rep_tank_fighter1_window");
        MissionUtility::PreloadConfig("rep_fly_gunship");
        MissionUtility::PreloadConfig("rep_tank_fighter1");
        MissionUtility::PreloadConfig("rep_inf_mace");
        MissionUtility::PreloadConfig("rep_inf_clone");
        MissionUtility::PreloadConfig("cis_walk_small_jedi");
        MissionUtility::PreloadConfig("rep_fly_vcarrier");
        MissionUtility::PreloadConfig("thu_bldg_gate");
        MissionUtility::PreloadConfig("thu_prop_mechanism");
        MissionUtility::PreloadConfig("cis_fly_fighter_dest");
        MissionUtility::PreloadConfig("cis_bike_speeder");
        MissionUtility::PreloadConfig("cis_tank_wheeled");
        MissionUtility::PreloadConfig("cis_fly_vcarrier");
        MissionUtility::PreloadConfig("thu_bldg_hangar_dest");
        MissionUtility::PreloadConfig("REP_mortar_assault_xpl_soft");
        mInts[9] = 0;
        mInts[12] = 0;
        mInts[13] = 0;
}

SPMission *Thule2BuildMission()
{
    return new Thule2Script();
}
