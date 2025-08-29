
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_DSTAR)

#if !defined(DSTARMODE_H)
#define  DSTARMODE_H


class CDStarMode : public AbstractMode {
public:
  CDStarMode(MMDVM_STATE* m, bool* en);
  uint8_t setConfig(const uint8_t* data, uint16_t length);

private:

};

#endif

#endif

