#include "InterfaceTX.h"
#include "Globals.h"

InterfaceTX::InterfaceTX(void) {}
InterfaceTX::~InterfaceTX(void) {}
void InterfaceTX::process() {}
uint8_t InterfaceTX::getSpace() const {return 0;}
uint8_t InterfaceTX::getSpace(uint8_t index) const {return 0;}
uint8_t InterfaceTX::setConfig(const uint8_t* data, uint16_t length) {return 0;};