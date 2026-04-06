from __future__ import annotations

from collections import defaultdict
from dataclasses import asdict
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
from typing import Any

from .collector import collect_samples
from .config import AppConfig
from .mapping import map_sample


def run_server(*, config: AppConfig, host: str, port: int) -> None:
    handler = _build_handler(config)
    server = ThreadingHTTPServer((host, port), handler)
    print(f"PowerBridge EDU demo server running at http://{host}:{port}")
    server.serve_forever()


def _build_handler(config: AppConfig) -> type[BaseHTTPRequestHandler]:
    class DemoHandler(BaseHTTPRequestHandler):
        def do_GET(self) -> None:  # noqa: N802
            if self.path in ("/", "/index.html"):
                self._send_html(_html_page(config.site_name))
                return
            if self.path == "/api/telemetry":
                payload = _build_payload(config)
                self._send_json(payload)
                return
            self.send_error(404, "Not Found")

        def log_message(self, format: str, *args: Any) -> None:
            return

        def _send_html(self, body: str) -> None:
            encoded = body.encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(encoded)))
            self.end_headers()
            self.wfile.write(encoded)

        def _send_json(self, payload: dict[str, Any]) -> None:
            encoded = json.dumps(payload).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Cache-Control", "no-store")
            self.send_header("Content-Length", str(len(encoded)))
            self.end_headers()
            self.wfile.write(encoded)

    return DemoHandler


def _build_payload(config: AppConfig) -> dict[str, Any]:
    timestamp, raw_samples = collect_samples(config)
    events = [
        map_sample(
            sample,
            timestamp=timestamp,
            site_name=config.site_name,
            source_name=config.source_name,
        )
        for sample in raw_samples
    ]

    grouped: dict[str, dict[str, float]] = defaultdict(dict)
    units: dict[str, str] = {}
    for event in events:
        grouped[event.metric_name][event.phase] = event.value
        units[event.metric_name] = event.unit

    summary = []
    for metric_name in config.enabled_metrics:
        metric_group = grouped.get(metric_name, {})
        if not metric_group:
            continue
        values = list(metric_group.values())
        summary.append(
            {
                "metric_name": metric_name,
                "unit": units[metric_name],
                "min": round(min(values), 3),
                "max": round(max(values), 3),
                "avg": round(sum(values) / len(values), 3),
                "phases": metric_group,
            }
        )

    return {
        "timestamp": timestamp,
        "site_name": config.site_name,
        "source_name": config.source_name,
        "summary": summary,
        "events": [asdict(event) for event in events],
    }


