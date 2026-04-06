from __future__ import annotations

import argparse
from pathlib import Path
import time

from .collector import collect_samples
from .config import load_config
from .mapping import map_sample
from .server import run_server
from .sinks import CsvSink, InfluxSink, StdoutSink


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="PowerBridge EDU demo runner")
    parser.add_argument("--config", default="config/demo-config.json")
    parser.add_argument("--cycles", type=int, default=1)
    parser.add_argument("--serve", action="store_true")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8010)
    parser.add_argument("--forever", action="store_true")
    parser.add_argument("--interval-seconds", type=float)
    return parser


def run(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    config = load_config(args.config)

    if args.serve:
        run_server(config=config, host=args.host, port=args.port)
        return 0

    csv_sink = CsvSink(_resolve_csv_path(args.config, config.csv_path))
    stdout_sink = StdoutSink()
    sinks = [stdout_sink, csv_sink]
    if config.influx_enabled:
        sinks.append(
            InfluxSink(
                url=_required("influx_url", config.influx_url),
                org=_required("influx_org", config.influx_org),
                bucket=_required("influx_bucket", config.influx_bucket),
                token=_required("influx_token", config.influx_token),
                measurement=config.influx_measurement,
            )
        )

    if args.forever:
        interval_seconds = args.interval_seconds if args.interval_seconds is not None else config.polling_interval_seconds
        while True:
            _run_collection_cycle(config, sinks)
            time.sleep(interval_seconds)
    else:
        for _ in range(args.cycles):
            _run_collection_cycle(config, sinks)

    return 0


def _resolve_csv_path(config_path: str, csv_path: str) -> str:
    config_dir = Path(config_path).resolve().parent
    return str((config_dir.parent / csv_path).resolve())


def _run_collection_cycle(config, sinks) -> None:
    timestamp, raw_samples = collect_samples(config)
    for sample in raw_samples:
        event = map_sample(
            sample,
            timestamp=timestamp,
            site_name=config.site_name,
            source_name=config.source_name,
        )
        for sink in sinks:
            sink.write(event)


def _required(name: str, value: str | None) -> str:
    if value:
        return value
    raise ValueError(f"{name} is required when InfluxDB output is enabled")
