package org.example

import android.content.Context
import android.hardware.usb.UsbManager
import com.hoho.android.usbserial.driver.UsbSerialPort
import com.hoho.android.usbserial.driver.UsbSerialProber

class SerialHelper {
    companion object {
        private var port: UsbSerialPort? = null

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
            port?.setParameters(baudRate, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE)
            return true
        }

        @JvmStatic
        fun writeSerial(data: ByteArray): Int {
            return port?.write(data, 1000) ?: -1
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
    }
}