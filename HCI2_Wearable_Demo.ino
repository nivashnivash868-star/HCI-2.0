/* HCI 2.0 starter firmware
   Board assumption: original ESP32-WROOM-32 Dev Module with Bluetooth Classic SPP.
   MPU6050: VCC=3V3, GND=GND, SCL=GPIO22, SDA=GPIO21.
   Button: GPIO27 to GND, using INPUT_PULLUP (pressed reads LOW).
   Sends CSV: ax,ay,az,gx,gy,gz,button over Bluetooth and Serial.

   This is telemetry starter code, not calibrated gesture recognition or cursor control.
*/
#include <Wire.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
static const uint8_t MPU_ADDR = 0x68;
static const int SDA_PIN = 21;
static const int SCL_PIN = 22;
static const int BUTTON_PIN = 27;
static const uint32_t SAMPLE_PERIOD_MS = 50;
uint32_t lastSample = 0;

bool writeMPU(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool readMPU(uint8_t startReg, uint8_t *buf, size_t count) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(startReg);
  if (Wire.endTransmission(false) != 0) return false;
  size_t got = Wire.requestFrom((int)MPU_ADDR, (int)count, (int)true);
  if (got != count) return false;
  for (size_t i = 0; i < count; ++i) buf[i] = Wire.read();
  return true;
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  if (!writeMPU(0x6B, 0x00)) {
    Serial.println("ERROR: MPU6050 not responding at 0x68; check wiring/address.");
  } else {
    writeMPU(0x1B, 0x00); // gyro +/-250 deg/s
    writeMPU(0x1C, 0x00); // accel +/-2 g
    Serial.println("MPU6050 initialized.");
  }

  if (SerialBT.begin("HCI2_Wearable")) Serial.println("Bluetooth device: HCI2_Wearable");
  else Serial.println("Bluetooth start failed; check board compatibility.");
  Serial.println("ax,ay,az,gx,gy,gz,button");
}

void loop() {
  const uint32_t now = millis();
  if (now - lastSample < SAMPLE_PERIOD_MS) return;
  lastSample = now;

  uint8_t raw[14];
  if (!readMPU(0x3B, raw, sizeof(raw))) {
    Serial.println("ERROR,MPU6050_READ");
    return;
  }
  int16_t ax = (int16_t)((raw[0] << 8) | raw[1]);
  int16_t ay = (int16_t)((raw[2] << 8) | raw[3]);
  int16_t az = (int16_t)((raw[4] << 8) | raw[5]);
  int16_t gx = (int16_t)((raw[8] << 8) | raw[9]);
  int16_t gy = (int16_t)((raw[10] << 8) | raw[11]);
  int16_t gz = (int16_t)((raw[12] << 8) | raw[13]);
  const int button = (digitalRead(BUTTON_PIN) == LOW) ? 1 : 0;

  char line[100];
  snprintf(line, sizeof(line), "%d,%d,%d,%d,%d,%d,%d", ax, ay, az, gx, gy, gz, button);
  Serial.println(line);
  if (SerialBT.hasClient()) SerialBT.println(line);
}
