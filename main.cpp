#include "QtCore/qtimer.h"
#include "QtGui/qpainter.h"
#include "QtWidgets/qgraphicsview.h"
#include "QtWidgets/qmenubar.h"
#include "graphicsview.h"
#include "gun.h"
#include "healthbar.h"
#include "mainwindow.h"
#include "player.h"
#include "QScreen"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow *w = new MainWindow();
    w->setMaximumSize(QGuiApplication::primaryScreen()->size());
    w->setMinimumSize(900, 600);
    w->setWindowTitle("BenMaxGame");

    QGraphicsScene scene;
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    GraphicsView view(&scene);
    view.setMouseTracking(true);
    view.setRenderHint(QPainter::Antialiasing);
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);


    QMenuBar *menuBar = new QMenuBar(w);
    w->setMenuBar(menuBar);

    QMenu *fileMenu = new QMenu("File", menuBar);
    QAction *closeAction = new QAction("Close Window", fileMenu);
    closeAction->setShortcuts(QKeySequence::Close);
    QObject::connect(closeAction, &QAction::triggered, w, &QMainWindow::close);
    fileMenu->addAction(closeAction);
    menuBar->addMenu(fileMenu);

    w->setCentralWidget(&view);
    w->show();

    return a.exec();
}
