#include "QtCore/qtimer.h"
#include "QtGui/qpainter.h"
#include "QtWidgets/qgraphicsview.h"
#include "QtWidgets/qmenubar.h"
#include "graphicsview.h"
#include "gun.h"
#include "mainwindow.h"
#include "player.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow *w = new MainWindow();

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, 500, 500);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    Player *player = new Player();
    scene.addItem(player);

    GraphicsView view(&scene);
    view.setMinimumSize(500, 500);
    view.setPlayer(player);
    view.setMouseTracking(true);
    view.setRenderHint(QPainter::Antialiasing);

//    view.setBackgroundBrush(QPixmap(":/images/space3.jpg"));
//    view.setBackgroundBrush(QBrush(Qt::red, Qt::SolidPattern));

    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    view.setWindowTitle("BenMaxGame");

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &scene, &QGraphicsScene::advance);
    QObject::connect(&timer, &QTimer::timeout, &view, &GraphicsView::moveGun);
    timer.start(10);


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
