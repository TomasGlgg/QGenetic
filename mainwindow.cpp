#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->DrawArea->setScene(scene);

    QTimer *timer = new QTimer(this);

    connect(ui->startButton, SIGNAL(released()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->newWorldButton, SIGNAL(released()), this, SLOT(new_world()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWorld() {
    int genome_len = ui->genome_len->value();
    int max_energy = ui->max_energy->value();
    world = new GeneticWorld(genome_len, max_energy);
    Bot *newBot = world->newBot();
    newBot->energy = 5;
}

void MainWindow::start() {
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->newWorldButton->setEnabled(false);
    int w = this->width();
    int h = this->height();
    this->setFixedSize(w, h);

    std::string str_size = std::to_string(w) + " x " + std::to_string(h);
    ui->sizeLabel->setText(QString().fromStdString(str_size));

    run = true;
    if (new_world_flag)
        initWorld();
    new_world_flag = false;
    timer->singleShot(ui->timerInterval->value(), this, SLOT(render())); //один раз вызвать render() через
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
}

void MainWindow::render() {
    if (!run) return;
    std::cout << world->bots.size() << std::endl; //пустой) удалился после return опходу опять
    for(std::vector<int>::size_type i = 0; i != world->bots.size(); i++) {
        QColor bot_color = BotColor(world->bots[i]);
        QPen pen(bot_color);
        QPainter painter;
        painter.setPen(pen);
    }

    timer->singleShot(ui->timerInterval->value(), this, SLOT(render())); //ну и тка по кругу
}
