from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class RegisterSpec:
    register_key: str
    phase: str
    address: int
    word_order: str = "abcd"


@dataclass(frozen=True)
class RawSample:
    register_key: str
    phase: str
    value: float


@dataclass(frozen=True)
class TelemetryEvent:
    timestamp: str
    site_name: str
    source_name: str
    metric_name: str
    phase: str
    unit: str
    value: float
