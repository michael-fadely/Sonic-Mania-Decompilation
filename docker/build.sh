#!/usr/bin/env bash
# =============================================================================
# Sonic Mania Plus -> Dreamcast : build orchestrator
# =============================================================================
# Builds/uses the KallistiOS toolchain Docker image and compiles the engine.
# Designed to run the heavy Docker work on a remote host with disk space
# (default: ryzen.lan) while the source tree lives on the dev laptop.
#
# Usage:
#   docker/build.sh image            # build the toolchain image (slow, ~30-60m)
#   docker/build.sh engine           # compile RetroEngine ELF (rev2)
#   docker/build.sh shell            # interactive shell inside the toolchain
#   docker/build.sh assets <Data.rsdk>   # run the DC asset conversion pipeline
#
# Env overrides:
#   REMOTE=ryzen.lan     remote docker host (empty = build locally)
#   REMOTE_DIR=/mnt/200GB/sonic-mania-dreamcast   remote checkout path
#   IMAGE=sonic-dc-toolchain:latest
#   JOBS=8
# =============================================================================
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REMOTE="${REMOTE:-ryzen.lan}"
REMOTE_DIR="${REMOTE_DIR:-/mnt/200GB/sonic-mania-dreamcast}"
IMAGE="${IMAGE:-sonic-dc-toolchain:latest}"
JOBS="${JOBS:-8}"

log()  { printf '\033[1;36m[build]\033[0m %s\n' "$*"; }
die()  { printf '\033[1;31m[build:error]\033[0m %s\n' "$*" >&2; exit 1; }

# Run a command either locally or on the remote docker host.
dhost() {
  if [[ -n "$REMOTE" ]]; then ssh "$REMOTE" "$@"; else bash -c "$*"; fi
}

sync_to_remote() {
  [[ -z "$REMOTE" ]] && return 0
  log "syncing source tree -> $REMOTE:$REMOTE_DIR (excluding workspace/, .git blobs kept)"
  ssh "$REMOTE" "mkdir -p '$REMOTE_DIR'"
  rsync -a --delete \
    --exclude 'workspace/' \
    --exclude '*/cmake-build-release/' \
    --exclude 'cmake-build-release/' \
    --exclude 'build-dc/' \
    "$REPO_ROOT/" "$REMOTE:$REMOTE_DIR/"
}

cmd_image() {
  sync_to_remote
  log "building toolchain image $IMAGE (this compiles the SH4 dc-chain; slow)"
  dhost "cd '$REMOTE_DIR' && docker build \
      --build-arg MAKE_JOBS=$JOBS \
      -f docker/Dockerfile -t '$IMAGE' docker/"
}

# Run the engine build inside the toolchain container against the synced tree.
cmd_engine() {
  sync_to_remote
  log "compiling RetroEngine (RETRO_REVISION=2, GAME_STATIC=ON)"
  dhost "docker run --rm -v '$REMOTE_DIR':/work -w /work '$IMAGE' bash -lc '
      set -e
      source \$KOS_BASE/environ.sh
      cmake -B workspace/build-dc -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE=\$KOS_BASE/utils/cmake/kallistios.toolchain.cmake \
        -DPLATFORM=KallistiOS \
        -DRETRO_REVISION=2 \
        -DGAME_STATIC=ON \
        -DCMAKE_BUILD_TYPE=Release
      cmake --build workspace/build-dc -j$JOBS
  '"
  log "done. artifacts under $REMOTE_DIR/workspace/build-dc"
}

cmd_shell() {
  sync_to_remote
  dhost "docker run --rm -it -v '$REMOTE_DIR':/work -w /work '$IMAGE' bash"
}

