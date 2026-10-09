// Kashyyyk1Script.cpp -- reconstruction of a shipped mission script.
//
// The second campaign mission attempted and the next by size: 24,684 bytes, of which
// `Execute` alone is 22,792 -- twice the whole of `Raxus1Script`. Same shape as Raxus1
// (no tuning tables, 5,328 bytes of .rodata that is nothing but the string pool, 64 bytes
// of .data that is nothing but the vtable and the RTTI records) with three additions:
//
//   * a file-scope helper, `GetDistToHarvester`, which sprintf()s marker names in a loop;
//   * `Matrix` and `Vector`, whose *implicit* copy-assignment operators are emitted into
//     this TU because `GetLocation`/`GetPosition` return them by value;
//   * 51 by-value `Quat` arguments to `CreateObject` -- 43 defaulted, 8 written out. See
//     analysis/phase5_status.md: that split is what identified the default argument.

class Quat
{
public:
    Quat();
    Quat(float, float, float, float);

    float s, x, y, z;
};

// Returned by value from GetPosition/GetLocation and assigned to a member, which is what
// pulls their implicit `operator=` into this TU -- 40 and 28 bytes, between `Execute` and
// `GetDistToHarvester` in the shipped .text.
class Vector
{
public:
    float x, y, z;
};

class Matrix
{
public:
    float m[4][4];
};

// Twelve bytes, constructed out of line -- pinned by the thirty-one `__ct__5TimerFv` calls
// in the constructor.
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
void ResumeTimer(Timer &t);

namespace MissionUtility
{
    int    AddBonusObjective(const char*);
    void   AddFlockMember(int, int);
    int    AddObjective(const char*);
    void   AddTurnAroundRegion(const char*, const char*, const char*, const char*,
                               const char*, const char*);
    void   AttackTarget(int, int, bool, bool, bool, bool);
    void   BeginWave(const char*);
    void   BonusObjectiveComplete(int, bool);
    void   BonusObjectiveFailed(int);
    int    CreateFlock();
    int    CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    int    CreateObject(const char*, const char*, const char*, int, int, int);
    // The default argument. Forty-three of this mission's fifty-one call sites stop after
    // `b` and share one 16-byte argument slot; the eight that write a rotation out get a
    // private slot each. Nothing else reproduces that split -- see phase5_status.md.
    int    CreateObject(const char*, const char*, int, const char*, int, int,
                        Quat facing = Quat(), int c = -1);
    int    CreateObject(const char*, const Matrix&, const char*, int, int, int);
    int    CreateObject(const char*, const Vector&, const char*, int, int, Quat, int);
    void    DamageObject(int, float, float);
    void   DisplayText(const char*, float, float);
    void   EvictConfig(const char*);
    void   Fire(int, bool, bool, bool);
    void   FlushSoundQueue();
    int    GetCinId(int);
    float  GetCurHealth(int);
    float  GetDistance(int, const char*, int);
    float  GetDistance(int, int);
    float  GetDistance(int, int, const char*);
    float  GetGameClock();
    int    GetHandle(const char*);
    Matrix GetLocation(int);
    int    GetPlayerHandle(int);
    Vector GetPosition(int);
    int    GetWhoShotMe(int);
    void   GotoDirect(int, const char*, bool);
    void   Goto(int, const char*, bool);
    void   Goto(int, const char*, int);
    void   Goto(int, int);
    bool   IsAlive(int);
    bool   IsCinRunning(int);
    bool   IsInsideRegion(int, const char*);
    bool   IsSoundPlaying(int);
    bool   IsWaveSpawned(const char*);
    void   MissionFailure();
    void   MissionSuccess();
    void   MoveObjectWithRotation(int, const char*, int, bool);
    void   MoveObject(int, const char*, int, bool);
    void   ObjectifyPiece(int, const char*, const char*, bool, bool, float);
    void   Objectify(int, const char*, bool, bool, float);
    void   ObjectiveComplete(int);
    void   PlayMusic(const char*, bool);
    void   PreloadConfig(const char*);
    int   QueueSound(const char*, float, float, float, const char*, int, const char*);
    void   RemoveObject(int);
    void   RemoveObjectifyPiece(int, const char*);
    void   RemoveObjectify(int);
    void   RemoveTurnAroundRegion(const char*);
    int    RunCin(const char*, bool, bool);
    void   SetAccelThrust(int, float);
    void   SetAnimation(int, const char*, float, int);
    void   SetApplyDynamics(int, bool);
    void   SetAsPlayer(int, int);
    void   SetCollidable(int, bool);
    void   SetCurHealth(int, float);
    void   SetEnemiesOneWay(int, int);
    void   SetEnemies(int, int);
    void   SetFOV(float);
    void   SetImportantFlag(int, bool);
    void   SetMapZoom(float, float);
    void   SetMaxHealth(int, float);
    void   SetMusicLooping(bool);
    void   SetOverlayEnable(bool);
    void   SetQueueFlag(bool);
    void   SetVelocForward(int, float);
    void   SetVisible(int, bool);
    void   ShakeCamera(float, float, float);
    void   StartAmbiences(const char*, const char*, float, float);
    void   StartSoundAtPathPoint(const char*, const char*, int, bool, float);
    int   StartSound(const char*, bool, float, float, float, const char*, int, const char*);
    void   StopSound(int);
    void   Stop(int);
    void   TransIn(float, char);
    void   TransOut(float, char);
}

extern "C" int sprintf(char *, const char *, ...);

// All three levels of the hierarchy, in the shipped pool order. "DLLBase" is eight bytes
// with its terminator so it goes to .sdata2 and takes no .rodata space -- but it still
// occupies a place in the numbering, which tools/pool_check.py counts.
static const char *const kClassName = "Kashyyyk1Script";
static const char *const kRootName  = "DLLBase";
static const char *const kBaseName  = "SPMission";

// ---------------------------------------------------------------------------------------
// The classes. See analysis/mission_script_abi.md: DLLBase declares no data so its vptr
// sits at offset 0, SPMission's four (base, count) pairs occupy +0x04..+0x23, and the
// derived members start at +0x24.
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

// A file-scope helper: the closest harvester marker, by name. Defined after `Execute` --
// which is where it sits in the shipped .text -- so it needs a declaration here.
static float GetDistToHarvester(int who, int what, int count);

class Kashyyyk1Script : public SPMission
{
public:
    virtual ~Kashyyyk1Script();

    Kashyyyk1Script();

    virtual void Setup();
    virtual void Execute();

    char   mPad24;                       // +0x024
    bool   mFlags[155];                  // +0x025  the one-shot latches
    char   mPadC0[8];                    // +0x0c0
    int    mUnusedB;                     // +0x0c8  registered with count 0, never read
    Timer  mTimer0;                      // +0x0cc
    Timer  mTimer1,  mTimer2,  mTimer3,  mTimer4,  mTimer5;
    Timer  mTimer6,  mTimer7,  mTimer8,  mTimer9,  mTimer10;
    Timer  mTimer11, mTimer12, mTimer13, mTimer14, mTimer15;
    Timer  mTimer16, mTimer17, mTimer18, mTimer19, mTimer20;
    Timer  mTimer21, mTimer22, mTimer23, mTimer24, mTimer25;
    Timer  mTimer26, mTimer27, mTimer28, mTimer29, mTimer30;
    int    mPad240;                      // +0x240
    int    mHandles[170];                // +0x244
    int    mPad4EC[2];                   // +0x4ec
    int    mInts[16];                    // +0x4f4
    int    mPad534[4];                   // +0x534
    Vector mPosition;                    // +0x544  = GetPosition(command droid)
    int    mPad550[3];                   // +0x550
    Matrix mMatrixA;                     // +0x55c  all three zeroed by the constructor
    Matrix mMatrixB;                     // +0x59c  = GetLocation(the player)
    Matrix mMatrixC;                     // +0x5dc
    int    mPhase;                       // +0x61c  the switch in Execute
                                         //         -> sizeof == 0x620
};

// ---------------------------------------------------------------------------------------

Kashyyyk1Script::~Kashyyyk1Script()
{
}

