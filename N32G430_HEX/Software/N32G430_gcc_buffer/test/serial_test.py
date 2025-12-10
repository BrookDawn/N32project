#!/usr/bin/env python3
"""
Serial port test script for USART1
Tests basic send/receive functionality
"""

import serial
import time
import sys

def test_serial(port='COM3', baudrate=115200, timeout=2):
    """Test serial port communication"""
    print(f"Opening {port} at {baudrate} baud...")

    try:
        ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=timeout
        )

        print(f"Port {port} opened successfully")

        # Clear buffers
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        # Test 1: Send test data and check for echo or response
        test_data = b"Hello USART1!\r\n"
        print(f"\nSending: {test_data}")
        ser.write(test_data)

        # Wait for response
        time.sleep(0.5)

        # Read any available data
        if ser.in_waiting > 0:
            response = ser.read(ser.in_waiting)
            print(f"Received: {response}")
            print(f"Hex: {response.hex()}")
        else:
            print("No response received (this may be expected if no echo is implemented)")

        # Test 2: Send hex bytes
        test_hex = bytes([0x01, 0x02, 0x03, 0x04, 0x05])
        print(f"\nSending hex: {test_hex.hex()}")
        ser.write(test_hex)

        time.sleep(0.5)

        if ser.in_waiting > 0:
            response = ser.read(ser.in_waiting)
            print(f"Received: {response.hex()}")
        else:
            print("No response received")

        # Monitor incoming data for a few seconds
        print("\nMonitoring for 3 seconds...")
        start_time = time.time()
        while time.time() - start_time < 3:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                print(f"Received: {data} (hex: {data.hex()})")
            time.sleep(0.1)

        ser.close()
        print(f"\nPort {port} closed")
        return True

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return False
    except Exception as e:
        print(f"Error: {e}")
        return False

if __name__ == "__main__":
    port = sys.argv[1] if len(sys.argv) > 1 else "COM3"
    baudrate = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
    test_serial(port, baudrate)
