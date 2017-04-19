#ifndef Webserver_Automat_h
#define Webserver_Automat_h
#include "Arduino.h"
#include "W5100_Automat.h"
#include <stdio.h>
#define MAX_BUF 1024
uint8_t buf[MAX_BUF];
char buf1[100];
String stringOne;

#define TXBUFADDR  0x5800      // W5100 Send Buffer Base Address
#define RXBUFADDR  0x7800      // W5100 Read Buffer Base Address
#define RX_BUF_MASK 0x7FF
#define TX_BUF_MASK 0x7FF
#define WebserverSocket_MR      0x0700      // Socket 0: Mode Register Address
#define WebserverSocket_CR      0x0701      // Socket 0: Command Register Address
#define WebserverSocket_IR      0x0702      // Socket 0: Interrupt Register Address
#define WebserverSocket_SR      0x0703      // Socket 0: Status Register Address
#define WebserverSocket_PORT    0x0704      // Socket 0: Source Port: 0x0404 to 0x0405
#define WebserverSocket_TX_FSR  0x0720      // Socket 0: Tx Free Size Register: 0x0420 to 0x0421
#define WebserverSocket_TX_RD   0x0722      // Socket 0: Tx Read Pointer Register: 0x0422 to 0x0423
#define WebserverSocket_TX_WR   0x0724      // Socket 0: Tx Write Pointer Register: 0x0424 to 0x0425
#define WebserverSocket_RX_RSR  0x0726      // Socket 0: Rx Received Size Pointer Register: 0x0425 to 0x0427
#define WebserverSocket_RX_RD   0x0728      // Socket 0: Rx Read Pointer: 0x0428 to 0x0429
#define WebserverSocketNum      3

// WebserverSocket_SR values
#define SOCK_CLOSED      0x00     // Closed
#define SOCK_INIT        0x13    // Init state
#define SOCK_LISTEN      0x14   // Listen state
#define SOCK_SYNSENT     0x15   // Connection state
#define SOCK_SYNRECV     0x16   // Connection state
#define SOCK_ESTABLISHED 0x17   // Success to connect
#define SOCK_FIN_WAIT    0x18   // Closing state
#define SOCK_CLOSING     0x1A   // Closing state
#define SOCK_TIME_WAIT   0x1B   // Closing state
#define SOCK_CLOSE_WAIT  0x1C   // Closing state
#define SOCK_LAST_ACK    0x1D   // Closing state
#define SOCK_UDP         0x22   // UDP socket
#define SOCK_IPRAW       0x32   // IP raw mode socket
#define SOCK_MACRAW      0x42   // MAC raw mode socket
#define SOCK_PPPOE       0x5F   // PPPOE socket
// WebserverSocket_MR values
#define MR_CLOSE    0x00    // Unused socket
#define MR_TCP      0x01    // TCP
#define MR_UDP      0x02    // UDP
#define MR_IPRAW    0x03    // IP LAYER RAW SOCK
#define MR_MACRAW   0x04    // MAC LAYER RAW SOCK
#define MR_PPPOE    0x05    // PPPoE
#define MR_ND     0x20    // No Delayed Ack(TCP) flag
#define MR_MULTI    0x80    // support multicating

// WebserverSocket_CR values
#define CR_OPEN          0x01    // Initialize or open socket
#define CR_LISTEN        0x02   // Wait connection request in tcp mode(Server mode)
#define CR_CONNECT       0x04   // Send connection request in tcp mode(Client mode)
#define CR_DISCON        0x08   // Send closing reqeuset in tcp mode
#define CR_CLOSE         0x10   // Close socket
#define CR_SEND          0x20   // Update Tx memory pointer and send data
#define CR_SEND_MAC      0x21   // Send data with MAC address, so without ARP process
#define CR_SEND_KEEP     0x22   // Send keep alive message
#define CR_RECV          0x40   // Update Rx memory buffer pointer and receive data

