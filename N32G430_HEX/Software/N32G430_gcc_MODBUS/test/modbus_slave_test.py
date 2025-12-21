#!/usr/bin/env python3
"""
Modbus RTU Slave Test Script
Tests the N32G430 Modbus slave implementation via COM3
"""

import serial
import struct
import time
import sys

def calc_crc16(data: bytes) -> int:
    """Calculate Modbus CRC16"""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc

def build_request(slave_addr: int, func_code: int, *args) -> bytes:
    """Build Modbus RTU request frame"""
    frame = bytes([slave_addr, func_code]) + bytes(args)
    crc = calc_crc16(frame)
    frame += struct.pack('<H', crc)
    return frame

def read_response(ser: serial.Serial, timeout: float = 0.5) -> bytes:
    """Read Modbus response with timeout"""
    time.sleep(0.1)
    end_time = time.time() + timeout
    response = b''
    while time.time() < end_time:
        if ser.in_waiting > 0:
            response += ser.read(ser.in_waiting)
            time.sleep(0.05)
        else:
            time.sleep(0.01)
    return response

def verify_crc(frame: bytes) -> bool:
    """Verify CRC of response frame"""
    if len(frame) < 4:
        return False
    crc_received = struct.unpack('<H', frame[-2:])[0]
    crc_calc = calc_crc16(frame[:-2])
    return crc_received == crc_calc

def test_read_holding_registers(ser: serial.Serial, slave_addr: int, start_addr: int, quantity: int):
    """Test Function Code 0x03: Read Holding Registers"""
    print(f"\n[TEST] Read Holding Registers (0x03): addr=0x{start_addr:04X}, qty={quantity}")

    request = build_request(slave_addr, 0x03,
                           (start_addr >> 8) & 0xFF, start_addr & 0xFF,
                           (quantity >> 8) & 0xFF, quantity & 0xFF)
    print(f"  TX: {request.hex()}")

    ser.reset_input_buffer()
    ser.write(request)

    response = read_response(ser)
    print(f"  RX: {response.hex()}" if response else "  RX: (no response)")

    if response and verify_crc(response):
        if response[1] == 0x03:
            byte_count = response[2]
            values = []
            for i in range(0, byte_count, 2):
                val = (response[3 + i] << 8) | response[3 + i + 1]
                values.append(val)
            print(f"  [OK] Values: {[f'0x{v:04X}' for v in values]}")
            return True
        elif response[1] == 0x83:
            print(f"  [EXCEPTION] Code: 0x{response[2]:02X}")
    else:
        print("  [FAIL] CRC error or no response")
    return False

def test_read_input_registers(ser: serial.Serial, slave_addr: int, start_addr: int, quantity: int):
    """Test Function Code 0x04: Read Input Registers"""
    print(f"\n[TEST] Read Input Registers (0x04): addr=0x{start_addr:04X}, qty={quantity}")

    request = build_request(slave_addr, 0x04,
                           (start_addr >> 8) & 0xFF, start_addr & 0xFF,
                           (quantity >> 8) & 0xFF, quantity & 0xFF)
    print(f"  TX: {request.hex()}")

    ser.reset_input_buffer()
    ser.write(request)

    response = read_response(ser)
    print(f"  RX: {response.hex()}" if response else "  RX: (no response)")

    if response and verify_crc(response):
        if response[1] == 0x04:
            byte_count = response[2]
            values = []
            for i in range(0, byte_count, 2):
                val = (response[3 + i] << 8) | response[3 + i + 1]
                values.append(val)
            print(f"  [OK] Values: {[f'0x{v:04X}' for v in values]}")
            return True
        elif response[1] == 0x84:
            print(f"  [EXCEPTION] Code: 0x{response[2]:02X}")
    else:
        print("  [FAIL] CRC error or no response")
    return False

