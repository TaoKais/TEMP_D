FROM python:3.12-slim

WORKDIR /app

COPY pyproject.toml /app/pyproject.toml
COPY src /app/src
COPY config /app/config

ENV PYTHONPATH=/app/src

CMD ["python", "-m", "powerbridge_edu", "--config", "config/deployment-config.json", "--forever"]