#define TCP_PORT 80
uint8_t sockstat;
uint16_t rsize;
int getidx,get0idx,get1idx,get2idx,get3idx,risidx,ris1idx;
uint16_t Webserver_recv_size(void)
{
  return ((W5100read(WebserverSocket_RX_RSR) & 0x00FF) << 8 ) + W5100read(WebserverSocket_RX_RSR + 1);
}
void WebserverSocket_close()
{
  // if (sock != 0) return;
   // Send Close Command
   W5100write(WebserverSocket_CR,CR_CLOSE);
   // Waiting until the S0_CR is clear
   while(W5100read(WebserverSocket_CR));
}
uint8_t WebserverSocketOpen(uint8_t sock,uint8_t eth_protocol,uint16_t tcp_port)
{
    uint8_t retval=0;

    //if (sock != 0) return retval;
    // Make sure we close the socket first
    if (W5100read(WebserverSocket_SR) == SOCK_CLOSED) {
      WebserverSocket_close();
    };
    // Assigned Socket 0 Mode Register
    W5100write(WebserverSocket_MR,eth_protocol);
    // Now open the Socket 0
    W5100write(WebserverSocket_PORT,((tcp_port & 0xFF00) >> 8 ));
    W5100write(WebserverSocket_PORT + 1,(tcp_port & 0x00FF));
    W5100write(WebserverSocket_CR,CR_OPEN);                   // Open Socket
    // Wait for Opening Process
    while(W5100read(WebserverSocket_CR));
    // Check for Init Status
    if (W5100read(WebserverSocket_SR) == SOCK_INIT)
      retval=1;
    else
      WebserverSocket_close();

    return retval;
}

uint8_t Webserver_listen()
{
   uint8_t retval = 0;
   
   if (W5100read(WebserverSocket_SR) == SOCK_INIT) {
     // Send the LISTEN Command
     W5100write(WebserverSocket_CR,CR_LISTEN);
     // Wait for Listening Process
     while(W5100read(WebserverSocket_CR));
     // Check for Listen Status
     if (W5100read(WebserverSocket_SR) == SOCK_LISTEN)
       retval=1;
     else
       WebserverSocket_close();
    }
    return retval;
}
uint16_t Webserver_recv(uint8_t *buf,uint16_t buflen)
{
    uint16_t ptr,offaddr,realaddr;     
    if (buflen <= 0) return 1;   
    // If the request size > MAX_BUF,just truncate it
    if (buflen > MAX_BUF)
      buflen=MAX_BUF - 2;
    // Read the Rx Read Pointer
    ptr = W5100read(WebserverSocket_RX_RD);
    offaddr = (((ptr & 0x00FF) << 8 ) + W5100read(WebserverSocket_RX_RD + 1));
    while(buflen) {
      buflen--;
      realaddr=RXBUFADDR + (offaddr & RX_BUF_MASK);
      *buf = W5100read(realaddr);
      offaddr++;
      buf++;
    }
    *buf='\0';        // String terminated character
    // Increase the S0_RX_RD value, so it point to the next receive
    W5100write(WebserverSocket_RX_RD,(offaddr & 0xFF00) >> 8 );
    W5100write(WebserverSocket_RX_RD + 1,(offaddr & 0x00FF)); 

    // Now Send the RECV command
    W5100write(WebserverSocket_CR,CR_RECV);
    _delay_us(5);    // Wait for Receive Process
    return 1;
}
void WebserverSocket_disconnect()
{
   
   // Send Disconnect Command
   W5100write(WebserverSocket_CR,CR_DISCON);
   // Wait for Disconecting Process
   while(W5100read(WebserverSocket_CR));
}
uint16_t Webserver_send(const uint8_t *buf,uint16_t buflen)
{
    uint16_t ptr,offaddr,realaddr,txsize,timeout;   
    if (buflen <= 0 ) return 0;
    // Make sure the TX Free Size Register is available
    txsize=W5100read(WebserverSocket_TX_FSR);
    txsize=(((txsize & 0x00FF) << 8 ) + W5100read(WebserverSocket_TX_FSR + 1));
    timeout=0;
    while (txsize < buflen) {
      _delay_ms(1);
     txsize=W5100read(WebserverSocket_TX_FSR);
     txsize=(((txsize & 0x00FF) << 8 ) + W5100read(WebserverSocket_TX_FSR + 1));
     // Timeout for approx 1000 ms
     if (timeout++ > 1000) {
       // Disconnect the connection
       WebserverSocket_disconnect();
       return 0;
     }
   }  
   // Read the Tx Write Pointer
   ptr = W5100read(WebserverSocket_TX_WR);
   offaddr = (((ptr & 0x00FF) << 8 ) + W5100read(WebserverSocket_TX_WR + 1));
    while(buflen) {
      buflen--;
      // Calculate the real W5100 physical Tx Buffer Address
      realaddr = TXBUFADDR + (offaddr & TX_BUF_MASK);
      // Copy the application data to the W5100 Tx Buffer
      W5100write(realaddr,*buf);
      offaddr++;
      buf++;
    }
    // Increase the S0_TX_WR value, so it point to the next transmit
    W5100write(WebserverSocket_TX_WR,(offaddr & 0xFF00) >> 8 );
    W5100write(WebserverSocket_TX_WR + 1,(offaddr & 0x00FF)); 
    // Now Send the SEND command
    W5100write(WebserverSocket_CR,CR_SEND);
    // Wait for Sending Process
    while(W5100read(WebserverSocket_CR)); 
    return 1;
}



