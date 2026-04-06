from __future__ import annotations

from dataclasses import dataclass
import json
import os
from pathlib import Path

from .models import RegisterSpec


@dataclass(frozen=True)
class AppConfig:
    site_name: str
    source_name: str
    csv_path: str
    phases: list[str]
    enabled_metrics: list[str]
    modbus_host: str | None
    modbus_port: int
    modbus_unit_id: int
    modbus_timeout_seconds: float
    register_map: list[RegisterSpec]
    polling_interval_seconds: float
    influx_enabled: bool
    influx_url: str | None
    influx_org: str | None
    influx_bucket: str | None
    influx_token: str | None
    influx_measurement: str


def load_config(path: str) -> AppConfig:
    data = json.loads(Path(path).read_text(encoding="utf-8"))
    return AppConfig(
        site_name=data["site_name"],
        source_name=data["source_name"],
        csv_path=data["csv_path"],
        phases=list(data["phases"]),
        enabled_metrics=list(data["enabled_metrics"]),
        modbus_host=data.get("modbus_host"),
        modbus_port=int(data.get("modbus_port", 502)),
        modbus_unit_id=int(data.get("modbus_unit_id", 1)),
        modbus_timeout_seconds=float(data.get("modbus_timeout_seconds", 3.0)),
        register_map=[
            RegisterSpec(
                register_key=item["register_key"],
                phase=item["phase"],
                address=int(item["address"]),
                word_order=item.get("word_order", "abcd"),
            )
            for item in data.get("register_map", [])
        ],
        polling_interval_seconds=float(data.get("polling_interval_seconds", 5.0)),
        influx_enabled=_env_or_value("POWERBRIDGE_INFLUX_ENABLED", data.get("influx_enabled", False)).lower() in {"1", "true", "yes", "on"},
        influx_url=_env_or_value("POWERBRIDGE_INFLUX_URL", data.get("influx_url")),
        influx_org=_env_or_value("POWERBRIDGE_INFLUX_ORG", data.get("influx_org")),
        influx_bucket=_env_or_value("POWERBRIDGE_INFLUX_BUCKET", data.get("influx_bucket")),
        influx_token=_env_or_value("POWERBRIDGE_INFLUX_TOKEN", data.get("influx_token")),
        influx_measurement=str(_env_or_value("POWERBRIDGE_INFLUX_MEASUREMENT", data.get("influx_measurement", "powerbridge"))),
    )


def _env_or_value(name: str, value: object) -> str:
    env_value = os.getenv(name)
    if env_value is not None:
        return env_value
    if value is None:
        return ""
    return str(value)
