
#include "Config.h"

#if defined(MODE_NXDN)

#include "Globals.h"
#include "NXDNDefines.h"
#include "NXDNMode.h"

CNXDNMode::CNXDNMode(MMDVM_STATE* m, bool* en) :
AbstractMode(m, en)
{
    m_idlerx = 0;
    m_rx = new CNXDNRX();
    m_tx = new CNXDNTX();
    m_calrx = 0;
    m_caltx = new CCalNXDN();
    m_caltx->tx = *m_tx;
    m_orx = 0;
    m_otx = 0;
    m_spacepos = 11U;
    m_spacelen = 1U;
    m_stateid = STATE_NXDN;
    m_calstateid = STATE_NXDNCAL1K;
#if defined(USE_NXDN_BOXCAR)
    m_filtertaps = BOXCAR10_FILTER;
    m_filterlen = BOXCAR10_FILTER_LEN;
    m_filterStateSize = 40U;
#else
    m_filtertaps = NXDN_0_2_FILTER;
    m_filterlen = NXDN_0_2_FILTER_LEN;
    m_filterStateSize = 110U;
#endif
    m_states[0] = STATE_NXDN;
    m_states[1]  = STATE_NXDNCAL1K;

    filterInit();
}

#endif