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

unsigned int counter = 0;
unsigned long counter_last_millis = 0;
unsigned long icon_last_millis = 0;
uint8_t icon_display_state = 0; // This will hold the combined icon pattern to display

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

#define ICON_NONE 0b0000000        // No icon
#define ICON_ARROW_LEFT 0b0000100  // Example: let's say this is segment C
#define ICON_BLUETOOTH 0b0001000   // Example: let's say this is segment D
#define ICON_HEADLAMP 0b0010000    // Example: let's say this is segment E
#define ICON_ARROW_RIGHT 0b0100000 // Example: let's say this is segment FD
#define ICON_ONE 0b0000011         // Based on your '1' lookup: B and C (assuming 0bGFEDCBA)

// const byte icons_without_one[6]{
//     0b0000000, // 0
//     0b0000100, // <-- icon
//     0b0001000, // Bluetooth icon
//     0b0010000, // Headlamp icon
//     0b0100000, // --> icon
// };

// const byte icons_with_one[6]{
//     0b0000011, // 0
//     0b0000011, // 1
//     0b0000111, // <-- icon
//     0b0001011, // Bluetooth icon
//     0b0010011, // Headlamp icon
//     0b0100011, // --> icon
// };

void writeSegments(byte segmentPattern)
{
  for (int i = 0; i < NUM_ANODES; i++)
  { // Loop only 7 times
    if ((segmentPattern >> i) & 0x01)
    {
      // Turn segment ON (assuming common anode, active LOW)
      digitalWrite(segmentPins[i], HIGH);
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
}

void loop()
{
  // writeSegments(seven_segment_lookup[counter]);

  if (millis() - counter_last_millis > 10)
  {
    counter++;
    counter_last_millis = millis();
    if (counter > 1999)
    {
      counter = 0;
    }
  }

// This is where you would determine which icons to display
  // based on some logic (e.g., sensor readings, button presses, etc.)
  // For demonstration, let's cycle through combinations:
  if (millis() - icon_last_millis > 500) // Change icon combination every 0.5 seconds
  {
    icon_last_millis = millis();

    // Example logic: Cycle through different combinations
    static uint8_t combination_index = 0;
    combination_index++;
    if (combination_index > 6) combination_index = 0; // 7 combinations (0-6)

    switch (combination_index) {
      case 0: icon_display_state = ICON_NONE; break;
      case 1: icon_display_state = ICON_ARROW_LEFT; break;
      case 2: icon_display_state = ICON_BLUETOOTH; break;
      case 3: icon_display_state = ICON_HEADLAMP; break;
      case 4: icon_display_state = ICON_ARROW_RIGHT; break;
      case 5: icon_display_state = ICON_ARROW_LEFT | ICON_BLUETOOTH; break; // Combo 1
      case 6: icon_display_state = ICON_HEADLAMP | ICON_ARROW_RIGHT; break; // Combo 2
    }
  }

  // get digit data. Digit 0 is leftmost
  uint8_t dig3 = (counter % 10);        // ones
  uint8_t dig2 = (counter / 10) % 10;   // tens
  uint8_t dig1 = (counter / 100) % 10;  // hundreds
  uint8_t dig0 = (counter / 1000) % 10; // thousands
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
    writeSegments(seven_segment_lookup[dig3]);
    digitalWrite(M, HIGH); // Turn on the cathode for digit 0
    break;
  case 1:
    writeSegments(seven_segment_lookup[dig2]);
    digitalWrite(L, HIGH); // Turn on the cathode for digit 1
    break;
  case 2:
    writeSegments(seven_segment_lookup[dig1]);
    digitalWrite(K, HIGH); // Turn on the cathode for digit 2
    break;
  case 3: // Thousands place 
  {
    byte display_pattern = icon_display_state; // Start with the desired icon pattern

    if (dig0 == 1) // If the thousands digit is '1', OR its segments into the pattern
    {
      display_pattern |= ICON_ONE; // Combine '1' segments with icon segments
    }
    writeSegments(display_pattern);
    digitalWrite(J, HIGH); // Turn on the cathode for digit 3
  }
  break;
  }

  // if(dig0 == 1){
  //   writeSegments(icons_with_one[icon_counter]);
  // }
  // else{
  //   writeSegments(icons_without_one[icon_counter]);
  // }
  // digitalWrite(J, HIGH); // Turn on the cathode for digit 3

  digit_to_be_displayed++; // Increment the digit to be displayed
  // Reset to 0 if it exceeds the number of digits available on the display
  if (digit_to_be_displayed >= 4)
    digit_to_be_displayed = 0;

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

  delay(2);
}
