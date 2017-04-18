#ifndef W5100_Automat_h
#define W5100_Automat_h

#define MAX_SOCK_NUM 4
#define TXBUF_BASE 0x4000
#define RXBUF_BASE 0x6000

//#define DEBUG

#define SS_PIN 10



  static const uint8_t CLOSED      = 0x00;
  static const uint8_t INIT        = 0x13;
  static const uint8_t LISTEN      = 0x14;
  static const uint8_t SYNSENT     = 0x15;
  static const uint8_t SYNRECV     = 0x16;
  static const uint8_t ESTABLISHED = 0x17;
  static const uint8_t FIN_WAIT    = 0x18;
  static const uint8_t CLOSING     = 0x1A;
  static const uint8_t TIME_WAIT   = 0x1B;
  static const uint8_t CLOSE_WAIT  = 0x1C;
  static const uint8_t LAST_ACK    = 0x1D;
  static const uint8_t UDP         = 0x22;
  static const uint8_t IPRAW       = 0x32;
  static const uint8_t MACRAW      = 0x42;
  static const uint8_t PPPOE       = 0x5F;

uint8_t dA[]={ 10, 0, 0, 51};


void SPI_beginTransaction(){};
void SPI_endTransaction(){};

void SPI_Init(){
     // noInterrupts();
      SPCR |=(1<<SPE)|(1<<MSTR);
      SPSR = (0<<SPI2X);
      pinMode(SCK, OUTPUT);
      pinMode(MOSI, OUTPUT);

      pinMode(MISO, INPUT);
      pinMode(SS_PIN, OUTPUT);
      pinMode(53, OUTPUT);
    /*   digitalWrite(SCK, HIGH);
        digitalWrite(MISO, HIGH);
         digitalWrite(MOSI, HIGH);*/
      digitalWrite(SS_PIN, HIGH);
      digitalWrite(53, HIGH);
     //initSS();  
  }
void a(){};
uint8_t SPItransfer(uint8_t data) {
 // SPI_Init();
    SPDR = data;
    asm volatile("nop");
    while (!(SPSR & _BV(SPIF))) ; // wait
   
    return SPDR;
  }


  static const uint8_t  RST = 7; // Reset BIT
  static const int SOCKETS = 4;
  static const uint16_t SMASK = 0x07FF; // Tx buffer MASK
  static const uint16_t RMASK = 0x07FF; // Rx buffer MASK
  static const uint16_t SSIZE = 2048; // Max Tx buffer size
  static const uint16_t RSIZE = 2048; // Max Rx buffer size
  uint16_t SBASE[SOCKETS]; // Tx buffer base address
  uint16_t RBASE[SOCKETS]; // Rx buffer base address

extern void resetEthernet(void);  
  uint8_t W5100write(uint16_t _addr, uint8_t _data)
{

noInterrupts();
 digitalWrite(SS_PIN, LOW);
  SPItransfer(0xF0);
  SPItransfer(_addr >> 8); 
  SPItransfer(_addr & 0xFF);
  uint8_t sym = SPItransfer(_data);
  if(sym!=3){  
    Serial.println("RESET FROM W5100 WRITE");
    resetEthernet();
    }

  //resetSS();

  digitalWrite(SS_PIN, HIGH);
interrupts();
  return 1;
}

uint8_t W5100read(uint16_t _addr)
{

 // setSS();  
  digitalWrite(SS_PIN, LOW);
  SPItransfer(0x0F);
  SPItransfer(_addr >> 8);
  SPItransfer(_addr & 0xFF);
  uint8_t _data = SPItransfer(0);
  //resetSS();
  digitalWrite(SS_PIN, HIGH);

  return _data;
}

uint16_t W5100readBlock(uint16_t _addr, uint8_t *_buf, uint16_t _len)
{
  for (uint16_t i=0; i<_len; i++)
  {

    digitalWrite(SS_PIN, LOW);
    SPItransfer(0x0F);
    SPItransfer(_addr >> 8);
    SPItransfer(_addr & 0xFF);
    _addr++;
    _buf[i] = SPItransfer(0);
    digitalWrite(SS_PIN, HIGH);
  }
  return _len;
}

uint16_t W5100writeBlock(uint16_t _addr, const uint8_t *_buf, uint16_t _len)
{
  for (uint16_t i=0; i<_len; i++)
  {
    digitalWrite(SS_PIN, LOW);  
    SPItransfer(0xF0);
    SPItransfer(_addr >> 8);
    SPItransfer(_addr & 0xFF);
    _addr++;
    SPItransfer(_buf[i]);
    digitalWrite(SS_PIN, HIGH);
  }
  return _len;
}


