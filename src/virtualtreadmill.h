#ifndef VIRTUALTREADMILL_H
#define VIRTUALTREADMILL_H

#include <QObject>

#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>
#include <QtCore/qbytearray.h>
#ifndef Q_OS_ANDROID
#include <QtCore/qcoreapplication.h>
#else
#include <QtGui/qguiapplication.h>
#endif
#include <QtCore/qlist.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qtimer.h>

#include "dirconmanager.h"
#include "treadmill.h"

class virtualtreadmill : public QObject {
    Q_OBJECT
  public:
    virtualtreadmill(bluetoothdevice *t, bool noHeartService);
    bool connected();

  private:
    QLowEnergyController *leController = nullptr;
    QLowEnergyService *service = nullptr;
    QLowEnergyService *serviceHR = nullptr;
    QLowEnergyAdvertisingData advertisingData;
    QLowEnergyServiceData serviceData;
    QLowEnergyServiceData serviceDataHR;
    QTimer treadmillTimer;
    bluetoothdevice *treadMill;
    CharacteristicWriteProcessor2AD9 *writeP2AD9 = 0;
    CharacteristicNotifier2AD2 *notif2AD2 = 0;
    CharacteristicNotifier2A53 *notif2A53 = 0;
    CharacteristicNotifier2ACD *notif2ACD = 0;
    CharacteristicNotifier2A37 *notif2A37 = 0;
    DirconManager *dirconManager = 0;

    bool noHeartService = false;

    void slopeChanged(int16_t iresistance);

  signals:
    void debug(QString string);
    void changeInclination(double grade, double percentage);

  private slots:
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void treadmillProvider();
    void reconnect();
};

#endif // VIRTUALTREADMILL_H
