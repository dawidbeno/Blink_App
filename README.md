# Blink_App
LED strip controlled by Android app. 

## Android app
Android app contains four toggle buttons, used for turning on and off LED strips.
One button is for all three strips together. Then one button for each LED strip.

App is connected to wifi by default with IP and port: **192.168.1.101:5005**

After start app checks actual status by sending an UDP packet with status request and gets the answer. On base of status, toggle buttons are set. If connection was successful, waits for user interaction.

##### UDP communication
For sending UDP packets Anko library is used and DatagramSocket is created. This is done as background coroutine. 

## NodeMCU app
App running on Lolin NodeMCU is responsible for controlling LED strips. Communicates with android app and gets commands for turning on/off strips. App is able to communicate with 16 android devices at the same time, so it creates list of contacts. Each android app, which is connected to the same wifi network is automaticaly added to this list.
By default connection to wifi is set as: **192.168.1.104:5005**

## Hardware requirements
- Android 5.0 (or higher) device 
- Lolin NodeMCU V3 ESP8266
- 3x TIP122 transistor
- 3x LED strip (one color)
- Jumper cables

## Wiring

##### Transistors
![Alt text](img/transistors.png "Transistors")

##### LED strips
![Alt text](img/led_strips.png "LED strips")

##### Power supply
![Alt text](img/power.png "Power")

##### NodeMCU
![Alt text](img/nodeMCU_wiring.png "NodeMCU")

