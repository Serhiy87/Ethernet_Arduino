#ifndef UDP_Automat_h
#define UDP_Automat_h
#include "Arduino.h"
#include "W5100_Automat.h"

uint16_t _remaining[4];
uint16_t _port; // local port to listen on
uint8_t _remoteIP[4]; // remote IP address for the incoming packet whilst it's being processed
uint16_t _remotePort[4]; // remote port for the incoming packet whilst it's being processed
uint16_t _offset, _offset1, _offset2; // offset into the packet being sent
extern void resetTEST(void);
uint8_t UDPbegin0_Automat(uint8_t event,uint16_t port){
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef UDPbegin0_Automat_LOGGING
        Serial.print("UDPbegin0_automat state:") ;
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
          if(execCmdS0_Automat(1,Sock_CLOSE)!=255)
          {
            break;
          }
          W5100write(0x402, 0xFF);
          W5100write(0x400, 0x02); 
          W5100write(0x404, (port&(0xFF00))>>8);
          W5100write(0x405, (port&(0x00FF)));  
          state=2;
      break;
      
      case 2:
          if(execCmdS0_Automat(1,Sock_OPEN)!=255)
          {
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
                execCmdS0_Automat(0,0);
            break;

            case 2:
                execCmdS0_Automat(0,0);
            break;
            
            case 255:
            break;
          }
      } 
      return state;
    };

uint8_t UDPbegin1_Automat(uint8_t event,uint16_t port){
  
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef UDPbegin1_Automat_LOGGING
        Serial.print("UDPbegin1_automat state:") ;
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
          if(execCmdS1_Automat(1,Sock_CLOSE)!=255)
          {
            break;
          }
          W5100write(0x502, 0xFF);
          W5100write(0x500, 0x02); 
          W5100write(0x504, (port&(0xFF00))>>8);
          W5100write(0x505, (port&(0x00FF)));  
          state=2;
      break;
      
      case 2:
          if(execCmdS1_Automat(1,Sock_OPEN)!=255)
          {
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
                execCmdS1_Automat(0,0);
            break;

            case 2:
                execCmdS1_Automat(0,0);
            break;
            
            case 255:
            break;
          }
      } 
      return state;
    };

uint8_t UDPbegin2_Automat(uint8_t event,uint16_t port){

   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef UDPbegin2_Automat_LOGGING
        Serial.print("UDPbegin2_automat state:") ;
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

          if(execCmdS2_Automat(1,Sock_CLOSE)!=255)
          {
            
            break;
          }
         // execCmdSn(2,Sock_CLOSE);
          W5100write(0x602, 0xFF);
          W5100write(0x600, 0x02); 
          W5100write(0x604, (port&(0xFF00))>>8);
          W5100write(0x605, (port&(0x00FF)));
            
          state=2;
      break;
      
      case 2:
         if(execCmdS2_Automat(1,Sock_OPEN)!=255)
          {
            break;
          } 
          //execCmdSn(2,Sock_OPEN); 
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
               execCmdS2_Automat(0,Sock_CLOSE);
            break;

            case 2:
                execCmdS2_Automat(0,Sock_OPEN);
            break;
            
            case 255:
            break;
          }
      } 
      return state;
    };
int startUDP(uint8_t s, uint8_t* addr, uint16_t port)
{
  if
    (
     ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
     ((port == 0x00))
    ) 
  {
    return 0;
  }
  else
  {
   //SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
   if(s==0){
         W5100_writeS0DIPR(addr);
         W5100_writeS0DPORT(port);
   }
   else if(s==1)
   {
        W5100_writeS1DIPR(addr);
        W5100_writeS1DPORT(port);
    }
       else if(s==2)
   {
        W5100_writeS2DIPR(addr);
        W5100_writeS2DPORT(port);
    }
   // SPI.endTransaction();
    return 1;
  }
};
	
int _dhcpUdpSocket_beginPacket(uint8_t* ip, uint16_t port)
{
  _offset = 0;
  return startUDP(0, ip, port);
};

int _UDPtoSerialSocket_beginPacket(uint8_t* ip, uint16_t port)
{
  _offset1 = 0;
  return startUDP(1, ip, port);
};

