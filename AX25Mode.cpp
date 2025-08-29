
#include "Config.h"

#if defined(MODE_AX25)

#include "Globals.h"
#include "AX25Defines.h"
#include "AX25Mode.h"

CAX25Mode::CAX25Mode(MMDVM_STATE* m, bool* en) :
AbstractMode(m,en)
{
    m_idlerx = 0;
    m_rx = new CAX25RX();
    m_tx = new CAX25TX();
    m_calrx = 0;
    m_caltx = 0;
    //m_caltx->tx = *m_tx;
    m_orx = 0;
    m_otx = 0;
    m_spacepos = 15;
    m_spacelen = 1U;
    m_stateid = STATE_AX25;
    m_calstateid = 0;
    //m_filtertaps = {MODE_FILTER_NAME};
    //m_filterlen = {MODE_FILTER_LENGTH};
    //m_filterStateSize = {MODE_FILTER_STATE_SIZE};
    m_states[0] = STATE_AX25;
    m_stateNames[0] = "AX25";
    m_conf_en_mask = 0x020000;
    m_versionCaps = 0x0200;

    filterInit();
}

#endif