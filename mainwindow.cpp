#include "mainwindow.h"
#include "QSettings"
#include "QMenuBar"
#include "QGraphicsView"
#include "QGraphicsRectItem"
#include "player.h"
#include "QTimer"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//    QSettings settings("BenMax Productions", "BenMaxGame");
//    restoreGeometry(settings.value("geometry").toByteArray());
//    restoreState(settings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("BenMax Productions", "BenMaxGame");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

