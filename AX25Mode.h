
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_AX25)

#if !defined(AX25MODE_H)
#define  AX25MODE_H


class CAX25Mode : public AbstractMode {
public:
  CAX25Mode(MMDVM_STATE* m, bool* en);

private:

};

#endif

#endif

