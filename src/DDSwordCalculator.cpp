#include "DDSwordCalculator.h"

uint64_t DDSwordCalculatorClass::ArrayToSingle(uint8_t DDSresolution, uint8_t *DDSwordArray) {
  uint64_t data = 0;
  if (DDSresolution >= 8 && DDSresolution <= 48) {
    uint8_t MaximumDDSwordElement = DDSresolution;
    MaximumDDSwordElement /= 8;
    if ((DDSresolution % 8) == 0) {
      MaximumDDSwordElement--;
    }
    for (int i = 0; i <= MaximumDDSwordElement; i++) {
      data <<= 8;
      data |= DDSwordArray[i];
    }
  }
  return data;
}

void DDSwordCalculatorClass::convert(char *DDSreference, char *DDSfrequency, uint8_t DDSresolution, uint8_t *DDSword) {
  if (DDSresolution >= 8 && DDSresolution <= 48) {
    uint8_t MaximumDDSwordElement = DDSresolution;
    MaximumDDSwordElement /= 8;
    if ((DDSresolution % 8) == 0) {
      MaximumDDSwordElement--;
    }
    BigNumber::begin(20); // can handle up to 1 THz
    BigNumber BN_DDSreference = BigNumber(DDSreference);
    BigNumber BN_DDSresult = BigNumber(DDSfrequency);
    if (BN_DDSresult <= BN_DDSreference) {
      BigNumber BN_temp = 2;
      BigNumber BN_DDSsteps = BN_temp.pow(DDSresolution);
      // BigNumber is symmetrical in integers and decimal places, so we are optimizing for a balance
      while(true) {
        if (BN_DDSreference <= BigNumber(1000)) {
          break;
        }
        BN_DDSreference /= BigNumber(1000);
        BN_DDSresult /= BigNumber(1000);
      }
      BN_DDSresult *= BN_DDSsteps;
      BN_DDSresult /= BN_DDSreference;
      BN_DDSresult += BigNumber("0.5"); // rounding
      BN_DDSsteps -= BigNumber(1);
      if (BN_DDSresult >= BN_DDSsteps) { // deal with overflow
        BN_DDSresult = BN_DDSsteps;
      }
      MaximumDDSwordElement *= 8;
      BN_DDSsteps = BN_temp.pow(MaximumDDSwordElement);
      MaximumDDSwordElement /= 8;
      for (int i = 0; i <= MaximumDDSwordElement; i++) {
        BN_temp = BN_DDSresult;
        BN_temp /= BN_DDSsteps;
        uint8_t DDSelementValue = (uint32_t)((uint32_t) BN_temp);
        BN_temp = BigNumber(DDSelementValue); // decimal has been rounded off
        BN_temp *= BN_DDSsteps;
        BN_DDSresult -= BN_temp;
        DDSword[i] = DDSelementValue;
        BN_DDSsteps /= BigNumber(256);
      }
    }
    BigNumber::finish();
  }
}

DDSwordCalculatorClass DDSwordCalculator;