uint8_t W5100_SetMac(uint16_t oct1, uint16_t oct2, uint16_t oct3, uint16_t oct4, uint16_t oct5, uint16_t oct6)
{
   W5100write(0x09, oct1);
   W5100write(0x0A, oct2);
   W5100write(0x0B, oct3);
   W5100write(0x0C, oct4);
   W5100write(0x0D, oct5);
   W5100write(0x0E, oct6);

  return 1;
}

uint8_t W5100_SetIP(uint16_t oct1, uint16_t oct2, uint16_t oct3, uint16_t oct4)
{
   W5100write(0x0F, oct1);
   W5100write(0x10, oct2);
   W5100write(0x11, oct3);
   W5100write(0x12, oct4);
  return 1;
}
uint8_t W5100_SetGateway(uint16_t oct1, uint16_t oct2, uint16_t oct3, uint16_t oct4)
{
   W5100write(0x01, oct1);
   W5100write(0x02, oct2);
   W5100write(0x03, oct3);
   W5100write(0x04, oct4);
  return 1;
}
uint8_t W5100_SetMask(uint16_t oct1, uint16_t oct2, uint16_t oct3, uint16_t oct4)
{
   W5100write(0x05, oct1);
   W5100write(0x06, oct2);
   W5100write(0x07, oct3);
   W5100write(0x08, oct4);
  return 1;
}

uint8_t W5100_Init(){
  SPI_Init();
  W5100write(0, 0x80);
  W5100write(0x1B, 0x55);
  W5100write(0x1A, 0x55);
    for (int i=0; i<MAX_SOCK_NUM; i++) {
    SBASE[i] = TXBUF_BASE + SSIZE * i;
    RBASE[i] = RXBUF_BASE + RSIZE * i;
  }
  return 1;  
}


  static uint8_t write(uint16_t _addr, uint8_t _data);
  static uint16_t write(uint16_t addr, const uint8_t *buf, uint16_t len);
  static uint8_t read(uint16_t addr);
  static uint16_t read(uint16_t addr, uint8_t *buf, uint16_t len);
  
#define __GP_REGISTER8(name, address)             \
  static inline void write##name(uint8_t _data) { \
    write(address, _data);                        \
  }                                               \
  static inline uint8_t read##name() {            \
    return read(address);                         \
  }
#define __GP_REGISTER16(name, address)            \
  static void write##name(uint16_t _data) {       \
    write(address,   _data >> 8);                 \
    write(address+1, _data & 0xFF);               \
  }                                               \
  static uint16_t read##name() {                  \
    uint16_t res = read(address);                 \
    res = (res << 8) + read(address + 1);         \
    return res;                                   \
  }
#define __GP_REGISTER_N(name, address, size)      \
  static uint16_t write##name(uint8_t *_buff) {   \
    return write(address, _buff, size);           \
  }                                               \
  static uint16_t read##name(uint8_t *_buff) {    \
    return read(address, _buff, size);            \
  }


  __GP_REGISTER8 (MR,     0x0000);    // Mode
  __GP_REGISTER_N(GAR,    0x0001, 4); // Gateway IP address
  __GP_REGISTER_N(SUBR,   0x0005, 4); // Subnet mask address
  __GP_REGISTER_N(SHAR,   0x0009, 6); // Source MAC address
  __GP_REGISTER_N(SIPR,   0x000F, 4); // Source IP address
  __GP_REGISTER8 (IR,     0x0015);    // Interrupt
  __GP_REGISTER8 (IMR,    0x0016);    // Interrupt Mask
  __GP_REGISTER16(RTR,    0x0017);    // Timeout address
  __GP_REGISTER8 (RCR,    0x0019);    // Retry count
  __GP_REGISTER8 (RMSR,   0x001A);    // Receive memory size
  __GP_REGISTER8 (TMSR,   0x001B);    // Transmit memory size
  __GP_REGISTER8 (PATR,   0x001C);    // Authentication type address in PPPoE mode
  __GP_REGISTER8 (PTIMER, 0x0028);    // PPP LCP Request Timer
  __GP_REGISTER8 (PMAGIC, 0x0029);    // PPP LCP Magic Number
  __GP_REGISTER_N(UIPR,   0x002A, 4); // Unreachable IP address in UDP mode
  __GP_REGISTER16(UPORT,  0x002E);    // Unreachable Port address in UDP mode
  
