#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select the motor port (M1, M2, M3, or M4). Here we use M1:
Adafruit_DCMotor *RMotor = AFMS.getMotor(1);
Adafruit_DCMotor *LMotor = AFMS.getMotor(2);


const int micPin = A0; // Connect MAX4466 OUT to pin A0
const int sampleWindow = 50; // Sample window width in ms (50ms = 20Hz)
unsigned int sample;

void setup() {
  Serial.begin(9600);           
  Serial.println("Adafruit Motor Shield V2 - DC Motor Test");

  // Start the shield with default frequency 1.6KHz
  if (!AFMS.begin()) { 
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  // Set the initial speed (0 to 255)
  RMotor->setSpeed(250);
  RMotor->run(RELEASE);
  LMotor->setSpeed(250);
  LMotor->run(RELEASE);


  
}

void loop() {
  unsigned long startMillis = millis(); 
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // Collect data fored in the sample window
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(micPin);
    if (sample < 1024) {  // toss out spurious readings
      if (sample > signalMax) {
        signalMax = sample;  // save just the max
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 3.3) / 1024.0; // Convert to voltage (using 3.3V VCC)

  Serial.println(volts);

  if (volts < 1){
    RMotor->run(FORWARD);
    RMotor->setSpeed(200); // Set speed (0-255)
    LMotor->run(FORWARD);
    LMotor->setSpeed(100); // Set speed (0-255)
    Serial.println("Zoom");
  }
  else if (volts > 1){
    RMotor->run(RELEASE);
    LMotor->run(RELEASE);
    delay(1000); // needs to be changed in way where it counts milliseconds and doesn't use delay()
    Serial.println("Freeze");
  }


}