// 22,792 bytes: the largest function attempted so far, and the same polled state machine as
// Raxus1Script -- one-shot blocks over 155 latches, 170 handles and 31 timers. Generated
// with tools/gen_block.py and checked a block at a time with tools/pool_check.py.
void Kashyyyk1Script::Execute()
{
    // ---- +0x0014  1148 bytes ----
    if (!mFlags[69]) {
    if (mFlags[2]) {
    MissionUtility::SetMusicLooping(true);
    MissionUtility::SetCollidable(mHandles[106], false);
    MissionUtility::SetVisible(mHandles[106], false);
    BeginTimer(mTimer8);
    mHandles[97] = MissionUtility::RunCin("Cin1", true, true);
    mFlags[67] = true;
    MissionUtility::SetCurHealth(mHandles[108], 1.0f);
    MissionUtility::SetVelocForward(mHandles[75], 0.0f);
    MissionUtility::SetVelocForward(mHandles[76], 0.0f);
    MissionUtility::SetVelocForward(mHandles[77], 0.0f);
    MissionUtility::SetVisible(mHandles[142], false);
    MissionUtility::SetCollidable(mHandles[142], false);
    MissionUtility::SetCollidable(mHandles[143], false);
    MissionUtility::AddTurnAroundRegion("escape_turnaround", "escape_turnaround_focus", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("escape_turnaround2", "escape_turnaround_focus", 0, 0, 0, 0);
    MissionUtility::AddTurnAroundRegion("escape_turnaround3", "escape_turnaround_focus", 0, 0, 0, 0);
    mHandles[12] = MissionUtility::CreateFlock();
    MissionUtility::SetMaxHealth(mHandles[87], 350.0f);
    MissionUtility::SetCurHealth(mHandles[87], 350.0f);
    MissionUtility::SetVelocForward(mHandles[62], 0.0f);
    MissionUtility::SetVelocForward(mHandles[63], 0.0f);
    MissionUtility::SetVelocForward(mHandles[64], 0.0f);
    MissionUtility::SetVelocForward(mHandles[65], 0.0f);
    MissionUtility::SetOverlayEnable(false);
    MissionUtility::SetImportantFlag(mHandles[2], true);
    MissionUtility::SetImportantFlag(mHandles[3], true);
    MissionUtility::SetImportantFlag(mHandles[4], true);
    MissionUtility::SetImportantFlag(mHandles[5], true);
    MissionUtility::SetImportantFlag(mHandles[6], true);
    MissionUtility::SetImportantFlag(mHandles[7], true);
    MissionUtility::SetImportantFlag(mHandles[8], true);
    MissionUtility::SetImportantFlag(mHandles[9], true);
    MissionUtility::SetImportantFlag(mHandles[10], true);
    MissionUtility::PlayMusic("EP6_V2_T04_02", true);
    MissionUtility::SetVelocForward(mHandles[57], 0.0f);
    MissionUtility::SetVelocForward(mHandles[58], 0.0f);
    mInts[2] = MissionUtility::AddBonusObjective("missions.Kashyyyk1.objective.str0002");
    mInts[6] = MissionUtility::AddBonusObjective("missions.Kashyyyk1.bonus.str0001");
    mInts[5] = MissionUtility::AddBonusObjective("missions.Kashyyyk1.bonus.str0003");
    MissionUtility::SetVelocForward(mHandles[16], 0.0f);
    MissionUtility::SetVelocForward(mHandles[17], 0.0f);
    MissionUtility::SetVelocForward(mHandles[18], 0.0f);
    MissionUtility::SetVelocForward(mHandles[19], 0.0f);
    MissionUtility::SetVelocForward(mHandles[20], 0.0f);
    MissionUtility::SetVelocForward(mHandles[21], 0.0f);
    MissionUtility::SetVelocForward(mHandles[22], 0.0f);
    MissionUtility::SetVelocForward(mHandles[23], 0.0f);
    MissionUtility::SetVelocForward(mHandles[24], 0.0f);
    MissionUtility::SetVelocForward(mHandles[25], 0.0f);
    MissionUtility::SetVelocForward(mHandles[26], 0.0f);
    MissionUtility::SetVelocForward(mHandles[27], 0.0f);
    MissionUtility::SetVelocForward(mHandles[28], 0.0f);
    MissionUtility::SetVelocForward(mHandles[29], 0.0f);
    MissionUtility::SetVelocForward(mHandles[30], 0.0f);
    MissionUtility::SetCollidable(mHandles[48], false);
    MissionUtility::SetCollidable(mHandles[49], false);
    MissionUtility::SetCollidable(mHandles[50], false);
    MissionUtility::SetCollidable(mHandles[51], false);
    MissionUtility::SetCollidable(mHandles[52], false);
    MissionUtility::SetCollidable(mHandles[53], false);
    MissionUtility::SetCollidable(mHandles[54], false);
    MissionUtility::StartSoundAtPathPoint("PropKash_waterfall_lp", "s_waterfall", 0, true, 50.0f);
    MissionUtility::StartSoundAtPathPoint("PropKash_waterfall_lp", "s_waterfall_2", 0, true, 50.0f);
    MissionUtility::StartSoundAtPathPoint("PropKash_stream_lp", "s_stream", 0, true, 30.0f);
    MissionUtility::StartSoundAtPathPoint("PropKash_stream_lp", "s_stream", 1, true, 30.0f);
    MissionUtility::StartSoundAtPathPoint("PropKash_stream_lp", "s_stream", 2, true, 30.0f);
    MissionUtility::StartSoundAtPathPoint("PropKash_stream_lp", "s_stream", 3, true, 30.0f);
    MissionUtility::StartSoundAtPathPoint("PropKash_stream_lp", "s_stream", 4, true, 30.0f);
    MissionUtility::StartSoundAtPathPoint("PropKash_stream_lp", "s_stream", 5, true, 30.0f);
    MissionUtility::StartSoundAtPathPoint("PropKash_stream_lp", "s_stream", 6, true, 30.0f);
    MissionUtility::SetEnemiesOneWay(6, 1);
    MissionUtility::SetMapZoom(100.0f, 999999.0f);
    MissionUtility::StartAmbiences("AmbKshyk_forestday01_pl2", "amb_kashyyk_day_stinger01", 10.0f, 30.0f);
    mFlags[67] = true;
    mFlags[2] = false;
    mHandles[93] = MissionUtility::CreateObject("cis_bike_speeder_empty", "anakin_stap_spawn", "", 0, -1, -1);
    mHandles[94] = MissionUtility::CreateObject("cis_bike_speeder_empty", "bera_stap_spawn", "", 0, -1, -1);
    mPhase = 0;
    }
    }

    // ---- +0x0490  56 bytes ----
    if (MissionUtility::GetGameClock() > 155.0f) {
    if (!mFlags[69]) {
    if (!mFlags[47]) {
    MissionUtility::BonusObjectiveFailed(mInts[5]);
    mFlags[47] = true;
    }
    }
    }

    // ---- +0x04c8  72 bytes ----
    if (!MissionUtility::IsAlive(mHandles[87])) {
    if (!mFlags[60]) {
    if (!mFlags[7]) {
    StopTimer(mTimer22);
    StopTimer(mTimer2);
    BeginTimer(mTimer27);
    mFlags[7] = true;
    }
    }
    }

    // ---- +0x0510  96 bytes ----
    if (!MissionUtility::IsAlive(mHandles[88])) {
    if (!mFlags[15]) {
    if (mFlags[60]) {
    if (!mFlags[64]) {
    if (!mFlags[7]) {
    StopTimer(mTimer22);
    StopTimer(mTimer2);
    BeginTimer(mTimer27);
    mFlags[7] = true;
    }
    }
    }
    }
    }

    // ---- +0x0570  44 bytes ----
    if (mTimer27 > 4.0f) {
    if (!mFlags[1]) {
    MissionUtility::MissionFailure();
    mFlags[1] = true;
    }
    }

    // ---- +0x059c  32 bytes ----
    if (mFlags[76] && !mFlags[19]) {
        mFlags[19] = true;
    }

    switch (mPhase) {
    case 0:

    // ---- +0x05d8  272 bytes ----
    if (mFlags[67]) {
        if (!MissionUtility::IsCinRunning(mHandles[97])) {
        if (mFlags[13]) {
        BeginTimer(mTimer22);
        MissionUtility::FlushSoundQueue();
        MissionUtility::StopSound(mHandles[169]);
        mFlags[153] = true;
        MissionUtility::SetCollidable(mHandles[106], true);
        MissionUtility::SetVisible(mHandles[106], true);
        MissionUtility::RemoveObject(mHandles[107]);
        MissionUtility::PlayMusic("EP6_V2_T05_02", true);
        mFlags[76] = true;
        MissionUtility::Objectify(mHandles[106], "missions.Kashyyyk1.marker.str0000", false, true, 0.0f);
        MissionUtility::SetCurHealth(mHandles[106], 1.0f);
        MissionUtility::QueueSound("BKK12_08A", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[13] = false;
        MissionUtility::RemoveObject(mHandles[95]);
        MissionUtility::RemoveObject(mHandles[96]);
        MissionUtility::RemoveObject(mHandles[141]);
        MissionUtility::RemoveObject(mHandles[143]);
        MissionUtility::SetVisible(mHandles[142], true);
        MissionUtility::SetApplyDynamics(mHandles[142], true);
        MissionUtility::SetAnimation(mHandles[142], "fullanimation", 1.0f, -1);
        }
        }
    }

    // ---- +0x06e8  100 bytes ----
    if (mTimer2 > 15.0f) {
    if (!mFlags[4]) {
    if (!mFlags[68]) {
    MissionUtility::DisplayText("missions.Kashyyyk1.text.str0006", 6.0f, -1.0f);
    MissionUtility::QueueSound("BKK12_17", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[4] = true;
    }
    }
    }

    // ---- +0x074c  132 bytes ----
    if (mTimer2 > 30.0f) {
    if (!mFlags[5]) {
    if (!mFlags[68]) {
    mHandles[2] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "harvester_spawn", "harvester1", 0, -1, -1);
    MissionUtility::DisplayText("missions.Kashyyyk1.text.str0007", 6.0f, -1.0f);
    MissionUtility::QueueSound("BKK12_11A", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[5] = true;
    }
    }
    }

    // ---- +0x07d0  1368 bytes ----
    if (mTimer2 > 45.0f) {
    if (!mFlags[68]) {
    if (!mFlags[6]) {
    if (!mFlags[60]) {
    mHandles[168] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin_cin", "AnakinCinPath", 0, "AnakinCinAnakin", 0, -1, -1);
    }
    if (mFlags[60]) {
    mHandles[168] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin", "AnakinCinPath", 0, "AnakinCinAnakin", 0, -1, -1);
    }
    MissionUtility::RemoveObject(mHandles[2]);
    MissionUtility::RemoveObject(mHandles[3]);
    MissionUtility::RemoveObject(mHandles[4]);
    MissionUtility::RemoveObject(mHandles[5]);
    MissionUtility::RemoveObject(mHandles[6]);
    MissionUtility::RemoveObject(mHandles[7]);
    MissionUtility::RemoveObject(mHandles[8]);
    MissionUtility::RemoveObject(mHandles[9]);
    MissionUtility::RemoveObject(mHandles[10]);
    mHandles[2] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 0, "harvester1", 0, -1);
    mHandles[3] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 1, "harvester2", 0, -1);
    mHandles[4] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 2, "harvester3", 0, -1);
    mHandles[5] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 3, "harvester4", 0, -1);
    mHandles[6] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 4, "harvester5", 0, -1);
    mHandles[7] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 5, "harvester6", 0, -1);
    mHandles[8] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 6, "harvester7", 0, -1);
    mHandles[9] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 7, "harvester8", 0, -1);
    mHandles[10] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "AnakinCinHarvesterPath", 8, "harvester9", 0, -1);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[2]);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[3]);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[4]);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[5]);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[6]);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[7]);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[8]);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[9]);
    MissionUtility::AddFlockMember(mHandles[12], mHandles[10]);
    MissionUtility::SetVelocForward(mHandles[2], 150.0f);
    MissionUtility::SetVelocForward(mHandles[3], 150.0f);
    MissionUtility::SetVelocForward(mHandles[4], 150.0f);
    MissionUtility::SetVelocForward(mHandles[5], 150.0f);
    MissionUtility::SetVelocForward(mHandles[6], 150.0f);
    MissionUtility::SetVelocForward(mHandles[7], 150.0f);
    MissionUtility::SetVelocForward(mHandles[8], 150.0f);
    MissionUtility::SetVelocForward(mHandles[9], 150.0f);
    MissionUtility::SetVelocForward(mHandles[10], 150.0f);
    MissionUtility::Goto(mHandles[2], "AnakinCinHarvesterPath1", 0);
    MissionUtility::Goto(mHandles[3], "AnakinCinHarvesterPath1", 1);
    MissionUtility::Goto(mHandles[4], "AnakinCinHarvesterPath1", 2);
    MissionUtility::Goto(mHandles[5], "AnakinCinHarvesterPath1", 3);
    MissionUtility::Goto(mHandles[6], "AnakinCinHarvesterPath1", 4);
    MissionUtility::Goto(mHandles[7], "AnakinCinHarvesterPath1", 5);
    MissionUtility::Goto(mHandles[8], "AnakinCinHarvesterPath1", 6);
    MissionUtility::Goto(mHandles[9], "AnakinCinHarvesterPath1", 7);
    MissionUtility::Goto(mHandles[10], "AnakinCinHarvesterPath1", 8);
    mHandles[101] = MissionUtility::RunCin("AnakinCin", true, true);
    mFlags[6] = true;
    MissionUtility::MoveObject(mHandles[88], "EndCinAnakin1Path", 0, true);
    MissionUtility::MoveObject(mHandles[87], "EndCinBera1Path", 0, true);
    BeginTimer(mTimer28);
    }
    }
    }

    // ---- +0x0d28  60 bytes ----
    if (mTimer28 > 2.0f) {
    MissionUtility::SetAnimation(mHandles[168], "death01", 1.0f, 1);
    StopTimer(mTimer28);
    mTimer28 = 0.0f;
    }

    // ---- +0x0d64  52 bytes ----
    if (mFlags[6]) {
        if (!MissionUtility::IsCinRunning(mHandles[101])) {
        if (!mFlags[1]) {
        MissionUtility::MissionFailure();
        mFlags[1] = true;
        }
        }
    }

    // ---- +0x0d98  40 bytes ----
    if (MissionUtility::IsAlive(mHandles[108])) {
    mPosition = MissionUtility::GetPosition(mHandles[108]);
    }

    // ---- +0x0dc0  40 bytes ----
    if (MissionUtility::IsAlive(mHandles[87])) {
    mMatrixB = MissionUtility::GetLocation(mHandles[87]);
    }

    // ---- +0x0de8  96 bytes ----
    if (!mFlags[57] && mFlags[67]) {
        if (!MissionUtility::IsCinRunning(mHandles[97])) {
        mFlags[153] = true;
        mTimer1 = 88.0f;
        mInts[0] = MissionUtility::AddObjective("missions.Kashyyyk1.objective.str0000");
        MissionUtility::DisplayText("missions.Kashyyyk1.text.str0000", 6.0f, -1.0f);
        mFlags[57] = true;
        }
    }

    // ---- +0x0e48  1168 bytes ----
    if (!MissionUtility::IsAlive(mHandles[106])) {
    if (!mFlags[87]) {
    MissionUtility::ObjectiveComplete(mInts[0]);
    mFlags[58] = true;
    MissionUtility::SetEnemies(1, 3);
    if (!mFlags[74]) {
    MissionUtility::ObjectiveComplete(mInts[0]);
    mFlags[58] = true;
    MissionUtility::SetEnemies(1, 3);
    BeginTimer(mTimer5);
    mFlags[74] = true;
    }
    if (mTimer5 > 2.0f) {
    if (!mFlags[77]) {
    MissionUtility::QueueSound("BKK12_09A", 1.0f, 0.0f, 0.0f, "", 0, "");
    mHandles[110] = MissionUtility::CreateObject("cis_inf_droid", "droid_spawn", "droid1", 3, -1, -1);
    MissionUtility::Goto(mHandles[110], "droid1_goto", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[110]);
    MissionUtility::SetQueueFlag(false);
    mFlags[77] = true;
    }
    }
    if (mTimer5 > 3.5f) {
    if (!mFlags[78]) {
    mHandles[111] = MissionUtility::CreateObject("cis_inf_droid", "droid_spawn", "droid2", 3, -1, -1);
    MissionUtility::Goto(mHandles[111], "droid2_goto", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[111]);
    MissionUtility::SetQueueFlag(false);
    mFlags[78] = true;
    }
    }
    if (mTimer5 > 5.0f) {
    if (!mFlags[79]) {
    mHandles[112] = MissionUtility::CreateObject("cis_inf_droid", "droid_spawn", "droid3", 3, -1, -1);
    MissionUtility::Goto(mHandles[112], "droid3_goto", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[112]);
    MissionUtility::SetQueueFlag(false);
    mFlags[79] = true;
    }
    }
    if (mTimer5 > 7.0f) {
    if (!mFlags[80]) {
    mHandles[113] = MissionUtility::CreateObject("cis_walk_small_jedi", "droid_spawn", "droid4", 3, -1, -1);
    MissionUtility::Goto(mHandles[113], "droid4_goto", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[113]);
    MissionUtility::SetQueueFlag(false);
    mFlags[80] = true;
    }
    }
    if (mTimer5 > 9.0f) {
    if (!mFlags[77]) {
    mHandles[114] = MissionUtility::CreateObject("cis_inf_droid", "droid_spawn", "droid5", 3, -1, -1);
    MissionUtility::Goto(mHandles[114], "droid5_goto", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[114]);
    MissionUtility::SetQueueFlag(false);
    mFlags[81] = true;
    }
    }
    if (mTimer5 > 11.0f) {
    if (!mFlags[82]) {
    mHandles[115] = MissionUtility::CreateObject("cis_inf_droid", "droid_spawn", "droid6", 3, -1, -1);
    MissionUtility::Goto(mHandles[115], "droid6_goto", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[115]);
    MissionUtility::SetQueueFlag(false);
    mFlags[82] = true;
    }
    }
    if (mTimer5 > 13.0f) {
    if (!mFlags[83]) {
    mHandles[116] = MissionUtility::CreateObject("cis_inf_droid", "droid_spawn", "droid7", 3, -1, -1);
    MissionUtility::Goto(mHandles[116], "droid7_goto", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[116]);
    MissionUtility::SetQueueFlag(false);
    mFlags[83] = true;
    }
    }
    if (mTimer5 > 15.0f) {
    if (!mFlags[84]) {
    mHandles[117] = MissionUtility::CreateObject("cis_inf_droid", "droid_spawn", "droid8", 3, -1, -1);
    MissionUtility::Goto(mHandles[117], "droid8_goto", true);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[117]);
    MissionUtility::SetQueueFlag(false);
    mFlags[84] = true;
    }
    }
    if (mFlags[77]) {
    if (mFlags[78]) {
    if (mFlags[79]) {
    if (mFlags[80]) {
    if (mFlags[81]) {
    if (mFlags[82]) {
    if (mFlags[83]) {
    if (mFlags[84]) {
    if (mFlags[85]) {
    if (mFlags[86]) {
    mFlags[87] = true;
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

    // ---- +0x12d8  192 bytes ----
    if (mFlags[58] && !mFlags[59]) {
        MissionUtility::Goto(mHandles[108], "command_droid_goto", true);
        mInts[1] = MissionUtility::AddObjective("missions.Kashyyyk1.objective.str0001");
        MissionUtility::DisplayText("missions.Kashyyyk1.text.str0001", 5.0f, -1.0f);
        BeginTimer(mTimer2);
        MissionUtility::Objectify(mHandles[108], "missions.Kashyyyk1.marker.str0001", true, true, 0.0f);
        MissionUtility::StartSound("klaxon1", false, 1.0f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::Goto(mHandles[90], "bera_cage_exit", true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Goto(mHandles[90], "bera_goto_cage", true);
        MissionUtility::SetQueueFlag(false);
        mFlags[59] = true;
    }

    // ---- +0x1398  136 bytes ----
    if (!MissionUtility::IsAlive(mHandles[108])) {
    if (!mFlags[73]) {
    mHandles[109] = MissionUtility::CreateObject("REP_prop_lightsaber", mPosition, "lightsaber", 0, -1, Quat(), -1);
    MissionUtility::Objectify(mHandles[109], "missions.Kashyyyk1.marker.str0000", true, true, 0.0f);
    mFlags[73] = true;
    }
    }

    // ---- +0x1420  336 bytes ----
    if (!MissionUtility::IsAlive(mHandles[108])) {
    if (MissionUtility::GetDistance(mHandles[87], mHandles[109]) < 5.0f) {
    mFlags[60] = true;
    mHandles[88] = MissionUtility::CreateObject("rep_inf_anakin", mMatrixB, "anakin_lightsaber", 1, -1, -1);
    MissionUtility::ObjectiveComplete(mInts[1]);
    MissionUtility::SetMaxHealth(mHandles[88], 350.0f);
    MissionUtility::Objectify(mHandles[90], "missions.Kashyyyk1.marker.str0003", false, true, 0.0f);
    MissionUtility::SetAsPlayer(mHandles[88], 0);
    MissionUtility::SetCurHealth(mHandles[88], MissionUtility::GetCurHealth(mHandles[87]));
    MissionUtility::RemoveObject(mHandles[87]);
    mInts[3] = MissionUtility::AddObjective("missions.Kashyyyk1.objective.str0005");
    MissionUtility::DisplayText("missions.Kashyyyk1.text.str0003", 5.0f, -1.0f);
    MissionUtility::QueueSound("BKK12_20", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::Goto(mHandles[90], "bera_stap_goto", true);
    MissionUtility::Objectify(mHandles[93], "missions.Kashyyyk1.marker.str0000", true, true, 0.0f);
    MissionUtility::Objectify(mHandles[94], "missions.Kashyyyk1.marker.str0000", false, true, 0.0f);
    MissionUtility::SetCurHealth(mHandles[103], 1.0f);
    MissionUtility::SetCurHealth(mHandles[104], 1.0f);
    MissionUtility::SetCurHealth(mHandles[105], 1.0f);
    MissionUtility::RemoveObjectify(mHandles[109]);
    MissionUtility::RemoveObject(mHandles[109]);
    }
    }

    // ---- +0x1570  468 bytes ----
    if (!MissionUtility::IsAlive(mHandles[103])) {
    if (!mFlags[117]) {
    if (!mFlags[116]) {
    MissionUtility::StartSound("PropKash_Cellbreak01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::Goto(mHandles[120], "cage1_prisoners_goto1", true);
    BeginTimer(mTimer18);
    mFlags[116] = true;
    }
    if (mTimer18 > 2.0f) {
    if (!mFlags[119]) {
    MissionUtility::Goto(mHandles[121], "cage1_prisoners_goto2", true);
    mFlags[119] = true;
    }
    }
    if (mTimer18 > 4.0f) {
    if (!mFlags[120]) {
    MissionUtility::Goto(mHandles[122], "cage1_prisoners_goto3", true);
    mFlags[120] = true;
    }
    }
    if (mTimer18 > 6.0f) {
    if (!mFlags[121]) {
    MissionUtility::Goto(mHandles[123], "cage1_prisoners_goto1", true);
    mFlags[121] = true;
    }
    }
    if (mTimer18 > 8.0f) {
    if (!mFlags[122]) {
    MissionUtility::Goto(mHandles[124], "cage1_prisoners_goto2", true);
    mFlags[122] = true;
    }
    }
    if (mTimer18 > 10.0f) {
    if (!mFlags[123]) {
    MissionUtility::Goto(mHandles[125], "cage1_prisoners_goto2", true);
    mFlags[123] = true;
    }
    }
    if (mFlags[118]) {
    if (mFlags[119]) {
    if (mFlags[120]) {
    if (mFlags[121]) {
    if (mFlags[122]) {
    if (mFlags[123]) {
    mFlags[117] = true;
    }
    }
    }
    }
    }
    }
    }
    }

    // ---- +0x1744  468 bytes ----
    if (!MissionUtility::IsAlive(mHandles[104])) {
    if (!mFlags[125]) {
    if (!mFlags[124]) {
    MissionUtility::Goto(mHandles[126], "cage2_prisoners_goto1", true);
    MissionUtility::StartSound("PropKash_Cellbreak01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    BeginTimer(mTimer19);
    mFlags[124] = true;
    }
    if (mTimer19 > 2.0f) {
    if (!mFlags[127]) {
    MissionUtility::Goto(mHandles[127], "cage2_prisoners_goto2", true);
    mFlags[127] = true;
    }
    }
    if (mTimer19 > 4.0f) {
    if (!mFlags[128]) {
    MissionUtility::Goto(mHandles[128], "cage2_prisoners_goto1", true);
    mFlags[128] = true;
    }
    }
    if (mTimer19 > 6.0f) {
    if (!mFlags[129]) {
    MissionUtility::Goto(mHandles[129], "cage2_prisoners_goto1", true);
    mFlags[129] = true;
    }
    }
    if (mTimer19 > 8.0f) {
    if (!mFlags[130]) {
    MissionUtility::Goto(mHandles[130], "cage2_prisoners_goto2", true);
    mFlags[130] = true;
    }
    }
    if (mTimer19 > 10.0f) {
    if (!mFlags[131]) {
    MissionUtility::Goto(mHandles[131], "cage2_prisoners_goto2", true);
    mFlags[131] = true;
    }
    }
    if (mFlags[126]) {
    if (mFlags[127]) {
    if (mFlags[128]) {
    if (mFlags[129]) {
    if (mFlags[130]) {
    if (mFlags[131]) {
    mFlags[125] = true;
    }
    }
    }
    }
    }
    }
    }
    }

    // ---- +0x1918  468 bytes ----
    if (!MissionUtility::IsAlive(mHandles[105])) {
    if (!mFlags[133]) {
    if (!mFlags[132]) {
    MissionUtility::Goto(mHandles[132], "cage3_prisoners_goto1", true);
    MissionUtility::StartSound("PropKash_Cellbreak01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    BeginTimer(mTimer20);
    mFlags[132] = true;
    }
    if (mTimer20 > 2.0f) {
    if (!mFlags[135]) {
    MissionUtility::Goto(mHandles[133], "cage3_prisoners_goto2", true);
    mFlags[135] = true;
    }
    }
    if (mTimer20 > 4.0f) {
    if (!mFlags[136]) {
    MissionUtility::Goto(mHandles[134], "cage3_prisoners_goto1", true);
    mFlags[136] = true;
    }
    }
    if (mTimer20 > 6.0f) {
    if (!mFlags[137]) {
    MissionUtility::Goto(mHandles[135], "cage3_prisoners_goto1", true);
    mFlags[137] = true;
    }
    }
    if (mTimer20 > 8.0f) {
    if (!mFlags[138]) {
    MissionUtility::Goto(mHandles[136], "cage3_prisoners_goto2", true);
    mFlags[138] = true;
    }
    }
    if (mTimer20 > 10.0f) {
    if (!mFlags[139]) {
    MissionUtility::Goto(mHandles[137], "cage3_prisoners_goto2", true);
    mFlags[139] = true;
    }
    }
    if (mFlags[134]) {
    if (mFlags[135]) {
    if (mFlags[136]) {
    if (mFlags[137]) {
    if (mFlags[138]) {
    if (mFlags[139]) {
    mFlags[133] = true;
    }
    }
    }
    }
    }
    }
    }
    }

    // ---- +0x1aec  80 bytes ----
    if (!MissionUtility::IsAlive(mHandles[103])) {
    if (!MissionUtility::IsAlive(mHandles[104])) {
    if (!MissionUtility::IsAlive(mHandles[105])) {
    if (!mFlags[62]) {
    MissionUtility::BonusObjectiveComplete(mInts[2], true);
    mFlags[62] = true;
    }
    }
    }
    }

    // ---- +0x1b3c  72 bytes ----
    if (mFlags[58] && mFlags[60]) {
        if (MissionUtility::GetDistance(mHandles[88], mHandles[93]) < 20.0f) {
        MissionUtility::RemoveObject(mHandles[88]);
        MissionUtility::RemoveObject(mHandles[90]);
        mFlags[64] = true;
        }
    }

    // ---- +0x1b84  1408 bytes ----
    if (mFlags[58] && mFlags[60] && mFlags[64] && !mFlags[15]) {
        if (!mFlags[68]) {
        MissionUtility::ObjectiveComplete(mInts[3]);
        mHandles[98] = MissionUtility::RunCin("Cin2", true, true);
        StopTimer(mTimer22);
        mHandles[13] = MissionUtility::CreateObjectWithRotation("NEU_prop_harvester_effect", "harvester_chase_spawn", 0, "", 2, -1, -1);
        MissionUtility::SetImportantFlag(mHandles[13], true);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_1", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_2", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_3", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_4", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_5", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_6", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_7", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_8", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_9", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_10", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_11", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_12", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_13", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_14", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_15", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_16", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_17", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_18", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_19", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_20", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_21", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_22", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_23", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_24", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_25", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_26", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_27", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_28", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_29", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_30", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_31", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_32", "", false, false, 0.0f);
        MissionUtility::ObjectifyPiece(mHandles[13], "hp_fx_33", "", false, false, 0.0f);
        mFlags[68] = true;
        }
        if (!mFlags[75]) {
        mFlags[75] = true;
        }
        if (!mFlags[154]) {
        if (!mFlags[15]) {
        mHandles[146] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder_player", "MidCinStapPath", 0, "MidCinAnakin", 0, -1, -1);
        mHandles[147] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder_bera", "MidCinStapPath1", 0, "MidCinBera", 0, -1, -1);
        MissionUtility::RemoveObject(mHandles[93]);
        MissionUtility::RemoveObject(mHandles[94]);
        mHandles[92] = MissionUtility::CreateObject("cis_bike_speeder_player_mount", "anakin_stap_spawn", 0, "anakin_stap", 0, -1, Quat(0.362398f, 0.0f, -0.932023f, 0.0f), -1);
        mHandles[91] = MissionUtility::CreateObject("cis_bike_speeder_bera_mount", "bera_stap_spawn", 0, "", 0, -1, Quat(0.362398f, 0.0f, -0.932023f, 0.0f), -1);
        mFlags[15] = true;
        }
        }
    }

    // ---- +0x2104  1244 bytes ----
    if (!mFlags[154] && mFlags[68] && !mFlags[152]) {
        if (MissionUtility::GetCinId(mHandles[98]) == 2) {
        MissionUtility::Goto(mHandles[146], "MidCinStapPath", false);
        MissionUtility::Goto(mHandles[147], "MidCinStapPath1", false);
        MissionUtility::RemoveObject(mHandles[2]);
        MissionUtility::RemoveObject(mHandles[3]);
        MissionUtility::RemoveObject(mHandles[4]);
        MissionUtility::RemoveObject(mHandles[5]);
        MissionUtility::RemoveObject(mHandles[6]);
        MissionUtility::RemoveObject(mHandles[7]);
        MissionUtility::RemoveObject(mHandles[8]);
        MissionUtility::RemoveObject(mHandles[9]);
        MissionUtility::RemoveObject(mHandles[10]);
        mHandles[148] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 0, "MidCinHarv", 0, -1);
        mHandles[149] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 1, "MidCinHarv1", 0, -1);
        mHandles[150] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 2, "MidCinHarv2", 0, -1);
        mHandles[151] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 3, "MidCinHarv3", 0, -1);
        mHandles[152] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 4, "MidCinHarv4", 0, -1);
        mHandles[153] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 5, "MidCinHarv5", 0, -1);
        mHandles[154] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 6, "MidCinHarv6", 0, -1);
        mHandles[155] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 7, "MidCinHarv7", 0, -1);
        mHandles[156] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 8, "MidCinHarv8", 0, -1);
        mHandles[157] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "MidCinHarvPath", 9, "MidCinHarv9", 0, -1);
        MissionUtility::SetVelocForward(mHandles[148], 50.0f);
        MissionUtility::SetVelocForward(mHandles[149], 50.0f);
        MissionUtility::SetVelocForward(mHandles[150], 50.0f);
        MissionUtility::SetVelocForward(mHandles[151], 50.0f);
        MissionUtility::SetVelocForward(mHandles[152], 50.0f);
        MissionUtility::SetVelocForward(mHandles[153], 50.0f);
        MissionUtility::SetVelocForward(mHandles[154], 50.0f);
        MissionUtility::SetVelocForward(mHandles[155], 50.0f);
        MissionUtility::SetVelocForward(mHandles[156], 50.0f);
        MissionUtility::SetVelocForward(mHandles[157], 50.0f);
        MissionUtility::Goto(mHandles[148], "MidCinHarvPath1", 0);
        MissionUtility::Goto(mHandles[149], "MidCinHarvPath1", 1);
        MissionUtility::Goto(mHandles[150], "MidCinHarvPath1", 2);
        MissionUtility::Goto(mHandles[151], "MidCinHarvPath1", 3);
        MissionUtility::Goto(mHandles[152], "MidCinHarvPath1", 4);
        MissionUtility::Goto(mHandles[153], "MidCinHarvPath1", 5);
        MissionUtility::Goto(mHandles[154], "MidCinHarvPath1", 6);
        MissionUtility::Goto(mHandles[155], "MidCinHarvPath1", 7);
        MissionUtility::Goto(mHandles[156], "MidCinHarvPath1", 8);
        MissionUtility::Goto(mHandles[157], "MidCinHarvPath1", 9);
        mFlags[152] = true;
        }
    }

    // ---- +0x25e0  212 bytes ----
    if (mFlags[68]) {
        if (!MissionUtility::IsCinRunning(mHandles[98])) {
        MissionUtility::QueueSound("BKK13_20", 1.0f, 0.0f, 0.0f, "", 0, "");
        ResumeTimer(mTimer22);
        MissionUtility::FlushSoundQueue();
        mFlags[154] = true;
        MissionUtility::RemoveObject(mHandles[148]);
        MissionUtility::RemoveObject(mHandles[149]);
        MissionUtility::RemoveObject(mHandles[150]);
        MissionUtility::RemoveObject(mHandles[151]);
        MissionUtility::RemoveObject(mHandles[152]);
        MissionUtility::RemoveObject(mHandles[153]);
        MissionUtility::RemoveObject(mHandles[154]);
        MissionUtility::RemoveObject(mHandles[155]);
        MissionUtility::RemoveObject(mHandles[156]);
        MissionUtility::RemoveObject(mHandles[157]);
        MissionUtility::RemoveObject(mHandles[146]);
        MissionUtility::RemoveObject(mHandles[147]);
        MissionUtility::SetOverlayEnable(true);
        MissionUtility::SetFOV(90.0f);
        mInts[4] = MissionUtility::AddObjective("missions.Kashyyyk1.objective.str0006");
        mPhase = 1;
        }
    }

    // ---- +0x26b4  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[87], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[87], 0.2f, 0.0f);
    }

    // ---- +0x26dc  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[120], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[120], 0.05f, 0.0f);
    }

    // ---- +0x2704  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[121], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[121], 0.05f, 0.0f);
    }

    // ---- +0x272c  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[122], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[122], 0.05f, 0.0f);
    }

    // ---- +0x2754  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[123], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[123], 0.05f, 0.0f);
    }

    // ---- +0x277c  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[124], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[124], 0.05f, 0.0f);
    }

    // ---- +0x27a4  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[125], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[125], 0.05f, 0.0f);
    }

    // ---- +0x27cc  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[126], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[126], 0.05f, 0.0f);
    }

    // ---- +0x27f4  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[127], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[127], 0.05f, 0.0f);
    }

    // ---- +0x281c  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[128], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[128], 0.05f, 0.0f);
    }

    // ---- +0x2844  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[129], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[129], 0.05f, 0.0f);
    }

    // ---- +0x286c  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[130], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[130], 0.05f, 0.0f);
    }

    // ---- +0x2894  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[131], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[131], 0.05f, 0.0f);
    }

    // ---- +0x28bc  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[88], mHandles[12]) < 120.0f) {
    MissionUtility::DamageObject(mHandles[88], 0.2f, 0.0f);
    }

    // ---- +0x28e4  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[87], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[87], 6.0f, 0.0f);
    }

    // ---- +0x290c  40 bytes ----
    if (MissionUtility::GetDistance(mHandles[88], mHandles[12]) < 80.0f) {
    MissionUtility::DamageObject(mHandles[88], 6.0f, 0.0f);
    }

    // ---- +0x2934  256 bytes ----
    if (mTimer7 > 5.0f) {
    if (!mFlags[88]) {
    MissionUtility::SetVisible(mHandles[142], true);
    MissionUtility::SetApplyDynamics(mHandles[142], true);
    MissionUtility::SetAnimation(mHandles[142], "fullanimation", 1.0f, -1);
    MissionUtility::SetVisible(mHandles[143], false);
    MissionUtility::QueueSound("generic_powerup_med01", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("B_Harverster_steady_lp03", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("B_Harverster_steady_lp03", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("B_Harverster_steady_lp03", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("B_Harverster_steady_lp03", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[88] = true;
    }
    }

    // ---- +0x2a34  104 bytes ----
    if (!mFlags[153]) {
        if (MissionUtility::IsCinRunning(mHandles[97])) {
        if (mTimer8 > 0.1f) {
        if (!mFlags[96]) {
        mInts[7] = MissionUtility::QueueSound("cdk12_01", 1.0f, 0.0f, 0.0f, "", mHandles[95], "talk01");
        mFlags[96] = true;
        }
        }
        }
    }

    // ---- +0x2a9c  68 bytes ----
    if (!mFlags[153]) {
        if (mTimer8 > 1.0f) {
        if (!mFlags[143]) {
        mFlags[143] = true;
        MissionUtility::Goto(mHandles[141], "OpenCinDroidPath", false);
        }
        }
    }

    // ---- +0x2ae0  104 bytes ----
    if (!mFlags[153]) {
        if (mTimer8 > 3.0f) {
        if (!mFlags[144]) {
        mFlags[144] = true;
        MissionUtility::SetVelocForward(mHandles[95], 3.0f);
        MissionUtility::Goto(mHandles[95], "OpenCinDookuPath", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[95]);
        MissionUtility::SetQueueFlag(false);
        }
        }
    }

    // ---- +0x2b48  148 bytes ----
    if (!mFlags[153]) {
        if (mTimer8 > 4.5f) {
        if (!mFlags[148]) {
        mFlags[148] = true;
        MissionUtility::QueueSound("cdk12_02", 1.0f, 0.0f, 0.0f, "", mHandles[95], "talk01");
        MissionUtility::QueueSound("ask12_03", 1.0f, 0.0f, 0.0f, "", mHandles[87], "talk01");
        MissionUtility::QueueSound("cdk12_04", 1.0f, 0.0f, 0.0f, "", mHandles[95], "talk01");
        }
        }
    }

    // ---- +0x2bdc  128 bytes ----
    if (!mFlags[153]) {
        if (MissionUtility::GetCinId(mHandles[97]) == 4) {
        if (!mFlags[145]) {
        mFlags[145] = true;
        MissionUtility::Goto(mHandles[95], "OpenCinDookuPath1", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[95]);
        MissionUtility::SetQueueFlag(false);
        MissionUtility::QueueSound("cdk12_07", 1.0f, 0.0f, 0.0f, "", mHandles[95], "talk01");
        BeginTimer(mTimer25);
        }
        }
    }

    // ---- +0x2c5c  124 bytes ----
    if (!mFlags[153]) {
        if (mTimer25 > 17.0f) {
        MissionUtility::MoveObjectWithRotation(mHandles[95], "OpenCinDookuMoveToPath", 0, true);
        MissionUtility::MoveObjectWithRotation(mHandles[96], "OpenCinCydonMoveToPath", 0, true);
        StopTimer(mTimer25);
        mTimer25 = 0.0f;
        MissionUtility::SetCollidable(mHandles[106], true);
        MissionUtility::SetVisible(mHandles[106], true);
        MissionUtility::RemoveObject(mHandles[107]);
        }
    }

    // ---- +0x2cd8  140 bytes ----
    if (!mFlags[153]) {
        if (MissionUtility::GetCinId(mHandles[97]) == 5) {
        if (!mFlags[146]) {
        mFlags[146] = true;
        MissionUtility::SetVelocForward(mHandles[95], 0.0f);
        MissionUtility::Goto(mHandles[95], mHandles[96]);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[95]);
        MissionUtility::SetQueueFlag(false);
        mHandles[144] = MissionUtility::QueueSound("cdk12_08", 1.0f, 0.0f, 0.0f, "", mHandles[95], "talk01");
        BeginTimer(mTimer26);
        }
        }
    }

    // ---- +0x2d64  144 bytes ----
    if (!mFlags[153]) {
        if (mTimer26 > 11.0f) {
        if (mFlags[146]) {
        if (!mFlags[147]) {
        if (!MissionUtility::IsSoundPlaying(mHandles[144])) {
        mFlags[147] = true;
        MissionUtility::SetVelocForward(mHandles[95], 3.0f);
        MissionUtility::Goto(mHandles[95], "OpenCinDookuPath2", false);
        mHandles[145] = MissionUtility::QueueSound("cxk12_06", 1.0f, 0.0f, 0.0f, "", 0, "");
        }
        }
        }
        }
    }

    // ---- +0x2df4  108 bytes ----
    if (!mFlags[153]) {
        if (mTimer26 > 13.5f) {
        if (mFlags[147]) {
        if (!mFlags[149]) {
        if (!MissionUtility::IsSoundPlaying(mHandles[145])) {
        mFlags[149] = true;
        MissionUtility::SetVelocForward(mHandles[96], 12.0f);
        MissionUtility::Goto(mHandles[96], "OpenCinCydonPath", false);
        }
        }
        }
        }
    }

    // ---- +0x2e60  124 bytes ----
    if (!mFlags[153]) {
        if (MissionUtility::GetCinId(mHandles[97]) == 6) {
        if (!mFlags[150]) {
        mFlags[150] = true;
        BeginTimer(mTimer23);
        MissionUtility::MoveObjectWithRotation(mHandles[96], "OpenCinCydonPath1", 0, true);
        MissionUtility::Goto(mHandles[96], "OpenCinCydonPath1", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[96]);
        MissionUtility::SetQueueFlag(false);
        BeginTimer(mTimer7);
        }
        }
    }

    // ---- +0x2edc  92 bytes ----
    if (!mFlags[153]) {
        if (mTimer23 > 6.0f) {
        StopTimer(mTimer23);
        mTimer23 = 0.0f;
        MissionUtility::Goto(mHandles[96], "OpenCinCydonPath2", false);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Stop(mHandles[96]);
        MissionUtility::SetQueueFlag(false);
        }
    }

    // ---- +0x2f38  92 bytes ----
    if (!mFlags[153]) {
    if (MissionUtility::GetCinId(mHandles[97]) == 7) {
    if (!mFlags[151]) {
    mFlags[151] = true;
    mHandles[169] = MissionUtility::StartSound("bkk12_08", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    }
    }
    }

        break;

    case 1:

    // ---- +0x2f94  516 bytes ----
    if (mFlags[14] && !mFlags[41]) {
        if (MissionUtility::IsAlive(mHandles[103])
            || MissionUtility::IsAlive(mHandles[104])
            || MissionUtility::IsAlive(mHandles[105])) {
        MissionUtility::BonusObjectiveFailed(mInts[2]);
        }
        MissionUtility::SetMapZoom(800.0f, 999999.0f);
        MissionUtility::PlayMusic("EP2_V1_T03_02", true);
        MissionUtility::RemoveTurnAroundRegion("escape_turnaround");
        MissionUtility::RemoveTurnAroundRegion("escape_turnaround2");
        MissionUtility::RemoveTurnAroundRegion("escape_turnaround3");
        MissionUtility::StartSound("BKK13_20", false, 1.5f, 0.0f, 0.0f, "", 0, "");
        MissionUtility::RemoveObject(mHandles[93]);
        MissionUtility::RemoveObject(mHandles[94]);
        MissionUtility::RemoveObject(mHandles[92]);
        MissionUtility::RemoveObject(mHandles[91]);
        mHandles[92] = MissionUtility::CreateObject("cis_bike_speeder_player", "anakin_stap_spawn", 0, "anakin_stap", 1, -1, Quat(0.362398f, 0.0f, -0.932023f, 0.0f), -1);
        mHandles[91] = MissionUtility::CreateObject("cis_bike_speeder_bera", "bera_stap_spawn", 0, "", 0, -1, Quat(0.362398f, 0.0f, -0.932023f, 0.0f), -1);
        MissionUtility::SetMaxHealth(mHandles[92], 300.0f);
        MissionUtility::SetCurHealth(mHandles[92], 300.0f);
        MissionUtility::SetAsPlayer(mHandles[92], 0);
        BeginTimer(mTimer9);
        MissionUtility::DisplayText("missions.Kashyyyk1.text.str0004", 5.0f, -1.0f);
        MissionUtility::Objectify(mHandles[91], "missions.Kashyyyk1.marker.str0000", true, true, 0.0f);
        mFlags[43] = true;
        MissionUtility::SetVelocForward(mHandles[91], 200.0f);
        MissionUtility::GotoDirect(mHandles[91], "bera_chase", true);
        mFlags[14] = false;
    }

    // ---- +0x3198  132 bytes ----
    if (MissionUtility::IsWaveSpawned("gnasps1")) {
    if (!mFlags[8]) {
    mHandles[27] = MissionUtility::GetHandle("gnasp12");
    mHandles[28] = MissionUtility::GetHandle("gnasp13");
    mHandles[29] = MissionUtility::GetHandle("gnasp14");
    mHandles[30] = MissionUtility::GetHandle("gnasp15");
    MissionUtility::SetCollidable(mHandles[27], false);
    MissionUtility::SetCollidable(mHandles[28], false);
    MissionUtility::SetCollidable(mHandles[29], false);
    MissionUtility::SetCollidable(mHandles[30], false);
    mFlags[8] = true;
    }
    }

    // ---- +0x321c  300 bytes ----
    if (MissionUtility::IsWaveSpawned("gnasps2")) {
    if (!mFlags[9]) {
    mHandles[16] = MissionUtility::GetHandle("gnasp1");
    mHandles[17] = MissionUtility::GetHandle("gnasp2");
    mHandles[18] = MissionUtility::GetHandle("gnasp3");
    mHandles[19] = MissionUtility::GetHandle("gnasp4");
    mHandles[20] = MissionUtility::GetHandle("gnasp5");
    mHandles[21] = MissionUtility::GetHandle("gnasp6");
    mHandles[22] = MissionUtility::GetHandle("gnasp7");
    mHandles[23] = MissionUtility::GetHandle("gnasp8");
    mHandles[24] = MissionUtility::GetHandle("gnasp9");
    mHandles[25] = MissionUtility::GetHandle("gnasp10");
    mHandles[26] = MissionUtility::GetHandle("gnasp11");
    MissionUtility::SetCollidable(mHandles[16], false);
    MissionUtility::SetCollidable(mHandles[17], false);
    MissionUtility::SetCollidable(mHandles[18], false);
    MissionUtility::SetCollidable(mHandles[19], false);
    MissionUtility::SetCollidable(mHandles[20], false);
    MissionUtility::SetCollidable(mHandles[21], false);
    MissionUtility::SetCollidable(mHandles[22], false);
    MissionUtility::SetCollidable(mHandles[23], false);
    MissionUtility::SetCollidable(mHandles[24], false);
    MissionUtility::SetCollidable(mHandles[25], false);
    MissionUtility::SetCollidable(mHandles[26], false);
    mFlags[9] = true;
    }
    }

    // ---- +0x3348  48 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "trees1_spawn")) {
    if (!mFlags[10]) {
    MissionUtility::BeginWave("trees1");
    mFlags[10] = true;
    }
    }

    // ---- +0x3378  48 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "trees2_spawn")) {
    if (!mFlags[11]) {
    MissionUtility::BeginWave("trees2");
    mFlags[11] = true;
    }
    }

    // ---- +0x33a8  48 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "trees3_spawn")) {
    if (!mFlags[12]) {
    MissionUtility::BeginWave("trees3");
    mFlags[12] = true;
    }
    }

    // ---- +0x33d8  472 bytes ----
    if (mFlags[14] && mFlags[42]) {
        if (!MissionUtility::IsCinRunning(mHandles[102])) {
        MissionUtility::SetFOV(90.0f);
        mFlags[76] = true;
        MissionUtility::RemoveObject(mHandles[93]);
        MissionUtility::RemoveObject(mHandles[94]);
        MissionUtility::RemoveObject(mHandles[92]);
        MissionUtility::RemoveObject(mHandles[91]);
        MissionUtility::DisplayText("missions.Kashyyyk1.objective.str0004", 5.0f, -1.0f);
        mInts[3] = MissionUtility::AddObjective("missions.Kashyyyk1.objective.str0004");
        mHandles[92] = MissionUtility::CreateObject("cis_bike_speeder_player", "anakin_stap_spawn", 0, "anakin_stap", 1, -1, Quat(0.362398f, 0.0f, -0.932023f, 0.0f), -1);
        mHandles[91] = MissionUtility::CreateObject("cis_bike_speeder_bera", "bera_stap_spawn", 0, "", 0, -1, Quat(0.362398f, 0.0f, -0.932023f, 0.0f), -1);
        MissionUtility::SetMaxHealth(mHandles[92], 300.0f);
        MissionUtility::SetCurHealth(mHandles[92], 300.0f);
        BeginTimer(mTimer9);
        MissionUtility::SetAsPlayer(mHandles[92], 0);
        MissionUtility::Objectify(mHandles[91], "missions.Kashyyyk1.marker.str0000", true, true, 0.0f);
        MissionUtility::SetVelocForward(mHandles[91], 200.0f);
        MissionUtility::DisplayText("missions.Kashyyyk1.text.str0005", 5.0f, -1.0f);
        MissionUtility::GotoDirect(mHandles[91], "bera_chase", true);
        MissionUtility::StartSound("BKK13_20", false, 1.5f, 0.0f, 0.0f, "", 0, "");
        mFlags[43] = true;
        mFlags[14] = false;
        }
    }

    // ---- +0x35b0  416 bytes ----
    if (mFlags[41] && !mFlags[42]) {
        mHandles[102] = MissionUtility::RunCin("e3", true, true);
        mHandles[110] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "droid2_goto", 0, "", 2, -1, -1);
        MissionUtility::SetVelocForward(mHandles[110], 0.0f);
        MissionUtility::Fire(mHandles[110], true, true, false);
        mHandles[111] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "droid8_goto", 0, "", 2, -1, -1);
        MissionUtility::SetVelocForward(mHandles[111], 0.0f);
        MissionUtility::Fire(mHandles[111], true, true, false);
        mHandles[112] = MissionUtility::CreateObjectWithRotation("cis_inf_droid", "droid7_goto", 0, "", 2, -1, -1);
        MissionUtility::SetVelocForward(mHandles[112], 0.0f);
        MissionUtility::Fire(mHandles[112], true, true, false);
        MissionUtility::Fire(mHandles[58], true, true, false);
        MissionUtility::SetVelocForward(mHandles[58], 0.0f);
        mHandles[138] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin", "droid4_goto", 0, "", 1, -1, -1);
        mHandles[139] = MissionUtility::CreateObjectWithRotation("rep_inf_bera", "droid5_goto", 0, "", 1, -1, -1);
        MissionUtility::SetVelocForward(mHandles[138], 10.0f);
        MissionUtility::SetVelocForward(mHandles[139], 10.0f);
        MissionUtility::Goto(mHandles[138], "e3_cin_anakin_goto", true);
        MissionUtility::Goto(mHandles[139], "e3_cin1_bera_goto", true);
        mFlags[42] = true;
    }

    // ---- +0x3750  344 bytes ----
    if (mFlags[42]) {
        if (MissionUtility::GetCinId(mHandles[102]) == 2) {
        if (!mFlags[43]) {
        MissionUtility::RemoveObject(mHandles[93]);
        MissionUtility::RemoveObject(mHandles[94]);
        mHandles[92] = MissionUtility::CreateObject("cis_bike_speeder_player_mount", "anakin_stap_spawn", 0, "anakin_stap", 0, -1, Quat(0.362398f, 0.0f, -0.932023f, 0.0f), -1);
        mHandles[91] = MissionUtility::CreateObject("cis_bike_speeder_bera_mount", "bera_stap_spawn", 0, "", 0, -1, Quat(0.362398f, 0.0f, -0.932023f, 0.0f), -1);
        mHandles[146] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder_player", "MidCinStapPath", 0, "MidCinAnakin", 0, -1, -1);
        mHandles[147] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder_bera", "MidCinStapPath1", 0, "MidCinBera", 0, -1, -1);
        MissionUtility::RemoveObject(mHandles[138]);
        MissionUtility::RemoveObject(mHandles[139]);
        mFlags[43] = true;
        }
        }
    }

    // ---- +0x38a8  68 bytes ----
    if (!mFlags[152]) {
        if (MissionUtility::GetCinId(mHandles[102]) == 3) {
        MissionUtility::Goto(mHandles[146], "MidCinStapPath", false);
        MissionUtility::Goto(mHandles[147], "MidCinStapPath1", false);
        mFlags[152] = true;
        }
    }

    // ---- +0x38ec  72 bytes ----
    if (!MissionUtility::IsAlive(mHandles[92])) {
    if (!mFlags[69]) {
    if (!mFlags[16]) {
    if (mFlags[43]) {
    BeginTimer(mTimer21);
    mFlags[71] = true;
    mFlags[16] = true;
    }
    }
    }
    }

    // ---- +0x3934  56 bytes ----
    if (mFlags[71]) {
        if (mTimer21 > 4.0f) {
        if (!mFlags[1]) {
        MissionUtility::MissionFailure();
        mFlags[1] = true;
        }
        }
    }

    // ---- +0x396c  56 bytes ----
    if (mTimer21 > 3.0f) {
    if (!mFlags[17]) {
    MissionUtility::DamageObject(mHandles[1], 999999.0f, 999999.0f);
    mFlags[17] = true;
    }
    }

    // ---- +0x39a4  144 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "harvester_send")) {
    if (!mFlags[115]) {
    MissionUtility::RemoveObject(mHandles[2]);
    MissionUtility::RemoveObject(mHandles[3]);
    MissionUtility::RemoveObject(mHandles[4]);
    MissionUtility::RemoveObject(mHandles[5]);
    MissionUtility::RemoveObject(mHandles[6]);
    MissionUtility::RemoveObject(mHandles[7]);
    MissionUtility::RemoveObject(mHandles[8]);
    MissionUtility::RemoveObject(mHandles[9]);
    MissionUtility::RemoveObject(mHandles[10]);
    MissionUtility::RemoveObject(mHandles[110]);
    MissionUtility::RemoveObject(mHandles[111]);
    MissionUtility::RemoveObject(mHandles[112]);
    MissionUtility::RemoveObject(mHandles[58]);
    mFlags[115] = true;
    }
    }

    // ---- +0x3a34  56 bytes ----
    if (mTimer9 > 10.0f) {
    if (!mFlags[3]) {
    MissionUtility::GotoDirect(mHandles[13], "bera_chase", true);
    mFlags[3] = true;
    }
    }

    // ---- +0x3a6c  60 bytes ----
    if (GetDistToHarvester(mHandles[92], mHandles[13], 17) < 200.0f) {
    MissionUtility::DamageObject(mHandles[92], 0.4f, 9999.0f);
    MissionUtility::ShakeCamera(0.1f, 0.1f, 0.1f);
    }

    // ---- +0x3aa8  60 bytes ----
    if (GetDistToHarvester(mHandles[92], mHandles[13], 17) < 80.0f) {
    MissionUtility::DamageObject(mHandles[92], 10.0f, 9999.0f);
    MissionUtility::ShakeCamera(0.1f, 0.1f, 0.1f);
    }

    // ---- +0x3ae4  36 bytes ----
    if (MissionUtility::GetDistance(mHandles[91], mHandles[92]) < 150.0f) {
    MissionUtility::SetVelocForward(mHandles[91], 250.0f);
    }

    // ---- +0x3b08  60 bytes ----
    if (MissionUtility::GetDistance(mHandles[91], mHandles[92]) > 150.0f) {
    if (MissionUtility::GetDistance(mHandles[91], mHandles[92]) > 250.0f) {
    MissionUtility::SetVelocForward(mHandles[91], 130.0f);
    }
    }

    // ---- +0x3b44  36 bytes ----
    if (MissionUtility::GetDistance(mHandles[91], mHandles[92]) > 350.0f) {
    MissionUtility::SetVelocForward(mHandles[91], 0.0f);
    }

    // ---- +0x3b68  40 bytes ----
    if (GetDistToHarvester(mHandles[92], mHandles[13], 17) > 300.0f) {
    MissionUtility::SetVelocForward(mHandles[13], 250.0f);
    }

    // ---- +0x3b90  68 bytes ----
    if (GetDistToHarvester(mHandles[92], mHandles[13], 17) < 300.0f) {
    if (GetDistToHarvester(mHandles[92], mHandles[2], 17) > 260.0f) {
    MissionUtility::SetVelocForward(mHandles[13], 180.0f);
    }
    }

    // ---- +0x3bd4  56 bytes ----
    if (GetDistToHarvester(mHandles[92], mHandles[13], 17) < 260.0f) {
    MissionUtility::ShakeCamera(0.1f, 0.1f, 0.2f);
    MissionUtility::SetVelocForward(mHandles[13], 20.0f);
    }

    // ---- +0x3c0c  56 bytes ----
    if (MissionUtility::GetDistance(mHandles[13], "harvester_chase", 21) < 30.0f) {
    if (!mFlags[90]) {
    BeginTimer(mTimer10);
    mFlags[90] = true;
    }
    }

    // ---- +0x3c44  76 bytes ----
    if (mTimer10 > 4.0f) {
    if (!mFlags[106]) {
    MissionUtility::MoveObjectWithRotation(mHandles[13], "harvester_chase", 22, true);
    MissionUtility::GotoDirect(mHandles[13], "harvester_chase", false);
    mFlags[106] = true;
    }
    }

    // ---- +0x3c90  56 bytes ----
    if (MissionUtility::GetDistance(mHandles[13], "harvester_chase", 24) < 30.0f) {
    if (!mFlags[91]) {
    BeginTimer(mTimer11);
    mFlags[91] = true;
    }
    }

    // ---- +0x3cc8  76 bytes ----
    if (mTimer11 > 4.0f) {
    if (!mFlags[107]) {
    MissionUtility::MoveObjectWithRotation(mHandles[13], "harvester_chase", 25, true);
    MissionUtility::GotoDirect(mHandles[13], "harvester_chase", false);
    mFlags[107] = true;
    }
    }

    // ---- +0x3d14  68 bytes ----
    if (MissionUtility::GetDistance(mHandles[91], "harvester_chase", 21) < 30.0f) {
    if (!mFlags[112]) {
    MissionUtility::MoveObject(mHandles[91], "harvester_chase", 22, true);
    mFlags[112] = true;
    }
    }

    // ---- +0x3d58  68 bytes ----
    if (MissionUtility::GetDistance(mHandles[91], "harvester_chase", 24) < 30.0f) {
    if (!mFlags[113]) {
    MissionUtility::MoveObject(mHandles[91], "harvester_chase", 25, true);
    mFlags[113] = true;
    }
    }

    // ---- +0x3d9c  160 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "stap_trigger1")) {
    if (!mFlags[104]) {
    MissionUtility::SetVelocForward(mHandles[80], 250.0f);
    MissionUtility::SetVelocForward(mHandles[81], 250.0f);
    mHandles[14] = MissionUtility::CreateObjectWithRotation("neu_prop_harvester_effect", "h_spawn1", 0, "h1", 2, -1, -1);
    MissionUtility::SetVelocForward(mHandles[14], 75.0f);
    MissionUtility::Goto(mHandles[14], "h1_goto", true);
    MissionUtility::Goto(mHandles[80], "staps1_path", true);
    MissionUtility::Goto(mHandles[81], "staps1_path", true);
    mFlags[104] = true;
    }
    }

    // ---- +0x3e3c  112 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "harvester_send2")) {
    if (!mFlags[18]) {
    MissionUtility::BeginWave("gnasps2");
    mHandles[15] = MissionUtility::CreateObjectWithRotation("neu_prop_harvester_effect", "h_spawn2", 0, "h2", 2, -1, -1);
    MissionUtility::SetVelocForward(mHandles[15], 35.0f);
    MissionUtility::Goto(mHandles[15], "h2_goto", true);
    mFlags[18] = true;
    }
    }

    // ---- +0x3eac  112 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "stap_trigger3")) {
    if (!mFlags[52]) {
    MissionUtility::BeginWave("trees5");
    MissionUtility::BeginWave("runaways");
    MissionUtility::SetVelocForward(mHandles[84], 250.0f);
    MissionUtility::SetVelocForward(mHandles[85], 250.0f);
    MissionUtility::Goto(mHandles[84], "stap3a_goto", true);
    MissionUtility::Goto(mHandles[85], "stap3b_goto", true);
    mFlags[52] = true;
    }
    }

    // ---- +0x3f1c  116 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "stap_trigger4")) {
    if (!mFlags[53]) {
    mHandles[86] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder", "stap4_spawn", 0, "", 2, -1, -1);
    MissionUtility::SetAccelThrust(mHandles[86], 500.0f);
    MissionUtility::SetVelocForward(mHandles[86], 450.0f);
    MissionUtility::Goto(mHandles[86], "stap4_goto", true);
    mFlags[53] = true;
    }
    }

    // ---- +0x3f90  104 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "stap_trigger2")) {
    if (!mFlags[103]) {
    MissionUtility::BeginWave("trees4");
    MissionUtility::SetVelocForward(mHandles[82], 250.0f);
    MissionUtility::SetVelocForward(mHandles[83], 250.0f);
    MissionUtility::Goto(mHandles[82], "stap2a_path", true);
    MissionUtility::Goto(mHandles[83], "stap2a_path", true);
    mFlags[103] = true;
    }
    }

    // ---- +0x3ff8  180 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "aat_trigger1")) {
    MissionUtility::EvictConfig("kas_crea_wookiee");
    MissionUtility::EvictConfig("cis_inf_dooku");
    MissionUtility::EvictConfig("neu_prop_harvester");
    MissionUtility::EvictConfig("kas_bldg_barracks");
    MissionUtility::EvictConfig("kas_bldg_storage");
    MissionUtility::EvictConfig("kas_bldg_lpad");
    MissionUtility::EvictConfig("cis_inf_droid");
    MissionUtility::EvictConfig("cis_walk_small_jedi");
    MissionUtility::EvictConfig("rep_prop_crate_small");
    MissionUtility::EvictConfig("rep_prop_crate_tiny");
    MissionUtility::BeginWave("gnasps1");
    MissionUtility::AttackTarget(mHandles[75], mHandles[70], true, true, false, false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[75], mHandles[92], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    }

    // ---- +0x40ac  92 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "aat_trigger2")) {
    MissionUtility::AttackTarget(mHandles[76], mHandles[71], true, true, false, false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[76], mHandles[92], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    }

    // ---- +0x4108  92 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "aat_trigger3")) {
    MissionUtility::AttackTarget(mHandles[77], mHandles[74], true, true, false, false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::AttackTarget(mHandles[77], mHandles[92], true, true, false, false);
    MissionUtility::SetQueueFlag(false);
    }

    // ---- +0x4164  36 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "ravine_kill")) {
    MissionUtility::DamageObject(mHandles[92], 99999.0f, 99999.0f);
    }

    // ---- +0x4188  52 bytes ----
    if (MissionUtility::IsAlive(mHandles[85])) {
    if (MissionUtility::IsInsideRegion(mHandles[85], "stap_hit_tree")) {
    MissionUtility::DamageObject(mHandles[85], 99999.0f, 99999.0f);
    }
    }

    // ---- +0x41bc  52 bytes ----
    if (MissionUtility::IsAlive(mHandles[86])) {
    if (MissionUtility::IsInsideRegion(mHandles[86], "stap_hitrock")) {
    MissionUtility::DamageObject(mHandles[86], 99999.0f, 99999.0f);
    }
    }

    // ---- +0x41f0  104 bytes ----
    if (mTimer9 > 30.0f) {
    if (MissionUtility::GetDistance(mHandles[92], mHandles[13]) < 200.0f) {
    if (!mFlags[49]) {
    BeginTimer(mTimer9);
    MissionUtility::QueueSound("BKK12_13", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[49] = true;
    }
    }
    }

    // ---- +0x4258  108 bytes ----
    if (mFlags[49]) {
        if (mTimer9 > 90.0f) {
        if (MissionUtility::GetDistance(mHandles[92], mHandles[13]) < 200.0f) {
        if (!mFlags[50]) {
        MissionUtility::QueueSound("BKK12_14", 1.0f, 0.0f, 0.0f, "", 0, "");
        mFlags[50] = true;
        }
        }
        }
    }

    // ---- +0x42c4  68 bytes ----
    if (MissionUtility::GetDistance(mHandles[92], mHandles[56]) < 300.0f) {
    if (!mFlags[66]) {
    MissionUtility::Objectify(mHandles[56], "missions.Kashyyyk1.marker.str0002", false, true, 0.0f);
    mFlags[66] = true;
    }
    }

    // ---- +0x4308  112 bytes ----
    if (MissionUtility::IsAlive(mHandles[92])) {
    if (mHandles[92] == MissionUtility::GetWhoShotMe(mHandles[56])) {
    if (!mFlags[65]) {
    MissionUtility::BonusObjectiveComplete(mInts[6], true);
    MissionUtility::RemoveObjectify(mHandles[56]);
    MissionUtility::StartSound("PropKash_WookieAlarm01", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[65] = true;
    }
    }
    }

    // ---- +0x4378  68 bytes ----
    if (!mFlags[65] && !mFlags[48]) {
        if (MissionUtility::IsInsideRegion(mHandles[92], "wookiee_alarm_missed")) {
        MissionUtility::RemoveObjectify(mHandles[56]);
        MissionUtility::BonusObjectiveFailed(mInts[6]);
        mFlags[48] = true;
        }
    }

    // ---- +0x43bc  88 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "wookiees_run_trigger1")) {
    if (!mFlags[44]) {
    MissionUtility::Goto(mHandles[48], "wookiee_run1", true);
    MissionUtility::Goto(mHandles[49], "wookiee_run1", true);
    MissionUtility::Goto(mHandles[50], "wookiee_run1", true);
    mFlags[44] = true;
    }
    }

    // ---- +0x4414  104 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "wookiees_run_trigger2")) {
    if (!mFlags[45]) {
    MissionUtility::Goto(mHandles[51], "wookiee_run4", true);
    MissionUtility::Goto(mHandles[52], "wookiee_run5", true);
    MissionUtility::Goto(mHandles[53], "wookiee_run6", true);
    MissionUtility::Goto(mHandles[54], "wookiee_run7", true);
    mFlags[45] = true;
    }
    }

    // ---- +0x447c  68 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "gnasp_trigger")) {
    if (!mFlags[23]) {
    if (!mFlags[24]) {
    BeginTimer(mTimer24);
    mFlags[24] = true;
    }
    mFlags[23] = true;
    }
    }

    // ---- +0x44c0  136 bytes ----
    if (mTimer24 > 1.0f) {
    if (!mFlags[25]) {
    mHandles[31] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[31], "gnasp_flight1", true);
    mFlags[25] = true;
    }
    }

    // ---- +0x4548  136 bytes ----
    if (mTimer24 > 1.3f) {
    if (!mFlags[26]) {
    mHandles[32] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[32], "gnasp_flight2", true);
    mFlags[26] = true;
    }
    }

    // ---- +0x45d0  136 bytes ----
    if (mTimer24 > 1.6f) {
    if (!mFlags[27]) {
    mHandles[33] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[33], "gnasp_flight3", true);
    mFlags[27] = true;
    }
    }

    // ---- +0x4658  136 bytes ----
    if (mTimer24 > 1.9f) {
    if (!mFlags[28]) {
    mHandles[34] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[34], "gnasp_flight4", true);
    mFlags[28] = true;
    }
    }

    // ---- +0x46e0  136 bytes ----
    if (mTimer24 > 2.2f) {
    if (!mFlags[29]) {
    mHandles[35] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[35], "gnasp_flight1", true);
    mFlags[29] = true;
    }
    }

    // ---- +0x4768  136 bytes ----
    if (mTimer24 > 2.5f) {
    if (!mFlags[30]) {
    mHandles[36] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[36], "gnasp_flight2", true);
    mFlags[30] = true;
    }
    }

    // ---- +0x47f0  136 bytes ----
    if (mTimer24 > 2.8f) {
    if (!mFlags[31]) {
    mHandles[37] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[37], "gnasp_flight3", true);
    mFlags[31] = true;
    }
    }

    // ---- +0x4878  136 bytes ----
    if (mTimer24 > 3.1f) {
    if (!mFlags[32]) {
    mHandles[38] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[38], "gnasp_flight4", true);
    mFlags[32] = true;
    }
    }

    // ---- +0x4900  136 bytes ----
    if (mTimer24 > 3.4f) {
    if (!mFlags[33]) {
    mHandles[39] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[39], "gnasp_flight1", true);
    mFlags[33] = true;
    }
    }

    // ---- +0x4988  136 bytes ----
    if (mTimer24 > 3.7f) {
    if (!mFlags[34]) {
    mHandles[40] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[40], "gnasp_flight2", true);
    mFlags[34] = true;
    }
    }

    // ---- +0x4a10  136 bytes ----
    if (mTimer24 > 4.0f) {
    if (!mFlags[35]) {
    mHandles[41] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[41], "gnasp_flight3", true);
    mFlags[35] = true;
    }
    }

    // ---- +0x4a98  136 bytes ----
    if (mTimer24 > 4.3f) {
    if (!mFlags[36]) {
    mHandles[42] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[42], "gnasp_flight4", true);
    mFlags[36] = true;
    }
    }

    // ---- +0x4b20  136 bytes ----
    if (mTimer24 > 4.6f) {
    if (!mFlags[37]) {
    mHandles[43] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[43], "gnasp_flight1", true);
    mFlags[37] = true;
    }
    }

    // ---- +0x4ba8  136 bytes ----
    if (mTimer24 > 4.9f) {
    if (!mFlags[38]) {
    mHandles[44] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[44], "gnasp_flight2", true);
    mFlags[38] = true;
    }
    }

    // ---- +0x4c30  136 bytes ----
    if (mTimer24 > 5.2f) {
    if (!mFlags[39]) {
    mHandles[45] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[45], "gnasp_flight3", true);
    mFlags[39] = true;
    }
    }

    // ---- +0x4cb8  136 bytes ----
    if (mTimer24 > 5.5f) {
    if (!mFlags[40]) {
    mHandles[46] = MissionUtility::CreateObject("kas_crea_gnasp_flight", "gnasp_flight_spawn", 0, "", 0, -1);
    MissionUtility::Goto(mHandles[46], "gnasp_flight4", true);
    mFlags[40] = true;
    }
    }

    // ---- +0x4d40  80 bytes ----
    if (MissionUtility::GetDistance(mHandles[91], "bera_chase", 32) < 50.0f) {
    if (!mFlags[20]) {
    MissionUtility::QueueSound("BKK12_12", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[20] = true;
    }
    }

    // ---- +0x4d90  80 bytes ----
    if (MissionUtility::GetDistance(mHandles[91], "bera_chase", 41) < 50.0f) {
    if (!mFlags[21]) {
    MissionUtility::QueueSound("BKK12_23", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[21] = true;
    }
    }

    // ---- +0x4de0  80 bytes ----
    if (MissionUtility::GetDistance(mHandles[91], "bera_chase", 44) < 50.0f) {
    if (!mFlags[22]) {
    MissionUtility::QueueSound("BKK12_24", 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[22] = true;
    }
    }

    // ---- +0x4e30  248 bytes ----
    if (MissionUtility::IsInsideRegion(mHandles[92], "endcin_trigger")) {
    if (!mFlags[69]) {
    mHandles[68] = MissionUtility::CreateObject("cis_bike_speeder_player", "end_cin_anakin_spawn", "anakin_stap_endcin", 0, -1, -1);
    mHandles[69] = MissionUtility::CreateObject("cis_bike_speeder_bera", "end_cin_bera_spawn", "bera_stap_endcin", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[69], 400.0f);
    MissionUtility::SetVelocForward(mHandles[68], 400.0f);
    MissionUtility::SetCollidable(mHandles[68], false);
    MissionUtility::SetCollidable(mHandles[69], false);
    MissionUtility::Goto(mHandles[68], "end_cin_anakin_goto", true);
    MissionUtility::Goto(mHandles[69], "end_cin_bera_goto", true);
    mFlags[69] = true;
    MissionUtility::RemoveObject(mHandles[92]);
    MissionUtility::RemoveObject(mHandles[91]);
    mHandles[99] = MissionUtility::RunCin("Cin3", true, true);
    MissionUtility::ObjectiveComplete(mInts[4]);
    MissionUtility::PlayMusic("EP2_V1_T13_01", true);
    MissionUtility::ObjectiveComplete(mInts[3]);
    }
    }

    // ---- +0x4f28  1616 bytes ----
    if (MissionUtility::IsCinRunning(mHandles[99])) {
    if (MissionUtility::GetCinId(mHandles[99]) == 2) {
    if (!mFlags[54]) {
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_1");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_2");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_3");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_4");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_5");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_6");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_7");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_8");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_9");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_10");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_11");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_12");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_13");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_14");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_15");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_16");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_17");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_18");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_19");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_20");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_21");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_22");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_23");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_24");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_25");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_26");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_27");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_28");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_29");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_30");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_31");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_32");
    MissionUtility::RemoveObjectifyPiece(mHandles[13], "hp_fx_33");
    MissionUtility::RemoveObject(mHandles[13]);
    MissionUtility::RemoveObject(mHandles[2]);
    MissionUtility::RemoveObject(mHandles[3]);
    MissionUtility::RemoveObject(mHandles[4]);
    MissionUtility::RemoveObject(mHandles[5]);
    MissionUtility::RemoveObject(mHandles[6]);
    MissionUtility::RemoveObject(mHandles[7]);
    MissionUtility::RemoveObject(mHandles[8]);
    MissionUtility::RemoveObject(mHandles[9]);
    MissionUtility::RemoveObject(mHandles[10]);
    mHandles[158] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "EndCinHarvPath", 0, "EndCinHarv", 0, -1);
    mHandles[159] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "EndCinHarvPath", 1, "EndCinHarv1", 0, -1);
    mHandles[160] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "EndCinHarvPath", 2, "EndCinHarv2", 0, -1);
    mHandles[161] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "EndCinHarvPath", 3, "EndCinHarv3", 0, -1);
    mHandles[162] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "EndCinHarvPath", 4, "EndCinHarv4", 0, -1);
    mHandles[163] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "EndCinHarvPath", 5, "EndCinHarv5", 0, -1);
    mHandles[164] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "EndCinHarvPath", 6, "EndCinHarv6", 0, -1);
    mHandles[165] = MissionUtility::CreateObject("NEU_prop_harvester_effect_spread", "EndCinHarvPath", 7, "EndCinHarv7", 0, -1);
    MissionUtility::SetVelocForward(mHandles[158], 50.0f);
    MissionUtility::SetVelocForward(mHandles[159], 50.0f);
    MissionUtility::SetVelocForward(mHandles[160], 50.0f);
    MissionUtility::SetVelocForward(mHandles[161], 50.0f);
    MissionUtility::SetVelocForward(mHandles[162], 50.0f);
    MissionUtility::SetVelocForward(mHandles[163], 50.0f);
    MissionUtility::SetVelocForward(mHandles[164], 50.0f);
    MissionUtility::SetVelocForward(mHandles[165], 50.0f);
    MissionUtility::Goto(mHandles[158], "EndCinHarvPath1", 0);
    MissionUtility::Goto(mHandles[159], "EndCinHarvPath1", 1);
    MissionUtility::Goto(mHandles[160], "EndCinHarvPath1", 2);
    MissionUtility::Goto(mHandles[161], "EndCinHarvPath1", 3);
    MissionUtility::Goto(mHandles[162], "EndCinHarvPath1", 4);
    MissionUtility::Goto(mHandles[163], "EndCinHarvPath1", 5);
    MissionUtility::Goto(mHandles[164], "EndCinHarvPath1", 6);
    MissionUtility::Goto(mHandles[165], "EndCinHarvPath1", 7);
    MissionUtility::QueueSound("BKK12_27", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("ASK12_15", 1.0f, 0.0f, 0.0f, "", 0, "");
    MissionUtility::QueueSound("BKK12_16", 1.0f, 0.0f, 0.0f, "", 0, "");
    mHandles[66] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder_bera", "cin3_shot3_stap_spawn1", 0, "", 0, -1, -1);
    mHandles[67] = MissionUtility::CreateObjectWithRotation("cis_bike_speeder_player", "cin3_shot3_stap_spawn2", 0, "", 0, -1, -1);
    MissionUtility::SetVelocForward(mHandles[66], 400.0f);
    MissionUtility::SetVelocForward(mHandles[67], 400.0f);
    MissionUtility::Goto(mHandles[66], "cin3_shot3_stap_goto", true);
    MissionUtility::Goto(mHandles[67], "cin3_shot3_stap_goto", true);
    mFlags[54] = true;
    }
    }
    }

    // ---- +0x5578  312 bytes ----
    if (MissionUtility::IsCinRunning(mHandles[99])) {
    if (MissionUtility::GetCinId(mHandles[99]) == 3) {
    if (!mFlags[141]) {
    MissionUtility::MoveObjectWithRotation(mHandles[66], "EndCinBeraStapPath", 0, true);
    MissionUtility::MoveObjectWithRotation(mHandles[67], "EndCinAnakinStapPath", 0, true);
    MissionUtility::Goto(mHandles[66], "EndCinBeraStapPath", false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[66]);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::Goto(mHandles[67], "EndCinAnakinStapPath", false);
    MissionUtility::SetQueueFlag(true);
    MissionUtility::Stop(mHandles[67]);
    MissionUtility::SetQueueFlag(false);
    MissionUtility::SetVelocForward(mHandles[66], 200.0f);
    MissionUtility::SetVelocForward(mHandles[67], 200.0f);
    mFlags[141] = true;
    mHandles[140] = MissionUtility::CreateObjectWithRotation("kas_inf_wookiee", "EndCinWookiePath", 0, "EndCinWookie", 0, -1, -1);
    mHandles[166] = MissionUtility::CreateObjectWithRotation("rep_inf_anakin_cin", "EndCinAnakin1Path", 0, "EndCinAnakin1", 0, -1, -1);
    mHandles[167] = MissionUtility::CreateObjectWithRotation("rep_inf_bera", "EndCinBera1Path", 0, "EndCinBera1", 0, -1, -1);
    BeginTimer(mTimer29);
    }
    }
    }

    // ---- +0x56b0  52 bytes ----
    if (mTimer29 > 5.0f) {
    StopTimer(mTimer29);
    mTimer29 = 0.0f;
    MissionUtility::TransOut(1.0f, 1);
    }

    // ---- +0x56e4  196 bytes ----
    if (MissionUtility::IsCinRunning(mHandles[99])) {
    if (MissionUtility::GetCinId(mHandles[99]) == 4) {
    if (!mFlags[142]) {
    MissionUtility::TransIn(1.0f, 0);
    MissionUtility::QueueSound("Vocal_wookie_call03", 1.0f, 0.0f, 0.0f, "", mHandles[140], "talk01");
    MissionUtility::QueueSound("Vocal_wookie_call01", 1.0f, 0.0f, 0.0f, "", mHandles[140], "talk01");
    MissionUtility::QueueSound("Vocal_wookie_call02", 1.0f, 0.0f, 0.0f, "", mHandles[140], "talk01");
    MissionUtility::StartSound("bkk12_28", false, 1.0f, 0.0f, 0.0f, "", 0, "");
    mFlags[142] = true;
    }
    }
    }

    // ---- +0x57a8  84 bytes ----
    if (MissionUtility::IsCinRunning(mHandles[99])) {
    if (MissionUtility::GetCinId(mHandles[99]) == 6) {
    if (!mFlags[70]) {
    MissionUtility::QueueSound("ask12_29", 1.0f, 0.0f, 0.0f, "", mHandles[166], "talk01");
    mFlags[70] = true;
    }
    }
    }

    // ---- +0x57fc  60 bytes ----
    if (MissionUtility::GetCinId(mHandles[99]) == 7) {
    if (!mFlags[101]) {
    BeginTimer(mTimer16);
    MissionUtility::Goto(mHandles[166], "EndCinAnakin1Path", false);
    mFlags[101] = true;
    }
    }

    // ---- +0x5838  72 bytes ----
    if (mTimer16 > 0.5f) {
    if (!mFlags[102]) {
    MissionUtility::Goto(mHandles[167], "EndCinBera1Path", false);
    MissionUtility::Goto(mHandles[140], "EndCinWookiePath", false);
    mFlags[102] = true;
    }
    }

    // ---- +0x5880  56 bytes ----
    if (mFlags[69]) {
        if (!MissionUtility::IsCinRunning(mHandles[99])) {
        if (!mFlags[0]) {
        MissionUtility::FlushSoundQueue();
        MissionUtility::MissionSuccess();
        mFlags[0] = true;
        }
        }
    }

    // ---- +0x58b8  60 bytes ----
    if (MissionUtility::GetGameClock() < 155.0f) {
    if (mFlags[69]) {
    if (!mFlags[140]) {
    MissionUtility::BonusObjectiveComplete(mInts[5], true);
    mFlags[140] = true;
    }
    }
    }

        break;
    }
}

