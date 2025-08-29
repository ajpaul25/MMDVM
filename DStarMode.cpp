
#include "Config.h"

#if defined(MODE_DSTAR)

#include "Globals.h"
#include "DStarDefines.h"
#include "DStarMode.h"

CDStarMode::CDStarMode(MMDVM_STATE* m) :
AbstractMode(m)
{
    m_idlerx = 0;
    m_rx = new CDStarRX();
    m_tx = new CDStarTX();
    m_calrx = new CCalDStarRX();
    m_caltx = new CCalDStarTX();
    m_caltx->tx = *m_tx;
    m_orx = 0;
    m_otx = 0;
    m_spacepos = 6U;
    m_spacelen = 1U;
    m_stateid = STATE_DSTAR;
    m_calstateid = STATE_DSTARCAL;
    //m_enabled = &m_dstarEnable;
    m_filtertaps = GAUSSIAN_0_5_FILTER;
    m_filterlen = GAUSSIAN_0_5_FILTER_LEN;
    m_filterStateSize = 40U;

    filterInit();
}

#endif