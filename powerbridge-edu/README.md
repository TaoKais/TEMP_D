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
- `sinks` write events to stdout, CSV, and InfluxDB
- `config` is externalized in JSON

InfluxDB, Grafana, and MQTT can be added next without changing the overall structure.

## Project layout

```text
powerbridge-edu/
  config/
    demo-config.json
    deployment-config.json
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
  Dockerfile
  docker-compose.yml
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

## Local deployment from GitHub

Clone your own repository copy:

```powershell
git clone https://github.com/TaoKais/TEMP_D.git
cd TEMP_D\powerbridge-edu
```

Start the local deployment stack:

```powershell
docker compose up --build
```

This launches:

- `collector` writing simulator telemetry every 5 seconds
- InfluxDB on `http://localhost:8086`
- Grafana on `http://localhost:3000`
- a provisioned Grafana datasource pointing at InfluxDB
- a provisioned dashboard named `PowerBridge Overview`

Default local credentials:

- InfluxDB username: `admin`
- InfluxDB password: `adminadmin`
- Grafana username: `admin`
- Grafana password: `adminadmin`

The compose deployment uses `config/deployment-config.json` and writes to the `telemetry` bucket in InfluxDB.

After Grafana starts, open the `PowerBridge / PowerBridge Overview` dashboard.

To stop it:

```powershell
docker compose down
```

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
- `influx_enabled`: enable or disable the InfluxDB sink
- `influx_url`: InfluxDB base URL
- `influx_org`: InfluxDB organization name
- `influx_bucket`: target bucket
- `influx_token`: API token
- `polling_interval_seconds`: delay between polling cycles in continuous mode

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

Environment variables can override the InfluxDB settings in deployment:

- `POWERBRIDGE_INFLUX_ENABLED`
- `POWERBRIDGE_INFLUX_URL`
- `POWERBRIDGE_INFLUX_ORG`
- `POWERBRIDGE_INFLUX_BUCKET`
- `POWERBRIDGE_INFLUX_TOKEN`
- `POWERBRIDGE_INFLUX_MEASUREMENT`

## Continuous collection

Run the collector continuously without Docker:

```powershell
$env:PYTHONPATH = "src"
python -m powerbridge_edu --config config/deployment-config.json --forever --interval-seconds 5
```

## Near-term roadmap

1. Add alarm rules for imbalance, power factor, and harmonic thresholds.
2. Add a small REST API or MQTT publisher for classroom demos.
3. Package dashboards and sample datasets.
4. Add batch polling and retry logic for noisy field networks.
5. Add alert rules and dashboard variables for device selection.
