// Kashyyyk2Script.cpp -- reconstruction of a shipped mission script. 42,776 bytes, byte-exact.
//
// Five functions, including `Matrix::Matrix(const Matrix&)`, which the shipped .text places
// between `Execute` and `Setup`. Hand corrections (analysis/mission_batch_b.md):
//
//   * that copy constructor is out of line because it is DEFINED after `Execute`: a
//     single-pass compiler cannot inline a body it has not read. Left implicit it is inlined
//     at all three call sites. Its body is the implicit `operator=`, which expands to exactly
//     the compiler's own 64-byte copy loop -- 9 words plus the `blr`;
//   * three named `Matrix` locals copy-initialised from `GetLocation`. The frame says the
//     named locals (0xcd8, 0xc98, 0xc58) sit above the three hidden sret buffers
//     (0xc18, 0xbd8, 0xb98) rather than interleaved with them;
//   * six "all nests cleared" chains test ONE FLAG TWICE -- `lbz` once, `clrlwi.` rather than
//     `cmplwi`, and two `beq` to the same target -- while the flag after it is never tested.
//     The seventh nest group has no such pair, so it is a slip in the mission source;
//   * the singly-registered block at +0x12c is a float (the player's health across the jedi
//     swap), and the two region sweeps are loops sharing one counter.

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

class Matrix
{
public:
    Matrix(const Matrix &o);

