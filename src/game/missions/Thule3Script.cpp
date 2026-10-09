// Thule3Script.cpp -- reconstruction of a shipped mission script.  BYTE-EXACT, all 4 functions.
//
// Head from tools/gen_mission_head.py, bodies from tools/gen_block.py, assembled by
// tools/gen_mission_tu.py.  Five edits -- see analysis/mission_batch_a.md.  Two are about
// the object layout rather than the code:
//   * `mPos` is a real `Vector` member at +0x24, inside what the head generator prints as
//     padding before the latches;
//   * the four `CreateObject` calls all copy into argument slot 0xcc, so the `Quat` is a
//     DEFAULT argument and the calls stop before it.

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
    Vector() {}
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
    int    AddBonusObjective(const char*);
    int    AddFlockMember(int, int, Vector);
    int    AddFlyerArmy(char, const char*, int, int, float);
    int    AddObjective(const char*);
    int    AddPropArmy(char, const char*, int, int, float);
    int    AddScreenTopHealthBar(int, const char*);
    void   AddSquadMember(int, int);
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
    int    CreateObject(const char*, int, const char*, const char*, int, int, int);
    int    CreateObjectSnap(const char*, int, const char*, const char*, int, int);
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    int    CreateRegionList(const char*, bool, bool);
    int    CreateSquad(int, Formation);
    void   DamageObject(int, float, float);
    void   DestroyRegionList(int);
    void   DisableKillCount();
    void   DisplayText(const char*, float, float);
    int    DropAmmoBox(int);
    int    DropHealthBox(int);
    void   EvictConfig(const char*);
    void   ExcludeTeam(int, int);
    void   FlushSoundQueue();
    int    GetCinId(int);
    float  GetDistance(int, const char*);
    float  GetDistance(int, const char*, int);
    float  GetDistance(int, int);
    int    GetHandle(const char*);
    int    GetPlayerHandle(int);
    int    GetPlayerKillCount();
    Vector GetPosition(int);
    int    GetRegionNewMember(int, int);
    int    GetRegionNewMemberCount(int);
    float  GetTime();
    void   Goto(int, const char*, bool);
    void   Goto(int, int);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsWaveSpawned(const char*);
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
    void   RemoveObject(int);
    void   RemoveObjectify(const char*, int);
    void   RemoveObjectify(int);
    void   RemoveTurnAroundRegion(const char*);
    int    RunCin(const char*, bool, bool);
    void   SetAlliance(int, int);
    void   SetAllianceOneWay(int, int);
    void   SetAltitude(int, float);
    void   SetAnimation(int, const char*, float, int);
    void   SetApplyDynamics(int, bool);
    void   SetAttackRange(int, int);
    void   SetCameraAngle(float);
    void   SetCameraDistance(float);
    void   SetCameraOffset(Vector);
    void   SetCollidable(int, bool);
    void   SetCurHealth(int, float);
    void   SetCurShield(int, float);
    void   SetEnemiesOneWay(int, int);
    void   SetFogRange(float, float, float);
    void   SetImportantFlag(int, bool);
    void   SetLightningCount(int);
    void   SetLightningDelay(float, float);
    void   SetLightningSource(int);
    void   SetMaxAltitude(int, float);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetNeutralOneWay(int, int);
    void   SetOmega(int, const Vector&);
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
    void   SetWeaponOrd(int, const char*, const char*);
    void   ShakeCamera(float, float, float);
    void   StartAmbiences(const char*, const char*, float, float);
    int    StartSound(const char*, bool, float, float, float, const char*, int, const char*);
    void   StartSoundAtObject(const char*, int, bool, float);
    void   Stop(int);
    void   StopMusic(int);
    void   StopSound(int);
    void   Wait(int);
}

static const char *const kClassName = "Thule3Script";
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

class Thule3Script : public SPMission
{
public:
    virtual ~Thule3Script();