#undef __GP_REGISTER8
#undef __GP_REGISTER16
#undef __GP_REGISTER_N

  // W5100 Socket registers
  // ----------------------

   uint8_t readSn(uint8_t _s, uint16_t _addr);
   uint8_t writeSn(uint8_t _s, uint16_t _addr, uint8_t _data);
   uint16_t readSn(uint8_t _s, uint16_t _addr, uint8_t *_buf, uint16_t len);
   uint16_t writeSn(uint8_t _s, uint16_t _addr, uint8_t *_buf, uint16_t len);

  static const uint16_t CH_BASE = 0x0400;
  static const uint16_t CH_SIZE = 0x0100;

#define __SOCKET_REGISTER8(name, address)                    \
  static inline void write##name(uint8_t _s, uint8_t _data) { \
    writeSn(_s, address, _data);                             \
  }                                                          \
  static inline uint8_t read##name(uint8_t _s) {              \
    return readSn(_s, address);                              \
  }
#define __SOCKET_REGISTER16(name, address)                   \
  static void write##name(uint8_t _s, uint16_t _data) {       \
    writeSn(_s, address,   _data >> 8);                      \
    writeSn(_s, address+1, _data & 0xFF);                    \
  }                                                          \
  static uint16_t read##name(uint8_t _s) {                    \
    uint16_t res = readSn(_s, address);                      \
    uint16_t res2 = readSn(_s,address + 1);                  \
    res = res << 8;                                          \
    res2 = res2 & 0xFF;                                      \
    res = res | res2;                                        \
    return res;                                              \
  }
#define __SOCKET_REGISTER_N(name, address, size)             \
  static uint16_t write##name(uint8_t _s, uint8_t *_buff) {   \
    return writeSn(_s, address, _buff, size);                \
  }                                                          \
  static uint16_t read##name(uint8_t _s, uint8_t *_buff) {    \
    return readSn(_s, address, _buff, size);                 \
  }
  

  __SOCKET_REGISTER8(SnMR,        0x0000)        // Mode
  __SOCKET_REGISTER8(SnCR,        0x0001)        // Command
  __SOCKET_REGISTER8(SnIR,        0x0002)        // Interrupt
  __SOCKET_REGISTER8(SnSR,        0x0003)        // Status
  __SOCKET_REGISTER16(SnPORT,     0x0004)        // Source Port
  __SOCKET_REGISTER_N(SnDHAR,     0x0006, 6)     // Destination Hardw Addr
  __SOCKET_REGISTER_N(SnDIPR,     0x000C, 4)     // Destination IP Addr
  __SOCKET_REGISTER16(SnDPORT,    0x0010)        // Destination Port
  __SOCKET_REGISTER16(SnMSSR,     0x0012)        // Max Segment Size
  __SOCKET_REGISTER8(SnPROTO,     0x0014)        // Protocol in IP RAW Mode
  __SOCKET_REGISTER8(SnTOS,       0x0015)        // IP TOS
  __SOCKET_REGISTER8(SnTTL,       0x0016)        // IP TTL
  __SOCKET_REGISTER16(SnTX_FSR,   0x0020)        // TX Free Size
  __SOCKET_REGISTER16(SnTX_RD,    0x0022)        // TX Read Pointer
  __SOCKET_REGISTER16(SnTX_WR,    0x0024)        // TX Write Pointer
  __SOCKET_REGISTER16(SnRX_RSR,   0x0026)        // RX Free Size
  __SOCKET_REGISTER16(SnRX_RD,    0x0028)        // RX Read Pointer
  __SOCKET_REGISTER16(SnRX_WR,    0x002A)        // RX Write Pointer (supported?)
  
#undef __SOCKET_REGISTER8
#undef __SOCKET_REGISTER16
#undef __SOCKET_REGISTER_N



//----------IP

  


void W5100_writeS0DPORT(uint16_t port){
   W5100write(0x410, (port&(0xFF00))>>8);
   W5100write(0x411, (port&(0x00FF)));
};
void W5100_writeS0DIPR(uint8_t* addr){
  W5100write(0x40C, addr[0]);
  W5100write(0x40D, addr[1]);
  W5100write(0x40E, addr[2]);
  W5100write(0x40F, addr[3]);
};

