package com.example.dejvid.blinkled

import android.annotation.SuppressLint
import android.content.Context
import android.os.AsyncTask
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*
import org.jetbrains.anko.doAsync
import org.jetbrains.anko.uiThread
import java.lang.Exception
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress
import android.net.NetworkInfo
import android.content.Context.CONNECTIVITY_SERVICE
import android.support.v4.content.ContextCompat.getSystemService
import android.net.ConnectivityManager



class MainActivity : AppCompatActivity() {

    val GETSTATUS = "S"
    val SETON = "D"
    val SETOFF = "N"
    val STRIP_ONE = "1"
    val STRIP_TWO = "2"
    val STRIP_THREE = "3"

    val host = "192.168.1.101"
    val port = 5005
    val bufferSize = 16

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        textConnected.text = "Nepripojené"

        sendUDP(GETSTATUS.toByteArray()) // registracia
        switch1.setOnClickListener {
            if(switch1.isChecked) {
                sendUDP(SETON.toByteArray())
                switch2.isChecked
                switch3.isChecked
                switch4.isChecked
            }else{
                sendUDP(SETOFF.toByteArray())
                !switch2.isChecked
                !switch3.isChecked
                !switch4.isChecked
            }
        }
        switch2.setOnClickListener {
            sendUDP(STRIP_ONE.toByteArray())
        }
        switch3.setOnClickListener {
            sendUDP(STRIP_TWO.toByteArray())
        }
        switch4.setOnClickListener {
            sendUDP(STRIP_THREE.toByteArray())
        }
        receiveUDP()
        onlineChecker()
    }

    fun isOnline(): Boolean {
        val cm = getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        val netInfo = cm.activeNetworkInfo
        return netInfo != null && netInfo.isConnectedOrConnecting
    }

    fun onlineChecker(){
        doAsync {
            while (!isOnline()){
                ;
            }
            sendUDP(GETSTATUS.toByteArray()) // registracia
        }

    }

    fun sendUDP(data: ByteArray){
        doAsync {
            var socket: DatagramSocket? = null
            try{
                socket = DatagramSocket(5006)
                val address = InetAddress.getByName(host)
                val packet = DatagramPacket(data, data.size, address, port)
                socket.send(packet)

            } catch (e: Exception){
                e.printStackTrace()
            } finally {
                socket?.close()
            }
        }
    }



    fun receiveUDP(){
        var status = ""
        var connected = false
        var led1 = 0
        var led2 = 0
        var led3 = 0

        doAsync {
            val ret = ByteArray(bufferSize)
            var socket: DatagramSocket? = null
            socket = DatagramSocket(port)
            try {
                while (true){
                    var packet = DatagramPacket(ret, ret.size)
                    socket.receive(packet)

                    val address = packet.address
                    val portUdp = packet.port
                    packet = DatagramPacket(ret, ret.size, address, portUdp)
                    var receivedmsg = String(packet.data, 0 , packet.length)
                    if (receivedmsg.contains("STAT")){
                        connected = true
                    }
                    uiThread {
                        if (connected) {
                            textConnected.setText("Pripojené")
                            textConnected.setBackgroundColor(resources.getColor(R.color.colorPrimary))
                            try{
                                status = receivedmsg.split(":")[1].take(5)
                                led1 = status.split(",")[0].toInt()
                                led2 = status.split(",")[1].toInt()
                                led3 = status.split(",")[2].toInt()
                            }catch (e: Exception){
                                e.printStackTrace()
                            }
                            switch2.isChecked = led1 == 1
                            switch3.isChecked = led2 == 1
                            switch4.isChecked = led3 == 1
                        }
                    }
                }

            } catch (e: Exception) {
                e.printStackTrace()
            } finally {
                socket?.close()
            }
        }
    }


}
