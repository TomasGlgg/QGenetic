#ifndef BOTEDITOR_H
#define BOTEDITOR_H

#include <QDialog>

namespace Ui {
class botEditor;
}

class botEditor : public QDialog
{
    Q_OBJECT

public:
    explicit botEditor(QWidget *parent = nullptr);
    ~botEditor();

private:
    Ui::botEditor *ui;
};

#endif // BOTEDITOR_H
