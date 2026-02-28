#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
BUILD_TYPE="${BUILD_TYPE:-Release}"

log() {
  printf "[setup] %s\n" "$1"
}

warn() {
  printf "[setup][warn] %s\n" "$1"
}

has_cmd() {
  command -v "$1" >/dev/null 2>&1
}

install_macos_deps() {
  if ! has_cmd brew; then
    warn "Homebrew is required on macOS. Install from https://brew.sh and re-run this script."
    exit 1
  fi

  log "Installing macOS build dependencies with Homebrew"
  brew update
  brew install cmake ninja pkg-config

  if [[ "$(uname -m)" == "arm64" ]]; then
    log "Detected Apple Silicon (arm64)"
  else
    log "Detected Intel macOS"
  fi
}

install_linux_deps() {
  if has_cmd apt-get; then
    log "Installing Linux build dependencies with apt"
    sudo apt-get update
    sudo apt-get install -y \
      build-essential \
      cmake \
      ninja-build \
      pkg-config \
      libx11-dev \
      libxext-dev \
      libxrandr-dev \
      libxinerama-dev \
      libxcursor-dev \
      libxi-dev \
      libxfixes-dev \
      libwayland-dev \
      libxkbcommon-dev \
      wayland-protocols
  else
    warn "Unsupported Linux package manager. Install cmake, ninja, pkg-config, and X11/Wayland dev libraries manually."
  fi
}

configure_and_build() {
  log "Configuring CMake (${BUILD_TYPE})"
  cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

  log "Building project"
  cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}" -j

  if [[ -f "${BUILD_DIR}/compile_commands.json" ]]; then
    cp "${BUILD_DIR}/compile_commands.json" "${PROJECT_ROOT}/compile_commands.json"
    log "Wrote compile_commands.json at project root"
  fi

  log "Done. Run executable from ${BUILD_DIR}"
}

main() {
  local os
  os="$(uname -s)"

  case "${os}" in
    Darwin)
      log "Detected macOS"
      install_macos_deps
      ;;
    Linux)
      log "Detected Linux"
      install_linux_deps
      ;;
    MINGW*|MSYS*|CYGWIN*)
      warn "Detected Windows shell. Use Visual Studio tools + CMake, or run compile.bat from Developer Command Prompt."
      ;;
    *)
      warn "Unsupported OS: ${os}"
      ;;
  esac

  if ! has_cmd cmake; then
    warn "cmake is required but not found in PATH."
    exit 1
  fi

  configure_and_build
}

main "$@"
