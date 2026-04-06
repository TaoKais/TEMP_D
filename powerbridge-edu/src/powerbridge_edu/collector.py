from __future__ import annotations

from datetime import UTC, datetime
import socket
import struct

from .config import AppConfig
from .models import RawSample, RegisterSpec
from .simulator import build_demo_samples


def collect_samples(config: AppConfig) -> tuple[str, list[RawSample]]:
    if config.source_name == "simulator":
        return build_demo_samples(config.phases, config.enabled_metrics)
    if config.source_name == "modbus":
        return collect_modbus_samples(config)
    raise ValueError(f"Unsupported source_name: {config.source_name}")


def collect_modbus_samples(config: AppConfig) -> tuple[str, list[RawSample]]:
    if not config.modbus_host:
        raise ValueError("modbus_host is required when source_name is 'modbus'")
    if not config.register_map:
        raise ValueError("register_map is required when source_name is 'modbus'")

    timestamp = datetime.now(UTC).isoformat()
    registers = _read_register_block(
        host=config.modbus_host,
        port=config.modbus_port,
        unit_id=config.modbus_unit_id,
        start_address=min(spec.address for spec in config.register_map),
        count=(max(spec.address for spec in config.register_map) - min(spec.address for spec in config.register_map)) + 2,
        timeout_seconds=config.modbus_timeout_seconds,
    )
    block_start = min(spec.address for spec in config.register_map)

    samples = [
        RawSample(
            register_key=spec.register_key,
            phase=spec.phase,
            value=_decode_float_from_registers(
                registers=registers[spec.address - block_start : spec.address - block_start + 2],
                word_order=spec.word_order,
            ),
        )
        for spec in config.register_map
        if spec.register_key in config.enabled_metrics
    ]
    return timestamp, samples


def _read_register_block(
    *,
    host: str,
    port: int,
    unit_id: int,
    start_address: int,
    count: int,
    timeout_seconds: float,
) -> list[int]:
    transaction_id = 1
    protocol_id = 0
    function_code = 3
    pdu = struct.pack(">BHH", function_code, start_address, count)
    mbap = struct.pack(">HHHB", transaction_id, protocol_id, len(pdu) + 1, unit_id)

    with socket.create_connection((host, port), timeout=timeout_seconds) as sock:
        sock.sendall(mbap + pdu)
        header = _recv_exact(sock, 7)
        _, _, length, response_unit_id = struct.unpack(">HHHB", header)
        if response_unit_id != unit_id:
            raise ValueError(f"Unexpected Modbus unit id: {response_unit_id}")
        payload = _recv_exact(sock, length - 1)

    response_function = payload[0]
    if response_function & 0x80:
        raise ValueError(f"Modbus exception code: {payload[1]}")
    if response_function != function_code:
        raise ValueError(f"Unexpected function code: {response_function}")

    byte_count = payload[1]
    register_bytes = payload[2 : 2 + byte_count]
    if len(register_bytes) != count * 2:
        raise ValueError("Incomplete register payload received from Modbus device")
    return [struct.unpack(">H", register_bytes[index : index + 2])[0] for index in range(0, len(register_bytes), 2)]


def _recv_exact(sock: socket.socket, size: int) -> bytes:
    chunks = bytearray()
    while len(chunks) < size:
        chunk = sock.recv(size - len(chunks))
        if not chunk:
            raise ConnectionError("Socket closed before enough bytes were received")
        chunks.extend(chunk)
    return bytes(chunks)


def _decode_float_from_registers(*, registers: list[int], word_order: str) -> float:
    if len(registers) != 2:
        raise ValueError("Expected exactly two registers for float decoding")
    high = struct.pack(">H", registers[0])
    low = struct.pack(">H", registers[1])

    orders = {
        "abcd": high + low,
        "badc": high[1:2] + high[0:1] + low[1:2] + low[0:1],
        "cdab": low + high,
        "dcba": low[1:2] + low[0:1] + high[1:2] + high[0:1],
    }
    if word_order not in orders:
        raise ValueError(f"Unsupported word order: {word_order}")
    return round(struct.unpack(">f", orders[word_order])[0], 6)
