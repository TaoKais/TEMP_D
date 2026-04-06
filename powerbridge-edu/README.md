# PowerBridge EDU

PowerBridge EDU is a teaching-ready industrial edge gateway scaffold inspired by `TEMP_D`.

It is designed to bridge legacy Modbus-style electrical measurements into a modern observability flow:

- collect device readings
- map raw registers into named metrics
- write telemetry to sinks such as CSV or stdout
- support simulation mode for demos and labs

This local scaffold does not modify the original `TEMP_D` repository. It gives you a clean place to evolve the project before deciding what belongs in the real repo.

## Why this exists

The original thesis project already established a strong direction:

- Modbus TCP acquisition
- metric mapping by phase
- CSV export
- InfluxDB/Grafana persistence

This rewrite starts by separating those responsibilities so the project can become both:

- a practical edge gateway for legacy electrical equipment
- an educational platform for Industry 4.0 and observability labs

## Current scope

The first version included here is intentionally small and runnable:

- `simulator` generates realistic three-phase electrical measurements
- `collector` supports simulator mode and direct Modbus TCP reads
- `mapping` converts register-oriented data into telemetry events
- `sinks` write events to stdout and CSV
- `config` is externalized in JSON

InfluxDB, Grafana, and MQTT can be added next without changing the overall structure.

## Project layout

```text
powerbridge-edu/
  config/
    demo-config.json
  data/
  src/powerbridge_edu/
    __init__.py
    __main__.py
    collector.py
    config.py
    main.py
    mapping.py
    models.py
    simulator.py
    sinks.py
  pyproject.toml
  README.md
```

## Run the demo

From `C:\Users\Kdt_T\powerbridge-edu`:

```powershell
python -m powerbridge_edu --config config/demo-config.json --cycles 3
```

If `PYTHONPATH` is not set, use:

```powershell
$env:PYTHONPATH = "src"
python -m powerbridge_edu --config config/demo-config.json --cycles 3
```

The demo writes:

- structured output to the terminal
- CSV output to `data/telemetry.csv`

## Run the browser demo

From `C:\Users\Kdt_T\powerbridge-edu`:

```powershell
$env:PYTHONPATH = "src"
python -m powerbridge_edu --config config/demo-config.json --serve --port 8010
```

Then open:

```text
http://127.0.0.1:8010
```

The page refreshes automatically and shows live simulated telemetry by metric and phase.

## Run with a real Modbus TCP device

A sample config is included at `config/modbus-config.json`.

From `C:\Users\Kdt_T\powerbridge-edu`:

```powershell
$env:PYTHONPATH = "src"
python -m powerbridge_edu --config config/modbus-config.json --cycles 1
```

Or start the browser view against the live device:

```powershell
$env:PYTHONPATH = "src"
python -m powerbridge_edu --config config/modbus-config.json --serve --port 8010
```

The Modbus collector uses function code `03` to read holding registers over TCP and decodes two-register IEEE-754 floats.

## Configuration

The main config fields are:

- `source_name`: `simulator` or `modbus`
- `modbus_host`: device IP or hostname
- `modbus_port`: usually `502`
- `modbus_unit_id`: Modbus slave id
- `register_map`: list of metric-to-address mappings

Each register map entry looks like:

```json
{
  "register_key": "voltage",
  "phase": "L1",
  "address": 81,
  "word_order": "abcd"
}
```

Supported `word_order` values:

- `abcd`
- `badc`
- `cdab`
- `dcba`

## Near-term roadmap

1. Add an InfluxDB sink and line protocol output.
2. Add alarm rules for imbalance, power factor, and harmonic thresholds.
3. Add a small REST API or MQTT publisher for classroom demos.
4. Package dashboards and sample datasets.
5. Add batch polling and retry logic for noisy field networks.
