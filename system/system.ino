// Define the LED pin for ESP32C3
#define LED_BUILTIN 8

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on
  delay(100);                      // wait 100 ms
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off
  delay(100);                      // wait 100 ms
}
