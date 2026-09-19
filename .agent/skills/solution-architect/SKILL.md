---
name: solution-architect
description: >-
  Expertise of a Principal Solution Architect for IoT, Industrial, and Embedded Systems. Use when designing end-to-end architectures, defining communication protocols (Protobuf, MQTT, Modbus, CANopen, REST, WebSocket), cloud/edge integration, secure boot/OTA update workflows, security & cryptographic strategy, data contracts, and cross-system trade-off analysis.
---

# Principal Solution Architect Skill

## Core Competencies
- **End-to-End System Design**: Edge-to-Gateway-to-Cloud / Host PC topology, edge computing partitioning, offline buffering and fault-tolerant synchronization.
- **Protocol Engineering**: Fieldbus (Modbus RTU/TCP, CANopen, EtherCAT), Network/IoT (MQTT, CoAP, WebSockets, gRPC/Protobuf, CBOR, Packet serial framing with CRC/COBS).
- **Security & Cryptography**: Secure Boot, Root of Trust (RoT), Hardware Security Modules (HSM / Secure Elements e.g. ATECC608), TLS 1.3 / mTLS, AES-GCM, ECDSA, Secure Firmware Over-The-Air (FOTA) with dual-bank rollback.
- **Integration & Data Contracts**: Schema versioning, backward compatibility, telemetry streaming, command-and-control state synchronization.
- **Non-Functional Requirements (NFRs)**: Latency budgets, bandwidth constraints, MTBF/availability, scalability, compliance (IEC 62443, ISO 26262, UL).

## Solution Architecture Workflow
1. **Requirements & Constraints Analysis**:
   - Classify functional vs non-functional requirements (NFRs: Latency, Throughput, Reliability, Security, Cost).
   - Identify system boundaries, external dependencies, and network topologies (Wired RS-485/CAN/Ethernet vs Wireless BLE/Wi-Fi/Cellular).
2. **Protocol & Data Contract Specification**:
   - Select encoding format: Binary (Protobuf, CBOR) for constrained bandwidth/compute vs Text (JSON) for host ease.
   - Define framing, synchronization bytes, sequence numbers, ack/nack schemes, and error detection (CRC-16/32).
3. **Security Architecture**:
   - Establish authentication and authorization matrix across communication boundaries.
   - Design key management lifecycle: In-factory provisioning, session key derivation, zero-trust device identity.
   - Design dual-slot A/B FOTA with digital signature verification (RSA-3072 / ECDSA P-256) and automated rollback on boot failure.
4. **Resilience & Fault Handling**:
   - Design graceful degradation modes (Fallback to safe state upon communication loss).
   - Implement rate limiting, backoff-retry strategies, and circular telemetry buffers in non-volatile storage (SPI Flash/FRAM).
5. **Trade-Off & Decision Records (ADRs)**:
   - Formulate Architecture Decision Records documenting: Context, Decision, Consequences, Alternatives Considered.

