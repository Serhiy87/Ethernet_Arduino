#ifndef UDPtoSerial_Automat_h
#define UDPtoSerial_Automat_h
#include "Arduino.h"
#include "UDP_Automat.h"

uint16_t UDP_to_Serial_LocalPort = 50000;


uint8_t destination_addr[] = {10,0,0,51};
uint16_t destination_port = 50000;

uint8_t data[300];
uint16_t _size=4;
uint32_t UtoS_Timer=0;
uint16_t SerialVal1, SerialVal2;

uint8_t UDPtoSerial_Automat(uint8_t event)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef UDPtoSerial_Automat_LOGGING
        Serial.print("UDPtoSerial_Automatt state:") ;
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
        if(UDPbegin1_Automat(1, UDP_to_Serial_LocalPort)!=255)
        {
          break;
         }
         state=3;            
      break;

      case 2:
     
        if(UDPtoSerial_sendmessage_Automat(1, destination_addr,  destination_port, data, SerialVal2)!=255){break;};
        state=3;   
          //UtoS_Timer=millis();     
      break;

      case 3:
          SerialVal1=Serial.available();
          if(SerialVal1!=0)
            {
                state=4;
                UtoS_Timer=millis();
                break;
            }        
          if(_UDPtoSerialSocket_parsePacket())
            {
              if(_remaining[1]){
                Serial.write(UDP_read(1));
                }
            }
            break;
          break;
      case 4:
        if((millis()-UtoS_Timer)>10){
              SerialVal2=Serial.available();
              if(SerialVal2==SerialVal1)
              {
                 state=2;
                 break;
              }
              else{
                state=3;
                break;
                }
           }
       break;
        
      case 255:

      break;
     }
     if(state!=stateOld){
          switch(state)
          {
            case 0:
            break;
            
            case 1:
                UDPbegin1_Automat(0, UDP_to_Serial_LocalPort); 
            break;
            
            case 2:
                for(uint16_t i=0;i<SerialVal2;i++)
                {
                  data[i]=Serial.read();
                }
                UDPtoSerial_sendmessage_Automat(0, destination_addr,  destination_port, data, SerialVal2);
            break;
            
            case 255:             
            break;
          }
      } 
    #ifdef UDPtoSerial_Automat_LOGGING
        Serial.print("UDPtoSerial_Automat_End_state:") ;
        Serial.println(state) ;
    #endif
      return state;
  }

#endif
