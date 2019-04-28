#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include <QThread>


class RecvThread : public QThread
{
    Q_OBJECT
public:
    RecvThread(char *ip, int port);

protected:
    void run();

signals:
    void sigNoRecv();

private:
    char *ip;
    int port;
};

#endif // RECVTHREAD_H