cmd_assets() {
  cmd_fixperms
  sync_to_remote
  local rsdk_host="${1:-workspace/assets/Data.rsdk}"
  log "processing assets from $rsdk_host (RSDKv5 extract -> DC-native dtex/adpcm/mpeg)"
  # generate_assets.sh must run from its own dir (uses local *.txt concat lists).
  # It writes the staged, DC-ready data tree we later burn to /cd/.
  # Run as the host UID/GID so build artifacts don't pollute the tree as root
  # (breaks the next rsync --delete). id resolved on the remote host.
  dhost "uid=\$(id -u); gid=\$(id -g); docker run --rm --user \$uid:\$gid -v '$REMOTE_DIR':/work -w /work '$IMAGE' bash -lc '
      set -e
      source \$KOS_BASE/environ.sh
      export HOME=/tmp
      cd dependencies/RSDKv5/dreamcast
      ./generate_assets.sh /work/${rsdk_host} /work/workspace/asset-src /work/workspace/cd-data
  '"
  log "staged DC assets under $REMOTE_DIR/workspace/cd-data"
}

# Reclaim ownership of any root-owned files a previous root container left in the
# tree (a root container chowns them back to the invoking host user). Idempotent.
cmd_fixperms() {
  [[ -z "$REMOTE" ]] && return 0
  dhost "uid=\$(id -u); gid=\$(id -g);
    if find '$REMOTE_DIR' -user 0 -print -quit 2>/dev/null | grep -q .; then
      echo '[build] reclaiming root-owned files via chown container';
      docker run --rm -v '$REMOTE_DIR':/work -w /work '$IMAGE' chown -R \$uid:\$gid /work;
    fi"
}

# Build a bootable Dreamcast disc image (.cdi by default) from the ELF + assets.
# Disc layout the KallistiOS port expects at /cd/:
#   /cd/Data.rsdk           datapack (GameConfig, Stages, Objects, Tiles, Palettes)
#   /cd/Data/Sprites|Images|Music|SoundFX|Video|Meshes   loose DC-native media
# The media loaders (Sprite/Audio/Video/Scene3D) fOpen loose files under
# ${KOS_USER_DIR}/Data/... directly; everything else comes from the datapack.
cmd_disc() {
  cmd_fixperms
  sync_to_remote
  local fmt="${1:-cdi}"
  log "assembling /cd root (Data.rsdk + Data/<media>) and packaging .$fmt with mkdcdisc"
  dhost "uid=\$(id -u); gid=\$(id -g); docker run --rm --user \$uid:\$gid -v '$REMOTE_DIR':/work -w /work '$IMAGE' bash -lc '
      set -e
      source \$KOS_BASE/environ.sh
      elf=\$(find workspace/build-dc -name RSDKv5.elf -o -name RetroEngine -o -name \"*.elf\" | head -1)
      [ -n \"\$elf\" ] || { echo \"no engine ELF found — run engine first\"; exit 1; }
      command -v mkdcdisc >/dev/null || { echo \"mkdcdisc missing in image\"; exit 3; }
      rsdk=workspace/assets/Data.rsdk
      [ -f \"\$rsdk\" ] || { echo \"Data.rsdk missing at \$rsdk\"; exit 4; }
      # Assemble the /cd root fresh each time.
      root=workspace/cd-root
      rm -rf \"\$root\"; mkdir -p \"\$root/Data\"
      cp \"\$rsdk\" \"\$root/Data.rsdk\"
      for d in Sprites Images Music SoundFX Video Meshes; do
        [ -d workspace/cd-data/\$d ] && cp -a workspace/cd-data/\$d \"\$root/Data/\"
      done
      echo \"cd-root assembled:\"; du -sh \"\$root\"; ls -1 \"\$root\" \"\$root/Data\"
      # strip the ELF to shrink the bootable binary (debug info is huge)
      cp \"\$elf\" workspace/RSDKv5-stripped.elf
      sh-elf-strip workspace/RSDKv5-stripped.elf || true
      mkdcdisc -e workspace/RSDKv5-stripped.elf -d \\\"\\$root\\\" -o workspace/SonicManiaDC.$fmt -V SONICMANIA
  '"
  log "disc image: $REMOTE_DIR/workspace/SonicManiaDC.$fmt"
}

case "${1:-}" in
  image)  cmd_image ;;
  engine) cmd_engine ;;
  shell)  cmd_shell ;;
  assets) shift; cmd_assets "$@" ;;
  disc)   shift; cmd_disc "$@" ;;
  *) die "usage: build.sh {image|engine|shell|assets [Data.rsdk]|disc [cdi|iso]}" ;;
esac
