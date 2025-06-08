#include "SimConnectClient.h"
#include <loguru.hpp>

SimConnectClient::SimConnectClient(QObject *parent) : QObject(parent)
{
    processTimer = new QTimer(this);
    connect(processTimer, &QTimer::timeout, this, &SimConnectClient::processSimConnectEvents);
}

SimConnectClient::~SimConnectClient()
{
    disconnectFromSim();
}

bool SimConnectClient::isConnected() const
{
    return hSimConnect != nullptr;
}

void SimConnectClient::connectToSim()
{
    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "MSFS Dashboard", nullptr, 0, 0, 0)))
    {
        LOG_F(INFO, "Connected to MSFS.");
        emit connected();

        setupDataRequests();
        setupEvents();

        processTimer->start(16); // ~60 FPS
    }
    else
    {
        LOG_F(ERROR, "Failed to connect to MSFS.");
    }
}

void SimConnectClient::disconnectFromSim()
{
    if (hSimConnect)
    {
        processTimer->stop();
        SimConnect_Close(hSimConnect);
        hSimConnect = nullptr;
        LOG_F(INFO, "Disconnected from MSFS.");
        emit disconnected();
    }
}

void SimConnectClient::processSimConnectEvents()
{
    if(hSimConnect)
    {
        SimConnect_CallDispatch(hSimConnect, dispatchProc, this);
    }
}

void SimConnectClient::transmitEvent(EVENT_ID eventId, DWORD data)
{
    if (hSimConnect)
    {
        VLOG_F(1, "Transmitting SimConnect event: ID=%d, data=%lu", static_cast<int>(eventId), data);
        SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER, eventId, data, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
    }
    else
    {
        LOG_F(WARNING, "Cannot transmit event - SimConnect not connected");
    }
}

void CALLBACK SimConnectClient::dispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
    SimConnectClient* client = static_cast<SimConnectClient*>(pContext);

    switch (pData->dwID)
    {
        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
        {
            SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

            if (pObjData->dwRequestID == static_cast<DWORD>(REQUEST_ID::AIRCRAFT_DATA))
            {
                AircraftData* pS = (AircraftData*)&pObjData->dwData;
                VLOG_F(3, "Received aircraft data update");
                emit client->aircraftDataUpdated(*pS);
            }
            break;
        }

        case SIMCONNECT_RECV_ID_QUIT:
        {
            LOG_F(INFO, "Received SimConnect quit signal");
            client->disconnectFromSim();
            break;
        }

        default:
            VLOG_F(3, "Received unknown SimConnect message: ID=%lu", pData->dwID);
            break;
    }
}

void SimConnectClient::setupDataRequests()
{
    if (!hSimConnect) {
        LOG_F(WARNING, "Cannot setup data requests - SimConnect not connected");
        return;
    }
    
    LOG_F(INFO, "Setting up SimConnect data requests...");

    // Define the data structure
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR TOTAL PCT EXTENDED", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "BRAKE PARKING INDICATOR", "Bool");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "AUTOPILOT MASTER", "Bool");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "ATTITUDE INDICATOR BANK DEGREES", "Radians");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "ATTITUDE INDICATOR PITCH DEGREES", "Radians");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR HANDLE POSITION", "Bool");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "PLANE HEADING DEGREES TRUE", "Degrees");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR DAMAGE BY SPEED", "Bool");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR WARNING:0", "Number");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR WARNING:1", "Number");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR WARNING:2", "Number");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR CENTER POSITION", "Percent Over 100");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR LEFT POSITION", "Percent Over 100");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GEAR RIGHT POSITION", "Percent Over 100");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "TURB ENG N1:1", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "TURB ENG N1:2", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "TURB ENG N1:3", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "TURB ENG N1:4", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GENERAL ENG THROTTLE LEVER POSITION:1", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GENERAL ENG THROTTLE LEVER POSITION:2", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GENERAL ENG THROTTLE LEVER POSITION:3", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GENERAL ENG THROTTLE LEVER POSITION:4", "Percent");

    // Request data periodically
    SimConnect_RequestDataOnSimObject(hSimConnect, static_cast<SIMCONNECT_DATA_REQUEST_ID>(REQUEST_ID::AIRCRAFT_DATA), static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);
    
    LOG_F(INFO, "SimConnect data requests setup complete");
}

void SimConnectClient::setupEvents()
{
    if (!hSimConnect) {
        LOG_F(WARNING, "Cannot setup events - SimConnect not connected");
        return;
    }
    
    LOG_F(INFO, "Setting up SimConnect events...");

    // Map client events to SimEvents
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_TOGGLE_FLIGHT_DIRECTOR, "TOGGLE_FLIGHT_DIRECTOR");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_MASTER, "AP_MASTER");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_TOGGLE_NAV_GPS, "TOGGLE_GPS_DRIVES_NAV1");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_NAV1_HOLD, "AP_NAV1_HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_APR_HOLD, "AP_APR_HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_BC_HOLD, "AP_BC_HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_WING_LEVELER, "AP_WING_LEVELER_HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_ALT_HOLD, "AP_ALT_HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_VS_HOLD, "AP_VS_HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_FLC_HOLD, "AP_FL_CHANGE_HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_HEADING_BUG_SET, "HEADING_BUG_SET");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_SPD_VAR_SET, "AP_SPD_VAR_SET");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_ALT_VAR_SET, "AP_ALT_VAR_SET");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AP_VS_VAR_SET, "AP_VS_VAR_SET");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_AUTO_THROTTLE_ARM, "AUTO_THROTTLE_ARM");

    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_TOGGLE_ENGINE1_STARTER, "TOGGLE_ENGINE1_STARTER");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_TOGGLE_ENGINE2_STARTER, "TOGGLE_ENGINE2_STARTER");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_TOGGLE_ENGINE3_STARTER, "TOGGLE_ENGINE3_STARTER");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_TOGGLE_ENGINE4_STARTER, "TOGGLE_ENGINE4_STARTER");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_ENGINE_AUTO_SHUTDOWN, "ENGINE_AUTO_SHUTDOWN");

    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_GEAR_UP, "GEAR_UP");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_GEAR_DOWN, "GEAR_DOWN");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_FLAPS_UP, "FLAPS_UP");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_FLAPS_DOWN, "FLAPS_DOWN");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_PARKING_BRAKES, "PARKING_BRAKES");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_SPOILERS_ARM, "SPOILERS_ARM_TOGGLE");
    
    LOG_F(INFO, "SimConnect events setup complete");
} 