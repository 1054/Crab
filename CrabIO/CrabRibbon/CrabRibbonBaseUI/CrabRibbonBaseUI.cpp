#include "CrabRibbonBaseUI.h"
#include "ui_CrabRibbonBaseUI.h"

CrabRibbonBaseUI::CrabRibbonBaseUI(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    ui(new Ui::CrabRibbonBaseUI)
{
    ui->setupUi(this);

    m_CoreWidget = NULL;

    this->setAcceptDrops(true);

    /*
     * Head Widget -- Buttons
     */
    setupHead();

    /*
     * Core Widget
     */
    // setupCore();

}


CrabRibbonBaseUI::~CrabRibbonBaseUI()
{
    qDebug() << "CrabRibbonBaseUI::~CrabRibbonBaseUI() m_CoreWidget =" << m_CoreWidget;
    delete ui;
}


void CrabRibbonBaseUI::setupHead(QString label)
{
    qDebug() << "CrabRibbonBaseUI::setupHead()";
    if(!label.isEmpty()) {
        ui->Label->setText(label);
    }
    if(!ui->ButtonBye->menu()) {
        QMenu *menu = new QMenu();
        menu->addAction("Bye");
        ui->ButtonBye->setupMenu(menu);
        connect(ui->ButtonBye,SIGNAL(bye()),this,SLOT(close()));
    }
}


void CrabRibbonBaseUI::setupCore(QWidget *widget)
{
    qDebug() << "CrabRibbonBaseUI::setupCore()";
    if(widget) {
        // if(!ui->Core->layout()) <TODO>
        QHBoxLayout *layout = new QHBoxLayout(ui->Core);
        layout->addWidget(widget);
        layout->setMargin(0);
        layout->setSpacing(0);
        ui->Core->setLayout(layout);
        m_CoreWidget = widget;
        qDebug() << "CrabRibbonBaseUI::setupCore()" << "ui->Core->styleSheet()";
        qDebug() << ui->Core->styleSheet();
        qDebug() << "CrabRibbonBaseUI::setupCore()" << "widget->styleSheet()";
        qDebug() << widget->styleSheet();
    }
}


void CrabRibbonBaseUI::paintEvent(QPaintEvent *event)
{
    /*
     * Rounded Rect
     * see http://developer.nokia.com/community/wiki/Qt_rounded_rect_widget
     */
    if(1==0) {
        qreal opacity(1.0);
        int roundness(5);
        QRect widget_rect = this->rect();
        // painter
        QColor ribbon_color = this->palette().color(this->backgroundRole());
        QPainter painter(this);
        painter.save();
        // painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(ribbon_color);
        // clip
        QPainterPath rounded_rect;
        // rounded_rect.addRoundRect(1, 1, widget_rect.width()-2, widget_rect.height()-2, roundness, roundness);
        rounded_rect.addRoundRect(0, 0, widget_rect.width(), widget_rect.height(), roundness, roundness);
        painter.setClipPath(rounded_rect);
        // get clipping region
        QRegion maskregion = painter.clipRegion();
        // mask the widget
        this->setMask(maskregion);
        painter.setOpacity(opacity);
        // fill path with color
        painter.fillPath(rounded_rect,QBrush(ribbon_color));
        // restore painter
        painter.restore();
    }
    /*
     * QWidget::paintEvent(event);
     */
    QWidget::paintEvent(event);
}

void CrabRibbonBaseUI::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list")){event->acceptProposedAction();}
    qDebug() << "CrabRibbonBaseUI::dragEnterEvent()" << event->mimeData();
    QWidget::dragEnterEvent(event);
}

void CrabRibbonBaseUI::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls(); if(urls.isEmpty()) {return;}
    QString FilePath = urls.first().toLocalFile(); QFileInfo FileInfo(FilePath);
    if(!FileInfo.isFile()) {qDebug() << "CrabRibbonBaseUI::dropEvent() File should be a file!"; return;}
    if(!FileInfo.isReadable()) {qDebug() << "CrabRibbonBaseUI::dropEvent() Error! File is unreadable!"; return;}
    //QString FileType = FilePath.section(".",-1); //section separated by ".", and -1 means start from the right end.
    //if(FileType!="pdf") {qDebug() << "CrabPDFReader::dropEvent() Error! File should be a pdf!"; return;}
    /* Send open file signal */
    emit signal_open_file(FilePath);
    QWidget::dropEvent(event);
}

void CrabRibbonBaseUI::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "CrabRibbonBaseUI::mousePressEvent()" << this->childAt(event->pos()) << ui->Core->childAt(event->pos());
    if(Qt::LeftButton==event->button()) {
        if(!this->childAt(event->pos()) || this->childAt(event->pos())!=ui->Core->childAt(event->pos())) {
            m_MovePos = event->pos(); // qDebug() << "CrabRibbonBaseUI::mousePressEvent()" << event->globalPos() << event->pos() << m_MovePos.x() << m_MovePos.y();
        }
    }
    QWidget::mousePressEvent(event);
}


void CrabRibbonBaseUI::mouseReleaseEvent(QMouseEvent *event)
{
    if(Qt::LeftButton==event->button()) {
        m_MovePos = QPoint(); // qDebug() << "CrabRibbonBaseUI::mouseReleaseEvent()" << m_MovePos.isNull();
    }
    QWidget::mouseReleaseEvent(event);
}

void CrabRibbonBaseUI::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "CrabRibbonBaseUI::mouseMoveEvent()" << event->buttons() << (event->buttons()%Qt::LeftButton);
    if((!(event->buttons()%Qt::LeftButton)) && !(m_MovePos.isNull())) {
        this->move(event->globalX()-m_MovePos.x(),event->globalY()-m_MovePos.y()); // qDebug() << "CrabRibbonBaseUI::mouseMoveEvent()" << event->globalPos() << event->pos();
    }
    QWidget::mouseMoveEvent(event);
}
