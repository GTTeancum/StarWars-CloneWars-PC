// Raxus2Script.cpp -- reconstruction of a shipped mission script. 30,380 bytes, byte-exact.
//
// The survey called this the most concentrated mission in the tier: 86% of its statements
// generate clean and 4% of its bytes, because one 14 KB statement carries every marker. It
// needed three hand corrections (analysis/mission_batch_b.md):
//
//   * that statement is a compare-chain `switch (mInts[2])` over the end cinematic's three
//     clone waves. Cases 1 and 2 branch to the end of the switch; case 3 falls out of it;
//   * the registered block at +0xac is 24 floats -- `Setup` clears every one with `stfs`;
//   * one mid-mission timer restore through a stack float.

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
    int    AddFlockMember(int, int, Vector);
    int    AddHealthBar(int, const char*, float);
    int    AddObjective(const char*);
    void   AddSquadMember(int, int);
    void   AddTurnAroundRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    void   AttackTarget(int, int, bool, bool, bool, bool);
    void   BonusObjectiveComplete(int, bool);
    void   BonusObjectiveFailed(int);
    void   CarrierAddCargo(int, const char*, int, const char*, bool);
    void   CarrierDropoff(int, const char*, int, float);
    int    CountUnitsNearObject(int, float, int, const char*);
    int    CreateFlock(int, Formation);
    int    CreateObject(const char*, const char*, int, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    int    CreateSquad(int, Formation);
    void   DisbandSquadMember(int, int);
    void   DisplayText(const char*, float, float);
    void   FlushSoundQueue();
    void   Garbage(int, const char*, bool);
    int    GetCinId(int);
    float  GetDistance(int, const char*);
    float  GetDistance(int, const char*, int);
    float  GetDistance(int, int);
    int    GetHandle(const char*);
    int    GetPlayerHandle(int);
    float  GetTime();
    int    GetWhoShotMe(int);
    void   Goto(int, const char*, bool);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsSoundPlaying(int);
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
    void   MoveObject(int, const char*, int, bool);
    void   MoveObjectWithRotation(int, const char*, int, bool);
    void   Objectify(const char*, int, const char*, bool, bool, float, float);
    void   Objectify(int, const char*, bool, bool, float);
    void   ObjectiveComplete(int);
    void   OverrideSoundRange(int, bool);
    void   PlayMusic(const char*, bool);
    void   PreloadConfig(const char*);
    int    QueueSound(const char*, float, float, float, const char*, int, const char*);
    void   RemoveFlock(int, bool);
    void   RemoveObject(int);
    void   RemoveObjectify(const char*, int);
    void   RemoveObjectify(int);
    int    RunCin(const char*, bool, bool);
    void   SetAlliance(int, int);
    void   SetApplyDynamics(int, bool);
    void   SetAsPlayer(int, int);
    void   SetAttackRange(int, int);
    void   SetCollidable(int, bool);
    void   SetCurHealth(int, float);
    void   SetEnemies(int, int);
    void   SetFogRange(float, float, float);
    void   SetMaxAltitude(int, float);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetOriginalFog(float);
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
    void   StartAmbiences(const char*, const char*, float, float);
    void   StopSound(int);
    void   TakeOff(int);
}

static const char *const kClassName = "Raxus2Script";
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

