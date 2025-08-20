#include "InterfaceCal.h"

InterfaceCal::InterfaceCal(void) {}
InterfaceCal::~InterfaceCal(void) {}
void InterfaceCal::process(void){}
InterfaceTX tx;
uint8_t InterfaceCal::setConfig(const uint8_t* data, uint16_t length){return 0;}
uint8_t InterfaceCal::processMessage(uint8_t type, const uint8_t* buffer, uint16_t length){return 0;};