    Thule3Script()
    {
        mBoolCount  = 127;           mBools = mFlags;
        mCountB     = 31;            mIntsB = mTimes;
        mCountC     = 184;           mIntsC = mHandles;
        mCountD     = 14;            mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    Vector mPos;                        // +0x024
    char   mPad30[1];
    bool   mFlags[127];            // +0x031  the one-shot latches
    char   mPadB0[8];                  // +0x0b0
    float    mTimes[31];            // +0x0b8
    char   mPad134[8];                  // +0x134
    int    mHandles[184];          // +0x13c
    char   mPad41C[8];                  // +0x41c
    int    mInts[14];             // +0x424
    char   mPad45C[4];                  // +0x45c
    Timer  mTimer0;                      // +0x460
    Timer  mTimer1;                      // +0x46c
    Timer  mTimer2;                      // +0x478
    Timer  mTimer3;                      // +0x484
    Timer  mTimer4;                      // +0x490
    Timer  mTimer5;                      // +0x49c
    Timer  mTimer6;                      // +0x4a8
    Timer  mTimer7;                      // +0x4b4
};

Thule3Script::~Thule3Script()
{
}

void Thule3Script::Execute()
{
    // ---- +0x0008  1300 bytes ----
    mHandles[0] = MissionUtility::GetPlayerHandle(0);
    if (!mFlags[17]) {
    MissionUtility::SetMusicLooping(true);
    mInts[11] = MissionUtility::AddBonusObjective("missions.Thule3.bonus.str0003");
    mInts[12] = MissionUtility::AddBonusObjective("missions.Thule3.bonus.str0004");
    mInts[13] = MissionUtility::AddBonusObjective("missions.Thule3.bonus.str0002");
    MissionUtility::Stop(mHandles[32]);
    MissionUtility::Stop(mHandles[33]);
    int savePoint = MissionUtility::MidMissionGetSavePoint();
    if (!savePoint) {
    MissionUtility::SetMaxHealth(mHandles[16], 800.0f);
    MissionUtility::SetCurHealth(mHandles[16], 800.0f);
    MissionUtility::SetMaxHealth(mHandles[19], 800.0f);
    MissionUtility::SetCurHealth(mHandles[19], 800.0f);
    MissionUtility::SetMaxHealth(mHandles[20], 800.0f);
    MissionUtility::SetCurHealth(mHandles[20], 800.0f);
    MissionUtility::SetMaxHealth(mHandles[21], 800.0f);
    MissionUtility::SetCurHealth(mHandles[21], 800.0f);
    MissionUtility::SetMaxHealth(mHandles[22], 800.0f);
    MissionUtility::SetCurHealth(mHandles[22], 800.0f);
    MissionUtility::SetMaxHealth(mHandles[23], 800.0f);
    MissionUtility::SetCurHealth(mHandles[23], 800.0f);
    BeginTimer(mTimer1);
    }
    if (savePoint == 1) {
    MissionUtility::EvictConfig("rep_walk_sixleg");
    mFlags[82] = true;
    mFlags[23] = true;
    MissionUtility::BeginWave("cydon_wave");
    MissionUtility::BeginWave("major_wave");
    MissionUtility::BeginWave("trigger10");
    mInts[1] = MissionUtility::AddObjective("missions.Thule3.objective.str0003");
    MissionUtility::ObjectiveComplete(mInts[1]);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mFlags[86]);
    MissionUtility::MidMissionLoad(mFlags[87]);
    MissionUtility::MidMissionLoad(mFlags[74]);
    MissionUtility::MidMissionLoad(mFlags[75]);
    MissionUtility::MidMissionLoad(mFlags[76]);
    MissionUtility::MidMissionLoad(mFlags[77]);
    MissionUtility::MidMissionLoad(mFlags[78]);
    MissionUtility::MidMissionLoad(mFlags[79]);
    MissionUtility::MoveObjectWithRotation(mHandles[0], "loadpoint1_player", 0, true);
    if (mFlags[86]
        || mFlags[87]) {
    mHandles[2] = MissionUtility::CreateSquad(mHandles[0], (Formation)1);
    }
    if (!mFlags[86]) {
    MissionUtility::RemoveObject(mHandles[8]);
    }
    if (!mFlags[87]) {
    MissionUtility::RemoveObject(mHandles[9]);
    }
    if (mFlags[86]) {
    MissionUtility::MoveObjectWithRotation(mHandles[8], "loadpoint1_squadmate1", 0, true);
    MissionUtility::AddSquadMember(mHandles[2], mHandles[8]);
    }
    if (mFlags[87]) {
    MissionUtility::MoveObjectWithRotation(mHandles[9], "loadpoint1_squadmate2", 0, true);
    MissionUtility::AddSquadMember(mHandles[2], mHandles[9]);
    }
    if (mFlags[74]) {
    MissionUtility::BonusObjectiveComplete(mInts[11], false);
    }
    if (mFlags[75]) {
    MissionUtility::BonusObjectiveFailed(mInts[11]);
    }
    if (mFlags[76]) {
    MissionUtility::BonusObjectiveComplete(mInts[12], false);
    }
    if (mFlags[77]) {
    MissionUtility::BonusObjectiveFailed(mInts[12]);
    }
    if (mFlags[78]) {
    MissionUtility::BonusObjectiveComplete(mInts[13], false);
    }
    if (mFlags[79]) {
    MissionUtility::BonusObjectiveFailed(mInts[13]);
    }
    }
    if (savePoint == 2) {
    mFlags[82] = true;
    mFlags[83] = true;
    MissionUtility::EvictConfig("rep_walk_sixleg");
    mTimes[17] = MissionUtility::GetTime();
    MissionUtility::AddTurnAroundRegion("turnaround1", "turnaround1_point1", "turnaround1_point2", "turnaround1_point3", 0, 0);
    mInts[1] = MissionUtility::AddObjective("missions.Thule3.objective.str0003");
    MissionUtility::ObjectiveComplete(mInts[1]);
    mInts[2] = MissionUtility::AddObjective("missions.Thule3.objective.str0004");
    MissionUtility::ObjectiveComplete(mInts[2]);
    MissionUtility::RemoveObject(mHandles[10]);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mFlags[86]);
    MissionUtility::MidMissionLoad(mFlags[87]);
    MissionUtility::MidMissionLoad(mFlags[74]);
    MissionUtility::MidMissionLoad(mFlags[75]);
    MissionUtility::MidMissionLoad(mFlags[76]);
    MissionUtility::MidMissionLoad(mFlags[77]);
    MissionUtility::MidMissionLoad(mFlags[78]);
    MissionUtility::MidMissionLoad(mFlags[79]);
    if (mFlags[86]
        || mFlags[87]) {
    mHandles[2] = MissionUtility::CreateSquad(mHandles[0], (Formation)1);
    }
    if (!mFlags[86]) {
    MissionUtility::RemoveObject(mHandles[8]);
    }
    if (!mFlags[87]) {
    MissionUtility::RemoveObject(mHandles[9]);
    }
    if (mFlags[86]) {
    MissionUtility::MoveObjectWithRotation(mHandles[8], "loadpoint2_squadmate1", 0, true);
    MissionUtility::AddSquadMember(mHandles[2], mHandles[8]);
    }
    if (mFlags[87]) {
    MissionUtility::MoveObjectWithRotation(mHandles[9], "loadpoint2_squadmate2", 0, true);
    MissionUtility::AddSquadMember(mHandles[2], mHandles[9]);
    }
    if (mFlags[74]) {
    MissionUtility::BonusObjectiveComplete(mInts[11], false);
    }
    if (mFlags[75]) {
    MissionUtility::BonusObjectiveFailed(mInts[11]);
    }
    if (mFlags[76]) {
    MissionUtility::BonusObjectiveComplete(mInts[12], false);
    }
    if (mFlags[77]) {
    MissionUtility::BonusObjectiveFailed(mInts[12]);
    }
    if (mFlags[78]) {
    MissionUtility::BonusObjectiveComplete(mInts[13], false);
    }
    if (mFlags[79]) {
    MissionUtility::BonusObjectiveFailed(mInts[13]);
    }
    MissionUtility::BeginWave("cydon_wave");
    MissionUtility::BeginWave("major_wave");
    MissionUtility::BeginWave("trigger10");
    }
    MissionUtility::StartAmbiences("AmbThule_Main01_pl2", "PropGen_thunderLt01", 10.0f, 30.0f);
    MissionUtility::SetSquadAttackSound("squad_attacktarget");
    MissionUtility::SetSquadBreakSound("squad_breakandattack");
    MissionUtility::SetSquadHoldSound("squad_holdposition");
    MissionUtility::SetSquadRegroupSound("squad_regroup");
    MissionUtility::SetSquadInvalidSound("squad_invalidtarget");
    mFlags[17] = true;
    }

    // ---- +0x051c  84 bytes ----
    if (!mFlags[74] && !mFlags[75]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "temple")) {
        if (mTimer1 < 331.0f) {
        MissionUtility::BonusObjectiveComplete(mInts[11], true);
        mFlags[74] = true;
        }
        }
    }

    // ---- +0x0570  80 bytes ----
    if (!mFlags[75] && !mFlags[74]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "temple")) {
        if (mTimer1 > 330.0f) {
        MissionUtility::BonusObjectiveFailed(mInts[11]);
        mFlags[75] = true;
        }
        }
    }

    // ---- +0x05c0  96 bytes ----
    if (!mFlags[76] && !mFlags[77]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "temple")) {
        if (MissionUtility::IsAlive(mHandles[8])) {
        if (MissionUtility::IsAlive(mHandles[9])) {
        MissionUtility::BonusObjectiveComplete(mInts[12], true);
        mFlags[76] = true;
        }
        }
        }
    }

    // ---- +0x0620  72 bytes ----
    if (!mFlags[77] && !mFlags[76]) {
        if (!MissionUtility::IsAlive(mHandles[8])
            || !MissionUtility::IsAlive(mHandles[9])) {
        MissionUtility::BonusObjectiveFailed(mInts[12]);
        mFlags[77] = true;
        }
    }

    // ---- +0x0668  56 bytes ----
    if (!mFlags[78] && !mFlags[79]) {
        if (MissionUtility::GetPlayerKillCount() > 44) {
        MissionUtility::BonusObjectiveComplete(mInts[13], true);
        mFlags[78] = true;
        }
    }

    // ---- +0x06a0  72 bytes ----
    if (!mFlags[79] && !mFlags[78]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "temple")) {
        if (MissionUtility::GetPlayerKillCount() < 45) {
        MissionUtility::BonusObjectiveFailed(mInts[13]);
        mFlags[79] = true;
        }
        }
    }

    // ---- +0x06e8  10884 bytes ----
    if (!mFlags[82]) {
        if (!mFlags[73]) {
        if (!mFlags[72]) {
        MissionUtility::PlayMusic("EP2_V1_T03_02", true);
        mHandles[39] = MissionUtility::RunCin("open_cin", true, true);
        mFlags[72] = true;
        mHandles[169] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "OpenCinSixLegPath", 0, "OpenCinSixLeg", 1, -1, -1);
        mHandles[170] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "OpenCinSixLegPath", 1, "OpenCinSixLeg1", 1, -1, -1);
        mHandles[171] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "OpenCinSixLegPath", 2, "OpenCinSixLeg2", 1, -1, -1);
        mHandles[166] = MissionUtility::CreateObjectWithRotation("rep_walk_assault", "OpenCinAssaultWalkerPath", 0, "OpenCinAssaultWalker", 1, -1, -1);
        mHandles[167] = MissionUtility::CreateObjectWithRotation("rep_walk_assault", "OpenCinAssaultWalkerPath", 1, "OpenCinAssaultWalker1", 1, -1, -1);
        mHandles[158] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "OpenCinDroidTankPath", 0, "OpenCinDroidTank", 2, -1, -1);
        mHandles[159] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "OpenCinDroidTankPath", 1, "OpenCinDroidTank1", 2, -1, -1);
        mHandles[160] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "OpenCinDroidTankPath", 2, "OpenCinDroidTank2", 2, -1, -1);
        mHandles[164] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "OpenCinWheelTankPath", 0, "OpenCinWheelTank", 2, -1, -1);
        mHandles[165] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "OpenCinWheelTankPath", 1, "OpenCinWheelTank", 2, -1, -1);
        mHandles[161] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "OpenCinDroidWalkerPath", 0, "OpenCinDroidWalker", 2, -1, -1);
        mHandles[162] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "OpenCinDroidWalkerPath", 1, "OpenCinDroidWalker1", 2, -1, -1);
        mHandles[163] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "OpenCinDroidWalkerPath", 2, "OpenCinDroidWalker2", 2, -1, -1);
        mHandles[154] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath", 0, 18, 1.5f);
        mHandles[155] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath", 1, 25, 1.5f);
        mHandles[156] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath", 2, 20, 1.5f);
        mHandles[157] = MissionUtility::AddPropArmy(0, "OpenCinCloneArmyPath", 3, 10, 1.5f);
        mHandles[150] = MissionUtility::AddPropArmy(1, "OpenCinDroidArmyPath", 0, 15, 1.5f);
        mHandles[151] = MissionUtility::AddPropArmy(1, "OpenCinDroidArmyPath", 1, 20, 1.5f);
        mHandles[152] = MissionUtility::AddPropArmy(1, "OpenCinDroidArmyPath", 2, 12, 1.5f);
        mHandles[153] = MissionUtility::AddPropArmy(1, "OpenCinDroidArmyPath", 3, 15, 1.5f);
        mHandles[172] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "OpenCinPlayerPath", 0, "OpenCinPlayer", 1, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[169], true);
        MissionUtility::OverrideSoundRange(mHandles[170], true);
        MissionUtility::OverrideSoundRange(mHandles[171], true);
        MissionUtility::OverrideSoundRange(mHandles[166], true);
        MissionUtility::OverrideSoundRange(mHandles[167], true);
        MissionUtility::OverrideSoundRange(mHandles[158], true);
        MissionUtility::OverrideSoundRange(mHandles[159], true);
        MissionUtility::OverrideSoundRange(mHandles[160], true);
        MissionUtility::OverrideSoundRange(mHandles[164], true);
        MissionUtility::OverrideSoundRange(mHandles[165], true);
        MissionUtility::OverrideSoundRange(mHandles[161], true);
        MissionUtility::OverrideSoundRange(mHandles[162], true);
        MissionUtility::OverrideSoundRange(mHandles[163], true);
        MissionUtility::SetVelocForward(mHandles[164], 50.0f);
        MissionUtility::SetVelocForward(mHandles[165], 50.0f);
        MissionUtility::AttackTarget(mHandles[172], mHandles[158], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[8], mHandles[161], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[9], mHandles[162], true, true, false, false);
        MissionUtility::SetCurHealth(mHandles[158], 100.0f);
        MissionUtility::SetCurShield(mHandles[158], 10.0f);
        MissionUtility::SetCurHealth(mHandles[159], 100.0f);
        MissionUtility::SetCurShield(mHandles[159], 10.0f);
        MissionUtility::SetCurHealth(mHandles[160], 100.0f);
        MissionUtility::SetCurShield(mHandles[160], 10.0f);
        MissionUtility::SetCurHealth(mHandles[164], 100.0f);
        MissionUtility::SetCurShield(mHandles[164], 10.0f);
        MissionUtility::SetCurHealth(mHandles[165], 100.0f);
        MissionUtility::SetCurShield(mHandles[165], 10.0f);
        BeginTimer(mTimer2);
        BeginTimer(mTimer3);
        }
        if (!mFlags[121]) {
        if (mTimer2 > 1.0f) {
        StopTimer(mTimer2);
        mTimer2 = 0.0f;
        MissionUtility::QueueSound("MWT23_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("ANT23_01", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        }
        if (!mFlags[121]) {
        if (mTimer3 > 10.0f) {
        if (MissionUtility::GetCinId(mHandles[39]) == 3) {
        StopTimer(mTimer3);
        mTimer3 = 0.0f;
        MissionUtility::MoveObjectWithRotation(mHandles[172], "PlayerStart", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[8], "PlayerStart", 1, true);
        MissionUtility::MoveObjectWithRotation(mHandles[9], "PlayerStart", 2, true);
        MissionUtility::SetVelocForward(mHandles[172], 300.0f);
        MissionUtility::SetVelocForward(mHandles[8], 300.0f);
        MissionUtility::SetVelocForward(mHandles[9], 300.0f);
        MissionUtility::Stop(mHandles[172]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[172], "OpenCinGotoPath", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Stop(mHandles[8]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[8], "OpenCinGotoPath", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Stop(mHandles[9]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[9], "OpenCinGotoPath", false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
        if (mFlags[72]) {
        if (!MissionUtility::IsCinRunning(mHandles[39])) {
        MissionUtility::FlushSoundQueue();
        mFlags[121] = true;
        MissionUtility::Objectify("marker1", 0, "missions.Thule3.marker.str0005", true, false, 0.0f, 2.0f);
        MissionUtility::DisplayText("missions.Thule3.text.str0011", 6.0f, -1.0f);
        mInts[1] = MissionUtility::AddObjective("missions.Thule3.objective.str0003");
        mHandles[73] = MissionUtility::AddFlyerArmy(0, "prop_flyers_spawn1", 0, 3, 0.1f);
        mHandles[74] = MissionUtility::AddFlyerArmy(2, "prop_flyers_spawn2", 0, 3, 0.1f);
        mHandles[75] = MissionUtility::AddFlyerArmy(1, "prop_flyers_spawn3", 0, 3, 0.1f);
        mHandles[76] = MissionUtility::AddFlyerArmy(2, "prop_flyers_spawn4", 0, 3, 0.1f);
        mHandles[77] = MissionUtility::AddFlyerArmy(0, "prop_flyers_spawn5", 0, 3, 0.1f);
        mHandles[78] = MissionUtility::AddFlyerArmy(2, "prop_flyers_spawn6", 0, 3, 0.1f);
        mHandles[79] = MissionUtility::AddFlyerArmy(1, "prop_flyers_spawn7", 0, 3, 0.1f);
        mHandles[80] = MissionUtility::AddFlyerArmy(2, "prop_flyers_spawn8", 0, 3, 0.1f);
        mHandles[81] = MissionUtility::AddFlyerArmy(0, "prop_flyers_spawn9", 0, 3, 0.1f);
        mHandles[82] = MissionUtility::AddFlyerArmy(2, "prop_flyers_spawn10", 0, 3, 0.1f);
        mHandles[85] = MissionUtility::AddFlyerArmy(1, "prop_flyers_spawn13", 0, 3, 0.1f);
        mHandles[86] = MissionUtility::AddFlyerArmy(2, "prop_flyers_spawn14", 0, 3, 0.1f);
        mHandles[89] = MissionUtility::AddPropArmy(1, "inf_army_spawn1", 0, 50, 1.3f);
        mHandles[90] = MissionUtility::AddPropArmy(0, "inf_army_spawn2", 0, 50, 1.3f);
        mHandles[91] = MissionUtility::AddPropArmy(1, "inf_army_spawn3", 0, 50, 1.5f);
        mHandles[92] = MissionUtility::AddPropArmy(0, "inf_army_spawn4", 0, 50, 1.5f);
        mHandles[93] = MissionUtility::AddPropArmy(1, "inf_army_spawn5", 0, 50, 1.0f);
        mHandles[94] = MissionUtility::AddPropArmy(0, "inf_army_spawn6", 0, 50, 1.0f);
        mHandles[95] = MissionUtility::AddPropArmy(1, "inf_army_spawn7", 0, 50, 1.0f);
        mHandles[96] = MissionUtility::AddPropArmy(0, "inf_army_spawn8", 0, 50, 1.0f);
        mHandles[97] = MissionUtility::AddPropArmy(1, "inf_army_spawn9", 0, 50, 1.0f);
        mHandles[98] = MissionUtility::AddPropArmy(0, "inf_army_spawn10", 0, 50, 1.0f);
        mHandles[99] = MissionUtility::AddPropArmy(1, "inf_army_spawn11", 0, 50, 1.0f);
        mHandles[100] = MissionUtility::AddPropArmy(0, "inf_army_spawn12", 0, 50, 1.0f);
        mHandles[101] = MissionUtility::AddPropArmy(1, "inf_army_spawn13", 0, 50, 1.0f);
        mHandles[102] = MissionUtility::AddPropArmy(0, "inf_army_spawn14", 0, 50, 1.0f);
        mHandles[103] = MissionUtility::AddPropArmy(1, "inf_army_spawn15", 0, 50, 1.0f);
        mHandles[104] = MissionUtility::AddPropArmy(0, "inf_army_spawn16", 0, 50, 1.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[89], "inf_army_spawn1", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[90], "army_spawn2_patrol", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[91], "inf_army_spawn3", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[92], "inf_army_spawn4", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[93], "inf_army_spawn5", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[94], "inf_army_spawn6", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[95], "inf_army_spawn7", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[96], "inf_army_spawn8", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[97], "inf_army_spawn9", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[98], "inf_army_spawn10", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[99], "inf_army_spawn11", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[100], "inf_army_spawn12", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[101], "inf_army_spawn13", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[102], "inf_army_spawn14", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[103], "inf_army_spawn15", true);
        MissionUtility::SetPropArmyWayPoints(mHandles[104], "inf_army_spawn16", true);
        MissionUtility::PlayMusic("EP2_V1_T03_02", true);
        MissionUtility::RemoveObject(mHandles[158]);
        MissionUtility::RemoveObject(mHandles[159]);
        MissionUtility::RemoveObject(mHandles[160]);
        MissionUtility::RemoveObject(mHandles[164]);
        MissionUtility::RemoveObject(mHandles[165]);
        MissionUtility::RemoveObject(mHandles[161]);
        MissionUtility::RemoveObject(mHandles[162]);
        MissionUtility::RemoveObject(mHandles[163]);
        MissionUtility::RemoveObject(mHandles[169]);
        MissionUtility::RemoveObject(mHandles[170]);
        MissionUtility::RemoveObject(mHandles[171]);
        MissionUtility::RemoveObject(mHandles[166]);
        MissionUtility::RemoveObject(mHandles[167]);
        MissionUtility::RemoveObject(mHandles[172]);
        MissionUtility::RemoveArmy(mHandles[154]);
        MissionUtility::RemoveArmy(mHandles[155]);
        MissionUtility::RemoveArmy(mHandles[156]);
        MissionUtility::RemoveArmy(mHandles[157]);
        MissionUtility::RemoveArmy(mHandles[150]);
        MissionUtility::RemoveArmy(mHandles[151]);
        MissionUtility::RemoveArmy(mHandles[152]);
        MissionUtility::RemoveArmy(mHandles[153]);
        MissionUtility::MoveObjectWithRotation(mHandles[0], "PlayerStart", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[8], "PlayerStart", 1, true);
        MissionUtility::MoveObjectWithRotation(mHandles[9], "PlayerStart", 2, true);
        mHandles[2] = MissionUtility::CreateSquad(mHandles[0], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[2], mHandles[8]);
        MissionUtility::AddSquadMember(mHandles[2], mHandles[9]);
        MissionUtility::EvictConfig("rep_walk_sixleg");
        mFlags[73] = true;
        }
        }
        }
        if (!MissionUtility::IsAlive(mHandles[68])) {
        mHandles[68] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship_path1", 0, "landingship1", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[68], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[68], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[68], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[68], true);
        MissionUtility::Goto(mHandles[68], "landingship_path1", true);
        }
        if (MissionUtility::IsAlive(mHandles[68])) {
        if (MissionUtility::GetDistance(mHandles[68], "landingship_path1", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[68]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[69])) {
        mHandles[69] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship_path2", 0, "landingship2", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[69], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[69], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[69], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[69], true);
        MissionUtility::Goto(mHandles[69], "landingship_path2", true);
        }
        if (MissionUtility::IsAlive(mHandles[69])) {
        if (MissionUtility::GetDistance(mHandles[69], "landingship_path2", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[69]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[70])) {
        mHandles[70] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship_path3", 0, "landingship3", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[70], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[70], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[70], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[70], true);
        MissionUtility::Goto(mHandles[70], "landingship_path3", true);
        }
        if (MissionUtility::IsAlive(mHandles[70])) {
        if (MissionUtility::GetDistance(mHandles[70], "landingship_path3", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[70]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[71])) {
        mHandles[71] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship_path4", 0, "landingship4", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[71], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[71], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[71], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[71], true);
        MissionUtility::Goto(mHandles[71], "landingship_path4", true);
        }
        if (MissionUtility::IsAlive(mHandles[71])) {
        if (MissionUtility::GetDistance(mHandles[71], "landingship_path4", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[71]);
        }
        }
        if (!MissionUtility::IsAlive(mHandles[72])) {
        mHandles[72] = MissionUtility::CreateObjectWithRotation("rep_fly_assault_far", "landingship_path5", 0, "landingship5", 0, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[72], 15.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[72], 15.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[72], 15.0f);
        MissionUtility::SetApplyDynamics(mHandles[72], true);
        MissionUtility::Goto(mHandles[72], "landingship_path5", true);
        }
        if (MissionUtility::IsAlive(mHandles[72])) {
        if (MissionUtility::GetDistance(mHandles[72], "landingship_path5", 1) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[72]);
        }
        }
        if (!mFlags[67]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger1")) {
        mHandles[40] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_path1", 0, "spawned_enemy1", 2, -1, -1);
        mHandles[41] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_path2", 0, "spawned_enemy2", 2, -1, -1);
        mHandles[48] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_path9", 0, "spawned_enemy9", 2, -1, -1);
        mHandles[49] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_path10", 0, "spawned_enemy10", 2, -1, -1);
        MissionUtility::Goto(mHandles[40], "enemy_path1", true);
        MissionUtility::Goto(mHandles[41], "enemy_path2", true);
        MissionUtility::Goto(mHandles[48], "enemy_path9", true);
        MissionUtility::Goto(mHandles[49], "enemy_path10", true);
        mHandles[42] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_spawn3", 0, "spawned_enemy3", 2, -1, -1);
        mHandles[43] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_spawn4", 0, "spawned_enemy4", 2, -1, -1);
        mHandles[34] = MissionUtility::CreateFlock(mHandles[42], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[34], mHandles[43], Vector(25.0f, 0.0f, -110.0f));
        MissionUtility::SetAttackRange(mHandles[42], 250);
        MissionUtility::SetAttackRange(mHandles[43], 250);
        MissionUtility::Patrol(mHandles[34], "enemy_spawn34_path", 500.0f, true);
        mHandles[44] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_spawn5", 0, "spawned_enemy5", 2, -1, -1);
        mHandles[45] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_spawn6", 0, "spawned_enemy6", 2, -1, -1);
        mHandles[35] = MissionUtility::CreateFlock(mHandles[44], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[35], mHandles[45], Vector(25.0f, 0.0f, -110.0f));
        MissionUtility::SetAttackRange(mHandles[44], 250);
        MissionUtility::SetAttackRange(mHandles[45], 250);
        MissionUtility::Patrol(mHandles[35], "enemy_spawn56_path", 500.0f, true);
        mHandles[117] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "spawned_walker1_path", 0, "spawned_walker1", 2, -1, -1);
        mHandles[118] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "spawned_walker2_path", 0, "spawned_walker2", 2, -1, -1);
        MissionUtility::Patrol(mHandles[117], "spawned_walker1_path", 300.0f, true);
        MissionUtility::Patrol(mHandles[118], "spawned_walker2_path", 300.0f, true);
        mFlags[67] = true;
        }
        }
        if (!mFlags[68]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger2")) {
        mHandles[46] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_spawn7", 0, "spawned_enemy7", 2, -1, -1);
        mHandles[47] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_spawn8", 0, "spawned_enemy8", 2, -1, -1);
        MissionUtility::Goto(mHandles[46], "enemy_go7", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Wait(mHandles[46]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[47], "enemy_go8", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Wait(mHandles[47]);
        MissionUtility::SetQueueFlag(false);
        mHandles[121] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "aat3", 0, "aat3", 2, -1, -1);
        mHandles[122] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "aat4", 0, "aat4", 2, -1, -1);
        MissionUtility::Stop(mHandles[121]);
        MissionUtility::Stop(mHandles[122]);
        MissionUtility::BeginWave("turrets_wave1");
        mFlags[68] = true;
        }
        }
        if (!mFlags[20]) {
        if (MissionUtility::IsWaveSpawned("turrets_wave1")) {
        mHandles[144] = MissionUtility::GetHandle("window_tank1");
        mHandles[145] = MissionUtility::GetHandle("window_tank2");
        MissionUtility::SetWeaponOrd(mHandles[144], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        MissionUtility::SetWeaponOrd(mHandles[145], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        MissionUtility::SetCurHealth(mHandles[144], 200.0f);
        MissionUtility::SetCurHealth(mHandles[145], 200.0f);
        mFlags[20] = true;
        }
        }
        if (!mFlags[69]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger3")) {
        MissionUtility::Patrol(mHandles[55], "wheeled1_go", 500.0f, true);
        MissionUtility::Patrol(mHandles[56], "wheeled2_go", 500.0f, true);
        MissionUtility::Patrol(mHandles[61], "wheeled1_go", 500.0f, true);
        MissionUtility::Patrol(mHandles[62], "wheeled2_go", 500.0f, true);
        mFlags[69] = true;
        }
        }
        if (!mFlags[70]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger4")) {
        mHandles[57] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "enemy_spawn9", 0, "wheeled3", 2, -1, -1);
        mHandles[58] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "enemy_spawn10", 0, "wheeled4", 2, -1, -1);
        MissionUtility::Patrol(mHandles[57], "wheeled1_go", 500.0f, true);
        MissionUtility::Patrol(mHandles[58], "wheeled2_go", 500.0f, true);
        mFlags[70] = true;
        }
        }
        if (!mFlags[71]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger5")) {
        mHandles[59] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "enemy_spawn9", 0, "wheeled5", 2, -1, -1);
        mHandles[60] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "enemy_spawn10", 0, "wheeled6", 2, -1, -1);
        MissionUtility::Patrol(mHandles[59], "wheeled1_go", 500.0f, true);
        MissionUtility::Patrol(mHandles[60], "wheeled2_go", 500.0f, true);
        mFlags[71] = true;
        }
        }
        if (!mFlags[18]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger6")) {
        mHandles[50] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_path11", 0, "spawned_enemy11", 2, -1, -1);
        mHandles[51] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_path12", 0, "spawned_enemy12", 2, -1, -1);
        mHandles[52] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_path13", 0, "spawned_enemy13", 2, -1, -1);
        mHandles[53] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_path14", 0, "spawned_enemy14", 2, -1, -1);
        MissionUtility::Patrol(mHandles[50], "enemy_path11", 500.0f, true);
        MissionUtility::Patrol(mHandles[51], "enemy_path12", 500.0f, true);
        MissionUtility::Patrol(mHandles[52], "enemy_path13", 500.0f, true);
        MissionUtility::Patrol(mHandles[53], "enemy_path14", 500.0f, true);
        mHandles[119] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "aat_spawn1", 0, "aat1", 2, -1, -1);
        mHandles[120] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "aat_spawn1", 1, "aat2", 2, -1, -1);
        MissionUtility::Stop(mHandles[119]);
        MissionUtility::Stop(mHandles[120]);
        MissionUtility::BeginWave("major_wave");
        mFlags[18] = true;
        }
        }
        if (!mFlags[97]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger7")) {
        mHandles[123] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier1", 0, "enemy_vcarrier1", 0, -1, -1);
        mHandles[124] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier2", 0, "enemy_vcarrier2", 0, -1, -1);
        mHandles[127] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier1", 0, "dropped_walker1", 2, -1, -1);
        mHandles[128] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "enemy_vcarrier2", 0, "dropped_walker2", 2, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[123], "hp_link_1", mHandles[127], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[124], "hp_link_1", mHandles[128], "hp_link_1", true);
        MissionUtility::SetVelocVertical(mHandles[123], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[123], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[123], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[123], 525.0f);
        MissionUtility::CarrierDropoff(mHandles[123], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[123], "enemy_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocVertical(mHandles[124], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[124], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[124], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[124], 525.0f);
        MissionUtility::CarrierDropoff(mHandles[124], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[124], "enemy_vcarrier2_go", true);
        MissionUtility::SetQueueFlag(false);
        mTimes[26] = 1.0f + MissionUtility::GetTime();
        mFlags[97] = true;
        }
        }
        if (!mFlags[100]) {
        if (mFlags[97]) {
        if (MissionUtility::GetDistance(mHandles[123], "enemy_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[123]);
        mFlags[100] = true;
        }
        }
        }
        if (!mFlags[101]) {
        if (mFlags[97]) {
        if (MissionUtility::GetDistance(mHandles[124], "enemy_vcarrier2_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[124]);
        mFlags[101] = true;
        }
        }
        }
        if (!mFlags[104]) {
        if (mTimes[26] < MissionUtility::GetTime()) {
        mHandles[129] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "powerup_drop1_path", 0, "powerup_dropper1", 1, -1, -1);
        MissionUtility::SetVelocMinimumFly(mHandles[129], 70.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[129], 70.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[129], 70.0f);
        MissionUtility::SetMaxHealth(mHandles[129], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[129], 999999.0f);
        MissionUtility::Goto(mHandles[129], "powerup_drop1_path", true);
        mFlags[104] = true;
        }
        }
        if (!mFlags[105]) {
        if (mFlags[104]) {
        if (MissionUtility::GetDistance(mHandles[129], "powerup_drop1_path", 1) < 75.0f) {
        MissionUtility::DropAmmoBox(mHandles[129]);
        MissionUtility::QueueSound("CTT22_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[105] = true;
        }
        }
        }
        if (!mFlags[106]) {
        if (mFlags[104]) {
        if (MissionUtility::GetDistance(mHandles[129], "powerup_drop1_path", 2) < 75.0f) {
        MissionUtility::DropHealthBox(mHandles[129]);
        mFlags[106] = true;
        }
        }
        }
        if (!mFlags[107]) {
        if (mFlags[104]) {
        if (MissionUtility::GetDistance(mHandles[129], "powerup_drop1_path", 3) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[129]);
        mFlags[107] = true;
        }
        }
        }
        if (!mFlags[28]) {
        if (mTimes[27] < MissionUtility::GetTime()) {
        mHandles[11] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "powerup_drop2_path", 0, "powerup_dropper2", 1, -1, -1);
        MissionUtility::SetMaxHealth(mHandles[11], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[11], 999999.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[11], 70.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[11], 70.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[11], 70.0f);
        MissionUtility::Goto(mHandles[11], "powerup_drop2_path", true);
        mFlags[28] = true;
        }
        }
        if (!mFlags[29]) {
        if (mFlags[28]) {
        if (MissionUtility::GetDistance(mHandles[11], "powerup_drop2_path", 2) < 75.0f) {
        MissionUtility::DropHealthBox(mHandles[11]);
        MissionUtility::QueueSound("CTT22_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[29] = 2.0f + MissionUtility::GetTime();
        mFlags[29] = true;
        }
        }
        }
        if (!mFlags[0]) {
        if (mTimes[29] < MissionUtility::GetTime()) {
        MissionUtility::DropAmmoBox(mHandles[11]);
        mFlags[0] = true;
        }
        }
        if (!mFlags[30]) {
        if (mFlags[28]) {
        if (MissionUtility::GetDistance(mHandles[11], "powerup_drop2_path", 4) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[11]);
        mFlags[30] = true;
        }
        }
        }
        if (!mFlags[98]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger8")) {
        MissionUtility::BeginWave("small_walker_wave1");
        mHandles[30] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "stationary_walker1", 0, "stationary_walker1", 2, -1, -1);
        mHandles[31] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "stationary_walker2", 0, "stationary_walker2", 2, -1, -1);
        MissionUtility::Stop(mHandles[30]);
        MissionUtility::Stop(mHandles[31]);
        mHandles[138] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "aat5", 0, "aat5", 2, -1, -1);
        mHandles[139] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "aat6", 0, "aat6", 2, -1, -1);
        MissionUtility::Stop(mHandles[138]);
        MissionUtility::Stop(mHandles[139]);
        mHandles[24] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "rep_vcarrier_spawn1", 0, "rep_vcarrier1", 0, -1, -1);
        mHandles[29] = MissionUtility::CreateObject("rep_tank_fighter1", "rep_vcarrier_spawn1", 0, "sixleg1", 1, -1);
        MissionUtility::CarrierAddCargo(mHandles[24], "hp_link_1", mHandles[29], "hp_link_1", true);
        MissionUtility::SetCurHealth(mHandles[29], 250.0f);
        MissionUtility::SetVelocVertical(mHandles[24], 30.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[24], 30.0f);
        MissionUtility::SetMaxAltitude(mHandles[24], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[24], 400.0f);
        MissionUtility::CarrierDropoff(mHandles[24], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[24], "rep_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[98] = true;
        }
        }
        if (!mFlags[5]) {
        if (mFlags[98]) {
        if (MissionUtility::GetDistance(mHandles[24], "rep_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[24]);
        mFlags[5] = true;
        }
        }
        }
        if (!mFlags[21]) {
        if (MissionUtility::IsWaveSpawned("small_walker_wave1")) {
        mHandles[13] = MissionUtility::GetHandle("window_walker1");
        MissionUtility::SetCurHealth(mHandles[13], 200.0f);
        mFlags[21] = true;
        }
        }
        if (!mFlags[19]) {
        if (MissionUtility::IsWaveSpawned("small_walker_wave1")) {
        mHandles[55] = MissionUtility::GetHandle("wheeled1");
        mHandles[56] = MissionUtility::GetHandle("wheeled2");
        mHandles[61] = MissionUtility::GetHandle("wheeled7");
        mHandles[62] = MissionUtility::GetHandle("wheeled8");
        mFlags[19] = true;
        }
        }
        if (!mFlags[99]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger9")) {
        MissionUtility::BeginWave("trigger9");
        mHandles[32] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "stationary_walker5", 0, "stationary_walker5", 2, -1, -1);
        mHandles[33] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "stationary_walker6", 0, "stationary_walker6", 2, -1, -1);
        MissionUtility::Stop(mHandles[32]);
        MissionUtility::Stop(mHandles[33]);
        mHandles[140] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "aat7", 0, "aat7", 2, -1, -1);
        mHandles[141] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "aat8", 0, "aat8", 2, -1, -1);
        mHandles[142] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "aat9", 0, "aat9", 2, -1, -1);
        mHandles[143] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "aat10", 0, "aat10", 2, -1, -1);
        MissionUtility::Stop(mHandles[140]);
        MissionUtility::Stop(mHandles[141]);
        MissionUtility::Stop(mHandles[142]);
        MissionUtility::Stop(mHandles[143]);
        mHandles[25] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "rep_vcarrier_spawn2", 0, "rep_vcarrier2", 0, -1, -1);
        mHandles[27] = MissionUtility::CreateObject("rep_tank_fighter1", "rep_vcarrier_spawn2", 0, "sixleg2", 1, -1);
        MissionUtility::CarrierAddCargo(mHandles[25], "hp_link_1", mHandles[27], "hp_link_1", true);
        MissionUtility::SetCurHealth(mHandles[27], 250.0f);
        MissionUtility::SetVelocVertical(mHandles[25], 30.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[25], 30.0f);
        MissionUtility::SetMaxAltitude(mHandles[25], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[25], 400.0f);
        MissionUtility::CarrierDropoff(mHandles[25], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[25], "rep_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mHandles[26] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "rep_vcarrier_spawn3", 0, "rep_vcarrier3", 0, -1, -1);
        mHandles[28] = MissionUtility::CreateObject("rep_tank_fighter1", "rep_vcarrier_spawn3", 0, "sixleg3", 1, -1);
        MissionUtility::CarrierAddCargo(mHandles[26], "hp_link_1", mHandles[28], "hp_link_1", true);
        MissionUtility::SetCurHealth(mHandles[28], 250.0f);
        MissionUtility::SetVelocVertical(mHandles[26], 30.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[26], 30.0f);
        MissionUtility::SetMaxAltitude(mHandles[26], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[26], 400.0f);
        MissionUtility::CarrierDropoff(mHandles[26], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[26], "rep_vcarrier1_go", true);
        MissionUtility::SetQueueFlag(false);
        mHandles[146] = MissionUtility::CreateRegionList("deleter_region1", true, false);
        mFlags[99] = true;
        }
        }
        if (!mFlags[12]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "deleter_region4")) {
        mHandles[147] = MissionUtility::CreateRegionList("deleter_region2", true, false);
        mFlags[12] = true;
        }
        }
        if (!mFlags[8]) {
        if (mFlags[99]) {
        if (MissionUtility::GetDistance(mHandles[25], "rep_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[25]);
        mFlags[8] = true;
        }
        }
        }
        if (!mFlags[11]) {
        if (mFlags[99]) {
        if (MissionUtility::GetDistance(mHandles[26], "rep_vcarrier1_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[26]);
        mFlags[11] = true;
        }
        }
        }
        if (!mFlags[22]) {
        if (MissionUtility::IsWaveSpawned("trigger9")) {
        MissionUtility::Stop(mHandles[17]);
        MissionUtility::Stop(mHandles[18]);
        mFlags[22] = true;
        }
        }
        if (!mFlags[108]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger10")) {
        mHandles[125] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier3", 0, "enemy_vcarrier3", 0, -1, -1);
        mHandles[126] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "enemy_vcarrier4", 0, "enemy_vcarrier4", 0, -1, -1);
        mHandles[130] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_vcarrier3", 0, "dropped_gat1", 2, -1, -1);
        mHandles[131] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_vcarrier3", 0, "dropped_gat2", 2, -1, -1);
        mHandles[134] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_vcarrier4", 0, "dropped_gat5", 2, -1, -1);
        mHandles[135] = MissionUtility::CreateObjectWithRotation("cis_tank_fighter", "enemy_vcarrier4", 0, "dropped_gat6", 2, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[125], "hp_link_17", mHandles[130], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[125], "hp_link_18", mHandles[131], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[126], "hp_link_17", mHandles[134], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[126], "hp_link_18", mHandles[135], "hp_link_1", true);
        MissionUtility::SetVelocVertical(mHandles[125], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[125], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[125], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[125], 525.0f);
        MissionUtility::CarrierDropoff(mHandles[125], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[125], "enemy_vcarrier3_go", true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocVertical(mHandles[126], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[126], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[126], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[126], 525.0f);
        MissionUtility::CarrierDropoff(mHandles[126], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[126], "enemy_vcarrier4_go", true);
        MissionUtility::SetQueueFlag(false);
        mTimes[27] = 0.1f + MissionUtility::GetTime();
        mFlags[108] = true;
        }
        }
        if (!mFlags[10]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "enemy_trigger10b")) {
        MissionUtility::BeginWave("trigger10");
        mFlags[10] = true;
        }
        }
        if (!mFlags[102]) {
        if (mFlags[108]) {
        if (MissionUtility::GetDistance(mHandles[125], "enemy_vcarrier3_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[125]);
        mFlags[102] = true;
        }
        }
        }
        if (!mFlags[103]) {
        if (mFlags[108]) {
        if (MissionUtility::GetDistance(mHandles[126], "enemy_vcarrier4_go") < 150.0f) {
        MissionUtility::RemoveObject(mHandles[126]);
        mFlags[103] = true;
        }
        }
        }
        if (!mFlags[109]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "turnaround1")) {
        MissionUtility::BeginWave("cydon_wave");
        mHandles[148] = MissionUtility::CreateRegionList("deleter_region3", true, false);
        mFlags[109] = true;
        }
        }
        if (!mFlags[33]) {
        if (MissionUtility::GetDistance(mHandles[0], mHandles[16]) < 400.0f) {
        MissionUtility::DisplayText("missions.Thule3.text.str0012", 6.0f, -1.0f);
        if (MissionUtility::IsAlive(mHandles[8])
            || MissionUtility::IsAlive(mHandles[9])) {
        MissionUtility::StartSound("CTT23_02", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        mFlags[33] = true;
        }
        }
        if (!mFlags[37]) {
        if (!MissionUtility::IsAlive(mHandles[16])) {
        if (!MissionUtility::IsAlive(mHandles[19])) {
        if (!MissionUtility::IsAlive(mHandles[37])) {
        if (!MissionUtility::IsAlive(mHandles[38])) {
        mTimes[0] = 3.0f + MissionUtility::GetTime();
        mFlags[37] = true;
        }
        }
        }
        }
        }
        if (!mFlags[35]) {
        if (mTimes[0] < MissionUtility::GetTime()) {
        mFlags[35] = true;
        }
        }
        if (!mFlags[39]) {
        if (MissionUtility::GetDistance(mHandles[0], "marker1") < 150.0f) {
        MissionUtility::StartSound("tone", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObjectify("marker1", 0);
        MissionUtility::Objectify("marker2", 0, "missions.Thule3.marker.str0007", true, false, 0.0f, 2.0f);
        mFlags[39] = true;
        }
        }
        if (!mFlags[44]) {
        if (MissionUtility::GetDistance(mHandles[0], "marker2") < 150.0f) {
        MissionUtility::StartSound("tone", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObjectify("marker2", 0);
        MissionUtility::Objectify("marker3", 0, "missions.Thule3.marker.str0008", true, false, 0.0f, 2.0f);
        mFlags[44] = true;
        }
        }
        if (!mFlags[46]) {
        if (MissionUtility::GetDistance(mHandles[0], "marker3") < 150.0f) {
        MissionUtility::StartSound("tone", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObjectify("marker3", 0);
        MissionUtility::Objectify("marker4", 0, "missions.Thule3.marker.str0009", true, false, 0.0f, 2.0f);
        mFlags[46] = true;
        }
        }
        if (!mFlags[48]) {
        if (MissionUtility::GetDistance(mHandles[0], "marker4") < 150.0f) {
        MissionUtility::StartSound("tone", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObjectify("marker4", 0);
        mFlags[48] = true;
        }
        }
        if (!mFlags[23]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "temple")) {
        MissionUtility::MidMissionSavePlayer(1);
        MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[8]));
        MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[9]));
        MissionUtility::MidMissionSave((bool)mFlags[74]);
        MissionUtility::MidMissionSave((bool)mFlags[75]);
        MissionUtility::MidMissionSave((bool)mFlags[76]);
        MissionUtility::MidMissionSave((bool)mFlags[77]);
        MissionUtility::MidMissionSave((bool)mFlags[78]);
        MissionUtility::MidMissionSave((bool)mFlags[79]);
        MissionUtility::ObjectiveComplete(mInts[1]);
        MissionUtility::RemoveObject(mHandles[68]);
        MissionUtility::RemoveObject(mHandles[69]);
        MissionUtility::RemoveObject(mHandles[70]);
        MissionUtility::RemoveObject(mHandles[71]);
        MissionUtility::RemoveObject(mHandles[72]);
        MissionUtility::RemoveArmy(mHandles[73]);
        MissionUtility::RemoveArmy(mHandles[74]);
        MissionUtility::RemoveArmy(mHandles[75]);
        MissionUtility::RemoveArmy(mHandles[76]);
        MissionUtility::RemoveArmy(mHandles[77]);
        MissionUtility::RemoveArmy(mHandles[78]);
        MissionUtility::RemoveArmy(mHandles[79]);
        MissionUtility::RemoveArmy(mHandles[80]);
        MissionUtility::RemoveArmy(mHandles[81]);
        MissionUtility::RemoveArmy(mHandles[82]);
        MissionUtility::RemoveArmy(mHandles[85]);
        MissionUtility::RemoveArmy(mHandles[86]);
        MissionUtility::RemoveArmy(mHandles[89]);
        MissionUtility::RemoveArmy(mHandles[90]);
        MissionUtility::RemoveArmy(mHandles[91]);
        MissionUtility::RemoveArmy(mHandles[92]);
        MissionUtility::RemoveArmy(mHandles[93]);
        MissionUtility::RemoveArmy(mHandles[94]);
        MissionUtility::RemoveArmy(mHandles[95]);
        MissionUtility::RemoveArmy(mHandles[96]);
        MissionUtility::RemoveArmy(mHandles[97]);
        MissionUtility::RemoveArmy(mHandles[98]);
        MissionUtility::RemoveArmy(mHandles[99]);
        MissionUtility::RemoveArmy(mHandles[100]);
        MissionUtility::RemoveArmy(mHandles[101]);
        MissionUtility::RemoveArmy(mHandles[102]);
        MissionUtility::RemoveArmy(mHandles[103]);
        MissionUtility::RemoveArmy(mHandles[104]);
        MissionUtility::EvictConfig("rep_fly_assault_far");
        mFlags[82] = true;
        mHandles[149] = MissionUtility::CreateRegionList("deleter_region4", true, false);
        mFlags[23] = true;
        }
        }
    }

    // ---- +0x316c  3044 bytes ----
    if (!mFlags[83]) {
        if (!mFlags[88]) {
        if (!mFlags[89]) {
        if (mFlags[23]) {
        MissionUtility::RemoveObjectify("marker1", 0);
        MissionUtility::RemoveObjectify("marker2", 0);
        MissionUtility::RemoveObjectify("marker3", 0);
        MissionUtility::RemoveObjectify("marker4", 0);
        MissionUtility::MoveObjectWithRotation(mHandles[0], "CydonCinPlayerMovePath", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[8], "CydonCinPlayerMovePath", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[9], "CydonCinPlayerMovePath", 0, true);
        mHandles[175] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "CydonCinPlayerPath", 0, "CydonCinPlayer", 0, -1, -1);
        MissionUtility::SetVelocForward(mHandles[175], 150.0f);
        mHandles[173] = MissionUtility::CreateObjectWithRotation("cis_inf_dooku", "CydonCinDookuPath", 0, "CydonCinDooku", 0, -1, -1);
        mHandles[174] = MissionUtility::CreateObjectWithRotation("cis_inf_cydon", "CydonCinCydonPath", 0, "CydonCinCydon", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[175], true);
        MissionUtility::OverrideSoundRange(mHandles[173], true);
        MissionUtility::OverrideSoundRange(mHandles[174], true);
        MissionUtility::OverrideSoundRange(mHandles[10], true);
        MissionUtility::SetVelocForward(mHandles[174], 0.0f);
        MissionUtility::SetVelocForward(mHandles[173], 3.0f);
        MissionUtility::Goto(mHandles[175], "CydonCinPlayerPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[175]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::PlayMusic("EP6_V2_T04_02", true);
        MissionUtility::AddTurnAroundRegion("turnaround1", "turnaround1_point2", 0, 0, 0, 0);
        MissionUtility::AddTurnAroundRegion("turnaround2", "turnaround2_point1", 0, 0, 0, 0);
        MissionUtility::SetAttackRange(mHandles[10], 1);
        mHandles[63] = MissionUtility::RunCin("cydon_cin", true, true);
        mFlags[89] = true;
        BeginTimer(mTimer4);
        }
        }
        if (!mFlags[122]) {
        if (mTimer4 > 5.5f) {
        if (!mFlags[110]) {
        MissionUtility::QueueSound("CDT23_04", 1.0f, 0.0f, 0.0f, "", mHandles[173], "talk01");
        mFlags[110] = true;
        }
        }
        }
        if (!mFlags[122]) {
        if (mTimer4 > 10.0f) {
        if (!mFlags[111]) {
        MissionUtility::Goto(mHandles[173], mHandles[174]);
        mFlags[111] = true;
        }
        }
        }
        if (!mFlags[122]) {
        if (mTimer4 > 11.0f) {
        if (!mFlags[112]) {
        MissionUtility::QueueSound("CDT23_05", 1.0f, 0.0f, 0.0f, "", mHandles[173], "talk01");
        MissionUtility::SetVelocForward(mHandles[174], 0.0f);
        MissionUtility::Goto(mHandles[174], mHandles[173]);
        mFlags[112] = true;
        }
        }
        }
        if (!mFlags[122]) {
        if (mTimer4 > 12.0f) {
        if (!mFlags[113]) {
        MissionUtility::Goto(mHandles[173], "CydonCinDookuPath", false);
        MissionUtility::Goto(mHandles[174], mHandles[0]);
        mFlags[113] = true;
        }
        }
        }
        if (!mFlags[122]) {
        if (mTimer4 > 13.0f) {
        if (!mFlags[114]) {
        MissionUtility::QueueSound("CXT23_06", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[114] = true;
        }
        }
        }
        if (!mFlags[122]) {
        if (mTimer4 > 14.0f) {
        if (!mFlags[115]) {
        MissionUtility::SetVelocForward(mHandles[174], 10.0f);
        MissionUtility::Goto(mHandles[174], "CydonCinCydonPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[174]);
        MissionUtility::SetQueueFlag(false);
        mFlags[115] = true;
        }
        }
        }
        if (!mFlags[122]) {
        if (mTimer4 > 16.0f) {
        if (!mFlags[116]) {
        MissionUtility::SetVelocForward(mHandles[173], 10.0f);
        MissionUtility::StartSound("generic_powerup_med01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Goto(mHandles[10], mHandles[175]);
        MissionUtility::RemoveObject(mHandles[174]);
        mFlags[116] = true;
        }
        }
        }
        if (mFlags[89]) {
        if (!MissionUtility::IsCinRunning(mHandles[63])) {
        MissionUtility::OverrideSoundRange(mHandles[10], false);
        MissionUtility::FlushSoundQueue();
        mFlags[122] = true;
        MissionUtility::MoveObjectWithRotation(mHandles[0], "CydonCinPlayerMovePath", 1, true);
        MissionUtility::RemoveObject(mHandles[174]);
        MissionUtility::RemoveObject(mHandles[173]);
        MissionUtility::RemoveObject(mHandles[175]);
        MissionUtility::MoveObjectWithRotation(mHandles[10], "CydonStartPath", 0, true);
        MissionUtility::PlayMusic("EP1_V2_T22", true);
        mFlags[88] = true;
        }
        }
        }
        if (!mFlags[85]) {
        if (mFlags[88]) {
        MissionUtility::DisplayText("missions.Thule3.text.str0013", 6.0f, -1.0f);
        mInts[2] = MissionUtility::AddObjective("missions.Thule3.objective.str0004");
        mTimes[3] = 20.0f + MissionUtility::GetTime();
        MissionUtility::AddScreenTopHealthBar(mHandles[10], "Cydon Prax");
        MissionUtility::AttackTarget(mHandles[10], mHandles[0], true, true, false, false);
        MissionUtility::SetAttackRange(mHandles[10], 2000);
        MissionUtility::SetTeamNum(mHandles[10], 2);
        mFlags[85] = true;
        }
        }
        if (mFlags[85]) {
        if (MissionUtility::IsAlive(mHandles[10])) {
        if (!mFlags[54]) {
        if (mTimes[3] < MissionUtility::GetTime()) {
        mTimes[7] = 3.5f + MissionUtility::GetTime();
        MissionUtility::Stop(mHandles[10]);
        MissionUtility::StartSound("B_CyTank_SonicAttack04", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetAttackRange(mHandles[10], 1);
        mFlags[54] = true;
        }
        }
        if (!mFlags[58]) {
        if (mTimes[7] < MissionUtility::GetTime()) {
        mPos = MissionUtility::GetPosition(mHandles[10]);
        MissionUtility::CreateObject("cis_cydon_special_xpl", mPos, "", 2, -1);
        MissionUtility::StartSound("explosion", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetAttackRange(mHandles[10], 1000);
        MissionUtility::AttackTarget(mHandles[10], mHandles[0], true, true, false, false);
        mTimes[4] = 20.0f + MissionUtility::GetTime();
        mFlags[58] = true;
        }
        }
        if (!mFlags[55]) {
        if (mTimes[4] < MissionUtility::GetTime()) {
        mTimes[8] = 3.5f + MissionUtility::GetTime();
        MissionUtility::Stop(mHandles[10]);
        MissionUtility::StartSound("B_CyTank_SonicAttack04", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetAttackRange(mHandles[10], 1);
        mFlags[55] = true;
        }
        }
        if (!mFlags[59]) {
        if (mTimes[8] < MissionUtility::GetTime()) {
        mPos = MissionUtility::GetPosition(mHandles[10]);
        MissionUtility::CreateObject("cis_cydon_special_xpl", mPos, "", 2, -1);
        MissionUtility::StartSound("explosion", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetAttackRange(mHandles[10], 1000);
        MissionUtility::AttackTarget(mHandles[10], mHandles[0], true, true, false, false);
        mTimes[5] = 20.0f + MissionUtility::GetTime();
        mFlags[59] = true;
        }
        }
        if (!mFlags[56]) {
        if (mTimes[5] < MissionUtility::GetTime()) {
        mTimes[9] = 3.5f + MissionUtility::GetTime();
        MissionUtility::Stop(mHandles[10]);
        MissionUtility::StartSound("B_CyTank_SonicAttack04", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetAttackRange(mHandles[10], 1);
        mFlags[56] = true;
        }
        }
        if (!mFlags[60]) {
        if (mTimes[9] < MissionUtility::GetTime()) {
        mPos = MissionUtility::GetPosition(mHandles[10]);
        MissionUtility::CreateObject("cis_cydon_special_xpl", mPos, "", 2, -1);
        MissionUtility::StartSound("explosion", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetAttackRange(mHandles[10], 1000);
        MissionUtility::AttackTarget(mHandles[10], mHandles[0], true, true, false, false);
        mTimes[6] = 5.0f + MissionUtility::GetTime();
        mFlags[60] = true;
        }
        }
        if (!mFlags[57]) {
        if (mTimes[6] < MissionUtility::GetTime()) {
        mTimes[10] = 3.5f + MissionUtility::GetTime();
        MissionUtility::Stop(mHandles[10]);
        MissionUtility::StartSound("B_CyTank_SonicAttack04", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetAttackRange(mHandles[10], 1);
        mFlags[57] = true;
        }
        }
        if (!mFlags[61]) {
        if (mTimes[10] < MissionUtility::GetTime()) {
        mPos = MissionUtility::GetPosition(mHandles[10]);
        MissionUtility::CreateObject("cis_cydon_special_xpl", mPos, "", 2, -1);
        MissionUtility::SetAttackRange(mHandles[10], 1000);
        MissionUtility::AttackTarget(mHandles[10], mHandles[0], true, true, false, false);
        mTimes[14] = 0.5f + MissionUtility::GetTime();
        mFlags[61] = true;
        }
        }
        if (!mFlags[65] & (mTimes[14] < MissionUtility::GetTime())) {
        mTimes[3] = 20.0f + MissionUtility::GetTime();
        mTimes[4] = 999999.9f;
        mTimes[5] = 999999.9f;
        mTimes[6] = 999999.9f;
        mTimes[7] = 999999.9f;
        mTimes[8] = 999999.9f;
        mTimes[9] = 999999.9f;
        mTimes[10] = 999999.9f;
        mTimes[14] = 999999.9f;
        mFlags[54] = false;
        mFlags[55] = false;
        mFlags[56] = false;
        mFlags[57] = false;
        mFlags[58] = false;
        mFlags[59] = false;
        mFlags[60] = false;
        mFlags[61] = false;
        mFlags[62] = false;
        mFlags[63] = false;
        mFlags[64] = false;
        }
        }
        }
        if (!mFlags[27]) {
        if (mFlags[23]) {
        if (!MissionUtility::IsAlive(mHandles[10])) {
        MissionUtility::ObjectiveComplete(mInts[2]);
        mFlags[83] = true;
        MissionUtility::MidMissionSavePlayer(2);
        MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[8]));
        MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[9]));
        MissionUtility::MidMissionSave((bool)mFlags[74]);
        MissionUtility::MidMissionSave((bool)mFlags[75]);
        MissionUtility::MidMissionSave((bool)mFlags[76]);
        MissionUtility::MidMissionSave((bool)mFlags[77]);
        MissionUtility::MidMissionSave((bool)mFlags[78]);
        MissionUtility::MidMissionSave((bool)mFlags[79]);
        mTimes[17] = 3.0f + MissionUtility::GetTime();
        mFlags[27] = true;
        }
        }
        }
    }

    // ---- +0x3d50  36 bytes ----
    if (!mFlags[81]) {
        if (MissionUtility::IsWaveSpawned("major_wave")) {
        mFlags[81] = true;
        }
    }

    // ---- +0x3d74  8044 bytes ----
    if (!mFlags[84]) {
        if (!mFlags[91]) {
        if (mTimes[17] < MissionUtility::GetTime()) {
        if (!mFlags[90]) {
        MissionUtility::RemoveObjectify("marker1", 0);
        MissionUtility::RemoveObjectify("marker2", 0);
        MissionUtility::RemoveObjectify("marker3", 0);
        MissionUtility::RemoveObjectify("marker4", 0);
        MissionUtility::DisableKillCount();
        mHandles[115] = MissionUtility::CreateObjectSnap("cis_boss_reaper_shield", mHandles[12], "hp_shield", "reaper_shield", 2, -1);
        mHandles[105] = MissionUtility::CreateObjectSnap("cis_boss_reaper_power", mHandles[12], "hp_power_1", "reaper_power1", 2, -1);
        mHandles[106] = MissionUtility::CreateObjectSnap("cis_boss_reaper_power", mHandles[12], "hp_power_2", "reaper_power2", 2, -1);
        mHandles[107] = MissionUtility::CreateObjectSnap("cis_boss_reaper_power", mHandles[12], "hp_power_3", "reaper_power3", 2, -1);
        mHandles[108] = MissionUtility::CreateObjectSnap("cis_boss_reaper_power", mHandles[12], "hp_power_4", "reaper_power4", 2, -1);
        mHandles[109] = MissionUtility::CreateObjectSnap("cis_boss_reaper_turret", mHandles[12], "hp_turret_1", "reaper_turret1", 0, -1);
        mHandles[110] = MissionUtility::CreateObjectSnap("cis_boss_reaper_turret", mHandles[12], "hp_turret_2", "reaper_turret2", 0, -1);
        mHandles[111] = MissionUtility::CreateObjectSnap("cis_boss_reaper_turret", mHandles[12], "hp_turret_3", "reaper_turret3", 0, -1);
        mHandles[112] = MissionUtility::CreateObjectSnap("cis_boss_reaper_turret", mHandles[12], "hp_turret_4", "reaper_turret4", 0, -1);
        mHandles[113] = MissionUtility::CreateObjectSnap("cis_boss_reaper_launchport", mHandles[12], "hp_launchport_1", "reaper_launchport1", 2, -1);
        mHandles[114] = MissionUtility::CreateObjectSnap("cis_boss_reaper_launchport", mHandles[12], "hp_launchport_2", "reaper_launchport2", 2, -1);
        MissionUtility::SetFogRange(1400.0f, 1600.0f, 2.0f);
        mHandles[3] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread_reaper", "harvester_fx1", "", 0, -1, 0);
        mHandles[4] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread_reaper", "harvester_fx2", "", 0, -1, 0);
        mHandles[5] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread_reaper", "harvester_fx3", "", 0, -1, 0);
        mHandles[6] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread_reaper", "harvester_fx4", "", 0, -1, 0);
        mHandles[7] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread_reaper", "harvester_fx5", "", 0, -1, 0);
        MissionUtility::SetImportantFlag(mHandles[3], true);
        MissionUtility::SetImportantFlag(mHandles[4], true);
        MissionUtility::SetImportantFlag(mHandles[5], true);
        MissionUtility::SetImportantFlag(mHandles[6], true);
        MissionUtility::SetImportantFlag(mHandles[7], true);
        MissionUtility::PlayMusic("EP2_V1_T10", true);
        mHandles[176] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "ReaperCinPlayerPath", 0, "ReaperCinPlayer", 0, -1, -1);
        MissionUtility::MoveObject(mHandles[0], "ReaperCinPlayerMovePath", 0, true);
        MissionUtility::OverrideSoundRange(mHandles[176], true);
        MissionUtility::OverrideSoundRange(mHandles[64], true);
        MissionUtility::OverrideSoundRange(mHandles[12], true);
        MissionUtility::SetVelocForward(mHandles[176], 150.0f);
        MissionUtility::Goto(mHandles[176], "ReaperCinPlayerPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[176]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetApplyDynamics(mHandles[109], true);
        MissionUtility::SetAnimation(mHandles[109], "fullanimation", 1.0f, 1);
        MissionUtility::SetApplyDynamics(mHandles[113], true);
        MissionUtility::SetAnimation(mHandles[113], "fullanimation", 1.0f, 1);
        mHandles[65] = MissionUtility::RunCin("reaper_cin", true, true);
        mTimes[30] = 3.0f + MissionUtility::GetTime();
        mTimes[16] = 6.0f + MissionUtility::GetTime();
        mFlags[90] = true;
        BeginTimer(mTimer5);
        }
        if (mTimes[30] < MissionUtility::GetTime()) {
        MissionUtility::StartSound("B_Reaper_startup08", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[30] = 999999.9f + MissionUtility::GetTime();
        }
        if (!mFlags[123]) {
        if (mTimer5 > 1.0f) {
        if (!mFlags[117]) {
        mFlags[117] = true;
        MissionUtility::QueueSound("OBT23_01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("UDT23_07", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("ANT23_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("OBT23_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        }
        }
        if (!mFlags[124]) {
        if (mFlags[90]) {
        if (MissionUtility::GetCinId(mHandles[65]) == 3) {
        MissionUtility::StartSound("c_infcar_dooropen01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        BeginTimer(mTimer6);
        MissionUtility::SetAnimation(mHandles[109], "fullanimation", -1.0f, 1);
        mFlags[124] = true;
        }
        }
        }
        if (mTimer6 > 0.5f) {
        MissionUtility::StartSound("c_aatank_turret02", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        StopTimer(mTimer6);
        mTimer6 = 0.0f;
        }
        if (!mFlags[125]) {
        if (mFlags[90]) {
        if (MissionUtility::GetCinId(mHandles[65]) == 5) {
        MissionUtility::StartSound("c_aatank_turret02", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetAnimation(mHandles[113], "fullanimation", -1.0f, 1);
        mFlags[125] = true;
        }
        }
        }
        if (!mFlags[123]) {
        if (mTimer5 > 25.0f) {
        if (!mFlags[118]) {
        mFlags[118] = true;
        MissionUtility::Goto(mHandles[176], "ReaperCinPlayerPath1", false);
        }
        }
        }
        if (!mFlags[126]) {
        if (!mFlags[123]) {
        if (mTimes[16] < MissionUtility::GetTime()) {
        MissionUtility::ShakeCamera(20.0f, 1.0f, 0.05f);
        MissionUtility::SetApplyDynamics(mHandles[64], true);
        MissionUtility::SetAnimation(mHandles[64], "fullanimation", 1.0f, 1);
        MissionUtility::SetAltitude(mHandles[12], 160.0f);
        mHandles[36] = MissionUtility::StartSound("B_Reaper_wallsMove_lp01", true, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetOmega(mHandles[12], Vector(0.0f, 0.03f, 0.0f));
        MissionUtility::SetImportantFlag(mHandles[12], true);
        MissionUtility::SetLightningSource(mHandles[12]);
        MissionUtility::SetLightningCount(4);
        MissionUtility::SetLightningDelay(0.25f, 1.0f);
        mFlags[1] = true;
        mFlags[126] = true;
        mTimes[16] = 999999.9f;
        }
        }
        }
        if (!mFlags[123]) {
        if (MissionUtility::GetCinId(mHandles[65]) == 2) {
        if (!mFlags[119]) {
        MissionUtility::MoveObjectWithRotation(mHandles[176], "ReaperCinPlayerPath1", 0, true);
        mFlags[119] = true;
        }
        }
        }
        if (mFlags[90]) {
        if (!MissionUtility::IsCinRunning(mHandles[65])) {
        MissionUtility::OverrideSoundRange(mHandles[64], false);
        MissionUtility::OverrideSoundRange(mHandles[12], false);
        MissionUtility::FlushSoundQueue();
        mFlags[123] = true;
        MissionUtility::RemoveObject(mHandles[176]);
        MissionUtility::MoveObjectWithRotation(mHandles[0], "loadpoint2_player", 0, true);
        MissionUtility::StopSound(mHandles[36]);
        MissionUtility::RemoveTurnAroundRegion("turnaround2");
        MissionUtility::PlayMusic("EP1_V2_T26", true);
        MissionUtility::SetApplyDynamics(mHandles[64], true);
        MissionUtility::SetAnimation(mHandles[64], "fullanimation", -1.0f, 0);
        MissionUtility::SetOmega(mHandles[12], Vector(0.0f, 0.03f, 0.0f));
        MissionUtility::SetImportantFlag(mHandles[12], true);
        MissionUtility::SetLightningSource(mHandles[12]);
        MissionUtility::SetLightningCount(4);
        MissionUtility::SetLightningDelay(0.25f, 1.0f);
        MissionUtility::SetCurHealth(mHandles[0], 700.0f);
        mFlags[91] = true;
        }
        }
        }
        }
        if (!mFlags[31]) {
        if (mFlags[91]) {
        mInts[3] = MissionUtility::AddObjective("missions.Thule3.objective.str0005");
        MissionUtility::DisplayText("missions.Thule3.text.str0014", 6.0f, -1.0f);
        MissionUtility::SetMaxHealth(mHandles[12], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[12], 999999.0f);
        MissionUtility::SetOnRadar(mHandles[109], false);
        MissionUtility::SetOnRadar(mHandles[110], false);
        MissionUtility::SetOnRadar(mHandles[111], false);
        MissionUtility::SetOnRadar(mHandles[112], false);
        MissionUtility::SetOnRadar(mHandles[105], false);
        MissionUtility::SetOnRadar(mHandles[106], false);
        MissionUtility::SetOnRadar(mHandles[107], false);
        MissionUtility::SetOnRadar(mHandles[108], false);
        MissionUtility::SetOnRadar(mHandles[113], false);
        MissionUtility::SetOnRadar(mHandles[114], false);
        MissionUtility::SetOnRadar(mHandles[12], false);
        MissionUtility::SetTeamNum(mHandles[115], 5);
        MissionUtility::SetTeamNum(mHandles[12], 4);
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], false, false, false, false);
        MissionUtility::SetMaxHealth(mHandles[105], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[105], 1500.0f);
        MissionUtility::SetMaxHealth(mHandles[106], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[106], 1500.0f);
        MissionUtility::SetMaxHealth(mHandles[107], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[107], 1500.0f);
        MissionUtility::SetMaxHealth(mHandles[108], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[108], 1500.0f);
        MissionUtility::SetMaxHealth(mHandles[109], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[109], 1500.0f);
        MissionUtility::SetMaxHealth(mHandles[110], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[110], 1500.0f);
        MissionUtility::SetMaxHealth(mHandles[111], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[111], 1500.0f);
        MissionUtility::SetMaxHealth(mHandles[112], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[112], 1500.0f);
        MissionUtility::SetMaxHealth(mHandles[113], 1000.0f);
        MissionUtility::SetCurHealth(mHandles[113], 1000.0f);
        MissionUtility::SetMaxHealth(mHandles[114], 1000.0f);
        MissionUtility::SetCurHealth(mHandles[114], 1000.0f);
        MissionUtility::SetEnemiesOneWay(3, 1);
        MissionUtility::SetNeutralOneWay(1, 3);
        MissionUtility::SetEnemiesOneWay(4, 1);
        MissionUtility::SetNeutralOneWay(1, 4);
        MissionUtility::SetAllianceOneWay(1, 5);
        MissionUtility::SetNeutralOneWay(5, 1);
        MissionUtility::SetAlliance(2, 5);
        MissionUtility::QueueSound("UDT23_07", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[31] = true;
        }
        }
        if (!mFlags[9]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "reaper_on")) {
        mFlags[9] = true;
        }
        }
        if (!mFlags[3]) {
        if (MissionUtility::IsInsideRegion(mHandles[8], "reaper_on")) {
        MissionUtility::DamageObject(mHandles[8], 999999.0f, 999999.0f);
        mFlags[3] = true;
        }
        }
        if (!mFlags[4]) {
        if (MissionUtility::IsInsideRegion(mHandles[9], "reaper_on")) {
        MissionUtility::DamageObject(mHandles[9], 999999.0f, 999999.0f);
        mFlags[4] = true;
        }
        }
        if (!mFlags[49]) {
        if (mFlags[31]) {
        if (mFlags[9]) {
        if (!mFlags[45]) {
        if (!mFlags[92]) {
        MissionUtility::SetCameraAngle(-0.05f);
        MissionUtility::SetCameraOffset(Vector(0.0f, 9.13f, 0.0f));
        MissionUtility::SetCameraDistance(25.75f);
        MissionUtility::SetTeamNum(mHandles[113], 5);
        MissionUtility::SetTeamNum(mHandles[114], 5);
        MissionUtility::SetTeamNum(mHandles[109], 3);
        MissionUtility::SetTeamNum(mHandles[110], 3);
        MissionUtility::SetTeamNum(mHandles[111], 3);
        MissionUtility::SetTeamNum(mHandles[112], 3);
        mTimes[19] = MissionUtility::GetTime();
        MissionUtility::QueueSound("UDT23_09", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetApplyDynamics(mHandles[109], true);
        MissionUtility::SetApplyDynamics(mHandles[110], true);
        MissionUtility::SetApplyDynamics(mHandles[111], true);
        MissionUtility::SetApplyDynamics(mHandles[112], true);
        MissionUtility::SetApplyDynamics(mHandles[113], true);
        MissionUtility::SetApplyDynamics(mHandles[113], true);
        mFlags[92] = true;
        }
        if (!mFlags[52]) {
        if (mTimes[19] < MissionUtility::GetTime()) {
        MissionUtility::SetAltitude(mHandles[12], 118.0f);
        if (mFlags[53]) {
        MissionUtility::SetAnimation(mHandles[109], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[110], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[111], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[112], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[113], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[114], "fullanimation", -1.0f, 1);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[109], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[110], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[111], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[112], false, 250.0f);
        }
        MissionUtility::Objectify(mHandles[105], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[106], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[107], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[108], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::SetOnRadar(mHandles[105], true);
        MissionUtility::SetOnRadar(mHandles[106], true);
        MissionUtility::SetOnRadar(mHandles[107], true);
        MissionUtility::SetOnRadar(mHandles[108], true);
        MissionUtility::SetTeamNum(mHandles[109], 3);
        MissionUtility::SetTeamNum(mHandles[110], 3);
        MissionUtility::SetTeamNum(mHandles[111], 3);
        MissionUtility::SetTeamNum(mHandles[112], 3);
        MissionUtility::SetTeamNum(mHandles[105], 2);
        MissionUtility::SetTeamNum(mHandles[106], 2);
        MissionUtility::SetTeamNum(mHandles[107], 2);
        MissionUtility::SetTeamNum(mHandles[108], 2);
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], false, false, false, false);
        mFlags[47] = true;
        mTimes[18] = 25.0f + MissionUtility::GetTime();
        mFlags[53] = false;
        mFlags[52] = true;
        }
        }
        if (!mFlags[53]) {
        if (mTimes[18] < MissionUtility::GetTime()) {
        if (!mFlags[26]) {
        mTimes[25] = 5.0f + MissionUtility::GetTime();
        mFlags[26] = true;
        }
        MissionUtility::SetAltitude(mHandles[12], 218.0f);
        MissionUtility::SetAnimation(mHandles[109], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[110], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[111], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[112], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[113], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[114], "fullanimation", 1.0f, 1);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[109], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[110], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[111], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[112], false, 250.0f);
        MissionUtility::SetTeamNum(mHandles[105], 5);
        MissionUtility::SetTeamNum(mHandles[106], 5);
        MissionUtility::SetTeamNum(mHandles[107], 5);
        MissionUtility::SetTeamNum(mHandles[108], 5);
        MissionUtility::SetTeamNum(mHandles[109], 5);
        MissionUtility::SetTeamNum(mHandles[110], 5);
        MissionUtility::SetTeamNum(mHandles[111], 5);
        MissionUtility::SetTeamNum(mHandles[112], 5);
        MissionUtility::RemoveObjectify(mHandles[105]);
        MissionUtility::RemoveObjectify(mHandles[106]);
        MissionUtility::RemoveObjectify(mHandles[107]);
        MissionUtility::RemoveObjectify(mHandles[108]);
        MissionUtility::SetOnRadar(mHandles[105], false);
        MissionUtility::SetOnRadar(mHandles[106], false);
        MissionUtility::SetOnRadar(mHandles[107], false);
        MissionUtility::SetOnRadar(mHandles[108], false);
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], true, false, true, false);
        mFlags[47] = false;
        mTimes[19] = 20.0f + MissionUtility::GetTime();
        mFlags[52] = false;
        mFlags[53] = true;
        }
        }
        }
        if (mTimes[25] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("UDT23_11", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("UDT23_15", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[25] = 999999.9f + MissionUtility::GetTime();
        }
        if (!mFlags[45]) {
        if (mFlags[92]) {
        if (!MissionUtility::IsAlive(mHandles[105])) {
        if (!MissionUtility::IsAlive(mHandles[106])) {
        if (!MissionUtility::IsAlive(mHandles[107])) {
        if (!MissionUtility::IsAlive(mHandles[108])) {
        MissionUtility::RemoveObject(mHandles[115]);
        MissionUtility::StartSound("AmbGen_FrcField_powerdn01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[45] = true;
        }
        }
        }
        }
        }
        }
        }
        }
        }
        if (!mFlags[93]) {
        if (mFlags[45]) {
        if (!mFlags[94]) {
        mTimes[20] = MissionUtility::GetTime();
        mFlags[94] = true;
        }
        if (!mFlags[95]) {
        if (mTimes[20] < MissionUtility::GetTime()) {
        MissionUtility::SetAltitude(mHandles[12], 218.0f);
        MissionUtility::SetTeamNum(mHandles[109], 5);
        MissionUtility::SetTeamNum(mHandles[110], 5);
        MissionUtility::SetTeamNum(mHandles[111], 5);
        MissionUtility::SetTeamNum(mHandles[112], 5);
        MissionUtility::RemoveObjectify(mHandles[109]);
        MissionUtility::RemoveObjectify(mHandles[110]);
        MissionUtility::RemoveObjectify(mHandles[111]);
        MissionUtility::RemoveObjectify(mHandles[112]);
        MissionUtility::SetOnRadar(mHandles[109], false);
        MissionUtility::SetOnRadar(mHandles[110], false);
        MissionUtility::SetOnRadar(mHandles[111], false);
        MissionUtility::SetOnRadar(mHandles[112], false);
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], true, false, true, false);
        MissionUtility::SetAnimation(mHandles[109], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[110], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[111], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[112], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[113], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[114], "fullanimation", 1.0f, 1);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[109], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[110], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[111], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[112], false, 250.0f);
        mFlags[47] = false;
        mTimes[21] = 20.0f + MissionUtility::GetTime();
        mFlags[96] = false;
        mFlags[95] = true;
        }
        }
        if (!mFlags[96]) {
        if (mTimes[21] < MissionUtility::GetTime()) {
        MissionUtility::SetAltitude(mHandles[12], 118.0f);
        MissionUtility::Objectify(mHandles[109], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[110], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[111], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[112], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::SetTeamNum(mHandles[109], 2);
        MissionUtility::SetTeamNum(mHandles[110], 2);
        MissionUtility::SetTeamNum(mHandles[111], 2);
        MissionUtility::SetTeamNum(mHandles[112], 2);
        MissionUtility::SetOnRadar(mHandles[109], true);
        MissionUtility::SetOnRadar(mHandles[110], true);
        MissionUtility::SetOnRadar(mHandles[111], true);
        MissionUtility::SetOnRadar(mHandles[112], true);
        MissionUtility::SetAnimation(mHandles[109], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[110], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[111], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[112], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[113], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[114], "fullanimation", -1.0f, 1);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[109], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[110], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[111], false, 250.0f);
        MissionUtility::StartSoundAtObject("c_aatank_turret02", mHandles[112], false, 250.0f);
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], false, false, false, false);
        mTimes[20] = 25.0f + MissionUtility::GetTime();
        mFlags[47] = true;
        mFlags[95] = false;
        if (!mFlags[34]) {
        MissionUtility::QueueSound("UDT23_13", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[34] = true;
        }
        mFlags[96] = true;
        }
        }
        if (!mFlags[93]) {
        if (!MissionUtility::IsAlive(mHandles[109])) {
        if (!MissionUtility::IsAlive(mHandles[110])) {
        if (!MissionUtility::IsAlive(mHandles[111])) {
        if (!MissionUtility::IsAlive(mHandles[112])) {
        mFlags[93] = true;
        }
        }
        }
        }
        }
        }
        }
        if (!mFlags[40]) {
        if (mFlags[93]) {
        if (!mFlags[41]) {
        mTimes[23] = MissionUtility::GetTime();
        mFlags[41] = true;
        }
        if (!mFlags[42]) {
        if (mTimes[23] < MissionUtility::GetTime()) {
        MissionUtility::SetAltitude(mHandles[12], 218.0f);
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], true, false, true, false);
        MissionUtility::SetTeamNum(mHandles[113], 5);
        MissionUtility::SetTeamNum(mHandles[114], 5);
        MissionUtility::RemoveObjectify(mHandles[113]);
        MissionUtility::RemoveObjectify(mHandles[114]);
        MissionUtility::SetOnRadar(mHandles[113], false);
        MissionUtility::SetOnRadar(mHandles[114], false);
        MissionUtility::SetAnimation(mHandles[113], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[114], "fullanimation", 1.0f, 1);
        mFlags[47] = false;
        mTimes[24] = 20.0f + MissionUtility::GetTime();
        mFlags[43] = false;
        mFlags[42] = true;
        }
        }
        if (!mFlags[43]) {
        if (mTimes[24] < MissionUtility::GetTime()) {
        MissionUtility::SetAltitude(mHandles[12], 118.0f);
        if (!mFlags[24]) {
        MissionUtility::QueueSound("UDT23_19", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[24] = true;
        }
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], false, false, false, false);
        MissionUtility::SetTeamNum(mHandles[113], 2);
        MissionUtility::SetTeamNum(mHandles[114], 2);
        MissionUtility::Objectify(mHandles[113], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[114], "missions.Thule3.marker.str0010", false, true, 0.0f);
        MissionUtility::SetAnimation(mHandles[113], "fullanimation", -1.0f, 1);
        MissionUtility::SetAnimation(mHandles[114], "fullanimation", -1.0f, 1);
        MissionUtility::SetOnRadar(mHandles[113], true);
        MissionUtility::SetOnRadar(mHandles[114], true);
        mTimes[23] = 25.0f + MissionUtility::GetTime();
        mFlags[47] = true;
        mFlags[42] = false;
        mFlags[43] = true;
        }
        }
        if (!mFlags[40]) {
        if (!MissionUtility::IsAlive(mHandles[113])) {
        if (!MissionUtility::IsAlive(mHandles[114])) {
        mFlags[47] = false;
        mFlags[40] = true;
        }
        }
        }
        }
        }
        if (!mFlags[38]) {
        if (mFlags[40]) {
        if (!mFlags[36]) {
        mHandles[116] = MissionUtility::CreateObjectSnap("neu_prop_harvester", mHandles[12], "hp_reaper", "", 2, -1);
        mHandles[178] = MissionUtility::CreateObject("reaper_harbinger_weapon", "end_gun1", "end_gun1", 2, -1, 0);
        mHandles[179] = MissionUtility::CreateObject("reaper_harbinger_weapon", "end_gun2", "end_gun2", 2, -1, 0);
        mHandles[182] = MissionUtility::CreateObject("reaper_harbinger_weapon", "end_gun3", "end_gun3", 2, -1, 0);
        mHandles[180] = MissionUtility::CreateObject("KAS_prop_lasertarget", "end_beam_1", 0, "", 1, -1);
        mHandles[181] = MissionUtility::CreateObject("KAS_prop_lasertarget", "end_beam_2", 0, "", 1, -1);
        mHandles[183] = MissionUtility::CreateObject("KAS_prop_lasertarget", "end_beam_3", 0, "", 1, -1);
        MissionUtility::Goto(mHandles[180], "end_beam_1", true);
        MissionUtility::Goto(mHandles[181], "end_beam_2", true);
        MissionUtility::Goto(mHandles[183], "end_beam_3", true);
        MissionUtility::AttackTarget(mHandles[178], mHandles[180], true, false, true, false);
        MissionUtility::AttackTarget(mHandles[179], mHandles[181], true, false, true, false);
        MissionUtility::AttackTarget(mHandles[182], mHandles[183], true, false, true, false);
        MissionUtility::SetAnimation(mHandles[12], "fullanimation", 1.0f, 1);
        MissionUtility::SetCollidable(mHandles[12], false);
        MissionUtility::SetMaxHealth(mHandles[116], 3000.0f);
        MissionUtility::SetCurHealth(mHandles[116], 3000.0f);
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], true, true, false, false);
        MissionUtility::QueueSound("UDT23_22", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[36] = true;
        }
        if (!mFlags[38]) {
        if (!MissionUtility::IsAlive(mHandles[116])) {
        MissionUtility::SetLightningSource(0);
        MissionUtility::SetLightningCount(1);
        MissionUtility::SetLightningDelay(0.5f, 2.0f);
        MissionUtility::AttackTarget(mHandles[12], mHandles[0], false, false, false, false);
        MissionUtility::SetTeamNum(mHandles[12], 0);
        MissionUtility::RemoveObject(mHandles[178]);
        MissionUtility::RemoveObject(mHandles[179]);
        MissionUtility::RemoveObject(mHandles[182]);
        mFlags[38] = true;
        }
        }
        }
        }
        if (mFlags[47]) {
        if (!MissionUtility::IsAlive(mHandles[66])) {
        mHandles[66] = MissionUtility::CreateObject("cis_spirit_ord", mHandles[113], "hp_special_4", "", 2, -1, 0);
        }
        if (!MissionUtility::IsAlive(mHandles[67])) {
        mHandles[67] = MissionUtility::CreateObject("cis_spirit_ord", mHandles[114], "hp_special_4", "", 2, -1, 0);
        }
        }
        if (!mFlags[32]) {
        if (mFlags[31]) {
        if (mFlags[81]) {
        if (mFlags[38]) {
        MissionUtility::ObjectiveComplete(mInts[3]);
        mTimes[2] = 0.5f + MissionUtility::GetTime();
        mFlags[32] = true;
        }
        }
        }
        }
        if (!mFlags[6]) {
        if (mTimes[2] < MissionUtility::GetTime()) {
        if (!mFlags[7]) {
        MissionUtility::RemoveObject(mHandles[3]);
        MissionUtility::RemoveObject(mHandles[4]);
        MissionUtility::RemoveObject(mHandles[5]);
        MissionUtility::RemoveObject(mHandles[6]);
        MissionUtility::RemoveObject(mHandles[7]);
        mTimes[22] = 3.5f + MissionUtility::GetTime();
        mHandles[177] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath", 0, "EndCinTank", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[177], true);
        MissionUtility::OverrideSoundRange(mHandles[12], true);
        MissionUtility::Goto(mHandles[177], "EndCinTankPath", false);
        MissionUtility::SetVelocForward(mHandles[177], 150.0f);
        mHandles[1] = MissionUtility::RunCin("EndCin", true, true);
        mFlags[7] = true;
        }
        if (!mFlags[2]) {
        if (mTimes[22] < MissionUtility::GetTime()) {
        MissionUtility::DamageObject(mHandles[12], 999999.9f, 999999.9f);
        MissionUtility::QueueSound("B_Reaper_die02", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Huge01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Lg01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Huge01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Lg01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Huge01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Lg01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Huge01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Lg01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Huge01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Lg01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Huge01", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("Veh_Explo_Lg01", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[2] = true;
        }
        }
        if (mFlags[7]) {
        if (!MissionUtility::IsCinRunning(mHandles[1])) {
        mTimes[1] = MissionUtility::GetTime();
        mFlags[6] = true;
        }
        }
        }
        }
        if (!mFlags[51]) {
        if (mTimes[1] < MissionUtility::GetTime()) {
        MissionUtility::StopMusic(0);
        MissionUtility::MissionSuccess();
        mFlags[51] = true;
        }
        }
    }

    // ---- +0x5ce0  112 bytes ----
    if (!mFlags[80]) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        if (mFlags[73]) {
        MissionUtility::BonusObjectiveFailed(mInts[1]);
        }
        if (mFlags[23]) {
        MissionUtility::BonusObjectiveFailed(mInts[2]);
        }
        if (mFlags[31]) {
        MissionUtility::BonusObjectiveFailed(mInts[3]);
        }
        mTimes[28] = 4.0f + MissionUtility::GetTime();
        mFlags[80] = true;
        }
    }

    // ---- +0x5d50  36 bytes ----
    if (mTimes[28] < MissionUtility::GetTime()) {
    MissionUtility::MissionFailure();
    mTimes[28] = 999999.9f + MissionUtility::GetTime();
    }

    // ---- +0x5d74  52 bytes ----
    MissionUtility::ExcludeTeam(mHandles[146], 1);
    MissionUtility::ExcludeTeam(mHandles[146], 0);
    int i;
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[146]); i++)
        MissionUtility::RemoveObject(MissionUtility::GetRegionNewMember(mHandles[146], i));

    // ---- +0x5da8  68 bytes ----
    MissionUtility::ExcludeTeam(mHandles[147], 1);
    MissionUtility::ExcludeTeam(mHandles[147], 0);
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[147]); i++)
        MissionUtility::RemoveObject(MissionUtility::GetRegionNewMember(mHandles[147], i));

    // ---- +0x5dec  68 bytes ----
    MissionUtility::ExcludeTeam(mHandles[148], 1);
    MissionUtility::ExcludeTeam(mHandles[148], 0);
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[148]); i++)
        MissionUtility::RemoveObject(MissionUtility::GetRegionNewMember(mHandles[148], i));

    // ---- +0x5e30  68 bytes ----
    MissionUtility::ExcludeTeam(mHandles[149], 1);
    MissionUtility::ExcludeTeam(mHandles[149], 0);
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[149]); i++)
        MissionUtility::RemoveObject(MissionUtility::GetRegionNewMember(mHandles[149], i));

    // ---- +0x5e74  76 bytes ----
    if (!mFlags[13]) {
    if (mFlags[99]) {
    if (MissionUtility::IsInsideRegion(mHandles[0], "deleter_region1")) {
    MissionUtility::DestroyRegionList(mHandles[146]);
    mFlags[13] = true;
    }
    }
    }

    // ---- +0x5ec0  60 bytes ----
    if (!mFlags[14] && mFlags[12]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "deleter_region2")) {
        MissionUtility::DestroyRegionList(mHandles[147]);
        mFlags[14] = true;
        }
    }

    // ---- +0x5efc  60 bytes ----
    if (!mFlags[15] && mFlags[109]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "deleter_region3")) {
        MissionUtility::DestroyRegionList(mHandles[148]);
        mFlags[15] = true;
        }
    }

    // ---- +0x5f38  60 bytes ----
    if (!mFlags[16] && mFlags[23]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "deleter_region4")) {
        MissionUtility::DestroyRegionList(mHandles[149]);
        mFlags[16] = true;
        }
    }

}

