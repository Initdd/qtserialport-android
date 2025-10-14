package org.example

// This file is optional — demonstrates a Kotlin helper which can be used
// from the Activity side if you want. The C++ code above calls the
// MyKotlinClass directly via JNI, so this file isn't strictly required.

class QtKotlinBridge {
    companion object {
        @JvmStatic
        fun pingFromQt(msg: String) {
            android.util.Log.i("QtKotlinBridge", "pingFromQt: $msg")
        }
    }
}
