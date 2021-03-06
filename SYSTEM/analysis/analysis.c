#include "stdio.h"
#include "string.h"
#include "analysis.h"
#include "sys.h"
#include "includes.h"
#include "command.h"
#include "delay.h"

volatile u8 DataBiteInfo=0;
u8 Hand_Data[60];
static u8 num0=0;
Core_data my_core_data;


int normalMonthDays[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};  


int dateDiff(Mydatastrcut mindate,Mydatastrcut maxdate)
{
	int days=0,j,flag;
	const int primeMonth[][12]={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}};	
	Mydatastrcut tmp;
	if ((mindate.MyYear>maxdate.MyYear)|| (mindate.MyYear==maxdate.MyYear&&mindate.MyMonth>maxdate.MyMonth)||(mindate.MyYear==maxdate.MyYear&&mindate.MyMonth==maxdate.MyMonth&&mindate.MyData>maxdate.MyData))
	{
		tmp=mindate;
		mindate=maxdate;
		maxdate=tmp;
	} 
	for(j=mindate.MyYear;j<maxdate.MyYear;++j)
		days+=isPrime(j)?366:365;
	
	flag=isPrime(maxdate.MyYear);
	for (j=1;j<maxdate.MyMonth;j++)
		days+=primeMonth[flag][j-1];
    flag=isPrime(maxdate.MyYear);
    for (j=1;j<mindate.MyMonth;j++)
          days-=primeMonth[flag][j-1];
    days=days+maxdate.MyData-mindate.MyData;
    return days;
} 

int leapYear(int year)  
{
    if(year %4 ==0 && year%100 !=0 || year %400 ==0) return 1;  
    else return 0;  
}  
Mydatastrcut getNewDate(Mydatastrcut initDate, int diffDays)  
{  
    Mydatastrcut reDate = initDate;  
  
    int daysAyear = 365;  
    if(leapYear(reDate.MyYear))  
    {
        daysAyear = 366;  
    }  
    while(diffDays/daysAyear)  
    {  
        diffDays = diffDays - daysAyear;  
        reDate.MyYear ++;  
        if(leapYear(reDate.MyYear))  
        {  
            daysAyear = 366;  
        }  
    }  
  
    if(leapYear(reDate.MyYear))  
    {  
        normalMonthDays[2]=29;  
    }  
    while(diffDays/normalMonthDays[reDate.MyMonth])  
    {  
        diffDays = diffDays - normalMonthDays[reDate.MyMonth];  
        reDate.MyMonth++;  
        if(reDate.MyMonth >= 13)  
        {  
            reDate.MyYear++;  
            if(leapYear(reDate.MyYear))  
            {  
                normalMonthDays[2]=29;  
            }  
            reDate.MyMonth = reDate.MyMonth%12;  
        }  
    }  
    if(leapYear(reDate.MyYear))  
    {  
        normalMonthDays[2]=29;  
    }  
    if(diffDays + reDate.MyData <= normalMonthDays[reDate.MyMonth])  
        reDate.MyData = diffDays + reDate.MyData;  
    else  
    {  
        reDate.MyData = diffDays + reDate.MyData - normalMonthDays[reDate.MyMonth];  
        reDate.MyMonth++;  
        if(reDate.MyMonth > 12)  
        {  
            reDate.MyYear++;  
            reDate.MyMonth = reDate.MyMonth%12;   
        }     
    }  
  
    return reDate;  
}  




/*从字符串的中间截取n个字符*/
char * mid(char *dst,char *src, int n,int m) /*n为长度，m为位置*/
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len-m;    /*从第m个到最后*/
    if(m<0) m=0;    /*从第一个开始*/
    if(m>len) return NULL;
    p += m;
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; /*有必要吗？很有必要*/
    return dst;
}


u8 Dec2Hex(u8 num)
{
	u8 numbuf[2]={0};
	u8 hex;
	sprintf((char*)numbuf,"%02d",num);
	hex=strtol((const char*)numbuf,NULL,16);
	return hex;
}

u8 Hex2Dec(u8 num)
{
	u8 numbuf[2]={0};
	u8 dec;
	sprintf((char*)numbuf,"%02x",num);
	dec=strtol((const char*)numbuf,NULL,10);
	return dec;
}



void DataCore(u8 *databuf)
{
	u8 i=0;
	if(databuf[0]==0x7e&&databuf[1]==0x7e)
		{
			if(databuf[4]==0x00)DataBiteInfo=1;
		}
}

/////////////////////////////////////////////////////////////////////////////////////


