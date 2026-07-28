# Building Sonic Mania Plus for the Sega Dreamcast

This document describes how to build the Dreamcast port of Sonic Mania (RSDKv5 /
KallistiOS + PowerVR backend) from a clean checkout, and how to produce a
bootable disc image (`.cdi`) playable in Flycast or on real hardware.

Branch: `requeijaum/dc-build-fixes` (based on the upstream
`sf94/dreamcast-kallistios-pvr` work).

---

## 0. TL;DR

```bash
# One-time: build the SH4/AICA/KOS/sh4zam toolchain image (~40-60 min)
REMOTE=ryzen.lan JOBS=12 bash docker/build.sh image

# Compile the engine ELF (RETRO_REVISION=2, sh4zam, static link)
REMOTE=ryzen.lan JOBS=12 bash docker/build.sh engine

# Convert game assets to DC-native formats (needs a legit Data.rsdk)
REMOTE=ryzen.lan JOBS=12 bash docker/build.sh assets workspace/assets/Data.rsdk

# Assemble the /cd layout and burn a bootable CDI
REMOTE=ryzen.lan JOBS=12 bash docker/build.sh disc cdi
# -> workspace/SonicManiaDC.cdi
```

The build runs inside a Docker container on a remote host with disk space
(default `ryzen.lan:/mnt/200GB`). The source tree lives on the dev machine and
is rsync'd to the remote before each step. Set `REMOTE=` (empty) to build
locally instead.

---

## 1. Why Docker + a remote host

- The KallistiOS toolchain (SH4 GCC 15.2 + AICA arm-eabi GCC 8.5 + KOS kernel +
  kos-ports + sh4zam) is ~3.6 GB and takes 40-60 min to compile from source.
- We pin it in a reproducible image (`docker/Dockerfile`) so the toolchain is
  built once and cached.
- The dev laptop was disk-constrained (`/` at 99%), so heavy work is offloaded
  to `ryzen.lan` (`/mnt/200GB`). `docker/build.sh` rsyncs the tree there,
  runs the container, and leaves artifacts under `workspace/` on the remote.

## 2. The toolchain image (`docker/build.sh image`)

`docker/Dockerfile` builds, in order:
1. Debian bookworm base + build deps.
2. `kos-chain` (SH4 `sh-elf-gcc` 15.2.0, C++20) via the `stable` profile.
   - **Pitfall:** `makejobs` must be a bare number (`makejobs=12`), NOT `-j12` —
     KOS `init.mk` already prefixes `-j` (`jobs_arg = -j$(makejobs)`). Passing
     `-j12` silently builds only binutils and sh4zam later fails with
     `sh-elf-gcc: not found`.
3. AICA sound CPU toolchain (`arm-eabi-gcc` 8.5.0).
4. KallistiOS kernel + libc + kos-ports (`make` at `$KOS_BASE` also builds the
   `utils/` — this is where `pvrtex` and `wav2adpcm` come from).
5. sh4zam (SH4 SIMD math lib; the engine links against `libsh4zam.a`).
6. **Late layer** (added after the expensive toolchain so it never invalidates
   the cache): `ffmpeg`, `imagemagick`, and `mkdcdisc` (built from source via
   meson/ninja + libisofs). These are host tools for the asset pipeline and
   disc packaging.

Verify the image:
```bash
docker run --rm sonic-dc-toolchain:latest bash -lc \
  'source $KOS_BASE/environ.sh; sh-elf-gcc --version; \
   ls $KOS_BASE/addons/lib/dreamcast/libsh4zam.a; \
   which pvrtex wav2adpcm mkdcdisc ffmpeg convert'
```

## 3. Compiling the engine (`docker/build.sh engine`)

CMake config (see `dependencies/RSDKv5/platforms/KallistiOS.cmake`):
- `-DRETRO_REVISION=2` (Mania Plus content revision)
- `-DGAME_STATIC=ON` (game logic linked into the ELF, no dynamic mod loader)
- KallistiOS CMake toolchain file, `KOS_USER_DIR="/cd/"`
- Links `sh4zam`; `libogg` + `libtheora` are vendored and compiled in-tree
  (NOT from kos-ports).

Output: `workspace/build-dc/dependencies/RSDKv5/RSDKv5.elf`
(~14.5 MB with debug info; a 32-bit LSB SH ELF, statically linked).
The disc step strips it down to ~2.1 MB.

## 4. Asset pipeline (`docker/build.sh assets <Data.rsdk>`)

Input: a legitimate `Data.rsdk` (RSDKv5 container, `RSDKv5c` magic = compressed
rev2). Provide your own dump — it is NOT included in the repo. A PortMaster
Mania Plus `Data.rsdk` (v1.06, 207 MB) works.

