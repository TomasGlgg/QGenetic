#include "boteditor.h"
#include "ui_boteditor.h"

botEditor::botEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::botEditor)
{
    ui->setupUi(this);
}

botEditor::~botEditor()
{
    delete ui;
}
