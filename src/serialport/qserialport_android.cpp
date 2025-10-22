#include "qserialport_p.h"
#include "qserialport.h"
#include "nativebridge.h"
#include <QtCore/QDebug>
#include <QtCore/QTimer>

bool QSerialPortPrivate::open(QIODevice::OpenMode mode)
{
    qDebug() << "QSerialPortPrivate::open called with mode:" << mode;
    Q_UNUSED(mode); // TODO: Handle different modes if needed
    bool result = NativeBridge::openSerial(systemLocation);
    qDebug() << "QSerialPortPrivate::open result:" << result;
    
    // Start polling for incoming data if opened successfully
    if (result) {
        startAsyncRead();
    }
    
    return result;
}

qint64 QSerialPortPrivate::writeData(const char *data, qint64 maxSize)
{
    qDebug() << "QSerialPortPrivate::writeData called with maxSize:" << maxSize;
    QByteArray byteArray(data, maxSize);
    qint64 result = NativeBridge::writeSerial(byteArray);
    qDebug() << "QSerialPortPrivate::writeData result:" << result;
    
    // Emit bytesWritten signal for synchronous writes
    // Qt's QIODevice may not automatically emit this for all platforms
    if (result > 0) {
        Q_Q(QSerialPort);
        QMetaObject::invokeMethod(q, [q, result]() {
            emit q->bytesWritten(result);
        }, Qt::QueuedConnection);
    }
    
    return result;
}

void QSerialPortPrivate::close()
{
    qDebug() << "QSerialPortPrivate::close called";
    
    // Stop the read timer if it exists
    if (readTimer && readTimer->isActive()) {
        readTimer->stop();
        qDebug() << "QSerialPortPrivate::close - stopped read timer";
    }
    
    NativeBridge::closeSerial();
    qDebug() << "QSerialPortPrivate::close finished";
}

bool QSerialPortPrivate::setBaudRate(qint32 baudRate, QSerialPort::Directions directions)
{
    qDebug() << "QSerialPortPrivate::setBaudRate called with baudRate:" << baudRate << ", directions:" << directions;
    Q_UNUSED(directions); // Android implementation applies to both directions
    bool result = NativeBridge::setBaudRate(baudRate);
    if (!result) {
        qDebug() << "QSerialPortPrivate::setBaudRate failed";
        return false;
    }
    // Update the stored baud rate values
    inputBaudRate = baudRate;
    outputBaudRate = baudRate;
    qDebug() << "QSerialPortPrivate::setBaudRate success";
    return true;
}

bool QSerialPortPrivate::setDataBits(QSerialPort::DataBits dataBitsValue)
{
    qDebug() << "QSerialPortPrivate::setDataBits called with dataBitsValue:" << dataBitsValue;
    int dataBits = static_cast<int>(dataBitsValue);
    bool result = NativeBridge::setDataBits(dataBits);
    if (!result) {
        qDebug() << "QSerialPortPrivate::setDataBits failed";
        return false;
    }
    // Update the stored value
    dataBits = dataBitsValue;
    qDebug() << "QSerialPortPrivate::setDataBits success";
    return true;
}

bool QSerialPortPrivate::setParity(QSerialPort::Parity parityValue)
{
    qDebug() << "QSerialPortPrivate::setParity called with parityValue:" << parityValue;
    int parityInt = static_cast<int>(parityValue);
    bool result = NativeBridge::setParity(parityInt);
    if (!result) {
        qDebug() << "QSerialPortPrivate::setParity failed";
        return false;
    }
    // Update the stored value
    parity = parityValue;
    qDebug() << "QSerialPortPrivate::setParity success";
    return true;
}

bool QSerialPortPrivate::setStopBits(QSerialPort::StopBits stopBitsValue)
{
    qDebug() << "QSerialPortPrivate::setStopBits called with stopBitsValue:" << stopBitsValue;
    int stopBitsInt = static_cast<int>(stopBitsValue);
    bool result = NativeBridge::setStopBits(stopBitsInt);
    if (!result) {
        qDebug() << "QSerialPortPrivate::setStopBits failed";
        return false;
    }
    // Update the stored value
    stopBits = stopBitsValue;
    qDebug() << "QSerialPortPrivate::setStopBits success";
    return true;
}

bool QSerialPortPrivate::setFlowControl(QSerialPort::FlowControl flowControlValue)
{
    qDebug() << "QSerialPortPrivate::setFlowControl called with flowControlValue:" << flowControlValue;
    int flowControlInt = static_cast<int>(flowControlValue);
    bool result = NativeBridge::setFlowControl(flowControlInt);
    if (!result) {
        qDebug() << "QSerialPortPrivate::setFlowControl failed";
        return false;
    }
    // Update the stored value
    flowControl = flowControlValue;
    qDebug() << "QSerialPortPrivate::setFlowControl success";
    return true;
}

bool QSerialPortPrivate::setDataTerminalReady(bool set)
{
    qDebug() << "QSerialPortPrivate::setDataTerminalReady called with set:" << set;
    bool result = NativeBridge::setDataTerminalReady(set);
    qDebug() << "QSerialPortPrivate::setDataTerminalReady result:" << result;
    return result;
}

