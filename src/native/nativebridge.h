#ifndef NATIVEBRIDGE_H
#define NATIVEBRIDGE_H

#include <jni.h>
#include <QtCore/QJniObject>
#include <QtCore/QJniEnvironment>

class NativeBridge {
public:
    static bool openSerial(int baudRate);
    static int writeSerial(const QByteArray &data);
    static QByteArray readSerial(int maxLen);
    static bool closeSerial();
    static bool setBaudRate(int baudRate);
    static bool setDataBits(int dataBits);
    static bool setParity(int parity);
    static bool setStopBits(int stopBits);
    static bool setFlowControl(int flowControl);
    static bool flush();
    static bool clear(int directions);
    static bool setDataTerminalReady(bool set);
    static bool setRequestToSend(bool set);
    static int getPinoutSignals();
    static qint64 bytesAvailable();
    static bool setBreakEnabled(bool set);
};

#endif // NATIVEBRIDGE_H
