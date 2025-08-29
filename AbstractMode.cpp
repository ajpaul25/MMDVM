#include "AbstractMode.h"

AbstractMode::AbstractMode(MMDVM_STATE* m)
{
    m_modemState=m;
    //m_idlerx = 0;
    //m_rx = 0;
    //m_tx = 0;
    //m_calrx = 0;
    //m_caltx = 0;
    //m_caltx->tx = *m_tx;
    //m_orx = 0;
    //m_otx = 0;
    //m_spacepos = 0;
    //m_spacelen = 0;
    //m_stateid = 0;
    //m_calstateid = 0;
    //m_enabled = 0;
    m_filtertaps = {};
    //m_filterlen = 0;
    //m_filterStateSize = 0;
    //m_firFilter = ;
    //m_txlevel = 0;
    //filterInit();
}

AbstractMode::~AbstractMode(void) {}

bool AbstractMode::condition()
{
    return enabled && *m_modemState == stateid;
}

bool AbstractMode::ocondition()
{
    return false;
}

bool AbstractMode::calcondition()
{
    return *m_modemState == calstateid;
}

void AbstractMode::filterInit()
{
    q15_t state[filterStateSize];
    ::memset(state, 0x00U, filterStateSize * sizeof(q15_t));
    m_firFilter.numTaps = filterlen;
    m_firFilter.pState = state;
    m_firFilter.pCoeffs = m_filtertaps;
}

void AbstractMode::setTXStart(bool on)
{
    m_tx->setStart(on);
}

void AbstractMode::setTXLevel(q15_t level, bool txInvert)
{
    m_txlevel = m_txlevel * 128 * txInvert ? -1 : 1;
}

