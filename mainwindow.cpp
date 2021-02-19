#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->DrawArea->setScene(scene);
    ui->DrawArea->rotate(180);
    //            y
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

void MainWindow::initWorld(uint x, uint y) {
    int genome_len = ui->genome_len->value();
    int max_energy = ui->max_energy->value();
    world = new GeneticWorld(genome_len, max_energy, x, y);

    Bot *newBot = world->newBot();
    newBot->energy = 10;
    newBot->direction = 0;
    newBot->x = 10;
    newBot->y = 10;
    for (int i = 0; i<genome_len; i++){
        if (i%2 == 0)
            newBot->genome[i] = -2;
        else if (i%2==1)
            newBot->genome[i] = -5;
    }
    world->inited = true;
}

void MainWindow::start() {
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->newWorldButton->setEnabled(false);
    ui->max_energy->setEnabled(false);
    ui->genome_len->setEnabled(false);
    ui->timerInterval->setEnabled(false);
    ui->process_delay->setEnabled(false);
    ui->mutation_chance->setEnabled(false);
    int window_w = this->width();
    int window_h = this->height();
    this->setMinimumSize(window_w, window_h);
    this->setMaximumSize(window_w, window_h);

    int world_w = ui->DrawArea->width() / botsize;
    int world_h = ui->DrawArea->height() / botsize;
    QString str_size = QString::number(world_w) + " x " + QString::number(world_h);
    ui->sizeLabel->setText(str_size);
    scene->setSceneRect(0, 0, ui->DrawArea->width(), ui->DrawArea->height());

    if (!world->inited)
        initWorld(world_w, world_h);
    world->mutate_chance = ui->mutation_chance->value();
    world->start(ui->process_delay->value());
    timer->start(ui->timerInterval->value());
}

void MainWindow::stop() {
    world->stop();
    timer->stop();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->newWorldButton->setEnabled(true);
    ui->timerInterval->setEnabled(true);
    ui->process_delay->setEnabled(true);
    ui->max_energy->setEnabled(true);
    ui->mutation_chance->setEnabled(true);
    //delete world;
}

void MainWindow::new_world() {
    ui->newWorldButton->setEnabled(false);
    ui->max_energy->setEnabled(true);
    ui->genome_len->setEnabled(true);
    ui->generation->setText("0");
    this->setMinimumSize(0, 0);
    this->setMaximumSize(16777215, 16777215);
    scene->clear();
    delete world;
}

QColor MainWindow::BotColor(Bot *bot) {
    //float max_energy = world->max_energy;
    float genome_len = world->genome_len;

    unsigned int minerals_count = std::count(bot->genome.begin(), bot->genome.end(), -2);
    unsigned int photosynthesis_count = std::count(bot->genome.begin(), bot->genome.end(), -3);
    unsigned int step_count = std::count(bot->genome.begin(), bot->genome.end(), -1);

    unsigned int B = minerals_count/genome_len*255;
    unsigned int G = photosynthesis_count/genome_len*255;
    unsigned int R = step_count/genome_len*255;
    return QColor(R, G, B);
    //return QColor(254, 254, 254);
}


void MainWindow::render() {
    scene->clear();
    unsigned int bot_len = world->bots.size();
    ui->botLen->display(QString::number(bot_len));
    ui->generation->setText(QString::number(world->generation));
    for(unsigned int i = 0; i < bot_len; i++) {
        QColor botColor = BotColor(world->bots[i]);
        scene->addRect(world->bots[i]->x * botsize + 1, world->bots[i]->y * botsize + 1, botsize, botsize, QPen(botColor), QBrush(botColor));
    }
}
