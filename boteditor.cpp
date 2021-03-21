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

void BotEditor::loadBot(Bot *bot) {
    this->bot = bot;
    inited = true;
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
}
