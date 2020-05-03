#include "mainwindow.h"
#include "ui_mainwindow.h"


QColor BotColor(Bot bot) {
    int energy = bot.energy;
    return QColor(energy, energy, energy);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->DrawArea->setScene(scene);
    ui->DrawArea->rotate(270);
    //x
    // /\
    // |
    // |
    // |
    // |
    // 0---------> y

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
    newBot->energy = 254;
}

void MainWindow::start() {
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->newWorldButton->setEnabled(false);
    int w = this->width();
    int h = this->height();
    this->setMinimumSize(w, h);
    this->setMaximumSize(w, h);

    std::string str_size = std::to_string(w) + " x " + std::to_string(h);
    ui->sizeLabel->setText(QString().fromStdString(str_size));

    scene->setSceneRect(0, 0, w, h);

    run = true;
    if (new_world_flag)
        initWorld(w, h);
    new_world_flag = false;
    timer->singleShot(ui->timerInterval->value(), this, SLOT(render()));
}

void MainWindow::stop() {
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->newWorldButton->setEnabled(true);
    run = false;
}

void MainWindow::new_world() {
    new_world_flag = true;
    ui->newWorldButton->setEnabled(false);
    this->setMinimumSize(0, 0);
    this->setMaximumSize(16777215, 16777215);
    scene->clear();
}

void MainWindow::render() {
    if (!run) return;

    scene->clear();


    unsigned int bot_len = world->bots.size();
    ui->botLen->display(QString().fromStdString(std::to_string(bot_len))); //display bot lenght
    ui->generation->setText(QString().fromStdString(std::to_string(world->generation)));
    for(unsigned int i = 0; i != bot_len; i++) {
        scene->addRect(world->bots[i].xy[0], world->bots[i].xy[1], 1, 1, QPen(BotColor(world->bots[i])));
    }
    timer->singleShot(ui->timerInterval->value(), this, SLOT(render()));
}