u16 SXProtoco_CRC16(unsigned char *puchMsg, unsigned short usDataLen)
{
	unsigned char uchCRCHi = 0xFF ;
	unsigned char uchCRCLo = 0xFF ; 
	return (uchCRCLo << 8 | uchCRCHi) ;
}



void Sync_HandData(u8 *len)
{
	u16 crcdata;
	u8 length=0x02;
	Hand_Data[0]=0x7e;
	Hand_Data[1]=0x7e;
	Hand_Data[2]=++num0;
	Hand_Data[3]=length;
	Hand_Data[4]=0x53;
	Hand_Data[5]=systemset.Addrnum;
	crcdata=SXProtoco_CRC16(Hand_Data,6);
	Hand_Data[6]=(crcdata>>8);
	Hand_Data[7]=(crcdata&0xff);
	*len=8;
}


void Command_HandData(u8 *len)
{
	u8 i;
	u16 crcdata;
	u8 length=0x02;
	Hand_Data[0]=0x7e;
	Hand_Data[1]=0x7e;
	Hand_Data[2]=++num0;
	Hand_Data[3]=length;
	Hand_Data[4]=0x50;
	Hand_Data[5]=my_core_data.speed;
	Hand_Data[6]=my_core_data.fangxiang;
	Hand_Data[7]=my_core_data.frontled;
	Hand_Data[8]=my_core_data.bhandled;
	Hand_Data[9]=my_core_data.waningled;
	Hand_Data[10]=my_core_data.maxsepeed;
	Hand_Data[11]=0;
	for(i=0;i<6;i++)Hand_Data[12+i]=my_core_data.back[i];
	crcdata=SXProtoco_CRC16(Hand_Data,18);
	Hand_Data[18]=(crcdata>>8);
	Hand_Data[19]=(crcdata&0xff);
	*len=20;
	
}


void ComRoad_HandData(u8 *len,u8 statu)
{
	u16 crcdata;
	u8 length=0x02;
	Hand_Data[0]=0x7e;
	Hand_Data[1]=0x7e;
	Hand_Data[2]=++num0;
	Hand_Data[3]=length;
	Hand_Data[4]=0x43;
	Hand_Data[5]=statu;
	crcdata=SXProtoco_CRC16(Hand_Data,6);
	Hand_Data[6]=(crcdata>>8);
	Hand_Data[7]=(crcdata&0xff);
	*len=8;
}


void Sync_HandData1(u8 *len)
{
	u8 i,j;
	u8 lentem;
	u8 length=0x03;
	u16 crcdata;
	Hand_Data[0]=0x7e;
	Hand_Data[1]=0x7e;
	Hand_Data[2]=++num0;
	Hand_Data[4]=0x44;
	lentem=strlen((char*)systemset.UserName);
	length+=lentem;
	for(i=0;i<lentem;i++)Hand_Data[5+i]=systemset.UserName[i];
	Hand_Data[5+i]=0x2C;
	lentem=strlen((char*)systemset.Passwd);
	length+=lentem;
	for(j=0;j<lentem;j++)Hand_Data[6+i+j]=systemset.Passwd[j];
	Hand_Data[3]=length;
	Hand_Data[6+i+j]=0x2C;
	crcdata=SXProtoco_CRC16(Hand_Data,7+i+j);
	Hand_Data[7+i+j]=(crcdata>>8);
	Hand_Data[8+i+j]=(crcdata&0xff);
	*len=9+i+j;
}

void Sync_HandData2(u8 *len)
{
	u16 crcdata;
	u8 length=0x02;
	Hand_Data[0]=0x7e;
	Hand_Data[1]=0x7e;
	Hand_Data[2]=++num0;
	Hand_Data[3]=length;
	Hand_Data[4]=0x53;
	Hand_Data[5]=systemset.Addrnum;
	Hand_Data[6]=0x01;
	Hand_Data[7]=0x01;
	Hand_Data[8]=0x01;
	Hand_Data[9]=0x01;
	crcdata=SXProtoco_CRC16(Hand_Data,10);
	Hand_Data[10]=(crcdata>>8);
	Hand_Data[11]=(crcdata&0xff);
	*len=12;
}

