#ifndef ABSTRACTMODE_H
#define ABSTRACTMODE_H

#include "InterfaceRX.h"
#include "InterfaceTX.h"
#include "InterfaceCal.h"
#include "Filters.h"

class AbstractMode {
protected:
    InterfaceRX* m_idlerx;
    InterfaceRX* m_rx;
    InterfaceTX* m_tx;
    InterfaceCal* m_calrx;
    InterfaceCal* m_caltx;
    InterfaceRX* m_orx;
    InterfaceTX* m_otx;
    uint8_t m_spacelen;
    uint8_t m_spacepos;
    uint8_t m_stateid;
    uint8_t m_calstateid;
    bool* m_enabled;
    q15_t *m_filtertaps;
    uint16_t m_filterlen;
    uint8_t m_filterStateSize;
    arm_fir_instance_q15 m_firFilter;
    q15_t m_txlevel;
    MMDVM_STATE* m_modemState;
    uint8_t m_states[10];
    void filterInit();
    uint16_t m_conf_en_mask;

public:
    AbstractMode(MMDVM_STATE* m, bool* en);
    virtual ~AbstractMode(void);
    InterfaceRX* idlerx = m_idlerx;
    InterfaceRX* rx = m_rx;
    InterfaceTX* tx = m_tx;
    InterfaceCal* calrx = m_calrx;
    InterfaceCal* caltx = m_caltx;
    InterfaceRX* orx = m_orx;
    InterfaceTX* otx = m_otx;
    const uint8_t& spacelen = m_spacelen;
    const uint8_t& spacepos = m_spacepos;
    const uint8_t& stateid = m_stateid;
    const uint8_t& calstateid = m_calstateid;
    bool* enabled = m_enabled;
    const q15_t* filtertaps = m_filtertaps;
    const uint16_t& filterlen = m_filterlen;
    const uint8_t& filterStateSize = m_filterStateSize;
    const arm_fir_instance_q15& firFilter = m_firFilter;
    const q15_t& txlevel = m_txlevel;

    bool condition();
    bool ocondition();
    bool calcondition();

    void setTXStart(bool on);
    void setTXLevel(q15_t level,bool txInvert=false);
    bool hasState(MMDVM_STATE state);
    uint8_t setConfig(const uint8_t* data, uint16_t length);


};

#endif // ABSTRACTMODE_H