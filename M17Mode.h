
#include "Config.h"
#include "AbstractMode.h"

#if defined(MODE_M17)

#if !defined(M17MODE_H)
#define  M17MODE_H


class CM17Mode : public AbstractMode {
public:
  CM17Mode(MMDVM_STATE* m);

private:

};

#endif

#endif

