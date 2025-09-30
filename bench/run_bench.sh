#!/usr/bin/env bash
# Minimal benchmark runner for pfaedle
set -euo pipefail

# Usage: bench/run_bench.sh --dataset freiburg_small --osm <path> --gtfs <path> --out report.json
DATASET=""
OSM=""
GTFS=""
OUT="report.json"
THREADS=${THREADS:-$(sysctl -n hw.ncpu 2>/dev/null || echo 1)}

# Enable bash nullglob to avoid literal patterns when no file matches
shopt -s nullglob

first_existing() {
  for f in "$@"; do
    if [[ -f "$f" ]]; then
      echo "$f"
      return 0
    fi
  done
  return 1
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --dataset) DATASET="$2"; shift 2;;
    --osm) OSM="$2"; shift 2;;
    --gtfs) GTFS="$2"; shift 2;;
    --out) OUT="$2"; shift 2;;
    *) echo "Unknown arg: $1"; exit 1;;
  esac
done

# Provide defaults for known datasets
if [[ -n "$DATASET" ]]; then
  case "$DATASET" in
    freiburg_small)
      # Prefer local files if present; prefer .osm.bz2 over .pbf
      if [[ -z "$OSM" ]]; then
        CAND_BZ2=(
          datasets/freiburg/*.osm.bz2
          datasets/*frie*/**/*.osm.bz2
          datasets/*frie*/*.osm.bz2
          datasets/*.osm.bz2
        )
        CAND_PBF=(
          datasets/freiburg/*.osm.pbf
          datasets/*frie*/**/*.osm.pbf
          datasets/*frie*/*.osm.pbf
          datasets/*.osm.pbf
        )
        OSM=$(first_existing "${CAND_BZ2[@]}" 2>/dev/null || true)
        if [[ -z "${OSM:-}" ]]; then
          OSM=$(first_existing "${CAND_PBF[@]}" 2>/dev/null || true)
        fi
      fi
      if [[ -z "$GTFS" ]]; then
        CAND_ZIP=(
          datasets/freiburg/*.zip
          datasets/*frie*/**/*.zip
          datasets/*frie*/*.zip
          datasets/*.zip
        )
        GTFS=$(first_existing "${CAND_ZIP[@]}" 2>/dev/null || true)
      fi
      ;;
  esac
fi

if [[ -z "$OSM" || -z "$GTFS" ]]; then
  echo "Please provide --osm and --gtfs paths (or use --dataset freiburg_small)." >&2
  exit 2
fi

# Temporary: PBF support lands in PR-2. For PR-1, prefer .osm[.bz2]
if [[ "$OSM" == *.pbf ]]; then
  echo "NOTE: .pbf input is planned (PR-2). Current build expects .osm[.bz2]." >&2
  echo "If this run fails on .pbf, convert with osmium: osmium cat input.osm.pbf -o output.osm.bz2" >&2
fi

BIN="$(pwd)/build/pfaedle"
if [[ ! -x "$BIN" ]]; then
  echo "Binary not found at $BIN. Build with CMake first." >&2
  exit 3
fi

# Capture peak RSS using /usr/bin/time if available
TIME_BIN="/usr/bin/time"
UNAME_S=$(uname -s 2>/dev/null || echo "")
if [[ "$UNAME_S" == "Darwin" ]]; then
  TIME_FLAG="-l"   # macOS uses -l for resource usage
else
  TIME_FLAG="-v"   # GNU time verbose
fi
if ! command -v "$TIME_BIN" >/dev/null 2>&1; then
  TIME_BIN="time"; TIME_FLAG="";
fi

# Run baseline matching with metrics output
$TIME_BIN $TIME_FLAG "$BIN" -D -x "$OSM" -i "$GTFS" --metrics-out "$OUT" -o gtfs-out

echo "Wrote metrics to $OUT"
