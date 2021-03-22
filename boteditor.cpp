#include "boteditor.h"
#include "ui_boteditor.h"


BotEditor::BotEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BotEditor)
{
    ui->setupUi(this);
    infoUpdateTimer = new QTimer(this);
    tableUpdateTimer = new QTimer(this);
    connect(infoUpdateTimer, SIGNAL(timeout()), this, SLOT(renderInfo()));

    tableSignalMapper = new QSignalMapper(this);
    infoSignalMapper = new QSignalMapper(this);

    connect(ui->type, SIGNAL(currentIndexChanged(int)), infoSignalMapper, SLOT(map()));
    infoSignalMapper->setMapping(ui->type, 0);
    connect(ui->energy, SIGNAL(valueChanged(int)), infoSignalMapper, SLOT(map()));
    infoSignalMapper->setMapping(ui->energy, 1);
    connect(ui->old, SIGNAL(valueChanged(int)), infoSignalMapper, SLOT(map()));
    infoSignalMapper->setMapping(ui->old, 2);
    connect(ui->iterator, SIGNAL(valueChanged(int)), infoSignalMapper, SLOT(map()));
    infoSignalMapper->setMapping(ui->iterator, 3);
    connect(ui->minerals, SIGNAL(valueChanged(int)), infoSignalMapper, SLOT(map()));
    infoSignalMapper->setMapping(ui->minerals, 4);

    connect(tableSignalMapper, SIGNAL(mapped(int)), this, SLOT(botGenomeEdited(int)));
    connect(infoSignalMapper, SIGNAL(mapped(int)), this, SLOT(botInfoEdited(int)));

    ui->tableWidget->setHorizontalHeaderLabels(progression(columntCount));
}

BotEditor::~BotEditor()
{
    delete ui;
}

void BotEditor::botInfoEdited(int index) {
    switch (index) {
        case 0: {
            bot->type = ui->type->currentIndex();
            break;
        }
        case 1: {
            bot->energy = ui->energy->value();
            break;
        }
        case 2: {
            bot->old = ui->old->value();
            break;
        }
        case 3: {
            bot->iterator = ui->iterator->value();
            break;
        }
        case 4: {
            bot->minerals = ui->minerals->value();
            break;
        }
    }
}

void BotEditor::botGenomeEdited(int genomeIndex) {
    uint rowIndex = floor(static_cast<float>(genomeIndex) / static_cast<float>(columntCount));
    uint columnIndex = genomeIndex%columntCount;
    uint newValue = ((QSpinBox*)ui->tableWidget->cellWidget(rowIndex, columnIndex))->value();
    bot->genome[genomeIndex] = newValue;
}

void BotEditor::disableUI() {
    ui->tableWidget->setEnabled(false);
    ui->type->setEnabled(false);
    ui->energy->setEnabled(false);
    ui->old->setEnabled(false);
    ui->iterator->setEnabled(false);
    ui->minerals->setEnabled(false);
    ui->photosynthesisUsed->setEnabled(false);
    ui->mineralsUsed->setEnabled(false);
    ui->eatStealUsed->setEnabled(false);
}

void BotEditor::enableUI() {
    ui->tableWidget->setEnabled(true);
    ui->type->setEnabled(true);
    ui->energy->setEnabled(true);
    ui->old->setEnabled(true);
    ui->iterator->setEnabled(true);
    ui->minerals->setEnabled(true);
    ui->photosynthesisUsed->setEnabled(true);
    ui->mineralsUsed->setEnabled(true);
    ui->eatStealUsed->setEnabled(true);
}

void BotEditor::loadBot(Bot *bot) {
    this->bot = bot;
    bot->monitoring = true;
    connect(bot, SIGNAL(botKilled()), this, SLOT(botKilled()));
    inited = true;
    enableUI();
    uint rowCount = ceil(static_cast<float>(bot->genome.size()) / static_cast<float>(columntCount));
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setVerticalHeaderLabels(progression(rowCount));
}

void BotEditor::startMon() {
    if (inited && !monitoring) {  // TODO: move to ui
        infoSignalMapper->blockSignals(false);
        infoUpdateTimer->start(100);    // info  - 0.1 s
        tableUpdateTimer->start(1000);  // table - 1 s
        monitoring = true;
    }
}

void BotEditor::stopMon() {
    if (monitoring) {
        infoUpdateTimer->stop();
        tableUpdateTimer->stop();
        monitoring = false;
    }
}

void BotEditor::single() {
    infoUpdateTimer->singleShot(0, this, SLOT(renderInfo()));
    tableUpdateTimer->singleShot(0, this, SLOT(renderTable()));
}

void BotEditor::renderTable() {
    for (uint genomeIndex = 0; genomeIndex < (uint)bot->genome.size(); genomeIndex++) {
        uint columntIndex = genomeIndex%columntCount;
        uint rowIndex = floor(static_cast<float>(genomeIndex) / static_cast<float>(columntCount));

        QSpinBox *spinBox = new QSpinBox();
        int genValue = bot->genome[genomeIndex];
        spinBox->setMinimum(INT_MIN);
        spinBox->setValue(genValue);
        ui->tableWidget->setCellWidget(rowIndex, columntIndex, spinBox);
        connect(spinBox, SIGNAL(valueChanged(int)), tableSignalMapper, SLOT(map()));
        tableSignalMapper->setMapping(spinBox, genomeIndex);
    }
}

void BotEditor::renderInfo() {
    infoSignalMapper->blockSignals(true);
    ui->energy->setValue(bot->energy);
    ui->iterator->setValue(bot->iterator);
    ui->old->setValue(bot->old);
    ui->minerals->setValue(bot->minerals);
    ui->photosynthesisUsed->setText(QString::number(bot->usedPhotosynthesis));
    ui->mineralsUsed->setText(QString::number(bot->usedMinerals));
    ui->eatStealUsed->setText(QString::number(bot->usedEat));
    ui->type->setCurrentIndex(bot->type);
    infoSignalMapper->blockSignals(false);
}


void BotEditor::botKilled() {
    infoSignalMapper->blockSignals(true);
    bot = nullptr;
    stopMon();
    inited = false;
    disableUI();
    ui->type->setCurrentIndex(KILLED);
}

ulong BotEditor::botHash() {
    if (!inited) return ULONG_MAX;
    return bot->getHash();
}

void BotEditor::closeEvent(QCloseEvent *event) {
    stopMon();
    QWidget::closeEvent(event);
}

QStringList progression(uint max) {
    QStringList result;
    for (uint i = 0; i<max; i++) result << QString::number(i);
    return result;
}
