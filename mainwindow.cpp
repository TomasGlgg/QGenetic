#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    botEditorWindow = new BotEditor();
    renderTimer = new QTimer(this);
    graphTimer = new QTimer(this);

    ui->DrawArea->setScene(scene);

    connect(ui->startButton, SIGNAL(released()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->newWorldButton, SIGNAL(released()), this, SLOT(newWorld()));
    connect(ui->botEditorCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkBoxStateChanged(int)));

    connect(renderTimer, SIGNAL(timeout()), this, SLOT(renderUI()));
    connect(graphTimer, SIGNAL(timeout()), this, SLOT(renderGraph()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::checkBoxStateChanged(int state) {
    if (state == Qt::Checked) {
        botEditorWindow->show();
        if (renderTimer->isActive()) botEditorWindow->startMon();
    } else {
        botEditorWindow->close();
        botEditorWindow->stopMon();
    }
}

void MainWindow::mousePress(QPointF position) {
    uint position_x = position.x();
    uint position_y = ui->DrawArea->height() - position.y();
    int botX = floor(static_cast<float>(position_x) / static_cast<float>(botSize));
    int botY = floor(static_cast<float>(position_y) / static_cast<float>(botSize)) + 1;

    ulong botHash = hashxy(botX, botY);
    if (world->bots.contains(botHash)) {
        Bot *bot = world->bots.value(botHash);
        botEditorWindow->loadBot(bot);
        botEditorWindow->single();
    }
}

void MainWindow::initWorld(uint x, uint y) {
    world = new GeneticWorld();
    world->genomeLen = ui->genome_len->value();
    world->worldPartsCount = ui->world_parts_count->value();
    world->partLenght = y/(world->worldPartsCount);

    y = (world->worldPartsCount*world->partLenght);

    world->maxX = x;
    world->maxY = y;
    world->maxBotCount = x*y;

    Bot *newBot = world->newBot(10, y - 10);
    newBot->energy = ui->first_bot_energy->value();
    newBot->direction = ui->first_bot_direction->currentIndex();

    for (uint i = 0; i<world->genomeLen; i++){
        if (i%2 == 0)
            newBot->genome[i] = commands::left_command;
        else if (i%2 == 1)
            newBot->genome[i] = commands::photosynthesis_command;
    }
    newBot->genomeInit();
    worldInited = true;
}

void MainWindow::updateWorldSettings() {
    world->genomeLen = ui->genome_len->value();
    uint maxEnergy = ui->max_energy->value();
    world->stealPower = maxEnergy*ui->eat_k->value();
    world->maxEnergy = maxEnergy;
    world->mutateChance = ui->mutation_chance->value();
    world->startWorldEnergy = ui->start_world_energy->value();
    world->maxOld = ui->max_old->value();
    world->maxOrganicOld = ui->max_organic_old->value();
    world->organicEnabled = ui->organic->isChecked();
    world->newBotEnergy = ui->new_bot_energy->value();
    world->processDelay = ui->process_delay->value();
    world->organicEnergy = ui->organicEnergy->value();
    ui->bot_completion->setMaximum(world->maxBotCount);
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
    ui->max_organic_old->setEnabled(false);
    ui->eat_k->setEnabled(false);
    ui->organic->setEnabled(false);
    ui->new_bot_energy->setEnabled(false);
    ui->first_bot_energy->setEnabled(false);
    ui->first_bot_direction->setEnabled(false);
    ui->organicEnergy->setEnabled(false);

    //graph
    if (ui->groupBox_graph->isChecked()) {
        ui->historyPlot->setEnabled(true);
        graphTimer->start(ui->graph_freq->value());
    }
    ui->groupBox_graph->setEnabled(false);



    // world
    if (!worldInited) {
        botSize = ui->bot_size->value();
        uint worldW = ui->DrawArea->width() / botSize;
        uint worldH = ui->DrawArea->height() / botSize;
        initWorld(worldW, worldH);

        scene->setSceneRect(0, botSize, ui->DrawArea->width(), ui->DrawArea->height());
        uint windowW = botSize * world->maxX;
        uint windowH = botSize * world->maxY;
        ui->dockWidget->setMinimumSize(windowW, windowH);
        ui->dockWidget->setMaximumSize(windowW, windowH);

        QString strSize = QString::number(world->maxX) + " x " + QString::number(world->maxY) + " (" + QString::number(world->maxBotCount) + ")";
        ui->sizeLabel->setText(strSize);
        ui->bot_completion->setMaximum(world->maxBotCount);
        ui->bot_completion->setFormat("%v/" + QString::number(world->maxBotCount) + " (%p%)");

        aliveBotHistory.clear();
        organicBotHistory.clear();
        aliveBotHistory << QPointF(0, 0);
        organicBotHistory << QPointF(0, 0);
    }
    updateWorldSettings();

    // starting
    world->start();
    renderTimer->start(ui->timerInterval->value());
    botEditorWindow->startMon();
    ui->status_led->setColor(QColor(0, 255, 0));
}

void MainWindow::stop() {
    world->stop();
    renderTimer->stop();
    graphTimer->stop();
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
    ui->max_organic_old->setEnabled(true);
    ui->eat_k->setEnabled(true);
    ui->new_bot_energy->setEnabled(true);
    ui->organicEnergy->setEnabled(true);

    //graph
    ui->groupBox_graph->setEnabled(true);
    ui->historyPlot->setEnabled(false);
    ui->groupBox_graph->setEnabled(true);

    ui->status_led->setColor(QColor(255, 128, 0));
    botEditorWindow->stopMon();
    mousePressHandler = new MouseHandlerItem(ui->DrawArea->width(), ui->DrawArea->height());
    scene->addItem(mousePressHandler);
    connect(mousePressHandler, SIGNAL(mousePress(QPointF)), this, SLOT(mousePress(QPointF)));
}

void MainWindow::newWorld() {
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
    ui->organic->setEnabled(true);
    ui->first_bot_direction->setEnabled(true);
    ui->first_bot_energy->setEnabled(true);

    //graph
    ui->historyPlot->detachItems();
    ui->groupBox_graph->setEnabled(true);
    ui->historyPlot->setEnabled(false);

    ui->generation->setText("0");
    ui->mutation_count->setText("0");
    ui->kill_count->setText("0");
    ui->sizeLabel->setText("");
    ui->bot_count->display(0);
    ui->processing_time->setText("0");
    ui->bot_completion->setValue(0);
    ui->status_led->setColor(QColor(255, 128, 0));
    ui->process_time_led->setColor(QColor(123, 123, 123));
    ui->dockWidget->setMinimumSize(0, 0);
    ui->dockWidget->setMaximumSize(16777215, 16777215);
    scene->clear();
    worldInited = false;
    delete world;
}

QColor MainWindow::botColorByType(Bot *bot) {
    if (bot->getHash() == botEditorWindow->botHash()) return QColor(Qt::magenta);  // if bot is monited
    if (bot->type == ORGANIC) return QColor(Qt::gray);
    float total_number = bot->mineralsCount + bot->photosynthesisCount + bot->eatCount;
    uint B = bot->mineralsCount/total_number * 255;
    uint G = bot->photosynthesisCount/total_number * 255;
    uint R = bot->eatCount/total_number * 255;
    return QColor(R, G, B);
}

inline QColor MainWindow::botColorByEnergy(Bot *bot) {
    if (bot->getHash() == botEditorWindow->botHash()) return QColor(Qt::blue);  // if bot is monited
    if (bot->type == ORGANIC) return QColor(Qt::gray);
    uint color = (static_cast<float>(bot->energy)/static_cast<float>(world->maxEnergy))*255;
    if (color > 255) color = 255;
    return QColor(255, 255-color, 0);
}

QColor MainWindow::botColorByUsedGens(Bot *bot) {
    if (bot->getHash() == botEditorWindow->botHash()) return QColor(Qt::magenta);  // if bot is monited
    if (bot->type == ORGANIC) return QColor(Qt::gray);
    float totalNumber = bot->used_eat + bot->used_minerals + bot->used_photosynthesis;
    if (!totalNumber) return QColor(Qt::white);
    uint B = bot->used_minerals/totalNumber * 255;
    uint G = bot->used_photosynthesis/totalNumber * 255;
    uint R = bot->used_eat/totalNumber * 255;
    return QColor(R, G, B);
}

void MainWindow::renderGraph() {
    uint aliveBotCount = world->aliveBotsCount;
    uint organicBotCount = world->bots.size() - aliveBotCount;
    aliveBotHistory << QPointF(aliveBotHistory[aliveBotHistory.size()-1].x()+1, aliveBotCount);
    organicBotHistory << QPointF(organicBotHistory[organicBotHistory.size()-1].x()+1, organicBotCount);

    if (aliveBotHistory.size() > ui->graph_count->value()) {
        aliveBotHistory.pop_front();
        organicBotHistory.pop_front();
    }

    QwtPlotCurve *aliveHistoryCurve = new QwtPlotCurve();
    QwtPlotCurve *organicHistoryCurve = new QwtPlotCurve();
    aliveHistoryCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    organicHistoryCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

    aliveHistoryCurve->setPen(Qt::red, 1);
    aliveHistoryCurve->setSamples(aliveBotHistory);

    organicHistoryCurve->setPen(Qt::blue, 1);
    organicHistoryCurve->setSamples(organicBotHistory);

    ui->historyPlot->detachItems();
    aliveHistoryCurve->attach(ui->historyPlot);
    organicHistoryCurve->attach(ui->historyPlot);
}

QGraphicsTextItem* textWidget(QString text, uint x, uint y, QColor color) {
    QGraphicsTextItem *textItem = new QGraphicsTextItem(text);
    textItem->setPos(x, y);
    textItem->setDefaultTextColor(color);
    return textItem;
}

void MainWindow::renderUI() {
    scene->clear();

    // process time led
    if (world->processDelay) {
        if (world->processingTime >= world->processDelay*1000) ui->process_time_led->setColor(QColor(255, 0, 0));
        else ui->process_time_led->setColor(QColor(0, 255, 0));
    } else ui->process_time_led->setColor(QColor(133, 133, 133));

    uint aliveBotCount = world->aliveBotsCount;
    uint botCount = world->bots.size();
    assert(botCount >= aliveBotCount);

    if (!aliveBotCount) {  // all bots died
        world->runFlag = false;
        renderTimer->stop();
        graphTimer->stop();
        botEditorWindow->stopMon();
        ui->newWorldButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        ui->status_led->setColor(QColor(255, 0, 0));
        return;
    }

    // bot render
    QColor botColor;
    world->botsMutex.lock();
    if (ui->radio_gens_type->isChecked()) {
        foreach (Bot *bot, world->bots) {
            botColor = botColorByType(bot);
            scene->addRect(bot->getX() * botSize, ui->DrawArea->height() - (bot->getY() * botSize), botSize-1, botSize-1, QPen(botColor), QBrush(botColor));
        }
    } else if (ui->radio_energy_count->isChecked()) {
        foreach (Bot *bot, world->bots) {
            botColor = botColorByEnergy(bot);
            scene->addRect(bot->getX() * botSize, ui->DrawArea->height() - (bot->getY() * botSize), botSize-1, botSize-1, QPen(botColor), QBrush(botColor));
        }
    } else if (ui->radio_used_gens->isChecked()) {
        foreach (Bot *bot, world->bots) {
            botColor = botColorByUsedGens(bot);
            scene->addRect(bot->getX() * botSize, ui->DrawArea->height() - (bot->getY() * botSize), botSize-1, botSize-1, QPen(botColor), QBrush(botColor));
        }
    }
    world->botsMutex.unlock();

    // ui info update
    ui->bot_count->display(QString::number(aliveBotCount));
    ui->generation->setText(QString::number(world->generation));
    ui->mutation_count->setText(QString::number(world->mutationCount));
    ui->kill_count->setText(QString::number(world->kills));
    ui->bot_completion->setValue(botCount);
    ui->processing_time->setText(QString::number(world->processingTime) + " (мкс)");

    // draw lines
    if (ui->draw_lines->isChecked()) {
        uint current_height, coordinates_y;
        for (uint part = 0; part<=world->worldPartsCount; ++part) {
            current_height = ui->DrawArea->height() - (world->partLenght*part)*botSize;

            coordinates_y = world->partLenght * part;
            if (current_height != 0)
                scene->addLine(0, current_height, ui->DrawArea->width(), current_height, QPen(QColor(128, 128, 128)));
            if (part <= world->startWorldEnergy)  // minerals
               scene->addItem(textWidget(QString::number(world->getMineralsCount(coordinates_y)), 0, current_height, QColor(255, 255, 255)));
            if (part >= (world->worldPartsCount - world->startWorldEnergy)) // photosynthesis
                scene->addItem(textWidget(QString::number(world->getPhotosynthesisEnergy(coordinates_y)), ui->DrawArea->width() - 20, current_height, QColor(255, 255, 255)));
        }


        /*uint last_energy_p = 0, last_energy_m = 0, energy;
         for (uint i = world->maxY; i>0; --i) {
             energy = world->getPhotosynthesisEnergy(i);
             if (energy != last_energy_p) {
                  scene->addItem(textWidget(QString::number(energy) + " " + QString::number(last_energy_p), ui->DrawArea->width() - 40, ui->DrawArea->height()-i*botSize, QColor(255, 0, 0)));
                  last_energy_p = energy;
                  scene->addRect(ui->DrawArea->width() - 20, ui->DrawArea->height()-i*botSize, 1, 1, QPen(QColor(255, 0, 0)));
              }
              energy = world->getMineralsCount(i);
              if (energy != last_energy_m) {
                  scene->addItem(textWidget(QString::number(energy) + " " + QString::number(last_energy_m), 0, ui->DrawArea->height()-i*botSize, QColor(255, 0, 0)));
                  last_energy_m = energy;
                  scene->addRect(0, ui->DrawArea->height()-i*botSize, 1, 1, QPen(QColor(255, 0, 0)));
              }
        }*/

    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    botEditorWindow->stopMon();
    botEditorWindow->close();
    renderTimer->stop();
    graphTimer->stop();
    if (worldInited) {
        world->runFlag = false;
        world->wait();
    }
    QWidget::closeEvent(event);
}
