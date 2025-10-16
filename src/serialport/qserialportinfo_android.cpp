// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qserialportinfo.h"
#include "qserialportinfo_p.h"
#include "qserialport_p.h"
#include "nativebridge.h"
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

QList<QSerialPortInfo> QSerialPortInfo::availablePorts()
{
    QList<QSerialPortInfo> serialPortInfoList;
    
    qDebug() << "[QtSerialPort] Requesting available ports from NativeBridge...";
    
    // Get available ports from the native bridge
    QStringList portNames = NativeBridge::getAvailablePorts();
    
    qDebug() << "[QtSerialPort] Found" << portNames.size() << "port(s):" << portNames;
    
    for (const QString &portName : portNames) {
        QSerialPortInfoPrivate priv;
        priv.portName = portName;
        priv.device = portName; // Use the port name directly as the device path
        priv.description = QStringLiteral("USB Serial Port");
        priv.manufacturer = QString(); // Would need additional USB device info from Android
        priv.serialNumber = QString();
        priv.vendorIdentifier = 0;
        priv.productIdentifier = 0;
        priv.hasVendorIdentifier = false;
        priv.hasProductIdentifier = false;

        qDebug() << "[QtSerialPort] Port:" << priv.portName << "Device:" << priv.device;

        serialPortInfoList.append(QSerialPortInfo(priv));
    }
    
    qDebug() << "[QtSerialPort] Returning" << serialPortInfoList.size() << "port info object(s)";

    return serialPortInfoList;
}

QString QSerialPortInfoPrivate::portNameToSystemLocation(const QString &source)
{
    // On Android, the port name is the system location
    return source;
}

QString QSerialPortInfoPrivate::portNameFromSystemLocation(const QString &source)
{
    // On Android, the port name is the system location
    return source;
}

QT_END_NAMESPACE