def _html_page(site_name: str) -> str:
    return f"""<!DOCTYPE html>
<html lang=\"en\">
<head>
  <meta charset=\"utf-8\">
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
  <title>PowerBridge EDU Demo</title>
  <style>
    :root {{
      --bg: #f2eee3;
      --panel: #fffaf0;
      --ink: #132a13;
      --soft: #4f6f52;
      --accent: #d97706;
      --grid: #d8d0bf;
    }}
    * {{ box-sizing: border-box; }}
    body {{
      margin: 0;
      font-family: Georgia, \"Times New Roman\", serif;
      color: var(--ink);
      background:
        radial-gradient(circle at top left, #fce7b2, transparent 28%),
        linear-gradient(135deg, #f4f1e8, #e8efe3 55%, #dce6dc);
      min-height: 100vh;
    }}
    main {{
      max-width: 1100px;
      margin: 0 auto;
      padding: 32px 20px 48px;
    }}
    .hero {{
      display: grid;
      gap: 12px;
      margin-bottom: 24px;
    }}
    .eyebrow {{
      text-transform: uppercase;
      letter-spacing: 0.18em;
      font-size: 0.76rem;
      color: var(--soft);
    }}
    h1 {{
      margin: 0;
      font-size: clamp(2rem, 5vw, 4rem);
      line-height: 0.95;
    }}
    .meta {{
      display: flex;
      gap: 16px;
      flex-wrap: wrap;
      color: var(--soft);
    }}
    .grid {{
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
      gap: 16px;
    }}
    .card {{
      background: rgba(255, 250, 240, 0.86);
      border: 1px solid var(--grid);
      border-radius: 16px;
      padding: 18px;
      backdrop-filter: blur(8px);
      box-shadow: 0 10px 30px rgba(19, 42, 19, 0.07);
    }}
    .card h2 {{
      margin: 0 0 10px;
      font-size: 1.15rem;
    }}
    .reading {{
      display: grid;
      gap: 6px;
      font-size: 0.96rem;
    }}
    .reading strong {{
      font-size: 1.45rem;
      color: var(--accent);
      font-weight: 700;
    }}
    table {{
      width: 100%;
      border-collapse: collapse;
      margin-top: 24px;
      background: rgba(255, 250, 240, 0.88);
      border: 1px solid var(--grid);
      border-radius: 16px;
      overflow: hidden;
    }}
    th, td {{
      padding: 12px 10px;
      border-bottom: 1px solid var(--grid);
      text-align: left;
      font-size: 0.95rem;
    }}
    th {{
      background: rgba(79, 111, 82, 0.08);
    }}
    .pill {{
      display: inline-block;
      padding: 6px 10px;
      border-radius: 999px;
      background: rgba(217, 119, 6, 0.12);
      color: #8a4b08;
      font-size: 0.8rem;
    }}
    @media (max-width: 640px) {{
      table {{
        display: block;
        overflow-x: auto;
      }}
    }}
  </style>
</head>
<body>
  <main>
    <section class=\"hero\">
      <div class=\"eyebrow\">Industrial Edge Gateway Demo</div>
      <h1>PowerBridge EDU</h1>
      <div class=\"meta\">
        <span id=\"site\">{site_name}</span>
        <span class=\"pill\">Simulation mode</span>
        <span id=\"updated\">Waiting for data</span>
      </div>
    </section>
    <section class=\"grid\" id=\"cards\"></section>
    <table>
      <thead>
        <tr>
          <th>Metric</th>
          <th>Unit</th>
          <th>Min</th>
          <th>Avg</th>
          <th>Max</th>
          <th>L1</th>
          <th>L2</th>
          <th>L3</th>
          <th>N</th>
        </tr>
      </thead>
      <tbody id=\"rows\"></tbody>
    </table>
  </main>
  <script>
    async function refresh() {{
      const response = await fetch('/api/telemetry', {{ cache: 'no-store' }});
      const payload = await response.json();
      document.getElementById('site').textContent = payload.site_name + ' / ' + payload.source_name;
      document.getElementById('updated').textContent = 'Updated ' + new Date(payload.timestamp).toLocaleString();

      const cards = document.getElementById('cards');
      cards.innerHTML = payload.summary.slice(0, 4).map(item => `
        <article class=\"card\">
          <h2>${{item.metric_name.replaceAll('_', ' ')}}</h2>
          <div class=\"reading\">
            <span>Average</span>
            <strong>${{item.avg}} ${{item.unit}}</strong>
            <span>Range: ${{item.min}} to ${{item.max}}</span>
          </div>
        </article>
      `).join('');

      const rows = document.getElementById('rows');
      rows.innerHTML = payload.summary.map(item => `
        <tr>
          <td>${{item.metric_name}}</td>
          <td>${{item.unit}}</td>
          <td>${{item.min}}</td>
          <td>${{item.avg}}</td>
          <td>${{item.max}}</td>
          <td>${{item.phases.L1 ?? ''}}</td>
          <td>${{item.phases.L2 ?? ''}}</td>
          <td>${{item.phases.L3 ?? ''}}</td>
          <td>${{item.phases.N ?? ''}}</td>
        </tr>
      `).join('');
    }}

    refresh();
    setInterval(refresh, 2000);
  </script>
</body>
</html>
"""
