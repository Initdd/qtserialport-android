#ifndef QTWRAPPER_H
#define QTWRAPPER_H

#include <QObject>

class QtWrapper : public QObject {
    Q_OBJECT
public:
    explicit QtWrapper(QObject *parent = nullptr);

    Q_INVOKABLE bool openSerial(int baudRate);
    Q_INVOKABLE int writeSerial(const QByteArray &data);
    Q_INVOKABLE QByteArray readSerial(int maxLen = 1024);
    Q_INVOKABLE bool closeSerial();
};

#endif // QTWRAPPER_H