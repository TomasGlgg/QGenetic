#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QPair>

#include <cmath>

#include "geneticworld.h"
#include "boteditor.h"
#include "botstruct.h"
#include "kled.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MouseHandlerItem : public QObject, public QGraphicsItem
{
Q_OBJECT
private:
    uint w = 0, h = 0;
public:
    explicit MouseHandlerItem(QObject *parent = 0) : QObject(parent), QGraphicsItem() {
        setPos(0, 0);
    }
    explicit MouseHandlerItem(uint w = 0, uint h = 0, QObject *parent = 0) : QObject(parent), QGraphicsItem() {
        this->w = w;
        this->h = h;
    }
    QRectF boundingRect() const {
        return QRectF(0,0,w,h);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }
    void mousePressEvent(QGraphicsSceneMouseEvent *event) {
        emit mousePress(mapToScene(event->pos()));
    }

signals:
    void mousePress(QPointF position);
};


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    BotEditor *botEditorWindow;
    QGraphicsScene *scene;
    GeneticWorld *world;
    MouseHandlerItem *mousePressHandler;
    KLed *statusLed;

    KLed *processTimeLed;
    QTimer *renderTimer;

    QTimer *graphTimer;
    QChart *historyPlot;

    QValueAxis *historyAxisX;
    QValueAxis *historyAxisY;
    QGridLayout *historyLayout;
    QChartView *historyView;
    QLineSeries *aliveBotHistory;
    QLineSeries *organicBotHistory;

    uint botSize;
    bool worldInited = false;

    QColor botColorByType(Bot *bot);
    QColor botColorByEnergy(Bot *bot);
    QColor botColorByUsedGens(Bot *bot);
    QColor botColorByOld(Bot *bot);
    QColor botColorByMinerals(Bot *bot);
    void initWorld(uint x, uint y);
    void updateWorldSettings();
    void startMouseHandler();
    QPair<QPair<int, int>, QPair<int, int>> _find2Max();
    void renderGraph(bool reset);
    void closeEvent(QCloseEvent *event);

private slots:
    void renderTypeChanged();
    void renderGraph();
    void openBotEditor();
    void mousePress(QPointF position);
    void renderUI();
    void start();
    void stop();
    void newWorld();
};

QGraphicsTextItem* textWidget(QString text, uint x, uint y, QColor color);
#endif // MAINWINDOW_H
