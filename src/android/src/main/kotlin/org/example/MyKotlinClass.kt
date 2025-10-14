package org.example

object MyKotlinClass {
    // Expose a static JVM method so JNI can easily call it
    @JvmStatic
    fun printFromNative(msg: String?) {
        // For demo: log it using Android Log
        android.util.Log.i("MyKotlinClass", "Called from native: $msg")

        // You could also access other Android APIs here if needed
    }
}
