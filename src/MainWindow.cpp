#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <loguru.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_simConnectClient(new SimConnectClient(this))
{
    ui->setupUi(this);

    connect(m_simConnectClient, &SimConnectClient::connected, this, &MainWindow::onSimConnected);
    connect(m_simConnectClient, &SimConnectClient::disconnected, this, &MainWindow::onSimDisconnected);
    connect(m_simConnectClient, &SimConnectClient::aircraftDataUpdated, this, &MainWindow::onAircraftDataUpdated);

    m_simConnectClient->connectToSim();

    ui->rpmIndicator1->setTitle("ENG 1");
    ui->rpmIndicator2->setTitle("ENG 2");
    ui->rpmIndicator3->setTitle("ENG 3");
    ui->rpmIndicator4->setTitle("ENG 4");

    updateControlsState(false);

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(ui->gearButton, &QPushButton::clicked, this, &MainWindow::onGearButtonToggled);

    connect(ui->eng1Button, &QPushButton::toggled, this, &MainWindow::on_eng1Button_toggled);
    connect(ui->eng2Button, &QPushButton::toggled, this, &MainWindow::on_eng2Button_toggled);
    connect(ui->eng3Button, &QPushButton::toggled, this, &MainWindow::on_eng3Button_toggled);
    connect(ui->eng4Button, &QPushButton::toggled, this, &MainWindow::on_eng4Button_toggled);

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
    LOG_F(INFO, "SimConnect connected - updating UI state");
    ui->connectButton->setText("Disconnect");
    ui->statusLight->setStyleSheet("border-radius: 10px; background-color: green;");
    updateControlsState(true);
}

void MainWindow::onSimDisconnected()
{
    LOG_F(INFO, "SimConnect disconnected - resetting UI state");
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

    ui->rpmIndicator1->setRpmPercent(0);
    ui->rpmIndicator2->setRpmPercent(0);
    ui->rpmIndicator3->setRpmPercent(0);
    ui->rpmIndicator4->setRpmPercent(0);
    ui->rpmIndicator1->setThrottlePercent(0);
    ui->rpmIndicator2->setThrottlePercent(0);
    ui->rpmIndicator3->setThrottlePercent(0);
    ui->rpmIndicator4->setThrottlePercent(0);
    ui->eng1Button->setChecked(false);
    ui->eng1Button->setText("Start Eng 1");
    ui->eng2Button->setChecked(false);
    ui->eng2Button->setText("Start Eng 2");
    ui->eng3Button->setChecked(false);
    ui->eng3Button->setText("Start Eng 3");
    ui->eng4Button->setChecked(false);
    ui->eng4Button->setText("Start Eng 4");
}

void MainWindow::onAircraftDataUpdated(const AircraftData &data)
{
    VLOG_F(2, "Aircraft data updated - gear: %.1f%%, heading: %.1f°", 
           data.gear_total_extended_pct * 100.0, data.plane_heading_degrees_true);
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

    // Update Attitude Indicator
    float roll_deg = static_cast<float>(data.attitude_bank_radians * 180.0 / M_PI);
    float pitch_deg = static_cast<float>(data.attitude_pitch_radians * 180.0 / M_PI);
    ui->attitudeIndicator->setRoll(roll_deg);
    ui->attitudeIndicator->setPitch(pitch_deg);

    // Update Compass
    ui->compass->setHeading(static_cast<float>(data.plane_heading_degrees_true));

    // Update RPM Indicators
    ui->rpmIndicator1->setRpmPercent(static_cast<float>(data.eng_n1_1));
    ui->rpmIndicator2->setRpmPercent(static_cast<float>(data.eng_n1_2));
    ui->rpmIndicator3->setRpmPercent(static_cast<float>(data.eng_n1_3));
    ui->rpmIndicator4->setRpmPercent(static_cast<float>(data.eng_n1_4));

    ui->rpmIndicator1->setThrottlePercent(static_cast<float>(data.throttle_1));
    ui->rpmIndicator2->setThrottlePercent(static_cast<float>(data.throttle_2));
    ui->rpmIndicator3->setThrottlePercent(static_cast<float>(data.throttle_3));
    ui->rpmIndicator4->setThrottlePercent(static_cast<float>(data.throttle_4));

    // Update engine button states based on N1
    const float n1_running_threshold = 15.0f;
    ui->eng1Button->blockSignals(true);
    bool eng1_running = data.eng_n1_1 > n1_running_threshold;
    ui->eng1Button->setChecked(eng1_running);
    ui->eng1Button->setText(eng1_running ? "Stop Eng 1" : "Start Eng 1");
    ui->eng1Button->blockSignals(false);

    ui->eng2Button->blockSignals(true);
    bool eng2_running = data.eng_n1_2 > n1_running_threshold;
    ui->eng2Button->setChecked(eng2_running);
    ui->eng2Button->setText(eng2_running ? "Stop Eng 2" : "Start Eng 2");
    ui->eng2Button->blockSignals(false);

    ui->eng3Button->blockSignals(true);
    bool eng3_running = data.eng_n1_3 > n1_running_threshold;
    ui->eng3Button->setChecked(eng3_running);
    ui->eng3Button->setText(eng3_running ? "Stop Eng 3" : "Start Eng 3");
    ui->eng3Button->blockSignals(false);

    ui->eng4Button->blockSignals(true);
    bool eng4_running = data.eng_n1_4 > n1_running_threshold;
    ui->eng4Button->setChecked(eng4_running);
    ui->eng4Button->setText(eng4_running ? "Stop Eng 4" : "Start Eng 4");
    ui->eng4Button->blockSignals(false);

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
    ui->engineGroup->setEnabled(isConnected);
}

