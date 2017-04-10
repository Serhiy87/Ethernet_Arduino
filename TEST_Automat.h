#ifndef TEST_Automat_h
#define TEST_Automat_h
#include "Arduino.h"
#include "UDP_Automat.h"
uint32_t resetDHCP_Counter=0;
uint32_t resetEthernet_Counter=0;
uint32_t resetTEST_Counter=0;
uint16_t resetSNMP_Counter=0;
uint8_t FreeCoolListenerIP[]={194,176,97,118};
uint16_t FreeCoolPort = 2026;
uint8_t GPRS_DATA[62];
uint16_t Vega_SN=50004;
int32_t Tfree2cond_var=204, Tair_var=106, Troom_var=222, Tcond_var=200;
int16_t *ptr;
int32_t* pi4;
extern uint8_t  _dhcpLocalIp[4];
uint8_t Socket2_sendmessage_Automat(uint8_t event, uint8_t* remote_ip, uint16_t remote_port, uint8_t* data,uint16_t _size)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef Socket2_sendmessage_Automat_LOGGING
        Serial.print("    Socket2_sendmessage_Automat_start_state:") ;
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

         if(_Socket2_endPacket_Automat(1)!=255){
              break;
         }
         state=255;
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

              _Socket2_beginPacket(remote_ip, remote_port);
              _Socket2_write(data, _size);
              _Socket2_endPacket_Automat(0);
            break;
            
            case 255:
            break;
          }
      } 
   #ifdef Socket2_sendmessage_Automat_LOGGING
        Serial.print("Socket2_sendmessage_Automat_end_state:") ;
        Serial.println(state) ;
    #endif
      return state;
}

extern uint32_t DHCP_Leased_count;
extern uint32_t _dhcpLeaseTime;
uint8_t TEST_Automat(uint8_t event)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
   static uint32_t Timer;
   uint16_t numBytes=0;
   uint8_t messageType = 0;
    #ifdef TEST_Automat_LOGGING
        Serial.print("  TEST_automat_Start_state:") ;
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
            if(UDPbegin2_Automat(1, 2025)!=255){
              break;
            }
            Timer=millis();   
            state=2;   
      break;

      case 2: 
         if((millis()-Timer)>10000)
         {
            state=4;
            break;
          }
          break;
      break;
      
      case 4: 
          if(readSnSR(2)!=0x22){
                state = 0;
                Serial.println(readSnSR(2));
                break;
         }
          if(Socket2_sendmessage_Automat(1, FreeCoolListenerIP, FreeCoolPort, GPRS_DATA, sizeof(GPRS_DATA))!=255){
            break;
            };
             Timer=millis(); 
             state=2;
              
      break;
      
      case 5:
       
      break;
      
      case 6:

      break;
      case 7:
          
     
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
                UDPbegin2_Automat(0, 2025); 
                break;
                
            case 2:

               break;
                
            case 4:
               GPRS_DATA[0]  = 5;
               GPRS_DATA[1]  = 1;
               GPRS_DATA[2]  = Vega_SN>>8;
               GPRS_DATA[3]  = (uint8_t)Vega_SN;  
               GPRS_DATA[4]  = 1;
               GPRS_DATA[5]  = 2;
               GPRS_DATA[6]  = 19;
               GPRS_DATA[7]  = 54;
               GPRS_DATA[8]  = _dhcpLocalIp[0];
               GPRS_DATA[9]  = _dhcpLocalIp[1];
               GPRS_DATA[10] = _dhcpLocalIp[2];
               GPRS_DATA[11] = _dhcpLocalIp[3];
               GPRS_DATA[12] = 'W';   
               GPRS_DATA[13] = '5';
               GPRS_DATA[14] = '1';
               GPRS_DATA[15] = '0';
               GPRS_DATA[16] = '0';
               GPRS_DATA[17] = ' ';
               GPRS_DATA[18] = 'T';   
               GPRS_DATA[19] = 'E';
               GPRS_DATA[20] = 'S';
               GPRS_DATA[21] = 'T';

               ptr = (int16_t*)&GPRS_DATA[27];   
               ptr[2] = Troom_var;   
               ptr[3] = Tair_var; 
               ptr[4] = Tcond_var; 
               ptr[5] = Tfree2cond_var;            
               ptr[6] = 123;
               ptr[7] = 1800;
               ptr[8] = 2200;
               pi4 = (int32_t*)&GPRS_DATA[45];
               pi4[0] = millis();
               pi4[1] = DHCP_Leased_count;
               pi4[2] = resetDHCP_Counter;
               pi4[3] = _dhcpLeaseTime;
               GPRS_DATA[61] = 26;                               
               Socket2_sendmessage_Automat(0, FreeCoolListenerIP, FreeCoolPort, GPRS_DATA, sizeof(GPRS_DATA));
               break;            

            case 255:
              break;
          }
      } 
    #ifdef TEST_Automat_LOGGING
        Serial.print("  TEST_automat_End_state:") ;
        Serial.println(state) ;
    #endif
      return state;
  }


extern void resetTEST(void){
  resetTEST_Counter++;
  TEST_Automat(0);
  Serial.println("RESET TEST!!!");
  };

#endif
