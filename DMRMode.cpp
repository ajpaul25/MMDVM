
#include "Config.h"

#if defined(MODE_DMR)

#include "Globals.h"
#include "DMRDefines.h"
#include "DMRMode.h"

CDMRMode::CDMRMode(MMDVM_STATE* m, bool* en) :
AbstractMode(m, en)
{
    m_idlerx = new CDMRIdleRX();
    m_rx = new CDMRRX();
    m_tx = new CDMRTX();
    m_calrx = 0;
    m_caltx = new CCalDMR();
    m_caltx->tx = *m_tx;
    m_orx = new CDMRDMORX();
    m_otx = new CDMRDMOTX();
    m_spacepos = 7U;
    m_spacelen = 2U;
    m_stateid = STATE_DMR;
    m_calstateid = 0;
    m_filtertaps = RRC_0_2_FILTER;
    m_filterlen = RRC_0_2_FILTER_LEN;
    m_filterStateSize = 70U;
    m_states[0] = STATE_DMR;
    m_states[1] = STATE_DMRDMO1K;
    m_states[2] = STATE_DMRCAL;
    m_states[3] = STATE_DMRCAL1K;

    filterInit();
}

#endif