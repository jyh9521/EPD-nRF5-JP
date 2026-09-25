#!/usr/bin/env bash
set -euo pipefail
script_dir="$(cd "$(dirname "$0")" && pwd)"
target="${1:?usage: ROLLBACK.sh TARGET_COPY}"
cp "$script_dir/EPD_service.c.original" "$target"
sha256sum "$target"
