import os
FILE_PATH = "/dev/co_sensor"
# Example: Writing hex 0x01 and 0x64 (decimal 100)
HEX_DATA = [0x01,0x64]
def write_hex_to_device(device_path, hex_values):
    try:
        # Convert list of integers to raw bytes
        raw_payload = bytes(hex_values)
        
        with open(device_path, 'wb') as f:
            written = f.write(raw_payload)
            
        print(f"Successfully wrote {written} bytes to {device_path}")
        
    except PermissionError:
        print("Error: Permission denied. Use sudo.")
    except FileNotFoundError:
        print(f"Error: Device {device_path} not found.")
    except OSError as e:
        # Captures kernel errors like -ENOMEM, -EFAULT, etc.
        print(f"Kernel Error: {e.strerror}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    write_hex_to_device(FILE_PATH, HEX_DATA)
