#include "CrabBlobListWidget.h"

CrabBlobListWidget::CrabBlobListWidget(QWidget *parent) : QListWidget(parent)
{

    this->setStyleSheet("border-width: 5px; background-color: transparent;");
    this->setMinimumWidth(200);

}
