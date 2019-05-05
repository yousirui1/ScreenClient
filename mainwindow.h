#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *e);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

    void init(char *ip, int port, int window_flag);
	void testEdge();

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void slotGetFrame(QImage img);
    void slotCloseWindow();
private:
    Ui::MainWindow *ui;
    QImage mImage;
	int window_flag;
    int m_width;
    int m_height;

	int edgeMargin;
	QPoint m_position;
	enum {
		nodir,
		top = 0x01,
		bottom = 0x02,
		left = 0x04,	
		right = 0x08,
		topLeft = 0x01 | 0x04,
		topRight = 0x01 | 0x08,
		bottomLeft = 0x02 | 0x04,
		bottomRight = 0x02 | 0x08
	}resizeDir;
};

#endif // MAINWINDOW_H
