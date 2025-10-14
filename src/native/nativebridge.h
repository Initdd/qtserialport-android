#ifndef NATIVEBRIDGE_H
#define NATIVEBRIDGE_H

#include <jni.h>
#include <QtCore/QJniObject>
#include <QtCore/QJniEnvironment>

class NativeBridge {
public:
    static bool openSerial(int baudRate);
    static int writeSerial(const QByteArray &data);
    static QByteArray readSerial(int maxLen = 1024);
    static bool closeSerial();
};

#endif // NATIVEBRIDGE_H