    float m[4][4];
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
    int    AddHealthBox(const char*, int, float);
    int    AddObjective(const char*);
    void   AddTurnAroundRegion(const char*, const char*, const char*, const char*, const char*, const char*);
    void   AttackTarget(int, int, bool, bool, bool, bool);
    void   BeginWave(const char*);
    void   BonusObjectiveComplete(int, bool);
    void   BonusObjectiveFailed(int);
    int    CreateFlock();
    int    CreateFlock(int, Formation);
    int    CreateObject(const char*, const Matrix&, const char*, int, int, int);
    int    CreateObject(const char*, const Vector&, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObject(const char*, const char*, int, const char*, int, int, Quat facing = Quat(), int c = -1);
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    int    CreateRegionList(const char*, bool, bool);
    void   DamageObject(int, float, float);
    void   Defend(int, int, float);
    void   DisplayText(const char*, float, float);
    void   ExcludeObject(int, int);
    void   FlushSoundQueue();
    int    GetCinId(int);
    float  GetCurHealth(int);
    float  GetDistance(int, const char*);
    float  GetDistance(int, const char*, int);
    float  GetDistance(int, int);
    int    GetFlockCount(int);
    float  GetGameClock();
    int    GetHandle(const char*);
    Matrix GetLocation(int);
    int    GetPlayerHandle(int);
    int    GetPlayerShotCount();
    int    GetRegionNewMember(int, int);
    int    GetRegionNewMemberCount(int);
    int    GetWhoShotMe(int);
    void   Goto(int, const char*, bool);
    void   Goto(int, const char*, int);
    void   GotoDirect(int, const char*, bool);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsFlockAlive(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsWaveSpawned(const char*);
    int    MidMissionGetSavePoint();
    void   MidMissionLoad(float&);
    void   MidMissionLoadPlayer();
    void   MidMissionSave(float);
    void   MidMissionSavePlayer(int);
    void   MissionFailure();
    void   MissionSuccess();
    void   MoveObject(int, const char*, int, bool);
    void   MoveObjectWithRotation(int, const char*, int, bool);
    void   Objectify(const char*, int, const char*, bool, bool, float, float);
    void   Objectify(int, const char*, bool, bool, float);
    void   ObjectiveComplete(int);
    void   Patrol(int, const char*, float, bool);
    void   PlayMusic(const char*, bool);
    void   PreloadConfig(const char*);
    int    QueueSound(const char*, float, float, float, const char*, int, const char*);
    void   RemoveFlock(int, bool);
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
    void   SetEnemies(int, int);
    void   SetEnemiesOneWay(int, int);
    void   SetMapZoom(float, float);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetQueueFlag(bool);
    void   SetTeamNum(int, int);
    void   SetVelocForward(int, float);
    void   SetVisible(int, bool);
    void   SetWeaponOrd(int, const char*, const char*);
    void   StartAmbiences(const char*, const char*, float, float);
    int    StartSound(const char*, bool, float, float, float, const char*, int, const char*);
    void   Stop(int);
    void   StopMusic(int);
    void   Wait(int);
}

static const char *const kClassName = "Kashyyyk2Script";
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

class Kashyyyk2Script : public SPMission
{
public:
    virtual ~Kashyyyk2Script();
    Kashyyyk2Script()
    {
        mBoolCount = 255;   mBools = mFlags;
        mCountB    = 1;     mIntsB = (int *)&mSavedHealth;
        mCountC    = 391;   mIntsC = mHandles;
        mCountD    = 15;    mBlockD = mInts;
    }

    virtual void Setup();
    virtual void Execute();

    char   mPad24[1];
    bool   mFlags[255];            // +0x025  the one-shot latches
    char   mPad124[8];                  // +0x124
    float  mSavedHealth;            // +0x12c
    char   mPad130[4];                  // +0x130
    Timer  mTimer0;                      // +0x134
    Timer  mTimer1;                      // +0x140
    Timer  mTimer2;                      // +0x14c
    Timer  mTimer3;                      // +0x158
    Timer  mTimer4;                      // +0x164
    Timer  mTimer5;                      // +0x170
    Timer  mTimer6;                      // +0x17c
    Timer  mTimer7;                      // +0x188
    Timer  mTimer8;                      // +0x194
    Timer  mTimer9;                      // +0x1a0
    Timer  mTimer10;                      // +0x1ac
    Timer  mTimer11;                      // +0x1b8
    Timer  mTimer12;                      // +0x1c4
    Timer  mTimer13;                      // +0x1d0
    Timer  mTimer14;                      // +0x1dc
    Timer  mTimer15;                      // +0x1e8
    Timer  mTimer16;                      // +0x1f4
    Timer  mTimer17;                      // +0x200
    Timer  mTimer18;                      // +0x20c
    Timer  mTimer19;                      // +0x218
    Timer  mTimer20;                      // +0x224
    Timer  mTimer21;                      // +0x230
    Timer  mTimer22;                      // +0x23c
    Timer  mTimer23;                      // +0x248
    Timer  mTimer24;                      // +0x254
    Timer  mTimer25;                      // +0x260
    Timer  mTimer26;                      // +0x26c
    Timer  mTimer27;                      // +0x278
    Timer  mTimer28;                      // +0x284
    Timer  mTimer29;                      // +0x290
    Timer  mTimer30;                      // +0x29c
    Timer  mTimer31;                      // +0x2a8
    Timer  mTimer32;                      // +0x2b4
    char   mPad2C0[4];                  // +0x2c0
    int    mHandles[391];          // +0x2c4
    char   mPad8E0[8];                  // +0x8e0
    int    mInts[15];             // +0x8e8
    int    mInts2[11];              // +0x924
};

Kashyyyk2Script::~Kashyyyk2Script()
{
}

void Kashyyyk2Script::Execute()
{
    int i;

    // ---- +0x000c  784 bytes ----
    if (mFlags[21]) {
    MissionUtility::SetAlliance(1, 20);
    MissionUtility::SetMapZoom(500.0f, 1e+09f);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[225]);
    MissionUtility::SetMusicLooping(true);
    MissionUtility::PlayMusic("EP2_V1_T09_03", true);
    MissionUtility::AddTurnAroundRegion("pre_canyon_turnaround", "pre_canyon_turnaround_focus", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("post_canyon_turnaround", "post_canyon_turnaround_focus", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("turnaround1", "turnaround1_focus", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("turnaround2", "turnaround2_focus", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("turnaround3", "aat_start", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("oops", "oops_focus", 0, 0, 0, 0);
    MissionUtility::StartAmbiences("AmbKshyk_forestrain01_pl2", "PropGen_thunderHvy01", 10.0f, 30.0f);
    mInts2[10] = MissionUtility::MidMissionGetSavePoint();
    MissionUtility::SetCurHealth(mHandles[18], 99999.0f);
    MissionUtility::SetCurHealth(mHandles[188], 75.0f);
    MissionUtility::SetCurHealth(mHandles[189], 75.0f);
    MissionUtility::SetCurHealth(mHandles[191], 75.0f);
    MissionUtility::SetCurHealth(mHandles[192], 75.0f);
    MissionUtility::SetCurHealth(mHandles[193], 75.0f);
    MissionUtility::SetCurHealth(mHandles[194], 75.0f);
    MissionUtility::Patrol(mHandles[203], "sbdroid_patrol", 100.0f, false);
    MissionUtility::SetVelocForward(mHandles[18], 40.0f);
    MissionUtility::SetVelocForward(mHandles[201], 0.0f);
    MissionUtility::SetEnemiesOneWay(1, 4);
    MissionUtility::SetEnemiesOneWay(1, 5);
    MissionUtility::SetEnemiesOneWay(1, 6);
    MissionUtility::SetEnemiesOneWay(1, 14);
    MissionUtility::SetEnemiesOneWay(1, 15);
    MissionUtility::SetEnemies(2, 3);
    mHandles[157] = MissionUtility::CreateFlock();
    mHandles[72] = MissionUtility::CreateFlock(mHandles[225], (Formation)0);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[226]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[227]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[228]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[229]);
    MissionUtility::SetCurHealth(mHandles[75], 999999.0f);
    mHandles[24] = MissionUtility::CreateFlock();
    MissionUtility::AddFlockMember(mHandles[24], mHandles[201]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[198]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[200]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[199]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[19]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[20]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[21]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[22]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[23]);
    MissionUtility::AddFlockMember(mHandles[24], mHandles[203]);
    mHandles[10] = MissionUtility::CreateRegionList("gnasp_switch", true, false);
    mHandles[7] = MissionUtility::CreateRegionList("unitsnearcomm", true, false);
    mInts[10] = MissionUtility::AddBonusObjective("missions.Kashyyyk2.bonus.str0005");
    mInts[12] = MissionUtility::AddBonusObjective("missions.Kashyyyk2.bonus.str0006");
    mInts[11] = MissionUtility::AddBonusObjective("missions.Kashyyyk2.bonus.str0003");
    mFlags[21] = false;
    }

    // ---- +0x031c  44 bytes ----
    if (MissionUtility::GetGameClock() > 510.0f) {
    if (!mFlags[3]) {
    MissionUtility::BonusObjectiveFailed(mInts[11]);
    mFlags[3] = true;
    }
    }

    // ---- +0x0348  96 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[12]) < 30.0f) {
    if (!mFlags[7]) {
    MissionUtility::StartSound("Ifc_LowShield01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0009", 6.0f, -1.0f);
    mFlags[7] = true;
    }
    }

    // ---- +0x03a8  96 bytes ----
    if (MissionUtility::GetDistance(mHandles[202], mHandles[12]) < 30.0f) {
    if (!mFlags[7]) {
    MissionUtility::StartSound("Ifc_LowShield01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0009", 6.0f, -1.0f);
    mFlags[7] = true;
    }
    }

    // ---- +0x0408  96 bytes ----
    if (MissionUtility::GetDistance(mHandles[202], mHandles[13]) < 30.0f) {
    if (!mFlags[8]) {
    MissionUtility::StartSound("Ifc_LowShield01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0009", 6.0f, -1.0f);
    mFlags[8] = true;
    }
    }

    // ---- +0x0468  96 bytes ----
    if (MissionUtility::GetDistance(mHandles[16], mHandles[14]) < 20.0f) {
    if (!mFlags[9]) {
    MissionUtility::StartSound("Ifc_LowShield01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0009", 6.0f, -1.0f);
    mFlags[9] = true;
    }
    }

    // ---- +0x04c8  68 bytes ----
    if (mFlags[7] && mFlags[8] && mFlags[9] && !mFlags[20]) {
        MissionUtility::BonusObjectiveComplete(mInts[10], true);
        mFlags[20] = true;
    }

    // ---- +0x050c  72 bytes ----
    MissionUtility::ExcludeObject(mHandles[10], MissionUtility::GetHandle("player_maru"));
    MissionUtility::ExcludeObject(mHandles[10], MissionUtility::GetHandle("bera_maru"));
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[10]); i++)
        MissionUtility::SetTeamNum(MissionUtility::GetRegionNewMember(mHandles[10], i), 2);

    // ---- +0x0554  56 bytes ----
    for (i = 0; i < MissionUtility::GetRegionNewMemberCount(mHandles[7]); i++) {
        int m = MissionUtility::GetRegionNewMember(mHandles[7], i);
        MissionUtility::AddFlockMember(mHandles[6], m);
    }

    // ---- +0x058c  104 bytes ----
    if (!mFlags[40]) {
    if (mTimer30 > 3.0f) {
    MissionUtility::StartSound("maru_idle_01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    StopTimer(mTimer30);
    mTimer30 = 0.0f;
    }
    }

    // ---- +0x05f4  152 bytes ----
    if (!mFlags[40] && !mFlags[247]) {
        if (MissionUtility::GetCinId(mHandles[76]) == 2) {
        mFlags[247] = true;
        Matrix maruLoc = MissionUtility::GetLocation(mHandles[377]);
        MissionUtility::RemoveObject(mHandles[377]);
        MissionUtility::RemoveObject(mHandles[374]);
        MissionUtility::QueueSound("BKK13_24", 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[375] = MissionUtility::CreateObject("kas_crea_maru_player", maruLoc, "OpenCinAnakinMaru", 0, -1, -1);
        }
    }

    // ---- +0x068c  112 bytes ----
    if (!mFlags[40] && !mFlags[248]) {
        if (MissionUtility::GetCinId(mHandles[76]) == 3) {
        mFlags[248] = true;
        MissionUtility::SetApplyDynamics(mHandles[375], true);
        MissionUtility::SetAnimation(mHandles[375], "mount", 1.0f, 1);
        MissionUtility::QueueSound("bkk13_25", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
    }

    // ---- +0x06fc  60 bytes ----
    if (!mFlags[40] && !mFlags[249] && mFlags[46]) {
        if (!MissionUtility::IsCinRunning(mHandles[76])) {
        mFlags[249] = true;
        }
    }

    // ---- +0x0738  80 bytes ----
    mInts[14] = MissionUtility::GetPlayerShotCount();
    if (MissionUtility::GetDistance(mHandles[15], mHandles[18]) > 200.0f) {
    if (MissionUtility::GetDistance(mHandles[15], "aat_comm_gate") > MissionUtility::GetDistance(mHandles[18], "aat_comm_gate")) {
    MissionUtility::SetVelocForward(mHandles[18], 0.0f);
    }
    }

    // ---- +0x0788  36 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[18]) < 200.0f) {
    MissionUtility::SetVelocForward(mHandles[18], 40.0f);
    }

    // ---- +0x07ac  84 bytes ----
    if (!MissionUtility::IsAlive(mHandles[230])) {
    if (!MissionUtility::IsAlive(mHandles[246])) {
    if (!MissionUtility::IsAlive(mHandles[262])) {
    if (!MissionUtility::IsAlive(mHandles[278])) {
    if (!mFlags[92]) {
    mFlags[92] = true;
    }
    }
    }
    }
    }

    // ---- +0x0800  40 bytes ----
    if (mFlags[34]) {
        if (mTimer1 > 102.0f) {
        BeginTimer(mTimer1);
        }
    }

    switch (mInts2[10]) {
    default:
    // ---- +0x0838  24 bytes ----

        break;
    case 0:
    // ---- +0x0850  348 bytes ----
    if (mFlags[76]) {
        MissionUtility::SetEnemies(20, 2);
        MissionUtility::SetCurHealth(mHandles[202], 9999999.0f);
        mHandles[374] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin_cin", "OpenCinAnakinPath", 0, "OpenCinAnakin", 0, -1, -1);
        mHandles[378] = MissionUtility::CreateObjectWithRotation("kas_crea_maru_bera", "OpenCinBeraPath", 0, "OpenCinBera", 0, -1, -1);
        mHandles[376] = MissionUtility::CreateObjectWithRotation("kas_crea_maru_wookiee", "OpenCinWookiePath", 0, "OpenCinWookie", 0, -1, -1);
        mHandles[377] = MissionUtility::CreateObjectWithRotation("kas_crea_maru", "OpenCinMaruPath", 0, "OpenCinMaru", 0, -1, -1);
        MissionUtility::Goto(mHandles[377], "OpenCinMaruPath", false);
        MissionUtility::Goto(mHandles[376], "OpenCinWookiePath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[376], "OpenCinWookiePath1", false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::SetCollidable(mHandles[374], false);
        MissionUtility::SetCollidable(mHandles[375], false);
        MissionUtility::SetCollidable(mHandles[378], false);
        MissionUtility::SetCollidable(mHandles[376], false);
        MissionUtility::SetCollidable(mHandles[377], false);
        mHandles[76] = MissionUtility::RunCin("Cin1", true, true);
        mFlags[46] = true;
        BeginTimer(mTimer30);
        mFlags[76] = false;
    }

    // ---- +0x09ac  48 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[18], "turnaround1_off")) {
    if (!mFlags[11]) {
    MissionUtility::RemoveTurnAroundRegion("turnaround1");
    mFlags[11] = true;
    }
    }

    // ---- +0x09dc  68 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[15], "turnaround1_switch")) {
    if (!mFlags[12]) {
    MissionUtility::AddTurnAroundRegion("turnaround1", "turnaround1_focus2", 0, 0, 0, 0);
    mFlags[12] = true;
    }
    }

    // ---- +0x0a20  48 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[18], "turnaround2_off")) {
    if (!mFlags[13]) {
    MissionUtility::RemoveTurnAroundRegion("turnaround2");
    mFlags[13] = true;
    }
    }

    // ---- +0x0a50  56 bytes ----
    if (!mFlags[30]) {
        if (!MissionUtility::IsAlive(mHandles[15])) {
        mHandles[85] = MissionUtility::RunCin("deathcin", true, false);
        mFlags[30] = true;
        }
    }

    // ---- +0x0a88  52 bytes ----
    if (mFlags[30]) {
        if (!MissionUtility::IsCinRunning(mHandles[85])) {
        if (!mFlags[1]) {
        MissionUtility::MissionFailure();
        mFlags[1] = true;
        }
        }
    }

    // ---- +0x0abc  224 bytes ----
    if (mFlags[46]) {
        if (!MissionUtility::IsCinRunning(mHandles[76])) {
        if (!mFlags[40]) {
        MissionUtility::MoveObjectWithRotation(mHandles[15], "forward_start", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[18], "forward_start", 1, true);
        MissionUtility::PlayMusic("EP6_V2_T05_02", true);
        mFlags[34] = true;
        MissionUtility::Objectify(mHandles[18], "missions.Kashyyyk2.marker.str0000", true, true, 150.0f);
        MissionUtility::Patrol(mHandles[18], "bera_patrol_path", 400.0f, false);
        MissionUtility::FlushSoundQueue();
        MissionUtility::RemoveObject(mHandles[376]);
        MissionUtility::RemoveObject(mHandles[375]);
        MissionUtility::RemoveObject(mHandles[378]);
        BeginTimer(mTimer22);
        mInts[0] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0006");
        MissionUtility::DisplayText("missions.Kashyyyk2.objective.str0006", 5.0f, -1.0f);
        BeginTimer(mTimer20);
        mFlags[40] = true;
        }
        }
    }

    // ---- +0x0b9c  104 bytes ----
    if (MissionUtility::GetDistance(mHandles[18], mHandles[4]) < 150.0f) {
    if (!mFlags[4]) {
    MissionUtility::SetTeamNum(mHandles[1], 2);
    MissionUtility::SetTeamNum(mHandles[2], 2);
    MissionUtility::SetTeamNum(mHandles[3], 2);
    MissionUtility::SetTeamNum(mHandles[4], 2);
    MissionUtility::SetTeamNum(mHandles[5], 2);
    mFlags[4] = true;
    }
    }

    // ---- +0x0c04  132 bytes ----
    if (!MissionUtility::IsAlive(mHandles[1])) {
    if (!MissionUtility::IsAlive(mHandles[2])) {
    if (!MissionUtility::IsAlive(mHandles[3])) {
    if (!MissionUtility::IsAlive(mHandles[4])) {
    if (!MissionUtility::IsAlive(mHandles[5])) {
    if (!mFlags[5]) {
    MissionUtility::QueueSound("BKK13_06", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[5] = true;
    }
    }
    }
    }
    }
    }

    // ---- +0x0c88  260 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[188]) < 200.0f) {
    if (!mFlags[63]) {
    MissionUtility::SetEnemies(1, 5);
    MissionUtility::SetEnemies(20, 5);
    BeginTimer(mTimer5);
    MissionUtility::AttackTarget(mHandles[188], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[189], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[190], mHandles[15], true, true, false, false);
    MissionUtility::StartSound("BKK13_11", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0005", 6.0f, -1.0f);
    MissionUtility::Objectify(mHandles[188], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[189], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    mFlags[63] = true;
    }
    }

    // ---- +0x0d8c  120 bytes ----
    if (mTimer5 > 7.0f) {
    if (!mFlags[67]) {
    if (!MissionUtility::IsAlive(mHandles[188])) {
    MissionUtility::IsAlive(mHandles[189]);
    }
    MissionUtility::SetVelocForward(mHandles[188], 75.0f);
    MissionUtility::SetVelocForward(mHandles[189], 75.0f);
    MissionUtility::GotoDirect(mHandles[188], "patrol2_retreat", true);
    MissionUtility::GotoDirect(mHandles[189], "patrol2_retreat", true);
    mFlags[67] = true;
    }
    }

    // ---- +0x0e04  336 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[191]) < 200.0f) {
    if (!mFlags[64]) {
    MissionUtility::SetEnemies(1, 6);
    MissionUtility::SetEnemies(20, 6);
    BeginTimer(mTimer6);
    MissionUtility::AttackTarget(mHandles[191], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[192], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[193], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[194], mHandles[15], true, true, false, false);
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0005", 6.0f, -1.0f);
    MissionUtility::StartSound("BKK13_11", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::Objectify(mHandles[191], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[192], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[193], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[194], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    mFlags[64] = true;
    }
    }

    // ---- +0x0f54  332 bytes ----
    if (mHandles[15] == MissionUtility::GetWhoShotMe(mHandles[191])) {
    if (!mFlags[64]) {
    MissionUtility::SetEnemies(1, 6);
    MissionUtility::SetEnemies(20, 6);
    BeginTimer(mTimer6);
    MissionUtility::AttackTarget(mHandles[191], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[192], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[193], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[194], mHandles[15], true, true, false, false);
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0005", 6.0f, -1.0f);
    MissionUtility::StartSound("BKK13_11", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::Objectify(mHandles[191], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[192], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[193], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[194], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    mFlags[64] = true;
    }
    }

    // ---- +0x10a0  332 bytes ----
    if (mHandles[15] == MissionUtility::GetWhoShotMe(mHandles[192])) {
    if (!mFlags[64]) {
    MissionUtility::SetEnemies(1, 6);
    MissionUtility::SetEnemies(20, 6);
    BeginTimer(mTimer6);
    MissionUtility::AttackTarget(mHandles[191], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[192], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[193], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[194], mHandles[15], true, true, false, false);
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0005", 6.0f, -1.0f);
    MissionUtility::StartSound("BKK13_11", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::Objectify(mHandles[191], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[192], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[193], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[194], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    mFlags[64] = true;
    }
    }

    // ---- +0x11ec  332 bytes ----
    if (mHandles[15] == MissionUtility::GetWhoShotMe(mHandles[193])) {
    if (!mFlags[64]) {
    MissionUtility::SetEnemies(1, 6);
    MissionUtility::SetEnemies(20, 6);
    BeginTimer(mTimer6);
    MissionUtility::AttackTarget(mHandles[191], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[192], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[193], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[194], mHandles[15], true, true, false, false);
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0005", 6.0f, -1.0f);
    MissionUtility::StartSound("BKK13_11", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::Objectify(mHandles[191], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[192], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[193], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[194], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    mFlags[64] = true;
    }
    }

    // ---- +0x1338  332 bytes ----
    if (mHandles[15] == MissionUtility::GetWhoShotMe(mHandles[194])) {
    if (!mFlags[64]) {
    MissionUtility::SetEnemies(1, 6);
    MissionUtility::SetEnemies(20, 6);
    BeginTimer(mTimer6);
    MissionUtility::AttackTarget(mHandles[191], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[192], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[193], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[194], mHandles[15], true, true, false, false);
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0005", 6.0f, -1.0f);
    MissionUtility::StartSound("BKK13_11", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::Objectify(mHandles[191], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[192], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[193], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[194], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    mFlags[64] = true;
    }
    }

    // ---- +0x1484  208 bytes ----
    if (mTimer6 > 7.0f) {
    if (!mFlags[68]) {
    if (!MissionUtility::IsAlive(mHandles[191])) {
    if (!MissionUtility::IsAlive(mHandles[192])) {
    if (!MissionUtility::IsAlive(mHandles[193])) {
    MissionUtility::IsAlive(mHandles[194]);
    }
    }
    }
    MissionUtility::SetVelocForward(mHandles[191], 75.0f);
    MissionUtility::SetVelocForward(mHandles[192], 75.0f);
    MissionUtility::SetVelocForward(mHandles[193], 75.0f);
    MissionUtility::SetVelocForward(mHandles[194], 75.0f);
    MissionUtility::GotoDirect(mHandles[191], "patrol3_retreat", true);
    MissionUtility::GotoDirect(mHandles[192], "patrol3_retreat", true);
    MissionUtility::GotoDirect(mHandles[193], "patrol3_retreat", true);
    MissionUtility::GotoDirect(mHandles[194], "patrol3_retreat", true);
    mFlags[68] = true;
    }
    }

    // ---- +0x1554  132 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[195]) < 200.0f) {
    if (!mFlags[65]) {
    MissionUtility::SetEnemies(1, 7);
    MissionUtility::SetEnemies(20, 7);
    BeginTimer(mTimer7);
    MissionUtility::AttackTarget(mHandles[195], mHandles[15], true, true, false, false);
    MissionUtility::AttackTarget(mHandles[196], mHandles[15], true, true, false, false);
    mFlags[65] = true;
    }
    }

    // ---- +0x15d8  228 bytes ----
    if (mTimer7 > 7.0f) {
    if (!mFlags[69]) {
    if (MissionUtility::IsAlive(mHandles[195])
        || MissionUtility::IsAlive(mHandles[196])) {
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0005", 6.0f, -1.0f);
    MissionUtility::StartSound("BKK13_11", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    }
    MissionUtility::Objectify(mHandles[195], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::Objectify(mHandles[196], "missions.Kashyyyk2.marker.str0004", false, true, 0.0f);
    MissionUtility::SetVelocForward(mHandles[195], 75.0f);
    MissionUtility::SetVelocForward(mHandles[196], 75.0f);
    MissionUtility::GotoDirect(mHandles[195], "patrol4_runaway", true);
    MissionUtility::GotoDirect(mHandles[196], "patrol4_runaway", true);
    mFlags[69] = true;
    }
    }

    // ---- +0x16bc  208 bytes ----
    if (mFlags[63] && !mFlags[44]) {
        if (MissionUtility::IsAlive(mHandles[188])) {
        if (MissionUtility::GetDistance(mHandles[15], mHandles[188]) > 400.0f) {
        BeginTimer(mTimer17);
        MissionUtility::StartSound("BKK13_28", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[44] = true;
        }
        }
        if (MissionUtility::IsAlive(mHandles[189])) {
        if (MissionUtility::GetDistance(mHandles[15], mHandles[189]) > 400.0f) {
        BeginTimer(mTimer17);
        MissionUtility::StartSound("BKK13_28", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[44] = true;
        }
        }
    }

    // ---- +0x178c  392 bytes ----
    if (mFlags[64] && !mFlags[44]) {
        if (MissionUtility::IsAlive(mHandles[191])) {
        if (MissionUtility::GetDistance(mHandles[15], mHandles[191]) > 400.0f) {
        BeginTimer(mTimer17);
        MissionUtility::StartSound("BKK13_28", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[44] = true;
        }
        }
        if (MissionUtility::IsAlive(mHandles[192])) {
        if (MissionUtility::GetDistance(mHandles[15], mHandles[192]) > 400.0f) {
        BeginTimer(mTimer17);
        MissionUtility::StartSound("BKK13_28", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[44] = true;
        }
        }
        if (MissionUtility::IsAlive(mHandles[193])) {
        if (MissionUtility::GetDistance(mHandles[15], mHandles[193]) > 400.0f) {
        BeginTimer(mTimer17);
        MissionUtility::StartSound("BKK13_28", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[44] = true;
        }
        }
        if (MissionUtility::IsAlive(mHandles[194])) {
        if (MissionUtility::GetDistance(mHandles[15], mHandles[194]) > 400.0f) {
        BeginTimer(mTimer17);
        MissionUtility::StartSound("BKK13_28", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[44] = true;
        }
        }
    }

    // ---- +0x1914  44 bytes ----
    if (mTimer17 > 5.0f) {
    if (!mFlags[1]) {
    MissionUtility::MissionFailure();
    mFlags[1] = true;
    }
    }

    // ---- +0x1940  68 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[15], "nest3_trigger")) {
    if (!mFlags[127]) {
    if (!mFlags[128]) {
    BeginTimer(mTimer3);
    BeginTimer(mTimer10);
    mFlags[128] = true;
    }
    }
    }

    // ---- +0x1984  136 bytes ----
    if (mTimer10 > 0.5f) {
    if (!mFlags[129]) {
    mHandles[263] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[263], mHandles[262], 300.0f);
    mFlags[129] = true;
    }
    }

    // ---- +0x1a0c  136 bytes ----
    if (mTimer10 > 1.0f) {
    if (!mFlags[130]) {
    mHandles[264] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[264], mHandles[262], 300.0f);
    mFlags[130] = true;
    }
    }

    // ---- +0x1a94  136 bytes ----
    if (mTimer10 > 1.5f) {
    if (!mFlags[131]) {
    mHandles[265] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[265], mHandles[262], 300.0f);
    mFlags[131] = true;
    }
    }

    // ---- +0x1b1c  136 bytes ----
    if (mTimer10 > 2.0f) {
    if (!mFlags[132]) {
    mHandles[266] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[266], mHandles[262], 300.0f);
    mFlags[132] = true;
    }
    }

    // ---- +0x1ba4  148 bytes ----
    if (mTimer10 > 2.5f) {
    if (!mFlags[133]) {
    mHandles[267] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[267], mHandles[17], true, true, false, false);
    mFlags[133] = true;
    }
    }

    // ---- +0x1c38  136 bytes ----
    if (mTimer10 > 3.0f) {
    if (!mFlags[134]) {
    mHandles[268] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[268], mHandles[262], 300.0f);
    mFlags[134] = true;
    }
    }

    // ---- +0x1cc0  136 bytes ----
    if (mTimer10 > 3.5f) {
    if (!mFlags[135]) {
    mHandles[269] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[269], mHandles[262], 300.0f);
    mFlags[135] = true;
    }
    }

    // ---- +0x1d48  136 bytes ----
    if (mTimer10 > 4.0f) {
    if (!mFlags[136]) {
    mHandles[270] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[270], mHandles[262], 300.0f);
    mFlags[136] = true;
    }
    }

    // ---- +0x1dd0  148 bytes ----
    if (mTimer10 > 4.5f) {
    if (!mFlags[137]) {
    mHandles[271] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[271], mHandles[17], true, true, false, false);
    mFlags[137] = true;
    }
    }

    // ---- +0x1e64  136 bytes ----
    if (mTimer10 > 5.0f) {
    if (!mFlags[138]) {
    mHandles[272] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[272], mHandles[262], 300.0f);
    mFlags[138] = true;
    }
    }

    // ---- +0x1eec  136 bytes ----
    if (mTimer10 > 5.5f) {
    if (!mFlags[139]) {
    mHandles[273] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[273], mHandles[262], 300.0f);
    mFlags[139] = true;
    }
    }

    // ---- +0x1f74  136 bytes ----
    if (mTimer10 > 6.0f) {
    if (!mFlags[140]) {
    mHandles[274] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[274], mHandles[262], 300.0f);
    mFlags[140] = true;
    }
    }

    // ---- +0x1ffc  136 bytes ----
    if (mTimer10 > 6.5f) {
    if (!mFlags[141]) {
    mHandles[275] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[275], mHandles[262], 300.0f);
    mFlags[141] = true;
    }
    }

    // ---- +0x2084  136 bytes ----
    if (mTimer10 > 7.0f) {
    if (!mFlags[142]) {
    mHandles[276] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[276], mHandles[262], 300.0f);
    mFlags[142] = true;
    }
    }

    // ---- +0x210c  136 bytes ----
    if (mTimer10 > 7.5f) {
    if (!mFlags[143]) {
    mHandles[277] = MissionUtility::CreateObject("kas_crea_gnasp", "nest3_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[277], mHandles[262], 300.0f);
    mFlags[143] = true;
    }
    }

    // ---- +0x2194  180 bytes ----
    if (mFlags[129] && mFlags[130] && mFlags[131] && mFlags[132] && mFlags[133] && mFlags[134] && mFlags[135] && mFlags[136] && mFlags[137] && mFlags[138] && mFlags[139] && mFlags[140] && mFlags[140]) {
        if (mFlags[142]) {
        if (mFlags[143]) {
        mFlags[127] = true;
        }
        }
    }

    // ---- +0x2248  2320 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[278]) < 500.0f) {
    if (!mFlags[144]) {
    if (!mFlags[145]) {
    BeginTimer(mTimer11);
    mFlags[145] = true;
    }
    if (mTimer11 > 0.5f) {
    if (!mFlags[146]) {
    mHandles[279] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[279], mHandles[278], 300.0f);
    mFlags[146] = true;
    }
    }
    if (mTimer11 > 1.0f) {
    if (!mFlags[147]) {
    mHandles[280] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[280], mHandles[17], true, true, false, false);
    mFlags[147] = true;
    }
    }
    if (mTimer11 > 1.5f) {
    if (!mFlags[148]) {
    mHandles[281] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[281], mHandles[278], 300.0f);
    mFlags[148] = true;
    }
    }
    if (mTimer11 > 2.0f) {
    if (!mFlags[149]) {
    mHandles[282] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[282], mHandles[278], 300.0f);
    mFlags[149] = true;
    }
    }
    if (mTimer11 > 2.5f) {
    if (!mFlags[150]) {
    mHandles[283] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[283], mHandles[278], 300.0f);
    mFlags[150] = true;
    }
    }
    if (mTimer11 > 3.0f) {
    if (!mFlags[151]) {
    mHandles[284] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[284], mHandles[17], true, true, false, false);
    mFlags[151] = true;
    }
    }
    if (mTimer11 > 3.5f) {
    if (!mFlags[152]) {
    mHandles[285] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[285], mHandles[278], 300.0f);
    mFlags[152] = true;
    }
    }
    if (mTimer11 > 4.0f) {
    if (!mFlags[153]) {
    mHandles[286] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[286], mHandles[278], 300.0f);
    mFlags[153] = true;
    }
    }
    if (mTimer11 > 4.5f) {
    if (!mFlags[154]) {
    mHandles[287] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[287], mHandles[278], 300.0f);
    mFlags[154] = true;
    }
    }
    if (mTimer11 > 5.0f) {
    if (!mFlags[155]) {
    mHandles[288] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[288], mHandles[17], true, true, false, false);
    mFlags[155] = true;
    }
    }
    if (mTimer11 > 5.5f) {
    if (!mFlags[156]) {
    mHandles[289] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[289], mHandles[278], 300.0f);
    mFlags[156] = true;
    }
    }
    if (mTimer11 > 6.0f) {
    if (!mFlags[157]) {
    mHandles[290] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[290], mHandles[278], 300.0f);
    mFlags[157] = true;
    }
    }
    if (mTimer11 > 6.5f) {
    if (!mFlags[158]) {
    mHandles[291] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[291], mHandles[278], 300.0f);
    mFlags[158] = true;
    }
    }
    if (mTimer11 > 7.0f) {
    if (!mFlags[159]) {
    mHandles[292] = MissionUtility::CreateObject("kas_crea_gnasp", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[292], mHandles[278], 300.0f);
    mFlags[159] = true;
    }
    }
    if (mTimer11 > 7.5f) {
    if (!mFlags[160]) {
    mHandles[293] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest4_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[293], mHandles[278], 300.0f);
    mFlags[160] = true;
    }
    }
    if (mFlags[146]) {
    if (mFlags[147]) {
    if (mFlags[148]) {
    if (mFlags[149]) {
    if (mFlags[150]) {
    if (mFlags[151]) {
    if (mFlags[152]) {
    if (mFlags[153]) {
    if (mFlags[154]) {
    if (mFlags[155]) {
    if (mFlags[156] && mFlags[157] && mFlags[157]) {
    if (mFlags[159]) {
    if (mFlags[160]) {
    mFlags[144] = true;
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

    // ---- +0x2b58  2320 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[294]) < 500.0f) {
    if (!mFlags[161]) {
    if (!mFlags[162]) {
    BeginTimer(mTimer12);
    mFlags[162] = true;
    }
    if (mTimer12 > 0.5f) {
    if (!mFlags[163]) {
    mHandles[295] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[295], mHandles[294], 300.0f);
    mFlags[163] = true;
    }
    }
    if (mTimer12 > 1.0f) {
    if (!mFlags[164]) {
    mHandles[296] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[296], mHandles[294], 300.0f);
    mFlags[164] = true;
    }
    }
    if (mTimer12 > 1.5f) {
    if (!mFlags[165]) {
    mHandles[297] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[297], mHandles[294], 300.0f);
    mFlags[165] = true;
    }
    }
    if (mTimer12 > 2.0f) {
    if (!mFlags[166]) {
    mHandles[298] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[298], mHandles[17], true, true, false, false);
    mFlags[166] = true;
    }
    }
    if (mTimer12 > 2.5f) {
    if (!mFlags[167]) {
    mHandles[299] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[299], mHandles[294], 300.0f);
    mFlags[167] = true;
    }
    }
    if (mTimer12 > 3.0f) {
    if (!mFlags[168]) {
    mHandles[300] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[300], mHandles[294], 300.0f);
    mFlags[168] = true;
    }
    }
    if (mTimer12 > 3.5f) {
    if (!mFlags[169]) {
    mHandles[301] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[301], mHandles[294], 300.0f);
    mFlags[169] = true;
    }
    }
    if (mTimer12 > 4.0f) {
    if (!mFlags[170]) {
    mHandles[302] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[302], mHandles[294], 300.0f);
    mFlags[170] = true;
    }
    }
    if (mTimer12 > 4.5f) {
    if (!mFlags[171]) {
    mHandles[303] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[303], mHandles[17], true, true, false, false);
    mFlags[171] = true;
    }
    }
    if (mTimer12 > 5.0f) {
    if (!mFlags[172]) {
    mHandles[304] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[304], mHandles[294], 300.0f);
    mFlags[172] = true;
    }
    }
    if (mTimer12 > 5.5f) {
    if (!mFlags[173]) {
    mHandles[305] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[305], mHandles[294], 300.0f);
    mFlags[173] = true;
    }
    }
    if (mTimer12 > 6.0f) {
    if (!mFlags[174]) {
    mHandles[306] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[306], mHandles[294], 300.0f);
    mFlags[174] = true;
    }
    }
    if (mTimer12 > 6.5f) {
    if (!mFlags[175]) {
    mHandles[307] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[307], mHandles[294], 300.0f);
    mFlags[175] = true;
    }
    }
    if (mTimer12 > 7.0f) {
    if (!mFlags[176]) {
    mHandles[308] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[308], mHandles[17], true, true, false, false);
    mFlags[176] = true;
    }
    }
    if (mTimer12 > 7.5f) {
    if (!mFlags[177]) {
    mHandles[309] = MissionUtility::CreateObject("kas_crea_gnasp", "nest5_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[309], mHandles[294], 300.0f);
    mFlags[177] = true;
    }
    }
    if (mFlags[163]) {
    if (mFlags[164]) {
    if (mFlags[165]) {
    if (mFlags[166]) {
    if (mFlags[167]) {
    if (mFlags[168]) {
    if (mFlags[169]) {
    if (mFlags[170]) {
    if (mFlags[171]) {
    if (mFlags[172]) {
    if (mFlags[173] && mFlags[174] && mFlags[174]) {
    if (mFlags[176]) {
    if (mFlags[177]) {
    mFlags[161] = true;
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

    // ---- +0x3468  2320 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[310]) < 500.0f) {
    if (!mFlags[178]) {
    if (!mFlags[179]) {
    BeginTimer(mTimer13);
    mFlags[179] = true;
    }
    if (mTimer13 > 0.5f) {
    if (!mFlags[180]) {
    mHandles[311] = MissionUtility::CreateObject("kas_crea_gnasp", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[311], mHandles[310], 300.0f);
    mFlags[180] = true;
    }
    }
    if (mTimer13 > 1.0f) {
    if (!mFlags[181]) {
    mHandles[312] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[312], mHandles[310], 300.0f);
    mFlags[181] = true;
    }
    }
    if (mTimer13 > 1.5f) {
    if (!mFlags[182]) {
    mHandles[313] = MissionUtility::CreateObject("kas_crea_gnasp", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[313], mHandles[310], 300.0f);
    mFlags[182] = true;
    }
    }
    if (mTimer13 > 2.0f) {
    if (!mFlags[183]) {
    mHandles[314] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[314], mHandles[17], true, true, false, false);
    mFlags[183] = true;
    }
    }
    if (mTimer13 > 2.5f) {
    if (!mFlags[184]) {
    mHandles[315] = MissionUtility::CreateObject("kas_crea_gnasp", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[315], mHandles[310], 300.0f);
    mFlags[184] = true;
    }
    }
    if (mTimer13 > 3.0f) {
    if (!mFlags[185]) {
    mHandles[316] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[316], mHandles[310], 300.0f);
    mFlags[185] = true;
    }
    }
    if (mTimer13 > 3.5f) {
    if (!mFlags[186]) {
    mHandles[317] = MissionUtility::CreateObject("kas_crea_gnasp", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[317], mHandles[310], 300.0f);
    mFlags[186] = true;
    }
    }
    if (mTimer13 > 4.0f) {
    if (!mFlags[187]) {
    mHandles[318] = MissionUtility::CreateObject("kas_crea_gnasp", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[318], mHandles[17], true, true, false, false);
    mFlags[187] = true;
    }
    }
    if (mTimer13 > 4.5f) {
    if (!mFlags[188]) {
    mHandles[319] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[319], mHandles[310], 300.0f);
    mFlags[188] = true;
    }
    }
    if (mTimer13 > 5.0f) {
    if (!mFlags[189]) {
    mHandles[320] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[320], mHandles[310], 300.0f);
    mFlags[189] = true;
    }
    }
    if (mTimer13 > 5.5f) {
    if (!mFlags[190]) {
    mHandles[321] = MissionUtility::CreateObject("kas_crea_gnasp", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[321], mHandles[310], 300.0f);
    mFlags[190] = true;
    }
    }
    if (mTimer13 > 6.0f) {
    if (!mFlags[191]) {
    mHandles[322] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[322], mHandles[310], 300.0f);
    mFlags[191] = true;
    }
    }
    if (mTimer13 > 6.5f) {
    if (!mFlags[192]) {
    mHandles[323] = MissionUtility::CreateObject("kas_crea_gnasp", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[323], mHandles[17], true, true, false, false);
    mFlags[192] = true;
    }
    }
    if (mTimer13 > 7.0f) {
    if (!mFlags[193]) {
    mHandles[324] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[324], mHandles[310], 300.0f);
    mFlags[193] = true;
    }
    }
    if (mTimer13 > 7.5f) {
    if (!mFlags[194]) {
    mHandles[325] = MissionUtility::CreateObject("kas_crea_gnasp", "nest6_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[325], mHandles[310], 300.0f);
    mFlags[194] = true;
    }
    }
    if (mFlags[180]) {
    if (mFlags[181]) {
    if (mFlags[182]) {
    if (mFlags[183]) {
    if (mFlags[184]) {
    if (mFlags[185]) {
    if (mFlags[186]) {
    if (mFlags[187]) {
    if (mFlags[188]) {
    if (mFlags[189]) {
    if (mFlags[190] && mFlags[191] && mFlags[191]) {
    if (mFlags[193]) {
    if (mFlags[194]) {
    mFlags[178] = true;
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

    // ---- +0x3d78  60 bytes ----
    if (mTimer3 > 5.0f) {
    if (!mFlags[212]) {
    if (!mFlags[213]) {
    BeginTimer(mTimer15);
    mFlags[213] = true;
    }
    }
    }

    // ---- +0x3db4  136 bytes ----
    if (mTimer15 > 0.5f) {
    if (!mFlags[214]) {
    mHandles[343] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[343], mHandles[342], 300.0f);
    mFlags[214] = true;
    }
    }

    // ---- +0x3e3c  148 bytes ----
    if (mTimer15 > 1.0f) {
    if (!mFlags[215]) {
    mHandles[344] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[344], mHandles[17], true, true, false, false);
    mFlags[215] = true;
    }
    }

    // ---- +0x3ed0  136 bytes ----
    if (mTimer15 > 1.5f) {
    if (!mFlags[216]) {
    mHandles[345] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[345], mHandles[342], 300.0f);
    mFlags[216] = true;
    }
    }

    // ---- +0x3f58  136 bytes ----
    if (mTimer15 > 2.0f) {
    if (!mFlags[217]) {
    mHandles[346] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[346], mHandles[342], 300.0f);
    mFlags[217] = true;
    }
    }

    // ---- +0x3fe0  148 bytes ----
    if (mTimer15 > 2.5f) {
    if (!mFlags[218]) {
    mHandles[347] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[347], mHandles[17], true, true, false, false);
    mFlags[218] = true;
    }
    }

    // ---- +0x4074  136 bytes ----
    if (mTimer15 > 3.0f) {
    if (!mFlags[219]) {
    mHandles[348] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[348], mHandles[342], 300.0f);
    mFlags[219] = true;
    }
    }

    // ---- +0x40fc  136 bytes ----
    if (mTimer15 > 3.5f) {
    if (!mFlags[220]) {
    mHandles[349] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[349], mHandles[342], 300.0f);
    mFlags[220] = true;
    }
    }

    // ---- +0x4184  136 bytes ----
    if (mTimer15 > 4.0f) {
    if (!mFlags[221]) {
    mHandles[350] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[350], mHandles[342], 300.0f);
    mFlags[221] = true;
    }
    }

    // ---- +0x420c  148 bytes ----
    if (mTimer15 > 4.5f) {
    if (!mFlags[222]) {
    mHandles[351] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[351], mHandles[17], true, true, false, false);
    mFlags[222] = true;
    }
    }

    // ---- +0x42a0  136 bytes ----
    if (mTimer15 > 5.0f) {
    if (!mFlags[223]) {
    mHandles[352] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[352], mHandles[342], 300.0f);
    mFlags[223] = true;
    }
    }

    // ---- +0x4328  136 bytes ----
    if (mTimer15 > 5.5f) {
    if (!mFlags[224]) {
    mHandles[353] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[353], mHandles[342], 300.0f);
    mFlags[224] = true;
    }
    }

    // ---- +0x43b0  136 bytes ----
    if (mTimer15 > 6.0f) {
    if (!mFlags[225]) {
    mHandles[354] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[354], mHandles[342], 300.0f);
    mFlags[225] = true;
    }
    }

    // ---- +0x4438  136 bytes ----
    if (mTimer15 > 6.5f) {
    if (!mFlags[226]) {
    mHandles[355] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[355], mHandles[342], 300.0f);
    mFlags[226] = true;
    }
    }

    // ---- +0x44c0  136 bytes ----
    if (mTimer15 > 7.0f) {
    if (!mFlags[227]) {
    mHandles[356] = MissionUtility::CreateObject("kas_crea_gnasp", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[356], mHandles[342], 300.0f);
    mFlags[227] = true;
    }
    }

    // ---- +0x4548  148 bytes ----
    if (mTimer15 > 7.5f) {
    if (!mFlags[228]) {
    mHandles[357] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest8_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[357], mHandles[17], true, true, false, false);
    mFlags[228] = true;
    }
    }

    // ---- +0x45dc  180 bytes ----
    if (mFlags[214] && mFlags[215] && mFlags[216] && mFlags[217] && mFlags[218] && mFlags[219] && mFlags[220] && mFlags[221] && mFlags[222] && mFlags[223] && mFlags[224] && mFlags[225] && mFlags[225]) {
        if (mFlags[227]) {
        if (mFlags[228]) {
        mFlags[212] = true;
        }
        }
    }

    // ---- +0x4690  2320 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[358]) < 500.0f) {
    if (!mFlags[229]) {
    if (!mFlags[230]) {
    BeginTimer(mTimer16);
    mFlags[230] = true;
    }
    if (mTimer16 > 0.5f) {
    if (!mFlags[231]) {
    mHandles[359] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[359], mHandles[358], 300.0f);
    mFlags[231] = true;
    }
    }
    if (mTimer16 > 1.0f) {
    if (!mFlags[232]) {
    mHandles[360] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[360], mHandles[17], true, true, false, false);
    mFlags[232] = true;
    }
    }
    if (mTimer16 > 1.5f) {
    if (!mFlags[233]) {
    mHandles[361] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[361], mHandles[358], 300.0f);
    mFlags[233] = true;
    }
    }
    if (mTimer16 > 2.0f) {
    if (!mFlags[234]) {
    mHandles[362] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[362], mHandles[358], 300.0f);
    mFlags[234] = true;
    }
    }
    if (mTimer16 > 2.5f) {
    if (!mFlags[235]) {
    mHandles[363] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[363], mHandles[358], 300.0f);
    mFlags[235] = true;
    }
    }
    if (mTimer16 > 3.0f) {
    if (!mFlags[236]) {
    mHandles[364] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[364], mHandles[358], 300.0f);
    mFlags[236] = true;
    }
    }
    if (mTimer16 > 3.5f) {
    if (!mFlags[237]) {
    mHandles[365] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[365], mHandles[17], true, true, false, false);
    mFlags[237] = true;
    }
    }
    if (mTimer16 > 4.0f) {
    if (!mFlags[238]) {
    mHandles[366] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[366], mHandles[358], 300.0f);
    mFlags[238] = true;
    }
    }
    if (mTimer16 > 4.5f) {
    if (!mFlags[239]) {
    mHandles[367] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[367], mHandles[358], 300.0f);
    mFlags[239] = true;
    }
    }
    if (mTimer16 > 5.0f) {
    if (!mFlags[240]) {
    mHandles[368] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[368], mHandles[358], 300.0f);
    mFlags[240] = true;
    }
    }
    if (mTimer16 > 5.5f) {
    if (!mFlags[241]) {
    mHandles[369] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::AttackTarget(mHandles[369], mHandles[17], true, true, false, false);
    mFlags[241] = true;
    }
    }
    if (mTimer16 > 6.0f) {
    if (!mFlags[242]) {
    mHandles[370] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[370], mHandles[358], 300.0f);
    mFlags[242] = true;
    }
    }
    if (mTimer16 > 6.5f) {
    if (!mFlags[243]) {
    mHandles[371] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[371], mHandles[358], 300.0f);
    mFlags[243] = true;
    }
    }
    if (mTimer16 > 7.0f) {
    if (!mFlags[244]) {
    mHandles[372] = MissionUtility::CreateObject("kas_crea_gnasp", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[372], mHandles[358], 300.0f);
    mFlags[244] = true;
    }
    }
    if (mTimer16 > 7.5f) {
    if (!mFlags[245]) {
    mHandles[373] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "nest9_spawn", 0, "", 2, -1);
    MissionUtility::Defend(mHandles[373], mHandles[358], 300.0f);
    mFlags[245] = true;
    }
    }
    if (mFlags[231]) {
    if (mFlags[232]) {
    if (mFlags[233]) {
    if (mFlags[234]) {
    if (mFlags[235]) {
    if (mFlags[236]) {
    if (mFlags[237]) {
    if (mFlags[238]) {
    if (mFlags[239]) {
    if (mFlags[240]) {
    if (mFlags[241] && mFlags[242] && mFlags[242]) {
    if (mFlags[244]) {
    if (mFlags[245]) {
    mFlags[229] = true;
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

    // ---- +0x4fa0  2140 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[15], "valley_trigger1")) {
    if (!mFlags[36]) {
    mHandles[25] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 0, "", 0, -1);
    mHandles[26] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn1", 1, "", 0, -1);
    mHandles[27] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 2, "", 0, -1);
    mHandles[28] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 3, "", 0, -1);
    mHandles[29] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn1", 4, "", 0, -1);
    mHandles[30] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 5, "", 0, -1);
    mHandles[31] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 6, "", 0, -1);
    mHandles[32] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn1", 7, "", 0, -1);
    mHandles[33] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 8, "", 0, -1);
    mHandles[34] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 9, "", 0, -1);
    mHandles[35] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn1", 10, "", 0, -1);
    mHandles[36] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 11, "", 0, -1);
    mHandles[37] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn1", 12, "", 0, -1);
    mHandles[38] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn1", 13, "", 0, -1);
    mHandles[39] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn1", 14, "", 0, -1);
    MissionUtility::Goto(mHandles[25], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[26], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[27], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[28], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[29], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[30], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[31], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[32], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[33], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[34], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[35], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[36], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[37], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[38], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[39], "valley_gnasp_goto1", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[25], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    mFlags[36] = true;
    }
    }

    // ---- +0x57fc  2140 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[15], "valley_trigger2")) {
    if (!mFlags[37]) {
    mHandles[40] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 0, "", 0, -1);
    mHandles[41] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn2", 1, "", 0, -1);
    mHandles[42] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 2, "", 0, -1);
    mHandles[43] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 3, "", 0, -1);
    mHandles[44] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn2", 4, "", 0, -1);
    mHandles[45] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 5, "", 0, -1);
    mHandles[46] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 6, "", 0, -1);
    mHandles[47] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn2", 7, "", 0, -1);
    mHandles[48] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 8, "", 0, -1);
    mHandles[49] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 9, "", 0, -1);
    mHandles[50] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn2", 10, "", 0, -1);
    mHandles[51] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 11, "", 0, -1);
    mHandles[52] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn2", 12, "", 0, -1);
    mHandles[53] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn2", 13, "", 0, -1);
    mHandles[54] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn2", 14, "", 0, -1);
    MissionUtility::Goto(mHandles[40], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[41], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[42], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[43], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[44], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[45], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[46], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[47], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[48], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[49], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[50], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[51], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[52], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[53], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[54], "valley_gnasp_goto2", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[40], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    mFlags[37] = true;
    }
    }

    // ---- +0x6058  2140 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[15], "valley_trigger3")) {
    if (!mFlags[38]) {
    mHandles[55] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 0, "", 0, -1);
    mHandles[56] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn3", 1, "", 0, -1);
    mHandles[57] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 2, "", 0, -1);
    mHandles[58] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 3, "", 0, -1);
    mHandles[59] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn3", 4, "", 0, -1);
    mHandles[60] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 5, "", 0, -1);
    mHandles[61] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 6, "", 0, -1);
    mHandles[62] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn3", 7, "", 0, -1);
    mHandles[63] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 8, "", 0, -1);
    mHandles[64] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 9, "", 0, -1);
    mHandles[65] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn3", 10, "", 0, -1);
    mHandles[66] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 11, "", 0, -1);
    mHandles[67] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn3", 12, "", 0, -1);
    mHandles[68] = MissionUtility::CreateObject("kas_crea_gnasp", "valley_gnasp_spawn3", 13, "", 0, -1);
    mHandles[69] = MissionUtility::CreateObject("kas_crea_gnasp_encircle", "valley_gnasp_spawn3", 14, "", 0, -1);
    MissionUtility::Goto(mHandles[55], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[56], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[57], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[58], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[59], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[60], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[61], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[62], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[63], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[64], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[65], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[66], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[67], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[68], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[15], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[69], "valley_gnasp_goto3", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[55], mHandles[18], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    mFlags[38] = true;
    }
    }

    // ---- +0x68b4  392 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[18], "patrol5_reached")
        || MissionUtility::IsInsideRegion(mHandles[15], "patrol5_reached")) {
    if (!mFlags[48]) {
    mHandles[380] = MissionUtility::CreateObjectWithRotation("kas_crea_maru_player", "MidCinAnakinPath", 0, "MidCinAnakin", 0, -1, -1);
    mHandles[379] = MissionUtility::CreateObjectWithRotation("kas_crea_maru_bera", "MidCinBeraPath", 0, "MidCinBera", 0, -1, -1);
    mHandles[382] = MissionUtility::CreateObjectWithRotation("kas_crea_maru_player", "MidCin2AnakinPath", 0, "MidCin2Anakin", 0, -1, -1);
    mHandles[381] = MissionUtility::CreateObjectWithRotation("kas_crea_maru_bera", "MidCin2BeraPath", 0, "MidCin2Bera", 0, -1, -1);
    MissionUtility::SetCollidable(mHandles[382], false);
    MissionUtility::SetCollidable(mHandles[381], false);
    MissionUtility::SetVisible(mHandles[382], false);
    MissionUtility::SetVisible(mHandles[381], false);
    MissionUtility::SetCollidable(mHandles[379], false);
    MissionUtility::SetCollidable(mHandles[380], false);
    MissionUtility::Goto(mHandles[380], "MidCinAnakinPath", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[380]);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::StopMusic(0);
    mHandles[78] = MissionUtility::RunCin("Cin3", true, true);
    MissionUtility::QueueSound("BKK13_29", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::RemoveObjectify(mHandles[18]);
    mFlags[48] = true;
    BeginTimer(mTimer23);
    }
    }

    // ---- +0x6a3c  104 bytes ----
    if (mTimer23 > 7.0f) {
    MissionUtility::QueueSound("bkk13_30", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::Goto(mHandles[379], "MidCinBeraPath1", true);
    MissionUtility::Goto(mHandles[380], "MidCinAnakinPath1", true);
    mTimer23 = 0.0f;
    StopTimer(mTimer23);
    }

    // ---- +0x6aa4  88 bytes ----
    if (mFlags[48]) {
    if (!MissionUtility::IsCinRunning(mHandles[78])) {
    mInts[3] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0005");
    MissionUtility::DisplayText("missions.Kashyyyk2.objective.str0005", 5.0f, -1.0f);
    MissionUtility::FlushSoundQueue();
    MissionUtility::RemoveObject(mHandles[380]);
    MissionUtility::RemoveObject(mHandles[379]);
    mInts2[10] = 4;

        break;
    case 4:
    // ---- +0x6afc  88 bytes ----
    if (mFlags[80]) {
        MissionUtility::PlayMusic("EP5_V1_T06_02", true);
        MissionUtility::Patrol(mHandles[18], "bera_patrol5", 500.0f, true);
        MissionUtility::SetCurHealth(mHandles[18], 750.0f);
        MissionUtility::SetWeaponOrd(mHandles[18], "REP_blaster_maru", "rep_blaster_maru_weak_ord");
        MissionUtility::ObjectiveComplete(mInts[2]);
        mFlags[80] = false;
    }

    // ---- +0x6b54  56 bytes ----
    if (!mFlags[83]) {
        if (!MissionUtility::IsAlive(mHandles[15])) {
        mHandles[85] = MissionUtility::RunCin("deathcin", true, false);
        mFlags[30] = true;
        }
    }

    // ---- +0x6b8c  88 bytes ----
    if (!mFlags[83]) {
        if (!MissionUtility::IsAlive(mHandles[18])) {
        if (!mFlags[2]) {
        MissionUtility::QueueSound("BKDS004", 1.0f, 0.0f, 0.0f, "", 0, "");
        BeginTimer(mTimer18);
        mFlags[2] = true;
        }
        }
    }

    // ---- +0x6be4  56 bytes ----
    if (mTimer18 > 3.0f) {
    if (!mFlags[1]) {
    mInts[1] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0013");
    MissionUtility::MissionFailure();
    mFlags[1] = true;
    }
    }

    // ---- +0x6c1c  52 bytes ----
    if (mFlags[30]) {
        if (!MissionUtility::IsCinRunning(mHandles[85])) {
        if (!mFlags[1]) {
        MissionUtility::MissionFailure();
        mFlags[1] = true;
        }
        }
    }

    // ---- +0x6c50  44 bytes ----
    if (MissionUtility::GetDistance(mHandles[15], mHandles[204]) < 200.0f) {
    if (!mFlags[84]) {
    mFlags[84] = true;
    }
    }

    // ---- +0x6c7c  184 bytes ----
    if (mFlags[48]) {
        if (!MissionUtility::IsCinRunning(mHandles[78])) {
        if (!mFlags[85]) {
        MissionUtility::RemoveObject(mHandles[380]);
        MissionUtility::RemoveObject(mHandles[379]);
        MissionUtility::SetEnemies(1, 10);
        MissionUtility::SetEnemies(20, 10);
        MissionUtility::MoveObjectWithRotation(mHandles[15], "patrol5_attack_startpoint", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[18], "patrol5_attack_startpoint", 1, true);
        MissionUtility::Patrol(mHandles[18], "patrol5_attack", 500.0f, true);
        MissionUtility::StartSound("BKK13_14", false, 1.5f, 0.0f, 0.0f, "", 0, "");
        mFlags[85] = true;
        }
        }
    }

    // ---- +0x6d34  88 bytes ----
    if (mFlags[51]) {
        if (!MissionUtility::IsCinRunning(mHandles[81])) {
        MissionUtility::RemoveObject(mHandles[384]);
        MissionUtility::RemoveObject(mHandles[383]);
        MissionUtility::RemoveObject(mHandles[382]);
        MissionUtility::RemoveObject(mHandles[381]);
        MissionUtility::RemoveObject(mHandles[380]);
        MissionUtility::RemoveObject(mHandles[379]);
        MissionUtility::FlushSoundQueue();
        mInts2[10] = 6;
        }
    }

    // ---- +0x6d8c  44 bytes ----
    if (!MissionUtility::IsFlockAlive(mHandles[24])) {
    if (!mFlags[83]) {
    mFlags[83] = true;
    BeginTimer(mTimer21);
    }
    }

    // ---- +0x6db8  504 bytes ----
    if (mFlags[83]) {
        if (mTimer21 > 5.0f) {
        if (!mFlags[51]) {
        MissionUtility::MoveObject(mHandles[15], "pre_canyon_turnaround_focus", 0, true);
        MissionUtility::RemoveObject(mHandles[18]);
        MissionUtility::SetCollidable(mHandles[382], true);
        MissionUtility::SetCollidable(mHandles[381], true);
        MissionUtility::SetVisible(mHandles[382], true);
        MissionUtility::SetVisible(mHandles[381], true);
        mHandles[384] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin_cin", "MidCin2AnakinPath1", 0, "MidCin2Anakin1", 0, -1, -1);
        mHandles[383] = MissionUtility::CreateObjectWithRotation("rep_inf_bera", "MidCin2BeraPath1", 0, "MidCin2Bera1", 0, -1, -1);
        mHandles[385] = MissionUtility::CreateObjectWithRotation("kas_crea_maru", "MidCin2AnakinPath", 1, "MidCin2Maru1", 0, -1, -1);
        mHandles[386] = MissionUtility::CreateObjectWithRotation("kas_crea_maru", "MidCin2BeraPath", 1, "MidCin2Maru2", 0, -1, -1);
        MissionUtility::SetCollidable(mHandles[384], false);
        MissionUtility::SetCollidable(mHandles[383], false);
        MissionUtility::SetVisible(mHandles[384], false);
        MissionUtility::SetVisible(mHandles[383], false);
        MissionUtility::SetCollidable(mHandles[385], false);
        MissionUtility::SetCollidable(mHandles[386], false);
        MissionUtility::SetVisible(mHandles[385], false);
        MissionUtility::SetVisible(mHandles[386], false);
        MissionUtility::SetCollidable(mHandles[202], false);
        MissionUtility::SetApplyDynamics(mHandles[382], true);
        MissionUtility::Goto(mHandles[382], "MidCin2AnakinPath", false);
        MissionUtility::Goto(mHandles[381], "MidCin2BeraPath", false);
        MissionUtility::ObjectiveComplete(mInts[3]);
        mHandles[81] = MissionUtility::RunCin("Cin6", true, true);
        mFlags[51] = true;
        MissionUtility::QueueSound("bkk13_31", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::PlayMusic("EP2_V1_T12_03", true);
        BeginTimer(mTimer29);
        }
        }
    }

    // ---- +0x6fb0  96 bytes ----
    if (mTimer29 > 1.5f) {
    StopTimer(mTimer29);
    mTimer29 = 0.0f;
    MissionUtility::Stop(mHandles[381]);
    MissionUtility::Stop(mHandles[382]);
    MissionUtility::SetAnimation(mHandles[382], "dismount", 1.0f, 1);
    MissionUtility::SetAnimation(mHandles[381], "dismount", 1.0f, 1);
    }

    // ---- +0x7010  212 bytes ----
    if (!mFlags[250]) {
        if (MissionUtility::GetCinId(mHandles[81]) == 2) {
        mFlags[250] = true;
        Matrix aatLoc = MissionUtility::GetLocation(mHandles[382]);
        Matrix ridLoc = MissionUtility::GetLocation(mHandles[381]);
        MissionUtility::RemoveObject(mHandles[381]);
        MissionUtility::RemoveObject(mHandles[382]);
        MissionUtility::SetVisible(mHandles[384], true);
        MissionUtility::SetVisible(mHandles[383], true);
        MissionUtility::SetCollidable(mHandles[385], true);
        MissionUtility::SetCollidable(mHandles[386], true);
        MissionUtility::SetVisible(mHandles[385], true);
        MissionUtility::SetVisible(mHandles[386], true);
        MissionUtility::Goto(mHandles[384], "MidCin2AATPoint", false);
        MissionUtility::Goto(mHandles[383], "MidCin2AATPoint", false);
        BeginTimer(mTimer25);
        }
    }

    // ---- +0x70e4  76 bytes ----
    if (!mFlags[251]) {
        if (MissionUtility::GetCinId(mHandles[81]) == 3) {
        mFlags[251] = true;
        MissionUtility::RemoveObject(mHandles[384]);
        MissionUtility::RemoveObject(mHandles[383]);
        MissionUtility::Goto(mHandles[202], "MidCin2AATPath", false);
        BeginTimer(mTimer24);
        }
    }

    // ---- +0x7130  140 bytes ----
    if (mTimer24 > 2.0f) {
    MissionUtility::QueueSound("maru_idle_02", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("maru_idle_04", 1.0f, 0.0f, 0.0f, "", 0, "");
    StopTimer(mTimer24);
    mTimer24 = 0.0f;
    MissionUtility::Goto(mHandles[385], "MidCin2Maru1Path", false);
    MissionUtility::Goto(mHandles[386], "MidCin2Maru2Path", false);

        break;
    case 6:
    // ---- +0x71bc  32 bytes ----
    MissionUtility::MidMissionSavePlayer(7);
    MissionUtility::MidMissionSave(mTimer20);
    mInts2[10] = 5;

        break;
    case 7:
    // ---- +0x71dc  84 bytes ----
    mInts[0] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0006");
    mInts[3] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0005");
    MissionUtility::MidMissionLoadPlayer();
    float savedClock;
    MissionUtility::MidMissionLoad(savedClock);
    MissionUtility::ObjectiveComplete(mInts[3]);
    StopTimer(mTimer20);
    mTimer20 = savedClock;
    ResumeTimer(mTimer20);
    mInts2[10] = 5;

        break;
    case 5:
    // ---- +0x7230  400 bytes ----
    if (mFlags[81]) {
        MissionUtility::SetCollidable(mHandles[202], true);
        MissionUtility::RemoveTurnAroundRegion("turnaround3");
        MissionUtility::RemoveTurnAroundRegion("turnaround2");
        MissionUtility::ObjectiveComplete(mInts[0]);
        MissionUtility::RemoveObject(mHandles[385]);
        MissionUtility::RemoveObject(mHandles[386]);
        mHandles[385] = MissionUtility::CreateObjectWithRotation("kas_crea_maru", "MidCin2Maru1Path", 0, "", 0, -1, -1);
        mHandles[386] = MissionUtility::CreateObjectWithRotation("kas_crea_maru", "MidCin2Maru2Path", 0, "", 0, -1, -1);
        MissionUtility::RemoveObject(mHandles[73]);
        MissionUtility::RemoveObject(mHandles[74]);
        MissionUtility::RemoveFlock(mHandles[24], false);
        MissionUtility::MoveObjectWithRotation(mHandles[202], "aat_start", 0, true);
        MissionUtility::PlayMusic("EP6_V2_T05_01", true);
        mInts[4] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0003");
        MissionUtility::DisplayText("missions.Kashyyyk2.objective.str0003", 5.0f, -1.0f);
        MissionUtility::SetAsPlayer(mHandles[202], 0);
        MissionUtility::AddTurnAroundRegion("comm_turnaround", "aat_start", 0, 0, 0, 0);
        MissionUtility::AddTurnAroundRegion("turnaround2", "aat_start", 0, 0, 0, 0);
        MissionUtility::SetCurHealth(mHandles[202], 2100.0f);
        MissionUtility::SetTeamNum(mHandles[202], 1);
        MissionUtility::StartSound("BKK13_16", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObjectify(mHandles[202]);
        MissionUtility::Objectify("aat_comm_gate", 0, "missions.Kashyyyk2.marker.str0000", true, false, 0.0f, 2.0f);
        mFlags[81] = false;
    }

    // ---- +0x73c0  56 bytes ----
    if (!MissionUtility::IsAlive(mHandles[202])) {
    if (!mFlags[31]) {
    mHandles[86] = MissionUtility::RunCin("deathcin_aat", true, true);
    mFlags[31] = true;
    }
    }

    // ---- +0x73f8  52 bytes ----
    if (mFlags[31]) {
        if (!MissionUtility::IsCinRunning(mHandles[86])) {
        if (!mFlags[1]) {
        MissionUtility::MissionFailure();
        mFlags[1] = true;
        }
        }
    }

    // ---- +0x742c  180 bytes ----
    if (MissionUtility::GetDistance(mHandles[202], "aat_comm_gate", 0) < 50.0f) {
    if (!mFlags[39]) {
    if (!mFlags[25]) {
    MissionUtility::RemoveObjectify("aat_comm_gate", 0);
    MissionUtility::Objectify("aat_comm_goto", 0, "missions.Kashyyyk2.marker.str0000", true, false, 0.0f, 2.0f);
    MissionUtility::QueueSound("BKK13_18", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::ObjectiveComplete(mInts[4]);
    mInts[5] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0011");
    MissionUtility::DisplayText("missions.Kashyyyk2.text.str0011", 6.0f, -1.0f);
    MissionUtility::RemoveObject(mHandles[70]);
    mFlags[39] = true;
    }
    }
    }

    // ---- +0x74e0  60 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[202], "comm_dontshoot")) {
    if (!mFlags[22]) {
    if (!mFlags[18]) {
    mInts[13] = MissionUtility::GetPlayerShotCount();
    mFlags[22] = true;
    }
    }
    }

    // ---- +0x751c  152 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[202], "comm_gate_on")) {
    if (!mFlags[29]) {
    mHandles[71] = MissionUtility::CreateObject("kas_bldg_forcefence_1", Vector(-436.0f, -437.37933f, -2095.9998f), "gate_poweredup", 0, 0, Quat(1.000016f, 0.0f, 0.0f, 0.0f), -1);
    mFlags[29] = true;
    }
    }

    // ---- +0x75b4  88 bytes ----
    if (mFlags[22]) {
        if (mInts[14] > mInts[13]) {
        if (!mFlags[23]) {
        BeginTimer(mTimer28);
        MissionUtility::QueueSound("BKK13_17", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[23] = true;
        }
        }
    }

    // ---- +0x760c  48 bytes ----
    if (mTimer28 > 6.0f) {
    if (!mFlags[26]) {
    mInts[13] = MissionUtility::GetPlayerShotCount();
    mFlags[26] = true;
    }
    }

    // ---- +0x763c  72 bytes ----
    if (mTimer28 > 6.0f) {
    if (mFlags[22]) {
    if (mInts[14] > mInts[13]) {
    if (!mFlags[24]) {
    mFlags[32] = true;
    mFlags[24] = true;
    }
    }
    }
    }

    // ---- +0x7684  128 bytes ----
    if (!MissionUtility::IsAlive(mHandles[8])) {
    if (!mFlags[39]) {
    if (!mFlags[25]) {
    mFlags[32] = true;
    BeginTimer(mTimer17);
    MissionUtility::QueueSound("BKK13_32", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("BKK13_34", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[25] = true;
    }
    }
    }

    // ---- +0x7704  128 bytes ----
    if (!MissionUtility::IsAlive(mHandles[9])) {
    if (!mFlags[39]) {
    if (!mFlags[25]) {
    mFlags[32] = true;
    BeginTimer(mTimer17);
    MissionUtility::QueueSound("BKK13_32", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("BKK13_34", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[25] = true;
    }
    }
    }

    // ---- +0x7784  40 bytes ----
    if (!MissionUtility::IsAlive(mHandles[8])) {
    if (!mFlags[32]) {
    mFlags[32] = true;
    mFlags[25] = true;
    }
    }

    // ---- +0x77ac  40 bytes ----
    if (!MissionUtility::IsAlive(mHandles[9])) {
    if (!mFlags[32]) {
    mFlags[32] = true;
    mFlags[25] = true;
    }
    }

    // ---- +0x77d4  116 bytes ----
    if (mFlags[24] && !mFlags[39] && !mFlags[25]) {
        BeginTimer(mTimer17);
        MissionUtility::QueueSound("BKK13_32", 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::QueueSound("BKK13_34", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[25] = true;
    }

    // ---- +0x7848  76 bytes ----
    if (mFlags[24] && !mFlags[25] && !mFlags[27]) {
        MissionUtility::QueueSound("BKK13_33", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[27] = true;
    }

    // ---- +0x7894  44 bytes ----
    if (mTimer17 > 6.0f) {
    if (!mFlags[1]) {
    MissionUtility::MissionFailure();
    mFlags[1] = true;
    }
    }

    // ---- +0x78c0  400 bytes ----
    if (mFlags[32] && !mFlags[33]) {
        MissionUtility::StartSound("klaxon1", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[22] = false;
        MissionUtility::SetEnemies(1, 8);
        MissionUtility::SetEnemies(20, 8);
        MissionUtility::Goto(mHandles[225], "comm_aat_spawn", 0);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[225], mHandles[202], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[226], "comm_aat_spawn", 1);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[226], mHandles[202], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[227], "comm_aat_spawn", 2);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[227], mHandles[202], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[228], "comm_aat_spawn", 3);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[228], mHandles[202], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::Goto(mHandles[229], "comm_aat_spawn", 4);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::AttackTarget(mHandles[229], mHandles[202], true, true, false, false);
        MissionUtility::SetQueueFlag(false);
        mFlags[33] = true;
    }

    // ---- +0x7a50  1264 bytes ----
    if (mFlags[54]) {
        if (!MissionUtility::IsCinRunning(mHandles[84])) {
        if (!mFlags[86]) {
        MissionUtility::SetMapZoom(600.0f, 9999999.0f);
        MissionUtility::FlushSoundQueue();
        MissionUtility::SetTeamNum(mHandles[202], 1);
        MissionUtility::SetAsPlayer(mHandles[202], 0);
        MissionUtility::SetCurHealth(mHandles[202], mSavedHealth);
        MissionUtility::RemoveTurnAroundRegion("jedi_turnaround");
        mInts[6] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0007");
        MissionUtility::DisplayText("missions.Kashyyyk2.objective.str0007", 5.0f, -1.0f);
        MissionUtility::SetVelocForward(mHandles[202], 25.0f);
        MissionUtility::RemoveObject(mHandles[16]);
        MissionUtility::RemoveObjectify(mHandles[202]);
        MissionUtility::SetEnemies(1, 8);
        MissionUtility::SetEnemies(20, 8);
        mFlags[32] = true;
        MissionUtility::RemoveObject(mHandles[387]);
        mHandles[207] = MissionUtility::CreateObject("cis_tank_assault", "comm_droids_spawn", 2, "", 2, -1);
        MissionUtility::AttackTarget(mHandles[207], mHandles[202], true, true, false, false);
        mHandles[209] = MissionUtility::CreateObject("cis_walk_small", "comm_droids_spawn", 4, "", 2, -1);
        MissionUtility::AttackTarget(mHandles[209], mHandles[202], true, true, false, false);
        mHandles[211] = MissionUtility::CreateObject("cis_walk_small", "comm_droids_spawn", 6, "", 2, -1);
        MissionUtility::AttackTarget(mHandles[211], mHandles[202], true, true, false, false);
        mHandles[212] = MissionUtility::CreateObject("cis_walk_small", "comm_droids_spawn", 7, "", 2, -1);
        MissionUtility::AttackTarget(mHandles[212], mHandles[202], true, true, false, false);
        mHandles[214] = MissionUtility::CreateObject("cis_walk_small", "comm_droids_spawn", 9, "", 2, -1);
        MissionUtility::AttackTarget(mHandles[214], mHandles[202], true, true, false, false);
        mHandles[216] = MissionUtility::CreateObject("cis_tank_assault", "comm_droids_spawn", 11, "", 2, -1);
        MissionUtility::AttackTarget(mHandles[216], mHandles[202], true, true, false, false);
        mHandles[218] = MissionUtility::CreateObject("cis_walk_small", "comm_droids_spawn", 13, "", 2, -1);
        MissionUtility::AttackTarget(mHandles[218], mHandles[202], true, true, false, false);
        mHandles[220] = MissionUtility::CreateObject("cis_walk_small", "comm_droids_spawn", 14, "", 2, -1);
        MissionUtility::AttackTarget(mHandles[220], mHandles[202], true, true, false, false);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[205]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[207]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[208]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[209]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[212]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[214]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[216]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[218]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[220]);
        MissionUtility::Objectify("aat_comm_gate", 0, "missions.Kashyyyk2.marker.str0005", true, false, 0.0f, 2.0f);
        MissionUtility::StartSound("klaxon1", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::PlayMusic("EP6_V2_T03", true);
        BeginTimer(mTimer19);
        mFlags[86] = true;
        }
        }
    }

    // ---- +0x7f40  76 bytes ----
    if (mTimer19 > 3.0f) {
    if (!mFlags[45]) {
    MissionUtility::StartSound("BKK13_19", false, 1.5f, 0.0f, 0.0f, "", 0, "");
    mFlags[45] = true;
    }
    }

    // ---- +0x7f8c  376 bytes ----
    if (mFlags[86]) {
        if (MissionUtility::GetDistance(mHandles[202], "aat_comm_gate", 0) < 100.0f) {
        if (!mFlags[42]) {
        MissionUtility::StartSound("BKK13_37", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::SetMaxHealth(mHandles[88], 300.0f);
        MissionUtility::SetMaxHealth(mHandles[89], 300.0f);
        MissionUtility::SetMaxHealth(mHandles[90], 300.0f);
        MissionUtility::SetMaxHealth(mHandles[91], 300.0f);
        MissionUtility::SetCurHealth(mHandles[88], 300.0f);
        MissionUtility::SetCurHealth(mHandles[89], 300.0f);
        MissionUtility::SetCurHealth(mHandles[90], 300.0f);
        MissionUtility::SetCurHealth(mHandles[91], 300.0f);
        MissionUtility::SetTeamNum(mHandles[88], 2);
        MissionUtility::SetTeamNum(mHandles[89], 2);
        MissionUtility::SetTeamNum(mHandles[90], 2);
        MissionUtility::SetTeamNum(mHandles[91], 2);
        mInts[9] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0008");
        MissionUtility::DisplayText("missions.Kashyyyk2.objective.str0008", 6.0f, -1.0f);
        MissionUtility::RemoveObjectify("aat_comm_gate", 0);
        MissionUtility::Objectify(mHandles[88], "missions.Kashyyyk2.marker.str0003", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[89], "missions.Kashyyyk2.marker.str0003", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[90], "missions.Kashyyyk2.marker.str0003", false, true, 0.0f);
        MissionUtility::Objectify(mHandles[91], "missions.Kashyyyk2.marker.str0003", false, true, 0.0f);
        mFlags[42] = true;
        }
        }
    }

    // ---- +0x8104  232 bytes ----
    if (!mFlags[43]) {
        if (!MissionUtility::IsAlive(mHandles[88])
            || !MissionUtility::IsAlive(mHandles[89])
            || !MissionUtility::IsAlive(mHandles[90])
            || !MissionUtility::IsAlive(mHandles[91])) {
        MissionUtility::ObjectiveComplete(mInts[9]);
        MissionUtility::ObjectiveComplete(mInts[6]);
        mInts[8] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0009");
        MissionUtility::DisplayText("missions.Kashyyyk2.objective.str0009", 6.0f, -1.0f);
        MissionUtility::DamageObject(mHandles[88], 99999.0f, 99999.0f);
        MissionUtility::DamageObject(mHandles[89], 99999.0f, 99999.0f);
        MissionUtility::DamageObject(mHandles[90], 99999.0f, 99999.0f);
        MissionUtility::DamageObject(mHandles[91], 99999.0f, 99999.0f);
        MissionUtility::Objectify("patrol5_attack", 0, "missions.Kashyyyk2.marker.str0000", true, false, 0.0f, 2.0f);
        MissionUtility::RemoveObject(mHandles[71]);
        mFlags[43] = true;
        }
    }

    // ---- +0x81ec  36 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[72]) < 1) {
    if (!mFlags[59]) {
    mFlags[59] = true;
    }
    }

    // ---- +0x8210  428 bytes ----
    if (MissionUtility::GetDistance(mHandles[202], "patrol5_attack", 0) < 100.0f) {
    if (!mFlags[50]) {
    if (mFlags[43]) {
    MissionUtility::RemoveObject(mHandles[385]);
    MissionUtility::RemoveObject(mHandles[386]);
    mHandles[388] = MissionUtility::CreateObjectWithRotation("kas_crea_maru_player", "EndCinAnakinPath", 0, "EndCinAnakin", 0, -1, -1);
    mHandles[389] = MissionUtility::CreateObjectWithRotation("kas_crea_maru_bera", "EndCinBeraPath", 0, "EndCinBera", 0, -1, -1);
    MissionUtility::SetApplyDynamics(mHandles[388], true);
    MissionUtility::SetApplyDynamics(mHandles[389], true);
    MissionUtility::SetAnimation(mHandles[388], "mount", 1.0f, 1);
    MissionUtility::SetAnimation(mHandles[389], "mount", 1.0f, 1);
    BeginTimer(mTimer27);
    MissionUtility::QueueSound("bkk13_21", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("ask13_22", 1.0f, 0.0f, 0.0f, "", 0, "");
    mHandles[80] = MissionUtility::RunCin("Cin5", true, true);
    MissionUtility::SetTeamNum(mHandles[202], 0);
    MissionUtility::PlayMusic("EP4_V2_T09_05", true);
    mHandles[80] = MissionUtility::RunCin("Cin5", true, true);
    MissionUtility::ObjectiveComplete(mInts[8]);
    if (MissionUtility::GetFlockCount(mHandles[0]) < 1) {
    MissionUtility::BonusObjectiveComplete(mInts[12], true);
    }
    if (MissionUtility::GetFlockCount(mHandles[0]) > 1) {
    MissionUtility::BonusObjectiveFailed(mInts[12]);
    }
    if (!mFlags[20]) {
    MissionUtility::BonusObjectiveFailed(mInts[10]);
    }
    mFlags[50] = true;
    }
    }
    }

    // ---- +0x83bc  72 bytes ----
    if (mTimer27 > 5.0f) {
    MissionUtility::Goto(mHandles[388], "EndCinAnakinPath", false);
    MissionUtility::Goto(mHandles[389], "EndCinBeraPath", false);
    StopTimer(mTimer27);
    mTimer27 = 0.0f;
    }

    // ---- +0x8404  672 bytes ----
    if (mFlags[43] && !mFlags[41]) {
        mHandles[168] = MissionUtility::CreateObject("cis_tank_assault", "gauntlet_spawn", 8, "", 2, -1);
        mHandles[174] = MissionUtility::CreateObject("cis_bike_speeder", "gauntlet_spawn", 1, "", 2, -1);
        mHandles[175] = MissionUtility::CreateObject("cis_bike_speeder", "gauntlet_spawn", 2, "", 2, -1);
        mHandles[176] = MissionUtility::CreateObject("cis_bike_speeder", "gauntlet_spawn", 3, "", 2, -1);
        mHandles[177] = MissionUtility::CreateObject("cis_bike_speeder", "gauntlet_spawn", 4, "", 2, -1);
        mHandles[179] = MissionUtility::CreateObject("cis_walk_small", "gauntlet_spawn", 6, "", 2, -1);
        mHandles[181] = MissionUtility::CreateObject("cis_walk_small", "gauntlet_spawn", 0, "", 2, -1);
        mHandles[183] = MissionUtility::CreateObject("cis_walk_small", "gauntlet_spawn", 11, "", 2, -1);
        mFlags[41] = true;
    }

    // ---- +0x86a4  80 bytes ----
    if (mFlags[50]) {
        if (!MissionUtility::IsCinRunning(mHandles[80])) {
        if (!mFlags[0]) {
        if (MissionUtility::GetGameClock() < 510.0f) {
        MissionUtility::BonusObjectiveComplete(mInts[11], true);
        }
        MissionUtility::MissionSuccess();
        mFlags[0] = true;
        }
        }
    }

    // ---- +0x86f4  64 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[16], "droid_ambush_trigger")) {
    if (!mFlags[19]) {
    MissionUtility::SetEnemies(1, 14);
    MissionUtility::SetEnemies(1, 15);
    mFlags[19] = true;
    }
    }

    // ---- +0x8734  1116 bytes ----
    if (MissionUtility::IsWaveSpawned("comm_jedi")) {
    if (mFlags[6]) {
    mHandles[111] = MissionUtility::GetHandle("comm_droid20");
    mHandles[112] = MissionUtility::GetHandle("comm_droid21");
    mHandles[113] = MissionUtility::GetHandle("comm_droid22");
    mHandles[114] = MissionUtility::GetHandle("comm_droid23");
    mHandles[115] = MissionUtility::GetHandle("comm_droid24");
    mHandles[116] = MissionUtility::GetHandle("comm_droid25");
    mHandles[117] = MissionUtility::GetHandle("comm_droid26");
    mHandles[118] = MissionUtility::GetHandle("comm_droid27");
    mHandles[119] = MissionUtility::GetHandle("comm_droid28");
    mHandles[120] = MissionUtility::GetHandle("comm_droid29");
    mHandles[121] = MissionUtility::GetHandle("comm_droid30");
    mHandles[122] = MissionUtility::GetHandle("comm_droid31");
    mHandles[123] = MissionUtility::GetHandle("comm_droid32");
    mHandles[124] = MissionUtility::GetHandle("comm_droid33");
    mHandles[125] = MissionUtility::GetHandle("comm_droid34");
    mHandles[126] = MissionUtility::GetHandle("comm_droid35");
    mHandles[127] = MissionUtility::GetHandle("comm_droid36");
    mHandles[128] = MissionUtility::GetHandle("comm_droid37");
    mHandles[129] = MissionUtility::GetHandle("comm_droid38");
    mHandles[130] = MissionUtility::GetHandle("comm_droid39");
    mHandles[131] = MissionUtility::GetHandle("comm_droid40");
    mHandles[132] = MissionUtility::GetHandle("comm_droid41");
    mHandles[133] = MissionUtility::GetHandle("comm_droid42");
    mHandles[134] = MissionUtility::GetHandle("comm_droid43");
    mHandles[135] = MissionUtility::GetHandle("comm_droid44");
    mHandles[136] = MissionUtility::GetHandle("comm_droid45");
    mHandles[137] = MissionUtility::GetHandle("comm_droid46");
    mHandles[138] = MissionUtility::GetHandle("comm_droid47");
    mHandles[139] = MissionUtility::GetHandle("comm_droid48");
    mHandles[140] = MissionUtility::GetHandle("comm_droid49");
    MissionUtility::SetAttackRange(mHandles[111], 60);
    MissionUtility::SetAttackRange(mHandles[112], 60);
    MissionUtility::SetAttackRange(mHandles[113], 60);
    MissionUtility::SetAttackRange(mHandles[114], 60);
    MissionUtility::SetAttackRange(mHandles[115], 60);
    MissionUtility::SetAttackRange(mHandles[116], 60);
    MissionUtility::SetAttackRange(mHandles[117], 60);
    MissionUtility::SetAttackRange(mHandles[118], 60);
    MissionUtility::SetAttackRange(mHandles[119], 60);
    MissionUtility::SetAttackRange(mHandles[120], 60);
    MissionUtility::SetAttackRange(mHandles[121], 60);
    MissionUtility::SetAttackRange(mHandles[122], 60);
    MissionUtility::SetAttackRange(mHandles[123], 60);
    MissionUtility::SetAttackRange(mHandles[124], 60);
    MissionUtility::SetAttackRange(mHandles[125], 60);
    MissionUtility::SetAttackRange(mHandles[126], 60);
    MissionUtility::SetAttackRange(mHandles[127], 60);
    MissionUtility::SetAttackRange(mHandles[128], 60);
    MissionUtility::SetAttackRange(mHandles[129], 60);
    MissionUtility::SetAttackRange(mHandles[130], 60);
    MissionUtility::SetAttackRange(mHandles[131], 60);
    MissionUtility::SetAttackRange(mHandles[132], 60);
    MissionUtility::SetAttackRange(mHandles[133], 60);
    MissionUtility::SetAttackRange(mHandles[134], 60);
    MissionUtility::SetAttackRange(mHandles[135], 60);
    MissionUtility::SetAttackRange(mHandles[136], 60);
    MissionUtility::SetAttackRange(mHandles[137], 60);
    MissionUtility::SetAttackRange(mHandles[138], 60);
    MissionUtility::SetAttackRange(mHandles[139], 60);
    MissionUtility::SetAttackRange(mHandles[140], 60);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[111]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[112]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[113]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[114]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[115]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[116]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[117]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[118]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[119]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[120]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[121]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[122]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[123]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[124]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[125]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[126]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[127]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[128]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[129]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[130]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[131]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[132]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[133]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[134]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[135]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[136]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[137]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[138]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[139]);
    MissionUtility::AddFlockMember(mHandles[0], mHandles[140]);
    mFlags[6] = true;
    }
    }

    // ---- +0x8b90  340 bytes ----
    if (mFlags[49]) {
        if (!MissionUtility::IsCinRunning(mHandles[79])) {
        if (!mFlags[18]) {
        MissionUtility::SetMapZoom(85.0f, 9999999.0f);
        MissionUtility::PlayMusic("EP2_V1_T12_03", false);
        mHandles[16] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin", "jedi_spawn", 0, "player_anakin", 1, -1, -1);
        MissionUtility::SetAsPlayer(mHandles[16], 0);
        MissionUtility::BeginWave("comm_jedi");
        MissionUtility::AddTurnAroundRegion("jedi_turnaround", "runaway_droid_spawn", 0, 0, 0, 0);
        mHandles[11] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "runaway_droid_spawn", 0, "runaway_droid", 2, -1, -1);
        MissionUtility::Goto(mHandles[11], "droid_runaway", true);
        MissionUtility::SetVelocForward(mHandles[202], 0.0f);
        MissionUtility::Wait(mHandles[202]);
        mSavedHealth = MissionUtility::GetCurHealth(mHandles[202]);
        MissionUtility::SetCurHealth(mHandles[202], 999999.0f);
        MissionUtility::SetTeamNum(mHandles[202], 30);
        if (mFlags[32]) {
        MissionUtility::SetEnemies(30, 8);
        }
        MissionUtility::SetAlliance(1, 30);
        mFlags[22] = false;
        MissionUtility::Objectify("comm_jedi_goto", 0, "missions.Kashyyyk2.marker.str0006", true, false, 0.0f, 2.0f);
        mFlags[18] = true;
        }
        }
    }

    // ---- +0x8ce4  80 bytes ----
    if (mFlags[18] && !mFlags[15]) {
        if (!MissionUtility::IsAlive(mHandles[16])) {
        if (!mFlags[17]) {
        mHandles[87] = MissionUtility::RunCin("deathcin_jedi", true, false);
        mFlags[17] = true;
        }
        }
    }

    // ---- +0x8d34  52 bytes ----
    if (mFlags[17]) {
        if (!MissionUtility::IsCinRunning(mHandles[87])) {
        if (!mFlags[1]) {
        MissionUtility::MissionFailure();
        mFlags[1] = true;
        }
        }
    }

    // ---- +0x8d68  232 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[6]) < 1) {
    if (MissionUtility::IsInsideRegion(mHandles[16], "comm_jedi_trigger")) {
    if (!mFlags[53]) {
    mHandles[83] = MissionUtility::RunCin("Cin4", true, true);
    MissionUtility::ObjectiveComplete(mInts[5]);
    mHandles[387] = MissionUtility::CreateObjectWithRotation("REP_inf_anakin_cin", "MidCin4AnakinPath", 0, "MidCin4Anakin", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[387], 20.0f);
    MissionUtility::SetCollidable(mHandles[387], false);
    MissionUtility::Goto(mHandles[387], "MidCin4AnakinPath", false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[387]);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::PlayMusic("EP4_V2_T09_02", true);
    BeginTimer(mTimer26);
    BeginTimer(mTimer31);
    MissionUtility::MoveObject(mHandles[16], "OpenCinWookiePath1", 0, true);
    mFlags[53] = true;
    }
    }
    }

    // ---- +0x8e50  76 bytes ----
    if (mTimer31 > 5.0f) {
    MissionUtility::StartSound("Ifc_typing_B_lp01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    StopTimer(mTimer31);
    mTimer31 = 0.0f;
    }

    // ---- +0x8e9c  56 bytes ----
    if (mTimer26 > 20.0f) {
    StopTimer(mTimer26);
    mTimer26 = 0.0f;
    MissionUtility::Goto(mHandles[387], "MidCin4AnakinPath1", false);
    }

    // ---- +0x8ed4  3356 bytes ----
    if (mFlags[53]) {
        if (!MissionUtility::IsCinRunning(mHandles[83])) {
        if (!mFlags[16]) {
        MissionUtility::AddHealthBox("jedi_health_spawn", 0, -1.0f);
        MissionUtility::StartSound("klaxon1", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::PlayMusic("EP4_V1_T03_02", false);
        mInts[7] = MissionUtility::AddObjective("missions.Kashyyyk2.objective.str0012");
        MissionUtility::DisplayText("missions.Kashyyyk2.text.str0012", 6.0f, -1.0f);
        StopTimer(mTimer26);
        mTimer26 = 0.0f;
        MissionUtility::RemoveObject(mHandles[387]);
        MissionUtility::MoveObjectWithRotation(mHandles[16], "jedi_after_message_sent", 0, true);
        MissionUtility::SetTeamNum(mHandles[16], 1);
        mFlags[16] = true;
        mHandles[92] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn", 0, "", 2, -1);
        mHandles[93] = MissionUtility::CreateObject("cis_inf_sbdroid", "comm_afterjedi_spawn", 1, "", 2, -1);
        mHandles[95] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn", 3, "", 2, -1);
        mHandles[96] = MissionUtility::CreateObject("cis_inf_sbdroid", "comm_afterjedi_spawn", 4, "", 2, -1);
        mHandles[97] = MissionUtility::CreateObject("cis_walk_small_jedi", "comm_afterjedi_spawn", 5, "", 2, -1);
        mHandles[98] = MissionUtility::CreateObject("cis_inf_sbdroid", "comm_afterjedi_spawn", 6, "", 2, -1);
        mHandles[100] = MissionUtility::CreateObject("cis_inf_sbdroid", "comm_afterjedi_spawn", 8, "", 2, -1);
        mHandles[101] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn", 9, "", 2, -1);
        mHandles[103] = MissionUtility::CreateObject("cis_inf_sbdroid", "comm_afterjedi_spawn", 11, "", 2, -1);
        mHandles[105] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn", 13, "", 2, -1);
        mHandles[107] = MissionUtility::CreateObject("cis_inf_sbdroid", "comm_afterjedi_spawn", 15, "", 2, -1);
        mHandles[108] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn", 16, "", 2, -1);
        mHandles[109] = MissionUtility::CreateObject("cis_inf_sbdroid", "comm_afterjedi_spawn", 17, "", 2, -1);
        mHandles[110] = MissionUtility::CreateObject("cis_walk_small_jedi", "comm_afterjedi_spawn", 18, "", 2, -1);
        mHandles[141] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 0, "", 2, -1);
        mHandles[142] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 1, "", 2, -1);
        mHandles[143] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 2, "", 2, -1);
        mHandles[144] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 3, "", 2, -1);
        mHandles[145] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 4, "", 2, -1);
        mHandles[146] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 5, "", 2, -1);
        mHandles[147] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 6, "", 2, -1);
        mHandles[148] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 7, "", 2, -1);
        mHandles[149] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 8, "", 2, -1);
        mHandles[150] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 9, "", 2, -1);
        mHandles[151] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 10, "", 2, -1);
        mHandles[152] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 11, "", 2, -1);
        mHandles[153] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 12, "", 2, -1);
        mHandles[154] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 13, "", 2, -1);
        mHandles[155] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 14, "", 2, -1);
        mHandles[156] = MissionUtility::CreateObject("cis_inf_droid", "comm_afterjedi_spawn2", 15, "", 2, -1);
        MissionUtility::SetAttackRange(mHandles[92], 60);
        MissionUtility::SetAttackRange(mHandles[93], 60);
        MissionUtility::SetAttackRange(mHandles[95], 60);
        MissionUtility::SetAttackRange(mHandles[96], 60);
        MissionUtility::SetAttackRange(mHandles[97], 60);
        MissionUtility::SetAttackRange(mHandles[98], 60);
        MissionUtility::SetAttackRange(mHandles[99], 60);
        MissionUtility::SetAttackRange(mHandles[101], 60);
        MissionUtility::SetAttackRange(mHandles[103], 60);
        MissionUtility::SetAttackRange(mHandles[105], 60);
        MissionUtility::SetAttackRange(mHandles[107], 60);
        MissionUtility::SetAttackRange(mHandles[108], 60);
        MissionUtility::SetAttackRange(mHandles[109], 60);
        MissionUtility::SetAttackRange(mHandles[110], 60);
        MissionUtility::SetAttackRange(mHandles[141], 60);
        MissionUtility::SetAttackRange(mHandles[142], 60);
        MissionUtility::SetAttackRange(mHandles[143], 60);
        MissionUtility::SetAttackRange(mHandles[144], 60);
        MissionUtility::SetAttackRange(mHandles[145], 60);
        MissionUtility::SetAttackRange(mHandles[146], 60);
        MissionUtility::SetAttackRange(mHandles[147], 60);
        MissionUtility::SetAttackRange(mHandles[148], 60);
        MissionUtility::SetAttackRange(mHandles[149], 60);
        MissionUtility::SetAttackRange(mHandles[150], 60);
        MissionUtility::SetAttackRange(mHandles[151], 60);
        MissionUtility::SetAttackRange(mHandles[152], 60);
        MissionUtility::SetAttackRange(mHandles[153], 60);
        MissionUtility::SetAttackRange(mHandles[154], 60);
        MissionUtility::SetAttackRange(mHandles[155], 60);
        MissionUtility::SetAttackRange(mHandles[156], 60);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[92]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[93]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[95]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[96]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[97]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[98]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[100]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[101]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[103]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[105]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[107]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[108]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[109]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[110]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[141]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[142]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[143]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[144]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[145]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[146]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[147]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[148]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[149]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[150]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[151]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[152]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[153]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[154]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[155]);
        MissionUtility::AddFlockMember(mHandles[0], mHandles[156]);
        MissionUtility::RemoveObjectify("comm_jedi_goto", 0);
        MissionUtility::Objectify(mHandles[202], "missions.Kashyyyk2.marker.str0005", true, true, 0.0f);
        }
        }
    }

    // ---- +0x9bf0  228 bytes ----
    if (mFlags[16]) {
        if (MissionUtility::GetDistance(mHandles[16], mHandles[202]) < 25.0f) {
        if (!mFlags[54]) {
        MissionUtility::QueueSound("ask13_36", 1.0f, 0.0f, 0.0f, "", 0, "");
        mHandles[84] = MissionUtility::RunCin("Cin9", true, true);
        MissionUtility::ObjectiveComplete(mInts[7]);
        mFlags[15] = true;
        mFlags[54] = true;
        mHandles[387] = MissionUtility::CreateObjectWithRotation("REP_inf_anakin_cin", "MidCin4AnakinPathAAT2", 0, "MidCin4Anakin", 0, -1, -1);
        MissionUtility::SetVelocForward(mHandles[387], 20.0f);
        MissionUtility::SetCollidable(mHandles[387], false);
        MissionUtility::Goto(mHandles[387], "MidCin4AnakinPathAAT2", false);
        MissionUtility::MoveObject(mHandles[16], "OpenCinWookiePath1", 0, true);
        MissionUtility::SetTeamNum(mHandles[16], 0);
        }
        }
    }

    // ---- +0x9cd4  56 bytes ----
    if (mFlags[54] && !mFlags[254]) {
        if (!MissionUtility::IsCinRunning(mHandles[84])) {
        mFlags[254] = true;
        MissionUtility::RemoveObject(mHandles[387]);
        }
    }

    // ---- +0x9d0c  52 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[16], "team13_switch")) {
    if (!mFlags[14]) {
    MissionUtility::SetEnemies(1, 13);
    mFlags[14] = true;
    }
    }

    // ---- +0x9d40  212 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[202], "comm_aat_trigger")) {
    if (!mFlags[49]) {
    MissionUtility::RemoveObjectify("aat_comm_goto", 0);
    mHandles[79] = MissionUtility::RunCin("Cin8", true, true);
    MissionUtility::MoveObjectWithRotation(mHandles[202], "aat_comm_goto", 0, true);
    mHandles[387] = MissionUtility::CreateObjectWithRotation("REP_inf_anakin_cin", "MidCin4AnakinPathAAT1", 0, "MidCin4Anakin", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[387], 20.0f);
    MissionUtility::SetCollidable(mHandles[387], false);
    MissionUtility::Goto(mHandles[387], "MidCin4AnakinPathAAT1", false);
    MissionUtility::PlayMusic("EP4_V2_T09_02", true);
    mFlags[49] = true;
    MissionUtility::MoveObject(mHandles[16], "OpenCinWookiePath1", 0, true);
    MissionUtility::SetTeamNum(mHandles[16], 0);
    }
    }

    // ---- +0x9e14  92 bytes ----
    if (mFlags[49]) {
    if (!mFlags[253]) {
    if (!MissionUtility::IsCinRunning(mHandles[79])) {
    mFlags[253] = true;
    MissionUtility::RemoveObject(mHandles[387]);
    MissionUtility::MoveObject(mHandles[16], "jedi_spawn", 0, true);
    MissionUtility::SetTeamNum(mHandles[16], 1);

        break;
    case 8:
    // ---- +0x9e70  212 bytes ----
    if (mFlags[82]) {
        MissionUtility::Objectify(mHandles[18], "missions.Kashyyyk2.marker.str0002", false, true, 0.0f);
        MissionUtility::Goto(mHandles[18], "chase_trap", true);
        MissionUtility::SetVelocForward(mHandles[158], 40.0f);
        MissionUtility::SetVelocForward(mHandles[159], 40.0f);
        MissionUtility::SetVelocForward(mHandles[160], 40.0f);
        MissionUtility::SetVelocForward(mHandles[161], 40.0f);
        MissionUtility::SetVelocForward(mHandles[162], 40.0f);
        MissionUtility::SetVelocForward(mHandles[163], 40.0f);
        MissionUtility::SetVelocForward(mHandles[164], 40.0f);
        MissionUtility::SetVelocForward(mHandles[165], 40.0f);
        MissionUtility::SetVelocForward(mHandles[166], 40.0f);
        MissionUtility::SetVelocForward(mHandles[167], 40.0f);
        MissionUtility::Objectify("chase_trap", 0, "missions.Kashyyyk2.marker.str0001", false, false, 0.0f, 2.0f);
        mFlags[82] = false;
    }

    // ---- +0x9f44  180 bytes ----
    if (MissionUtility::GetDistance(mHandles[17], "chase_trap", 0) < 100.0f) {
    if (!mFlags[61]) {
    MissionUtility::SetCurHealth(mHandles[158], 1.0f);
    MissionUtility::SetCurHealth(mHandles[159], 1.0f);
    MissionUtility::SetCurHealth(mHandles[160], 1.0f);
    MissionUtility::SetCurHealth(mHandles[161], 1.0f);
    MissionUtility::SetCurHealth(mHandles[162], 1.0f);
    MissionUtility::SetCurHealth(mHandles[163], 1.0f);
    MissionUtility::SetCurHealth(mHandles[164], 1.0f);
    MissionUtility::SetCurHealth(mHandles[165], 1.0f);
    MissionUtility::SetCurHealth(mHandles[166], 1.0f);
    MissionUtility::SetCurHealth(mHandles[167], 1.0f);
    MissionUtility::RemoveObjectify("chase_trap", 0);
    mFlags[61] = true;
    }
    }

    // ---- +0x9ff8  56 bytes ----
    if (MissionUtility::GetFlockCount(mHandles[157]) < 1) {
    if (!mFlags[52]) {
    mHandles[82] = MissionUtility::RunCin("Cin7", true, true);
    mFlags[52] = true;
    }
    }

    // ---- +0xa030  52 bytes ----
    if (mFlags[52]) {
        if (!MissionUtility::IsCinRunning(mHandles[82])) {
        if (!mFlags[0]) {
        MissionUtility::MissionSuccess();
        mFlags[0] = true;
        }
        }
    }

        break;
    }
    }
    }
    }
    }
    }
    }
}

