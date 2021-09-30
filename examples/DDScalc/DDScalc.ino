/*

  DDSwordCalculator by Bryce Cherry
  Enter a reference and desired frequency in Hz and bit resolution (8-48) and it will return a hexadecimal frequency word
  Due to limitations of Serial.print, only the 32 least significant bits will be printed from conversion with ArrayToSingle

*/

#include <DDSwordCalculator.h>

const byte DDSarrayElements = 6;

// ensures that the serial port is flushed fully on request
const unsigned long SerialPortRate = 9600;
const byte SerialPortRateTolerance = 5; // +percent - increase to 50 for rates above 115200 up to 4000000
const byte SerialPortBits = 10; // start (1), data (8), stop (1)
const unsigned long TimePerByte = ((((1000000ULL * SerialPortBits) / SerialPortRate) * (100 + SerialPortRateTolerance)) / 100); // calculated on serial port rate + tolerance and rounded down to the nearest uS, long caters for even the slowest serial port of 75 bps

const word commandSize = 50;
char command[commandSize];
const byte FieldSize = 30;

void FlushSerialBuffer() {
  while (true) {
    if (Serial.available() > 0) {
      byte dummy = Serial.read();
      while (Serial.available() > 0) { // flush additional bytes from serial buffer if present
        dummy = Serial.read();
      }
      if (TimePerByte <= 16383) {
        delayMicroseconds(TimePerByte); // delay in case another byte may be received via the serial port
      }
      else { // deal with delayMicroseconds limitation
        unsigned long DelayTime = TimePerByte;
        DelayTime /= 1000;
        if (DelayTime > 0) {
          delay(DelayTime);
        }
        DelayTime = TimePerByte;
        DelayTime %= 1000;
        if (DelayTime > 0) {
          delayMicroseconds(DelayTime);
        }
      }
    }
    else {
      break;
    }
  }
}

void getField (char* buffer, int index) {
  int commandPos = 0;
  int fieldPos = 0;
  int spaceCount = 0;
  while (commandPos < commandSize) {
    if (command[commandPos] == 0x20) {
      spaceCount++;
      commandPos++;
    }
    else if (command[commandPos] == 0x0D || command[commandPos] == 0x0A) {
      break;
    }
    if (spaceCount == index) {
      buffer[fieldPos] = command[commandPos];
      fieldPos++;
    }
    commandPos++;
  }
  for (int ch = 0; ch < strlen(buffer); ch++) { // correct case of command
    buffer[ch] = toupper(buffer[ch]);
  }
  buffer[fieldPos] = '\0';
}

void setup() {
  Serial.begin(SerialPortRate);
}

void loop() {
  static int ByteCount = 0;
  if (Serial.available() > 0) {
    char inData = Serial.read();
    if (inData != '\n' && ByteCount < commandSize) {
      command[ByteCount] = inData;
      ByteCount++;
    }
    else {
      ByteCount = 0;
      char field1[FieldSize];
      char field2[FieldSize];
      char field3[3];
      uint8_t DDSdata[DDSarrayElements];
      for (int i = 0; i < DDSarrayElements; i++) {
        DDSdata[i] = 0x00;
      }
      getField(field1, 0);
      getField(field2, 1);
      getField(field3, 2);
      uint8_t DDSbits = atoi(field3);
      DDSwordCalculator.convert(field1, field2, DDSbits, DDSdata);
      uint32_t SingleDDSword = DDSwordCalculator.ArrayToSingle(DDSbits, DDSdata);
      uint8_t DDSarrayElementsToPrint = DDSbits;
      DDSarrayElementsToPrint /= 8;
      if ((DDSbits % 8) == 0) {
        DDSarrayElementsToPrint--;
      }
      Serial.print(F("DDS array contents: "));
      for (int i = 0; i <= DDSarrayElementsToPrint; i++) {
        if (DDSdata[i] < 0x10) { // pad with zero
          Serial.print(F("0"));
        }
        Serial.print(DDSdata[i], HEX);
      }
      Serial.println(F(""));
      Serial.print(F("DDS single element contents (32 LSBs): "));
      Serial.println(SingleDDSword, HEX);
      FlushSerialBuffer();
      Serial.println(F("OK"));
    }
  }
}