package org.qtproject.example

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.os.Build
import android.util.Log
import com.hoho.android.usbserial.driver.UsbSerialDriver
import com.hoho.android.usbserial.driver.UsbSerialPort
import com.hoho.android.usbserial.driver.UsbSerialProber
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit

class SerialHelper {
    companion object {
        private const val ACTION_USB_PERMISSION = "org.qtproject.example.USB_PERMISSION"
        
        @Volatile private var port: UsbSerialPort? = null
        @Volatile private var currentBaudRate: Int = 9600
        @Volatile private var currentDataBits: Int = 8
        @Volatile private var currentStopBits: Int = UsbSerialPort.STOPBITS_1
        @Volatile private var currentParity: Int = UsbSerialPort.PARITY_NONE
        
        // Buffer for peeked data (since USB serial doesn't support non-destructive read)
        private val readBuffer = mutableListOf<Byte>()

        /**
         * Requests USB permission for the given device and waits for the result.
         * Returns true if permission was granted, false otherwise.
         */
        private fun requestPermission(context: Context, usbManager: UsbManager, device: UsbDevice): Boolean {
            // Check if we already have permission
            if (usbManager.hasPermission(device)) {
                return true
            }

            Log.i("SerialHelper", "Requesting USB permission for device: ${device.deviceName}")
            
            // Create a latch to wait for the permission result
            val latch = CountDownLatch(1)
            var permissionGranted = false

            // Create a BroadcastReceiver to handle the permission response
            val usbReceiver = object : BroadcastReceiver() {
                override fun onReceive(context: Context, intent: Intent) {
                    if (ACTION_USB_PERMISSION == intent.action) {
                        synchronized(this) {
                            val receivedDevice: UsbDevice? = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                                intent.getParcelableExtra(UsbManager.EXTRA_DEVICE, UsbDevice::class.java)
                            } else {
                                @Suppress("DEPRECATION")
                                intent.getParcelableExtra(UsbManager.EXTRA_DEVICE)
                            }

                            if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                                receivedDevice?.apply {
                                    Log.i("SerialHelper", "USB permission granted for: $deviceName")
                                    permissionGranted = true
                                }
                            } else {
                                Log.e("SerialHelper", "USB permission denied for: ${receivedDevice?.deviceName}")
                                permissionGranted = false
                            }
                            latch.countDown()
                        }
                    }
                }
            }

