#include "SimConnectClient.h"
#include <QDebug>

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
        qDebug() << "Connected to MSFS.";
        emit connected();

        setupDataRequests();
        setupEvents();

        processTimer->start(16); // ~60 FPS
    }
    else
    {
        qDebug() << "Failed to connect to MSFS.";
    }
}

void SimConnectClient::disconnectFromSim()
{
    if (hSimConnect)
    {
        processTimer->stop();
        SimConnect_Close(hSimConnect);
        hSimConnect = nullptr;
        qDebug() << "Disconnected from MSFS.";
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

void SimConnectClient::setGear(bool down)
{
    if (hSimConnect)
    {
        SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(EVENT_ID::GEAR_SET_EVENT), down, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
    }
}

void SimConnectClient::toggleParkingBrake()
{
    if (hSimConnect)
    {
        SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(EVENT_ID::PARKING_BRAKE_TOGGLE), 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
    }
}

void SimConnectClient::toggleAutopilot()
{
    if (hSimConnect)
    {
        SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(EVENT_ID::AP_MASTER_TOGGLE), 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
    }
}

void SimConnectClient::setEngineState(int engineIndex, bool start)
{
    if (hSimConnect)
    {
        EVENT_ID event = start ? EVENT_ID::ENGINE_START : EVENT_ID::ENGINE_SHUTDOWN;
        SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(event), engineIndex, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
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
                emit client->aircraftDataUpdated(*pS);
            }
            break;
        }

        case SIMCONNECT_RECV_ID_QUIT:
        {
            client->disconnectFromSim();
            break;
        }

        default:
            break;
    }
}

void SimConnectClient::setupDataRequests()
{
    if (!hSimConnect) return;

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
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GENERAL ENG RPM:1", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GENERAL ENG RPM:2", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GENERAL ENG RPM:3", "Percent");
    SimConnect_AddToDataDefinition(hSimConnect, static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), "GENERAL ENG RPM:4", "Percent");
    
    // Request data periodically
    SimConnect_RequestDataOnSimObject(hSimConnect, static_cast<SIMCONNECT_DATA_REQUEST_ID>(REQUEST_ID::AIRCRAFT_DATA), static_cast<SIMCONNECT_DATA_DEFINITION_ID>(DEFINITION_ID::AIRCRAFT_DATA), SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);
}

void SimConnectClient::setupEvents()
{
    if (!hSimConnect) return;
    
    // Map client events to SimEvents
    SimConnect_MapClientEventToSimEvent(hSimConnect, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(EVENT_ID::GEAR_SET_EVENT), "GEAR_SET");
    SimConnect_MapClientEventToSimEvent(hSimConnect, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(EVENT_ID::PARKING_BRAKE_TOGGLE), "PARKING_BRAKES");
    SimConnect_MapClientEventToSimEvent(hSimConnect, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(EVENT_ID::AP_MASTER_TOGGLE), "AP_MASTER");
    SimConnect_MapClientEventToSimEvent(hSimConnect, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(EVENT_ID::ENGINE_START), "ENGINE_AUTO_START");
    SimConnect_MapClientEventToSimEvent(hSimConnect, static_cast<SIMCONNECT_CLIENT_EVENT_ID>(EVENT_ID::ENGINE_SHUTDOWN), "ENGINE_AUTO_SHUTDOWN");
} 