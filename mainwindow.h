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

    void init(char *ip, int port, int window_flag);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void slotGetFrame(QImage img);
    void slotCloseWindow();
private:
    Ui::MainWindow *ui;
    QImage mImage;
    int m_width;
    int m_height;
};

#endif // MAINWINDOW_H
