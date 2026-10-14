#!/usr/bin/env bash
# Install the Python bindings and OpenGL backend without modifying SteamOS.
set -euo pipefail

if [[ ${1:-} == --help ]]; then
    cat <<'EOF'
Usage: bash scripts/install-steam-deck.sh

Run on an x86_64 Steam Deck, from any directory. Installs this checkout.
VEPHOR_INSTALL_DIR overrides the default ~/vephor-tools installation directory.
CMAKE_BUILD_PARALLEL_LEVEL overrides the default of 2 build jobs.
VEPHOR_BUILD_DIR overrides the default installation-directory/build cache.
EOF
    exit 0
fi
if (( $# )); then
    echo "Unexpected argument. Use --help for usage." >&2
    exit 1
fi
if [[ $(uname -s) != Linux || $(uname -m) != x86_64 ]]; then
    echo "This installer requires x86_64 Linux (Steam Deck)." >&2
    exit 1
fi
if (( EUID == 0 )); then
    echo "Run as your normal user, without sudo." >&2
    exit 1
fi
for tool in curl tar bzip2 git python3; do
    command -v "$tool" >/dev/null || { echo "Missing required tool: $tool" >&2; exit 1; }
done

source_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
install_dir=${VEPHOR_INSTALL_DIR:-$HOME/vephor-tools}
mkdir -p -- "$install_dir"
install_dir=$(cd -- "$install_dir" && pwd)
mamba_root=$install_dir/mamba
env_dir=$mamba_root/envs/vephor
micromamba=$install_dir/bin/micromamba
manif_dir=$install_dir/manif
manif_revision=65166b3bfd15b1020c29f3eaf6888e1009c79aea

exec > >(tee "$install_dir/install.log") 2>&1
trap 'echo "Installation failed. See $install_dir/install.log" >&2' ERR

if [[ ! -x $micromamba ]]; then
    curl --fail --location --retry 3 https://micro.mamba.pm/api/micromamba/linux-64/latest \
        --output "$install_dir/micromamba.tar.bz2"
    tar -xjf "$install_dir/micromamba.tar.bz2" -C "$install_dir" bin/micromamba
fi

# Eigen 3 and CMake 3 match the upstream project. GLFW 3.3 uses X11/GLX,
# required by Vephor's GLEW backend even on the Deck's Wayland desktop.
environment_action=create
if [[ -d $env_dir/conda-meta ]]; then
    environment_action=install
fi
"$micromamba" "$environment_action" --yes --root-prefix "$mamba_root" --name vephor \
    --override-channels --channel conda-forge \
    python=3.12 pip 'cmake<4' ninja cxx-compiler 'eigen<4' glew 'glfw<3.4' \
    libgl-devel xorg-libx11

run_env() {
    "$micromamba" run --root-prefix "$mamba_root" --name vephor "$@"
}

# Install manif separately: Vephor expects its installed MANIF::manif target.
if [[ ! -d $manif_dir ]]; then
    git clone https://github.com/artivis/manif.git "$manif_dir"
fi
if [[ $(git -C "$manif_dir" rev-parse HEAD) != "$manif_revision" ]]; then
    if [[ -n $(git -C "$manif_dir" status --porcelain --untracked-files=no) ]]; then
        echo "Local manif changes found in $manif_dir; cannot select tested revision." >&2
        exit 1
    fi
    git -C "$manif_dir" fetch origin "$manif_revision"
    git -C "$manif_dir" checkout --detach "$manif_revision"
fi
run_env cmake -G Ninja -S "$manif_dir" -B "$manif_dir/build-ninja" \
    -DCMAKE_INSTALL_PREFIX="$env_dir" -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF
run_env cmake --install "$manif_dir/build-ninja"

# Explicit paths prevent CMake mixing SteamOS and environment libraries.
run_env env CMAKE_BUILD_PARALLEL_LEVEL="${CMAKE_BUILD_PARALLEL_LEVEL:-2}" \
    python -m pip install --verbose "$source_dir" \
    --config-settings="build-dir=${VEPHOR_BUILD_DIR:-$install_dir/build}" \
    --config-settings="cmake.define.GLEW_SHARED_LIBRARY_RELEASE=$env_dir/lib/libGLEW.so" \
    --config-settings=cmake.define.OpenGL_GL_PREFERENCE=LEGACY \
    --config-settings="cmake.define.OPENGL_gl_LIBRARY=$env_dir/lib/libGL.so"

# Quote the installation directory for Bash, including paths containing spaces.
{
    printf 'export MAMBA_ROOT_PREFIX=%q\n' "$mamba_root"
    printf 'eval "$(%q shell hook --shell bash)"\n' "$micromamba"
    printf 'micromamba activate vephor\n'
} > "$install_dir/activate.sh"

run_env python - <<'PY'
import tempfile
from pathlib import Path
import numpy as np
import vephor as v

assert hasattr(v, "ogl"), "OpenGL bindings missing"
w = v.Window(320, 240, "Vephor installation test")
w.add(v.Axes(), np.array([0., 0., 0.]))
w.add(v.Sphere(), np.array([0., 0., -3.]))
assert w.getNumObjects() >= 2
with tempfile.TemporaryDirectory() as directory:
    output = Path(directory) / "smoke-test.vephor"
    w.save(str(output))
    assert output.exists()
print("PASS: import, NumPy arguments, scene creation, saving, OpenGL bindings")
PY
run_env python -m pip check
"$micromamba" env export --root-prefix "$mamba_root" --name vephor > "$install_dir/environment.yml"
printf '\nInstalled. In a Desktop Mode Bash terminal, run:\n  source %q\n  python your_script.py\n' "$install_dir/activate.sh"