// `inline` for the same reason as Geonosis2Script.cpp's `Quat::Quat(float,float,float,float)`:
// the shipped symbol is WEAK (`__ct__6MatrixFRC6Matrix`, 0x801e357c, 40 B), which is what
// CodeWarrior emits for an inline function it could not inline, not the GLOBAL a plain
// namespace-scope definition gets. Every one of these little value-type members is weak in the
// retail symbol table -- `Vector::Vector(fff)`, `Vector::operator=`, `Matrix::operator=`,
// `Quat::Quat()` -- so they were all inline in the shared header. It still has to sit after
// `Execute` so the three call sites are calls.
inline Matrix::Matrix(const Matrix &o)
{
    *this = o;
}

void Kashyyyk2Script::Setup()
{
        mFlags[21] = true;
        mFlags[34] = false;
        mFlags[76] = true;
        mFlags[77] = true;
        mFlags[78] = true;
        mFlags[79] = true;
        mFlags[80] = true;
        mFlags[81] = true;
        mFlags[82] = true;
        mHandles[15] = MissionUtility::GetPlayerHandle(0);
        mHandles[18] = MissionUtility::GetHandle("bera_maru");
        mHandles[8] = MissionUtility::GetHandle("comm_turret1");
        mHandles[9] = MissionUtility::GetHandle("comm_turret2");
        mHandles[12] = MissionUtility::GetHandle("r1");
        mHandles[13] = MissionUtility::GetHandle("r2");
        mHandles[14] = MissionUtility::GetHandle("r3");
        mHandles[1] = MissionUtility::GetHandle("gnasp1");
        mHandles[2] = MissionUtility::GetHandle("gnasp2");
        mHandles[3] = MissionUtility::GetHandle("gnasp3");
        mHandles[4] = MissionUtility::GetHandle("gnasp4");
        mHandles[5] = MissionUtility::GetHandle("gnasp5");
        mHandles[184] = MissionUtility::GetHandle("patrol1_stap1");
        mHandles[185] = MissionUtility::GetHandle("patrol1_stap2");
        mHandles[186] = MissionUtility::GetHandle("patrol1_stap3");
        mHandles[187] = MissionUtility::GetHandle("patrol1_stap4");
        mHandles[188] = MissionUtility::GetHandle("patrol2_stap1");
        mHandles[189] = MissionUtility::GetHandle("patrol2_stap2");
        mHandles[190] = MissionUtility::GetHandle("patrol2_tank");
        mHandles[191] = MissionUtility::GetHandle("patrol3_stap1");
        mHandles[192] = MissionUtility::GetHandle("patrol3_stap2");
        mHandles[193] = MissionUtility::GetHandle("patrol3_stap3");
        mHandles[194] = MissionUtility::GetHandle("patrol3_stap4");
        mHandles[195] = MissionUtility::GetHandle("patrol4_stap1");
        mHandles[196] = MissionUtility::GetHandle("patrol4_stap2");
        mHandles[197] = MissionUtility::GetHandle("patrol5_aat");
        mHandles[198] = MissionUtility::GetHandle("patrol5_stap1");
        mHandles[199] = MissionUtility::GetHandle("patrol5_stap2");
        mHandles[200] = MissionUtility::GetHandle("patrol5_stap3");
        mHandles[203] = MissionUtility::GetHandle("sbdroid");
        mHandles[204] = MissionUtility::GetHandle("hangar");
        mHandles[201] = MissionUtility::GetHandle("aat");
        mHandles[19] = MissionUtility::GetHandle("patrol5_sbdroid1");
        mHandles[20] = MissionUtility::GetHandle("patrol5_sbdroid2");
        mHandles[21] = MissionUtility::GetHandle("patrol5_sbdroid3");
        mHandles[22] = MissionUtility::GetHandle("patrol5_sbdroid4");
        mHandles[23] = MissionUtility::GetHandle("patrol5_sbdroid5");
        mHandles[230] = MissionUtility::GetHandle("nest1");
        mHandles[246] = MissionUtility::GetHandle("nest2");
        mHandles[262] = MissionUtility::GetHandle("nest3");
        mHandles[278] = MissionUtility::GetHandle("nest4");
        mHandles[294] = MissionUtility::GetHandle("nest5");
        mHandles[310] = MissionUtility::GetHandle("nest6");
        mHandles[326] = MissionUtility::GetHandle("nest7");
        mHandles[342] = MissionUtility::GetHandle("nest8");
        mHandles[358] = MissionUtility::GetHandle("nest9");
        mHandles[70] = MissionUtility::GetHandle("comm_gate");
        mHandles[205] = MissionUtility::GetHandle("sbdroid1");
        mHandles[206] = MissionUtility::GetHandle("sbdroid2");
        mHandles[207] = MissionUtility::GetHandle("sbdroid3");
        mHandles[208] = MissionUtility::GetHandle("sbdroid4");
        mHandles[209] = MissionUtility::GetHandle("sbdroid5");
        mHandles[210] = MissionUtility::GetHandle("sbdroid6");
        mHandles[211] = MissionUtility::GetHandle("sbdroid7");
        mHandles[212] = MissionUtility::GetHandle("sbdroid8");
        mHandles[213] = MissionUtility::GetHandle("sbdroid9");
        mHandles[214] = MissionUtility::GetHandle("sbdroid10");
        mHandles[215] = MissionUtility::GetHandle("sbdroid11");
        mHandles[216] = MissionUtility::GetHandle("sbdroid12");
        mHandles[217] = MissionUtility::GetHandle("sbdroid13");
        mHandles[218] = MissionUtility::GetHandle("sbdroid14");
        mHandles[219] = MissionUtility::GetHandle("sbdroid15");
        mHandles[220] = MissionUtility::GetHandle("sbdroid16");
        mHandles[221] = MissionUtility::GetHandle("sbdroid17");
        mHandles[222] = MissionUtility::GetHandle("sbdroid18");
        mHandles[223] = MissionUtility::GetHandle("sbdroid19");
        mHandles[224] = MissionUtility::GetHandle("sbdroid20");
        mHandles[225] = MissionUtility::GetHandle("comm_aat1");
        mHandles[226] = MissionUtility::GetHandle("comm_aat2");
        mHandles[227] = MissionUtility::GetHandle("comm_aat3");
        mHandles[228] = MissionUtility::GetHandle("comm_aat4");
        mHandles[229] = MissionUtility::GetHandle("comm_aat5");
        mHandles[88] = MissionUtility::GetHandle("power1");
        mHandles[89] = MissionUtility::GetHandle("power2");
        mHandles[90] = MissionUtility::GetHandle("power3");
        mHandles[91] = MissionUtility::GetHandle("power4");
        mHandles[75] = MissionUtility::GetHandle("comm");
        mHandles[202] = MissionUtility::GetHandle("aat_jacked");
        MissionUtility::PreloadConfig("kas_crea_gnasp");
        MissionUtility::PreloadConfig("kas_crea_gnasp_encircle");
        MissionUtility::PreloadConfig("cis_tank_assault_player");
        MissionUtility::PreloadConfig("cis_tank_fighter");
        MissionUtility::PreloadConfig("rep_fly_landing");
        MissionUtility::PreloadConfig("rep_inf_anakin");
        MissionUtility::PreloadConfig("rep_fly_landed");
        MissionUtility::PreloadConfig("rep_inf_bera");
        MissionUtility::PreloadConfig("kas_bldg_power_dest");
        MissionUtility::PreloadConfig("kas_bldg_comm_dest");
        MissionUtility::PreloadConfig("rep_inf_anakin_cin");
        MissionUtility::PreloadConfig("kas_crea_maru");
        MissionUtility::PreloadConfig("cis_walk_small");
        MissionUtility::PreloadConfig("kas_crea_maru_wookiee");
}

SPMission *Kashyyyk2BuildMission()
{
    return new Kashyyyk2Script();
}
