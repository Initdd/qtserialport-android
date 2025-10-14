#include "qserialport_p.h"
#include "qserialport.h"
#include "nativebridge.h"

bool QSerialPortPrivate::open(QIODevice::OpenMode mode)
{
    Q_UNUSED(mode); // TODO: Handle different modes if needed
    return NativeBridge::openSerial(inputBaudRate);
}

qint64 QSerialPortPrivate::writeData(const char *data, qint64 maxSize)
{
    QByteArray byteArray(data, maxSize);
    return NativeBridge::writeSerial(byteArray);
}

void QSerialPortPrivate::close()
{
    NativeBridge::closeSerial();
}

bool QSerialPortPrivate::setBaudRate(qint32 baudRate, QSerialPort::Directions directions)
{
    Q_UNUSED(directions); // Android implementation applies to both directions
    
    if (!NativeBridge::setBaudRate(baudRate)) {
        return false;
    }
    
    // Update the stored baud rate values
    inputBaudRate = baudRate;
    outputBaudRate = baudRate;
    
    return true;
}

bool QSerialPortPrivate::setDataBits(QSerialPort::DataBits dataBitsValue)
{
    int dataBits = static_cast<int>(dataBitsValue);
    
    if (!NativeBridge::setDataBits(dataBits)) {
        return false;
    }
    
    // Update the stored value
    dataBits = dataBitsValue;
    
    return true;
}

bool QSerialPortPrivate::setParity(QSerialPort::Parity parityValue)
{
    int parityInt = static_cast<int>(parityValue);
    
    if (!NativeBridge::setParity(parityInt)) {
        return false;
    }
    
    // Update the stored value
    parity = parityValue;
    
    return true;
}

bool QSerialPortPrivate::setStopBits(QSerialPort::StopBits stopBitsValue)
{
    int stopBitsInt = static_cast<int>(stopBitsValue);
    
    if (!NativeBridge::setStopBits(stopBitsInt)) {
        return false;
    }
    
    // Update the stored value
    stopBits = stopBitsValue;
    
    return true;
}

bool QSerialPortPrivate::setFlowControl(QSerialPort::FlowControl flowControlValue)
{
    int flowControlInt = static_cast<int>(flowControlValue);
    
    if (!NativeBridge::setFlowControl(flowControlInt)) {
        return false;
    }
    
    // Update the stored value
    flowControl = flowControlValue;
    
    return true;
}

bool QSerialPortPrivate::setDataTerminalReady(bool set)
{
    return NativeBridge::setDataTerminalReady(set);
}

bool QSerialPortPrivate::setRequestToSend(bool set)
{
    return NativeBridge::setRequestToSend(set);
}

QSerialPort::PinoutSignals QSerialPortPrivate::pinoutSignals()
{
    int pinSignals = NativeBridge::getPinoutSignals();
    return static_cast<QSerialPort::PinoutSignals>(pinSignals);
}

bool QSerialPortPrivate::flush()
{
    return NativeBridge::flush();
}

bool QSerialPortPrivate::clear(QSerialPort::Directions directions)
{
    int directionsInt = static_cast<int>(directions);
    return NativeBridge::clear(directionsInt);
}

bool QSerialPortPrivate::startAsyncRead()
{
    // Async read would require setting up a background thread or callback
    // For now, return false to indicate it's not supported
    // Applications can use synchronous reads or implement their own threading
    return false;
}

bool QSerialPortPrivate::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    
    // Check if there are bytes available
    qint64 available = NativeBridge::bytesAvailable();
    if (available > 0) {
        return true;
    }
    
    // For simplicity, we'll do a blocking read attempt
    // In a real implementation, this should respect the timeout
    QByteArray data = NativeBridge::readSerial(1024);
    return !data.isEmpty();
}

bool QSerialPortPrivate::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);
    
    // USB serial writes are typically synchronous
    // So if write() succeeded, the bytes are written
    return true;
}

bool QSerialPortPrivate::setBreakEnabled(bool set)
{
    if (!NativeBridge::setBreakEnabled(set)) {
        return false;
    }
    
    // Update the stored value
    isBreakEnabled = set;
    
    return true;
}

QList<qint32> QSerialPortPrivate::standardBaudRates()
{
    return {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
}