int _Socket2_beginPacket(uint8_t* ip, uint16_t port)
{
  _offset2 = 0;
  return startUDP(2, ip, port);
};

uint16_t _dhcpUdpSocket_write(const uint8_t *buffer, uint16_t size)
{
  uint16_t bytes_written = bufferData(0, _offset, buffer, size);
  _offset += bytes_written;
  return bytes_written;
};

uint16_t _UDPtoSerialSocket_write(const uint8_t *buffer, uint16_t size)
{
  uint16_t bytes_written = bufferData(1, _offset1, buffer, size);
  _offset1 += bytes_written;
  return bytes_written;
};

uint16_t _Socket2_write(const uint8_t *buffer, uint16_t size)
{
  uint16_t bytes_written = bufferData(2, _offset2, buffer, size);
  _offset2 += bytes_written;
  return bytes_written;
};
extern void resetSNMP(void);
uint8_t _UDPtoSerialSocket_endPacket_Automat(uint8_t event)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
   static uint32_t Timer;

    switch(event){
      case 0:
        state=0;
        break;
      default:
        break;
      }
          #ifdef _UDPtoSerialSocket_endPacket0_LOGGING
        Serial.print("_UDPtoSerialSocket_endPacket0_LOGGING state:") ;
        Serial.println(state) ;
    #endif
     switch(state)
     {
      case 0:
        state=1;
      break;
      
      case 1:
         if(execCmdS1_Automat(1,Sock_SEND)!=255)
          {
            break;
          }
          Timer=millis();
          state=2;
      break;
      case 2:
          if((readSnIR(1) & SEND_OK) != SEND_OK){
                if (readSnIR(1) & TIMEOUT)
                {
                  writeSnIR(1, (SEND_OK|TIMEOUT)); 
                }
                else 
                if(millis()-Timer>5000){
                resetSNMP();
                state=255;
                }
                break;
            }
            writeSnIR(1, SEND_OK);
            Serial.println("SEND OK");         
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
              execCmdS1_Automat(0,Sock_SEND);
            break;
            case 255:
            break;
          }
      } 
      return state;
}
extern void resetDHCP(void);
uint8_t _dhcpUdpSocket_endPacket_Automat(uint8_t event)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
   static uint32_t Timer;
   
    #ifdef _dhcpUdpSocket_endPacket0_LOGGING
        Serial.print("_dhcpUdpSocket_endPacket0_LOGGING state:") ;
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
         if(execCmdS0_Automat(1,Sock_SEND)!=255)
          {
            break;
          }
          Timer=millis();
          state=2;
      break;
      case 2:
          if((readSnIR(0) & SEND_OK) != SEND_OK){
                if (readSnIR(0) & TIMEOUT)
                {
                  writeSnIR(0, (SEND_OK|TIMEOUT));
                }
                else 
                if(millis()-Timer>5000){
                resetDHCP();
                state=255;
                }
                break;
            }
              writeSnIR(0, SEND_OK);
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
              execCmdS0_Automat(0,Sock_SEND);
            break;
            case 255:
            break;
          }
      } 
      return state;
}
extern void resetTEST(void);
uint8_t _Socket2_endPacket_Automat(uint8_t event)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
   static uint32_t Timer;
    #ifdef _Socket2_endPacket_LOGGING
        Serial.print("      _Socket2_endPacket_start_state:") ;
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
         if(execCmdS2_Automat(1,Sock_SEND)!=255)
          {
            break;
          }
          Timer=millis();
          state=2;
      break;
      case 2:
          if((readSnIR(2) & SEND_OK) != SEND_OK){
                if (readSnIR(2) & TIMEOUT)
                {
                  writeSnIR(2, (SEND_OK|TIMEOUT));
                }
                else 
                  if(millis()-Timer>10000){
                    resetTEST();
                    state=255;
                    }

                break;
            }
              writeSnIR(2, SEND_OK);
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
              execCmdS2_Automat(0,Sock_SEND);
            break;
            case 255:
            break;
          }
      } 
          #ifdef _dhcpUdpSocket_endPacket0_LOGGING
        Serial.print("      _Socket2_endPacket_end_state:") ;
        Serial.println(state) ;
    #endif
      return state;
}


