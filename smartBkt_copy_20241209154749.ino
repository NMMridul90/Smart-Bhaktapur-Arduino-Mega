#include <Servo.h>
#include <Adafruit_LiquidCrystal.h>

// Pin definitions
#define TRIG_PIN 7
#define ECHO_PIN 6
#define SERVO_PIN 9
#define VIBRATION_PIN 4  // Vibration sensor pin
#define LED_PIN 13       // LED indicator for earthquake
#define GREEN_LED_PIN 10 // Green LED for available slot
#define RED_LED_PIN 11   // Red LED for full parking
#define BUZZER_PIN 2     // Buzzer pin

// Create Servo and LCD objects
Servo gateServo;
Adafruit_LiquidCrystal lcdParking(0);  // First LCD for parking
Adafruit_LiquidCrystal lcdVibration(1);  // Second LCD for vibration

// Variables
int parkingSlots = 0;  // Tracks the current number of vehicles (0 to 10)
const int maxSlots = 10;
long duration;
int distance;
bool isSirenActive = false;  // Flag to check if siren is active

void setup() {
  // Set up pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);  // Set green LED as output
  pinMode(RED_LED_PIN, OUTPUT);    // Set red LED as output
  pinMode(BUZZER_PIN, OUTPUT);     // Set buzzer as output

  // Set up Serial, Servo, and LCDs
  Serial.begin(9600); // Start Serial communication
  gateServo.attach(SERVO_PIN);
  lcdParking.begin(16, 2);
  lcdVibration.begin(16, 2);

  lcdParking.print("Parking System");
  lcdVibration.print("Earthquake Mon.");

  // Close the gate initially
  gateServo.write(0);  // Adjust for gate closing position

  // Set the LEDs initially (both off)
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);   // Ensure the buzzer is off initially

  delay(1000);  // Wait for system to initialize
  Serial.println("Parking and Earthquake Detection System Initialized");
}

void loop() {
  // Check for vehicle presence
  distance = readUltrasonicDistance();

  if (distance < 10) {  // If a vehicle is within 10 cm
    if (parkingSlots < maxSlots) {
      lcdParking.clear();
      lcdParking.print("Slot Available");
      Serial.println("Slot Available: Gate Opening");

      gateServo.write(90);  // Adjust for gate opening position
      delay(1000);          // Wait for the car to pass through

      parkingSlots++;
      lcdParking.clear();
      lcdParking.print("Parked in Slot");
      lcdParking.setCursor(0, 1);
      lcdParking.print("Slots: " + String(parkingSlots) + "/" + String(maxSlots));

      Serial.print("Vehicle Parked. Slots Used: ");
      Serial.print(parkingSlots);
      Serial.print("/" + String(maxSlots));

      delay(500);
      gateServo.write(0);  // Close the gate
      Serial.println("Gate Closed");

      // Update LED status
      if (parkingSlots == maxSlots) {
        digitalWrite(GREEN_LED_PIN, LOW);  // Turn off green LED
        digitalWrite(RED_LED_PIN, HIGH);   // Turn on red LED
      } else {
        digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green LED
        digitalWrite(RED_LED_PIN, LOW);    // Turn off red LED
      }

    } else {
      lcdParking.clear();
      lcdParking.print("Parking Full!");
      Serial.println("Parking Full: Gate Closed");
      delay(500);
    }
  } else {
    lcdParking.clear();
    lcdParking.print("Parking Slots:");
    lcdParking.setCursor(0, 1);
    lcdParking.print("Available: " + String(maxSlots - parkingSlots));

    Serial.print("Available Slots: ");
    Serial.println(maxSlots - parkingSlots);

    // Update LED status
    if (parkingSlots == maxSlots) {
      digitalWrite(GREEN_LED_PIN, LOW);  // Turn off green LED
      digitalWrite(RED_LED_PIN, HIGH);   // Turn on red LED
    } else {
      digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green LED
      digitalWrite(RED_LED_PIN, LOW);    // Turn off red LED
    }
  }

  // Check for vibrations (earthquake detection)
  if (digitalRead(VIBRATION_PIN) == HIGH && !isSirenActive) {
    lcdVibration.clear();
    lcdVibration.print("Earthquake Alert!");
    Serial.println("Earthquake Detected!");

    isSirenActive = true;  // Set siren flag to true
    playReshamFiririBeat();     // Trigger the "Resham Firiri" melody
  } else {
    lcdVibration.clear();
    lcdVibration.print("No Vibrations");
    Serial.println("No Vibrations Detected");
  }

  delay(50);  // Small delay to avoid bouncing
}

long readUltrasonicDistance() {
  // Trigger the ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the time it takes for the echo to return
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance in cm
  distance = duration * 0.034 / 2;

  return distance;
}

// Complex Resham Firiri melody (improvised with variations)
void playReshamFiririBeat() {
  // Define the frequencies for the melody
  int tones[] = { 330, 349, 392, 440, 466, 523, 587, 659, 698, 784, 880, 988, 1046, 1174, 1318, 1396 };  // Extended frequency array
  
  // Define the durations for each note (in milliseconds)
  int durations[] = { 500, 500, 400, 400, 300, 200, 300, 200, 300, 400, 500, 300, 400, 500, 600, 700 };  // Extended duration array
  
  // Adding variation to melody
  for (int i = 0; i < 16; i++) {  // Loop through tones
    Serial.print("Playing note: ");
    Serial.println(tones[i]);
    
    tone(BUZZER_PIN, tones[i]);  // Play the current tone
    digitalWrite(LED_PIN, HIGH); // Turn on the LED
    delay(durations[i]);         // Wait for the note duration
    digitalWrite(LED_PIN, LOW);  // Turn off the LED
    delay(50);                   // Small delay between notes

    // Add rhythm with slight pauses and syncopation
    if (i == 4 || i == 6) { 
      delay(200);  // Pause after some notes for the beat
    }
  }

  // Add a dramatic end
  tone(BUZZER_PIN, 1000);  // Play an ending tone (high pitch)
  digitalWrite(LED_PIN, HIGH);
  delay(150);
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);      // Stop the sound

  Serial.println("Melody finished.");
}
