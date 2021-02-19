#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <iostream>

#include "geneticworld.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QColor BotColor(Bot *bot);
    QTimer *timer;
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    GeneticWorld *world;

    const uint botsize = 5;

    void initWorld(uint x, uint y);

private slots:
    void render();
    void start();
    void stop();
    void new_world();
};
#endif // MAINWINDOW_H
