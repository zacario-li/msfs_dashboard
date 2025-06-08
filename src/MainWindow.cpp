#include "MainWindow.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_simConnectClient = new SimConnectClient(this);
    setupUi();

    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_gearButton, &QPushButton::clicked, m_simConnectClient, &SimConnectClient::toggleGear);
    connect(m_brakeButton, &QPushButton::clicked, m_simConnectClient, &SimConnectClient::toggleParkingBrake);
    connect(m_apButton, &QPushButton::clicked, m_simConnectClient, &SimConnectClient::toggleAutopilot);

    connect(m_simConnectClient, &SimConnectClient::connected, this, &MainWindow::onSimConnected);
    connect(m_simConnectClient, &SimConnectClient::disconnected, this, &MainWindow::onSimDisconnected);
    connect(m_simConnectClient, &SimConnectClient::aircraftDataUpdated, this, &MainWindow::onAircraftDataUpdated);

    updateControlsState(false);
    onSimDisconnected(); // Set initial state
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_layout = new QGridLayout(m_centralWidget);

    // --- Connection Group ---
    QGroupBox *connectionGroup = new QGroupBox("Connection");
    QHBoxLayout *connectionLayout = new QHBoxLayout;
    m_connectButton = new QPushButton("Connect");
    m_statusLight = new QLabel();
    m_statusLight->setFixedSize(20, 20);
    m_statusLight->setStyleSheet("border-radius: 10px; background-color: red;");
    connectionLayout->addWidget(m_connectButton);
    connectionLayout->addWidget(m_statusLight);
    connectionGroup->setLayout(connectionLayout);

    // --- Controls Group ---
    QGroupBox *controlsGroup = new QGroupBox("Controls");
    QVBoxLayout *controlsLayout = new QVBoxLayout;
    m_gearButton = new QPushButton("Toggle Gear");
    m_gearButton->setCheckable(true);
    m_brakeButton = new QPushButton("Parking Brake");
    m_brakeButton->setCheckable(true);
    m_apButton = new QPushButton("Autopilot");
    m_apButton->setCheckable(true);
    controlsLayout->addWidget(m_gearButton);
    controlsLayout->addWidget(m_brakeButton);
    controlsLayout->addWidget(m_apButton);
    controlsGroup->setLayout(controlsLayout);

    // --- Data Display Group ---
    QGroupBox *dataGroup = new QGroupBox("Data");
    QVBoxLayout *dataLayout = new QVBoxLayout;
    m_gearLabel = new QLabel("Gear: ---%");
    dataLayout->addWidget(m_gearLabel);
    dataGroup->setLayout(dataLayout);

    // --- Attitude Indicator ---
    m_attitudeIndicator = new AttitudeIndicator();

    // --- Layout ---
    m_layout->addWidget(connectionGroup, 0, 0);
    m_layout->addWidget(controlsGroup, 1, 0);
    m_layout->addWidget(dataGroup, 2, 0);
    m_layout->addWidget(m_attitudeIndicator, 0, 1, 3, 1);
    m_layout->setColumnStretch(1, 1);
}


void MainWindow::onConnectClicked()
{
    if (m_simConnectClient->isConnected())
    {
        m_simConnectClient->disconnectFromSim();
    }
    else
    {
        m_simConnectClient->connectToSim();
    }
}

void MainWindow::onSimConnected()
{
    m_connectButton->setText("Disconnect");
    m_statusLight->setStyleSheet("border-radius: 10px; background-color: green;");
    updateControlsState(true);
}

void MainWindow::onSimDisconnected()
{
    m_connectButton->setText("Connect");
    m_statusLight->setStyleSheet("border-radius: 10px; background-color: red;");
    updateControlsState(false);
    
    // Reset UI to default state
    m_gearLabel->setText("Gear: ---%");
    m_attitudeIndicator->setRoll(0);
    m_attitudeIndicator->setPitch(0);
    m_gearButton->setChecked(false);
    m_brakeButton->setChecked(false);
    m_apButton->setChecked(false);
}

void MainWindow::onAircraftDataUpdated(const AircraftData &data)
{
    // Update Gear
    m_gearLabel->setText(QString("Gear: %1%").arg(data.gear_total_extended_pct * 100.0, 0, 'f', 1));
    m_gearButton->setChecked(data.gear_handle_position > 0.5);

    // Update Brakes
    m_brakeButton->setChecked(data.parking_brake_position > 0.5);

    // Update Autopilot
    m_apButton->setChecked(data.autopilot_master > 0.5);

    // Update Attitude Indicator
    float roll_deg = static_cast<float>(data.attitude_bank_radians * 180.0 / M_PI);
    float pitch_deg = static_cast<float>(data.attitude_pitch_radians * 180.0 / M_PI);
    m_attitudeIndicator->setRoll(roll_deg);
    m_attitudeIndicator->setPitch(pitch_deg);
}

void MainWindow::updateControlsState(bool isConnected)
{
    m_gearButton->setEnabled(isConnected);
    m_brakeButton->setEnabled(isConnected);
    m_apButton->setEnabled(isConnected);
} 