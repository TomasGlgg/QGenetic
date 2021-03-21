#include "boteditor.h"
#include "ui_boteditor.h"

BotEditor::BotEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BotEditor)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(render()));

    ui->tableWidget->setHorizontalHeaderLabels(numberProgression(columntCount));
}

BotEditor::~BotEditor()
{
    delete ui;
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
    ui->tableWidget->setVerticalHeaderLabels(numberProgression(rowCount));
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
    ui->type->setCurrentIndex(KILLED);
}

ulong BotEditor::botHash() {
    if (!inited) return 0;
    return bot->getHash();
}

void BotEditor::closeEvent(QCloseEvent *event) {
    stopMon();
    QWidget::closeEvent(event);
}

QStringList numberProgression(uint max) {
    QStringList result;
    for (uint i = 0; i<max; i++) result << QString::number(i);
    return result;
}
