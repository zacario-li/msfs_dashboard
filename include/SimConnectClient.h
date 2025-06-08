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
    double plane_heading_degrees_true;
    double gear_damage_by_speed;
    double gear_warning_center;
    double gear_warning_left;
    double gear_warning_right;
    double gear_pos_center;
    double gear_pos_left;
    double gear_pos_right;
    double eng_rpm_1;
    double eng_rpm_2;
    double eng_rpm_3;
    double eng_rpm_4;
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
    void setGear(bool down);
    void toggleParkingBrake();
    void toggleAutopilot();
    void setEngineState(int engineIndex, bool start);

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
        GEAR_SET_EVENT,
        PARKING_BRAKE_TOGGLE,
        AP_MASTER_TOGGLE,
        ENGINE_START,
        ENGINE_SHUTDOWN
    };
};

#endif // SIMCONNECTCLIENT_H 