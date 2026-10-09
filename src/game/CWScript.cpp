// CWScript.cpp -- reconstruction of the mission-script host.
//
// Not a mission: this is the TU that attaches the mission-script system to the frame loop.
// It confirms the `ScriptFeature : GameFeature` design end to end, including the exact
// registration priorities, which are visible nowhere else.
//
//   ClearAmbientStreams   100 B
//   PostRunScript         164 B
//   SimulateScript         56 B
//   PreLoadScript          76 B
//   LoadMissionScript      32 B
//   __sinit_CWScript_cpp   76 B
//                         -----
//                         504 B  == the TU's shipped size
//
// Build: the mission-script flag set (-O4, -RTTI on). See analysis/phase5_status.md.

// ---------------------------------------------------------------------------------------
// Engine side -- declarations only; every one of these is defined in another TU.
// ---------------------------------------------------------------------------------------

// The return types are the engine's own (ZeroRef.h): AddRef returns `this`, Release the new
// count. CodeWarrior does not mangle a return type, so `void` here was byte-exact and wrong --
// found by the whole-probes/src link audit (tools/link_audit.py), which is the only instrument
// that can see this defect class.
class ZeroRef
{
public:
    ZeroRef *AddRef();
    int Release();
};

class SoundQueue : public ZeroRef {};
class SoundStream : public ZeroRef {};

class SoundEngine
{
public:
    static SoundEngine *sInstance;
    SoundQueue *CreateQueue();
};

// A namespace, not a class with statics -- ZeroProfiler.h says so. CodeWarrior mangles a
// static member and a namespace function identically (`Start__12ZeroProfilerFPCc`), MSVC does
// not; the class spelling was byte-exact and unlinkable (link audit, same round).
namespace ZeroProfiler
{
    void Start(const char *name);
    void End(const char *name);
}

class SquadCommandSounds
{
public:
    static void Create();
    static void Destroy();
};

class GameFeature
{
public:
    static void PostLoadAdd(int priority, void (*fn)());
    static void SimulateAdd(int priority, void (*fn)(float));
    static void PostRunAdd(int priority, void (*fn)());
};

// The script system proper, in its own TU (`LoadScript`/`InitializeScript`/`UpdateScript`/
// `PostRunScript` are free functions there, not members). LoadScript returns bool -- the
// r3 result DllBase.cpp's recovery settled (include/game/mission.h); `void` was another
// CW-invisible return-type miss the link audit caught. This TU discards the result.
bool  LoadScript(const char *name);
void  InitializeScript();
void  UpdateScript();
void  PostRunScript();

// ---------------------------------------------------------------------------------------

class ScriptFeature
{
public:
    static void PreLoadScript();
    static void SimulateScript(float dt);
    static void PostRunScript();
    static void ClearAmbientStreams();

    static SoundQueue   *mSoundQueue;
    static SoundQueue   *mAmbientQueue;
    static SoundStream  *mStepTerrainSound;
    static SoundStream  *mAmbientStreams[2];
};

SoundQueue  *ScriptFeature::mSoundQueue;
SoundQueue  *ScriptFeature::mAmbientQueue;
SoundStream *ScriptFeature::mStepTerrainSound;
SoundStream *ScriptFeature::mAmbientStreams[2];

void ScriptFeature::ClearAmbientStreams()
{
    for (int i = 0; i < 2; i++) {
        if (mAmbientStreams[i]) {
            mAmbientStreams[i]->Release();
            mAmbientStreams[i] = 0;
        }
    }
}

void ScriptFeature::PostRunScript()
{
    ::PostRunScript();

    mSoundQueue->Release();
    mSoundQueue = 0;
    mAmbientQueue->Release();
    mAmbientQueue = 0;

    if (mStepTerrainSound) {
        mStepTerrainSound->Release();
        mStepTerrainSound = 0;
    }

    ClearAmbientStreams();

    SquadCommandSounds::Destroy();
}

void ScriptFeature::SimulateScript(float)
{
    ZeroProfiler::Start("MissionScript");
    UpdateScript();
    ZeroProfiler::End("MissionScript");
}

void ScriptFeature::PreLoadScript()
{
    InitializeScript();

    mSoundQueue = SoundEngine::sInstance->CreateQueue();
    mSoundQueue->AddRef();
    mAmbientQueue = SoundEngine::sInstance->CreateQueue();
    mAmbientQueue->AddRef();

    mStepTerrainSound = 0;

    SquadCommandSounds::Create();
}

void LoadMissionScript(const char *name)
{
    LoadScript(name);
}

// The three frame-loop hooks and their priorities, which is the whole reason this TU exists.
// Registration happens from file scope, so it lands in __sinit_CWScript_cpp; an empty helper
// object emits the call and no store, which is what the shipped code does.
struct PostLoadReg { PostLoadReg(int p, void (*f)())      { GameFeature::PostLoadAdd(p, f); } };
struct SimulateReg { SimulateReg(int p, void (*f)(float)) { GameFeature::SimulateAdd(p, f); } };
struct PostRunReg  { PostRunReg (int p, void (*f)())      { GameFeature::PostRunAdd (p, f); } };

static PostLoadReg sPreLoadReg (0x159, ScriptFeature::PreLoadScript);
static SimulateReg sSimulateReg(0x15a, ScriptFeature::SimulateScript);
static PostRunReg  sPostRunReg (0x15b, ScriptFeature::PostRunScript);
