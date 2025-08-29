
#include "Config.h"

#if defined(MODE_M17)

#include "Globals.h"
#include "M17Defines.h"
#include "M17Mode.h"

CM17Mode::CM17Mode(MMDVM_STATE* m, bool* en) :
AbstractMode(m, en)
{
    m_idlerx = 0;
    m_rx = new CM17RX();
    m_tx = new CM17TX();
    m_calrx = 0;
    m_caltx = new CCalM17();;
    m_caltx->tx = *m_tx;
    m_orx = 0;
    m_otx = 0;
    m_spacepos = 12U;
    m_spacelen = 1U;
    m_stateid = STATE_M17;
    m_calstateid = STATE_M17CAL;
    m_filtertaps = RRC_0_5_FILTER;
    m_filterlen = RRC_0_5_FILTER_LEN;
    m_filterStateSize = 70U;
    m_states[0] = STATE_M17;
    m_states[1] = STATE_M17CAL;
    m_conf_en_mask = 0x4000;

    filterInit();
}

#endif