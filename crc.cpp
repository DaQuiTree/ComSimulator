#include <QtGlobal>
#include "crc.h"

/* 联合体声明&变量声明 */

union crc
{
    unsigned short word;
	struct 
	{
        unsigned char	lo;
        unsigned char	hi;
	}bytes;
};

union crc crc_chec;

/**
  * @func   void crc_chec_fuc(unsigned char datain);
  * @brief  CRC16校验子函数
  * @param  datain:
  * @retval 无
  */

static void crc_chec_fuc(unsigned char datain)
{
  unsigned char i;

  crc_chec.bytes.lo=crc_chec.bytes.lo^datain;
  for(i=0;i<8;i++)
  {
    if((crc_chec.word & 0x0001)==1)
    {
      crc_chec.word=crc_chec.word>>1;
      crc_chec.word=crc_chec.word^0xa001;
    }
    else
      crc_chec.word=crc_chec.word>>1;
  }
}

/**
  * @func   unsigned short generic_crc(unsigned short num, unsigned char *pbuf);
  * @brief  CRC16校验接口函数
  * @param  num: 校验总数
  *        pbuf: 需要校验的数组
  * @retval CRC16的校验和(word)
  */

unsigned short generic_crc(unsigned short num, unsigned char *pbuf)
{
  unsigned short i;
  unsigned short crc_chec_retval = 0x0000;
	
  crc_chec.word=0xffff;
  for(i=0;i<num;i++)
  {
     crc_chec_fuc(pbuf[i]);
  }
  
  crc_chec_retval = (unsigned short)crc_chec.bytes.hi;
  crc_chec_retval <<= 8;
  crc_chec_retval |= (unsigned short)crc_chec.bytes.lo;
  
  return crc_chec_retval;
}
