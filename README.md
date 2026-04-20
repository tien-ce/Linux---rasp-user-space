# Linux User-Space Examples — Raspberry Pi 4

User-space example programs for interacting with Linux kernel modules developed in [kernel-modulle-raspi4](https://github.com/tien-ce/kernel-modulle-raspi4).

## Projects

| Project | Description | Kernel companion |
|---------|-------------|-----------------|
| [01_cli_and_shellscript](01_cli_and_shellscript) | CLI tools and shell scripting basics | — |
| [02_vfs](02_vfs) | Virtual filesystem interaction | — |
| [03_file_operation](03_file_operation) | File operations via system calls | — |
| [04_write_dev_file](04_write_dev_file) | Writing to device files | — |
| [05_device_modbus](05_device_modbus) | CO/PM sensor client over Modbus RTU | [17_modbus_rtu](https://github.com/tien-ce/kernel-modulle-raspi4/tree/main/17_modbus_rtu) |

## Target

- Board: Raspberry Pi 4
- Architecture: ARM64 (`aarch64`)
- Cross-compiler: `aarch64-linux-gnu-gcc`

## Author

Văn Tiến — tien11102004@gmail.com
