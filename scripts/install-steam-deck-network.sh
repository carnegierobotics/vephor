#!/usr/bin/env bash
# Copy this checkout to a Steam Deck and run the on-device installer.
set -euo pipefail

usage() {
    cat <<'EOF'
Usage: bash scripts/install-steam-deck-network.sh IP [USER [PASSWORD]]

USER defaults to deck. If PASSWORD is omitted, prompt without echoing it.
Requires ssh, sshpass, and tar locally, and an enabled SSH server on the Deck.
The local checkout (including uncommitted files) is installed on the Deck.
EOF
}
if [[ ${1:-} == --help ]]; then
    usage
    exit 0
fi
if (( $# < 1 || $# > 3 )); then
    usage >&2
    exit 1
fi
host=$1
user=${2:-deck}
# Reject option-like values and shell syntax before constructing the SSH target.
if [[ ! $host =~ ^[a-zA-Z0-9][a-zA-Z0-9.:-]*$ || ! $user =~ ^[a-zA-Z_][a-zA-Z0-9_-]*$ ]]; then
    echo "Invalid IP/hostname or SSH user." >&2
    exit 1
fi
for tool in ssh sshpass tar; do
    command -v "$tool" >/dev/null || { echo "Missing required local tool: $tool" >&2; exit 1; }
done
if (( $# == 3 )); then
    password=$3
else
    read -r -s -p "SSH password for $user@$host: " password
    printf '\n'
fi
source_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)

remote() {
    # Supply the password on a separate descriptor, never in sshpass's arguments.
    sshpass -d 3 ssh -o StrictHostKeyChecking=accept-new -o ConnectTimeout=15 \
        -o ServerAliveInterval=30 -o ServerAliveCountMax=3 \
        "$user@$host" "$@" 3<<< "$password"
}

# A fresh source directory prevents overlaying stale files or changing an
# existing checkout. Preserve it on failure so the build can be diagnosed.
remote_source=$(remote 'mkdir -p "$HOME/vephor-tools/sources" && mktemp -d "$HOME/vephor-tools/sources/checkout.XXXXXXXX"')
if [[ $remote_source != /* || $remote_source == *$'\n'* ]]; then
    echo "Unexpected remote source directory: $remote_source" >&2
    exit 1
fi
printf -v quoted_source '%q' "$remote_source"
printf 'Copying checkout to %s:%s\n' "$host" "$remote_source"
tar --exclude=.git --exclude=build --exclude='build-*' --exclude=.venv \
    --exclude=__pycache__ -C "$source_dir" -czf - . \
    | remote "tar -xzf - -C $quoted_source"

# CMake caches bind to a source directory; each snapshot needs its own cache.
printf 'Running Steam Deck installer on %s...\n' "$host"
remote "VEPHOR_BUILD_DIR=$quoted_source/.build bash $quoted_source/scripts/install-steam-deck.sh"
unset password
printf '\nFinished. On the Deck: source ~/vephor-tools/activate.sh\nSource snapshot: %s\n' "$remote_source"
