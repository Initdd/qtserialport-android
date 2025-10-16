#include "nativebridge.h"
#include <QtCore/QJniObject>
#include <QtCore/QJniEnvironment>
#include <QDebug>

#define CLASS_NAME "org/qtproject/example"

bool NativeBridge::openSerial(int baudRate) {
    // Open serial connection
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
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
        CLASS_NAME "/SerialHelper",
        "writeSerial",
        "([B)I",
        jData
    );
    env->DeleteLocalRef(jData);
    return result;
}

QByteArray NativeBridge::readSerial(int maxLen) {
    QJniObject jResult = QJniObject::callStaticObjectMethod(
        CLASS_NAME "/SerialHelper",
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
        CLASS_NAME "/SerialHelper",
        "closeSerial",
        "()Z"
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setBaudRate(int baudRate) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "setBaudRate",
        "(I)Z",
        static_cast<jint>(baudRate)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setDataBits(int dataBits) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "setDataBits",
        "(I)Z",
        static_cast<jint>(dataBits)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setParity(int parity) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "setParity",
        "(I)Z",
        static_cast<jint>(parity)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setStopBits(int stopBits) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "setStopBits",
        "(I)Z",
        static_cast<jint>(stopBits)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setFlowControl(int flowControl) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "setFlowControl",
        "(I)Z",
        static_cast<jint>(flowControl)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::flush() {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "flush",
        "()Z"
    );
    return result == JNI_TRUE;
}

bool NativeBridge::clear(int directions) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "clear",
        "(I)Z",
        static_cast<jint>(directions)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setDataTerminalReady(bool set) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "setDataTerminalReady",
        "(Z)Z",
        static_cast<jboolean>(set)
    );
    return result == JNI_TRUE;
}

bool NativeBridge::setRequestToSend(bool set) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "setRequestToSend",
        "(Z)Z",
        static_cast<jboolean>(set)
    );
    return result == JNI_TRUE;
}

int NativeBridge::getPinoutSignals() {
    jint result = QJniObject::callStaticMethod<jint>(
        CLASS_NAME "/SerialHelper",
        "getPinoutSignals",
        "()I"
    );
    return result;
}

qint64 NativeBridge::bytesAvailable() {
    jlong result = QJniObject::callStaticMethod<jlong>(
        CLASS_NAME "/SerialHelper",
        "bytesAvailable",
        "()J"
    );
    return static_cast<qint64>(result);
}

bool NativeBridge::setBreakEnabled(bool set) {
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        CLASS_NAME "/SerialHelper",
        "setBreakEnabled",
        "(Z)Z",
        static_cast<jboolean>(set)
    );
    return result == JNI_TRUE;
}

QStringList NativeBridge::getAvailablePorts() {
    QStringList portList;
    
    qDebug() << "[NativeBridge] getAvailablePorts() called";
    
    // Get the Android context
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative",
        "activity",
        "()Landroid/app/Activity;"
    );

    if (!activity.isValid()) {
        qWarning() << "[NativeBridge] Failed to get Android activity";
        return portList;
    }
    
    qDebug() << "[NativeBridge] Got Android activity, calling SerialHelper.availablePorts()";

    // Call the Kotlin method to get available ports
    QJniObject portListObj = QJniObject::callStaticObjectMethod(
        CLASS_NAME "/SerialHelper",
        "availablePorts",
        "(Landroid/content/Context;)Ljava/util/List;",
        activity.object()
    );

    if (!portListObj.isValid()) {
        qWarning() << "[NativeBridge] SerialHelper.availablePorts() returned invalid object";
        return portList;
    }
    
    qDebug() << "[NativeBridge] Got port list object, converting to QStringList";

    // Convert Java List to QStringList
    QJniEnvironment env;
    jobject jList = portListObj.object();
    
    jclass listClass = env->FindClass("java/util/List");
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    
    jint size = env->CallIntMethod(jList, sizeMethod);
    
    qDebug() << "[NativeBridge] Java list has" << size << "item(s)";
    
    for (jint i = 0; i < size; ++i) {
        QJniObject portNameObj = env->CallObjectMethod(jList, getMethod, i);
        QString portName = portNameObj.toString();
        qDebug() << "[NativeBridge] Port" << i << ":" << portName;
        if (!portName.isEmpty()) {
            portList.append(portName);
        }
    }
    
    env->DeleteLocalRef(listClass);
    
    qDebug() << "[NativeBridge] Returning" << portList.size() << "port(s):" << portList;
    
    return portList;
}
