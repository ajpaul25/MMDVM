
#include "Config.h"

#if defined(MODE_FM)

#include "Globals.h"
//#include "FMDefines.h"
#include "FMMode.h"

CFMMode::CFMMode(MMDVM_STATE* m, bool* en) :
AbstractMode(m, en)
{
    m_idlerx = 0;
    m_rx = 0;
    m_tx = new CFM();
    m_calrx = 0;
    m_caltx = new CCalFM();
    m_caltx->tx = *m_tx;
    m_orx = 0;
    m_otx = 0;
    m_spacepos = 13U;
    m_spacelen = 1U;
    m_stateid = STATE_FM;
    m_calstateid = 0;
    //m_filtertaps = {MODE_FILTER_NAME};
    //m_filterlen = {MODE_FILTER_LENGTH};
    //m_filterStateSize = {MODE_FILTER_STATE_SIZE};
    m_states[0] = STATE_FM;
    m_states[1] = STATE_FMCAL10K;
    m_states[2] = STATE_FMCAL12K;
    m_states[3] = STATE_FMCAL15K;
    m_states[4] = STATE_FMCAL20K;
    m_states[5] = STATE_FMCAL25K;
    m_states[6] = STATE_FMCAL30K;
    m_stateNames[0] = "FM";
    m_stateNames[1] = "FM 10Khz Calibrate";
    m_stateNames[2] = "FM 12.5Khz Calibrate";
    m_stateNames[3] = "FM 15Khz Calibrate";
    m_stateNames[4] = "FM 20Khz Calibrate";
    m_stateNames[5] = "FM 10Khz Calibrate";
    m_stateNames[6] = "Mode set to FM 30Khz Calibrate";
    m_conf_en_mask = 0x2000;
    m_versionCaps = 0x20;
    m_txLevelAddr = 16U;

    filterInit();
}

bool CFMMode::calcondition()
{
    return *m_modemState == STATE_FMCAL10K || 
        *m_modemState == STATE_FMCAL12K || 
        *m_modemState == STATE_FMCAL15K || 
        *m_modemState == STATE_FMCAL20K || 
        *m_modemState == STATE_FMCAL25K || 
        *m_modemState == STATE_FMCAL30K;
}


#endif
