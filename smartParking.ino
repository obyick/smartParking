// Global variables to store the last measured distance
int lastDistance = 0;

// Global variables to store the time of the last distance change
unsigned long lastDistanceChangeTime = 0;

// Calculates the prescaler value to achieve a baud rate of 9600 bps
const int BAUD_PRESCALE = (F_CPU / (9600 * 16UL)) - 1;

// Function to initialize USART0 serial communication
void initUSART0() {
  UBRR0H = (BAUD_PRESCALE >> 8);
  UBRR0L = BAUD_PRESCALE;
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Function to initialize USART1 serial communication
void initUSART1() {
  UBRR1H = (BAUD_PRESCALE >> 8);
  UBRR1L = BAUD_PRESCALE;
  UCSR1B = (1 << TXEN1) | (1 << RXEN1);
  UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

// Function to initialize output pin configuration
void initIO() {
  DDRB |= (1 << DDB7);
  DDRB |= (1 << DDB6);
}

// Function to make the LED blink at a specific frequency
void blinkLED(int freq) {
  PORTB |= (1 << PORTB7);
  delay(freq);
  PORTB &= ~(1 << PORTB7);
  delay(freq);
}

// Function to update the LED state based on the measured distance
void updateLEDState(int distance) {
  if (distance < 10 || (millis() - lastDistanceChangeTime >= 10000 && distance < 130)) {
    PORTB &= ~(1 << PORTB6);
    PORTB |= (1 << PORTB7);
    delay(1000);
  } else {
    PORTB &= ~(1 << PORTB6);
    // Seleciona a frequência do piscar com base na distância medida
    if (distance < 40) {
      blinkLED(100);
    } else if (distance < 70) {
      blinkLED(200);
    } else if (distance < 100) {
      blinkLED(300);
    } else if (distance < 130) {
      blinkLED(400);
    } else {
      lastDistanceChangeTime = 0;
      PORTB &= ~(1 << PORTB7);
      PORTB |= (1 << PORTB6);
      delay(1000);
    }
  }
}

// Setup function
void setup() {
  initUSART0();
  initUSART1();
  initIO();
  DDRE |= (1 << DDE5);
  DDRG &= ~(1 << DDG5);
}

// Loop function
void loop() {
  // Sends a pulse to the ultrasonic sensor
  PORTE |= (1 << PORTE5);
  delay(100);
  PORTE &= ~(1 << PORTE5);

  // Measures the duration of the echo pulse
  int duration = pulseIn(4, HIGH);

  // Calculates the distance based on the pulse duration
  int distance = duration * 0.0343 / 2;

  // Checks if the current distance is different from the last measured one
  if (distance != lastDistance) {
    lastDistanceChangeTime = millis();  // Updates the time of the last distance change
    lastDistance = distance;            // Updates the last measured distance
  }

  // Updates the LED state based on the measured distance
  updateLEDState(distance);

  // Transmitting information about the distance
  if (millis() - lastDistanceChangeTime <= 10000 && distance < 130) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print("cm / ");
  }

  // Transmitting information about the duration
  if (distance < 130) {
    Serial.print("Duration: ");
    Serial.print((millis() - lastDistanceChangeTime) / 1000);
    Serial.println("s");
  }
}