int UDP_read(uint8_t _sock)
{
  uint8_t byte;

  if ((_remaining[_sock] > 0) && (recv(_sock, &byte, 1) > 0))
  {
    // We read things without any problems
    _remaining[_sock]--;
    return byte;
  }

  // If we get here, there's no data available
  return -1;
}

int UDP_parsePacket(uint8_t _sock)
{
  // discard any remaining bytes in the last packet
  while (_remaining[_sock]) {
    // could this fail (loop endlessly) if _remaining > 0 and recv in read fails?
    // should only occur if recv fails after telling us the data is there, lets
    // hope the w5100 always behaves :)
    UDP_read(_sock);
  }

  if (recvAvailable(_sock) > 0)
  {
    //HACK - hand-parse the UDP packet using TCP recv method
    uint8_t tmpBuf[8];
    int ret =0; 
    //read 8 header bytes and get IP and port from it
    ret = recv(_sock,tmpBuf,8);
    if (ret > 0)
    {
      _remoteIP[0] = tmpBuf[0];
      _remoteIP[1] = tmpBuf[1];
      _remoteIP[2] = tmpBuf[2];
      _remoteIP[3] = tmpBuf[3];
      _remotePort[_sock] = tmpBuf[4];
      _remotePort[_sock] = (_remotePort[_sock] << 8) + tmpBuf[5];
      _remaining[_sock] = tmpBuf[6];
      _remaining[_sock] = (_remaining[_sock] << 8) + tmpBuf[7];

      // When we get here, any remaining bytes are the data
      ret = _remaining[_sock];
    }
    return ret;
  }
  // There aren't any packets available
  return 0;
}

int _dhcpUdpSocket_parsePacket(){
  return UDP_parsePacket(0);
  }
int _UDPtoSerialSocket_parsePacket(){
  return UDP_parsePacket(1);
  }

int _dhcpUdpSocket_read(unsigned char* buffer, size_t len)
{

  if (_remaining[0] > 0)
  {

    int got;

    if (_remaining[0] <= len)
    {
      // data should fit in the buffer
      got = recv(0, buffer, _remaining[0]);
    }
    else
    {
      // too much data for the buffer, 
      // grab as much as will fit
      got = recv(0, buffer, len);
    }

    if (got > 0)
    {
      _remaining[0] -= got;
      return got;
    }

  }

  // If we get here, there's no data available or recv failed
  return -1;

}

int _SNMPSocket_read(unsigned char* buffer, size_t len)
{

  if (_remaining[1] > 0)
  {

    int got;

    if (_remaining[1] <= len)
    {
      // data should fit in the buffer
      got = recv(1, buffer, _remaining[1]);
    }
    else
    {
      // too much data for the buffer, 
      // grab as much as will fit
      got = recv(1, buffer, len);
    }

    if (got > 0)
    {
      _remaining[1] -= got;
      return got;
    }

  }

  // If we get here, there's no data available or recv failed
  return -1;

}
uint16_t _dhcpUdpSocket_remotePort(){
  return _remotePort[0];
  }

uint16_t _snmpUdpSocket_remotePort(){
  return _remotePort[1];
  }
int _dhcpUdpSocket_read(){
  return UDP_read(0);
};
_dhcpUdpSocket_flush(){};
uint16_t _dhcpUdpSocket_available(){
  return _remaining[0];
  };
uint16_t _SNMPUdpSocket_available(){
  return _remaining[1];
  };

uint8_t UDPtoSerial_sendmessage_Automat(uint8_t event, uint8_t* remote_ip, uint16_t remote_port, uint8_t* data,uint16_t _size)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef sendUDPmessage_Automat_LOGGING
        Serial.print("sendUDPmessage_Automat state:") ;
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
         if(_UDPtoSerialSocket_endPacket_Automat(1)!=255){
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
              _UDPtoSerialSocket_beginPacket(remote_ip, remote_port);
              _UDPtoSerialSocket_write(data, _size);
              _UDPtoSerialSocket_endPacket_Automat(0);
            break;
            
            case 255:
            break;
          }
      } 
          #ifdef send_DHCP_MESSAGE_Automat_LOGGING
        Serial.print("send_DHCP_MESSAGE_Automat End_state:") ;
        Serial.println(state) ;
    #endif
      return state;
}
  
#endif
