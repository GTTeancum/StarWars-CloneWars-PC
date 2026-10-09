// Multi17Script.cpp -- reconstruction of a shipped mission script.
//
// The Geonosis arena wave mode: the same twenty-six-wave skeleton as Multi5Script, with
// a Jedi/Amidala escort and a landing gunship on top. 9,728 bytes over 10 KB of .rodata
// and 6 KB of .data, rebuilt from the shipped bytes with tools/gen_mission_data.py and
// tools/dis_func.py.
//
// All nine functions match the shipped bytes, checked by tools/verify_missions.py.
//
// What is different from Multi5Script, beyond the mission itself:
//
//   * The tables live in `namespace multi17Names`. That is not a guess: CodeWarrior
//     appends the enclosing namespace to every file-scope variable, so the shipped
//     symbol table literally reads `Wave1Enemies_4__12multi17Names`. Multi5Script uses
//     the same table names at global scope, which is presumably why the second mission
//     to want them wrapped its own.
//   * The per-wave descriptor is 28 bytes, not 32: this mode has no bonus pickups, so
//     there is no `spaceFruits` column. Execute indexes it with `mulli rN, wave, 0x1c`
//     rather than a shift.
//   * The script owns four `Timer` objects, constructed out of line by the derived
//     constructor -- which is why `Multi17BuildMission` carries four `__ct__5TimerFv`
//     calls that Multi5's does not. Two of them drive the escort and the gunship; the
//     other two are constructed and never read.
//   * Execute spawns through the `Quat` overload of CreateObject, which confirms the
//     `Quat{s,x,y,z}` shape the SWBF2 maintainer's audit response described.

// The script-facing quaternion from CWScriptUtils.h. It is passed to CreateObject by
// value, and every call site builds its own temporary, so it is constructed rather than
// assigned -- which is why it has a constructor and `Enemy` does not.
// `class`, not `struct`: the other 28 scripts and the generated stub layer all spell Quat as
// a class, and MSVC mangles the keyword into every by-value parameter (UQuat@@ vs VQuat@@) --
// so the `struct` spelling left this TU's CreateObject call unresolvable against the very
// stubs generated from its siblings. CodeWarrior does not make the distinction, so the byte
// match never could either (link audit, round 24).
class Quat
{
public:
    Quat(float s_, float x_, float y_, float z_) { s = s_; x = x_; y = y_; z = z_; }

    float s, x, y, z;
};

namespace MissionUtility
{
    void  SetMusicLooping(bool loop);
    void  PlayMusic(const char *name, bool loop);
    void  StartAmbiences(const char *ambience, const char *stinger, float in, float out);
    int   RunCin(const char *name, bool a, bool b);
    int   AddObjective(const char *text);
    void  SetEnemiesOneWay(int a, int b);
    void  SetAlliance(int a, int b);
    void  SetMapZoom(float in, float out);
    void  PreloadConfig(const char *odf);

    int   MPGetNumPlayers();
    bool  MPHasGameStarted();
    int   MPGetWave();
    void  MPStartGladiatorRound();

    int   GetRandomInt(int lo, int hi);
    float GetTime();
    bool  IsAlive(int handle);
    void  RemoveObject(int handle);
    float GetDistance(int handle, const char *path, int point);

    int   GetPlayerHandle(int player);
    int   GetODF(int handle);
    int   GetCRC(const char *name);
    int   MPGetNumLivePlayers();
    void  MPSetWave(int wave);
    bool  MPIsGladiatorRound();
    int   StartSound(const char *name, bool loop, float volume, float fadeIn,
                     float fadeOut, const char *bus, int flags, const char *tag);
    int   AddAmmoBox(const char *odf, int a, float b);
    int   AddHealthBox(const char *odf, int a, float b);

    bool  IsLanded(int handle);
    void  TakeOff(int handle);
    void  Land(int handle, const char *path, int point, float speed);
    void  SetVelocNeutralFly(int handle, float v);
    void  SetVelocMaximumFly(int handle, float v);
    void  SetVelocMinimumFly(int handle, float v);
    void  SetTakeoffAltitude(int handle, float v);

    int   CreateObject(const char *odf, const char *where, const char *team,
                       int a, int b, int c);
    int   CreateObject(const char *odf, const char *where, int a, const char *name,
                       int team, int b,
                       Quat facing = Quat(1.0f, 0.0f, 0.0f, 0.0f), int c = -1);
    int   CreateObject(const char *odf, int carrier, const char *where,
                       const char *name, int team, int a, int b);
    float GetMaxHealth(int handle);
    float GetMaxShield(int handle);
    void  SetMaxHealth(int handle, float v);
    void  SetMaxShield(int handle, float v);
    void  SetCurHealth(int handle, float v);
    void  SetCurShield(int handle, float v);
    void  SetQueueFlag(bool on);
    void  Patrol(int handle, const char *path, float speed, bool loop);
    void  Goto(int handle, const char *path, int point);
    void  Goto(int handle, const char *path, bool queue);
}

extern "C" {
    void *memset(void *dst, int c, unsigned long n);
    int   strcmp(const char *a, const char *b);
}

// The two script-facing table shapes this mission uses. `Enemy` is read one entry at a
// time with `lhzx` and then split into its two signed bytes.
struct Enemy
{
    char odf;                   // index into ENEMY_ODFS
    char spawn;                 // index into SpawnPoints / PatrolPaths; 32 means "any"
};

// ---------------------------------------------------------------------------------------
// The mission's tuning data, in the shipped declaration order. Generated by
// tools/gen_mission_data.py; do not reorder or resize anything here without re-running
// the ordering audit.
// ---------------------------------------------------------------------------------------

