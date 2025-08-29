
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_YSF)

#if !defined(YSFMODE_H)
#define  YSFMODE_H


class CYSFMode : public AbstractMode {
public:
  CYSFMode(MMDVM_STATE* m, bool* en);

private:

};

#endif

#endif

