// Geonosis1Script.cpp -- reconstruction of a shipped mission script.  BYTE-EXACT, all 5 functions.
//
// Head from tools/gen_mission_head.py, bodies from tools/gen_block.py, assembled by
// tools/gen_mission_tu.py.  `gen_block.py` left no markers at all here; three edits -- see
// analysis/mission_batch_a.md:
//   * `Vector::Vector(float, float, float)` is emitted out of line in this TU (the fifth
//     function), so it needs a body in the class;
//   * `GetRandomFloat` returns `float`, not `int` -- `fmr f31, f1` right after the call;
//   * `shooter` is one `GetWhoShotMe` handed straight to `GetTeamNum` in r3.

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
    Vector(float a, float b, float c) { x = a; y = b; z = c; }

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
    int    AddHealthBar(int, const char*, float);
    int    AddHealthBox(const char*, int, float);
    int    AddObjective(const char*);
    void   AddTurnAroundRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    void   AttackTarget(int, int, bool, bool, bool, bool);
    void   BonusObjectiveFailed(int);
    void   CarrierAddCargo(int, const char*, int, const char*, bool);
    void   CarrierDropoff(int, const char*, int, float);
    int    CountUnitsNearObject(int, float, int, const char*);
    int    CreateFlock(int, Formation);
    int    CreateObject(const char*, const char*, const char*, int, int, int);
    int    CreateObject(const char*, const char*, int, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObject(const char*, int, const char*, const char*, int, int, int);
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    void   DamageObject(int, float, float);
    void   Defend(int, int, float);
    void   DisplayText(const char*, float, float);
    int    DropAmmoBox(int);
    int    DropHealthBox(int);
    void   EvictConfig(const char*);
    void   FadeInDust(float);
    void   FadeOutDust(float);
    void   FlushSoundQueue();
    int    GetCinId(int);
    float  GetCurHealth(int);
    float  GetDistance(int, const char*);
    float  GetDistance(int, const char*, int);
    float  GetDistance(int, int);
    int    GetHandle(const char*);
    int    GetPlayerHandle(int);
    float  GetRandomFloat(float, float);
    int    GetTeamNum(int);
    float  GetTime();
    int    GetWhoShotMe(int);
    void   Goto(int, const char*, bool);
    void   Goto(int, const char*, int);
    void   Goto(int, int);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsDropped(int);
    bool   IsFlockAlive(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsPowerupAlive(unsigned int);
    bool   IsSoundPlaying(int);
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
    void   Objectify(int, const char*, bool, bool, float);
    void   ObjectiveComplete(int);
    void   OverrideSoundRange(int, bool);
    void   Patrol(int, const char*, float, bool);
    void   PlayMusic(const char*, bool);
    void   PreloadConfig(const char*);
    int    QueueSound(const char*, float, float, float, const char*, int, const char*);
    void   RemoveFlock(int, bool);
    void   RemoveObject(int);
    void   RemoveObjectify(int);
    void   RemoveTurnAroundRegion(const char*);
    int    RunCin(const char*, bool, bool);
    void   SetAlliance(int, int);
    void   SetAnimation(int, const char*, float, int);
    void   SetApplyDynamics(int, bool);
    void   SetAttackRange(int, int);
    void   SetCollidable(int, bool);
    void   SetCurHealth(int, float);
    void   SetEnemies(int, int);
    void   SetFOV(float);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetQueueFlag(bool);
    void   SetTeamNum(int, int);
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

static const char *const kClassName = "Geonosis1Script";
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

class Geonosis1Script : public SPMission
{
public:
    virtual ~Geonosis1Script();

    Geonosis1Script()
    {
        mBoolCount  = 243;           mBools = mFlags;
        mCountB     = 50;            mIntsB = mTimes;
        mCountC     = 236;           mIntsC = mHandles;
        mCountD     = 17;            mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    char   mPad24[13];
    bool   mFlags[243];            // +0x031  the one-shot latches
    char   mPad124[8];                  // +0x124
    float    mTimes[50];            // +0x12c
    char   mPad1F4[8];                  // +0x1f4
    int    mHandles[236];          // +0x1fc
    char   mPad5AC[8];                  // +0x5ac
    int    mInts[17];             // +0x5b4
    char   mPad5F8[4];                  // +0x5f8
    Timer  mTimer0;                      // +0x5fc
    Timer  mTimer1;                      // +0x608
    Timer  mTimer2;                      // +0x614
    Timer  mTimer3;                      // +0x620
    Timer  mTimer4;                      // +0x62c
    Timer  mTimer5;                      // +0x638
    Timer  mTimer6;                      // +0x644
    Timer  mTimer7;                      // +0x650
    Timer  mTimer8;                      // +0x65c
    Timer  mTimer9;                      // +0x668
    Timer  mTimer10;                      // +0x674
};

Geonosis1Script::~Geonosis1Script()
{
}

void Geonosis1Script::Execute()
{
    // ---- +0x000c  1388 bytes ----
    if (!mFlags[10]) {
    MissionUtility::SetMusicLooping(true);
    MissionUtility::SetMaxHealth(mHandles[77], 10000.0f);
    MissionUtility::SetCurHealth(mHandles[77], 10000.0f);
    if (MissionUtility::MidMissionGetSavePoint() == 1) {
    mHandles[13] = MissionUtility::AddAmmoBox("spawn_ammo_here", 0, -1.0f);
    MissionUtility::AddHealthBox("spawn_health_here", 0, -1.0f);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mTimes[3]);
    MissionUtility::MidMissionLoad(mTimes[4]);
    MissionUtility::MidMissionLoad(mTimes[5]);
    MissionUtility::MidMissionLoad(mTimes[6]);
    MissionUtility::MidMissionLoad(mFlags[62]);
    MissionUtility::MidMissionLoad(mFlags[63]);
    MissionUtility::MidMissionLoad(mFlags[64]);
    MissionUtility::MidMissionLoad(mFlags[134]);
    MissionUtility::MidMissionLoad(mFlags[135]);
    MissionUtility::MidMissionLoad(mFlags[136]);
    MissionUtility::MidMissionLoad(mInts[16]);
    MissionUtility::MidMissionLoad(mFlags[48]);
    MissionUtility::MidMissionLoad(mFlags[51]);
    MissionUtility::MidMissionLoad(mFlags[52]);
    MissionUtility::MidMissionLoad(mFlags[234]);
    MissionUtility::MidMissionLoad(mFlags[235]);
    MissionUtility::MidMissionLoad(mFlags[236]);
    MissionUtility::MidMissionLoad(mFlags[237]);
    MissionUtility::MidMissionLoad(mFlags[238]);
    MissionUtility::MidMissionLoad(mFlags[239]);
    MissionUtility::MidMissionLoad(mFlags[240]);
    MissionUtility::MidMissionLoad(mFlags[241]);
    MissionUtility::MidMissionLoad(mFlags[242]);
    MissionUtility::MidMissionLoad(mFlags[0]);
    MissionUtility::MidMissionLoad(mFlags[1]);
    if (mFlags[234]) {
    MissionUtility::RemoveObject(mHandles[43]);
    }
    if (mFlags[235]) {
    MissionUtility::RemoveObject(mHandles[44]);
    }
    if (mFlags[236]) {
    MissionUtility::RemoveObject(mHandles[45]);
    }
    if (mFlags[237]) {
    MissionUtility::RemoveObject(mHandles[46]);
    }
    if (mFlags[238]) {
    MissionUtility::RemoveObject(mHandles[47]);
    }
    if (mFlags[239]) {
    MissionUtility::RemoveObject(mHandles[48]);
    }
    if (mFlags[240]) {
    MissionUtility::RemoveObject(mHandles[49]);
    }
    if (mFlags[241]) {
    MissionUtility::RemoveObject(mHandles[50]);
    }
    if (mFlags[242]) {
    MissionUtility::RemoveObject(mHandles[4]);
    }
    MissionUtility::RemoveObject(mHandles[34]);
    MissionUtility::RemoveObject(mHandles[35]);
    MissionUtility::SetMaxHealth(mHandles[77], 8000.0f);
    MissionUtility::SetCurHealth(mHandles[77], mTimes[3]);
    MissionUtility::RemoveFlock(mHandles[184], true);
    MissionUtility::RemoveFlock(mHandles[185], true);
    MissionUtility::RemoveFlock(mHandles[186], true);
    MissionUtility::RemoveObject(mHandles[0]);
    MissionUtility::RemoveObject(mHandles[1]);
    MissionUtility::RemoveObject(mHandles[2]);
    MissionUtility::RemoveObject(mHandles[3]);
    MissionUtility::MoveObjectWithRotation(mHandles[11], "put_player_here", 0, true);
    MissionUtility::MoveObjectWithRotation(mHandles[77], "put_lum_here", 0, true);
    mInts[0] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0000");
    MissionUtility::ObjectiveComplete(mInts[0]);
    mInts[1] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0004");
    MissionUtility::ObjectiveComplete(mInts[1]);
    mInts[2] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0001");
    MissionUtility::DisplayText("missions.Geonosis1.text.str0001", 6.0f, -1.0f);
    mHandles[12] = MissionUtility::CreateObject("rep_tank_gtrans", "mace_spawn_1b", "transport1", 1, -1, -1);
    MissionUtility::AddHealthBar(mHandles[12], "", 400.0f);
    MissionUtility::SetMaxHealth(mHandles[12], 2800.0f);
    MissionUtility::SetCurHealth(mHandles[12], mTimes[4]);
    mHandles[109] = MissionUtility::CreateFlock(mHandles[12], (Formation)8);
    MissionUtility::SetVelocForward(mHandles[12], 37.0f);
    MissionUtility::Objectify(mHandles[12], "missions.Geonosis1.marker.str0001", false, true, 0.0f);
    if (mFlags[63]) {
    mHandles[14] = MissionUtility::CreateObject("rep_tank_gtrans", "mace_spawn_2b", "transport2", 1, -1, -1);
    MissionUtility::SetMaxHealth(mHandles[14], 2800.0f);
    MissionUtility::SetCurHealth(mHandles[14], mTimes[5]);
    MissionUtility::AddHealthBar(mHandles[14], "", 400.0f);
    MissionUtility::AddFlockMember(mHandles[109], mHandles[14], Vector(0.0f, 0.0f, 10.0f));
    MissionUtility::SetVelocForward(mHandles[14], 37.0f);
    }
    if (mFlags[64]) {
    mHandles[15] = MissionUtility::CreateObject("rep_tank_gtrans", "mace_spawn_3b", "transport3", 1, -1, -1);
    MissionUtility::SetMaxHealth(mHandles[15], 1200.0f);
    MissionUtility::SetCurHealth(mHandles[15], mTimes[6]);
    MissionUtility::AddHealthBar(mHandles[15], "", 400.0f);
    MissionUtility::AddFlockMember(mHandles[109], mHandles[15], Vector(0.0f, 0.0f, 70.0f));
    }
    MissionUtility::Goto(mHandles[109], "convoy_path", true);
    MissionUtility::Defend(mHandles[77], mHandles[12], 250.0f);
    MissionUtility::SetAttackRange(mHandles[77], 300);
    mFlags[7] = true;
    mFlags[201] = true;
    }
    MissionUtility::SetEnemies(1, 2);
    MissionUtility::SetEnemies(1, 3);
    MissionUtility::SetAlliance(3, 2);
    MissionUtility::SetAlliance(1, 0);
    MissionUtility::SetAlliance(2, 0);
    MissionUtility::SetAlliance(2, 4);
    MissionUtility::SetAlliance(1, 4);
    mHandles[129] = MissionUtility::AddBonusObjective("missions.Geonosis1.bonus.str0000");
    mHandles[130] = MissionUtility::AddBonusObjective("missions.Geonosis1.bonus.str0004");
    mHandles[131] = MissionUtility::AddBonusObjective("missions.Geonosis1.bonus.str0003");
    MissionUtility::SetCollidable(mHandles[138], false);
    MissionUtility::SetCollidable(mHandles[139], false);
    MissionUtility::SetCollidable(mHandles[140], false);
    MissionUtility::AddTurnAroundRegion("turnaround3", "turnaround3_point1", 0, 0, 0, 0);
    mFlags[10] = true;
    }

    // ---- +0x0578  13980 bytes ----
    if (!mFlags[201]) {
        if (!mFlags[203]) {
        MissionUtility::SetTeamNum(mHandles[38], 0);
        MissionUtility::SetTeamNum(mHandles[39], 0);
        MissionUtility::SetTeamNum(mHandles[40], 0);
        MissionUtility::SetTeamNum(mHandles[41], 0);
        MissionUtility::SetTeamNum(mHandles[42], 0);
        MissionUtility::Stop(mHandles[0]);
        MissionUtility::Stop(mHandles[1]);
        MissionUtility::Stop(mHandles[2]);
        MissionUtility::Stop(mHandles[3]);
        MissionUtility::SetMaxHealth(mHandles[34], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[34], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[35], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[35], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[0], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[0], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[1], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[1], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[2], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[2], 999999.0f);
        MissionUtility::SetMaxHealth(mHandles[3], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[3], 999999.0f);
        MissionUtility::AddTurnAroundRegion("turnaround2", "turnaround2_point1", "turnaround2_point2", "turnaround2_point3", "turnaround2_point4", "turnaround2_point5");
        MissionUtility::SetTeamNum(mHandles[77], 0);
        mFlags[203] = true;
        }
        if (mTimes[44] < MissionUtility::GetTime()) {
        if (!mFlags[25]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "turnaround2")) {
        MissionUtility::QueueSound("LMG02_30", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[44] = 8.0f + MissionUtility::GetTime();
        }
        }
        }
        if (!mFlags[43]) {
        if (!mFlags[42]) {
        MissionUtility::SetVisible(mHandles[11], false);
        MissionUtility::SetVisible(mHandles[77], false);
        mHandles[121] = MissionUtility::RunCin("new_open_cin", true, true);
        MissionUtility::OverrideSoundRange(mHandles[197], true);
        MissionUtility::OverrideSoundRange(mHandles[196], true);
        MissionUtility::OverrideSoundRange(mHandles[198], true);
        MissionUtility::OverrideSoundRange(mHandles[199], true);
        MissionUtility::OverrideSoundRange(mHandles[200], true);
        MissionUtility::SetVelocForward(mHandles[197], 200.0f);
        MissionUtility::SetVelocForward(mHandles[196], 200.0f);
        MissionUtility::Stop(mHandles[198]);
        MissionUtility::Stop(mHandles[199]);
        MissionUtility::Stop(mHandles[200]);
        MissionUtility::Goto(mHandles[197], "OpenCinNewTankPath", false);
        MissionUtility::Goto(mHandles[196], "OpenCinNewTankPath1", false);
        mFlags[42] = true;
        BeginTimer(mTimer7);
        MissionUtility::PlayMusic("EP1_V2_T02", true);
        }
        if (!mFlags[230]) {
        if (!mFlags[223]) {
        if (mTimer7 > 4.0f) {
        StopTimer(mTimer7);
        mTimer7 = 0.0f;
        MissionUtility::QueueSound("LMG02_15", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("MWG02_16", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("LMG02_16", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("MWG02_17", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("LMG02_17", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[223] = true;
        }
        }
        }
        if (!mFlags[230]) {
        if (!mFlags[218]) {
        if (MissionUtility::GetCinId(mHandles[121]) == 3) {
        MissionUtility::SetVelocForward(mHandles[197], 75.0f);
        MissionUtility::SetVelocForward(mHandles[196], 75.0f);
        MissionUtility::AttackTarget(mHandles[197], mHandles[198], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[196], mHandles[201], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[198], mHandles[196], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[199], mHandles[196], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[200], mHandles[196], true, true, false, false);
        BeginTimer(mTimer6);
        mFlags[218] = true;
        }
        }
        }
        if (!mFlags[219]) {
        if (!MissionUtility::IsAlive(mHandles[198])) {
        if (MissionUtility::IsAlive(mHandles[199])) {
        MissionUtility::AttackTarget(mHandles[197], mHandles[199], true, true, false, false);
        mFlags[219] = true;
        }
        }
        }
        if (!mFlags[220]) {
        if (!MissionUtility::IsAlive(mHandles[198])) {
        if (!MissionUtility::IsAlive(mHandles[199])) {
        if (MissionUtility::IsAlive(mHandles[200])) {
        MissionUtility::AttackTarget(mHandles[197], mHandles[200], true, true, false, false);
        mFlags[220] = true;
        }
        }
        }
        }
        if (!mFlags[221]) {
        if (!MissionUtility::IsAlive(mHandles[201])) {
        if (MissionUtility::IsAlive(mHandles[202])) {
        MissionUtility::AttackTarget(mHandles[196], mHandles[202], true, true, false, false);
        mFlags[221] = true;
        }
        }
        }
        if (mHandles[197] == MissionUtility::GetWhoShotMe(mHandles[198])) {
        MissionUtility::DamageObject(mHandles[198], 1000.0f, 1000.0f);
        }
        if (mHandles[197] == MissionUtility::GetWhoShotMe(mHandles[199])) {
        MissionUtility::DamageObject(mHandles[199], 1000.0f, 1000.0f);
        }
        if (mHandles[197] == MissionUtility::GetWhoShotMe(mHandles[200])) {
        MissionUtility::DamageObject(mHandles[200], 1000.0f, 1000.0f);
        }
        if (!mFlags[230]) {
        if (!mFlags[222]) {
        if (MissionUtility::GetCinId(mHandles[121]) == 4) {
        if (MissionUtility::IsAlive(mHandles[198])) {
        MissionUtility::DamageObject(mHandles[198], 1000.0f, 1000.0f);
        }
        if (MissionUtility::IsAlive(mHandles[199])) {
        MissionUtility::DamageObject(mHandles[199], 1000.0f, 1000.0f);
        }
        if (MissionUtility::IsAlive(mHandles[200])) {
        MissionUtility::DamageObject(mHandles[200], 1000.0f, 1000.0f);
        }
        if (MissionUtility::IsAlive(mHandles[201])) {
        MissionUtility::DamageObject(mHandles[201], 1000.0f, 1000.0f);
        }
        if (MissionUtility::IsAlive(mHandles[202])) {
        MissionUtility::DamageObject(mHandles[202], 1000.0f, 1000.0f);
        }
        mFlags[222] = true;
        MissionUtility::SetVelocForward(mHandles[197], 200.0f);
        MissionUtility::SetVelocForward(mHandles[196], 200.0f);
        MissionUtility::Goto(mHandles[197], "OpenCinFinalTankPath", false);
        MissionUtility::Goto(mHandles[196], "OpenCinFinalTankPath", false);
        }
        }
        }
        if (mFlags[42]) {
        if (!MissionUtility::IsCinRunning(mHandles[121])) {
        mFlags[230] = true;
        MissionUtility::FlushSoundQueue();
        MissionUtility::SetVelocForward(mHandles[77], 63.0f);
        MissionUtility::SetAttackRange(mHandles[77], 1);
        MissionUtility::Stop(mHandles[77]);
        MissionUtility::SetTeamNum(mHandles[77], 0);
        MissionUtility::SetMaxHealth(mHandles[37], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[37], 999999.0f);
        mHandles[56] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol1_spawn1", "", 2, -1, -1);
        mHandles[57] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol1_spawn2", "", 2, -1, -1);
        mHandles[58] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol1_spawn3", "", 2, -1, -1);
        mHandles[184] = MissionUtility::CreateFlock(mHandles[56], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[184], mHandles[57]);
        MissionUtility::AddFlockMember(mHandles[184], mHandles[58]);
        MissionUtility::Patrol(mHandles[184], "stap_patrol1_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[56], 450);
        mHandles[59] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol2_spawn1", "", 2, -1, -1);
        mHandles[60] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol2_spawn2", "", 2, -1, -1);
        mHandles[61] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol2_spawn3", "", 2, -1, -1);
        mHandles[185] = MissionUtility::CreateFlock(mHandles[59], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[185], mHandles[60]);
        MissionUtility::AddFlockMember(mHandles[185], mHandles[61]);
        MissionUtility::Patrol(mHandles[185], "stap_patrol2_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[59], 450);
        mHandles[62] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol3_spawn1", "", 2, -1, -1);
        mHandles[63] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol3_spawn2", "", 2, -1, -1);
        mHandles[64] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol3_spawn3", "", 2, -1, -1);
        mHandles[186] = MissionUtility::CreateFlock(mHandles[62], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[186], mHandles[63]);
        MissionUtility::AddFlockMember(mHandles[186], mHandles[64]);
        MissionUtility::Patrol(mHandles[186], "stap_patrol3_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[62], 450);
        mHandles[65] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol4_spawn1", "", 2, -1, -1);
        mHandles[66] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol4_spawn2", "", 2, -1, -1);
        mHandles[67] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol4_spawn3", "", 2, -1, -1);
        mHandles[187] = MissionUtility::CreateFlock(mHandles[65], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[187], mHandles[66]);
        MissionUtility::AddFlockMember(mHandles[187], mHandles[67]);
        MissionUtility::Patrol(mHandles[187], "stap_patrol4_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[65], 450);
        mHandles[68] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol5_spawn1", "", 2, -1, -1);
        mHandles[69] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol5_spawn2", "", 2, -1, -1);
        mHandles[70] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol5_spawn3", "", 2, -1, -1);
        mHandles[188] = MissionUtility::CreateFlock(mHandles[68], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[188], mHandles[69]);
        MissionUtility::AddFlockMember(mHandles[188], mHandles[70]);
        MissionUtility::Patrol(mHandles[188], "stap_patrol5_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[68], 450);
        mHandles[71] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol6_spawn1", "", 2, -1, -1);
        mHandles[72] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol6_spawn2", "", 2, -1, -1);
        mHandles[73] = MissionUtility::CreateObject("cis_bike_speeder", "stap_patrol6_spawn3", "", 2, -1, -1);
        mHandles[189] = MissionUtility::CreateFlock(mHandles[71], (Formation)1);
        MissionUtility::AddFlockMember(mHandles[189], mHandles[72]);
        MissionUtility::AddFlockMember(mHandles[189], mHandles[73]);
        MissionUtility::Patrol(mHandles[189], "stap_patrol6_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[71], 450);
        if (MissionUtility::IsAlive(mHandles[122])) {
        MissionUtility::RemoveObject(mHandles[122]);
        }
        if (MissionUtility::IsAlive(mHandles[123])) {
        MissionUtility::RemoveObject(mHandles[123]);
        }
        if (MissionUtility::IsAlive(mHandles[198])) {
        MissionUtility::RemoveObject(mHandles[198]);
        }
        if (MissionUtility::IsAlive(mHandles[199])) {
        MissionUtility::RemoveObject(mHandles[199]);
        }
        if (MissionUtility::IsAlive(mHandles[200])) {
        MissionUtility::RemoveObject(mHandles[200]);
        }
        if (MissionUtility::IsAlive(mHandles[201])) {
        MissionUtility::RemoveObject(mHandles[201]);
        }
        if (MissionUtility::IsAlive(mHandles[202])) {
        MissionUtility::RemoveObject(mHandles[202]);
        }
        if (MissionUtility::IsAlive(mHandles[196])) {
        MissionUtility::RemoveObject(mHandles[196]);
        }
        if (MissionUtility::IsAlive(mHandles[197])) {
        MissionUtility::RemoveObject(mHandles[197]);
        }
        MissionUtility::SetVisible(mHandles[11], true);
        MissionUtility::SetVisible(mHandles[77], true);
        MissionUtility::MoveObjectWithRotation(mHandles[11], "mace_tank_spawn", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[77], "lum_tank_spawn", 0, true);
        mInts[0] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0000");
        MissionUtility::DisplayText("missions.Geonosis1.text.str0000", 6.0f, -1.0f);
        MissionUtility::Objectify(mHandles[128], "missions.Geonosis1.marker.str0000", false, true, 0.0f);
        mTimes[17] = 1.0f + MissionUtility::GetTime();
        mTimes[21] = 5.0f + MissionUtility::GetTime();
        MissionUtility::SetTeamNum(mHandles[77], 1);
        MissionUtility::StartAmbiences("AmbGeon_plains01_pl2", "AmbGeon_plains_stinger01", 10.0f, 30.0f);
        mFlags[43] = true;
        MissionUtility::PlayMusic("EP1_V1_T12", true);
        }
        }
        }
        if (!mFlags[120]) {
        if (mFlags[43]) {
        if (!mFlags[14]) {
        if (!MissionUtility::IsAlive(mHandles[11])) {
        MissionUtility::BonusObjectiveFailed(mInts[0]);
        mTimes[2] = 3.0f + MissionUtility::GetTime();
        mFlags[120] = true;
        }
        }
        }
        }
        if (!mFlags[8]) {
        if (mFlags[5]) {
        if (!mFlags[25]) {
        if (!MissionUtility::IsAlive(mHandles[11])) {
        MissionUtility::BonusObjectiveFailed(mInts[1]);
        mTimes[2] = 3.0f + MissionUtility::GetTime();
        mFlags[8] = true;
        }
        }
        }
        }
        if (!mFlags[29]) {
        if (MissionUtility::GetDistance(mHandles[184], mHandles[11]) < 400.0f) {
        MissionUtility::AttackTarget(mHandles[184], mHandles[11], true, true, false, false);
        mFlags[29] = true;
        }
        }
        if (!mFlags[30]) {
        if (MissionUtility::GetDistance(mHandles[185], mHandles[11]) < 400.0f) {
        MissionUtility::AttackTarget(mHandles[185], mHandles[11], true, true, false, false);
        mFlags[30] = true;
        }
        }
        if (!mFlags[31]) {
        if (MissionUtility::GetDistance(mHandles[186], mHandles[11]) < 400.0f) {
        MissionUtility::AttackTarget(mHandles[186], mHandles[11], true, true, false, false);
        mFlags[31] = true;
        }
        }
        if (!mFlags[68]) {
        if (mTimes[17] < MissionUtility::GetTime()) {
        MissionUtility::SetTeamNum(mHandles[77], 1);
        MissionUtility::SetAttackRange(mHandles[77], 400);
        MissionUtility::AttackTarget(mHandles[77], mHandles[43], true, true, false, false);
        mFlags[68] = true;
        }
        }
        if (!mFlags[69]) {
        if (!MissionUtility::IsAlive(mHandles[43])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[44], true, true, false, false);
        mFlags[69] = true;
        }
        }
        if (!mFlags[70]) {
        if (!MissionUtility::IsAlive(mHandles[44])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[45], true, true, false, false);
        mFlags[70] = true;
        }
        }
        if (!mFlags[74]) {
        if (!MissionUtility::IsAlive(mHandles[45])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[46], true, true, false, false);
        mFlags[74] = true;
        }
        }
        if (!mFlags[75]) {
        if (!MissionUtility::IsAlive(mHandles[46])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[47], true, true, false, false);
        mFlags[75] = true;
        }
        }
        if (!mFlags[85]) {
        if (!MissionUtility::IsAlive(mHandles[47])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[48], true, true, false, false);
        mFlags[85] = true;
        }
        }
        if (!mFlags[86]) {
        if (!MissionUtility::IsAlive(mHandles[48])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[49], true, true, false, false);
        mFlags[86] = true;
        }
        }
        if (!mFlags[87]) {
        if (!MissionUtility::IsAlive(mHandles[49])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[50], true, true, false, false);
        mFlags[87] = true;
        }
        }
        if (!mFlags[137]) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[34])
            || mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[35])
            || mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[0])
            || mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[1])
            || mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[2])
            || mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[3])) {
        MissionUtility::SetMaxHealth(mHandles[34], 200.0f);
        MissionUtility::SetCurHealth(mHandles[34], 200.0f);
        MissionUtility::SetMaxHealth(mHandles[35], 200.0f);
        MissionUtility::SetCurHealth(mHandles[35], 200.0f);
        MissionUtility::SetMaxHealth(mHandles[0], 250.0f);
        MissionUtility::SetCurHealth(mHandles[0], 250.0f);
        MissionUtility::SetMaxHealth(mHandles[1], 250.0f);
        MissionUtility::SetCurHealth(mHandles[1], 250.0f);
        MissionUtility::SetMaxHealth(mHandles[2], 250.0f);
        MissionUtility::SetCurHealth(mHandles[2], 250.0f);
        MissionUtility::SetMaxHealth(mHandles[3], 250.0f);
        MissionUtility::SetCurHealth(mHandles[3], 250.0f);
        MissionUtility::Wait(mHandles[0]);
        MissionUtility::Wait(mHandles[1]);
        MissionUtility::Wait(mHandles[2]);
        MissionUtility::Wait(mHandles[3]);
        MissionUtility::SetAttackRange(mHandles[56], 3000);
        MissionUtility::SetAttackRange(mHandles[59], 3000);
        MissionUtility::SetAttackRange(mHandles[62], 3000);
        MissionUtility::SetAttackRange(mHandles[65], 3000);
        MissionUtility::SetAttackRange(mHandles[68], 3000);
        MissionUtility::SetAttackRange(mHandles[71], 3000);
        MissionUtility::AttackTarget(mHandles[56], mHandles[11], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[59], mHandles[11], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[62], mHandles[11], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[65], mHandles[11], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[68], mHandles[11], true, true, false, false);
        MissionUtility::AttackTarget(mHandles[71], mHandles[11], true, true, false, false);
        mFlags[137] = true;
        }
        }
        if (!mFlags[88]) {
        if (!MissionUtility::IsAlive(mHandles[50])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[34], true, true, false, false);
        mFlags[88] = true;
        }
        }
        if (!mFlags[76]) {
        if (!MissionUtility::IsAlive(mHandles[34])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[35], true, true, false, false);
        mFlags[76] = true;
        }
        }
        if (!mFlags[98]) {
        if (!MissionUtility::IsAlive(mHandles[35])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[0], true, true, false, false);
        mFlags[98] = true;
        }
        }
        if (!mFlags[99]) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[1], true, true, false, false);
        mFlags[99] = true;
        }
        }
        if (!mFlags[100]) {
        if (!MissionUtility::IsAlive(mHandles[1])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[2], true, true, false, false);
        mFlags[100] = true;
        }
        }
        if (!mFlags[101]) {
        if (!MissionUtility::IsAlive(mHandles[2])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[3], true, true, false, false);
        mFlags[101] = true;
        }
        }
        if (!mFlags[92]) {
        if (!MissionUtility::IsAlive(mHandles[43])) {
        if (!MissionUtility::IsAlive(mHandles[44])) {
        if (!MissionUtility::IsAlive(mHandles[45])) {
        if (!MissionUtility::IsAlive(mHandles[46])) {
        if (!MissionUtility::IsAlive(mHandles[47])) {
        if (!MissionUtility::IsAlive(mHandles[48])) {
        if (!MissionUtility::IsAlive(mHandles[49])) {
        if (!MissionUtility::IsAlive(mHandles[50])) {
        if (!MissionUtility::IsAlive(mHandles[34])) {
        if (!MissionUtility::IsAlive(mHandles[35])) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        if (!MissionUtility::IsAlive(mHandles[1])) {
        if (!MissionUtility::IsAlive(mHandles[2])) {
        if (!MissionUtility::IsAlive(mHandles[3])) {
        MissionUtility::Patrol(mHandles[77], "lum_patrol", 500.0f, true);
        mFlags[92] = true;
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
        if (!mFlags[84]) {
        if (MissionUtility::GetDistance(mHandles[77], mHandles[43]) < 300.0f
            || !MissionUtility::IsAlive(mHandles[43])) {
        MissionUtility::QueueSound("LMG02_18", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[84] = true;
        }
        }
        if (!mFlags[14]) {
        if (!MissionUtility::IsAlive(mHandles[0])) {
        if (!MissionUtility::IsAlive(mHandles[1])) {
        if (!MissionUtility::IsAlive(mHandles[2])) {
        if (!MissionUtility::IsAlive(mHandles[2])) {
        if (!MissionUtility::IsAlive(mHandles[34])) {
        if (!MissionUtility::IsAlive(mHandles[35])) {
        if (MissionUtility::CountUnitsNearObject(mHandles[54], 800.0f, 2, 0) == 0) {
        MissionUtility::Goto(mHandles[77], "put_lum_here", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Wait(mHandles[77]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::ObjectiveComplete(mInts[0]);
        mTimes[0] = 4.0f + MissionUtility::GetTime();
        mFlags[14] = true;
        }
        }
        }
        }
        }
        }
        }
        }
        if (!mFlags[16]) {
        if (mTimes[0] < MissionUtility::GetTime()) {
        if (!mFlags[18]) {
        mHandles[132] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "landing_gunship1_path", 0, "landing_gunship1", 1, -1, -1);
        mHandles[203] = MissionUtility::CreateObjectWithRotation("rep_tank_gtrans", "LandingCinGtrans1Path", 0, "LandingCinGtrans1", 0, -1, -1);
        MissionUtility::CarrierAddCargo(mHandles[132], "hp_link_1", mHandles[203], "hp_link_1", true);
        MissionUtility::Goto(mHandles[132], "landing_gunship1_path", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[132], "LandingCinGtrans1Path", 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[132], "Landing_gunship1_path1", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::OverrideSoundRange(mHandles[132], true);
        MissionUtility::OverrideSoundRange(mHandles[203], true);
        mHandles[16] = MissionUtility::RunCin("landing_cin", true, true);
        MissionUtility::MoveObjectWithRotation(mHandles[11], "put_player_here", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[77], "put_lum_here", 0, true);
        mTimes[22] = 10.0f + MissionUtility::GetTime();
        MissionUtility::PlayMusic("EP2_V1_T12_01", true);
        mFlags[18] = true;
        }
        if (!mFlags[225]) {
        if (MissionUtility::IsDropped(mHandles[203])) {
        mFlags[225] = true;
        MissionUtility::Goto(mHandles[203], "LandingCinGtrans1Path", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[203]);
        MissionUtility::SetQueueFlag(false);
        }
        }
        if (mFlags[18]) {
        if (!MissionUtility::IsCinRunning(mHandles[16])) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::RemoveObject(mHandles[132]);
        MissionUtility::RemoveObject(mHandles[203]);
        mHandles[12] = MissionUtility::CreateObject("rep_tank_gtrans", "mace_spawn_1b", "transport1", 1, -1, -1);
        MissionUtility::Stop(mHandles[12]);
        MissionUtility::SetMaxHealth(mHandles[12], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[12], 999999.0f);
        MissionUtility::AddHealthBar(mHandles[12], "", 400.0f);
        mHandles[13] = MissionUtility::AddAmmoBox("spawn_ammo_here", 0, -1.0f);
        MissionUtility::AddHealthBox("spawn_health_here", 0, -1.0f);
        MissionUtility::PlayMusic("EP1_V1_T15", true);
        MissionUtility::SetWeaponOrd(mHandles[77], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[159] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_initial1", 0, "reinforcer1", 2, -1);
        mHandles[160] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_initial2", 0, "reinforcer2", 2, -1);
        MissionUtility::Goto(mHandles[159], "reinforcements_initial_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[159], "reinforcers_patrol1", 200.0f, true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[160], "reinforcements_initial_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[160], "reinforcers_patrol1", 200.0f, true);
        MissionUtility::SetQueueFlag(false);
        mHandles[161] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_initial3", 0, "reinforcer3", 2, -1);
        mHandles[162] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_initial4", 0, "reinforcer4", 2, -1);
        MissionUtility::Goto(mHandles[161], "reinforcements_initial_path2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[161], "reinforcers_patrol1", 200.0f, true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[162], "reinforcements_initial_path2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[162], "reinforcers_patrol1", 200.0f, true);
        MissionUtility::SetQueueFlag(false);
        mTimes[34] = 5.0f + MissionUtility::GetTime();
        mTimes[35] = 10.0f + MissionUtility::GetTime();
        mTimes[36] = 15.0f + MissionUtility::GetTime();
        mTimes[37] = 25.0f + MissionUtility::GetTime();
        mTimes[38] = 40.0f + MissionUtility::GetTime();
        mTimes[39] = 50.0f + MissionUtility::GetTime();
        mTimes[7] = 10.0f + MissionUtility::GetTime();
        mTimes[8] = 20.0f + MissionUtility::GetTime();
        mFlags[16] = true;
        }
        }
        }
        }
        if (!mFlags[28]) {
        if (mFlags[16]) {
        mHandles[10] = MissionUtility::QueueSound("LMG02_19", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[30] = 8.0f + MissionUtility::GetTime();
        mFlags[28] = true;
        }
        }
        if (!mFlags[36]) {
        if (mFlags[28]) {
        if (MissionUtility::IsSoundPlaying(mHandles[10])) {
        if (!MissionUtility::IsPowerupAlive(mHandles[13])) {
        MissionUtility::StopSound(mHandles[10]);
        mFlags[36] = true;
        }
        }
        }
        }
        if (!mFlags[6]) {
        if (mTimes[7] < MissionUtility::GetTime()) {
        mHandles[133] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "transport_carrier2_path", 0, "landing_gunship2", 1, -1, -1);
        MissionUtility::SetCurHealth(mHandles[133], 999999.0f);
        mHandles[14] = MissionUtility::CreateObjectWithRotation("rep_tank_gtrans", "transport_carrier2_path", 0, "transport2", 1, -1, -1);
        MissionUtility::SetMaxHealth(mHandles[14], 2800.0f);
        MissionUtility::SetCurHealth(mHandles[14], 2800.0f);
        MissionUtility::CarrierAddCargo(mHandles[133], "hp_link_1", mHandles[14], "hp_link_1", true);
        MissionUtility::Stop(mHandles[14]);
        MissionUtility::Goto(mHandles[133], "transport_carrier2_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[133], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[133], "delete_point", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[6] = true;
        }
        }
        if (!mFlags[7]) {
        if (mTimes[8] < MissionUtility::GetTime()) {
        mHandles[134] = MissionUtility::CreateObjectWithRotation("rep_fly_vcarrier", "transport_carrier3_path", 0, "landing_gunship3", 1, -1, -1);
        MissionUtility::SetCurHealth(mHandles[134], 999999.0f);
        mHandles[15] = MissionUtility::CreateObjectWithRotation("rep_tank_gtrans", "transport_carrier3_path", 0, "transport3", 1, -1, -1);
        MissionUtility::SetMaxHealth(mHandles[15], 1500.0f);
        MissionUtility::SetCurHealth(mHandles[15], 1500.0f);
        MissionUtility::CarrierAddCargo(mHandles[134], "hp_link_1", mHandles[15], "hp_link_1", true);
        MissionUtility::Stop(mHandles[15]);
        MissionUtility::Goto(mHandles[134], "transport_carrier3_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::CarrierDropoff(mHandles[134], 0, 0, 80.0f);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[134], "delete_point", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[7] = true;
        }
        }
        if (!mFlags[226]) {
        if (mFlags[6]) {
        if (MissionUtility::IsDropped(mHandles[14])) {
        MissionUtility::AddHealthBar(mHandles[14], "", 400.0f);
        mFlags[226] = true;
        }
        }
        }
        if (!mFlags[227]) {
        if (mFlags[7]) {
        if (MissionUtility::IsDropped(mHandles[15])) {
        MissionUtility::AddHealthBar(mHandles[15], "", 400.0f);
        mFlags[227] = true;
        }
        }
        }
        if (!mFlags[3]) {
        if (mFlags[6]) {
        if (MissionUtility::GetDistance(mHandles[133], "delete_point") < 100.0f) {
        MissionUtility::RemoveObject(mHandles[133]);
        mFlags[3] = true;
        }
        }
        }
        if (!mFlags[4]) {
        if (mFlags[7]) {
        if (MissionUtility::GetDistance(mHandles[134], "delete_point") < 100.0f) {
        MissionUtility::RemoveObject(mHandles[134]);
        mFlags[4] = true;
        }
        }
        }
        if (!mFlags[15]) {
        if (mTimes[30] < MissionUtility::GetTime()) {
        mHandles[7] = MissionUtility::QueueSound("LMG02_23", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[15] = true;
        }
        }
        if (!mFlags[5]) {
        if (mFlags[15]) {
        if (!MissionUtility::IsSoundPlaying(mHandles[7])) {
        mInts[1] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0004");
        MissionUtility::DisplayText("missions.Geonosis1.objective.str0004", 6.0f, -1.0f);
        mFlags[5] = true;
        }
        }
        }
        if (!mFlags[162]) {
        if (mTimes[34] < MissionUtility::GetTime()) {
        mHandles[163] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn1", 0, "reinforcer5", 2, -1);
        mHandles[164] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn2", 0, "reinforcer6", 2, -1);
        MissionUtility::Goto(mHandles[163], "reinforcements12_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[163], mHandles[77], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[164], "reinforcements12_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[164], mHandles[77], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[162] = true;
        }
        }
        if (!mFlags[163]) {
        if (mTimes[35] < MissionUtility::GetTime()) {
        mHandles[165] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn3", 0, "reinforcer7", 2, -1);
        mHandles[166] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn4", 0, "reinforcer8", 2, -1);
        MissionUtility::Goto(mHandles[165], "reinforcements34_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[165], mHandles[14], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[166], "reinforcements34_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[166], mHandles[14], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[163] = true;
        }
        }
        if (!mFlags[164]) {
        if (mTimes[36] < MissionUtility::GetTime()) {
        mHandles[167] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn1", 0, "reinforcer9", 2, -1);
        mHandles[168] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn2", 0, "reinforcer10", 2, -1);
        MissionUtility::Goto(mHandles[167], "reinforcements12_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[167], mHandles[14], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[168], "reinforcements12_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[168], mHandles[14], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mHandles[175] = MissionUtility::CreateObject("cis_tank_wheeled", "wheel_spawn2", 0, "reinforcer_wheel1", 2, -1);
        MissionUtility::Goto(mHandles[175], "wheel_path2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[175], mHandles[11], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocMinimumFly(mHandles[193], 60.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[193], 60.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[193], 60.0f);
        mFlags[164] = true;
        }
        }
        if (!mFlags[165]) {
        if (mTimes[37] < MissionUtility::GetTime()) {
        mHandles[170] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn4", 0, "reinforcer12", 2, -1);
        MissionUtility::Goto(mHandles[170], "reinforcements34_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[170], "reinforcers_patrol1", 200.0f, true);
        MissionUtility::SetQueueFlag(false);
        mHandles[176] = MissionUtility::CreateObject("cis_tank_wheeled", "wheel_spawn1", 0, "reinforcer_wheel2", 2, -1);
        MissionUtility::Goto(mHandles[176], "wheel_path1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[176], mHandles[11], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[165] = true;
        }
        }
        if (!mFlags[166]) {
        if (mTimes[38] < MissionUtility::GetTime()) {
        mHandles[171] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn1", 0, "reinforcer13", 2, -1);
        mHandles[172] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn2", 0, "reinforcer14", 2, -1);
        MissionUtility::Goto(mHandles[171], "reinforcements12_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[171], "reinforcers_patrol1", 200.0f, true);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[172], "reinforcements12_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[172], "reinforcers_patrol1", 200.0f, true);
        MissionUtility::SetQueueFlag(false);
        mHandles[178] = MissionUtility::CreateObject("cis_tank_wheeled", "wheel_spawn1", 0, "reinforcer_wheel4", 2, -1);
        MissionUtility::Goto(mHandles[178], "wheel_path1", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[178], mHandles[11], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[166] = true;
        }
        }
        if (!mFlags[167]) {
        if (mTimes[39] < MissionUtility::GetTime()) {
        mHandles[173] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn3", 0, "reinforcer15", 2, -1);
        mHandles[174] = MissionUtility::CreateObject("cis_tank_fighter", "reinforcements_spawn4", 0, "reinforcer16", 2, -1);
        MissionUtility::Goto(mHandles[173], "reinforcements34_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[173], mHandles[15], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[174], "reinforcements34_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[174], mHandles[15], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mHandles[180] = MissionUtility::CreateObject("cis_tank_wheeled", "wheel_spawn2", 0, "reinforcer_wheel6", 2, -1);
        MissionUtility::Goto(mHandles[180], "wheel_path2", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[180], mHandles[11], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mHandles[193] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "powerup_gunship_spawn", 0, "", 1, -1, 0);
        MissionUtility::Goto(mHandles[193], "powerup_gunship_path", true);
        mFlags[167] = true;
        }
        }
        if (!mFlags[183]) {
        if (mFlags[167]) {
        if (MissionUtility::IsInsideRegion(mHandles[193], "drop")) {
        MissionUtility::DropAmmoBox(mHandles[193]);
        MissionUtility::QueueSound("CTT22_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[41] = 0.5f + MissionUtility::GetTime();
        mFlags[183] = true;
        }
        }
        }
        if (!mFlags[184]) {
        if (mTimes[41] < MissionUtility::GetTime()) {
        MissionUtility::DropHealthBox(mHandles[193]);
        mFlags[184] = true;
        }
        }
        if (!mFlags[9]) {
        if (mFlags[164]) {
        if (MissionUtility::GetDistance(mHandles[193], "powerup_gunship_path", 8) < 100.0f) {
        MissionUtility::RemoveObject(mHandles[193]);
        mFlags[9] = true;
        }
        }
        }
        if (!mFlags[185]) {
        if (MissionUtility::IsAlive(mHandles[159])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[159])) {
        MissionUtility::AttackTarget(mHandles[159], mHandles[11], true, true, false, false);
        mFlags[185] = true;
        }
        }
        }
        if (!mFlags[186]) {
        if (MissionUtility::IsAlive(mHandles[160])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[160])) {
        MissionUtility::AttackTarget(mHandles[160], mHandles[11], true, true, false, false);
        mFlags[186] = true;
        }
        }
        }
        if (!mFlags[187]) {
        if (MissionUtility::IsAlive(mHandles[161])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[161])) {
        MissionUtility::AttackTarget(mHandles[161], mHandles[11], true, true, false, false);
        mFlags[187] = true;
        }
        }
        }
        if (!mFlags[188]) {
        if (MissionUtility::IsAlive(mHandles[162])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[162])) {
        MissionUtility::AttackTarget(mHandles[162], mHandles[11], true, true, false, false);
        mFlags[188] = true;
        }
        }
        }
        if (!mFlags[189]) {
        if (MissionUtility::IsAlive(mHandles[163])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[163])) {
        MissionUtility::AttackTarget(mHandles[163], mHandles[11], true, true, false, false);
        mFlags[189] = true;
        }
        }
        }
        if (!mFlags[190]) {
        if (MissionUtility::IsAlive(mHandles[164])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[164])) {
        MissionUtility::AttackTarget(mHandles[164], mHandles[11], true, true, false, false);
        mFlags[190] = true;
        }
        }
        }
        if (!mFlags[191]) {
        if (MissionUtility::IsAlive(mHandles[165])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[165])) {
        MissionUtility::AttackTarget(mHandles[165], mHandles[11], true, true, false, false);
        mFlags[191] = true;
        }
        }
        }
        if (!mFlags[192]) {
        if (MissionUtility::IsAlive(mHandles[166])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[166])) {
        MissionUtility::AttackTarget(mHandles[166], mHandles[11], true, true, false, false);
        mFlags[192] = true;
        }
        }
        }
        if (!mFlags[193]) {
        if (MissionUtility::IsAlive(mHandles[167])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[167])) {
        MissionUtility::AttackTarget(mHandles[167], mHandles[11], true, true, false, false);
        mFlags[193] = true;
        }
        }
        }
        if (!mFlags[194]) {
        if (MissionUtility::IsAlive(mHandles[168])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[168])) {
        MissionUtility::AttackTarget(mHandles[168], mHandles[11], true, true, false, false);
        mFlags[194] = true;
        }
        }
        }
        if (!mFlags[196]) {
        if (MissionUtility::IsAlive(mHandles[170])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[170])) {
        MissionUtility::AttackTarget(mHandles[170], mHandles[11], true, true, false, false);
        mFlags[196] = true;
        }
        }
        }
        if (!mFlags[197]) {
        if (MissionUtility::IsAlive(mHandles[171])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[171])) {
        MissionUtility::AttackTarget(mHandles[171], mHandles[11], true, true, false, false);
        mFlags[197] = true;
        }
        }
        }
        if (!mFlags[198]) {
        if (MissionUtility::IsAlive(mHandles[172])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[172])) {
        MissionUtility::AttackTarget(mHandles[172], mHandles[11], true, true, false, false);
        mFlags[198] = true;
        }
        }
        }
        if (!mFlags[199]) {
        if (MissionUtility::IsAlive(mHandles[173])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[173])) {
        MissionUtility::AttackTarget(mHandles[173], mHandles[11], true, true, false, false);
        mFlags[197] = true;
        }
        }
        }
        if (!mFlags[200]) {
        if (MissionUtility::IsAlive(mHandles[174])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[174])) {
        MissionUtility::AttackTarget(mHandles[174], mHandles[11], true, true, false, false);
        mFlags[200] = true;
        }
        }
        }
        if (!mFlags[205]) {
        if (MissionUtility::IsAlive(mHandles[175])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[175])) {
        MissionUtility::AttackTarget(mHandles[175], mHandles[11], true, true, false, false);
        mFlags[205] = true;
        }
        }
        }
        if (!mFlags[206]) {
        if (MissionUtility::IsAlive(mHandles[176])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[176])) {
        MissionUtility::AttackTarget(mHandles[176], mHandles[11], true, true, false, false);
        mFlags[206] = true;
        }
        }
        }
        if (!mFlags[208]) {
        if (MissionUtility::IsAlive(mHandles[178])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[178])) {
        MissionUtility::AttackTarget(mHandles[178], mHandles[11], true, true, false, false);
        mFlags[208] = true;
        }
        }
        }
        if (!mFlags[210]) {
        if (MissionUtility::IsAlive(mHandles[180])) {
        if (mHandles[11] == MissionUtility::GetWhoShotMe(mHandles[180])) {
        MissionUtility::AttackTarget(mHandles[180], mHandles[11], true, true, false, false);
        mFlags[210] = true;
        }
        }
        }
        if (!mFlags[25]) {
        if (mFlags[14]) {
        if (!mFlags[177]) {
        if (MissionUtility::GetDistance(mHandles[77], "put_lum_here") > 400.0f) {
        MissionUtility::Goto(mHandles[77], "put_lum_here", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Wait(mHandles[77]);
        MissionUtility::SetQueueFlag(false);
        mFlags[177] = true;
        mFlags[178] = false;
        }
        }
        if (!mFlags[178]) {
        if (mFlags[177]) {
        if (MissionUtility::GetDistance(mHandles[77], "put_lum_here") < 50.0f) {
        mFlags[177] = false;
        mFlags[178] = true;
        }
        }
        }
        }
        }
        if (!mFlags[25]) {
        if (mFlags[167]) {
        if (!MissionUtility::IsAlive(mHandles[159])) {
        if (!MissionUtility::IsAlive(mHandles[160])) {
        if (!MissionUtility::IsAlive(mHandles[161])) {
        if (!MissionUtility::IsAlive(mHandles[162])) {
        if (!MissionUtility::IsAlive(mHandles[163])) {
        if (!MissionUtility::IsAlive(mHandles[164])) {
        if (!MissionUtility::IsAlive(mHandles[165])) {
        if (!MissionUtility::IsAlive(mHandles[166])) {
        if (!MissionUtility::IsAlive(mHandles[167])) {
        if (!MissionUtility::IsAlive(mHandles[168])) {
        if (!MissionUtility::IsAlive(mHandles[170])) {
        if (!MissionUtility::IsAlive(mHandles[171])) {
        if (!MissionUtility::IsAlive(mHandles[172])) {
        if (!MissionUtility::IsAlive(mHandles[173])) {
        if (!MissionUtility::IsAlive(mHandles[174])) {
        if (!MissionUtility::IsAlive(mHandles[175])) {
        if (!MissionUtility::IsAlive(mHandles[176])) {
        if (!MissionUtility::IsAlive(mHandles[178])) {
        if (!MissionUtility::IsAlive(mHandles[180])) {
        MissionUtility::RemoveFlock(mHandles[184], true);
        MissionUtility::RemoveFlock(mHandles[185], true);
        MissionUtility::RemoveFlock(mHandles[186], true);
        MissionUtility::RemoveFlock(mHandles[187], true);
        MissionUtility::RemoveFlock(mHandles[188], true);
        MissionUtility::RemoveFlock(mHandles[189], true);
        MissionUtility::RemoveObjectify(mHandles[128]);
        mTimes[29] = 1.5f + MissionUtility::GetTime();
        MissionUtility::SetWeaponOrd(mHandles[77], "rep_blaster_fighter1", "rep_blaster_fighter1_ord");
        MissionUtility::ObjectiveComplete(mInts[1]);
        MissionUtility::RemoveTurnAroundRegion("turnaround2");
        mFlags[25] = true;
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
        if (mTimes[29] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("LMG02_03", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[32] = 2.0f + MissionUtility::GetTime();
        mTimes[29] = 999999.9f;
        }
        if (mTimes[32] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("MWG02_04", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[47] = 4.0f + MissionUtility::GetTime();
        mTimes[32] = 999999.9f;
        }
        if (!mFlags[17]) {
        if (mTimes[47] < MissionUtility::GetTime()) {
        mTimes[1] = 4.0f + MissionUtility::GetTime();
        MissionUtility::Objectify(mHandles[12], "missions.Geonosis1.marker.str0001", false, true, 0.0f);
        MissionUtility::EvictConfig("rep_fly_vcarrier");
        mFlags[17] = true;
        }
        }
        if (!mFlags[20]) {
        if (mTimes[1] < MissionUtility::GetTime()) {
        MissionUtility::SetMaxHealth(mHandles[12], 3000.0f);
        MissionUtility::SetCurHealth(mHandles[12], 3000.0f);
        mInts[2] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0001");
        MissionUtility::DisplayText("missions.Geonosis1.text.str0001", 6.0f, -1.0f);
        mHandles[109] = MissionUtility::CreateFlock(mHandles[12], (Formation)8);
        MissionUtility::AddFlockMember(mHandles[109], mHandles[14], Vector(0.0f, 0.0f, 10.0f));
        MissionUtility::AddFlockMember(mHandles[109], mHandles[15], Vector(0.0f, 0.0f, 70.0f));
        MissionUtility::Goto(mHandles[109], "convoy_path", true);
        MissionUtility::SetVelocForward(mHandles[12], 37.0f);
        MissionUtility::SetVelocForward(mHandles[14], 37.0f);
        MissionUtility::SetVelocForward(mHandles[15], 37.0f);
        MissionUtility::Defend(mHandles[77], mHandles[12], 250.0f);
        MissionUtility::SetAttackRange(mHandles[77], 300);
        mTimes[3] = MissionUtility::GetCurHealth(mHandles[77]);
        if (MissionUtility::IsAlive(mHandles[12])) {
        mTimes[4] = MissionUtility::GetCurHealth(mHandles[12]);
        mFlags[62] = true;
        }
        if (MissionUtility::IsAlive(mHandles[14])) {
        mTimes[5] = MissionUtility::GetCurHealth(mHandles[14]);
        mFlags[63] = true;
        }
        if (MissionUtility::IsAlive(mHandles[15])) {
        mTimes[6] = MissionUtility::GetCurHealth(mHandles[15]);
        mFlags[64] = true;
        }
        if (!mFlags[234]) {
        if (!MissionUtility::IsAlive(mHandles[43])) {
        mFlags[234] = true;
        }
        }
        if (!mFlags[235]) {
        if (!MissionUtility::IsAlive(mHandles[44])) {
        mFlags[235] = true;
        }
        }
        if (!mFlags[236]) {
        if (!MissionUtility::IsAlive(mHandles[45])) {
        mFlags[236] = true;
        }
        }
        if (!mFlags[237]) {
        if (!MissionUtility::IsAlive(mHandles[46])) {
        mFlags[237] = true;
        }
        }
        if (!mFlags[238]) {
        if (!MissionUtility::IsAlive(mHandles[47])) {
        mFlags[238] = true;
        }
        }
        if (!mFlags[239]) {
        if (!MissionUtility::IsAlive(mHandles[48])) {
        mFlags[239] = true;
        }
        }
        if (!mFlags[240]) {
        if (!MissionUtility::IsAlive(mHandles[49])) {
        mFlags[240] = true;
        }
        }
        if (!mFlags[241]) {
        if (!MissionUtility::IsAlive(mHandles[50])) {
        mFlags[241] = true;
        }
        }
        if (!mFlags[242]) {
        if (!MissionUtility::IsAlive(mHandles[4])) {
        mFlags[242] = true;
        }
        }
        MissionUtility::MidMissionSavePlayer(1);
        MissionUtility::MidMissionSave(mTimes[3]);
        MissionUtility::MidMissionSave(mTimes[4]);
        MissionUtility::MidMissionSave(mTimes[5]);
        MissionUtility::MidMissionSave(mTimes[6]);
        MissionUtility::MidMissionSave((bool)mFlags[62]);
        MissionUtility::MidMissionSave((bool)mFlags[63]);
        MissionUtility::MidMissionSave((bool)mFlags[64]);
        MissionUtility::MidMissionSave((bool)mFlags[134]);
        MissionUtility::MidMissionSave((bool)mFlags[135]);
        MissionUtility::MidMissionSave((bool)mFlags[136]);
        MissionUtility::MidMissionSave(mInts[16]);
        MissionUtility::MidMissionSave((bool)mFlags[48]);
        MissionUtility::MidMissionSave((bool)mFlags[51]);
        MissionUtility::MidMissionSave((bool)mFlags[52]);
        MissionUtility::MidMissionSave((bool)mFlags[234]);
        MissionUtility::MidMissionSave((bool)mFlags[235]);
        MissionUtility::MidMissionSave((bool)mFlags[236]);
        MissionUtility::MidMissionSave((bool)mFlags[237]);
        MissionUtility::MidMissionSave((bool)mFlags[238]);
        MissionUtility::MidMissionSave((bool)mFlags[239]);
        MissionUtility::MidMissionSave((bool)mFlags[240]);
        MissionUtility::MidMissionSave((bool)mFlags[241]);
        MissionUtility::MidMissionSave((bool)mFlags[242]);
        MissionUtility::MidMissionSave((bool)mFlags[0]);
        MissionUtility::MidMissionSave((bool)mFlags[1]);
        mFlags[201] = true;
        mFlags[20] = true;
        }
        }
    }

    // ---- +0x3c14  12676 bytes ----
    if (mFlags[201]) {
        if (!mFlags[204]) {
        MissionUtility::QueueSound("LMG02_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        if (MissionUtility::IsAlive(mHandles[12])) {
        if (MissionUtility::IsAlive(mHandles[14])) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::QueueSound("LMG02_28", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        }
        }
        mHandles[79] = MissionUtility::CreateObject("cis_tank_fighter", "first_guy1", "", 3, -1, -1);
        mHandles[80] = MissionUtility::CreateObject("cis_tank_fighter", "first_guy2", "", 3, -1, -1);
        MissionUtility::SetMaxHealth(mHandles[75], 600.0f);
        MissionUtility::SetCurHealth(mHandles[75], 600.0f);
        MissionUtility::SetMaxHealth(mHandles[76], 600.0f);
        MissionUtility::SetCurHealth(mHandles[76], 600.0f);
        MissionUtility::SetApplyDynamics(mHandles[75], true);
        MissionUtility::SetApplyDynamics(mHandles[76], true);
        MissionUtility::AddTurnAroundRegion("turnaround1", "turnaround1_point1", 0, 0, 0, 0);
        MissionUtility::AddTurnAroundRegion("turnaround4", "turnaround4_point1", "turnaround4_point2", "turnaround4_point3", 0, 0);
        MissionUtility::PlayMusic("EP1_V1_T15", true);
        mFlags[204] = true;
        }
        if (!mFlags[232]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "dust_off")) {
        MissionUtility::FadeOutDust(4.0f);
        mFlags[233] = false;
        mFlags[232] = true;
        }
        }
        if (!mFlags[233]) {
        if (mFlags[232]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "dust_on")) {
        MissionUtility::FadeInDust(4.0f);
        mFlags[232] = false;
        mFlags[233] = true;
        }
        }
        }
        if (mTimes[43] < MissionUtility::GetTime()) {
        if (!mFlags[159]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "turnaround1")) {
        MissionUtility::QueueSound("LMG02_30", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[43] = 8.0f + MissionUtility::GetTime();
        }
        }
        }
        if (mTimes[46] < MissionUtility::GetTime()) {
        if (!mFlags[170]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "turnaround4")) {
        MissionUtility::QueueSound("LMG02_30", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[46] = 8.0f + MissionUtility::GetTime();
        }
        }
        }
        if (!mFlags[143]) {
        if (!MissionUtility::IsAlive(mHandles[12])) {
        if (MissionUtility::IsAlive(mHandles[14])) {
        MissionUtility::Objectify(mHandles[14], "missions.Geonosis1.marker.str0001", false, true, 0.0f);
        mFlags[145] = true;
        }
        if (!mFlags[145]) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::Objectify(mHandles[15], "missions.Geonosis1.marker.str0001", false, true, 0.0f);
        mFlags[146] = true;
        }
        }
        mFlags[143] = true;
        }
        }
        if (!mFlags[144]) {
        if (mFlags[143]) {
        if (mFlags[145]) {
        if (!MissionUtility::IsAlive(mHandles[14])) {
        MissionUtility::Objectify(mHandles[15], "missions.Geonosis1.marker.str0001", false, true, 0.0f);
        mFlags[144] = true;
        }
        }
        if (mFlags[146]) {
        if (!MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::Objectify(mHandles[14], "missions.Geonosis1.marker.str0001", false, true, 0.0f);
        mFlags[144] = true;
        }
        }
        }
        }
        if (!mFlags[121]) {
        if (mFlags[16]) {
        if (!mFlags[23]) {
        if (!MissionUtility::IsAlive(mHandles[11])) {
        MissionUtility::BonusObjectiveFailed(mInts[2]);
        mTimes[2] = 3.0f + MissionUtility::GetTime();
        mFlags[121] = true;
        }
        }
        }
        }
        if (!mFlags[154]) {
        if (!MissionUtility::IsAlive(mHandles[12])) {
        if (MissionUtility::IsAlive(mHandles[14])) {
        MissionUtility::Defend(mHandles[77], mHandles[14], 250.0f);
        mFlags[156] = true;
        }
        if (!mFlags[156]) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::Defend(mHandles[77], mHandles[15], 250.0f);
        mFlags[157] = true;
        }
        }
        mFlags[154] = true;
        }
        }
        if (!mFlags[155]) {
        if (mFlags[154]) {
        if (mFlags[156]) {
        if (!MissionUtility::IsAlive(mHandles[14])) {
        MissionUtility::Defend(mHandles[77], mHandles[15], 250.0f);
        mFlags[155] = true;
        }
        }
        if (mFlags[157]) {
        if (!MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::Defend(mHandles[77], mHandles[14], 250.0f);
        mFlags[155] = true;
        }
        }
        }
        }
        if (!mFlags[54]) {
        if (MissionUtility::IsInsideRegion(mHandles[109], "walker_cin_trigger")) {
        mFlags[54] = true;
        }
        }
        if (!mFlags[158]) {
        if (mFlags[54]) {
        if (!mFlags[159]) {
        mTimes[26] = 1.5f + MissionUtility::GetTime();
        mHandles[158] = MissionUtility::RunCin("walker_cin", true, true);
        mHandles[152] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "walker_spawn1", 0, "walker1", 2, -1, -1);
        mHandles[153] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "walker_spawn2", 0, "walker2", 2, -1, -1);
        mHandles[154] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "walker_spawn3", 0, "walker3", 2, -1, -1);
        mHandles[155] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "walker_spawn4", 0, "walker4", 2, -1, -1);
        mHandles[223] = MissionUtility::CreateObjectWithRotation("rep_inf_mace_cin", "WalkerCinMacePath", 0, "WalkerCinMace", 0, -1, -1);
        MissionUtility::Goto(mHandles[223], "WalkerCinMacePath", false);
        mHandles[225] = MissionUtility::CreateObjectWithRotation("rep_inf_luminara_cin", "WalkerCinLuminaraPath", 0, "WalkerCinLuminara", 0, -1, -1);
        MissionUtility::Goto(mHandles[225], "WalkerCinLuminaraPath", false);
        MissionUtility::OverrideSoundRange(mHandles[152], true);
        MissionUtility::OverrideSoundRange(mHandles[153], true);
        MissionUtility::OverrideSoundRange(mHandles[154], true);
        MissionUtility::OverrideSoundRange(mHandles[155], true);
        MissionUtility::OverrideSoundRange(mHandles[223], true);
        MissionUtility::OverrideSoundRange(mHandles[225], true);
        MissionUtility::OverrideSoundRange(mHandles[11], true);
        MissionUtility::OverrideSoundRange(mHandles[77], true);
        MissionUtility::MoveObjectWithRotation(mHandles[11], "walker_cin_move_player", 0, true);
        MissionUtility::Wait(mHandles[77]);
        MissionUtility::MoveObjectWithRotation(mHandles[77], "walker_cin_move_lum", 0, true);
        MissionUtility::Stop(mHandles[77]);
        MissionUtility::SetAlliance(1, 2);
        if (MissionUtility::IsAlive(mHandles[19])) {
        MissionUtility::DamageObject(mHandles[19], 999999.0f, 999999.0f);
        }
        if (MissionUtility::IsAlive(mHandles[20])) {
        MissionUtility::DamageObject(mHandles[20], 999999.0f, 999999.0f);
        }
        if (MissionUtility::IsAlive(mHandles[27])) {
        MissionUtility::DamageObject(mHandles[27], 999999.0f, 999999.0f);
        }
        if (MissionUtility::IsAlive(mHandles[28])) {
        MissionUtility::DamageObject(mHandles[28], 999999.0f, 999999.0f);
        }
        if (MissionUtility::IsAlive(mHandles[79])) {
        MissionUtility::DamageObject(mHandles[79], 999999.0f, 999999.0f);
        }
        if (MissionUtility::IsAlive(mHandles[80])) {
        MissionUtility::DamageObject(mHandles[80], 999999.0f, 999999.0f);
        }
        MissionUtility::RemoveTurnAroundRegion("turnaround1");
        MissionUtility::PlayMusic("EP5_V1_T05_02", true);
        mFlags[159] = true;
        }
        if (!mFlags[231]) {
        if (mTimes[26] < MissionUtility::GetTime()) {
        MissionUtility::SetVelocForward(mHandles[109], 0.001f);
        MissionUtility::SetVelocForward(mHandles[12], 0.001f);
        MissionUtility::SetVelocForward(mHandles[14], 0.001f);
        MissionUtility::SetVelocForward(mHandles[15], 0.001f);
        mTimes[31] = 4.0f + MissionUtility::GetTime();
        mTimes[26] = 999999.9f;
        }
        }
        if (!mFlags[231]) {
        if (!mFlags[169]) {
        if (MissionUtility::GetCinId(mHandles[158]) == 2) {
        mHandles[224] = MissionUtility::CreateObjectWithRotation("rep_inf_mace", "WalkerCinMace1Path", 0, "WalkerCinMace1", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[224], true);
        MissionUtility::Goto(mHandles[224], "WalkerCinMace1Path", true);
        MissionUtility::SetFOV(8.0f);
        MissionUtility::Goto(mHandles[152], "walker1_path", true);
        MissionUtility::Goto(mHandles[153], "walker2_path", true);
        MissionUtility::Goto(mHandles[154], "walker3_path", true);
        MissionUtility::Goto(mHandles[155], "walker4_path", true);
        BeginTimer(mTimer9);
        mFlags[169] = true;
        }
        }
        }
        if (!mFlags[231]) {
        if (!mFlags[168]) {
        if (mTimes[31] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("LMG02_34", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[168] = true;
        }
        }
        }
        if (!mFlags[231]) {
        if (!mFlags[224]) {
        if (MissionUtility::GetCinId(mHandles[158]) == 3) {
        mFlags[224] = true;
        }
        }
        }
        if (!mFlags[231]) {
        if (mTimer8 > 1.0f) {
        StopTimer(mTimer8);
        mTimer8 = 0.0f;
        MissionUtility::Goto(mHandles[225], mHandles[77]);
        }
        }
        if (!mFlags[231]) {
        if (mTimer9 > 5.0f) {
        StopTimer(mTimer9);
        mTimer9 = 0.0f;
        MissionUtility::Goto(mHandles[223], mHandles[11]);
        MissionUtility::Goto(mHandles[225], mHandles[77]);
        }
        }
        if (mFlags[159]) {
        if (!MissionUtility::IsCinRunning(mHandles[158])) {
        MissionUtility::OverrideSoundRange(mHandles[11], false);
        MissionUtility::OverrideSoundRange(mHandles[77], false);
        mFlags[231] = true;
        MissionUtility::FlushSoundQueue();
        MissionUtility::SetFOV(55.4f);
        MissionUtility::AttackTarget(mHandles[77], mHandles[153], true, true, false, false);
        MissionUtility::RemoveObject(mHandles[223]);
        MissionUtility::RemoveObject(mHandles[224]);
        MissionUtility::RemoveObject(mHandles[225]);
        MissionUtility::SetEnemies(1, 2);
        if (!mFlags[169]) {
        MissionUtility::Patrol(mHandles[152], "walker1_path", 500.0f, true);
        MissionUtility::Patrol(mHandles[153], "walker2_path", 500.0f, true);
        MissionUtility::Patrol(mHandles[154], "walker3_path", 500.0f, true);
        MissionUtility::Patrol(mHandles[155], "walker4_path", 500.0f, true);
        }
        MissionUtility::SetVelocForward(mHandles[109], 0.001f);
        MissionUtility::SetVelocForward(mHandles[12], 0.001f);
        MissionUtility::SetVelocForward(mHandles[14], 0.001f);
        MissionUtility::SetVelocForward(mHandles[15], 0.001f);
        mFlags[158] = true;
        }
        }
        }
        }
        if (mFlags[158]) {
        if (!mFlags[170]) {
        if (MissionUtility::IsAlive(mHandles[152])) {
        if (!mFlags[179]) {
        if (MissionUtility::GetDistance(mHandles[152], mHandles[109]) < 175.0f) {
        MissionUtility::AttackTarget(mHandles[152], mHandles[109], true, true, false, false);
        mFlags[179] = true;
        }
        }
        }
        if (MissionUtility::IsAlive(mHandles[153])) {
        if (!mFlags[180]) {
        if (MissionUtility::GetDistance(mHandles[153], mHandles[109]) < 175.0f) {
        MissionUtility::AttackTarget(mHandles[153], mHandles[109], true, true, false, false);
        mFlags[180] = true;
        }
        }
        }
        if (MissionUtility::IsAlive(mHandles[154])) {
        if (!mFlags[181]) {
        if (!mFlags[211]) {
        if (MissionUtility::GetDistance(mHandles[154], mHandles[109]) < 175.0f) {
        MissionUtility::AttackTarget(mHandles[154], mHandles[109], true, true, false, false);
        mFlags[181] = true;
        }
        }
        }
        }
        if (MissionUtility::IsAlive(mHandles[155])) {
        if (!mFlags[182]) {
        if (MissionUtility::GetDistance(mHandles[155], mHandles[109]) < 175.0f) {
        MissionUtility::AttackTarget(mHandles[155], mHandles[109], true, true, false, false);
        mFlags[182] = true;
        }
        }
        }
        if (!mFlags[212]) {
        if (MissionUtility::GetDistance(mHandles[152], mHandles[11]) < 210.0f) {
        MissionUtility::AttackTarget(mHandles[152], mHandles[11], true, true, false, false);
        mFlags[212] = true;
        }
        }
        if (!mFlags[33]) {
        if (MissionUtility::GetDistance(mHandles[153], mHandles[11]) < 210.0f) {
        MissionUtility::AttackTarget(mHandles[153], mHandles[11], true, true, false, false);
        mFlags[33] = true;
        }
        }
        if (!mFlags[211]) {
        if (MissionUtility::GetDistance(mHandles[154], mHandles[11]) < 210.0f) {
        MissionUtility::AttackTarget(mHandles[154], mHandles[11], true, true, false, false);
        mFlags[211] = true;
        }
        }
        if (!mFlags[34]) {
        if (MissionUtility::GetDistance(mHandles[155], mHandles[11]) < 210.0f) {
        MissionUtility::AttackTarget(mHandles[155], mHandles[11], true, true, false, false);
        mFlags[34] = true;
        }
        }
        }
        }
        if (!mFlags[13]) {
        if (!MissionUtility::IsAlive(mHandles[153])) {
        if (!mFlags[160]) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[155], true, true, false, false);
        mFlags[13] = true;
        }
        }
        }
        if (!mFlags[160]) {
        if (mFlags[158]) {
        if (!MissionUtility::IsAlive(mHandles[153])) {
        if (!MissionUtility::IsAlive(mHandles[155])) {
        if (MissionUtility::IsAlive(mHandles[12])) {
        MissionUtility::Defend(mHandles[77], mHandles[12], 550.0f);
        mFlags[160] = true;
        }
        if (!mFlags[160]) {
        if (MissionUtility::IsAlive(mHandles[14])) {
        MissionUtility::Defend(mHandles[77], mHandles[14], 550.0f);
        mFlags[160] = true;
        }
        }
        if (!mFlags[160]) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::Defend(mHandles[77], mHandles[15], 550.0f);
        mFlags[160] = true;
        }
        }
        MissionUtility::SetWeaponOrd(mHandles[77], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mFlags[160] = true;
        }
        }
        }
        }
        if (!mFlags[170]) {
        if (mFlags[158]) {
        if (!MissionUtility::IsAlive(mHandles[152])) {
        if (!MissionUtility::IsAlive(mHandles[153])) {
        if (!MissionUtility::IsAlive(mHandles[154])) {
        if (!MissionUtility::IsAlive(mHandles[155])) {
        mTimes[28] = 2.0f + MissionUtility::GetTime();
        MissionUtility::SetWeaponOrd(mHandles[77], "rep_blaster_fighter1", "rep_blaster_fighter1_ord");
        MissionUtility::RemoveTurnAroundRegion("turnaround4");
        mHandles[156] = MissionUtility::CreateObjectWithRotation("cis_walk_assault", "walker5_spawn", 0, "walker5", 2, -1, -1);
        mFlags[170] = true;
        }
        }
        }
        }
        }
        }
        if (!mFlags[171]) {
        if (mTimes[28] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("LMG02_35", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetVelocForward(mHandles[109], 37.0f);
        MissionUtility::SetVelocForward(mHandles[12], 37.0f);
        MissionUtility::SetVelocForward(mHandles[14], 37.0f);
        MissionUtility::SetVelocForward(mHandles[15], 37.0f);
        mFlags[171] = true;
        }
        }
        if (!mFlags[213]) {
        if (mFlags[170]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "turnaround4")) {
        mHandles[181] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "end_wheel1_spawn", 0, "", 2, -1, -1);
        MissionUtility::AttackTarget(mHandles[181], mHandles[11], true, true, false, false);
        mFlags[213] = true;
        }
        }
        }
        if (!mFlags[38]) {
        if (!mFlags[103]) {
        mHandles[19] = MissionUtility::CreateObject("cis_tank_fighter", "convoy_attacker_spawn1", "convoy_attacker1", 3, -1, -1);
        mHandles[20] = MissionUtility::CreateObject("cis_tank_fighter", "convoy_attacker_spawn2", "convoy_attacker2", 3, -1, -1);
        mHandles[99] = MissionUtility::CreateFlock(mHandles[19], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[99], mHandles[20]);
        MissionUtility::Patrol(mHandles[99], "convoy_attacker_path1", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[19], 200);
        MissionUtility::SetAttackRange(mHandles[20], 200);
        mHandles[190] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "convoy_wheel_spawn_front", 0, "", 2, -1, -1);
        MissionUtility::Goto(mHandles[190], "convoy_attacker_path1", true);
        mFlags[103] = true;
        }
        if (!mFlags[106]) {
        if (MissionUtility::GetDistance(mHandles[109], "trigger_front_attack4") < 150.0f) {
        mHandles[111] = MissionUtility::CreateObject("cis_tank_fighter", "convoy_attacker_spawn1", "convoy_attacker7", 3, -1, -1);
        mHandles[112] = MissionUtility::CreateObject("cis_tank_fighter", "convoy_attacker_spawn2", "convoy_attacker8", 3, -1, -1);
        mHandles[105] = MissionUtility::CreateFlock(mHandles[111], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[105], mHandles[112]);
        MissionUtility::Patrol(mHandles[105], "convoy_attacker_path1", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[111], 200);
        MissionUtility::SetAttackRange(mHandles[112], 200);
        mFlags[106] = true;
        }
        }
        if (!mFlags[107]) {
        if (MissionUtility::GetDistance(mHandles[109], "trigger_front_attack5") < 150.0f) {
        mHandles[113] = MissionUtility::CreateObject("cis_tank_fighter", "convoy_attacker_spawn1", "convoy_attacker9", 3, -1, -1);
        mHandles[114] = MissionUtility::CreateObject("cis_tank_fighter", "convoy_attacker_spawn2", "convoy_attacker10", 3, -1, -1);
        mHandles[106] = MissionUtility::CreateFlock(mHandles[113], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[106], mHandles[114]);
        MissionUtility::Patrol(mHandles[106], "convoy_attacker_path1", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[113], 200);
        MissionUtility::SetAttackRange(mHandles[114], 200);
        mHandles[191] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "convoy_wheel_spawn_front", 0, "", 2, -1, -1);
        MissionUtility::Goto(mHandles[191], "convoy_attacker_path1", true);
        mFlags[107] = true;
        }
        }
        if (!mFlags[47]) {
        if (MissionUtility::GetDistance(mHandles[109], "trigger_front_attack7") < 150.0f) {
        mHandles[117] = MissionUtility::CreateObject("cis_tank_fighter", "convoy_attacker_spawn1", "convoy_attacker13", 3, -1, -1);
        mHandles[118] = MissionUtility::CreateObject("cis_tank_fighter", "convoy_attacker_spawn2", "convoy_attacker14", 3, -1, -1);
        mHandles[108] = MissionUtility::CreateFlock(mHandles[117], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[108], mHandles[118]);
        MissionUtility::Patrol(mHandles[108], "convoy_attacker_path1", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[117], 200);
        MissionUtility::SetAttackRange(mHandles[118], 200);
        mFlags[47] = true;
        }
        }
        }
        if (!mFlags[32]) {
        if (MissionUtility::GetDistance(mHandles[109], "trigger_behind_attack1") < 150.0f) {
        mHandles[27] = MissionUtility::CreateObject("cis_tank_fighter", "behind_attack1_spawn1", "behind_attacker1", 3, -1, -1);
        mHandles[28] = MissionUtility::CreateObject("cis_tank_fighter", "behind_attack1_spawn2", "behind_attacker2", 3, -1, -1);
        mHandles[100] = MissionUtility::CreateFlock(mHandles[27], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[100], mHandles[28]);
        MissionUtility::Patrol(mHandles[100], "behind_attack1_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[27], 250);
        MissionUtility::SetAttackRange(mHandles[28], 250);
        mFlags[32] = true;
        }
        }
        if (MissionUtility::IsFlockAlive(mHandles[100])) {
        if (!mFlags[71]) {
        if (MissionUtility::GetDistance(mHandles[109], mHandles[100]) < 250.0f) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::AttackTarget(mHandles[100], mHandles[15], true, true, false, false);
        }
        if (!MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::AttackTarget(mHandles[100], mHandles[14], true, true, false, false);
        }
        mFlags[71] = true;
        }
        }
        }
        if (!mFlags[35]) {
        if (MissionUtility::GetDistance(mHandles[109], "trigger_behind_attack2") < 150.0f) {
        mHandles[29] = MissionUtility::CreateObject("cis_tank_fighter", "behind_attack2_spawn1", "behind_attacker3", 3, -1, -1);
        mHandles[30] = MissionUtility::CreateObject("cis_tank_fighter", "behind_attack2_spawn2", "behind_attacker4", 3, -1, -1);
        mHandles[101] = MissionUtility::CreateFlock(mHandles[29], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[101], mHandles[30]);
        MissionUtility::Patrol(mHandles[101], "behind_attack2_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[29], 250);
        MissionUtility::SetAttackRange(mHandles[30], 250);
        mHandles[192] = MissionUtility::CreateObjectWithRotation("cis_tank_wheeled", "convoy_wheel_spawn_behind", 0, "", 2, -1, -1);
        MissionUtility::Goto(mHandles[192], "behind_attack1_path", true);
        mFlags[35] = true;
        }
        }
        if (MissionUtility::IsFlockAlive(mHandles[101])) {
        if (!mFlags[72]) {
        if (MissionUtility::GetDistance(mHandles[109], mHandles[101]) < 250.0f) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::AttackTarget(mHandles[101], mHandles[15], true, true, false, false);
        }
        if (!MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::AttackTarget(mHandles[101], mHandles[14], true, true, false, false);
        }
        mFlags[72] = true;
        }
        }
        }
        if (!mFlags[37]) {
        if (MissionUtility::GetDistance(mHandles[109], "trigger_behind_attack3") < 150.0f) {
        mHandles[31] = MissionUtility::CreateObject("cis_tank_fighter", "behind_attack2_spawn1", "behind_attacker5", 3, -1, -1);
        mHandles[32] = MissionUtility::CreateObject("cis_tank_fighter", "behind_attack2_spawn2", "behind_attacker6", 3, -1, -1);
        mHandles[102] = MissionUtility::CreateFlock(mHandles[31], (Formation)0);
        MissionUtility::AddFlockMember(mHandles[102], mHandles[32]);
        MissionUtility::Patrol(mHandles[102], "behind_attack2_path", 500.0f, true);
        MissionUtility::SetAttackRange(mHandles[31], 250);
        MissionUtility::SetAttackRange(mHandles[32], 250);
        mFlags[37] = true;
        }
        }
        if (MissionUtility::IsFlockAlive(mHandles[102])) {
        if (!mFlags[73]) {
        if (MissionUtility::GetDistance(mHandles[109], mHandles[102]) < 250.0f) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::AttackTarget(mHandles[102], mHandles[15], true, true, false, false);
        }
        if (!MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::AttackTarget(mHandles[102], mHandles[14], true, true, false, false);
        }
        mFlags[73] = true;
        }
        }
        }
        if (!mFlags[151]) {
        if (MissionUtility::IsAlive(mHandles[190])) {
        if (MissionUtility::GetDistance(mHandles[190], mHandles[11]) < 200.0f) {
        MissionUtility::AttackTarget(mHandles[190], mHandles[11], true, true, false, false);
        mFlags[151] = true;
        }
        }
        }
        if (!mFlags[152]) {
        if (MissionUtility::IsAlive(mHandles[191])) {
        if (MissionUtility::GetDistance(mHandles[191], mHandles[11]) < 200.0f) {
        MissionUtility::AttackTarget(mHandles[191], mHandles[11], true, true, false, false);
        mFlags[152] = true;
        }
        }
        }
        if (!mFlags[153]) {
        if (MissionUtility::IsAlive(mHandles[192])) {
        if (MissionUtility::GetDistance(mHandles[192], mHandles[11]) < 200.0f) {
        MissionUtility::AttackTarget(mHandles[192], mHandles[11], true, true, false, false);
        mFlags[153] = true;
        }
        }
        }
        if (!mFlags[23]) {
        if (MissionUtility::IsAlive(mHandles[12])) {
        if (MissionUtility::GetDistance(mHandles[12], "arena") < 50.0f) {
        MissionUtility::ObjectiveComplete(mInts[2]);
        mFlags[23] = true;
        }
        }
        if (MissionUtility::IsAlive(mHandles[14])) {
        if (MissionUtility::GetDistance(mHandles[14], "arena") < 50.0f) {
        MissionUtility::ObjectiveComplete(mInts[2]);
        mFlags[23] = true;
        }
        }
        if (MissionUtility::IsAlive(mHandles[15])) {
        if (MissionUtility::GetDistance(mHandles[15], "arena") < 50.0f) {
        MissionUtility::ObjectiveComplete(mInts[2]);
        mFlags[23] = true;
        }
        }
        }
        if (!mFlags[96]) {
        if (mFlags[23]) {
        if (MissionUtility::IsAlive(mHandles[12])) {
        MissionUtility::Stop(mHandles[12]);
        MissionUtility::SetAttackRange(mHandles[12], 1);
        }
        if (MissionUtility::IsAlive(mHandles[14])) {
        MissionUtility::Stop(mHandles[14]);
        MissionUtility::SetAttackRange(mHandles[14], 1);
        }
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::Stop(mHandles[15]);
        MissionUtility::SetAttackRange(mHandles[15], 1);
        }
        mFlags[96] = true;
        }
        }
        if (!mFlags[38]) {
        if (!mFlags[23]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "end_base_trigger")) {
        MissionUtility::QueueSound("MWG02_09", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::PlayMusic("EP5_V1_T06_02", true);
        MissionUtility::RemoveObjectify(mHandles[14]);
        MissionUtility::RemoveObjectify(mHandles[12]);
        MissionUtility::RemoveObjectify(mHandles[15]);
        mInts[3] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0003");
        MissionUtility::DisplayText("missions.Geonosis1.text.str0006", 6.0f, -1.0f);
        if (!MissionUtility::IsAlive(mHandles[79])) {
        MissionUtility::RemoveObject(mHandles[79]);
        }
        if (!MissionUtility::IsAlive(mHandles[80])) {
        MissionUtility::RemoveObject(mHandles[80]);
        }
        if (MissionUtility::IsAlive(mHandles[27])) {
        MissionUtility::RemoveObject(mHandles[27]);
        }
        if (MissionUtility::IsAlive(mHandles[28])) {
        MissionUtility::RemoveObject(mHandles[28]);
        }
        if (MissionUtility::IsAlive(mHandles[29])) {
        MissionUtility::RemoveObject(mHandles[29]);
        }
        if (MissionUtility::IsAlive(mHandles[30])) {
        MissionUtility::RemoveObject(mHandles[30]);
        }
        if (MissionUtility::IsAlive(mHandles[31])) {
        MissionUtility::RemoveObject(mHandles[31]);
        }
        if (MissionUtility::IsAlive(mHandles[32])) {
        MissionUtility::RemoveObject(mHandles[32]);
        }
        if (MissionUtility::IsAlive(mHandles[19])) {
        MissionUtility::RemoveObject(mHandles[19]);
        }
        if (MissionUtility::IsAlive(mHandles[20])) {
        MissionUtility::RemoveObject(mHandles[20]);
        }
        if (MissionUtility::IsAlive(mHandles[23])) {
        MissionUtility::RemoveObject(mHandles[23]);
        }
        if (MissionUtility::IsAlive(mHandles[24])) {
        MissionUtility::RemoveObject(mHandles[24]);
        }
        if (MissionUtility::IsAlive(mHandles[25])) {
        MissionUtility::RemoveObject(mHandles[25]);
        }
        if (MissionUtility::IsAlive(mHandles[26])) {
        MissionUtility::RemoveObject(mHandles[26]);
        }
        if (MissionUtility::IsAlive(mHandles[111])) {
        MissionUtility::RemoveObject(mHandles[111]);
        }
        if (MissionUtility::IsAlive(mHandles[112])) {
        MissionUtility::RemoveObject(mHandles[112]);
        }
        if (MissionUtility::IsAlive(mHandles[113])) {
        MissionUtility::RemoveObject(mHandles[113]);
        }
        if (MissionUtility::IsAlive(mHandles[114])) {
        MissionUtility::RemoveObject(mHandles[114]);
        }
        if (MissionUtility::IsAlive(mHandles[115])) {
        MissionUtility::RemoveObject(mHandles[115]);
        }
        if (MissionUtility::IsAlive(mHandles[116])) {
        MissionUtility::RemoveObject(mHandles[116]);
        }
        if (MissionUtility::IsAlive(mHandles[117])) {
        MissionUtility::RemoveObject(mHandles[117]);
        }
        if (MissionUtility::IsAlive(mHandles[118])) {
        MissionUtility::RemoveObject(mHandles[118]);
        }
        mFlags[161] = true;
        mFlags[38] = true;
        }
        }
        }
        if (mFlags[23]) {
        if (!mFlags[38]) {
        if (!mFlags[82]) {
        if (!MissionUtility::IsAlive(mHandles[79])) {
        if (!MissionUtility::IsAlive(mHandles[80])) {
        if (!MissionUtility::IsAlive(mHandles[27])) {
        if (!MissionUtility::IsAlive(mHandles[28])) {
        if (!MissionUtility::IsAlive(mHandles[29])) {
        if (!MissionUtility::IsAlive(mHandles[30])) {
        if (!MissionUtility::IsAlive(mHandles[31])) {
        if (!MissionUtility::IsAlive(mHandles[32])) {
        if (!MissionUtility::IsAlive(mHandles[19])) {
        if (!MissionUtility::IsAlive(mHandles[20])) {
        if (!MissionUtility::IsAlive(mHandles[23])) {
        if (!MissionUtility::IsAlive(mHandles[24])) {
        if (!MissionUtility::IsAlive(mHandles[25])) {
        if (!MissionUtility::IsAlive(mHandles[26])) {
        if (!MissionUtility::IsAlive(mHandles[111])) {
        if (!MissionUtility::IsAlive(mHandles[112])) {
        if (!MissionUtility::IsAlive(mHandles[113])) {
        if (!MissionUtility::IsAlive(mHandles[114])) {
        mTimes[19] = 3.0f + MissionUtility::GetTime();
        mFlags[82] = true;
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
        if (!mFlags[82]) {
        if (!mFlags[97]) {
        if (MissionUtility::CountUnitsNearObject(mHandles[14], 700.0f, 3, 0) == 0) {
        if (!MissionUtility::IsAlive(mHandles[79])) {
        MissionUtility::RemoveObject(mHandles[79]);
        }
        if (!MissionUtility::IsAlive(mHandles[80])) {
        MissionUtility::RemoveObject(mHandles[80]);
        }
        if (MissionUtility::IsAlive(mHandles[27])) {
        MissionUtility::RemoveObject(mHandles[27]);
        }
        if (MissionUtility::IsAlive(mHandles[28])) {
        MissionUtility::RemoveObject(mHandles[28]);
        }
        if (MissionUtility::IsAlive(mHandles[29])) {
        MissionUtility::RemoveObject(mHandles[29]);
        }
        if (MissionUtility::IsAlive(mHandles[30])) {
        MissionUtility::RemoveObject(mHandles[30]);
        }
        if (MissionUtility::IsAlive(mHandles[31])) {
        MissionUtility::RemoveObject(mHandles[31]);
        }
        if (MissionUtility::IsAlive(mHandles[32])) {
        MissionUtility::RemoveObject(mHandles[32]);
        }
        if (MissionUtility::IsAlive(mHandles[19])) {
        MissionUtility::RemoveObject(mHandles[19]);
        }
        if (MissionUtility::IsAlive(mHandles[20])) {
        MissionUtility::RemoveObject(mHandles[20]);
        }
        if (MissionUtility::IsAlive(mHandles[23])) {
        MissionUtility::RemoveObject(mHandles[23]);
        }
        if (MissionUtility::IsAlive(mHandles[24])) {
        MissionUtility::RemoveObject(mHandles[24]);
        }
        if (MissionUtility::IsAlive(mHandles[25])) {
        MissionUtility::RemoveObject(mHandles[25]);
        }
        if (MissionUtility::IsAlive(mHandles[26])) {
        MissionUtility::RemoveObject(mHandles[26]);
        }
        if (MissionUtility::IsAlive(mHandles[111])) {
        MissionUtility::RemoveObject(mHandles[111]);
        }
        if (MissionUtility::IsAlive(mHandles[112])) {
        MissionUtility::RemoveObject(mHandles[112]);
        }
        if (MissionUtility::IsAlive(mHandles[113])) {
        MissionUtility::RemoveObject(mHandles[113]);
        }
        if (MissionUtility::IsAlive(mHandles[114])) {
        MissionUtility::RemoveObject(mHandles[114]);
        }
        if (MissionUtility::IsAlive(mHandles[115])) {
        MissionUtility::RemoveObject(mHandles[115]);
        }
        if (MissionUtility::IsAlive(mHandles[116])) {
        MissionUtility::RemoveObject(mHandles[116]);
        }
        if (MissionUtility::IsAlive(mHandles[117])) {
        MissionUtility::RemoveObject(mHandles[117]);
        }
        if (MissionUtility::IsAlive(mHandles[118])) {
        MissionUtility::RemoveObject(mHandles[118]);
        }
        mTimes[19] = 3.0f + MissionUtility::GetTime();
        mFlags[82] = true;
        mFlags[97] = true;
        }
        }
        }
        }
        }
        if (!mFlags[58]) {
        if (mTimes[19] < MissionUtility::GetTime()) {
        if (!mFlags[59]) {
        mTimes[15] = 1.0f + MissionUtility::GetTime();
        mFlags[59] = true;
        }
        if (!mFlags[66]) {
        if (mTimes[15] < MissionUtility::GetTime()) {
        mFlags[66] = true;
        }
        }
        if (mFlags[59]) {
        MissionUtility::RemoveObjectify(mHandles[14]);
        MissionUtility::RemoveObjectify(mHandles[12]);
        MissionUtility::RemoveObjectify(mHandles[15]);
        mInts[3] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0003");
        MissionUtility::DisplayText("missions.Geonosis1.text.str0006", 6.0f, -1.0f);
        MissionUtility::PlayMusic("EP5_V1_T06_02", true);
        MissionUtility::QueueSound("MWG02_09", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[161] = true;
        mFlags[58] = true;
        }
        }
        }
        if (mFlags[161]) {
        if (!mFlags[172]) {
        MissionUtility::SetAttackRange(mHandles[77], 4000);
        MissionUtility::AttackTarget(mHandles[77], mHandles[40], true, true, false, false);
        mFlags[172] = true;
        }
        if (!mFlags[173]) {
        if (!MissionUtility::IsAlive(mHandles[40])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[38], true, true, false, false);
        mFlags[173] = true;
        }
        }
        if (!mFlags[174]) {
        if (!MissionUtility::IsAlive(mHandles[38])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[42], true, true, false, false);
        mFlags[174] = true;
        }
        }
        if (!mFlags[175]) {
        if (!MissionUtility::IsAlive(mHandles[42])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[39], true, true, false, false);
        mFlags[175] = true;
        }
        }
        if (!mFlags[176]) {
        if (!MissionUtility::IsAlive(mHandles[39])) {
        MissionUtility::AttackTarget(mHandles[77], mHandles[41], true, true, false, false);
        mFlags[176] = true;
        }
        }
        if (!mFlags[24]) {
        if (!MissionUtility::IsAlive(mHandles[38])) {
        if (!MissionUtility::IsAlive(mHandles[39])) {
        if (!MissionUtility::IsAlive(mHandles[40])) {
        if (!MissionUtility::IsAlive(mHandles[41])) {
        if (!MissionUtility::IsAlive(mHandles[42])) {
        MissionUtility::SetWeaponOrd(mHandles[77], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        MissionUtility::SetAttackRange(mHandles[77], 300);
        MissionUtility::Wait(mHandles[77]);
        mFlags[24] = true;
        }
        }
        }
        }
        }
        }
        }
        if (!mFlags[122]) {
        if (mFlags[58]
            || mFlags[38]) {
        if (!mFlags[61]) {
        if (!MissionUtility::IsAlive(mHandles[11])) {
        mTimes[2] = 3.0f + MissionUtility::GetTime();
        MissionUtility::BonusObjectiveFailed(mInts[3]);
        mFlags[122] = true;
        }
        }
        }
        }
        if (mTimes[14] < MissionUtility::GetTime()) {
        int shooter;
        if (MissionUtility::IsAlive(mHandles[12])) {
        shooter = MissionUtility::GetWhoShotMe(mHandles[12]);
        if (shooter > 0) {
        if (MissionUtility::GetTeamNum(shooter) == 3) {
        MissionUtility::QueueSound("LMG02_06E", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[14] = 30.0f + MissionUtility::GetTime();
        }
        }
        }
        if (MissionUtility::IsAlive(mHandles[14])) {
        shooter = MissionUtility::GetWhoShotMe(mHandles[14]);
        if (shooter > 0) {
        if (MissionUtility::GetTeamNum(shooter) == 3) {
        MissionUtility::QueueSound("LMG02_06E", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[14] = 30.0f + MissionUtility::GetTime();
        }
        }
        }
        if (MissionUtility::IsAlive(mHandles[15])) {
        shooter = MissionUtility::GetWhoShotMe(mHandles[15]);
        if (shooter > 0) {
        if (MissionUtility::GetTeamNum(shooter) == 3) {
        MissionUtility::QueueSound("LMG02_06E", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[14] = 30.0f + MissionUtility::GetTime();
        }
        }
        }
        }
        if (mFlags[58]
            || mFlags[38]) {
        if (!mFlags[44]) {
        if (!MissionUtility::IsAlive(mHandles[38])
            || !MissionUtility::IsAlive(mHandles[39])
            || !MissionUtility::IsAlive(mHandles[40])
            || !MissionUtility::IsAlive(mHandles[41])
            || !MissionUtility::IsAlive(mHandles[42])) {
        mHandles[51] = MissionUtility::StartSound("klaxon1", true, 0.88f, 0.0f, 0.0f, "", 0, "");
        mTimes[18] = 10.0f + MissionUtility::GetTime();
        mTimes[12] = 1.0f + MissionUtility::GetTime();
        mFlags[44] = true;
        }
        }
        if (mTimes[18] < MissionUtility::GetTime()) {
        MissionUtility::StopSound(mHandles[51]);
        mTimes[18] = 999999.9f + MissionUtility::GetTime();
        }
        if (mFlags[44]) {
        if (!mFlags[61]) {
        if (mTimes[12] < MissionUtility::GetTime()) {
        if (MissionUtility::CountUnitsNearObject(mHandles[37], 500.0f, 2, "cis_tank_fighter") < 2) {
        if (!mFlags[150]) {
        mTimes[27] = 20.0f + MissionUtility::GetTime();
        mFlags[150] = true;
        }
        MissionUtility::SetAnimation(mHandles[75], "fullanimation", 1.0f, 1);
        MissionUtility::SetAnimation(mHandles[76], "fullanimation", 1.0f, 1);
        mTimes[48] = 1.0f + MissionUtility::GetTime();
        mTimes[12] = 18.0f + MissionUtility::GetTime();
        }
        }
        if (mTimes[48] < MissionUtility::GetTime()) {
        if (MissionUtility::IsAlive(mHandles[75])) {
        mHandles[52] = MissionUtility::CreateObject("cis_tank_fighter", mHandles[75], "hp_spawnpoint", "base_attacker1", 2, -1, 0);
        MissionUtility::SetAttackRange(mHandles[52], 1000);
        MissionUtility::Goto(mHandles[52], "spawner1_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Wait(mHandles[52]);
        MissionUtility::SetQueueFlag(false);
        }
        if (MissionUtility::IsAlive(mHandles[76])) {
        mHandles[53] = MissionUtility::CreateObject("cis_tank_fighter", mHandles[76], "hp_spawnpoint", "base_attacker2", 2, -1, 0);
        MissionUtility::SetAttackRange(mHandles[53], 1000);
        MissionUtility::Goto(mHandles[53], "spawner2_path", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Wait(mHandles[53]);
        MissionUtility::SetQueueFlag(false);
        }
        mTimes[48] = 999999.9f + MissionUtility::GetTime();
        }
        if (MissionUtility::IsAlive(mHandles[75])
            || MissionUtility::IsAlive(mHandles[76])) {
        if (!mFlags[149]) {
        if (!mFlags[61]) {
        if (mTimes[27] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("LMG02_36", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[149] = true;
        }
        }
        }
        }
        if (!MissionUtility::IsAlive(mHandles[52])) {
        if (!MissionUtility::IsAlive(mHandles[75])) {
        if (!MissionUtility::IsAlive(mHandles[76])) {
        if (!MissionUtility::IsAlive(mHandles[53])) {
        if (!MissionUtility::IsAlive(mHandles[38])) {
        if (!MissionUtility::IsAlive(mHandles[39])) {
        if (!MissionUtility::IsAlive(mHandles[40])) {
        if (!MissionUtility::IsAlive(mHandles[41])) {
        if (!MissionUtility::IsAlive(mHandles[42])) {
        if (MissionUtility::CountUnitsNearObject(mHandles[37], 700.0f, 2, 0) == 0) {
        if (MissionUtility::CountUnitsNearObject(mHandles[37], 700.0f, 3, 0) == 0) {
        MissionUtility::ObjectiveComplete(mInts[3]);
        mTimes[13] = 4.0f + MissionUtility::GetTime();
        mFlags[61] = true;
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
        if (!mFlags[127]) {
        if (mFlags[61]) {
        if (!mFlags[23]) {
        MissionUtility::ObjectiveComplete(mInts[2]);
        mFlags[127] = true;
        }
        }
        }
        if (!mFlags[119]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "end_base_trigger")) {
        MissionUtility::SetTeamNum(mHandles[38], 2);
        MissionUtility::SetTeamNum(mHandles[39], 2);
        MissionUtility::SetTeamNum(mHandles[40], 2);
        MissionUtility::SetTeamNum(mHandles[41], 2);
        MissionUtility::SetTeamNum(mHandles[42], 2);
        mFlags[119] = true;
        }
        }
        if (mFlags[61]) {
        if (!mFlags[60]) {
        if (mTimes[13] < MissionUtility::GetTime()) {
        if (!mFlags[65]) {
        if (MissionUtility::IsAlive(mHandles[12])) {
        MissionUtility::RemoveObject(mHandles[12]);
        mHandles[12] = MissionUtility::CreateObjectWithRotation("rep_tank_gtrans", "convoy1_spawn_end", 0, "gtrans1", 0, -1, -1);
        MissionUtility::Goto(mHandles[12], "convoy1_go", true);
        }
        if (MissionUtility::IsAlive(mHandles[14])) {
        MissionUtility::RemoveObject(mHandles[14]);
        mHandles[14] = MissionUtility::CreateObjectWithRotation("rep_tank_gtrans", "convoy2_spawn_end", 0, "gtrans2", 0, -1, -1);
        MissionUtility::Goto(mHandles[14], "convoy2_go", true);
        }
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::RemoveObject(mHandles[15]);
        mHandles[15] = MissionUtility::CreateObjectWithRotation("rep_tank_gtrans", "convoy3_spawn_end", 0, "gtrans3", 0, -1, -1);
        MissionUtility::Goto(mHandles[15], "convoy3_go", true);
        }
        mHandles[148] = MissionUtility::CreateObjectWithRotation("rep_inf_jedi", "end_cin_jedis_spawntemp", 0, "end_cin_jedi1", 0, -1, -1);
        mHandles[149] = MissionUtility::CreateObjectWithRotation("rep_inf_jedi", "end_cin_jedis_spawntemp", 1, "end_cin_jedi2", 0, -1, -1);
        mHandles[150] = MissionUtility::CreateObjectWithRotation("rep_inf_jedi", "end_cin_jedis_spawntemp", 2, "end_cin_jedi3", 0, -1, -1);
        mHandles[151] = MissionUtility::CreateObjectWithRotation("rep_inf_jedi", "end_cin_jedis_spawntemp", 3, "end_cin_jedi4", 0, -1, -1);
        mHandles[227] = MissionUtility::CreateObjectWithRotation("rep_inf_jedi", "end_cin_jedis_spawntemp", 0, "end_cin_jedi1", 0, -1, -1);
        mHandles[228] = MissionUtility::CreateObjectWithRotation("rep_inf_jedi", "end_cin_jedis_spawntemp", 1, "end_cin_jedi2", 0, -1, -1);
        mHandles[229] = MissionUtility::CreateObjectWithRotation("rep_inf_jedi", "end_cin_jedis_spawntemp", 2, "end_cin_jedi3", 0, -1, -1);
        mHandles[230] = MissionUtility::CreateObjectWithRotation("rep_inf_jedi", "end_cin_jedis_spawntemp", 3, "end_cin_jedi4", 0, -1, -1);
        MissionUtility::MoveObject(mHandles[11], "end_cin_player_move", 0, true);
        MissionUtility::MoveObject(mHandles[77], "end_cin_lum_move", 0, true);
        mHandles[144] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_player", "end_cin_mace_spawn", 0, "end_cin_mace", 0, -1, -1);
        mHandles[145] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1_player", "end_cin_lum_spawn", 0, "end_cin_lum", 0, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[144], true);
        MissionUtility::OverrideSoundRange(mHandles[145], true);
        MissionUtility::OverrideSoundRange(mHandles[148], true);
        MissionUtility::OverrideSoundRange(mHandles[149], true);
        MissionUtility::OverrideSoundRange(mHandles[150], true);
        MissionUtility::OverrideSoundRange(mHandles[151], true);
        MissionUtility::OverrideSoundRange(mHandles[227], true);
        MissionUtility::OverrideSoundRange(mHandles[228], true);
        MissionUtility::OverrideSoundRange(mHandles[229], true);
        MissionUtility::OverrideSoundRange(mHandles[230], true);
        MissionUtility::OverrideSoundRange(mHandles[12], true);
        MissionUtility::OverrideSoundRange(mHandles[14], true);
        MissionUtility::OverrideSoundRange(mHandles[15], true);
        MissionUtility::SetVelocForward(mHandles[144], 70.0f);
        MissionUtility::Goto(mHandles[144], "end_cin_mace_go", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[144]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetVelocForward(mHandles[145], 70.0f);
        MissionUtility::Goto(mHandles[145], "end_cin_lum_go", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[145]);
        MissionUtility::SetQueueFlag(false);
        mHandles[147] = MissionUtility::CreateObjectWithRotation("rep_inf_mace_cin", "end_cin_lum2_spawn", 0, "end_cin_lum2", 0, -1, -1);
        mHandles[146] = MissionUtility::CreateObjectWithRotation("rep_inf_luminara_cin", "end_cin_mace2_spawn", 0, "end_cin_mace2", 0, -1, -1);
        MissionUtility::SetVisible(mHandles[147], false);
        MissionUtility::SetVisible(mHandles[146], false);
        MissionUtility::SetCollidable(mHandles[147], false);
        MissionUtility::SetCollidable(mHandles[146], false);
        mHandles[231] = MissionUtility::CreateObjectWithRotation("geo_inf_geonosian", "EndCinGeonosianPath", 0, "EndCinGeonosian", 0, -1, -1);
        mHandles[232] = MissionUtility::CreateObjectWithRotation("geo_inf_geonosian", "EndCinGeonosianPath1", 0, "EndCinGeonosian1", 0, -1, -1);
        mHandles[233] = MissionUtility::CreateObjectWithRotation("geo_inf_geonosian", "EndCinGeonosianPath2", 0, "EndCinGeonosian2", 0, -1, -1);
        mHandles[234] = MissionUtility::CreateObjectWithRotation("geo_inf_geonosian", "EndCinGeonosianPath3", 0, "EndCinGeonosian3", 0, -1, -1);
        mHandles[235] = MissionUtility::CreateObjectWithRotation("geo_inf_geonosian", "EndCinGeonosianPath4", 0, "EndCinGeonosian4", 0, -1, -1);
        mHandles[18] = MissionUtility::RunCin("end_cin", true, true);
        MissionUtility::PlayMusic("EP2_V1_T13_01", true);
        mFlags[65] = true;
        }
        if (!mFlags[11]) {
        if (mFlags[65]) {
        if (MissionUtility::GetCinId(mHandles[18]) == 2) {
        mFlags[11] = true;
        }
        }
        }
        if (!mFlags[118]) {
        if (mFlags[65]) {
        if (MissionUtility::GetCinId(mHandles[18]) == 4) {
        mFlags[118] = true;
        MissionUtility::Goto(mHandles[231], "EndCinGeonosianPath", false);
        MissionUtility::Goto(mHandles[232], "EndCinGeonosianPath1", false);
        MissionUtility::Goto(mHandles[233], "EndCinGeonosianPath2", false);
        MissionUtility::Goto(mHandles[234], "EndCinGeonosianPath3", false);
        MissionUtility::Goto(mHandles[235], "EndCinGeonosianPath4", false);
        MissionUtility::OverrideSoundRange(mHandles[231], true);
        MissionUtility::OverrideSoundRange(mHandles[232], true);
        MissionUtility::OverrideSoundRange(mHandles[233], true);
        MissionUtility::OverrideSoundRange(mHandles[234], true);
        MissionUtility::OverrideSoundRange(mHandles[235], true);
        }
        }
        }
        if (!mFlags[12]) {
        if (mFlags[65]) {
        if (MissionUtility::GetCinId(mHandles[18]) == 2) {
        MissionUtility::SetVisible(mHandles[147], true);
        MissionUtility::SetVisible(mHandles[146], true);
        MissionUtility::OverrideSoundRange(mHandles[147], true);
        MissionUtility::OverrideSoundRange(mHandles[146], true);
        MissionUtility::QueueSound("LMG02_12", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("MWG02_19", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Goto(mHandles[147], "end_cin_focus4", true);
        MissionUtility::Goto(mHandles[146], "end_cin_focus4", true);
        mTimes[25] = 2.0f + MissionUtility::GetTime();
        mFlags[12] = true;
        }
        }
        }
        if (!mFlags[229]) {
        if (mTimes[25] < MissionUtility::GetTime()) {
        if (!mFlags[228]) {
        MissionUtility::MoveObjectWithRotation(mHandles[148], "end_cin_jedis_spawn", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[149], "end_cin_jedis_spawn", 1, true);
        MissionUtility::MoveObjectWithRotation(mHandles[150], "end_cin_jedis_spawn", 2, true);
        MissionUtility::MoveObjectWithRotation(mHandles[151], "end_cin_jedis_spawn", 3, true);
        MissionUtility::Goto(mHandles[148], "end_cin_jedis_go", 0);
        MissionUtility::Goto(mHandles[149], "end_cin_jedis_go", 1);
        MissionUtility::Goto(mHandles[150], "end_cin_jedis_go", 2);
        MissionUtility::Goto(mHandles[151], "end_cin_jedis_go", 3);
        mTimes[25] = 2.0f + MissionUtility::GetTime();
        mFlags[228] = true;
        } else {
        MissionUtility::MoveObjectWithRotation(mHandles[227], "end_cin_jedis_spawn", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[228], "end_cin_jedis_spawn", 1, true);
        MissionUtility::MoveObjectWithRotation(mHandles[229], "end_cin_jedis_spawn", 2, true);
        MissionUtility::MoveObjectWithRotation(mHandles[230], "end_cin_jedis_spawn", 3, true);
        MissionUtility::Goto(mHandles[227], "end_cin_jedis_go", 0);
        MissionUtility::Goto(mHandles[228], "end_cin_jedis_go", 1);
        MissionUtility::Goto(mHandles[229], "end_cin_jedis_go", 2);
        MissionUtility::Goto(mHandles[230], "end_cin_jedis_go", 3);
        mFlags[229] = true;
        }
        }
        }
        if (mFlags[65]) {
        if (!MissionUtility::IsCinRunning(mHandles[18])) {
        MissionUtility::MissionSuccess();
        mFlags[60] = true;
        }
        }
        }
        }
        }
        }
    }

    // ---- +0x6d98  636 bytes ----
    if (!mFlags[61] && mFlags[7]) {
        if (!mFlags[134]) {
        if (!MissionUtility::IsAlive(mHandles[12])) {
        MissionUtility::QueueSound("CTINT_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        mInts[16] = mInts[16] + 1;
        mFlags[134] = true;
        }
        }
        if (!mFlags[135]) {
        if (!MissionUtility::IsAlive(mHandles[14])) {
        MissionUtility::QueueSound("CTINT_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        mInts[16] = mInts[16] + 1;
        mFlags[135] = true;
        }
        }
        if (!mFlags[136]) {
        if (!MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::QueueSound("CTINT_21", 1.0f, 0.0f, 0.0f, "", 0, "");
        mInts[16] = mInts[16] + 1;
        mFlags[136] = true;
        }
        }
        if (!mFlags[48]) {
        if (mInts[16] == 1) {
        mTimes[9] = 2.0f + MissionUtility::GetTime();
        mFlags[48] = true;
        }
        }
        if (!mFlags[53]) {
        if (mTimes[9] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("LMG02_06B", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[53] = true;
        }
        }
        if (!mFlags[51]) {
        if (mInts[16] == 2) {
        mTimes[10] = 2.0f + MissionUtility::GetTime();
        mFlags[51] = true;
        }
        }
        if (!mFlags[55]) {
        if (mTimes[10] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("LMG02_06B", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[55] = true;
        }
        }
        if (!mFlags[52]) {
        if (mInts[16] == 3) {
        mTimes[11] = 2.0f + MissionUtility::GetTime();
        mFlags[52] = true;
        }
        }
        if (!mFlags[56]) {
        if (mTimes[11] < MissionUtility::GetTime()) {
        MissionUtility::QueueSound("LMG02_29", 1.0f, 0.0f, 0.0f, "", 0, "");
        mTimes[2] = 5.0f + MissionUtility::GetTime();
        MissionUtility::BonusObjectiveFailed(mInts[2]);
        mFlags[56] = true;
        }
        }
    }

    // ---- +0x7014  140 bytes ----
    if (!mFlags[93] && mFlags[65]) {
        if (MissionUtility::IsAlive(mHandles[12])) {
        if (MissionUtility::IsAlive(mHandles[14])) {
        if (MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::QueueSound("tone", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::DisplayText("missions.Geonosis1.text.str0003", 6.0f, -1.0f);
        MissionUtility::ObjectiveComplete(mHandles[129]);
        mFlags[123] = true;
        mFlags[93] = true;
        }
        }
        }
    }

    // ---- +0x70a0  92 bytes ----
    if (!mFlags[123] && mFlags[7]) {
        if (!MissionUtility::IsAlive(mHandles[12])
            || !MissionUtility::IsAlive(mHandles[14])
            || !MissionUtility::IsAlive(mHandles[15])) {
        MissionUtility::BonusObjectiveFailed(mHandles[129]);
        mFlags[93] = true;
        mFlags[123] = true;
        }
    }

    // ---- +0x70fc  576 bytes ----
    if (!mFlags[94]) {
        if (!MissionUtility::IsAlive(mHandles[81])) {
        if (!MissionUtility::IsAlive(mHandles[82])) {
        if (!MissionUtility::IsAlive(mHandles[83])) {
        if (!MissionUtility::IsAlive(mHandles[84])) {
        if (!MissionUtility::IsAlive(mHandles[85])) {
        if (!MissionUtility::IsAlive(mHandles[86])) {
        if (!MissionUtility::IsAlive(mHandles[87])) {
        if (!MissionUtility::IsAlive(mHandles[88])) {
        if (!MissionUtility::IsAlive(mHandles[89])) {
        if (!MissionUtility::IsAlive(mHandles[90])) {
        if (!MissionUtility::IsAlive(mHandles[91])) {
        if (!MissionUtility::IsAlive(mHandles[92])) {
        if (!MissionUtility::IsAlive(mHandles[93])) {
        if (!MissionUtility::IsAlive(mHandles[94])) {
        if (!MissionUtility::IsAlive(mHandles[95])) {
        if (!MissionUtility::IsAlive(mHandles[96])) {
        if (!MissionUtility::IsAlive(mHandles[97])) {
        if (!MissionUtility::IsAlive(mHandles[43])) {
        if (!MissionUtility::IsAlive(mHandles[44])) {
        if (!MissionUtility::IsAlive(mHandles[45])) {
        if (!MissionUtility::IsAlive(mHandles[46])) {
        if (!MissionUtility::IsAlive(mHandles[47])) {
        if (!MissionUtility::IsAlive(mHandles[48])) {
        if (!MissionUtility::IsAlive(mHandles[49])) {
        if (!MissionUtility::IsAlive(mHandles[50])) {
        if (!MissionUtility::IsAlive(mHandles[4])) {
        if (!MissionUtility::IsAlive(mHandles[38])) {
        if (!MissionUtility::IsAlive(mHandles[39])) {
        if (!MissionUtility::IsAlive(mHandles[40])) {
        if (!MissionUtility::IsAlive(mHandles[41])) {
        if (!MissionUtility::IsAlive(mHandles[42])) {
        MissionUtility::QueueSound("tone", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::DisplayText("missions.Geonosis1.text.str0004", 6.0f, -1.0f);
        MissionUtility::ObjectiveComplete(mHandles[130]);
        mFlags[124] = true;
        mFlags[94] = true;
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

    // ---- +0x733c  56 bytes ----
    if (!mFlags[124] && mFlags[65] && !mFlags[94]) {
        MissionUtility::BonusObjectiveFailed(mHandles[130]);
        mFlags[94] = true;
        mFlags[124] = true;
    }

    // ---- +0x7374  104 bytes ----
    if (!mFlags[95]) {
        if (MissionUtility::GetDistance(mHandles[11], mHandles[98]) < 100.0f) {
        MissionUtility::QueueSound("tone", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::DisplayText("missions.Geonosis1.text.str0005", 6.0f, -1.0f);
        MissionUtility::ObjectiveComplete(mHandles[131]);
        mFlags[125] = true;
        mFlags[95] = true;
        }
    }

    // ---- +0x73dc  56 bytes ----
    if (!mFlags[125] && mFlags[60] && !mFlags[95]) {
        MissionUtility::BonusObjectiveFailed(mHandles[131]);
        mFlags[95] = true;
        mFlags[125] = true;
    }

    // ---- +0x7414  72 bytes ----
    if (!mFlags[22]) {
        if (!MissionUtility::IsAlive(mHandles[77])) {
        mHandles[17] = MissionUtility::AddObjective("missions.Geonosis1.objective.str0005");
        MissionUtility::BonusObjectiveFailed(mHandles[17]);
        mTimes[2] = 2.0f + MissionUtility::GetTime();
        mFlags[22] = true;
        }
    }

    // ---- +0x745c  40 bytes ----
    if (!mFlags[40]) {
        if (mTimes[2] < MissionUtility::GetTime()) {
        MissionUtility::MissionFailure();
        mFlags[40] = true;
        }
    }

    // ---- +0x7484  52 bytes ----
    if (!mFlags[126]) {
        if (!MissionUtility::IsAlive(mHandles[11])) {
        mTimes[2] = 3.0f + MissionUtility::GetTime();
        mFlags[126] = true;
        }
    }

    // ---- +0x74b8  84 bytes ----
    if (mTimes[45] < MissionUtility::GetTime()) {
    if (MissionUtility::IsInsideRegion(mHandles[11], "turnaround3")) {
    MissionUtility::QueueSound("LMG02_30", 1.0f, 0.0f, 0.0f, "", 0, "");
    mTimes[45] = 8.0f + MissionUtility::GetTime();
    }
    }

    // ---- +0x750c  80 bytes ----
    if (mTimes[42] < MissionUtility::GetTime()) {
    MissionUtility::StartSound("AmbGeon_plains_stinger01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mTimes[42] = MissionUtility::GetTime() + MissionUtility::GetRandomFloat(10.0f, 20.0f);
    }

    // ---- +0x755c  100 bytes ----
    if (!mFlags[119]) {
        if (MissionUtility::IsInsideRegion(mHandles[11], "end_base_trigger")) {
        MissionUtility::SetTeamNum(mHandles[38], 2);
        MissionUtility::SetTeamNum(mHandles[39], 2);
        MissionUtility::SetTeamNum(mHandles[40], 2);
        MissionUtility::SetTeamNum(mHandles[41], 2);
        MissionUtility::SetTeamNum(mHandles[42], 2);
        mFlags[119] = true;
        }
    }

    // ---- +0x75c0  72 bytes ----
    if (!mFlags[0]) {
        if (MissionUtility::GetCurHealth(mHandles[77]) < 1000.0f) {
        MissionUtility::QueueSound("LMG02_06A", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[0] = true;
        }
    }

    // ---- +0x7608  72 bytes ----
    if (!mFlags[1]) {
        if (MissionUtility::GetCurHealth(mHandles[77]) < 300.0f) {
        MissionUtility::QueueSound("LMG02_31", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[1] = true;
        }
    }

    // ---- +0x7650  68 bytes ----
    if (!mFlags[2]) {
        if (!MissionUtility::IsAlive(mHandles[77])) {
        MissionUtility::QueueSound("LMDS004R", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[2] = true;
        }
    }

}

void Geonosis1Script::Setup()
{
        mFlags[10] = false;
        mFlags[14] = false;
        mFlags[16] = false;
        mFlags[18] = false;
        mFlags[20] = false;
        mFlags[23] = false;
        mFlags[26] = false;
        mFlags[65] = false;
        mFlags[40] = false;
        mFlags[42] = false;
        mFlags[43] = false;
        mFlags[48] = false;
        mFlags[49] = false;
        mFlags[50] = false;
        mFlags[51] = false;
        mFlags[53] = false;
        mFlags[55] = false;
        mFlags[56] = false;
        mFlags[57] = false;
        mFlags[44] = false;
        mFlags[60] = false;
        mFlags[62] = false;
        mFlags[63] = false;
        mFlags[64] = false;
        mTimes[49] = 0.0f;
        mTimes[0] = 999999.9f;
        mTimes[1] = 999999.9f;
        mTimes[2] = 999999.9f;
        mTimes[9] = 999999.9f;
        mTimes[10] = 999999.9f;
        mTimes[11] = 999999.9f;
        mTimes[12] = 999999.9f;
        mTimes[13] = 999999.9f;
        mTimes[15] = 999999.9f;
        mTimes[17] = 999999.9f;
        mTimes[18] = 999999.9f;
        mTimes[16] = 30.9f;
        mTimes[14] = 1.0f;
        mTimes[19] = 999999.9f;
        mTimes[20] = 999999.9f;
        mTimes[21] = 999999.9f;
        mTimes[23] = 999999.9f;
        mTimes[24] = 999999.9f;
        mTimes[29] = 999999.9f;
        mTimes[22] = 999999.9f;
        mTimes[27] = 999999.9f;
        mTimes[25] = 999999.9f;
        mTimes[26] = 999999.9f;
        mTimes[28] = 999999.9f;
        mTimes[31] = 999999.9f;
        mTimes[42] = 999999.9f;
        mTimes[32] = 999999.9f;
        mTimes[41] = 999999.9f;
        mTimes[30] = 999999.9f;
        mTimes[33] = 999999.9f;
        mTimes[34] = 999999.9f;
        mTimes[35] = 999999.9f;
        mTimes[36] = 999999.9f;
        mTimes[37] = 999999.9f;
        mTimes[38] = 999999.9f;
        mTimes[39] = 999999.9f;
        mTimes[40] = 999999.9f;
        mTimes[47] = 999999.9f;
        mTimes[48] = 999999.9f;
        mTimes[7] = 999999.9f;
        mTimes[8] = 999999.9f;
        mTimes[44] = 3.9f;
        mTimes[46] = 3.9f;
        mTimes[43] = 3.9f;
        mTimes[45] = 3.9f;
        mHandles[0] = MissionUtility::GetHandle("nav1_tank1");
        mHandles[1] = MissionUtility::GetHandle("nav1_tank2");
        mHandles[2] = MissionUtility::GetHandle("nav1_tank3");
        mHandles[3] = MissionUtility::GetHandle("nav1_tank4");
        mHandles[34] = MissionUtility::GetHandle("pad_turret1");
        mHandles[35] = MissionUtility::GetHandle("pad_turret2");
        mHandles[37] = MissionUtility::GetHandle("enemy_base1");
        mHandles[38] = MissionUtility::GetHandle("base_turret1");
        mHandles[39] = MissionUtility::GetHandle("base_turret2");
        mHandles[40] = MissionUtility::GetHandle("base_turret3");
        mHandles[41] = MissionUtility::GetHandle("base_turret4");
        mHandles[42] = MissionUtility::GetHandle("base_turret5");
        mHandles[43] = MissionUtility::GetHandle("turret1");
        mHandles[44] = MissionUtility::GetHandle("turret2");
        mHandles[45] = MissionUtility::GetHandle("turret3");
        mHandles[46] = MissionUtility::GetHandle("turret4");
        mHandles[47] = MissionUtility::GetHandle("turret5");
        mHandles[48] = MissionUtility::GetHandle("turret6");
        mHandles[49] = MissionUtility::GetHandle("turret7");
        mHandles[50] = MissionUtility::GetHandle("turret8");
        mHandles[128] = MissionUtility::GetHandle("landingpad");
        mHandles[54] = MissionUtility::GetHandle("tester1");
        mHandles[9] = MissionUtility::GetHandle("nav1");
        mHandles[75] = MissionUtility::GetHandle("spawner1");
        mHandles[76] = MissionUtility::GetHandle("spawner2");
        mHandles[77] = MissionUtility::GetHandle("lum");
        mHandles[11] = MissionUtility::GetPlayerHandle(0);
        mHandles[98] = MissionUtility::GetHandle("secret_place");
        mHandles[81] = MissionUtility::GetHandle("wallturret1");
        mHandles[82] = MissionUtility::GetHandle("wallturret2");
        mHandles[83] = MissionUtility::GetHandle("wallturret3");
        mHandles[84] = MissionUtility::GetHandle("wallturret4");
        mHandles[85] = MissionUtility::GetHandle("wallturret5");
        mHandles[86] = MissionUtility::GetHandle("wallturret6");
        mHandles[87] = MissionUtility::GetHandle("wallturret7");
        mHandles[88] = MissionUtility::GetHandle("wallturret8");
        mHandles[89] = MissionUtility::GetHandle("wallturret9");
        mHandles[90] = MissionUtility::GetHandle("wallturret10");
        mHandles[91] = MissionUtility::GetHandle("wallturret11");
        mHandles[92] = MissionUtility::GetHandle("wallturret12");
        mHandles[93] = MissionUtility::GetHandle("wallturret13");
        mHandles[94] = MissionUtility::GetHandle("wallturret14");
        mHandles[95] = MissionUtility::GetHandle("wallturret15");
        mHandles[96] = MissionUtility::GetHandle("wallturret16");
        mHandles[97] = MissionUtility::GetHandle("wallturret17");
        mHandles[119] = MissionUtility::GetHandle("starfighter1");
        mHandles[120] = MissionUtility::GetHandle("starfighter2");
        mHandles[138] = MissionUtility::GetHandle("spire1");
        mHandles[139] = MissionUtility::GetHandle("spire2");
        mHandles[140] = MissionUtility::GetHandle("spire3");
        mHandles[4] = MissionUtility::GetHandle("turret9");
        mHandles[197] = MissionUtility::GetHandle("OpenCinTank");
        mHandles[196] = MissionUtility::GetHandle("OpenCinTank1");
        mHandles[201] = MissionUtility::GetHandle("OpenCinTurret");
        mHandles[202] = MissionUtility::GetHandle("OpenCinTurret1");
        mHandles[198] = MissionUtility::GetHandle("OpenCinSpeeder");
        mHandles[199] = MissionUtility::GetHandle("OpenCinSpeeder1");
        mHandles[200] = MissionUtility::GetHandle("OpenCinSpeeder2");
        MissionUtility::PreloadConfig("rep_inf_anakin_cin");
        MissionUtility::PreloadConfig("rep_tank_gtrans");
        MissionUtility::PreloadConfig("cis_walk_assault");
        MissionUtility::PreloadConfig("rep_fly_gunship");
        MissionUtility::PreloadConfig("rep_tank_fighter1_player");
        MissionUtility::PreloadConfig("cis_bike_speeder");
        MissionUtility::PreloadConfig("GEO_bldg_wallturret_dest");
        MissionUtility::PreloadConfig("rep_blaster_fighter1_weak_ord");
        MissionUtility::PreloadConfig("rep_inf_mace_cin");
        MissionUtility::PreloadConfig("rep_inf_luminara_cin");
        MissionUtility::PreloadConfig("rep_inf_obiwan_cin");
        MissionUtility::PreloadConfig("rep_fly_assault");
        MissionUtility::PreloadConfig("cis_tank_wheeled");
        MissionUtility::PreloadConfig("rep_fly_vcarrier");
        MissionUtility::PreloadConfig("rep_inf_jedi");
        MissionUtility::PreloadConfig("geo_inf_geonosian");
        mInts[16] = 0;
}

SPMission *Geonosis1BuildMission()
{
    return new Geonosis1Script();
}
