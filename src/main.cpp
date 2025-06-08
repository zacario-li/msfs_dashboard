#include <QApplication>
#include "MainWindow.h"
#include <loguru.hpp>

int main(int argc, char *argv[])
{
    // Initialize loguru
    loguru::init(argc, argv);
    
    // Set up logging files
    loguru::add_file("msfs_dashboard.log", loguru::Append, loguru::Verbosity_MAX);
    
    // Configure stderr output
    loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
    
    LOG_F(INFO, "MSFS Dashboard starting...");
    
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("MSFS Dashboard");
    w.show();
    
    LOG_F(INFO, "MSFS Dashboard window shown");
    
    int result = a.exec();
    
    LOG_F(INFO, "MSFS Dashboard shutting down with exit code: %d", result);
    return result;
} 