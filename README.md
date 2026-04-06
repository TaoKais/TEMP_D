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
- CSV output
- InfluxDB line protocol writes
- a browser demo with a simple live dashboard
- a local Docker deployment with InfluxDB and Grafana

## Test it from your own GitHub checkout

```powershell
git clone https://github.com/TaoKais/TEMP_D.git
cd TEMP_D\powerbridge-edu
docker compose up --build
```

Then open:

- InfluxDB: `http://localhost:8086`
- Grafana: `http://localhost:3000`

Default local credentials:

- InfluxDB username: `admin`
- InfluxDB password: `adminadmin`
- Grafana username: `admin`
- Grafana password: `adminadmin`

## Start with the new scaffold

See:

- `powerbridge-edu/README.md`
- `powerbridge-edu/config/demo-config.json`
- `powerbridge-edu/config/modbus-config.json`
- `powerbridge-edu/config/deployment-config.json`

## Goal

The intention is to preserve the original thesis implementation while building a cleaner, more reusable version oriented toward:

- industrial telemetry gateways
- legacy Modbus integration
- educational Industry 4.0 labs
- future InfluxDB/Grafana integration