void Thule3Script::Setup()
{
        mFlags[17] = false;
        mTimes[0] = 999999.9f;
        mTimes[1] = 999999.9f;
        mTimes[28] = 999999.9f;
        mTimes[2] = 999999.9f;
        mTimes[3] = 999999.9f;
        mTimes[4] = 999999.9f;
        mTimes[5] = 999999.9f;
        mTimes[6] = 999999.9f;
        mTimes[22] = 999999.9f;
        mTimes[29] = 999999.9f;
        mTimes[30] = 999999.9f;
        mTimes[7] = 999999.9f;
        mTimes[8] = 999999.9f;
        mTimes[9] = 999999.9f;
        mTimes[10] = 999999.9f;
        mTimes[11] = 999999.9f;
        mTimes[12] = 999999.9f;
        mTimes[13] = 999999.9f;
        mTimes[14] = 999999.9f;
        mTimes[15] = 999999.9f;
        mTimes[16] = 999999.9f;
        mTimes[17] = 999999.9f;
        mTimes[18] = 999999.9f;
        mTimes[19] = 999999.9f;
        mTimes[21] = 999999.9f;
        mTimes[20] = 999999.9f;
        mTimes[24] = 999999.9f;
        mTimes[23] = 999999.9f;
        mTimes[25] = 999999.9f;
        mTimes[26] = 999999.9f;
        mTimes[27] = 999999.9f;
        mHandles[8] = MissionUtility::GetHandle("tank_squadmate1");
        mHandles[9] = MissionUtility::GetHandle("tank_squadmate2");
        mHandles[10] = MissionUtility::GetHandle("cydon_prax");
        mHandles[16] = MissionUtility::GetHandle("roadblock1a");
        mHandles[19] = MissionUtility::GetHandle("roadblock1b");
        mHandles[20] = MissionUtility::GetHandle("roadblock3a");
        mHandles[21] = MissionUtility::GetHandle("roadblock3b");
        mHandles[22] = MissionUtility::GetHandle("roadblock4a");
        mHandles[23] = MissionUtility::GetHandle("roadblock4b");
        mHandles[17] = MissionUtility::GetHandle("last_tank1");
        mHandles[18] = MissionUtility::GetHandle("last_tank2");
        mHandles[12] = MissionUtility::GetHandle("dark_reaper");
        mHandles[64] = MissionUtility::GetHandle("reaper_temple");
        mHandles[37] = MissionUtility::GetHandle("roadblock1a_turret1");
        mHandles[38] = MissionUtility::GetHandle("roadblock1a_turret2");
        MissionUtility::PreloadConfig("cis_walk_assault_dest");
        MissionUtility::PreloadConfig("cis_cydon_special_xpl");
        MissionUtility::PreloadConfig("cis_boss_reaper");
        MissionUtility::PreloadConfig("rep_fly_assault_far");
        MissionUtility::PreloadConfig("cis_tank_assault");
        MissionUtility::PreloadConfig("cis_tank_fighter");
        MissionUtility::PreloadConfig("cis_fly_vcarrier");
        MissionUtility::PreloadConfig("rep_fly_gunship");
        MissionUtility::PreloadConfig("cis_inf_dooku");
        MissionUtility::PreloadConfig("cis_inf_cydon");
        MissionUtility::PreloadConfig("rep_walk_assault");
        MissionUtility::PreloadConfig("rep_blaster_walk");
        MissionUtility::PreloadConfig("rep_blaster_walk_ord");
        MissionUtility::PreloadConfig("rep_mortar_assault");
        MissionUtility::PreloadConfig("rep_mortar_assault_ord");
        MissionUtility::PreloadConfig("rep_mortar_assault_xpl");
        MissionUtility::PreloadConfig("cis_walk_assault");
        MissionUtility::PreloadConfig("cis_beam_walk");
        MissionUtility::PreloadConfig("cis_blaster_walk");
        MissionUtility::PreloadConfig("cis_blaster_walk_ord");
        MissionUtility::PreloadConfig("thu_bldg_aquaduct_02");
        MissionUtility::PreloadConfig("CIS_boss_reaper_dest");
        MissionUtility::PreloadConfig("reaper_harbinger_weapon");
        MissionUtility::PreloadConfig("KAS_prop_lasertarget");
        MissionUtility::PreloadConfig("neu_prop_harvester");
        MissionUtility::PreloadConfig("cis_boss_reaper_shield");
        MissionUtility::PreloadConfig("thu_bldg_temple");
        MissionUtility::PreloadConfig("cis_boss_reaper");
        MissionUtility::PreloadConfig("rep_fly_vcarrier");
        MissionUtility::PreloadConfig("neu_prop_harvester_effect_spread_reaper");
        MissionUtility::PreloadConfig("cis_boss_reaper_power");
        MissionUtility::PreloadConfig("cis_boss_reaper_turret");
        MissionUtility::PreloadConfig("cis_blaster_reaper");
        MissionUtility::PreloadConfig("cis_blaster_reaper_ord");
        MissionUtility::PreloadConfig("cis_boss_reaper_launchport");
        MissionUtility::PreloadConfig("cis_spirit_ord");
        MissionUtility::PreloadConfig("cis_beam_walk_weak_ord");
        MissionUtility::PreloadConfig("cis_blaster_assault_weak_ord");
        MissionUtility::PreloadConfig("cis_cannon_assault_weak_ord");
        MissionUtility::PreloadConfig("rep_walk_sixleg");
        MissionUtility::PreloadConfig("rep_blaster_fighter1_weak_ord");
}

SPMission *Thule3BuildMission()
{
    return new Thule3Script();
}
