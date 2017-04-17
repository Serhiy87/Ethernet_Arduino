//#define LOGGING
#ifdef LOGGING
//#define SNMP_Automat_LOGGING
#define Ethernet_Automat_BEGIN_LOGGING
//#define ResetW5100_Automat_BEGIN_LOGGING
//#define UDPbegin0_Automat_LOGGING
#define DHCP_Automat_LOGGING
//#define execCmdSocket0_LOGGING
#define execCmdSocket2_LOGGING
//#define _dhcpUdpSocket_endPacket0_LOGGING
#define send_DHCP_MESSAGE_Automat_LOGGING
//#define sendUDPmessage_Automat_LOGGING
//#define UDPbegin1_Automat_LOGGING
//#define UDPbegin2_Automat_LOGGING
#define _UDPtoSerialSocket_endPacket0_LOGGING
#define execCmdSocket10_LOGGING
#define TEST_Automat_LOGGING
#define Socket2_sendmessage_Automat_LOGGING
#define _Socket2_endPacket_LOGGING
#endif
#include "Ethernet_Automat.h"
uint32_t _1secTimer=0;
void setup() {
  _1secTimer=millis();
  #ifdef LOGGING
  
  #endif
Serial.begin(9600);
  Serial1.begin(9600);
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  Ethernet_Automat(0);
  
}

void loop() {
  // put your main code here, to run repeatedly:
 // delay(1000);
 
  if((millis()-_1secTimer)>1000)
  {
    _1secTimer=millis();
    Serial1.println("Hello1!");
  }

  Ethernet_Automat(1);
  if(ledstate){
      digitalWrite(13,HIGH);
    }
    else{
      digitalWrite(13,LOW);    
    }
  }
