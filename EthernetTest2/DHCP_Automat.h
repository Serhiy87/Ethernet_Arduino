#ifndef DHCP_Automat_h
#define DHCP_Automat_h
#include "Arduino.h"
#include "UDP_Automat.h"

#define DHCP_FLAGSBROADCAST  0x8000

#define  DHCP_SERVER_PORT  67  /* from server to client */
#define DHCP_CLIENT_PORT  68  /* from client to server */
/* DHCP message OP code */
#define DHCP_BOOTREQUEST  1
#define DHCP_BOOTREPLY    2

#define DHCP_HTYPE10MB    1
#define DHCP_HTYPE100MB   2

#define DHCP_HLENETHERNET  6
#define DHCP_HOPS   0
#define DHCP_SECS   0


#define MAGIC_COOKIE    0x63825363

/* DHCP message type */
#define  DHCP_DISCOVER   1
#define DHCP_OFFER        2
#define DHCP_REQUEST    3
#define DHCP_DECLINE    4
#define DHCP_ACK        5
#define DHCP_NAK        6
#define DHCP_RELEASE    7
#define DHCP_INFORM       8

/* DHCP state machine. */
#define STATE_DHCP_START 0
#define  STATE_DHCP_DISCOVER 1
#define STATE_DHCP_REQUEST  2
#define STATE_DHCP_LEASED 3
#define STATE_DHCP_REREQUEST  4
#define STATE_DHCP_RELEASE  5

#define htons(x) ( ((x)<< 8 & 0xFF00) | \
                   ((x)>> 8 & 0x00FF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x) 
              uint32_t TimeLeasedStart=0;
#define HOST_NAME "WIZnet"
enum
{
  padOption   = 0,
  subnetMask1    = 1,
  timerOffset   = 2,
  routersOnSubnet   = 3,
  /* timeServer   = 4,
  nameServer    = 5,*/
  dns     = 6,
  /*logServer   = 7,
  cookieServer    = 8,
  lprServer   = 9,
  impressServer   = 10,
  resourceLocationServer  = 11,*/
  hostName    = 12,
  /*bootFileSize    = 13,
  meritDumpFile   = 14,*/
  domainName    = 15,
  /*swapServer    = 16,
  rootPath    = 17,
  extentionsPath    = 18,
  IPforwarding    = 19,
  nonLocalSourceRouting = 20,
  policyFilter    = 21,
  maxDgramReasmSize = 22,
  defaultIPTTL    = 23,
  pathMTUagingTimeout = 24,
  pathMTUplateauTable = 25,
  ifMTU     = 26,
  allSubnetsLocal   = 27,
  broadcastAddr   = 28,
  performMaskDiscovery  = 29,
  maskSupplier    = 30,
  performRouterDiscovery  = 31,
  routerSolicitationAddr  = 32,
  staticRoute   = 33,
  trailerEncapsulation  = 34,
  arpCacheTimeout   = 35,
  ethernetEncapsulation = 36,
  tcpDefaultTTL   = 37,
  tcpKeepaliveInterval  = 38,
  tcpKeepaliveGarbage = 39,
  nisDomainName   = 40,
  nisServers    = 41,
  ntpServers    = 42,
  vendorSpecificInfo  = 43,
  netBIOSnameServer = 44,
  netBIOSdgramDistServer  = 45,
  netBIOSnodeType   = 46,
  netBIOSscope    = 47,
  xFontServer   = 48,
  xDisplayManager   = 49,*/
  dhcpRequestedIPaddr = 50,
  dhcpIPaddrLeaseTime = 51,
  /*dhcpOptionOverload  = 52,*/
  dhcpMessageType   = 53,
  dhcpServerIdentifier  = 54,
  dhcpParamRequest  = 55,
  /*dhcpMsg     = 56,
  dhcpMaxMsgSize    = 57,*/
  dhcpT1value   = 58,
  dhcpT2value   = 59,
  /*dhcpClassIdentifier = 60,*/
  dhcpClientIdentifier  = 61,
  endOption   = 255
};
typedef struct __attribute__((packed)) _RIP_MSG_FIXED
{
  uint8_t  op; 
  uint8_t  htype; 
  uint8_t  hlen;
  uint8_t  hops;
  uint32_t xid;
  uint16_t secs;
  uint16_t flags;
  uint8_t  ciaddr[4];
  uint8_t  yiaddr[4];
  uint8_t  siaddr[4];
  uint8_t  giaddr[4];
  uint8_t  chaddr[6];
}RIP_MSG_FIXED;

