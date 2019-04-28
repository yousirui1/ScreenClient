#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <QThread>
#include <QImage>

class DecodeThread : public QThread
{
    Q_OBJECT
public:
    DecodeThread();

signals:
    void sigGetFrame(QImage img);

protected:
    void run();
};

#endif // DECODETHREAD_H
