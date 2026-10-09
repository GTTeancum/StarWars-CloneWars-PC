// Thule1Script.cpp -- reconstruction of a shipped mission script. 49,576 bytes, byte-exact.
//
// Head and body generated (gen_mission_head.py + gen_block.py --all). Six hand corrections,
// written up in analysis/mission_batch_b.md:
//
//   * the two mid-mission restore blocks read eight object-alive flags and one timer value
//     into STACK LOCALS -- `MidMissionLoad(bool&)` writes through the reference and the
//     `if (!local)` tests that follow are those locals read back. Frame slots order them
//     within a size class: the floats at 0x1c/0x18, the sixteen bools at 0x17..0x08, so they
//     are declared floats-first at the top of Execute in textual block order;
//   * `StopTimer` returns void -- the shape is `StopTimer(t); t = <restored>; ResumeTimer(t);`;
//   * two guards are a non-short-circuit `&` (clrlwi/cntlzw/rlwinm twice, then `and.`), which
//     the generator drops silently: `if (!IsAlive(h) & !mFlags[n])`;
//   * the one loop in the function, which the generator reads as straight-line code.

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
    int    AddFlyerAmmoBox(const char*, int, float);
    int    AddFlyerArmy(char, const char*, int, int, float);
    int    AddFlyerHealthBox(const char*, int, float);
    int    AddHealthBar(int, const char*, float);
    int    AddObjective(const char*);
    int    AddPitchRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    int    AddPropArmy(char, const char*, int, int, float);
    void   AddTurnAroundRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    void   AttackTarget(int, int, bool, bool, bool, bool);
    void   BeginWave(const char*);
    void   BonusObjectiveComplete(int, bool);
    void   BonusObjectiveFailed(int);
    void   Cargo(int);
    int    CreateDropOffRegion(const char*);
    int    CreateFlock();
    int    CreateFlock(int, Formation);
    int    CreateObject(const char*, const Vector&, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObject(const char*, const char*, const char*, int, int, int);
    int    CreateObject(const char*, const char*, int, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    int    CreatePickUpRegion(const char*);
    int    CreateRegionList(const char*, bool, bool);
    void   DamageObject(int, float, float);
    void   Defend(int, const char*, int, float);
    void   DisbandFlock(int);
    void   DisplayText(const char*, float, float);
    void   EvictConfig(const char*);
    void   FlushSoundQueue();
    int    GetCinId(int);
    float  GetDistance(int, const char*, int);
    float  GetDistance(int, int);
    int    GetFlockCount(int);
    float  GetGameClock();
    int    GetHandle(const char*);
    int    GetPlayerHandle(int);
    int    GetRegionNewMember(int, int);
    int    GetRegionNewMemberCount(int);
    void   Goto(int, const char*, bool);
    void   Goto(int, const char*, int);
    void   GotoDirect(int, const char*, bool);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsFlockAlive(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsLoaded(int, int);
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
    void   MoveObject(int, const Vector&, Quat, bool);
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
    void   RemoveCargo(int);
    void   RemoveFlock(int, bool);
    void   RemoveObject(int);
    void   RemoveObjectify(const char*, int);
    void   RemoveObjectify(int);
    void   RemoveObjective(int);
    void   RemoveTurnAroundRegion(const char*);
    int    RunCin(const char*, bool, bool);
    void   SetAlliance(int, int);
    void   SetApplyDynamics(int, bool);
    void   SetCollidable(int, bool);
    void   SetCurHealth(int, float);
    void   SetCurShield(int, float);
    void   SetEnemies(int, int);
    void   SetEnemiesOneWay(int, int);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetPropArmyWayPoints(int, const char*, bool);
    void   SetQueueFlag(bool);
    void   SetTeamNum(int, int);
    void   SetVelocForward(int, float);
    void   SetVelocMaximumFly(int, float);
    void   SetVelocMinimumFly(int, float);
    void   SetVelocNeutralFly(int, float);
    void   SetWeaponOrd(int, const char*, const char*);
    void   StartAmbiences(const char*, const char*, float, float);
    int    StartSound(const char*, bool, float, float, float, const char*, int, const char*);
    void   StopCin(int);
    void   TakeOff(int);
}

static const char *const kClassName = "Thule1Script";
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

class Thule1Script : public SPMission
{
public:
    virtual ~Thule1Script();
    Thule1Script()
    {
        mBoolCount = 180;   mBools = mFlags;
        mCountB    = 0;     mIntsB = mIntsB_;
        mCountC    = 613;   mIntsC = mHandles;
        mCountD    = 18;    mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    char   mPad24[1];
    bool   mFlags[180];            // +0x025  the one-shot latches
    char   mPadD9[7];                  // +0x0d9
    int    mIntsB_[1];            // +0x0e0
    char   mPadE4[4];                  // +0x0e4
    int    mHandles[613];          // +0x0e8
    char   mPadA7C[8];                  // +0xa7c
    int    mInts[18];             // +0xa84
    char   mPadACC[4];                  // +0xacc
    Timer  mTimer0;                      // +0xad0
    Timer  mTimer1;                      // +0xadc
    Timer  mTimer2;                      // +0xae8
    Timer  mTimer3;                      // +0xaf4
    Timer  mTimer4;                      // +0xb00
    Timer  mTimer5;                      // +0xb0c
    Timer  mTimer6;                      // +0xb18
    Timer  mTimer7;                      // +0xb24
    Timer  mTimer8;                      // +0xb30
    Timer  mTimer9;                      // +0xb3c
    Timer  mTimer10;                      // +0xb48
    Timer  mTimer11;                      // +0xb54
    Timer  mTimer12;                      // +0xb60
    Timer  mTimer13;                      // +0xb6c
    Timer  mTimer14;                      // +0xb78
    Timer  mTimer15;                      // +0xb84
    Timer  mTimer16;                      // +0xb90
    Timer  mTimer17;                      // +0xb9c
    Timer  mTimer18;                      // +0xba8
    Timer  mTimer19;                      // +0xbb4
    Timer  mTimer20;                      // +0xbc0
    Timer  mTimer21;                      // +0xbcc
    Timer  mTimer22;                      // +0xbd8
    Timer  mTimer23;                      // +0xbe4
    Timer  mTimer24;                      // +0xbf0
    Timer  mTimer25;                      // +0xbfc
    int    mPhase;                  // +0xc08
};

Thule1Script::~Thule1Script()
{
}

void Thule1Script::Execute()
{
    // The mid-mission restore blocks read eight object-alive flags and one timer value into
    // stack locals. Frame slots follow declaration order within a size class (Rule 3 of
    // analysis/codegen_corpus.md): floats 0x1c/0x18, bools 0x17..0x10 then 0xf..0x8.
    int   i;
    float mLoadTA;
    bool  mLoadA0, mLoadA1, mLoadA2, mLoadA3, mLoadA4, mLoadA5, mLoadA6, mLoadA7;
    float mLoadTB;
    bool  mLoadB0, mLoadB1, mLoadB2, mLoadB3, mLoadB4, mLoadB5, mLoadB6, mLoadB7;

    // ---- +0x0014  2172 bytes ----
    if (mFlags[28]) {
    MissionUtility::SetEnemies(10, 20);
    MissionUtility::SetEnemies(1, 20);
    MissionUtility::SetMusicLooping(true);
    BeginTimer(mTimer22);
    MissionUtility::SetCurHealth(mHandles[341], 70.0f);
    MissionUtility::SetCurHealth(mHandles[342], 70.0f);
    MissionUtility::SetCurHealth(mHandles[344], 70.0f);
    MissionUtility::SetCurHealth(mHandles[345], 70.0f);
    MissionUtility::SetCurHealth(mHandles[346], 70.0f);
    MissionUtility::SetCurHealth(mHandles[347], 70.0f);
    MissionUtility::SetMaxHealth(mHandles[262], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[263], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[264], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[265], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[266], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[267], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[268], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[269], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[270], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[262], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[263], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[264], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[265], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[266], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[267], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[268], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[269], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[270], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[411], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[411], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[335], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[335], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[336], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[336], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[327], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[327], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[328], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[328], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[329], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[329], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[330], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[330], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[331], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[331], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[332], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[332], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[333], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[333], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[334], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[334], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[141], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[141], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[142], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[142], 999999.0f);
    MissionUtility::SetMaxHealth(mHandles[143], 999999.0f);
    MissionUtility::SetCurHealth(mHandles[143], 999999.0f);
    MissionUtility::SetEnemiesOneWay(1, 3);
    MissionUtility::SetAlliance(1, 6);
    MissionUtility::SetEnemies(6, 7);
    MissionUtility::CreatePickUpRegion("troop_pickup");
    mHandles[173] = MissionUtility::CreateFlock();
    mHandles[126] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[126], mHandles[133]);
    MissionUtility::AddFlockMember(mHandles[126], mHandles[134]);
    MissionUtility::AddFlockMember(mHandles[126], mHandles[135]);
    MissionUtility::AddFlockMember(mHandles[126], mHandles[136]);
    MissionUtility::AddFlockMember(mHandles[126], mHandles[137]);
    MissionUtility::AddFlockMember(mHandles[126], mHandles[138]);
    MissionUtility::AddFlockMember(mHandles[126], mHandles[139]);
    MissionUtility::AddFlockMember(mHandles[126], mHandles[140]);
    mInts[15] = MissionUtility::AddBonusObjective("missions.Thule1.bonus.str0000");
    mInts[16] = MissionUtility::AddBonusObjective("missions.Thule1.bonus.str0003");
    mInts[17] = MissionUtility::AddBonusObjective("missions.Thule1.bonus.str0004");
    MissionUtility::AddTurnAroundRegion("landing_turnaround", "landing_turnaround_focus", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("landing_turnaround1", "landing_turnaround_focus", 0, 0, 0, 0);
    BeginTimer(mTimer5);
    mHandles[172] = MissionUtility::CreateFlock();
    MissionUtility::StartAmbiences("AmbThule_Main01_pl2", "PropGen_thunderLt01", 10.0f, 30.0f);
    mHandles[385] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[385], mHandles[390]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[391]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[392]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[393]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[394]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[395]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[396]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[397]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[398]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[399]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[400]);
    MissionUtility::AddFlockMember(mHandles[385], mHandles[401]);
    mHandles[425] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[425], mHandles[426]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[427]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[428]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[431]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[432]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[433]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[434]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[435]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[436]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[437]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[438]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[439]);
    MissionUtility::AddFlockMember(mHandles[425], mHandles[440]);
    mHandles[322] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[322], mHandles[337]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[338]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[339]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[340]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[341]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[342]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[343]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[344]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[345]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[346]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[347]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[348]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[349]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[350]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[351]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[323]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[324]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[325]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[352]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[353]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[354]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[355]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[356]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[357]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[358]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[359]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[360]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[361]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[362]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[363]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[364]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[365]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[366]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[367]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[368]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[369]);
    MissionUtility::AddFlockMember(mHandles[322], mHandles[370]);
    MissionUtility::SetVelocForward(mHandles[337], 0.0f);
    MissionUtility::SetVelocForward(mHandles[338], 0.0f);
    MissionUtility::SetVelocForward(mHandles[339], 0.0f);
    MissionUtility::SetVelocForward(mHandles[340], 0.0f);
    MissionUtility::SetVelocForward(mHandles[343], 0.0f);
    MissionUtility::SetTeamNum(mHandles[271], 0);
    MissionUtility::SetTeamNum(mHandles[272], 0);
    MissionUtility::SetTeamNum(mHandles[273], 0);
    MissionUtility::SetTeamNum(mHandles[274], 0);
    MissionUtility::SetTeamNum(mHandles[275], 0);
    MissionUtility::SetTeamNum(mHandles[276], 0);
    MissionUtility::SetTeamNum(mHandles[277], 0);
    MissionUtility::SetTeamNum(mHandles[278], 0);
    MissionUtility::SetTeamNum(mHandles[279], 0);
    MissionUtility::SetTeamNum(mHandles[280], 0);
    MissionUtility::SetTeamNum(mHandles[270], 0);
    MissionUtility::AddTurnAroundRegion("turnaround", "turnaround_focus", 0, 0, 0, 0);
    MissionUtility::AddPitchRegion("ceiling", 0, 0, 0, 0, 0);
    mPhase = MissionUtility::MidMissionGetSavePoint();
    mHandles[321] = MissionUtility::CreateFlock(mHandles[318], (Formation)1);
    MissionUtility::Patrol(mHandles[321], "fighter_patrol1", 300.0f, true);
    mHandles[43] = MissionUtility::CreateRegionList("crater_kill", true, false);
    MissionUtility::SetEnemies(10, 11);
    MissionUtility::SetEnemies(12, 13);
    MissionUtility::SetEnemies(14, 15);
    MissionUtility::SetEnemiesOneWay(1, 11);
    MissionUtility::SetEnemiesOneWay(1, 13);
    MissionUtility::SetEnemiesOneWay(1, 15);
    MissionUtility::SetAlliance(1, 10);
    MissionUtility::SetAlliance(1, 12);
    MissionUtility::SetAlliance(1, 14);
    mFlags[69] = true;
    mFlags[28] = false;
    }

    // ---- +0x0890  36 bytes ----
    if (MissionUtility::GetGameClock() > 540.0f) {
    if (!mFlags[0]) {
    MissionUtility::BonusObjectiveFailed(mInts[16]);
    }
    }

    // ---- +0x08b4  36 bytes ----
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[43]); i++)
        MissionUtility::DamageObject(MissionUtility::GetRegionNewMember(mHandles[43], i),
                                     99999.0f, 99999.0f);

    // ---- +0x08d8  72 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "hangar_kill")) {
    if (!mFlags[3]) {
    MissionUtility::DamageObject(mHandles[171], 999999.0f, 999999.0f);
    mFlags[3] = true;
    }
    }

    // ---- +0x0920  44 bytes ----
    if (!MissionUtility::IsAlive(mHandles[171])) {
    if (!mFlags[41]) {
    BeginTimer(mTimer1);
    mFlags[41] = true;
    }
    }

    // ---- +0x094c  44 bytes ----
    if (mTimer1 > 5.0f) {
    if (!mFlags[1]) {
    MissionUtility::MissionFailure();
    mFlags[1] = true;
    }
    }

    // ---- +0x0978  160 bytes ----
    if (!MissionUtility::IsAlive(mHandles[133])) {
    if (!MissionUtility::IsAlive(mHandles[134])) {
    if (!MissionUtility::IsAlive(mHandles[135])) {
    if (!MissionUtility::IsAlive(mHandles[136])) {
    if (!MissionUtility::IsAlive(mHandles[137])) {
    if (!MissionUtility::IsAlive(mHandles[138])) {
    if (!MissionUtility::IsAlive(mHandles[139])) {
    if (!MissionUtility::IsAlive(mHandles[140])) {
    if (!mFlags[53]) {
    MissionUtility::BonusObjectiveComplete(mInts[15], true);
    mFlags[53] = true;
    }
    }
    }
    }
    }
    }
    }
    }
    }

    // ---- +0x0a18  52 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[126]) == 7) {
    if (!mFlags[34]) {
    MissionUtility::DisplayText("missions.Thule1.text.str0011", 5.0f, -1.0f);
    mFlags[34] = true;
    }
    }

    // ---- +0x0a4c  52 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[126]) == 6) {
    if (!mFlags[35]) {
    MissionUtility::DisplayText("missions.Thule1.text.str0012", 5.0f, -1.0f);
    mFlags[35] = true;
    }
    }

    // ---- +0x0a80  52 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[126]) == 5) {
    if (!mFlags[36]) {
    MissionUtility::DisplayText("missions.Thule1.text.str0013", 5.0f, -1.0f);
    mFlags[36] = true;
    }
    }

    // ---- +0x0ab4  52 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[126]) == 4) {
    if (!mFlags[37]) {
    MissionUtility::DisplayText("missions.Thule1.text.str0014", 5.0f, -1.0f);
    mFlags[37] = true;
    }
    }

    // ---- +0x0ae8  52 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[126]) == 3) {
    if (!mFlags[38]) {
    MissionUtility::DisplayText("missions.Thule1.text.str0015", 5.0f, -1.0f);
    mFlags[38] = true;
    }
    }

    // ---- +0x0b1c  52 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[126]) == 2) {
    if (!mFlags[39]) {
    MissionUtility::DisplayText("missions.Thule1.text.str0016", 5.0f, -1.0f);
    mFlags[39] = true;
    }
    }

    // ---- +0x0b50  52 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[126]) == 1) {
    if (!mFlags[40]) {
    MissionUtility::DisplayText("missions.Thule1.text.str0017", 5.0f, -1.0f);
    mFlags[40] = true;
    }
    }

    // ---- +0x0b84  392 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "gen1_proparmy_set1_on")) {
    if (!mFlags[23]) {
    mHandles[56] = MissionUtility::AddPropArmy(3, "set1_prop_armyfight", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[56], "set1_prop_armyfight", false);
    mHandles[73] = MissionUtility::AddPropArmy(2, "set1_prop_armyfight", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[73], "set1_prop_armyfight", false);
    mHandles[57] = MissionUtility::AddPropArmy(3, "set1_prop_armyfight", 2, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[57], "set1_prop_armyfight", false);
    mHandles[74] = MissionUtility::AddPropArmy(2, "set1_prop_armyfight", 3, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[74], "set1_prop_armyfight", false);
    mHandles[58] = MissionUtility::AddPropArmy(3, "set1_prop_armyfight2", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[58], "set1_prop_armyfight2", false);
    mHandles[75] = MissionUtility::AddPropArmy(2, "set1_prop_armyfight2", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[75], "set1_prop_armyfight2", false);
    mHandles[60] = MissionUtility::AddPropArmy(3, "set1_prop_armyfight4", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[60], "set1_prop_armyfight4", false);
    mHandles[77] = MissionUtility::AddPropArmy(2, "set1_prop_armyfight4", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[77], "set1_prop_armyfight4", false);
    mFlags[23] = true;
    }
    }

    // ---- +0x0d0c  104 bytes ----
    if (!MissionUtility::IsInsideRegion(mHandles[171], "gen1_proparmy_set1_on")) {
    if (mFlags[23]) {
    MissionUtility::RemoveArmy(mHandles[56]);
    MissionUtility::RemoveArmy(mHandles[57]);
    MissionUtility::RemoveArmy(mHandles[58]);
    MissionUtility::RemoveArmy(mHandles[60]);
    MissionUtility::RemoveArmy(mHandles[73]);
    MissionUtility::RemoveArmy(mHandles[74]);
    MissionUtility::RemoveArmy(mHandles[75]);
    MissionUtility::RemoveArmy(mHandles[77]);
    mFlags[23] = false;
    }
    }

    // ---- +0x0d74  320 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "gen1_proparmy_set2_on")) {
    if (!mFlags[24]) {
    MissionUtility::RemoveArmy(mHandles[90]);
    MissionUtility::RemoveArmy(mHandles[91]);
    mHandles[69] = MissionUtility::AddPropArmy(3, "set2_prop_armyfight13", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[69], "set2_prop_armyfight13", false);
    mHandles[86] = MissionUtility::AddPropArmy(2, "set2_prop_armyfight13", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[86], "set2_prop_armyfight13", false);
    mHandles[70] = MissionUtility::AddPropArmy(3, "set2_prop_armyfight14", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[70], "set2_prop_armyfight14", false);
    mHandles[87] = MissionUtility::AddPropArmy(2, "set2_prop_armyfight14", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[87], "set2_prop_armyfight14", false);
    mHandles[71] = MissionUtility::AddPropArmy(3, "set2_prop_armyfight15", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[71], "set2_prop_armyfight15", false);
    mHandles[88] = MissionUtility::AddPropArmy(2, "set2_prop_armyfight15", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[88], "set2_prop_armyfight15", false);
    mFlags[24] = true;
    }
    }

    // ---- +0x0eb4  88 bytes ----
    if (!MissionUtility::IsInsideRegion(mHandles[171], "gen1_proparmy_set2_on")) {
    if (mFlags[24]) {
    MissionUtility::RemoveArmy(mHandles[69]);
    MissionUtility::RemoveArmy(mHandles[70]);
    MissionUtility::RemoveArmy(mHandles[71]);
    MissionUtility::RemoveArmy(mHandles[86]);
    MissionUtility::RemoveArmy(mHandles[87]);
    MissionUtility::RemoveArmy(mHandles[88]);
    mFlags[24] = false;
    }
    }

    // ---- +0x0f0c  40 bytes ----
    if (mFlags[104]) {
        if (mTimer2 > 129.0f) {
        BeginTimer(mTimer2);
        }
    }

    switch (mPhase) {
    default:
    // ---- +0x0f44  24 bytes ----

        break;
    case 0:
    // ---- +0x0f5c  560 bytes ----
    if (!mFlags[32]) {
        mHandles[608] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "OpenCinGunshipPath", 0, "OpenCinGunship", 0, -1, -1);
        mHandles[609] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "OpenCinGunshipPath1", 0, "OpenCinGunship1", 1, -1, -1);
        mHandles[610] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "OpenCinGunshipPath2", 0, "OpenCinGunship2", 1, -1, -1);
        mHandles[611] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "OpenCinGunshipPath3", 0, "OpenCinGunship3", 1, -1, -1);
        mHandles[612] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "OpenCinGunshipPath4", 0, "OpenCinGunship4", 1, -1, -1);
        MissionUtility::OverrideSoundRange(mHandles[608], true);
        MissionUtility::OverrideSoundRange(mHandles[609], true);
        MissionUtility::OverrideSoundRange(mHandles[610], true);
        MissionUtility::OverrideSoundRange(mHandles[611], true);
        MissionUtility::OverrideSoundRange(mHandles[612], true);
        MissionUtility::SetVelocMinimumFly(mHandles[608], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[608], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[608], 200.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[609], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[609], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[609], 200.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[610], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[610], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[610], 200.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[611], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[611], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[611], 200.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[612], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[612], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[612], 200.0f);
        MissionUtility::Goto(mHandles[608], "OpenCinGunshipPath", false);
        MissionUtility::Goto(mHandles[609], "OpenCinGunshipPath1", false);
        MissionUtility::Goto(mHandles[610], "OpenCinGunshipPath2", false);
        MissionUtility::Goto(mHandles[611], "OpenCinGunshipPath3", false);
        MissionUtility::Goto(mHandles[612], "OpenCinGunshipPath4", false);
        mHandles[281] = MissionUtility::RunCin("Cin1", true, true);
        mFlags[32] = true;
        BeginTimer(mTimer24);
        MissionUtility::PlayMusic("EP4_V2_T06_01", true);
    }

    // ---- +0x118c  72 bytes ----
    if (mTimer24 > 7.0f) {
    StopTimer(mTimer24);
    mTimer24 = 0.0f;
    MissionUtility::QueueSound("mwt21_07", 1.0f, 0.0f, 0.0f, "", 0, "");
    }

    // ---- +0x11d4  216 bytes ----
    if (!mFlags[173]) {
        if (MissionUtility::GetCinId(mHandles[281]) == 2) {
        mFlags[173] = true;
        MissionUtility::SetVelocMinimumFly(mHandles[608], 300.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[608], 300.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[608], 300.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[609], 300.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[609], 300.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[609], 300.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[610], 300.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[610], 300.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[610], 300.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[611], 300.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[611], 300.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[611], 300.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[612], 300.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[612], 300.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[612], 300.0f);
        }
    }

    // ---- +0x12ac  316 bytes ----
    if (!mFlags[174]) {
        if (MissionUtility::GetCinId(mHandles[281]) == 3) {
        mFlags[174] = true;
        MissionUtility::MoveObjectWithRotation(mHandles[608], "OpenCinGunshipPathA", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[609], "OpenCinGunshipPath1A", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[610], "OpenCinGunshipPath2A", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[611], "OpenCinGunshipPath3A", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[612], "OpenCinGunshipPath4A", 0, true);
        MissionUtility::SetVelocMinimumFly(mHandles[608], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[608], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[608], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[609], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[609], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[609], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[610], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[610], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[610], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[611], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[611], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[611], 0.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[612], 0.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[612], 0.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[612], 0.0f);
        }
    }

    // ---- +0x13e8  456 bytes ----
    if (!mFlags[175]) {
        if (MissionUtility::GetCinId(mHandles[281]) == 4) {
        MissionUtility::QueueSound("obt21_08", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("mwt21_08", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetVelocMinimumFly(mHandles[608], 220.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[608], 220.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[608], 220.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[609], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[609], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[609], 200.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[610], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[610], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[610], 200.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[611], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[611], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[611], 200.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[612], 200.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[612], 200.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[612], 200.0f);
        MissionUtility::Goto(mHandles[608], "OpenCinGunshipPathA", false);
        MissionUtility::Goto(mHandles[609], "OpenCinGunshipPath1A", false);
        MissionUtility::Goto(mHandles[610], "OpenCinGunshipPath2A", false);
        MissionUtility::Goto(mHandles[611], "OpenCinGunshipPath3A", false);
        MissionUtility::Goto(mHandles[612], "OpenCinGunshipPath4A", false);
        MissionUtility::SetCurHealth(mHandles[609], 1.0f);
        MissionUtility::SetCurHealth(mHandles[610], 1.0f);
        MissionUtility::SetCurHealth(mHandles[611], 1.0f);
        MissionUtility::SetCurHealth(mHandles[612], 1.0f);
        MissionUtility::SetCurShield(mHandles[609], 0.0f);
        MissionUtility::SetCurShield(mHandles[610], 0.0f);
        MissionUtility::SetCurShield(mHandles[611], 0.0f);
        MissionUtility::SetCurShield(mHandles[612], 0.0f);
        mFlags[175] = true;
        }
    }

    // ---- +0x15b0  200 bytes ----
    if (!mFlags[176]) {
        if (MissionUtility::GetCinId(mHandles[281]) == 5) {
        if (MissionUtility::IsAlive(mHandles[609])) {
        MissionUtility::DamageObject(mHandles[609], 1000.0f, 1000.0f);
        }
        if (MissionUtility::IsAlive(mHandles[610])) {
        MissionUtility::DamageObject(mHandles[610], 1000.0f, 1000.0f);
        }
        if (MissionUtility::IsAlive(mHandles[611])) {
        MissionUtility::DamageObject(mHandles[611], 1000.0f, 1000.0f);
        }
        if (MissionUtility::IsAlive(mHandles[612])) {
        MissionUtility::DamageObject(mHandles[612], 1000.0f, 1000.0f);
        }
        MissionUtility::SetVelocMinimumFly(mHandles[608], 190.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[608], 190.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[608], 190.0f);
        mFlags[176] = true;
        }
    }

    // ---- +0x1678  132 bytes ----
    if (!mFlags[177]) {
        if (MissionUtility::GetCinId(mHandles[281]) == 6) {
        MissionUtility::SetVelocMinimumFly(mHandles[608], 300.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[608], 300.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[608], 300.0f);
        MissionUtility::Goto(mHandles[608], "OpenCinGunshipPathB", false);
        MissionUtility::SetCollidable(mHandles[608], false);
        MissionUtility::QueueSound("obt21_02", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[177] = true;
        }
    }

    // ---- +0x16fc  300 bytes ----
    if (mFlags[69]) {
        if (!MissionUtility::IsCinRunning(mHandles[281])) {
        if (mFlags[105]) {
        BeginTimer(mTimer20);
        MissionUtility::FlushSoundQueue();
        StopTimer(mTimer24);
        mTimer24 = 0.0f;
        MissionUtility::RemoveObject(mHandles[608]);
        MissionUtility::RemoveObject(mHandles[609]);
        MissionUtility::RemoveObject(mHandles[610]);
        MissionUtility::RemoveObject(mHandles[611]);
        MissionUtility::RemoveObject(mHandles[612]);
        MissionUtility::PlayMusic("EP1_V1_T05", true);
        mTimer2 = 130.0f;
        mInts[1] = MissionUtility::AddObjective("missions.Thule1.objective.str0010");
        MissionUtility::DisplayText("missions.Thule1.objective.str0010", 5.0f, -1.0f);
        MissionUtility::Objectify("lpad", 0, "missions.Thule1.marker.str0000", true, false, 0.0f, 2.0f);
        MissionUtility::MoveObject(mHandles[171], Vector(700.38416f, 121.63165f, -2934.0142f), Quat(0.689541f, 0.0f, 0.724247f, 0.0f), true);
        mFlags[105] = false;
        }
        }
    }

    // ---- +0x1828  136 bytes ----
    if (MissionUtility::GetDistance(mHandles[171], mHandles[321]) < 1000.0f) {
    if (!mFlags[52]) {
    MissionUtility::DisbandFlock(mHandles[321]);
    MissionUtility::AttackTarget(mHandles[318], mHandles[171], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[319], mHandles[171], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[320], mHandles[171], true, true, false, false);
    mFlags[52] = true;
    }
    }

    // ---- +0x18b0  128 bytes ----
    if (!MissionUtility::IsFlockAlive(mHandles[322])) {
    if (!mFlags[51]) {
    MissionUtility::StartSound("OBT21_02A", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    BeginTimer(mTimer21);
    MissionUtility::Goto(mHandles[318], "landing_fighter_spawn", true);
    MissionUtility::Goto(mHandles[319], "landing_fighter_spawn", true);
    MissionUtility::Goto(mHandles[320], "landing_fighter_spawn", true);
    mFlags[51] = true;
    }
    }

    // ---- +0x1930  912 bytes ----
    if (mTimer21 > 5.0f) {
    if (!mFlags[71]) {
    MissionUtility::PlayMusic("ep5_v1_t05_01", true);
    MissionUtility::RemoveObjectify("lpad", 0);
    mHandles[282] = MissionUtility::RunCin("Cin2", true, true);
    StopTimer(mTimer20);
    mHandles[38] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 0, "troop1", 1, -1);
    mHandles[39] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 1, "troop2", 1, -1);
    mHandles[40] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 2, "troop3", 1, -1);
    mHandles[41] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 3, "troop4", 1, -1);
    mHandles[42] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 4, "troop5", 1, -1);
    MissionUtility::MoveObject(mHandles[171], Vector(3295.7715f, 99.91385f, -3523.73f), Quat(0.935861f, 0.0f, -0.35237f, 0.0f), true);
    MissionUtility::RemoveObject(mHandles[318]);
    MissionUtility::RemoveObject(mHandles[319]);
    MissionUtility::RemoveObject(mHandles[320]);
    mFlags[71] = true;
    MissionUtility::SetVelocNeutralFly(mHandles[171], 0.0f);
    mHandles[174] = MissionUtility::CreateObject("rep_fly_assault", Vector(2811.9082f, 150.91821f, -2523.271f), "rep_dropship", 1, -1, Quat(0.497562f, 0.0f, -0.867428f, 0.0f), -1);
    MissionUtility::SetApplyDynamics(mHandles[174], true);
    MissionUtility::Land(mHandles[174], 0, 0, 80.0f);
    mHandles[175] = MissionUtility::CreateObject("rep_fly_assault", Vector(2215.3833f, 130.56618f, -2952.762f), "rep_dropship2", 1, -1, Quat(0.999205f, 0.0f, -0.039873f, 0.0f), -1);
    MissionUtility::SetApplyDynamics(mHandles[175], true);
    MissionUtility::Land(mHandles[175], 0, 0, 80.0f);
    }
    }

    // ---- +0x1cc0  132 bytes ----
    if (mFlags[119] && !mFlags[46]) {
        if (MissionUtility::IsInsideRegion(mHandles[171], "land_land")) {
        MissionUtility::MoveObjectWithRotation(mHandles[171], "landing_land_startpoint", 0, true);
        MissionUtility::Land(mHandles[171], "lpad", 0, 80.0f);
        MissionUtility::SetTeamNum(mHandles[171], 0);
        mHandles[294] = MissionUtility::RunCin("landing_pickup", true, false);
        StopTimer(mTimer20);
        mFlags[46] = true;
        }
    }

    // ---- +0x1d44  200 bytes ----
    if (!MissionUtility::IsFlockAlive(mHandles[322])) {
    if (mFlags[71]) {
    if (!MissionUtility::IsCinRunning(mHandles[282])) {
    if (!mFlags[72]) {
    MissionUtility::RemoveObject(mHandles[38]);
    MissionUtility::RemoveObject(mHandles[39]);
    MissionUtility::RemoveObject(mHandles[40]);
    MissionUtility::RemoveObject(mHandles[41]);
    MissionUtility::RemoveObject(mHandles[42]);
    MissionUtility::FlushSoundQueue();
    MissionUtility::ObjectiveComplete(mInts[1]);
    MissionUtility::RemoveObject(mHandles[187]);
    MissionUtility::RemoveObject(mHandles[188]);
    MissionUtility::RemoveObject(mHandles[189]);
    mInts[3] = MissionUtility::AddObjective("missions.Thule1.objective.str0002");
    MissionUtility::DisplayText("missions.Thule1.text.str0003", 5.0f, -1.0f);
    MissionUtility::Objectify("lpad", 0, "missions.Thule1.marker.str0001", true, true, 0.0f, 2.0f);
    mFlags[72] = true;
    }
    }
    }
    }

    // ---- +0x1e0c  160 bytes ----
    if (!MissionUtility::IsAlive(mHandles[318])) {
    if (MissionUtility::IsFlockAlive(mHandles[322])) {
    if (MissionUtility::IsInsideRegion(mHandles[171], "l_fighter_spawn")) {
    mHandles[318] = MissionUtility::CreateObject("cis_fly_fighter_combat", "landing_fighter_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[318], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0x1eac  160 bytes ----
    if (!MissionUtility::IsAlive(mHandles[319])) {
    if (MissionUtility::IsFlockAlive(mHandles[322])) {
    if (MissionUtility::IsInsideRegion(mHandles[171], "l_fighter_spawn")) {
    mHandles[319] = MissionUtility::CreateObject("cis_fly_fighter_combat", "landing_fighter_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[319], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0x1f4c  160 bytes ----
    if (!MissionUtility::IsAlive(mHandles[320])) {
    if (MissionUtility::IsFlockAlive(mHandles[322])) {
    if (MissionUtility::IsInsideRegion(mHandles[171], "l_fighter_spawn")) {
    mHandles[320] = MissionUtility::CreateObject("cis_fly_fighter_combat", "landing_fighter_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[320], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0x1fec  312 bytes ----
    if (mFlags[71]) {
        if (!MissionUtility::IsCinRunning(mHandles[282])) {
        if (!mFlags[74]) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::RemoveObject(mHandles[174]);
        MissionUtility::RemoveObject(mHandles[175]);
        MissionUtility::RemoveObject(mHandles[176]);
        MissionUtility::RemoveObject(mHandles[177]);
        mHandles[176] = MissionUtility::CreateObject("rep_fly_assault", Vector(2822.6416f, -69.72191f, -2520.7546f), "", 1, -1, Quat(0.497562f, 0.0f, -0.867428f, 0.0f), -1);
        mHandles[177] = MissionUtility::CreateObject("rep_fly_assault", Vector(2215.3833f, -57.566177f, -2952.762f), "", 1, -1, Quat(0.999205f, 0.0f, -0.039873f, 0.0f), -1);
        mFlags[74] = true;
        mPhase = 1;
        }
        }
    }

    // ---- +0x2124  148 bytes ----
    if (MissionUtility::IsCinRunning(mHandles[281])) {
    if (MissionUtility::GetCinId(mHandles[281]) == 4) {
    if (!mFlags[70]) {
    MissionUtility::MoveObject(mHandles[171], Vector(-953.38416f, 121.63165f, -2934.0142f), Quat(0.689541f, 0.0f, 0.724247f, 0.0f), true);
    mFlags[70] = true;
    }
    }
    }

    // ---- +0x21b8  300 bytes ----
    if (MissionUtility::IsCinRunning(mHandles[282])) {
    if (MissionUtility::GetCinId(mHandles[282]) == 4) {
    if (!mFlags[106]) {
    BeginTimer(mTimer10);
    MissionUtility::RemoveObject(mHandles[174]);
    MissionUtility::RemoveObject(mHandles[175]);
    mHandles[176] = MissionUtility::CreateObject("rep_fly_assault", Vector(2822.6416f, -69.72191f, -2520.7546f), "", 1, -1, Quat(0.497562f, 0.0f, -0.867428f, 0.0f), -1);
    mHandles[177] = MissionUtility::CreateObject("rep_fly_assault", Vector(2215.3833f, -57.566177f, -2952.762f), "", 1, -1, Quat(0.999205f, 0.0f, -0.039873f, 0.0f), -1);
    mFlags[106] = true;
    }
    }
    }

    // ---- +0x22e4  136 bytes ----
    if (mTimer10 > 0.0f) {
    if (!mFlags[107]) {
    mHandles[181] = MissionUtility::CreateObject("rep_tank_fighter1", "tank_spawn", 0, "tank1", 1, -1);
    MissionUtility::Goto(mHandles[181], "tank_goto", 0);
    mFlags[107] = true;
    }
    }

    // ---- +0x236c  136 bytes ----
    if (mTimer10 > 5.0f) {
    if (!mFlags[108]) {
    mHandles[182] = MissionUtility::CreateObject("rep_tank_fighter1", "tank_spawn", 0, "tank2", 1, -1);
    MissionUtility::Goto(mHandles[182], "tank_goto", 1);
    mFlags[108] = true;
    }
    }

    // ---- +0x23f4  136 bytes ----
    if (mTimer10 > 10.0f) {
    if (!mFlags[109]) {
    mHandles[183] = MissionUtility::CreateObject("rep_tank_fighter1", "tank_spawn", 0, "tank3", 1, -1);
    MissionUtility::Goto(mHandles[183], "tank_goto", 2);
    mFlags[109] = true;
    }
    }

    // ---- +0x247c  136 bytes ----
    if (mTimer10 > 15.0f) {
    if (!mFlags[110]) {
    mHandles[184] = MissionUtility::CreateObject("rep_tank_fighter1", "tank_spawn", 0, "tank4", 1, -1);
    MissionUtility::Goto(mHandles[184], "tank_goto", 3);
    mFlags[110] = true;
    }
    }

    // ---- +0x2504  136 bytes ----
    if (mTimer10 > 20.0f) {
    if (!mFlags[111]) {
    mHandles[185] = MissionUtility::CreateObject("rep_tank_fighter1", "tank_spawn", 0, "tank5", 1, -1);
    MissionUtility::Goto(mHandles[185], "tank_goto", 4);
    mFlags[111] = true;
    }
    }

    // ---- +0x258c  136 bytes ----
    if (mTimer10 > 25.0f) {
    if (!mFlags[112]) {
    mHandles[186] = MissionUtility::CreateObject("rep_tank_fighter1", "tank_spawn", 0, "tank6", 1, -1);
    MissionUtility::Goto(mHandles[186], "tank_goto", 5);
    mFlags[112] = true;
    }
    }

    // ---- +0x2614  1068 bytes ----
    if (MissionUtility::IsCinRunning(mHandles[282])) {
    if (MissionUtility::GetCinId(mHandles[282]) == 3) {
    if (!mFlags[73]) {
    mHandles[241] = MissionUtility::CreateObject("rep_walk_sixleg", Vector(2688.0f, -120.554375f, -2800.0f), "", 6, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
    mHandles[243] = MissionUtility::CreateObject("rep_walk_sixleg", Vector(2640.0f, -120.91637f, -2720.0f), "", 6, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
    mHandles[242] = MissionUtility::CreateObject("rep_walk_sixleg", Vector(2736.0f, -120.933334f, -2720.0f), "", 6, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
    mHandles[178] = MissionUtility::CreateFlock(mHandles[241], (Formation)8);
    MissionUtility::AddFlockMember(mHandles[178], mHandles[243], Vector(-48.0f, 0.0f, -80.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[242], Vector(48.0f, 0.0f, -80.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[244], Vector(-24.0f, 0.0f, -128.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[245], Vector(88.0f, 0.0f, -96.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[246], Vector(24.0f, 0.0f, -128.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[247], Vector(-88.0f, 0.0f, -96.0f));
    MissionUtility::Goto(mHandles[178], "assault1_goto1", true);
    mHandles[187] = MissionUtility::CreateObject("rep_fly_gunship", "cin2_gunship_flight", 0, "walker_dropship1", 1, -1);
    MissionUtility::GotoDirect(mHandles[187], "cin2_gunship_flight", true);
    mHandles[188] = MissionUtility::CreateObject("rep_fly_gunship", "cin2_gunship_flight2", 0, "walker_dropship2", 1, -1);
    MissionUtility::GotoDirect(mHandles[188], "cin2_gunship_flight2", true);
    mHandles[189] = MissionUtility::CreateObject("rep_fly_gunship", "cin2_gunship_flight3", 0, "walker_dropship3", 1, -1);
    MissionUtility::GotoDirect(mHandles[189], "cin2_gunship_flight3", true);
    mFlags[73] = true;
    }
    }
    }

    // ---- +0x2a40  76 bytes ----
    if (MissionUtility::GetDistance(mHandles[187], "cin2_gunship_flight", 1) < 50.0f) {
    if (!mFlags[113]) {
    BeginTimer(mTimer9);
    MissionUtility::Land(mHandles[187], 0, 0, 80.0f);
    mFlags[113] = true;
    }
    }

    // ---- +0x2a8c  68 bytes ----
    if (MissionUtility::GetDistance(mHandles[188], "cin2_gunship_flight2", 1) < 50.0f) {
    if (!mFlags[114]) {
    MissionUtility::Land(mHandles[188], 0, 0, 80.0f);
    mFlags[114] = true;
    }
    }

    // ---- +0x2ad0  68 bytes ----
    if (MissionUtility::GetDistance(mHandles[189], "cin2_gunship_flight3", 1) < 50.0f) {
    if (!mFlags[115]) {
    MissionUtility::Land(mHandles[189], 0, 0, 80.0f);
    mFlags[115] = true;
    }
    }

    // ---- +0x2b14  92 bytes ----
    if (mFlags[113]) {
        if (mTimer9 > 3.0f) {
        if (!mFlags[116]) {
        MissionUtility::TakeOff(mHandles[187]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::GotoDirect(mHandles[187], "walker_dropship1_exit", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[116] = true;
        }
        }
    }

    // ---- +0x2b70  92 bytes ----
    if (mFlags[114]) {
        if (mTimer9 > 5.0f) {
        if (!mFlags[117]) {
        MissionUtility::TakeOff(mHandles[188]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::GotoDirect(mHandles[188], "walker_dropship2_exit", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[117] = true;
        }
        }
    }

    // ---- +0x2bcc  92 bytes ----
    if (mFlags[115]) {
        if (mTimer9 > 4.0f) {
        if (!mFlags[118]) {
        MissionUtility::TakeOff(mHandles[189]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::GotoDirect(mHandles[189], "walker_dropship3_exit", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[118] = true;
        }
        }
    }

    // ---- +0x2c28  132 bytes ----
    if (MissionUtility::IsCinRunning(mHandles[282])) {
    if (MissionUtility::GetCinId(mHandles[282]) == 2) {
    if (!mFlags[101]) {
    MissionUtility::QueueSound("MWT21_09", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("OBT21_04", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[101] = true;
    }
    }
    }

        break;
    case 1:
    // ---- +0x2cac  128 bytes ----
    MissionUtility::MidMissionSavePlayer(2);
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[133]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[134]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[135]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[136]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[137]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[138]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[139]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[140]));
    MissionUtility::MidMissionSave(mTimer20);
    mPhase = 3;

        break;
    case 2:
    // ---- +0x2d2c  1156 bytes ----
    mInts[1] = MissionUtility::AddObjective("missions.Thule1.objective.str0010");
    mInts[3] = MissionUtility::AddObjective("missions.Thule1.objective.str0002");
    MissionUtility::ObjectiveComplete(mInts[1]);
    mHandles[176] = MissionUtility::CreateObject("rep_fly_assault", Vector(2822.6416f, -69.72191f, -2520.7546f), "", 1, -1, Quat(0.497562f, 0.0f, -0.867428f, 0.0f), -1);
    mHandles[177] = MissionUtility::CreateObject("rep_fly_assault", Vector(2215.3833f, -57.566177f, -2952.762f), "", 1, -1, Quat(0.999205f, 0.0f, -0.039873f, 0.0f), -1);
    mHandles[241] = MissionUtility::CreateObject("rep_walk_sixleg", Vector(2688.0f, -120.554375f, -2800.0f), "", 6, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
    mHandles[243] = MissionUtility::CreateObject("rep_walk_sixleg", Vector(2640.0f, -120.91637f, -2720.0f), "", 6, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
    mHandles[242] = MissionUtility::CreateObject("rep_walk_sixleg", Vector(2736.0f, -120.933334f, -2720.0f), "", 6, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
    mHandles[178] = MissionUtility::CreateFlock(mHandles[241], (Formation)8);
    MissionUtility::AddFlockMember(mHandles[178], mHandles[243], Vector(-48.0f, 0.0f, -80.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[242], Vector(48.0f, 0.0f, -80.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[244], Vector(-24.0f, 0.0f, -128.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[245], Vector(88.0f, 0.0f, -96.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[246], Vector(24.0f, 0.0f, -128.0f));
    MissionUtility::AddFlockMember(mHandles[178], mHandles[247], Vector(-88.0f, 0.0f, -96.0f));
    MissionUtility::Goto(mHandles[178], "assault1_goto1", true);
    MissionUtility::RemoveFlock(mHandles[322], false);
    MissionUtility::Objectify("lpad", 0, "missions.Thule1.marker.str0001", true, true, 0.0f, 2.0f);
    MissionUtility::RemoveObject(mHandles[318]);
    MissionUtility::RemoveObject(mHandles[319]);
    MissionUtility::RemoveObject(mHandles[320]);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mLoadA0);
    MissionUtility::MidMissionLoad(mLoadA1);
    MissionUtility::MidMissionLoad(mLoadA2);
    MissionUtility::MidMissionLoad(mLoadA3);
    MissionUtility::MidMissionLoad(mLoadA4);
    MissionUtility::MidMissionLoad(mLoadA5);
    MissionUtility::MidMissionLoad(mLoadA6);
    MissionUtility::MidMissionLoad(mLoadA7);
    MissionUtility::MidMissionLoad(mLoadTA);
    if (!mLoadA0) {
    MissionUtility::DamageObject(mHandles[133], 99999.0f, 99999.0f);
    }

    // ---- +0x31b0  28 bytes ----
    if (!mLoadA1) {
    MissionUtility::DamageObject(mHandles[134], 99999.0f, 99999.0f);
    }

    // ---- +0x31cc  28 bytes ----
    if (!mLoadA2) {
    MissionUtility::DamageObject(mHandles[135], 99999.0f, 99999.0f);
    }

    // ---- +0x31e8  28 bytes ----
    if (!mLoadA3) {
    MissionUtility::DamageObject(mHandles[136], 99999.0f, 99999.0f);
    }

    // ---- +0x3204  28 bytes ----
    if (!mLoadA4) {
    MissionUtility::DamageObject(mHandles[137], 99999.0f, 99999.0f);
    }

    // ---- +0x3220  28 bytes ----
    if (!mLoadA5) {
    MissionUtility::DamageObject(mHandles[138], 99999.0f, 99999.0f);
    }

    // ---- +0x323c  28 bytes ----
    if (!mLoadA6) {
    MissionUtility::DamageObject(mHandles[139], 99999.0f, 99999.0f);
    }

    // ---- +0x3258  28 bytes ----
    if (!mLoadA7) {
    MissionUtility::DamageObject(mHandles[140], 99999.0f, 99999.0f);
    }

    // ---- +0x3274  40 bytes ----
    StopTimer(mTimer20);
    mTimer20 = mLoadTA;
    ResumeTimer(mTimer20);
    mPhase = 3;

        break;
    case 3:
    // ---- +0x329c  680 bytes ----
    if (mFlags[120]) {
        MissionUtility::PlayMusic("EP4_V2_T06_01", true);
        MissionUtility::AddFlyerAmmoBox("health_ammo_spawn", 0, -1.0f);
        MissionUtility::AddFlyerHealthBox("health_ammo_spawn", 1, -1.0f);
        MissionUtility::MoveObject(mHandles[171], Vector(3295.7715f, 99.91385f, -3523.73f), Quat(0.935861f, 0.0f, -0.35237f, 0.0f), true);
        MissionUtility::SetVelocNeutralFly(mHandles[171], 110.0f);
        mHandles[303] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 0, "troop1", 1, -1);
        mHandles[304] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 1, "troop2", 1, -1);
        mHandles[305] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 2, "troop3", 1, -1);
        mHandles[306] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 3, "troop4", 1, -1);
        mHandles[307] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 4, "troop5", 1, -1);
        MissionUtility::SetCurHealth(mHandles[303], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[304], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[305], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[306], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[307], 999999.0f);
        MissionUtility::Cargo(mHandles[303]);
        MissionUtility::Cargo(mHandles[304]);
        MissionUtility::Cargo(mHandles[305]);
        MissionUtility::Cargo(mHandles[306]);
        MissionUtility::Cargo(mHandles[307]);
        mFlags[119] = true;
        mFlags[120] = false;
    }

    // ---- +0x3544  160 bytes ----
    if (!MissionUtility::IsAlive(mHandles[32])) {
    if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
    if (!mFlags[124]) {
    mHandles[32] = MissionUtility::CreateObject("cis_fly_fighter_combat", "cis_flier_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[32], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0x35e4  160 bytes ----
    if (!MissionUtility::IsAlive(mHandles[33])) {
    if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
    if (!mFlags[124]) {
    mHandles[33] = MissionUtility::CreateObject("cis_fly_fighter_combat", "cis_flier_spawn", 1, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[33], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0x3684  2624 bytes ----
    if (mFlags[119]) {
        if (MissionUtility::IsLoaded(mHandles[303], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[304], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[305], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[306], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[307], mHandles[171])) {
        if (!mFlags[33]) {
        MissionUtility::TakeOff(mHandles[171]);
        MissionUtility::SetTeamNum(mHandles[171], 1);
        MissionUtility::StopCin(mHandles[294]);
        MissionUtility::RemoveObjectify("lpad", 0);
        MissionUtility::RemoveObjective(mInts[3]);
        mInts[4] = MissionUtility::AddObjective("missions.Thule1.objective.str0001");
        MissionUtility::DisplayText("missions.Thule1.text.str0002", 5.0f, -1.0f);
        MissionUtility::RemoveTurnAroundRegion("landing_turnaround");
        MissionUtility::RemoveTurnAroundRegion("landing_turnaround1");
        MissionUtility::AddTurnAroundRegion("gen1_turnaround", "gen1_turnaround_focus", 0, 0, 0, 0);
        MissionUtility::CreateDropOffRegion("generator1_land");
        MissionUtility::StartSound("CTR06_05", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Objectify(mHandles[141], "missions.Thule1.marker.str0002", true, true, 0.0f);
        mHandles[44] = MissionUtility::CreateObject("rep_fly_gunship", "amb_gunship1", 0, "", 0, -1);
        MissionUtility::Goto(mHandles[44], "amb_gunship1", true);
        mHandles[45] = MissionUtility::CreateObject("rep_fly_gunship", "amb_gunship2", 0, "", 0, -1);
        MissionUtility::Goto(mHandles[45], "amb_gunship2", true);
        mHandles[46] = MissionUtility::CreateObject("rep_fly_gunship", "amb_gunship3", 0, "", 0, -1);
        MissionUtility::Goto(mHandles[46], "amb_gunship3", true);
        mHandles[47] = MissionUtility::CreateObject("rep_fly_gunship", "amb_gunship4", 0, "", 0, -1);
        MissionUtility::Goto(mHandles[47], "amb_gunship4", true);
        MissionUtility::SetApplyDynamics(mHandles[137], true);
        MissionUtility::TakeOff(mHandles[137]);
        mHandles[48] = MissionUtility::AddFlyerArmy(0, "prop_flierfight1", 0, 2, 2.0f);
        mHandles[52] = MissionUtility::AddFlyerArmy(2, "prop_flierfight1", 0, 2, 2.0f);
        mHandles[49] = MissionUtility::AddFlyerArmy(1, "prop_flierfight2", 0, 2, 2.0f);
        mHandles[53] = MissionUtility::AddFlyerArmy(2, "prop_flierfight2", 0, 2, 2.0f);
        mHandles[50] = MissionUtility::AddFlyerArmy(1, "prop_flierfight3", 0, 5, 12.0f);
        mHandles[54] = MissionUtility::AddFlyerArmy(2, "prop_flierfight3", 0, 5, 12.0f);
        mHandles[59] = MissionUtility::AddPropArmy(3, "prop_armyfight3", 1, 50, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[59], "prop_armyfight2", false);
        mHandles[76] = MissionUtility::AddPropArmy(2, "prop_armyfight3", 0, 50, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[76], "prop_armyfight3", false);
        mHandles[68] = MissionUtility::AddPropArmy(3, "prop_armyfight12", 1, 50, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[68], "prop_armyfight12", false);
        mHandles[85] = MissionUtility::AddPropArmy(2, "prop_armyfight12", 0, 50, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[85], "prop_armyfight12", false);
        mHandles[72] = MissionUtility::AddPropArmy(3, "set2_prop_armyfight16", 1, 50, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[72], "set2_prop_armyfight16", false);
        mHandles[89] = MissionUtility::AddPropArmy(2, "set2_prop_armyfight16", 0, 50, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[89], "set2_prop_armyfight16", false);
        mHandles[482] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 0, "", 10, -1);
        MissionUtility::SetWeaponOrd(mHandles[482], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        MissionUtility::Defend(mHandles[482], "battle1_defend", 0, 600.0f);
        mHandles[484] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 3, "", 10, -1);
        MissionUtility::SetWeaponOrd(mHandles[484], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        MissionUtility::Defend(mHandles[484], "battle1_defend", 0, 600.0f);
        mHandles[485] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 5, "", 10, -1);
        MissionUtility::SetWeaponOrd(mHandles[485], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        MissionUtility::Defend(mHandles[485], "battle1_defend", 0, 600.0f);
        MissionUtility::SetCurHealth(mHandles[482], 9999.0f);
        MissionUtility::SetCurHealth(mHandles[483], 9999.0f);
        MissionUtility::SetCurHealth(mHandles[484], 9999.0f);
        MissionUtility::SetCurHealth(mHandles[485], 9999.0f);
        mHandles[479] = MissionUtility::CreateObject("rep_walk_sixleg", "b1_walker1", 0, "b1_rep_sixleg1", 6, -1);
        mHandles[480] = MissionUtility::CreateObject("rep_walk_sixleg", "b1_walker2", 0, "b1_rep_sixleg2", 6, -1);
        MissionUtility::SetVelocForward(mHandles[479], 10.0f);
        MissionUtility::SetVelocForward(mHandles[480], 10.0f);
        MissionUtility::Goto(mHandles[479], "b1_walker1", true);
        MissionUtility::Goto(mHandles[480], "b1_walker2", true);
        mHandles[481] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[481], mHandles[482]);
        MissionUtility::AddFlockMember(mHandles[481], mHandles[483]);
        MissionUtility::AddFlockMember(mHandles[481], mHandles[484]);
        MissionUtility::AddFlockMember(mHandles[481], mHandles[485]);
        mHandles[495] = MissionUtility::CreateObject("cis_tank_fighter", "battle1_spawn2", 0, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[495], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        MissionUtility::Defend(mHandles[495], "battle1_defend", 0, 600.0f);
        mHandles[496] = MissionUtility::CreateObject("cis_tank_wheeled", "battle1_spawn2", 1, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[496], "cis_missile_wheeled", "cis_missile_wheeled_weak_ord");
        MissionUtility::Defend(mHandles[496], "battle1_defend", 0, 600.0f);
        mHandles[497] = MissionUtility::CreateObject("cis_walk_assault", "battle1_spawn2", 2, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[497], "cis_beam_walk", "cis_beam_walk_weak_ord");
        MissionUtility::Defend(mHandles[497], "battle1_defend", 0, 600.0f);
        mHandles[498] = MissionUtility::CreateObject("cis_tank_fighter", "battle1_spawn2", 3, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[498], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        MissionUtility::Defend(mHandles[498], "battle1_defend", 0, 600.0f);
        mHandles[494] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[494], mHandles[495]);
        MissionUtility::AddFlockMember(mHandles[494], mHandles[496]);
        MissionUtility::AddFlockMember(mHandles[494], mHandles[497]);
        MissionUtility::AddFlockMember(mHandles[494], mHandles[498]);
        mHandles[566] = MissionUtility::CreateObject("cis_fly_fighter", "flier_fight1", 0, 20, -1, -1);
        mHandles[573] = MissionUtility::CreateObject("rep_fly_fighter", "flier_fight1", 0, 10, -1, -1);
        mHandles[559] = MissionUtility::CreateFlock(mHandles[566], (Formation)3);
        MissionUtility::Goto(mHandles[559], "flier_fight1", true);
        mHandles[572] = MissionUtility::CreateObject("cis_fly_fighter", "flier_fight1", 0, 20, -1, -1);
        mHandles[579] = MissionUtility::CreateObject("rep_fly_fighter", "flier_fight1", 0, 10, -1, -1);
        mHandles[565] = MissionUtility::CreateFlock(mHandles[572], (Formation)3);
        MissionUtility::Goto(mHandles[565], "flier_fight7", true);
        mHandles[90] = MissionUtility::AddPropArmy(2, "rep_army_march1_spawn", 0, 50, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[90], "rep_army_march1_spawn", true);
        mHandles[91] = MissionUtility::AddPropArmy(2, "rep_army_march2_spawn", 0, 50, 3.0f);
        MissionUtility::SetPropArmyWayPoints(mHandles[91], "rep_army_march2_spawn", true);
        mFlags[33] = true;
        }
        }
        }
        }
        }
        }
    }

    // ---- +0x40c4  88 bytes ----
    if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
    if (!mFlags[94]) {
    MissionUtility::RemoveObjectify(mHandles[141]);
    MissionUtility::StartSound("CTT21_01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[94] = true;
    }
    }

    // ---- +0x411c  72 bytes ----
    if (mFlags[33]) {
        if (!MissionUtility::IsFlockAlive(mHandles[385])) {
        if (!mFlags[30]) {
        MissionUtility::Objectify(mHandles[141], "missions.Thule1.marker.str0009", true, true, 0.0f);
        mFlags[30] = true;
        }
        }
    }

    // ---- +0x4164  160 bytes ----
    if (mFlags[33]) {
        if (!MissionUtility::IsFlockAlive(mHandles[385])) {
        if (!mFlags[125]) {
        if (MissionUtility::IsInsideRegion(mHandles[171], "gen1_land1")) {
        if (!mFlags[47]) {
        MissionUtility::MoveObjectWithRotation(mHandles[171], "gen1_land_startpoint", 0, true);
        MissionUtility::Land(mHandles[171], "gen1_land", 0, 80.0f);
        MissionUtility::SetTeamNum(mHandles[171], 0);
        mHandles[295] = MissionUtility::RunCin("gen1_pickup", true, false);
        StopTimer(mTimer20);
        mFlags[47] = true;
        }
        }
        }
        }
    }

    // ---- +0x4204  144 bytes ----
    if (mFlags[4] && mFlags[93]) {
        if (MissionUtility::IsInsideRegion(mHandles[171], "gen1_land2")) {
        if (!mFlags[48]) {
        MissionUtility::MoveObjectWithRotation(mHandles[171], "gen1_land_startpoint", 0, true);
        MissionUtility::Land(mHandles[171], "gen1_land", 0, 80.0f);
        MissionUtility::SetTeamNum(mHandles[171], 0);
        mHandles[295] = MissionUtility::RunCin("gen1_pickup", true, false);
        StopTimer(mTimer20);
        mFlags[48] = true;
        }
        }
    }

    // ---- +0x4294  132 bytes ----
    if (mFlags[119] && !mFlags[46]) {
        if (MissionUtility::IsInsideRegion(mHandles[171], "land_land")) {
        MissionUtility::MoveObjectWithRotation(mHandles[171], "landing_land_startpoint", 0, true);
        MissionUtility::Land(mHandles[171], "lpad", 0, 80.0f);
        MissionUtility::SetTeamNum(mHandles[171], 0);
        mHandles[294] = MissionUtility::RunCin("landing_pickup", true, false);
        StopTimer(mTimer20);
        mFlags[46] = true;
        }
    }

    // ---- +0x4318  784 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "generator1_land")) {
    if (!mFlags[125]) {
    if (!MissionUtility::IsLoaded(mHandles[303], mHandles[171])) {
    if (!mFlags[88]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[303]);
    MissionUtility::SetCurHealth(mHandles[303], 20.0f);
    MissionUtility::Goto(mHandles[303], "g1_troops_goto", true);
    MissionUtility::Objectify(mHandles[303], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[88] = true;
    }
    }
    if (!MissionUtility::IsLoaded(mHandles[304], mHandles[171])) {
    if (!mFlags[89]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[304]);
    MissionUtility::SetCurHealth(mHandles[304], 20.0f);
    MissionUtility::Goto(mHandles[304], "g1_troops_goto", true);
    MissionUtility::Objectify(mHandles[304], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[89] = true;
    }
    }
    if (!MissionUtility::IsLoaded(mHandles[305], mHandles[171])) {
    if (!mFlags[90]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[305]);
    MissionUtility::SetCurHealth(mHandles[305], 20.0f);
    MissionUtility::Goto(mHandles[305], "g1_troops_goto", true);
    MissionUtility::Objectify(mHandles[305], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[90] = true;
    }
    }
    if (!MissionUtility::IsLoaded(mHandles[306], mHandles[171])) {
    if (!mFlags[91]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[306]);
    MissionUtility::SetCurHealth(mHandles[306], 20.0f);
    MissionUtility::Goto(mHandles[306], "g1_troops_goto", true);
    MissionUtility::Objectify(mHandles[306], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[91] = true;
    }
    }
    if (!MissionUtility::IsLoaded(mHandles[307], mHandles[171])) {
    if (!mFlags[92]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[307]);
    MissionUtility::SetCurHealth(mHandles[307], 20.0f);
    MissionUtility::Goto(mHandles[307], "g1_troops_goto", true);
    MissionUtility::Objectify(mHandles[307], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[92] = true;
    }
    }
    if (mFlags[88]) {
    if (mFlags[89]) {
    if (mFlags[90]) {
    if (!mFlags[126]) {
    mFlags[126] = true;
    }
    }
    }
    }
    if (mFlags[88]) {
    if (mFlags[89]) {
    if (mFlags[90]) {
    if (mFlags[91]) {
    if (mFlags[92]) {
    MissionUtility::TakeOff(mHandles[171]);
    MissionUtility::SetTeamNum(mHandles[171], 1);
    MissionUtility::StopCin(mHandles[295]);
    MissionUtility::StartSound("ERT21_05C", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::RemoveObjective(mInts[4]);
    mInts[5] = MissionUtility::AddObjective("missions.Thule1.objective.str0003");
    MissionUtility::DisplayText("missions.Thule1.text.str0004", 5.0f, -1.0f);
    MissionUtility::RemoveObjectify(mHandles[141]);
    mFlags[125] = true;
    }
    }
    }
    }
    }
    }
    }

    // ---- +0x4628  68 bytes ----
    if (mFlags[125]) {
        if (MissionUtility::GetDistance(mHandles[29], "g1_transport", 0) < 20.0f) {
        if (mFlags[136]) {
        MissionUtility::RemoveObject(mHandles[29]);
        mFlags[136] = true;
        }
        }
    }

    // ---- +0x466c  116 bytes ----
    if (!mFlags[93] && !mFlags[123] && mFlags[125]) {
        if (MissionUtility::GetFlockCount(mHandles[173]) < 5) {
        if (!mFlags[57]) {
        MissionUtility::BonusObjectiveFailed(mInts[17]);
        MissionUtility::StartSound("CTINT_31", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[57] = true;
        }
        }
    }

    // ---- +0x46e0  108 bytes ----
    if (!mFlags[93] && !mFlags[123] && mFlags[125]) {
        if (MissionUtility::GetFlockCount(mHandles[173]) < 3) {
        if (!mFlags[58]) {
        MissionUtility::StartSound("CTINT_32", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[58] = true;
        }
        }
    }

    // ---- +0x474c  116 bytes ----
    if (mFlags[93] && !mFlags[123] && mFlags[125]) {
        if (MissionUtility::GetFlockCount(mHandles[173]) < 5) {
        if (!mFlags[59]) {
        MissionUtility::BonusObjectiveFailed(mInts[17]);
        MissionUtility::StartSound("CTINT_31", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[59] = true;
        }
        }
    }

    // ---- +0x47c0  108 bytes ----
    if (mFlags[93] && !mFlags[123] && mFlags[125]) {
        if (MissionUtility::GetFlockCount(mHandles[173]) < 3) {
        if (!mFlags[60]) {
        MissionUtility::StartSound("CTINT_32", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[60] = true;
        }
        }
    }

    // ---- +0x482c  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[303], "g1_troops_goto", 1) < 20.0f) {
    if (!mFlags[83]) {
    mFlags[83] = true;
    }
    }

    // ---- +0x485c  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[304], "g1_troops_goto", 1) < 20.0f) {
    if (!mFlags[84]) {
    mFlags[84] = true;
    }
    }

    // ---- +0x488c  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[305], "g1_troops_goto", 1) < 20.0f) {
    if (!mFlags[85]) {
    mFlags[85] = true;
    }
    }

    // ---- +0x48bc  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[306], "g1_troops_goto", 1) < 20.0f) {
    if (!mFlags[86]) {
    mFlags[86] = true;
    }
    }

    // ---- +0x48ec  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[307], "g1_troops_goto", 1) < 20.0f) {
    if (!mFlags[87]) {
    mFlags[87] = true;
    }
    }

    // ---- +0x491c  96 bytes ----
    if (!mFlags[123]) {
        if (mFlags[83]
            || mFlags[84]
            || mFlags[85]
            || mFlags[86]
            || mFlags[87]) {
        mFlags[123] = true;
        BeginTimer(mTimer6);
        MissionUtility::RemoveObjective(mInts[5]);
        }
    }

    // ---- +0x497c  236 bytes ----
    if (mFlags[126]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[371])) {
        if (!mFlags[124]) {
        mHandles[371] = MissionUtility::CreateObject("cis_tank_fighter", "g1_unit_spawner", 0, "g1_gat1", 2, -1);
        MissionUtility::Goto(mHandles[371], "g1_spawner2_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[371], "g1_gats_goto1", 0);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[371], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x4a68  236 bytes ----
    if (mFlags[126]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[372])) {
        if (!mFlags[124]) {
        mHandles[372] = MissionUtility::CreateObject("cis_tank_fighter", "g1_unit_spawner", 1, "g1_gat2", 2, -1);
        MissionUtility::Goto(mHandles[372], "g1_spawner2_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[372], "g1_gats_goto1", 1);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[372], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x4b54  236 bytes ----
    if (mFlags[126]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[373])) {
        if (!mFlags[124]) {
        mHandles[373] = MissionUtility::CreateObject("cis_tank_fighter", "g1_unit_spawner", 2, "g1_gat3", 2, -1);
        MissionUtility::Goto(mHandles[373], "g1_spawner2_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[373], "g1_gats_goto1", 2);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[373], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x4c40  236 bytes ----
    if (mFlags[126]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[374])) {
        if (!mFlags[124]) {
        mHandles[374] = MissionUtility::CreateObject("cis_tank_fighter", "g1_unit_spawner2", 0, "g1_gat4", 2, -1);
        MissionUtility::Goto(mHandles[374], "g1_spawner1_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[374], "g1_gats_goto2", 0);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[374], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x4d2c  236 bytes ----
    if (mFlags[126]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[375])) {
        if (!mFlags[124]) {
        mHandles[375] = MissionUtility::CreateObject("cis_tank_fighter", "g1_unit_spawner2", 1, "g1_gat5", 2, -1);
        MissionUtility::Goto(mHandles[375], "g1_spawner1_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[375], "g1_gats_goto2", 1);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[375], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x4e18  236 bytes ----
    if (mFlags[126]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[141]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[376])) {
        if (!mFlags[124]) {
        mHandles[376] = MissionUtility::CreateObject("cis_tank_fighter", "g1_unit_spawner2", 2, "g1_gat6", 2, -1);
        MissionUtility::Goto(mHandles[376], "g1_spawner1_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[376], "g1_gats_goto2", 2);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[376], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x4f04  376 bytes ----
    if (mTimer6 > 20.0f) {
    if (!mFlags[124]) {
    MissionUtility::StartSound("ERT21_07", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mInts[2] = MissionUtility::AddObjective("missions.Thule1.objective.str0004");
    MissionUtility::DisplayText("missions.Thule1.text.str0005", 5.0f, -1.0f);
    MissionUtility::RemoveObjectify(mHandles[141]);
    MissionUtility::Objectify(mHandles[387], "missions.Thule1.marker.str0004", false, true, 0.0f);
    MissionUtility::SetMaxHealth(mHandles[387], 10000.0f);
    MissionUtility::SetCurHealth(mHandles[387], 10000.0f);
    MissionUtility::AddHealthBar(mHandles[387], 0, 400.0f);
    MissionUtility::SetTeamNum(mHandles[387], 2);
    MissionUtility::SetVelocForward(mHandles[303], 15.0f);
    MissionUtility::SetVelocForward(mHandles[304], 13.0f);
    MissionUtility::SetVelocForward(mHandles[305], 16.0f);
    MissionUtility::SetVelocForward(mHandles[306], 15.0f);
    MissionUtility::SetVelocForward(mHandles[307], 18.0f);
    MissionUtility::Goto(mHandles[303], "g1_troops_goto", 0);
    MissionUtility::Goto(mHandles[304], "g1_troops_goto", 0);
    MissionUtility::Goto(mHandles[305], "g1_troops_goto", 0);
    MissionUtility::Goto(mHandles[306], "g1_troops_goto", 0);
    MissionUtility::Goto(mHandles[307], "g1_troops_goto", 0);
    MissionUtility::RemoveObjectify(mHandles[303]);
    MissionUtility::RemoveObjectify(mHandles[304]);
    MissionUtility::RemoveObjectify(mHandles[305]);
    MissionUtility::RemoveObjectify(mHandles[306]);
    MissionUtility::RemoveObjectify(mHandles[307]);
    MissionUtility::BeginWave("gen2");
    mFlags[124] = true;
    }
    }

    // ---- +0x507c  1408 bytes ----
    if (!MissionUtility::IsAlive(mHandles[387]) & !mFlags[76]) {
    MissionUtility::RemoveObject(mHandles[303]);
    MissionUtility::RemoveObject(mHandles[304]);
    MissionUtility::RemoveObject(mHandles[305]);
    MissionUtility::RemoveObject(mHandles[306]);
    MissionUtility::RemoveObject(mHandles[307]);
    mHandles[147] = MissionUtility::CreateObject("efarm_explo", "g1_smoke_spawn", 0, "", 0, -1);
    mHandles[388] = MissionUtility::CreateObject("thu_bldg_efarm", Vector(1818.914f, -306.1004f, 1017.35126f), "", 1, -1, Quat(1.0f, 0.0f, 0.0f, 0.0f), -1);
    mHandles[144] = MissionUtility::CreateObject("smoke_bigblack", "g1_smoke_spawn", 0, "", 0, -1);
    mHandles[145] = MissionUtility::CreateObject("smoke_bigblack", "g1_smoke_spawn", 1, "", 0, -1);
    mHandles[146] = MissionUtility::CreateObject("smoke_bigblack", "g1_smoke_spawn", 2, "", 0, -1);
    MissionUtility::RemoveObject(mHandles[318]);
    MissionUtility::RemoveObject(mHandles[319]);
    MissionUtility::RemoveObject(mHandles[320]);
    MissionUtility::ObjectiveComplete(mInts[2]);
    mFlags[76] = true;
    MissionUtility::RemoveFlock(mHandles[481], false);
    MissionUtility::RemoveFlock(mHandles[494], false);
    mHandles[482] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 0, "", 10, -1);
    MissionUtility::SetWeaponOrd(mHandles[482], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    mHandles[483] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 1, "", 10, -1);
    MissionUtility::SetWeaponOrd(mHandles[483], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    mHandles[484] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 3, "", 10, -1);
    MissionUtility::SetWeaponOrd(mHandles[484], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    mHandles[481] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[481], mHandles[482]);
    MissionUtility::AddFlockMember(mHandles[481], mHandles[483]);
    MissionUtility::AddFlockMember(mHandles[481], mHandles[484]);
    MissionUtility::AddFlockMember(mHandles[481], mHandles[485]);
    MissionUtility::SetCurHealth(mHandles[482], 9999.0f);
    MissionUtility::SetCurHealth(mHandles[483], 9999.0f);
    MissionUtility::SetCurHealth(mHandles[484], 9999.0f);
    MissionUtility::SetCurHealth(mHandles[485], 9999.0f);
    mHandles[495] = MissionUtility::CreateObject("cis_tank_fighter", "battle1_spawn2", 0, "", 11, -1);
    MissionUtility::SetWeaponOrd(mHandles[495], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    mHandles[496] = MissionUtility::CreateObject("cis_tank_wheeled", "battle1_spawn2", 1, "", 11, -1);
    MissionUtility::SetWeaponOrd(mHandles[496], "cis_missile_wheeled", "cis_missile_wheeled_weak_ord");
    mHandles[497] = MissionUtility::CreateObject("cis_walk_assault", "battle1_spawn2", 2, "", 11, -1);
    MissionUtility::SetWeaponOrd(mHandles[497], "cis_beam_walk", "cis_beam_walk_weak_ord");
    mHandles[498] = MissionUtility::CreateObject("cis_tank_fighter", "battle1_spawn2", 3, "", 11, -1);
    MissionUtility::SetWeaponOrd(mHandles[498], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    mHandles[494] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[494], mHandles[495]);
    MissionUtility::AddFlockMember(mHandles[494], mHandles[496]);
    MissionUtility::AddFlockMember(mHandles[494], mHandles[497]);
    MissionUtility::AddFlockMember(mHandles[494], mHandles[498]);
    }

    // ---- +0x55fc  532 bytes ----
    if (mFlags[76]) {
        if (!MissionUtility::IsCinRunning(mHandles[284])) {
        mFlags[46] = false;
        mHandles[450] = MissionUtility::GetHandle("g3_aat1");
        mHandles[451] = MissionUtility::GetHandle("g3_aat2");
        mHandles[452] = MissionUtility::GetHandle("g3_aat3");
        mHandles[453] = MissionUtility::GetHandle("g3_aat4");
        mHandles[454] = MissionUtility::GetHandle("g3_aat5");
        mHandles[455] = MissionUtility::GetHandle("g3_aat6");
        mHandles[456] = MissionUtility::GetHandle("g3_aat7");
        mHandles[457] = MissionUtility::GetHandle("g3_aat8");
        mHandles[458] = MissionUtility::GetHandle("g3_t1");
        mHandles[459] = MissionUtility::GetHandle("g3_t2");
        mHandles[460] = MissionUtility::GetHandle("g3_t3");
        mHandles[461] = MissionUtility::GetHandle("g3_t4");
        mHandles[462] = MissionUtility::GetHandle("g3_t5");
        mHandles[463] = MissionUtility::GetHandle("g3_t6");
        mHandles[464] = MissionUtility::GetHandle("g3_t7");
        mHandles[465] = MissionUtility::GetHandle("g3_t8");
        mHandles[466] = MissionUtility::GetHandle("g3_t9");
        mHandles[467] = MissionUtility::GetHandle("g3_t10");
        mHandles[143] = MissionUtility::GetHandle("g3_land");
        mHandles[447] = MissionUtility::GetHandle("generator3");
        MissionUtility::SetMaxHealth(mHandles[447], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[447], 999999.0f);
        mHandles[449] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[449], mHandles[450]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[451]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[452]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[453]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[454]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[455]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[456]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[457]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[458]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[459]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[460]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[461]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[462]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[463]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[464]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[465]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[466]);
        MissionUtility::AddFlockMember(mHandles[449], mHandles[467]);
        mPhase = 5;
        }
    }

    // ---- +0x5810  1660 bytes ----
    if (!mFlags[123] && mFlags[125]) {
        if (!MissionUtility::IsAlive(mHandles[303])) {
        if (!MissionUtility::IsAlive(mHandles[304])) {
        if (!MissionUtility::IsAlive(mHandles[305])) {
        if (!MissionUtility::IsAlive(mHandles[306])) {
        if (!MissionUtility::IsAlive(mHandles[307])) {
        if (!mFlags[93]) {
        mFlags[88] = false;
        mFlags[89] = false;
        mFlags[90] = false;
        mFlags[91] = false;
        mFlags[92] = false;
        mFlags[46] = false;
        MissionUtility::RemoveObjective(mInts[5]);
        mInts[3] = MissionUtility::AddObjective("missions.Thule1.objective.str0002");
        MissionUtility::DisplayText("missions.Thule1.text.str0003", 5.0f, -1.0f);
        MissionUtility::RemoveObjectify(mHandles[141]);
        MissionUtility::Objectify("lpad", 0, "missions.Thule1.marker.str0001", true, true, 0.0f, 2.0f);
        mFlags[125] = false;
        mFlags[126] = false;
        mFlags[93] = true;
        MissionUtility::AddFlyerAmmoBox("health_ammo_spawn", 0, -1.0f);
        mHandles[303] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 0, "troop1", 1, -1);
        mHandles[304] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 1, "troop2", 1, -1);
        mHandles[305] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 2, "troop3", 1, -1);
        mHandles[306] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 3, "troop4", 1, -1);
        mHandles[307] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 4, "troop5", 1, -1);
        MissionUtility::SetCurHealth(mHandles[303], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[304], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[305], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[306], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[307], 999999.0f);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[303]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[304]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[305]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[306]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[307]);
        MissionUtility::Cargo(mHandles[303]);
        MissionUtility::Cargo(mHandles[304]);
        MissionUtility::Cargo(mHandles[305]);
        MissionUtility::Cargo(mHandles[306]);
        MissionUtility::Cargo(mHandles[307]);
        MissionUtility::StartSound("MWT21_05F", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveFlock(mHandles[481], false);
        MissionUtility::RemoveFlock(mHandles[494], false);
        mHandles[482] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 0, "", 10, -1);
        MissionUtility::SetWeaponOrd(mHandles[482], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[483] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 1, "", 10, -1);
        MissionUtility::SetWeaponOrd(mHandles[483], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[484] = MissionUtility::CreateObject("rep_tank_fighter1", "battle1_spawn1", 3, "", 10, -1);
        MissionUtility::SetWeaponOrd(mHandles[484], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[481] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[481], mHandles[482]);
        MissionUtility::AddFlockMember(mHandles[481], mHandles[483]);
        MissionUtility::AddFlockMember(mHandles[481], mHandles[484]);
        MissionUtility::AddFlockMember(mHandles[481], mHandles[485]);
        mHandles[495] = MissionUtility::CreateObject("cis_tank_fighter", "battle1_spawn2", 0, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[495], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        mHandles[496] = MissionUtility::CreateObject("cis_tank_wheeled", "battle1_spawn2", 1, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[496], "cis_missile_wheeled", "cis_missile_wheeled_weak_ord");
        mHandles[497] = MissionUtility::CreateObject("cis_walk_assault", "battle1_spawn2", 2, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[497], "cis_beam_walk", "cis_beam_walk_weak_ord");
        mHandles[498] = MissionUtility::CreateObject("cis_tank_fighter", "battle1_spawn2", 3, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[498], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        mHandles[494] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[494], mHandles[495]);
        MissionUtility::AddFlockMember(mHandles[494], mHandles[496]);
        MissionUtility::AddFlockMember(mHandles[494], mHandles[497]);
        MissionUtility::AddFlockMember(mHandles[494], mHandles[498]);
        }
        }
        }
        }
        }
        }
    }

    // ---- +0x5e8c  272 bytes ----
    if (mFlags[93] && !mFlags[121]) {
        if (MissionUtility::IsLoaded(mHandles[303], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[304], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[305], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[306], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[307], mHandles[171])) {
        MissionUtility::TakeOff(mHandles[171]);
        MissionUtility::SetTeamNum(mHandles[171], 1);
        MissionUtility::StopCin(mHandles[295]);
        MissionUtility::RemoveObjectify("lpad", 0);
        MissionUtility::RemoveObjective(mInts[3]);
        MissionUtility::Objectify(mHandles[141], "missions.Thule1.marker.str0002", false, true, 0.0f);
        mInts[4] = MissionUtility::AddObjective("missions.Thule1.objective.str0001");
        MissionUtility::DisplayText("missions.Thule1.text.str0002", 5.0f, -1.0f);
        MissionUtility::StartSound("CTR06_05", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[4] = true;
        mFlags[121] = true;
        }
        }
        }
        }
        }
    }

    // ---- +0x5f9c  140 bytes ----
    if (mFlags[93] && mFlags[125] && !mFlags[123]) {
        if (!MissionUtility::IsAlive(mHandles[303])) {
        if (!MissionUtility::IsAlive(mHandles[304])) {
        if (!MissionUtility::IsAlive(mHandles[305])) {
        if (!MissionUtility::IsAlive(mHandles[306])) {
        if (!MissionUtility::IsAlive(mHandles[307])) {
        if (!mFlags[1]) {
        MissionUtility::MissionFailure();
        mFlags[1] = true;
        }
        }
        }
        }
        }
        }
    }

    // ---- +0x6028  140 bytes ----
    if (!mFlags[120]) {
        if (!MissionUtility::IsAlive(mHandles[495])) {
        mHandles[495] = MissionUtility::CreateObject("cis_tank_fighter", "battle1_respawn_fighter", 0, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[495], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        MissionUtility::Goto(mHandles[495], "battle1_respawn_fighter_goto", true);
        }
    }

    // ---- +0x60b4  140 bytes ----
    if (!mFlags[120]) {
        if (!MissionUtility::IsAlive(mHandles[496])) {
        mHandles[496] = MissionUtility::CreateObject("cis_tank_wheeled", "battle1_respawn_wheel", 0, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[496], "cis_missile_wheeled", "cis_missile_wheeled_weak_ord");
        MissionUtility::Goto(mHandles[496], "battle1_respawn_wheel_goto", true);
        }
    }

    // ---- +0x6140  140 bytes ----
    if (!mFlags[120]) {
        if (!MissionUtility::IsAlive(mHandles[497])) {
        mHandles[497] = MissionUtility::CreateObject("cis_walk_assault", "battle1_respawn_walk", 0, "", 11, -1);
        MissionUtility::SetWeaponOrd(mHandles[497], "cis_beam_walk", "cis_beam_walk_weak_ord");
        MissionUtility::Goto(mHandles[497], "battle1_respawn_walk_goto", true);
        }
    }

    // ---- +0x61cc  144 bytes ----
    if (!mFlags[120]) {
    if (!MissionUtility::IsAlive(mHandles[498])) {
    mHandles[498] = MissionUtility::CreateObject("cis_tank_fighter", "battle1_respawn_fighter", 0, "", 11, -1);
    MissionUtility::SetWeaponOrd(mHandles[498], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    MissionUtility::Goto(mHandles[498], "battle1_respawn_fighter_goto", true);

        break;
    case 5:
    // ---- +0x625c  828 bytes ----
    if (mFlags[76]) {
        if (!MissionUtility::IsCinRunning(mHandles[284])) {
        if (mFlags[155]) {
        mFlags[83] = false;
        mFlags[84] = false;
        mFlags[85] = false;
        mFlags[86] = false;
        mFlags[87] = false;
        mFlags[88] = false;
        mFlags[89] = false;
        mFlags[90] = false;
        mFlags[91] = false;
        mFlags[92] = false;
        MissionUtility::RemoveCargo(mHandles[171]);
        MissionUtility::RemoveObjectify(mHandles[387]);
        mInts[7] = MissionUtility::AddObjective("missions.Thule1.objective.str0002");
        MissionUtility::DisplayText("missions.Thule1.text.str0003", 5.0f, -1.0f);
        MissionUtility::SetEnemies(1, 5);
        MissionUtility::StartSound("ERT21_09", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::PlayMusic("EP1_V1_T12", true);
        MissionUtility::Objectify("lpad", 0, "missions.Thule1.marker.str0001", true, true, 0.0f, 2.0f);
        MissionUtility::AddFlyerAmmoBox("health_ammo_spawn", 0, -1.0f);
        MissionUtility::AddFlyerHealthBox("health_ammo_spawn", 1, -1.0f);
        mHandles[303] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 0, "troop1", 1, -1);
        mHandles[304] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 1, "troop2", 1, -1);
        mHandles[305] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 2, "troop3", 1, -1);
        mHandles[306] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 3, "troop4", 1, -1);
        mHandles[307] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 4, "troop5", 1, -1);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[303]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[304]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[305]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[306]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[307]);
        MissionUtility::SetCurHealth(mHandles[303], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[304], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[305], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[306], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[307], 999999.0f);
        MissionUtility::Cargo(mHandles[303]);
        MissionUtility::Cargo(mHandles[304]);
        MissionUtility::Cargo(mHandles[305]);
        MissionUtility::Cargo(mHandles[306]);
        MissionUtility::Cargo(mHandles[307]);
        MissionUtility::CreateDropOffRegion("generator3_land");
        mFlags[155] = false;
        }
        }
    }

    // ---- +0x6598  160 bytes ----
    if (!MissionUtility::IsAlive(mHandles[34])) {
    if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
    if (!mFlags[168]) {
    mHandles[34] = MissionUtility::CreateObject("cis_fly_fighter_combat", "cis_flier_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[34], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0x6638  160 bytes ----
    if (!MissionUtility::IsAlive(mHandles[35])) {
    if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
    if (!mFlags[168]) {
    mHandles[35] = MissionUtility::CreateObject("cis_fly_fighter_combat", "cis_flier_spawn", 1, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[34], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0x66d8  148 bytes ----
    if (!MissionUtility::IsFlockAlive(mHandles[449])) {
    if (!mFlags[156]) {
    if (MissionUtility::IsInsideRegion(mHandles[171], "gen2_land1")) {
    if (!mFlags[49]) {
    MissionUtility::MoveObjectWithRotation(mHandles[171], "gen3_land_startpoint", 0, true);
    MissionUtility::Land(mHandles[171], "gen2_land", 0, 80.0f);
    MissionUtility::SetTeamNum(mHandles[171], 0);
    mHandles[296] = MissionUtility::RunCin("gen2_pickup", true, false);
    StopTimer(mTimer20);
    mFlags[49] = true;
    }
    }
    }
    }

    // ---- +0x676c  144 bytes ----
    if (mFlags[5] && mFlags[93]) {
        if (MissionUtility::IsInsideRegion(mHandles[171], "gen2_land2")) {
        if (!mFlags[50]) {
        MissionUtility::MoveObjectWithRotation(mHandles[171], "gen2_land_startpoint", 0, true);
        MissionUtility::Land(mHandles[171], "gen2_land", 0, 80.0f);
        MissionUtility::SetTeamNum(mHandles[171], 0);
        mHandles[296] = MissionUtility::RunCin("gen2_pickup", true, false);
        StopTimer(mTimer20);
        mFlags[50] = true;
        }
        }
    }

    // ---- +0x67fc  132 bytes ----
    if (mFlags[119] && !mFlags[46]) {
        if (MissionUtility::IsInsideRegion(mHandles[171], "land_land")) {
        MissionUtility::MoveObjectWithRotation(mHandles[171], "landing_land_startpoint", 0, true);
        MissionUtility::Land(mHandles[171], "lpad", 0, 80.0f);
        MissionUtility::SetTeamNum(mHandles[171], 0);
        mHandles[294] = MissionUtility::RunCin("landing_pickup", true, false);
        StopTimer(mTimer20);
        mFlags[46] = true;
        }
    }

    // ---- +0x6880  112 bytes ----
    if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
    if (!mFlags[95]) {
    MissionUtility::RemoveFlock(mHandles[507], false);
    MissionUtility::RemoveFlock(mHandles[520], false);
    MissionUtility::RemoveObjectify(mHandles[143]);
    MissionUtility::StartSound("CTT21_02", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[95] = true;
    }
    }

    // ---- +0x68f0  60 bytes ----
    if (!MissionUtility::IsFlockAlive(mHandles[449])) {
    if (!mFlags[31]) {
    MissionUtility::Objectify(mHandles[143], "missions.Thule1.marker.str0009", true, true, 0.0f);
    mFlags[31] = true;
    }
    }

    // ---- +0x692c  480 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "gen2_proparmy_set3_on")) {
    if (!mFlags[25]) {
    mHandles[61] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight5", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[61], "set3_prop_armyfight5", false);
    mHandles[78] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight5", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[78], "set3_prop_armyfight5", false);
    mHandles[62] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight6", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[62], "set3_prop_armyfight6", false);
    mHandles[79] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight6", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[79], "set3_prop_armyfight6", false);
    mHandles[63] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight7", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[63], "set3_prop_armyfight7", false);
    mHandles[80] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight7", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[80], "set3_prop_armyfight7", false);
    mHandles[64] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight8", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[64], "set3_prop_armyfight8", false);
    mHandles[81] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight8", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[81], "set3_prop_armyfight8", false);
    mHandles[67] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight11", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[67], "set3_prop_armyfight11", false);
    mHandles[84] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight11", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[84], "set3_prop_armyfight11", false);
    mFlags[25] = true;
    }
    }

    // ---- +0x6b0c  120 bytes ----
    if (!MissionUtility::IsInsideRegion(mHandles[171], "gen2_proparmy_set3_on")) {
    if (mFlags[25]) {
    MissionUtility::RemoveArmy(mHandles[61]);
    MissionUtility::RemoveArmy(mHandles[62]);
    MissionUtility::RemoveArmy(mHandles[63]);
    MissionUtility::RemoveArmy(mHandles[64]);
    MissionUtility::RemoveArmy(mHandles[67]);
    MissionUtility::RemoveArmy(mHandles[78]);
    MissionUtility::RemoveArmy(mHandles[79]);
    MissionUtility::RemoveArmy(mHandles[80]);
    MissionUtility::RemoveArmy(mHandles[81]);
    MissionUtility::RemoveArmy(mHandles[84]);
    mFlags[25] = false;
    }
    }

    // ---- +0x6b84  392 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "gen2_proparmy_set4_on")) {
    if (!mFlags[26]) {
    mHandles[92] = MissionUtility::AddPropArmy(3, "set4_armyfight1", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[92], "set4_armyfight1", false);
    mHandles[96] = MissionUtility::AddPropArmy(2, "set4_armyfight1", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[96], "set4_armyfight1", false);
    mHandles[93] = MissionUtility::AddPropArmy(3, "set4_armyfight2", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[93], "set4_armyfight2", false);
    mHandles[97] = MissionUtility::AddPropArmy(2, "set4_armyfight2", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[97], "set4_armyfight2", false);
    mHandles[94] = MissionUtility::AddPropArmy(3, "set4_armyfight3", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[94], "set4_armyfight3", false);
    mHandles[98] = MissionUtility::AddPropArmy(2, "set4_armyfight3", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[98], "set4_armyfight3", false);
    mHandles[95] = MissionUtility::AddPropArmy(3, "set4_armyfight4", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[95], "set4_armyfight4", false);
    mHandles[99] = MissionUtility::AddPropArmy(2, "set4_armyfight4", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[99], "set4_armyfight4", false);
    mFlags[26] = true;
    }
    }

    // ---- +0x6d0c  104 bytes ----
    if (!MissionUtility::IsInsideRegion(mHandles[171], "gen2_proparmy_set4_on")) {
    if (mFlags[26]) {
    MissionUtility::RemoveArmy(mHandles[92]);
    MissionUtility::RemoveArmy(mHandles[93]);
    MissionUtility::RemoveArmy(mHandles[94]);
    MissionUtility::RemoveArmy(mHandles[95]);
    MissionUtility::RemoveArmy(mHandles[96]);
    MissionUtility::RemoveArmy(mHandles[97]);
    MissionUtility::RemoveArmy(mHandles[98]);
    MissionUtility::RemoveArmy(mHandles[99]);
    mFlags[26] = false;
    }
    }

    // ---- +0x6d74  3424 bytes ----
    if (MissionUtility::IsLoaded(mHandles[303], mHandles[171])) {
    if (MissionUtility::IsLoaded(mHandles[304], mHandles[171])) {
    if (MissionUtility::IsLoaded(mHandles[305], mHandles[171])) {
    if (MissionUtility::IsLoaded(mHandles[306], mHandles[171])) {
    if (MissionUtility::IsLoaded(mHandles[307], mHandles[171])) {
    if (!mFlags[169]) {
    MissionUtility::RemoveObject(mHandles[241]);
    MissionUtility::RemoveObject(mHandles[243]);
    MissionUtility::RemoveObject(mHandles[242]);
    MissionUtility::RemoveObject(mHandles[479]);
    MissionUtility::RemoveObject(mHandles[480]);
    MissionUtility::EvictConfig("cis_walk_assault");
    MissionUtility::RemoveTurnAroundRegion("gen1_turnaround");
    MissionUtility::AddTurnAroundRegion("gen2_turnaround", "gen3_turnaround_focus", 0, 0, 0, 0);
    MissionUtility::TakeOff(mHandles[171]);
    MissionUtility::SetTeamNum(mHandles[171], 1);
    MissionUtility::StopCin(mHandles[294]);
    MissionUtility::RemoveObjectify("lpad", 0);
    MissionUtility::Objectify(mHandles[143], "missions.Thule1.marker.str0002", true, true, 0.0f);
    MissionUtility::RemoveObjective(mInts[7]);
    mInts[8] = MissionUtility::AddObjective("missions.Thule1.objective.str0001");
    MissionUtility::DisplayText("missions.Thule1.text.str0002", 5.0f, -1.0f);
    MissionUtility::StartSound("CTR06_05", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[169] = true;
    MissionUtility::RemoveFlock(mHandles[481], false);
    MissionUtility::RemoveFlock(mHandles[494], false);
    MissionUtility::RemoveArmy(mHandles[56]);
    MissionUtility::RemoveArmy(mHandles[57]);
    MissionUtility::RemoveArmy(mHandles[58]);
    MissionUtility::RemoveArmy(mHandles[59]);
    MissionUtility::RemoveArmy(mHandles[60]);
    MissionUtility::RemoveArmy(mHandles[73]);
    MissionUtility::RemoveArmy(mHandles[74]);
    MissionUtility::RemoveArmy(mHandles[75]);
    MissionUtility::RemoveArmy(mHandles[76]);
    MissionUtility::RemoveArmy(mHandles[77]);
    MissionUtility::RemoveArmy(mHandles[68]);
    MissionUtility::RemoveArmy(mHandles[69]);
    MissionUtility::RemoveArmy(mHandles[70]);
    MissionUtility::RemoveArmy(mHandles[71]);
    MissionUtility::RemoveArmy(mHandles[85]);
    MissionUtility::RemoveArmy(mHandles[86]);
    MissionUtility::RemoveArmy(mHandles[87]);
    MissionUtility::RemoveArmy(mHandles[88]);
    mHandles[100] = MissionUtility::AddPropArmy(3, "gen3_armyfight1", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[100], "gen3_armyfight1", false);
    mHandles[102] = MissionUtility::AddPropArmy(2, "gen3_armyfight1", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[102], "gen3_armyfight1", false);
    mHandles[101] = MissionUtility::AddPropArmy(3, "gen3_armyfight2", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[101], "gen3_armyfight2", false);
    mHandles[103] = MissionUtility::AddPropArmy(2, "gen3_armyfight2", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[103], "gen3_armyfight2", false);
    mHandles[65] = MissionUtility::AddPropArmy(3, "prop_armyfight9", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[65], "prop_armyfight9", false);
    mHandles[82] = MissionUtility::AddPropArmy(2, "prop_armyfight9", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[82], "prop_armyfight9", false);
    mHandles[66] = MissionUtility::AddPropArmy(3, "prop_armyfight10", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[66], "prop_armyfight10", false);
    mHandles[83] = MissionUtility::AddPropArmy(2, "prop_armyfight10", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[83], "prop_armyfight10", false);
    mHandles[508] = MissionUtility::CreateObject("rep_tank_fighter1", "battle2_spawn1", 0, "", 12, -1);
    MissionUtility::SetWeaponOrd(mHandles[508], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    MissionUtility::Defend(mHandles[508], "battle2_defend", 0, 600.0f);
    mHandles[511] = MissionUtility::CreateObject("rep_tank_fighter1", "battle2_spawn1", 4, "", 12, -1);
    MissionUtility::SetWeaponOrd(mHandles[511], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    MissionUtility::Defend(mHandles[511], "battle2_defend", 0, 600.0f);
    mHandles[512] = MissionUtility::CreateObject("rep_tank_fighter1", "battle2_spawn1", 7, "", 12, -1);
    MissionUtility::SetWeaponOrd(mHandles[512], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    MissionUtility::Defend(mHandles[512], "battle2_defend", 0, 600.0f);
    mHandles[507] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[507], mHandles[508]);
    MissionUtility::AddFlockMember(mHandles[507], mHandles[509]);
    MissionUtility::AddFlockMember(mHandles[507], mHandles[510]);
    MissionUtility::AddFlockMember(mHandles[507], mHandles[511]);
    MissionUtility::AddFlockMember(mHandles[507], mHandles[512]);
    mHandles[521] = MissionUtility::CreateObject("cis_tank_fighter", "battle2_spawn2", 0, "", 13, -1);
    MissionUtility::SetWeaponOrd(mHandles[521], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    MissionUtility::Defend(mHandles[521], "battle2_defend", 0, 600.0f);
    mHandles[522] = MissionUtility::CreateObject("cis_tank_wheeled", "battle2_spawn2", 1, "", 13, -1);
    MissionUtility::SetWeaponOrd(mHandles[522], "cis_missile_wheeled", "cis_missile_wheeled_weak_ord");
    MissionUtility::Defend(mHandles[522], "battle2_defend", 0, 600.0f);
    mHandles[523] = MissionUtility::CreateObject("cis_tank_assault", "battle2_spawn2", 2, "", 13, -1);
    MissionUtility::SetWeaponOrd(mHandles[523], "cis_blaster_assault", "cis_blaster_assault_weak_ord");
    MissionUtility::SetWeaponOrd(mHandles[523], "cis_cannon_assault", "cis_cannon_assault_weak_ord");
    MissionUtility::Defend(mHandles[523], "battle2_defend", 0, 600.0f);
    mHandles[524] = MissionUtility::CreateObject("cis_tank_fighter", "battle2_spawn2", 3, "", 13, -1);
    MissionUtility::SetWeaponOrd(mHandles[524], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    MissionUtility::Defend(mHandles[524], "battle2_defend", 0, 600.0f);
    mHandles[525] = MissionUtility::CreateObject("cis_tank_fighter", "battle2_spawn2", 4, "", 13, -1);
    MissionUtility::SetWeaponOrd(mHandles[525], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    MissionUtility::Defend(mHandles[525], "battle2_defend", 0, 600.0f);
    mHandles[520] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[520], mHandles[521]);
    MissionUtility::AddFlockMember(mHandles[520], mHandles[522]);
    MissionUtility::AddFlockMember(mHandles[520], mHandles[523]);
    MissionUtility::AddFlockMember(mHandles[520], mHandles[524]);
    MissionUtility::AddFlockMember(mHandles[520], mHandles[525]);
    mHandles[534] = MissionUtility::CreateObject("rep_tank_fighter1", "battle3_spawn1", 0, "", 14, -1);
    MissionUtility::SetWeaponOrd(mHandles[534], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    MissionUtility::Defend(mHandles[534], "battle3_defend", 0, 600.0f);
    mHandles[535] = MissionUtility::CreateObject("rep_tank_fighter1", "battle3_spawn1", 1, "", 14, -1);
    MissionUtility::SetWeaponOrd(mHandles[535], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    MissionUtility::Defend(mHandles[535], "battle3_defend", 0, 600.0f);
    mHandles[537] = MissionUtility::CreateObject("rep_tank_fighter1", "battle3_spawn1", 4, "", 14, -1);
    MissionUtility::SetWeaponOrd(mHandles[537], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    MissionUtility::Defend(mHandles[537], "battle3_defend", 0, 600.0f);
    mHandles[538] = MissionUtility::CreateObject("rep_tank_fighter1", "battle3_spawn1", 7, "", 14, -1);
    MissionUtility::SetWeaponOrd(mHandles[538], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
    MissionUtility::Defend(mHandles[538], "battle3_defend", 0, 600.0f);
    mHandles[533] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[533], mHandles[534]);
    MissionUtility::AddFlockMember(mHandles[533], mHandles[535]);
    MissionUtility::AddFlockMember(mHandles[533], mHandles[536]);
    MissionUtility::AddFlockMember(mHandles[533], mHandles[537]);
    MissionUtility::AddFlockMember(mHandles[533], mHandles[538]);
    mHandles[547] = MissionUtility::CreateObject("cis_tank_fighter", "battle3_spawn2", 0, "", 15, -1);
    MissionUtility::SetWeaponOrd(mHandles[547], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    MissionUtility::Defend(mHandles[547], "battle3_defend", 0, 600.0f);
    mHandles[548] = MissionUtility::CreateObject("cis_tank_wheeled", "battle3_spawn2", 1, "", 15, -1);
    MissionUtility::SetWeaponOrd(mHandles[548], "cis_missile_wheeled", "cis_missile_wheeled_weak_ord");
    MissionUtility::Defend(mHandles[548], "battle3_defend", 0, 600.0f);
    mHandles[549] = MissionUtility::CreateObject("cis_tank_assault", "battle3_spawn2", 2, "", 15, -1);
    MissionUtility::SetWeaponOrd(mHandles[549], "cis_blaster_assault", "cis_blaster_assault_weak_ord");
    MissionUtility::SetWeaponOrd(mHandles[549], "cis_cannon_assault", "cis_cannon_assault_weak_ord");
    MissionUtility::Defend(mHandles[549], "battle3_defend", 0, 600.0f);
    mHandles[550] = MissionUtility::CreateObject("cis_tank_fighter", "battle3_spawn2", 3, "", 15, -1);
    MissionUtility::SetWeaponOrd(mHandles[550], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    MissionUtility::Defend(mHandles[550], "battle3_defend", 0, 600.0f);
    mHandles[551] = MissionUtility::CreateObject("cis_tank_fighter", "battle3_spawn2", 4, "", 15, -1);
    MissionUtility::SetWeaponOrd(mHandles[551], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
    MissionUtility::Defend(mHandles[551], "battle3_defend", 0, 600.0f);
    mHandles[546] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[546], mHandles[547]);
    MissionUtility::AddFlockMember(mHandles[546], mHandles[548]);
    MissionUtility::AddFlockMember(mHandles[546], mHandles[549]);
    MissionUtility::AddFlockMember(mHandles[546], mHandles[550]);
    MissionUtility::AddFlockMember(mHandles[546], mHandles[551]);
    mHandles[569] = MissionUtility::CreateObject("cis_fly_fighter", "flier_fight4", 0, 20, -1, -1);
    mHandles[576] = MissionUtility::CreateObject("rep_fly_fighter", "flier_fight4", 0, 10, -1, -1);
    mHandles[562] = MissionUtility::CreateFlock(mHandles[569], (Formation)3);
    MissionUtility::Goto(mHandles[562], "flier_fight4", true);
    mHandles[570] = MissionUtility::CreateObject("cis_fly_fighter", "flier_fight5", 0, 20, -1, -1);
    mHandles[577] = MissionUtility::CreateObject("rep_fly_fighter", "flier_fight5", 0, 10, -1, -1);
    mHandles[563] = MissionUtility::CreateFlock(mHandles[570], (Formation)3);
    MissionUtility::Goto(mHandles[563], "flier_fight5", true);
    mHandles[571] = MissionUtility::CreateObject("cis_fly_fighter", "flier_fight6", 0, 20, -1, -1);
    mHandles[578] = MissionUtility::CreateObject("rep_fly_fighter", "flier_fight6", 0, 10, -1, -1);
    mHandles[564] = MissionUtility::CreateFlock(mHandles[571], (Formation)3);
    MissionUtility::Goto(mHandles[564], "flier_fight6", true);
    }
    }
    }
    }
    }
    }

    // ---- +0x7ad4  784 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "generator3_land")) {
    if (!mFlags[156]) {
    if (!MissionUtility::IsLoaded(mHandles[303], mHandles[171])) {
    if (!mFlags[88]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[303]);
    MissionUtility::Goto(mHandles[303], "g3_troops_goto", true);
    MissionUtility::SetCurHealth(mHandles[303], 20.0f);
    MissionUtility::Objectify(mHandles[303], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[88] = true;
    }
    }
    if (!MissionUtility::IsLoaded(mHandles[304], mHandles[171])) {
    if (!mFlags[89]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[304]);
    MissionUtility::SetCurHealth(mHandles[304], 20.0f);
    MissionUtility::Goto(mHandles[304], "g3_troops_goto", true);
    MissionUtility::Objectify(mHandles[304], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[89] = true;
    }
    }
    if (!MissionUtility::IsLoaded(mHandles[305], mHandles[171])) {
    if (!mFlags[90]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[305]);
    MissionUtility::SetCurHealth(mHandles[305], 20.0f);
    MissionUtility::Goto(mHandles[305], "g3_troops_goto", true);
    MissionUtility::Objectify(mHandles[305], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[90] = true;
    }
    }
    if (!MissionUtility::IsLoaded(mHandles[306], mHandles[171])) {
    if (!mFlags[91]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[306]);
    MissionUtility::SetCurHealth(mHandles[306], 20.0f);
    MissionUtility::Goto(mHandles[306], "g3_troops_goto", true);
    MissionUtility::Objectify(mHandles[306], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[91] = true;
    }
    }
    if (!MissionUtility::IsLoaded(mHandles[307], mHandles[171])) {
    if (!mFlags[92]) {
    MissionUtility::AddFlockMember(mHandles[173], mHandles[307]);
    MissionUtility::SetCurHealth(mHandles[307], 20.0f);
    MissionUtility::Goto(mHandles[307], "g3_troops_goto", true);
    MissionUtility::Objectify(mHandles[307], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[92] = true;
    }
    }
    if (mFlags[88]) {
    if (mFlags[89]) {
    if (mFlags[90]) {
    if (mFlags[157]) {
    mFlags[157] = true;
    }
    }
    }
    }
    if (mFlags[88]) {
    if (mFlags[89]) {
    if (mFlags[90]) {
    if (mFlags[91]) {
    if (mFlags[92]) {
    MissionUtility::TakeOff(mHandles[171]);
    MissionUtility::SetTeamNum(mHandles[171], 1);
    MissionUtility::StopCin(mHandles[296]);
    MissionUtility::StartSound("ERT21_05D", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::RemoveObjective(mInts[8]);
    mInts[9] = MissionUtility::AddObjective("missions.Thule1.objective.str0005");
    MissionUtility::DisplayText("missions.Thule1.text.str0006", 5.0f, -1.0f);
    MissionUtility::RemoveObjectify(mHandles[143]);
    mFlags[156] = true;
    }
    }
    }
    }
    }
    }
    }

    // ---- +0x7de4  116 bytes ----
    if (!mFlags[93] && !mFlags[167] && mFlags[156]) {
        if (MissionUtility::GetFlockCount(mHandles[173]) < 5) {
        if (!mFlags[61]) {
        MissionUtility::BonusObjectiveFailed(mInts[17]);
        MissionUtility::StartSound("CTINT_30", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[61] = true;
        }
        }
    }

    // ---- +0x7e58  108 bytes ----
    if (!mFlags[93] && !mFlags[167] && mFlags[156]) {
        if (MissionUtility::GetFlockCount(mHandles[173]) < 3) {
        if (!mFlags[62]) {
        MissionUtility::StartSound("CTINT_32", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[62] = true;
        }
        }
    }

    // ---- +0x7ec4  116 bytes ----
    if (mFlags[93] && !mFlags[167] && mFlags[156]) {
        if (MissionUtility::GetFlockCount(mHandles[173]) < 5) {
        if (!mFlags[63]) {
        MissionUtility::BonusObjectiveFailed(mInts[17]);
        MissionUtility::StartSound("CTINT_30", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[63] = true;
        }
        }
    }

    // ---- +0x7f38  108 bytes ----
    if (mFlags[93] && !mFlags[167] && mFlags[156]) {
        if (MissionUtility::GetFlockCount(mHandles[173]) < 3) {
        if (!mFlags[64]) {
        MissionUtility::StartSound("CTINT_32", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[64] = true;
        }
        }
    }

    // ---- +0x7fa4  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[303], "g3_troops_goto", 1) < 20.0f) {
    if (!mFlags[83]) {
    mFlags[83] = true;
    }
    }

    // ---- +0x7fd4  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[304], "g3_troops_goto", 1) < 20.0f) {
    if (!mFlags[84]) {
    mFlags[84] = true;
    }
    }

    // ---- +0x8004  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[305], "g3_troops_goto", 1) < 20.0f) {
    if (!mFlags[85]) {
    mFlags[85] = true;
    }
    }

    // ---- +0x8034  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[306], "g3_troops_goto", 1) < 20.0f) {
    if (!mFlags[86]) {
    mFlags[86] = true;
    }
    }

    // ---- +0x8064  48 bytes ----
    if (MissionUtility::GetDistance(mHandles[307], "g3_troops_goto", 1) < 20.0f) {
    if (!mFlags[87]) {
    mFlags[87] = true;
    }
    }

    // ---- +0x8094  88 bytes ----
    if (!mFlags[167]) {
        if (mFlags[83]
            || mFlags[84]
            || mFlags[85]
            || mFlags[86]
            || mFlags[87]) {
        mFlags[167] = true;
        BeginTimer(mTimer8);
        }
    }

    // ---- +0x80ec  240 bytes ----
    if (mFlags[156]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[468])) {
        if (MissionUtility::IsFlockAlive(mHandles[173])) {
        mHandles[468] = MissionUtility::CreateObject("cis_tank_assault", "g3_unit_spawner1", 0, "g3_gat1", 2, -1);
        MissionUtility::Goto(mHandles[468], "g3_spawner2_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[468], "g3_gats_goto1", 0);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[468], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x81dc  240 bytes ----
    if (mFlags[156]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[469])) {
        if (MissionUtility::IsFlockAlive(mHandles[173])) {
        mHandles[469] = MissionUtility::CreateObject("cis_tank_fighter", "g3_unit_spawner1", 1, "g3_gat2", 2, -1);
        MissionUtility::Goto(mHandles[469], "g3_spawner2_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[469], "g3_gats_goto1", 1);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[469], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x82cc  240 bytes ----
    if (mFlags[156]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[470])) {
        if (MissionUtility::IsFlockAlive(mHandles[173])) {
        mHandles[470] = MissionUtility::CreateObject("cis_tank_fighter", "g3_unit_spawner1", 2, "g3_gat3", 2, -1);
        MissionUtility::Goto(mHandles[470], "g3_spawner2_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[470], "g3_gats_goto1", 2);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[470], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x83bc  240 bytes ----
    if (mFlags[156]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[471])) {
        if (MissionUtility::IsFlockAlive(mHandles[173])) {
        mHandles[471] = MissionUtility::CreateObject("cis_tank_fighter", "g3_unit_spawner2", 0, "g3_gat4", 2, -1);
        MissionUtility::Goto(mHandles[471], "g3_spawner1_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[471], "g3_gats_goto2", 0);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[471], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x84ac  240 bytes ----
    if (mFlags[156]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[472])) {
        if (MissionUtility::IsFlockAlive(mHandles[173])) {
        mHandles[472] = MissionUtility::CreateObject("cis_tank_assault", "g3_unit_spawner2", 1, "g3_gat5", 2, -1);
        MissionUtility::Goto(mHandles[472], "g3_spawner1_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[472], "g3_gats_goto2", 1);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[472], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x859c  240 bytes ----
    if (mFlags[156]) {
        if (MissionUtility::GetDistance(mHandles[171], mHandles[143]) < 800.0f) {
        if (!MissionUtility::IsAlive(mHandles[473])) {
        if (MissionUtility::IsFlockAlive(mHandles[173])) {
        mHandles[473] = MissionUtility::CreateObject("cis_tank_fighter", "g3_unit_spawner2", 2, "g3_gat6", 2, -1);
        MissionUtility::Goto(mHandles[473], "g3_spawner1_clear", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[473], "g3_gats_goto2", 2);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[473], mHandles[173], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        }
        }
        }
    }

    // ---- +0x868c  384 bytes ----
    if (mTimer8 > 20.0f) {
    if (!mFlags[168]) {
    MissionUtility::StartSound("ERT21_11", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::RemoveObjectify(mHandles[143]);
    MissionUtility::Objectify(mHandles[447], "missions.Thule1.marker.str0005", false, true, 0.0f);
    MissionUtility::RemoveObjective(mInts[9]);
    mInts[6] = MissionUtility::AddObjective("missions.Thule1.objective.str0006");
    MissionUtility::DisplayText("missions.Thule1.text.str0007", 5.0f, -1.0f);
    MissionUtility::SetMaxHealth(mHandles[447], 10000.0f);
    MissionUtility::SetCurHealth(mHandles[447], 10000.0f);
    MissionUtility::AddHealthBar(mHandles[447], 0, 400.0f);
    MissionUtility::SetTeamNum(mHandles[447], 2);
    MissionUtility::SetVelocForward(mHandles[303], 15.0f);
    MissionUtility::SetVelocForward(mHandles[304], 13.0f);
    MissionUtility::SetVelocForward(mHandles[305], 16.0f);
    MissionUtility::SetVelocForward(mHandles[306], 15.0f);
    MissionUtility::SetVelocForward(mHandles[307], 18.0f);
    MissionUtility::Goto(mHandles[303], "g3_troops_goto", 0);
    MissionUtility::Goto(mHandles[304], "g3_troops_goto", 0);
    MissionUtility::Goto(mHandles[305], "g3_troops_goto", 0);
    MissionUtility::Goto(mHandles[306], "g3_troops_goto", 0);
    MissionUtility::Goto(mHandles[307], "g3_troops_goto", 0);
    MissionUtility::RemoveObjectify(mHandles[303]);
    MissionUtility::RemoveObjectify(mHandles[304]);
    MissionUtility::RemoveObjectify(mHandles[305]);
    MissionUtility::RemoveObjectify(mHandles[306]);
    MissionUtility::RemoveObjectify(mHandles[307]);
    MissionUtility::BeginWave("inst");
    mFlags[168] = true;
    }
    }

    // ---- +0x880c  540 bytes ----
    if (!MissionUtility::IsAlive(mHandles[447]) & !mFlags[79]) {
    MissionUtility::RemoveTurnAroundRegion("gen2_turnaround");
    BeginTimer(mTimer15);
    mHandles[163] = MissionUtility::CreateObject("efarm_explo", "g3_smoke_spawn", 0, "", 0, -1);
    mHandles[448] = MissionUtility::CreateObject("thu_bldg_efarm", Vector(-2111.3738f, -337.05655f, -2994.3433f), "", 1, -1, Quat(1.000007f, 0.0f, 0.0f, 0.0f), -1);
    mHandles[160] = MissionUtility::CreateObject("smoke_bigblack", "g3_smoke_spawn", 0, "", 0, -1);
    mHandles[161] = MissionUtility::CreateObject("smoke_bigblack", "g3_smoke_spawn", 1, "", 0, -1);
    mHandles[162] = MissionUtility::CreateObject("smoke_bigblack", "g3_smoke_spawn", 2, "", 0, -1);
    MissionUtility::RemoveObject(mHandles[318]);
    MissionUtility::RemoveObject(mHandles[319]);
    MissionUtility::RemoveObject(mHandles[320]);
    MissionUtility::ObjectiveComplete(mInts[6]);
    mInts[10] = MissionUtility::AddObjective("missions.Thule1.objective.str0007");
    mFlags[79] = true;
    }

    // ---- +0x8a28  360 bytes ----
    if (mFlags[79]) {
        if (!MissionUtility::IsCinRunning(mHandles[287])) {
        MissionUtility::StartSound("ERT21_10", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[114] = MissionUtility::GetHandle("inst_arm_unit1");
        mHandles[115] = MissionUtility::GetHandle("inst_arm_unit2");
        mHandles[116] = MissionUtility::GetHandle("inst_arm_unit3");
        mHandles[117] = MissionUtility::GetHandle("inst_arm_unit4");
        mHandles[118] = MissionUtility::GetHandle("inst_arm_unit5");
        mHandles[119] = MissionUtility::GetHandle("inst_arm_unit6");
        mHandles[120] = MissionUtility::GetHandle("inst_arm_unit7");
        mHandles[121] = MissionUtility::GetHandle("inst_arm_unit8");
        mHandles[122] = MissionUtility::GetHandle("inst_arm_unit9");
        mHandles[123] = MissionUtility::GetHandle("inst_arm_unit10");
        mHandles[124] = MissionUtility::GetHandle("inst_arm_unit11");
        mHandles[125] = MissionUtility::GetHandle("inst_arm_unit12");
        MissionUtility::SetVelocForward(mHandles[114], 0.0f);
        MissionUtility::SetVelocForward(mHandles[115], 0.0f);
        MissionUtility::SetVelocForward(mHandles[116], 0.0f);
        MissionUtility::SetVelocForward(mHandles[117], 0.0f);
        MissionUtility::SetVelocForward(mHandles[118], 0.0f);
        MissionUtility::SetVelocForward(mHandles[119], 0.0f);
        MissionUtility::SetVelocForward(mHandles[120], 0.0f);
        MissionUtility::SetVelocForward(mHandles[121], 0.0f);
        MissionUtility::SetVelocForward(mHandles[122], 0.0f);
        MissionUtility::SetVelocForward(mHandles[123], 0.0f);
        MissionUtility::SetVelocForward(mHandles[124], 0.0f);
        MissionUtility::SetVelocForward(mHandles[125], 0.0f);
        mPhase = 7;
        }
    }

    // ---- +0x8b90  2948 bytes ----
    if (!mFlags[167] && mFlags[156]) {
        if (!MissionUtility::IsAlive(mHandles[303])) {
        if (!MissionUtility::IsAlive(mHandles[304])) {
        if (!MissionUtility::IsAlive(mHandles[305])) {
        if (!MissionUtility::IsAlive(mHandles[306])) {
        if (!MissionUtility::IsAlive(mHandles[307])) {
        if (!mFlags[93]) {
        mFlags[88] = false;
        mFlags[89] = false;
        mFlags[90] = false;
        mFlags[91] = false;
        mFlags[92] = false;
        mFlags[46] = false;
        MissionUtility::RemoveObjectify(mHandles[143]);
        MissionUtility::RemoveObjective(mInts[9]);
        mInts[7] = MissionUtility::AddObjective("missions.Thule1.objective.str0008");
        MissionUtility::DisplayText("missions.Thule1.text.str0008", 5.0f, -1.0f);
        MissionUtility::Objectify("lpad", 0, "missions.Thule1.marker.str0006", true, true, 0.0f, 2.0f);
        mFlags[156] = false;
        mFlags[157] = false;
        mFlags[93] = true;
        MissionUtility::AddFlyerAmmoBox("health_ammo_spawn", 0, -1.0f);
        MissionUtility::AddFlyerHealthBox("health_ammo_spawn", 1, -1.0f);
        mHandles[303] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 0, "troop1", 1, -1);
        mHandles[304] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 1, "troop2", 1, -1);
        mHandles[305] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 2, "troop3", 1, -1);
        mHandles[306] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 3, "troop4", 1, -1);
        mHandles[307] = MissionUtility::CreateObject("rep_inf_clone", "troops_spawn", 4, "troop5", 1, -1);
        MissionUtility::SetCurHealth(mHandles[303], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[304], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[305], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[306], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[307], 999999.0f);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[303]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[304]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[305]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[306]);
        MissionUtility::AddFlockMember(mHandles[173], mHandles[307]);
        MissionUtility::Cargo(mHandles[303]);
        MissionUtility::Cargo(mHandles[304]);
        MissionUtility::Cargo(mHandles[305]);
        MissionUtility::Cargo(mHandles[306]);
        MissionUtility::Cargo(mHandles[307]);
        MissionUtility::StartSound("MWT21_05F", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveFlock(mHandles[507], false);
        MissionUtility::RemoveFlock(mHandles[520], false);
        MissionUtility::RemoveFlock(mHandles[533], false);
        MissionUtility::RemoveFlock(mHandles[546], false);
        mHandles[508] = MissionUtility::CreateObject("rep_tank_fighter1", "battle2_spawn1", 0, "", 12, -1);
        MissionUtility::SetWeaponOrd(mHandles[508], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[510] = MissionUtility::CreateObject("rep_tank_fighter1", "battle2_spawn1", 2, "", 12, -1);
        MissionUtility::SetWeaponOrd(mHandles[510], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[511] = MissionUtility::CreateObject("rep_tank_fighter1", "battle2_spawn1", 4, "", 12, -1);
        MissionUtility::SetWeaponOrd(mHandles[511], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[512] = MissionUtility::CreateObject("rep_tank_fighter1", "battle2_spawn1", 7, "", 12, -1);
        MissionUtility::SetWeaponOrd(mHandles[512], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[507] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[507], mHandles[508]);
        MissionUtility::AddFlockMember(mHandles[507], mHandles[509]);
        MissionUtility::AddFlockMember(mHandles[507], mHandles[510]);
        MissionUtility::AddFlockMember(mHandles[507], mHandles[511]);
        MissionUtility::AddFlockMember(mHandles[507], mHandles[512]);
        mHandles[521] = MissionUtility::CreateObject("cis_tank_fighter", "battle2_spawn2", 0, "", 13, -1);
        MissionUtility::SetWeaponOrd(mHandles[521], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        mHandles[522] = MissionUtility::CreateObject("cis_tank_wheeled", "battle2_spawn2", 1, "", 13, -1);
        MissionUtility::SetWeaponOrd(mHandles[522], "cis_missile_wheeled", "cis_missile_wheeled_weak_ord");
        mHandles[523] = MissionUtility::CreateObject("cis_tank_assault", "battle2_spawn2", 2, "", 13, -1);
        MissionUtility::SetWeaponOrd(mHandles[523], "cis_blaster_assault", "cis_blaster_assault_weak_ord");
        MissionUtility::SetWeaponOrd(mHandles[523], "cis_cannon_assault", "cis_cannon_assault_weak_ord");
        mHandles[524] = MissionUtility::CreateObject("cis_tank_fighter", "battle2_spawn2", 3, "", 13, -1);
        MissionUtility::SetWeaponOrd(mHandles[524], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        mHandles[525] = MissionUtility::CreateObject("cis_tank_fighter", "battle2_spawn2", 4, "", 13, -1);
        MissionUtility::SetWeaponOrd(mHandles[525], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        mHandles[520] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[520], mHandles[521]);
        MissionUtility::AddFlockMember(mHandles[520], mHandles[522]);
        MissionUtility::AddFlockMember(mHandles[520], mHandles[523]);
        MissionUtility::AddFlockMember(mHandles[520], mHandles[524]);
        MissionUtility::AddFlockMember(mHandles[520], mHandles[525]);
        mHandles[534] = MissionUtility::CreateObject("rep_tank_fighter1", "battle3_spawn1", 0, "", 14, -1);
        MissionUtility::SetWeaponOrd(mHandles[534], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[535] = MissionUtility::CreateObject("rep_tank_fighter1", "battle3_spawn1", 1, "", 14, -1);
        MissionUtility::SetWeaponOrd(mHandles[535], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[537] = MissionUtility::CreateObject("rep_tank_fighter1", "battle3_spawn1", 4, "", 14, -1);
        MissionUtility::SetWeaponOrd(mHandles[537], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[538] = MissionUtility::CreateObject("rep_tank_fighter1", "battle3_spawn1", 7, "", 14, -1);
        MissionUtility::SetWeaponOrd(mHandles[538], "rep_blaster_fighter1", "rep_blaster_fighter1_weak_ord");
        mHandles[533] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[533], mHandles[534]);
        MissionUtility::AddFlockMember(mHandles[533], mHandles[535]);
        MissionUtility::AddFlockMember(mHandles[533], mHandles[536]);
        MissionUtility::AddFlockMember(mHandles[533], mHandles[537]);
        MissionUtility::AddFlockMember(mHandles[533], mHandles[538]);
        mHandles[547] = MissionUtility::CreateObject("cis_tank_fighter", "battle3_spawn2", 0, "", 15, -1);
        MissionUtility::SetWeaponOrd(mHandles[547], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        mHandles[548] = MissionUtility::CreateObject("cis_tank_wheeled", "battle3_spawn2", 1, "", 15, -1);
        MissionUtility::SetWeaponOrd(mHandles[548], "cis_missile_wheeled", "cis_missile_wheeled_weak_ord");
        mHandles[549] = MissionUtility::CreateObject("cis_tank_assault", "battle3_spawn2", 2, "", 15, -1);
        MissionUtility::SetWeaponOrd(mHandles[549], "cis_blaster_assault", "cis_blaster_assault_weak_ord");
        MissionUtility::SetWeaponOrd(mHandles[549], "cis_cannon_assault", "cis_cannon_assault_weak_ord");
        mHandles[550] = MissionUtility::CreateObject("cis_tank_fighter", "battle3_spawn2", 3, "", 15, -1);
        MissionUtility::SetWeaponOrd(mHandles[550], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        mHandles[551] = MissionUtility::CreateObject("cis_tank_fighter", "battle3_spawn2", 4, "", 15, -1);
        MissionUtility::SetWeaponOrd(mHandles[551], "cis_blaster_fighter", "cis_blaster_fighter_weak_ord");
        mHandles[546] = MissionUtility::CreateFlock();
        MissionUtility::AddFlockMember(mHandles[546], mHandles[547]);
        MissionUtility::AddFlockMember(mHandles[546], mHandles[548]);
        MissionUtility::AddFlockMember(mHandles[546], mHandles[549]);
        MissionUtility::AddFlockMember(mHandles[546], mHandles[550]);
        MissionUtility::AddFlockMember(mHandles[546], mHandles[551]);
        }
        }
        }
        }
        }
        }
    }

    // ---- +0x9714  244 bytes ----
    if (mFlags[93] && !mFlags[169]) {
        if (MissionUtility::IsLoaded(mHandles[303], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[304], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[305], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[306], mHandles[171])) {
        if (MissionUtility::IsLoaded(mHandles[307], mHandles[171])) {
        MissionUtility::RemoveObjectify("lpad", 0);
        MissionUtility::RemoveObjective(mInts[7]);
        MissionUtility::Objectify(mHandles[143], "missions.Thule1.marker.str0007", false, true, 0.0f);
        mInts[8] = MissionUtility::AddObjective("missions.Thule1.objective.str0009");
        MissionUtility::DisplayText("missions.Thule1.text.str0009", 5.0f, -1.0f);
        MissionUtility::StartSound("CTR06_05", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[5] = true;
        mFlags[169] = true;
        }
        }
        }
        }
        }
    }

    // ---- +0x9808  144 bytes ----
    if (mFlags[93]) {
    if (mFlags[156]) {
    if (!mFlags[167]) {
    if (!MissionUtility::IsAlive(mHandles[303])) {
    if (!MissionUtility::IsAlive(mHandles[304])) {
    if (!MissionUtility::IsAlive(mHandles[305])) {
    if (!MissionUtility::IsAlive(mHandles[306])) {
    if (!MissionUtility::IsAlive(mHandles[307])) {
    if (!mFlags[1]) {
    MissionUtility::MissionFailure();
    mFlags[1] = true;

        break;
    case 7:
    // ---- +0x9898  200 bytes ----
    MissionUtility::MidMissionSavePlayer(8);
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[133]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[134]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[135]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[136]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[137]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[138]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[139]));
    MissionUtility::MidMissionSave((bool)MissionUtility::IsAlive(mHandles[140]));
    MissionUtility::MidMissionSave((bool)mFlags[93]);
    MissionUtility::MidMissionSave(mTimer20);
    MissionUtility::MidMissionSave((bool)mFlags[57]);
    MissionUtility::MidMissionSave((bool)mFlags[58]);
    MissionUtility::MidMissionSave((bool)mFlags[59]);
    MissionUtility::MidMissionSave((bool)mFlags[60]);
    MissionUtility::MidMissionSave((bool)mFlags[61]);
    MissionUtility::MidMissionSave((bool)mFlags[62]);
    MissionUtility::MidMissionSave((bool)mFlags[63]);
    MissionUtility::MidMissionSave((bool)mFlags[64]);
    mPhase = 6;

        break;
    case 8:
    // ---- +0x9960  292 bytes ----
    mInts[1] = MissionUtility::AddObjective("missions.Thule1.objective.str0010");
    mInts[2] = MissionUtility::AddObjective("missions.Thule1.objective.str0004");
    mInts[6] = MissionUtility::AddObjective("missions.Thule1.objective.str0006");
    mInts[10] = MissionUtility::AddObjective("missions.Thule1.objective.str0007");
    MissionUtility::ObjectiveComplete(mInts[1]);
    MissionUtility::ObjectiveComplete(mInts[2]);
    MissionUtility::ObjectiveComplete(mInts[6]);
    MissionUtility::RemoveFlock(mHandles[322], false);
    MissionUtility::RemoveFlock(mHandles[385], false);
    MissionUtility::MoveObjectWithRotation(mHandles[171], "player_aftercin7", 0, true);
    MissionUtility::MidMissionLoadPlayer();
    MissionUtility::MidMissionLoad(mLoadB0);
    MissionUtility::MidMissionLoad(mLoadB1);
    MissionUtility::MidMissionLoad(mLoadB2);
    MissionUtility::MidMissionLoad(mLoadB3);
    MissionUtility::MidMissionLoad(mLoadB4);
    MissionUtility::MidMissionLoad(mLoadB5);
    MissionUtility::MidMissionLoad(mLoadB6);
    MissionUtility::MidMissionLoad(mLoadB7);
    MissionUtility::MidMissionLoad(mFlags[93]);
    MissionUtility::MidMissionLoad(mLoadTB);
    MissionUtility::MidMissionLoad(mFlags[57]);
    MissionUtility::MidMissionLoad(mFlags[58]);
    MissionUtility::MidMissionLoad(mFlags[59]);
    MissionUtility::MidMissionLoad(mFlags[60]);
    MissionUtility::MidMissionLoad(mFlags[61]);
    MissionUtility::MidMissionLoad(mFlags[62]);
    MissionUtility::MidMissionLoad(mFlags[63]);
    MissionUtility::MidMissionLoad(mFlags[64]);
    if (!mLoadB0) {
    MissionUtility::DamageObject(mHandles[133], 99999.0f, 99999.0f);
    }

    // ---- +0x9a84  28 bytes ----
    if (!mLoadB1) {
    MissionUtility::DamageObject(mHandles[134], 99999.0f, 99999.0f);
    }

    // ---- +0x9aa0  28 bytes ----
    if (!mLoadB2) {
    MissionUtility::DamageObject(mHandles[135], 99999.0f, 99999.0f);
    }

    // ---- +0x9abc  28 bytes ----
    if (!mLoadB3) {
    MissionUtility::DamageObject(mHandles[136], 99999.0f, 99999.0f);
    }

    // ---- +0x9ad8  28 bytes ----
    if (!mLoadB4) {
    MissionUtility::DamageObject(mHandles[137], 99999.0f, 99999.0f);
    }

    // ---- +0x9af4  28 bytes ----
    if (!mLoadB5) {
    MissionUtility::DamageObject(mHandles[138], 99999.0f, 99999.0f);
    }

    // ---- +0x9b10  28 bytes ----
    if (!mLoadB6) {
    MissionUtility::DamageObject(mHandles[139], 99999.0f, 99999.0f);
    }

    // ---- +0x9b2c  28 bytes ----
    if (!mLoadB7) {
    MissionUtility::DamageObject(mHandles[140], 99999.0f, 99999.0f);
    }

    // ---- +0x9b48  104 bytes ----
    if (mFlags[57]
        || mFlags[58]
        || mFlags[59]
        || mFlags[60]
        || mFlags[61]
        || mFlags[62]
        || mFlags[63]
        || mFlags[64]) {
    MissionUtility::BonusObjectiveFailed(mInts[17]);
    }

    // ---- +0x9bb0  56 bytes ----
    MissionUtility::RemoveTurnAroundRegion("landing_turnaround");
    MissionUtility::RemoveTurnAroundRegion("landing_turnaround1");
    StopTimer(mTimer20);
    mTimer20 = mLoadTB;
    ResumeTimer(mTimer20);
    mPhase = 6;

        break;
    case 6:
    // ---- +0x9be8  884 bytes ----
    if (mFlags[172]) {
        if (!mFlags[57]) {
        if (!mFlags[58]) {
        if (!mFlags[59]) {
        if (!mFlags[60]) {
        if (!mFlags[61]) {
        if (!mFlags[62]) {
        if (!mFlags[63]) {
        if (!mFlags[64]) {
        MissionUtility::BonusObjectiveComplete(mInts[17], true);
        }
        }
        }
        }
        }
        }
        }
        }
        MissionUtility::PlayMusic("EP2_V1_T12_01", true);
        MissionUtility::RemoveArmy(mHandles[100]);
        MissionUtility::RemoveArmy(mHandles[101]);
        MissionUtility::RemoveArmy(mHandles[102]);
        MissionUtility::RemoveArmy(mHandles[103]);
        MissionUtility::SetTeamNum(mHandles[271], 9);
        MissionUtility::SetTeamNum(mHandles[272], 9);
        MissionUtility::SetTeamNum(mHandles[273], 9);
        MissionUtility::SetTeamNum(mHandles[274], 9);
        MissionUtility::SetTeamNum(mHandles[275], 9);
        MissionUtility::SetTeamNum(mHandles[276], 9);
        MissionUtility::SetTeamNum(mHandles[277], 9);
        MissionUtility::SetTeamNum(mHandles[278], 9);
        MissionUtility::SetTeamNum(mHandles[279], 9);
        MissionUtility::SetTeamNum(mHandles[280], 9);
        MissionUtility::SetTeamNum(mHandles[270], 7);
        MissionUtility::SetEnemies(6, 7);
        MissionUtility::SetEnemies(1, 9);
        mHandles[51] = MissionUtility::AddFlyerArmy(0, "prop_flierfight1", 0, 6, 12.0f);
        mHandles[55] = MissionUtility::AddFlyerArmy(2, "prop_flierfight1", 0, 6, 12.0f);
        MissionUtility::Objectify(mHandles[270], "missions.Thule1.marker.str0008", false, true, 0.0f);
        MissionUtility::Patrol(mHandles[172], "base", 500.0f, true);
        MissionUtility::SetMaxHealth(mHandles[271], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[272], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[273], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[274], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[275], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[276], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[277], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[278], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[279], 2000.0f);
        MissionUtility::SetMaxHealth(mHandles[280], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[271], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[272], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[273], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[274], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[275], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[276], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[277], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[278], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[279], 2000.0f);
        MissionUtility::SetCurHealth(mHandles[280], 2000.0f);
        BeginTimer(mTimer16);
        BeginTimer(mTimer17);
        BeginTimer(mTimer18);
        mFlags[172] = false;
        mHandles[567] = MissionUtility::CreateObject("cis_fly_fighter", "flier_fight2", 0, 20, -1, -1);
        mHandles[574] = MissionUtility::CreateObject("rep_fly_fighter", "flier_fight2", 0, 10, -1, -1);
        mHandles[560] = MissionUtility::CreateFlock(mHandles[567], (Formation)3);
        MissionUtility::Goto(mHandles[560], "flier_fight2", true);
        mHandles[568] = MissionUtility::CreateObject("cis_fly_fighter", "flier_fight3", 0, 20, -1, -1);
        mHandles[575] = MissionUtility::CreateObject("rep_fly_fighter", "flier_fight3", 0, 10, -1, -1);
        mHandles[561] = MissionUtility::CreateFlock(mHandles[568], (Formation)3);
        MissionUtility::Goto(mHandles[561], "flier_fight3", true);
    }

    // ---- +0x9f5c  480 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "inst_proparmy_on")) {
    if (!mFlags[27]) {
    mHandles[104] = MissionUtility::AddPropArmy(2, "prop_armyfight_inst1", 0, 50, 2.0f);
    mHandles[105] = MissionUtility::AddPropArmy(3, "prop_armyfight_inst1", 1, 50, 2.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[104], "prop_armyfight_inst1", false);
    MissionUtility::SetPropArmyWayPoints(mHandles[105], "prop_armyfight_inst1", false);
    mHandles[106] = MissionUtility::AddPropArmy(2, "prop_armyfight_inst2", 0, 50, 2.0f);
    mHandles[107] = MissionUtility::AddPropArmy(3, "prop_armyfight_inst2", 1, 50, 2.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[106], "prop_armyfight_inst2", false);
    MissionUtility::SetPropArmyWayPoints(mHandles[107], "prop_armyfight_inst2", false);
    mHandles[108] = MissionUtility::AddPropArmy(2, "prop_armyfight_inst3", 0, 50, 2.0f);
    mHandles[109] = MissionUtility::AddPropArmy(3, "prop_armyfight_inst3", 1, 50, 2.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[108], "prop_armyfight_inst3", false);
    MissionUtility::SetPropArmyWayPoints(mHandles[109], "prop_armyfight_inst3", false);
    mHandles[110] = MissionUtility::AddPropArmy(2, "prop_armyfight_inst4", 0, 50, 2.0f);
    mHandles[111] = MissionUtility::AddPropArmy(3, "prop_armyfight_inst4", 1, 50, 2.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[110], "prop_armyfight_inst4", false);
    MissionUtility::SetPropArmyWayPoints(mHandles[111], "prop_armyfight_inst4", false);
    mHandles[112] = MissionUtility::AddPropArmy(2, "prop_armyfight_inst5", 0, 50, 2.0f);
    mHandles[113] = MissionUtility::AddPropArmy(3, "prop_armyfight_inst5", 1, 50, 2.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[112], "prop_armyfight_inst5", false);
    MissionUtility::SetPropArmyWayPoints(mHandles[113], "prop_armyfight_inst5", false);
    mFlags[27] = true;
    }
    }

    // ---- +0xa13c  120 bytes ----
    if (!MissionUtility::IsInsideRegion(mHandles[171], "inst_proparmy_on")) {
    if (mFlags[27]) {
    MissionUtility::RemoveArmy(mHandles[104]);
    MissionUtility::RemoveArmy(mHandles[105]);
    MissionUtility::RemoveArmy(mHandles[106]);
    MissionUtility::RemoveArmy(mHandles[107]);
    MissionUtility::RemoveArmy(mHandles[108]);
    MissionUtility::RemoveArmy(mHandles[109]);
    MissionUtility::RemoveArmy(mHandles[110]);
    MissionUtility::RemoveArmy(mHandles[111]);
    MissionUtility::RemoveArmy(mHandles[112]);
    MissionUtility::RemoveArmy(mHandles[113]);
    mFlags[27] = false;
    }
    }

    // ---- +0xa1b4  480 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "gen2_proparmy_set3_on")) {
    if (!mFlags[25]) {
    mHandles[61] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight5", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[61], "set3_prop_armyfight5", false);
    mHandles[78] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight5", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[78], "set3_prop_armyfight5", false);
    mHandles[62] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight6", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[62], "set3_prop_armyfight6", false);
    mHandles[79] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight6", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[79], "set3_prop_armyfight6", false);
    mHandles[63] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight7", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[63], "set3_prop_armyfight7", false);
    mHandles[80] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight7", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[80], "set3_prop_armyfight7", false);
    mHandles[64] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight8", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[64], "set3_prop_armyfight8", false);
    mHandles[81] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight8", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[81], "set3_prop_armyfight8", false);
    mHandles[67] = MissionUtility::AddPropArmy(3, "set3_prop_armyfight11", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[67], "set3_prop_armyfight11", false);
    mHandles[84] = MissionUtility::AddPropArmy(2, "set3_prop_armyfight11", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[84], "set3_prop_armyfight11", false);
    mFlags[25] = true;
    }
    }

    // ---- +0xa394  120 bytes ----
    if (!MissionUtility::IsInsideRegion(mHandles[171], "gen2_proparmy_set3_on")) {
    if (mFlags[25]) {
    MissionUtility::RemoveArmy(mHandles[61]);
    MissionUtility::RemoveArmy(mHandles[62]);
    MissionUtility::RemoveArmy(mHandles[63]);
    MissionUtility::RemoveArmy(mHandles[64]);
    MissionUtility::RemoveArmy(mHandles[67]);
    MissionUtility::RemoveArmy(mHandles[78]);
    MissionUtility::RemoveArmy(mHandles[79]);
    MissionUtility::RemoveArmy(mHandles[80]);
    MissionUtility::RemoveArmy(mHandles[81]);
    MissionUtility::RemoveArmy(mHandles[84]);
    mFlags[25] = false;
    }
    }

    // ---- +0xa40c  392 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[171], "gen2_proparmy_set4_on")) {
    if (!mFlags[26]) {
    mHandles[92] = MissionUtility::AddPropArmy(3, "set4_armyfight1", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[92], "set4_armyfight1", false);
    mHandles[96] = MissionUtility::AddPropArmy(2, "set4_armyfight1", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[96], "set4_armyfight1", false);
    mHandles[93] = MissionUtility::AddPropArmy(3, "set4_armyfight2", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[93], "set4_armyfight2", false);
    mHandles[97] = MissionUtility::AddPropArmy(2, "set4_armyfight2", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[97], "set4_armyfight2", false);
    mHandles[94] = MissionUtility::AddPropArmy(3, "set4_armyfight3", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[94], "set4_armyfight3", false);
    mHandles[98] = MissionUtility::AddPropArmy(2, "set4_armyfight3", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[98], "set4_armyfight3", false);
    mHandles[95] = MissionUtility::AddPropArmy(3, "set4_armyfight4", 1, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[95], "set4_armyfight4", false);
    mHandles[99] = MissionUtility::AddPropArmy(2, "set4_armyfight4", 0, 50, 3.0f);
    MissionUtility::SetPropArmyWayPoints(mHandles[99], "set4_armyfight4", false);
    mFlags[26] = true;
    }
    }

    // ---- +0xa594  104 bytes ----
    if (!MissionUtility::IsInsideRegion(mHandles[171], "gen2_proparmy_set4_on")) {
    if (mFlags[26]) {
    MissionUtility::RemoveArmy(mHandles[92]);
    MissionUtility::RemoveArmy(mHandles[93]);
    MissionUtility::RemoveArmy(mHandles[94]);
    MissionUtility::RemoveArmy(mHandles[95]);
    MissionUtility::RemoveArmy(mHandles[96]);
    MissionUtility::RemoveArmy(mHandles[97]);
    MissionUtility::RemoveArmy(mHandles[98]);
    MissionUtility::RemoveArmy(mHandles[99]);
    mFlags[26] = false;
    }
    }

    // ---- +0xa5fc  112 bytes ----
    if (!MissionUtility::IsAlive(mHandles[36])) {
    mHandles[36] = MissionUtility::CreateObject("rep_fly_gunship", "gunship_inst_circle1_spawn", 0, "", 1, -1);
    MissionUtility::Goto(mHandles[36], "gunship_inst_circle1", true);
    }

    // ---- +0xa66c  112 bytes ----
    if (!MissionUtility::IsAlive(mHandles[37])) {
    mHandles[37] = MissionUtility::CreateObject("rep_fly_gunship", "gunship_inst_circle2_spawn", 0, "", 1, -1);
    MissionUtility::Goto(mHandles[37], "gunship_inst_circle2", true);
    }

    // ---- +0xa6dc  336 bytes ----
    if (MissionUtility::GetDistance(mHandles[171], mHandles[270]) < 1000.0f) {
    if (!mFlags[96]) {
    MissionUtility::AddTurnAroundRegion("inst_turnaround", "inst_turnaround_focus", 0, 0, 0, 0);
    MissionUtility::EvictConfig("rep_inf_clone");
    MissionUtility::EvictConfig("thu_bldg_efarm");
    MissionUtility::EvictConfig("rep_fly_assault");
    MissionUtility::Objectify(mHandles[271], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[272], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[273], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[274], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[275], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[276], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[277], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[278], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[279], "missions.Thule1.marker.str0003", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[280], "missions.Thule1.marker.str0003", false, true, 0.0f);
    mFlags[96] = true;
    }
    }

    // ---- +0xa82c  44 bytes ----
    if (!mFlags[65]) {
        if (!MissionUtility::IsAlive(mHandles[129])) {
        BeginTimer(mTimer16);
        mFlags[65] = true;
        }
    }

    // ---- +0xa858  176 bytes ----
    if (!MissionUtility::IsAlive(mHandles[129])) {
    if (mTimer16 > 5.0f) {
    if (mFlags[65]) {
    mFlags[65] = false;
    mHandles[129] = MissionUtility::CreateObject("cis_tank_wheeled", "base_tankspawn", 0, "", 2, -1);
    MissionUtility::SetVelocForward(mHandles[129], 0.0f);
    MissionUtility::AttackTarget(mHandles[129], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0xa908  44 bytes ----
    if (!mFlags[66]) {
        if (!MissionUtility::IsAlive(mHandles[130])) {
        BeginTimer(mTimer17);
        mFlags[66] = true;
        }
    }

    // ---- +0xa934  176 bytes ----
    if (!MissionUtility::IsAlive(mHandles[130])) {
    if (mTimer17 > 5.0f) {
    if (mFlags[66]) {
    mFlags[66] = false;
    mHandles[130] = MissionUtility::CreateObject("cis_tank_wheeled", "base_tankspawn", 1, "", 2, -1);
    MissionUtility::SetVelocForward(mHandles[130], 0.0f);
    MissionUtility::AttackTarget(mHandles[130], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0xa9e4  44 bytes ----
    if (!mFlags[67]) {
        if (!MissionUtility::IsAlive(mHandles[131])) {
        BeginTimer(mTimer18);
        mFlags[67] = true;
        }
    }

    // ---- +0xaa10  176 bytes ----
    if (!MissionUtility::IsAlive(mHandles[131])) {
    if (mTimer18 > 5.0f) {
    if (mFlags[67]) {
    mFlags[67] = false;
    mHandles[131] = MissionUtility::CreateObject("cis_tank_wheeled", "base_tankspawn", 2, "", 3, -1);
    MissionUtility::SetVelocForward(mHandles[131], 0.0f);
    MissionUtility::AttackTarget(mHandles[131], mHandles[171], true, true, false, false);
    }
    }
    }

    // ---- +0xaac0  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[271])) {
    if (!mFlags[13]) {
    MissionUtility::DamageObject(mHandles[9], 999999.0f, 999999.0f);
    mFlags[13] = true;
    }
    }

    // ---- +0xaaf4  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[272])) {
    if (!mFlags[14]) {
    MissionUtility::DamageObject(mHandles[10], 999999.0f, 999999.0f);
    mFlags[14] = true;
    }
    }

    // ---- +0xab28  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[273])) {
    if (!mFlags[15]) {
    MissionUtility::DamageObject(mHandles[11], 999999.0f, 999999.0f);
    mFlags[15] = true;
    }
    }

    // ---- +0xab5c  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[274])) {
    if (!mFlags[16]) {
    MissionUtility::DamageObject(mHandles[12], 999999.0f, 999999.0f);
    mFlags[16] = true;
    }
    }

    // ---- +0xab90  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[275])) {
    if (!mFlags[17]) {
    MissionUtility::DamageObject(mHandles[13], 999999.0f, 999999.0f);
    mFlags[17] = true;
    }
    }

    // ---- +0xabc4  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[276])) {
    if (!mFlags[18]) {
    MissionUtility::DamageObject(mHandles[14], 999999.0f, 999999.0f);
    mFlags[18] = true;
    }
    }

    // ---- +0xabf8  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[277])) {
    if (!mFlags[19]) {
    MissionUtility::DamageObject(mHandles[15], 999999.0f, 999999.0f);
    mFlags[19] = true;
    }
    }

    // ---- +0xac2c  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[278])) {
    if (!mFlags[20]) {
    MissionUtility::DamageObject(mHandles[16], 999999.0f, 999999.0f);
    mFlags[20] = true;
    }
    }

    // ---- +0xac60  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[279])) {
    if (!mFlags[21]) {
    MissionUtility::DamageObject(mHandles[17], 999999.0f, 999999.0f);
    mFlags[21] = true;
    }
    }

    // ---- +0xac94  52 bytes ----
    if (!MissionUtility::IsAlive(mHandles[280])) {
    if (!mFlags[22]) {
    MissionUtility::DamageObject(mHandles[18], 999999.0f, 999999.0f);
    mFlags[22] = true;
    }
    }

    // ---- +0xacc8  1240 bytes ----
    if (!MissionUtility::IsAlive(mHandles[271])) {
    if (!MissionUtility::IsAlive(mHandles[272])) {
    if (!MissionUtility::IsAlive(mHandles[273])) {
    if (!MissionUtility::IsAlive(mHandles[274])) {
    if (!MissionUtility::IsAlive(mHandles[275])) {
    if (!MissionUtility::IsAlive(mHandles[276])) {
    if (!MissionUtility::IsAlive(mHandles[277])) {
    if (!MissionUtility::IsAlive(mHandles[278])) {
    if (!MissionUtility::IsAlive(mHandles[279])) {
    if (!MissionUtility::IsAlive(mHandles[280])) {
    if (!mFlags[82]) {
    BeginTimer(mTimer23);
    MissionUtility::PlayMusic("EP4_V2_T09_05", true);
    mHandles[290] = MissionUtility::RunCin("Cin10", true, true);
    MissionUtility::SetVelocNeutralFly(mHandles[171], 0.0f);
    MissionUtility::ObjectiveComplete(mInts[10]);
    mFlags[82] = true;
    MissionUtility::QueueSound("obt21_07", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("mwt21_12", 1.0f, 0.0f, 0.0f, "", 0, "");
    mHandles[580] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinWalkerPath", 0, "EndCinWalker", 0, -1, -1);
    mHandles[581] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinWalkerPath", 1, "EndCinWalker1", 0, -1, -1);
    mHandles[582] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinWalkerPath", 2, "EndCinWalker2", 0, -1, -1);
    mHandles[583] = MissionUtility::CreateObjectWithRotation("rep_walk_sixleg", "EndCinWalkerPath", 3, "EndCinWalker3", 0, -1, -1);
    mHandles[591] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath", 0, "EndCinTank", 0, -1, -1);
    mHandles[592] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath", 1, "EndCinTank1", 0, -1, -1);
    mHandles[593] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath", 2, "EndCinTank2", 0, -1, -1);
    mHandles[594] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath", 3, "EndCinTank3", 0, -1, -1);
    mHandles[595] = MissionUtility::CreateObjectWithRotation("rep_tank_fighter1", "EndCinTankPath", 4, "EndCinTank4", 0, -1, -1);
    mHandles[599] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "EndCinFighterPath", 0, "EndCinFighter", 0, -1, -1);
    mHandles[600] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "EndCinFighterPath", 1, "EndCinFighter1", 0, -1, -1);
    mHandles[601] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "EndCinFighterPath", 2, "EndCinFighter2", 0, -1, -1);
    mHandles[602] = MissionUtility::CreateObjectWithRotation("rep_fly_fighter", "EndCinFighterPath1", 0, "EndCinFighter3", 0, -1, -1);
    mHandles[605] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "EndCinGunshipPath", 0, "EndCinGunship", 0, -1, -1);
    mHandles[606] = MissionUtility::CreateObjectWithRotation("rep_fly_gunship", "EndCinGunshipPath", 1, "EndCinGunship1", 0, -1, -1);
    MissionUtility::OverrideSoundRange(mHandles[580], true);
    MissionUtility::OverrideSoundRange(mHandles[581], true);
    MissionUtility::OverrideSoundRange(mHandles[582], true);
    MissionUtility::OverrideSoundRange(mHandles[583], true);
    MissionUtility::OverrideSoundRange(mHandles[591], true);
    MissionUtility::OverrideSoundRange(mHandles[592], true);
    MissionUtility::OverrideSoundRange(mHandles[593], true);
    MissionUtility::OverrideSoundRange(mHandles[594], true);
    MissionUtility::OverrideSoundRange(mHandles[595], true);
    MissionUtility::OverrideSoundRange(mHandles[599], true);
    MissionUtility::OverrideSoundRange(mHandles[600], true);
    MissionUtility::OverrideSoundRange(mHandles[601], true);
    MissionUtility::OverrideSoundRange(mHandles[602], true);
    MissionUtility::OverrideSoundRange(mHandles[605], true);
    MissionUtility::OverrideSoundRange(mHandles[606], true);
    MissionUtility::SetVelocMinimumFly(mHandles[599], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[599], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[599], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[600], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[600], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[600], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[601], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[601], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[601], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[602], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[602], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[602], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[605], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[605], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[605], 0.0f);
    MissionUtility::SetVelocMinimumFly(mHandles[606], 0.0f);
    MissionUtility::SetVelocMaximumFly(mHandles[606], 0.0f);
    MissionUtility::SetVelocNeutralFly(mHandles[606], 0.0f);
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

    // ---- +0xb1a0  120 bytes ----
    if (mTimer23 > 0.2f) {
    if (!mFlags[6]) {
    mHandles[2] = MissionUtility::CreateObject("cis_mortar_assault_xpl", "inst_death_explosions", 0, "", 0, -1);
    mFlags[6] = true;
    }
    }

    // ---- +0xb218  120 bytes ----
    if (mTimer23 > 0.5f) {
    if (!mFlags[7]) {
    mHandles[3] = MissionUtility::CreateObject("cis_mortar_assault_xpl", "inst_death_explosions", 3, "", 0, -1);
    mFlags[7] = true;
    }
    }

    // ---- +0xb290  120 bytes ----
    if (mTimer23 > 0.9f) {
    if (!mFlags[8]) {
    mHandles[4] = MissionUtility::CreateObject("cis_mortar_assault_xpl", "inst_death_explosions", 5, "", 0, -1);
    mFlags[8] = true;
    }
    }

    // ---- +0xb308  120 bytes ----
    if (mTimer23 > 1.4f) {
    if (!mFlags[9]) {
    mHandles[5] = MissionUtility::CreateObject("cis_mortar_assault_xpl", "inst_death_explosions", 4, "", 0, -1);
    mFlags[9] = true;
    }
    }

    // ---- +0xb380  120 bytes ----
    if (mTimer23 > 1.6f) {
    if (!mFlags[10]) {
    mHandles[6] = MissionUtility::CreateObject("cis_mortar_assault_xpl", "inst_death_explosions", 2, "", 0, -1);
    mFlags[10] = true;
    }
    }

    // ---- +0xb3f8  120 bytes ----
    if (mTimer23 > 2.0f) {
    if (!mFlags[11]) {
    mHandles[7] = MissionUtility::CreateObject("cis_mortar_assault_xpl", "inst_death_explosions", 1, "", 0, -1);
    mFlags[11] = true;
    }
    }

    // ---- +0xb470  120 bytes ----
    if (mTimer23 > 2.2f) {
    if (!mFlags[12]) {
    mHandles[8] = MissionUtility::CreateObject("cis_mortar_assault_xpl", "inst_death_explosions", 6, "", 0, -1);
    mFlags[12] = true;
    }
    }

    // ---- +0xb4e8  492 bytes ----
    if (!mFlags[179]) {
        if (MissionUtility::GetCinId(mHandles[290]) == 2) {
        mFlags[179] = true;
        MissionUtility::SetVelocMinimumFly(mHandles[599], 100.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[599], 100.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[599], 100.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[600], 100.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[600], 100.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[600], 100.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[601], 100.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[601], 100.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[601], 100.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[602], 100.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[602], 100.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[602], 100.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[605], 100.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[605], 100.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[605], 100.0f);
        MissionUtility::SetVelocMinimumFly(mHandles[606], 100.0f);
        MissionUtility::SetVelocMaximumFly(mHandles[606], 100.0f);
        MissionUtility::SetVelocNeutralFly(mHandles[606], 100.0f);
        MissionUtility::Goto(mHandles[580], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[581], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[582], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[583], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[591], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[592], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[593], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[594], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[595], "EndCinGotoPath", false);
        MissionUtility::Goto(mHandles[599], "EndCinFighterPath", false);
        MissionUtility::Goto(mHandles[600], "EndCinFighterPath", false);
        MissionUtility::Goto(mHandles[601], "EndCinFighterPath", false);
        MissionUtility::Goto(mHandles[602], "EndCinFighterPath1", false);
        MissionUtility::Goto(mHandles[605], "EndCinGunshipPath", false);
        MissionUtility::Goto(mHandles[606], "EndCinGunshipPath", false);
        }
    }

    // ---- +0xb6d4  224 bytes ----
    if (!MissionUtility::IsAlive(mHandles[271])) {
    if (!MissionUtility::IsAlive(mHandles[272])) {
    if (!MissionUtility::IsAlive(mHandles[273])) {
    if (!MissionUtility::IsAlive(mHandles[274])) {
    if (!MissionUtility::IsAlive(mHandles[275])) {
    if (!MissionUtility::IsAlive(mHandles[276])) {
    if (!MissionUtility::IsAlive(mHandles[277])) {
    if (!MissionUtility::IsAlive(mHandles[278])) {
    if (!MissionUtility::IsAlive(mHandles[279])) {
    if (!MissionUtility::IsAlive(mHandles[280])) {
    if (MissionUtility::GetGameClock() < 540.0f) {
    if (!mFlags[54]) {
    MissionUtility::BonusObjectiveComplete(mInts[16], true);
    MissionUtility::DisplayText("missions.Thule1.text.str0010", 5.0f, -1.0f);
    mFlags[54] = true;
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

    // ---- +0xb7b4  176 bytes ----
    if (mFlags[82]) {
    if (!MissionUtility::IsCinRunning(mHandles[290])) {
    if (!mFlags[2]) {
    if (MissionUtility::IsAlive(mHandles[133])
        || MissionUtility::IsAlive(mHandles[134])
        || MissionUtility::IsAlive(mHandles[135])
        || MissionUtility::IsAlive(mHandles[136])
        || MissionUtility::IsAlive(mHandles[137])
        || MissionUtility::IsAlive(mHandles[138])
        || MissionUtility::IsAlive(mHandles[139])
        || MissionUtility::IsAlive(mHandles[140])) {
    MissionUtility::BonusObjectiveFailed(mInts[15]);
    }

    // ---- +0xb864  12 bytes ----
    MissionUtility::MissionSuccess();
    mFlags[2] = true;

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

void Thule1Script::Setup()
{
        mFlags[104] = true;
        mFlags[28] = true;
        mFlags[105] = true;
        mFlags[120] = true;
        mFlags[138] = true;
        mFlags[155] = true;
        mFlags[172] = true;
        mHandles[171] = MissionUtility::GetPlayerHandle(0);
        mHandles[383] = MissionUtility::GetHandle("g1_factory1");
        mHandles[384] = MissionUtility::GetHandle("g1_factory2");
        mHandles[308] = MissionUtility::GetHandle("army1");
        mHandles[309] = MissionUtility::GetHandle("army2");
        mHandles[310] = MissionUtility::GetHandle("army3");
        mHandles[311] = MissionUtility::GetHandle("army4");
        mHandles[312] = MissionUtility::GetHandle("army5");
        mHandles[313] = MissionUtility::GetHandle("army6");
        mHandles[314] = MissionUtility::GetHandle("army7");
        mHandles[315] = MissionUtility::GetHandle("army8");
        mHandles[316] = MissionUtility::GetHandle("army9");
        mHandles[317] = MissionUtility::GetHandle("army10");
        mHandles[318] = MissionUtility::GetHandle("fighter1");
        mHandles[319] = MissionUtility::GetHandle("fighter2");
        mHandles[320] = MissionUtility::GetHandle("fighter3");
        mHandles[326] = MissionUtility::GetHandle("landing_pad");
        mHandles[335] = MissionUtility::GetHandle("lpad1");
        mHandles[336] = MissionUtility::GetHandle("lpad2");
        mHandles[323] = MissionUtility::GetHandle("landing_walk1");
        mHandles[324] = MissionUtility::GetHandle("landing_walk2");
        mHandles[325] = MissionUtility::GetHandle("landing_walk3");
        mHandles[327] = MissionUtility::GetHandle("l1");
        mHandles[328] = MissionUtility::GetHandle("l2");
        mHandles[329] = MissionUtility::GetHandle("l3");
        mHandles[330] = MissionUtility::GetHandle("l4");
        mHandles[331] = MissionUtility::GetHandle("l5");
        mHandles[332] = MissionUtility::GetHandle("l6");
        mHandles[333] = MissionUtility::GetHandle("l7");
        mHandles[334] = MissionUtility::GetHandle("l8");
        mHandles[337] = MissionUtility::GetHandle("aat1");
        mHandles[338] = MissionUtility::GetHandle("aat2");
        mHandles[339] = MissionUtility::GetHandle("aat3");
        mHandles[340] = MissionUtility::GetHandle("aat4");
        mHandles[341] = MissionUtility::GetHandle("aat5");
        mHandles[342] = MissionUtility::GetHandle("aat6");
        mHandles[343] = MissionUtility::GetHandle("aat7");
        mHandles[344] = MissionUtility::GetHandle("aat8");
        mHandles[345] = MissionUtility::GetHandle("aat9");
        mHandles[346] = MissionUtility::GetHandle("aat10");
        mHandles[347] = MissionUtility::GetHandle("aat11");
        mHandles[348] = MissionUtility::GetHandle("bigwheel1");
        mHandles[349] = MissionUtility::GetHandle("bigwheel2");
        mHandles[350] = MissionUtility::GetHandle("bigwheel3");
        mHandles[351] = MissionUtility::GetHandle("bigwheel4");
        mHandles[352] = MissionUtility::GetHandle("landing_turret1");
        mHandles[353] = MissionUtility::GetHandle("landing_turret2");
        mHandles[354] = MissionUtility::GetHandle("landing_turret3");
        mHandles[355] = MissionUtility::GetHandle("landing_turret4");
        mHandles[356] = MissionUtility::GetHandle("landing_turret5");
        mHandles[357] = MissionUtility::GetHandle("landing_turret6");
        mHandles[358] = MissionUtility::GetHandle("landing_turret7");
        mHandles[359] = MissionUtility::GetHandle("landing_turret8");
        mHandles[360] = MissionUtility::GetHandle("landing_turret9");
        mHandles[361] = MissionUtility::GetHandle("landing_turret10");
        mHandles[362] = MissionUtility::GetHandle("landing_turret11");
        mHandles[363] = MissionUtility::GetHandle("landing_turret12");
        mHandles[364] = MissionUtility::GetHandle("landing_turret13");
        mHandles[365] = MissionUtility::GetHandle("landing_turret14");
        mHandles[366] = MissionUtility::GetHandle("landing_turret15");
        mHandles[367] = MissionUtility::GetHandle("landing_turret16");
        mHandles[368] = MissionUtility::GetHandle("landing_turret17");
        mHandles[369] = MissionUtility::GetHandle("landing_turret18");
        mHandles[370] = MissionUtility::GetHandle("landing_turret19");
        mHandles[390] = MissionUtility::GetHandle("g1_aat1");
        mHandles[391] = MissionUtility::GetHandle("g1_aat2");
        mHandles[392] = MissionUtility::GetHandle("g1_aat3");
        mHandles[393] = MissionUtility::GetHandle("g1_aat4");
        mHandles[394] = MissionUtility::GetHandle("g1_aat5");
        mHandles[395] = MissionUtility::GetHandle("g1_aat6");
        mHandles[396] = MissionUtility::GetHandle("g1_t1");
        mHandles[397] = MissionUtility::GetHandle("g1_t2");
        mHandles[398] = MissionUtility::GetHandle("g1_t3");
        mHandles[399] = MissionUtility::GetHandle("g1_t4");
        mHandles[400] = MissionUtility::GetHandle("g1_t5");
        mHandles[401] = MissionUtility::GetHandle("g1_t6");
        mHandles[141] = MissionUtility::GetHandle("g1_land");
        mHandles[387] = MissionUtility::GetHandle("generator1");
        MissionUtility::SetMaxHealth(mHandles[387], 999999.0f);
        MissionUtility::SetCurHealth(mHandles[387], 999999.0f);
        mHandles[383] = MissionUtility::GetHandle("g1_factory1");
        mHandles[384] = MissionUtility::GetHandle("g1_factory2");
        mHandles[142] = MissionUtility::GetHandle("g2_land");
        mHandles[411] = MissionUtility::GetHandle("generator2");
        mHandles[426] = MissionUtility::GetHandle("g2_aat1");
        mHandles[427] = MissionUtility::GetHandle("g2_aat2");
        mHandles[428] = MissionUtility::GetHandle("g2_aat3");
        mHandles[429] = MissionUtility::GetHandle("g2_aat4");
        mHandles[430] = MissionUtility::GetHandle("g2_aat5");
        mHandles[431] = MissionUtility::GetHandle("g2_aat6");
        mHandles[432] = MissionUtility::GetHandle("g2_aat7");
        mHandles[433] = MissionUtility::GetHandle("g2_t1");
        mHandles[434] = MissionUtility::GetHandle("g2_t2");
        mHandles[435] = MissionUtility::GetHandle("g2_t3");
        mHandles[436] = MissionUtility::GetHandle("g2_t4");
        mHandles[437] = MissionUtility::GetHandle("g2_t5");
        mHandles[438] = MissionUtility::GetHandle("g2_t6");
        mHandles[439] = MissionUtility::GetHandle("g2_t7");
        mHandles[440] = MissionUtility::GetHandle("g2_t8");
        mHandles[262] = MissionUtility::GetHandle("base1");
        mHandles[263] = MissionUtility::GetHandle("base2");
        mHandles[264] = MissionUtility::GetHandle("base3");
        mHandles[265] = MissionUtility::GetHandle("base4");
        mHandles[266] = MissionUtility::GetHandle("base5");
        mHandles[267] = MissionUtility::GetHandle("base6");
        mHandles[268] = MissionUtility::GetHandle("base7");
        mHandles[269] = MissionUtility::GetHandle("base8");
        mHandles[270] = MissionUtility::GetHandle("base");
        mHandles[0] = MissionUtility::GetHandle("test1");
        mHandles[1] = MissionUtility::GetHandle("test2");
        mHandles[133] = MissionUtility::GetHandle("factory1");
        mHandles[134] = MissionUtility::GetHandle("factory2");
        mHandles[135] = MissionUtility::GetHandle("factory3");
        mHandles[136] = MissionUtility::GetHandle("factory4");
        mHandles[137] = MissionUtility::GetHandle("factory5");
        mHandles[138] = MissionUtility::GetHandle("factory6");
        mHandles[139] = MissionUtility::GetHandle("factory7");
        mHandles[140] = MissionUtility::GetHandle("factory8");
        mHandles[271] = MissionUtility::GetHandle("p1");
        mHandles[272] = MissionUtility::GetHandle("p2");
        mHandles[273] = MissionUtility::GetHandle("p3");
        mHandles[274] = MissionUtility::GetHandle("p4");
        mHandles[275] = MissionUtility::GetHandle("p5");
        mHandles[276] = MissionUtility::GetHandle("p6");
        mHandles[277] = MissionUtility::GetHandle("p7");
        mHandles[278] = MissionUtility::GetHandle("p8");
        mHandles[279] = MissionUtility::GetHandle("p9");
        mHandles[280] = MissionUtility::GetHandle("p10");
        mHandles[9] = MissionUtility::GetHandle("arm1");
        mHandles[10] = MissionUtility::GetHandle("arm2");
        mHandles[11] = MissionUtility::GetHandle("arm3");
        mHandles[12] = MissionUtility::GetHandle("arm4");
        mHandles[13] = MissionUtility::GetHandle("arm5");
        mHandles[14] = MissionUtility::GetHandle("arm6");
        mHandles[15] = MissionUtility::GetHandle("arm7");
        mHandles[16] = MissionUtility::GetHandle("arm8");
        mHandles[17] = MissionUtility::GetHandle("arm9");
        mHandles[18] = MissionUtility::GetHandle("arm10");
        MissionUtility::PreloadConfig("rep_walk_sixleg");
        MissionUtility::PreloadConfig("rep_tank_fighter1");
        MissionUtility::PreloadConfig("rep_fly_assault");
        MissionUtility::PreloadConfig("cis_tank_assault");
        MissionUtility::PreloadConfig("rep_inf_clone");
        MissionUtility::PreloadConfig("cis_tank_fighter");
        MissionUtility::PreloadConfig("rep_fly_gunship");
        MissionUtility::PreloadConfig("rep_blaster_sixleg_weak_ord");
        MissionUtility::PreloadConfig("rep_blaster_walk_weak_ord");
        MissionUtility::PreloadConfig("rep_blaster_fighter1_weak_ord");
        MissionUtility::PreloadConfig("efarm_explo");
        MissionUtility::PreloadConfig("smoke_bigblack");
        MissionUtility::PreloadConfig("rep_fly_fighter");
        MissionUtility::PreloadConfig("cis_cannon_assault_weak_ord");
        MissionUtility::PreloadConfig("cis_blaster_assault_weak_ord");
        MissionUtility::PreloadConfig("cis_beam_walk_weak_ord");
        MissionUtility::PreloadConfig("cis_blaster_fighter_weak_ord");
        MissionUtility::PreloadConfig("cis_fly_fighter");
        MissionUtility::PreloadConfig("cis_missile_wheeled_weak_ord");
}

SPMission *Thule1BuildMission()
{
    return new Thule1Script();
}