bool QSerialPortPrivate::setRequestToSend(bool set)
{
    qDebug() << "QSerialPortPrivate::setRequestToSend called with set:" << set;
    bool result = NativeBridge::setRequestToSend(set);
    qDebug() << "QSerialPortPrivate::setRequestToSend result:" << result;
    return result;
}

QSerialPort::PinoutSignals QSerialPortPrivate::pinoutSignals()
{
    qDebug() << "QSerialPortPrivate::pinoutSignals called";
    int pinSignals = NativeBridge::getPinoutSignals();
    qDebug() << "QSerialPortPrivate::pinoutSignals result:" << pinSignals;
    return static_cast<QSerialPort::PinoutSignals>(pinSignals);
}

bool QSerialPortPrivate::flush()
{
    qDebug() << "QSerialPortPrivate::flush called";
    bool result = NativeBridge::flush();
    qDebug() << "QSerialPortPrivate::flush result:" << result;
    return result;
}

bool QSerialPortPrivate::clear(QSerialPort::Directions directions)
{
    qDebug() << "QSerialPortPrivate::clear called with directions:" << directions;
    int directionsInt = static_cast<int>(directions);
    bool result = NativeBridge::clear(directionsInt);
    qDebug() << "QSerialPortPrivate::clear result:" << result;
    return result;
}



bool QSerialPortPrivate::startAsyncRead()
{
    qDebug() << "QSerialPortPrivate::startAsyncRead called";
    
    Q_Q(QSerialPort);
    
    // Use a timer to poll for data periodically
    // This is a simple approach for Android where we don't have file descriptor notifications
    if (!readTimer) {
        readTimer = new QTimer(q);
        QObject::connect(readTimer, &QTimer::timeout, q, [this]() {
            pollForIncomingData();
        });
    }
    
    if (!readTimer->isActive()) {
        readTimer->start(50); // Poll every 50ms
        qDebug() << "QSerialPortPrivate::startAsyncRead - timer started";
    }
    
    return true;
}

void QSerialPortPrivate::pollForIncomingData()
{
    Q_Q(QSerialPort);
    
    // Check if there's data available
    qint64 bytesAvailable = NativeBridge::bytesAvailable();
    if (bytesAvailable <= 0) {
        return;
    }
    
    // Calculate how much we can read
    qint64 bytesToRead = bytesAvailable;
    if (readBufferMaxSize && bytesToRead > (readBufferMaxSize - buffer.size())) {
        bytesToRead = readBufferMaxSize - buffer.size();
        if (bytesToRead <= 0) {
            // Buffer is full, can't read more
            return;
        }
    }
    
    // Read data from native bridge
    QByteArray data = NativeBridge::readSerial(bytesToRead);
    if (data.isEmpty()) {
        return;
    }
    
    // Append to Qt's internal buffer
    buffer.append(data);
    
    // Emit readyRead signal
    emit q->readyRead();
    
    qDebug() << "QSerialPortPrivate::pollForIncomingData - read" << data.size() << "bytes";
}

qint64 QSerialPortPrivate::readData(char *data, qint64 maxSize)
{
    qDebug() << "QSerialPortPrivate::readData called with maxSize:" << maxSize;
    
    // This shouldn't be called directly in Qt's architecture,
    // but we implement it for completeness
    // Qt's QIODevice will read from the buffer that we populated in pollForIncomingData()
    
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    
    // Return 0 to indicate we use buffered reading
    return 0;
}

bool QSerialPortPrivate::waitForReadyRead(int msecs)
{
    qDebug() << "QSerialPortPrivate::waitForReadyRead called with msecs:" << msecs;
    Q_UNUSED(msecs);
    // Check if there are bytes available
    qint64 available = NativeBridge::bytesAvailable();
    qDebug() << "QSerialPortPrivate::waitForReadyRead bytesAvailable:" << available;
    if (available > 0) {
        qDebug() << "QSerialPortPrivate::waitForReadyRead ready";
        return true;
    }
    // For simplicity, we'll do a blocking read attempt
    // In a real implementation, this should respect the timeout
    QByteArray data = NativeBridge::readSerial(1024);
    qDebug() << "QSerialPortPrivate::waitForReadyRead read data size:" << data.size();
    return !data.isEmpty();
}

bool QSerialPortPrivate::waitForBytesWritten(int msecs)
{
    qDebug() << "QSerialPortPrivate::waitForBytesWritten called with msecs:" << msecs;
    Q_UNUSED(msecs);
    // USB serial writes are typically synchronous
    // So if write() succeeded, the bytes are written
    qDebug() << "QSerialPortPrivate::waitForBytesWritten always true";
    return true;
}

bool QSerialPortPrivate::setBreakEnabled(bool set)
{
    qDebug() << "QSerialPortPrivate::setBreakEnabled called with set:" << set;
    bool result = NativeBridge::setBreakEnabled(set);
    if (!result) {
        qDebug() << "QSerialPortPrivate::setBreakEnabled failed";
        return false;
    }
    // Update the stored value
    isBreakEnabled = set;
    qDebug() << "QSerialPortPrivate::setBreakEnabled success";
    
    return true;
}

QList<qint32> QSerialPortPrivate::standardBaudRates()
{
    return {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
}
