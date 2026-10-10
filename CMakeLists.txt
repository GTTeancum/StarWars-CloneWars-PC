cmake_minimum_required(VERSION 3.20)

# ============================================================================
# Star Wars: The Clone Wars (Xbox, 2003) - Static Recompilation
#
# Copy this template to your game project root and customize:
#   1. Change the project name below
#   2. Set XBOXRECOMP_DIR to where you cloned xboxrecomp
#   3. Adjust source file lists as you add game-specific code
#   4. Add any game-specific compile definitions or link libraries
# ============================================================================

project(CloneWars C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Common definitions
add_compile_definitions(
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# -DCW_ABI_CHECK=ON: diagnostic build that reports every lifted function
# returning with ebx/esi/edi not restored (RECOMP_ABI_CALL in recomp_types.h).
option(CW_ABI_CHECK "Check callee-saved registers after every call" OFF)
if(CW_ABI_CHECK)
    add_compile_definitions(RECOMP_ABI_CHECK)
endif()

# ── xboxrecomp toolkit ──────────────────────────────────────────
#
# Option A: xboxrecomp as a subdirectory (simplest)
#   Place xboxrecomp/ next to your game project and use a relative path.
#
# Option B: find_package (if you install xboxrecomp system-wide)
#   find_package(xboxrecomp REQUIRED)
#
# Adjust this path to point to your xboxrecomp clone:
set(XBOXRECOMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../xboxrecomp" CACHE PATH
    "Path to the xboxrecomp toolkit root directory")

# Add xboxrecomp as a subdirectory -- this builds all 6 runtime libraries
# and provides the 'xboxrecomp' umbrella target to link against.
add_subdirectory(${XBOXRECOMP_DIR} ${CMAKE_BINARY_DIR}/xboxrecomp)

# ── Recompiled game executable ──────────────────────────────────

# Collect generated recompiled code.
# The recomp pipeline outputs files into src/recomp/gen/.
# With --split 1000, you get gen_0000.c, gen_0001.c, etc.
# CONFIGURE_DEPENDS so re-running the recompiler picks up added/removed files
# without a manual re-configure. A plain GLOB is evaluated once at configure
# time, so a newly generated file (an extra chunk, recomp_stubs_unresolved.c)
# is silently left out of the build and shows up as unresolved externals.
file(GLOB RECOMP_GEN_SOURCES CONFIGURE_DEPENDS "src/recomp/gen/*.c")

# Core game sources
# Layout (mirrors the GameCube decompilation's port/ tree):
#   src/host/       entry point, directories and saves, input and live control
#   src/platform/   the title's XDK libraries replaced: Direct3D 8, DirectSound, XMV
#   src/overrides/  hand-lifted replacements for single recompiled functions
#   src/game/       recovered original game source, one file per original TU
set(GAME_SOURCES
    src/host/main.c
    src/host/input.c
    src/platform/d3d8.c
    src/platform/dsound.c
    src/platform/xmv.c
    src/overrides/recomp_manual.c
)
# Recovered source (Shaymin's GameCube decompilation) runs through the bridge in
# src/game/glue; the generated glue (glue_gen.cpp) comes from tools/gc_bind.py.
# 32-bit only: the bridge is x86 (thiscall/fastcall).
option(CW_RECOVERED_GAME "Build the recovered original source in src/game" ON)
if(CW_RECOVERED_GAME AND CMAKE_SIZEOF_VOID_P EQUAL 4)   # the bridge is x86 (thiscall/fastcall)
    file(GLOB_RECURSE GAME_RECOVERED_SOURCES CONFIGURE_DEPENDS "src/game/*.cpp" "src/game/*.c")
    # CWScript.cpp is the engine-side script host: it registers with the frame
    # loop from static constructors and touches engine data the bridge does
    # not map. Kept as reference, not built. glue_fallback.c is for builds
    # without the bridge.
    list(FILTER GAME_RECOVERED_SOURCES EXCLUDE REGEX "/(CWScript\.cpp|glue_fallback\.c)$")
    list(APPEND GAME_SOURCES ${GAME_RECOVERED_SOURCES})
else()
    # Hooked functions resolve to their translated bodies (sub_X_gen).
    list(APPEND GAME_SOURCES src/game/glue/glue_fallback.c)
endif()

# The recomp pipeline also generates a dispatch header and function declarations.
# These are included by the generated .c files:
#   src/recomp/gen/recomp_funcs.h    - extern declarations for all recompiled functions
#   src/recomp/recomp_dispatch.h     - binary search dispatch table
#   src/recomp/recomp_types.h        - register model, ICALL macros, MEM helpers

add_executable(${PROJECT_NAME} WIN32
    ${GAME_SOURCES}
    ${RECOMP_GEN_SOURCES}
)

# Link against the xboxrecomp umbrella target (all 6 runtime libraries)
target_link_libraries(${PROJECT_NAME} PRIVATE xboxrecomp)

# Include paths for generated recomp headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recomp
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${XBOXRECOMP_DIR}/include
)

# Windows SDK libraries needed at link time
target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d11
    dxgi
    dxguid
    xinput
    winmm
    # Lets the fault handler resolve the host RIP back to the generated
    # function's symbol, i.e. to the guest function that faulted.
    dbghelp
)

