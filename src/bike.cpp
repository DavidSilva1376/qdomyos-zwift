#include <QDebug>
#include "bike.h"

bike::bike()
{

}

void bike::changeResistance(int8_t resistance) { requestResistance = resistance;}
double bike::currentCrankRevolutions() { return CrankRevs;}
uint16_t bike::lastCrankEventTime() { return LastCrankEventTime;}
metric bike::currentResistance() { return Resistance;}
metric bike::currentCadence() { return Cadence;}
uint8_t bike::fanSpeed() { return FanSpeed; }
bool bike::connected() { return false; }
uint16_t bike::watts() { return 0; }
metric bike::pelotonResistance() { return m_pelotonResistance; }

bluetoothdevice::BLUETOOTH_TYPE bike::deviceType() { return bluetoothdevice::BIKE; }

void bike::clearStats()
{
    elapsed.clear(true);
    Speed.clear(false);
    KCal.clear(true);
    Distance.clear(true);
    Heart.clear(false);
    m_jouls.clear(true);
    elevationAcc = 0;
    m_watt.clear(false);

    m_pelotonResistance.clear(false);
    Cadence.clear(false);
    Resistance.clear(false);
}

void bike::setPaused(bool p)
{
    paused = p;
    elapsed.setPaused(p);
    Speed.setPaused(p);
    KCal.setPaused(p);
    Distance.setPaused(p);
    Heart.setPaused(p);
    m_jouls.setPaused(p);
    m_watt.setPaused(p);
    m_pelotonResistance.setPaused(p);
    Cadence.setPaused(p);
    Resistance.setPaused(p);
}
