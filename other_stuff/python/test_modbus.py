import serial

# Configuration
PORT = '/dev/ttyUSB1'
BAUDRATE = 115200
# Modbus RTU Frame: [Addr][Func][Data...][CRC_L][CRC_H]
frame = bytearray([0x01, 0x03,0x02, 0x10,0x10,0xB4, 0x48])

try:
    ser = serial.Serial(
        port=PORT,
        baudrate=BAUDRATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
    )
    
    ser.write(frame)
    print(f"Successfully sent {len(frame)} bytes to {PORT}: {frame.hex(' ')}")
    ser.close()

except Exception as e:
    print(f"Failed to send: {e}")