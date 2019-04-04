/*****************************************
 * 文件名：   ESP8266.h
 * 
 * 说明：    
 * 
 * 
 * 作者：    汜方(15977877715@qq.com)
 * 修改时间：  2019-02-23
 *****************************************/

#ifndef _ESP8266_H
#define _ESP8266_H

/***************** 库 *****************/
#include <mbed.h>
#include "typedef.h"

/***************** 测试 *****************/
#ifdef TEST

extern Serial DEBUG;

#endif

/***************** 定义 *****************/


#define OK 1
#define ERR 0

#define REC_BUF_MAXLEN 128


/***************** 类 *****************/
class ESP8266
{

public:

  ESP8266(PinName rx, PinName tx, PinName rst=NC);

  void Send(uint8_t *buf);

  void Send(uint8_t *buf, uint16_t len);

  uint8_t SendCmd(const char *data, const char *rec, uint16_t timeout=2000);

  uint8_t SendData(uint8_t *data, uint8_t *rec, uint16_t timeout=2000);

  uint8_t ConnectAP(const char *ssid, const char *pawd);

  bool ConnectIP(const char* protocol, const char* IP, const char* port);

  void Reset(void);

  uint8_t WaitRecive(void);

  uint8_t* GetRecive(void);

  void Clear(void);

  unsigned char *GetIPD(unsigned short timeOut);

private:

  Serial Uart;
  DigitalOut Rst;

  enum ESP8266_RESET_MODE{Soft=0,Hard};
  ESP8266_RESET_MODE ResetMode;

  uint8_t RecBuf[REC_BUF_MAXLEN];
  uint16_t RecCnt,RecCntPre;


  void UartIRQ(void);
};


#endif //_ESP8266_H
