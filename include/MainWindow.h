#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

#include "SimConnectClient.h"
#include "AttitudeIndicator.h"

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

private:
    void setupUi();
    void updateControlsState(bool isConnected);

    SimConnectClient *m_simConnectClient;

    // UI Elements
    QWidget *m_centralWidget;
    QGridLayout *m_layout;
    
    QPushButton *m_connectButton;
    QLabel *m_statusLight;

    QPushButton *m_gearButton;
    QPushButton *m_brakeButton;
    QPushButton *m_apButton;
    
    QLabel *m_gearLabel;
    
    AttitudeIndicator *m_attitudeIndicator;
};
#endif // MAINWINDOW_H 