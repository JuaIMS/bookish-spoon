// Weigh scale up 10KG
#include <Arduino.h>
#include <HX711_ADC.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

const int tare_sw = 9;
long int offset = 0;
long int calfactor = 0;
int digit = 0;

const int clk = 13;   // SRCK
const int latch = 10; // RCK
const int data = 11;  // DIN
const int OE = 14;    // output enable
const int digit0 = 2;
const int digit1 = 3;
const int digit2 = 4;
const int digit3 = 5;
const int digit4 = 6;

HX711_ADC LoadCell(8, 7); // parameters: dt pin, sck pin

// 7-segment digits 0-9
// {Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7} --> {g, f, e, d, c, b, a, DP}
byte ssddigits[10] = // array without decimal points on
    {
        B01111110, // 0
        B00001100, // 1
        B10110110, // 2
        B10011110, // 3
        B11001100, // 4
        B11011010, // 5
        B11111010, // 6
        B01001110, // 7
        B11111110, // 8
        B11011110, // 9
};

byte ssddigitsDP[10] = // array with decimal points on
    {
        B01111111, // 0
        B00001101, // 1
        B10110111, // 2
        B10011111, // 3
        B11001101, // 4
        B11011011, // 5
        B11111011, // 6
        B01001111, // 7
        B11111111, // 8
        B11011111, // 9
};

void setup() {
  pinMode(tare_sw, INPUT_PULLUP);
  pinMode(clk, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(OE, OUTPUT); // use external pull-up
  pinMode(digit0, OUTPUT);
  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  // Serial.begin(9600); // for testing. Serial not connected in final version
  LoadCell.begin();     // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  // calfactor = analogRead(1);
  // LoadCell.setCalFactor(calfactor);
  LoadCell.setCalFactor(201.4); // calibration factor for load cell => strongly
                                // dependent on your individual setup
  LoadCell
      .tareNoDelay(); // cancel out weight of the plate attached to the sensor
  digitalWrite(OE, LOW); // turn LED display on. OE / G is pulled up to +5V
                         // externally by 4.7K resistor.
  // This prevents LED burnout if at power on / bootup random segments are lit
  // (multiplexing code will not be running)
}

void loop() {
  LoadCell.update(); // retrieves data from the load cell
  long int grams = LoadCell.getData() - offset; // get output value
  // long int grams = 12345; // for testing
  // Serial.println(grams);

  static uint16_t btndbc = 0;
  btndbc = (btndbc << 1) | digitalRead(tare_sw) | 0xe000; // debounce button
  if (btndbc == 0xf000)

  {
    delay(1); // wait 1 millisecond
    offset =
        LoadCell.getData(); // read initial HX711 value and store it as offset
  }

  // get digit data. Digit 0 is leftmost
  int dig4 = (grams % 10);         // ones
  int dig3 = (grams / 10) % 10;    // tens
  int dig2 = (grams / 100) % 10;   // hundreds
  int dig1 = (grams / 1000) % 10;  // thousands
  int dig0 = (grams / 10000) % 10; // ten thousands

  delay(1);

  // turn all digits off by setting digit pin high on each loop through.
  PORTD |= (1 << PORTD2);
  PORTD |= (1 << PORTD3);
  PORTD |= (1 << PORTD4);
  PORTD |= (1 << PORTD5);
  PORTD |= (1 << PORTD6);

  if (grams < -9 ||
      grams > 10010) // show error if minus figure or scale is overloaded

  {
    switch (digit) {
    case 0:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(B11110010); // E
      PORTB |= (1 << PORTB2);  // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD2); // set digit pin 0 LOW
      delay(2);
      break;

    case 1:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(B10100000); // r
      PORTB |= (1 << PORTB2);  // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD3);
      delay(2);
      break;

    case 2:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(B10100000); // r
      PORTB |= (1 << PORTB2);  // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD4);
      delay(2);
      break;

    case 3:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(B10111000); // o
      PORTB |= (1 << PORTB2);  // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD5);
      delay(2);
      break;

    case 4:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(B10100000); // r
      PORTB |= (1 << PORTB2);  // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD6);
      delay(2);
      break;
    }

    digit++;

    if (digit == 5) {
      digit = 0;
    }
  }

  else // display normally
  {
    switch (digit) {
    case 0:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(ssddigits[dig0]);
      PORTB |= (1 << PORTB2); // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD2); // set digit pin 0 LOW
      delay(2);
      break;

    case 1:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(
          ssddigitsDP[dig1]); // put a decimal point here. HX711 output is in
                              // grams and display will show in KG.
      PORTB |= (1 << PORTB2); // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD3);
      delay(2);
      break;

    case 2:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(ssddigits[dig2]);
      PORTB |= (1 << PORTB2); // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD4);
      delay(2);
      break;

    case 3:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(ssddigits[dig3]);
      PORTB |= (1 << PORTB2); // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD5);
      delay(2);
      break;

    case 4:
      SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
      PORTB &= ~(1 << PORTB2); // set latch LOW
      SPI.transfer(ssddigits[dig4]);
      PORTB |= (1 << PORTB2); // set latch HIGH
      SPI.endTransaction();
      PORTD &= ~(1 << PORTD6);
      delay(2);
      break;
    }

    digit++;

    if (digit == 5) {
      digit = 0;
    }
  }
}