void W5100_writeS1DPORT(uint16_t port){
   W5100write(0x510, (port&(0xFF00))>>8);
   W5100write(0x511, (port&(0x00FF)));
};
void W5100_writeS1DIPR(uint8_t* addr){
  W5100write(0x50C, addr[0]);
  W5100write(0x50D, addr[1]);
  W5100write(0x50E, addr[2]);
  W5100write(0x50F, addr[3]);
};

void W5100_writeS2DPORT(uint16_t port){
   W5100write(0x610, (port&(0xFF00))>>8);
   W5100write(0x611, (port&(0x00FF)));
};
void W5100_writeS2DIPR(uint8_t* addr){
  W5100write(0x60C, addr[0]);
  W5100write(0x60D, addr[1]);
  W5100write(0x60E, addr[2]);
  W5100write(0x60F, addr[3]);
};

uint8_t readSn(uint8_t _s, uint16_t _addr) {
  return W5100read(CH_BASE + _s * CH_SIZE + _addr);
}

uint8_t writeSn(uint8_t _s, uint16_t _addr, uint8_t _data) {
  return W5100write(CH_BASE + _s * CH_SIZE + _addr, _data);
}

uint16_t readSn(uint8_t _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
  return W5100readBlock(CH_BASE + _s * CH_SIZE + _addr, _buf, _len);
}

uint16_t writeSn(uint8_t _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
  return W5100writeBlock(CH_BASE + _s * CH_SIZE + _addr, _buf, _len);
}

extern void resetEthernet(void);

uint16_t W5100_getTXFreeSize(uint8_t s)
{
  uint16_t val=0, val1=0;
  uint16_t i;
  do {
	  i++;
    val1 = readSnTX_FSR(s);
    if (val1 != 0)
      val = readSnTX_FSR(s);
  } 
  while ((val != val1)&&(i<65535));
  if(i==65535){
    resetEthernet();
    }
  return val;
};

uint16_t W5100_getRXReceivedSize(uint8_t s)
{
  uint16_t val=0,val1=0;
  uint16_t i;
  do {
	  i++;
    val1 = readSnRX_RSR(s);
    if (val1 != 0)
      val = readSnRX_RSR(s);
  } 
  while ((val != val1)&&(i<65535));
    if(i==65535){
    resetEthernet();
    }
  return val;
}
void W5100_send_data_processing_offset(uint8_t s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
  uint16_t ptr = readSnTX_WR(s);
  ptr += data_offset;
  uint16_t offset = ptr & SMASK;
  uint16_t dstAddr = offset + SBASE[s];

  if (offset + len > SSIZE) 
  {
    // Wrap around circular buffer
    uint16_t size = SSIZE - offset;
    W5100writeBlock(dstAddr, data, size);
    W5100writeBlock(SBASE[s], data + size, len - size);
  } 
  else {
    W5100writeBlock(dstAddr, data, len);
  }

  ptr += len;
  writeSnTX_WR(s, ptr);
}
uint16_t bufferData(uint8_t s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
  uint16_t ret =0;
  SPI_beginTransaction();
  if (len > W5100_getTXFreeSize(s))
  {
    ret = W5100_getTXFreeSize(s); // check size not to exceed MAX size.
  }
  else
  {
    ret = len;
  }
  W5100_send_data_processing_offset(s, offset, buf, ret);
  SPI_endTransaction();
  return ret;
}

void printByte(char * buf, uint8_t n ) {
  char *str = &buf[1];
  buf[0]='0';
  do {
    unsigned long m = n;
    n /= 16;
    char c = m - 16 * n;
    *str-- = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);
};

uint8_t   Sock_OPEN      = 0x01;
uint8_t   Sock_LISTEN    = 0x02;
uint8_t   Sock_CONNECT   = 0x04;
uint8_t   Sock_DISCON    = 0x08;
uint8_t   Sock_CLOSE     = 0x10;
uint8_t   Sock_SEND      = 0x20;
uint8_t   Sock_SEND_MAC  = 0x21;
uint8_t   Sock_SEND_KEEP = 0x22;
uint8_t   Sock_RECV      = 0x40;



void execCmdSn(uint8_t s, uint8_t _cmd) {
  // Send command to socket
  writeSnCR(s, _cmd);
  // Wait for command to complete
  uint8_t i=0;
  while (readSnCR(s)&&(i<255)){i++;}
    ;
      if(i==255){
    resetEthernet();
    }
    
}
  static const uint8_t SEND_OK = 0x10;
  static const uint8_t TIMEOUT = 0x08;
  static const uint8_t RECV    = 0x04;
  static const uint8_t DISCON  = 0x02;
  static const uint8_t CON     = 0x01;



