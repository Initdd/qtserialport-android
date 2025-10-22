package org.qtproject.example

import android.app.Application
import android.util.Log
import java.io.File

class LogcatApp : Application() {
    override fun onCreate() {
        super.onCreate()

        // Start logcat redirect to app's filesDir/logcat.txt with rotation
        try {
            val outFile = File(filesDir, "logcat.txt")
            // -f <file> -r <kbytes> -n <count>
            val cmd = arrayOf("logcat", "-f", outFile.absolutePath, "-r", "1024", "-n", "1")
            // Use Runtime.exec with the array form to avoid shell interpretation
            Runtime.getRuntime().exec(cmd)
            Log.i("LogcatApp", "Started logcat to ${outFile.absolutePath}")
        } catch (e: Exception) {
            Log.e("LogcatApp", "Failed to start logcat", e)
        }
    }
}
