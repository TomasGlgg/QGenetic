#include "boteditor.h"
#include "ui_boteditor.h"

BotEditor::BotEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BotEditor)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(render()));
}

BotEditor::~BotEditor()
{
    delete ui;
}

void BotEditor::disableUI() {
    ui->tableWidget->setEnabled(false);
    ui->energy->setEnabled(false);
}

void BotEditor::enableUI() {
    ui->tableWidget->setEnabled(true);
    ui->energy->setEnabled(true);
}

void BotEditor::loadBot(Bot *bot) {
    this->bot = bot;
    bot->monitoring = true;
    connect(bot, SIGNAL(botKilled()), this, SLOT(botKilled()));
    inited = true;
    enableUI();
}

void BotEditor::startMon() {
    if (inited && !monitoring) {
        timer->start(1000);
        monitoring = true;
    }
}

void BotEditor::stopMon() {
    if (monitoring) {
        timer->stop();
        monitoring = false;
    }
}

void BotEditor::single() {
    timer->singleShot(0, this, SLOT(render()));
}

void BotEditor::render() {
    ui->tableWidget->setRowCount(ceil(static_cast<float>(bot->genome.size()) / static_cast<float>(columntCount)));
    for (uint genIndex = 0; genIndex < (uint)bot->genome.size(); genIndex++) {
        uint columntIndex = genIndex%columntCount;
        uint rowIndex = floor(static_cast<float>(genIndex) / static_cast<float>(columntCount));

        QSpinBox *spinBox = new QSpinBox();
        int genValue = bot->genome[genIndex];
        spinBox->setMinimum(INT_MIN);
        spinBox->setValue(genValue);
        ui->tableWidget->setCellWidget(rowIndex, columntIndex, spinBox);
    }

    // update info
    ui->energy->setValue(bot->energy);
    ui->iterator->setValue(bot->iterator);
    ui->old->setValue(bot->old);
    ui->minerals->setValue(bot->minerals);
    ui->photosynthesisUsed->setText(QString::number(bot->usedPhotosynthesis));
    ui->mineralsUsed->setText(QString::number(bot->usedMinerals));
    ui->eatStealUsed->setText(QString::number(bot->usedEat));
    ui->type->setCurrentIndex(bot->type);
}


void BotEditor::botKilled() {
    bot = nullptr;
    stopMon();
    inited = false;
    disableUI();
}

ulong BotEditor::botHash() {
    if (!inited) return 0;
    return bot->getHash();
}
