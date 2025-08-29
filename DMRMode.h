
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_DMR)

#if !defined(DMRMODE_H)
#define  DMRMODE_H


class CDMRMode : public AbstractMode {
public:
  CDMRMode(MMDVM_STATE* m);

private:

};

#endif

#endif