namespace multi17Names
{

const bool Wave1DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave2DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave3DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave4WaitTimes[1] = {
    0.0f,
};

const bool Wave4DontWaitForDead[1] = {
    0,
};

const Enemy Wave4Enemies[1] = {
    {  0,  0 },
};

const bool Wave5DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave6DontWaitForDead[6] = {
    0, 0, 0, 1, 0, 0,
};

const bool Wave7DontWaitForDead[6] = {
    1, 1, 1, 1, 0, 0,
};

const float Wave8WaitTimes[1] = {
    0.0f,
};

const bool Wave8DontWaitForDead[1] = {
    0,
};

const Enemy Wave8Enemies[1] = {
    {  0,  0 },
};

const bool Wave9DontWaitForDead[6] = {
    1, 1, 1, 1, 0, 0,
};

const bool Wave10DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave11DontWaitForDead[6] = {
    0, 1, 0, 1, 0, 0,
};

const float Wave12WaitTimes[1] = {
    0.0f,
};

const bool Wave12DontWaitForDead[1] = {
    0,
};

const Enemy Wave12Enemies[1] = {
    {  0,  0 },
};

const bool Wave13DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave14DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave15DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave16WaitTimes[1] = {
    0.0f,
};

const bool Wave16DontWaitForDead[1] = {
    0,
};

const Enemy Wave16Enemies[1] = {
    {  0,  0 },
};

const bool Wave17DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave18DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave19DontWaitForDead[6] = {
    0, 0, 0, 0, 0, 0,
};

const float Wave20WaitTimes[1] = {
    0.0f,
};

const bool Wave20DontWaitForDead[1] = {
    0,
};

const Enemy Wave20Enemies[1] = {
    {  0,  0 },
};

const bool Wave21DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave22DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave23DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave24DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave25DontWaitForDead[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave1DontWaitForDead_2[6] = {
    0, 0, 0, 0, 0, 0,
};

const bool Wave2DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave3DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave4WaitTimes_2[1] = {
    2.0f,
};

const bool Wave4DontWaitForDead_2[1] = {
    0,
};

const Enemy Wave4Enemies_2[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave5DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave6DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave7DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave8WaitTimes_2[1] = {
    2.0f,
};

const bool Wave8DontWaitForDead_2[1] = {
    0,
};

const Enemy Wave8Enemies_2[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave9DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave10DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave11DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave12WaitTimes_2[1] = {
    2.0f,
};

const bool Wave12DontWaitForDead_2[1] = {
    0,
};

const Enemy Wave12Enemies_2[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave13DontWaitForDead_2[6] = {
    0, 1, 0, 1, 0, 0,
};

const bool Wave14DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave15DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave16WaitTimes_2[1] = {
    2.0f,
};

const bool Wave16DontWaitForDead_2[1] = {
    0,
};

const Enemy Wave16Enemies_2[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave17DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave18DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave19DontWaitForDead_2[6] = {
    0, 0, 0, 1, 0, 0,
};

const float Wave20WaitTimes_2[1] = {
    2.0f,
};

const bool Wave20DontWaitForDead_2[1] = {
    0,
};

const Enemy Wave20Enemies_2[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave21DontWaitForDead_2[6] = {
    0, 1, 0, 1, 0, 0,
};

const bool Wave22DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave23DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave24DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave25DontWaitForDead_2[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave1DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave2DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave3DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave4WaitTimes_3[1] = {
    2.0f,
};

const bool Wave4DontWaitForDead_3[1] = {
    0,
};

const Enemy Wave4Enemies_3[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave5DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave6DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave7DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave8WaitTimes_3[1] = {
    2.0f,
};

const bool Wave8DontWaitForDead_3[1] = {
    0,
};

const Enemy Wave8Enemies_3[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave9DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave10DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave11DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave12WaitTimes_3[1] = {
    2.0f,
};

const bool Wave12DontWaitForDead_3[1] = {
    0,
};

const Enemy Wave12Enemies_3[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave13DontWaitForDead_3[6] = {
    0, 0, 0, 0, 0, 0,
};

const bool Wave14DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave15DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave16WaitTimes_3[1] = {
    2.0f,
};

const bool Wave16DontWaitForDead_3[1] = {
    0,
};

const Enemy Wave16Enemies_3[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave17DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave18DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave19DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave20WaitTimes_3[1] = {
    2.0f,
};

const bool Wave20DontWaitForDead_3[1] = {
    0,
};

const Enemy Wave20Enemies_3[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave21DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave22DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave23DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave24DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave25DontWaitForDead_3[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave1DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave2DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave3DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave4WaitTimes_4[1] = {
    2.0f,
};

const bool Wave4DontWaitForDead_4[1] = {
    0,
};

const Enemy Wave4Enemies_4[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave5DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave6DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave7DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave8WaitTimes_4[1] = {
    2.0f,
};

const bool Wave8DontWaitForDead_4[1] = {
    0,
};

const Enemy Wave8Enemies_4[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave9DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave10DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave11DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave12WaitTimes_4[1] = {
    2.0f,
};

const bool Wave12DontWaitForDead_4[1] = {
    0,
};

const Enemy Wave12Enemies_4[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave13DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave14DontWaitForDead_4[7] = {
    0, 1, 1, 1, 1, 0, 0,
};

const bool Wave15DontWaitForDead_4[6] = {
    0, 1, 0, 1, 0, 0,
};

const float Wave16WaitTimes_4[1] = {
    2.0f,
};

const bool Wave16DontWaitForDead_4[1] = {
    0,
};

const Enemy Wave16Enemies_4[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave17DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave18DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave19DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const float Wave20WaitTimes_4[1] = {
    2.0f,
};

const bool Wave20DontWaitForDead_4[1] = {
    0,
};

const Enemy Wave20Enemies_4[2] = {
    {  7,  0 }, {  0,  0 },
};

const bool Wave21DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave22DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave23DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave24DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const bool Wave25DontWaitForDead_4[6] = {
    0, 1, 1, 1, 0, 0,
};

const char *Wave4Sounds[1] = {
    0,
};

const char *Wave4HealthPacks[1] = {
    0,
};

const char *Wave4AmmoPacks[1] = {
    0,
};

const char *Wave8Sounds[1] = {
    0,
};

const char *Wave8HealthPacks[1] = {
    0,
};

const char *Wave8AmmoPacks[1] = {
    0,
};

const char *Wave12Sounds[1] = {
    0,
};

const char *Wave12HealthPacks[1] = {
    0,
};

const char *Wave12AmmoPacks[1] = {
    0,
};

const char *Wave16Sounds[1] = {
    0,
};

const char *Wave16HealthPacks[1] = {
    0,
};

const char *Wave16AmmoPacks[1] = {
    0,
};

const char *Wave20Sounds[1] = {
    0,
};

const char *Wave20HealthPacks[1] = {
    0,
};

const char *Wave20AmmoPacks[1] = {
    0,
};

const char *ENEMY_ODFS[8] = {
    0,
    "cis_bike_speeder",
    "geo_inf_geonosian",
    "cis_inf_droid",
    "cis_inf_sbdroid",
    "CIS_walk_small_jedi",
    "",
    "GLADIATOR",
};

const char *SpawnPoints[32] = {
    "enemyspawn1a",
    "enemyspawn1b",
    "enemyspawn1c",
    "enemyspawn1d",
    "enemyspawn1e",
    "enemyspawn1f",
    "enemyspawn1g",
    "enemyspawn1h",
    "enemyspawn2a",
    "enemyspawn2b",
    "enemyspawn2c",
    "enemyspawn2d",
    "enemyspawn2e",
    "enemyspawn2f",
    "enemyspawn2g",
    "enemyspawn2h",
    "enemyspawn3a",
    "enemyspawn3b",
    "enemyspawn3c",
    "enemyspawn3d",
    "enemyspawn3e",
    "enemyspawn3f",
    "enemyspawn3g",
    "enemyspawn3h",
    "enemyspawn4a",
    "enemyspawn4b",
    "enemyspawn4c",
    "enemyspawn4d",
    "enemyspawn4e",
    "enemyspawn4f",
    "enemyspawn4g",
    "enemyspawn4h",
};

const char *PatrolPaths[32] = {
    "PatrolPath1a",
    "PatrolPath1b",
    "PatrolPath1c",
    "PatrolPath1d",
    "PatrolPath1e",
    "PatrolPath1f",
    "PatrolPath1g",
    "PatrolPath1h",
    "PatrolPath2a",
    "PatrolPath2b",
    "PatrolPath2c",
    "PatrolPath2d",
    "PatrolPath2e",
    "PatrolPath2f",
    "PatrolPath2g",
    "PatrolPath2h",
    "PatrolPath3a",
    "PatrolPath3b",
    "PatrolPath3c",
    "PatrolPath3d",
    "PatrolPath3e",
    "PatrolPath3f",
    "PatrolPath3g",
    "PatrolPath3h",
    "PatrolPath4a",
    "PatrolPath4b",
    "PatrolPath4c",
    "PatrolPath4d",
    "PatrolPath4e",
    "PatrolPath4f",
    "PatrolPath4g",
    "PatrolPath4h",
};

const unsigned char WaveIncrements[26] = {
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 0,
};

const char *Wave1Sounds[5] = {
    "MWM26_074",
    0,
    0,
    0,
    0,
};

const char *Wave1HealthPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave1AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave1WaitTimes[6] = {
    2.0f, 1.0f, 2.0f, 1.0f, 2.0f, 3.0f,
};

const Enemy Wave1Enemies[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave2Sounds[5] = {
    "MWM26_075",
    0,
    0,
    0,
    0,
};

const char *Wave2HealthPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave2AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave2WaitTimes[6] = {
    1.0f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f,
};

const Enemy Wave2Enemies[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave3Sounds[5] = {
    "MWM26_076",
    0,
    0,
    0,
    0,
};

const char *Wave3HealthPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave3AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave3WaitTimes[6] = {
    1.0f, 1.0f, 1.5f, 1.0f, 1.0f, 2.0f,
};

const Enemy Wave3Enemies[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave5Sounds[5] = {
    "MWM26_077",
    0,
    0,
    0,
    0,
};

const float Wave5WaitTimes[6] = {
    1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f,
};

const char *Wave5HealthPacks[7] = {
    0,
    0,
    0,
    "powerup2",
    0,
    0,
    0,
};

const char *Wave5AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave5Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave6Sounds[5] = {
    "MWM26_078",
    0,
    0,
    0,
    0,
};

const char *Wave6HealthPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave6AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave6WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 2.0f, 1.0f, 3.0f,
};

const Enemy Wave6Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave7Sounds[5] = {
    "MWM26_079",
    0,
    0,
    0,
    0,
};

const float Wave7WaitTimes[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f,
};

const char *Wave7HealthPacks[7] = {
    0,
    0,
    0,
    0,
    0,
    "powerup3",
    0,
};

const char *Wave7AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave7Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave9Sounds[5] = {
    "MWM26_080",
    0,
    0,
    0,
    0,
};

const float Wave9WaitTimes[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f,
};

const char *Wave9HealthPacks[7] = {
    0,
    0,
    0,
    0,
    0,
    "powerup4",
    0,
};

const char *Wave9AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave9Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave10Sounds[5] = {
    "MWM26_081",
    0,
    0,
    0,
    0,
};

const float Wave10WaitTimes[6] = {
    2.0f, 1.0f, 2.0f, 2.0f, 1.0f, 2.0f,
};

const char *Wave10HealthPacks[8] = {
    0,
    0,
    0,
    "powerup8",
    "powerup6",
    0,
    0,
    0,
};

const char *Wave10AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave10Enemies[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  3, 20 }, {  4, 21 },
    {  3, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave11Sounds[5] = {
    "MWM26_082",
    0,
    0,
    0,
    0,
};

const float Wave11WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const char *Wave11HealthPacks[7] = {
    0,
    0,
    0,
    0,
    "powerup10",
    0,
    0,
};

const char *Wave11AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave11Enemies[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave13Sounds[5] = {
    "MWM26_083",
    0,
    0,
    0,
    0,
};

const float Wave13WaitTimes[6] = {
    2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 2.0f,
};

const char *Wave13HealthPacks[9] = {
    0,
    0,
    "powerup1",
    "powerup2",
    0,
    0,
    "powerup3",
    0,
    0,
};

const char *Wave13AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave13Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave14Sounds[5] = {
    "MWM26_084",
    0,
    0,
    0,
    0,
};

const char *Wave14HealthPacks[7] = {
    0,
    0,
    0,
    0,
    0,
    "powerup4",
    0,
};

const char *Wave14AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave14WaitTimes[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.0f,
};

const Enemy Wave14Enemies[38] = {
    {  0,  0 }, {  2,  0 }, {  4,  1 }, {  2,  2 }, {  4,  3 },
    {  2,  4 }, {  4,  5 }, {  2,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  3,  9 }, {  4, 10 }, {  3, 11 }, {  4, 12 },
    {  3, 13 }, {  4, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  2, 20 }, {  4, 21 },
    {  2, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  4, 26 }, {  4, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave15Sounds[5] = {
    "MWM26_085",
    0,
    0,
    0,
    0,
};

const float Wave15WaitTimes[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const char *Wave15HealthPacks[8] = {
    0,
    0,
    "powerup5",
    0,
    0,
    "powerup6",
    0,
    0,
};

const char *Wave15AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave15Enemies[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave17Sounds[5] = {
    "MWM26_086",
    0,
    0,
    0,
    0,
};

const float Wave17WaitTimes[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const char *Wave17HealthPacks[7] = {
    0,
    "powerup7",
    0,
    0,
    0,
    0,
    0,
};

const char *Wave17AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave17Enemies[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave18Sounds[5] = {
    "MWM26_087",
    0,
    0,
    0,
    0,
};

const float Wave18WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const char *Wave18HealthPacks[8] = {
    0,
    0,
    "powerup8",
    "powerup9",
    0,
    0,
    0,
    0,
};

const char *Wave18AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave18Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  4,  1 }, {  3,  2 }, {  4,  3 },
    {  3,  4 }, {  4,  5 }, {  3,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  3, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  3, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave19Sounds[5] = {
    "MWM26_088",
    0,
    0,
    0,
    0,
};

const float Wave19WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const char *Wave19HealthPacks[8] = {
    0,
    0,
    "powerup10",
    0,
    0,
    "powerup11",
    0,
    0,
};

const char *Wave19AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave19Enemies[34] = {
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  5,  5 }, {  5, 13 }, {  5, 21 }, {  5, 29 }, {  0,  0 },
    {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 }, {  5,  5 },
    {  5, 13 }, {  5, 21 }, {  5, 29 }, {  5,  7 }, {  5, 15 },
    {  5, 23 }, {  5, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave21Sounds[5] = {
    "MWM26_089",
    0,
    0,
    0,
    0,
};

const char *Wave21HealthPacks[8] = {
    0,
    0,
    "powerup1",
    0,
    0,
    "powerup2",
    0,
    0,
};

const char *Wave21AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave21WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave21Enemies[38] = {
    {  0,  0 }, {  4,  0 }, {  3,  1 }, {  3,  2 }, {  4,  3 },
    {  3,  4 }, {  4,  5 }, {  3,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  4, 25 },
    {  3, 26 }, {  4, 27 }, {  3, 28 }, {  4, 29 }, {  3, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave22Sounds[5] = {
    "MWM26_090",
    0,
    0,
    0,
    0,
};

const char *Wave22HealthPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave22AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave22WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 4.0f, 1.0f, 3.0f,
};

const Enemy Wave22Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  5, 28 }, {  4, 29 }, {  4, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave23Sounds[5] = {
    "MWM26_091",
    0,
    0,
    0,
    0,
};

const char *Wave23HealthPacks[8] = {
    0,
    "powerup3",
    0,
    0,
    0,
    "powerup4",
    0,
    0,
};

const char *Wave23AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave23WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 3.0f,
};

const Enemy Wave23Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  5,  7 }, {  0,  0 },
    {  5,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  5, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave24Sounds[5] = {
    "MWM26_092",
    0,
    0,
    0,
    0,
};

const char *Wave24HealthPacks[7] = {
    0,
    0,
    0,
    0,
    0,
    "powerup5",
    0,
};

const char *Wave24AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const float Wave24WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave24Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  5,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  5, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave25Sounds[5] = {
    "MWM26_093",
    0,
    0,
    0,
    0,
};

const char *Wave25HealthPacks[9] = {
    0,
    "powerup6",
    0,
    0,
    "powerup7",
    0,
    "powerup8",
    0,
    0,
};

const char *Wave25AmmoPacks[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave1Sounds_2[5] = {
    "MWM26_074",
    0,
    0,
    0,
    0,
};

const char *Wave1HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave1AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave2Sounds_2[5] = {
    "MWM26_075",
    0,
    0,
    0,
    0,
};

const char *Wave2HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave2AmmoPacks_2[7] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave3Sounds_2[5] = {
    "MWM26_076",
    0,
    0,
    0,
    0,
};

const char *Wave3HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave3AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave5Sounds_2[5] = {
    "MWM26_077",
    0,
    0,
    0,
    0,
};

const char *Wave5HealthPacks_2[8] = {
    0,
    0,
    "powerup1",
    "powerup2",
    0,
    0,
    0,
    0,
};

const char *Wave5AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave6Sounds_2[5] = {
    "MWM26_078",
    0,
    0,
    0,
    0,
};

const char *Wave6HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave6AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave7Sounds_2[5] = {
    "MWM26_079",
    0,
    0,
    0,
    0,
};

const char *Wave7HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave7AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave9Sounds_2[5] = {
    "MWM26_080",
    0,
    0,
    0,
    0,
};

const char *Wave9HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave9AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave10Sounds_2[5] = {
    "MWM26_081",
    0,
    0,
    0,
    0,
};

const char *Wave10HealthPacks_2[8] = {
    0,
    0,
    "powerup3",
    "powerup4",
    0,
    0,
    0,
    0,
};

const char *Wave10AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave11Sounds_2[5] = {
    "MWM26_082",
    0,
    0,
    0,
    0,
};

const char *Wave11HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave11AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave13Sounds_2[5] = {
    "MWM26_083",
    0,
    0,
    0,
    0,
};

const char *Wave13HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave13AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave14Sounds_2[5] = {
    "MWM26_084",
    0,
    0,
    0,
    0,
};

const char *Wave14HealthPacks_2[8] = {
    0,
    0,
    "powerup5",
    "powerup6",
    0,
    0,
    0,
    0,
};

const char *Wave14AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave15Sounds_2[5] = {
    "MWM26_085",
    0,
    0,
    0,
    0,
};

const char *Wave15HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave15AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave17Sounds_2[5] = {
    "MWM26_086",
    0,
    0,
    0,
    0,
};

const char *Wave17HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave17AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave18Sounds_2[5] = {
    "MWM26_087",
    0,
    0,
    0,
    0,
};

const char *Wave18HealthPacks_2[8] = {
    0,
    0,
    "powerup7",
    "powerup8",
    0,
    0,
    0,
    0,
};

const char *Wave18AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave19Sounds_2[5] = {
    "MWM26_088",
    0,
    0,
    0,
    0,
};

const char *Wave19HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave19AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave21Sounds_2[5] = {
    "MWM26_089",
    0,
    0,
    0,
    0,
};

const char *Wave21HealthPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave21AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave22Sounds_2[5] = {
    "MWM26_090",
    0,
    0,
    0,
    0,
};

const char *Wave22HealthPacks_2[10] = {
    0,
    "powerup11",
    0,
    "powerup8",
    "powerup9",
    "powerup10",
    0,
    0,
    0,
    0,
};

const char *Wave22AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave23Sounds_2[5] = {
    "MWM26_091",
    0,
    0,
    0,
    0,
};

const char *Wave23HealthPacks_2[7] = {
    0,
    "powerup10",
    0,
    0,
    0,
    0,
    0,
};

const char *Wave23AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave24Sounds_2[5] = {
    "MWM26_092",
    0,
    0,
    0,
    0,
};

const float Wave25WaitTimes[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave25Enemies[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  5,  8 }, {  5,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  4, 20 }, {  5, 21 },
    {  5, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  5, 28 }, {  4, 29 }, {  4, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const unsigned char WaveIncrements_2[26] = {
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 0,
};

const float Wave1WaitTimes_2[6] = {
    2.0f, 1.0f, 2.0f, 1.0f, 2.0f, 3.0f,
};

const Enemy Wave1Enemies_2[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave2WaitTimes_2[6] = {
    1.0f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f,
};

const Enemy Wave2Enemies_2[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave3WaitTimes_2[6] = {
    2.0f, 1.0f, 1.5f, 1.0f, 1.0f, 3.0f,
};

const Enemy Wave3Enemies_2[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const char *Wave4Sounds_2[1] = {
    "MWM26_001",
};

const char *Wave4HealthPacks_2[1] = {
    0,
};

const char *Wave4AmmoPacks_2[1] = {
    0,
};

const char *Wave8Sounds_2[1] = {
    "MWM26_001",
};

const char *Wave8HealthPacks_2[1] = {
    0,
};

const char *Wave8AmmoPacks_2[1] = {
    0,
};

const char *Wave12Sounds_2[1] = {
    "MWM26_001",
};

const char *Wave12HealthPacks_2[1] = {
    0,
};

const char *Wave12AmmoPacks_2[1] = {
    0,
};

const char *Wave16Sounds_2[1] = {
    "MWM26_001",
};

const char *Wave16HealthPacks_2[1] = {
    0,
};

const char *Wave16AmmoPacks_2[1] = {
    0,
};

const char *Wave20Sounds_2[1] = {
    "MWM26_001",
};

const char *Wave20HealthPacks_2[1] = {
    0,
};

const char *Wave20AmmoPacks_2[1] = {
    0,
};

const char *Wave4Sounds_3[1] = {
    "MWM26_001",
};

const char *Wave4HealthPacks_3[1] = {
    0,
};

const char *Wave4AmmoPacks_3[1] = {
    0,
};

const char *Wave8Sounds_3[1] = {
    "MWM26_001",
};

const char *Wave8HealthPacks_3[1] = {
    0,
};

const char *Wave8AmmoPacks_3[1] = {
    0,
};

const char *Wave12Sounds_3[1] = {
    "MWM26_001",
};

const char *Wave12HealthPacks_3[1] = {
    0,
};

const char *Wave12AmmoPacks_3[1] = {
    0,
};

const char *Wave16Sounds_3[1] = {
    "MWM26_001",
};

const char *Wave16HealthPacks_3[1] = {
    0,
};

const char *Wave16AmmoPacks_3[1] = {
    0,
};

const char *Wave20Sounds_3[1] = {
    "MWM26_001",
};

const char *Wave20HealthPacks_3[1] = {
    0,
};

const char *Wave20AmmoPacks_3[1] = {
    0,
};

const char *Wave4Sounds_4[1] = {
    "MWM26_001",
};

const char *Wave4HealthPacks_4[1] = {
    0,
};

const char *Wave4AmmoPacks_4[1] = {
    0,
};

const char *Wave8Sounds_4[1] = {
    "MWM26_001",
};

const char *Wave8HealthPacks_4[1] = {
    0,
};

const char *Wave8AmmoPacks_4[1] = {
    0,
};

const char *Wave12Sounds_4[1] = {
    "MWM26_001",
};

const char *Wave12HealthPacks_4[1] = {
    0,
};

const char *Wave12AmmoPacks_4[1] = {
    0,
};

const char *Wave16Sounds_4[1] = {
    "MWM26_001",
};

const char *Wave16HealthPacks_4[1] = {
    0,
};

const char *Wave16AmmoPacks_4[1] = {
    0,
};

const char *Wave20Sounds_4[1] = {
    "MWM26_001",
};

const char *Wave20HealthPacks_4[1] = {
    0,
};

const char *Wave20AmmoPacks_4[1] = {
    0,
};

const float Wave5WaitTimes_2[6] = {
    1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f,
};

const Enemy Wave5Enemies_2[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave6WaitTimes_2[6] = {
    2.0f, 1.0f, 1.0f, 2.0f, 1.0f, 3.0f,
};

const Enemy Wave6Enemies_2[38] = {
    {  0,  0 }, {  3,  0 }, {  2,  1 }, {  3,  2 }, {  2,  3 },
    {  3,  4 }, {  2,  5 }, {  3,  6 }, {  2,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  2, 16 },
    {  3, 17 }, {  2, 18 }, {  3, 19 }, {  2, 20 }, {  3, 21 },
    {  2, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave7WaitTimes_2[6] = {
    2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f,
};

const Enemy Wave7Enemies_2[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave9WaitTimes_2[6] = {
    2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 2.0f,
};

const Enemy Wave9Enemies_2[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave10WaitTimes_2[6] = {
    2.0f, 1.0f, 2.0f, 2.0f, 1.0f, 3.0f,
};

const Enemy Wave10Enemies_2[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave11WaitTimes_2[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave11Enemies_2[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave13WaitTimes_2[6] = {
    2.0f, 4.0f, 4.0f, 1.0f, 1.0f, 4.0f,
};

const Enemy Wave13Enemies_2[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave14WaitTimes_2[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave14Enemies_2[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave15WaitTimes_2[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave15Enemies_2[38] = {
    {  0,  0 }, {  2,  0 }, {  4,  1 }, {  2,  2 }, {  4,  3 },
    {  2,  4 }, {  4,  5 }, {  2,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  3,  9 }, {  4, 10 }, {  3, 11 }, {  4, 12 },
    {  3, 13 }, {  4, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  2, 20 }, {  4, 21 },
    {  2, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  4, 26 }, {  4, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave17WaitTimes_2[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave17Enemies_2[38] = {
    {  0,  0 }, {  2,  0 }, {  4,  1 }, {  2,  2 }, {  4,  3 },
    {  2,  4 }, {  4,  5 }, {  2,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  3,  9 }, {  4, 10 }, {  3, 11 }, {  4, 12 },
    {  3, 13 }, {  4, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  2, 20 }, {  4, 21 },
    {  2, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  4, 26 }, {  4, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave18WaitTimes_2[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave18Enemies_2[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave19WaitTimes_2[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave19Enemies_2[34] = {
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  5,  5 }, {  5, 13 }, {  5, 21 }, {  5, 29 }, {  0,  0 },
    {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 }, {  5,  5 },
    {  5, 13 }, {  5, 21 }, {  5, 29 }, {  5,  7 }, {  5, 15 },
    {  5, 23 }, {  5, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave21WaitTimes_2[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.0f,
};

const Enemy Wave21Enemies_2[38] = {
    {  0,  0 }, {  3,  0 }, {  5,  1 }, {  5,  2 }, {  3,  3 },
    {  3,  4 }, {  5,  5 }, {  5,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  5, 10 }, {  5, 11 }, {  3, 12 },
    {  3, 13 }, {  5, 14 }, {  5, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  5, 19 }, {  5, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  5, 27 }, {  5, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave22WaitTimes_2[6] = {
    2.0f, 1.0f, 1.0f, 4.0f, 1.0f, 3.0f,
};

const Enemy Wave22Enemies_2[38] = {
    {  0,  0 }, {  4,  0 }, {  5,  1 }, {  5,  2 }, {  4,  3 },
    {  4,  4 }, {  5,  5 }, {  5,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  5, 10 }, {  5, 11 }, {  4, 12 },
    {  4, 13 }, {  5, 14 }, {  5, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  5, 19 }, {  5, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  5, 27 }, {  5, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave23WaitTimes_2[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.5f,
};

const Enemy Wave23Enemies_2[38] = {
    {  0,  0 }, {  4,  0 }, {  5,  1 }, {  5,  2 }, {  4,  3 },
    {  4,  4 }, {  5,  5 }, {  5,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  5, 10 }, {  5, 11 }, {  4, 12 },
    {  4, 13 }, {  5, 14 }, {  5, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  5, 19 }, {  5, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  5, 27 }, {  5, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave24WaitTimes_2[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const char *Wave24HealthPacks_2[8] = {
    0,
    0,
    "powerup11",
    0,
    "powerup12",
    0,
    0,
    0,
};

const char *Wave24AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave25Sounds_2[5] = {
    "MWM26_093",
    0,
    0,
    0,
    0,
};

const char *Wave25HealthPacks_2[9] = {
    0,
    "powerup1",
    "powerup2",
    "powerup3",
    0,
    0,
    0,
    0,
    0,
};

const char *Wave25AmmoPacks_2[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave1Sounds_3[5] = {
    "MWM26_074",
    0,
    0,
    0,
    0,
};

const char *Wave1HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave1AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave2Sounds_3[5] = {
    "MWM26_075",
    0,
    0,
    0,
    0,
};

const char *Wave2HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave2AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave3Sounds_3[5] = {
    "MWM26_076",
    0,
    0,
    0,
    0,
};

const char *Wave3HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave3AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave5Sounds_3[5] = {
    "MWM26_077",
    0,
    0,
    0,
    0,
};

const char *Wave5HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave5AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave6Sounds_3[5] = {
    "MWM26_078",
    0,
    0,
    0,
    0,
};

const char *Wave6HealthPacks_3[7] = {
    0,
    0,
    "powerup1",
    0,
    0,
    0,
    0,
};

const char *Wave6AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave7Sounds_3[5] = {
    "MWM26_079",
    0,
    0,
    0,
    0,
};

const char *Wave7HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave7AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave9Sounds_3[5] = {
    "MWM26_080",
    0,
    0,
    0,
    0,
};

const char *Wave9HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave9AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave10Sounds_3[5] = {
    "MWM26_081",
    0,
    0,
    0,
    0,
};

const char *Wave10HealthPacks_3[7] = {
    0,
    0,
    "powerup4",
    0,
    0,
    0,
    0,
};

const char *Wave10AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave11Sounds_3[5] = {
    "MWM26_082",
    0,
    0,
    0,
    0,
};

const char *Wave11HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave11AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave13Sounds_3[5] = {
    "MWM26_083",
    0,
    0,
    0,
    0,
};

const char *Wave13HealthPacks_3[7] = {
    0,
    0,
    "powerup7",
    0,
    0,
    0,
    0,
};

const char *Wave13AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave14Sounds_3[5] = {
    "MWM26_084",
    0,
    0,
    0,
    0,
};

const char *Wave14HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave14AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave15Sounds_3[5] = {
    "MWM26_085",
    0,
    0,
    0,
    0,
};

const char *Wave15HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave15AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave17Sounds_3[5] = {
    "MWM26_086",
    0,
    0,
    0,
    0,
};

const char *Wave17HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave17AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave18Sounds_3[5] = {
    "MWM26_087",
    0,
    0,
    0,
    0,
};

const char *Wave18HealthPacks_3[7] = {
    0,
    0,
    "powerup10",
    0,
    0,
    0,
    0,
};

const char *Wave18AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave19Sounds_3[5] = {
    "MWM26_088",
    0,
    0,
    0,
    0,
};

const char *Wave19HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave19AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave21Sounds_3[5] = {
    "MWM26_089",
    0,
    0,
    0,
    0,
};

const char *Wave21HealthPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave21AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave22Sounds_3[5] = {
    "MWM26_090",
    0,
    0,
    0,
    0,
};

const char *Wave22HealthPacks_3[7] = {
    0,
    0,
    "powerup1",
    0,
    0,
    0,
    0,
};

const char *Wave22AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave23Sounds_3[5] = {
    "MWM26_091",
    0,
    0,
    0,
    0,
};

const char *Wave23HealthPacks_3[7] = {
    0,
    0,
    "powerup4",
    0,
    0,
    0,
    0,
};

const char *Wave23AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave24Sounds_3[5] = {
    "MWM26_092",
    0,
    0,
    0,
    0,
};

const char *Wave24HealthPacks_3[7] = {
    0,
    0,
    0,
    "powerup9",
    0,
    0,
    0,
};

const char *Wave24AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave25Sounds_3[5] = {
    "MWM26_093",
    0,
    0,
    0,
    0,
};

const char *Wave25HealthPacks_3[10] = {
    0,
    "powerup2",
    "powerup4",
    0,
    "powerup11",
    "powerup12",
    0,
    0,
    0,
    0,
};

const char *Wave25AmmoPacks_3[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave1Sounds_4[5] = {
    "MWM26_074",
    0,
    0,
    0,
    0,
};

const char *Wave1HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave1AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave2Sounds_4[5] = {
    "MWM26_075",
    0,
    0,
    0,
    0,
};

const char *Wave2HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave2AmmoPacks_4[7] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave3Sounds_4[5] = {
    "MWM26_076",
    0,
    0,
    0,
    0,
};

const char *Wave3HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave3AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave5Sounds_4[5] = {
    "MWM26_077",
    0,
    0,
    0,
    0,
};

const char *Wave5HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave5AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave6Sounds_4[5] = {
    "MWM26_078",
    0,
    0,
    0,
    0,
};

const char *Wave6HealthPacks_4[7] = {
    0,
    0,
    "powerup1",
    0,
    0,
    0,
    0,
};

const char *Wave6AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave7Sounds_4[5] = {
    "MWM26_079",
    0,
    0,
    0,
    0,
};

const char *Wave7HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave7AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave9Sounds_4[5] = {
    "MWM26_080",
    0,
    0,
    0,
    0,
};

const char *Wave9HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave9AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave10Sounds_4[5] = {
    "MWM26_081",
    0,
    0,
    0,
    0,
};

const char *Wave10HealthPacks_4[7] = {
    0,
    0,
    "powerup4",
    0,
    0,
    0,
    0,
};

const char *Wave10AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave11Sounds_4[5] = {
    "MWM26_082",
    0,
    0,
    0,
    0,
};

const char *Wave11HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave11AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave13Sounds_4[5] = {
    "MWM26_083",
    0,
    0,
    0,
    0,
};

const char *Wave13HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave13AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave14Sounds_4[6] = {
    "MWM26_084",
    0,
    0,
    0,
    0,
    0,
};

const char *Wave14HealthPacks_4[8] = {
    0,
    0,
    "powerup6",
    0,
    0,
    0,
    0,
    0,
};

const char *Wave14AmmoPacks_4[7] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave15Sounds_4[5] = {
    "MWM26_085",
    0,
    0,
    0,
    0,
};

const char *Wave15HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave15AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave17Sounds_4[5] = {
    "MWM26_086",
    0,
    0,
    0,
    0,
};

const char *Wave17HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave17AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave18Sounds_4[5] = {
    "MWM26_087",
    0,
    0,
    0,
    0,
};

const char *Wave18HealthPacks_4[7] = {
    0,
    0,
    "powerup9",
    0,
    0,
    0,
    0,
};

const char *Wave18AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave19Sounds_4[5] = {
    "MWM26_088",
    0,
    0,
    0,
    0,
};

const char *Wave19HealthPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave19AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave21Sounds_4[5] = {
    "MWM26_089",
    0,
    0,
    0,
    0,
};

const char *Wave21HealthPacks_4[7] = {
    0,
    0,
    "powerup11",
    0,
    0,
    0,
    0,
};

const char *Wave21AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave22Sounds_4[5] = {
    "MWM26_090",
    0,
    0,
    0,
    0,
};

const char *Wave22HealthPacks_4[7] = {
    0,
    0,
    "powerup2",
    0,
    0,
    0,
    0,
};

const char *Wave22AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave23Sounds_4[5] = {
    "MWM26_091",
    0,
    0,
    0,
    0,
};

const char *Wave23HealthPacks_4[7] = {
    0,
    0,
    "powerup4",
    0,
    0,
    0,
    0,
};

const char *Wave23AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave24Sounds_4[5] = {
    "MWM26_092",
    0,
    0,
    0,
    0,
};

const char *Wave24HealthPacks_4[7] = {
    0,
    0,
    "powerup5",
    0,
    0,
    0,
    0,
};

const char *Wave24AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const char *Wave25Sounds_4[5] = {
    "MWM26_093",
    0,
    0,
    0,
    0,
};

const char *Wave25HealthPacks_4[7] = {
    0,
    "powerup8",
    0,
    0,
    0,
    0,
    0,
};

const char *Wave25AmmoPacks_4[6] = {
    0,
    0,
    0,
    0,
    0,
    0,
};

const Enemy Wave24Enemies_2[38] = {
    {  0,  0 }, {  4,  0 }, {  5,  1 }, {  5,  2 }, {  4,  3 },
    {  4,  4 }, {  5,  5 }, {  5,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  5, 10 }, {  5, 11 }, {  4, 12 },
    {  4, 13 }, {  5, 14 }, {  5, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  5, 19 }, {  5, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  5, 27 }, {  5, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave25WaitTimes_2[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 1.0f,
};

const Enemy Wave25Enemies_2[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const unsigned char WaveIncrements_3[26] = {
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 0,
};

const float Wave1WaitTimes_3[6] = {
    1.0f, 1.0f, 2.0f, 1.0f, 2.0f, 1.0f,
};

const Enemy Wave1Enemies_3[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave2WaitTimes_3[6] = {
    1.0f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f,
};

const Enemy Wave2Enemies_3[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave3WaitTimes_3[6] = {
    2.0f, 1.0f, 1.5f, 1.0f, 1.0f, 3.0f,
};

const Enemy Wave3Enemies_3[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave5WaitTimes_3[6] = {
    1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f,
};

const Enemy Wave5Enemies_3[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave6WaitTimes_3[6] = {
    1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f,
};

const Enemy Wave6Enemies_3[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave7WaitTimes_3[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
};

const Enemy Wave7Enemies_3[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave9WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 4.0f,
};

const Enemy Wave9Enemies_3[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave10WaitTimes_3[6] = {
    2.0f, 1.0f, 2.0f, 2.0f, 1.0f, 3.0f,
};

const Enemy Wave10Enemies_3[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave11WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave11Enemies_3[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  3 }, {  4,  4 },
    {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 }, {  4,  8 },
    {  4,  9 }, {  4, 11 }, {  4, 12 }, {  4, 13 }, {  4, 14 },
    {  4, 15 }, {  0,  0 }, {  4, 16 }, {  4, 17 }, {  4, 19 },
    {  4, 20 }, {  4, 21 }, {  4, 22 }, {  4, 23 }, {  0,  0 },
    {  4, 24 }, {  4, 25 }, {  4, 27 }, {  4, 28 }, {  4, 29 },
    {  4, 30 }, {  4, 31 }, {  5,  2 }, {  5, 10 }, {  5, 18 },
    {  5, 26 }, {  0,  0 }, {  0,  0 },
};

const float Wave13WaitTimes_3[6] = {
    1.0f, 1.0f, 2.4f, 1.0f, 1.0f, 2.5f,
};

const Enemy Wave13Enemies_3[34] = {
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 },
    {  5,  5 }, {  5, 13 }, {  5, 21 }, {  5, 29 }, {  0,  0 },
    {  5,  2 }, {  5, 10 }, {  5, 18 }, {  5, 26 }, {  5,  5 },
    {  5, 13 }, {  5, 21 }, {  5, 29 }, {  5,  7 }, {  5, 15 },
    {  5, 23 }, {  5, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave14WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave14Enemies_3[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  5,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  5, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  5, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  5, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave15WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave15Enemies_3[38] = {
    {  0,  0 }, {  5,  0 }, {  3,  1 }, {  3,  2 }, {  5,  3 },
    {  3,  4 }, {  5,  5 }, {  3,  6 }, {  5,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  5, 25 },
    {  3, 26 }, {  5, 27 }, {  3, 28 }, {  5, 29 }, {  3, 30 },
    {  5, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave17WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave17Enemies_3[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave18WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave18Enemies_3[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave19WaitTimes_3[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave19Enemies_3[38] = {
    {  0,  0 }, {  2,  0 }, {  4,  1 }, {  2,  2 }, {  4,  3 },
    {  2,  4 }, {  4,  5 }, {  2,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  3,  9 }, {  4, 10 }, {  3, 11 }, {  4, 12 },
    {  3, 13 }, {  4, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  2, 20 }, {  4, 21 },
    {  2, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  4, 26 }, {  4, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave21WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave21Enemies_3[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave22WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 4.0f, 1.0f, 3.0f,
};

const Enemy Wave22Enemies_3[38] = {
    {  0,  0 }, {  2,  0 }, {  4,  1 }, {  2,  2 }, {  4,  3 },
    {  2,  4 }, {  4,  5 }, {  2,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  3,  9 }, {  4, 10 }, {  3, 11 }, {  4, 12 },
    {  3, 13 }, {  4, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  2, 20 }, {  4, 21 },
    {  2, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  4, 26 }, {  4, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave23WaitTimes_3[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.5f,
};

const Enemy Wave23Enemies_3[38] = {
    {  0,  0 }, {  2,  0 }, {  4,  1 }, {  2,  2 }, {  4,  3 },
    {  2,  4 }, {  4,  5 }, {  2,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  3,  9 }, {  4, 10 }, {  3, 11 }, {  4, 12 },
    {  3, 13 }, {  4, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  2, 20 }, {  4, 21 },
    {  2, 22 }, {  4, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  4, 26 }, {  4, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave24WaitTimes_3[6] = {
    2.0f, 1.0f, 1.0f, 0.75f, 1.0f, 3.0f,
};

const Enemy Wave24Enemies_3[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave25WaitTimes_3[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 1.0f,
};

const Enemy Wave25Enemies_3[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const unsigned char WaveIncrements_4[26] = {
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 0,
};

const float Wave1WaitTimes_4[6] = {
    2.0f, 1.0f, 2.0f, 1.0f, 2.0f, 1.0f,
};

const Enemy Wave1Enemies_4[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave2WaitTimes_4[6] = {
    1.0f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f,
};

const Enemy Wave2Enemies_4[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave3WaitTimes_4[6] = {
    1.5f, 1.0f, 1.5f, 1.0f, 1.0f, 1.0f,
};

const Enemy Wave3Enemies_4[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  2, 16 },
    {  2, 17 }, {  2, 18 }, {  2, 19 }, {  2, 20 }, {  2, 21 },
    {  2, 22 }, {  2, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave5WaitTimes_4[6] = {
    1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 2.5f,
};

const Enemy Wave5Enemies_4[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  3,  8 }, {  2,  9 }, {  3, 10 }, {  2, 11 }, {  3, 12 },
    {  2, 13 }, {  3, 14 }, {  2, 15 }, {  0,  0 }, {  3, 16 },
    {  2, 17 }, {  3, 18 }, {  2, 19 }, {  3, 20 }, {  2, 21 },
    {  3, 22 }, {  2, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave6WaitTimes_4[6] = {
    1.5f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f,
};

const Enemy Wave6Enemies_4[38] = {
    {  0,  0 }, {  2,  0 }, {  2,  1 }, {  2,  2 }, {  2,  3 },
    {  2,  4 }, {  2,  5 }, {  2,  6 }, {  2,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  2, 24 }, {  2, 25 },
    {  2, 26 }, {  2, 27 }, {  2, 28 }, {  2, 29 }, {  2, 30 },
    {  2, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave7WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
};

const Enemy Wave7Enemies_4[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  3, 27 }, {  3, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave9WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.5f,
};

const Enemy Wave9Enemies_4[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  4, 10 }, {  3, 11 }, {  3, 12 },
    {  4, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  3, 20 }, {  4, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  4, 25 },
    {  3, 26 }, {  3, 27 }, {  4, 28 }, {  3, 29 }, {  4, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave10WaitTimes_4[6] = {
    1.0f, 1.0f, 2.0f, 2.0f, 1.0f, 2.0f,
};

const Enemy Wave10Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  2,  8 }, {  2,  9 }, {  2, 10 }, {  2, 11 }, {  2, 12 },
    {  2, 13 }, {  2, 14 }, {  2, 15 }, {  0,  0 }, {  3, 16 },
    {  3, 17 }, {  3, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave11WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.0f,
};

const Enemy Wave11Enemies_4[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  3,  3 },
    {  3,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  3, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave13WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.5f,
};

const Enemy Wave13Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave14WaitTimes_4[7] = {
    1.0f, 1.0f, 1.0f, 0.75f, 2.0f, 1.0f, 2.5f,
};

const Enemy Wave14Enemies_4[43] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  5,  2 }, {  5, 10 }, {  5, 18 },
    {  5, 26 }, {  0,  0 }, {  0,  0 },
};

const float Wave15WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave15Enemies_4[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  3,  2 }, {  5,  3 },
    {  5,  4 }, {  3,  5 }, {  3,  6 }, {  3,  7 }, {  0,  0 },
    {  3,  8 }, {  3,  9 }, {  3, 10 }, {  3, 11 }, {  3, 12 },
    {  5, 13 }, {  5, 14 }, {  3, 15 }, {  0,  0 }, {  3, 16 },
    {  5, 17 }, {  5, 18 }, {  3, 19 }, {  3, 20 }, {  3, 21 },
    {  3, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  3, 25 },
    {  3, 26 }, {  5, 27 }, {  5, 28 }, {  3, 29 }, {  3, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave17WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave17Enemies_4[38] = {
    {  0,  0 }, {  3,  0 }, {  3,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  5,  6 }, {  5,  7 }, {  0,  0 },
    {  5,  8 }, {  5,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  3, 14 }, {  3, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  3, 18 }, {  4, 19 }, {  4, 20 }, {  5, 21 },
    {  5, 22 }, {  3, 23 }, {  0,  0 }, {  3, 24 }, {  4, 25 },
    {  4, 26 }, {  5, 27 }, {  5, 28 }, {  4, 29 }, {  4, 30 },
    {  3, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave18WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave18Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  5,  1 }, {  5,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  5,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  5, 10 }, {  4, 11 }, {  4, 12 },
    {  5, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  5, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  5, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave19WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave19Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  5,  1 }, {  5,  2 }, {  4,  3 },
    {  4,  4 }, {  5,  5 }, {  5,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  5, 10 }, {  5, 11 }, {  4, 12 },
    {  4, 13 }, {  5, 14 }, {  5, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  5, 19 }, {  5, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  5, 27 }, {  5, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave21WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave21Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  5, 14 }, {  5, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  5, 19 }, {  5, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave22WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.5f,
};

const Enemy Wave22Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave23WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.5f,
};

const Enemy Wave23Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave24WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave24Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

const float Wave25WaitTimes_4[6] = {
    1.0f, 1.0f, 1.0f, 0.75f, 1.0f, 2.5f,
};

const Enemy Wave25Enemies_4[38] = {
    {  0,  0 }, {  4,  0 }, {  4,  1 }, {  4,  2 }, {  4,  3 },
    {  4,  4 }, {  4,  5 }, {  4,  6 }, {  4,  7 }, {  0,  0 },
    {  4,  8 }, {  4,  9 }, {  4, 10 }, {  4, 11 }, {  4, 12 },
    {  4, 13 }, {  4, 14 }, {  4, 15 }, {  0,  0 }, {  4, 16 },
    {  4, 17 }, {  4, 18 }, {  4, 19 }, {  4, 20 }, {  4, 21 },
    {  4, 22 }, {  4, 23 }, {  0,  0 }, {  4, 24 }, {  4, 25 },
    {  4, 26 }, {  4, 27 }, {  4, 28 }, {  4, 29 }, {  4, 30 },
    {  4, 31 }, {  0,  0 }, {  0,  0 },
};

// literal @568 created here: "Multi17Script"

// literal @571 created here: "SPMission"

// literal @738 created here: "EP2_V1_T12_01"

// literal @739 created here: "AmbGeon_plains01"

// literal @740 created here: "AmbGeon_plains_stinger01"

// literal @743 created here: "flythrough"

// literal @744 created here: "multiplayer.types.academy.rule01"

// literal @745 created here: "multiplayer.types.academy.rule02"

// literal @746 created here: "multiplayer.types.academy.rule03"

// literal @750 created here: "rep_inf_anakin_multi"

// literal @753 created here: "rep_inf_mace_multi"

// literal @755 created here: "rep_inf_obiwan_multi"

// literal @758 created here: "rep_inf_jedi_multi"

// literal @760 created here: "rep_inf_amidala"

// literal @761 created here: "amidalapath"

// literal @766 created here: "rep_inf_clone"

// literal @767 created here: "hp_spawnpoint_1"

// literal @769 created here: "clonepath"

// literal @773 created here: "gunshipdelete"

// literal @775 created here: "rep_fly_gunship"

// literal @776 created here: "gunshipspawn"

// literal @778 created here: "gunshipland"

}

using namespace multi17Names;


// The RTTI class names -- the same stand-in probes/src/Multi12Script.cpp documents, for the
// same reason: CodeWarrior emits RTTI at end-of-TU, but the shipped pool has them here,
// after the tuning tables and ahead of Execute's literals.
//
// Nothing else needs listing. Execute creates the other twenty literals itself and, written
// faithfully, creates them in exactly the shipped order -- which is a useful independent
// check on the reconstruction, since a misplaced block would have moved them.
static const char *const kClassName = "Multi17Script";
static const char *const kBaseName  = "SPMission";

// ---------------------------------------------------------------------------------------
// The classes. DLLBase declares no data, so its vptr sits at offset 0; SPMission's four
// (base, count) pairs occupy +0x04..+0x23 and the derived members start at +0x24. See
// analysis/mission_script_abi.md.
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

// Twelve bytes, constructed out of line, with an `operator float()` that reads the
// elapsed time -- all three pinned by Multi17BuildMission and Execute.
class Timer
{
public:
    Timer();
    operator float();

    int mState[3];
};

void BeginTimer(Timer &t);

// One row of the wave schedule: 28 bytes, indexed by `mulli rN, wave, 0x1c`.
struct Wave
{
    int                  count;              // +0x00  steps in this wave
    const Enemy         *enemies;            // +0x04  NUL-group-separated
    const float         *waitTimes;          // +0x08
    const char         **sounds;             // +0x0c
    const bool          *dontWaitForDead;    // +0x10
    const char         **healthPacks;        // +0x14
    const char         **ammoPacks;          // +0x18
};

class Multi17Script : public SPMission
{
public:
    virtual ~Multi17Script();

    Multi17Script()
    {
        mBoolCount = 8;    mBools  = &mDone;
        mCountB    = 3;    mIntsB  = &mUnusedB;
        mCountC    = 57;   mIntsC  = mHandles;
        mCountD    = 5;    mBlockD = &mStep;
    }

    virtual void Setup();
    virtual void Execute();

    bool AllDead()
    {
        int i;
        for (i = 0; i < 50; i++) {
            if (mHandles[i] && !MissionUtility::IsAlive(mHandles[i])) {
                mHandles[i] = 0;
            }
        }
        bool allDead = true;
        for (i = 0; i < 50; i++) {
            allDead &= (mHandles[i] == 0);
        }
        return allDead;
    }

    void SpawnEnemies(int wave, int step);

    // Inline members taking `(int wave, int step)`, the same shape as SpawnEnemies. Written
    // out in Execute instead, the scaled wave index and the loop counter swap registers --
    // see probes/src/Multi5Script.cpp, where that cost 49 words.

    void GiveAmmo(int wave, int step)
    {
        if (wave >= 0 && wave < 26 && step >= 0 && step < mWaves[wave].count) {
            int i = 0;
            int n = i;
            while (n < step) {
                if (!mWaves[wave].ammoPacks[i]) {
                    n++;
                }
                i++;
            }
            while (mWaves[wave].ammoPacks[i]) {
                MissionUtility::AddAmmoBox(mWaves[wave].ammoPacks[i], 0, -1.0f);
                i++;
            }
        }
    }

    void GiveHealth(int wave, int step)
    {
        if (wave >= 0 && wave < 26 && step >= 0 && step < mWaves[wave].count) {
            int i = 0;
            int n = i;
            while (n < step) {
                if (!mWaves[wave].healthPacks[i]) {
                    n++;
                }
                i++;
            }
            while (mWaves[wave].healthPacks[i]) {
                MissionUtility::AddHealthBox(mWaves[wave].healthPacks[i], 0, -1.0f);
                i++;
            }
        }
    }
    void OnePlayerInit();
    void TwoPlayerInit();
    void ThreePlayerInit();
    void FourPlayerInit();

    Wave   mWaves[26];                       // +0x024  memset to zero in Setup
    const unsigned char *mIncrements;        // +0x2fc
    bool   mPad300;                          // +0x300
    bool   mDone;                            // +0x301  the one-shot latch
    bool   mWaiting;                         // +0x302
    bool   mDontWaitForDead;                 // +0x303
    bool   mStarted;                         // +0x304  the escort timer is running
    bool   mAmidalaSpawned;                  // +0x305
    bool   mAmidalaWalking;                  // +0x306
    bool   mCloneTimerRunning;               // +0x307
    bool   mJediSpawned;                     // +0x308
    char   mPad309[7];                       // +0x309
    int    mUnusedB;                         // +0x310  registered, never read
    float  mSpawnDelay;                      // +0x314
    float  mNextSpawn;                       // +0x318
    int    mPad31c[2];                       // +0x31c
    int    mHandles[51];                     // +0x324  Setup clears the first fifty
    int    mAmidala;                         // +0x3f0  the tail of the same registered
    int    mClone1;                          // +0x3f4  block -- 57 ints in all, which is
    int    mClone2;                          // +0x3f8  the count BuildMission passes
    int    mClone3;                          // +0x3fc
    int    mGunship;                         // +0x400
    int    mCinematic;                       // +0x404
    int    mPad408[2];                       // +0x408
    int    mStep;                            // +0x410
    int    mWave;                            // +0x414
    int    mAt418;                           // +0x418
    int    mAt41c;                           // +0x41c
    int    mNumPlayers;                      // +0x420
    int    mPad424;                          // +0x424
    Timer  mTimerA;                          // +0x428  constructed, never read
    Timer  mEscortTimer;                     // +0x434
    Timer  mCloneTimer;                      // +0x440
    Timer  mTimerD;                          // +0x44c            -> sizeof == 0x458
};

// ---------------------------------------------------------------------------------------

Multi17Script::~Multi17Script()
{
}

void Multi17Script::Execute()
{
    if (!mDone) {
        MissionUtility::SetMusicLooping(true);
        MissionUtility::PlayMusic("EP2_V1_T12_01", true);
        MissionUtility::StartAmbiences("AmbGeon_plains01", "AmbGeon_plains_stinger01",
                                       10.0f, 30.0f);
        mCinematic = MissionUtility::RunCin("flythrough", true, true);

        MissionUtility::AddObjective("multiplayer.types.academy.rule01");
        MissionUtility::AddObjective("multiplayer.types.academy.rule02");
        MissionUtility::AddObjective("multiplayer.types.academy.rule03");

        MissionUtility::SetEnemiesOneWay(1, 8);
        MissionUtility::SetEnemiesOneWay(6, 5);
        MissionUtility::SetAlliance(1, 6);
        MissionUtility::SetAlliance(2, 6);
        MissionUtility::SetAlliance(3, 6);
        MissionUtility::SetAlliance(4, 6);
        MissionUtility::SetMapZoom(300.0f, 10000000.0f);
        mDone = true;

        mNumPlayers = MissionUtility::MPGetNumPlayers();
        switch (mNumPlayers) {
        case 1:  OnePlayerInit();   break;
        case 2:  TwoPlayerInit();   break;
        case 3:  ThreePlayerInit(); break;
        case 4:  FourPlayerInit();  break;
        }
    }

    if (!MissionUtility::MPHasGameStarted()) {
        return;
    }

    // ---- the escort: two Jedi at a minute in, Amidala at a minute after that ----------
    //
    // The timer thresholds are `double` literals, not `float`: the shipped code loads them
    // with `lfd` and compares the Timer's `operator float()` result against them promoted.
    if (!mStarted) {
        BeginTimer(mEscortTimer);
        mStarted = true;
    } else {
        if (!mJediSpawned) {
            if (mEscortTimer > 3.0) {
                // Whichever Jedi the player picked, the other two turn up as allies.
                int odf = MissionUtility::GetODF(MissionUtility::GetPlayerHandle(0));
                if (mNumPlayers == 1) {
                    if (odf == MissionUtility::GetCRC("rep_inf_anakin_multi")) {
                        MissionUtility::CreateObject("rep_inf_mace_multi", "powerup1", 0,
                                                     "mace", 6, -1);
                        MissionUtility::CreateObject("rep_inf_obiwan_multi", "powerup2", 0,
                                                     "obiwan", 6, -1);
                    }
                    if (odf == MissionUtility::GetCRC("rep_inf_obiwan_multi")) {
                        MissionUtility::CreateObject("rep_inf_mace_multi", "powerup1", 0,
                                                     "mace", 6, -1);
                        MissionUtility::CreateObject("rep_inf_anakin_multi", "powerup2", 0,
                                                     "anakin", 6, -1);
                    }
                    if (odf == MissionUtility::GetCRC("rep_inf_mace_multi")) {
                        MissionUtility::CreateObject("rep_inf_obiwan_multi", "powerup1", 0,
                                                     "obiwan", 6, -1);
                        MissionUtility::CreateObject("rep_inf_anakin_multi", "powerup2", 0,
                                                     "anakin", 6, -1);
                    }
                    if (odf == MissionUtility::GetCRC("rep_inf_jedi_multi")) {
                        MissionUtility::CreateObject("rep_inf_obiwan_multi", "powerup1", 0,
                                                     "obiwan", 6, -1);
                        MissionUtility::CreateObject("rep_inf_anakin_multi", "powerup2", 0,
                                                     "anakin", 6, -1);
                        MissionUtility::CreateObject("rep_inf_mace_multi", "powerup3", 0,
                                                     "mace", 6, -1);
                    }
                }
                mJediSpawned = true;
            }
        }

        if (!mAmidalaSpawned) {
            if (mEscortTimer > 60.0) {
                mAmidala = MissionUtility::CreateObject("rep_inf_amidala", "amidalapath", 0,
                                                        "amidala", 6, -1);
                MissionUtility::Goto(mAmidala, "amidalapath", 1);
                mAmidalaSpawned = true;
                mAmidalaWalking = true;
            }
        }

        if (mAmidalaWalking) {
            if (mEscortTimer > 120.0) {
                MissionUtility::Goto(mAmidala, "amidalapath", 1);
                MissionUtility::SetQueueFlag(true);
                MissionUtility::Goto(mAmidala, "amidalapath", 0);
                MissionUtility::SetQueueFlag(false);
                mAmidalaWalking = false;
            }
        }

        if (MissionUtility::IsAlive(mAmidala)) {
            if (mEscortTimer > 130.0) {
                if (MissionUtility::GetDistance(mAmidala, "amidalapath", 0) < 30.0f) {
                    MissionUtility::RemoveObject(mAmidala);
                }
            }
        }
    }

    // ---- the gunship: lands, unloads three clones a second apart, takes off -----------
    if (MissionUtility::IsAlive(mGunship) && MissionUtility::IsLanded(mGunship)) {
        if (!mCloneTimerRunning) {
            BeginTimer(mCloneTimer);
            mCloneTimerRunning = true;
        }
        if (!MissionUtility::IsAlive(mClone1)) {
            if (mCloneTimer > 1.0) {
                mClone1 = MissionUtility::CreateObject("rep_inf_clone", mGunship,
                                                       "hp_spawnpoint_1", "clone1",
                                                       6, -1, -1);
                MissionUtility::Goto(mClone1, "clonepath", true);
            }
        }
        if (!MissionUtility::IsAlive(mClone2)) {
            if (mCloneTimer > 2.0) {
                mClone2 = MissionUtility::CreateObject("rep_inf_clone", mGunship,
                                                       "hp_spawnpoint_1", "clone2",
                                                       6, -1, -1);
                MissionUtility::Goto(mClone2, "clonepath", true);
            }
        }
        if (!MissionUtility::IsAlive(mClone3)) {
            if (mCloneTimer > 3.0) {
                mClone3 = MissionUtility::CreateObject("rep_inf_clone", mGunship,
                                                       "hp_spawnpoint_1", "clone3",
                                                       6, -1, -1);
                MissionUtility::Goto(mClone3, "clonepath", true);
            }
        }
        if (MissionUtility::IsAlive(mClone1) && MissionUtility::IsAlive(mClone2)
            && MissionUtility::IsAlive(mClone3)) {
            MissionUtility::TakeOff(mGunship);
            MissionUtility::SetQueueFlag(true);
            MissionUtility::Goto(mGunship, "gunshipdelete", true);
            MissionUtility::SetQueueFlag(false);
        }
    }

    if (MissionUtility::IsAlive(mGunship)) {
        if (mCloneTimer > 40.0) {
            MissionUtility::RemoveObject(mGunship);
        }
    }

    // ---- the wave pump, the same shape as Multi5Script --------------------------------
    if (mWaiting) {
        if (mNextSpawn < MissionUtility::GetTime()) {
            // The gunship arrives on one specific step of wave 7.
            if (mWave == 7 && mStep == 1) {
                mGunship = MissionUtility::CreateObject("rep_fly_gunship", "gunshipspawn", 0,
                                                        "gunship", 0, -1);
                MissionUtility::Land(mGunship, "gunshipland", 0, 80.0f);
                MissionUtility::SetVelocNeutralFly(mGunship, 10.0f);
                MissionUtility::SetVelocMaximumFly(mGunship, 10.0f);
                MissionUtility::SetVelocMinimumFly(mGunship, 10.0f);
                MissionUtility::SetTakeoffAltitude(mGunship, 1000.0f);
                MissionUtility::SetCurHealth(mGunship, 999999.0f);
            }

            mDontWaitForDead = false;
            SpawnEnemies(mWave, mStep);
            GiveAmmo(mWave, mStep);
            GiveHealth(mWave, mStep);

            if (mWave >= 0 && mWave < 26 && mStep >= 0 && mStep < mWaves[mWave].count) {
                mDontWaitForDead = mWaves[mWave].dontWaitForDead[mStep];
                mSpawnDelay = mWaves[mWave].waitTimes[mStep];
                if (mWaves[mWave].sounds[mStep]
                    && MissionUtility::MPGetWave() <= 20
                    && (!MissionUtility::MPIsGladiatorRound()
                        || MissionUtility::MPGetNumLivePlayers() >= 2)) {
                    MissionUtility::StartSound(mWaves[mWave].sounds[mStep], false,
                                               1.0f, 0.0f, 0.0f, "", 0, "");
                }
            }

            if (mWave >= 0 && mWave < 26) {
                if (mStep >= mWaves[mWave].count - 1) {
                    mWave++;
                    MissionUtility::MPSetWave(mIncrements[mWave - 1]
                                              + MissionUtility::MPGetWave());
                    mStep = -1;
                }
            } else if (mWave == 26) {
                mSpawnDelay = 0.0f;
                mNextSpawn = -1.0f;
                mStep = -1;
                mWave = 0;
            }
            mStep++;
            mWaiting = false;
        }
    } else {
        if (!mDontWaitForDead) {
            if (!AllDead()) {
                return;
            }
        }
        if (!MissionUtility::MPIsGladiatorRound()) {
            mNextSpawn = mSpawnDelay + MissionUtility::GetTime();
            mWaiting = true;
        }
    }
}

void Multi17Script::SpawnEnemies(int wave, int step)
{
    if (wave >= 0 && wave < 26 && step >= 0 && step < mWaves[wave].count) {

    // Skip `step` NUL separators, then spawn everything up to the next one.
    int i = 0;
    int n = i;
    while (n < step) {
        if (!ENEMY_ODFS[mWaves[wave].enemies[i].odf]) {
            n++;
        }
        i++;
    }

    int slot = 0;
    while (ENEMY_ODFS[mWaves[wave].enemies[i].odf]) {
        Enemy e = mWaves[wave].enemies[i];
        i++;

        if (strcmp(ENEMY_ODFS[e.odf], "GLADIATOR") == 0) {
            MissionUtility::MPStartGladiatorRound();
            continue;
        }

        // Spawn point 32 is the wildcard: pick one of the thirty-two at random.
        int where = e.spawn;
        if (where == 32) {
            where = MissionUtility::GetRandomInt(0, 31);
        }

        while (mHandles[slot]) {
            slot++;
        }

        mHandles[slot] = MissionUtility::CreateObject(ENEMY_ODFS[e.odf],
                                                      SpawnPoints[where],
                                                      "enemy", 5, -1, -1);

        // Every twenty waves the droids come back tougher.
        int tier = (MissionUtility::MPGetWave() - 1) / 20;
        if (tier) {
            float health = MissionUtility::GetMaxHealth(mHandles[slot]);
            float shield = MissionUtility::GetMaxShield(mHandles[slot]);
            float scale  = 0.5f * tier + 1.0f;
            health *= scale;
            shield *= scale;
            MissionUtility::SetMaxHealth(mHandles[slot], health);
            MissionUtility::SetMaxShield(mHandles[slot], shield);
            MissionUtility::SetCurHealth(mHandles[slot], health);
            MissionUtility::SetCurShield(mHandles[slot], shield);
        }

        MissionUtility::Patrol(mHandles[slot], SpawnPoints[where], 8.0f, true);
        MissionUtility::SetQueueFlag(true);
        MissionUtility::Patrol(mHandles[slot], PatrolPaths[where], 20.0f, true);
        MissionUtility::SetQueueFlag(false);
    }

    }
}

void Multi17Script::FourPlayerInit()
{
    mIncrements = WaveIncrements_4;
    mWaves[1].count = 6;
    mWaves[1].enemies = Wave1Enemies_4;
    mWaves[1].waitTimes = Wave1WaitTimes_4;
    mWaves[1].sounds = Wave1Sounds_4;
    mWaves[1].dontWaitForDead = Wave1DontWaitForDead_4;
    mWaves[1].healthPacks = Wave1HealthPacks_4;
    mWaves[1].ammoPacks = Wave1AmmoPacks_4;
    mWaves[2].count = 6;
    mWaves[2].enemies = Wave2Enemies_4;
    mWaves[2].waitTimes = Wave2WaitTimes_4;
    mWaves[2].sounds = Wave2Sounds_4;
    mWaves[2].dontWaitForDead = Wave2DontWaitForDead_4;
    mWaves[2].healthPacks = Wave2HealthPacks_4;
    mWaves[2].ammoPacks = Wave2AmmoPacks_4;
    mWaves[3].count = 6;
    mWaves[3].enemies = Wave3Enemies_4;
    mWaves[3].waitTimes = Wave3WaitTimes_4;
    mWaves[3].sounds = Wave3Sounds_4;
    mWaves[3].dontWaitForDead = Wave3DontWaitForDead_4;
    mWaves[3].healthPacks = Wave3HealthPacks_4;
    mWaves[3].ammoPacks = Wave3AmmoPacks_4;
    mWaves[4].count = 1;
    mWaves[4].enemies = Wave4Enemies_4;
    mWaves[4].waitTimes = Wave4WaitTimes_4;
    mWaves[4].sounds = Wave4Sounds_4;
    mWaves[4].dontWaitForDead = Wave4DontWaitForDead_4;
    mWaves[4].healthPacks = Wave4HealthPacks_4;
    mWaves[4].ammoPacks = Wave4AmmoPacks_4;
    mWaves[5].count = 6;
    mWaves[5].enemies = Wave5Enemies_4;
    mWaves[5].waitTimes = Wave5WaitTimes_4;
    mWaves[5].sounds = Wave5Sounds_4;
    mWaves[5].dontWaitForDead = Wave5DontWaitForDead_4;
    mWaves[5].healthPacks = Wave5HealthPacks_4;
    mWaves[5].ammoPacks = Wave5AmmoPacks_4;
    mWaves[6].count = 6;
    mWaves[6].enemies = Wave6Enemies_4;
    mWaves[6].waitTimes = Wave6WaitTimes_4;
    mWaves[6].sounds = Wave6Sounds_4;
    mWaves[6].dontWaitForDead = Wave6DontWaitForDead_4;
    mWaves[6].healthPacks = Wave6HealthPacks_4;
    mWaves[6].ammoPacks = Wave6AmmoPacks_4;
    mWaves[7].count = 6;
    mWaves[7].enemies = Wave7Enemies_4;
    mWaves[7].waitTimes = Wave7WaitTimes_4;
    mWaves[7].sounds = Wave7Sounds_4;
    mWaves[7].dontWaitForDead = Wave7DontWaitForDead_4;
    mWaves[7].healthPacks = Wave7HealthPacks_4;
    mWaves[7].ammoPacks = Wave7AmmoPacks_4;
    mWaves[8].count = 1;
    mWaves[8].enemies = Wave8Enemies_4;
    mWaves[8].waitTimes = Wave8WaitTimes_4;
    mWaves[8].sounds = Wave8Sounds_4;
    mWaves[8].dontWaitForDead = Wave8DontWaitForDead_4;
    mWaves[8].healthPacks = Wave8HealthPacks_4;
    mWaves[8].ammoPacks = Wave8AmmoPacks_4;
    mWaves[9].count = 6;
    mWaves[9].enemies = Wave9Enemies_4;
    mWaves[9].waitTimes = Wave9WaitTimes_4;
    mWaves[9].sounds = Wave9Sounds_4;
    mWaves[9].dontWaitForDead = Wave9DontWaitForDead_4;
    mWaves[9].healthPacks = Wave9HealthPacks_4;
    mWaves[9].ammoPacks = Wave9AmmoPacks_4;
    mWaves[10].count = 6;
    mWaves[10].enemies = Wave10Enemies_4;
    mWaves[10].waitTimes = Wave10WaitTimes_4;
    mWaves[10].sounds = Wave10Sounds_4;
    mWaves[10].dontWaitForDead = Wave10DontWaitForDead_4;
    mWaves[10].healthPacks = Wave10HealthPacks_4;
    mWaves[10].ammoPacks = Wave10AmmoPacks_4;
    mWaves[11].count = 6;
    mWaves[11].enemies = Wave11Enemies_4;
    mWaves[11].waitTimes = Wave11WaitTimes_4;
    mWaves[11].sounds = Wave11Sounds_4;
    mWaves[11].dontWaitForDead = Wave11DontWaitForDead_4;
    mWaves[11].healthPacks = Wave11HealthPacks_4;
    mWaves[11].ammoPacks = Wave11AmmoPacks_4;
    mWaves[12].count = 1;
    mWaves[12].enemies = Wave12Enemies_4;
    mWaves[12].waitTimes = Wave12WaitTimes_4;
    mWaves[12].sounds = Wave12Sounds_4;
    mWaves[12].dontWaitForDead = Wave12DontWaitForDead_4;
    mWaves[12].healthPacks = Wave12HealthPacks_4;
    mWaves[12].ammoPacks = Wave12AmmoPacks_4;
    mWaves[13].count = 6;
    mWaves[13].enemies = Wave13Enemies_4;
    mWaves[13].waitTimes = Wave13WaitTimes_4;
    mWaves[13].sounds = Wave13Sounds_4;
    mWaves[13].dontWaitForDead = Wave13DontWaitForDead_4;
    mWaves[13].healthPacks = Wave13HealthPacks_4;
    mWaves[13].ammoPacks = Wave13AmmoPacks_4;
    mWaves[14].count = 7;
    mWaves[14].enemies = Wave14Enemies_4;
    mWaves[14].waitTimes = Wave14WaitTimes_4;
    mWaves[14].sounds = Wave14Sounds_4;
    mWaves[14].dontWaitForDead = Wave14DontWaitForDead_4;
    mWaves[14].healthPacks = Wave14HealthPacks_4;
    mWaves[14].ammoPacks = Wave14AmmoPacks_4;
    mWaves[15].count = 6;
    mWaves[15].enemies = Wave15Enemies_4;
    mWaves[15].waitTimes = Wave15WaitTimes_4;
    mWaves[15].sounds = Wave15Sounds_4;
    mWaves[15].dontWaitForDead = Wave15DontWaitForDead_4;
    mWaves[15].healthPacks = Wave15HealthPacks_4;
    mWaves[15].ammoPacks = Wave15AmmoPacks_4;
    mWaves[16].count = 1;
    mWaves[16].enemies = Wave16Enemies_4;
    mWaves[16].waitTimes = Wave16WaitTimes_4;
    mWaves[16].sounds = Wave16Sounds_4;
    mWaves[16].dontWaitForDead = Wave16DontWaitForDead_4;
    mWaves[16].healthPacks = Wave16HealthPacks_4;
    mWaves[16].ammoPacks = Wave16AmmoPacks_4;
    mWaves[17].count = 6;
    mWaves[17].enemies = Wave17Enemies_4;
    mWaves[17].waitTimes = Wave17WaitTimes_4;
    mWaves[17].sounds = Wave17Sounds_4;
    mWaves[17].dontWaitForDead = Wave17DontWaitForDead_4;
    mWaves[17].healthPacks = Wave17HealthPacks_4;
    mWaves[17].ammoPacks = Wave17AmmoPacks_4;
    mWaves[18].count = 6;
    mWaves[18].enemies = Wave18Enemies_4;
    mWaves[18].waitTimes = Wave18WaitTimes_4;
    mWaves[18].sounds = Wave18Sounds_4;
    mWaves[18].dontWaitForDead = Wave18DontWaitForDead_4;
    mWaves[18].healthPacks = Wave18HealthPacks_4;
    mWaves[18].ammoPacks = Wave18AmmoPacks_4;
    mWaves[19].count = 6;
    mWaves[19].enemies = Wave19Enemies_4;
    mWaves[19].waitTimes = Wave19WaitTimes_4;
    mWaves[19].sounds = Wave19Sounds_4;
    mWaves[19].dontWaitForDead = Wave19DontWaitForDead_4;
    mWaves[19].healthPacks = Wave19HealthPacks_4;
    mWaves[19].ammoPacks = Wave19AmmoPacks_4;
    mWaves[20].count = 1;
    mWaves[20].enemies = Wave20Enemies_4;
    mWaves[20].waitTimes = Wave20WaitTimes_4;
    mWaves[20].sounds = Wave20Sounds_4;
    mWaves[20].dontWaitForDead = Wave20DontWaitForDead_4;
    mWaves[20].healthPacks = Wave20HealthPacks_4;
    mWaves[20].ammoPacks = Wave20AmmoPacks_4;
    mWaves[21].count = 6;
    mWaves[21].enemies = Wave21Enemies_4;
    mWaves[21].waitTimes = Wave21WaitTimes_4;
    mWaves[21].sounds = Wave21Sounds_4;
    mWaves[21].dontWaitForDead = Wave21DontWaitForDead_4;
    mWaves[21].healthPacks = Wave21HealthPacks_4;
    mWaves[21].ammoPacks = Wave21AmmoPacks_4;
    mWaves[22].count = 6;
    mWaves[22].enemies = Wave22Enemies_4;
    mWaves[22].waitTimes = Wave22WaitTimes_4;
    mWaves[22].sounds = Wave22Sounds_4;
    mWaves[22].dontWaitForDead = Wave22DontWaitForDead_4;
    mWaves[22].healthPacks = Wave22HealthPacks_4;
    mWaves[22].ammoPacks = Wave22AmmoPacks_4;
    mWaves[23].count = 6;
    mWaves[23].enemies = Wave23Enemies_4;
    mWaves[23].waitTimes = Wave23WaitTimes_4;
    mWaves[23].sounds = Wave23Sounds_4;
    mWaves[23].dontWaitForDead = Wave23DontWaitForDead_4;
    mWaves[23].healthPacks = Wave23HealthPacks_4;
    mWaves[23].ammoPacks = Wave23AmmoPacks_4;
    mWaves[24].count = 6;
    mWaves[24].enemies = Wave24Enemies_4;
    mWaves[24].waitTimes = Wave24WaitTimes_4;
    mWaves[24].sounds = Wave24Sounds_4;
    mWaves[24].dontWaitForDead = Wave24DontWaitForDead_4;
    mWaves[24].healthPacks = Wave24HealthPacks_4;
    mWaves[24].ammoPacks = Wave24AmmoPacks_4;
    mWaves[25].count = 6;
    mWaves[25].enemies = Wave25Enemies_4;
    mWaves[25].waitTimes = Wave25WaitTimes_4;
    mWaves[25].sounds = Wave25Sounds_4;
    mWaves[25].dontWaitForDead = Wave25DontWaitForDead_4;
    mWaves[25].healthPacks = Wave25HealthPacks_4;
    mWaves[25].ammoPacks = Wave25AmmoPacks_4;
}

void Multi17Script::ThreePlayerInit()
{
    mIncrements = WaveIncrements_3;
    mWaves[1].count = 6;
    mWaves[1].enemies = Wave1Enemies_3;
    mWaves[1].waitTimes = Wave1WaitTimes_3;
    mWaves[1].sounds = Wave1Sounds_3;
    mWaves[1].dontWaitForDead = Wave1DontWaitForDead_3;
    mWaves[1].healthPacks = Wave1HealthPacks_3;
    mWaves[1].ammoPacks = Wave1AmmoPacks_3;
    mWaves[2].count = 6;
    mWaves[2].enemies = Wave2Enemies_3;
    mWaves[2].waitTimes = Wave2WaitTimes_3;
    mWaves[2].sounds = Wave2Sounds_3;
    mWaves[2].dontWaitForDead = Wave2DontWaitForDead_3;
    mWaves[2].healthPacks = Wave2HealthPacks_3;
    mWaves[2].ammoPacks = Wave2AmmoPacks_3;
    mWaves[3].count = 6;
    mWaves[3].enemies = Wave3Enemies_3;
    mWaves[3].waitTimes = Wave3WaitTimes_3;
    mWaves[3].sounds = Wave3Sounds_3;
    mWaves[3].dontWaitForDead = Wave3DontWaitForDead_3;
    mWaves[3].healthPacks = Wave3HealthPacks_3;
    mWaves[3].ammoPacks = Wave3AmmoPacks_3;
    mWaves[4].count = 1;
    mWaves[4].enemies = Wave4Enemies_3;
    mWaves[4].waitTimes = Wave4WaitTimes_3;
    mWaves[4].sounds = Wave4Sounds_3;
    mWaves[4].dontWaitForDead = Wave4DontWaitForDead_3;
    mWaves[4].healthPacks = Wave4HealthPacks_3;
    mWaves[4].ammoPacks = Wave4AmmoPacks_3;
    mWaves[5].count = 6;
    mWaves[5].enemies = Wave5Enemies_3;
    mWaves[5].waitTimes = Wave5WaitTimes_3;
    mWaves[5].sounds = Wave5Sounds_3;
    mWaves[5].dontWaitForDead = Wave5DontWaitForDead_3;
    mWaves[5].healthPacks = Wave5HealthPacks_3;
    mWaves[5].ammoPacks = Wave5AmmoPacks_3;
    mWaves[6].count = 6;
    mWaves[6].enemies = Wave6Enemies_3;
    mWaves[6].waitTimes = Wave6WaitTimes_3;
    mWaves[6].sounds = Wave6Sounds_3;
    mWaves[6].dontWaitForDead = Wave6DontWaitForDead_3;
    mWaves[6].healthPacks = Wave6HealthPacks_3;
    mWaves[6].ammoPacks = Wave6AmmoPacks_3;
    mWaves[7].count = 6;
    mWaves[7].enemies = Wave7Enemies_3;
    mWaves[7].waitTimes = Wave7WaitTimes_3;
    mWaves[7].sounds = Wave7Sounds_3;
    mWaves[7].dontWaitForDead = Wave7DontWaitForDead_3;
    mWaves[7].healthPacks = Wave7HealthPacks_3;
    mWaves[7].ammoPacks = Wave7AmmoPacks_3;
    mWaves[8].count = 1;
    mWaves[8].enemies = Wave8Enemies_3;
    mWaves[8].waitTimes = Wave8WaitTimes_3;
    mWaves[8].sounds = Wave8Sounds_3;
    mWaves[8].dontWaitForDead = Wave8DontWaitForDead_3;
    mWaves[8].healthPacks = Wave8HealthPacks_3;
    mWaves[8].ammoPacks = Wave8AmmoPacks_3;
    mWaves[9].count = 6;
    mWaves[9].enemies = Wave9Enemies_3;
    mWaves[9].waitTimes = Wave9WaitTimes_3;
    mWaves[9].sounds = Wave9Sounds_3;
    mWaves[9].dontWaitForDead = Wave9DontWaitForDead_3;
    mWaves[9].healthPacks = Wave9HealthPacks_3;
    mWaves[9].ammoPacks = Wave9AmmoPacks_3;
    mWaves[10].count = 6;
    mWaves[10].enemies = Wave10Enemies_3;
    mWaves[10].waitTimes = Wave10WaitTimes_3;
    mWaves[10].sounds = Wave10Sounds_3;
    mWaves[10].dontWaitForDead = Wave10DontWaitForDead_3;
    mWaves[10].healthPacks = Wave10HealthPacks_3;
    mWaves[10].ammoPacks = Wave10AmmoPacks_3;
    mWaves[11].count = 6;
    mWaves[11].enemies = Wave11Enemies_3;
    mWaves[11].waitTimes = Wave11WaitTimes_3;
    mWaves[11].sounds = Wave11Sounds_3;
    mWaves[11].dontWaitForDead = Wave11DontWaitForDead_3;
    mWaves[11].healthPacks = Wave11HealthPacks_3;
    mWaves[11].ammoPacks = Wave11AmmoPacks_3;
    mWaves[12].count = 1;
    mWaves[12].enemies = Wave12Enemies_3;
    mWaves[12].waitTimes = Wave12WaitTimes_3;
    mWaves[12].sounds = Wave12Sounds_3;
    mWaves[12].dontWaitForDead = Wave12DontWaitForDead_3;
    mWaves[12].healthPacks = Wave12HealthPacks_3;
    mWaves[12].ammoPacks = Wave12AmmoPacks_3;
    mWaves[13].count = 6;
    mWaves[13].enemies = Wave13Enemies_3;
    mWaves[13].waitTimes = Wave13WaitTimes_3;
    mWaves[13].sounds = Wave13Sounds_3;
    mWaves[13].dontWaitForDead = Wave13DontWaitForDead_3;
    mWaves[13].healthPacks = Wave13HealthPacks_3;
    mWaves[13].ammoPacks = Wave13AmmoPacks_3;
    mWaves[14].count = 6;
    mWaves[14].enemies = Wave14Enemies_3;
    mWaves[14].waitTimes = Wave14WaitTimes_3;
    mWaves[14].sounds = Wave14Sounds_3;
    mWaves[14].dontWaitForDead = Wave14DontWaitForDead_3;
    mWaves[14].healthPacks = Wave14HealthPacks_3;
    mWaves[14].ammoPacks = Wave14AmmoPacks_3;
    mWaves[15].count = 6;
    mWaves[15].enemies = Wave15Enemies_3;
    mWaves[15].waitTimes = Wave15WaitTimes_3;
    mWaves[15].sounds = Wave15Sounds_3;
    mWaves[15].dontWaitForDead = Wave15DontWaitForDead_3;
    mWaves[15].healthPacks = Wave15HealthPacks_3;
    mWaves[15].ammoPacks = Wave15AmmoPacks_3;
    mWaves[16].count = 1;
    mWaves[16].enemies = Wave16Enemies_3;
    mWaves[16].waitTimes = Wave16WaitTimes_3;
    mWaves[16].sounds = Wave16Sounds_3;
    mWaves[16].dontWaitForDead = Wave16DontWaitForDead_3;
    mWaves[16].healthPacks = Wave16HealthPacks_3;
    mWaves[16].ammoPacks = Wave16AmmoPacks_3;
    mWaves[17].count = 6;
    mWaves[17].enemies = Wave17Enemies_3;
    mWaves[17].waitTimes = Wave17WaitTimes_3;
    mWaves[17].sounds = Wave17Sounds_3;
    mWaves[17].dontWaitForDead = Wave17DontWaitForDead_3;
    mWaves[17].healthPacks = Wave17HealthPacks_3;
    mWaves[17].ammoPacks = Wave17AmmoPacks_3;
    mWaves[18].count = 6;
    mWaves[18].enemies = Wave18Enemies_3;
    mWaves[18].waitTimes = Wave18WaitTimes_3;
    mWaves[18].sounds = Wave18Sounds_3;
    mWaves[18].dontWaitForDead = Wave18DontWaitForDead_3;
    mWaves[18].healthPacks = Wave18HealthPacks_3;
    mWaves[18].ammoPacks = Wave18AmmoPacks_3;
    mWaves[19].count = 6;
    mWaves[19].enemies = Wave19Enemies_3;
    mWaves[19].waitTimes = Wave19WaitTimes_3;
    mWaves[19].sounds = Wave19Sounds_3;
    mWaves[19].dontWaitForDead = Wave19DontWaitForDead_3;
    mWaves[19].healthPacks = Wave19HealthPacks_3;
    mWaves[19].ammoPacks = Wave19AmmoPacks_3;
    mWaves[20].count = 1;
    mWaves[20].enemies = Wave20Enemies_3;
    mWaves[20].waitTimes = Wave20WaitTimes_3;
    mWaves[20].sounds = Wave20Sounds_3;
    mWaves[20].dontWaitForDead = Wave20DontWaitForDead_3;
    mWaves[20].healthPacks = Wave20HealthPacks_3;
    mWaves[20].ammoPacks = Wave20AmmoPacks_3;
    mWaves[21].count = 6;
    mWaves[21].enemies = Wave21Enemies_3;
    mWaves[21].waitTimes = Wave21WaitTimes_3;
    mWaves[21].sounds = Wave21Sounds_3;
    mWaves[21].dontWaitForDead = Wave21DontWaitForDead_3;
    mWaves[21].healthPacks = Wave21HealthPacks_3;
    mWaves[21].ammoPacks = Wave21AmmoPacks_3;
    mWaves[22].count = 6;
    mWaves[22].enemies = Wave22Enemies_3;
    mWaves[22].waitTimes = Wave22WaitTimes_3;
    mWaves[22].sounds = Wave22Sounds_3;
    mWaves[22].dontWaitForDead = Wave22DontWaitForDead_3;
    mWaves[22].healthPacks = Wave22HealthPacks_3;
    mWaves[22].ammoPacks = Wave22AmmoPacks_3;
    mWaves[23].count = 6;
    mWaves[23].enemies = Wave23Enemies_3;
    mWaves[23].waitTimes = Wave23WaitTimes_3;
    mWaves[23].sounds = Wave23Sounds_3;
    mWaves[23].dontWaitForDead = Wave23DontWaitForDead_3;
    mWaves[23].healthPacks = Wave23HealthPacks_3;
    mWaves[23].ammoPacks = Wave23AmmoPacks_3;
    mWaves[24].count = 6;
    mWaves[24].enemies = Wave24Enemies_3;
    mWaves[24].waitTimes = Wave24WaitTimes_3;
    mWaves[24].sounds = Wave24Sounds_3;
    mWaves[24].dontWaitForDead = Wave24DontWaitForDead_3;
    mWaves[24].healthPacks = Wave24HealthPacks_3;
    mWaves[24].ammoPacks = Wave24AmmoPacks_3;
    mWaves[25].count = 6;
    mWaves[25].enemies = Wave25Enemies_3;
    mWaves[25].waitTimes = Wave25WaitTimes_3;
    mWaves[25].sounds = Wave25Sounds_3;
    mWaves[25].dontWaitForDead = Wave25DontWaitForDead_3;
    mWaves[25].healthPacks = Wave25HealthPacks_3;
    mWaves[25].ammoPacks = Wave25AmmoPacks_3;
}

void Multi17Script::TwoPlayerInit()
{
    mIncrements = WaveIncrements_2;
    mWaves[1].count = 6;
    mWaves[1].enemies = Wave1Enemies_2;
    mWaves[1].waitTimes = Wave1WaitTimes_2;
    mWaves[1].sounds = Wave1Sounds_2;
    mWaves[1].dontWaitForDead = Wave1DontWaitForDead_2;
    mWaves[1].healthPacks = Wave1HealthPacks_2;
    mWaves[1].ammoPacks = Wave1AmmoPacks_2;
    mWaves[2].count = 6;
    mWaves[2].enemies = Wave2Enemies_2;
    mWaves[2].waitTimes = Wave2WaitTimes_2;
    mWaves[2].sounds = Wave2Sounds_2;
    mWaves[2].dontWaitForDead = Wave2DontWaitForDead_2;
    mWaves[2].healthPacks = Wave2HealthPacks_2;
    mWaves[2].ammoPacks = Wave2AmmoPacks_2;
    mWaves[3].count = 6;
    mWaves[3].enemies = Wave3Enemies_2;
    mWaves[3].waitTimes = Wave3WaitTimes_2;
    mWaves[3].sounds = Wave3Sounds_2;
    mWaves[3].dontWaitForDead = Wave3DontWaitForDead_2;
    mWaves[3].healthPacks = Wave3HealthPacks_2;
    mWaves[3].ammoPacks = Wave3AmmoPacks_2;
    mWaves[4].count = 1;
    mWaves[4].enemies = Wave4Enemies_2;
    mWaves[4].waitTimes = Wave4WaitTimes_2;
    mWaves[4].sounds = Wave4Sounds_2;
    mWaves[4].dontWaitForDead = Wave4DontWaitForDead_2;
    mWaves[4].healthPacks = Wave4HealthPacks_2;
    mWaves[4].ammoPacks = Wave4AmmoPacks_2;
    mWaves[5].count = 6;
    mWaves[5].enemies = Wave5Enemies_2;
    mWaves[5].waitTimes = Wave5WaitTimes_2;
    mWaves[5].sounds = Wave5Sounds_2;
    mWaves[5].dontWaitForDead = Wave5DontWaitForDead_2;
    mWaves[5].healthPacks = Wave5HealthPacks_2;
    mWaves[5].ammoPacks = Wave5AmmoPacks_2;
    mWaves[6].count = 6;
    mWaves[6].enemies = Wave6Enemies_2;
    mWaves[6].waitTimes = Wave6WaitTimes_2;
    mWaves[6].sounds = Wave6Sounds_2;
    mWaves[6].dontWaitForDead = Wave6DontWaitForDead_2;
    mWaves[6].healthPacks = Wave6HealthPacks_2;
    mWaves[6].ammoPacks = Wave6AmmoPacks_2;
    mWaves[7].count = 6;
    mWaves[7].enemies = Wave7Enemies_2;
    mWaves[7].waitTimes = Wave7WaitTimes_2;
    mWaves[7].sounds = Wave7Sounds_2;
    mWaves[7].dontWaitForDead = Wave7DontWaitForDead_2;
    mWaves[7].healthPacks = Wave7HealthPacks_2;
    mWaves[7].ammoPacks = Wave7AmmoPacks_2;
    mWaves[8].count = 1;
    mWaves[8].enemies = Wave8Enemies_2;
    mWaves[8].waitTimes = Wave8WaitTimes_2;
    mWaves[8].sounds = Wave8Sounds_2;
    mWaves[8].dontWaitForDead = Wave8DontWaitForDead_2;
    mWaves[8].healthPacks = Wave8HealthPacks_2;
    mWaves[8].ammoPacks = Wave8AmmoPacks_2;
    mWaves[9].count = 6;
    mWaves[9].enemies = Wave9Enemies_2;
    mWaves[9].waitTimes = Wave9WaitTimes_2;
    mWaves[9].sounds = Wave9Sounds_2;
    mWaves[9].dontWaitForDead = Wave9DontWaitForDead_2;
    mWaves[9].healthPacks = Wave9HealthPacks_2;
    mWaves[9].ammoPacks = Wave9AmmoPacks_2;
    mWaves[10].count = 6;
    mWaves[10].enemies = Wave10Enemies_2;
    mWaves[10].waitTimes = Wave10WaitTimes_2;
    mWaves[10].sounds = Wave10Sounds_2;
    mWaves[10].dontWaitForDead = Wave10DontWaitForDead_2;
    mWaves[10].healthPacks = Wave10HealthPacks_2;
    mWaves[10].ammoPacks = Wave10AmmoPacks_2;
    mWaves[11].count = 6;
    mWaves[11].enemies = Wave11Enemies_2;
    mWaves[11].waitTimes = Wave11WaitTimes_2;
    mWaves[11].sounds = Wave11Sounds_2;
    mWaves[11].dontWaitForDead = Wave11DontWaitForDead_2;
    mWaves[11].healthPacks = Wave11HealthPacks_2;
    mWaves[11].ammoPacks = Wave11AmmoPacks_2;
    mWaves[12].count = 1;
    mWaves[12].enemies = Wave12Enemies_2;
    mWaves[12].waitTimes = Wave12WaitTimes_2;
    mWaves[12].sounds = Wave12Sounds_2;
    mWaves[12].dontWaitForDead = Wave12DontWaitForDead_2;
    mWaves[12].healthPacks = Wave12HealthPacks_2;
    mWaves[12].ammoPacks = Wave12AmmoPacks_2;
    mWaves[13].count = 6;
    mWaves[13].enemies = Wave13Enemies_2;
    mWaves[13].waitTimes = Wave13WaitTimes_2;
    mWaves[13].sounds = Wave13Sounds_2;
    mWaves[13].dontWaitForDead = Wave13DontWaitForDead_2;
    mWaves[13].healthPacks = Wave13HealthPacks_2;
    mWaves[13].ammoPacks = Wave13AmmoPacks_2;
    mWaves[14].count = 6;
    mWaves[14].enemies = Wave14Enemies_2;
    mWaves[14].waitTimes = Wave14WaitTimes_2;
    mWaves[14].sounds = Wave14Sounds_2;
    mWaves[14].dontWaitForDead = Wave14DontWaitForDead_2;
    mWaves[14].healthPacks = Wave14HealthPacks_2;
    mWaves[14].ammoPacks = Wave14AmmoPacks_2;
    mWaves[15].count = 6;
    mWaves[15].enemies = Wave15Enemies_2;
    mWaves[15].waitTimes = Wave15WaitTimes_2;
    mWaves[15].sounds = Wave15Sounds_2;
    mWaves[15].dontWaitForDead = Wave15DontWaitForDead_2;
    mWaves[15].healthPacks = Wave15HealthPacks_2;
    mWaves[15].ammoPacks = Wave15AmmoPacks_2;
    mWaves[16].count = 1;
    mWaves[16].enemies = Wave16Enemies_2;
    mWaves[16].waitTimes = Wave16WaitTimes_2;
    mWaves[16].sounds = Wave16Sounds_2;
    mWaves[16].dontWaitForDead = Wave16DontWaitForDead_2;
    mWaves[16].healthPacks = Wave16HealthPacks_2;
    mWaves[16].ammoPacks = Wave16AmmoPacks_2;
    mWaves[17].count = 6;
    mWaves[17].enemies = Wave17Enemies_2;
    mWaves[17].waitTimes = Wave17WaitTimes_2;
    mWaves[17].sounds = Wave17Sounds_2;
    mWaves[17].dontWaitForDead = Wave17DontWaitForDead_2;
    mWaves[17].healthPacks = Wave17HealthPacks_2;
    mWaves[17].ammoPacks = Wave17AmmoPacks_2;
    mWaves[18].count = 6;
    mWaves[18].enemies = Wave18Enemies_2;
    mWaves[18].waitTimes = Wave18WaitTimes_2;
    mWaves[18].sounds = Wave18Sounds_2;
    mWaves[18].dontWaitForDead = Wave18DontWaitForDead_2;
    mWaves[18].healthPacks = Wave18HealthPacks_2;
    mWaves[18].ammoPacks = Wave18AmmoPacks_2;
    mWaves[19].count = 6;
    mWaves[19].enemies = Wave19Enemies_2;
    mWaves[19].waitTimes = Wave19WaitTimes_2;
    mWaves[19].sounds = Wave19Sounds_2;
    mWaves[19].dontWaitForDead = Wave19DontWaitForDead_2;
    mWaves[19].healthPacks = Wave19HealthPacks_2;
    mWaves[19].ammoPacks = Wave19AmmoPacks_2;
    mWaves[20].count = 1;
    mWaves[20].enemies = Wave20Enemies_2;
    mWaves[20].waitTimes = Wave20WaitTimes_2;
    mWaves[20].sounds = Wave20Sounds_2;
    mWaves[20].dontWaitForDead = Wave20DontWaitForDead_2;
    mWaves[20].healthPacks = Wave20HealthPacks_2;
    mWaves[20].ammoPacks = Wave20AmmoPacks_2;
    mWaves[21].count = 6;
    mWaves[21].enemies = Wave21Enemies_2;
    mWaves[21].waitTimes = Wave21WaitTimes_2;
    mWaves[21].sounds = Wave21Sounds;
    mWaves[21].dontWaitForDead = Wave21DontWaitForDead_2;
    mWaves[21].healthPacks = Wave21HealthPacks_2;
    mWaves[21].ammoPacks = Wave21AmmoPacks_2;
    mWaves[22].count = 6;
    mWaves[22].enemies = Wave22Enemies_2;
    mWaves[22].waitTimes = Wave22WaitTimes_2;
    mWaves[22].sounds = Wave22Sounds_2;
    mWaves[22].dontWaitForDead = Wave22DontWaitForDead_2;
    mWaves[22].healthPacks = Wave22HealthPacks_2;
    mWaves[22].ammoPacks = Wave22AmmoPacks_2;
    mWaves[23].count = 6;
    mWaves[23].enemies = Wave23Enemies_2;
    mWaves[23].waitTimes = Wave23WaitTimes_2;
    mWaves[23].sounds = Wave23Sounds_2;
    mWaves[23].dontWaitForDead = Wave23DontWaitForDead_2;
    mWaves[23].healthPacks = Wave23HealthPacks_2;
    mWaves[23].ammoPacks = Wave23AmmoPacks_2;
    mWaves[24].count = 6;
    mWaves[24].enemies = Wave24Enemies_2;
    mWaves[24].waitTimes = Wave24WaitTimes_2;
    mWaves[24].sounds = Wave24Sounds_2;
    mWaves[24].dontWaitForDead = Wave24DontWaitForDead_2;
    mWaves[24].healthPacks = Wave24HealthPacks;
    mWaves[24].ammoPacks = Wave24AmmoPacks;
    mWaves[25].count = 6;
    mWaves[25].enemies = Wave25Enemies_2;
    mWaves[25].waitTimes = Wave25WaitTimes_2;
    mWaves[25].sounds = Wave25Sounds_2;
    mWaves[25].dontWaitForDead = Wave25DontWaitForDead_2;
    mWaves[25].healthPacks = Wave25HealthPacks_2;
    mWaves[25].ammoPacks = Wave25AmmoPacks_2;
}

void Multi17Script::OnePlayerInit()
{
    mIncrements = WaveIncrements;
    mWaves[1].count = 6;
    mWaves[1].enemies = Wave1Enemies;
    mWaves[1].waitTimes = Wave1WaitTimes;
    mWaves[1].sounds = Wave1Sounds;
    mWaves[1].dontWaitForDead = Wave1DontWaitForDead;
    mWaves[1].healthPacks = Wave1HealthPacks;
    mWaves[1].ammoPacks = Wave1AmmoPacks;
    mWaves[2].count = 6;
    mWaves[2].enemies = Wave2Enemies;
    mWaves[2].waitTimes = Wave2WaitTimes;
    mWaves[2].sounds = Wave2Sounds;
    mWaves[2].dontWaitForDead = Wave2DontWaitForDead;
    mWaves[2].healthPacks = Wave2HealthPacks;
    mWaves[2].ammoPacks = Wave2AmmoPacks;
    mWaves[3].count = 6;
    mWaves[3].enemies = Wave3Enemies;
    mWaves[3].waitTimes = Wave3WaitTimes;
    mWaves[3].sounds = Wave3Sounds;
    mWaves[3].dontWaitForDead = Wave3DontWaitForDead;
    mWaves[3].healthPacks = Wave3HealthPacks;
    mWaves[3].ammoPacks = Wave3AmmoPacks;
    mWaves[4].count = 1;
    mWaves[4].enemies = Wave4Enemies;
    mWaves[4].waitTimes = Wave4WaitTimes;
    mWaves[4].sounds = Wave4Sounds;
    mWaves[4].dontWaitForDead = Wave4DontWaitForDead;
    mWaves[4].healthPacks = Wave4HealthPacks;
    mWaves[4].ammoPacks = Wave4AmmoPacks;
    mWaves[5].count = 6;
    mWaves[5].enemies = Wave5Enemies;
    mWaves[5].waitTimes = Wave5WaitTimes;
    mWaves[5].sounds = Wave5Sounds;
    mWaves[5].dontWaitForDead = Wave5DontWaitForDead;
    mWaves[5].healthPacks = Wave5HealthPacks;
    mWaves[5].ammoPacks = Wave5AmmoPacks;
    mWaves[6].count = 6;
    mWaves[6].enemies = Wave6Enemies;
    mWaves[6].waitTimes = Wave6WaitTimes;
    mWaves[6].sounds = Wave6Sounds;
    mWaves[6].dontWaitForDead = Wave6DontWaitForDead;
    mWaves[6].healthPacks = Wave6HealthPacks;
    mWaves[6].ammoPacks = Wave6AmmoPacks;
    mWaves[7].count = 6;
    mWaves[7].enemies = Wave7Enemies;
    mWaves[7].waitTimes = Wave7WaitTimes;
    mWaves[7].sounds = Wave7Sounds;
    mWaves[7].dontWaitForDead = Wave7DontWaitForDead;
    mWaves[7].healthPacks = Wave7HealthPacks;
    mWaves[7].ammoPacks = Wave7AmmoPacks;
    mWaves[8].count = 1;
    mWaves[8].enemies = Wave8Enemies;
    mWaves[8].waitTimes = Wave8WaitTimes;
    mWaves[8].sounds = Wave8Sounds;
    mWaves[8].dontWaitForDead = Wave8DontWaitForDead;
    mWaves[8].healthPacks = Wave8HealthPacks;
    mWaves[8].ammoPacks = Wave8AmmoPacks;
    mWaves[9].count = 6;
    mWaves[9].enemies = Wave9Enemies;
    mWaves[9].waitTimes = Wave9WaitTimes;
    mWaves[9].sounds = Wave9Sounds;
    mWaves[9].dontWaitForDead = Wave9DontWaitForDead;
    mWaves[9].healthPacks = Wave9HealthPacks;
    mWaves[9].ammoPacks = Wave9AmmoPacks;
    mWaves[10].count = 6;
    mWaves[10].enemies = Wave10Enemies;
    mWaves[10].waitTimes = Wave10WaitTimes;
    mWaves[10].sounds = Wave10Sounds;
    mWaves[10].dontWaitForDead = Wave10DontWaitForDead;
    mWaves[10].healthPacks = Wave10HealthPacks;
    mWaves[10].ammoPacks = Wave10AmmoPacks;
    mWaves[11].count = 6;
    mWaves[11].enemies = Wave11Enemies;
    mWaves[11].waitTimes = Wave11WaitTimes;
    mWaves[11].sounds = Wave11Sounds;
    mWaves[11].dontWaitForDead = Wave11DontWaitForDead;
    mWaves[11].healthPacks = Wave11HealthPacks;
    mWaves[11].ammoPacks = Wave11AmmoPacks;
    mWaves[12].count = 1;
    mWaves[12].enemies = Wave12Enemies;
    mWaves[12].waitTimes = Wave12WaitTimes;
    mWaves[12].sounds = Wave12Sounds;
    mWaves[12].dontWaitForDead = Wave12DontWaitForDead;
    mWaves[12].healthPacks = Wave12HealthPacks;
    mWaves[12].ammoPacks = Wave12AmmoPacks;
    mWaves[13].count = 6;
    mWaves[13].enemies = Wave13Enemies;
    mWaves[13].waitTimes = Wave13WaitTimes;
    mWaves[13].sounds = Wave13Sounds;
    mWaves[13].dontWaitForDead = Wave13DontWaitForDead;
    mWaves[13].healthPacks = Wave13HealthPacks;
    mWaves[13].ammoPacks = Wave13AmmoPacks;
    mWaves[14].count = 6;
    mWaves[14].enemies = Wave14Enemies;
    mWaves[14].waitTimes = Wave14WaitTimes;
    mWaves[14].sounds = Wave14Sounds;
    mWaves[14].dontWaitForDead = Wave14DontWaitForDead;
    mWaves[14].healthPacks = Wave14HealthPacks;
    mWaves[14].ammoPacks = Wave14AmmoPacks;
    mWaves[15].count = 6;
    mWaves[15].enemies = Wave15Enemies;
    mWaves[15].waitTimes = Wave15WaitTimes;
    mWaves[15].sounds = Wave15Sounds;
    mWaves[15].dontWaitForDead = Wave15DontWaitForDead;
    mWaves[15].healthPacks = Wave15HealthPacks;
    mWaves[15].ammoPacks = Wave15AmmoPacks;
    mWaves[16].count = 1;
    mWaves[16].enemies = Wave16Enemies;
    mWaves[16].waitTimes = Wave16WaitTimes;
    mWaves[16].sounds = Wave16Sounds;
    mWaves[16].dontWaitForDead = Wave16DontWaitForDead;
    mWaves[16].healthPacks = Wave16HealthPacks;
    mWaves[16].ammoPacks = Wave16AmmoPacks;
    mWaves[17].count = 6;
    mWaves[17].enemies = Wave17Enemies;
    mWaves[17].waitTimes = Wave17WaitTimes;
    mWaves[17].sounds = Wave17Sounds;
    mWaves[17].dontWaitForDead = Wave17DontWaitForDead;
    mWaves[17].healthPacks = Wave17HealthPacks;
    mWaves[17].ammoPacks = Wave17AmmoPacks;
    mWaves[18].count = 6;
    mWaves[18].enemies = Wave18Enemies;
    mWaves[18].waitTimes = Wave18WaitTimes;
    mWaves[18].sounds = Wave18Sounds;
    mWaves[18].dontWaitForDead = Wave18DontWaitForDead;
    mWaves[18].healthPacks = Wave18HealthPacks;
    mWaves[18].ammoPacks = Wave18AmmoPacks;
    mWaves[19].count = 6;
    mWaves[19].enemies = Wave19Enemies;
    mWaves[19].waitTimes = Wave19WaitTimes;
    mWaves[19].sounds = Wave19Sounds;
    mWaves[19].dontWaitForDead = Wave19DontWaitForDead;
    mWaves[19].healthPacks = Wave19HealthPacks;
    mWaves[19].ammoPacks = Wave19AmmoPacks;
    mWaves[20].count = 1;
    mWaves[20].enemies = Wave20Enemies;
    mWaves[20].waitTimes = Wave20WaitTimes;
    mWaves[20].sounds = Wave20Sounds;
    mWaves[20].dontWaitForDead = Wave20DontWaitForDead;
    mWaves[20].healthPacks = Wave20HealthPacks;
    mWaves[20].ammoPacks = Wave20AmmoPacks;
    mWaves[21].count = 6;
    mWaves[21].enemies = Wave21Enemies;
    mWaves[21].waitTimes = Wave21WaitTimes;
    mWaves[21].sounds = Wave21Sounds;
    mWaves[21].dontWaitForDead = Wave21DontWaitForDead;
    mWaves[21].healthPacks = Wave21HealthPacks;
    mWaves[21].ammoPacks = Wave21AmmoPacks;
    mWaves[22].count = 6;
    mWaves[22].enemies = Wave22Enemies;
    mWaves[22].waitTimes = Wave22WaitTimes;
    mWaves[22].sounds = Wave22Sounds;
    mWaves[22].dontWaitForDead = Wave22DontWaitForDead;
    mWaves[22].healthPacks = Wave22HealthPacks;
    mWaves[22].ammoPacks = Wave22AmmoPacks;
    mWaves[23].count = 6;
    mWaves[23].enemies = Wave23Enemies;
    mWaves[23].waitTimes = Wave23WaitTimes;
    mWaves[23].sounds = Wave23Sounds;
    mWaves[23].dontWaitForDead = Wave23DontWaitForDead;
    mWaves[23].healthPacks = Wave23HealthPacks;
    mWaves[23].ammoPacks = Wave23AmmoPacks;
    mWaves[24].count = 6;
    mWaves[24].enemies = Wave24Enemies;
    mWaves[24].waitTimes = Wave24WaitTimes;
    mWaves[24].sounds = Wave24Sounds;
    mWaves[24].dontWaitForDead = Wave24DontWaitForDead;
    mWaves[24].healthPacks = Wave24HealthPacks;
    mWaves[24].ammoPacks = Wave24AmmoPacks;
    mWaves[25].count = 6;
    mWaves[25].enemies = Wave25Enemies;
    mWaves[25].waitTimes = Wave25WaitTimes;
    mWaves[25].sounds = Wave25Sounds;
    mWaves[25].dontWaitForDead = Wave25DontWaitForDead;
    mWaves[25].healthPacks = Wave25HealthPacks;
    mWaves[25].ammoPacks = Wave25AmmoPacks;
}

void Multi17Script::Setup()
{
    // One `i` for both loops -- see probes/src/Multi5Script.cpp; two scoped copies put
    // the first loop's counter in a volatile register.
    int i;

    mDone = false;
    mStarted = false;
    mSpawnDelay = 0.0f;
    mNextSpawn = -1.0f;
    mWaiting = false;
    mStep = 0;
    mWave = 0;

    for (i = 0; i < 50; i++) {
        mHandles[i] = 0;
    }
    memset(mWaves, 0, sizeof(mWaves));

    for (i = 0; i < 6; i++) {
        if (ENEMY_ODFS[i]) {
            MissionUtility::PreloadConfig(ENEMY_ODFS[i]);
        }
    }
    MissionUtility::PreloadConfig("rep_inf_amidala");
    MissionUtility::PreloadConfig("rep_inf_mace_multi");
    MissionUtility::PreloadConfig("rep_inf_obiwan_multi");
    MissionUtility::PreloadConfig("rep_inf_anakin_multi");
    MissionUtility::PreloadConfig("rep_inf_jedi_multi");
    MissionUtility::PreloadConfig("rep_fly_gunship");
    MissionUtility::PreloadConfig("rep_inf_clone");
}

SPMission *Multi17BuildMission()
{
    return new Multi17Script();
}
