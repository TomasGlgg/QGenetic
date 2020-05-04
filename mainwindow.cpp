#include "mainwindow.h"
#include "ui_mainwindow.h"


QColor BotColor(Bot bot) {
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

    QTimer *timer = new QTimer(this);

    connect(ui->startButton, SIGNAL(released()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->newWorldButton, SIGNAL(released()), this, SLOT(new_world()));
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
    newBot->xy[0] = 10;
    newBot->xy[1] = 10;
    int flag = 0;
    for (int i = 0; i<20; i++){
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

    std::string str_size = std::to_string(widget_w) + " x " + std::to_string(widget_h);
    ui->sizeLabel->setText(QString().fromStdString(str_size));

    scene->setSceneRect(0, 0, widget_w, widget_h);

    run = true;
    if (new_world_flag)
        initWorld(widget_w, widget_h);
    new_world_flag = false;
    world->start();
    timer->singleShot(ui->timerInterval->value(), this, SLOT(render()));
}

void MainWindow::stop() {
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->newWorldButton->setEnabled(true);
    run = false;
    world->run_flag = false;
    //delete world;
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
    if (!run) return;

    world->process_delay = ui->process_delay->value();
    scene->clear();
    unsigned int bot_len = world->bots.size();
    ui->botLen->display(QString().fromStdString(std::to_string(bot_len))); //display bot lenght
    ui->generation->setText(QString().fromStdString(std::to_string(world->generation)));
    for(unsigned int i = 0; i != bot_len; i++) {
        scene->addRect(world->bots[i].xy[0], world->bots[i].xy[1], 1, 1, QPen(BotColor(world->bots[i])));
    }
    timer->singleShot(ui->timerInterval->value(), this, SLOT(render()));//рекурсия не функция то завершается   там тоже самое было я знаю
}
