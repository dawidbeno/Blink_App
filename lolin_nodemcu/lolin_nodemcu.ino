#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Constants
#define DEBUG (true)
#define CONTACTS_COUNT 16
#define GETSTATUS 'S'
#define GETCONTACTS 'C'
#define SETON 'D'
#define SETOFF 'N'
#define STRIP_ONE '1'
#define STRIP_TWO '2'
#define STRIP_THREE '3'

#define LEDPIN1 16
#define LEDPIN2 5
#define LEDPIN3 4

// Connection credentials
const char* ssid = ""; // wifi SSID
const char* passwd = ""; // wifi password
unsigned int udpPort = 5005;
char targetIP[15] = "192.168.1.104\0"; // local IP of NodeMCU (change if needed)

// Globals
WiFiUDP Udp;
char incomingPacket[32];
char message[4];
int strips[3] = {0,0,0};
char contacts[CONTACTS_COUNT][15];


void setup() {
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(LEDPIN3, OUTPUT);
  if(DEBUG){
    Serial.begin(115200);
    Serial.println();
  }
  for(int x=0; x<CONTACTS_COUNT; x++){
    contacts[x][0] = '\0';
  }
  strcpy(contacts[0], targetIP);
  
  sprintf(message, "Connecting to %s . ", ssid); output(contacts[0], message, 0);
  
  WiFi.begin(ssid, passwd);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    output(contacts[0], ".", 0);
  }
  output(contacts[0], "Connected", 0);

  Udp.begin(udpPort);
  printStatus(contacts[0]);
}


// Receiving packets
void loop() {
  char c;
  int packetSize = Udp.parsePacket();
  
  if(packetSize){
    //sprintf(message, "Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort()); output(message, 1);
    int len = Udp.read(incomingPacket, 255);
    if(len > 0){
      incomingPacket[len] = '\0';
    }
    c = incomingPacket[0];
    switch(c){
      case GETSTATUS:
        addToContacts(Udp.remoteIP().toString().c_str());
        printStatus(Udp.remoteIP().toString().c_str());
        break;
      case GETCONTACTS:
        printContacts(Udp.remoteIP().toString().c_str());
        break;
      case STRIP_ONE:
        setStripOne(!strips[0]);
        sprintf(message, "STATUS:%d,%d,%d", strips[0], strips[1], strips[2]); output_broadcast(message);
        break;
      case STRIP_TWO:
        setStripTwo(!strips[1]);
        sprintf(message, "STATUS:%d,%d,%d", strips[0], strips[1], strips[2]); output_broadcast(message);
        break;
      case STRIP_THREE:
        setStripThree(!strips[2]);
        sprintf(message, "STATUS:%d,%d,%d", strips[0], strips[1], strips[2]); output_broadcast(message);
        break;
      case SETON:
        setStripOne(1);
        setStripTwo(1);
        setStripThree(1);
        sprintf(message, "STATUS:%d,%d,%d", strips[0], strips[1], strips[2]); output_broadcast(message);
        break;
      case SETOFF:
        setStripOne(0);
        setStripTwo(0);
        setStripThree(0);
        sprintf(message, "STATUS:%d,%d,%d", strips[0], strips[1], strips[2]); output_broadcast(message);
        break;
    }
  }
}



void setStripOne(int value){
  if(value == 1){
    strips[0] = 1;
    digitalWrite(LEDPIN1, HIGH);
  }else if (value == 0){
    strips[0] = 0;
    digitalWrite(LEDPIN1, LOW);
  }
}

void setStripTwo(int value){
  if(value == 1){
    strips[1] = 1;
    digitalWrite(LEDPIN2, HIGH);
  }else if (value == 0){
    strips[1] = 0;
    digitalWrite(LEDPIN2, LOW);
  }
}

void setStripThree(int value){
  if(value == 1){
    strips[2] = 1;
    digitalWrite(LEDPIN3, HIGH);
  }else if (value == 0){
    strips[2] = 0;
    digitalWrite(LEDPIN3, LOW);
  }
}

void addToContacts(const char* ip){
  int x = 0, found = 0, cmp = 1;
  while(contacts[x][0] != '\0'){
    cmp = strcmp(contacts[x], ip);
    if(cmp == 0){
      found = 1;
      break;
    }
    x++;
  }
  if(found == 0){
    strcpy(contacts[x], ip);
  }
}

void printStatus(const char* ip){
  sprintf(message, "IP:%s:%d", WiFi.localIP().toString().c_str(), udpPort); output(ip, message, 1);
  sprintf(message, "STATUS:%d,%d,%d", strips[0], strips[1], strips[2]); output(ip, message, 1);
}

void printContacts(const char* ip){
  output(ip, "CONTACTS:", 1);
  for(int x=0; x<CONTACTS_COUNT; x++){
    if(contacts[x][0] != '\0'){
      sprintf(message, "C_IP_%d:%s", x, contacts[x]); output(ip, message, 1);
    }
  }
}

void output_broadcast(char act_message[]){
  for(int x=0; x<CONTACTS_COUNT; x++){
    if(contacts[x][0] != '\0'){
      output(contacts[x], act_message, 1);
    }
  }
}

void output(const char* ip, const char act_message[], boolean writeUdp){
  if(DEBUG) Serial.println(act_message);
  
  if(writeUdp){
    Udp.beginPacket(ip, udpPort);
    Udp.write(act_message);
    Udp.endPacket();
  }
}

