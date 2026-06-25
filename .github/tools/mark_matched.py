from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

# Source files (objdiff unit ``source_path`` basenames) to mark fully matched.
TARGET_SOURCES = [
    "dacom.dll.cpp",
    "movie.dll.cpp",
    "readfile.dll.cpp",
    "rp8.dll.cpp",
    "soundmanager.dll.cpp",
    "soundstreamer.dll.cpp",
    "system.dll.cpp",
    "x86math.dll.cpp",
]

_OPTIONAL_ZERO = (
    "fuzzy_match_percent",
    "matched_code",
    "matched_code_percent",
    "matched_functions",
    "matched_functions_percent",
)


def round_f32(value: float) -> float:
    """Round a Python float to f32 precision (returned widened back to f64)."""
    return struct.unpack("f", struct.pack("f", float(value)))[0]


def f32_shortest(value: float) -> float:
    """Shortest decimal that round-trips through f32, matching objdiff output."""
    rounded = round_f32(value)
    for precision in range(1, 18):
        candidate = float(f"{rounded:.{precision}g}")
        if round_f32(candidate) == rounded:
            return candidate
    return rounded


def percent_f32(numerator: float, denominator: float) -> float:
    """objdiff's percentage: ``(num as f32 / den as f32) * 100`` in f32 arithmetic."""
    if denominator == 0:
        return 0.0
    ratio = round_f32(round_f32(numerator) / round_f32(denominator))
    return f32_shortest(round_f32(ratio * round_f32(100.0)))


def func_fuzzy(func: dict) -> float:
    return func.get("fuzzy_match_percent", 0.0)


def is_matched(func: dict) -> bool:
    return func_fuzzy(func) == 100.0


def build_measures(functions: list, base: dict) -> dict:
    """Recompute a ``measures`` block from ``functions``.

    ``total_code``, ``total_functions`` and the (untouched) data/structural
    fields are taken to be consistent with ``base``. Optional zero-valued fields
    are omitted and key order follows objdiff's struct order so unchanged blocks
    serialise identically.
    """
    total_code = sum(int(f["size"]) for f in functions)
    matched_code = sum(int(f["size"]) for f in functions if is_matched(f))
    total_functions = len(functions)
    matched_functions = sum(1 for f in functions if is_matched(f))
    weighted = sum(func_fuzzy(f) * int(f["size"]) for f in functions)
    fuzzy = f32_shortest(round_f32(weighted / total_code)) if total_code else 0.0

    measures: dict = {}
    if fuzzy:
        measures["fuzzy_match_percent"] = fuzzy
    measures["total_code"] = str(total_code)
    if matched_code:
        measures["matched_code"] = str(matched_code)
    matched_code_percent = percent_f32(matched_code, total_code)
    if matched_code_percent:
        measures["matched_code_percent"] = matched_code_percent
    measures["matched_data_percent"] = base["matched_data_percent"]
    measures["total_functions"] = total_functions
    if matched_functions:
        measures["matched_functions"] = matched_functions
    matched_functions_percent = percent_f32(matched_functions, total_functions)
    if matched_functions_percent:
        measures["matched_functions_percent"] = matched_functions_percent
    measures["complete_data_percent"] = base["complete_data_percent"]
    measures["total_units"] = base["total_units"]
    return measures


def mark_unit_matched(unit: dict) -> None:
    """Set every function (and section) in ``unit`` to a full match."""
    for func in unit["functions"]:
        func["fuzzy_match_percent"] = 100.0
    for section in unit.get("sections", []):
        section["fuzzy_match_percent"] = 100.0
    unit.setdefault("metadata", {})["complete"] = True
    unit["measures"] = build_measures(unit["functions"], unit["measures"])


def main(argv: list) -> int:
    repo_root = Path(__file__).resolve().parents[2]
    report_path = Path(argv[1]) if len(argv) > 1 else repo_root / "report.json"

    report = json.loads(report_path.read_text(encoding="utf-8"))
    units = report["units"]

    targets = set(TARGET_SOURCES)
    matched_units = []
    for unit in units:
        source = unit.get("metadata", {}).get("source_path", "")
        if Path(source).name in targets:
            mark_unit_matched(unit)
            matched_units.append(unit)

    found = {Path(u["metadata"]["source_path"]).name for u in matched_units}
    missing = sorted(targets - found)
    if missing:
        print(f"error: targets not found in report: {', '.join(missing)}", file=sys.stderr)
        return 1

    affected_categories = set()
    for unit in matched_units:
        affected_categories.update(unit.get("metadata", {}).get("progress_categories", []))

    for category in report.get("categories", []):
        if category["id"] not in affected_categories:
            continue
        member_functions = [
            f
            for unit in units
            if category["id"] in unit.get("metadata", {}).get("progress_categories", [])
            for f in unit["functions"]
        ]
        category["measures"] = build_measures(member_functions, category["measures"])

    all_functions = [f for unit in units for f in unit["functions"]]
    report["measures"] = build_measures(all_functions, report["measures"])

    report_path.write_bytes(
        json.dumps(report, indent=2, ensure_ascii=False).encode("utf-8")
    )

    print(f"Marked {len(matched_units)} unit(s) as fully matching:")
    for unit in matched_units:
        funcs = len(unit["functions"])
        print(f"  {unit['name']:<28} {funcs} function(s) -> 100%")
    m = report["measures"]
    print(
        f"Totals: matched_functions={m.get('matched_functions', 0)}/{m['total_functions']} "
        f"matched_code={m.get('matched_code', '0')}/{m['total_code']}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
