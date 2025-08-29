
#include "Config.h"

#if defined(MODE_POCSAG)

#include "Globals.h"
//#include "POCSAGDefines.h"
#include "POCSAGMode.h"

CPOCSAGMode::CPOCSAGMode(MMDVM_STATE* m) :
AbstractMode(m)
{
    m_idlerx = 0;
    m_rx = 0;
    m_tx = new CPOCSAGTX();
    m_calrx = 0;
    m_caltx = new CCalPOCSAG();
    m_caltx->tx = *m_tx;
    m_orx = 0;
    m_otx = 0;
    m_spacepos = 14U;
    m_spacelen = 1U;
    m_stateid = STATE_POCSAG;
    m_calstateid = STATE_POCSAGCAL;
    //m_filtertaps = {MODE_FILTER_NAME};
    //m_filterlen = {MODE_FILTER_LENGTH};
    //m_filterStateSize = {MODE_FILTER_STATE_SIZE};

    filterInit();
}

bool CPOCSAGMode::condition()
{
    return AbstractMode::condition() && (static_cast<CPOCSAGTX*>(m_tx))->busy();
}
#endif