            // Register the receiver
            val filter = IntentFilter(ACTION_USB_PERMISSION)
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                context.registerReceiver(usbReceiver, filter, Context.RECEIVER_NOT_EXPORTED)
            } else {
                context.registerReceiver(usbReceiver, filter)
            }

            try {
                // Request permission
                val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                    PendingIntent.FLAG_MUTABLE
                } else {
                    0
                }
                val permissionIntent = PendingIntent.getBroadcast(context, 0, Intent(ACTION_USB_PERMISSION), flags)
                usbManager.requestPermission(device, permissionIntent)

                // Wait for the permission result (timeout after 30 seconds)
                if (!latch.await(30, TimeUnit.SECONDS)) {
                    Log.e("SerialHelper", "USB permission request timed out")
                    return false
                }
            } finally {
                // Unregister the receiver
                try {
                    context.unregisterReceiver(usbReceiver)
                } catch (e: Exception) {
                    Log.w("SerialHelper", "Failed to unregister receiver", e)
                }
            }

            return permissionGranted
        }

        /**
         * Opens the serial port with the given parameters. Returns true on success, false
         * otherwise. Handles exceptions and permission errors gracefully.
         * 
         * @param context The Android context.
         * @param deviceName The name of the USB device to open (optional). If null, opens the first available device.
         */
        @JvmStatic
        @Synchronized
        fun openSerial(context: Context, deviceName: String? = null): Boolean {
            return try {
                val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
                val drivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager)
                if (drivers.isEmpty()) {
                    Log.e("SerialHelper", "No USB serial devices found")
                    return false
                }

                // Find the driver for the specified device, or use the first one if not specified
                val driver = if (deviceName != null) {
                    drivers.find { it.device.deviceName == deviceName }
                } else {
                    drivers[0]
                }
                
                // Check if the driver was found
                if (driver == null) {
                    Log.e("SerialHelper", "Device not found: $deviceName")
                    return false
                }

                // Request permission if not already granted
                if (!requestPermission(context, usbManager, driver.device)) {
                    Log.e("SerialHelper", "Failed to get USB permission for device: ${driver.device.deviceName}")
                    return false
                }

                // Open a connection to the USB device
                val connection = usbManager.openDevice(driver.device)
                if (connection == null) {
                    Log.e(
                        "SerialHelper",
                        "No permission to open USB device: ${driver.device.deviceName}"
                    )
                    return false
                }

                // Expecting just one port in the USB connection (for simplicity)
                port = driver.ports[0]
                port?.open(connection)
                port?.setParameters(
                    currentBaudRate,
                    currentDataBits,
                    currentStopBits,
                    currentParity
                )
                Log.i("SerialHelper", "Serial port opened: ${driver.device.deviceName}")
                true

            } catch (e: Exception) {
                Log.e("SerialHelper", "Failed to open serial port", e)
                port = null
                false
            }
        }

        /** Writes data to the serial port. Returns the number of bytes written, or -1 on error. */
        @JvmStatic
        @Synchronized
        fun writeSerial(data: ByteArray): Int {
            return try {
                if (port != null) {
                    port!!.write(data, 1000)
                    data.size
                } else {
                    -1
                }
            } catch (e: Exception) {
                Log.e("SerialHelper", "Failed to write to serial port", e)
                -1
            }
        }

        /**
         * Reads up to maxLen bytes from the serial port. Returns the bytes read, or null on error.
         */
        @JvmStatic
        @Synchronized
        fun readSerial(maxLen: Int = 1024): ByteArray? {
            return try {
                if (port == null) {
                    return null
                }
                
                // First, return any buffered data from previous bytesAvailable() check
                if (readBuffer.isNotEmpty()) {
                    val bytesToReturn = minOf(readBuffer.size, maxLen)
                    val result = ByteArray(bytesToReturn)
                    for (i in 0 until bytesToReturn) {
                        result[i] = readBuffer.removeAt(0)
                    }
                    return result
                }
                
                // If no buffered data, read from port
                val buffer = ByteArray(maxLen)
                val len = port!!.read(buffer, 1000) ?: -1
                if (len > 0) buffer.copyOf(len) else null
            } catch (e: Exception) {
                Log.e("SerialHelper", "Failed to read from serial port", e)
                null
            }
        }

        /** Returns the number of bytes available to read from the serial port, or 0 if none or on error. */
        @JvmStatic
        @Synchronized
        fun bytesAvailable(): Long {
            return try {
                if (port == null) {
                    return 0L
                }
                
                // Return buffered bytes plus check for new data
                var availableBytes = readBuffer.size.toLong()
                
                // Try non-blocking read to check for new data (0 timeout)
                val buffer = ByteArray(1024)
                val bytesRead = port!!.read(buffer, 0)
                
                if (bytesRead > 0) {
                    // Add new data to buffer
                    for (i in 0 until bytesRead) {
                        readBuffer.add(buffer[i])
                    }
                    availableBytes += bytesRead
                }
                
                availableBytes
            } catch (e: Exception) {
                // Timeout or error - return just buffered data count
                readBuffer.size.toLong()
            }
        }

        /** Closes the serial port. Returns true if closed successfully, false otherwise. */
        @JvmStatic
        @Synchronized
        fun closeSerial(): Boolean {
            return try {
                port?.close()
                port = null
                readBuffer.clear() // Clear any buffered data
                true
            } catch (e: Exception) {
                Log.e("SerialHelper", "Failed to close serial port", e)
                port = null
                readBuffer.clear()
                false
            }
        }

        /** Returns a list of available USB serial port device names. */
        @JvmStatic
        fun availablePorts(context: Context): List<String> {
            return try {
                val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
                val drivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager)
                drivers.map { it.device.deviceName }
            } catch (e: Exception) {
                Log.e("SerialHelper", "Failed to list available ports", e)
                emptyList()
            }
        }
    }
}
