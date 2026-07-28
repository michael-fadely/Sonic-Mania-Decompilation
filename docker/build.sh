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
  local rsdk="${1:-}"
  [[ -n "$rsdk" ]] || die "usage: build.sh assets <path/to/Data.rsdk>"
  die "asset pipeline not wired yet — needs a legit Data.rsdk on the remote host"
}

case "${1:-}" in
  image)  cmd_image ;;
  engine) cmd_engine ;;
  shell)  cmd_shell ;;
  assets) shift; cmd_assets "$@" ;;
  *) die "usage: build.sh {image|engine|shell|assets <Data.rsdk>}" ;;
esac
