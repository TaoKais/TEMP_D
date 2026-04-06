from __future__ import annotations

import csv
from pathlib import Path

from .models import TelemetryEvent


CSV_HEADERS = [
    "timestamp",
    "site_name",
    "source_name",
    "metric_name",
    "phase",
    "unit",
    "value",
]


class CsvSink:
    def __init__(self, csv_path: str) -> None:
        self.path = Path(csv_path)
        self.path.parent.mkdir(parents=True, exist_ok=True)
        if not self.path.exists():
            with self.path.open("w", newline="", encoding="utf-8") as handle:
                writer = csv.writer(handle)
                writer.writerow(CSV_HEADERS)

    def write(self, event: TelemetryEvent) -> None:
        with self.path.open("a", newline="", encoding="utf-8") as handle:
            writer = csv.writer(handle)
            writer.writerow(
                [
                    event.timestamp,
                    event.site_name,
                    event.source_name,
                    event.metric_name,
                    event.phase,
                    event.unit,
                    event.value,
                ]
            )


class StdoutSink:
    def write(self, event: TelemetryEvent) -> None:
        print(
            f"{event.timestamp} "
            f"site={event.site_name} "
            f"source={event.source_name} "
            f"metric={event.metric_name} "
            f"phase={event.phase} "
            f"value={event.value} {event.unit}"
        )