void MainWindow::on_actionsource_code_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/zacario-li/msfs_dashboard"));
}

void MainWindow::onGearButtonToggled(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        LOG_F(INFO, "Gear button toggled: %s", checked ? "DOWN" : "UP");
        m_simConnectClient->transmitEvent(checked ? SimConnectClient::EVENT_GEAR_DOWN : SimConnectClient::EVENT_GEAR_UP);
    } else {
        LOG_F(WARNING, "Gear button toggled but SimConnect not connected");
    }
}

void MainWindow::on_eng1Button_toggled(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        if (checked) {
            m_simConnectClient->transmitEvent(SimConnectClient::EVENT_TOGGLE_ENGINE1_STARTER, 1);
        } else {
            m_simConnectClient->transmitEvent(SimConnectClient::EVENT_ENGINE_AUTO_SHUTDOWN, 1);
        }
    }
}

void MainWindow::on_eng2Button_toggled(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        if (checked) {
            m_simConnectClient->transmitEvent(SimConnectClient::EVENT_TOGGLE_ENGINE2_STARTER, 1);
        } else {
            m_simConnectClient->transmitEvent(SimConnectClient::EVENT_ENGINE_AUTO_SHUTDOWN, 2);
        }
    }
}

void MainWindow::on_eng3Button_toggled(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        if (checked) {
            m_simConnectClient->transmitEvent(SimConnectClient::EVENT_TOGGLE_ENGINE3_STARTER, 1);
        } else {
            m_simConnectClient->transmitEvent(SimConnectClient::EVENT_ENGINE_AUTO_SHUTDOWN, 3);
        }
    }
}

void MainWindow::on_eng4Button_toggled(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        if (checked) {
            m_simConnectClient->transmitEvent(SimConnectClient::EVENT_TOGGLE_ENGINE4_STARTER, 1);
        } else {
            m_simConnectClient->transmitEvent(SimConnectClient::EVENT_ENGINE_AUTO_SHUTDOWN, 4);
        }
    }
}

void MainWindow::on_gearButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(checked ? SimConnectClient::EVENT_GEAR_DOWN : SimConnectClient::EVENT_GEAR_UP);
    }
}

void MainWindow::on_parkingBrakeButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_PARKING_BRAKES);
    }
}

void MainWindow::on_fdButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_TOGGLE_FLIGHT_DIRECTOR);
    }
}

void MainWindow::on_apButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_AP_MASTER);
    }
}

void MainWindow::on_navButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_AP_NAV1_HOLD);
    }
}

void MainWindow::on_aprButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_AP_APR_HOLD);
    }
}

void MainWindow::on_athrButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_AUTO_THROTTLE_ARM);
    }
}

void MainWindow::on_altButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_AP_ALT_HOLD);
    }
}

void MainWindow::on_vsButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_AP_VS_HOLD);
    }
}

void MainWindow::on_flcButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_AP_FLC_HOLD);
    }
}

void MainWindow::on_hdgButton_clicked(bool checked)
{
    if (m_simConnectClient->isConnected()) {
        m_simConnectClient->transmitEvent(SimConnectClient::EVENT_AP_WING_LEVELER);
    }
}