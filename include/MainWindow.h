#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SimConnectClient.h"
#include "AttitudeIndicator.h"
#include "Compass.h"
#include "RpmIndicator.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectClicked();
    void onSimConnected();
    void onSimDisconnected();
    void onAircraftDataUpdated(const AircraftData &data);
    void on_actionsource_code_triggered();
    void onGearButtonToggled(bool checked);
    void on_gearButton_clicked(bool checked);
    void on_parkingBrakeButton_clicked(bool checked);
    void on_fdButton_clicked(bool checked);
    void on_apButton_clicked(bool checked);
    void on_navButton_clicked(bool checked);
    void on_aprButton_clicked(bool checked);
    void on_athrButton_clicked(bool checked);
    void on_altButton_clicked(bool checked);
    void on_vsButton_clicked(bool checked);
    void on_flcButton_clicked(bool checked);
    void on_hdgButton_clicked(bool checked);
    void on_eng1Button_toggled(bool checked);
    void on_eng2Button_toggled(bool checked);
    void on_eng3Button_toggled(bool checked);
    void on_eng4Button_toggled(bool checked);

private:
    void updateControlsState(bool isConnected);

    SimConnectClient *m_simConnectClient;
    Ui::MainWindow *ui;
    AircraftData m_currentAircraftData;
};
#endif // MAINWINDOW_H 