uint32_t _dhcpInitialTransactionId;
uint32_t _dhcpTransactionId; 
uint8_t  _dhcpMacAddr[6];
uint8_t  _dhcpLocalIp[4];
uint8_t  _dhcpSubnetMask[4];
uint8_t  _dhcpGatewayIp[4];
uint8_t  _dhcpDhcpServerIp[4];
uint8_t  _dhcpDnsServerIp[4];
uint32_t _dhcpLeaseTime;
  uint32_t _dhcpT1, _dhcpT2;
  unsigned long _renewInSec;
  unsigned long _rebindInSec;
  unsigned long _timeout;
  unsigned long _responseTimeout;
  unsigned long _lastCheckLeaseMillis;
  uint8_t _dhcp_state;

void send_DHCP_MESSAGE(uint8_t messageType, uint16_t secondsElapsed)
{
    uint8_t buffer[32];
    memset(buffer, 0, 32);
    uint8_t dest_addr[4]={ 255, 255, 255, 255 }; // Broadcast address

    _dhcpUdpSocket_beginPacket(dest_addr, DHCP_SERVER_PORT);


    buffer[0] = DHCP_BOOTREQUEST;   // op
    buffer[1] = DHCP_HTYPE10MB;     // htype
    buffer[2] = DHCP_HLENETHERNET;  // hlen
    buffer[3] = DHCP_HOPS;          // hops

    // xid
   #ifdef send_DHCP_MESSAGE_Automat_LOGGING
        Serial.print("Request Transaction ID: ");
        Serial.println(_dhcpTransactionId);
        #endif
    unsigned long xid = htonl(_dhcpTransactionId);
    memcpy(buffer + 4, &(xid), 4);

    // 8, 9 - seconds elapsed
    buffer[8] = ((secondsElapsed & 0xff00) >> 8);
    buffer[9] = (secondsElapsed & 0x00ff);

    // flags
    unsigned short flags = htons(DHCP_FLAGSBROADCAST);
    memcpy(buffer + 10, &(flags), 2);

    // ciaddr: already zeroed
    // yiaddr: already zeroed
    // siaddr: already zeroed
    // giaddr: already zeroed

    //put data in W5100 transmit buffer
    _dhcpUdpSocket_write(buffer, 28);

    memset(buffer, 0, 32); // clear local buffer

    memcpy(buffer, _dhcpMacAddr, 6); // chaddr
   #ifdef send_DHCP_MESSAGE_Automat_LOGGING
    Serial.print("Buffer Mac: ");
    Serial.print(_dhcpMacAddr[0]);
    Serial.print(":");
    Serial.print(_dhcpMacAddr[1]);
    Serial.print(":");
    Serial.print(_dhcpMacAddr[2]);
    Serial.print(":");
    Serial.print(_dhcpMacAddr[3]);
    Serial.print(":");
    Serial.print(_dhcpMacAddr[4]);
    Serial.print(":");
    Serial.println(_dhcpMacAddr[5]);
  #endif
    //put data in W5100 transmit buffer
    _dhcpUdpSocket_write(buffer, 16);

    memset(buffer, 0, 32); // clear local buffer

    // leave zeroed out for sname && file
    // put in W5100 transmit buffer x 6 (192 bytes)
  
    for(int i = 0; i < 6; i++) {
        _dhcpUdpSocket_write(buffer, 32);
    }
  
    // OPT - Magic Cookie
    buffer[0] = (uint8_t)((MAGIC_COOKIE >> 24)& 0xFF);
    buffer[1] = (uint8_t)((MAGIC_COOKIE >> 16)& 0xFF);
    buffer[2] = (uint8_t)((MAGIC_COOKIE >> 8)& 0xFF);
    buffer[3] = (uint8_t)(MAGIC_COOKIE& 0xFF);

    // OPT - message type
    buffer[4] = dhcpMessageType;
    buffer[5] = 0x01;
    buffer[6] = messageType; //DHCP_REQUEST;

    // OPT - client identifier
    buffer[7] = dhcpClientIdentifier;
    buffer[8] = 0x07;
    buffer[9] = 0x01;
    memcpy(buffer + 10, _dhcpMacAddr, 6);

    // OPT - host name
    buffer[16] = hostName;
    buffer[17] = strlen(HOST_NAME) + 6; // length of hostname + last 3 bytes of mac address
    strcpy((char*)&(buffer[18]), HOST_NAME);

    printByte((char*)&(buffer[24]), _dhcpMacAddr[3]);
    printByte((char*)&(buffer[26]), _dhcpMacAddr[4]);
    printByte((char*)&(buffer[28]), _dhcpMacAddr[5]);

    //put data in W5100 transmit buffer
    _dhcpUdpSocket_write(buffer, 30);

    if(messageType == DHCP_REQUEST)
    {
        buffer[0] = dhcpRequestedIPaddr;
        buffer[1] = 0x04;
        buffer[2] = _dhcpLocalIp[0];
        buffer[3] = _dhcpLocalIp[1];
        buffer[4] = _dhcpLocalIp[2];
        buffer[5] = _dhcpLocalIp[3];

        buffer[6] = dhcpServerIdentifier;
        buffer[7] = 0x04;
        buffer[8] = _dhcpDhcpServerIp[0];
        buffer[9] = _dhcpDhcpServerIp[1];
        buffer[10] = _dhcpDhcpServerIp[2];
        buffer[11] = _dhcpDhcpServerIp[3];

        //put data in W5100 transmit buffer
        _dhcpUdpSocket_write(buffer, 12);
    }
    
    buffer[0] = dhcpParamRequest;
    buffer[1] = 0x06;
    buffer[2] = subnetMask1;
    buffer[3] = routersOnSubnet;
    buffer[4] = dns;
    buffer[5] = domainName;
    buffer[6] = dhcpT1value;
    buffer[7] = dhcpT2value;
    buffer[8] = endOption;
    
    //put data in W5100 transmit buffer
    _dhcpUdpSocket_write(buffer, 9);

   // _dhcpUdpSocket_endPacket_Automat(0);
}