void Command_HandData2(u8 *len)
{
	u8 i;
	u8 length=0x10;
	u16 crcdata;
	Hand_Data[0]=0x7e;
	Hand_Data[1]=0x7e;
	Hand_Data[2]=++num0;
	Hand_Data[3]=length;
	Hand_Data[4]=0x50;
	Hand_Data[5]=my_core_data.speed;
	Hand_Data[6]=0x01;
	Hand_Data[7]=my_core_data.ledstatu;
	Hand_Data[8]=my_core_data.waningled;
	Hand_Data[9]=my_core_data.maxsepeed;
	Hand_Data[10]=0x01;
	Hand_Data[11]=my_core_data.voice;
	Hand_Data[12]=my_core_data.frontled;
	Hand_Data[13]=my_core_data.jiansepeed;
	for(i=0;i<6;i++)Hand_Data[14+i]=my_core_data.back[i];
	crcdata=SXProtoco_CRC16(Hand_Data,20);
	Hand_Data[20]=(crcdata>>8);
	Hand_Data[21]=(crcdata&0xff);
	*len=22;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



void RePayUP_CarData(u8 *len)
{
	u8 length=0x10;
	Hand_Data[0]=0x7e;
	Hand_Data[1]=0x7e;
	Hand_Data[2]=++num0;
	Hand_Data[3]=length;
	Hand_Data[4]=0x01;
	Hand_Data[5]=0x00;
	Hand_Data[6] = 0x0d;
	Hand_Data[7] = 0x0a;
	*len=8;
}



void ReReg_CarData(u8 *len)
{
	u8 length=0x10;
	Hand_Data[0]=0x7e;
	Hand_Data[1]=0x7e;
	Hand_Data[2]=++num0;
	Hand_Data[3]=length;
	Hand_Data[4]=0x52;
	Hand_Data[5]=0x00;
	Hand_Data[6] = 0x0d;
	Hand_Data[7] = 0x0a;
	*len=8;
}

SYS_PARA_T	sys_data;

void change_rail(u8 flag)
{
	
}


void dispose_wifi_cmd(u8 *pRecStr)
{
	u8 i=0;
	if((pRecStr[0] == 0x7e)&&(pRecStr[1] == 0x7e))
		{	
			if(pRecStr[4] == 0x43)
				{
					change_rail(pRecStr[5]);
				}
			if(pRecStr[4] == 0x44)
				{
					mymemset(systemset.UserName,0,sizeof(systemset.UserName));
					mymemset(systemset.Passwd,0,sizeof(systemset.Passwd));
					while(pRecStr[5+i]!=0x2c)
						{
							systemset.UserName[i]=pRecStr[5+i];
							i++;
						}
					i++;
					while(pRecStr[5+i]!=0x2c)
						{
							systemset.Passwd[i]=pRecStr[5+i];
							i++;
						}
					DataBiteInfo|=1<<0;
				}
			if(pRecStr[4] == 0x50)
				{
					sys_data.speed = pRecStr[5];
					sys_data.run_direction = pRecStr[6];
					sys_data.car_max_speed_rate =  pRecStr[9];
					sys_data.cruve_slowdown_switch =  pRecStr[12];
					sys_data.cruve_slowdown_rate =  pRecStr[13];
					sys_data.speed_buf[sys_data.spedd_counter]=sys_data.speed;
					sys_data.spedd_counter++;
					if(sys_data.spedd_counter==20)
						{
							DataBiteInfo|=1<<2;
							sys_data.spedd_counter=0;
						}
					
				}
			if(pRecStr[4] == 0x53)
				{
					DataBiteInfo|=1<<1;
				}
		}
}


u8 SaveUsnamePswd(u8 outtime)
{
	u8 res=0;
	while(--outtime)	
		{
			delay_ms(10);
			if(DataBiteInfo&0x01)break;
		}
	DataBiteInfo&=~(1<<0);
	if(outtime==0)res=1;
	return res;
}


u8 RegSucess(u8 outtime)
{
	u8 res=0;
	while(--outtime)	
		{
			delay_ms(10);
			if(DataBiteInfo&0x02)break;
		}
	DataBiteInfo&=~(1<<1);
	if(outtime==0)res=1;
	return res;
}

void CheckSpeedBit(void)
{
	u8 i=0;
	if(DataBiteInfo&0x02)
		{
			for(i=0;i<20;i++)
				{
					if(sys_data.speed_buf[i]>5)
						{
							if(i==19)
								{
									systemset.SysSpeedBit=1;
									sysset_save_para(&systemset);
								}
						}
					else
						{
							break;
						}
				}
			for(i=0;i<20;i++)
				{
					if(sys_data.speed_buf[i]<=5)
						{
							if(i==19)
								{
									systemset.SysSpeedBit=0;
									sysset_save_para(&systemset);
								}
						}
					else
						{
							break;
						}
				}
			
		}
}


