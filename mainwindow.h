#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsTextItem>

#include <cmath>

#include "geneticworld.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    GeneticWorld *world;
    QTimer *timer;

    uint botsize;
    bool worldinited = false;

    QColor botColorByType(Bot *bot);
    QColor botColorByEnergy(Bot *bot);
    QColor botColorByUsedGens(Bot *bot);
    void initWorld(uint x, uint y);
    void updateWorld();
    QGraphicsTextItem* textWidget(QString text, uint x, uint y, QColor color);

private slots:
    void render();
    void start();
    void stop();
    void new_world();
};
#endif // MAINWINDOW_H
