#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_simConnectClient = new SimConnectClient(this);

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(ui->gearButton, &QPushButton::clicked, m_simConnectClient, &SimConnectClient::toggleGear);
    connect(ui->brakeButton, &QPushButton::clicked, m_simConnectClient, &SimConnectClient::toggleParkingBrake);
    connect(ui->apButton, &QPushButton::clicked, m_simConnectClient, &SimConnectClient::toggleAutopilot);

    connect(m_simConnectClient, &SimConnectClient::connected, this, &MainWindow::onSimConnected);
    connect(m_simConnectClient, &SimConnectClient::disconnected, this, &MainWindow::onSimDisconnected);
    connect(m_simConnectClient, &SimConnectClient::aircraftDataUpdated, this, &MainWindow::onAircraftDataUpdated);

    updateControlsState(false);
    onSimDisconnected(); // Set initial state
}

MainWindow::~MainWindow()
{
    delete ui;
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
    ui->connectButton->setText("Disconnect");
    ui->statusLight->setStyleSheet("border-radius: 10px; background-color: green;");
    updateControlsState(true);
}

void MainWindow::onSimDisconnected()
{
    ui->connectButton->setText("Connect");
    ui->statusLight->setStyleSheet("border-radius: 10px; background-color: red;");
    updateControlsState(false);
    
    // Reset UI to default state
    ui->gearLabel->setText("Gear: ---%");
    ui->attitudeIndicator->setRoll(0);
    ui->attitudeIndicator->setPitch(0);
    ui->gearButton->setChecked(false);
    ui->brakeButton->setChecked(false);
    ui->apButton->setChecked(false);
}

void MainWindow::onAircraftDataUpdated(const AircraftData &data)
{
    // Update Gear
    ui->gearLabel->setText(QString("Gear: %1%").arg(data.gear_total_extended_pct * 100.0, 0, 'f', 1));
    ui->gearButton->setChecked(data.gear_handle_position > 0.5);

    // Update Brakes
    ui->brakeButton->setChecked(data.parking_brake_position > 0.5);

    // Update Autopilot
    ui->apButton->setChecked(data.autopilot_master > 0.5);

    // Update Attitude Indicator
    float roll_deg = static_cast<float>(data.attitude_bank_radians * 180.0 / M_PI);
    float pitch_deg = static_cast<float>(data.attitude_pitch_radians * 180.0 / M_PI);
    ui->attitudeIndicator->setRoll(roll_deg);
    ui->attitudeIndicator->setPitch(pitch_deg);
}

void MainWindow::updateControlsState(bool isConnected)
{
    ui->gearButton->setEnabled(isConnected);
    ui->brakeButton->setEnabled(isConnected);
    ui->apButton->setEnabled(isConnected);
}

void MainWindow::on_actionsource_code_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/zacario-li/msfs_dashboard"));
}