# MSVC-specific: large address aware, increase section limit for big generated files
if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE
        /LARGEADDRESSAWARE
    )
    # The generated code files can be very large. Increase object file section limit.
    target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
endif()

# 32-bit build: the renderer uses Microsoft's own d3d8.dll, which Windows only
# ships as a 32-bit DLL. Named CloneWars32.exe so it stages beside the x64
# build. The image stays at its fixed default base: guest RAM is mapped at
# 0x20000000 and the Xbox's high windows wrap to 0x10000000-0x1F100000 (see
# xbox_MemoryLayoutInit), and ASLR could otherwise drop the image there.
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME CloneWars32)
    # The DX8 renderer (src/platform/d3d8.c) builds against the DirectX 8.1 SDK's
    # d3d8.h / d3d8.lib; without the SDK it compiles as pass-through wrappers.
    set(DXSDK8_DIR "C:/Program Files (x86)/DXSDK" CACHE PATH "DirectX 8.1 SDK root")
    if(EXISTS "${DXSDK8_DIR}/include/d3d8.h" AND EXISTS "${DXSDK8_DIR}/lib/d3d8.lib")
        # Only the three D3D8 headers are copied out: the SDK's include
        # directory also carries 2001-era basetsd.h/winnt.h pieces that break
        # the current Windows SDK headers if it is put on the search path.
        set(DX8_HEADER_DIR "${CMAKE_BINARY_DIR}/dx8include")
        file(COPY "${DXSDK8_DIR}/include/d3d8.h"
                  "${DXSDK8_DIR}/include/d3d8types.h"
                  "${DXSDK8_DIR}/include/d3d8caps.h"
             DESTINATION "${DX8_HEADER_DIR}")
        set_source_files_properties(src/platform/d3d8.c PROPERTIES
            COMPILE_DEFINITIONS CW_HAVE_D3D8=1
            INCLUDE_DIRECTORIES "${DX8_HEADER_DIR}")
        target_link_libraries(${PROJECT_NAME} PRIVATE "${DXSDK8_DIR}/lib/d3d8.lib")
        message(STATUS "DX8 renderer: using ${DXSDK8_DIR}")
    else()
        message(STATUS "DX8 renderer: DirectX 8.1 SDK not found at ${DXSDK8_DIR}; wrappers only")
    endif()
    if(MSVC)
        target_link_options(${PROJECT_NAME} PRIVATE /DYNAMICBASE:NO /BASE:0x00400000)
    endif()
endif()

# ── Game-specific additions ─────────────────────────────────────
#
# Add any game-specific libraries, loaders, or custom renderers below.
# Examples:
#
# # Custom texture loader for your game's format
# target_sources(${PROJECT_NAME} PRIVATE src/texture_loader.c)
#
# # If your game uses RenderWare, you may need a RW bridge layer
# target_sources(${PROJECT_NAME} PRIVATE src/rw_bridge.c)
#
# # Additional compile definitions for debugging
# target_compile_definitions(${PROJECT_NAME} PRIVATE
#     ENABLE_ICALL_TRACE=1
#     VERBOSE_KERNEL_LOG=1
# )

# ── Debug symbols ───────────────────────────────────────────────
#
# Keep symbols in the optimised build. The VEH crash handler in main.c
# resolves a faulting host address back to the guest function name through
# dbghelp, and without a .pdb beside the .exe every crash report during
# bring-up names no function at all -- which is most of what a crash report is
# worth here. /OPT:REF,ICF put back the size that /DEBUG would otherwise cost.
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Zi)
    target_link_options(${PROJECT_NAME} PRIVATE /DEBUG /OPT:REF /OPT:ICF)

    # Compile the generated sources in parallel. With the Visual Studio
    # generator, cmake --build -j hands MSBuild /m, which parallelises across
    # *projects* -- and this solution has one. Without /MP the 153 generated
    # files, 165 MB of C, are handed to a single cl.exe one after another.
    target_compile_options(${PROJECT_NAME} PRIVATE /MP)
endif()

# ── Staging ─────────────────────────────────────────────────────
#
# The game reads default.xbe, data.zwp and the Addon/Bins/Data/Movies folders
# out of the directory the .exe sits in (see resolve_dirs in src/host/main.c), so
# the build stages itself into the dumped game directory. Left in
# build/Release the .exe would find none of its data.
set(CLONEWARS_STAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." CACHE PATH
    "Directory holding the dumped game files; the .exe is staged here")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${PROJECT_NAME}>" "${CLONEWARS_STAGE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${CLONEWARS_STAGE_DIR}"
    COMMENT "Staging CloneWars.exe into the game directory")
