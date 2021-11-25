#include "mainwindow.h"
#include "ui_mainwindow.h"


inline int floorDivision(int x, int y) {
    return floor(static_cast<float>(x) / static_cast<float>(y));
}


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    botEditorWindow = new BotEditor(this);
    renderTimer = new QTimer(this);
    graphTimer = new QTimer(this);

    ui->DrawArea->setScene(scene);

    connect(ui->startButton, SIGNAL(released()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->newWorldButton, SIGNAL(released()), this, SLOT(newWorld()));
    connect(ui->openBotEditorButton, SIGNAL(released()), this, SLOT(openBotEditor()));

    connect(ui->radioColorByUsedGens, SIGNAL(clicked()), this, SLOT(renderTypeChanged()));
    connect(ui->radioColorByGensType, SIGNAL(clicked()), this, SLOT(renderTypeChanged()));
    connect(ui->radioColorByEnergyCount, SIGNAL(clicked()), this, SLOT(renderTypeChanged()));
    connect(ui->radioColorByOld, SIGNAL(clicked()), this, SLOT(renderTypeChanged()));
    connect(ui->radioColorByMineralsCount, SIGNAL(clicked()), this, SLOT(renderTypeChanged()));

    connect(renderTimer, SIGNAL(timeout()), this, SLOT(renderUI()));
    connect(graphTimer, SIGNAL(timeout()), this, SLOT(renderGraph()));

    statusLed = new KLed(ui->statusLed);
    statusLed->setShape(KLed::Circular);
    statusLed->setColor(QColor(255, 165, 0));  // orange

    processTimeLed = new KLed(ui->processTimeLed);
    processTimeLed->setColor(Qt::gray);

    // Plotting
    aliveBotHistory = new QLineSeries();
    organicBotHistory = new QLineSeries();
    *aliveBotHistory << QPoint(0, 0);
    *organicBotHistory << QPoint(0, 0);
    aliveBotHistory->setName("Живые");
    organicBotHistory->setName("Органика");
    aliveBotHistory->setColor(Qt::green);
    organicBotHistory->setColor(Qt::gray);

    historyPlot = new QChart();
    historyAxisX = new QValueAxis();
    historyAxisY = new QValueAxis();
    historyPlot->addSeries(aliveBotHistory);
    historyPlot->addSeries(organicBotHistory);
    historyPlot->addAxis(historyAxisX, Qt::AlignBottom);
    historyPlot->addAxis(historyAxisY, Qt::AlignLeft);
    aliveBotHistory->attachAxis(historyAxisX);
    aliveBotHistory->attachAxis(historyAxisY);
    organicBotHistory->attachAxis(historyAxisX);
    organicBotHistory->attachAxis(historyAxisY);

    historyLayout = new QGridLayout(ui->historyView);
    historyView = new QChartView(historyPlot);
    historyLayout->addWidget(historyView);
    historyView->setRenderHint(QPainter::Antialiasing);
    historyView->setParent(ui->historyView);
    historyView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->splitter->setSizes(QList<int>{100, 600});  // TODO: fix ui
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::renderTypeChanged() {
    if (worldInited) {
        renderUI();
        startMouseHandler();
    }
}

void MainWindow::openBotEditor() {
    botEditorWindow->show();
    if (renderTimer->isActive()) botEditorWindow->startMon();
}

void MainWindow::mousePress(QPointF position) {
    uint position_x = position.x();
    uint position_y = ui->DrawArea->height() - position.y();
    int botX = floorDivision(position_x, botSize);
    int botY = floorDivision(position_y, botSize) + 1;

    ulong botHash = hashxy(botX, botY);
    if (world->bots.contains(botHash)) {
        Bot *bot = world->bots.value(botHash);
        botEditorWindow->loadBot(bot);
        botEditorWindow->single();
    }
    renderUI();
    startMouseHandler();
}

void MainWindow::initWorld(uint x, uint y) {
    world = new GeneticWorld();
    world->genomeLen = ui->genome_len->value();
    world->worldPartsCount = ui->world_parts_count->value();
    world->partLength = floorDivision(y, world->worldPartsCount);

    y = (world->worldPartsCount*world->partLength);

    world->maxX = x;
    world->maxY = y;
    world->maxBotCount = x*y;

    Bot *newBot = world->newBot(10, world->maxY-10);
    newBot->energy = ui->first_bot_energy->value();
    newBot->direction = ui->first_bot_direction->currentIndex();
    newBot->genome.fill(commands::photosynthesis, world->genomeLen);
    newBot->genomeStatisticInit();
    worldInited = true;
}

void MainWindow::updateWorldSettings() {
    world->genomeLen = ui->genome_len->value();
    world->maxEnergy = ui->max_energy->value();
    world->botMutateChance = ui->botMutationChance->value();
    world->startWorldPhotosynthesisEnergy = ui->startWorldPhotosynthesis->value();
    world->startWorldMinerals = ui->starWorldMinerals->value();
    world->mineralsPartSize = ui->mineralsPartSize->value();
    world->maxOld = ui->max_old->value();
    world->maxOrganicOld = ui->max_organic_old->value();
    world->organicEnabled = ui->organic->isChecked();
    world->newBotEnergy = ui->new_bot_energy->value();
    world->processDelay = ui->process_delay->value();
    world->organicEnergy = ui->organicEnergy->value();
    world->mineralPrice = ui->mineralPrice->value();
    world->reproductionPrice = ui->reproductionPrice->value();
    world->mutateGenRange = ui->mutateGenRange->value();
    ui->bot_completion->setMaximum(world->maxBotCount);

    world->eatK = ui->eat_k->value();
    world->mutateAttackCount = ui->mutateAttackCount->value();
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
    ui->botMutationChance->setEnabled(false);
    ui->draw_lines->setEnabled(true);
    ui->world_parts_count->setEnabled(false);
    ui->startWorldPhotosynthesis->setEnabled(false);
    ui->starWorldMinerals->setEnabled(false);
    ui->mineralsPartSize->setEnabled(false);
    ui->max_old->setEnabled(false);
    ui->max_organic_old->setEnabled(false);
    ui->eat_k->setEnabled(false);
    ui->organic->setEnabled(false);
    ui->new_bot_energy->setEnabled(false);
    ui->first_bot_energy->setEnabled(false);
    ui->first_bot_direction->setEnabled(false);
    ui->organicEnergy->setEnabled(false);
    ui->mineralPrice->setEnabled(false);
    ui->reproductionPrice->setEnabled(false);
    ui->mutateAttackCount->setEnabled(false);
    ui->mutateGenRange->setEnabled(false);

    //graph
    if (ui->groupBox_graph->isChecked()) {
        historyPlot->setEnabled(true);
        graphTimer->start(ui->graph_freq->value());
    }
    ui->groupBox_graph->setEnabled(false);


    // world
    if (!worldInited) {
        botSize = ui->bot_size->value();
        uint worldW = ui->DrawArea->width() / botSize;
        uint worldH = ui->DrawArea->height() / botSize;
        initWorld(worldW, worldH);

        uint windowW = botSize * world->maxX;
        uint windowH = botSize * world->maxY;
        ui->DrawArea->setMinimumSize(windowW, windowH);
        ui->DrawArea->setMaximumSize(windowW, windowH);
        scene->setSceneRect(0, botSize*2, ui->DrawArea->width(), ui->DrawArea->height());

        QString strSize = QString::number(world->maxX) + " x " + QString::number(world->maxY) + " (" + QString::number(world->maxBotCount) + ")";
        ui->sizeLabel->setText(strSize);
        ui->bot_completion->setMaximum(world->maxBotCount);
        ui->bot_completion->setFormat("%v/" + QString::number(world->maxBotCount) + " (%p%)");

        aliveBotHistory->clear();
        organicBotHistory->clear();
        *aliveBotHistory << QPoint(0, 0);
        *organicBotHistory << QPoint(0, 0);
    }
    updateWorldSettings();

    // starting
    world->start();
    renderTimer->start(ui->timerInterval->value());
    botEditorWindow->startMon();
    statusLed->setColor(QColor(0, 255, 0));
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
    ui->botMutationChance->setEnabled(true);
    ui->draw_lines->setEnabled(false);
    ui->startWorldPhotosynthesis->setEnabled(true);
    ui->starWorldMinerals->setEnabled(true);
    ui->mineralsPartSize->setEnabled(true);
    ui->max_old->setEnabled(true);
    ui->max_organic_old->setEnabled(true);
    ui->eat_k->setEnabled(true);
    ui->new_bot_energy->setEnabled(true);
    ui->organicEnergy->setEnabled(true);
    ui->mineralPrice->setEnabled(true);
    ui->reproductionPrice->setEnabled(true);
    ui->mutateAttackCount->setEnabled(true);
    ui->mutateGenRange->setEnabled(true);

    //graph
    ui->groupBox_graph->setEnabled(true);
    historyPlot->setEnabled(false);
    ui->groupBox_graph->setEnabled(true);

    renderUI();  // render last
    statusLed->setColor(QColor(255, 128, 0));
    botEditorWindow->stopMon();
    startMouseHandler();
}

void MainWindow::newWorld() {
    if (world->isRunning()) return;
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
    ui->botMutationChance->setEnabled(true);
    ui->draw_lines->setEnabled(false);
    ui->world_parts_count->setEnabled(true);
    ui->startWorldPhotosynthesis->setEnabled(true);
    ui->starWorldMinerals->setEnabled(true);
    ui->mineralsPartSize->setEnabled(true);
    ui->max_old->setEnabled(true);
    ui->world_parts_count->setEnabled(true);
    ui->eat_k->setEnabled(true);
    ui->organic->setEnabled(true);
    ui->first_bot_direction->setEnabled(true);
    ui->first_bot_energy->setEnabled(true);
    ui->mineralPrice->setEnabled(true);
    ui->reproductionPrice->setEnabled(true);
    ui->new_bot_energy->setEnabled(true);
    ui->organicEnergy->setEnabled(true);
    ui->mutateAttackCount->setEnabled(true);
    ui->mutateGenRange->setEnabled(true);

    //graph
    historyPlot->setEnabled(false);
    ui->groupBox_graph->setEnabled(true);

    ui->generation->setText("0");
    ui->sizeLabel->setText("");
    ui->bot_count->display(0);
    ui->processing_time->setText("0");
    ui->bot_completion->setValue(0);
    statusLed->setColor(QColor(255, 128, 0));
    processTimeLed->setColor(QColor(123, 123, 123));
    ui->DrawArea->setMinimumSize(0, 0);
    ui->DrawArea->setMaximumSize(16777215, 16777215);
    scene->clear();
    worldInited = false;
    delete world;
    renderGraph(true);
}

void MainWindow::startMouseHandler() {
    mousePressHandler = new MouseHandlerItem(ui->DrawArea->width(), ui->DrawArea->height());
    scene->addItem(mousePressHandler);
    connect(mousePressHandler, SIGNAL(mousePress(QPointF)), this, SLOT(mousePress(QPointF)));
}

QColor MainWindow::botColorByType(Bot *bot) {
    if (bot->monitoring) return QColor(Qt::magenta);
    if (bot->type == ORGANIC) return QColor(Qt::gray);
    float total_number = bot->mineralsCount + bot->photosynthesisCount + bot->eatCount;
    uint B = floorDivision(bot->mineralsCount*255, total_number);
    uint G = floorDivision(bot->photosynthesisCount*255, total_number);
    uint R = floorDivision(bot->eatCount*255, total_number);
    return QColor(R, G, B);
}

QColor MainWindow::botColorByEnergy(Bot *bot) {
    if (bot->monitoring) return QColor(Qt::blue);
    if (bot->type == ORGANIC) return QColor(Qt::gray);
    uint color = floorDivision(bot->energy * 255, world->maxEnergy);
    if (color > 255) color = 255;
    return QColor(255, 255-color, 0);
}

QColor MainWindow::botColorByUsedGens(Bot *bot) {
    if (bot->monitoring) return QColor(Qt::magenta);
    if (bot->type == ORGANIC) return QColor(Qt::gray);
    float totalNumber = bot->usedEat + bot->usedMinerals + bot->usedPhotosynthesis;
    if (!totalNumber) return QColor(Qt::white);
    uint B = floorDivision(bot->usedMinerals*255, totalNumber);
    uint G = floorDivision(bot->usedPhotosynthesis*255, totalNumber);
    uint R = floor(pow(static_cast<float>(bot->usedEat) / static_cast<float>(totalNumber), 0.1)*255);
    return QColor(R, G, B);
}

QColor MainWindow::botColorByOld(Bot *bot) {
    if (bot->monitoring) return QColor(Qt::blue);
    if (bot->type == ORGANIC) return QColor(Qt::gray);
    uint color = floorDivision(bot->old * 255, world->maxOld);
    if (color > 255) color = 255;
    return QColor(255, 255-color, 0);
}

QColor MainWindow::botColorByMinerals(Bot *bot) {
    if (bot->monitoring) return QColor(Qt::magenta);
    if (bot->type == ORGANIC) return QColor(Qt::gray);
    uint color = floorDivision(bot->minerals * 255, 100);
    if (color > 255) color = 255;
    return QColor(255-color, 255-color, 255);
}

void MainWindow::renderGraph() {
    renderGraph(false);
}

QPair<QPair<int, int>, QPair<int, int>> MainWindow::_find2Max() {
    int Max = 0, maxCount = 0, preMax = 0, preMaxCount = 0;
    for (int i = 1; i<aliveBotHistory->count(); i++) {
        int value = aliveBotHistory->at(i).y();
        if (value > Max) {
            preMax = Max;
            preMaxCount = maxCount;
            Max = value;
            maxCount = 1;
        } else if (value == Max) maxCount++;
        else if (value > preMax) {
            preMax = value;
            preMaxCount = 1;
        } else if (value == preMax) preMaxCount++;
    }
    return qMakePair(qMakePair(Max, maxCount), qMakePair(preMax, preMaxCount));
}

void MainWindow::renderGraph(bool reset) {  // find max and set range
    static int x = 0;

    static int Max = 0;
    static int maxCount = 0;

    static int preMax = 0;
    static int preMaxCount = 0;

    if (reset) {
        x = Max = maxCount = preMax = preMaxCount = 0;
        aliveBotHistory->clear();
        organicBotHistory->clear();
        *aliveBotHistory << QPoint(0, 0);
        *organicBotHistory << QPoint(0, 0);
        return;
    }
    x++;
    int aliveBotCount = world->aliveBotsCount;
    int organicBotCount = world->bots.size() - aliveBotCount;
    int current = std::max(aliveBotCount, organicBotCount);

    if (current > Max) {
        preMax = Max;
        preMaxCount = maxCount;
        Max = current;
        maxCount = 1;
        historyAxisY->setRange(0, Max);
    } else if (current == Max) {
        maxCount++;
    } else if (current > preMax) {
        preMax = current;
        preMaxCount = 1;
    } else if (current == preMax) {
        preMaxCount++;
    }

    *aliveBotHistory << QPoint(x, aliveBotCount);
    *organicBotHistory << QPoint(x, organicBotCount);

    if (aliveBotHistory->count() > ui->graph_count->value()) {
        int lastElement = std::max(aliveBotHistory->at(0).y(), organicBotHistory->at(0).y());
        if (lastElement == Max) {
            maxCount--;
            if (!maxCount) {
                auto result = _find2Max();  // QPair<QPair<Max, maxCount>, QPair<preMax, preMaxCount>>
                Max = result.first.first;
                maxCount = result.first.second;
                historyAxisY->setRange(0, Max);
                preMax = result.second.first;
                preMaxCount = result.second.second;
            }
        } else if (lastElement == preMax) {
            preMaxCount--;
            if (!preMaxCount) {
                auto result = _find2Max();
                preMax = result.second.first;
                preMaxCount = result.second.second;
            }
        }

        aliveBotHistory->remove(0);
        organicBotHistory->remove(0);
    }

    historyAxisX->setRange(x-aliveBotHistory->count()+1, x);
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
        if (world->processingTime >= world->processDelay*1000) processTimeLed->setColor(Qt::red);
        else processTimeLed->setColor(Qt::green);
    } else processTimeLed->setColor(Qt::gray);

    uint botCount = world->bots.size();
    uint aliveBotCount = world->aliveBotsCount;
    assert(botCount >= aliveBotCount);

    if (!aliveBotCount) {  // all bots died
        world->stop();
        renderTimer->stop();
        graphTimer->stop();
        botEditorWindow->stopMon();
        ui->newWorldButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        statusLed->setColor(Qt::red);
        return;
    }

    // bot render
    std::function<QColor (MainWindow*, Bot*)> getColor;

    if (ui->radioColorByGensType->isChecked())
        getColor = &MainWindow::botColorByType;
    else if (ui->radioColorByEnergyCount->isChecked())
        getColor = &MainWindow::botColorByEnergy;
    else if (ui->radioColorByUsedGens->isChecked())
        getColor = &MainWindow::botColorByUsedGens;
    else if (ui->radioColorByOld->isChecked())
        getColor = &MainWindow::botColorByOld;
    else if (ui->radioColorByMineralsCount->isChecked())
        getColor = &MainWindow::botColorByMinerals;
    else
        assert(false);

    //world->botsMutex.lock();
    foreach (Bot *bot, world->bots) {
            if (bot->type == DEAD) continue;
            QColor botColor = getColor(this, bot);
            scene->addRect(bot->getX() * botSize, ui->DrawArea->height() - (bot->getY() * botSize), botSize-1, botSize-1, QPen(botColor), QBrush(botColor));
        }
    //world->botsMutex.unlock();

    // ui info update
    ui->bot_count->display(QString::number(aliveBotCount));
    ui->generation->setText(QString::number(world->generation));
    ui->bot_completion->setValue(botCount);
    ui->processing_time->setText(QString::number(world->processingTime) + " (мкс)");

    // draw lines
    if (ui->draw_lines->isChecked()) {
        uint current_height, coordinates_y;
        for (uint part = 0; part<=world->worldPartsCount; ++part) {
            current_height = ui->DrawArea->height() - (world->partLength * part) * botSize;

            coordinates_y = world->partLength * part;
            if (current_height != 0)
                scene->addLine(0, current_height, ui->DrawArea->width(), current_height, QPen(Qt::gray));
            if (!(part%world->mineralsPartSize) && part < world->mineralsPartSize*world->startWorldMinerals)  // minerals
                scene->addItem(textWidget(QString::number(world->getMineralsCount(coordinates_y)), 0, current_height - 25, Qt::white));
            if (part >= (world->worldPartsCount - world->startWorldPhotosynthesisEnergy)) // photosynthesis
                scene->addItem(textWidget(QString::number(world->getPhotosynthesisEnergy(coordinates_y)), ui->DrawArea->width() - 20, current_height - 25, Qt::white));
        }

        /* // Debug
        uint last_energy_p = 0, last_energy_m = 0, energy;
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
        world->stop();
        world->wait();
    }
    QWidget::closeEvent(event);
}
