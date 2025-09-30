#!/usr/bin/env python3
"""
Compute minimal quality metrics placeholder.
In this initial PR, we only validate that a report.json exists and has keys.
Later iterations will compute geometry-based metrics.
"""
import json
import sys

if len(sys.argv) < 2:
    print("usage: compute_quality.py report.json", file=sys.stderr)
    sys.exit(2)

p = sys.argv[1]
with open(p) as f:
    j = json.load(f)

required = ["version", "ingestion", "matching", "memory", "build"]
missing = [k for k in required if k not in j]
if missing:
    print(json.dumps({"ok": False, "missing": missing}), flush=True)
    sys.exit(1)

print(json.dumps({"ok": True}), flush=True)
