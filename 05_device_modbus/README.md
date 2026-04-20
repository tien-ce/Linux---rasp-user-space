# User-Space Modbus Sensor Client

User-space application for reading CO/PM sensor data via the Modbus RTU kernel driver ([`17_modbus_rtu`](https://github.com/tien-ce/kernel-modulle-raspi4/tree/main/17_modbus_rtu)).

## Overview

This application acts as a thin client to the kernel Modbus RTU driver. It:
1. Configures kernel-side parameters (timeout, sampling interval) by writing control frames to the character device.
2. Polls CO sensor readings from `/dev/co_sensor` in a 1-second loop.

All Modbus RTU protocol handling (frame construction, CRC, RS485 timing) runs in kernel space. This application only interacts with the exposed character devices.

## System Architecture

```
main_app (this repo)
    │  open / read / write
    ▼
/dev/co_sensor  /dev/pm_sensor        ← character devices (kernel)
    │
    ▼
modbus_device_module.ko               ← platform driver, caching layer
    │
    ▼
modbus_controller_module.ko           ← Modbus RTU FSM, SerDev UART2
    │
    ▼
RS485 transceiver → Modbus bus
    │
    ├── CO sensor  (slave 0x24)
    └── PM sensor  (slave 0x25)
```

## Prerequisites

### Build host

- Cross-compiler: `aarch64-linux-gnu-gcc` (ARM64)

  ```bash
  sudo apt install gcc-aarch64-linux-gnu
  ```

### Target (Raspberry Pi 4)

- Kernel modules loaded in order:

  ```bash
  sudo insmod modbus_controller_module.ko
  sudo insmod modbus_device_module.ko
  ```

- Device tree overlay applied:

  ```bash
  sudo dtoverlay rs485_overlay.dtbo
  ```

- Character devices present:

  ```
  /dev/co_sensor
  /dev/pm_sensor
  ```

Refer to the kernel module [README](https://github.com/tien-ce/kernel-modulle-raspi4/tree/main/17_modbus_rtu) for full hardware setup.

## Build

```bash
make          # cross-compile → main_app
make clean    # remove build artifacts
make install  # push binary to Raspberry Pi via push_user_rpi
```

The cross-compiler and target architecture are configured in the Makefile:

```makefile
ARCH          = arm64
CROSS_COMPILE = aarch64-linux-gnu-
```

## Usage

Copy `main_app` to the Raspberry Pi and run:

```bash
./main_app
```

Expected output:

```
Success: Config 0x01 wrote 5 bytes to /dev/co_sensor
Success: Config 0x02 wrote 5 bytes to /dev/co_sensor
Read CO value: 312
Read CO value: 315
...
```

The application exits with a non-zero code if the initial configuration writes fail (e.g., device not present or kernel module not loaded).

## Configuration

Parameters are sent to the kernel driver as 5-byte control frames at startup.

| Parameter | Function Code | Default  | Description |
|-----------|---------------|----------|-------------|
| Timeout   | `0x01`        | 100 ms   | Modbus response wait timeout |
| Interval  | `0x02`        | 1000 ms  | Minimum re-query interval (kernel cache TTL) |

> **Note on interval:** The kernel driver caches the last sensor reading and only re-issues a Modbus query after `interval` milliseconds. If multiple processes read the device simultaneously, they share this cache. Setting a very low interval increases RS485 bus load.

## Control Frame Format

Writes to the device use a 5-byte little-endian frame:

```
Byte 0     : function code (0x01 = timeout, 0x02 = interval)
Bytes 1–4  : 32-bit unsigned value, little-endian (LSB first)
```

Example — set timeout to 100 ms (`0x00000064`):

```
{0x01, 0x64, 0x00, 0x00, 0x00}
```

## File Structure

```
05_device_modbus/
├── main.c      — application entry point and sensor polling loop
├── config.h    — device paths, function codes, constants
└── Makefile    — cross-compile and install targets
```

## License

GPL v2 — consistent with the kernel module this application targets.

## Author

Văn Tiến — tien11102004@gmail.com
