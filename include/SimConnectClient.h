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
    double eng_n1_1;
    double eng_n1_2;
    double eng_n1_3;
    double eng_n1_4;
    double throttle_1;
    double throttle_2;
    double throttle_3;
    double throttle_4;
};

class SimConnectClient : public QObject
{
    Q_OBJECT

public:
    enum EVENT_ID
    {
        EVENT_TOGGLE_FLIGHT_DIRECTOR,
        EVENT_AP_MASTER,
        EVENT_TOGGLE_NAV_GPS,
        EVENT_AP_NAV1_HOLD,
        EVENT_AP_APR_HOLD,
        EVENT_AP_BC_HOLD,
        EVENT_AP_WING_LEVELER,
        EVENT_AP_ALT_HOLD,
        EVENT_AP_VS_HOLD,
        EVENT_AP_FLC_HOLD,
        EVENT_HEADING_BUG_SET,
        EVENT_AP_SPD_VAR_SET,
        EVENT_AP_ALT_VAR_SET,
        EVENT_AP_VS_VAR_SET,
        EVENT_AUTO_THROTTLE_ARM,
        EVENT_TOGGLE_ENGINE1_STARTER,
        EVENT_TOGGLE_ENGINE2_STARTER,
        EVENT_TOGGLE_ENGINE3_STARTER,
        EVENT_TOGGLE_ENGINE4_STARTER,
        EVENT_ENGINE_AUTO_SHUTDOWN,
        EVENT_GEAR_UP,
        EVENT_GEAR_DOWN,
        EVENT_FLAPS_UP,
        EVENT_FLAPS_DOWN,
        EVENT_PARKING_BRAKES,
        EVENT_SPOILERS_ARM
    };

    explicit SimConnectClient(QObject *parent = nullptr);
    ~SimConnectClient();

    bool isConnected() const;

public slots:
    void connectToSim();
    void disconnectFromSim();
    void transmitEvent(EVENT_ID eventId, DWORD data = 0);

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
};

#endif // SIMCONNECTCLIENT_H