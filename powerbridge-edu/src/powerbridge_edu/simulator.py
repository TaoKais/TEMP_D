from __future__ import annotations

from datetime import datetime, UTC
import random

from .models import RawSample


PHASE_OFFSETS = {
    "L1": 0.6,
    "L2": -0.4,
    "L3": 0.2,
    "N": -0.1,
}


def build_demo_samples(phases: list[str], enabled_metrics: list[str]) -> tuple[str, list[RawSample]]:
    timestamp = datetime.now(UTC).isoformat()
    samples: list[RawSample] = []

    for phase in phases:
        offset = PHASE_OFFSETS.get(phase, 0.0)
        for metric in enabled_metrics:
            samples.append(
                RawSample(
                    register_key=metric,
                    phase=phase,
                    value=_generate_value(metric, offset),
                )
            )

    return timestamp, samples


def _generate_value(metric: str, offset: float) -> float:
    if metric == "voltage":
        return round(random.uniform(228.0, 235.0) + offset, 3)
    if metric == "current":
        return round(random.uniform(8.0, 22.0) + offset, 3)
    if metric == "active_power":
        return round(random.uniform(1800.0, 4200.0) + 10 * offset, 3)
    if metric == "apparent_power":
        return round(random.uniform(2000.0, 4500.0) + 10 * offset, 3)
    if metric == "reactive_power":
        return round(random.uniform(150.0, 700.0) + 5 * offset, 3)
    if metric == "power_factor":
        return round(random.uniform(0.88, 0.99), 4)
    if metric == "frequency":
        return round(random.uniform(49.95, 50.05), 3)
    raise ValueError(f"Unsupported metric: {metric}")
