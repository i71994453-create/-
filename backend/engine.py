"""Validated Python-to-C++ bridge for GPAI calculations."""

from __future__ import annotations

import ctypes
import json
import math
import os
from pathlib import Path
from typing import Any

CATALOG_PATH = Path(__file__).with_name("calculations.json")
with CATALOG_PATH.open(encoding="utf-8") as catalog_file:
    CATALOG = json.load(catalog_file)
OPERATIONS = {item["id"]: item for item in CATALOG}

_library_path = os.environ.get("GPAI_LIBRARY", "/app/libgpai_core.so")
_library = ctypes.CDLL(_library_path)
_calculate = _library.gpai_calculate
_calculate.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t,
                       ctypes.POINTER(ctypes.c_double), ctypes.c_size_t]
_calculate.restype = ctypes.c_int


class CalculationError(ValueError):
    """Invalid calculation request or a C++ status-code failure."""


def calculate(operation_id: str, arguments: dict[str, Any]) -> dict[str, Any]:
    operation = OPERATIONS.get(operation_id)
    if operation is None:
        raise CalculationError("허용되지 않은 계산 항목입니다.")
    if not isinstance(arguments, dict):
        raise CalculationError("계산 입력은 객체여야 합니다.")
    fields = operation["inputs"]
    names = [field[0] for field in fields]
    if set(arguments) != set(names):
        missing = sorted(set(names) - set(arguments))
        extra = sorted(set(arguments) - set(names))
        details = []
        if missing:
            details.append("누락된 값: " + ", ".join(missing))
        if extra:
            details.append("불필요한 값: " + ", ".join(extra))
        raise CalculationError("입력 항목이 맞지 않습니다. " + "; ".join(details))

    values: list[float] = []
    for name in names:
        value = arguments[name]
        if isinstance(value, bool) or not isinstance(value, (int, float)):
            raise CalculationError(f"{name}에는 숫자가 필요합니다.")
        number = float(value)
        if not math.isfinite(number):
            raise CalculationError(f"{name}에는 유한한 숫자가 필요합니다.")
        values.append(number)

    input_array = (ctypes.c_double * len(values))(*values)
    output_count = len(operation["outputs"])
    output_array = (ctypes.c_double * output_count)()
    status = _calculate(operation_id.encode("ascii"), input_array, len(values),
                        output_array, output_count)
    if status != 0:
        reason = {1: "입력 조건을 확인하세요.", 2: "계산 결과가 숫자 범위를 벗어났습니다.",
                  3: "C++ 계산 코어 내부 오류입니다."}.get(
                      status, f"알 수 없는 C++ 상태 코드 {status}")
        raise CalculationError(reason)

    results = []
    for (label, unit), value in zip(operation["outputs"], output_array):
        if not math.isfinite(value):
            raise CalculationError("C++ 계산 결과가 유한한 숫자가 아닙니다.")
        results.append({"label": label, "unit": unit, "value": value})
    return {"operation": operation_id, "name": operation["name"],
            "formula": operation["formula"], "outputs": results}
