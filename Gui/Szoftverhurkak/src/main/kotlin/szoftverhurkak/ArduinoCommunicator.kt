package szoftverhurkak

import gnu.io.*

import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.io.PrintWriter

abstract class ArduinoCommunicator(comPort: String, baudRate: Int = 9600) {

    var outStream: OutputStream

    init {
        val portId = CommPortIdentifier.getPortIdentifier(comPort)
        val serialPort = portId.open("Demo application", 5000) as SerialPort

        try {
            // Set serial port to 57600bps-8N1..my favourite
            serialPort.setSerialPortParams(
                    baudRate,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE)
        } catch (ex: UnsupportedCommOperationException) {
            System.err.println(ex.message)
        }


        outStream = serialPort.outputStream
        val inStream = serialPort.inputStream

        val a = Thread(ReaderThread(inStream))
        a.start()
    }

    fun sendString(command: String) {
        val pw = PrintWriter(outStream)
        pw.write(command)
        pw.flush()
    }

    abstract fun onMessageReceived(command: String)

    private inner class ReaderThread(private val `is`: InputStream) : Runnable {

        override fun run() {
            var stringBuilder = StringBuilder()
            while (true) {
                try {
                    var i = `is`.read()
                    while (i != -1) {
                        stringBuilder.append(i.toChar())
                        i = `is`.read()
                    }
                    if (stringBuilder.isNotEmpty()) {
                        onMessageReceived(stringBuilder.toString())
                        stringBuilder.delete(0, stringBuilder.length)
                    }
                } catch (e: IOException) {
                    e.printStackTrace()
                }

            }
        }
    }
}
