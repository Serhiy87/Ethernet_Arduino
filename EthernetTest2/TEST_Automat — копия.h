#ifndef TEST_Automat_h
#define TEST_Automat_h
#include "Arduino.h"
#include "UDP_Automat.h"

uint8_t TEST_automat(uint8_t event)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
   static uint32_t Timer;
   uint16_t numBytes=0;
   uint8_t messageType = 0;
    #ifdef DHCP_Automat_LOGGING
        Serial.print("DHCP_automat state:") ;
        Serial.println(state) ;
    #endif
    switch(event){
      case 0:
        state=0;
        break;
      default:
        break;
      }
     switch(state)
     {
      case 0:
        state=1;
      break;
      
      case 1:
        _dhcpTransactionId = random(1UL, 2000UL);
        _dhcpInitialTransactionId = _dhcpTransactionId;
          memset(_dhcpLocalIp, 0, 4);
          memset(_dhcpSubnetMask, 0, 4);
          memset(_dhcpGatewayIp, 0, 4);
          memset(_dhcpDhcpServerIp, 0, 4);
          memset(_dhcpDnsServerIp, 0, 4);
        if(UDPbegin0_Automat(1, 68)!=255)
        {
          break;
         }
         state=3;            
      break;

      case 3: 
         // Broadcast address
        //_dhcpUdpSocket_beginPacket(dA, 162);
        if(readSnSR(0)!=0x22){
                resetEthernet();
                break;
        }
         if(send_DHCP_MESSAGE_Automat(1, 1, 10)!=255)
        {
             break;
        }
        _dhcp_state=DHCP_DISCOVER;
        Timer=millis();
        state=5;
      break;
      
      case 4: 


      break;
      case 5:
          uint32_t respId;
          messageType=parseDHCPResponse(_responseTimeout, respId);
          if(messageType==DHCP_OFFER){
               #ifdef send_DHCP_MESSAGE_Automat_LOGGING
              Serial.println("OFFER RECEIVED!");
              #endif
              _dhcpTransactionId=respId;
              state=6;
             // _dhcpTransactionId++;
              break;
           }
           if(((millis()-Timer)>5000)){
            state=1;
            break;
           }
          state=5;
      break;
      case 6:
           if(readSnSR(0)!=0x22){
                resetEthernet();
            }
           if(send_DHCP_MESSAGE_Automat(1, 3, 100)!=255)
            {
                 break;
            }
          _dhcp_state=STATE_DHCP_REQUEST;
                  Timer=millis();
           state=7;
      break;
      case 7:
          
          messageType=parseDHCPResponse(_responseTimeout, respId);
             #ifdef send_DHCP_MESSAGE_Automat_LOGGING
          Serial.println(messageType);
          #endif
          if(messageType==DHCP_ACK){
              #ifdef send_DHCP_MESSAGE_Automat_LOGGING
                  Serial.println("ACK RECEIVED!");
              #endif
              W5100_SetIP(_dhcpLocalIp[0],_dhcpLocalIp[1],_dhcpLocalIp[2],_dhcpLocalIp[3]);
              TimeLeasedStart=millis();
              state=255;
              break;
             
           }
           else{
            if(messageType==DHCP_NAK){
                           #ifdef send_DHCP_MESSAGE_Automat_LOGGING
                            Serial.println("NAK RECEIVED!");
                            #endif
                            state=3;
                            break;
              }
            }
           if(((millis()-Timer)>5000)){
            state=1;
            break;
           }
          state=7;
      break;
      case 255:
                if((millis()-TimeLeasedStart)>300000){
                  state=1;
                      #ifdef send_DHCP_MESSAGE_Automat_LOGGING
                          Serial.println("DHCP Rerequest!");
                      #endif
                  }
      break;
     }
     if(state!=stateOld){
          switch(state)
          {
            case 0:                 
                break;
                
            case 1:
                UDPbegin0_Automat(0, 68); 
                break;
                
            case 3:
               send_DHCP_MESSAGE_Automat(0, 1, 10);
               break;
            
            case 6:
               send_DHCP_MESSAGE_Automat(0, 3, 10);
               break;

            case 255:
              break;
          }
      } 
    #ifdef DHCP_Automat_LOGGING
        Serial.print("DHCP_automat_End_state:") ;
        Serial.println(state) ;
    #endif
      return state;
  }



#endif