// The closest `hp_fx_<n>` marker to `who`. A file-scope static, defined here because that
// is where it sits in the shipped .text -- after Execute, which calls it.
static float GetDistToHarvester(int who, int what, int count)
{
    char name[32];
    float best = 100000.0f;

    for (int i = 0; i < count; i++) {
        sprintf(name, "hp_fx_%d", i + 1);
        float d = MissionUtility::GetDistance(who, what, name);
        if (d < best) {
            best = d;
        }
    }
    return best;
}

void Kashyyyk1Script::Setup()
{
    mFlags[2] = true;
        mFlags[13] = true;
        mFlags[14] = true;
        mHandles[87] = MissionUtility::GetPlayerHandle(0);
        mHandles[103] = MissionUtility::GetHandle("prisoners_cage");
        mHandles[104] = MissionUtility::GetHandle("prisoners_cage2");
        mHandles[105] = MissionUtility::GetHandle("prisoners_cage3");
        mHandles[106] = MissionUtility::GetHandle("jedi_cage");
        mHandles[107] = MissionUtility::GetHandle("jedi_cage1");
        mHandles[96] = MissionUtility::GetHandle("cydon");
        mHandles[141] = MissionUtility::GetHandle("OpenCinDroid");
        mHandles[90] = MissionUtility::GetHandle("bera");
        mHandles[108] = MissionUtility::GetHandle("command_droid");
        mHandles[95] = MissionUtility::GetHandle("dooku");
        mHandles[96] = MissionUtility::GetHandle("cydon");
        mHandles[75] = MissionUtility::GetHandle("mtank1");
        mHandles[76] = MissionUtility::GetHandle("mtank2");
        mHandles[77] = MissionUtility::GetHandle("mtank3");
        mHandles[78] = MissionUtility::GetHandle("mtank4");
        mHandles[79] = MissionUtility::GetHandle("mtank5");
        mHandles[80] = MissionUtility::GetHandle("stap1a");
        mHandles[81] = MissionUtility::GetHandle("stap1b");
        mHandles[82] = MissionUtility::GetHandle("stap2a");
        mHandles[83] = MissionUtility::GetHandle("stap2b");
        mHandles[84] = MissionUtility::GetHandle("stap3a");
        mHandles[85] = MissionUtility::GetHandle("stap3b");
        mHandles[70] = MissionUtility::GetHandle("aat_target1");
        mHandles[71] = MissionUtility::GetHandle("aat_target2");
        mHandles[72] = MissionUtility::GetHandle("aat_target3");
        mHandles[73] = MissionUtility::GetHandle("aat_target4");
        mHandles[74] = MissionUtility::GetHandle("aat_target5");
        mHandles[120] = MissionUtility::GetHandle("cage1_prisoner1");
        mHandles[121] = MissionUtility::GetHandle("cage1_prisoner2");
        mHandles[122] = MissionUtility::GetHandle("cage1_prisoner3");
        mHandles[123] = MissionUtility::GetHandle("cage1_prisoner4");
        mHandles[124] = MissionUtility::GetHandle("cage1_prisoner5");
        mHandles[125] = MissionUtility::GetHandle("cage1_prisoner6");
        mHandles[126] = MissionUtility::GetHandle("cage2_prisoner1");
        mHandles[127] = MissionUtility::GetHandle("cage2_prisoner2");
        mHandles[128] = MissionUtility::GetHandle("cage2_prisoner3");
        mHandles[129] = MissionUtility::GetHandle("cage2_prisoner4");
        mHandles[130] = MissionUtility::GetHandle("cage2_prisoner5");
        mHandles[131] = MissionUtility::GetHandle("cage2_prisoner6");
        mHandles[132] = MissionUtility::GetHandle("cage3_prisoner1");
        mHandles[133] = MissionUtility::GetHandle("cage3_prisoner2");
        mHandles[134] = MissionUtility::GetHandle("cage3_prisoner3");
        mHandles[135] = MissionUtility::GetHandle("cage3_prisoner4");
        mHandles[136] = MissionUtility::GetHandle("cage3_prisoner5");
        mHandles[137] = MissionUtility::GetHandle("cage3_prisoner6");
        mHandles[57] = MissionUtility::GetHandle("small_walk1");
        mHandles[58] = MissionUtility::GetHandle("small_walk2");
        mHandles[59] = MissionUtility::GetHandle("small_walk3");
        mHandles[60] = MissionUtility::GetHandle("small_walk4");
        mHandles[61] = MissionUtility::GetHandle("small_walk5");
        mHandles[62] = MissionUtility::GetHandle("sbdroid1");
        mHandles[63] = MissionUtility::GetHandle("sbdroid2");
        mHandles[64] = MissionUtility::GetHandle("sbdroid3");
        mHandles[65] = MissionUtility::GetHandle("sbdroid4");
        mHandles[56] = MissionUtility::GetHandle("wookiee_alarm");
        mHandles[48] = MissionUtility::GetHandle("wookiee_runaway1");
        mHandles[49] = MissionUtility::GetHandle("wookiee_runaway2");
        mHandles[50] = MissionUtility::GetHandle("wookiee_runaway3");
        mHandles[51] = MissionUtility::GetHandle("wookiee_runaway4");
        mHandles[52] = MissionUtility::GetHandle("wookiee_runaway5");
        mHandles[53] = MissionUtility::GetHandle("wookiee_runaway6");
        mHandles[54] = MissionUtility::GetHandle("wookiee_runaway7");
        mHandles[55] = MissionUtility::GetHandle("wookiee_runaway8");
        mHandles[142] = MissionUtility::GetHandle("OpenCinHarvesterOn");
        mHandles[143] = MissionUtility::GetHandle("OpenCinHarvesterOff");
        mHandles[47] = MissionUtility::GetHandle("gnasp_hive");
        MissionUtility::PreloadConfig("rep_inf_anakin");
        MissionUtility::PreloadConfig("cis_bike_speeder_player");
        MissionUtility::PreloadConfig("cis_bike_speeder_bera");
        MissionUtility::PreloadConfig("NEU_prop_harvester_effect_spread");
        MissionUtility::PreloadConfig("Neu_prop_lightsaber");
        MissionUtility::PreloadConfig("NEU_prop_harvester_effect");
        MissionUtility::PreloadConfig("REP_prop_lightsaber");
        MissionUtility::PreloadConfig("cis_bike_speeder_player_mount");
        MissionUtility::PreloadConfig("cis_bike_speeder_bera_mount");
        MissionUtility::PreloadConfig("kas_crea_gnasp_flight");
}