uint8_t Webserver_Automat(uint8_t event)
{
    static uint8_t state=0;
    uint8_t stateOld=state;
    static uint32_t Timer;
    static int clo=0;
    #ifdef Webserver_Automat_LOGGING
        Serial.print("  Webserver_automat_Start_state:") ;
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
        sockstat=W5100read(WebserverSocket_SR);
        Serial.print("SOCKET_STATE:");
        Serial.println(sockstat);        
        switch(sockstat)
        {
            case SOCK_CLOSED:
                  state=2;
                  break;
            case SOCK_ESTABLISHED:
                  if(clo==0){
                    state=5;
                    Timer=millis();}
                  else{
                    state=6;
                    Timer=millis();
                    } 
                  break; 
            case SOCK_FIN_WAIT:   
            case SOCK_CLOSING:    
            case SOCK_TIME_WAIT:  
            case SOCK_CLOSE_WAIT:
            case SOCK_LAST_ACK:   
              // Force to close the socket
            WebserverSocket_close();
                  break;  
          }
      break;

      case 2: 
          if(WebserverSocketOpen(WebserverSocketNum, MR_TCP, TCP_PORT)>0)
            {
              state=3;
            }
            else{
              state=1;
              };
          break;
      break;

      case 3:
          if(Webserver_listen()<=0){
              state=4;
            }else{state=1;}
      break;
      
      case 4: 
          state=1;    
      break;
      
      case 5:
          if((millis()-Timer)>500){
            Timer=millis();
            state=6;
            };
      break;
      
      case 6:
          if((millis()-Timer)>50){
            if(clo>=5 && rsize==0){Timer=millis();state=7;} 
            else{
                  if(rsize>0)
                    {
                      state=9;
                     }
                   else{
                      state=1;
                    }
                  
                  }        
            }       
      break;
      case 7:
        if((millis()-Timer)>500){
          clo=0;
          state=8;
          break;
          }
      break;
      case 8:
          if(rsize>0){
            state=9;
            break;
            }else{state=1;}
     
      break;
      case 9:
          clo=0;
          Webserver_recv(buf,rsize);
              #ifdef Webserver_Automat_LOGGING
                  Serial.println("REQUEST:");
                  for(int i=0;i<rsize;i++){
                  Serial.print((char)buf[i]);
                  
                  }
                  Serial.println("");
              #endif
              stringOne = (char *)buf;
              
         // getidx=strindex((char *)buf,"GET / ");
         Serial.println(stringOne.indexOf("GET /"));
          if(stringOne.indexOf("GET /")>=0){state=10;};
          
      break;
      case 10:
          strcpy((char *)buf,("HTTP/1.0 200 OK\nContent-Type: text/html; charset=windows-1251\nContent-Length:3953\n\n"));
          strcat((char *)buf,("<!DOCTYPE HTML>\n<html><title>RS to Ethernet</title><body>\n"));
          strcat((char *)buf,("<h1>Вас приветствует веб интерфейс</h1>\r\n"));
          strcat((char *)buf,("<A href='t.htm'>Температура в помещении</a>"));  
          state=11;   
      break;
      case 11:
          Webserver_send(buf,strlen((char *)buf));
          state=15;
      break;
      case 12:
         
     
      break;
      case 13:
         
     
      break;
      case 14:
         
     
      break;
      case 15:
          WebserverSocket_disconnect();
          WebserverSocket_close();
          state=1;
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

                break;
                
            case 2:

               break;
                
            case 4:

               break;       

            case 6:
                rsize=Webserver_recv_size();
                clo++;
               break; 

            case 255:
              break;
          }
      } 
    #ifdef Webserver_Automat_LOGGING
        Serial.print("  Webserver_automat_End_state:") ;
        Serial.println(state) ;
    #endif
      return state;
  }

  
#endif
