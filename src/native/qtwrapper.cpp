#include "qtwrapper.h"
#include "nativebridge.h"

QtWrapper::QtWrapper(QObject *parent) : QObject(parent) {}

bool QtWrapper::openSerial(const QString &deviceName) {
    return NativeBridge::openSerial(deviceName);
}

int QtWrapper::writeSerial(const QByteArray &data) {
    return NativeBridge::writeSerial(data);
}

QByteArray QtWrapper::readSerial(int maxLen) {
    return NativeBridge::readSerial(maxLen);
}

bool QtWrapper::closeSerial() {
    return NativeBridge::closeSerial();
}