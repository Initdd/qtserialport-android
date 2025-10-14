#include "nativebridge.h"
#include <QtCore/QJniObject>
#include <QtCore/QJniEnvironment>
#include <QDebug>

bool NativeBridge::openSerial(int baudRate) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/example/SerialHelper",
        "openSerial",
        "(I)Landroid/content/Context;Z",
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