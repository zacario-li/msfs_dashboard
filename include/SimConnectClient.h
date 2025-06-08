#ifndef SIMCONNECTCLIENT_H
#define SIMCONNECTCLIENT_H

#include <QObject>
#include <QTimer>
#include <windows.h>
#include "SimConnect.h"

// Data structure to hold aircraft data received from SimConnect
struct AircraftData {
    double gear_total_extended_pct;
    double parking_brake_position;
    double autopilot_master;
    double attitude_bank_radians;
    double attitude_pitch_radians;
    double gear_handle_position;
};

class SimConnectClient : public QObject
{
    Q_OBJECT

public:
    explicit SimConnectClient(QObject *parent = nullptr);
    ~SimConnectClient();

    bool isConnected() const;

public slots:
    void connectToSim();
    void disconnectFromSim();
    void toggleGear();
    void toggleParkingBrake();
    void toggleAutopilot();

signals:
    void connected();
    void disconnected();
    void aircraftDataUpdated(const AircraftData &data);

private slots:
    void processSimConnectEvents();

private:
    static void CALLBACK dispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);
    void setupDataRequests();
    void setupEvents();

    HANDLE hSimConnect = nullptr;
    QTimer* processTimer;

    enum class DEFINITION_ID {
        AIRCRAFT_DATA,
    };

    enum class REQUEST_ID {
        AIRCRAFT_DATA,
    };
    
    enum class EVENT_ID {
        GEAR_TOGGLE,
        PARKING_BRAKE_TOGGLE,
        AP_MASTER_TOGGLE,
    };
};

#endif // SIMCONNECTCLIENT_H 