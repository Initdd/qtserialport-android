package org.qtproject.example

import android.content.Context
import android.hardware.usb.UsbManager
import com.hoho.android.usbserial.driver.UsbSerialPort
import com.hoho.android.usbserial.driver.UsbSerialProber

class SerialHelper {
    companion object {
        private var port: UsbSerialPort? = null
        private var currentBaudRate: Int = 9600
        private var currentDataBits: Int = 8
        private var currentStopBits: Int = UsbSerialPort.STOPBITS_1
        private var currentParity: Int = UsbSerialPort.PARITY_NONE

        @JvmStatic
        fun openSerial(baudRate: Int, context: Context): Boolean {
            val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
            val drivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager)
            if (drivers.isEmpty()) return false

            val driver = drivers[0]
            val connection = usbManager.openDevice(driver.device) ?: return false

            // Expecting just one port in the USB connection (for simplicity)
            // In other words, I'm ignoring the usb hubs and multiple ports
            port = driver.ports[0]
            port?.open(connection)
            currentBaudRate = baudRate
            port?.setParameters(currentBaudRate, currentDataBits, currentStopBits, currentParity)
            return true
        }

        @JvmStatic
        fun writeSerial(data: ByteArray): Int {
            return try {
                port?.write(data, 1000)
                data.size
            } catch (e: Exception) {
                -1
            }
        }

        @JvmStatic
        fun readSerial(maxLen: Int = 1024): ByteArray? {
            val buffer = ByteArray(maxLen)
            val len = port?.read(buffer, 1000) ?: -1
            return if (len > 0) buffer.copyOf(len) else null
        }

        @JvmStatic
        fun closeSerial(): Boolean {
            port?.close()
            port = null
            return true
        }

        @JvmStatic
        fun availablePorts(context: Context): List<String> {
            val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
            val drivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager)
            return drivers.map { it.device.deviceName }
        }
    }
}
