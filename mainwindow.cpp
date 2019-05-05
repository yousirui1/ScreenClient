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
extern unsigned int m_width;
extern unsigned int m_height;

#define min(a,b) (a<b? a:b)
#define max(a,b) (a>b? a:b)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::X11BypassWindowManagerHint);
	edgeMargin = 10; 		//设置检测边缘为10 
	resizeDir = nodir;	
	setMouseTracking(true);	//开启鼠标追踪
	QCursor tempCursor;                 //获得当前鼠标样式，注意:只能获得当前鼠标样式然后再重新设置鼠标样式
    tempCursor = cursor(); 
	tempCursor.setShape(Qt::ArrowCursor);
	setCursor(tempCursor);                    //重新设置鼠标,主要是改样式
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

	this->window_flag = window_flag;

    if(!window_flag)
    {
	    QDesktopWidget* desktopWidget = QApplication::desktop();
    	QRect screenRect = desktopWidget->screenGeometry();
        this->showFullScreen();
		setGeometry(0,0,screenRect.width(),screenRect.height());
		this->show();
        this->raise();
        this->activateWindow();
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
        QImage img = mImage.scaled(this->size(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        //QImage img = mImage.scaled(this->size(),Qt::KeepAspectRatio);
        painter.drawImage(QPoint(0, 0),img); //画出图像
    }
}

//用于窗口缩放 检测鼠标在窗口边缘事件
void MainWindow::testEdge()
{
    int diffLeft = abs(cursor().pos().x() - frameGeometry().left());      //计算鼠标距离窗口上下左右有多少距离
    int diffRight = abs(cursor().pos().x() - frameGeometry().right());
    int diffTop = abs(cursor().pos().y() - frameGeometry().top());
    int diffBottom = abs(cursor().pos().y() - frameGeometry().bottom());

    QCursor tempCursor;                 //获得当前鼠标样式，注意:只能获得当前鼠标样式然后再重新设置鼠标样式
    tempCursor = cursor();              //因为获得的不是鼠标指针，所以不能这样用:cursor().setXXXXX

    if(diffTop < edgeMargin){           //根据 边缘距离 分类改变尺寸的方向
        if(diffLeft < edgeMargin)
		{
            resizeDir = topLeft;
            tempCursor.setShape(Qt::SizeFDiagCursor);
        }
        else if(diffRight < edgeMargin)
		{
            resizeDir = topRight;
            tempCursor.setShape(Qt::SizeBDiagCursor);
        }
        else
		{
            resizeDir = top;
            tempCursor.setShape(Qt::SizeVerCursor);
        }
    }
    else if(diffBottom < edgeMargin)
	{
        if(diffLeft < edgeMargin)
		{
            resizeDir = bottomLeft;
            tempCursor.setShape(Qt::SizeBDiagCursor);
        }
        else if(diffRight < edgeMargin)
		{
            resizeDir = bottomRight;
            tempCursor.setShape(Qt::SizeFDiagCursor);
        }
        else
		{
            resizeDir = bottom;
            tempCursor.setShape(Qt::SizeVerCursor);
        }
    }
    else if(diffLeft < edgeMargin)
	{
        resizeDir = left;
        tempCursor.setShape(Qt::SizeHorCursor);
    }
    else if(diffRight < edgeMargin){
        resizeDir = right;
        tempCursor.setShape(Qt::SizeHorCursor);
    }
    else{
        resizeDir = nodir;
        tempCursor.setShape(Qt::ArrowCursor);
    }

    setCursor(tempCursor);                    //重新设置鼠标,主要是改样式
}
 
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	event->ignore();
	if (window_flag)
	{
		if (event->buttons() & Qt::LeftButton){                 //如果左键是按下的
		  if(resizeDir == nodir)
		  {                             //如果鼠标不是放在边缘那么说明这是在拖动窗口
			 move(event->globalPos() - m_position);
		  }
		  else{
			  int ptop,pbottom,pleft,pright;                   //窗口上下左右的值
			  ptop = frameGeometry().top();
			  pbottom = frameGeometry().bottom();
			  pleft = frameGeometry().left();
			  pright = frameGeometry().right();
			  if(resizeDir & top){                               //检测更改尺寸方向中包含的上下左右分量
				  if(height() == minimumHeight()){
					  ptop = min(event->globalY(),ptop);
				  }
				  else if(height() == maximumHeight()){
					  ptop = max(event->globalY(),ptop);
				  }
				  else{
					  ptop = event->globalY();
				  }
			  }
			  else if(resizeDir & bottom){
				  if(height() == minimumHeight()){
					  pbottom = max(event->globalY(),pbottom);
				  }
				  else if(height() == maximumHeight()){
					  pbottom = min(event->globalY(),pbottom);
				  }
				  else{
					  pbottom = event->globalY();
				  }
			  }

			  if(resizeDir & left){                        //检测左右分量
				  if(width() == minimumWidth()){
					  pleft = min(event->globalX(),pleft);
				  }
				  else if(width() == maximumWidth()){
					  pleft = max(event->globalX(),pleft);
				  }
				  else{
					  pleft = event->globalX();
				  }
			  }
			  else if(resizeDir & right){
				  if(width() == minimumWidth()){
					  pright = max(event->globalX(),pright);
				  }
				  else if(width() == maximumWidth()){
					  pright = min(event->globalX(),pright);
				  }
				  else{
					  pright = event->globalX();
				  }
			  }
			  
			  setGeometry(QRect(QPoint(pleft,ptop),QPoint(pright,pbottom)));
		  }
	  }
	  else 
	  {
		  edgeMargin =10;
		  testEdge();   //当不拖动窗口、不改变窗口大小尺寸的时候  检测鼠标边缘
	  }
	}
}




void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
	if(window_flag)
	{
		if(event->button() == Qt::LeftButton)
		{
			m_position = event->globalPos() - frameGeometry().topLeft(); //记录鼠标左键位置
		}

	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
	if(window_flag)
	{
		testEdge();
	}
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
