#include <Wire.h>
#include <math.h>

#define MPU_ADDR 0x68

// motor 1
#define M1_RPWM 25
#define M1_LPWM 26
#define M1_REN  27
#define M1_LEN  13

// motor 2
#define M2_RPWM 32
#define M2_LPWM 33
#define M2_REN  14
#define M2_LEN  19


float targetAngle = 3.46;

float Kp = 10.0;         
float Ki = 0.0;          
float Kd = 0.5;        
int minMotorSpeed = 0;  


unsigned long prevTime = 0;
unsigned long lastPrintTime = 0;
float currentAngle = 0.0;
float pidError = 0, pidIntegral = 0, pidDerivative = 0, prevError = 0;
float pidOutput = 0;
int motorSpeed = 0;


float gyroX_offset = 0;
float accelY_offset = 0;

void writeRegister(byte reg, byte value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  Wire.setClock(400000); 

  writeRegister(0x6B, 0x00); 
  writeRegister(0x1C, 0x00); 
  writeRegister(0x1B, 0x00); 

  pinMode(M1_RPWM, OUTPUT);
  pinMode(M1_LPWM, OUTPUT);
  pinMode(M1_REN, OUTPUT);
  pinMode(M1_LEN, OUTPUT);

  pinMode(M2_RPWM, OUTPUT);
  pinMode(M2_LPWM, OUTPUT);
  pinMode(M2_REN, OUTPUT);
  pinMode(M2_LEN, OUTPUT);

  digitalWrite(M1_REN, HIGH);
  digitalWrite(M1_LEN, HIGH);
  digitalWrite(M2_REN, HIGH);
  digitalWrite(M2_LEN, HIGH);

  Serial.println("\n[SYSTEM] Calibrating IMU... Keep the robot still on flat ground!");
  delay(1500);

  long gx_sum = 0;
  long ay_sum = 0;
  const int samples = 500;

  for (int i = 0; i < samples; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 14);

    if (Wire.available() == 14) {
      Wire.read(); Wire.read(); 
      int16_t ay = Wire.read() << 8 | Wire.read();
      Wire.read(); Wire.read(); 
      Wire.read(); Wire.read(); 
      int16_t gx = Wire.read() << 8 | Wire.read();
      Wire.read(); Wire.read(); 
      Wire.read(); Wire.read(); 

      gx_sum += gx;
      ay_sum += ay;
    }
    delay(2);
  }

  gyroX_offset = (float)gx_sum / samples;
  accelY_offset = (float)ay_sum / samples;

  Serial.print("[SYSTEM] Calibrated. GyroX Offset: ");
  Serial.println(gyroX_offset);
  Serial.println("[SYSTEM] Ready. Balancing active.");

  prevTime = micros();
}

void loop() {
  unsigned long currentTime = micros();
  float dt = (currentTime - prevTime) / 1000000.0;
  if (dt < 0.010) {
    return; 
  }
  prevTime = currentTime;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14);

  if (Wire.available() == 14) {
    Wire.read(); Wire.read(); 
    int16_t ay = Wire.read() << 8 | Wire.read();
    int16_t az = Wire.read() << 8 | Wire.read();
    Wire.read(); Wire.read(); 
    int16_t gx = Wire.read() << 8 | Wire.read();
    Wire.read(); Wire.read(); 
    Wire.read(); Wire.read(); 

    float accelAngle = atan2((float)ay - accelY_offset, (float)az) * 180.0 / PI;
    float gyroRate = ((float)gx - gyroX_offset) / 131.0;

    currentAngle = 0.98 * (currentAngle + gyroRate * dt) + 0.02 * accelAngle;

    pidError = targetAngle - currentAngle;
    pidIntegral += pidError * dt;
    pidIntegral = constrain(pidIntegral, -300, 300); 

    pidDerivative = (pidError - prevError) / dt;
    pidOutput = (Kp * pidError) + (Ki * pidIntegral) + (Kd * pidDerivative);
    prevError = pidError;

    if (abs(pidOutput) > 1.0) {
      motorSpeed = abs(pidOutput) + minMotorSpeed;
    } else {
      motorSpeed = 0;
    }
    motorSpeed = constrain(motorSpeed, 0, 255);

    if (abs(currentAngle - targetAngle) > 40.0) {
      motorSpeed = 0;
      pidIntegral = 0;
    }

    if (pidOutput > 0) {
      analogWrite(M1_RPWM, motorSpeed);
      analogWrite(M1_LPWM, 0);
      analogWrite(M2_RPWM, motorSpeed);
      analogWrite(M2_LPWM, 0);
    } else if (pidOutput < 0) {
      analogWrite(M1_RPWM, 0);
      analogWrite(M1_LPWM, motorSpeed);
      analogWrite(M2_RPWM, 0);
      analogWrite(M2_LPWM, motorSpeed);
    } else {
      analogWrite(M1_RPWM, 0);
      analogWrite(M1_LPWM, 0);
      analogWrite(M2_RPWM, 0);
      analogWrite(M2_LPWM, 0);
    }
  }


  if (millis() - lastPrintTime >= 100) {
    lastPrintTime = millis();
    Serial.print("Angle: ");
    Serial.print(currentAngle, 2);
    Serial.print(" deg | PWM: ");
    Serial.println(motorSpeed);
  }
}