class Raxus2Script : public SPMission
{
public:
    virtual ~Raxus2Script();
    Raxus2Script()
    {
        mBoolCount = 130;   mBools = mFlags;
        mCountB    = 24;    mIntsB = (int *)mFloatsB;
        mCountC    = 221;   mIntsC = mHandles;
        mCountD    = 3;     mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    char   mPad24[1];
    bool   mFlags[130];            // +0x025  the one-shot latches
    char   mPadA7[5];                  // +0x0a7
    float  mFloatsB[24];            // +0x0ac
    char   mPad10C[8];                  // +0x10c
    int    mHandles[221];          // +0x114
    char   mPad488[8];                  // +0x488
    int    mInts[3];             // +0x490
    char   mPad49C[4];                  // +0x49c
    Timer  mTimer0;                      // +0x4a0
    Timer  mTimer1;                      // +0x4ac
    Timer  mTimer2;                      // +0x4b8
    Timer  mTimer3;                      // +0x4c4
    Timer  mTimer4;                      // +0x4d0
    Timer  mTimer5;                      // +0x4dc
    Timer  mTimer6;                      // +0x4e8
    Timer  mTimer7;                      // +0x4f4
    Timer  mTimer8;                      // +0x500
    Timer  mTimer9;                      // +0x50c
    Timer  mTimer10;                      // +0x518
};

Raxus2Script::~Raxus2Script()
{
}

void Raxus2Script::Execute()
{
    float mLoadTA;

    // ---- +0x0008  2432 bytes ----
    mHandles[0] = MissionUtility::GetPlayerHandle(0);
    if (mFlags[0]) {
    MissionUtility::SetMusicLooping(true);
    BeginTimer(mTimer1);
    MissionUtility::SetEnemies(1, 2);
    MissionUtility::SetEnemies(1, 3);
    MissionUtility::SetEnemies(1, 4);
    MissionUtility::SetAlliance(1, 0);
    MissionUtility::SetAlliance(2, 0);
    MissionUtility::SetAlliance(1, 5);
    MissionUtility::SetAlliance(3, 5);
    MissionUtility::SetEnemies(5, 2);
    mHandles[99] = MissionUtility::AddBonusObjective("missions.Raxus2.bonus.str0011");
    mHandles[98] = MissionUtility::AddBonusObjective("missions.Raxus2.bonus.str0010");
    mHandles[100] = MissionUtility::AddBonusObjective("missions.Raxus2.bonus.str0002");
    mHandles[140] = MissionUtility::CreateFlock(mHandles[103], (Formation)8);
    MissionUtility::AddFlockMember(mHandles[140], mHandles[101], Vector(-14.0f, 0.0f, -44.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[102], Vector(14.0f, 0.0f, -44.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[106], Vector(-33.0f, 0.0f, -5.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[107], Vector(33.0f, 0.0f, -5.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[112], Vector(-51.0f, 0.0f, -5.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[113], Vector(51.0f, 0.0f, -5.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[104], Vector(0.0f, 0.0f, 60.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[108], Vector(-33.0f, 0.0f, 70.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[109], Vector(33.0f, 0.0f, 70.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[114], Vector(-51.0f, 0.0f, 70.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[115], Vector(51.0f, 0.0f, 70.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[105], Vector(0.0f, 0.0f, 130.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[110], Vector(-33.0f, 0.0f, 140.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[111], Vector(33.0f, 0.0f, 140.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[116], Vector(-51.0f, 0.0f, 140.0f));
    MissionUtility::AddFlockMember(mHandles[140], mHandles[117], Vector(51.0f, 0.0f, 140.0f));
    MissionUtility::AddTurnAroundRegion("turnaround1", "turnaround1_point1", 0, 0, 0, 0);
    if (MissionUtility::MidMissionGetSavePoint() == 1) {
    mHandles[0] = MissionUtility::CreateObject("rep_tank_fighter1_player", "move_player_here", 0, "player", 1, -1, Quat(0.0f, 0.0f, -1.0f, 0.0f), -1);
    MissionUtility::SetAsPlayer(mHandles[0], 0);
    mHandles[1] = MissionUtility::CreateSquad(mHandles[0], (Formation)1);
    MissionUtility::RemoveObject(mHandles[124]);
    if (!mFlags[41]) {
    mHandles[2] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "move_squadmate1_here", 0, "squadmate1", 1, -1, Quat(0.0f, 0.0f, -1.0f, 0.0f), -1);
    MissionUtility::AddSquadMember(mHandles[1], mHandles[2]);
    }
    if (!mFlags[42]) {
    mHandles[3] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "move_squadmate2_here", 0, "squadmate2", 1, -1, Quat(0.0f, 0.0f, -1.0f, 0.0f), -1);
    MissionUtility::AddSquadMember(mHandles[1], mHandles[3]);
    }
    MissionUtility::RemoveFlock(mHandles[140], true);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mFlags[41]);
    MissionUtility::MidMissionLoad(mFlags[42]);
    MissionUtility::MidMissionLoad(mLoadTA);
    StopTimer(mTimer1);
    mTimer1 = mLoadTA;
    ResumeTimer(mTimer1);
    MissionUtility::MidMissionLoad(mFlags[29]);
    MissionUtility::MidMissionLoad(mFlags[30]);
    MissionUtility::MidMissionLoad(mFlags[31]);
    if (mFlags[29]) {
    MissionUtility::RemoveObject(mHandles[85]);
    }
    if (mFlags[30]) {
    MissionUtility::RemoveObject(mHandles[86]);
    }
    if (mFlags[31]) {
    MissionUtility::RemoveObject(mHandles[87]);
    }
    mFloatsB[1] = MissionUtility::GetTime();
    mHandles[93] = MissionUtility::AddObjective("missions.Raxus2.objective.str0000");
    mHandles[94] = MissionUtility::AddObjective("missions.Raxus2.objective.str0001");
    MissionUtility::ObjectiveComplete(mHandles[93]);
    MissionUtility::ObjectiveComplete(mHandles[94]);
    MissionUtility::RemoveObject(mHandles[127]);
    MissionUtility::RemoveObject(mHandles[128]);
    MissionUtility::RemoveObject(mHandles[129]);
    MissionUtility::RemoveObject(mHandles[130]);
    MissionUtility::RemoveObject(mHandles[131]);
    MissionUtility::RemoveObject(mHandles[132]);
    MissionUtility::RemoveObject(mHandles[133]);
    MissionUtility::RemoveObject(mHandles[12]);
    MissionUtility::RemoveObject(mHandles[13]);
    mFlags[15] = true;
    mFlags[75] = true;
    }
    MissionUtility::StartAmbiences("AmbRaxus_rain01_pl2", "AmbRaxus_desert_stinger01", 10.0f, 30.0f);
    MissionUtility::SetSquadAttackSound("squad_attacktarget_anakin");
    MissionUtility::SetSquadBreakSound("squad_breakandattack_anakin");
    MissionUtility::SetSquadHoldSound("squad_holdposition_anakin");
    MissionUtility::SetSquadRegroupSound("squad_regroup_anakin");
    MissionUtility::SetSquadInvalidSound("squad_invalidtarget_anakin");
    mHandles[88] = MissionUtility::CreateObjectWithRotation("rax_fly_collector", "collector1_path", 0, "collector1", 0, -1, -1);
    MissionUtility::SetVelocMinimumFly(mHandles[88], 35.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[88], 35.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[88], 35.0f);
    MissionUtility::SetApplyDynamics(mHandles[88], true);
    MissionUtility::Garbage(mHandles[88], "collector1_path", true);
    mHandles[89] = MissionUtility::CreateObjectWithRotation("rax_fly_collector", "collector2_path", 0, "collector2", 0, -1, -1);
    MissionUtility::SetVelocMinimumFly(mHandles[89], 35.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[89], 35.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[89], 35.0f);
    MissionUtility::SetApplyDynamics(mHandles[89], true);
    MissionUtility::Garbage(mHandles[89], "collector2_path", true);
    mHandles[90] = MissionUtility::CreateObjectWithRotation("rax_fly_collector", "collector3_path", 0, "collector3", 0, -1, -1);
    MissionUtility::SetVelocMinimumFly(mHandles[90], 35.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[90], 35.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[90], 35.0f);
    MissionUtility::SetApplyDynamics(mHandles[90], true);
    MissionUtility::Garbage(mHandles[90], "collector3_path", true);
    mHandles[91] = MissionUtility::CreateObjectWithRotation("rax_fly_collector", "collector4_path", 0, "collector4", 0, -1, -1);
    MissionUtility::SetVelocMinimumFly(mHandles[91], 35.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[91], 35.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[91], 35.0f);
    MissionUtility::SetApplyDynamics(mHandles[91], true);
    MissionUtility::Garbage(mHandles[91], "collector4_path", true);
    mHandles[92] = MissionUtility::CreateObjectWithRotation("rax_fly_collector", "collector5_path", 0, "collector5", 0, -1, -1);
    MissionUtility::SetVelocMinimumFly(mHandles[92], 35.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[92], 35.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[92], 35.0f);
    MissionUtility::SetApplyDynamics(mHandles[92], true);
    MissionUtility::Garbage(mHandles[92], "collector5_path", true);
    mFlags[0] = false;
    }

    // ---- +0x0988  11544 bytes ----
    if (!mFlags[75]) {
        if (!mFlags[77]) {
        MissionUtility::Goto(mHandles[140], "flock1_go", true);
        MissionUtility::SetVelocForward(mHandles[140], 30.0f);
        mFlags[77] = true;
        }
        if (!mFlags[15]) {
        if (!mFlags[16]) {
        MissionUtility::PlayMusic("EP5_V1_T05_02", true);
        MissionUtility::SetTeamNum(mHandles[87], 0);
        StopTimer(mTimer1);
        mHandles[157] = MissionUtility::CreateObjectWithRotation("cis_tank_gtrans", "OpenCinConvoyPath", 0, "OpenCinConvoy", 0, -1, -1);
        mHandles[158] = MissionUtility::CreateObjectWithRotation("cis_tank_gtrans", "OpenCinConvoyPath1", 0, "OpenCinConvoy1", 0, -1, -1);
        mHandles[159] = MissionUtility::CreateObjectWithRotation("cis_tank_gtrans", "OpenCinConvoyPath2", 0, "OpenCinConvoy2", 0, -1, -1);
        mHandles[160] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "OpenCinConvoyPath3", 0, "OpenCinConvoy3", 0, -1, -1);
        mHandles[161] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "OpenCinConvoyPath4", 0, "OpenCinConvoy4", 0, -1, -1);
        mHandles[162] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "OpenCinConvoyPath5", 0, "OpenCinConvoy5", 0, -1, -1);
        mHandles[163] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "OpenCinConvoyPath6", 0, "OpenCinConvoy6", 0, -1, -1);
        mHandles[164] = MissionUtility::CreateObjectWithRotation("cis_tank_assault", "OpenCinConvoyPath7", 0, "OpenCinConvoy7", 0, -1, -1);
        MissionUtility::Goto(mHandles[157], "OpenCinConvoyPath", false);
        MissionUtility::Goto(mHandles[158], "OpenCinConvoyPath1", false);
        MissionUtility::Goto(mHandles[159], "OpenCinConvoyPath2", false);
        MissionUtility::Goto(mHandles[160], "OpenCinConvoyPath3", false);
        MissionUtility::Goto(mHandles[161], "OpenCinConvoyPath4", false);
        MissionUtility::Goto(mHandles[162], "OpenCinConvoyPath5", false);
        MissionUtility::Goto(mHandles[163], "OpenCinConvoyPath6", false);
        MissionUtility::Goto(mHandles[164], "OpenCinConvoyPath7", false);
        mHandles[154] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "OpenCinGunshipPath", 0, "OpenCinGunship", 0, -1, -1);
        mHandles[155] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "OpenCinGunshipPath1", 0, "OpenCinGunship1", 0, -1, -1);
        mHandles[156] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "OpenCinGunshipPath2", 0, "OpenCinGunship2", 0, -1, -1);
        mHandles[165] = MissionUtility::CreateObject("rep_tank_fighter1_player", "open_cin_move_player", 0, "OpenCinGunshipTank", 0, -1);
        mHandles[166] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "open_cin_move_squadmate1", 0, "OpenCinGunshipTank1", 0, -1);
        mHandles[167] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "open_cin_move_squadmate2", 0, "OpenCinGunshipTank2", 0, -1);
        MissionUtility::CarrierAddCargo(mHandles[154], "hp_link_1", mHandles[165], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[155], "hp_link_1", mHandles[166], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[156], "hp_link_1", mHandles[167], "hp_link_1", true);
        MissionUtility::Goto(mHandles[154], "OpenCinGunshipPath", true);
        MissionUtility::Goto(mHandles[155], "OpenCinGunshipPath1", true);
        MissionUtility::Goto(mHandles[156], "OpenCinGunshipPath2", true);
        MissionUtility::SetVelocNeutralFly(mHandles[154], 150.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[154], 150.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[154], 150.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[155], 150.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[155], 150.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[155], 150.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[156], 150.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[156], 150.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[156], 150.0f);
        MissionUtility::SetCollidable(mHandles[154], false);
        MissionUtility::SetCollidable(mHandles[155], false);
        MissionUtility::SetCollidable(mHandles[156], false);
        MissionUtility::OverrideSoundRange(mHandles[154], true);
        MissionUtility::OverrideSoundRange(mHandles[155], true);
        MissionUtility::OverrideSoundRange(mHandles[156], true);
        MissionUtility::SetFogRange(200.0f, 1800.0f, 2.0f);
        mHandles[81] = MissionUtility::RunCin("open_cin", true, true);
        mFloatsB[17] = 10.0f + MissionUtility::GetTime();
        mFlags[16] = true;
        BeginTimer(mTimer7);
        }
        if (!mFlags[129]) {
        if (mTimer7 > 2.0f) {
        StopTimer(mTimer7);
        mTimer7 = 0.0f;
        MissionUtility::QueueSound("obr09_19", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("asr09_15", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("obr09_20", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("asr09_16", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("obr09_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("asr09_17", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        }
        if (!mFlags[129]) {
        if (!mFlags[60]) {
        if (mFloatsB[17] < MissionUtility::GetTime()) {
        mHandles[121] = MissionUtility::CreateObject("rep_tank_fighter1_player", "open_cin_move_player", 0, "cin_player", 0, -1);
        mHandles[122] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "open_cin_move_squadmate1", 0, "cin_squadmate1", 0, -1);
        mHandles[123] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "open_cin_move_squadmate2", 0, "cin_squadmate2", 0, -1);
        mHandles[118] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "cin_gunship1_path", 0, "cin_gunship1", 0, -1, -1);
        mHandles[119] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "cin_gunship2_path", 0, "cin_gunship2", 0, -1, -1);
        mHandles[120] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "cin_gunship3_path", 0, "cin_gunship3", 0, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[118], "hp_link_1", mHandles[121], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[119], "hp_link_1", mHandles[122], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[120], "hp_link_1", mHandles[123], "hp_link_1", true);
        MissionUtility::OverrideSoundRange(mHandles[118], true);
        MissionUtility::OverrideSoundRange(mHandles[119], true);
        MissionUtility::OverrideSoundRange(mHandles[120], true);
        MissionUtility::OverrideSoundRange(mHandles[121], true);
        MissionUtility::OverrideSoundRange(mHandles[122], true);
        MissionUtility::OverrideSoundRange(mHandles[123], true);
        MissionUtility::Goto(mHandles[118], "cin_gunship1_path", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[118], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[118], "cin_gunship_exit", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[119], "cin_gunship2_path", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[119], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[119], "cin_gunship_exit", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[120], "cin_gunship3_path", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[120], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[120], "cin_gunship_exit", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetCollidable(mHandles[118], false);
        MissionUtility::SetCollidable(mHandles[119], false);
        MissionUtility::SetCollidable(mHandles[120], false);
        MissionUtility::SetVelocNeutralFly(mHandles[118], 70.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[118], 70.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[118], 70.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[119], 70.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[119], 70.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[119], 70.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[120], 70.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[120], 70.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[120], 70.0f);
        mFlags[60] = true;
        }
        }
        }
        if (!mFlags[129]) {
        if (!mFlags[126]) {
        if (MissionUtility::GetDistance(mHandles[118], "cin_gunship1_path", 3) < 20.0f) {
        mFlags[126] = true;
        MissionUtility::CarrierDropoff(mHandles[118], 0, 0, 80.0f);
        }
        }
        }
        if (!mFlags[129]) {
        if (!mFlags[127]) {
        if (MissionUtility::GetDistance(mHandles[119], "cin_gunship2_path", 3) < 20.0f) {
        mFlags[127] = true;
        MissionUtility::CarrierDropoff(mHandles[119], 0, 0, 80.0f);
        }
        }
        }
        if (!mFlags[129]) {
        if (!mFlags[128]) {
        if (MissionUtility::GetDistance(mHandles[120], "cin_gunship3_path", 3) < 50.0f) {
        mFlags[128] = true;
        MissionUtility::CarrierDropoff(mHandles[120], 0, 0, 80.0f);
        }
        }
        }
        if (!mFlags[129]) {
        if (mTimer3 > 8.0f) {
        StopTimer(mTimer3);
        mTimer3 = 0.0f;
        MissionUtility::TakeOff(mHandles[118]);
        }
        }
        if (!mFlags[129]) {
        if (mTimer4 > 8.5f) {
        StopTimer(mTimer4);
        mTimer4 = 0.0f;
        MissionUtility::TakeOff(mHandles[119]);
        }
        }
        if (!mFlags[129]) {
        if (mTimer5 > 8.9f) {
        StopTimer(mTimer5);
        mTimer5 = 0.0f;
        MissionUtility::TakeOff(mHandles[120]);
        }
        }
        if (!mFlags[129]) {
        if (!mFlags[68]) {
        if (MissionUtility::GetCinId(mHandles[81]) == 3) {
        mFloatsB[16] = 3.0f + MissionUtility::GetTime();
        mFlags[68] = true;
        }
        }
        }
        if (!mFlags[67]) {
        if (mFloatsB[16] < MissionUtility::GetTime()) {
        mFlags[67] = true;
        }
        }
        if (mFlags[16]) {
        if (!MissionUtility::IsCinRunning(mHandles[81])) {
        MissionUtility::FlushSoundQueue();
        mFlags[129] = true;
        MissionUtility::Objectify("ambush_point", 0, "missions.Raxus2.marker.str0000", true, false, 0.0f, 2.0f);
        MissionUtility::DisplayText("missions.Raxus2.text.str0000", 6.0f, -1.0f);
        mHandles[93] = MissionUtility::AddObjective("missions.Raxus2.objective.str0000");
        MissionUtility::RemoveFlock(mHandles[140], false);
        MissionUtility::RemoveObject(mHandles[118]);
        MissionUtility::RemoveObject(mHandles[119]);
        MissionUtility::RemoveObject(mHandles[120]);
        MissionUtility::RemoveObject(mHandles[154]);
        MissionUtility::RemoveObject(mHandles[155]);
        MissionUtility::RemoveObject(mHandles[156]);
        MissionUtility::RemoveObject(mHandles[165]);
        MissionUtility::RemoveObject(mHandles[166]);
        MissionUtility::RemoveObject(mHandles[167]);
        MissionUtility::RemoveObject(mHandles[121]);
        MissionUtility::RemoveObject(mHandles[122]);
        MissionUtility::RemoveObject(mHandles[123]);
        MissionUtility::RemoveObject(mHandles[157]);
        MissionUtility::RemoveObject(mHandles[158]);
        MissionUtility::RemoveObject(mHandles[159]);
        MissionUtility::RemoveObject(mHandles[160]);
        MissionUtility::RemoveObject(mHandles[161]);
        MissionUtility::RemoveObject(mHandles[162]);
        MissionUtility::RemoveObject(mHandles[163]);
        MissionUtility::RemoveObject(mHandles[164]);
        MissionUtility::RemoveObject(mHandles[155]);
        MissionUtility::RemoveObject(mHandles[156]);
        MissionUtility::RemoveObject(mHandles[154]);
        mHandles[0] = MissionUtility::CreateObject("rep_tank_fighter1_player", "open_cin_move_player", 0, "player", 1, -1, Quat(0.0f, 0.0f, -1.0f, 0.0f), -1);
        MissionUtility::SetAsPlayer(mHandles[0], 0);
        mHandles[2] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "open_cin_move_squadmate2", 0, "squadmate1", 1, -1, Quat(0.0f, 0.0f, -1.0f, 0.0f), -1);
        mHandles[3] = MissionUtility::CreateObject("rep_tank_fighter1_squadmate", "open_cin_move_squadmate1", 0, "squadmate2", 1, -1, Quat(0.0f, 0.0f, -1.0f, 0.0f), -1);
        mHandles[1] = MissionUtility::CreateSquad(mHandles[0], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[1], mHandles[2]);
        MissionUtility::AddSquadMember(mHandles[1], mHandles[3]);
        MissionUtility::RemoveObject(mHandles[124]);
        if (MissionUtility::IsSoundPlaying(mHandles[125])) {
        MissionUtility::StopSound(mHandles[125]);
        }
        if (MissionUtility::IsSoundPlaying(mHandles[126])) {
        MissionUtility::StopSound(mHandles[126]);
        }
        ResumeTimer(mTimer1);
        MissionUtility::PlayMusic("EP6_V2_T05_01", true);
        MissionUtility::SetTeamNum(mHandles[87], 2);
        MissionUtility::SetOriginalFog(0.1f);
        mFloatsB[4] = 360.0f + MissionUtility::GetTime();
        mFloatsB[23] = 420.0f + MissionUtility::GetTime();
        mFlags[15] = true;
        }
        }
        }
        if (!mFlags[41]) {
        if (mFlags[15]) {
        if (!MissionUtility::IsAlive(mHandles[2])) {
        mFlags[41] = true;
        }
        }
        }
        if (!mFlags[42]) {
        if (mFlags[15]) {
        if (!MissionUtility::IsAlive(mHandles[3])) {
        mFlags[42] = true;
        }
        }
        }
        if (!mFlags[74]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "turnaround1")) {
        MissionUtility::QueueSound("ASR09_24", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[74] = true;
        }
        }
        if (!mFlags[32]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "squad_tutorial_region")) {
        MissionUtility::QueueSound("ASR09_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[32] = true;
        }
        }
        if (!mFlags[1]) {
        if (!mFlags[11]) {
        if (mFloatsB[4] < MissionUtility::GetTime()) {
        MissionUtility::DisplayText("missions.Raxus2.text.str0015", 6.0f, -1.0f);
        mFlags[1] = true;
        }
        }
        }
        if (!mFlags[2]) {
        if (!mFlags[11]) {
        if (mFloatsB[23] < MissionUtility::GetTime()) {
        MissionUtility::DisplayText("missions.Raxus2.text.str0016", 6.0f, -1.0f);
        mFloatsB[6] = 4.0f + MissionUtility::GetTime();
        mFlags[2] = true;
        }
        }
        }
        if (!mFlags[11]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "ambush_region")) {
        MissionUtility::RemoveObjectify("ambush_point", 0);
        mHandles[16] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy_pattern", 4, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[17] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy_pattern", 9, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[18] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy_pattern", 14, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[19] = MissionUtility::CreateObject("cis_tank_assault", "convoy_pattern", 0, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[20] = MissionUtility::CreateObject("cis_tank_assault", "convoy_pattern", 1, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[21] = MissionUtility::CreateObject("cis_tank_assault", "convoy_pattern", 3, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[22] = MissionUtility::CreateObject("cis_tank_assault", "convoy_pattern", 5, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[23] = MissionUtility::CreateObject("cis_tank_assault", "convoy_pattern", 8, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[24] = MissionUtility::CreateObject("cis_tank_assault", "convoy_pattern", 10, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[25] = MissionUtility::CreateObject("cis_tank_assault", "convoy_pattern", 13, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[26] = MissionUtility::CreateObject("cis_tank_assault", "convoy_pattern", 15, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[27] = MissionUtility::CreateObject("cis_bike_speeder", "convoy_pattern", 2, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[28] = MissionUtility::CreateObject("cis_bike_speeder", "convoy_pattern", 6, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[29] = MissionUtility::CreateObject("cis_bike_speeder", "convoy_pattern", 7, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[30] = MissionUtility::CreateObject("cis_bike_speeder", "convoy_pattern", 11, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[31] = MissionUtility::CreateObject("cis_bike_speeder", "convoy_pattern", 12, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[32] = MissionUtility::CreateObject("cis_bike_speeder", "convoy_pattern", 16, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[15] = MissionUtility::CreateFlock(mHandles[16], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[15], mHandles[19], Vector(-14.0f, 0.0f, -44.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[20], Vector(14.0f, 0.0f, -44.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[21], Vector(-33.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[22], Vector(33.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[27], Vector(-51.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[28], Vector(51.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[17], Vector(0.0f, 0.0f, 60.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[23], Vector(-33.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[24], Vector(33.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[29], Vector(-51.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[30], Vector(51.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[18], Vector(0.0f, 0.0f, 130.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[25], Vector(-33.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[26], Vector(33.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[31], Vector(-51.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[15], mHandles[32], Vector(51.0f, 0.0f, 140.0f));
        MissionUtility::SetVelocForward(mHandles[15], 40.0f);
        MissionUtility::Goto(mHandles[15], "convoy1_path", true);
        MissionUtility::SetAttackRange(mHandles[19], 1);
        MissionUtility::SetAttackRange(mHandles[20], 1);
        MissionUtility::SetAttackRange(mHandles[16], 1);
        MissionUtility::SetAttackRange(mHandles[17], 1);
        MissionUtility::SetAttackRange(mHandles[18], 1);
        MissionUtility::SetAttackRange(mHandles[21], 1);
        MissionUtility::SetAttackRange(mHandles[22], 1);
        MissionUtility::SetAttackRange(mHandles[23], 1);
        MissionUtility::SetAttackRange(mHandles[24], 1);
        MissionUtility::SetAttackRange(mHandles[25], 1);
        MissionUtility::SetAttackRange(mHandles[26], 1);
        MissionUtility::SetAttackRange(mHandles[27], 1);
        MissionUtility::SetAttackRange(mHandles[28], 1);
        MissionUtility::SetAttackRange(mHandles[29], 1);
        MissionUtility::SetAttackRange(mHandles[30], 1);
        MissionUtility::SetAttackRange(mHandles[31], 1);
        MissionUtility::SetAttackRange(mHandles[32], 1);
        MissionUtility::ObjectiveComplete(mHandles[93]);
        mFloatsB[0] = 2.0f + MissionUtility::GetTime();
        mFlags[11] = true;
        }
        }
        if (!mFlags[55]) {
        if (!mFlags[11]) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        MissionUtility::BonusObjectiveFailed(mHandles[93]);
        mFloatsB[15] = 3.0f + MissionUtility::GetTime();
        mFlags[55] = true;
        }
        }
        }
        if (!mFlags[18]) {
        if (mFloatsB[0] < MissionUtility::GetTime()) {
        if (!mFlags[19]) {
        StopTimer(mTimer1);
        MissionUtility::RemoveObject(mHandles[12]);
        MissionUtility::RemoveObject(mHandles[13]);
        mHandles[82] = MissionUtility::RunCin("convoy1_cin", true, true);
        mFlags[19] = true;
        MissionUtility::SetFogRange(200.0f, 1800.0f, 2.0f);
        MissionUtility::MoveObjectWithRotation(mHandles[0], "ConvoyCinSpot", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[2], "ConvoyCinSpot1", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[3], "ConvoyCinSpot2", 0, true);
        }
        if (mFlags[19]) {
        if (!MissionUtility::IsCinRunning(mHandles[82])) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::Objectify(mHandles[19], "missions.Raxus2.marker.str0001", false, true, 0.0f);
        MissionUtility::DisplayText("missions.Raxus2.text.str0001", 6.0f, -1.0f);
        mHandles[94] = MissionUtility::AddObjective("missions.Raxus2.objective.str0001");
        MissionUtility::SetTeamNum(mHandles[2], 5);
        MissionUtility::SetTeamNum(mHandles[3], 5);
        if (MissionUtility::IsSoundPlaying(mHandles[134])) {
        MissionUtility::StopSound(mHandles[134]);
        }
        mFlags[18] = true;
        ResumeTimer(mTimer1);
        MissionUtility::SetOriginalFog(0.1f);
        }
        }
        }
        }
        if (!mFlags[110]) {
        if (MissionUtility::IsAlive(mHandles[19])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[19])) {
        MissionUtility::AttackTarget(mHandles[19], mHandles[0], true, true, false, false);
        mFlags[110] = true;
        }
        }
        }
        if (!mFlags[111]) {
        if (MissionUtility::IsAlive(mHandles[20])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[20])) {
        MissionUtility::AttackTarget(mHandles[20], mHandles[0], true, true, false, false);
        mFlags[111] = true;
        }
        }
        }
        if (!mFlags[112]) {
        if (MissionUtility::IsAlive(mHandles[21])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[21])) {
        MissionUtility::AttackTarget(mHandles[21], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[21], 35.0f);
        mFlags[112] = true;
        }
        }
        }
        if (!mFlags[113]) {
        if (MissionUtility::IsAlive(mHandles[22])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[22])) {
        MissionUtility::AttackTarget(mHandles[22], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[22], 35.0f);
        mFlags[113] = true;
        }
        }
        }
        if (!mFlags[114]) {
        if (MissionUtility::IsAlive(mHandles[23])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[23])) {
        MissionUtility::AttackTarget(mHandles[23], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[23], 35.0f);
        mFlags[114] = true;
        }
        }
        }
        if (!mFlags[115]) {
        if (MissionUtility::IsAlive(mHandles[24])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[24])) {
        MissionUtility::AttackTarget(mHandles[24], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[24], 35.0f);
        mFlags[115] = true;
        }
        }
        }
        if (!mFlags[116]) {
        if (MissionUtility::IsAlive(mHandles[25])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[25])) {
        MissionUtility::AttackTarget(mHandles[25], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[25], 35.0f);
        mFlags[116] = true;
        }
        }
        }
        if (!mFlags[117]) {
        if (MissionUtility::IsAlive(mHandles[26])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[26])) {
        MissionUtility::AttackTarget(mHandles[26], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[26], 35.0f);
        mFlags[117] = true;
        }
        }
        }
        if (!mFlags[50]) {
        if (!mFlags[51]) {
        if (!mFlags[47]) {
        if (mFlags[18]) {
        if (!MissionUtility::IsInsideRegion(mHandles[0], "hide_region")) {
        if (!mFlags[49]) {
        MissionUtility::QueueSound("ASR09_09A", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[49] = true;
        }
        MissionUtility::Objectify("ambush_point", 0, "missions.Raxus2.marker.str0002", true, false, 0.0f, 2.0f);
        mFloatsB[13] = 7.0f + MissionUtility::GetTime();
        mFlags[48] = false;
        mFlags[47] = true;
        }
        }
        }
        if (!mFlags[48]) {
        if (mFlags[47]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "hide_region")) {
        MissionUtility::RemoveObjectify("ambush_point", 0);
        mFloatsB[13] = 999999.9f + MissionUtility::GetTime();
        mFlags[47] = false;
        mFlags[48] = true;
        }
        }
        }
        }
        }
        if (!mFlags[50]) {
        if (!mFlags[51]) {
        if (mFloatsB[13] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("ASR09_12A", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("ASR09_10A", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::PlayMusic("EP1_V1_T05", true);
        MissionUtility::SetTeamNum(mHandles[2], 1);
        MissionUtility::SetTeamNum(mHandles[3], 1);
        MissionUtility::SetAttackRange(mHandles[19], 1000);
        MissionUtility::SetAttackRange(mHandles[20], 1000);
        MissionUtility::SetAttackRange(mHandles[16], 1000);
        MissionUtility::SetAttackRange(mHandles[17], 1000);
        MissionUtility::SetAttackRange(mHandles[18], 1000);
        MissionUtility::SetAttackRange(mHandles[21], 1000);
        MissionUtility::SetAttackRange(mHandles[22], 1000);
        MissionUtility::SetAttackRange(mHandles[23], 1000);
        MissionUtility::SetAttackRange(mHandles[24], 1000);
        MissionUtility::SetAttackRange(mHandles[25], 1000);
        MissionUtility::SetAttackRange(mHandles[26], 1000);
        MissionUtility::SetAttackRange(mHandles[27], 1000);
        MissionUtility::SetAttackRange(mHandles[28], 1000);
        MissionUtility::SetAttackRange(mHandles[29], 1000);
        MissionUtility::SetAttackRange(mHandles[30], 1000);
        MissionUtility::SetAttackRange(mHandles[31], 1000);
        MissionUtility::SetAttackRange(mHandles[32], 1000);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[28]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[30]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[32]);
        mHandles[71] = MissionUtility::CreateFlock(mHandles[28], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[71], mHandles[30]);
        MissionUtility::AddSquadMember(mHandles[71], mHandles[32]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[27]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[29]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[31]);
        mHandles[72] = MissionUtility::CreateFlock(mHandles[27], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[71], mHandles[29]);
        MissionUtility::AddSquadMember(mHandles[71], mHandles[31]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[22]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[24]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[26]);
        mHandles[69] = MissionUtility::CreateFlock(mHandles[22], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[69], mHandles[24]);
        MissionUtility::AddSquadMember(mHandles[69], mHandles[26]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[21]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[23]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[25]);
        mHandles[70] = MissionUtility::CreateFlock(mHandles[21], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[70], mHandles[23]);
        MissionUtility::AddSquadMember(mHandles[70], mHandles[25]);
        MissionUtility::RemoveObjectify("ambush_point", 0);
        MissionUtility::RemoveObjectify(mHandles[19]);
        MissionUtility::Objectify(mHandles[16], "missions.Raxus2.marker.str0003", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[17], "missions.Raxus2.marker.str0003", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[18], "missions.Raxus2.marker.str0003", false, true, 0.0f);
        MissionUtility::AddHealthBar(mHandles[16], "Enemy Transport", 400.0f);
        MissionUtility::AddHealthBar(mHandles[17], "Enemy Transport", 400.0f);
        MissionUtility::AddHealthBar(mHandles[18], "Enemy Transport", 400.0f);
        mFlags[124] = true;
        mFlags[50] = true;
        }
        }
        }
        if (mFlags[124]) {
        if (!mFlags[14]) {
        mHandles[6] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "carrier1_spawn", 0, "carrier1", 0, -1, -1);
        mHandles[147] = MissionUtility::CreateObject("cis_tank_fighter", "carrier1_spawn", 0, "punisher1", 2, -1);
        mHandles[148] = MissionUtility::CreateObject("cis_tank_fighter", "carrier1_spawn", 0, "punisher2", 2, -1);
        mHandles[149] = MissionUtility::CreateObject("cis_tank_fighter", "carrier1_spawn", 0, "punisher3", 2, -1);
        MissionUtility::CarrierAddCargo(mHandles[6], "hp_link_1", mHandles[147], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[6], "hp_link_2", mHandles[148], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[6], "hp_link_3", mHandles[149], "hp_link_1", true);
        MissionUtility::SetVelocVertical(mHandles[6], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[6], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[6], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[6], 250.0f);
        MissionUtility::CarrierDropoff(mHandles[6], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[6], "carriers_go", true);
        MissionUtility::SetQueueFlag(false);
        mHandles[7] = MissionUtility::CreateObjectWithRotation("cis_fly_vcarrier", "carrier2_spawn", 0, "carrier2", 0, -1, -1);
        mHandles[150] = MissionUtility::CreateObject("cis_tank_fighter", "carrier1_spawn", 0, "punisher5", 2, -1);
        mHandles[151] = MissionUtility::CreateObject("cis_tank_fighter", "carrier1_spawn", 0, "punisher6", 2, -1);
        mHandles[152] = MissionUtility::CreateObject("cis_tank_fighter", "carrier1_spawn", 0, "punisher7", 2, -1);
        MissionUtility::CarrierAddCargo(mHandles[7], "hp_link_1", mHandles[150], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[7], "hp_link_2", mHandles[151], "hp_link_1", true);
        MissionUtility::CarrierAddCargo(mHandles[7], "hp_link_3", mHandles[152], "hp_link_1", true);
        MissionUtility::SetVelocVertical(mHandles[7], 50.0f);
        MissionUtility::SetVelocVerticalFly(mHandles[7], 50.0f);
        MissionUtility::SetMaxAltitude(mHandles[7], 800.0f);
        MissionUtility::SetTakeoffAltitude(mHandles[7], 250.0f);
        MissionUtility::CarrierDropoff(mHandles[7], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[7], "carriers_go", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[14] = true;
        }
        if (!mFlags[9]) {
        if (mFlags[14]) {
        if (MissionUtility::GetDistance(mHandles[6], "carriers_go") < 100.0f) {
        MissionUtility::RemoveObject(mHandles[6]);
        mFlags[9] = true;
        }
        }
        }
        if (!mFlags[10]) {
        if (mFlags[14]) {
        if (MissionUtility::GetDistance(mHandles[7], "carriers_go") < 100.0f) {
        MissionUtility::RemoveObject(mHandles[7]);
        mFlags[10] = true;
        }
        }
        }
        }
        if (!mFlags[51]) {
        if (!mFlags[50]) {
        if (MissionUtility::IsInsideRegion(mHandles[19], "strike_region")
            || MissionUtility::IsInsideRegion(mHandles[20], "strike_region")
            || MissionUtility::IsInsideRegion(mHandles[16], "strike_region")) {
        MissionUtility::PlayMusic("EP1_V1_T05", true);
        MissionUtility::QueueSound("ASR09_11", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetTeamNum(mHandles[2], 1);
        MissionUtility::SetTeamNum(mHandles[3], 1);
        MissionUtility::SetAttackRange(mHandles[19], 1000);
        MissionUtility::SetAttackRange(mHandles[20], 1000);
        MissionUtility::SetAttackRange(mHandles[16], 1000);
        MissionUtility::SetAttackRange(mHandles[17], 1000);
        MissionUtility::SetAttackRange(mHandles[18], 1000);
        MissionUtility::SetAttackRange(mHandles[21], 1000);
        MissionUtility::SetAttackRange(mHandles[22], 1000);
        MissionUtility::SetAttackRange(mHandles[23], 1000);
        MissionUtility::SetAttackRange(mHandles[24], 1000);
        MissionUtility::SetAttackRange(mHandles[25], 1000);
        MissionUtility::SetAttackRange(mHandles[26], 1000);
        MissionUtility::SetAttackRange(mHandles[27], 1000);
        MissionUtility::SetAttackRange(mHandles[28], 1000);
        MissionUtility::SetAttackRange(mHandles[29], 1000);
        MissionUtility::SetAttackRange(mHandles[30], 1000);
        MissionUtility::SetAttackRange(mHandles[31], 1000);
        MissionUtility::SetAttackRange(mHandles[32], 1000);
        mFloatsB[14] = 10.0f + MissionUtility::GetTime();
        MissionUtility::RemoveObjectify("ambush_point", 0);
        MissionUtility::RemoveObjectify(mHandles[19]);
        MissionUtility::Objectify(mHandles[16], "missions.Raxus2.marker.str0003", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[17], "missions.Raxus2.marker.str0003", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[18], "missions.Raxus2.marker.str0003", false, true, 0.0f);
        MissionUtility::AddHealthBar(mHandles[16], "Enemy Transport", 400.0f);
        MissionUtility::AddHealthBar(mHandles[17], "Enemy Transport", 400.0f);
        MissionUtility::AddHealthBar(mHandles[18], "Enemy Transport", 400.0f);
        MissionUtility::SetVelocForward(mHandles[15], 30.0f);
        mFlags[51] = true;
        }
        }
        }
        if (!mFlags[38]) {
        if (mFloatsB[14] < MissionUtility::GetTime()) {
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[28]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[30]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[32]);
        mHandles[71] = MissionUtility::CreateFlock(mHandles[28], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[71], mHandles[30]);
        MissionUtility::AddSquadMember(mHandles[71], mHandles[32]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[27]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[29]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[31]);
        mHandles[72] = MissionUtility::CreateFlock(mHandles[27], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[71], mHandles[29]);
        MissionUtility::AddSquadMember(mHandles[71], mHandles[31]);
        mFloatsB[7] = 5.0f + MissionUtility::GetTime();
        MissionUtility::QueueSound("ASR09_13A", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[38] = true;
        }
        }
        if (!mFlags[39]) {
        if (mFloatsB[7] < MissionUtility::GetTime()) {
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[22]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[24]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[26]);
        mHandles[69] = MissionUtility::CreateFlock(mHandles[22], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[69], mHandles[24]);
        MissionUtility::AddSquadMember(mHandles[69], mHandles[26]);
        mFloatsB[8] = 10.0f + MissionUtility::GetTime();
        mFlags[39] = true;
        }
        }
        if (!mFlags[40]) {
        if (mFloatsB[8] < MissionUtility::GetTime()) {
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[21]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[23]);
        MissionUtility::DisbandSquadMember(mHandles[15], mHandles[25]);
        mHandles[70] = MissionUtility::CreateFlock(mHandles[21], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[70], mHandles[23]);
        MissionUtility::AddSquadMember(mHandles[70], mHandles[25]);
        mFlags[40] = true;
        }
        }
        if (!mFlags[56]) {
        if (mFlags[18]) {
        if (!mFlags[3]) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        MissionUtility::BonusObjectiveFailed(mHandles[94]);
        mFloatsB[15] = 3.0f + MissionUtility::GetTime();
        mFlags[56] = true;
        }
        }
        }
        }
        if (!mFlags[3]) {
        if (mFlags[11]) {
        if (!MissionUtility::IsAlive(mHandles[16])) {
        if (!MissionUtility::IsAlive(mHandles[17])) {
        if (!MissionUtility::IsAlive(mHandles[18])) {
        if (MissionUtility::CountUnitsNearObject(mHandles[0], 300.0f, 2, 0) == 0) {
        mFloatsB[1] = 1.0f + MissionUtility::GetTime();
        MissionUtility::ObjectiveComplete(mHandles[94]);
        MissionUtility::MidMissionSavePlayer(1);
        MissionUtility::MidMissionSave((bool)mFlags[41]);
        MissionUtility::MidMissionSave((bool)mFlags[42]);
        MissionUtility::MidMissionSave(mTimer1);
        MissionUtility::MidMissionSave((bool)mFlags[29]);
        MissionUtility::MidMissionSave((bool)mFlags[30]);
        MissionUtility::MidMissionSave((bool)mFlags[31]);
        mFlags[75] = true;
        mFlags[3] = true;
        }
        }
        }
        }
        }
        }
    }

    // ---- +0x36a0  148 bytes ----
    if (!mFlags[71]) {
        if (MissionUtility::IsInsideRegion(mHandles[16], "convoy2_fail_spot")
            || MissionUtility::IsInsideRegion(mHandles[17], "convoy2_fail_spot")
            || MissionUtility::IsInsideRegion(mHandles[18], "convoy2_fail_spot")) {
        MissionUtility::QueueSound("ASR09_13", 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[82] = MissionUtility::RunCin("convoy2_cin", true, true);
        MissionUtility::BonusObjectiveFailed(mHandles[94]);
        mFlags[71] = true;
        BeginTimer(mTimer9);
        }
    }

    // ---- +0x3734  100 bytes ----
    if (mFlags[71]) {
        if (mTimer9 > 2.0f) {
        StopTimer(mTimer9);
        mTimer9 = 0.0f;
        MissionUtility::SetVisible(mHandles[146], false);
        MissionUtility::SetCollidable(mHandles[146], false);
        MissionUtility::SetVisible(mHandles[145], false);
        MissionUtility::SetCollidable(mHandles[145], false);
        }
    }

    // ---- +0x3798  68 bytes ----
    if (!mFlags[72] && mFlags[71]) {
        if (!MissionUtility::IsCinRunning(mHandles[82])) {
        MissionUtility::FlushSoundQueue();
        mFloatsB[6] = 0.01f + MissionUtility::GetTime();
        mFlags[72] = true;
        }
    }

    // ---- +0x37dc  14072 bytes ----
    if (mFlags[75]) {
        if (!mFlags[20]) {
        if (mFloatsB[1] < MissionUtility::GetTime()) {
        if (!mFlags[21]) {
        MissionUtility::PlayMusic("EP5_V1_T06_02", true);
        StopTimer(mTimer1);
        MissionUtility::SetAlliance(1, 2);
        MissionUtility::SetAlliance(1, 3);
        if (MissionUtility::IsInsideRegion(mHandles[0], "split_cin_bad_place")) {
        MissionUtility::MoveObject(mHandles[0], "move_player_here", 0, true);
        MissionUtility::MoveObject(mHandles[2], "move_squadmate1_here", 0, true);
        MissionUtility::MoveObject(mHandles[3], "move_squadmate2_here", 0, true);
        }
        if (MissionUtility::IsInsideRegion(mHandles[2], "split_cin_bad_place")) {
        MissionUtility::MoveObject(mHandles[2], "move_squadmate1_here", 0, true);
        }
        if (MissionUtility::IsInsideRegion(mHandles[3], "split_cin_bad_place")) {
        MissionUtility::MoveObject(mHandles[3], "move_squadmate2_here", 0, true);
        }
        mHandles[34] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy2_pattern", 4, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[35] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy2_pattern", 9, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[36] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy2_pattern", 14, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[37] = MissionUtility::CreateObject("cis_tank_assault", "convoy2_pattern", 0, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[38] = MissionUtility::CreateObject("cis_tank_assault", "convoy2_pattern", 1, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[39] = MissionUtility::CreateObject("cis_tank_assault", "convoy2_pattern", 3, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[40] = MissionUtility::CreateObject("cis_tank_assault", "convoy2_pattern", 5, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[41] = MissionUtility::CreateObject("cis_tank_assault", "convoy2_pattern", 8, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[42] = MissionUtility::CreateObject("cis_tank_assault", "convoy2_pattern", 10, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[43] = MissionUtility::CreateObject("cis_tank_assault", "convoy2_pattern", 13, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[44] = MissionUtility::CreateObject("cis_tank_assault", "convoy2_pattern", 15, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[45] = MissionUtility::CreateObject("cis_bike_speeder", "convoy2_pattern", 2, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[46] = MissionUtility::CreateObject("cis_bike_speeder", "convoy2_pattern", 6, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[47] = MissionUtility::CreateObject("cis_bike_speeder", "convoy2_pattern", 7, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[48] = MissionUtility::CreateObject("cis_bike_speeder", "convoy2_pattern", 11, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[49] = MissionUtility::CreateObject("cis_bike_speeder", "convoy2_pattern", 12, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[50] = MissionUtility::CreateObject("cis_bike_speeder", "convoy2_pattern", 16, "", 3, -1, Quat(0.707107f, 0.0f, 0.707107f, 0.0f), -1);
        mHandles[33] = MissionUtility::CreateFlock(mHandles[34], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[33], mHandles[37], Vector(-14.0f, 0.0f, -44.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[38], Vector(14.0f, 0.0f, -44.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[39], Vector(-33.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[40], Vector(33.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[45], Vector(-51.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[46], Vector(51.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[35], Vector(0.0f, 0.0f, 60.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[41], Vector(-33.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[42], Vector(33.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[47], Vector(-51.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[48], Vector(51.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[36], Vector(0.0f, 0.0f, 130.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[43], Vector(-33.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[44], Vector(33.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[49], Vector(-51.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[33], mHandles[50], Vector(51.0f, 0.0f, 140.0f));
        mHandles[52] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy3_pattern", 4, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[53] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy3_pattern", 9, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[54] = MissionUtility::CreateObject("cis_tank_gtrans", "convoy3_pattern", 14, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[55] = MissionUtility::CreateObject("cis_tank_assault", "convoy3_pattern", 0, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[56] = MissionUtility::CreateObject("cis_tank_assault", "convoy3_pattern", 1, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[57] = MissionUtility::CreateObject("cis_tank_assault", "convoy3_pattern", 3, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[58] = MissionUtility::CreateObject("cis_tank_assault", "convoy3_pattern", 5, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[59] = MissionUtility::CreateObject("cis_tank_assault", "convoy3_pattern", 8, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[60] = MissionUtility::CreateObject("cis_tank_assault", "convoy3_pattern", 10, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[61] = MissionUtility::CreateObject("cis_tank_assault", "convoy3_pattern", 13, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[62] = MissionUtility::CreateObject("cis_tank_assault", "convoy3_pattern", 15, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[63] = MissionUtility::CreateObject("cis_bike_speeder", "convoy3_pattern", 2, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[64] = MissionUtility::CreateObject("cis_bike_speeder", "convoy3_pattern", 6, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[65] = MissionUtility::CreateObject("cis_bike_speeder", "convoy3_pattern", 7, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[66] = MissionUtility::CreateObject("cis_bike_speeder", "convoy3_pattern", 11, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[67] = MissionUtility::CreateObject("cis_bike_speeder", "convoy3_pattern", 12, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[68] = MissionUtility::CreateObject("cis_bike_speeder", "convoy3_pattern", 16, "", 4, -1, Quat(-0.120507f, 0.0f, -0.992712f, 0.0f), -1);
        mHandles[51] = MissionUtility::CreateFlock(mHandles[52], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[51], mHandles[55], Vector(-14.0f, 0.0f, -44.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[56], Vector(14.0f, 0.0f, -44.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[57], Vector(-33.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[58], Vector(33.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[63], Vector(-51.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[64], Vector(51.0f, 0.0f, -5.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[53], Vector(0.0f, 0.0f, 60.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[59], Vector(-33.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[60], Vector(33.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[65], Vector(-51.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[66], Vector(51.0f, 0.0f, 70.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[54], Vector(0.0f, 0.0f, 130.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[61], Vector(-33.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[62], Vector(33.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[67], Vector(-51.0f, 0.0f, 140.0f));
        MissionUtility::AddFlockMember(mHandles[51], mHandles[68], Vector(51.0f, 0.0f, 140.0f));
        MissionUtility::SetVelocForward(mHandles[33], 22.0f);
        MissionUtility::SetVelocForward(mHandles[51], 22.0f);
        MissionUtility::Goto(mHandles[33], "convoy2_path_cin", true);
        MissionUtility::Goto(mHandles[51], "convoy3_path_cin", true);
        MissionUtility::SetFogRange(200.0f, 1800.0f, 2.0f);
        mHandles[83] = MissionUtility::RunCin("split_cin", true, true);
        mFloatsB[22] = 1.0f + MissionUtility::GetTime();
        mFlags[21] = true;
        }
        if (!mFlags[73]) {
        if (mFloatsB[22] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("ASR09_14", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("OBR09_21A", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[73] = true;
        }
        }
        if (mFlags[21]) {
        if (!MissionUtility::IsCinRunning(mHandles[83])) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::MoveObject(mHandles[34], "convoy2_move_path", 4, true);
        MissionUtility::MoveObject(mHandles[35], "convoy2_move_path", 9, true);
        MissionUtility::MoveObject(mHandles[36], "convoy2_move_path", 14, true);
        MissionUtility::MoveObject(mHandles[37], "convoy2_move_path", 0, true);
        MissionUtility::MoveObject(mHandles[38], "convoy2_move_path", 1, true);
        MissionUtility::MoveObject(mHandles[39], "convoy2_move_path", 3, true);
        MissionUtility::MoveObject(mHandles[40], "convoy2_move_path", 5, true);
        MissionUtility::MoveObject(mHandles[41], "convoy2_move_path", 8, true);
        MissionUtility::MoveObject(mHandles[42], "convoy2_move_path", 10, true);
        MissionUtility::MoveObject(mHandles[43], "convoy2_move_path", 13, true);
        MissionUtility::MoveObject(mHandles[44], "convoy2_move_path", 15, true);
        MissionUtility::MoveObject(mHandles[45], "convoy2_move_path", 2, true);
        MissionUtility::MoveObject(mHandles[46], "convoy2_move_path", 6, true);
        MissionUtility::MoveObject(mHandles[47], "convoy2_move_path", 7, true);
        MissionUtility::MoveObject(mHandles[48], "convoy2_move_path", 11, true);
        MissionUtility::MoveObject(mHandles[49], "convoy2_move_path", 12, true);
        MissionUtility::MoveObject(mHandles[50], "convoy2_move_path", 16, true);
        MissionUtility::MoveObject(mHandles[52], "convoy3_move_path", 4, true);
        MissionUtility::MoveObject(mHandles[53], "convoy3_move_path", 9, true);
        MissionUtility::MoveObject(mHandles[54], "convoy3_move_path", 14, true);
        MissionUtility::MoveObject(mHandles[55], "convoy3_move_path", 0, true);
        MissionUtility::MoveObject(mHandles[56], "convoy3_move_path", 1, true);
        MissionUtility::MoveObject(mHandles[57], "convoy3_move_path", 3, true);
        MissionUtility::MoveObject(mHandles[58], "convoy3_move_path", 5, true);
        MissionUtility::MoveObject(mHandles[59], "convoy3_move_path", 8, true);
        MissionUtility::MoveObject(mHandles[60], "convoy3_move_path", 10, true);
        MissionUtility::MoveObject(mHandles[61], "convoy3_move_path", 13, true);
        MissionUtility::MoveObject(mHandles[62], "convoy3_move_path", 15, true);
        MissionUtility::MoveObject(mHandles[63], "convoy3_move_path", 2, true);
        MissionUtility::MoveObject(mHandles[64], "convoy3_move_path", 6, true);
        MissionUtility::MoveObject(mHandles[65], "convoy3_move_path", 7, true);
        MissionUtility::MoveObject(mHandles[66], "convoy3_move_path", 11, true);
        MissionUtility::MoveObject(mHandles[67], "convoy3_move_path", 12, true);
        MissionUtility::MoveObject(mHandles[68], "convoy3_move_path", 16, true);
        MissionUtility::Goto(mHandles[33], "convoy2_path", true);
        MissionUtility::Goto(mHandles[51], "convoy3_path", true);
        MissionUtility::Objectify(mHandles[34], "missions.Raxus2.marker.str0004", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[52], "missions.Raxus2.marker.str0004", false, true, 0.0f);
        MissionUtility::DisplayText("missions.Raxus2.text.str0002", 6.0f, -1.0f);
        mHandles[95] = MissionUtility::AddObjective("missions.Raxus2.objective.str0002");
        if (MissionUtility::IsSoundPlaying(mHandles[135])) {
        MissionUtility::StopSound(mHandles[135]);
        }
        MissionUtility::SetEnemies(1, 2);
        MissionUtility::SetEnemies(1, 3);
        MissionUtility::SetOriginalFog(0.1f);
        ResumeTimer(mTimer1);
        MissionUtility::PlayMusic("EP1_V1_T12", true);
        mFlags[20] = true;
        }
        }
        }
        }
        if (!mFlags[41]) {
        if (!MissionUtility::IsAlive(mHandles[2])) {
        mFlags[41] = true;
        }
        }
        if (!mFlags[42]) {
        if (!MissionUtility::IsAlive(mHandles[3])) {
        mFlags[42] = true;
        }
        }
        if (!mFlags[36]) {
        if (mFlags[21]) {
        if (MissionUtility::GetDistance(mHandles[0], mHandles[33]) < 400.0f) {
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[46]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[48]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[50]);
        mHandles[75] = MissionUtility::CreateFlock(mHandles[46], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[75], mHandles[48]);
        MissionUtility::AddSquadMember(mHandles[75], mHandles[50]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[45]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[47]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[49]);
        mHandles[76] = MissionUtility::CreateFlock(mHandles[45], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[75], mHandles[47]);
        MissionUtility::AddSquadMember(mHandles[75], mHandles[49]);
        MissionUtility::RemoveObjectify(mHandles[34]);
        MissionUtility::Objectify(mHandles[34], "missions.Raxus2.marker.str0005", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[35], "missions.Raxus2.marker.str0005", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[36], "missions.Raxus2.marker.str0005", false, true, 0.0f);
        MissionUtility::AddHealthBar(mHandles[34], "Enemy Transport", 400.0f);
        MissionUtility::AddHealthBar(mHandles[35], "Enemy Transport", 400.0f);
        MissionUtility::AddHealthBar(mHandles[36], "Enemy Transport", 400.0f);
        mFloatsB[9] = 3.0f + MissionUtility::GetTime();
        mFlags[36] = true;
        }
        }
        }
        if (!mFlags[43]) {
        if (mFloatsB[9] < MissionUtility::GetTime()) {
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[40]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[42]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[44]);
        mHandles[73] = MissionUtility::CreateFlock(mHandles[40], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[73], mHandles[42]);
        MissionUtility::AddSquadMember(mHandles[73], mHandles[44]);
        mFloatsB[10] = 3.0f + MissionUtility::GetTime();
        mFlags[43] = true;
        }
        }
        if (!mFlags[44]) {
        if (mFloatsB[10] < MissionUtility::GetTime()) {
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[39]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[41]);
        MissionUtility::DisbandSquadMember(mHandles[33], mHandles[43]);
        mHandles[74] = MissionUtility::CreateFlock(mHandles[39], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[74], mHandles[41]);
        MissionUtility::AddSquadMember(mHandles[74], mHandles[43]);
        mFlags[44] = true;
        }
        }
        if (!mFlags[37]) {
        if (mFlags[21]) {
        if (MissionUtility::GetDistance(mHandles[0], mHandles[51]) < 400.0f) {
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[64]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[66]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[68]);
        mHandles[79] = MissionUtility::CreateFlock(mHandles[64], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[79], mHandles[66]);
        MissionUtility::AddSquadMember(mHandles[79], mHandles[68]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[63]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[65]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[67]);
        mHandles[80] = MissionUtility::CreateFlock(mHandles[63], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[79], mHandles[65]);
        MissionUtility::AddSquadMember(mHandles[79], mHandles[67]);
        MissionUtility::RemoveObjectify(mHandles[52]);
        MissionUtility::Objectify(mHandles[52], "missions.Raxus2.marker.str0005", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[53], "missions.Raxus2.marker.str0005", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[54], "missions.Raxus2.marker.str0005", false, true, 0.0f);
        MissionUtility::AddHealthBar(mHandles[52], "Enemy Transport", 400.0f);
        MissionUtility::AddHealthBar(mHandles[53], "Enemy Transport", 400.0f);
        MissionUtility::AddHealthBar(mHandles[54], "Enemy Transport", 400.0f);
        mFloatsB[11] = 3.0f + MissionUtility::GetTime();
        mFlags[37] = true;
        }
        }
        }
        if (!mFlags[45]) {
        if (mFloatsB[11] < MissionUtility::GetTime()) {
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[58]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[60]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[62]);
        mHandles[77] = MissionUtility::CreateFlock(mHandles[58], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[77], mHandles[60]);
        MissionUtility::AddSquadMember(mHandles[77], mHandles[62]);
        mFloatsB[12] = 3.0f + MissionUtility::GetTime();
        mFlags[45] = true;
        }
        }
        if (!mFlags[46]) {
        if (mFloatsB[12] < MissionUtility::GetTime()) {
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[57]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[59]);
        MissionUtility::DisbandSquadMember(mHandles[51], mHandles[61]);
        mHandles[78] = MissionUtility::CreateFlock(mHandles[57], (Formation)1);
        MissionUtility::AddSquadMember(mHandles[78], mHandles[59]);
        MissionUtility::AddSquadMember(mHandles[78], mHandles[61]);
        mFlags[46] = true;
        }
        }
        if (!mFlags[96]) {
        if (MissionUtility::IsAlive(mHandles[37])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[37])) {
        MissionUtility::AttackTarget(mHandles[37], mHandles[0], true, true, false, false);
        mFlags[96] = true;
        }
        }
        }
        if (!mFlags[97]) {
        if (MissionUtility::IsAlive(mHandles[38])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[38])) {
        MissionUtility::AttackTarget(mHandles[38], mHandles[0], true, true, false, false);
        mFlags[97] = true;
        }
        }
        }
        if (!mFlags[98]) {
        if (MissionUtility::IsAlive(mHandles[39])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[39])) {
        MissionUtility::AttackTarget(mHandles[39], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[39], 35.0f);
        mFlags[98] = true;
        }
        }
        }
        if (!mFlags[99]) {
        if (MissionUtility::IsAlive(mHandles[40])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[40])) {
        MissionUtility::AttackTarget(mHandles[40], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[40], 35.0f);
        mFlags[99] = true;
        }
        }
        }
        if (!mFlags[100]) {
        if (MissionUtility::IsAlive(mHandles[41])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[41])) {
        MissionUtility::AttackTarget(mHandles[41], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[41], 35.0f);
        mFlags[100] = true;
        }
        }
        }
        if (!mFlags[101]) {
        if (MissionUtility::IsAlive(mHandles[42])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[42])) {
        MissionUtility::AttackTarget(mHandles[42], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[42], 35.0f);
        mFlags[101] = true;
        }
        }
        }
        if (!mFlags[102]) {
        if (MissionUtility::IsAlive(mHandles[43])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[43])) {
        MissionUtility::AttackTarget(mHandles[43], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[43], 35.0f);
        mFlags[102] = true;
        }
        }
        }
        if (!mFlags[103]) {
        if (MissionUtility::IsAlive(mHandles[44])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[44])) {
        MissionUtility::AttackTarget(mHandles[44], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[44], 35.0f);
        mFlags[103] = true;
        }
        }
        }
        if (!mFlags[82]) {
        if (MissionUtility::IsAlive(mHandles[55])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[55])) {
        MissionUtility::AttackTarget(mHandles[55], mHandles[0], true, true, false, false);
        mFlags[82] = true;
        }
        }
        }
        if (!mFlags[83]) {
        if (MissionUtility::IsAlive(mHandles[56])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[56])) {
        MissionUtility::AttackTarget(mHandles[56], mHandles[0], true, true, false, false);
        mFlags[83] = true;
        }
        }
        }
        if (!mFlags[84]) {
        if (MissionUtility::IsAlive(mHandles[57])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[57])) {
        MissionUtility::AttackTarget(mHandles[57], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[57], 35.0f);
        mFlags[84] = true;
        }
        }
        }
        if (!mFlags[85]) {
        if (MissionUtility::IsAlive(mHandles[58])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[58])) {
        MissionUtility::AttackTarget(mHandles[58], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[58], 35.0f);
        mFlags[85] = true;
        }
        }
        }
        if (!mFlags[86]) {
        if (MissionUtility::IsAlive(mHandles[59])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[59])) {
        MissionUtility::AttackTarget(mHandles[59], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[59], 35.0f);
        mFlags[86] = true;
        }
        }
        }
        if (!mFlags[87]) {
        if (MissionUtility::IsAlive(mHandles[60])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[60])) {
        MissionUtility::AttackTarget(mHandles[60], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[60], 35.0f);
        mFlags[87] = true;
        }
        }
        }
        if (!mFlags[88]) {
        if (MissionUtility::IsAlive(mHandles[61])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[61])) {
        MissionUtility::AttackTarget(mHandles[61], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[61], 35.0f);
        mFlags[88] = true;
        }
        }
        }
        if (!mFlags[89]) {
        if (MissionUtility::IsAlive(mHandles[62])) {
        if (mHandles[0] == MissionUtility::GetWhoShotMe(mHandles[62])) {
        MissionUtility::AttackTarget(mHandles[62], mHandles[0], true, true, false, false);
        MissionUtility::SetVelocForward(mHandles[62], 35.0f);
        mFlags[89] = true;
        }
        }
        }
        if (!mFlags[33]) {
        if (mFlags[20]) {
        if (MissionUtility::GetDistance(mHandles[33], "convoy2_warn_spot") < 600.0f
            || MissionUtility::GetDistance(mHandles[51], "convoy3_warn_spot") < 600.0f) {
        MissionUtility::QueueSound("ASR09_12", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[33] = true;
        }
        }
        }
        if (!mFlags[12]) {
        if (!mFlags[13]) {
        if (MissionUtility::IsInsideRegion(mHandles[34], "convoy2_fail_spot")
            || MissionUtility::IsInsideRegion(mHandles[35], "convoy2_fail_spot")
            || MissionUtility::IsInsideRegion(mHandles[36], "convoy2_fail_spot")) {
        MissionUtility::QueueSound("ASR09_13", 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[142] = MissionUtility::RunCin("convoy2_cin", true, true);
        MissionUtility::BonusObjectiveFailed(mHandles[95]);
        mFlags[12] = true;
        BeginTimer(mTimer9);
        }
        }
        }
        if (mFlags[12]) {
        if (mTimer9 > 2.0f) {
        StopTimer(mTimer9);
        mTimer9 = 0.0f;
        MissionUtility::SetVisible(mHandles[146], false);
        MissionUtility::SetCollidable(mHandles[146], false);
        MissionUtility::SetVisible(mHandles[145], false);
        MissionUtility::SetCollidable(mHandles[145], false);
        }
        }
        if (!mFlags[69]) {
        if (mFlags[12]) {
        if (!MissionUtility::IsCinRunning(mHandles[142])) {
        MissionUtility::FlushSoundQueue();
        mFloatsB[6] = 0.01f + MissionUtility::GetTime();
        mFlags[69] = true;
        }
        }
        }
        if (!mFlags[13]) {
        if (!mFlags[12]) {
        if (MissionUtility::IsInsideRegion(mHandles[52], "convoy3_fail_spot")
            || MissionUtility::IsInsideRegion(mHandles[53], "convoy3_fail_spot")
            || MissionUtility::IsInsideRegion(mHandles[54], "convoy3_fail_spot")) {
        MissionUtility::QueueSound("ASR09_13", 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[143] = MissionUtility::RunCin("convoy3_cin", true, true);
        MissionUtility::BonusObjectiveFailed(mHandles[95]);
        BeginTimer(mTimer9);
        mFlags[13] = true;
        }
        }
        }
        if (mFlags[13]) {
        if (mTimer9 > 2.0f) {
        StopTimer(mTimer9);
        mTimer9 = 0.0f;
        MissionUtility::SetVisible(mHandles[146], false);
        MissionUtility::SetCollidable(mHandles[146], false);
        MissionUtility::SetVisible(mHandles[145], false);
        MissionUtility::SetCollidable(mHandles[145], false);
        }
        }
        if (!mFlags[70]) {
        if (mFlags[13]) {
        if (!MissionUtility::IsCinRunning(mHandles[143])) {
        MissionUtility::FlushSoundQueue();
        mFloatsB[6] = 0.01f + MissionUtility::GetTime();
        mFlags[70] = true;
        }
        }
        }
        if (!mFlags[5]) {
        if (mFlags[20]) {
        if (!MissionUtility::IsAlive(mHandles[34])) {
        if (!MissionUtility::IsAlive(mHandles[35])) {
        if (!MissionUtility::IsAlive(mHandles[36])) {
        mInts[0] = mInts[0] + 1;
        mFlags[5] = true;
        }
        }
        }
        }
        }
        if (!mFlags[6]) {
        if (mFlags[20]) {
        if (!MissionUtility::IsAlive(mHandles[52])) {
        if (!MissionUtility::IsAlive(mHandles[53])) {
        if (!MissionUtility::IsAlive(mHandles[54])) {
        mInts[0] = mInts[0] + 1;
        mFlags[6] = true;
        }
        }
        }
        }
        }
        if (!mFlags[12]) {
        if (!mFlags[13]) {
        if (!mFlags[7]) {
        if (mInts[0] == 1) {
        MissionUtility::DisplayText("missions.Raxus2.text.str0003", 6.0f, -1.0f);
        MissionUtility::QueueSound("ASR09_35", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[7] = true;
        }
        }
        if (!mFlags[57]) {
        if (mFlags[20]) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        MissionUtility::BonusObjectiveFailed(mHandles[95]);
        mFloatsB[15] = 3.0f + MissionUtility::GetTime();
        mFlags[57] = true;
        }
        }
        }
        if (!mFlags[8]) {
        if (mInts[0] == 2) {
        MissionUtility::ObjectiveComplete(mHandles[95]);
        mFloatsB[2] = 1.0f + MissionUtility::GetTime();
        mFlags[8] = true;
        }
        }
        if (mFloatsB[20] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("OBR09_16", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFloatsB[20] = 999999.9f;
        }
        }
        }
        if (!mFlags[22]) {
        if (mFloatsB[2] < MissionUtility::GetTime()) {
        if (!mFlags[23]) {
        MissionUtility::PlayMusic("EP2_V1_T13_01", true);
        MissionUtility::SetMaxHealth(mHandles[0], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[0], 999999.0f);
        MissionUtility::SetAlliance(1, 2);
        MissionUtility::SetAlliance(1, 3);
        mHandles[168] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_player", "EndCinTankPath", 0, "EndCinTank", 0, -1, -1);
        MissionUtility::SetVelocForward(mHandles[168], 150.0f);
        if (MissionUtility::IsAlive(mHandles[3])) {
        mHandles[169] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_player", "EndCinTankPath1", 0, "EndCinTank1", 0, -1, -1);
        MissionUtility::SetVelocForward(mHandles[169], 150.0f);
        MissionUtility::Goto(mHandles[169], "EndCinTankPath1", true);
        }
        if (MissionUtility::IsAlive(mHandles[3])) {
        mHandles[170] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_player", "EndCinTankPath2", 0, "EndCinTank2", 0, -1, -1);
        MissionUtility::SetVelocForward(mHandles[170], 150.0f);
        MissionUtility::Goto(mHandles[170], "EndCinTankPath2", true);
        }
        mHandles[171] = MissionUtility::CreateObjectWithRotation("rep_walk_assault", "EndCinWalkerPath", 0, "EndCinWalker", 0, -1, -1);
        mHandles[172] = MissionUtility::CreateObjectWithRotation("rep_walk_assault", "EndCinWalkerPath", 1, "EndCinWalker1", 0, -1, -1);
        mHandles[173] = MissionUtility::CreateObjectWithRotation("rep_walk_assault", "EndCinWalkerPath", 2, "EndCinWalker2", 0, -1, -1);
        mHandles[174] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinWalkerPath1", 0, "EndCinWalker3", 0, -1, -1);
        mHandles[175] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinWalkerPath1", 1, "EndCinWalker4", 0, -1, -1);
        mHandles[176] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinWalkerPath1", 2, "EndCinWalker5", 0, -1, -1);
        mHandles[219] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinSixLegPath", 0, "EndCinSixLeg", 0, -1, -1);
        mHandles[220] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinSixLegPath1", 0, "EndCinSixLeg1", 0, -1, -1);
        mHandles[177] = MissionUtility::CreateObjectWithRotation("rep_fly_assault", "EndCinTransportLand", 0, "EndCinTransport", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[171], true);
        MissionUtility::OverrideSoundRange(mHandles[172], true);
        MissionUtility::OverrideSoundRange(mHandles[173], true);
        MissionUtility::OverrideSoundRange(mHandles[174], true);
        MissionUtility::OverrideSoundRange(mHandles[175], true);
        MissionUtility::OverrideSoundRange(mHandles[176], true);
        MissionUtility::OverrideSoundRange(mHandles[219], true);
        MissionUtility::OverrideSoundRange(mHandles[220], true);
        MissionUtility::OverrideSoundRange(mHandles[177], true);
        MissionUtility::OverrideSoundRange(mHandles[168], true);
        MissionUtility::OverrideSoundRange(mHandles[169], true);
        MissionUtility::OverrideSoundRange(mHandles[170], true);
        MissionUtility::SetApplyDynamics(mHandles[177], true);
        MissionUtility::Land(mHandles[177], 0, 0, 80.0f);
        mHandles[179] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath", 0, "EndCinClone", 0, -1, -1);
        mHandles[180] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath1", 0, "EndCinClone1", 0, -1, -1);
        mHandles[181] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath2", 0, "EndCinClone2", 0, -1, -1);
        mHandles[182] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath3", 0, "EndCinClone3", 0, -1, -1);
        mHandles[183] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath4", 0, "EndCinClone4", 0, -1, -1);
        mHandles[184] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath5", 0, "EndCinClone5", 0, -1, -1);
        mHandles[185] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath6", 0, "EndCinClone6", 0, -1, -1);
        mHandles[186] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath7", 0, "EndCinClone7", 0, -1, -1);
        mHandles[187] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath8", 0, "EndCinClone8", 0, -1, -1);
        mHandles[188] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath9", 0, "EndCinClone9", 0, -1, -1);
        MissionUtility::Goto(mHandles[168], "EndCinTankPath", true);
        MissionUtility::Goto(mHandles[171], "EndCinWalkerPath", false);
        MissionUtility::Goto(mHandles[172], "EndCinWalkerPath", false);
        MissionUtility::Goto(mHandles[173], "EndCinWalkerPath", false);
        MissionUtility::Goto(mHandles[174], "EndCinWalkerPath1", false);
        MissionUtility::Goto(mHandles[175], "EndCinWalkerPath1", false);
        MissionUtility::Goto(mHandles[176], "EndCinWalkerPath1", false);
        MissionUtility::Goto(mHandles[219], "EndCinSixLegPath", false);
        MissionUtility::Goto(mHandles[220], "EndCinSixLegPath1", false);
        MissionUtility::Goto(mHandles[179], "EndCinClonePath", true);
        MissionUtility::Goto(mHandles[180], "EndCinClonePath1", true);
        MissionUtility::Goto(mHandles[181], "EndCinClonePath2", true);
        MissionUtility::Goto(mHandles[182], "EndCinClonePath3", true);
        MissionUtility::Goto(mHandles[183], "EndCinClonePath4", true);
        MissionUtility::Goto(mHandles[184], "EndCinClonePath5", true);
        MissionUtility::Goto(mHandles[185], "EndCinClonePath6", true);
        MissionUtility::Goto(mHandles[186], "EndCinClonePath7", true);
        MissionUtility::Goto(mHandles[187], "EndCinClonePath8", true);
        MissionUtility::Goto(mHandles[188], "EndCinClonePath9", true);
        BeginTimer(mTimer6);
        MissionUtility::SetFogRange(200.0f, 1800.0f, 2.0f);
        mHandles[84] = MissionUtility::RunCin("end_cin", true, true);
        mFloatsB[3] = 2.0f + MissionUtility::GetTime();
        mFlags[23] = true;
        }
        if (mTimer8 > 2.0f) {
        if (mInts[2] < 4) {
        mTimer8 = 0.0f;
        mInts[2] = mInts[2] + 1;
        switch (mInts[2]) {
        case 1:
        mHandles[189] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath", 0, "EndCinCloneA", 0, -1, -1);
        mHandles[190] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath1", 0, "EndCinClone1A", 0, -1, -1);
        mHandles[191] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath2", 0, "EndCinClone2A", 0, -1, -1);
        mHandles[192] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath3", 0, "EndCinClone3A", 0, -1, -1);
        mHandles[193] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath4", 0, "EndCinClone4A", 0, -1, -1);
        mHandles[194] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath5", 0, "EndCinClone5A", 0, -1, -1);
        mHandles[195] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath6", 0, "EndCinClone6A", 0, -1, -1);
        mHandles[196] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath7", 0, "EndCinClone7A", 0, -1, -1);
        mHandles[197] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath8", 0, "EndCinClone8A", 0, -1, -1);
        mHandles[198] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath9", 0, "EndCinClone9A", 0, -1, -1);
        MissionUtility::Goto(mHandles[189], "EndCinClonePath", true);
        MissionUtility::Goto(mHandles[190], "EndCinClonePath1", true);
        MissionUtility::Goto(mHandles[191], "EndCinClonePath2", true);
        MissionUtility::Goto(mHandles[192], "EndCinClonePath3", true);
        MissionUtility::Goto(mHandles[193], "EndCinClonePath4", true);
        MissionUtility::Goto(mHandles[194], "EndCinClonePath5", true);
        MissionUtility::Goto(mHandles[195], "EndCinClonePath6", true);
        MissionUtility::Goto(mHandles[196], "EndCinClonePath7", true);
        MissionUtility::Goto(mHandles[197], "EndCinClonePath8", true);
        MissionUtility::Goto(mHandles[198], "EndCinClonePath9", true);
            break;
        case 2:
        mHandles[199] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath", 0, "EndCinCloneB", 0, -1, -1);
        mHandles[200] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath1", 0, "EndCinClone1B", 0, -1, -1);
        mHandles[201] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath2", 0, "EndCinClone2B", 0, -1, -1);
        mHandles[202] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath3", 0, "EndCinClone3B", 0, -1, -1);
        mHandles[203] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath4", 0, "EndCinClone4B", 0, -1, -1);
        mHandles[204] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath5", 0, "EndCinClone5B", 0, -1, -1);
        mHandles[205] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath6", 0, "EndCinClone6B", 0, -1, -1);
        mHandles[206] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath7", 0, "EndCinClone7B", 0, -1, -1);
        mHandles[207] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath8", 0, "EndCinClone8B", 0, -1, -1);
        mHandles[208] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath9", 0, "EndCinClone9B", 0, -1, -1);
        MissionUtility::Goto(mHandles[199], "EndCinClonePath", true);
        MissionUtility::Goto(mHandles[200], "EndCinClonePath1", true);
        MissionUtility::Goto(mHandles[201], "EndCinClonePath2", true);
        MissionUtility::Goto(mHandles[202], "EndCinClonePath3", true);
        MissionUtility::Goto(mHandles[203], "EndCinClonePath4", true);
        MissionUtility::Goto(mHandles[204], "EndCinClonePath5", true);
        MissionUtility::Goto(mHandles[205], "EndCinClonePath6", true);
        MissionUtility::Goto(mHandles[206], "EndCinClonePath7", true);
        MissionUtility::Goto(mHandles[207], "EndCinClonePath8", true);
        MissionUtility::Goto(mHandles[208], "EndCinClonePath9", true);
            break;
        case 3:
        mHandles[209] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath", 0, "EndCinCloneC", 0, -1, -1);
        mHandles[210] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath1", 0, "EndCinClone1C", 0, -1, -1);
        mHandles[211] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath2", 0, "EndCinClone2C", 0, -1, -1);
        mHandles[212] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath3", 0, "EndCinClone3C", 0, -1, -1);
        mHandles[213] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath4", 0, "EndCinClone4C", 0, -1, -1);
        mHandles[214] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath5", 0, "EndCinClone5C", 0, -1, -1);
        mHandles[215] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath6", 0, "EndCinClone6C", 0, -1, -1);
        mHandles[216] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath7", 0, "EndCinClone7C", 0, -1, -1);
        mHandles[217] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath8", 0, "EndCinClone8C", 0, -1, -1);
        mHandles[218] = MissionUtility::CreateObjectWithRotation("rep_inf_clone", "EndCinClonePath9", 0, "EndCinClone9C", 0, -1, -1);
        MissionUtility::Goto(mHandles[209], "EndCinClonePath", true);
        MissionUtility::Goto(mHandles[210], "EndCinClonePath1", true);
        MissionUtility::Goto(mHandles[211], "EndCinClonePath2", true);
        MissionUtility::Goto(mHandles[212], "EndCinClonePath3", true);
        MissionUtility::Goto(mHandles[213], "EndCinClonePath4", true);
        MissionUtility::Goto(mHandles[214], "EndCinClonePath5", true);
        MissionUtility::Goto(mHandles[215], "EndCinClonePath6", true);
        MissionUtility::Goto(mHandles[216], "EndCinClonePath7", true);
        MissionUtility::Goto(mHandles[217], "EndCinClonePath8", true);
        MissionUtility::Goto(mHandles[218], "EndCinClonePath9", true);
        StopTimer(mTimer8);
        }
        }
        }
        if (mTimer6 >= 7.0f) {
        StopTimer(mTimer6);
        mTimer6 = 0.0f;
        MissionUtility::MoveObjectWithRotation(mHandles[168], "EndCinTankPath", 2, true);
        MissionUtility::MoveObjectWithRotation(mHandles[169], "EndCinTankPath1", 2, true);
        MissionUtility::MoveObjectWithRotation(mHandles[170], "EndCinTankPath2", 2, true);
        MissionUtility::Goto(mHandles[168], "EndCinTankPath", false);
        MissionUtility::Goto(mHandles[169], "EndCinTankPath1", false);
        MissionUtility::Goto(mHandles[170], "EndCinTankPath2", false);
        MissionUtility::SetApplyDynamics(mHandles[177], true);
        MissionUtility::TakeOff(mHandles[177]);
        }
        if (!mFlags[24]) {
        if (mFloatsB[3] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("OBR09_16", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("CTR09_17", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("OBR09_18", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[24] = true;
        }
        }
        if (mFlags[23]) {
        if (!MissionUtility::IsCinRunning(mHandles[84])) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::MissionSuccess();
        mFlags[22] = true;
        }
        }
        }
        }
    }

    // ---- +0x6ed4  96 bytes ----
    if (!mFlags[25] && mFlags[8]) {
        if (mTimer1 < 361.0f) {
        MissionUtility::DisplayText("missions.Raxus2.text.str0005", 6.0f, -1.0f);
        MissionUtility::BonusObjectiveComplete(mHandles[98], true);
        mInts[1] = mInts[1] + 1;
        mFlags[52] = true;
        mFlags[25] = true;
        }
    }

    // ---- +0x6f34  64 bytes ----
    if (!mFlags[52] && mFlags[8]) {
        if (mTimer1 > 361.0f) {
        MissionUtility::BonusObjectiveFailed(mHandles[98]);
        mFlags[25] = true;
        mFlags[52] = true;
        }
    }

    // ---- +0x6f74  36 bytes ----
    if (!mFlags[29]) {
        if (!MissionUtility::IsAlive(mHandles[85])) {
        mFlags[29] = true;
        }
    }

    // ---- +0x6f98  36 bytes ----
    if (!mFlags[30]) {
        if (!MissionUtility::IsAlive(mHandles[86])) {
        mFlags[30] = true;
        }
    }

    // ---- +0x6fbc  36 bytes ----
    if (!mFlags[31]) {
        if (!MissionUtility::IsAlive(mHandles[87])) {
        mFlags[31] = true;
        }
    }

    // ---- +0x6fe0  100 bytes ----
    if (!mFlags[26] && mFlags[29] && mFlags[30] && mFlags[31]) {
        MissionUtility::DisplayText("missions.Raxus2.text.str0008", 6.0f, -1.0f);
        MissionUtility::BonusObjectiveComplete(mHandles[99], true);
        mInts[1] = mInts[1] + 1;
        mFlags[53] = true;
        mFlags[26] = true;
    }

    // ---- +0x7044  80 bytes ----
    if (!mFlags[53] && mFlags[23]) {
        if (!mFlags[29]
            || !mFlags[30]
            || !mFlags[31]) {
        MissionUtility::BonusObjectiveFailed(mHandles[99]);
        mFlags[26] = true;
        mFlags[53] = true;
        }
    }

    // ---- +0x7094  108 bytes ----
    if (!mFlags[27] && mFlags[23]) {
        if (MissionUtility::IsAlive(mHandles[2])) {
        if (MissionUtility::IsAlive(mHandles[3])) {
        MissionUtility::DisplayText("missions.Raxus2.text.str0008", 6.0f, -1.0f);
        MissionUtility::BonusObjectiveComplete(mHandles[100], true);
        mInts[1] = mInts[1] + 1;
        mFlags[54] = true;
        mFlags[27] = true;
        }
        }
    }

    // ---- +0x7100  56 bytes ----
    if (!mFlags[54]) {
        if (mFlags[41]
            || mFlags[42]) {
        MissionUtility::BonusObjectiveFailed(mHandles[100]);
        mFlags[27] = true;
        mFlags[54] = true;
        }
    }

    // ---- +0x7138  80 bytes ----
    if (!mFlags[125] && mFlags[15]) {
        if (!MissionUtility::IsAlive(mHandles[2])) {
        if (!MissionUtility::IsAlive(mHandles[3])) {
        mFloatsB[21] = 2.0f + MissionUtility::GetTime();
        mFlags[125] = true;
        }
        }
    }

    // ---- +0x7188  56 bytes ----
    if (!mFlags[17]) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        mFloatsB[15] = 0.5f + MissionUtility::GetTime();
        mFlags[58] = true;
        mFlags[17] = true;
        }
    }

    // ---- +0x71c0  60 bytes ----
    if (!mFlags[59]) {
        if (mFloatsB[21] < MissionUtility::GetTime()) {
        mHandles[14] = MissionUtility::AddObjective("missions.Raxus2.objective.str0003");
        MissionUtility::BonusObjectiveFailed(mHandles[14]);
        MissionUtility::MissionFailure();
        mFlags[59] = true;
        }
    }

    // ---- +0x71fc  40 bytes ----
    if (!mFlags[59]) {
        if (mFloatsB[15] < MissionUtility::GetTime()) {
        MissionUtility::MissionFailure();
        mFlags[59] = true;
        }
    }

    // ---- +0x7224  88 bytes ----
    if (!mFlags[4] && !mFlags[75]) {
        if (MissionUtility::IsInsideRegion(mHandles[0], "ambush_vo1_trigger")) {
        mHandles[134] = MissionUtility::QueueSound("ASR09_08A", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[4] = true;
        }
    }

    // ---- +0x727c  40 bytes ----
    if (!mFlags[34]) {
        if (mFloatsB[6] < MissionUtility::GetTime()) {
        MissionUtility::MissionFailure();
        mFlags[34] = true;
        }
    }
}

void Raxus2Script::Setup()
{
        mFlags[0] = true;
        mFloatsB[0] = 999999.9f;
        mFloatsB[1] = 999999.9f;
        mFloatsB[23] = 999999.9f;
        mFloatsB[4] = 999999.9f;
        mFloatsB[2] = 999999.9f;
        mFloatsB[3] = 999999.9f;
        mFloatsB[5] = 999999.9f;
        mFloatsB[6] = 999999.9f;
        mFloatsB[7] = 999999.9f;
        mFloatsB[8] = 999999.9f;
        mFloatsB[9] = 999999.9f;
        mFloatsB[10] = 999999.9f;
        mFloatsB[11] = 999999.9f;
        mFloatsB[12] = 999999.9f;
        mFloatsB[13] = 999999.9f;
        mFloatsB[15] = 999999.9f;
        mFloatsB[16] = 999999.9f;
        mFloatsB[17] = 999999.9f;
        mFloatsB[18] = 999999.9f;
        mFloatsB[19] = 999999.9f;
        mFloatsB[22] = 999999.9f;
        mFloatsB[20] = 999999.9f;
        mFloatsB[21] = 999999.9f;
        mHandles[2] = MissionUtility::GetHandle("squadmate1");
        mHandles[3] = MissionUtility::GetHandle("squadmate2");
        mHandles[12] = MissionUtility::GetHandle("begin_wheel1");
        mHandles[13] = MissionUtility::GetHandle("begin_wheel2");
        mHandles[85] = MissionUtility::GetHandle("hidden_thing1");
        mHandles[86] = MissionUtility::GetHandle("hidden_thing2");
        mHandles[87] = MissionUtility::GetHandle("hidden_thing3");
        mHandles[101] = MissionUtility::GetHandle("walker1");
        mHandles[102] = MissionUtility::GetHandle("walker2");
        mHandles[103] = MissionUtility::GetHandle("trans1");
        mHandles[104] = MissionUtility::GetHandle("trans2");
        mHandles[105] = MissionUtility::GetHandle("trans3");
        mHandles[106] = MissionUtility::GetHandle("tank1");
        mHandles[107] = MissionUtility::GetHandle("tank2");
        mHandles[108] = MissionUtility::GetHandle("tank3");
        mHandles[109] = MissionUtility::GetHandle("tank4");
        mHandles[110] = MissionUtility::GetHandle("tank5");
        mHandles[111] = MissionUtility::GetHandle("tank6");
        mHandles[112] = MissionUtility::GetHandle("stap1");
        mHandles[113] = MissionUtility::GetHandle("stap2");
        mHandles[114] = MissionUtility::GetHandle("stap3");
        mHandles[115] = MissionUtility::GetHandle("stap4");
        mHandles[116] = MissionUtility::GetHandle("stap5");
        mHandles[117] = MissionUtility::GetHandle("stap6");
        mHandles[124] = MissionUtility::GetHandle("initial_player");
        mHandles[145] = MissionUtility::GetHandle("force_field1");
        mHandles[146] = MissionUtility::GetHandle("force_field2");
        mHandles[127] = MissionUtility::GetHandle("turret1");
        mHandles[128] = MissionUtility::GetHandle("turret2");
        mHandles[129] = MissionUtility::GetHandle("turret3");
        mHandles[130] = MissionUtility::GetHandle("turret4");
        mHandles[131] = MissionUtility::GetHandle("turret5");
        mHandles[132] = MissionUtility::GetHandle("turret6");
        mHandles[133] = MissionUtility::GetHandle("turret7");
        mInts[0] = 0;
        mInts[1] = 0;
        mInts[2] = 0;
        MissionUtility::PreloadConfig("rep_walk_assault");
        MissionUtility::PreloadConfig("rep_walk_sixleg");
        MissionUtility::PreloadConfig("rep_fly_assault");
        MissionUtility::PreloadConfig("rep_inf_clone");
        MissionUtility::PreloadConfig("rep_fly_vcarrier");
        MissionUtility::PreloadConfig("cis_tank_assault");
        MissionUtility::PreloadConfig("rep_tank_fighter1_player");
        MissionUtility::PreloadConfig("rep_tank_fighter1_squadmate");
        MissionUtility::PreloadConfig("cis_tank_gtrans");
        MissionUtility::PreloadConfig("cis_bike_speeder");
        MissionUtility::PreloadConfig("REP_fly_gunship_1_dest");
        MissionUtility::PreloadConfig("cis_fly_vcarrier");
        MissionUtility::PreloadConfig("rax_fly_collector");
}

SPMission *Raxus2BuildMission()
{
    return new Raxus2Script();
}
