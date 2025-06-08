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

    ui->rpmIndicator1->setTitle("ENG 1");
    ui->rpmIndicator2->setTitle("ENG 2");
    ui->rpmIndicator3->setTitle("ENG 3");
    ui->rpmIndicator4->setTitle("ENG 4");

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(ui->gearButton, &QPushButton::toggled, this, &MainWindow::onGearButtonToggled);
    connect(ui->brakeButton, &QPushButton::clicked, m_simConnectClient, &SimConnectClient::toggleParkingBrake);
    connect(ui->apButton, &QPushButton::clicked, m_simConnectClient, &SimConnectClient::toggleAutopilot);

    connect(ui->eng1Button, &QPushButton::toggled, this, &MainWindow::onEngineButtonToggled);
    connect(ui->eng2Button, &QPushButton::toggled, this, &MainWindow::onEngineButtonToggled);
    connect(ui->eng3Button, &QPushButton::toggled, this, &MainWindow::onEngineButtonToggled);
    connect(ui->eng4Button, &QPushButton::toggled, this, &MainWindow::onEngineButtonToggled);

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
    ui->gearWarningLabel->setText("");
    ui->gearCenterLabel->setText("C: ---%");
    ui->gearLeftLabel->setText("L: ---%");
    ui->gearRightLabel->setText("R: ---%");
    ui->attitudeIndicator->setRoll(0);
    ui->attitudeIndicator->setPitch(0);
    ui->compass->setHeading(0);
    ui->gearButton->setChecked(false);
    ui->brakeButton->setChecked(false);
    ui->apButton->setChecked(false);

    ui->rpmIndicator1->setRpmPercent(0);
    ui->rpmIndicator2->setRpmPercent(0);
    ui->rpmIndicator3->setRpmPercent(0);
    ui->rpmIndicator4->setRpmPercent(0);
    ui->eng1Button->setChecked(false);
    ui->eng2Button->setChecked(false);
    ui->eng3Button->setChecked(false);
    ui->eng4Button->setChecked(false);
}

void MainWindow::onAircraftDataUpdated(const AircraftData &data)
{
    m_currentAircraftData = data;

    // Update Gear
    ui->gearLabel->setText(QString("Gear: %1%").arg(data.gear_total_extended_pct * 100.0, 0, 'f', 1));
    ui->gearCenterLabel->setText(QString("C: %1%").arg(data.gear_pos_center * 100.0, 3, 'f', 0));
    ui->gearLeftLabel->setText(QString("L: %1%").arg(data.gear_pos_left * 100.0, 3, 'f', 0));
    ui->gearRightLabel->setText(QString("R: %1%").arg(data.gear_pos_right * 100.0, 3, 'f', 0));
    
    // Block signals to prevent feedback loop while we set the checked state
    ui->gearButton->blockSignals(true);
    ui->gearButton->setChecked(data.gear_handle_position > 0.5);
    ui->gearButton->blockSignals(false);

    // Update Brakes
    ui->brakeButton->setChecked(data.parking_brake_position > 0.5);

    // Update Autopilot
    ui->apButton->setChecked(data.autopilot_master > 0.5);

    // Update Attitude Indicator
    float roll_deg = static_cast<float>(data.attitude_bank_radians * 180.0 / M_PI);
    float pitch_deg = static_cast<float>(data.attitude_pitch_radians * 180.0 / M_PI);
    ui->attitudeIndicator->setRoll(roll_deg);
    ui->attitudeIndicator->setPitch(pitch_deg);

    // Update Compass
    ui->compass->setHeading(static_cast<float>(data.plane_heading_degrees_true));

    // Update RPM Indicators
    ui->rpmIndicator1->setRpmPercent(data.eng_rpm_1);
    ui->rpmIndicator2->setRpmPercent(data.eng_rpm_2);
    ui->rpmIndicator3->setRpmPercent(data.eng_rpm_3);
    ui->rpmIndicator4->setRpmPercent(data.eng_rpm_4);

    // Update Gear Warning
    bool gearDamaged = data.gear_damage_by_speed > 0.5;
    bool gearWarning = data.gear_warning_center > 0 || data.gear_warning_left > 0 || data.gear_warning_right > 0;

    if (gearDamaged) {
        ui->gearWarningLabel->setText("GEAR DAMAGED");
    } else if (gearWarning) {
        ui->gearWarningLabel->setText("GEAR UNSAFE");
    } else {
        ui->gearWarningLabel->setText("");
    }
}

void MainWindow::updateControlsState(bool isConnected)
{
    ui->gearButton->setEnabled(isConnected);
    ui->brakeButton->setEnabled(isConnected);
    ui->apButton->setEnabled(isConnected);
    ui->engineGroup->setEnabled(isConnected);
}

void MainWindow::on_actionsource_code_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/zacario-li/msfs_dashboard"));
}

void MainWindow::onGearButtonToggled(bool checked)
{
    if (m_simConnectClient->isConnected())
    {
        m_simConnectClient->setGear(checked);
    }
}

void MainWindow::onEngineButtonToggled(bool checked)
{
    if (!m_simConnectClient->isConnected()) return;

    QObject* senderObj = sender();
    int engineIndex = 0;

    if (senderObj == ui->eng1Button) engineIndex = 1;
    else if (senderObj == ui->eng2Button) engineIndex = 2;
    else if (senderObj == ui->eng3Button) engineIndex = 3;
    else if (senderObj == ui->eng4Button) engineIndex = 4;
    
    if (engineIndex > 0)
    {
        m_simConnectClient->setEngineState(engineIndex, checked);
    }
}