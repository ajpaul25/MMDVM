
#include "Config.h"

#if defined(MODE_YSF)

#include "Globals.h"
#include "YSFDefines.h"
#include "YSFMode.h"

CYSFMode::CYSFMode(MMDVM_STATE* m, bool* en) :
AbstractMode(m, en)
{
    m_idlerx = 0;
    m_rx = new CYSFRX();
    m_tx = new CYSFTX();
    m_calrx = 0;
    m_caltx = 0;
    m_orx = 0;
    m_otx = 0;
    m_spacepos = 9U;
    m_spacelen = 1U;
    m_stateid = STATE_YSF;
    m_calstateid = 0;
    m_filtertaps = RRC_0_2_FILTER;
    m_filterlen = RRC_0_2_FILTER_LEN;
    m_filterStateSize = 70U;
    m_states[0] = STATE_YSF;
    m_stateNames[0] = "System Fusion";
    m_conf_en_mask = 0x0400;
    m_versionCaps = 0x04;
    m_txLevelAddr = 11U;

    filterInit();
}

#endif