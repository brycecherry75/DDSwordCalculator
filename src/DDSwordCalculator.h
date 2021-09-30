#ifndef DDSwordCalculator_h
#define DDSwordCalculator_h

#include <Arduino.h>
#include <BigNumber.h>

class DDSwordCalculatorClass {

  public:
  uint64_t ArrayToSingle(uint8_t DDSresolution, uint8_t *DDSwordArray);
  void convert(char *DDSreference, char *DDSfrequency, uint8_t DDSresolution, uint8_t *DDSword);

};

extern DDSwordCalculatorClass DDSwordCalculator;

#endif