def test_read_coils(ser: serial.Serial, slave_addr: int, start_addr: int, quantity: int):
    """Test Function Code 0x01: Read Coils"""
    print(f"\n[TEST] Read Coils (0x01): addr=0x{start_addr:04X}, qty={quantity}")

    request = build_request(slave_addr, 0x01,
                           (start_addr >> 8) & 0xFF, start_addr & 0xFF,
                           (quantity >> 8) & 0xFF, quantity & 0xFF)
    print(f"  TX: {request.hex()}")

    ser.reset_input_buffer()
    ser.write(request)

    response = read_response(ser)
    print(f"  RX: {response.hex()}" if response else "  RX: (no response)")

    if response and verify_crc(response):
        if response[1] == 0x01:
            byte_count = response[2]
            coils = []
            for i in range(quantity):
                byte_idx = i // 8
                bit_idx = i % 8
                coil_state = (response[3 + byte_idx] >> bit_idx) & 0x01
                coils.append(coil_state)
            print(f"  [OK] Coils: {coils}")
            return True
        elif response[1] == 0x81:
            print(f"  [EXCEPTION] Code: 0x{response[2]:02X}")
    else:
        print("  [FAIL] CRC error or no response")
    return False

def test_write_single_register(ser: serial.Serial, slave_addr: int, reg_addr: int, value: int):
    """Test Function Code 0x06: Write Single Register"""
    print(f"\n[TEST] Write Single Register (0x06): addr=0x{reg_addr:04X}, value=0x{value:04X}")

    request = build_request(slave_addr, 0x06,
                           (reg_addr >> 8) & 0xFF, reg_addr & 0xFF,
                           (value >> 8) & 0xFF, value & 0xFF)
    print(f"  TX: {request.hex()}")

    ser.reset_input_buffer()
    ser.write(request)

    response = read_response(ser)
    print(f"  RX: {response.hex()}" if response else "  RX: (no response)")

    if response and verify_crc(response):
        if response[1] == 0x06:
            resp_addr = (response[2] << 8) | response[3]
            resp_val = (response[4] << 8) | response[5]
            if resp_addr == reg_addr and resp_val == value:
                print(f"  [OK] Echo verified")
                return True
            else:
                print(f"  [FAIL] Echo mismatch")
        elif response[1] == 0x86:
            print(f"  [EXCEPTION] Code: 0x{response[2]:02X}")
    else:
        print("  [FAIL] CRC error or no response")
    return False

def test_write_multiple_registers(ser: serial.Serial, slave_addr: int, start_addr: int, values: list):
    """Test Function Code 0x10: Write Multiple Registers"""
    print(f"\n[TEST] Write Multiple Registers (0x10): addr=0x{start_addr:04X}, values={[f'0x{v:04X}' for v in values]}")

    quantity = len(values)
    byte_count = quantity * 2

    frame = bytes([slave_addr, 0x10,
                  (start_addr >> 8) & 0xFF, start_addr & 0xFF,
                  (quantity >> 8) & 0xFF, quantity & 0xFF,
                  byte_count])

    for val in values:
        frame += bytes([(val >> 8) & 0xFF, val & 0xFF])

    crc = calc_crc16(frame)
    frame += struct.pack('<H', crc)

    print(f"  TX: {frame.hex()}")

    ser.reset_input_buffer()
    ser.write(frame)

    response = read_response(ser)
    print(f"  RX: {response.hex()}" if response else "  RX: (no response)")

    if response and verify_crc(response):
        if response[1] == 0x10:
            resp_addr = (response[2] << 8) | response[3]
            resp_qty = (response[4] << 8) | response[5]
            if resp_addr == start_addr and resp_qty == quantity:
                print(f"  [OK] Write confirmed")
                return True
            else:
                print(f"  [FAIL] Response mismatch")
        elif response[1] == 0x90:
            print(f"  [EXCEPTION] Code: 0x{response[2]:02X}")
    else:
        print("  [FAIL] CRC error or no response")
    return False

