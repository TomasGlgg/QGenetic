#include "mainwindow.h"
#include "ui_mainwindow.h"


QColor BotColor(Bot *bot) {
    //int energy = bot.energy;
    //return QColor(energy, energy, energy);
    return QColor(254, 254, 254);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->DrawArea->setScene(scene);
    ui->DrawArea->rotate(180);
    //             y
    //           /\
    //            |
    //            |
    //            |
    //            |
    // x<---------0
    timer = new QTimer(this);
    connect(ui->startButton, SIGNAL(released()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->newWorldButton, SIGNAL(released()), this, SLOT(new_world()));
    connect(timer, SIGNAL(timeout()), this, SLOT(render()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initWorld(int x, int y) {
    int genome_len = ui->genome_len->value();
    int max_energy = ui->max_energy->value();
    world = new GeneticWorld(genome_len, max_energy, x, y);
    Bot *newBot = world->newBot();
    newBot->energy = 10;
    newBot->direction = 0;
    newBot->x = 10;
    newBot->y = 10;
    int flag = 0;
    for (int i = 0; i<genome_len; i++){
        flag++;
        flag %= 2;
        if (flag == 0)
            newBot->genom[i] = -2;
        else if (flag==1)
            newBot->genom[i] = -4;
    }
}

void MainWindow::start() {
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->newWorldButton->setEnabled(false);
    ui->max_energy->setEnabled(false);
    ui->genome_len->setEnabled(false);
    int window_w = this->width();
    int window_h = this->height();
    this->setMinimumSize(window_w, window_h);
    this->setMaximumSize(window_w, window_h);

    int widget_w = ui->DrawArea->width();
    int widget_h = ui->DrawArea->height();

    QString str_size = QString::number(widget_w) + " x " + QString::number(widget_h);
    ui->sizeLabel->setText(str_size);

    scene->setSceneRect(0, 0, widget_w, widget_h);

    run = true;
    if (new_world_flag)
        initWorld(widget_w, widget_h);
    new_world_flag = false;
    world->start();

    timer->start(ui->timerInterval->value());
}

void MainWindow::stop() {
    timer->stop();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->newWorldButton->setEnabled(true);
    run = false;
    world->run_flag = false;
//    delete world;
}

void MainWindow::new_world() {
    new_world_flag = true;
    ui->newWorldButton->setEnabled(false);
    ui->max_energy->setEnabled(true);
    ui->genome_len->setEnabled(true);
    ui->generation->setText("0");
    this->setMinimumSize(0, 0);
    this->setMaximumSize(16777215, 16777215);
    scene->clear();
}

void MainWindow::render() {
    world->process_delay = ui->process_delay->value();
    scene->clear();
    unsigned int bot_len = world->bots.size();
    ui->botLen->display(QString::number(bot_len)); //display bot lenght
    ui->generation->setText(QString::number(world->generation));
    for(unsigned int i = 0; i != bot_len; i++) {
        scene->addRect(world->bots[i].x, world->bots[i].y, 1, 1, QPen(BotColor(&world->bots[i])));
    }
}