uint8_t send_DHCP_MESSAGE_Automat(uint8_t event, uint8_t messageType, uint16_t secondsElapsed){
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef send_DHCP_MESSAGE_Automat_LOGGING
        Serial.print("send_DHCP_MESSAGE_Automat state:") ;
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
         if(_dhcpUdpSocket_endPacket_Automat(1)!=255){
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
               send_DHCP_MESSAGE(messageType, secondsElapsed);
              _dhcpUdpSocket_endPacket_Automat(0);
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
    
uint8_t parseDHCPResponse(unsigned long responseTimeout, uint32_t& transactionId)
{
    uint8_t type = 0;
    uint8_t opt_len = 0;
     
    unsigned long startTime = millis();

    if(_dhcpUdpSocket_parsePacket() <= 0)
    {
            return 255; 
    }
    // start reading in the packet
    RIP_MSG_FIXED fixedMsg;
    _dhcpUdpSocket_read((uint8_t*)&fixedMsg, sizeof(RIP_MSG_FIXED));
  
    if(fixedMsg.op == DHCP_BOOTREPLY && _dhcpUdpSocket_remotePort() == DHCP_SERVER_PORT)
    {
        transactionId = ntohl(fixedMsg.xid);
           #ifdef send_DHCP_MESSAGE_Automat_LOGGING
        Serial.print("Response Transaction ID: ");
        Serial.println(transactionId);
        #endif
        if(memcmp(fixedMsg.chaddr, _dhcpMacAddr, 6) != 0 || (transactionId < _dhcpInitialTransactionId) || (transactionId > _dhcpTransactionId))
        {
            // Need to read the rest of the packet here regardless
            _dhcpUdpSocket_flush();
            return 0;
        }

        memcpy(_dhcpLocalIp, fixedMsg.yiaddr, 4);

        // Skip to the option part
        // Doing this a byte at a time so we don't have to put a big buffer
        // on the stack (as we don't have lots of memory lying around)
        for (int i =0; i < (240 - (int)sizeof(RIP_MSG_FIXED)); i++)
        {
            _dhcpUdpSocket_read(); // we don't care about the returned byte
        }

        while (_dhcpUdpSocket_available() > 0) 
        {
            switch (_dhcpUdpSocket_read()) 
            {
                case endOption :
                    break;
                    
                case padOption :
                    break;
                
                case dhcpMessageType :
                    opt_len = _dhcpUdpSocket_read();
                    type    = _dhcpUdpSocket_read();
                    break;
                
                case subnetMask1 :
                    opt_len = _dhcpUdpSocket_read();
                    _dhcpUdpSocket_read(_dhcpSubnetMask, 4);
                    break;
                
                case routersOnSubnet :
                    opt_len = _dhcpUdpSocket_read();
                    _dhcpUdpSocket_read(_dhcpGatewayIp, 4);
                    for (int i = 0; i < opt_len-4; i++)
                    {
                        _dhcpUdpSocket_read();
                    }
                    break;
                
                case dns :
                    opt_len = _dhcpUdpSocket_read();
                    _dhcpUdpSocket_read(_dhcpDnsServerIp, 4);
                    for (int i = 0; i < opt_len-4; i++)
                    {
                        _dhcpUdpSocket_read();
                    }
                    break;
                
                case dhcpServerIdentifier :
                    opt_len = _dhcpUdpSocket_read();
                    if ((_dhcpDhcpServerIp[0] == 0 && _dhcpDhcpServerIp[1] == 0 &&
                         _dhcpDhcpServerIp[2] == 0 && _dhcpDhcpServerIp[3] == 0) ||
                        *((uint32_t*)_dhcpDhcpServerIp) == *((uint32_t*)_remotePort))
                    {
                        _dhcpUdpSocket_read(_dhcpDhcpServerIp, sizeof(_dhcpDhcpServerIp));
                    }
                    else
                    {
                        // Skip over the rest of this option
                        while (opt_len--)
                        {
                            _dhcpUdpSocket_read();
                        }
                    }
                    break;

                case dhcpT1value : 
                    opt_len = _dhcpUdpSocket_read();
                    _dhcpUdpSocket_read((uint8_t*)&_dhcpT1, sizeof(_dhcpT1));
                    _dhcpT1 = ntohl(_dhcpT1);
                    break;

                case dhcpT2value : 
                    opt_len = _dhcpUdpSocket_read();
                    _dhcpUdpSocket_read((uint8_t*)&_dhcpT2, sizeof(_dhcpT2));
                    _dhcpT2 = ntohl(_dhcpT2);
                    break;

                case dhcpIPaddrLeaseTime :
                    opt_len = _dhcpUdpSocket_read();
                    _dhcpUdpSocket_read((uint8_t*)&_dhcpLeaseTime, sizeof(_dhcpLeaseTime));
                    _dhcpLeaseTime = ntohl(_dhcpLeaseTime);
                    _renewInSec = _dhcpLeaseTime;
                    break;

                default :
                    opt_len = _dhcpUdpSocket_read();
                    // Skip over the rest of this option
                    while (opt_len--)
                    {
                        _dhcpUdpSocket_read();
                    }
                    break;
            }
        }
    }

    // Need to skip to end of the packet regardless here
    _dhcpUdpSocket_flush();

    return type;
}
extern uint32_t DHCP_Leased_count=0;
uint8_t DHCP_automat(uint8_t event)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
   static uint32_t Timer;
   uint16_t numBytes=0;
   uint8_t messageType = 0;
    #ifdef DHCP_Automat_LOGGING
        Serial.print("  DHCP_automat_start_state:") ;
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
          Serial.println("RESET FROM DHCP1");
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

        if(readSnSR(0)!=0x22){
          Serial.println("RESET FROM DHCP1");
                resetEthernet();
                break;
        }
         if(send_DHCP_MESSAGE_Automat(1, DHCP_REQUEST, 10)!=255)
        {
             break;
        }
        Serial.println("STATE = 4!!!");
        _dhcp_state=DHCP_DISCOVER;
        Timer=millis();
        state=7;
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
            Serial.println("RESET FROM DHCP2");
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
                  Serial.print("LEASE TIME IN SEC:");
                  Serial.println(_dhcpLeaseTime);
              #endif
              W5100_SetIP(_dhcpLocalIp[0],_dhcpLocalIp[1],_dhcpLocalIp[2],_dhcpLocalIp[3]);             
              W5100_SetGateway(_dhcpGatewayIp[0],_dhcpGatewayIp[1],_dhcpGatewayIp[2],_dhcpGatewayIp[3]);
              W5100_SetMask(_dhcpSubnetMask[0], _dhcpSubnetMask[1], _dhcpSubnetMask[2], _dhcpSubnetMask[3]);
              Serial.print(_dhcpLocalIp[0]);Serial.print(",");Serial.print(_dhcpLocalIp[1]);Serial.print(",");Serial.print(_dhcpLocalIp[2]);Serial.print(",");Serial.print(_dhcpLocalIp[3]);Serial.println(";");
              Serial.print(_dhcpGatewayIp[0]);Serial.print(",");Serial.print(_dhcpGatewayIp[1]);Serial.print(",");Serial.print(_dhcpGatewayIp[2]);Serial.print(",");Serial.print(_dhcpGatewayIp[3]);Serial.println(";");
              Serial.print(_dhcpSubnetMask[0]);Serial.print(",");Serial.print(_dhcpSubnetMask[1]);Serial.print(",");Serial.print(_dhcpSubnetMask[2]);Serial.print(",");Serial.print(_dhcpSubnetMask[3]);Serial.println(";");
              DHCP_Leased_count++;                                 
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


                 if(_dhcpLeaseTime>0){
                if(((millis()-TimeLeasedStart)>(_dhcpLeaseTime/2)*1000)){
                  state=4;
                      #ifdef send_DHCP_MESSAGE_Automat_LOGGING
                          Serial.println("DHCP Rerequest!");
                      #endif
                  }};
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

           case 4:
               send_DHCP_MESSAGE_Automat(0, DHCP_REQUEST, 10);
               break;
            
            case 6:
               send_DHCP_MESSAGE_Automat(0, 3, 10);
               break;

            case 255:
              break;
          }
      } 
    #ifdef DHCP_Automat_LOGGING
        Serial.print("  DHCP_automat_End_state:") ;
        Serial.println(state) ;
    #endif
      return state;
  }
extern uint32_t resetDHCP_Counter;
extern void resetDHCP(void){
  resetDHCP_Counter++;
 DHCP_automat(0);
  Serial.println("RESET DHCP!!!");
  };

#endif
