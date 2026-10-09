// UnitTestScript.cpp -- reconstruction of the developer memory-diagnostic script.
//
// Not a real mission: it measures how much texture and main memory a config costs, spawns one
// object, measures again, and prints the deltas in kilobytes between two marker lines so a
// harness can scrape them. The last mission-tier TU under 1 KB.
//
//   ~UnitTestScript       108 B
//   Execute                24 B
//   Setup                 308 B
//   UnitTestBuildMission  132 B
//                         -----
//                         572 B  == the TU's shipped size
//
// Build: the mission-script flag set (-O4, -RTTI on).

static const char *const kClassName = "UnitTestScript";
static const char *const kBaseName  = "SPMission";

extern "C" int sprintf(char *buf, const char *fmt, ...);

namespace MissionUtility
{
    int   CreateObjectWithRotation(const char*, const char*, int, const char*, int, int, int);
    void  DebugLog(char*);
    int   GetFreeMemory();
    int   GetTextureMemory();
    void  PreloadConfig(const char*);
}

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

class UnitTestScript : public SPMission
{
public:
    virtual ~UnitTestScript();

    UnitTestScript()
    {
        mBoolCount = 1;   mBools    = &mDone;
        mCountB    = 0;   mIntsB    = mIntsBStore;
        mCountC    = 2;   mIntsC    = &mObject;
        mCountD    = 0;   mBlockD   = &mBlockDStore;
    }

    virtual void Setup();
    virtual void Execute();

    char  mPad24;
    bool  mDone;                     // +0x25
    char  mPad26[6];
    int   mIntsBStore[2];           // +0x2c
    int   mObject;                  // +0x34  the spawned test object
    int   mIntsCStore2;             // +0x38
    int   mPad3c[2];                // +0x3c
    int   mBlockDStore;             // +0x44  -> sizeof == 0x48
};

UnitTestScript::~UnitTestScript()
{
}

void UnitTestScript::Execute()
{
    // Runs once on the tick after Setup, then disarms itself. The measurement all happens in
    // Setup; this only clears the latch.
    if (!mDone) {
        return;
    }
    mDone = false;
}

void UnitTestScript::Setup()
{
    mDone = true;

    int tex0  = MissionUtility::GetTextureMemory();
    int free0 = MissionUtility::GetFreeMemory();

    MissionUtility::PreloadConfig("unittest");

    int free1 = MissionUtility::GetFreeMemory();
    int tex1  = MissionUtility::GetTextureMemory();

    // Main memory the config cost, then texture memory. The printed order is
    // total / non-texture / texture / object.
    float memKB = (free0 - free1) / 1024.0f;
    float texKB = (tex1 - tex0) / 1024.0f;

    mObject = MissionUtility::CreateObjectWithRotation("unittest", "spawn1", 0,
                                                       "UnitTest1", 0, -1, -1);

    free0 = free1;
    float objKB = (free0 - MissionUtility::GetFreeMemory()) / 1024.0f;

    char buf[256];
    sprintf(buf, "UNITTEST OUTPUT BEGIN\n%0.1f\t%0.1f\t%0.1f\t%0.1f\nUNITTEST OUTPUT END\n",
            memKB, memKB - texKB, texKB, objKB);
    MissionUtility::DebugLog(buf);
}

SPMission *UnitTestBuildMission()
{
    return new UnitTestScript();
}
