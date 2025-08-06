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

const byte segmentPins[NUM_ANODES] = {A, B, C, D, E, FD, G};
const byte cathodePins[NUM_CATHODES] = {H, I, J, K, L, M};

// I must say that this lookup table is weird as hell and does not follow the
// standard seven-segment display pattern, but it works!
// What can I say? It's a hacky solution that works for this specific display.
const byte seven_segment_lookup[10]{
    0b0000000, // 0
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

void writeSegments(byte segmentPattern) {
  for (int i = 0; i < NUM_ANODES; i++) { // Loop only 7 times
    if ((segmentPattern >> i) & 0x01) {
      digitalWrite(segmentPins[i],
                   HIGH); // Turn segment ON (assuming common anode, active LOW)
    } else {
      digitalWrite(segmentPins[i], LOW); // Turn segment OFF
    }
  }
}

void setup() {
  for (int i = 0; i < NUM_ANODES; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  for (int i = 0; i < NUM_CATHODES; i++) {
    pinMode(cathodePins[i], OUTPUT);
  }

  pinMode(K, INPUT_PULLUP);
}

void loop() {
  for (int i = 0; i <= 9; i++) {
    writeSegments(seven_segment_lookup[i]);
    delay(MULTIPLEX_DELAY);
  }

  if(millis() - counter_last_millis > 1000){
      counter++;
      counter_last_millis = millis();
      if(counter > 199){
        counter = 0;
      }
  }
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
  // delay(1);
}
