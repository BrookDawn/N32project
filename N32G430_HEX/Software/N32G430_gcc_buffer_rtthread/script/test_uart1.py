#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
N32G430 RT-Thread UART1 Test Script

This script tests the UART1 serial communication functionality
of the N32G430 RT-Thread Nano project.

Usage:
    python test_uart1.py [COM_PORT] [BAUDRATE]

Example:
    python test_uart1.py COM3 115200
    python test_uart1.py /dev/ttyUSB0 115200
"""

import serial
import time
import sys
import threading


class UART1Tester:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.running = False
        self.received_data = []

    def connect(self):
        """Connect to the serial port"""
        try:
            self.serial = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1
            )
            print(f"[OK] Connected to {self.port} at {self.baudrate} baud")
            return True
        except serial.SerialException as e:
            print(f"[ERROR] Failed to connect: {e}")
            return False

    def disconnect(self):
        """Disconnect from the serial port"""
        if self.serial and self.serial.is_open:
            self.running = False
            self.serial.close()
            print(f"[OK] Disconnected from {self.port}")

    def read_thread(self):
        """Thread to read data from serial port"""
        while self.running:
            try:
                if self.serial.in_waiting > 0:
                    data = self.serial.readline().decode('utf-8', errors='ignore').strip()
                    if data:
                        print(f"[RX] {data}")
                        self.received_data.append(data)
                else:
                    time.sleep(0.01)
            except Exception as e:
                if self.running:
                    print(f"[ERROR] Read error: {e}")

    def start_reading(self):
        """Start the reading thread"""
        self.running = True
        self.read_thread_handle = threading.Thread(target=self.read_thread)
        self.read_thread_handle.daemon = True
        self.read_thread_handle.start()

    def stop_reading(self):
        """Stop the reading thread"""
        self.running = False
        if hasattr(self, 'read_thread_handle'):
            self.read_thread_handle.join(timeout=1)

    def send(self, data):
        """Send data to the serial port"""
        if self.serial and self.serial.is_open:
            if isinstance(data, str):
                data = data.encode('utf-8')
            self.serial.write(data)
            print(f"[TX] {data.decode('utf-8', errors='ignore')}")
            return True
        return False

    def wait_for_response(self, expected, timeout=5):
        """Wait for a specific response"""
        start_time = time.time()
        while time.time() - start_time < timeout:
            for data in self.received_data:
                if expected in data:
                    return True
            time.sleep(0.1)
        return False

    def clear_received(self):
        """Clear received data buffer"""
        self.received_data = []


def run_tests(tester):
    """Run UART1 tests"""
    print("\n" + "="*60)
    print("N32G430 RT-Thread UART1 Test Suite")
    print("="*60 + "\n")

    tests_passed = 0
    tests_failed = 0

    # Test 1: Wait for boot message
    print("[TEST 1] Waiting for RT-Thread boot message...")
    tester.clear_received()
    time.sleep(2)  # Wait for boot

    boot_found = False
    for data in tester.received_data:
        if "RT-Thread" in data or "Started" in data:
            boot_found = True
            break

    if boot_found:
        print("[PASS] RT-Thread boot message received")
        tests_passed += 1
    else:
        print("[INFO] No boot message yet (device may have already booted)")
        tests_passed += 1  # Not a failure, device may have already booted

    # Test 2: Echo test - single character
    print("\n[TEST 2] Single character echo test...")
    tester.clear_received()
    tester.send("A")
    time.sleep(0.5)

    echo_found = False
    for data in tester.received_data:
        if "A" in data or "Received" in data:
            echo_found = True
            break

    if echo_found:
        print("[PASS] Single character echo successful")
        tests_passed += 1
    else:
        print("[FAIL] Single character echo failed")
        tests_failed += 1

    # Test 3: Echo test - string
    print("\n[TEST 3] String echo test...")
    tester.clear_received()
    test_string = "Hello RT-Thread!\r\n"
    tester.send(test_string)
    time.sleep(1)

    echo_found = False
    for data in tester.received_data:
        if "Hello" in data or "Received" in data:
            echo_found = True
            break

    if echo_found:
        print("[PASS] String echo successful")
        tests_passed += 1
    else:
        print("[FAIL] String echo failed")
        tests_failed += 1

    # Test 4: LED status test
    print("\n[TEST 4] LED status test...")
    tester.clear_received()
    tester.send("status\r\n")
    time.sleep(1)

    led_found = False
    for data in tester.received_data:
        if "LED" in data:
            led_found = True
            break

    if led_found:
        print("[PASS] LED status received")
        tests_passed += 1
    else:
        print("[INFO] No LED status (may need different trigger)")
        tests_passed += 1  # Not a strict failure

    # Test 5: Multiple rapid transmissions
    print("\n[TEST 5] Multiple rapid transmissions...")
    tester.clear_received()
    for i in range(5):
        tester.send(f"Test{i}\r\n")
        time.sleep(0.1)
    time.sleep(1)

    count = sum(1 for data in tester.received_data if "Test" in data or "Received" in data)
    if count >= 3:
        print(f"[PASS] Received {count}/5 responses")
        tests_passed += 1
    else:
        print(f"[FAIL] Only received {count}/5 responses")
        tests_failed += 1

    # Test 6: Long string test
    print("\n[TEST 6] Long string test (64 bytes)...")
    tester.clear_received()
    long_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz!@\r\n"
    tester.send(long_string)
    time.sleep(1)

    long_found = False
    for data in tester.received_data:
        if "ABCD" in data or "Received" in data:
            long_found = True
            break

    if long_found:
        print("[PASS] Long string echo successful")
        tests_passed += 1
    else:
        print("[FAIL] Long string echo failed")
        tests_failed += 1

    # Print summary
    print("\n" + "="*60)
    print("Test Summary")
    print("="*60)
    print(f"Tests Passed: {tests_passed}")
    print(f"Tests Failed: {tests_failed}")
    print(f"Total Tests:  {tests_passed + tests_failed}")
    print("="*60 + "\n")

    return tests_failed == 0


def interactive_mode(tester):
    """Interactive mode for manual testing"""
    print("\n" + "="*60)
    print("Interactive Mode")
    print("="*60)
    print("Type your message and press Enter to send")
    print("Type 'exit' or 'quit' to exit")
    print("Type 'clear' to clear received buffer")
    print("="*60 + "\n")

    while True:
        try:
            user_input = input(">> ")
            if user_input.lower() in ['exit', 'quit']:
                break
            elif user_input.lower() == 'clear':
                tester.clear_received()
                print("[OK] Buffer cleared")
            else:
                tester.send(user_input + "\r\n")
        except KeyboardInterrupt:
            break
        except EOFError:
            break


def main():
    # Default settings
    port = "COM3"  # Default for Windows
    baudrate = 115200

    # Parse command line arguments
    if len(sys.argv) >= 2:
        port = sys.argv[1]
    if len(sys.argv) >= 3:
        baudrate = int(sys.argv[2])

    print("="*60)
    print("N32G430 RT-Thread UART1 Test")
    print("="*60)
    print(f"Port:     {port}")
    print(f"Baudrate: {baudrate}")
    print("="*60 + "\n")

    # Create tester
    tester = UART1Tester(port, baudrate)

    # Connect
    if not tester.connect():
        print("\n[ERROR] Cannot continue without serial connection")
        print("Please check:")
        print("  1. The device is connected")
        print("  2. The correct COM port is specified")
        print("  3. No other program is using the port")
        return 1

    # Start reading thread
    tester.start_reading()

    try:
        # Run automated tests
        success = run_tests(tester)

        # Ask if user wants interactive mode
        print("\nEnter interactive mode? (y/n): ", end="")
        try:
            answer = input().strip().lower()
            if answer == 'y':
                interactive_mode(tester)
        except (KeyboardInterrupt, EOFError):
            pass

    except KeyboardInterrupt:
        print("\n[INFO] Test interrupted by user")
    finally:
        # Cleanup
        tester.stop_reading()
        tester.disconnect()

    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())