`dependencies/RSDKv5/dreamcast/generate_assets.sh` extracts the container and
converts every asset to a DC-native format:
- **Sprites/Images** -> PowerVR `.dtex` textures (`pvrtex`, twiddled + compressed)
- **Music** -> Yamaha ADPCM (`ffmpeg` + `wav2adpcm`)
- **SoundFX** -> ADPCM
- **Video** (cutscenes) -> MPEG1 (`ffmpeg`, for the DC video decoder)
- **Meshes** (Special Stage 3D) -> optimized/stripified `.bin`
  (`mania-mesh-optimizer`, built on the fly)

Output staged under `workspace/cd-data/{Images,Music,SoundFX,Sprites,Video,Meshes}`
(~388 MB).

### Pitfalls fixed on this branch
- **`mania-mesh-optimizer` needed C++20 `<format>`** (GCC 13+), unavailable on
  the Debian bookworm host (GCC 12). Replaced `<format>` with a minimal
  `mmo::format` shim (sequential `{}` substitution only; all call sites use
  plain `{}`). Without this, every Special Stage mesh failed to convert.
- **Root-owned pollution:** the container ran as root and left
  `__pycache__/` and `cmake-build-release/` owned by root in the source tree,
  breaking the next `rsync --delete`. Fixed by (a) `cmd_fixperms` which chowns
  stale root files back via a throwaway root container, and (b) running the
  asset/disc containers as the host UID (`--user $uid:$gid`, `HOME=/tmp`).

## 5. Disc packaging (`docker/build.sh disc [cdi|iso]`)

The KallistiOS media loaders (`Sprite.cpp`, `Audio.cpp`, `Video.cpp`,
`Scene3D.cpp`) `fOpen` **loose** files under `${KOS_USER_DIR}/Data/<Media>/`
directly, while `GameConfig.bin`, Stages, Objects, Tiles and Palettes come from
the `Data.rsdk` datapack (`UserCore.cpp` -> `LoadDataPack("Data.rsdk")`).

So the disc root (`/cd/`) is assembled as:
```
/cd/Data.rsdk                 <- datapack (logic + original fallback assets)
/cd/Data/Sprites/  *.dtex     <- converted media (takes precedence over rsdk)
/cd/Data/Images/   *.dtex
/cd/Data/Music/    *.adpcm
/cd/Data/SoundFX/  *.adpcm
/cd/Data/Video/    *.mpg
/cd/Data/Meshes/   *.bin
```

`cmd_disc` builds this `workspace/cd-root`, strips the ELF, and runs:
```bash
mkdcdisc -e RSDKv5-stripped.elf -d workspace/cd-root \
         -o workspace/SonicManiaDC.cdi -N -V SONICMANIA
```

Output: **`workspace/SonicManiaDC.cdi`** (~710 MB). mkdcdisc writes the IP.BIN
bootstrap and `1ST_READ.BIN` automatically.

> Size note: 710 MB fits a CD-R only via overburn (99/90 min media). For real
> hardware prefer a GDEMU/MODE/USB-GDROM loader which reads the CDI directly,
> or produce a `.gdi` if your loader needs it. Because `Data.rsdk` still carries
> the original (unused) media, a future optimization is to trim the datapack to
> logic-only and shave ~200 MB.

## 6. Running it

### Flycast (emulator)
Load `SonicManiaDC.cdi` in Flycast (standalone or the RetroArch `flycast_libretro`
core). A working GPU/GL or Vulkan context is required — headless CI needs a real
GL driver (llvmpipe/EGL); a bare Xvfb without GLX will fail at
`video_driver_init_internal`.

### Real Dreamcast
Burn the CDI to CD-R (overburn) or load via GDEMU/MODE/USB-GDROM.

---

## 7. File map

| Path | Purpose |
|------|---------|
| `docker/Dockerfile` | KOS/sh4zam toolchain + asset/disc host tools |
| `docker/build.sh` | orchestrator: `image`/`engine`/`assets`/`disc`/`shell` |
| `dependencies/RSDKv5/platforms/KallistiOS.cmake` | engine CMake config (rev2, sh4zam) |
| `dependencies/RSDKv5/dreamcast/generate_assets.sh` | asset conversion pipeline |
| `dependencies/RSDKv5/dependencies/dreamcast/mania-mesh-optimizer/` | 3D mesh optimizer (host tool) |
| `workspace/` (gitignored, remote) | Data.rsdk, cd-data, cd-root, ELF, CDI |

All hardcoded `/opt/toolchains/dc` paths are provided by the container; nothing
is required on the host besides Docker.
