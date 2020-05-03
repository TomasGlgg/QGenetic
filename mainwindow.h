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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QTimer *timer;
    GeneticWorld *world;
    bool run;
    bool new_world_flag = true;

    void initWorld(int x, int y);

private slots:
    void render();
    void start();
    void stop();
    void new_world();
};
#endif // MAINWINDOW_H
