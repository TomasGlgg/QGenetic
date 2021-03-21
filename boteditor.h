#ifndef BOTEDITOR_H
#define BOTEDITOR_H

#include <QDialog>
#include <QTimer>
#include <QSpinBox>

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

    ulong botHash();

private:
    Ui::BotEditor *ui;
    QTimer *timer;
    Bot *bot;

    uint columntCount = 10;

    bool inited = false;
    bool monitoring = false;

    void disableUI();
    void enableUI();

    void closeEvent(QCloseEvent *event);

private slots:
    void render();
    void botKilled();
};

#endif // BOTEDITOR_H
