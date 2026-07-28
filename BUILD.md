# Building Sonic Mania Plus for the Sega Dreamcast

This is the **easy-path build guide** for the Dreamcast port of Sonic Mania Plus
(RSDKv5 + KallistiOS + PowerVR backend). Follow it top to bottom and you'll end
up with a bootable `SonicManiaDC.cdi` playable in Flycast or on real hardware.

> Deep internals, design decisions and every pitfall we hit are documented in
> **[BUILD-DREAMCAST.md](./BUILD-DREAMCAST.md)**. This file is the quickstart.

---

## 1. What you need (prerequisites)

The entire toolchain (SH4 GCC, KallistiOS, sh4zam, `pvrtex`, `wav2adpcm`,
`mkdcdisc`, `ffmpeg`) ships inside a **Docker image** — you do **not** install
any of it on your machine. You only need:

| Requirement | Why | Notes |
|-------------|-----|-------|
| **Docker** | builds & runs the toolchain image | the only hard dependency |
| **git** | clone the repo (recursively) | submodules matter — see below |
| **~6 GB free disk** | toolchain image (~3.6 GB) + assets (~400 MB) + CDI (~710 MB) | |
| **A legitimate `Data.rsdk`** | the game's data — **NOT** included, and never will be | see step 3 |
| *(optional)* **SSH + rsync** | only if building on a remote host | see "Remote builds" |

> ⚠️ **You must own Sonic Mania Plus.** The port ships **no game assets**. You
> provide your own `Data.rsdk` (e.g. from a Steam install or a PortMaster
> Mania Plus package, v1.06). Do **not** ask for or share it — that's piracy.

---

## 2. Get the source (recursively!)

This is a chain of git submodules. A non-recursive clone will **not** build.

```bash
git clone --recurse-submodules \
  --branch sf94/dreamcast-kallistios-pvr \
  https://github.com/michael-fadely/Sonic-Mania-Decompilation.git
cd Sonic-Mania-Decompilation
```

Already cloned without `--recurse-submodules`? Fix it with:

```bash
git submodule update --init --recursive
```

---

## 3. Provide your `Data.rsdk`

Drop your legitimate data pack here (create the folder if needed):

```bash
mkdir -p workspace/assets
cp /path/to/your/Data.rsdk workspace/assets/Data.rsdk
```

`workspace/` is gitignored — it holds your data and all build output.

---

## 4. Build — four commands

Run these from the repo root. The first one is slow (compiles the whole SH4
toolchain, ~40–60 min, **once**); the rest are fast.

```bash
# 1. One-time: build the toolchain Docker image (~40-60 min)
REMOTE= bash docker/build.sh image

# 2. Compile the engine ELF
REMOTE= bash docker/build.sh engine

# 3. Convert your assets to Dreamcast-native formats (dtex/adpcm/mpeg)
REMOTE= bash docker/build.sh assets workspace/assets/Data.rsdk

# 4. Assemble the /cd layout and burn the bootable disc
REMOTE= bash docker/build.sh disc cdi
```

Result: **`workspace/SonicManiaDC.cdi`** (~710 MB).

> `REMOTE=` (empty) forces a **local** build. Tune parallelism with
> `JOBS=$(nproc)`, e.g. `REMOTE= JOBS=12 bash docker/build.sh engine`.

---

## 5. Run it

### Flycast (emulator)
```bash
flycast workspace/SonicManiaDC.cdi
```
A real GPU/GL or Vulkan context is required. Headless CI needs a working GL
driver (llvmpipe/EGL); a bare `Xvfb` without GLX fails at video init.

### Real Dreamcast
Load the CDI via **GDEMU / MODE / USB-GDROM** (they read the CDI directly), or
burn to a 99/90-min CD-R with overburn. Region-free; boots as `SONICMANIA`.

---

## 6. Remote builds (optional)

If your machine is disk-constrained, offload the heavy Docker work to another
host over SSH. `build.sh` rsyncs the tree there and runs the container remotely.

```bash
# defaults: REMOTE=ryzen.lan  REMOTE_DIR=/mnt/200GB/sonic-mania-dreamcast
REMOTE=my-host JOBS=12 bash docker/build.sh image
REMOTE=my-host JOBS=12 bash docker/build.sh engine
REMOTE=my-host JOBS=12 bash docker/build.sh assets workspace/assets/Data.rsdk
REMOTE=my-host JOBS=12 bash docker/build.sh disc cdi
# -> $REMOTE:$REMOTE_DIR/workspace/SonicManiaDC.cdi
```

Put your `Data.rsdk` on the **remote** at `$REMOTE_DIR/workspace/assets/Data.rsdk`
(the `workspace/` folder is excluded from rsync, so copy it directly):

```bash
ssh my-host 'mkdir -p /mnt/200GB/sonic-mania-dreamcast/workspace/assets'
scp Data.rsdk my-host:/mnt/200GB/sonic-mania-dreamcast/workspace/assets/Data.rsdk
```

| Env var | Default | Meaning |
|---------|---------|---------|
| `REMOTE` | `ryzen.lan` | remote SSH host; **empty = build locally** |
| `REMOTE_DIR` | `/mnt/200GB/sonic-mania-dreamcast` | remote checkout path |
| `IMAGE` | `sonic-dc-toolchain:latest` | toolchain image tag |
| `JOBS` | `8` | parallel compile jobs |

---

## 7. Troubleshooting

| Symptom | Cause / fix |
|---------|-------------|
| `no engine ELF found — run engine first` | run step 2 (`engine`) before `disc` |
| `Data.rsdk missing at workspace/assets/Data.rsdk` | do step 3 (copy your data pack) |
| `mkdcdisc missing in image` | rebuild the image (step 1); it's in the late layer |
| `sh-elf-gcc: not found` during image build | KOS `makejobs` must be a bare number, not `-j12` — see BUILD-DREAMCAST.md §2 |
| CDI boots then resets in a loop | you're on an old artifact; this branch fixed it (GDB stub, `KOS_USER_DIR`, `mkdcdisc -D`, datapack streaming) — rebuild from step 2 |
| Flycast fails at `video_driver_init_internal` | no GL context; use a real driver or run headed |

---

## 8. Verifying the toolchain image

```bash
docker run --rm sonic-dc-toolchain:latest bash -lc \
  'source $KOS_BASE/environ.sh; sh-elf-gcc --version; \
   ls $KOS_BASE/addons/lib/dreamcast/libsh4zam.a; \
   which pvrtex wav2adpcm mkdcdisc ffmpeg convert'
```

---

For the full engineering breakdown — toolchain internals, asset pipeline,
disc layout, and every bug fixed on this branch — read
**[BUILD-DREAMCAST.md](./BUILD-DREAMCAST.md)**.
