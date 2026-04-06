from __future__ import annotations

from .models import RawSample, TelemetryEvent


REGISTER_MAP: dict[str, tuple[str, str]] = {
    "voltage": ("voltage", "V"),
    "current": ("current", "A"),
    "active_power": ("active_power", "W"),
    "apparent_power": ("apparent_power", "VA"),
    "reactive_power": ("reactive_power", "VAR"),
    "power_factor": ("power_factor", "ratio"),
    "frequency": ("frequency", "Hz"),
}


def map_sample(
    sample: RawSample,
    *,
    timestamp: str,
    site_name: str,
    source_name: str,
) -> TelemetryEvent:
    metric_name, unit = REGISTER_MAP[sample.register_key]
    return TelemetryEvent(
        timestamp=timestamp,
        site_name=site_name,
        source_name=source_name,
        metric_name=metric_name,
        phase=sample.phase,
        unit=unit,
        value=sample.value,
    )
