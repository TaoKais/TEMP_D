# TEMP_D

`TEMP_D` contains the original master's thesis project and a new rewrite scaffold for its next iteration.

## Repository structure

- Legacy C/Visual Studio thesis code remains at the repository root.
- The new Python-based edge gateway scaffold lives in `powerbridge-edu/`.

## What the new scaffold is

`powerbridge-edu/` is a teaching-ready industrial edge gateway prototype inspired by the original thesis work.

It currently supports:

- simulator-based telemetry generation
- direct Modbus TCP collection
- metric mapping by phase
- CSV output
- a browser demo with a simple live dashboard

## Start with the new scaffold

See:

- `powerbridge-edu/README.md`
- `powerbridge-edu/config/demo-config.json`
- `powerbridge-edu/config/modbus-config.json`

## Run the demo

From the project folder:

```powershell
cd powerbridge-edu
$env:PYTHONPATH='src'
python -m powerbridge_edu --config config/demo-config.json --serve --port 8010
```

Then open `http://127.0.0.1:8010`.

## Goal

The intention is to preserve the original thesis implementation while building a cleaner, more reusable version oriented toward:

- industrial telemetry gateways
- legacy Modbus integration
- educational Industry 4.0 labs
- future InfluxDB/Grafana integration
