#include "CrabWordsOnTop.h"
#include "ui_CrabWordsOnTop.h"

CrabWordsOnTop::CrabWordsOnTop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrabWordsOnTop)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_NoSystemBackground, true);
    //setAttribute(Qt::WA_OpaquePaintEvent, false);

    // Fully frameless and transparent window
    // https://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //setParent(0); // Create TopLevel-Widget
    setAttribute(Qt::WA_NoSystemBackground, true);
    //setAttribute(Qt::WA_TranslucentBackground, true);
    //setAttribute(Qt::WA_PaintOnScreen); // not needed in Qt 5.2 and up

    crab_move(0.05,0.0);

    // add menu
    //QVBoxLayout *Box_Layout = new QVBoxLayout(this); // Main layout of widget
    QMenuBar* Menu_Bar = new QMenuBar();
    QMenu *Menu_1 = new QMenu("Move");
    QAction *Menu_Action_1 = new QAction(tr("Move"));
    Menu_Bar->addMenu(Menu_1);
    Menu_1->addAction(Menu_Action_1);
    connect(Menu_Action_1,SIGNAL(triggered(bool)),this,SLOT(crab_move()));
    QMenu *Menu_2 = new QMenu("Edit");
    QAction *Menu_Action_2 = new QAction(tr("Edit"));
    Menu_Bar->addMenu(Menu_2);
    Menu_2->addAction(Menu_Action_2);
    connect(Menu_Action_2,SIGNAL(triggered(bool)),this,SLOT(crab_edit()));
    QMenu *Menu_3 = new QMenu("Exit");
    QAction *Menu_Action_3 = new QAction(tr("Exit"));
    Menu_Bar->addMenu(Menu_3);
    Menu_3->addAction(Menu_Action_3);
    connect(Menu_Action_3,SIGNAL(triggered(bool)),this,SLOT(close()));
    this->layout()->setMenuBar(Menu_Bar);

    // edit text
    crab_edit(crab_note());

}

CrabWordsOnTop::~CrabWordsOnTop()
{
    delete ui;
}

void CrabWordsOnTop::crab_edit(QString input_text)
{
    // pop up a window and input text <TODO>
    if(input_text.isEmpty()) {
        bool ok;
        QString temp_text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                                  tr("Pleaase input the text to display:"), QLineEdit::Normal,
                                                  crab_note(), &ok);
        if (ok && !temp_text.isEmpty()) {
            input_text = temp_text;
        }
    }
    // set text to ui
    if(!input_text.isEmpty()) {
        this->ui->lineEdit->setText(input_text);
        QFontMetrics fm(ui->lineEdit->font());
        this->ui->lineEdit->setMinimumWidth(fm.width(input_text)+2);
        this->ui->lineEdit->setMaximumWidth(fm.width(input_text)+2);
        qDebug() << input_text;
    }
}

void CrabWordsOnTop::crab_move(qreal crab_move_to_x, qreal crab_move_to_y)
{
    // pop up a window and input coordinate to move <TODO>
    if(crab_move_to_x<0.0 || crab_move_to_y<0.0) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("Please input the normalized coordinate to move to:"), QLineEdit::Normal,
                                             "0.2 0.005", &ok);
        if (ok && !text.isEmpty()) {
            QStringList text_split = text.split(QRegExp("[ ,]"),QString::SkipEmptyParts);
            if(text_split.count()>=2) {
                crab_move_to_x = text_split.at(0).toDouble();
                crab_move_to_y = text_split.at(1).toDouble();
            }
        }
    }
    if(crab_move_to_x>=0.0 && crab_move_to_y>=0.0) {
        move(QApplication::desktop()->availableGeometry().topLeft() + QPoint(QApplication::desktop()->availableGeometry().width() * crab_move_to_x, QApplication::desktop()->availableGeometry().height() * crab_move_to_y));
    }
}

QString CrabWordsOnTop::crab_note()
{
    QStringList crab_note_list;
    crab_note_list.append(tr("有志者、事竟成，破釜沉舟，百二秦关终属楚；苦心人、天不负，卧薪尝胆，三千越甲可吞吴。"));
    crab_note_list.append(tr("不要自卑，不要逃避，要像从前一样对身边每一个人保持微笑。"));
    // return random
    // see qt random number generation: https://stackoverflow.com/questions/3244999/create-a-random-string-or-number-in-qt4
    return crab_note_list.at(qrand()%(crab_note_list.count()));
}





