#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->DrawArea->setScene(scene);
    render_timer = new QTimer(this);
    graph_timer = new QTimer(this);

    connect(ui->startButton, SIGNAL(released()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->newWorldButton, SIGNAL(released()), this, SLOT(new_world()));

    connect(render_timer, SIGNAL(timeout()), this, SLOT(render_draw_area()));
    connect(graph_timer, SIGNAL(timeout()), this, SLOT(render_graph()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initWorld(uint x, uint y) {
    world = new GeneticWorld();
    world->genome_len = ui->genome_len->value();
    world->world_parts_count = ui->world_parts_count->value();
    world->part_lenght = y/(world->world_parts_count);

    y = (world->world_parts_count*world->part_lenght);

    world->max_x = x;
    world->max_y = y;
    world->max_bot_count = x*y;

    Bot *newBot = world->newBot();
    newBot->energy = 10;
    newBot->direction = 2;
    newBot->x = 10;
    newBot->y = 10;

    for (uint i = 0; i<world->genome_len; i++){
        if (i%2 == 0)
            newBot->genome[i] = -2;
        else if (i%2==1)
            newBot->genome[i] = -5;
    }

    worldinited = true;
}

void MainWindow::updateWorld() {
    world->genome_len = ui->genome_len->value();
    uint max_energy = ui->max_energy->value();
    world->eat_power = max_energy*ui->eat_k->value();
    world->max_energy = max_energy;
    world->mutate_chance = ui->mutation_chance->value();
    world->start_world_energy = ui->start_world_energy->value();
    world->max_old = ui->max_old->value();

    ui->bot_completion->setMaximum(world->max_bot_count);
}

void MainWindow::start() {
    ui->bot_size->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->newWorldButton->setEnabled(false);
    ui->max_energy->setEnabled(false);
    ui->genome_len->setEnabled(false);
    ui->timerInterval->setEnabled(false);
    ui->process_delay->setEnabled(false);
    ui->mutation_chance->setEnabled(false);
    ui->draw_lines->setEnabled(true);
    ui->world_parts_count->setEnabled(false);
    ui->start_world_energy->setEnabled(false);
    ui->max_old->setEnabled(false);
    ui->eat_k->setEnabled(false);

    //graph
    if (ui->groupBox_graph->isChecked()) {
        ui->historyPlot->setEnabled(true);
        graph_timer->start(ui->graph_freq->value());
    }
    ui->groupBox_graph->setEnabled(false);


    scene->setSceneRect(0, 0, ui->DrawArea->width(), ui->DrawArea->height());

    // world
    if (!worldinited) {
        botsize = ui->bot_size->value();
        uint world_w = ui->DrawArea->width() / botsize;
        uint world_h = ui->DrawArea->height() / botsize;
        initWorld(world_w, world_h);
        uint window_w = this->width();
        uint window_h = botsize * world->max_y + (this->height() - ui->DrawArea->height());
        this->setMinimumSize(window_w, window_h);
        this->setMaximumSize(window_w, window_h);

        QString str_size = QString::number(ui->DrawArea->width() / botsize) + " x " + QString::number(ui->DrawArea->height() / botsize) + " (" + QString::number(world->max_bot_count) + ")";
        ui->sizeLabel->setText(str_size);
        ui->bot_completion->setMaximum(world->max_bot_count);
        ui->bot_completion->setFormat("%v/" + QString::number(world->max_bot_count) + " (%p%)");
    }
    updateWorld();
    initGraph();

    // starting
    world->process_delay = ui->process_delay->value();
    world->start();
    render_timer->start(ui->timerInterval->value());
    ui->status_led->setColor(QColor(0, 255, 0));
}

void MainWindow::stop() {
    world->stop();
    render_timer->stop();
    graph_timer->stop();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->newWorldButton->setEnabled(true);
    ui->timerInterval->setEnabled(true);
    ui->process_delay->setEnabled(true);
    ui->max_energy->setEnabled(true);
    ui->mutation_chance->setEnabled(true);
    ui->draw_lines->setEnabled(false);
    ui->start_world_energy->setEnabled(true);
    ui->max_old->setEnabled(true);
    ui->eat_k->setEnabled(true);

    //graph
    ui->groupBox_graph->setEnabled(true);
    ui->historyPlot->setEnabled(false);
    ui->groupBox_graph->setEnabled(true);

    ui->status_led->setColor(QColor(255, 128, 0));
}

void MainWindow::initGraph() {  // очистка графика
    history.clear();
    for (int i = 0; i<ui->graph_count->value(); i++) history << QPointF(i, 0);
}

void MainWindow::new_world() {
    ui->newWorldButton->setEnabled(false);
    ui->max_energy->setEnabled(true);
    ui->genome_len->setEnabled(true);
    ui->startButton->setEnabled(true);
    ui->bot_size->setEnabled(true);
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->timerInterval->setEnabled(true);
    ui->process_delay->setEnabled(true);
    ui->max_energy->setEnabled(true);
    ui->mutation_chance->setEnabled(true);
    ui->draw_lines->setEnabled(false);
    ui->world_parts_count->setEnabled(true);
    ui->start_world_energy->setEnabled(true);
    ui->max_old->setEnabled(true);
    ui->world_parts_count->setEnabled(true);
    ui->eat_k->setEnabled(true);

    //graph
    ui->groupBox_graph->setEnabled(true);
    ui->historyPlot->setEnabled(false);

    ui->generation->setText("0");
    ui->mutation_count->setText("0");
    ui->kill_count->setText("0");
    ui->sizeLabel->setText("");
    ui->bot_count->display(0);
    ui->bot_completion->setValue(0);
    ui->status_led->setColor(QColor(255, 128, 0));
    this->setMinimumSize(0, 0);
    this->setMaximumSize(16777215, 16777215);
    scene->clear();
    worldinited = false;
    delete world;
}

QColor MainWindow::botColorByType(Bot *bot) {
    uint minerals_count = std::count(bot->genome.begin(), bot->genome.end(), minerals_command);
    uint photosynthesis_count = std::count(bot->genome.begin(), bot->genome.end(), photosynthesis_command);
    uint eat_count = std::count(bot->genome.begin(), bot->genome.end(), eat_command);
    float total_number = minerals_count + photosynthesis_count + eat_count;

    uint B = minerals_count/total_number * 255;
    uint G = photosynthesis_count/total_number * 255;
    uint R = eat_count/total_number * 255;
    return QColor(R, G, B);
}

inline QColor MainWindow::botColorByEnergy(Bot *bot) {
    float energy_k = static_cast<float>(bot->energy)/static_cast<float>(world->max_energy);
    return QColor(255, 255-(energy_k*255), 0);
}

QColor MainWindow::botColorByUsedGens(Bot *bot) {
    float total_number = bot->used_eat + bot->used_minerals + bot->used_photosynthesis;
    if (!total_number) return QColor(255, 255, 255);
    uint B = bot->used_minerals/total_number * 255;
    uint G = bot->used_photosynthesis/total_number * 255;
    uint R = bot->used_eat/total_number * 255;
    return QColor(R, G, B);
}

QGraphicsTextItem* MainWindow::textWidget(QString text, uint x, uint y, QColor color) {
    QGraphicsTextItem *textItem = new QGraphicsTextItem(text);
    textItem->setPos(x, y);
    textItem->setDefaultTextColor(color);
    return textItem;
}

void MainWindow::render_graph() {
    history.pop_front();
    history << QPointF(history[ui->graph_count->value()-2].x()+1, world->bots.size());
    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setPen(Qt::red, 1);
    curve->setSamples(history);
    ui->historyPlot->detachItems();
    curve->attach(ui->historyPlot);
}

void MainWindow::render_draw_area() {
    scene->clear();
    uint bot_len = world->bots.size();
    if (!bot_len) {
        world->run_flag = false;
        render_timer->stop();
        ui->newWorldButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        ui->status_led->setColor(QColor(255, 0, 0));
        return;
    }

    ui->bot_count->display(QString::number(bot_len));
    ui->generation->setText(QString::number(world->generation));
    ui->mutation_count->setText(QString::number(world->mutation_count));
    ui->kill_count->setText(QString::number(world->kills));
    ui->bot_completion->setValue(bot_len);
    ui->processing_time->setText(QString::number(world->processing_time) + " (мкс)");

    QColor botColor;
    world->bots_mutex.lock();
    bot_len = world->bots.size();
    if (ui->radio_gens_type->isChecked()) {
        for(uint i = 0; i < bot_len; i++) {
            botColor = botColorByType(world->bots[i]);
            scene->addRect(world->bots[i]->x * botsize, ui->DrawArea->height() - (world->bots[i]->y * botsize), botsize-1, botsize-1, QPen(botColor), QBrush(botColor));
        }
    } else if (ui->radio_energy_count->isChecked()) {
        for(uint i = 0; i < bot_len; i++) {
            botColor = botColorByEnergy(world->bots[i]);
            scene->addRect(world->bots[i]->x * botsize, ui->DrawArea->height() - (world->bots[i]->y * botsize), botsize-1, botsize-1, QPen(botColor), QBrush(botColor));
        }
    } else if (ui->radio_used_gens->isChecked()) {
        for(uint i = 0; i < bot_len; i++) {
            botColor = botColorByUsedGens(world->bots[i]);
            scene->addRect(world->bots[i]->x * botsize, ui->DrawArea->height() - (world->bots[i]->y * botsize), botsize-1, botsize-1, QPen(botColor), QBrush(botColor));
        }
    }
    world->bots_mutex.unlock();


    if (ui->draw_lines->isChecked()) {
        uint current_height, coordinates_y;
        for (uint part = 0; part<=world->world_parts_count; ++part) {
            current_height = ui->DrawArea->height() - (world->part_lenght*part)*botsize;

            coordinates_y = (ui->DrawArea->height()/world->world_parts_count*part)/botsize;
            if (current_height != 0)
                scene->addLine(0, current_height, ui->DrawArea->width(), current_height, QPen(QColor(128, 128, 128)));
            if (part <= world->start_world_energy)  // minerals
               scene->addItem(textWidget(QString::number(world->getMineralsEnergy(coordinates_y)), 0, current_height, QColor(255, 255, 255)));
            if (part > (world->world_parts_count - world->start_world_energy)) // photosynthesis
                scene->addItem(textWidget(QString::number(world->getPhotosynthesisEnergy(coordinates_y)), ui->DrawArea->width() - 20, current_height, QColor(255, 255, 255)));
        }


        /*uint last_energy_p = 0, last_energy_m = 0, energy;
         for (uint i = world->max_y; i>0; --i) {
             energy = world->getPhotosynthesisEnergy(i);
             if (energy != last_energy_p) {
                  scene->addItem(textWidget(QString::number(energy) + " " + QString::number(last_energy_p), ui->DrawArea->width() - 40, ui->DrawArea->height()-i*botsize, QColor(255, 0, 0)));
                  last_energy_p = energy;
                  scene->addRect(ui->DrawArea->width() - 20, ui->DrawArea->height()-i*botsize, 1, 1, QPen(QColor(255, 0, 0)));
              }
              energy = world->getMineralsEnergy(i);
              if (energy != last_energy_m) {
                  scene->addItem(textWidget(QString::number(energy) + " " + QString::number(last_energy_m), 0, ui->DrawArea->height()-i*botsize, QColor(255, 0, 0)));
                  last_energy_m = energy;
                  scene->addRect(0, ui->DrawArea->height()-i*botsize, 1, 1, QPen(QColor(255, 0, 0)));
              }
          }*/

    }
}
