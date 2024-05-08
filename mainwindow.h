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

class Scene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit Scene(QObject *parent = nullptr);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
signals:
    void mousePress(QPointF);
};


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    BotEditor *botEditorWindow;
    Scene *scene;
    GeneticWorld *world;
    KLed *statusLed;
    KLed *processTimeLed;
    KLed *seasonLed;
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
    QPair<QPair<int, int>, QPair<int, int>> _find2Max();
    void renderGraph(bool reset);
    void updateSeasonLed();
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