def test_write_single_coil(ser: serial.Serial, slave_addr: int, coil_addr: int, state: bool):
    """Test Function Code 0x05: Write Single Coil"""
    value = 0xFF00 if state else 0x0000
    print(f"\n[TEST] Write Single Coil (0x05): addr=0x{coil_addr:04X}, state={'ON' if state else 'OFF'}")

    request = build_request(slave_addr, 0x05,
                           (coil_addr >> 8) & 0xFF, coil_addr & 0xFF,
                           (value >> 8) & 0xFF, value & 0xFF)
    print(f"  TX: {request.hex()}")

    ser.reset_input_buffer()
    ser.write(request)

    response = read_response(ser)
    print(f"  RX: {response.hex()}" if response else "  RX: (no response)")

    if response and verify_crc(response):
        if response[1] == 0x05:
            print(f"  [OK] Coil set")
            return True
        elif response[1] == 0x85:
            print(f"  [EXCEPTION] Code: 0x{response[2]:02X}")
    else:
        print("  [FAIL] CRC error or no response")
    return False

def main():
    port = sys.argv[1] if len(sys.argv) > 1 else "COM3"
    baudrate = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
    slave_addr = int(sys.argv[3]) if len(sys.argv) > 3 else 0x01

    print("=" * 60)
    print("Modbus RTU Slave Test")
    print(f"Port: {port}, Baudrate: {baudrate}, Slave Address: 0x{slave_addr:02X}")
    print("=" * 60)

    try:
        ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )
        print(f"[INFO] Port {port} opened successfully")
        time.sleep(0.5)
        ser.reset_input_buffer()

        passed = 0
        total = 0

        # Test 1: Read Holding Registers (preset values)
        total += 1
        if test_read_holding_registers(ser, slave_addr, 0x0000, 3):
            passed += 1

        # Test 2: Read Input Registers
        total += 1
        if test_read_input_registers(ser, slave_addr, 0x0000, 2):
            passed += 1

        # Test 3: Read Coils
        total += 1
        if test_read_coils(ser, slave_addr, 0x0000, 8):
            passed += 1

        # Test 4: Write Single Register
        total += 1
        if test_write_single_register(ser, slave_addr, 0x0010, 0xDEAD):
            passed += 1

        # Test 5: Verify written value
        total += 1
        if test_read_holding_registers(ser, slave_addr, 0x0010, 1):
            passed += 1

        # Test 6: Write Multiple Registers
        total += 1
        if test_write_multiple_registers(ser, slave_addr, 0x0020, [0x1111, 0x2222, 0x3333]):
            passed += 1

        # Test 7: Verify written values
        total += 1
        if test_read_holding_registers(ser, slave_addr, 0x0020, 3):
            passed += 1

        # Test 8: Write Single Coil
        total += 1
        if test_write_single_coil(ser, slave_addr, 0x0005, True):
            passed += 1

        # Test 9: Verify coil state
        total += 1
        if test_read_coils(ser, slave_addr, 0x0000, 8):
            passed += 1

        # Test 10: Invalid address (should return exception)
        print("\n[TEST] Invalid Address (expect exception)")
        total += 1
        request = build_request(slave_addr, 0x03, 0x10, 0x00, 0x00, 0x01)
        print(f"  TX: {request.hex()}")
        ser.reset_input_buffer()
        ser.write(request)
        response = read_response(ser)
        print(f"  RX: {response.hex()}" if response else "  RX: (no response)")
        if response and response[1] == 0x83:
            print(f"  [OK] Got expected exception: 0x{response[2]:02X}")
            passed += 1
        else:
            print("  [FAIL] Expected exception not received")

        ser.close()

        print("\n" + "=" * 60)
        print(f"TEST RESULTS: {passed}/{total} passed")
        print("=" * 60)

        return 0 if passed == total else 1

    except serial.SerialException as e:
        print(f"[ERROR] Serial error: {e}")
        return 1
    except Exception as e:
        print(f"[ERROR] {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
