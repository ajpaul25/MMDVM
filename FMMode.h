
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_FM)

#if !defined(FMMODE_H)
#define  FMMODE_H


class CFMMode : public AbstractMode {
public:
  CFMMode(MMDVM_STATE* m, bool* en);

private:
  bool calcondition();
};

#endif

#endif

