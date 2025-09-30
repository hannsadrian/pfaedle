#!/usr/bin/env python3
import json
import sys

def load(p):
    with open(p) as f:
        return json.load(f)

if len(sys.argv) != 3:
    print("usage: compare_reports.py baseline.json candidate.json", file=sys.stderr)
    sys.exit(2)

base = load(sys.argv[1])
new = load(sys.argv[2])

# Very light comparison for now: print deltas for a few fields
fields = [
    ("ingestion", "parse_wall_ms"),
    ("matching", "match_wall_ms_total"),
    ("memory", "peak_rss_mb"),
]

report = {}
for section, key in fields:
    b = base.get(section, {}).get(key)
    c = new.get(section, {}).get(key)
    if b is None or c is None:
        continue
    report[f"{section}.{key}"] = {"baseline": b, "candidate": c, "delta": c - b}

print(json.dumps(report, indent=2))
