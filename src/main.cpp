#include <Arduino.h>

// Pin definitions for the 7-segment display segments (anodes)
#define A 2   // Segment A
#define B 3   // Segment B
#define C 4   // Segment C
#define D 5   // Segment D (ANODE OF THE DISPLAY)
#define E 6   // Segment E
#define FD 7  // Segment F (renamed to FD to avoid macro conflict)
#define G 8   // Segment G

// Pin definitions for the digit select lines (cathodes)
#define H 9    // Digit 0 (rightmost)
#define I 10   // Digit 1
#define J 11   // Digit 2
#define K 12   // Digit 3 (leftmost)
#define L 13   // Digit 4 (not used in this code)
#define M A0   // Digit 5 (not used in this code)

#define NUM_ANODES 7      // Number of segment pins
#define NUM_CATHODES 6    // Number of digit select pins

// All cathodes should be connected via transistors due to high current draw.
// The microcontroller cannot drive the LED arrays directly.

#define MULTIPLEX_DELAY 100 // Delay for multiplexing (not used in this code)

unsigned int counter = 0;                // Main counter to display
unsigned long counter_last_millis = 0;   // Last time the counter was updated
unsigned long icon_last_millis = 0;      // Last time the icon state was updated
uint8_t icon_display_state = 0;          // Holds the combined icon pattern to display

uint8_t digit_to_be_displayed = 0;       // Which digit is currently being displayed (for multiplexing)

// Segment pin mapping for the 7-segment display
const byte segmentPins[NUM_ANODES] = {A, B, C, D, E, FD, G};
// Cathode pin mapping for digit selection
const byte cathodePins[NUM_CATHODES] = {H, I, J, K, L, M};

// Lookup table for displaying digits 0-9 on the 7-segment display
// Note: This pattern is specific to the hardware wiring and may not match standard 7-segment layouts.
const byte seven_segment_lookup[10] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111  // 9
};

// Icon bitmasks for custom symbols (used on the thousands digit)
#define ICON_NONE         0b0000000
#define ICON_ARROW_LEFT   0b0000100
#define ICON_BLUETOOTH    0b0001000
#define ICON_HEADLAMP     0b0010000
#define ICON_ARROW_RIGHT  0b0100000
#define ICON_ONE          0b0000011 // Segments to display a '1'

// Writes the segment pattern to the display
// Each bit in segmentPattern corresponds to a segment (A-G)
void writeSegments(byte segmentPattern) {
    for (int i = 0; i < NUM_ANODES; i++) {
        if ((segmentPattern >> i) & 0x01) {
            // Turn segment ON (assuming common anode, active LOW)
            digitalWrite(segmentPins[i], HIGH);
        } else {
            // Turn segment OFF
            digitalWrite(segmentPins[i], LOW);
        }
    }
}

// Arduino setup function: initialize all segment and cathode pins as outputs
void setup() {
    for (int i = 0; i < NUM_ANODES; i++) {
        pinMode(segmentPins[i], OUTPUT);
    }
    for (int i = 0; i < NUM_CATHODES; i++) {
        pinMode(cathodePins[i], OUTPUT);
    }
}

// Main loop: handles counter increment, icon cycling, and display multiplexing
void loop() {
    // Update the counter every 10 ms
    if (millis() - counter_last_millis > 10) {
        counter++;
        counter_last_millis = millis();
        if (counter > 1999) {
            counter = 0;
        }
    }

    // Cycle through icon combinations every 0.5 seconds
    if (millis() - icon_last_millis > 500) {
        icon_last_millis = millis();

        // Cycle through 7 different icon combinations
        static uint8_t combination_index = 0;
        combination_index++;
        if (combination_index > 6)
            combination_index = 0;

        switch (combination_index) {
            case 0:
                icon_display_state = ICON_NONE;
                break;
            case 1:
                icon_display_state = ICON_ARROW_LEFT;
                break;
            case 2:
                icon_display_state = ICON_BLUETOOTH;
                break;
            case 3:
                icon_display_state = ICON_HEADLAMP;
                break;
            case 4:
                icon_display_state = ICON_ARROW_RIGHT;
                break;
            case 5:
                icon_display_state = ICON_ARROW_LEFT | ICON_BLUETOOTH; // Combo 1
                break;
            case 6:
                icon_display_state = ICON_HEADLAMP | ICON_ARROW_RIGHT; // Combo 2
                break;
        }
    }

    // Extract each digit from the counter value
    uint8_t dig3 = (counter % 10);         // Ones place
    uint8_t dig2 = (counter / 10) % 10;    // Tens place
    uint8_t dig1 = (counter / 100) % 10;   // Hundreds place
    uint8_t dig0 = (counter / 1000) % 10;  // Thousands place

    // Turn off all cathodes before updating the display to prevent ghosting
    for (int i = 0; i < NUM_CATHODES; i++) {
        digitalWrite(cathodePins[i], LOW);
    }

    // Multiplexing: display one digit at a time
    switch (digit_to_be_displayed) {
        case 0:
            // Display ones digit
            writeSegments(seven_segment_lookup[dig3]);
            digitalWrite(M, HIGH); // Enable cathode for digit 0 (rightmost)
            break;
        case 1:
            // Display tens digit
            writeSegments(seven_segment_lookup[dig2]);
            digitalWrite(L, HIGH); // Enable cathode for digit 1
            break;
        case 2:
            // Display hundreds digit
            writeSegments(seven_segment_lookup[dig1]);
            digitalWrite(K, HIGH); // Enable cathode for digit 2
            break;
        case 3: {
            // Display thousands digit and icons
            byte display_pattern = icon_display_state; // Start with icon pattern

            // If the thousands digit is '1', add its segments to the icon pattern
            if (dig0 == 1) {
                display_pattern |= ICON_ONE;
            }
            writeSegments(display_pattern);
            digitalWrite(J, HIGH); // Enable cathode for digit 3 (leftmost)
            break;
        }
    }

    // Move to the next digit for the next loop iteration
    digit_to_be_displayed++;
    if (digit_to_be_displayed >= 4)
        digit_to_be_displayed = 0;

    delay(2); // Short delay for multiplexing
}
