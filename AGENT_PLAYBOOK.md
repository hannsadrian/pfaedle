# Pfaedle Performance & Memory Optimization — Agent Playbook

Purpose: This document is the single source of truth for an agentic LLM (and human reviewers) to plan, implement, evaluate, and safely roll out end-to-end performance and memory improvements to pfaedle while maintaining or improving output quality. It also covers controllable shape complexity/simplification to keep outputs compact. The agent MUST append its progress notes to Section 16 at the bottom after each meaningful step.

It assumes the starting point is a fork of [ad-freiburg/pfaedle](https://github.com/ad-freiburg/pfaedle). All new features must be gated behind feature flags and validated by automated benchmarks and quality checks.

---

## 0. Goals and Non-Goals

- Goals

  - Add OSM PBF streaming input support.
  - Reduce memory and improve speed via:
    - Early OSM filtering and PBF output for incremental runs.
    - Per-mode (MOT) graph pruning.
    - Optional OSM public transport relation–aware matching (soft constraints first, then optional hard constraints with safe fallbacks).
    - Stop-to-stop path caching and result reuse.
    - Careful memory layout/quantization.
  - Add controllable shape complexity/simplification:
    - Let users trade off shape fidelity vs filesize via method and tolerance parameters.
    - Preserve stop positions and topology; guarantee bounded deviation.
  - Add a robust benchmark and quality evaluation harness runnable locally and in CI.
  - Ensure correctness with conservative defaults and fallbacks.

- Non-Goals (for this phase)
  - Replacing the core matching algorithm wholesale.
  - Guaranteeing perfect relation mapping for all regions without human oversight.
  - Heavy-weight routing backends unless proven necessary (CH/MLD as optional experiment).

---

## 1. Baseline and Context

- pfaedle generates high-quality GTFS shapes via OSM-based map matching (C++).
- CLI highlights:
  - `pfaedle -x <OSM FILE> <GTFS INPUT FEED>`
  - `-D` to drop existing shapes in input feed.
  - `-m` to target specific modes (tram, bus, coach, rail, subway, ferry, funicular, gondola, or numeric GTFS route types).
  - `-X` to filter OSM and output a minimal OSM file for subsequent runs.
  - Debug: `-T <GTFS TRIP ID>`, `--write-graph`, `--write-trgraph`.
- Today typical OSM inputs are `.osm` or compressed `.osm.bz2` (XML-based). We will add `.pbf`.

---

## 2. High-Impact Plan (Reordered)

Prioritize wins that cap memory, reduce time-to-first-work, and maximize parallelism on country-scale runs.

1. Spatial sharding for matching (early parallelism, bounded RAM)

- Partition trips by tiles (H3 or bbox clusters), build per-tile subgraphs with padding, match tiles in parallel.

2. Strict bus graph pruning preset (smaller search space)

- Default-on preset limiting bus graphs to bus-usable highways; exclude irrelevant classes unless bus is explicitly permitted.

3. Trip de-duplication and stop-pair cache (dramatic reuse)

- Compute once per route/direction pattern; reuse across departures.

4. Parallelize preprocessing (remove single-core warmup)

- Multi-thread NodeGrid/EdgeGrid and other indexes.

5. Relation-aided soft corridors (where PTv2 exists)

- Weight bias inside relation corridor with confidence scoring and automatic fallback.

6. Auto-tuning for large datasets

- Safer defaults (enable sharding, scale grid-size, disable heavy caches) based on data size.

7. Memory layout improvements (compact structures)

- ID compaction, CSR adjacency, fixed-point coords, deduped polylines.

8. Shape simplification and deduplication (output size)

- Optional, bounded deviation with stop preservation.

9. Optional routing backend experiments (MLD/CH) if justified.

Each phase ships behind flags with tests, benchmarks, and updated docs.

---

## 3. New Feature Flags and Configuration

Add the following CLI/config options (all default to current behavior = “off” or “no change”):

- Input and filtering

  - `--osm-format auto|xml|pbf` (default: auto by file extension).
  - `--filter-output-format xml|pbf` (default: pbf).
  - `--filter-buffer-meters <int>` (default: 200).
  - `--filter-aggressive` (tighten filters; fallback widens when connectivity fails).

- Graph and matching

  - `--per-mot-graphs` (default: on when `-m` is provided; auto when multiple MOTs).
  - `--cache-stop-pairs` (default: off; stores per-run cache).
  - `--relation-mode off|soft|hard` (default: off).
  - `--relation-confidence-threshold 0.75` (for soft/hard; 0–1).
  - `--relation-buffer-meters 100` (for corridors).
  - `--relation-min-coverage 0.6` (fallback if lower).

- Shape complexity/simplification

  - `--shape-simplification off|dp|vw` (default: off)
    - `dp` = Douglas–Peucker; `vw` = Visvalingam–Whyatt (effective area).
  - `--shape-simplify-tolerance-meters <float>` (default: 0; only active if simplification != off)
  - `--shape-simplify-max-points <int>` (default: 0 = unlimited)
  - `--shape-simplify-preserve-stops true|false` (default: true)
  - `--shape-simplify-max-deviation-meters <float>` (default: same as tolerance; hard guardrail)
  - `--shape-deduplicate true|false` (default: true; reuse identical geometries across trips via shape_id)

- Performance

  - `--parallelism auto|<n>` (default: auto).
  - `--routing-backend auto|dijkstra|mld|ch` (default: auto; mld/ch experimental).
  - `--coord-quantization 1e-6` (fixed-point quantization; 1e-6 ~ 0.11m).

- Instrumentation
  - `--metrics-out <path>/report.json`
  - `--write-filtered-osm <path>` (used with `-X`).

---

## 4. Detailed Design

### 4.1 PBF Streaming Ingestion

- Dependencies

  - Add `libosmium` and `protozero` (system packages preferred; fallback to submodules if needed).
  - Optional: `zlib`, `bz2` already present; `libzip` for GTFS ZIP.

- Reader design

  - Autodetect by extension; support `.pbf`, `.osm`, `.osm.{gz,bz2}`, `.xml.{gz,bz2}`.
  - Use `osmium::io::Reader` with node/way/relation handlers.
  - Early tag filtering:
    - Keep minimal tags relevant to MOTs and matching.
    - Drop irrelevant features immediately to reduce memory churn.
  - Location store:
    - Small/medium datasets: `osmium::index::map::FlexMem` (in-memory).
    - Large datasets: `osmium::index::map::SparseMem` or disk-backed variants (document tradeoffs).
  - Build graph incrementally:
    - Emit only node coordinates, way refs, minimal metadata (id, access, oneway, railway/highway class, bus/tram/subway allowances).
    - Normalize coordinates to fixed-point int32 using `--coord-quantization`.
    - Store adjacency in compact CSR after first pass (way assembly).

- Output filtered OSM (`-X`)

  - Write `.osm.pbf` by default for minimal size.
  - Two-stage filter:
    1. Static: tag-based prefilter for target MOTs and bounding buffers around GTFS stops (spatial filter).
    2. Dynamic: widen buffers if connectivity checks fail during a quick skeleton routing pass.

- Backward compatibility
  - Keep XML path intact; fallback to previous parser if `--osm-format xml`.

Expected benefits: significantly faster IO and parsing, reduced memory, smaller filtered outputs for iterative runs.

### 4.2 Per-MOT Graph Pruning

- For `-m` or `--per-mot-graphs`, build distinct subgraphs with strict filters:

  - Rail/subway/tram:
    - `railway=rail|light_rail|subway|tram`
    - Exclude `service=yard|siding|spur` unless passenger service is implied.
  - Bus/coach:
    - `highway=*` that allows bus (bus=yes / designated, busway, lanes with bus allowance).
    - Exclude `access=private` unless explicitly bus permitted.
    - Penalize or exclude motorways unless `bus=yes`.
  - Ferry/funicular/gondola:
    - `route=ferry`, `aerialway=*`, `railway=funicular`.

- Build spatial indexes per graph (e.g., R-tree) for nearest-edge snapping and candidate pruning.
- Plug-in matching uses the selected graph for the trip MOT, reducing search space.

### 4.3 Relation-Aware Matching (PTv2)

- Index OSM relations:

  - `type=route`, `route=bus|tram|subway|light_rail|train|trolleybus|ferry|railway` and `type=route_master`.
  - Cache mapping: ways per relation; stops (platforms) if present.

- Map GTFS routes to OSM relations (build confidence score):

  - Signals: `route_short_name` ↔ `ref`, `route_long_name` ↔ `name`, `agency_id` ↔ `operator`, and `network`.
  - Spatial overlap: GTFS stops vs relation’s platforms/ways (buffered).
  - Direction: use GTFS `direction_id`; check OSM roles/forward/backward if present.
  - Confidence score (0–1): weighted combination of name/ref match + stop overlap + MOT alignment.

- Corridor construction:

  - Concatenate relation ways (respect order where available); dissolve into polyline set.
  - Expand into a corridor by `--relation-buffer-meters`.
  - Build an induced subgraph consisting of corridor edges + small halo (to bridge gaps).

- Matching modes:

  - `relation-mode=soft`:
    - Apply weight bias (e.g., multiply base cost by 0.5 on corridor edges; >1 outside).
    - Fallback automatically if `coverage < --relation-min-coverage`.
  - `relation-mode=hard`:
    - Restrict graph to corridor+halo; fallback to soft if disconnected; fallback to vanilla if still failing.

- Safety and audit:
  - Log relation id(s), confidence, coverage ratio, and fallback reason.
  - Persist per-route mapping to JSON for reuse across trips.

### 4.4 Stop-to-Stop Path Caching

- Key: `(from_stop_id, to_stop_id, mot, graph_hash, relation_hash, params_hash)`
- Value: shape polyline id/reference + metrics (length, time).
- Cache scope: in-memory per run; optional on-disk cache for repeated runs (versioned; cleared if graph changes).
- Warm-up:
  - For each GTFS route/direction, precompute adjacent stop pairs once (common case).
  - Reuse for identical pairs across trips and days.

### 4.5 Routing Backends (Optional)

- Default: bidirectional Dijkstra for rail/tram/subway (sparse graphs).
- Optional experiments for bus/road:
  - MLD or CH preprocessed graphs to accelerate repeated queries.
  - Include turn-cost model only if needed; start edge-based without turns.
  - Evaluate complexity vs gains in benchmarks before merging.

### 4.6 Memory Layout Improvements

- Node ids and edge ids remapped to contiguous `[0..N)` integers.
- Store coordinates as 32-bit fixed-point ints using `--coord-quantization` scale.
- CSR adjacency arrays (contiguous vectors) for cache efficiency.
- Deduplicate polylines (segments) by hashing coordinates; reference from edges.
- Incremental writing of shapes; avoid retaining all trip shapes in memory.
- Use arena allocators for transient parsing/build phases.

### 4.7 Parallelism

- Build graphs and indexes once (read-only).
- Process trips in parallel by:
  - Route buckets or stop-pair buckets to maximize cache reuse.
- Concurrency:
  - Read-only structures are shared; caches are sharded (e.g., by route hash) to minimize contention.

### 4.8 Shape Complexity Control and Simplification

Objective: Reduce shapes.txt filesize while keeping path fidelity and stop alignment within user-defined bounds.

- Methods

  - Douglas–Peucker (DP): preserves extremes; parameter = epsilon (tolerance).
  - Visvalingam–Whyatt (VW): removes points with smallest effective area first; smoother for curvy roads.

- Pipeline

  1. Start from the exact matched polyline between consecutive stops (or entire trip).
  2. Lock critical vertices:
     - All GTFS stops (and optionally intermediate snap points).
     - Segment endpoints and nodes at sharp turns (curvature > threshold) to preserve geometry at intersections.
  3. Apply simplification (DP or VW) with `--shape-simplify-tolerance-meters`.
  4. Guardrails:
     - Enforce `--shape-simplify-max-deviation-meters` Hausdorff bound; if violated, reduce tolerance or keep original segment.
     - Preserve topology: no self-intersections; maintain monotonic progress along the matched edge sequence.
     - Ensure each simplified segment remains within a small buffer of the used OSM edges.
  5. Optional cap: if `--shape-simplify-max-points` > 0, truncate by further increasing tolerance or switching to VW until under cap (while respecting guardrails).
  6. Deduplication:
     - If `--shape-deduplicate` is true, hash the resulting shape geometry (after quantization) and reuse shape_id for identical polylines across trips.

- Implementation notes

  - Operate in projected coordinates (e.g., local UTM) for stable meter-based tolerances; convert back to WGS84 for output.
  - Quantization prior to hashing prevents floating-point drift generating spurious shape_ids.
  - Keep an option to simplify per edge and then stitch, or per stop-to-stop path:
    - Per-edge simplification avoids deviating from edge geometry but may keep more points.
    - Per-pair simplification yields better compression; ensure the polyline stays near the edge chain.

- Defaults
  - `--shape-simplification off` by default to preserve current behavior.
  - Suggested presets:
    - Rail/tram/subway: dp, tolerance 2–5 m.
    - Bus: dp or vw, tolerance 5–15 m.
    - Ferries: dp, tolerance 10–25 m.
  - Preserve stops always unless explicitly disabled.

---

## 5. Benchmark and Quality Evaluation Harness

Add a small repo-internal runner under `bench/` with scripts and configs. The harness must run locally and in CI.

### 5.1 Datasets

- Small (fast CI smoke tests):
  - Freiburg region OSM + GTFS demo
    - OSM: [Geofabrik Baden-Württemberg, Freiburg regbez](https://download.geofabrik.de/europe/germany/baden-wuerttemberg.html)
      - Example: `freiburg-regbez-latest.osm.pbf`
    - GTFS: Freiburg VAG (or another small, freely licensed GTFS).
- Medium:
  - A well-maintained metro with PTv2 relations (e.g., Berlin, Vienna, Zurich).
- Large:
  - A state/province extract (e.g., Baden-Württemberg or Switzerland) to stress memory and IO.

Pin URLs and store checksums in `bench/datasets.yaml`. Do not commit data; download on demand and cache in CI.

### 5.2 Metrics to Collect

- Ingestion

  - `osm_input_format`, `osm_file_size_bytes`, `parse_wall_ms`, `nodes_kept`, `ways_kept`, `relations_kept`, `filtered_output_size_bytes` (if -X).

- Matching

  - `trips_total`, `trips_succeeded`, `trips_failed`
  - `match_wall_ms_total`, `match_wall_ms_p50`, `match_wall_ms_p95`
  - `cache_hit_rate` (if caching enabled)

- Memory

  - `peak_rss_mb` (from `/usr/bin/time -v`)

- Quality
  - Stop alignment:
    - `stop_to_path_distance_mean_m`, `p95_m`, `max_m`
    - `stops_over_threshold_pct` (thresholds: bus 50m, rail 25m)
  - Path plausibility:
    - `path_length_m`, `gc_length_m`, `length_ratio` (`path/gc`, bounded e.g., [1.0, 3.0] typical)
    - continuity (no gaps), no self-intersections
  - Tag adherence:
    - `forbidden_edge_violations` per MOT
  - Relation consistency (if enabled):
    - `relation_coverage_pct` (fraction of length on relation ways)
    - `hausdorff_m_mean` or `frechet_m_mean` vs relation polyline (optional)
  - Simplification impact (if enabled):
    - `shape_points_before`, `shape_points_after`, `shape_point_reduction_pct`
    - `shapes_txt_size_bytes` (on-disk), `shapes_size_reduction_pct`
    - `simplification_hausdorff_p95_m` (must be ≤ max-deviation)
    - `stops_preserved_pct` (should be 100% if preserve_stops=true)

Emit a single JSON file per run.

### 5.3 JSON Schema (example)

```json
{
  "version": 1,
  "dataset": "freiburg_small",
  "config": {
    "relation_mode": "off",
    "per_mot_graphs": true,
    "cache_stop_pairs": true,
    "shape_simplification": "dp",
    "shape_simplify_tolerance_meters": 8.0,
    "shape_simplify_preserve_stops": true
  },
  "ingestion": {
    "osm_input_format": "pbf",
    "osm_file_size_bytes": 123456789,
    "parse_wall_ms": 12345,
    "nodes_kept": 1000000,
    "ways_kept": 200000,
    "relations_kept": 5000,
    "filtered_output_size_bytes": 34567890
  },
  "matching": {
    "trips_total": 12000,
    "trips_succeeded": 11980,
    "trips_failed": 20,
    "match_wall_ms_total": 4567890,
    "match_wall_ms_p50": 210,
    "match_wall_ms_p95": 470,
    "cache_hit_rate": 0.62
  },
  "memory": {
    "peak_rss_mb": 3800
  },
  "quality": {
    "stop_to_path_distance_mean_m": 8.2,
    "stop_to_path_distance_p95_m": 20.5,
    "stop_to_path_distance_max_m": 48.3,
    "stops_over_threshold_pct": 0.7,
    "path_length_ratio_mean": 1.13,
    "forbidden_edge_violations": 0,
    "relation_coverage_pct": 0.0,
    "simplification": {
      "shape_points_before": 452310,
      "shape_points_after": 182440,
      "shape_point_reduction_pct": 59.7,
      "shapes_txt_size_bytes": 14300000,
      "shapes_size_reduction_pct": 54.1,
      "simplification_hausdorff_p95_m": 6.9,
      "stops_preserved_pct": 100.0
    }
  },
  "build": {
    "git_sha": "abcd1234",
    "pfaedle_version": "x.y.z"
  }
}
```

### 5.4 Harness Files to Add

- `bench/datasets.yaml` — dataset definitions (URLs, checksums, MOT mixes).
- `bench/run_bench.sh` — orchestrates runs, captures `/usr/bin/time -v`, writes `report.json`.
- `bench/compute_quality.py` — computes quality metrics from generated shapes against stops and (optionally) relations; computes simplification metrics if enabled.
- `bench/compare_reports.py` — A/B compare JSON reports (baseline vs feature) with thresholds.

Example CLI usage:

```bash
# Baseline on main
git checkout main && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j
bench/run_bench.sh --dataset freiburg_small --out baseline.json

# Feature branch
git checkout feat/pbf && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j
bench/run_bench.sh --dataset freiburg_small --out feature.json

# Compare
python3 bench/compare_reports.py --baseline baseline.json --candidate feature.json
```

### 5.5 Acceptance Thresholds (per dataset)

- Performance:

  - Ingestion wall time: must not worsen by >10% unless documented as temporary regression.
  - Matching wall time p95: must not worsen by >10% (target is improvement).
  - Peak RSS: must not increase by >10% (target is reduction).

- Quality:
  - `stops_over_threshold_pct` must not increase by >1% absolute.
  - `stop_to_path_distance_p95_m` must not worsen by >10%.
  - `forbidden_edge_violations` must remain 0 or decrease.
  - If `relation-mode` is enabled: `relation_coverage_pct` should be ≥ `--relation-min-coverage` for trips using relations; otherwise auto-fallback.
  - If simplification is enabled:
    - `simplification_hausdorff_p95_m` ≤ `--shape-simplify-max-deviation-meters`.
    - `stops_preserved_pct` = 100% (if preserve_stops=true).

CI blocks merges if thresholds fail unless PR is marked "experiment" and approved.

---

## 6. CI Integration (GitHub Actions)

Add a workflow `ci-bench.yml` that runs a small dataset on PRs and a medium dataset nightly.

to be determined.

---

## 7. Step-by-Step Implementation Guide (Agent Tasks — Revised Order)

Create small, reviewable PRs in the following order. Each PR must include:

- Code changes behind feature flags.
- Unit/integration tests where applicable.
- Benchmark run and JSON report artifacts attached to the PR.
- Updated docs in this playbook if the plan changes.

### PR-1: Benchmark Harness

- Add `bench/datasets.yaml`, `bench/run_bench.sh`, `bench/compute_quality.py`, `bench/compare_reports.py`.
- Implement basic metrics collection:
  - Timing with `std::chrono` instrumentation for key phases; aggregate to JSON.
  - `/usr/bin/time -v` wrapper to capture peak RSS.
- Add CI workflow `ci-bench.yml` with small dataset.
- Deliverables: baseline JSON report on small dataset.

### PR-2: PBF Input Support

- Integrate libosmium/protozero; autodetect by extension.
- Streaming parse to build in-memory graph with early tag filtering.
- `--osm-format` and `--filter-output-format` flags.
- Update `-X` to emit `.osm.pbf` by default.
- Tests:
  - Equivalence: run small dataset with XML vs PBF; shape diffs within thresholds.
- Benchmarks: show ingestion speedup and memory reduction.

### PR-3: Spatial Sharding for Matching

- Tile trips by H3 or bbox clusters; crop OSM per tile with padding; run tiles in parallel.
- Flags: `--shard-mode off|auto|h3`, `--shard-size-km`, `--shard-padding-m`, `--shard-parallelism`.
- Tests: correctness on tiny multi-tile dataset; merging outputs is stable.
- Benchmarks: peak RSS bounded (target ≤ 8–12 GB on national bus), early multi-core utilization.

### PR-4: Strict Bus Graph Pruning Preset

- Add `--bus-graph-preset strict|baseline` (default: strict for bus unless disabled).
- Include only bus-usable highways (bus=yes|designated, lanes, busway); exclude track/path/footway/cycleway/private unless bus-allowed; motorways only if bus=yes.
- Tests: zero forbidden-edge violations; route plausibility unchanged.
- Benchmarks: 30–50% node/edge reduction on bus graph; faster matching.

### PR-5: Stop-to-Stop Cache and Trip De-duplication

- Cache segments by (route_id, direction_id, stop_seq signature, mot, graph_hash, params_hash).
- Flags: `--cache-stop-pairs`, `--cache-dir`, `--cache-max-gb`.
- Tests: reuse across identical departures; correctness under cache hits.
- Benchmarks: ≥30% fewer computations on frequent services.

### PR-6: Parallelize Preprocessing

- Multi-thread NodeGrid/EdgeGrid/index builds; remove single-core warmup bottleneck.
- Benchmarks: 3–5x preprocessing speedup on 10+ cores; earlier CPU fan-out.

### PR-7: Relation-Aided Matching (Soft)

- Index PTv2 relations, build corridors, apply weight bias with confidence and fallback.
- Flags: as previously specified (`--relation-mode soft`, etc.).
- Benchmarks: 10–20% speedup where relations exist; no regressions elsewhere.

### PR-8: Auto-Tuning for Large Datasets

- Heuristics to auto-enable sharding, scale `--grid-size`, and disable heavy caches on large bboxes.
- Tests: defaults adapt on country/state datasets; still conservative on small regions.

### PR-9: Memory Layout Optimizations

- ID compaction, CSR adjacency, coord quantization, deduped polylines.
- Benchmarks: 10–20% RAM reduction; equal throughput.

### PR-10: Shape Complexity Controls

- Implement simplification and dedup as described; defaults off.
- Benchmarks: 40–60% shapes.txt reduction with bounded deviation.

### PR-11: Optional Routing Backend Experiments

- Evaluate MLD/CH backends for bus graphs; proceed only if clear wins.

---

## 8. Quality Metric Definitions (Details)

- Stop-to-path distance:

  - For each stop point, compute minimum perpendicular distance to the polyline. Use Shapely or custom projection math.
  - Aggregate mean, p95, max; threshold checks: bus 50m, rail 25m.

- Path length ratio:

  - Compute path length along shape polyline.
  - Compute great-circle distance (WGS84 haversine) between consecutive stops.
  - Ratio = path_length / gc_length. Typical urban: 1.05–1.8. Flag >3.0 as suspicious.

- Tag adherence:

  - While traversing edges used in final shapes, verify they meet MOT tag constraints.
  - Count violations; must be 0 (or strictly decreasing vs baseline).

- Relation coverage:

  - When a relation is applied, compute length-overlap fraction: length of shape that lies on relation ways vs total length.
  - Coverage below `--relation-min-coverage` triggers fallback (for that trip).

- Simplification error:

  - Compute Hausdorff distance between unsimplified and simplified polylines; p95 must be ≤ `--shape-simplify-max-deviation-meters`.
  - Stops preserving:
    - All GTFS stop coordinates must appear as vertices or as points on vertices that are not removed (if preserve_stops=true).
  - Topology checks:
    - No self-intersections; continuity preserved.

- Geometry validation:
  - No self-intersections (Shapely `is_simple`).
  - Continuity: no gaps between edges; ensure consistent node references.

---

## 9. Developer Setup and Build Notes

- Dependencies (Ubuntu)

  - `sudo apt-get install -y build-essential cmake libosmium2-dev libprotozero-dev libbz2-dev zlib1g-dev libzip-dev`
  - Python tools for bench: `pip install shapely rtree pyproj numpy`

- Build

  - `mkdir -p build && cd build`
  - `cmake -DCMAKE_BUILD_TYPE=Release ..`
  - `make -j`

- Running
  - Baseline:
    - `pfaedle -x data.osm.bz2 feed.zip`
  - With PBF:
    - `pfaedle -x data.osm.pbf feed.zip --osm-format pbf`
  - Filtering:
    - `pfaedle -X -x planet.osm.pbf -i feed.zip --write-filtered-osm filtered.osm.pbf`
  - Relation-aware soft constraints:
    - `pfaedle -x data.osm.pbf -i feed.zip --relation-mode soft --relation-confidence-threshold 0.8`
  - Shape simplification (example):
    - `pfaedle -x data.osm.pbf -i feed.zip --shape-simplification dp --shape-simplify-tolerance-meters 8 --shape-simplify-preserve-stops true --shape-deduplicate true`

---

## 10. Risk Management and Fallbacks

- Region variability: PTv2 relation quality differs by city/country. Default `relation-mode=off`; enable via config per deployment.
- Over-constraining risks:
  - Start with soft mode; collect coverage metrics.
  - Only use hard mode when confidence is high and connectivity proven.
- Simplification risks:
  - Ensure max deviation guardrail; always preserve stops unless explicitly disabled.
  - If quality gates fail, automatically disable simplification for that trip or reduce tolerance.
- Reproducibility:
  - Pin dataset URLs and checksums in `bench/datasets.yaml`.
  - Record pfaedle version and git sha in `report.json`.
- Rollback:
  - Keep XML path intact until PBF parity proven.
  - Feature flags default off; easy to disable in CI.
  - Each PR small and reversible.

---

## 11. Documentation and User-Facing Changes

- Update README:

  - Mention PBF support and how to enable relation-aware mode.
  - Document new flags and their defaults.
  - Add a “Performance Tips” section (use PBF, -X filtered PBF, per-MOT runs).
  - Add a “Shape Simplification” section with suggested tolerances per mode and caveats.

- Add this Playbook to the repo (e.g., `AGENT_PLAYBOOK.md`) and keep it current.

---

## 12. Expected Gains (Targets)

- PBF streaming + filtering: 2–5x faster OSM ingestion; significant memory reduction.
- Per-MOT pruning: 2–4x faster matching on mixed networks.
- Relation-aware (where coverage is good): 3–10x faster for those trips.
- Caching: 2–5x savings for repeated stop pairs and frequent services.
- Shape simplification: 30–70% reduction in shape points and shapes.txt size with ≤5–10 m deviation (mode-dependent).
- Combined: 5–20x end-to-end speedup on well-annotated networks with lower peak RSS, no quality regressions.

---

## 13. Glossary

- GTFS: General Transit Feed Specification.
- MOT: Mode of transport (bus, rail, tram, subway, etc.).
- PTv2: Public Transport v2 tagging scheme in OSM.
- PBF: Protocolbuffer Binary Format for OSM.
- CSR: Compressed Sparse Row (graph adjacency representation).
- CH/MLD: Contraction Hierarchies / Multi-Level Dijkstra.
- DP: Douglas–Peucker line simplification.
- VW: Visvalingam–Whyatt line simplification.

---

## 14. References

- pfaedle README: usage and flags.
- OSM PTv2 schema: https://wiki.openstreetmap.org/wiki/Public_transport
- libosmium: https://osmcode.org/libosmium/
- protozero: https://github.com/mapbox/protozero
- Geofabrik extracts: https://download.geofabrik.de/
- Shapely: https://shapely.readthedocs.io/
- Rtree: https://rtree.readthedocs.io/
- PyProj: https://pyproj4.github.io/pyproj/
- Douglas–Peucker: https://en.wikipedia.org/wiki/Ramer–Douglas–Peucker_algorithm
- Visvalingam–Whyatt: https://en.wikipedia.org/wiki/Visvalingam%E2%80%93Whyatt_algorithm

---

## 15. Checklist for Each PR

- [ ] Feature behind a flag; defaults preserve current behavior.
- [ ] Unit/integration tests updated or added.
- [ ] Benchmarks run on small dataset; `report.json` attached in PR.
- [ ] A/B comparison against `main` meets thresholds.
- [ ] README and this playbook updated if behavior/flags changed.
- [ ] Logs/metrics include enough detail for diagnosis.

---

## 16. Documentation of the Agent

The agent MUST append a new entry to the log below after each significant step (PR created/merged, benchmark milestone, design decision, rollback). Do not modify previous entries. Keep entries concise but complete. Use UTC timestamps.

Template to copy for each new entry:

```
- timestamp: YYYY-MM-DD HH:MM:SSZ
  actor: agent
  branch: feat/<short-name>
  pr_url: https://github.com/<owner>/<repo>/pull/<number>
  status: opened|merged|updated|abandoned
  scope:
    - area: ingestion|graphs|relations|caching|memory|simplification|routing|bench|ci|docs|other
      summary: <one-line summary of the change>
  datasets:
    - name: freiburg_small
      metrics_before:
        parse_wall_ms: <int>
        match_p95_ms: <int>
        peak_rss_mb: <int>
        shapes_size_bytes: <int>
      metrics_after:
        parse_wall_ms: <int>
        match_p95_ms: <int>
        peak_rss_mb: <int>
        shapes_size_bytes: <int>
      quality_after:
        stop_p95_m: <float>
        forbidden_violations: <int>
        simplification_hausdorff_p95_m: <float|null>
        relation_coverage_pct: <float|null>
      notes: <key observations, regressions, or follow-ups>
  flags_used:
    osm_format: auto|xml|pbf
    relation_mode: off|soft|hard
    per_mot_graphs: true|false
    cache_stop_pairs: true|false
    shape_simplification: off|dp|vw
    shape_simplify_tolerance_meters: <float>
    shape_simplify_preserve_stops: true|false
    shape_deduplicate: true|false
  next_actions:
    - <short actionable item>
    - <short actionable item>
```

Agent progress log (append new entries below):

- timestamp: 2025-09-30 10:46:00Z
  actor: agent
  branch: init/playbook
  pr_url: ""
  status: initialized
  scope:

  - area: docs
    summary: Seeded AGENT_PLAYBOOK.md with performance, memory, relations, simplification, and benchmarking plan.
    datasets: []
    flags_used:
    osm_format: auto
    relation_mode: off
    per_mot_graphs: false
    cache_stop_pairs: false
    shape_simplification: off
    shape_simplify_tolerance_meters: 0
    shape_simplify_preserve_stops: true
    shape_deduplicate: true
    next_actions:
  - Create PR-1 adding benchmark harness, metrics, and CI small dataset job.
  - Produce baseline report.json for freiburg_small and attach to PR.

- timestamp: 2025-09-30 13:52:00Z
  actor: agent
  branch: feat/bench-pr1
  pr_url: ""
  status: merged
  scope:

  - area: bench
    summary: Completed PR-1 — added benchmark harness, metrics flag, and validated a Freiburg smoke run.
  - area: docs
    summary: README updated with metrics and bench usage; help includes --metrics-out.
    datasets:
  - name: freiburg_small
    metrics_before:
    parse_wall_ms: 0
    match_p95_ms: 0
    peak_rss_mb: 0
    shapes_size_bytes: 0
    metrics_after:
    parse_wall_ms: 157003
    match_p95_ms: 0
    peak_rss_mb: 578
    shapes_size_bytes: 0
    quality_after:
    stop_p95_m: null
    forbidden_violations: null
    simplification_hausdorff_p95_m: null
    relation_coverage_pct: null
    notes: |
    Report summary (report.json): trips_total=39671, trips_succeeded=39671, trips_failed=0, match_wall_ms_total≈2755ms. Ingestion time currently aggregates GTFS+OSM build. On macOS, both maximum resident set size and peak memory footprint are printed; report uses peak_rss_mb≈577.7. Earlier, an assertion in EDijkstra.tpp (A\* heuristic monotonicity) was observed under a different invocation; latest Freiburg run completed cleanly. If it reappears, a temporary workaround is to run with --no-a-star; PR-2/PR-3 should harden heuristics and add guards.
    flags_used:
    osm_format: xml
    relation_mode: off
    per_mot_graphs: false
    cache_stop_pairs: false
    shape_simplification: off
    shape_simplify_tolerance_meters: 0
    shape_simplify_preserve_stops: true
    shape_deduplicate: true
    next_actions:
  - Start PR-2: add libosmium PBF streaming ingestion and -X PBF output
  - Add CI smoke job to run freiburg_small and publish report.json artifact
  - Split ingestion metrics (gtfs_parse_ms, osm_parse_ms) and add p50/p95 match timings
  - Add heuristic guards or fallback path to avoid EDijkstra assertion on edge cases

- timestamp: 2025-09-30 14:30:00Z
  actor: agent
  branch: feat/pbf-pr2
  pr_url: ""
  status: opened
  scope:

  - area: ingestion
    summary: Added PBF scaffolding (flags, CMake detection, OsmPbfReader stub) and wiring with graceful fallbacks.
  - area: docs
    summary: Help text now documents --osm-format and --filter-output-format; default -X emits PBF when available.
    datasets:
  - name: freiburg_small
    metrics_before:
    parse_wall_ms: 157003
    match_p95_ms: 0
    peak_rss_mb: 578
    shapes_size_bytes: 0
    metrics_after:
    parse_wall_ms: 0
    match_p95_ms: 0
    peak_rss_mb: 0
    shapes_size_bytes: 0
    quality_after:
    stop_p95_m: null
    forbidden_violations: null
    simplification_hausdorff_p95_m: null
    relation_coverage_pct: null
    notes: Build succeeds without libosmium; PBF paths compile behind OSMIUM_ENABLED and log clear errors if requested without support.
    flags_used:
    osm_format: auto
    relation_mode: off
    per_mot_graphs: false
    cache_stop_pairs: false
    shape_simplification: off
    shape_simplify_tolerance_meters: 0
    shape_simplify_preserve_stops: true
    shape_deduplicate: true
    next_actions:
  - Implement full libosmium-based PBF streaming reader parity with XML path
  - Add .pbf conversion in bench harness and parity tests XML vs PBF on small dataset

- timestamp: 2025-09-30 11:05:00Z
  actor: agent
  branch: feat/bench-pr1
  pr_url: ""
  status: opened
  scope:

  - area: bench
    summary: Added minimal benchmark harness (datasets.yaml, run_bench.sh, compute_quality.py, compare_reports.py) and metrics emission via --metrics-out.
  - area: docs
    summary: Updated help to document --metrics-out.
    datasets:
  - name: freiburg_small
    metrics_before:
    parse_wall_ms: 0
    match_p95_ms: 0
    peak_rss_mb: 0
    shapes_size_bytes: 0
    metrics_after:
    parse_wall_ms: 0
    match_p95_ms: 0
    peak_rss_mb: 0
    shapes_size_bytes: 0
    quality_after:
    stop_p95_m: 0
    forbidden_violations: 0
    simplification_hausdorff_p95_m: null
    relation_coverage_pct: null
    notes: Initial scaffolding only; quality metrics to be implemented in later PRs.
    flags_used:
    osm_format: auto
    relation_mode: off
    per_mot_graphs: false
    cache_stop_pairs: false
    shape_simplification: off
    shape_simplify_tolerance_meters: 0
    shape_simplify_preserve_stops: true
    shape_deduplicate: true
    next_actions:
  - Build and run bench on small local dataset to produce baseline report.json
  - Add CI job to run bench smoke test

- timestamp: 2025-09-30 14:35:00Z
  actor: agent
  branch: feat/pbf-pr2
  pr_url: ""
  status: opened
  scope:

  - area: ingestion
    summary: Added CLI flags --osm-format and --filter-output-format; scaffolded PBF reader using libosmium with fallback; wired format selection in main; optional filtered PBF output for -X.
  - area: docs
    summary: Help text updated to mention PBF input and filter output formats.
    datasets:
  - name: freiburg_small
    metrics_before:
    parse_wall_ms: 157003
    match_p95_ms: 0
    peak_rss_mb: 578
    shapes_size_bytes: 0
    metrics_after:
    parse_wall_ms: 0
    match_p95_ms: 0
    peak_rss_mb: 0
    shapes_size_bytes: 0
    quality_after:
    stop_p95_m: null
    forbidden_violations: null
    simplification_hausdorff_p95_m: null
    relation_coverage_pct: null
    notes: Initial scaffolding does not change default behavior; PBF reader logs a placeholder message until streaming ingestion is fully implemented. Filtered PBF conversion uses libosmium when available.
    flags_used:
    osm_format: auto
    relation_mode: off
    per_mot_graphs: false
    cache_stop_pairs: false
    shape_simplification: off
    shape_simplify_tolerance_meters: 0
    shape_simplify_preserve_stops: true
    shape_deduplicate: true
    next_actions:
  - Implement full libosmium streaming read to build Graph (bbox nodes, relations, ways, edges) with early filtering
  - Add equivalence tests comparing XML vs PBF ingestion on small dataset
  - Update CI to install libosmium/protozero on Linux/macOS runners

- timestamp: 2025-09-30 16:30:00Z
  actor: agent
  branch: feat/pbf-pr2
  pr_url: ""
  status: in-progress
  scope:
  - area: build
    summary: Bumped C++ standard to C++17. Auto-detect libosmium/protozero headers; define OSMIUM_ENABLED. Linked Expat to satisfy libosmium XML parser symbols. Added PFAEDLE_SILENCE_WARNINGS flag.
  - area: ingestion
    summary: Implemented native PBF streaming reader (OsmPbfReader::read) using libosmium. Multi-pass: nodes (bbox, node flags), relations (keep/drop, restrictions), ways pre-scan (collect needed nodes), nodes+ways (NodeLocationsForWays) with only-index-needed-nodes optimization. Preserved XML post-processing sequence to maintain behavior.
  - area: tooling/docs
    summary: Updated bench/run_bench.sh to acknowledge PBF support and mention --osm-format. Added convertToPbf and convertToXml utilities using libosmium any_input.
  - area: filter (-X)
    summary: For PBF input, -X now converts input to temp XML, runs existing XML-only filterWrite, and optionally converts filtered XML to PBF. Fixed temp filename derivation and registered xml_output so libosmium can write XML.
    datasets:
  - name: freiburg_small
    metrics_before:
    parse_wall_ms: 157003
    peak_rss_mb: 578
    metrics_after_pbf_input:
    parse_wall_ms: ~12487–13292
    peak_rss_mb: ~824–848
    metrics_after_filtered_pbf_as_input:
    parse_wall_ms: ~3027
    peak_rss_mb: ~535
    notes: PBF streaming ingestion achieves ~12–13s parse time vs 157s baseline (≈12× faster). Initial RSS rose to ~1.26GB due to full node indexing; reduced to ~0.82–0.85GB with only-index-needed-nodes and earlier frees. Using the filtered PBF as input yields ~3s ingestion and ~-43MB vs baseline RSS. Matching time differences (~+0.07–0.45s) are within noise.
    flags_used:
    osm_format: auto
    filter_output_format: pbf
    relation_mode: off
    per_mot_graphs: false
    cache_stop_pairs: false
    shape_simplification: off
    fixes_and_learnings:
  - getopt long-option collisions fixed by unique enum codes.
  - libosmium requires modern C++; moved to C++17.
  - Expat must be linked for libosmium XML reader paths (symbols XML\_\*).
  - osmium::io::Writer must be passed by reference to osmium::apply; close writer explicitly.
  - mmap index backends (Sparse/Dense) may be unavailable on Homebrew; fallback to FlexMem for portability.
  - Avoid double .osm extension when converting from .osm.pbf.
    known_gaps:
  - -X filter still routes through XML intermediate; streaming PBF filter writer would remove both conversions.
  - Ingestion metrics currently aggregate GTFS+OSM; consider splitting parse_wall_ms into gtfs_parse_ms and osm_parse_ms.
    next_actions:
  - Implement native streaming filter writer (PBF→PBF) mirroring OsmBuilder::filterWrite semantics; eliminate temp XML and conversions.
  - Add pass-level timing logs (nodes, rels, ways scan, nodes+ways) for future tuning.
  - Optional: CMake switch to force index backend when available (auto|flex|dense|sparse); keep FlexMem default.
