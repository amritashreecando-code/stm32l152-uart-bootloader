import serial, struct

ser = serial.Serial('COM9', 9600, timeout=5)    # your ST-Link VCP COM port

ser.reset_input_buffer()                        # throw away any old, unread bytes

# full path to the .bin made by CubeIDE (change it to your real path)
data = open(r'C:\Users\DELL\Documents\stm32_bare_metal\led_app\Debug\led_app.bin', 'rb').read()

data += b'\xff' * ((4 - len(data) % 4) % 4)     # pad to a multiple of 4 bytes

def wait_ack():
    b = ser.read(1)                             # wait for one byte (up to 5 s)
    if b != b'A':
        raise RuntimeError(f"expected 'A', got {b!r}")

ser.write(struct.pack('<I', len(data)))         # 4-byte header, little-endian
wait_ack()                                      # wait: erase done

for i in range(0, len(data), 4):
    ser.write(data[i:i+4])                      # send one word (4 bytes)
    wait_ack()                                  # wait: word written

print("All", len(data), "bytes sent and written.")