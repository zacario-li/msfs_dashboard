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
    void onEngineButtonToggled(bool checked);

private:
    void updateControlsState(bool isConnected);

    SimConnectClient *m_simConnectClient;
    Ui::MainWindow *ui;
    AircraftData m_currentAircraftData;
};
#endif // MAINWINDOW_H 