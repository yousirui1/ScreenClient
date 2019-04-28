#include <QPainter>
#include <QRect>
#include <QDesktopWidget>
#include <QDir>
#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "decodethread.h"
#include "recvthread.h"

extern int run_flag;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#if 0
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    m_width = screenRect.width();
    m_height = screenRect.height();
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(char *ip, int port, int window_flag)
{
    RecvThread *recv = new RecvThread(ip, port);
    recv->start();

    DecodeThread *deocde = new DecodeThread();
    deocde->start();

    if(!window_flag)
    {
        this->showFullScreen();
    }


    QString filepath = QDir::currentPath()+"/images/background.jpg";
    mImage.load(filepath);

    QString iconpath = QDir::currentPath()+"/images/icon.jpg";
    setWindowIcon(QIcon(iconpath));

    connect(deocde, SIGNAL(sigGetFrame(QImage)), this, SLOT(slotGetFrame(QImage)));
    connect(recv, SIGNAL(sigNoRecv()), this, SLOT(slotCloseWindow()));
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height()); //先画成黑色

    if(!mImage.isNull())
    {
        if (mImage.size().width() <= 0) return;

        ///将图像按比例缩放成和窗口一样大小
        QImage img = mImage.scaled(this->size(),Qt::IgnoreAspectRatio, Qt::FastTransformation);
        painter.drawImage(QPoint(0, 0),img); //画出图像
    }
#if 0
    if(m_width != this->width() || m_height != this->height())
    {
        m_width = this->width();
        m_height = this->height();
        first_time = 1;
    }
#endif
}

void MainWindow::slotGetFrame(QImage img)
{
    mImage = img;
    update(); //调用update将执行 paintEvent函数
}

void MainWindow::slotCloseWindow()
{
    this->close();
    run_flag = 0;
    qApp->exit(0);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() == (Qt::ControlModifier | Qt::AltModifier) && e->key() == Qt::Key_S)
    {
        qWarning("Ctrl + ALT + S");
    }
    else if(e->modifiers() == (Qt::ControlModifier | Qt::AltModifier) && e->key() == Qt::Key_X)
    {
        qWarning("Ctrl + ALT + X");
    }
    else if(e->modifiers() == (Qt::ControlModifier | Qt::AltModifier) && e->key() == Qt::Key_T)
    {
        qWarning("Ctrl + ALT + T");
    }
    else if(e->modifiers() == (Qt::ControlModifier | Qt::AltModifier) && e->key() == Qt::Key_U)
    {
        qWarning("Ctrl + ALT + U");
    }
    else
    {

    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
}
