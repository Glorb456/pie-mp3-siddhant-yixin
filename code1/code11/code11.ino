#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select the motor port (M1, M2, M3, or M4). Here we use M1:
Adafruit_DCMotor *RMotor = AFMS.getMotor(1);
Adafruit_DCMotor *LMotor = AFMS.getMotor(2);


const int micPin = A0; // right microphone sensor
const int micPin2 = A1; // left microphone sensor
const int sampleWindow = 50; // Sample window width in ms (50ms = 20Hz) which is optimum frequency
unsigned int sample;
unsigned int sample2;

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
  unsigned int peakToPeak2 = 0;
  unsigned int signalMax = 0;
  unsigned int signalMax2 = 0;
  unsigned int signalMin = 1024;
  unsigned int signalMin2 = 1024;

  // Collect data fored in the sample window
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(micPin);
    sample2 = analogRead(micPin2);
    if (sample < 1024) {  // toss out spurious readings  
      if (sample > signalMax) {
        signalMax = sample;  // save just the max
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min
      }
    }

    if (sample2 < 1024) {  // toss out spurious readings
      if (sample2 > signalMax2) {
        signalMax2 = sample2;  // save just the max
      } else if (sample2 < signalMin2) {
        signalMin2 = sample2;  // save just the min
      }
    }

  }

  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  peakToPeak2 = signalMax2 - signalMin2;
  double volts = (peakToPeak * 3.3) / 1024.0; // Convert to voltage (using 3.3V VCC)
  double volts2 = (peakToPeak2 * 3.3) / 1024.0; // Convert to voltage (using 3.3V VCC) - Using 3.3 instead of 5v to reduce noise 
  Serial.println();
  Serial.println(volts);
  Serial.println(volts2);
  Serial.println();

  if (volts < .75 && volts2 < .75) {
      RMotor->setSpeed(80);
      LMotor->setSpeed(80);
      LMotor->run(BACKWARD);
      RMotor->run(FORWARD);
    }

  else if (volts > 1.5 || volts2 > 1.5) {
    if (volts - volts2 <= 0){
      // set right to forward
      RMotor->setSpeed(150);
      RMotor->run(FORWARD);
      LMotor->run(RELEASE);
      delay(500); // needs to be changed in way where it counts milliseconds and doesn't use delay()

    }    
    else{
      // set left to forward
      LMotor->setSpeed(150);
      LMotor->run(BACKWARD);
      RMotor->run(RELEASE);    
      }
      delay(500); // needs to be changed in way where it counts milliseconds and doesn't use delay()
  }

  else if (volts > 2.5 || volts2 > 2.5){
      RMotor->setSpeed(255);
      LMotor->setSpeed(255);
      LMotor->run(BACKWARD);
      RMotor->run(FORWARD);
      delay(500);
    }
  else {
    RMotor->run(RELEASE);
    LMotor->run(RELEASE);
  }
}
