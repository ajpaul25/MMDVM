
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_P25)

#if !defined(P25MODE_H)
#define  P25MODE_H


class CP25Mode : public AbstractMode {
public:
  CP25Mode(MMDVM_STATE* m, bool* en);

private:

};

#endif

#endif

