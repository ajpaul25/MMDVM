
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_NXDN)

#if !defined(NXDNMODE_H)
#define  NXDNMODE_H


class CNXDNMode : public AbstractMode {
public:
  CNXDNMode(MMDVM_STATE* m, bool* en);

private:

};

#endif

#endif

