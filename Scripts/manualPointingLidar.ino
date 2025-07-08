#include <Servo.h>
#include <Wire.h>
#include <LIDARLite.h>

Servo servoPan;
Servo servoTilt;
LIDARLite lidar;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  lidar.begin(0, true);       // Default I2C address 0x62
  lidar.configure(0);         // Default configuration

  servoPan.attach(9);
  servoTilt.attach(10);

  Serial.println("Listo. Envia: PAN TILT (ej: 120 45)");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      int spaceIndex = input.indexOf(' ');
      if (spaceIndex > 0) {
        int panAngle = input.substring(0, spaceIndex).toInt();
        int tiltAngle = input.substring(spaceIndex + 1).toInt();

        panAngle = constrain(panAngle, 0, 180);
        tiltAngle = constrain(tiltAngle, 0, 180);

        servoPan.write(panAngle);
        servoTilt.write(tiltAngle);
        delay(500);  // Esperar estabilización

        int distance = lidar.distance();  // Medir en cm
        Serial.print("Pan: ");
        Serial.print(panAngle);
        Serial.print("°, Tilt: ");
        Serial.print(tiltAngle);
        Serial.print("°, ");
        Serial.print(distance);
        Serial.println(" cm");
      } else {
        Serial.println("Entrada invalida. Usa: PAN TILT (ej: 90 45)");
      }
    }
  }
}
