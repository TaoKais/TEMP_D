from __future__ import annotations

import csv
from pathlib import Path
from urllib import request

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


class InfluxSink:
    def __init__(
        self,
        *,
        url: str,
        org: str,
        bucket: str,
        token: str,
        measurement: str,
    ) -> None:
        self.endpoint = f"{url.rstrip('/')}/api/v2/write?org={org}&bucket={bucket}&precision=ns"
        self.token = token
        self.measurement = measurement

    def write(self, event: TelemetryEvent) -> None:
        line = (
            f"{self.measurement},"
            f"site={_escape_tag(event.site_name)},"
            f"source={_escape_tag(event.source_name)},"
            f"metric={_escape_tag(event.metric_name)},"
            f"phase={_escape_tag(event.phase)},"
            f"unit={_escape_tag(event.unit)} "
            f"value={event.value} {_iso_to_ns(event.timestamp)}"
        ).encode("utf-8")
        req = request.Request(
            self.endpoint,
            data=line,
            method="POST",
            headers={
                "Authorization": f"Token {self.token}",
                "Content-Type": "text/plain; charset=utf-8",
            },
        )
        with request.urlopen(req, timeout=5):
            return


def _escape_tag(value: str) -> str:
    return value.replace("\\", "\\\\").replace(",", "\\,").replace(" ", "\\ ").replace("=", "\\=")


def _iso_to_ns(value: str) -> int:
    normalized = value.replace("Z", "+00:00")
    from datetime import datetime

    dt = datetime.fromisoformat(normalized)
    return int(dt.timestamp() * 1_000_000_000)
