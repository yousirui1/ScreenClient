#include "mainwindow.h"
#include <QApplication>
#include <unistd.h>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QFile>
#include <QDateTime>
int run_flag = 0;

#define _TIME_ qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))

void msg_out(QtMsgType type, const char *msg)
{
    QString qstrText;
    static QMutex mutex;
    switch(type)
    {
        case QtDebugMsg:
            qstrText = QString("[%1]  [Debug]").arg(_TIME_);
            break;
        case QtWarningMsg:
            qstrText = QString("[%1]  [WarningWarning]").arg(_TIME_);
            break;
        default:
            return;

    }
    QFile out("./logscreen-" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".log");
    mutex.lock();
    if(out.open(QIODevice::WriteOnly | QIODevice:: Text | QIODevice::Append))
    {
        if(out.size() > 1024 * 1024 * 64)
        {
            out.close();
            out.open(QIODevice::WriteOnly | QIODevice::Text);
        }
        QTextStream ts(&out);
        ts<<qstrText<<" ["<<QThread::currentThreadId()<<"]  ["<<QString::fromUtf8(msg)<<"]"<<endl;
        out.close();
    }
    mutex.unlock();
}



//参数依次：窗口,ip,port,chanel
int main(int argc, char *argv[])
{
    char ip[16] = "225.3.3.11";
    int port = 9999;
    int window_flag = 1;

    QApplication a(argc, argv);
    qInstallMsgHandler(msg_out);

    if(argc > 3)
    {
        if(atoi(argv[1]) == 1)
        {
            window_flag = 1;
        }
        else
        {
            window_flag = 0;
        }
        QRegExp regex("^((2[0-5]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-5]\\d|25[0-5]|[01]?\\d\\d?)$");
        if(regex.exactMatch(argv[2]))
        {
            port = atoi(argv[3]);
            if(port <= 0  && port >=65535)
                port = 9999;
            memcpy(ip, argv[2], strlen(argv[2]));
        }
        else {
           port = 9999;
        }
    }
    MainWindow w;
    w.init(ip, port, window_flag);
    w.show();
    return a.exec();
}


#if 0
int c;
char ip[16] = "224.0.1.2";
int port = 7838;
int window_flag = 1;
int channel = 0;
while((c = getopt(argc, argv, "w:i:p:c:")) != -1)
{
    switch(c)
    {
        case 'i':
            ip = strdup(optarg);
            qWarning("ip %s", ip);
            break;
        case 'p':
            if(atoi(optarg) > 2000 && atoi(optarg) < 20000)
                port = atoi(optarg);
            qWarning("port %d", port);
            break;
        case 'w':
            if(atoi(optarg) == 0 || atoi(optarg) == 1)
                window_flag = atoi(optarg);
            else
                window_flag = 1;
            qWarning("window_flag %d", window_flag);
            break;
        case 'c':
            if(atoi(optarg) >0 && atoi(optarg) < 10)
                c = atoi(optarg);
            qWarning("c %d", c);
            break;
    }
}
#endif
