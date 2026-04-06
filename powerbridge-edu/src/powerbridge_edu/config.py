from __future__ import annotations

from dataclasses import dataclass
import json
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
    )
