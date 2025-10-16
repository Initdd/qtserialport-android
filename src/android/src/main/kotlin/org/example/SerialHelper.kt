package org.example

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

        @JvmStatic
        fun setBaudRate(baudRate: Int): Boolean {
            return try {
                currentBaudRate = baudRate
                port?.setParameters(currentBaudRate, currentDataBits, currentStopBits, currentParity)
                true
            } catch (e: Exception) {
                false
            }
        }

        @JvmStatic
        fun setDataBits(dataBits: Int): Boolean {
            return try {
                // USB Serial expects constants like UsbSerialPort.DATABITS_5, etc.
                val mapped = when (dataBits) {
                    5 -> UsbSerialPort.DATABITS_5
                    6 -> UsbSerialPort.DATABITS_6
                    7 -> UsbSerialPort.DATABITS_7
                    8 -> UsbSerialPort.DATABITS_8
                    else -> return false
                }
                currentDataBits = mapped
                port?.setParameters(
                    currentBaudRate,
                    currentDataBits,
                    currentStopBits,
                    currentParity
                )
                true
            } catch (e: Exception) {
                false
            }
        }

        @JvmStatic
        fun setParity(parity: Int): Boolean {
            return try {
                // Convert Qt parity enum to USB serial values
                // Qt: NoParity=0, EvenParity=2, OddParity=3, SpaceParity=4, MarkParity=5
                currentParity = when (parity) {
                    0 -> UsbSerialPort.PARITY_NONE
                    2 -> UsbSerialPort.PARITY_EVEN
                    3 -> UsbSerialPort.PARITY_ODD
                    4 -> UsbSerialPort.PARITY_SPACE
                    5 -> UsbSerialPort.PARITY_MARK
                    else -> return false
                }
                port?.setParameters(currentBaudRate, currentDataBits, currentStopBits, currentParity)
                true
            } catch (e: Exception) {
                false
            }
        }

        @JvmStatic
        fun setStopBits(stopBits: Int): Boolean {
            return try {
                // Convert Qt stopbits enum to USB serial values
                // Qt: OneStop=1, OneAndHalfStop=3, TwoStop=2
                currentStopBits = when (stopBits) {
                    1 -> UsbSerialPort.STOPBITS_1
                    3 -> UsbSerialPort.STOPBITS_1_5
                    2 -> UsbSerialPort.STOPBITS_2
                    else -> return false
                }
                port?.setParameters(currentBaudRate, currentDataBits, currentStopBits, currentParity)
                true
            } catch (e: Exception) {
                false
            }
        }

        @JvmStatic
        fun setFlowControl(flowControl: Int): Boolean {
            // Qt: NoFlowControl=0, HardwareControl=1, SoftwareControl=2
            // USB Serial library doesn't have simple flow control API
            // This would need to be implemented if hardware supports it
            return when (flowControl) {
                0 -> true // No flow control is the default
                else -> false // Hardware/Software flow control not implemented
            }
        }

        @JvmStatic
        fun flush(): Boolean {
            // USB serial doesn't have explicit flush, writes are immediate
            return true
        }

        @JvmStatic
        fun clear(directions: Int): Boolean {
            return try {
                // Qt Directions: Input=1, Output=2, AllDirections=3
                // Purge input/output buffers
                when (directions) {
                    1 -> port?.purgeHwBuffers(true, false) // Input
                    2 -> port?.purgeHwBuffers(false, true) // Output
                    3 -> port?.purgeHwBuffers(true, true)  // Both
                    else -> return false
                }
                true
            } catch (e: Exception) {
                false
            }
        }

        @JvmStatic
        fun setDataTerminalReady(set: Boolean): Boolean {
            return try {
                port?.dtr = set
                true
            } catch (e: Exception) {
                false
            }
        }

        @JvmStatic
        fun setRequestToSend(set: Boolean): Boolean {
            return try {
                port?.rts = set
                true
            } catch (e: Exception) {
                false
            }
        }

        @JvmStatic
        fun getPinoutSignals(): Int {
            // Returns a bitmask of pinout signals
            // Qt: NoSignal=0, DataTerminalReady=4, DataCarrierDetect=8,
            //     DataSetReady=16, RingIndicator=32, RequestToSend=64,
            //     ClearToSend=128, SecondaryTransmittedData=256,
            //     SecondaryReceivedData=512
            var signals = 0
            try {
                val dtr = port?.dtr ?: false
                val rts = port?.rts ?: false
                
                if (dtr) signals = signals or 4  // DataTerminalReady
                if (rts) signals = signals or 64 // RequestToSend
                
                // CTS, DSR, etc. would need additional hardware support
            } catch (e: Exception) {
                // Ignore
            }
            return signals
        }

        @JvmStatic
        fun bytesAvailable(): Long {
            // USB serial doesn't provide bytes available directly
            // Would need to implement buffering if needed
            return 0L
        }

        @JvmStatic
        fun setBreakEnabled(set: Boolean): Boolean {
            return try {
                port?.setBreak(set)
                true
            } catch (e: Exception) {
                false
            }
        }

        @JvmStatic
        fun availablePorts(context: Context): List<String> {
            val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
            val drivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager)
            return drivers.map { it.device.deviceName }
        }
    }
}