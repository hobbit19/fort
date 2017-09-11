#ifndef DRIVERMANAGER_H
#define DRIVERMANAGER_H

#include <QObject>

class QThread;

class Device;
class DriverWorker;
class FirewallConf;
class LogBuffer;

class DriverManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit DriverManager(QObject *parent = nullptr);
    virtual ~DriverManager();

    QString errorMessage() const { return m_errorMessage; }

    bool isDeviceOpened() const;

signals:
    void errorMessageChanged();

    void readLogResult(bool success, const QString &errorMessage);

public slots:
    bool openDevice();

    void enableDeviceIo();
    bool cancelDeviceIo();

    bool writeConf(const FirewallConf &conf);
    bool writeConfFlags(const FirewallConf &conf);

    void readLogAsync(LogBuffer *logBuffer);

private:
    void setErrorMessage(const QString &errorMessage);

    void setupWorker();

    bool writeData(int code, QByteArray &buf, int size);

private:
    Device *m_device;
    DriverWorker *m_driverWorker;
    QThread *m_workerThread;

    QString m_errorMessage;
};

#endif // DRIVERMANAGER_H
