#include "nativebridge.h"
#include <QtCore/QJniObject>
#include <QtCore/QJniEnvironment>
#include <QDebug>

bool NativeBridge::openSerial(int baudRate) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "openSerial",
        "(I)Z",
        static_cast<jint>(baudRate)
    );
    return result == JNI_TRUE;
}

int NativeBridge::writeSerial(const QByteArray &data) {
    QJniEnvironment env;
    jbyteArray jData = env->NewByteArray(data.size());
    env->SetByteArrayRegion(jData, 0, data.size(), reinterpret_cast<const jbyte*>(data.constData()));
    jint result = QJniObject::callStaticMethod<jint>(
        "org/example/SerialHelper",
        "writeSerial",
        "([B)I",
        jData
    );
    env->DeleteLocalRef(jData);
    return result;
}

QByteArray NativeBridge::readSerial(int maxLen) {
    QJniObject jResult = QJniObject::callStaticObjectMethod(
        "org/example/SerialHelper",
        "readSerial",
        "(I)[B",
        maxLen
    );
    QJniEnvironment env;
    jbyteArray array = static_cast<jbyteArray>(jResult.object<jbyteArray>());
    if (!array) return QByteArray();
    jsize len = env->GetArrayLength(array);
    QByteArray result(len, 0);
    env->GetByteArrayRegion(array, 0, len, reinterpret_cast<jbyte*>(result.data()));
    env->DeleteLocalRef(array);
    return result;
}

bool NativeBridge::closeSerial() {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "closeSerial",
        "()Z"
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setBaudRate(int baudRate) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "setBaudRate",
        "(I)Z",
        static_cast<jint>(baudRate)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setDataBits(int dataBits) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "setDataBits",
        "(I)Z",
        static_cast<jint>(dataBits)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setParity(int parity) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "setParity",
        "(I)Z",
        static_cast<jint>(parity)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setStopBits(int stopBits) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "setStopBits",
        "(I)Z",
        static_cast<jint>(stopBits)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setFlowControl(int flowControl) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "setFlowControl",
        "(I)Z",
        static_cast<jint>(flowControl)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::flush() {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "flush",
        "()Z"
    );
    return result == JNI_TRUE;
}

bool NativeBridge::clear(int directions) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "clear",
        "(I)Z",
        static_cast<jint>(directions)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setDataTerminalReady(bool set) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "setDataTerminalReady",
        "(Z)Z",
        static_cast<jboolean>(set)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setRequestToSend(bool set) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "setRequestToSend",
        "(Z)Z",
        static_cast<jboolean>(set)
    );
    return result == JNI_TRUE;
}

int NativeBridge::getPinoutSignals() {
    jint result = QJniObject::callStaticMethod<jint>(
        "org/example/SerialHelper",
        "getPinoutSignals",
        "()I"
    );
    return result;
}

qint64 NativeBridge::bytesAvailable() {
    jlong result = QJniObject::callStaticMethod<jlong>(
        "org/example/SerialHelper",
        "bytesAvailable",
        "()J"
    );
    return static_cast<qint64>(result);
}

bool NativeBridge::setBreakEnabled(bool set) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "setBreakEnabled",
        "(Z)Z",
        static_cast<jboolean>(set)
    );
    return result == JNI_TRUE;
}