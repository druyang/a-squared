#include <SPI.h>
#include <SD.h>
#include <Adafruit_H3LIS331.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Set the pins for SD card
#define cardSelect 4
// Used for software SPI
#define H3LIS331_SCK 13
#define H3LIS331_MISO 12
#define H3LIS331_MOSI 11
// Used for hardware & software SPI
#define H3LIS331_CS 10
Adafruit_H3LIS331 lis = Adafruit_H3LIS331();

File logfile;

// blink out an error code
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}

// This line is not needed if you have Adafruit SAMD board package 1.6.2+
//   #define Serial SerialUSB

void setup() {
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  pinMode(13, OUTPUT);


  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    error(2);
  }
  char filename[15];
  strcpy(filename, "/ANALOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    error(3);
  }

  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);

  if (!lis.begin_SPI(H3LIS331_CS, H3LIS331_SCK, H3LIS331_MISO, H3LIS331_MOSI)) {
    logfile.println("Couldnt start");
    while (1) yield();
  }
  lis.setRange(H3LIS331_RANGE_400_G);   // 100, 200, or 400 G!
  logfile.print("Range set to: ");
  switch (lis.getRange()) {
    case H3LIS331_RANGE_100_G: logfile.println("100 g"); break;
    case H3LIS331_RANGE_200_G: logfile.println("200 g"); break;
    case H3LIS331_RANGE_400_G: logfile.println("400 g"); break;
  }
  // lis.setDataRate(LIS331_DATARATE_1000_HZ);
  logfile.print("Data rate set to: ");
  switch (lis.getDataRate()) {
    case LIS331_DATARATE_POWERDOWN: logfile.println("Powered Down"); break;
    case LIS331_DATARATE_50_HZ: logfile.println("50 Hz"); break;
    case LIS331_DATARATE_100_HZ: logfile.println("100 Hz"); break;
    case LIS331_DATARATE_400_HZ: logfile.println("400 Hz"); break;
    case LIS331_DATARATE_1000_HZ: logfile.println("1000 Hz"); break;
    case LIS331_DATARATE_LOWPOWER_0_5_HZ: logfile.println("0.5 Hz Low Power"); break;
    case LIS331_DATARATE_LOWPOWER_1_HZ: logfile.println("1 Hz Low Power"); break;
    case LIS331_DATARATE_LOWPOWER_2_HZ: logfile.println("2 Hz Low Power"); break;
    case LIS331_DATARATE_LOWPOWER_5_HZ: logfile.println("5 Hz Low Power"); break;
    case LIS331_DATARATE_LOWPOWER_10_HZ: logfile.println("10 Hz Low Power"); break;
  }
  logfile.flush(); 
  delay(10000); 
}

uint16_t i=0;
void loop() {


  while (i<10000) {
    digitalWrite(8, HIGH);
    sensors_event_t event;
    lis.getEvent(&event);

    // Write to the card 
    logfile.print(micros()); 
    logfile.print(", "); logfile.print(event.acceleration.x / SENSORS_GRAVITY_STANDARD);
    logfile.print(", "); logfile.print(event.acceleration.y / SENSORS_GRAVITY_STANDARD);
    logfile.print(", "); logfile.println(event.acceleration.z / SENSORS_GRAVITY_STANDARD);
    i++; 

  }
  logfile.flush();

  digitalWrite(8, LOW);

}
