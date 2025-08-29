#include "InterfaceRX.h"
#include "Globals.h"

InterfaceRX::InterfaceRX(void) {}
InterfaceRX::~InterfaceRX(void) {}
uint8_t InterfaceRX::setConfig(const uint8_t* data, uint16_t length) {return 0;};
void InterfaceRX::samples(const q15_t* samples, const uint16_t* rssi, uint8_t length) {};
void InterfaceRX::reset() {};
InterfaceRX rx;

void InterfaceRX::writeData(const uint8_t* data, uint8_t length){}
void InterfaceRX::writeLost(){}
void InterfaceRX::writeEOT(){}


