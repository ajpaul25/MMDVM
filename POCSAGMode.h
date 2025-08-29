
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_POCSAG)

#if !defined(POCSAGMODE_H)
#define  POCSAGMODE_H


class CPOCSAGMode : public AbstractMode {
public:
  CPOCSAGMode(MMDVM_STATE* m, bool* en);

private:
  bool condition();
};

#endif

#endif

