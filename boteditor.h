#ifndef BOTEDITOR_H
#define BOTEDITOR_H

#include <QDialog>
#include <QTimer>
#include <QSpinBox>
#include <QSignalMapper>

#include <cmath>

#include "botstruct.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BotEditor; }
QT_END_NAMESPACE

class BotEditor : public QDialog
{
    Q_OBJECT

public:
    explicit BotEditor(QWidget *parent = nullptr);
    ~BotEditor();

    void loadBot(Bot *bot);
    void startMon();
    void stopMon();
    void single();

    Bot* getBot();


private:
    Ui::BotEditor *ui;
    QSignalMapper *tableSignalMapper;
    QSignalMapper *infoSignalMapper;
    QTimer *infoUpdateTimer;
    QTimer *tableUpdateTimer;
    Bot *bot;

    uint columntCount = 10;

    bool inited = false;
    bool monitoring = false;

    void disableUI();
    void enableUI();

    void closeEvent(QCloseEvent *event);

private slots:
    void renderInfo();
    void renderTable();

    void resetSelection();

    void botKilled();
    void botInfoEdited(int index);
    void botGenomeEdited(int genomeIndex);
};

QStringList progression(uint max);

#endif // BOTEDITOR_H