// `SPMission *`, like the other 28 factories: `Kashyyyk1Script` derives from `SPMission`, and
// all 29 are called from one `strstr` chain in `DllBase.cpp::LoadScript` that assigns them to
// one variable, so the real header declared them with one type. Single inheritance means the
// pointer needs no adjustment, so `DLLBase *` produced the same 60 bytes -- CodeWarrior does
// not mangle a return type. MSVC does, and the port had to `static_cast` around it.
SPMission *Kashyyyk1BuildMission()
{
    return new Kashyyyk1Script;
}

Kashyyyk1Script::Kashyyyk1Script()
{
    int i, j;

    for (j = 0; j < 4; j++)
        for (i = 0; i < 4; i++)
            mMatrixA.m[j][i] = 0.0f;
    for (j = 0; j < 4; j++)
        for (i = 0; i < 4; i++)
            mMatrixB.m[j][i] = 0.0f;
    for (j = 0; j < 4; j++)
        for (i = 0; i < 4; i++)
            mMatrixC.m[j][i] = 0.0f;

    mBoolCount = 155;   mBools  = mFlags;
    mCountB    = 0;     mIntsB  = &mUnusedB;
    mCountC    = 170;   mIntsC  = mHandles;
    mCountD    = 16;    mBlockD = mInts;
}
