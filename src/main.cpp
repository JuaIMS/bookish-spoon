#include <Arduino.h>

// #define debug_active 0

// #if debug_active == 1
// #define debug_print(x) Serial.print(x)
// #define debug_println(x) Serial.println(x)
// #else
// #define debug_print(x)
// #define debug_println(x)
// #endif

#define A 2  //
#define B 3  //
#define C 4  //
#define D 5  //  ANODE OF THE DISPLAY
#define E 6  //
#define FD 7 //  F -> FD (F is already defined as a macro)
#define G 8  //

#define H 9  //
#define I 10 //
#define J 11 //  CATHODE OF THE DISPLAY
#define K 12 //
#define L 13 //
#define M A0 //

#define NUM_ANODES 7
// All the cathodes should be connected via transistors, the current
// consumption is relatively high for the microcontroller to controll the
// led arrays by itself.
#define NUM_CATHODES 6

#define MULTIPLEX_DELAY 100 // Sets the delay of multiplexing.

uint8_t counter = 0;
unsigned long counter_last_millis = 0;

uint8_t digit_to_be_displayed = 0; // The digit to be displayed on the display

const byte segmentPins[NUM_ANODES] = {A, B, C, D, E, FD, G};
const byte cathodePins[NUM_CATHODES] = {H, I, J, K, L, M};

// I must say that this lookup table is weird as hell and does not follow the
// standard seven-segment display pattern, but it works!
// What can I say? It's a hacky solution that works for this specific display.
const byte seven_segment_lookup[10]{
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111, // 9
};

void writeSegments(byte segmentPattern)
{
  for (int i = 0; i < NUM_ANODES; i++)
  { // Loop only 7 times
    if ((segmentPattern >> i) & 0x01)
    {
      digitalWrite(segmentPins[i],
                   HIGH); // Turn segment ON (assuming common anode, active LOW)
    }
    else
    {
      digitalWrite(segmentPins[i], LOW); // Turn segment OFF
    }
  }
}

void setup()
{
  for (int i = 0; i < NUM_ANODES; i++)
  {
    pinMode(segmentPins[i], OUTPUT);
  }
  for (int i = 0; i < NUM_CATHODES; i++)
  {
    pinMode(cathodePins[i], OUTPUT);
  }

  pinMode(K, INPUT_PULLUP);
}

void loop()
{
  //writeSegments(seven_segment_lookup[counter]);

  if (millis() - counter_last_millis > 1000)
  {
    counter++;
    counter_last_millis = millis();
    if (counter > 99)
    {
      counter = 0;
    }
  }

  // get digit data. Digit 0 is leftmost
  uint8_t dig2 = (counter % 10);       // ones
  uint8_t dig1 = (counter / 10) % 10;  // tens
  uint8_t dig0 = (counter / 100) % 10; // hundreds

  // Since all the cathodes are connected via transistors (NPN in this case),
  // we turn them off by pulling the base low.
  // This done every loop so that there is no ghosting on the display.
  for (int i = 0; i < NUM_CATHODES; i++)
  {
    digitalWrite(cathodePins[i], LOW);
  }

  switch (digit_to_be_displayed)
  {
  case 0:
    writeSegments(seven_segment_lookup[dig0]);
    digitalWrite(M, HIGH); // Turn on the cathode for digit 0
    delay(2); // Short delay to allow the display to stabilize
    break;
  case 1:
    writeSegments(seven_segment_lookup[dig1]);
    digitalWrite(L, HIGH); // Turn on the cathode for digit 1
    delay(2); // Short delay to allow the display to stabilize
    break;
  case 2:
    writeSegments(seven_segment_lookup[dig2]);
    digitalWrite(K, HIGH); // Turn on the cathode for digit 2
    delay(2); // Short delay to allow the display to stabilize
    break;
  }
  digit_to_be_displayed++; // Increment the digit to be displayed
  // Reset to 0 if it exceeds the number of digits available on the display
  if (digit_to_be_displayed >= 3) digit_to_be_displayed = 0;

  

  // writeSegments(seven_segment_lookup[counter]);

  // static uint16_t btndbc = 0;
  // btndbc = (btndbc << 1) | digitalRead(A3) | 0xe000; // debounce button
  // if (btndbc == 0xf000) {
  //   delay(1); // wait 1 millisecond
  //   if(counter < 9){
  //     counter++;
  //   } else {
  //     counter = 0;
  //   }
  // }
  delay(analogRead(A1)); // Delay based on the analog reading from A1
  // This is just to visually see the display multiplexing effect.
}
