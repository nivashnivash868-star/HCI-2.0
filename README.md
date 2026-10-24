# HCI 2.0 - Wearable ESP32 Telemetry Demo

Starter firmware and host monitor for wearable Human-Computer Interaction (HCI) telemetry.

## Hardware Setup
- **Microcontroller**: ESP32-WROOM-32 Dev Module
- **IMU Sensor**: MPU6050 (I2C address: `0x68`)
  - `VCC` -> `3V3`
  - `GND` -> `GND`
  - `SDA` -> `GPIO 21`
  - `SCL` -> `GPIO 22`
- **Button**:
  - `GPIO 27` to `GND` (`INPUT_PULLUP`, pressed reads LOW)

## Telemetry Format
The device streams comma-separated values over USB Serial (115200 baud) and Bluetooth Classic SPP (`HCI2_Wearable`):
```csv
ax,ay,az,gx,gy,gz,button
```

## Host Serial Monitor
Requires Python 3 and `pyserial`:
```bash
pip install pyserial
```

Run the monitor:
- **macOS / Linux**:
  ```bash
  python3 hci2_serial_monitor.py /dev/cu.usbserial-XXXX
  ```
- **Windows**:
  ```bash
  python hci2_serial_monitor.py COM3
  ```
