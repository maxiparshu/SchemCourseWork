#include <RH_ASK.h>
// Подключаем библиотеку SPI Library 
#include <SPI.h> 
 
// Создаем объект управления смещением амплитуды
RH_ASK rf_driver;
const int ledPin = 13;  // Pin for the LED (you can change it as needed)

void setup() {
  // Initialize the receiver pin
  rf_driver.init();
  pinMode(ledPin, OUTPUT);  // Set the LED pin as output
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  // Check for incoming messages
  if (rf_driver.recv(buf, &buflen)) {  
    // Turn on the LED when a signal is received
    digitalWrite(ledPin, HIGH);  // Turn on LED
    delay(1000);                  // Keep the LED on for 500ms
    digitalWrite(ledPin, LOW);   // Turn off LED

    // Clear the message buffer (overwrite with zeros)
    memset(buf, 0, sizeof(buf));  // This resets the buffer to zero
  }
}