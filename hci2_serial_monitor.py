"""HCI 2.0 starter host monitor over USB serial.
Install: python3 -m pip install pyserial
Usage: python3 hci2_serial_monitor.py /dev/cu.usbserial-XXXX
On Windows, pass COM3 or the port shown in Device Manager.
This monitors telemetry only; it does not move the cursor.
"""
import sys
import time
import serial

BAUD = 115200


def main() -> int:
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <serial-port>")
        return 2
    port = sys.argv[1]
    try:
        with serial.Serial(port, BAUD, timeout=1) as ser:
            time.sleep(1.5)  # Many ESP32 boards reset when the serial port opens.
            print("Listening for ax,ay,az,gx,gy,gz,button CSV lines. Press Ctrl+C to stop.")
            while True:
                line = ser.readline().decode("utf-8", errors="replace").strip()
                if line:
                    print(line)
    except KeyboardInterrupt:
        print("\nStopped.")
        return 0
    except serial.SerialException as exc:
        print(f"Serial error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
