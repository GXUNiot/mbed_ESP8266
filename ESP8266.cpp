#include <ESP8266.h>

ESP8266::ESP8266(PinName rx, PinName tx, PinName rst): Uart(rx,tx,115200), Rst(rst)
{
  if( rst == NC )
    ResetMode = Soft;
  else
    ResetMode = Hard;
  
  Reset();
  Uart.attach(callback(this,&ESP8266::UartIRQ));
  
}


void ESP8266::Send(uint8_t *buf)
{
  uint8_t len = strlen((const char*)buf);
  for(uint8_t i=0; i<len; i++)
    Uart.putc((int)*(buf+i));
}

void ESP8266::Send(uint8_t *buf, uint16_t len)
{
  for(uint8_t i=0; i<len; i++)
    Uart.putc((int)*(buf+i));
}

uint8_t ESP8266::SendCmd(const char *data, const char *rec, uint16_t timeout)
{
  Clear();
  Send((uint8_t*)data);

  timeout /= 100;

  while(timeout--)
  {
    if( WaitRecive() == OK )
    {
      if( strstr((const char*)RecBuf, (const char*)rec ) != NULL )
      {
        Clear();
        return OK;
      }
      Clear();
    }
    wait_ms(100);
  }
  return ERR;
}

uint8_t ESP8266::SendData(uint8_t *data, uint8_t *rec, uint16_t timeout)
{
  Clear();
  Send(data);

  timeout /= 100;

  while(timeout--)
  {
    if( WaitRecive() == OK && RecCnt!=0 )
    {
      memcpy(rec,RecBuf,( sizeof(rec)>RecCnt ? sizeof(rec) : RecCnt-1 ));
      
      Clear();
      return OK;
    }
    wait_ms(100);
  }
  return ERR;
}


uint8_t ESP8266::ConnectAP(const char *ssid, const char *pawd)
{
  uint8_t STEP = 5;
  Reset();

  if( SendCmd("AT\r\n", "OK", 5000) != OK )
    if( SendCmd("AT\r\n", "OK", 5000) != OK )
      if( SendCmd("AT\r\n", "OK", 5000) != OK )
        if( SendCmd("AT\r\n", "OK", 5000) != OK )
      return STEP;
  STEP--;
    
  

  if( SendCmd("AT+CWMODE=3\r\n", "OK", 5000) != OK )
    if( SendCmd("AT+CWMODE=3\r\n", "OK", 5000) != OK )
      return STEP;
  STEP--;

  if( SendCmd("AT+RST\r\n", "ready", 10000) != OK )
    if( SendCmd("AT+RST\r\n", "ready", 10000) != OK ) 
      return STEP;
      
  STEP--;

  char cmd[64];
  sprintf(cmd,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pawd);

  if( SendCmd(cmd, "GOT IP", 10000) != OK )
    if( SendCmd(cmd, "GOT IP", 10000) != OK )
      return STEP;
  STEP--;

  return STEP;
}

bool ESP8266::ConnectIP(const char* protocol, const char* IP, const char* port)
{
    char cmd_buf[48];
	  snprintf(cmd_buf, sizeof(cmd_buf), "AT+CIPSTART=\"%s\",\"%s\",%s\r\n", protocol, IP, port);
    return (SendCmd(cmd_buf,"CONNECT",5000) ? true : ( (SendCmd(cmd_buf,"CONNECT",5000) ? true : false) ));
}

void ESP8266::Reset(void)
{
  Clear();
  if( ResetMode == Soft )
    Send((uint8_t*)"+++");
  else
  {
    Rst = 0;
    wait_ms(10);
    Rst = 1;
  }
}

void ESP8266::Clear(void)
{
  memset(RecBuf, 0, sizeof(RecBuf));
	RecCnt = 0;
  RecCntPre = 0;
}

uint8_t ESP8266::WaitRecive(void)
{
  uint8_t State = ERR;

  if( RecCnt == 0 )
    State = OK;
  if( RecCnt == RecCntPre )
  {
    State = OK;
  }
  RecCntPre = RecCnt;

  return State;
}

uint8_t* ESP8266::GetRecive(void)
{
  memset(RecBuf,RecCnt,sizeof(RecBuf));
  return (uint8_t*)RecBuf;
}

void ESP8266::UartIRQ(void)
{
  if(Uart.readable())
  {
    if( RecCnt >= REC_BUF_MAXLEN )
      RecCnt = 0;
    RecBuf[RecCnt++] = (uint8_t)Uart.getc(); 
  }
}

unsigned char* ESP8266::GetIPD(unsigned short timeOut)
{
  char *ptrIPD = NULL;
	
	do
	{
		if(WaitRecive() == OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)RecBuf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		wait_ms(5);													//延时等待
	} while(timeOut--);
	
	return NULL;		
}
