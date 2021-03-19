#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsTextItem>

#include <cmath>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

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
    QTimer *renderTimer;
    QTimer *graphTimer;
    QPolygonF aliveBotHistory;
    QPolygonF organicBotHistory;

    uint botSize;
    bool worldInited = false;

    QColor botColorByType(Bot *bot);
    QColor botColorByEnergy(Bot *bot);
    QColor botColorByUsedGens(Bot *bot);
    void initWorld(uint x, uint y);
    void updateWorld();
    QGraphicsTextItem* textWidget(QString text, uint x, uint y, QColor color);

private slots:
    void render_graph();
    void render_draw_area();
    void start();
    void stop();
    void newWorld();
};
#endif // MAINWINDOW_H
