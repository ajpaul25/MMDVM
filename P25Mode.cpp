
#include "Config.h"

#if defined(MODE_P25)

#include "Globals.h"
#include "P25Defines.h"
#include "P25Mode.h"

CP25Mode::CP25Mode(MMDVM_STATE* m) :
AbstractMode(m)
{
    m_idlerx = 0;
    m_rx = new CP25RX();
    m_tx = new CP25TX();
    m_calrx = 0;
    m_caltx = new CCalP25();
    m_caltx->tx = *m_tx;
    m_orx = 0;
    m_otx = 0;
    m_spacepos = 10U;
    m_spacelen = 1U;
    m_stateid = STATE_P25;
    m_calstateid = STATE_P25CAL1K;
    m_filtertaps = BOXCAR5_FILTER;
    m_filterlen = BOXCAR5_FILTER_LEN;
    m_filterStateSize = 30U;

    filterInit();
}

#endif