uint8_t execCmdS0_Automat(uint8_t event,uint8_t _cmd)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef execCmdSocket0_LOGGING
        Serial.print("    execCmdSocket0_automat state:") ;
        Serial.print(state) ;
        Serial.print(" Command: ");
        Serial.println(_cmd);
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
          writeSnCR(0, _cmd);
          if(readSnCR(0)){
            state=2;
            }  
          else{
              state=255;
            }
      break;
      case 2:
           if(readSnCR(0)){
              state=2;
            }  
            else{
              state=255;
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
            break;
            case 255:
            break;
          }
      } 
      return state;
  
  }

uint8_t execCmdS1_Automat(uint8_t event,uint8_t _cmd)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef execCmdSocket10_LOGGING
        Serial.print("    execCmdSocket1_automat state:") ;
        Serial.print(state) ;
        Serial.print(" Command: ");
        Serial.println(_cmd);
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
          writeSnCR(1, _cmd);
          if(readSnCR(1)){
              state=2;
            }  
          else{
              state=255;
            }
      break;
      case 2:
           if(readSnCR(1)){
              state=2;
            }  
            else{
              state=255;
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
            break;
            case 255:
            break;
          }
      } 
      return state;
  }

uint8_t execCmdS2_Automat(uint8_t event,uint8_t _cmd)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef execCmdSocket2_LOGGING
        Serial.print("    execCmdSocket2_automat start state:") ;
        Serial.print(state) ;
        Serial.print(" Command: ");
        Serial.println(_cmd);
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
  
          writeSnCR(2, _cmd);
          if(readSnCR(2)){
              state=2;
            }  
          else{
              state=255;
            }
      break;
      case 2:
           if(readSnCR(2)){
              state=2;
            }  
            else{
              state=255;
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
            break;
            case 255:
            break;
          }
      } 

          #ifdef execCmdSocket2_LOGGING
        Serial.print("    execCmdSocket2_automat end state:") ;
        Serial.print(state) ;
        Serial.print(" Command: ");
        Serial.println(_cmd);
    #endif
      return state;
  }

uint8_t ResetW5100_automat(uint8_t event)
{
   static uint8_t state=0;
   uint8_t stateOld=state;
    #ifdef ResetW5100_Automat_BEGIN_LOGGING
        Serial.print("ResetW5100_Automat ") ;
        Serial.print("state:") ;
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
            break;
            case 255:
            break;
          }
      } 
      return state;
  }

void read_data(uint8_t s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len)
{
  uint16_t size;
  uint16_t src_mask;
  uint16_t src_ptr;

  src_mask = src & RMASK;
  src_ptr = RBASE[s] + src_mask;

  if( (src_mask + len) > RSIZE ) 
  {
    size = RSIZE - src_mask;
    W5100readBlock(src_ptr, (uint8_t *)dst, size);
    dst += size;
    W5100readBlock(RBASE[s], (uint8_t *) dst, len - size);
  } 
  else
    W5100readBlock(src_ptr, (uint8_t *) dst, len);
}

void W5100_recv_data_processing(uint8_t s, uint8_t *data, uint16_t len, uint8_t peek=0)
{
  uint16_t ptr;
  ptr = readSnRX_RD(s);
  read_data(s, ptr, data, len);
  if (!peek)
  {
    ptr += len;
    writeSnRX_RD(s, ptr);
  }
}

  int16_t recv(uint8_t s, uint8_t *buf, int16_t len)
{
  // Check how much data is available
  SPI_beginTransaction();
  int16_t ret = W5100_getRXReceivedSize(s);
  if ( ret == 0 )
  {
    // No data available.
    uint8_t status = readSnSR(s);
    if ( status == LISTEN || status == CLOSED || status == CLOSE_WAIT )
    {
      // The remote end has closed its side of the connection, so this is the eof state
      ret = 0;
    }
    else
    {
      // The connection is still up, but there's no data waiting to be read
      ret = -1;
    }
  }
  else if (ret > len)
  {
    ret = len;
  }

  if ( ret > 0 )
  {
    W5100_recv_data_processing(s, buf, ret);
    execCmdSn(s, Sock_RECV);
  }
  SPI_endTransaction();
  return ret;
}

int16_t recvAvailable(uint8_t s)
{
  SPI_beginTransaction();
  int16_t ret = W5100_getRXReceivedSize(s);
  SPI_endTransaction();
  return ret;
}


#endif
