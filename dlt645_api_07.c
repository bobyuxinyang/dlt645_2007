#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dlt645_api_07.h"



#ifdef D07_DEBUG_ON



INT32 g_need_pack_07 = D07_OFF;
char g_out_data_07[OUT_BUF_LEN] = {0};

#endif

// 数据域长度
#define D07_DATA_LEN_METER_PROG         50
#define D07_DATA_LEN_RECD_METER_CLEAR   106
#define D07_DATA_LEN_RECD_DEAMD_CLEAR     202



/*************************************************
Function:       d07_str_to_num
Description:   将长度为len的string转为数字转出
Calls:          
Called By:      
Input:          

Output:        

Return:        
Others:     

*************************************************/
INT32 d07_str_to_num(char* str, INT32 len)
{
    char buf[10] = {0};
    INT32 i;

    if(len > 10)
    {
        return -1;
    }
    
    for(i = 0; i < len; i++)
    {
        buf[i] = str[i];        
    }
    
    return atoi(buf);
}

/*****************************************************************************
 函 数 名  : d07_hex_str_to_long
 功能描述  : 将长度为len的内容，当作十六进制字符串，将为长整数
 输入参数  : char*str  
             INT32 len   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月23日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
long d07_hex_str_to_long(char*str, INT32 len)
{
    long lRet = 0;
    INT32 i = 0;
    char buf[9]= {0};
    
    if(len > 8 || len <= 0)
    {
        return 0xFFFFFFFF;
    }
    
    for(i = 0; i < len; i++)
    {
        buf[i] = str[i];
    }

    lRet = strtol(buf,NULL, 16);
    return lRet;
}

void str2bcd(char *str,UINT8 *BCD,INT32 *BCD_length)
{
    INT32 i = 0,j = 0,sLen;
    INT32 tmp=strlen(str);

    if(str==0) 
    {
        return;
    }
    
    sLen = tmp;
    tmp-=tmp%2;

    for(i=0;i<tmp;i++)
     {
         if(str==0 ||
             !((str[i]>='0' && str[i]<='9' )|| (str[i]>='a' && str[i]<='f') || (str[i]>='A' && str[i]<='F')))
            
             return;
     }
     
   // for(i=0,j=0;i<tmp/2;i++,j+=2) 
    for(i=0,j=sLen-1;i<tmp/2;i++,j-=2)
     {
        // (str[j] > '9') ? (str[j]>'F' ? BCD[i]=str[j]-'a'+10 : BCD[i]=str[j]-'A'+10):( BCD[i]=str[j]-'0');
            if(str[j] > '9')   
            {
                if(str[j]>'F')
                {
                    BCD[i]=str[j]-'a'+10;
                }
                else
                {
                    BCD[i]=str[j]-'A'+10;
                }
            }
            else
            {
                BCD[i]=str[j]-'0';
            }
            
            if(str[j-1]>'9')
            {
                if(str[j+1]>'F')
                {
                    BCD[i]=(BCD[i]<<4)+str[j+1]-'a'+10;
                }
                else
                {
                     BCD[i]=(BCD[i]<<4)+str[j+1]-'A'+10;
                }
            }
            else
            {
                BCD[i]=(BCD[i]<<4)+str[j+1]-'0';
            }
       //   ?  (str[j+1]>'F' ? BCD[i]=(BCD[i]<<4)+str[j+1]-'a'+10 : BCD[i]=(BCD[i]<<4)+str[j+1]-'A'+10)
           // : BCD[i]=(BCD[i]<<4)+str[j+1]-'0';        
     }
     if(BCD_length)
         *BCD_length=tmp/2;
    return;    
}

/*************************************************
Function:       get_d07_ruler_id
Description:    通过规约类型获得对应规约ID

Author:         liming 

Calls:          
Called By:      
Input:           E_D07_RULER_TYPE type,
                 S_D07_RULER_PARA para,
                
Output:          UINT32 rulerID 规约ID
Return:         正确返回 0 其它为错误类型

Others:         重要函数，规约解析核心函数
  
*************************************************/
INT32 get_d07_ruler_id(E_D07_RULER_TYPE type,
                     S_D07_RULER_PARA para,
                     UINT32 *poutRulerID)
{


    return E_D07_OK;
}


/*************************************************
Function:       pack_d07_frame_by_data
Description:    内部函数，仅完成结构体的填充

Author:         liming 

Calls:          
Called By:      
Input:          S_D07_PACK_FRAME *inPara 用于封帧数据
                
Output:         char *outBuffer 封装后的帧buffer
                INT32  *outLength 该帧总长
Return:         正确返回0

Others:         
  
*************************************************/
INT32 pack_d07_frame_by_data(S_D07_PACK_FRAME *inPara, char *outBuffer, INT32 *outLength)
{
    INT32 i;
    INT32 len = 0; /* 计算包的总字节长 */
    UINT8 ucCheckSum = 0;
    UINT8 ucDi0,ucDi1,ucDi2,ucDi3;
    UINT8 aucAddrTmp[6]; 
    
    if(NULL == outBuffer || NULL == inPara ||  NULL == outLength)
    {
        return E_D07_ERRO_NULL;
    }
    // 准备数据 
    ucDi0 = inPara->ruler_id & 0xFF;
    ucDi1 = (inPara->ruler_id >> 8) & 0xFF;
    ucDi2 = (inPara->ruler_id >> 16) & 0xFF;
    ucDi3 = (inPara->ruler_id >> 24) & 0xFF;

    d07_str2bcd(inPara->address, aucAddrTmp, 6);

    /* 开始封帧 */
    // 1 帧起始符
    outBuffer[len++] = 0x68; 

    // 2 地址域 
    for(i = 0; i < 6; i++)
    {
        outBuffer[len++] = aucAddrTmp[i];
    }

    // 3 帧起始符
    outBuffer[len++] = 0x68; 

    // 4 控制码
    outBuffer[len++] = inPara->ctrl_code; 

    // 5 数据域字节数
    outBuffer[len++] = inPara->data_len;
    //printf("len = %d\n", inPara->data_len);

    // 6 规约ID  - 以下数据域 逐字节加0x33
    outBuffer[len++] = ucDi0 + 0x33;
    outBuffer[len++] = ucDi1 + 0x33;
    outBuffer[len++] = ucDi2 + 0x33;
    outBuffer[len++] = ucDi3 + 0x33;

  
    // 7 其它数据
    for(i = 0; i < inPara->data_len - 4; i++)
    {
       outBuffer[len++] = inPara->data[i] + 0x33;
    }

    // 8 计数检验和
    for(i = 0; i < len; i++)
    {
        ucCheckSum += outBuffer[i];
    }

    // 9 检验和
    outBuffer[len++] = ucCheckSum;

    // 10 结束符
    outBuffer[len++] = 0x16;

    /* 输出参数 */
    *outLength = len;

    return E_D07_OK;
}



/*************************************************
Function:       trans_d07ctrl_char2struct
Description:    解析将代表它的字节值转化控制码结构输出
                
Author:         liming 

Calls:          
Called By:      
Input:          S_D07_CTRL_CODE *inStruct

Output:         UINT8 *outChar
Return:      

Others:        反转换 trans_d07ctrl_struct2char
  
*************************************************/
eD07Err trans_d07ctrl_char2struct(UINT8 inChar,  S_D07_CTRL_CODE *outStruct)
{
    UINT8 ucCtrlCode = inChar;
    INT32 func_code = 0;

    if(NULL == outStruct)
    {
        return E_D07_ERRO_NULL;
    }

    if((ucCtrlCode & 0x80) == 0)
    {
        outStruct->direct = E_D07_CTRL_DIR_M2S;
    }
    else
    {
        outStruct->direct = E_D07_CTRL_DIR_S2M;
    }

    if((ucCtrlCode & 0x40) == 0)
    {
        outStruct->reply = E_D07_CTRL_SR_OK;
    }
    else
    {
        outStruct->reply = E_D07_CTRL_SR_NO;
    }

    if((ucCtrlCode & 0x20) == 0)
    {
        outStruct->follow = E_D07_CTRL_FLW_NONE;
    }
    else
    {
        outStruct->follow = E_D07_CTRL_FLW_HAVE;
    }

    func_code = ucCtrlCode & 0x1F;

    switch(func_code)
    {
        case D07_CTRL_RESV:
            outStruct->funcode = E_D07_CTRL_RESV;
            break;
            
        case D07_CTRL_SYNC_TIME:
            outStruct->funcode = E_D07_CTRL_SYNC_TIME;
            break;
            
        case D07_CTRL_READ_DATA:
            outStruct->funcode = E_D07_CTRL_READ_DATA;
            break;
            
        case D07_CTRL_READ_AFTERDATA:
            outStruct->funcode = E_D07_CTRL_READ_AFTERDATA;
            break;
            
        case D07_CTRL_READ_ADDR:
            outStruct->funcode = E_D07_CTRL_READ_ADDR;
            break;
            
        case D07_CTRL_WRITE_DATA:
            outStruct->funcode = E_D07_CTRL_WRITE_DATA;
            break;
            
        case D07_CTRL_WRITE_ADDR:
            outStruct->funcode = E_D07_CTRL_WRITE_ADDR;
            break;
            
        case D07_CTRL_FREEZ_COMM:
            outStruct->funcode = E_D07_CTRL_FREEZ_COMM;
            break;
        case D07_CTRL_MODIFY_BAUD:
            outStruct->funcode = E_D07_CTRL_MODIFY_BAUD;
            break;
            
        case D07_CTRL_MODIFY_PASSWORD:
            outStruct->funcode = E_D07_CTRL_MODIFY_PASSWORD;
            break;

            
        case D07_CTRL_CLEAR_MAXDEMAND:
            outStruct->funcode = E_D07_CTRL_CLEAR_MAXDEMAND;
            break;

                
        case D07_CTRL_CLEAR_METER:
            outStruct->funcode = E_D07_CTRL_CLEAR_METER;
            break;

                
        case D07_CTRL_CLEAR_EVENT:
            outStruct->funcode = E_D07_CTRL_CLEAR_EVENT;
            break;
        case D07_CTRL_COMM:
            outStruct->funcode = E_D07_CTRL_COMM;
            break;    
        default:
            return E_D07_ERRO_PARA_OUT_RANGE;
            
    }

    return E_D07_OK;
}


/*************************************************
Function:       trans_d07ctrl_struct2char
Description:    解析控制码结构 将代码它的字节值输出
                
Author:         liming 

Calls:          
Called By:      
Input:          S_D07_CTRL_CODE *inStruct

Output:         UINT8 *outChar
Return:      

Others:        反转换 trans_d07ctrl_char2struct
  
*************************************************/
eD07Err trans_d07ctrl_struct2char(UINT8 *outChar, S_D07_CTRL_CODE *inStruct)
{
    UINT8 ucCtrlCode = 0;
    UINT8 ucD7,ucD6,ucD5,ucD40;

    if(NULL == outChar || NULL == inStruct)
    {
        return E_D07_ERRO_NULL;
    }

    ucD7 = inStruct->direct;
    ucD6 = inStruct->reply;
    ucD5 = inStruct->follow;
    ucD40 = inStruct->funcode;

    ucCtrlCode |= ucD7 << 7;
    ucCtrlCode |= ucD6 << 6;
    ucCtrlCode |= ucD5 << 5;
    ucCtrlCode |= ucD40 & 0x1F;

    *outChar = ucCtrlCode;

    return E_D07_OK;
}


/*************************************************
Function:       unpack_d07_frame
Description:    解析DLT645 2007帧功能函数

Author:         liming 

Calls:          
Called By:      
Input:          inpBuffer      传入包有帧buffer指针
                inBufferLength 该buffer长度
                
Output:         outpFrame 该帧的各类详细信息

Return:         正确返回0

Others:         重要的功能函数
  
*************************************************/
INT32 unpack_d07_frame(void *inpBuffer, INT32 inBufferLength, S_D07_UNPACK *outpFrame)
{

    INT32 nNumber0xFE = 0;                     /* 前导字符的个数 */
    INT32 i = 0;
    INT32 pos = 0;
    INT32 nCheckSumPosStart, nCheckSumPos, nEndPos;
    INT32 ret = 0;
    UINT8 ucCtrlCode = 0;
    UINT8 ucDataLen = 0;
    UINT8 ucCheckSum = 0;
    UINT32 ulRulerID = 0;
    
    UINT8 *buffer = (UINT8 *)inpBuffer;

    char addr_bcd[6] = {0};                  //地址信息(BCD形式)
    char addr_str[D07_ADDR_LEN+1] = {0};         //地址信息(string形式)
    UINT8 aucDataTmp[D07_DATA_MAX];
    S_D07_RULER_INFO stRulerInfor = {0};
    F_D07_RULER_TRANS pTransFunc;    /* 解析函数指针 */

    if(NULL == inpBuffer || NULL == outpFrame)
    {
        return E_D07_ERRO_NULL;
    }

    if(inBufferLength < D07_FRAME_LEN_MIN)
    {
        return E_D07_ERRO_FRAME_UNCOMP;
    }

    for(i = 0; i < 4; i++)
    {
        if(0xFE == buffer[i])
        {
            pos++;
            nNumber0xFE++;
        }
    }
    
    nCheckSumPosStart = pos;

    // 检查前导字符 0x68
    if(0x68 != buffer[pos] || 0x68 != buffer[pos+7])
    {
        return E_D07_ERRO_FRAME_0x68;
    }

    //地址
    pos++;
    for(i = 0; i < 6; i++)
    {
        addr_bcd[i] = buffer[pos++];
    }

    pos++;

    //地址
    d07_bcd2str(addr_bcd, addr_str, 6);
    //控制码
    ucCtrlCode = buffer[pos++];
    //printf("\nCtrlCode = 0x%02X\n", ucCtrlCode);
    //转换控制码
    ret =  trans_d07ctrl_char2struct(ucCtrlCode, &(outpFrame->ctrl_s));
    if(E_D07_OK != ret)
    {
        return ret;
    }

    //数据域长度
    ucDataLen = buffer[pos++];
    nCheckSumPos = pos + ucDataLen;
    nEndPos = nCheckSumPos+1;

    for(i = 0; i < ucDataLen; i++)
    {
        aucDataTmp[i] = (buffer[pos++] - 0x33); 
    }
    //查检checksum
    for(i = nCheckSumPosStart; i < nCheckSumPos; i++)
    {
        ucCheckSum +=  buffer[i];
    }


    if(ucCheckSum != buffer[nCheckSumPos])
    {
    
        return E_D07_ERRO_FRAME_CHECK_SUM;
    }

    //结束符
    if(0x16 != buffer[nEndPos])
    {
        return E_D07_ERRO_FRAME_END_0x16;
    }


    //地址
    for(i = 0; i < D07_ADDR_LEN; i++)
    {
        outpFrame->address[i] = addr_str[i];
        //printf("%d - %c \n",i, addr_str[i]);
    }

    // 正常的报文
    
    if(E_D07_CTRL_SR_OK == outpFrame->ctrl_s.reply)
    {    
        // 原数据 
        for(i = 4; i < ucDataLen; i++)
        {
            outpFrame->data_pack[i-4] = aucDataTmp[i];
        }

        // 规约ID
        ulRulerID = ((aucDataTmp[0] & 0xFF) | 
                    ((aucDataTmp[1] << 8) & 0xFF00) |
                    ((aucDataTmp[2] << 16) & 0xFF0000) |
                    ((aucDataTmp[3] << 24) & 0xFF000000));

        // 通过ID获取详细信息    
        ret = get_d07_ruler_info(ulRulerID, &stRulerInfor);

        if(E_D07_OK != ret)
        {
            return ret;
        }
        
        if(NULL == stRulerInfor.func)
        {
            outpFrame->flag    = E_D07_UNPD_FLG_FUNC_NULL;
        }
        else
        {

            if(ucDataLen > 4)
            {
                pTransFunc          = stRulerInfor.func;
                ret = pTransFunc(E_D07_TRANS_F2U, 
                                (void*)&(outpFrame->data_unpack),
                                (void*)&(outpFrame->data_pack));
              
                if(E_D07_OK == ret)
                {
                    outpFrame->flag    = E_D07_UNPD_FLG_OK;
                }
                else
                {
                    outpFrame->flag =  E_D07_UNPD_FLG_FUNC_ERROR;
                }
            }
            
        }

        outpFrame->data_len  = ucDataLen - 4;
    }
    else
    {
        for(i = 0; i < ucDataLen; i++)
        {
            outpFrame->data_pack[i] = aucDataTmp[i];
        }
        // 异常报文
        outpFrame->data_len  = ucDataLen;
        outpFrame->flag    = E_D07_UNPD_FLG_ERROR_OK;        
        ret = trans_d07_data_err(E_D07_TRANS_F2U, 
                    (void*)&(outpFrame->data_unpack),
                    (void*)&(outpFrame->data_pack));

        if(E_D07_OK == ret)
        {
            outpFrame->flag    = E_D07_UNPD_FLG_ERROR_OK;
        }
        else
        {
            outpFrame->flag =  E_D07_UNPD_FLG_FUNC_ERROR;
        }    
        
    
    }
    
    /* 封装输出最后信息 */
    outpFrame->lead_num  = nNumber0xFE;
    outpFrame->ctrl_c      = ucCtrlCode;
    outpFrame->ruler_id  = ulRulerID;
    
    outpFrame->frame_len = ucDataLen + 12;
    outpFrame->type      = stRulerInfor.type;
    outpFrame->format    = stRulerInfor.format;

    return E_D07_OK;
}

/*************************************************
Function:       get_d07_ruler_info
Description:    通过规约ID获得对应规约的详细信息:

                (1)规约的类型及额外数据
                (2)对应数据格式类型
                (3)数据域长度
                (4)数据域各数据单位
                (5)规约的可读写属性
                (6)该规约的数据项名称
                (7)实现对该条规约解析和封装函数指针

Author:         liming 

Calls:          
Called By:      
Input:          UINT32 rulerID 规约ID
                
Output:         S_D07_RULER_INFO *outRulerInfo
Return:         正确返回 0 其它为错误类型

Others:         重要函数，规约解析核心函数
  
*************************************************/
INT32 get_d07_ruler_info(UINT32 rulerID, S_D07_RULER_INFO *outRulerInfo)
{
    UINT8 ucDi3 = 0;
    UINT8 ucDi2 = 0;
    UINT8 ucDi1 = 0;
    UINT8 ucDi0 = 0;

    E_D07_RULER_TYPE     type   = E_D07_RULER_TYPE_UNKNOWN;   // 规约类型
    E_D07_RULER_RDWR     rdwr   = E_D07_RDWR_READ_ONLY;       // 规约数据的读写属性
    E_D07_RULER_FORMAT   format = E_D07_FMT_UNKOWN;           // 规约数据的格式
    F_D07_RULER_TRANS    func   = NULL;                       // 数据域转换函数指针
    S_D07_RULER_PARA     para   = {0};                        // 规约类型的额外数据
    INT32                len    = 0;                          // 数据域字节长度 
    char                 name[NAME_LEN]   = {0};              // 该条规约数据项名称
    char                 name_1[NAME_LEN] = {0};
    char strPayOff[32] = {0};

    if(NULL == outRulerInfo)
    {
        return E_D07_ERRO_NULL;
    }

    ucDi0 =  rulerID & 0xFF;
    ucDi1 = (rulerID >> 8) & 0xFF;
    ucDi2 = (rulerID >> 16) & 0xFF;
    ucDi3 = (rulerID >> 24) & 0xFF;
    
    //printf("D3 = 0x%02X \nD2 = 0x%02X \nD1 = 0x%02X \nD0 = 0x%02X\n", ucDi3, ucDi2,ucDi1,ucDi0);

    switch(ucDi3)
    {
        case 0: //{00}[*][*][*]
        {
            if(ucDi0 > 0xC) // [00][*][*]{!(0x00~0x0C)}
            {
                return E_D07_ERRO_UNKOWN_ID;
            }
            
            // 封装结算日字符串    
            if(ucDi0 == 0)
            {
                sprintf(strPayOff, "%s", "(当前)");        
            }
            else
            {
                sprintf(strPayOff,"(上%d结算日)", ucDi0);
            }
            
            /* 对于表 A.1 相同的数据属性 */
            para.payoff = (E_D07_PARA_PAYOFF)(ucDi0 + 1); /* 结算日 */
            para.rate    = E_D07_RATE_NULL;
            para.harm    = E_D07_HARM_NULL;
            para.last    = E_D07_LAST_NULL;
            rdwr         = E_D07_RDWR_READ_ONLY;
            format         = E_D07_FMT_XXXXXX_XX;
            len            = 4;
            func         = trans_d07_data_XXXXXX_XX;
            
            switch(ucDi2)
            {
                case 0x0://[00]{00}[*][*]
                {
                    if(0 == ucDi1)//[00][00]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_HAVE_POWER_TOTAL;
                        sprintf(name_1, "组合有功总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][00]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_HAVE_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "组合有功费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][00]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_COMB_HAVE_POWER_BLOCK;
                        sprintf(name_1, "组合有功电能数据块");    
                    }
                    else    //[00][00]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x1://[00]{01}[*][*]
                {
                    if(0 == ucDi1)//[00][01]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_FORTH_HAVE_POWER_TOTAL;
                        sprintf(name_1, "正向有功总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][01]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_FORTH_HAVE_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "正向有功费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][01]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_FORTH_HAVE_POWER_BLOCK;
                        sprintf(name_1, "正向有功电能数据块");    
                    }
                    else    //[00][01]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x2://[00]{02}[*][*]
                {
                    if(0 == ucDi1)//[00][02]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_BACK_HAVE_POWER_TOTAL;
                        sprintf(name_1, "反向有功总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][02]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_BACK_HAVE_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "反向有功费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][02]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_BACK_HAVE_POWER_BLOCK;
                        sprintf(name_1, "反向有功电能数据块");    
                    }
                    else    //[00][02]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }                
                break;

                case 0x3://[00]{03}[*][*]
                {
                    if(0 == ucDi1)//[00][03]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_NONE_1_POWER_TOTAL;
                        sprintf(name_1, "组合无功1总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[03][00]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_NONE_1_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "组合无功1费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][03]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_COMB_NONE_1_POWER_BLOCK;
                        sprintf(name_1, "组合无功1电能数据块");    
                    }
                    else    //[00][03]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x4://[00]{04}[*][*]
                {
                    if(0 == ucDi1)//[00][04]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_NONE_2_POWER_TOTAL;
                        sprintf(name_1, "组合无功2总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[04][00]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_NONE_2_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "组合无功2费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][04]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_COMB_NONE_2_POWER_BLOCK;
                        sprintf(name_1, "组合无功2电能数据块");    
                    }
                    else    //[00][04]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x5://[00]{05}[*][*]
                {
                    if(0 == ucDi1)//[00][05]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_1_NONE_POWER_TOTAL;
                        sprintf(name_1, "第一象限无功总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][05]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_1_NONE_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "第一象限无功费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][05]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_QUAD_1_NONE_POWER_BLOCK;
                        sprintf(name_1, "第一象限无功电能数据块");    
                    }
                    else    //[00][05]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x6://[00]{06}[*][*]
                {
                    if(0 == ucDi1)//[00][06]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_2_NONE_POWER_TOTAL;
                        sprintf(name_1, "第二象限无功总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][06]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_2_NONE_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "第二象限无功费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][06]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_QUAD_2_NONE_POWER_BLOCK;
                        sprintf(name_1, "第二象限无功电能数据块");    
                    }
                    else    //[00][06]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x7://[00]{07}[*][*]
                {
                    if(0 == ucDi1)//[00][07]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_3_NONE_POWER_TOTAL;
                        sprintf(name_1, "第三象限无功总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][07]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_3_NONE_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "第三象限无功费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][07]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_QUAD_3_NONE_POWER_BLOCK;
                        sprintf(name_1, "第三象限无功电能数据块");    
                    }
                    else    //[00][07]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x8://[00]{08}[*][*]
                {
                    if(0 == ucDi1)//[00][08]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_4_NONE_POWER_TOTAL;
                        sprintf(name_1, "第四象限无功总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][08]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_4_NONE_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "第四象限无功费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][08]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_QUAD_4_NONE_POWER_BLOCK;
                        sprintf(name_1, "第四象限无功电能数据块");    
                    }
                    else    //[00][08]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x9://[00]{09}[*][*]
                {
                    if(0 == ucDi1)//[00][09]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_FORTH_APPARENT_POWER_TOTAL;
                        sprintf(name_1, "正向视在总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][09]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_FORTH_APPARENT_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "正向视在费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][09]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_FORTH_APPARENT_POWER_BLOCK;
                        sprintf(name_1, "正向视在电能数据块");    
                    }
                    else    //[00][09]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;
                
                case 0xA://[00]{0A}[*][*]
                {
                    if(0 == ucDi1)//[00][0A]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_BACK_APPARENT_POWER_TOTAL;
                        sprintf(name_1, "反向视在总电能");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[00][0A]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_BACK_APPARENT_POWER_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "反向视在费率%d电能", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[00][0A]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_BACK_APPARENT_POWER_BLOCK;
                        sprintf(name_1, "反向视在电能数据块");    
                    }
                    else    //[00][0A]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;


                // [00]{!(0-A)}[*][*]
                if(0 != ucDi1)
                {
                    return E_D07_ERRO_UNKOWN_ID;
                }

                case 0x80:// [00][80][00][*]
                {
                    type = E_D07_RULER_TYPE_ASSO_POWER_TOTAL;
                    sprintf(name_1, "关联总电能");    
                }
                break;

                case 0x81:// [00][81][00][*]
                {
                    type = E_D07_RULER_TYPE_FORTH_FUND_HAVE_POWER_TOTAL;
                    sprintf(name_1, "正向有功基波总电能");    
                }
                break;

                case 0x82:// [00][82][00][*]
                {
                    type = E_D07_RULER_TYPE_BACK_FUND_HAVE_POWER_TOTAL;
                    sprintf(name_1, "反向有功基波总电能");    
                }
                break;
                
                case 0x83:// [00][83][00][*]
                {
                    type = E_D07_RULER_TYPE_FORTH_HARM_HAVE_POWER_TOTAL;
                    sprintf(name_1, "正向有功谐波总电能");    
                }
                break;

                case 0x84:// [00][84][00][*]
                {
                    type = E_D07_RULER_TYPE_BACK_HARM_HAVE_POWER_TOTAL;
                    sprintf(name_1, "反向有功谐波总电能");    
                }
                break;

                case 0x85:// [00][85][00][*]
                {
                    type = E_D07_RULER_TYPE_COPR_LOSS_HAVE_POWER_COMP_TOTAL;
                    sprintf(name_1, "铜损有功总电能补偿量");    
                }
                break;

                case 0x86:// [00][86][00][*]
                {
                    type = E_D07_RULER_TYPE_CORE_LOSS_HAVE_POWER_COMP_TOTAL;
                    sprintf(name_1, "铁损有功总电能补偿量");    
                }
                break;

                case 0x15:// [00][15][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_FORTH_HAVE_POWER;
                    sprintf(name_1, "A相正向有功电能");    
                }
                break;
                
                case 0x16:// [00][16][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_BACK_HAVE_POWER;
                    sprintf(name_1, "A相反向有功电能");    
                }
                break;
                
                case 0x17:// [00][17][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_COMB_NONE_1_POWER;
                    sprintf(name_1, "A相组合无功1电能");    
                }
                break;
                
                case 0x18:// [00][18][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_COMB_NONE_2_POWER;
                    sprintf(name_1, "A相组合无功2电能");    
                }
                break;
                
                case 0x19:// [00][19][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_QUAD_1_NONE_POWER;
                    sprintf(name_1, "A相第一象限无功电能");    
                }
                break;
                
                case 0x1A:// [00][1A][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_QUAD_2_NONE_POWER;
                    sprintf(name_1, "A相第二象限无功电能");    
                }
                break;
                
                case 0x1B:// [00][1B][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_QUAD_3_NONE_POWER;
                    sprintf(name_1, "A相第三象限无功电能");    
                }
                break;
                
                case 0x1C:// [00][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_QUAD_4_NONE_POWER;
                    sprintf(name_1, "A相第四象限无功电能");    
                }
                break;

                case 0x1D:// [00][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_FORTH_APPARENT_POWER;
                    sprintf(name_1, "A相正向视在电能");    
                }
                break;
                
                case 0x1E:// [00][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_BACK_APPARENT_POWER;
                    sprintf(name_1, "A相反向视在电能");    
                }
                break;
                
                case 0x94:// [00][94][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_ASSO_POWER;
                    sprintf(name_1, "A相关联电能");    
                }
                break;

                case 0x95:// [00][95][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_FORTH_FUND_HAVE_POWER;
                    sprintf(name_1, "A相正向有功基波电能");    
                }
                break;

                case 0x96:// [00][96][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_BACK_FUND_HAVE_POWER;
                    sprintf(name_1, "A相反向有功基波电能");    
                }
                break;

                case 0x97:// [00][07][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_FORTH_HARM_HAVE_POWER;
                    sprintf(name_1, "A相正向有功谐波电能");    
                }
                break;

                case 0x98:// [00][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_BACK_HARM_HAVE_POWER;
                    sprintf(name_1, "A相反向有功谐波电能");    
                }
                break;

                case 0x99:// [00][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_COPR_LOSS_HAVE_POWER_COMP;
                    sprintf(name_1, "A相铜损有功电能补偿量");    
                }
                break;

                case 0x9A:// [00][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_CORE_LOSS_HAVE_POWER_COMP;
                    sprintf(name_1, "A相铁损有功电能补偿量");    
                }
                break;
                
                case 0x29:// [00][29][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_FORTH_HAVE_POWER;
                    sprintf(name_1, "B相正向有功电能");    
                }
                break;
                
                case 0x2A:// [00][2A][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_BACK_HAVE_POWER;
                    sprintf(name_1, "B相反向有功电能");    
                }
                break;
                
                case 0x2B:// [00][2B][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_COMB_NONE_1_POWER;
                    sprintf(name_1, "B相组合无功1电能");    
                }
                break;
                
                case 0x2C:// [00][2C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_COMB_NONE_2_POWER;
                    sprintf(name_1, "B相组合无功2电能");    
                }
                break;
                
                case 0x2D:// [00][2D][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_QUAD_1_NONE_POWER;
                    sprintf(name_1, "B相第一象限无功电能");    
                }
                break;
                
                case 0x2E:// [00][2E][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_QUAD_2_NONE_POWER;
                    sprintf(name_1, "B相第二象限无功电能");    
                }
                break;
                
                case 0x2F:// [00][2F][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_QUAD_3_NONE_POWER;
                    sprintf(name_1, "B相第三象限无功电能");    
                }
                break;
                
                case 0x30:// [00][30][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_QUAD_4_NONE_POWER;
                    sprintf(name_1, "B相第四象限无功电能");    
                }
                break;

                case 0x31:// [00][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_FORTH_APPARENT_POWER;
                    sprintf(name_1, "B相正向视在电能");    
                }
                break;
                
                case 0x32:// [00][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_BACK_APPARENT_POWER;
                    sprintf(name_1, "B相反向视在电能");    
                }
                break;
                
                case 0xA8:// [00][A8][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_ASSO_POWER;
                    sprintf(name_1, "B相关联电能");    
                }
                break;

                case 0xA9:// [00][A9][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_FORTH_FUND_HAVE_POWER;
                    sprintf(name_1, "B相正向有功基波电能");    
                }
                break;

                case 0xAA:// [00][96][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_BACK_FUND_HAVE_POWER;
                    sprintf(name_1, "B相反向有功基波电能");    
                }
                break;

                case 0xAB:// [00][07][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_FORTH_HARM_HAVE_POWER;
                    sprintf(name_1, "B相正向有功谐波电能");    
                }
                break;

                case 0xAC:// [00][AC][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_BACK_HARM_HAVE_POWER;
                    sprintf(name_1, "B相反向有功谐波电能");    
                }
                break;

                case 0xAD:// [00][AD][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_COPR_LOSS_HAVE_POWER_COMP;
                    sprintf(name_1, "B相铜损有功电能补偿量");    
                }
                break;

                case 0xAE:// [00][AE][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_CORE_LOSS_HAVE_POWER_COMP;
                    sprintf(name_1, "B相铁损有功电能补偿量");    
                }
                break;
                case 0x3D:// [00][3D][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_FORTH_HAVE_POWER;
                    sprintf(name_1, "C相正向有功电能");    
                }
                break;
                
                case 0x3E:// [00][3E][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_BACK_HAVE_POWER;
                    sprintf(name_1, "C相反向有功电能");    
                }
                break;
                
                case 0x3F:// [00][3F][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_COMB_NONE_1_POWER;
                    sprintf(name_1, "C相组合无功1电能");    
                }
                break;
                
                case 0x40:// [00][40][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_COMB_NONE_2_POWER;
                    sprintf(name_1, "C相组合无功2电能");    
                }
                break;
                
                case 0x41:// [00][41][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_QUAD_1_NONE_POWER;
                    sprintf(name_1, "C相第一象限无功电能");    
                }
                break;
                
                case 0x42:// [00][42][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_QUAD_2_NONE_POWER;
                    sprintf(name_1, "C相第二象限无功电能");    
                }
                break;
                
                case 0x43:// [00][43][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_QUAD_3_NONE_POWER;
                    sprintf(name_1, "C相第三象限无功电能");    
                }
                break;
                
                case 0x44:// [00][44][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_QUAD_4_NONE_POWER;
                    sprintf(name_1, "C相第四象限无功电能");    
                }
                break;

                case 0x45:// [00][45][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_FORTH_APPARENT_POWER;
                    sprintf(name_1, "C相正向视在电能");    
                }
                break;
                
                case 0x46:// [00][46][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_BACK_APPARENT_POWER;
                    sprintf(name_1, "C相反向视在电能");    
                }
                break;
                
                case 0xBC:// [00][BC][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_ASSO_POWER;
                    sprintf(name_1, "C相关联电能");    
                }
                break;

                case 0xBD:// [00][BD][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_FORTH_FUND_HAVE_POWER;
                    sprintf(name_1, "C相正向有功基波电能");    
                }
                break;

                case 0xBE:// [00][BE][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_BACK_FUND_HAVE_POWER;
                    sprintf(name_1, "C相反向有功基波电能");    
                }
                break;

                case 0xBF:// [00][BF][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_FORTH_HARM_HAVE_POWER;
                    sprintf(name_1, "C相正向有功谐波电能");    
                }
                break;

                case 0xC0:// [00][C0][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_BACK_HARM_HAVE_POWER;
                    sprintf(name_1, "C相反向有功谐波电能");    
                }
                break;

                case 0xC1:// [00][C1][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_COPR_LOSS_HAVE_POWER_COMP;
                    sprintf(name_1, "C相铜损有功电能补偿量");    
                }
                break;

                case 0xC2:// [00][C2][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_CORE_LOSS_HAVE_POWER_COMP;
                    sprintf(name_1, "C相铁损有功电能补偿量");    
                }
                break;
                
                default://[00]{!(0~A,15~1E,29~32,80~86,94~9A,A8~A9)}[*][*]
                    return E_D07_ERRO_UNKOWN_ID;
            }

            
        }
        break;

        case 1: //{01}[*][*][*]
        {
            if(ucDi0 > 0xC) // [01][*][*]{!(0x00~0x0C)}
            {
                return E_D07_ERRO_UNKOWN_ID;
            }

            
            
            // 封装结算日字符串    
            if(ucDi0 == 0)
            {
                sprintf(strPayOff, "%s", "(当前)");        
            }
            else
            {
                sprintf(strPayOff,"(上%d结算日)", ucDi0);
            }
            
            /* 对于表 A.2 相同的数据属性 */
            para.payoff = (E_D07_PARA_PAYOFF)(ucDi0 + 1); /* 结算日 */
            para.rate    = E_D07_RATE_NULL;
            rdwr         = E_D07_RDWR_READ_ONLY;
            format         = E_D07_FMT_XX_XXXX_YYMMDDhhmm;
            len            = 8;
            func         = trans_d07_data_XX_XXXX_YYMMDDhhmm;
            
            switch(ucDi2)
            {            

                case 0x1://[01]{01}[*][*]
                {
                    if(0 == ucDi1)//[01][01]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_FORTH_HAVE_DEMAND_TOTAL;
                        sprintf(name_1, "正向有功总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[01][01]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_FORTH_HAVE_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "正向有功费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][01]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_FORTH_HAVE_DEMAND_BLOCK;
                        sprintf(name_1, "正向有功最大需量及发生时间数据块");    
                    }
                    else    //[01][01]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x2://[01]{02}[*][*]
                {
                    if(0 == ucDi1)//[01][02]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_BACK_HAVE_DEMAND_TOTAL;
                        sprintf(name_1, "反向有功总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[01][02]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_BACK_HAVE_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "反向有功费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][02]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_BACK_HAVE_DEMAND_BLOCK;
                        sprintf(name_1, "反向有功最大需量及发生时间数据块");    
                    }
                    else    //[01][02]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }                
                break;

                case 0x3://[01]{03}[*][*]
                {
                    if(0 == ucDi1)//[01][03]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_NONE_1_DEMAND_TOTAL;
                        sprintf(name_1, "组合无功1总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[03][00]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_NONE_1_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "组合无功1费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][03]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_COMB_NONE_1_DEMAND_BLOCK;
                        sprintf(name_1, "组合无功1最大需量及发生时间数据块");    
                    }
                    else    //[01][03]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x4://[01]{04}[*][*]
                {
                    if(0 == ucDi1)//[01][04]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_NONE_2_DEMAND_TOTAL;
                        sprintf(name_1, "组合无功2总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[04][00]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_COMB_NONE_2_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "组合无功2费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][04]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_COMB_NONE_2_DEMAND_BLOCK;
                        sprintf(name_1, "组合无功2最大需量及发生时间数据块");    
                    }
                    else    //[01][04]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x5://[01]{05}[*][*]
                {
                    if(0 == ucDi1)//[01][05]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_1_NONE_DEMAND_TOTAL;
                        sprintf(name_1, "第一象限无功总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[01][05]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_1_NONE_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "第一象限无功费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][05]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_QUAD_1_NONE_DEMAND_BLOCK;
                        sprintf(name_1, "第一象限无功最大需量及发生时间数据块");    
                    }
                    else    //[01][05]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x6://[01]{06}[*][*]
                {
                    if(0 == ucDi1)//[01][06]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_2_NONE_DEMAND_TOTAL;
                        sprintf(name_1, "第二象限无功总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[01][06]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_2_NONE_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "第二象限无功费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][06]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_QUAD_2_NONE_DEMAND_BLOCK;
                        sprintf(name_1, "第二象限无功最大需量及发生时间数据块");    
                    }
                    else    //[01][06]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x7://[01]{07}[*][*]
                {
                    if(0 == ucDi1)//[01][07]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_3_NONE_DEMAND_TOTAL;
                        sprintf(name_1, "第三象限无功总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[01][07]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_3_NONE_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "第三象限无功费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][07]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_QUAD_3_NONE_DEMAND_BLOCK;
                        sprintf(name_1, "第三象限无功最大需量及发生时间数据块");    
                    }
                    else    //[01][07]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x8://[01]{08}[*][*]
                {
                    if(0 == ucDi1)//[01][08]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_4_NONE_DEMAND_TOTAL;
                        sprintf(name_1, "第四象限无功总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[01][08]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_QUAD_4_NONE_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "第四象限无功费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][08]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_QUAD_4_NONE_DEMAND_BLOCK;
                        sprintf(name_1, "第四象限无功最大需量及发生时间数据块");    
                    }
                    else    //[01][08]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;

                case 0x9://[01]{09}[*][*]
                {
                    if(0 == ucDi1)//[01][09]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_FORTH_APPARENT_DEMAND_TOTAL;
                        sprintf(name_1, "正向视在总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[01][09]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_FORTH_APPARENT_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "正向视在费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][09]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_FORTH_APPARENT_DEMAND_BLOCK;
                        sprintf(name_1, "正向视在最大需量及发生时间数据块");    
                    }
                    else    //[01][09]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;
                
                case 0xA://[01]{0A}[*][*]
                {
                    if(0 == ucDi1)//[01][0A]{00}[*]
                    {
                        type = E_D07_RULER_TYPE_BACK_APPARENT_DEMAND_TOTAL;
                        sprintf(name_1, "反向视在总最大需量及发生时间");                        
                    }
                    else if(ucDi1 >= 0x01 && ucDi1 <= 0x3F) //[01][0A]{(1~3F)}[*]
                    {
                        type = E_D07_RULER_TYPE_BACK_APPARENT_DEMAND_RATE;
                        para.rate    = (E_D07_PARA_RATE)ucDi1;
                        sprintf(name_1, "反向视在费率%d最大需量及发生时间", ucDi1);    
                    }
                    else if(0xFF == ucDi1) //[01][0A]{FF}[*]
                    {                        
                        type = E_D07_RULER_TYPE_BACK_APPARENT_DEMAND_BLOCK;
                        sprintf(name_1, "反向视在最大需量及发生时间数据块");    
                    }
                    else    //[01][0A]{(!(0-3F,FF))}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                
                break;


                // [01]{!(0-A)}[*][*]
                if(0 != ucDi1)
                {
                    return E_D07_ERRO_UNKOWN_ID;
                }

                case 0x15:// [01][15][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_FORTH_HAVE_POWER;
                    sprintf(name_1, "A相正向有功最大需量及发生时间");    
                }
                break;
                
                case 0x16:// [01][16][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_BACK_HAVE_POWER;
                    sprintf(name_1, "A相反向有功最大需量及发生时间");    
                }
                break;
                
                case 0x17:// [01][17][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_COMB_NONE_1_POWER;
                    sprintf(name_1, "A相组合无功1最大需量及发生时间");    
                }
                break;
                
                case 0x18:// [01][18][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_COMB_NONE_2_POWER;
                    sprintf(name_1, "A相组合无功2最大需量及发生时间");    
                }
                break;
                
                case 0x19:// [01][19][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_QUAD_1_NONE_POWER;
                    sprintf(name_1, "A相第一象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x1A:// [01][1A][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_QUAD_2_NONE_POWER;
                    sprintf(name_1, "A相第二象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x1B:// [01][1B][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_QUAD_3_NONE_POWER;
                    sprintf(name_1, "A相第三象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x1C:// [01][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_QUAD_4_NONE_POWER;
                    sprintf(name_1, "A相第四象限无功最大需量及发生时间");    
                }
                break;

                case 0x1D:// [01][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_FORTH_APPARENT_POWER;
                    sprintf(name_1, "A相正向视在最大需量及发生时间");    
                }
                break;
                
                case 0x1E:// [01][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_A_BACK_APPARENT_POWER;
                    sprintf(name_1, "A相反向视在最大需量及发生时间");    
                }
                break;

                case 0x29:// [01][29][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_FORTH_HAVE_POWER;
                    sprintf(name_1, "B相正向有功最大需量及发生时间");    
                }
                break;
                
                case 0x2A:// [01][2A][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_BACK_HAVE_POWER;
                    sprintf(name_1, "B相反向有功最大需量及发生时间");    
                }
                break;
                
                case 0x2B:// [01][2B][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_COMB_NONE_1_POWER;
                    sprintf(name_1, "B相组合无功1最大需量及发生时间");    
                }
                break;
                
                case 0x2C:// [01][2C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_COMB_NONE_2_POWER;
                    sprintf(name_1, "B相组合无功2最大需量及发生时间");    
                }
                break;
                
                case 0x2D:// [01][2D][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_QUAD_1_NONE_POWER;
                    sprintf(name_1, "B相第一象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x2E:// [01][2E][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_QUAD_2_NONE_POWER;
                    sprintf(name_1, "B相第二象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x2F:// [01][2F][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_QUAD_3_NONE_POWER;
                    sprintf(name_1, "B相第三象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x30:// [01][30][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_QUAD_4_NONE_POWER;
                    sprintf(name_1, "B相第四象限无功最大需量及发生时间");    
                }
                break;

                case 0x31:// [01][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_FORTH_APPARENT_POWER;
                    sprintf(name_1, "B相正向视在最大需量及发生时间");    
                }
                break;
                
                case 0x32:// [01][1C][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_B_BACK_APPARENT_POWER;
                    sprintf(name_1, "B相反向视在最大需量及发生时间");    
                }
                break;
                
                case 0x3D:// [01][3D][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_FORTH_HAVE_POWER;
                    sprintf(name_1, "C相正向有功最大需量及发生时间");    
                }
                break;
                
                case 0x3E:// [01][3E][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_BACK_HAVE_POWER;
                    sprintf(name_1, "C相反向有功最大需量及发生时间");    
                }
                break;
                
                case 0x3F:// [01][3F][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_COMB_NONE_1_POWER;
                    sprintf(name_1, "C相组合无功1最大需量及发生时间");    
                }
                break;
                
                case 0x40:// [01][40][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_COMB_NONE_2_POWER;
                    sprintf(name_1, "C相组合无功2最大需量及发生时间");    
                }
                break;
                
                case 0x41:// [01][41][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_QUAD_1_NONE_POWER;
                    sprintf(name_1, "C相第一象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x42:// [01][42][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_QUAD_2_NONE_POWER;
                    sprintf(name_1, "C相第二象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x43:// [01][43][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_QUAD_3_NONE_POWER;
                    sprintf(name_1, "C相第三象限无功最大需量及发生时间");    
                }
                break;
                
                case 0x44:// [01][44][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_QUAD_4_NONE_POWER;
                    sprintf(name_1, "C相第四象限无功最大需量及发生时间");    
                }
                break;

                case 0x45:// [01][45][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_FORTH_APPARENT_POWER;
                    sprintf(name_1, "C相正向视在最大需量及发生时间");    
                }
                break;
                
                case 0x46:// [01][46][00][*]
                {
                    type = E_D07_RULER_TYPE_PHASE_C_BACK_APPARENT_POWER;
                    sprintf(name_1, "C相反向视在最大需量及发生时间");    
                }
                break;
                
                
                default://[01]{!(0~A,15~1E,29~32,80~86,94~9A,A8~A9)}[*][*]
                    return E_D07_ERRO_UNKOWN_ID;
            }

            // 合成最后的名字
            //sprintf(name, "%s%s", strPayOff, name_1);
        }
        break;

        case 2: //{02}[*][*][*]
        {
            /* 对于表 A.3 相同的数据属性 */
            para.payoff = E_D07_PAYOFF_NULL; /* 结算日 */
            para.rate    = E_D07_RATE_NULL;
            para.harm    = E_D07_HARM_NULL;
            para.last    = E_D07_LAST_NULL;
            rdwr         = E_D07_RDWR_READ_ONLY;
            format         = E_D07_FMT_UNKOWN;
            len            = 0;
            func         = NULL;
            
            switch(ucDi2)
            {
                case 0x01://[02]{01}[*][*]
                {
                    switch(ucDi1)
                    {
                        if(0 != ucDi0) //[02][01][*]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }

                        case 1://[02][01]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_VOLT;
                            len        = 2;
                            format     = E_D07_FMT_XXX_X;                    
                            func     = trans_d07_data_XXX_X;
                            sprintf(name_1, "A相电压");    
                        }
                        break;

                        case 2://[02][01]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_VOLT;
                            len        = 2;
                            format     = E_D07_FMT_XXX_X;                    
                            func     = trans_d07_data_XXX_X;
                            sprintf(name_1, "B相电压");                                
                        }
                        break;

                        case 3://[02][01]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_VOLT;
                            len        = 2;
                            format     = E_D07_FMT_XXX_X;                    
                            func     = trans_d07_data_XXX_X;
                            sprintf(name_1, "C相电压");                                
                        }
                        break;

                        case 0xFF://[02][01]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_VOLT_BLOCK;
                            len        = 2;
                            format     = E_D07_FMT_XXX_X;                    
                            func     = trans_d07_data_XXX_X;
                            sprintf(name_1, "电压数据块");                                    
                        }
                        break;

                        default://[02][01]{!(1-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x02://[02]{02}[*][*]
                {
                    switch(ucDi1)
                    {
                        if(0 != ucDi0) //[02][02][*]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }

                        case 1://[02][02]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_ELEC;
                            len        = 3;
                            format     = E_D07_FMT_XXX_XXX;                    
                            func     = trans_d07_data_XXX_XXX;
                            sprintf(name_1, "A相电流");                                
                        }
                        break;

                        case 2://[02][02]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_ELEC;
                            len        = 3;
                            format     = E_D07_FMT_XXX_XXX;                    
                            func     = trans_d07_data_XXX_XXX;
                            sprintf(name_1, "B相电流");                                                            
                        }
                        break;

                        case 3://[02][02]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_ELEC;
                            len        = 3;
                            format     = E_D07_FMT_XXX_XXX;                    
                            func     = trans_d07_data_XXX_XXX;
                            sprintf(name_1, "C相电流");                                                            
                        }
                        break;

                        case 0xFF://[02][02]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_ELEC_BLOCK;
                            len        = 3;
                            format     = E_D07_FMT_XXX_XXX;                    
                            func     = trans_d07_data_XXX_XXX;
                            sprintf(name_1, "电流数据块");                                                            
                        }
                        break;

                        default://[02][02]{!(1-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x03://[02]{03}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0://[02][03]{00}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_HAVE_POWER_RATE_TOTAL;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时总有功功率");                            
                        }
                        break;

                        case 1://[02][03]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_A_HAVE_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时A相有功功率");                            
                        }
                        break;

                        case 2://[02][03]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_B_HAVE_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时B相有功功率");    
                        }
                        break;

                        case 3://[02][03]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_C_HAVE_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时C相有功功率");                                
                        }
                        break;

                        case 0xFF://[02][03]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_HAVE_POWER_RATE_BLOCK;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时有功功率数据块");                                
                        }
                        break;

                        default://[02][03]{!(0-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x04://[02]{04}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0: //[02][04][01]{0}
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_NONE_POWER_RATE_TOTAL;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时总无功功率");    
                        }
                        break;
                        
                        case 1://[02][04]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_A_NONE_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时A相无功功率");                                
                        }
                        break;

                        case 2://[02][04]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_B_NONE_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时A相无功功率");                                                            
                        }
                        break;

                        case 3://[02][04]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_C_NONE_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时A相无功功率");                                
                        }
                        break;

                        case 0xFF://[02][04]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_NONE_POWER_RATE_BLOCK;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时无功功率数据块");                                
                        }
                        break;

                        default://[02][04]{!(1-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x05://[02]{05}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0: //[02][05][01]{0}
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_APPARENT_POWER_RATE_TOTAL;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时总视在功率");    
                        }
                        break;
                        
                        case 1://[02][05]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_A_APPARENT_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时A相视在功率");                                
                        }
                        break;

                        case 2://[02][05]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_B_APPARENT_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时A相视在功率");                                                            
                        }
                        break;

                        case 3://[02][05]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_PHASE_C_APPARENT_POWER_RATE;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时A相视在功率");                                
                        }
                        break;

                        case 0xFF://[02][05]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_INSTANT_APPARENT_POWER_RATE_BLOCK;
                            len        = 3;
                            format     = E_D07_FMT_XX_XXXX;                    
                            func     = trans_d07_data_XX_XXXX;
                            sprintf(name_1, "瞬时视在功率数据块");                                
                        }
                        break;

                        default://[02][05]{!(1-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x06://[02]{06}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0://[02][06]{00}[00]
                        {
                            type     = E_D07_RULER_TYPE_POWER_RATE_FACTOR_TOTAL;
                            len        = 2;
                            format     = E_D07_FMT_X_XXX;                    
                            func     = trans_d07_data_X_XXX;
                            sprintf(name_1, "总功率因数");                                
                        }
                        break;

                        case 1://[02][06]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_POWER_RATE_FACTOR;
                            len        = 2;
                            format     = E_D07_FMT_X_XXX;                    
                            func     = trans_d07_data_X_XXX;
                            sprintf(name_1, "A相功率因数");                                
                        }
                        break;

                        case 2://[02][06]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_POWER_RATE_FACTOR;
                            len        = 2;
                            format     = E_D07_FMT_X_XXX;                    
                            func     = trans_d07_data_X_XXX;
                            sprintf(name_1, "B相功率因数");                                
                        }
                        break;

                        case 3://[02][06]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_POWER_RATE_FACTOR;
                            len        = 2;
                            format     = E_D07_FMT_X_XXX;                    
                            func     = trans_d07_data_X_XXX;
                            sprintf(name_1, "C相功率因数");                                
                        }
                        break;

                        case 0xFF://[02][06]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_POWER_RATE_FACTOR_BLOCK;
                            len        = 3;
                            format     = E_D07_FMT_X_XXX;                    
                            func     = trans_d07_data_X_XXX;
                            sprintf(name_1, "总功率因数");                                
                        }
                        break;

                        default://[02][06]{!(1-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x07://[02]{07}[*][*]
                {
                    switch(ucDi1)
                    {
                        if(0 != ucDi0) //[02][07][*]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }

                        case 1://[02][07]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_ANGLE;
                            len        = 2;
                            format     = E_D07_FMT_XXX_X;                    
                            func     = trans_d07_data_XXX_X;
                            sprintf(name_1, "A相相角");                            
                        }
                        break;

                        case 2://[02][07]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_ANGLE;
                            len        = 2;
                            format     = E_D07_FMT_XXX_X;                    
                            func     = trans_d07_data_XXX_X;
                            sprintf(name_1, "B相相角");                                
                        }
                        break;

                        case 3://[02][07]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_ANGLE;
                            len        = 2;
                            format     = E_D07_FMT_XXX_X;                    
                            func     = trans_d07_data_XXX_X;
                            sprintf(name_1, "C相相角");                                
                        }
                        break;

                        case 0xFF://[02][07]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_ANGLE_BLOCK;
                            len        = 2;
                            format     = E_D07_FMT_XXX_X;                    
                            func     = trans_d07_data_XXX_X;
                            sprintf(name_1, "相角数据块");                                
                        }
                        break;

                        default://[02][07]{!(1-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x08://[02]{08}[*][*]
                {
                    switch(ucDi1)
                    {
                        if(0 != ucDi0) //[02][08][*]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }

                        case 1://[02][08]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_VOLT_WAVEFORM_DISTORTION;
                            len        = 2;
                            format     = E_D07_FMT_XX_XX;                    
                            func     = trans_d07_data_XX_XX;
                            sprintf(name_1, "A相电压波形失真度");                                
                        }
                        break;

                        case 2://[02][08]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_VOLT_WAVEFORM_DISTORTION;
                            len        = 2;
                            format     = E_D07_FMT_XX_XX;                    
                            func     = trans_d07_data_XX_XX;
                            sprintf(name_1, "B相电压波形失真度");                                
                        }
                        break;

                        case 3://[02][08]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_VOLT_WAVEFORM_DISTORTION;
                            len        = 2;
                            format     = E_D07_FMT_XX_XX;                    
                            func     = trans_d07_data_XX_XX;
                            sprintf(name_1, "C相电压波形失真度");                                
                        }
                        break;

                        case 0xFF://[02][08]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_VOLT_WAVEFORM_DISTORTION_BLOCK;
                            len        = 2;
                            format     = E_D07_FMT_XX_XX;                    
                            func     = trans_d07_data_XX_XX;
                            sprintf(name_1, "电压波形失真度数据块");                                
                        }
                        break;

                        default://[02][08]{!(1-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x09://[02]{09}[*][*]
                {
                    switch(ucDi1)
                    {
                        if(0 != ucDi0) //[02][09][*]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }

                        case 1://[02][09]{01}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_ELEC_WAVEFORM_DISTORTION;
                            len        = 2;
                            format     = E_D07_FMT_XX_XX;                    
                            func     = trans_d07_data_XX_XX;
                            sprintf(name_1, "A相电流波形失真度");                                
                        }
                        break;

                        case 2://[02][09]{02}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_ELEC_WAVEFORM_DISTORTION;
                            len        = 2;
                            format     = E_D07_FMT_XX_XX;                    
                            func     = trans_d07_data_XX_XX;
                            sprintf(name_1, "B相电流波形失真度");                                
                        }
                        break;

                        case 3://[02][09]{03}[00]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_ELEC_WAVEFORM_DISTORTION;
                            len        = 2;
                            format     = E_D07_FMT_XX_XX;                    
                            func     = trans_d07_data_XX_XX;
                            sprintf(name_1, "C相电流波形失真度");                                
                        }
                        break;

                        case 0xFF://[02][09]{FF}[00]
                        {
                            type     = E_D07_RULER_TYPE_ELEC_WAVEFORM_DISTORTION_BLOCK;
                            len        = 2;
                            format     = E_D07_FMT_XX_XX;                    
                            func     = trans_d07_data_XX_XX;
                            sprintf(name_1, "电流波形失真度数据块");                                
                        }
                        break;

                        default://[02][09]{!(1-3,F)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x0A://[02]{0A}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 1://[02][0A]{01}[00]
                        {

                            if(ucDi0 >= 0x01 && ucDi0 <= 0x15) // [02][0A][01]{(01~15)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_A_VOLT_HARMONIC_CONTENT_T;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;    
                                func     = trans_d07_data_XX_XX;
                                para.harm    = (E_D07_PARA_HARM)ucDi0;
                                sprintf(name_1, "A相电压%d次谐波含量", ucDi0);    
                            }
                            else if(0xFF == ucDi0) // [02][0A][01]{FF}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_A_VOLT_HARMONIC_CONTENT_BLOCK;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "A相电压次谐波含量数据块");    
                            }
                            else // [02][0A][01]{!(FF,1~15)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }                                                    
                        }
                        break;

                        case 2://[02][0A]{02}[00]
                        {
                            if(ucDi0 >= 0x01 && ucDi0 <= 0x15) // [02][0A][02]{(01~15)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_B_VOLT_HARMONIC_CONTENT_T;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;    
                                func     = trans_d07_data_XX_XX;
                                para.harm    = (E_D07_PARA_HARM)ucDi0;
                                sprintf(name_1, "B相电压%d次谐波含量", ucDi0);    
                            }
                            else if(0xFF == ucDi0) // [02][0A][02]{FF}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_B_VOLT_HARMONIC_CONTENT_BLOCK;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "B相电压次谐波含量数据块");    
                            }
                            else // [02][0A][02]{!(FF,1~15)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }                                                            
                        }
                        break;

                        case 3://[02][0A]{03}[00]
                        {
                            if(ucDi0 >= 0x01 && ucDi0 <= 0x15) // [02][0A][03]{(01~15)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_C_VOLT_HARMONIC_CONTENT_T;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;    
                                func     = trans_d07_data_XX_XX;
                                para.harm    = (E_D07_PARA_HARM)ucDi0;
                                sprintf(name_1, "C相电压%d次谐波含量", ucDi0);    
                            }
                            else if(0xFF == ucDi0) // [02][0A][03]{FF}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_C_VOLT_HARMONIC_CONTENT_BLOCK;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "C相电压次谐波含量数据块");    
                            }
                            else // [02][0A][03]{!(FF,1~15)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }                                                            
                        }
                        break;

                        
                        default://[02][0A]{!(1-3)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
    
                case 0x0B://[02]{0B}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 1://[02][0B]{01}[00]
                        {

                            if(ucDi0 >= 0x01 && ucDi0 <= 0x15) // [02][0B][01]{(01~15)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_A_ELEC_HARMONIC_CONTENT_T;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;    
                                func     = trans_d07_data_XX_XX;
                                para.harm    = (E_D07_PARA_HARM)ucDi0;
                                sprintf(name_1, "A相电流%d次谐波含量", ucDi0);    
                            }
                            else if(0xFF == ucDi0) // [02][0B][01]{FF}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_A_ELEC_HARMONIC_CONTENT_BLOCK;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "A相电流次谐波含量数据块");    
                            }
                            else // [02][0B][01]{!(FF,1~15)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }                                                    
                        }
                        break;

                        case 2://[02][0B]{02}[00]
                        {
                            if(ucDi0 >= 0x01 && ucDi0 <= 0x15) // [02][0B][02]{(01~15)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_B_ELEC_HARMONIC_CONTENT_T;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;    
                                func     = trans_d07_data_XX_XX;
                                para.harm    = (E_D07_PARA_HARM)ucDi0;
                                sprintf(name_1, "B相电流%d次谐波含量", ucDi0);    
                            }
                            else if(0xFF == ucDi0) // [02][0B][02]{FF}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_B_ELEC_HARMONIC_CONTENT_BLOCK;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "B相电流次谐波含量数据块");    
                            }
                            else // [02][0B][02]{!(FF,1~15)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }                                                            
                        }
                        break;

                        case 3://[02][0B]{03}[00]
                        {
                            if(ucDi0 >= 0x01 && ucDi0 <= 0x15) // [02][0B][03]{(01~15)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_C_ELEC_HARMONIC_CONTENT_T;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;    
                                func     = trans_d07_data_XX_XX;
                                para.harm    = (E_D07_PARA_HARM)ucDi0;
                                sprintf(name_1, "C相电流%d次谐波含量", ucDi0);    
                            }
                            else if(0xFF == ucDi0) // [02][0B][03]{FF}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_C_ELEC_HARMONIC_CONTENT_BLOCK;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "C相电流次谐波含量数据块");    
                            }
                            else // [02][0B][03]{!(FF,1~15)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }                                                                                        
                        }
                        break;
                        
                        default://[02][0B]{!(1-3)}[00]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 0x80: //[02]{80}[*][*]
                {
                    if(ucDi1 == 0)//[02][80]{00}[*]
                    {
                        switch(ucDi0)
                        {
                            case 1: //[02][80][00][01]
                            {
                                type     = E_D07_RULER_TYPE_ZERO_LINE_ELEC;
                                len        = 3;
                                format     = E_D07_FMT_XXX_XXX;                    
                                func     = trans_d07_data_XXX_XXX;
                                sprintf(name_1, "零线电流");    
                            }
                            break;
                            
                            case 2://[02][80][00][02]
                            {
                                type     = E_D07_RULER_TYPE_POWER_GRID_HZ;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "电网频率");    
                            }
                            break;

                            case 3://[02][80][00][03]
                            {
                                type     = E_D07_RULER_TYPE_1M_AVER_HAVE_POWER_RATE_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XX_XXXX;                    
                                func     = trans_d07_data_XX_XXXX;
                                sprintf(name_1, "一分钟有功总平均功率");    
                            }
                            break;

                            case 4://[02][80][00][04]
                            {
                                type     = E_D07_RULER_TYPE_CURRENT_HAVE_POWER_DEMAND;
                                len        = 3;
                                format     = E_D07_FMT_XX_XXXX;                    
                                func     = trans_d07_data_XX_XXXX;
                                sprintf(name_1, "当前有功需量");    
                            }
                            break;

                            case 5://[02][80][00][05]
                            {
                                type     = E_D07_RULER_TYPE_CURRENT_NONE_POWER_DEMAND;
                                len        = 3;
                                format     = E_D07_FMT_XX_XXXX;                    
                                func     = trans_d07_data_XX_XXXX;
                                sprintf(name_1, "当前无功需量");    
                            }
                            break;

                            case 6://[02][80][00][06]
                            {
                                type     = E_D07_RULER_TYPE_CURRENT_APPARENT_POWER_DEMAND;
                                len        = 3;
                                format     = E_D07_FMT_XX_XXXX;                    
                                func     = trans_d07_data_XX_XXXX;
                                sprintf(name_1, "当前视在需量");    
                            }
                            break;

                            case 7://[02][80][00][07]
                            {
                                type     = E_D07_RULER_TYPE_METER_TEMP;
                                len        = 2;
                                format     = E_D07_FMT_XXX_X;                    
                                func     = trans_d07_data_X_XXX;
                                sprintf(name_1, "表内温度");    
                            }
                            break;

                            case 8://[02][80][00][08]
                            {
                                type     = E_D07_RULER_TYPE_INTER_CLOCK_CELL_VOLT;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "时钟电池电压(内部)");
                            }
                            break;

                            case 9://[02][80][00][09]
                            {
                                type     = E_D07_RULER_TYPE_READ_METER_CELL_VOLT;
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;                    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "停电抄表电池电压(外部)");
                            }
                            break;

                            case 0xA://[02][80][00][0A]
                            {
                                type     = E_D07_RULER_TYPE_INTER_CLOCK_WORK_TIME;
                                len        = 4;
                                format     = E_D07_FMT_XXXXXXXX;                    
                                func     = trans_d07_data_XXXXXXXX;
                                sprintf(name_1, "停电抄表电池电压(外部)");
                            }
                            break;
                            
                            default: //[02][80][00]{!(1-A)}
                                return E_D07_ERRO_UNKOWN_ID;
                        }
                    }
                    else // [02][80]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                    
                }
                break;

                default:// [02]{!(1~B,80)}[*][*]
                    return E_D07_ERRO_UNKOWN_ID;

            }    
        }
        break;

        /* 对应表A.4 事件记录数据标识编码表 */
        case 3: //{03}[*][*][*]
        {
            /* 对表A.4 相同数据的初始化 */
            para.payoff = E_D07_PAYOFF_NULL; /* 结算日 */
            para.rate    = E_D07_RATE_NULL;
            para.harm    = E_D07_HARM_NULL;
            para.last    = E_D07_LAST_NULL;
            rdwr         = E_D07_RDWR_READ_ONLY;
            format         = E_D07_FMT_UNKOWN;
            len            = 0;
            func         = NULL;

            switch(ucDi2)
            {
                case 1:// [03]{01}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0:// [03][01]{00}[*]
                        {
                            if(0 == ucDi0)// [03][01][00]{00}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_ABC_LOSS_VOLT_TIMES;
                                len        = 18;
                                format     = E_D07_FMT_XXXXXX_6;                    
                                func     = trans_d07_data_XXXXXX_6;
                                sprintf(name_1, "ABC相失压次数，总累计时间");
                            }
                            else         // [03][01][00]{!(00)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 1: // [03][01]{01}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][01][01]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_A_LOSS_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)A相失压记录", ucDi0);
                            }
                            else                            // [03][01][01]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                    
                        case 2: // [03][01]{02}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][01][02]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_B_LOSS_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)B相失压记录", ucDi0);
                            }
                            else                            // [03][01][02]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                        
                        case 3: // [03][01]{03}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][01][03]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_C_LOSS_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)C相失压记录", ucDi0);
                            }
                            else                            // [03][01][03]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;

                        default:// [03][01]{!(0~3)}[*]
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 2:// [03]{02}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0:// [03][02]{00}[*]
                        {
                            if(0 == ucDi0)// [03][02][00]{00}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_ABC_LESS_VOLT_TIMES;
                                len        = 18;
                                format     = E_D07_FMT_XXXXXX_6;                    
                                func     = trans_d07_data_XXXXXX_6;
                                sprintf(name_1, "ABC相欠压次数，总累计时间");
                            }
                            else         // [03][02][00]{!(00)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 1: // [03][02]{01}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][02][01]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_A_LESS_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)A相欠压记录", ucDi0);
                            }
                            else                            // [03][02][01]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                    
                        case 2: // [03][02]{02}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][02][02]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_B_LESS_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)B相欠压记录", ucDi0);
                            }
                            else                            // [03][02][02]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                        
                        case 3: // [03][02]{03}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][02][03]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_C_LESS_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)C相欠压记录", ucDi0);
                            }
                            else                            // [03][02][03]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }    
                        }
                        break;

                        default:// [03][03]{!(0~3)}[*]
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 3:// [03]{03}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0:// [03][03]{00}[*]
                        {
                            if(0 == ucDi0)// [03][03][00]{00}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_ABC_MORE_VOLT_TIMES;
                                len        = 18;
                                format     = E_D07_FMT_XXXXXX_6;                    
                                func     = trans_d07_data_XXXXXX_6;
                                sprintf(name_1, "ABC相过压次数，总累计时间");
                            }
                            else         // [03][03][00]{!(00)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 1: // [03][03]{01}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][03][01]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_A_MORE_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)A相过压记录", ucDi0);
                            }
                            else                            // [03][03][01]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }

                        }
                        break;

                        case 2: // [03][03]{02}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][03][02]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_B_MORE_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)B相过压记录", ucDi0);
                            }
                            else                            // [03][03][02]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }

                        }
                        break;

                        case 3: // [03][03]{03}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][03][03]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_C_MORE_VOLT_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)C相过压记录", ucDi0);
                            }
                            else                            // [03][03][03]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }

                        }
                        break;

                        default:
                            return E_D07_ERRO_UNKOWN_ID;
                
                    }
                }
                break;
                
                case 4:// [03]{04}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0:// [03][04]{00}[*]
                        {
                            if(0 == ucDi0)// [03][04][00]{00}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_ABC_DROP_TIMES;
                                len        = 18;
                                format     = E_D07_FMT_XXXXXX_6;                    
                                func     = trans_d07_data_XXXXXX_6;
                                sprintf(name_1, "ABC相断相次数，总累计时间");
                            }
                            else         // [03][04][00]{!(00)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 1: // [03][04]{01}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][04][01]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_A_DROP_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)A相断相记录", ucDi0);
                            }
                            else                            // [03][04][01]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                    
                        case 2: // [03][04]{02}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][04][02]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_B_DROP_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)B相断相记录", ucDi0);
                            }
                            else                            // [03][04][02]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                        
                        case 3: // [03][04]{03}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][04][03]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PHASE_C_DROP_RECORD;
                                len        = 131;
                                format     = E_D07_FMT_RECD_LOSS_LESS_VOLT;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_loss_less_volt;
                                sprintf(name_1, "(上%d次)C相断相记录", ucDi0);
                            }
                            else                            // [03][04][03]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        default:// [03][04]{!(0~3)}[*]
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;    

                case 5:// [03]{05}[*][*]
                {
                    if(0 == ucDi1) // [03][05]{00}[*]
                    {
                        if(0 == ucDi0) // [03][05][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_LOSS_VOLT_TIMES_TOTAL;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "全失压次数，总累计时间");
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][05][00]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_LOSS_ALL_VOLT_TIME_N;
                            len        = 15;
                            format     = E_D07_FMT_YYMMDDhhmmss_XXX_XXX_YYMMDDhhmmss;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_YYMMDDhhmmss_XXX_XXX_YYMMDDhhmmss;
                            sprintf(name_1, "(上%d次)全失压发生时刻，电流值，结束时刻",ucDi0);
                        }
                        else         // [03][05][00]{!(1~A)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else          // [03][05]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 6:// [03]{06}[*][*]
                {
                    if(0 == ucDi1) // [03][06]{00}[*]
                    {
                        if(0 == ucDi0) // [03][06][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_APS_LOSS_ELEC_TIMES_TOTAL;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "辅助电源失电发生次数，总累计时间");
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][06][00]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_APS_LOSS_ELEC_TIME_N;
                            len        = 12;
                            format     = E_D07_FMT_YYMMDDhhmmss;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_YYMMDDhhmmss;
                            sprintf(name_1, "(上%d次)辅助电源失电发生时刻，结束时刻",ucDi0);
                        }
                        else         // [03][06][00]{!(1~A)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else          // [03][06]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 7:// [03]{07}[*][*]
                {
                    if(0 == ucDi1) // [03][07]{00}[*]
                    {
                        if(0 == ucDi0) // [03][07][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_VOLT_ANTI_PHASE_TIMES_TOTAL;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "电压逆相序总次数，总累计时间");
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][07][00]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_VOLT_ANTI_PHASE_RECORD_N;
                            len        = 76;
                            format     = E_D07_FMT_RECD_ANTI_PHASE;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_anti_phase;
                            sprintf(name_1, "(上%d次)电压逆相序记录",ucDi0);
                        }
                        else         // [03][07][00]{!(1~A)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else          // [03][07]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }

                }
                break;

                case 8:// [03]{08}[*][*]
                {
                   if(0 == ucDi1) // [03][08]{00}[*]
                    {
                        if(0 == ucDi0) // [03][08][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_ELEC_ANTI_PHASE_TIMES_TOTAL;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "电流逆相序总次数，总累计时间");
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][08][00]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_ELEC_ANTI_PHASE_RECORD_N;
                            len        = 76;
                            format     = E_D07_FMT_RECD_ANTI_PHASE;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_anti_phase;
                            sprintf(name_1, "(上%d次)电流逆相序记录",ucDi0);
                        }
                        else         // [03][08][00]{!(1~A)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else          // [03][08]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }

                }
                break;

                case 9:// [03]{09}[*][*]
                {
                    if(0 == ucDi1) // [03][09]{00}[*]
                    {
                        if(0 == ucDi0) // [03][09][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_VOLT_UNBALANCE_TIMES_TOTAL;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "电压不平衡总次数，总累计时间");
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][09][00]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_VOLT_UNBALANCE_RECORD_N;
                            len        = 78;
                            format     = E_D07_FMT_RECD_UN_BALANCE;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_un_balance;
                            sprintf(name_1, "(上%d次)电压不平衡记录",ucDi0);
                        }
                        else         // [03][09][00]{!(1~A)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else          // [03][09]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }

                }
                break;


                case 0xA:// [03]{0A}[*][*]
                {
                    if(0 == ucDi1) // [03][0A]{00}[*]
                    {
                        if(0 == ucDi0) // [03][0A][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_ELEC_UNBALANCE_TIMES_TOTAL;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "电流不平衡总次数，总累计时间");
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0A][00]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_ELEC_UNBALANCE_RECORD_N;
                            len        = 78;
                            format     = E_D07_FMT_RECD_UN_BALANCE;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_un_balance;
                            sprintf(name_1, "(上%d次)电流不平衡记录",ucDi0);
                        }
                        else         // [03][0A][00]{!(1~A)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else          // [03][0A]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 0xB:// [03]{0B}[*][*]
                {
                    if(0 == ucDi1) // [03][0B]{00}[*]
                    {
                        if(0 == ucDi0) // [03][0B][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_ABC_LOSS_ELEC_TIMES;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "ABC相失流次数，总累计时间");
                        }
                        else         // [03][0B][00]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else if(1 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0B][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_LOSS_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)A相失流记录",ucDi0);
                        }
                    }
                    else if(2 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0B][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_LOSS_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)B相失流记录",ucDi0);
                        }
                    }
                    else if(3 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0B][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_LOSS_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)C相失流记录",ucDi0);
                        }
                    }
                    else          // [03][0B]{!(0~3)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }

                case 0xC:// [03]{0C}[*][*]
                {
                    if(0 == ucDi1) // [03][0C]{00}[*]
                    {
                        if(0 == ucDi0) // [03][0C][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_ABC_MORE_ELEC_TIMES;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "ABC相过流次数，总累计时间");
                        }
                        else         // [03][0C][00]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else if(1 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0C][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_MORE_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)A相过流记录",ucDi0);
                        }
                    }
                    else if(2 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0C][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_MORE_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)B相过流记录",ucDi0);
                        }
                    }
                    else if(3 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0C][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_MORE_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)C相过流记录",ucDi0);
                        }
                    }
                    else          // [03][0C]{!(0~3)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 0xD:// [03]{0D}[*][*]
                {
                    if(0 == ucDi1) // [03][0D]{00}[*]
                    {
                        if(0 == ucDi0) // [03][0D][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_ABC_DROP_ELEC_TIMES;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "ABC相断流次数，总累计时间");
                        }
                        else         // [03][0D][00]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else if(1 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0D][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_DROP_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)A相断流记录",ucDi0);
                        }
                    }
                    else if(2 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0D][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_DROP_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)B相断流记录",ucDi0);
                        }
                    }
                    else if(3 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0D][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_DROP_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)C相断流记录",ucDi0);
                        }
                    }
                    else          // [03][0D]{!(0~3)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }

                case 0xE:// [03]{0E}[*][*]
                {
                    if(0 == ucDi1) // [03][0E]{00}[*]
                    {
                        if(0 == ucDi0) // [03][0E][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_ABC_BACKWARD_ELEC_TIMES;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "ABC相潮流反向次数，总累计时间");
                        }
                        else         // [03][0E][00]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else if(1 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0E][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_BACKWARD_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_less_more_loss_elec;
                            sprintf(name_1, "(上%d次)A相潮流反向记录",ucDi0);
                        }
                    }
                    else if(2 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0E][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_BACKWARD_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_anti_phase;
                            sprintf(name_1, "(上%d次)B相潮流反向记录",ucDi0);
                        }
                    }
                    else if(3 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0E][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_BACKWARD_ELEC_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_anti_phase;
                            sprintf(name_1, "(上%d次)C相潮流反向记录",ucDi0);
                        }
                    }
                    else          // [03][0E]{!(0~3)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }

                }
                break;

                case 0xF:// [03]{0F}[*][*]
                {
                    if(0 == ucDi1) // [03][0F]{00}[*]
                    {
                        if(0 == ucDi0) // [03][0F][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_ABC_OVERLOAD_TIMES;
                            len        = 6;
                            format     = E_D07_FMT_XXXXXX_2;    
                            func     = trans_d07_data_XXXXXX_2;
                            sprintf(name_1, "ABC相过载次数，总累计时间");
                        }
                        else         // [03][0F][00]{!(0)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else if(1 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0F][01]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_OVERLOAD_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_anti_phase;
                            sprintf(name_1, "(上%d次)A相过载记录",ucDi0);
                        }
                    }
                    else if(2 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0F][02]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_OVERLOAD_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_anti_phase;
                            sprintf(name_1, "(上%d次)B相过载记录",ucDi0);
                        }
                    }
                    else if(3 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xA) // [03][0F][03]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_OVERLOAD_RECORD;
                            len        = 115;
                            format     = E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_recd_anti_phase;
                            sprintf(name_1, "(上%d次)C相过载记录",ucDi0);
                        }
                    }
                    else          // [03][0F]{!(0~3)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 0x10:// [03]{10}[*][*]
                {
                    if(ucDi0 > 0xC) // [00][*][*]{!(0x00~0x0C)}
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                    
                    // 封装结算日字符串    
                    if(ucDi0 == 0)
                    {
                        sprintf(strPayOff, "%s", "(当前)");        
                    }
                    else
                    {
                        sprintf(strPayOff,"(上%d结算日)", ucDi0);
                    }
                    
                    para.payoff = (E_D07_PARA_PAYOFF)(ucDi0 + 1); /* 结算日 */
                    len        = 27;
                    format     = E_D07_FMT_RECD_VOLT_PASS_PERCENT;    
                    func     = trans_d07_data_recd_volt_pass_percent;
                    
                    switch(ucDi1)
                    {
                        case 0: // [03]{10}{00}[*]
                        {
                            type     = E_D07_RULER_TYPE_VOLT_PASS_PERCENT_M;
                            sprintf(name_1, "电压合格率统计数据");
                        }
                        break;
                        
                        case 1:// [03]{10}{01}[*]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_A_VOLT_PASS_PERCENT_M;
                            sprintf(name_1, "A相电压合格率统计数据");
                        }
                        break;
                        
                        case 2:// [03]{10}{02}[*]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_B_VOLT_PASS_PERCENT_M;
                            sprintf(name_1, "C相电压合格率统计数据");
                        }
                        break;
                        
                        case 3:// [03]{10}{03}[*]
                        {
                            type     = E_D07_RULER_TYPE_PHASE_C_VOLT_PASS_PERCENT_M;
                            sprintf(name_1, "C相电压合格率统计数据");
                        }
                        break;

                        default:// [03]{10}{!(0~3)}[*]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 0x11:// [03]{11}[*][*]
                {
                    if(ucDi1 != 0) // [03][11]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                    else
                    {
                        if(0 == ucDi0) // [03][11][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_POWER_DOWN_TIMES;
                            len        = 3;
                            format     = E_D07_FMT_XXXXXX;    
                            func     = trans_d07_data_XXXXXX;
                            sprintf(name_1, "掉电总次数");
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 0x0A) // [03][11][00]{(1~A)}
                        {
                            type     = E_D07_RULER_TYPE_POWER_DOWN_TIME_T;
                            len        = 12;
                            format     = E_D07_FMT_YYMMDDhhmmss_2;    
                            para.last = (E_D07_PARA_LAST)ucDi0;
                            func     = trans_d07_data_YYMMDDhhmmss_2;
                            sprintf(name_1, "(上%d次)掉电发生时刻, 结束时刻",ucDi0);
                        }
                        else // [03][11][00]{!(0~A)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                    }
                }
                case 0x12:// [03]{12}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0: // [03][12]{00}[*]
                        {
                            if(0 == ucDi0) // [03][12][00]{00}
                            {
                                type     = E_D07_RULER_TYPE_OVER_LIMIT_MEMAND_TIMES;
                                len        = 12;
                                format     = E_D07_FMT_XXXXXX_6;    
                                func     = trans_d07_data_XXXXXX_6;
                                sprintf(name_1, "需量超限总次数记录");
                            }
                            else // [03][12][00]{!(00)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 1: // [03][12]{01}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0x0A) // [03][12][01]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_FORTH_OVER_LIMIT_MEMAND_TIME_T;
                                len        = 20;
                                format     = E_D07_FMT_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;
                                sprintf(name_1, "(上%d次)正向有功需量超限记录",ucDi0);
                            }
                            else    // [03][12][01]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 2: // [03][12]{02}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0x0A) // [03][12][02]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_BACK_OVER_LIMIT_MEMAND_TIME_T;
                                len        = 20;
                                format     = E_D07_FMT_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;
                                sprintf(name_1, "(上%d次)反向有功需量超限记录",ucDi0);
                            }
                            else    // [03][12][02]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 3: // [03][12]{03}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0x0A) // [03][12][03]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_QUAD_1_OVER_LIMIT_MEMAND_TIME_T;
                                len        = 20;
                                format     = E_D07_FMT_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;
                                sprintf(name_1, "(上%d次)第1象限无功需量超限记录",ucDi0);
                            }
                            else    // [03][12][03]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 4: // [03][12]{04}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0x0A) // [03][12][04]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_QUAD_2_OVER_LIMIT_MEMAND_TIME_T;
                                len        = 20;
                                format     = E_D07_FMT_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;
                                sprintf(name_1, "(上%d次)第2象限无功需量超限记录",ucDi0);
                            }
                            else    // [03][12][04]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 5: // [03][12]{05}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0x0A) // [03][12][05]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_QUAD_3_OVER_LIMIT_MEMAND_TIME_T;
                                len        = 20;
                                format     = E_D07_FMT_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;
                                sprintf(name_1, "(上%d次)第3象限无功需量超限记录",ucDi0);
                            }
                            else    // [03][12][05]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 6: // [03][12]{06}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0x0A) // [03][12][06]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_QUAD_3_OVER_LIMIT_MEMAND_TIME_T;
                                len        = 20;
                                format     = E_D07_FMT_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;
                                sprintf(name_1, "(上%d次)第4象限无功需量超限记录",ucDi0);
                            }
                            else    // [03][12][06]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        default:
                            return E_D07_ERRO_UNKOWN_ID;        
                    }
                }
                break;

                case 0x30: // [03]{30}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0: // [03][30]{00}[*]
                        {
                            if(0 == ucDi0) // [03][30][00]{00} 
                            {
                                type     = E_D07_RULER_TYPE_METER_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][00]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_METER_PROG_RECORD_T;
                                len        = D07_DATA_LEN_METER_PROG;
                                format     = E_D07_FMT_RECD_METER_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_meter_prog;
                                sprintf(name_1, "(上%d次)编程记录",ucDi0);
                            }
                            else    // [03][30][00]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 1: // [03][30]{01}[*]
                        {
                            if(0 == ucDi0) // [03][30][01]{00} 
                            {
                                type     = E_D07_RULER_TYPE_METER_CLEAR_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "电表清零总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][01]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_METER_CLEAR_RECORD_T;
                                len        = D07_DATA_LEN_RECD_METER_CLEAR;
                                format     = E_D07_FMT_RECD_METER_CLEAR;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_meter_clear;
                                sprintf(name_1, "(上%d次)电表清零记录",ucDi0);
                            }
                            else    // [03][30][01]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                        
                        case 2: // [03][30]{02}[*]
                        {
                            if(0 == ucDi0) // [03][30][02]{00} 
                            {
                                type     = E_D07_RULER_TYPE_DEMAND_CLEAR_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "需量清零总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][02]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_DEMAND_CLEAR_RECORD_T;
                                len        = D07_DATA_LEN_RECD_DEAMD_CLEAR;
                                format     = E_D07_FMT_RECD_DEAMD_CLEAR;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_demand_clear;
                                sprintf(name_1, "(上%d次)需量清零记录",ucDi0);
                            }
                            else    // [03][30][02]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                        
                        case 3: // [03][30]{03}[*]
                        {
                            if(0 == ucDi0) // [03][30][03]{00} 
                            {
                                type     = E_D07_RULER_TYPE_EVENT_CLEAR_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "事件清零总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][03]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_EVENT_CLEAR_RECORD_T;
                                len        = 14;
                                format     = E_D07_FMT_RECD_EVENT_CLEAR;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_event_clear;
                                sprintf(name_1, "(上%d次)事件清零记录", ucDi0);
                            }
                            else    // [03][30][03]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                        case 4: // [03][30]{04}[*]
                        {
                            if(0 == ucDi0) // [03][30][04]{00} 
                            {
                                type     = E_D07_RULER_TYPE_ADJUST_TIME_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "校时总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][04]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_ADJUST_TIME_RECORD_T;
                                len        = 16;
                                format     = E_D07_FMT_RECD_ADJUST_TIME;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_adjust_time;
                                sprintf(name_1, "(上%d次)校时记录", ucDi0);
                            }
                            else    // [03][30][04]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 5: // [03][30]{05}[*]
                        {
                            if(0 == ucDi0) // [03][30][05]{00} 
                            {
                                type     = E_D07_RULER_TYPE_TIME_PART_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "时段表编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][05]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_TIME_PART_PROG_RECORD_T;
                                len        = 14;
                                format     = E_D07_FMT_RECD_PART_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_part_prog;
                                sprintf(name_1, "(上%d次)时段表编程记录", ucDi0);
                            }
                            else    // [03][30][05]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                        
                        case 6: // [03][30]{06}[*]
                        {
                            if(0 == ucDi0) // [03][30][06]{00} 
                            {
                                type     = E_D07_RULER_TYPE_TIME_ZONE_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "时区表编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][06]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_TIME_ZONE_PROG_TIMES_TOTAL;
                                len        = 94;
                                format     = E_D07_FMT_RECD_ZONE_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_zone_prog;
                                sprintf(name_1, "(上%d次)时区表编程记录", ucDi0);
                            }
                            else    // [03][30][06]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        
                        }
                        break;
                        
                        case 7: // [03][30]{07}[*]
                        {
                            if(0 == ucDi0) // [03][30][07]{00} 
                            {
                                type     = E_D07_RULER_TYPE_WEEKDAY_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "周休日编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][07]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_WEEKDAY_PROG_RECORD_T;
                                len        = 11;
                                format     = E_D07_FMT_RECD_WEEKDAY_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_weekday_prog;
                                sprintf(name_1, "(上%d次)周休日编程记录", ucDi0);
                            }
                            else    // [03][30][07]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        case 8: // [03][30]{08}[*]
                        {
                            if(0 == ucDi0) // [03][30][08]{00} 
                            {
                                type     = E_D07_RULER_TYPE_HOLIDAY_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "节假日编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][08]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_HOLIDAY_PROG_RECORD_T;
                                len        = 58;
                                format     = E_D07_FMT_RECD_HOLIDAY_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_holiday_prog;
                                sprintf(name_1, "(上%d次)节假日编程记录", ucDi0);
                            }
                            else    // [03][30][08]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 9: // [03][30]{09}[*]
                        {
                            if(0 == ucDi0) // [03][30][09]{00} 
                            {
                                type     = E_D07_RULER_TYPE_HAVE_COMB_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "有功组合方式编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][09]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_HAVE_COMB_PROG_RECORD_T;
                                len        = 11;
                                format     = E_D07_FMT_RECD_POWER_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_power_prog;
                                sprintf(name_1, "(上%d次)有功组合方式编程记录", ucDi0);
                            }
                            else    // [03][30][09]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 0x0A: // [03][30]{0A}[*]
                        {
                            if(0 == ucDi0) // [03][30][0A]{00} 
                            {
                                type     = E_D07_RULER_TYPE_NONE_COMB_1_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "无功组合方式1编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][0A]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_NONE_COMB_1_PROG_RECORD_T;
                                len        = 11;
                                format     = E_D07_FMT_RECD_POWER_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_power_prog;
                                sprintf(name_1, "(上%d次)无功组合方式1编程记录", ucDi0);
                            }
                            else    // [03][30][0A]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 0x0B: // [03][30]{0B}[*]
                        {
                            if(0 == ucDi0) // [03][30][0B]{00} 
                            {
                                type     = E_D07_RULER_TYPE_NONE_COMB_2_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "无功组合方式2编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][0B]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_NONE_COMB_2_PROG_RECORD_T;
                                len        = 11;
                                format     = E_D07_FMT_RECD_POWER_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_power_prog;
                                sprintf(name_1, "(上%d次)无功组合方式2编程记录", ucDi0);
                            }
                            else    // [03][30][0B]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 0x0C: // [03][30]{0C}[*]
                        {
                            if(0 == ucDi0) // [03][30][0C]{00} 
                            {
                                type     = E_D07_RULER_TYPE_PAYOFF_DAY_PROG_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "结算日编程总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][0C]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PAYOFF_DAY_PROG_RECORD_T;
                                len        = 18;
                                format     = E_D07_FMT_RECD_PAYOFF_PROG;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_payoff_prog;
                                sprintf(name_1, "(上%d次)结算日编程记录", ucDi0);
                            }
                            else    // [03][30][0C]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 0x0D: // [03][30]{0D}[*]
                        {
                            if(0 == ucDi0) // [03][30][0D]{00} 
                            {
                                type     = E_D07_RULER_TYPE_OPEN_METER_CAP_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "开表盖总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][0D]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_OPEN_METER_CAP_RECORD_T;
                                len        = 60;
                                format     = E_D07_FMT_RECD_OPEN_METER_CAP;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_open_meter_cap;
                                sprintf(name_1, "(上%d次)开表盖记录", ucDi0);
                            }
                            else    // [03][30][0D]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                            
                        }
                        break;
                        
                        case 0x0E: // [03][30]{0E}[*]
                        {
                            if(0 == ucDi0) // [03][30][0E]{00} 
                            {
                                type     = E_D07_RULER_TYPE_OPEN_BTN_BOX_TIMES_TOTAL;
                                len        = 3;
                                format     = E_D07_FMT_XXXXXX;    
                                func     = trans_d07_data_XXXXXX;
                                sprintf(name_1, "开端钮盒总次数");
                            }
                            else if(ucDi0 >= 1 && ucDi0 <= 0x0A)// [03][30][0E]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_OPEN_BTN_BOX_RECORD_T;
                                len        = 14;
                                format     = E_D07_FMT_RECD_OPEN_BTN_BOX;    
                                para.last = (E_D07_PARA_LAST)ucDi0;
                                func     = trans_d07_data_recd_open_btn_box;
                                sprintf(name_1, "(上%d次)开端钮盒记录", ucDi0);
                            }
                            else    // [03][30][0E]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        default: // [03][30]{!(0~E)}[*]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                
                default:// [03]{!(1-12)}[*]
                return E_D07_ERRO_UNKOWN_ID;
             }
        }
        break;


        /* 对应表A.5参变量数据标识编码表 */
        case 4: //{04}[*][*][*]
        {
            /* 对表A.5 相同数据的初始化 */
            para.payoff = E_D07_PAYOFF_NULL; /* 结算日 */
            para.rate    = E_D07_RATE_NULL;
            para.harm    = E_D07_HARM_NULL;
            para.last    = E_D07_LAST_NULL;
            rdwr         = E_D07_RDWR_READ_WRITE;
            format         = E_D07_FMT_UNKOWN;
            len            = 0;
            func         = NULL;
            
            switch(ucDi2)
            {
                case 0: // [04]{00}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 1: // [04][00]{01}[*]
                        {
                            switch(ucDi0)
                            {
                                case 1: // [04][00][01]{01}
                                {
                                    type     = E_D07_RULER_TYPE_DATE;
                                    len        = 4;
                                    format     = E_D07_FMT_YYMMDDWW;    
                                    func     = trans_d07_data_YYMMDDWW;
                                    sprintf(name_1, "日期及星期（其中0代表星期天)");
                                }
                                break;

                                case 2: // [04][00][01]{02}
                                {
                                    type     = E_D07_RULER_TYPE_TIME;
                                    len        = 3;
                                    format     = E_D07_FMT_hhmmss;    
                                    func     = trans_d07_data_hhmmss;
                                    sprintf(name_1, "时间");
                                }
                                break;

                                case 3: // [04][00][01]{03}
                                {
                                    type     = E_D07_RULER_TYPE_DEMAND_PERIOD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "最大需量周期");
                                }
                                break;
                                case 4: // [04][00][01]{04}
                                {
                                    type     = E_D07_RULER_TYPE_SLIT_TIME;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "滑差时间");
                                }
                                break;
                                case 5: // [04][00][01]{05}
                                {
                                    type     = E_D07_RULER_TYPE_ADJUST_PULSE_WEDTH;
                                    len        = 2;
                                    format     = E_D07_FMT_XXXX;    
                                    func     = trans_d07_data_XXXX;
                                    sprintf(name_1, "滑差时间");
                                }
                                break;
                                
                                case 6: // [04][00][01]{06}
                                {
                                    type     = E_D07_RULER_TYPE_SWITCH_ZONE_TIME;
                                    len        = 5;
                                    format     = E_D07_FMT_YYMMDDhhmm;    
                                    func     = trans_d07_data_YYMMDDhhmm;
                                    sprintf(name_1, "两套时区表切换时间");
                                }
                                break;
                                
                                case 7: // [04][00][01]{07}
                                {
                                    type     = E_D07_RULER_TYPE_SWITCH_PART_TIME;
                                    len        = 5;
                                    format     = E_D07_FMT_YYMMDDhhmm;    
                                    func     = trans_d07_data_YYMMDDhhmm;
                                    sprintf(name_1, "两套日时段表切换时间");

                                }
                                break;
                                
                                default:// [04][00][01]{(1~7)}
                                    return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 2:// [04][00]{02}[*]
                        {
                            switch(ucDi0)
                            {
                                case 1: // [04][00][02]{01}
                                {
                                    type     = E_D07_RULER_TYPE_YEAR_ZONE_P;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "年时区数p(p <= 14)");
                                }
                                break;

                                case 2: // [04][00][02]{02}
                                {
                                    type     = E_D07_RULER_TYPE_DAY_PART_Q;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "日时段表数 q <= 8");
                                }
                                break;
                                
                                case 3: // [04][00][02]{03}
                                {
                                    type     = E_D07_RULER_TYPE_DAY_PART_M;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "日时段数(每日切换数)m <=14");
                                }
                                break;

                                case 4: // [04][00][02]{04}
                                {
                                    type     = E_D07_RULER_TYPE_RATE_K;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "费率数 k <= 63");
                                }
                                break;

                                case 5: // [04][00][02]{05}
                                {
                                    type     = E_D07_RULER_TYPE_HOLIDAY_N;
                                    len        = 2;
                                    format     = E_D07_FMT_NNNN;    
                                    func     = trans_d07_data_NNNN;
                                    sprintf(name_1, "年时区数p(p <= 14)");
                                }
                                break;
                                
                                case 6: // [04][00][02]{06}
                                {
                                    type     = E_D07_RULER_TYPE_HARMONIC_ANALY_TIMES;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "谐波分析次数");
                                }
                                break;
                                
                                default: // [04][00][02]{!(1~6)}
                                    return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 3:// [04][00]{03}[*]
                        {
                            switch(ucDi0)
                            {
                                case 1: // [04][00][03]{01}
                                {
                                    type     = E_D07_RULER_TYPE_AUTO_DISPLAY_SCREENS;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "自动循环显示屏数");
                                }
                                break;
                                
                                case 2: // [04][00][03]{02}
                                {
                                    type     = E_D07_RULER_TYPE_DISPLAY_TIME;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "每屏显示时间");
                                }
                                break;
                                
                                case 3: // [04][00][03]{03}
                                {
                                    type     = E_D07_RULER_TYPE_DISPLAY_POWER_DECIMAL_DIGITS;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "显示电能小数位数");
                                }
                                break;
                                
                                case 4: // [04][00][03]{04}
                                {    
                                    type     = E_D07_RULER_TYPE_DISPLAY_DEMAND_DECIMAL_DIGITS;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "显示最大需量小数位数");
                                }
                                break;
                                case 5: // [04][00][03]{05}
                                {    
                                    type     = E_D07_RULER_TYPE_PRESS_DISPLAY_SCREENS;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "按键循环显示屏数");
                                }
                                break;
                                
                                default: // [04][00][03]{!(1~5)}
                                    return E_D07_ERRO_UNKOWN_ID;
                                
                            }
                        }
                        break;

                        case 4:// [04][00]{04}[*]
                        {
                            switch(ucDi0)
                            {
                                case 1: // [04][00][04]{01}
                                {
                                    type     = E_D07_RULER_TYPE_ADDRESS;
                                    len        = 6;
                                    format     = E_D07_FMT_NN_6;    
                                    func     = trans_d07_data_NN_6;
                                    sprintf(name_1, "按键循环显示屏数");
                                }
                                break;
                                
                                case 2: // [04][00][04]{02}
                                {
                                    type     = E_D07_RULER_TYPE_METER_ID;
                                    len        = 6;
                                    format     = E_D07_FMT_NN_6;    
                                    func     = trans_d07_data_NN_6;
                                    sprintf(name_1, "表号");
                                }
                                break;
                                
                                case 3: // [04][00][04]{03}
                                {
                                    type     = E_D07_RULER_TYPE_ASSETS_CODE;
                                    len        = 32;
                                    format     = E_D07_FMT_NN_32;    
                                    func     = trans_d07_data_NN_32;
                                    sprintf(name_1, "资产管理编码");
                                }
                                break;
                                
                                case 4: // [04][00][04]{04}
                                {
                                    type     = E_D07_RULER_TYPE_RATED_VOLT;
                                    len        = 6;
                                    format     = E_D07_FMT_XX_6;    
                                    func     = trans_d07_data_XX_6;
                                    sprintf(name_1, "额定电压");
                                }
                                break;
                                
                                case 5: // [04][00][04]{05}
                                {
                                    type     = E_D07_RULER_TYPE_RATED_ELEC;
                                    len        = 6;
                                    format     = E_D07_FMT_XX_6;    
                                    func     = trans_d07_data_XX_6;
                                    sprintf(name_1, "额定电流");
                                    
                                }
                                break;
                                
                                case 6: // [04][00][04]{06}
                                {
                                    type     = E_D07_RULER_TYPE_MAX_ELEC;
                                    len        = 6;
                                    format     = E_D07_FMT_XX_6;    
                                    func     = trans_d07_data_XX_6;
                                    sprintf(name_1, "最大电流");
                                }
                                break;
                                
                                case 7: // [04][00][04]{07}
                                {
                                    type     = E_D07_RULER_TYPE_HAVE_ACCURACY_CLASS;
                                    len        = 4;
                                    format     = E_D07_FMT_XX_4;    
                                    func     = trans_d07_data_XX_4;
                                    sprintf(name_1, "有功准确度等级");
                                }
                                break;
                                
                                case 8: // [04][00][04]{08}
                                {
                                    type     = E_D07_RULER_TYPE_NONE_ACCURACY_CLASS;
                                    len        = 4;
                                    format     = E_D07_FMT_XX_4;    
                                    func     = trans_d07_data_XX_4;
                                    sprintf(name_1, "无功准确度等级");
                                }
                                break;
                                
                                case 9: // [04][00][04]{09}
                                {
                                    type     = E_D07_RULER_TYPE_METER_HAVE_CONSTANT;
                                    len        = 3;
                                    format     = E_D07_FMT_XX_3;    
                                    func     = trans_d07_data_XX_3;
                                    sprintf(name_1, "电表有功常数");
                                }
                                break;
                                
                                case 0xA: // [04][00][04]{0A}
                                {
                                    type     = E_D07_RULER_TYPE_METER_NONE_CONSTANT;
                                    len        = 3;
                                    format     = E_D07_FMT_XX_3;    
                                    func     = trans_d07_data_XX_3;
                                    sprintf(name_1, "电表无功常数");
                                }
                                break;
                                
                                case 0xB: // [04][00][04]{0B}
                                {
                                    type     = E_D07_RULER_TYPE_METER_MODEL_NUM;
                                    len        = 10;
                                    format     = E_D07_FMT_XX_10;    
                                    func     = trans_d07_data_XX_10;
                                    sprintf(name_1, "电表型号");
                                }
                                break;
                                
                                case 0xC: // [04][00][04]{0C}
                                {
                                    type     = E_D07_RULER_TYPE_METER_PROD_DATE;
                                    len        = 10;
                                    format     = E_D07_FMT_XX_10;    
                                    func     = trans_d07_data_XX_10;
                                    sprintf(name_1, "生产日期");
                                }
                                break;
                                
                                case 0xD: // [04][00][04]{0D}
                                {
                                    type     = E_D07_RULER_TYPE_METER_RULER_VERSION;
                                    len        = 16;
                                    format     = E_D07_FMT_XX_16;    
                                    func     = trans_d07_data_XX_16;
                                    sprintf(name_1, "生产日期");
                                }
                                break;
                                
                                default:// [04][00][04]{!(1~D)}
                                    return E_D07_ERRO_UNKOWN_ID;

                            }
                        }
                        break;
                        
                        case 5:// [04][00]{05}[*]
                        {
                            if(0xFF == ucDi0) // [04][00][05]{FF}
                            {
                                type     = E_D07_RULER_TYPE_METER_RUN_STWD_BLOCK;
                                len        = 16;
                                format     = E_D07_FMT_XX_XX;    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "电表运行状态字数据块");
                            }
                            else if(ucDi0 >=1 && ucDi0 <= 7) // [04][00][05]{(1~7)}
                            {
                                
                                type     = E_D07_RULER_TYPE_METER_RUN_STWD_1 + (E_D07_RULER_TYPE)(ucDi0-1);
                                len        = 2;
                                format     = E_D07_FMT_XX_XX;    
                                func     = trans_d07_data_XX_XX;
                                sprintf(name_1, "电表运行状态字 %d", ucDi0);
                            }
                            else    // [04][00][05]{!(FF,1~7)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 6:// [04][00]{06}[*]
                        {
                            switch(ucDi0)
                            {
                                case 1:// [04][00][06]{01}
                                {
                                    type     = E_D07_RULER_TYPE_HAVE_COMB_STWD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "有功组合方式特征字");
                                }
                                break;

                                case 2:// [04][00][06]{02}
                                {
                                    type     = E_D07_RULER_TYPE_NONE_COMB_1_STWD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "无功组合方式1特征字");
                                }
                                break;

                                case 3:// [04][00][06]{03}
                                {
                                    type     = E_D07_RULER_TYPE_NONE_COMB_2_STWD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "无功组合方式2特征字");
                                }
                                break;
                                
                                default:// [04][00][06]{!(1~3)}
                                    return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 7:// [04][00]{07}[*]
                        {
                            switch(ucDi0)
                            {
                                case 1:// [04][00][07]{01}
                                {
                                    type     = E_D07_RULER_TYPE_MODEM_IRDA_RATE_STWD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "调制型红外光口通信速率特征字");
                                }
                                break;

                                case 2:// [04][00][07]{02}
                                {
                                    type     = E_D07_RULER_TYPE_TOUCH_IRDA_RATE_STWD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "接触式红外光口通信速率特征字");
                                }
                                break;

                                case 3:// [04][00][07]{03}
                                {
                                    type     = E_D07_RULER_TYPE_PART_1_RATE_STWD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "通信口 1 通信速率特征字");
                                }
                                break;

                                case 4:// [04][00][07]{04}
                                {
                                    type     = E_D07_RULER_TYPE_PART_2_RATE_STWD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "通信口 2 通信速率特征字");
                                }
                                break;

                                case 5:// [04][00][07]{05}
                                {
                                    type     = E_D07_RULER_TYPE_PART_3_RATE_STWD;
                                    len        = 1;
                                    format     = E_D07_FMT_NN;    
                                    func     = trans_d07_data_NN;
                                    sprintf(name_1, "通信口 3 通信速率特征字");
                                }
                                break;
                                default:// [04][00][07]{!(1~3)}
                                    return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 8:// [04][00]{08}[*]
                        {
                            if(1 == ucDi0) // [04][00][08]{01}
                            {
                                type     = E_D07_RULER_TYPE_WEEKDAY_STWD;
                                len        = 1;
                                format     = E_D07_FMT_NN;    
                                func     = trans_d07_data_NN;
                                sprintf(name_1, "周休日特征字");
                            }
                            else if(2 == ucDi0)// [04][00][08]{02}
                            {
                                type     = E_D07_RULER_TYPE_WEEKDAY_TBL;
                                len        = 1;
                                format     = E_D07_FMT_NN;    
                                func     = trans_d07_data_NN;
                                sprintf(name_1, "周休日采用的日时段表号");
                            }
                            else// [04][00][08]{!(1~2)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 9:// [04][00]{09}[*]
                        {
                            if(1 == ucDi0) // [04][00][08]{01}
                            {
                                type     = E_D07_RULER_TYPE_OVERLOAD_MODEL_STWD;
                                len        = 1;
                                format     = E_D07_FMT_NN;    
                                func     = trans_d07_data_NN;
                                sprintf(name_1, "周休日特征字");
                            }
                            else if(2 == ucDi0)// [04][00][08]{02}
                            {
                                type     = E_D07_RULER_TYPE_FREEZE_MODEL_STWD;
                                len        = 1;
                                format     = E_D07_FMT_NN;    
                                func     = trans_d07_data_NN;
                                sprintf(name_1, "周休日采用的日时段表号");
                            }
                            else// [04][00][08]{!(1~2)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 0xA:// [04][00]{0A}[*]
                        {
                            if(1 == ucDi0) // [04][00][0A]{01}
                            {
                                type     = E_D07_RULER_TYPE_OVERLOAD_START_TIME;
                                len        = 4;
                                format     = E_D07_FMT_MMDDhhmm;    
                                func     = trans_d07_data_MMDDhhmm;
                                sprintf(name_1, "负荷记录起始时间");
                            }
                            else if (ucDi0 >= 2 && ucDi0 <= 7) // [04][00][0A]{(2~7)}
                            {
                                type     = E_D07_RULER_TYPE_OVERLOAD_INTRV_TIME_1 + (E_D07_RULER_TYPE)(ucDi0 - 2);
                                len        = 2;
                                format     = E_D07_FMT_NNNN;    
                                func     = trans_d07_data_NNNN;
                                sprintf(name_1, "第 %d 类负荷记录间隔时间", ucDi0 - 1);
                            }
                            else// [04][00][0A]{!(1~7)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;

                        case 0xB:// [04][00]{0B}[*]
                        {
                            if(1 == ucDi0) //[04][00][0B]{01}
                            {
                                type     = E_D07_RULER_TYPE_PAYOFF_DAY_1;
                                len        = 2;
                                format     = E_D07_FMT_DDhh;    
                                func     = trans_d07_data_DDhh;
                                sprintf(name_1, "每月第 1 结算日");
                            }
                            else if(2 == ucDi0) // [04][00][0B]{02}
                            {
                                type     = E_D07_RULER_TYPE_PAYOFF_DAY_2;
                                len        = 2;
                                format     = E_D07_FMT_DDhh;    
                                func     = trans_d07_data_DDhh;
                                sprintf(name_1, "每月第 2 结算日");
                            }
                            else if(3 == ucDi0)// [04][00][0B]{03}
                            {
                                type     = E_D07_RULER_TYPE_PAYOFF_DAY_3;
                                len        = 2;
                                format     = E_D07_FMT_DDhh;    
                                func     = trans_d07_data_DDhh;
                                sprintf(name_1, "每月第 3 结算日");
                            }
                            else// [04][00][0B]{!(1~3)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }            
                        }
                        break;
                        
                        case 0xC:// [04][00]{0C}[*]
                        {
                            if(ucDi0 >= 1 && ucDi0 <= 0xA) // [04][00][0C]{(1~A)}
                            {
                                type     = E_D07_RULER_TYPE_PASSWORD_LV0 + (E_D07_RULER_TYPE)(ucDi0 - 1);
                                len        = 4;
                                format     = E_D07_FMT_NN_4;    
                                func     = trans_d07_data_NN_4;
                                sprintf(name_1, "%d 级密码", ucDi0 -1);
                            }
                            else// [04][00][0C]{!(1~A)}
                            {
                                return E_D07_ERRO_UNKOWN_ID;
                            }
                        }
                        break;
                        
                        case 0xD:// [04][00]{0D}[*]
                        {
                            switch(ucDi0)
                            {
                                case 1:// [04][00][0D]{01}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_A_CONDUCT_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "A相电导系数");
                                }
                                break;
                                case 2:// [04][00][0D]{02}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_A_SUSCEPT_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "A相电纳系数");
                                    
                                }
                                break;
                                case 3:// [04][00][0D]{03}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_A_RESIST_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "A相电阻系数");
                                }
                                break;
                                case 4:// [04][00][0D]{04}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_A_REACTANCE_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "A相电抗系数");
                                }
                                break;
                                case 5:// [04][00][0D]{05}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_B_CONDUCT_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "B相电导系数");
                                }
                                break;
                                
                                case 6:// [04][00][0D]{06}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_B_SUSCEPT_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "B相电纳系数");
                                }
                                break;
                                case 7:// [04][00][0D]{07}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_B_RESIST_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "B相电阻系数");
                                }
                                break;
                                case 8:// [04][00][0D]{08}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_B_REACTANCE_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "B相电抗系数");
                                }
                                break;
                                case 9:// [04][00][0D]{09}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_C_CONDUCT_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "C相电导系数");
                                }
                                break;
                                case 0xA:// [04][00][0D]{0A}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_C_SUSCEPT_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "C相电纳系数");
                                }
                                break;
                                case 0xB:// [04][00][0D]{0B}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_C_RESIST_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "C相电阻系数");
                                }
                                break;
                                case 0xC:// [04][00][0D]{0C}
                                {
                                    type     = E_D07_RULER_TYPE_PHASE_C_REACTANCE_FACTOR;
                                    len        = 2;
                                    format     = E_D07_FMT_N_NNN;    
                                    func     = trans_d07_data_N_NNN;
                                    sprintf(name_1, "C相电抗系数");
                                }
                                break;
                                
                                default:// [04][00][0D]{!(1~C)}
                                    return E_D07_ERRO_UNKOWN_ID;
                                
                            }
                        }
                        break;
                        
                        case 0xE:// [04][00]{0E}[*]
                        {
                            switch(ucDi0)
                            {
                                case 1:// [04][00][0E]{01}
                                {
                                    type     = E_D07_RULER_TYPE_FORTH_HAVE_POWER_MAX;
                                    len        = 3;
                                    format     = E_D07_FMT_NN_NNNN;    
                                    func     = trans_d07_data_NN_NNNN;
                                    sprintf(name_1, "正向有功功率上限值");
                                }
                                break;
                                
                                case 2:// [04][00][0E]{02}
                                {
                                    type     = E_D07_RULER_TYPE_BACK_HAVE_POWER_MAX;
                                    len        = 3;
                                    format     = E_D07_FMT_NN_NNNN;    
                                    func     = trans_d07_data_NN_NNNN;
                                    sprintf(name_1, "反向有功功率上限值");
                                }
                                break;
                                
                                case 3:// [04][00][0E]{03}
                                {
                                    type     = E_D07_RULER_TYPE_VOLT_MAX;
                                    len        = 2;
                                    format     = E_D07_FMT_NNN_N;    
                                    func     = trans_d07_data_NNN_N;
                                    sprintf(name_1, "电压上限值");
                                }
                                break;

                                case 4:// [04][00][0E]{04}
                                {
                                    type     = E_D07_RULER_TYPE_VOLT_MIN;
                                    len        = 2;
                                    format     = E_D07_FMT_NNN_N;    
                                    func     = trans_d07_data_NNN_N;
                                    sprintf(name_1, "电压下限值");
                                }
                                break;
                                
                                default:// [04][00][0E]{!(1~4)}
                                    return E_D07_ERRO_UNKOWN_ID; 
                            }
                        }
                        break;
                        
                        default:
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 1:// [04]{01}[*][*]
                {
                    if(0 == ucDi1) // [04][01]{00}[*]
                    {

                        if(0 == ucDi0)// [04][01][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_DAY_PART_DATA_1;
                            len        = 3*14;
                            format     = E_D07_FMT_MMDDNN_14;    
                            func     = trans_d07_data_MMDDNN_14;
                            sprintf(name_1, "第一套时区表数据");
                                
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 8)// [04][01][00]{(1~8)}
                        {
                            type     = E_D07_RULER_TYPE_DAY_PART_DATA_1_T;
                            len        = 3*14;
                            format     = E_D07_FMT_hhmmNN_14;    
                            func     = trans_d07_data_hhmmNN_14;
                            sprintf(name_1, "第一套第%d日时段表数据", ucDi0);
                        }
                        else// [04][01][00]{!(0~8)}
                        {    
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                    }
                    else            // [04][01]{!(0)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 2:// [04]{02}[*][*]
                {
                    if(0 == ucDi1) // [04][02]{00}[*]
                    {

                        if(0 == ucDi0)// [04][02][00]{00}
                        {
                            type     = E_D07_RULER_TYPE_DAY_PART_DATA_2;
                            len        = 3*14;
                            format     = E_D07_FMT_MMDDNN_14;    
                            func     = trans_d07_data_MMDDNN_14;
                            sprintf(name_1, "第二套时区表数据");
                                
                        }
                        else if(ucDi0 >= 1 && ucDi0 <= 8)// [04][02][00]{(1~8)}
                        {
                            type     = E_D07_RULER_TYPE_DAY_PART_DATA_2_T;
                            len        = 3*14;
                            format     = E_D07_FMT_hhmmNN_14;    
                            func     = trans_d07_data_hhmmNN_14;
                            sprintf(name_1, "第二套第%d日时段表数据", ucDi0);
                        }
                        else// [04][02][00]{!(0~8)}
                        {    
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                    }
                    else            // [04][02]{!(0)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 3:// [04]{03}[*][*]
                {
                    if(0 == ucDi1)
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xFE)// [04][03][00]{(1-FE)}
                        {
                            type     = E_D07_RULER_TYPE_HOLIDAY_DATE_T;
                            len        = 3*14;
                            format     = E_D07_FMT_YYMMDDNN;    
                            func     = trans_d07_data_YYMMDDNN;
                            sprintf(name_1, "第%d公共假日日期及日时段表号", ucDi0);
                        }
                        else // [04][03][00]{!(1-FE)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                    }
                    else // [04][03]{!(00)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 4:// [04]{04}[*][*]
                {
                    if(1 == ucDi1)        //[04][04]{01}[*]
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xFE)// [04][04][01]{(1-FE)}
                        {
                            type     = E_D07_RULER_TYPE_AUTO_DISPLAY_SCREEN_T;
                            len        = 4;
                            format     = E_D07_FMT_NN_4;    
                            func     = trans_d07_data_YYMMDDNN;
                            sprintf(name_1, "自动循环显示第%d屏显示数据项", ucDi0);
                        }
                        else // [04][04][01]{!(1-FE)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                        
                    }
                    else if(2 == ucDi1) // [04][04]{02}[*]
                    {
                        if(ucDi0 >= 1 && ucDi0 <= 0xFE)// [04][04][02]{(1-FE)}
                        {
                            type     = E_D07_RULER_TYPE_PREES_DISPLAY_SCREEN_T;
                            len        = 4;
                            format     = E_D07_FMT_NN_4;    
                            func     = trans_d07_data_YYMMDDNN;
                            sprintf(name_1, "按键循环显示第%d屏显示数据项", ucDi0);
                        }
                        else // [04][04][02]{!(1-FE)}
                        {
                            return E_D07_ERRO_UNKOWN_ID;
                        }
                    }
                    else                 // [04][04]{!(1,2)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;

                case 0x80:// [04]{80}[*][*]
                {
                    if(0 == ucDi1)//[04][80]{00}[*]
                    {
                        switch(ucDi0)
                        {
                            case 1: // [04][80][00]{01}
                            {
                                type     = E_D07_RULER_TYPE_FACTORY_SOFTWARE_VERSION;
                                len        = 32;
                                format     = E_D07_FMT_NN_32;    
                                func     = trans_d07_data_NN_32;
                                sprintf(name_1, "厂家软件版本号");
                            }
                            break;

                            case 2:// [04][80][00]{02}
                            {
                                type     = E_D07_RULER_TYPE_FACTORY_HARDWARE_VERSION;
                                len        = 32;
                                format     = E_D07_FMT_NN_32;    
                                func     = trans_d07_data_NN_32;
                                sprintf(name_1, "厂家硬件版本号");
                            }
                            break;
                            
                            case 3:// [04][80][00]{03}
                            {
                                type     = E_D07_RULER_TYPE_FACTORY_SERIAL_NUM;
                                len        = 32;
                                format     = E_D07_FMT_NN_32;    
                                func     = trans_d07_data_NN_32;
                                sprintf(name_1, "厂家编号");
                            }
                            break;

                            default:
                                return E_D07_ERRO_UNKOWN_ID;
                        }
                    }
                    else //[04][80]{!(0)}[*]
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }

                }
                break;

                default:// [04]{!(0~4,80)}[*][*]
                    return E_D07_ERRO_UNKOWN_ID;
            }
        }
        break;

        /* 表A.6 冻结数据标识编码表 */
        case 5: //{05}[*][*][*]
        {
            if(ucDi0 > 0xC || 0 == ucDi0) // [05][*][*]{!(0x00~0x0C)}
            {
                return E_D07_ERRO_UNKOWN_ID;
            }
            else
            {
                sprintf(strPayOff,"(上%d次)", ucDi0);
            }
                        
            /* 对于表 A.6 相同的数据属性 */
            para.payoff = E_D07_PAYOFF_NULL;
            para.rate    = E_D07_RATE_NULL;
            para.harm    = E_D07_HARM_NULL;
            para.last    = (E_D07_PARA_LAST)ucDi0;
            rdwr         = E_D07_RDWR_READ_ONLY;
            format         = E_D07_FMT_UNKOWN;
            len            = 0;
            func         = NULL;

            switch(ucDi2)
            {
                case 0: // [05]{00}[*][*]
                {
                    switch(ucDi1)
                    {
                        case 0: //[05][00]{00}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_TIME_T;
                            len        = 5;
                            format     = E_D07_FMT_YYMMDDhhmm;    
                            func     = trans_d07_data_YYMMDDhhmm;
                            sprintf(name_1, "定时冻结时间");
                        }
                        break;
                        
                        case 1: //[05][00]{01}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_FORTH_HAVE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "定时冻结正向有功电能数据");
                        }
                        break;

                        case 2: //[05][00]{02}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_BACK_HAVE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "定时冻结反向有功电能数据");
                        }
                        break;
                        case 3: //[05][00]{03}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_COMB_1_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "定时冻结组合无功1电能数据");
                        }
                        break;

                        case 4: //[05][00]{04}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_COMB_2_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "定时冻结组合无功2电能数据");
                        }
                        break;
                        case 5: //[05][00]{05}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_PHASE_1_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "定时冻结第一象限无功电能数据");
                        }
                        break;

                        case 6: //[05][00]{06}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_PHASE_2_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "定时冻结第二象限无功电能数据");
                        }
                        break;

                        case 7: //[05][00]{07}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_PHASE_3_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "定时冻结第三象限无功电能数据");
                        }
                        break;

                        case 8: //[05][00]{08}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_PHASE_4_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "定时冻结第四象限无功电能数据");
                        }
                        break;

                        case 9: //[05][00]{09}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_FORTH_HAVE_DEMAND_T;
                            len        = 8*64;
                            format     = E_D07_FMT_XX_XXXX_YYMMDDhhmm;    
                            func     = trans_d07_data_XX_XXXX_YYMMDDhhmm;
                            sprintf(name_1, "定时冻结正向有功最大需量数据及发生时间数据");
                        }
                        break;
                        
                        case 0xA: //[05][00]{0A}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_BACK_HAVE_DEMAND_T;
                            len        = 8*64;
                            format     = E_D07_FMT_XX_XXXX_YYMMDDhhmm;    
                            func     = trans_d07_data_XX_XXXX_YYMMDDhhmm;
                            sprintf(name_1, "定时冻结反向有功最大需量数据及发生时间数据");
                        }
                        break;

                        case 0x10: //[05][00]{10}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_BLOCK_T;
                            len        = 8*3;
                            format     = E_D07_FMT_XX_XXXX_8;    
                            func     = trans_d07_data_XX_XXXX_8;
                            sprintf(name_1, "定时冻结变量数据");
                        }
                        break;

                        case 0xFF: //[05][00]{FF}[*]
                        {
                            type     = E_D07_RULER_TYPE_FIX_FREZ_BLOCK_T;
                            len        = 0;
                            format     = E_D07_FMT_UNKOWN;    
                            func     = NULL;
                            sprintf(name_1, "定时冻结数据块");
                        }
                        break;
                        
                        default: //[*][*]{!(0~11,FF)}[*]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 1: // [05]{01}[*][*]
                {
                    if(ucDi0 > 3)
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                    
                    switch(ucDi1)
                    {
                        case 0: //[05][01]{00}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_TIME_T;
                            len        = 5;
                            format     = E_D07_FMT_YYMMDDhhmm;    
                            func     = trans_d07_data_YYMMDDhhmm;
                            sprintf(name_1, "瞬时冻结时间");
                        }
                        break;
                        
                        case 1: //[05][01]{01}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_FORTH_HAVE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "瞬时冻结正向有功电能数据");
                        }
                        break;

                        case 2: //[05][01]{02}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_BACK_HAVE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "瞬时冻结反向有功电能数据");
                        }
                        break;
                        case 3: //[05][01]{03}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_COMB_1_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "瞬时冻结组合无功1电能数据");
                        }
                        break;

                        case 4: //[05][01]{04}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_COMB_2_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "瞬时冻结组合无功2电能数据");
                        }
                        break;
                        case 5: //[05][01]{05}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_PHASE_1_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "瞬时冻结第一象限无功电能数据");
                        }
                        break;

                        case 6: //[05][01]{06}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_PHASE_2_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "瞬时冻结第二象限无功电能数据");
                        }
                        break;

                        case 7: //[05][01]{07}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_PHASE_3_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "瞬时冻结第三象限无功电能数据");
                        }
                        break;

                        case 8: //[05][01]{08}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_PHASE_4_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "瞬时冻结第四象限无功电能数据");
                        }
                        break;

                        case 9: //[05][01]{09}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_FORTH_HAVE_DEMAND_T;
                            len        = 8*64;
                            format     = E_D07_FMT_XX_XXXX_YYMMDDhhmm;    
                            func     = trans_d07_data_XX_XXXX_YYMMDDhhmm;
                            sprintf(name_1, "瞬时冻结正向有功最大需量数据及发生时间数据");
                        }
                        break;
                        
                        case 0xA: //[05][01]{0A}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_BACK_HAVE_DEMAND_T;
                            len        = 8*64;
                            format     = E_D07_FMT_XX_XXXX_YYMMDDhhmm;    
                            func     = trans_d07_data_XX_XXXX_YYMMDDhhmm;
                            sprintf(name_1, "瞬时冻结反向有功最大需量数据及发生时间数据");
                        }
                        break;

                        case 0x10: //[05][01]{0A}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_BLOCK_T;
                            len        = 8*3;
                            format     = E_D07_FMT_XX_XXXX_8;    
                            func     = trans_d07_data_XX_XXXX_8;
                            sprintf(name_1, "瞬时冻结变量数据");
                        }
                        break;

                        case 0xFF: //[05][01]{FF}[*]
                        {
                            type     = E_D07_RULER_TYPE_INST_FREZ_BLOCK_T;
                            len        = 0;
                            format     = E_D07_FMT_UNKOWN;    
                            func     = NULL;
                            sprintf(name_1, "瞬时冻结数据块");
                        }
                        break;
                        
                        default: //[*][*]{!(0~11,FF)}[*]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 2: // [05]{02}[*][*]
                {
                   if(ucDi0 > 2)
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                    switch(ucDi1)
                    {
                        case 0: //[05][02]{00}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_TIME_T;
                            len        = 5;
                            format     = E_D07_FMT_YYMMDDhhmm;    
                            func     = trans_d07_data_YYMMDDhhmm;
                            sprintf(name_1, "两套时区表切换时间");
                        }
                        break;
                        
                        case 1: //[05][02]{01}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_FORTH_HAVE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套时区表切换正向有功电能数据");
                        }
                        break;

                        case 2: //[05][02]{02}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_BACK_HAVE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套时区表切换反向有功电能数据");
                        }
                        break;
                        case 3: //[05][02]{03}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_COMB_1_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套时区表切换组合无功1电能数据");
                        }
                        break;

                        case 4: //[05][02]{04}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_COMB_2_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套时区表切换组合无功2电能数据");
                        }
                        break;
                        case 5: //[05][02]{05}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_QUAD_1_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套时区表切换第一象限无功电能数据");
                        }
                        break;

                        case 6: //[05][02]{06}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_QUAD_2_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套时区表切换第二象限无功电能数据");
                        }
                        break;

                        case 7: //[05][02]{07}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_QUAD_3_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套时区表切换第三象限无功电能数据");
                        }
                        break;

                        case 8: //[05][02]{08}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_QUAD_4_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套时区表切换第四象限无功电能数据");
                        }
                        break;

                        case 9: //[05][02]{09}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_FORTH_HAVE_DEMAND_T;
                            len        = 8*64;
                            format     = E_D07_FMT_XX_XXXX_YYMMDDhhmm;    
                            func     = trans_d07_data_XX_XXXX_YYMMDDhhmm;
                            sprintf(name_1, "两套时区表切换正向有功最大需量数据及发生时间数据");
                        }
                        break;
                        
                        case 0xA: //[05][02]{0A}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_BACK_HAVE_DEMAND_T;
                            len        = 8*64;
                            format     = E_D07_FMT_XX_XXXX_YYMMDDhhmm;    
                            func     = trans_d07_data_XX_XXXX_YYMMDDhhmm;
                            sprintf(name_1, "两套时区表切换反向有功最大需量数据及发生时间数据");
                        }
                        break;

                        case 0x10: //[05][02]{0A}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_BLOCK_T;
                            len        = 8*3;
                            format     = E_D07_FMT_XX_XXXX_8;    
                            func     = trans_d07_data_XX_XXXX_8;
                            sprintf(name_1, "两套时区表切换变量数据");
                        }
                        break;

                        case 0xFF: //[05][02]{FF}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_ZONE_BLOCK_T;
                            len        = 0;
                            format     = E_D07_FMT_UNKOWN;    
                            func     = NULL;
                            sprintf(name_1, "两套时区表切换数据块");
                        }
                        break;
                        
                        default: //[*][*]{!(0~11,FF)}[*]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                case 3: // [05]{03}[*][*]
                {
                   if(ucDi0 > 2)
                    {
                        return E_D07_ERRO_UNKOWN_ID;
                    }
                    switch(ucDi1)
                    {
                        case 0: //[05][03]{00}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_TIME_T;
                            len        = 5;
                            format     = E_D07_FMT_YYMMDDhhmm;    
                            func     = trans_d07_data_YYMMDDhhmm;
                            sprintf(name_1, "两套日时段表切换时间");
                        }
                        break;
                        
                        case 1: //[05][03]{01}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_FORTH_HAVE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套日时段表切换正向有功电能数据");
                        }
                        break;

                        case 2: //[05][03]{02}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_BACK_HAVE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套日时段表切换反向有功电能数据");
                        }
                        break;
                        case 3: //[05][03]{03}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_COMB_1_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套日时段表切换组合无功1电能数据");
                        }
                        break;

                        case 4: //[05][03]{04}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_COMB_2_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套日时段表切换组合无功2电能数据");
                        }
                        break;
                        case 5: //[05][03]{05}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_QUAD_1_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套日时段表切换第一象限无功电能数据");
                        }
                        break;

                        case 6: //[05][03]{06}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_QUAD_2_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套日时段表切换第二象限无功电能数据");
                        }
                        break;

                        case 7: //[05][03]{07}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_QUAD_3_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套日时段表切换第三象限无功电能数据");
                        }
                        break;

                        case 8: //[05][03]{08}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_QUAD_4_NONE_POWER_T;
                            len        = 4*64;
                            format     = E_D07_FMT_XXXXXX_XX;    
                            func     = trans_d07_data_XXXXXX_XX;
                            sprintf(name_1, "两套日时段表切换第四象限无功电能数据");
                        }
                        break;

                        case 9: //[05][03]{09}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_FORTH_HAVE_DEMAND_T;
                            len        = 8*64;
                            format     = E_D07_FMT_XX_XXXX_YYMMDDhhmm;    
                            func     = trans_d07_data_XX_XXXX_YYMMDDhhmm;
                            sprintf(name_1, "两套日时段表切换正向有功最大需量数据及发生时间数据");
                        }
                        break;
                        
                        case 0xA: //[05][03]{0A}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_BACK_HAVE_DEMAND_T;
                            len        = 8*64;
                            format     = E_D07_FMT_XX_XXXX_YYMMDDhhmm;    
                            func     = trans_d07_data_XX_XXXX_YYMMDDhhmm;
                            sprintf(name_1, "两套日时段表切换反向有功最大需量数据及发生时间数据");
                        }
                        break;

                        case 0x10: //[05][03]{0A}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_BLOCK_T;
                            len        = 8*3;
                            format     = E_D07_FMT_XX_XXXX_8;    
                            func     = trans_d07_data_XX_XXXX_8;
                            sprintf(name_1, "两套日时段表切换变量数据");
                        }
                        break;

                        case 0xFF: //[05][03]{FF}[*]
                        {
                            type     = E_D07_RULER_TYPE_SWITCH_PART_BLOCK_T;
                            len        = 0;
                            format     = E_D07_FMT_UNKOWN;    
                            func     = NULL;
                            sprintf(name_1, "两套日时段表切换数据块");
                        }
                        break;
                        
                        default: //[*][*]{!(0~11,FF)}[*]
                            return E_D07_ERRO_UNKOWN_ID;
                    }
                }
                break;
                
                default: // [05]{!(0~3)}[*][*]
                    return E_D07_ERRO_UNKOWN_ID;
            }
        }
        break;


        /* 表 A.7负荷记录数据标识编码表 */
        case 6: //{06}[*][*][*]
        {
            if(0 != ucDi1 || ucDi2 > 6 || ucDi0 > 2)
            {
                return E_D07_ERRO_UNKOWN_ID;
            }

            /* 表 A.7 */
            para.payoff = E_D07_PAYOFF_NULL;
            para.rate    = E_D07_RATE_NULL;
            para.harm    = E_D07_HARM_NULL;
            para.last    = E_D07_LAST_NULL;
            rdwr         = E_D07_RDWR_READ_ONLY;
            format         = E_D07_FMT_UNKOWN;
            len            = 0;
            func         = NULL;
            
            if(0 == ucDi2) //[06]{00}[00][*]
            {
                switch(ucDi0)
                {
                    case 0://[06][00][00]{00}
                    {
                        type     = E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD;
                        len        = 1;
                        format     = E_D07_FMT_NN;    
                        func     = trans_d07_data_NN;
                        sprintf(name_1, "给定时间记录块");    
                    }
                    break;

                    case 1://[06][00][00]{01}
                    {
                        type     = E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD;
                        len        = 6;
                        format     = E_D07_FMT_YYMMDDhhmmNN;    
                        func     = trans_d07_data_YYMMDDhhmmNN;
                        sprintf(name_1, "给定时间记录块");    
                    }
                    break;

                    case 2://[06][00][00]{02}
                    {
                        type     = E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD;
                        len        = 1;
                        format     = E_D07_FMT_NN;    
                        func     = trans_d07_data_NN; // 固定值01
                        sprintf(name_1, "给定时间记录块");    
                    }
                    break;

                    default://[06][00][00]{!(0~2)}
                        return E_D07_ERRO_UNKOWN_ID;
                        
                }
            }
            else  //[06]{(1~6)}[00][*]
            {
                switch(ucDi0) //[06][(1~6)][00]{02}
                {
                    case 0: 
                    {
                        type     = E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD_1 + (E_D07_RULER_TYPE)(ucDi2 - 1);
                        len        = 1;
                        format     = E_D07_FMT_NN;    
                        func     = trans_d07_data_NN;
                        sprintf(name_1, "第%d类负荷最早记录块", ucDi2);    
                    }
                    break;

                    case 1://[06][(1~6)][00]{02}
                    {
                        type     = E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD_1 + (E_D07_RULER_TYPE)(ucDi2 - 1);
                        len        = 6;
                        format     = E_D07_FMT_YYMMDDhhmmNN;    
                        func     = trans_d07_data_YYMMDDhhmmNN;
                        sprintf(name_1, "第%d类负荷给定时间记录块", ucDi2);    
                    }
                    break;

                    case 2://[06][(1~6)][00]{02}
                    {
                        type     = E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD_1 + (E_D07_RULER_TYPE)(ucDi2 - 1);
                        len        = 1;
                        format     = E_D07_FMT_NN;    
                        func     = trans_d07_data_NN; // 固定值01
                        sprintf(name_1, "第%d类负荷最近一个记录块", ucDi2);    
                    }
                    break;

                    default://[06][(1~6)][00]{!(0~2)}
                        return E_D07_ERRO_UNKOWN_ID;
                        
                }
            }
        }
        break;

        /* 用户扩展 */
        

        default: //{!(0~6)[*][*][*]}
            return  E_D07_ERRO_UNKOWN_ID;
        
    }

    // 合成最后的名字
    sprintf(name, "%s%s", strPayOff, name_1);

    // 结果数据输出 
    outRulerInfo->type = type;
    outRulerInfo->rdwr = rdwr;
    outRulerInfo->format = format;
    outRulerInfo->func = func;
    
    outRulerInfo->para.payoff = para.payoff;
    outRulerInfo->para.rate = para.rate;
    outRulerInfo->len = len;
    sprintf(outRulerInfo->name, "%s", name);
    
    return E_D07_OK;
}


/*****************************************************************************
 函 数 名  : d07_bcd2str
 功能描述  : 将长度为len的BCD码转换为字符串
 输入参数  : const char* inBCD  
             char *outStr       
             INT32 len            
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
void d07_bcd2str(const char* inBCD, char *outStr, INT32 len)
{
    INT32 i = 0,j= 0;
    char c1 = 0,c0 = 0;

    if(NULL == inBCD || NULL == outStr || len < 1)
    {
        return;
    }
    
    for(i = len - 1,j = 0; i >= 0; i--)
    {
        c0 = inBCD[i] & 0xF;
        c1 = (inBCD[i] >> 4) & 0xF;
        if(c1 >= 0 && c1 <= 9)
        {
           outStr[j++] = c1 + '0';    
        }
        else
        {
           outStr[j++] = c1 + 'A';
        }
        
        if(c0 >= 0 && c0 <= 9)
        {
           outStr[j++] = c0 + '0';            
        }
        else
        {
           outStr[j++] = c0 + 'A';
        }    
    }
    
}

/* 函数实现 */

/*************************************************
Function:       str2bcd
Description:    将长度为len的字符串pstr将为BCD码输出pbcd
Calls:          
Called By:      
Input:          

Output:        

Return:        
Others:       pstr的长度是pbcd的二倍 

*************************************************/
void d07_str2bcd(const char *pstr, UINT8 *pbcd, INT32 len)
{
    UINT8 tmpValue;
    INT32 i;
    INT32 j;
    INT32 m;
    INT32 sLen;
    
    sLen = strlen(pstr);
    for(i = 0; i < sLen; i++)
    {
        if((pstr[i] < '0')
            ||((pstr[i] > '9') && (pstr[i] < 'A')) 
            ||((pstr[i] > 'F') && (pstr[i] < 'a')) 
            ||(pstr[i] > 'f'))
        {
            sLen=i;
            break;
        }
    }

    sLen = (sLen <= (len * 2) ) ?  sLen : sLen * 2;
    memset((void *)pbcd, 0x00, len);
    
    for(i=sLen-1, j=0, m=0; (i>=0)&&(m<len); i--, j++)
    {
        if((pstr[i] >= '0') && (pstr[i] <= '9'))
        {
            tmpValue=pstr[i] - '0';
        }
        else if((pstr[i] >= 'A') && (pstr[i] <= 'F'))
        {
            tmpValue=pstr[i] - 'A' + 0x0A;
        }
        else if((pstr[i] >= 'a') && (pstr[i] <= 'f'))
        {
            tmpValue=pstr[i] - 'a' + 0x0A;
        }
        else
        {
            tmpValue=0;
        }
        
        if((j%2)==0)
        {
            pbcd[m]=tmpValue;        
        }
        else
        {
            pbcd[m++]|=(tmpValue << 4);
        }
        
        if((tmpValue==0) && (pstr[i] != '0'))
        {
            break;
        }
    }
}

INT32 d07_pow_r(INT32 value,INT32 x)
{
    INT32 tmp = 1;
    while(x > 0){
        tmp *= value;
        x--;
    }
    return tmp;
}

/*************************************************
  Function:       BCD2Decimal
  Description:    将bcd码转换为十进制数
  Calls:          
  Called By:      
  Input:          UINT8* pBcd, UINT8 len
                
  Output:         
                    
  Return:         返回包的总长度值
  Others:         最大支持位数 20, 最大数为 18446744073709551616
*************************************************/
double BCD2Decimal(UINT8* pBcd, INT32 len)
{
      INT32  i = 0;
    double dDeci = 0.0;
    
    for(i = len - 1; i >= 0; i--)
    {
        dDeci += (pBcd[i] >> 4) * 10;
        dDeci += (pBcd[i] & 0x0f);
        if(i != 0)
            dDeci *= 100;
    
    }    
    
    return dDeci;
}

/*****************************************************************************
 函 数 名  : d07_remove_dot
 功能描述  : 将inArray转出为长度为len的数组
 输入参数  : INT32 len         
             char*inArray    
             char *outArray  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
INT32 d07_remove_dot(INT32 len, char*inArray, char *outArray)
{
    INT32 i = 0;
    INT32 j = 0;

    if(NULL == inArray || NULL == outArray)
    {
        return E_D07_ERRO_NULL;
    }
    
    for(i = 0; j < len; i++)
    {
        if((inArray[i] < '0')
            ||((inArray[i] > '9') && (inArray[i] < 'A')) 
            ||((inArray[i] > 'F') && (inArray[i] < 'a')) 
            ||(inArray[i] > 'f'))
        {
            
            continue;
        }

        outArray[j++]= inArray[i];

    }

    outArray[j] = 0;

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : d07_add_dot
 功能描述  : 将inArray 中长度为len的内容加上一个小数点，位置是左数第dotpos
 输入参数  : INT32 len         
             char*inArray    
             char *outArray  
             INT32 dotpos      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月21日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
INT32 d07_add_dot(INT32 len, char*inArray, char *outArray,  INT32 dotpos)
{
    INT32 i = 0;
    INT32 j = 0;
    INT32 p = 0;
    
    if(inArray == NULL || outArray == NULL)
    {
        return E_D07_ERRO_NULL;
    }
    
    for(p = 0; i < len; p++)
    {
        if(p == dotpos)
        {
            outArray[j] = '.';
            j++;
        }
        else
        {
            outArray[j++] = inArray[i++];
        }
    }
    
    return E_D07_OK;
    
}
/*************************************************
  Function:       d07_bcd_to_double
  Description:    dlt645 2007将bcd码转换为十进制数浮点
  Calls:          
  Called By:      
  Input:          UINT8* pBcd, UINT8 len
                    UINT8 LeftDigit 小数点左移位是
                
  Output:         
                    
  Return:         返回包的总长度值
  Others:         最大支持位数 20, 最大数为 18446744073709551616
                  如果首位数为0x8X 则这个是负数
*************************************************/
double d07_bcd_to_double(UINT8* pBcd, INT32 len,INT32 LeftDigit)
{

    UINT8 ucPlus;
    INT32 flag = 1;
    double d = 0.0;
    ucPlus = pBcd[len-1];

    if(ucPlus & 0x80)
    {
        pBcd[len-1] &= 0x0F;
        flag = -1;
    }

    d = BCD2Decimal(pBcd, len);
    d = d/d07_pow_r(10, LeftDigit);
    d = d*flag;

    return d;

}

/*****************************************************************************
 函 数 名  : d07_str_to_double
 功能描述  : 将pstr转换为有效位数为digits，小数点位置在左数第rightDigitPos位的浮点数
 输入参数  : UINT8* pStr        
             INT32 digits         
             INT32 rightDigitPos  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月22日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
double d07_str_to_double(char* pStr, INT32 digits, INT32 rightDigitPos)
{    
    double dRet = 0.0;
    char buf[20] = {0};

    if(NULL == pStr)
    {
        return 0.0;
    }
    
    d07_add_dot(digits, pStr, buf, rightDigitPos);

    dRet = atof(buf);
    return dRet;
}
    

/*****************************************************************************
 函 数 名  : trans_d07_data_XX_2
 功能描述  : XX_2
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_XX_2(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    return trans_d07_data_XXXX(flag, user, frame);
}
/*****************************************************************************
 函 数 名  : trans_d07_data_XX_3
 功能描述  : XX_3
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_XX_3(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    return trans_d07_data_XXXXXX(flag, user, frame);
}

/*****************************************************************************
 函 数 名  : trans_d07_data_XX_4
 功能描述  : XX_4
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_XX_4(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    return trans_d07_data_XXXXXXXX(flag, user, frame);
}

/*****************************************************************************
 函 数 名  : trans_d07_data_XX_6
 功能描述  : XX_6结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_XX_6(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[13] = {0};

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {
        memcpy((void*)acTmp, (void*)user, 12);
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 13);
            printf("\t(XX_6) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 12);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 6);        
        memcpy((void*)user, (void*)acTmp, 12);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "XX_6\t=  %s\n", acTmp);
        }
       #endif

    }
    
    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_XX_8
 功能描述  : XX_8结构转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_XX_8(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[17] = {0};

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }    

    if(E_D07_TRANS_U2F == flag)
    {
        memcpy((void*)acTmp, (void*)user, 18);
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 18);
            printf("\t(XX_8) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 16);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 8);        
        memcpy((void*)user, (void*)acTmp, 16);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "XX_8\t=  %s\n", acTmp);
        }
       #endif

    }
    
    return E_D07_OK;
}
/*****************************************************************************
 函 数 名  : trans_d07_data_XX_10
 功能描述  : XX_10结构
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_XX_10(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[21] = {0};

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {
        memcpy((void*)acTmp, (void*)user, 20);
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 20);
            printf("\t(XX_10) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 20);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 10);        
        memcpy((void*)user, (void*)acTmp, 20);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "XX_10\t=  %s\n", acTmp);
        }
       #endif

    }
    
    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_XX_16
 功能描述  : XX_16结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_XX_16(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[33] = {0};

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {
        memcpy((void*)acTmp, (void*)user, 32);
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 32);
            printf("\t(XX_16) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 32);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 16);        
        memcpy((void*)user, (void*)acTmp, 32);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "XX_16\t=  %s\n", acTmp);
        }
       #endif

    }
    
    return E_D07_OK;
}


eD07Err trans_d07_data_XXXX(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[5] = {0};
    UINT16 usXXXX = 0;
    UINT16 * pUserData = NULL;
    
    if(NULL == user || NULL == frame )
    {
        return E_D07_ERRO_NULL;
    }
    printf("trans_d07_data_XXXX\n");

    if(E_D07_TRANS_U2F == flag)
    {        
        usXXXX = *(UINT16*)user;
        usXXXX = usXXXX & 0xFFFF;
        sprintf(acTmp, "%04d", usXXXX);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 5);
            printf("\t(XXXX) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 4);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 2);        
        pUserData = (UINT16*)user;
        *pUserData = atol(acTmp) & 0xFFFF;
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,"XXXX\t=  %d\n", *pUserData);
        }
       #endif
    }

    return E_D07_OK;
}

eD07Err trans_d07_data_XXXXXX(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    UINT32 ulXXXXXX = 0;
    char acTmp[7] = {0};
    UINT32 * pulUserData = NULL;
    
#ifdef D07_DEBUG_ON
    INT32 outLen = 0;
#endif    
    if(E_D07_TRANS_U2F == flag)
    {
        
        ulXXXXXX = *(UINT32*)user;
        ulXXXXXX = ulXXXXXX & 0xFFFFFF;
        
        sprintf(acTmp, "%06d", ulXXXXXX);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 7);
            printf("\t(XXXXXX) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 6);
    }
    else // frame to user
    {        
        d07_bcd2str(frame, acTmp, 6);        
        pulUserData = (UINT32*)user;
        * pulUserData = atol(acTmp);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_out_data_07,"XXXXXX\t=  %s\n", acTmp);
            outLen = sprintf(g_out_data_07 + outLen," XXXXXX\t=  %06d\n", *pulUserData);
        }
       #endif

    }

    return E_D07_OK;

}

// X.XXX
eD07Err trans_d07_data_X_XXX(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    float fltX_XXX = 0.0;
    char acTmp[6] = {0};
    char strOut[6] = {0};
    char acX_XXX[6] = {0};
    float * pfUserData = NULL;
    
#ifdef D07_DEBUG_ON
    INT32 outLen = 0;
#endif    

    if(E_D07_TRANS_U2F == flag)
    {        
        fltX_XXX = *(float*)user;
        sprintf(acTmp, "%1.3f", fltX_XXX);
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 6);
            printf("\t(X.XXX) << ");
            scanf("%s",(CHAR*)&acTmp);
        }
        #endif
        
        d07_remove_dot(4,acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 4);
    }
    else // frame to user
    {

        d07_bcd2str(frame, acTmp, 2);
        d07_add_dot(4, acTmp, acX_XXX, 1);    
        pfUserData = (float*)user;
        * pfUserData = atof(acX_XXX);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_out_data_07,"XXXX  =  %s\n", acTmp);
            outLen = sprintf(g_out_data_07 + outLen," X.XXX =  %1.3f\n", *pfUserData);
        }
        #endif
    }

    return E_D07_OK;
}

// XX.XXXX
eD07Err trans_d07_data_XX_XXXX(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    float fltXX_XXXX = 0.0;
    char acTmp[8] = {0};
    char strOut[8] = {0};    
    char acXX_XXXX[8] = {0};
    float * pfUserData = NULL;
    
#ifdef D07_DEBUG_ON
    INT32 outLen = 0;
#endif    

    if(E_D07_TRANS_U2F == flag)
    {
        
        fltXX_XXXX = *(float*)user;
        sprintf(acTmp, "%2.4f", fltXX_XXXX);
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 8);
            printf("\t(XX.XXXX) << ");
            scanf("%s",(CHAR*)&acTmp);
        }
        #endif
        
        d07_remove_dot(6,acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 6);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 3);
        d07_add_dot(6, acTmp, acXX_XXXX, 2);    
        pfUserData = (float*)user;
        * pfUserData = atof(acXX_XXXX);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_out_data_07,"XXXXXX  =  %s\n", acTmp);
            outLen = sprintf(g_out_data_07 + outLen," XX.XXXX =  %2.4f\n", *pfUserData);
        }
        #endif
    }

    return E_D07_OK;

}



eD07Err trans_d07_data_XX_XXXX_8(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_XX_XXXX_8 * pDataUser = NULL;
    char acTmp[60] = {0};
    char strOut[48] = {0};
    INT32 i = 0;
    
    #ifdef D07_DEBUG_ON
    S_D07_XX_XXXX_8 sXX_XXXX = {0};
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_XX_XXXX_8*)user;
 
    if(E_D07_TRANS_U2F == flag)
    {
          #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 60);
        
             sXX_XXXX.fXX_XXXXX[0] = 12.3456;
             sXX_XXXX.fXX_XXXXX[1] = 12.3456;
             sXX_XXXX.fXX_XXXXX[2] = 12.3456;
             sXX_XXXX.fXX_XXXXX[3] = 12.3456;
             sXX_XXXX.fXX_XXXXX[4] = 12.3456; 
             sXX_XXXX.fXX_XXXXX[5] = 12.3456;
             sXX_XXXX.fXX_XXXXX[6] = 12.3456;
             sXX_XXXX.fXX_XXXXX[7] = 12.3456;

            pDataUser = (S_D07_XX_XXXX_8 *)&sXX_XXXX;
        }
        #endif
        
        sprintf(acTmp, 
                 "%2.4f%2.4f%2.4f%2.4f%2.4f%2.4f%2.4f%2.4f", 
                 pDataUser->fXX_XXXXX[0],
                 pDataUser->fXX_XXXXX[1],
                 pDataUser->fXX_XXXXX[2],
                 pDataUser->fXX_XXXXX[3],
                 pDataUser->fXX_XXXXX[4],
                 pDataUser->fXX_XXXXX[5],
                 pDataUser->fXX_XXXXX[6],
                 pDataUser->fXX_XXXXX[7]);
        
        d07_remove_dot(48,acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 48);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 24);
    
        pDataUser->fXX_XXXXX[0] = d07_str_to_double(&acTmp[i],    6, 2);
        pDataUser->fXX_XXXXX[1] = d07_str_to_double(&acTmp[i+=6], 6, 2);
        pDataUser->fXX_XXXXX[2] = d07_str_to_double(&acTmp[i+=6], 6, 2);
        pDataUser->fXX_XXXXX[3] = d07_str_to_double(&acTmp[i+=6], 6, 2);
        pDataUser->fXX_XXXXX[4] = d07_str_to_double(&acTmp[i+=6], 6, 2);
        pDataUser->fXX_XXXXX[5] = d07_str_to_double(&acTmp[i+=6], 6, 2);
        pDataUser->fXX_XXXXX[6] = d07_str_to_double(&acTmp[i+=6], 6, 2);
        pDataUser->fXX_XXXXX[7] = d07_str_to_double(&acTmp[i+=6], 6, 2);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,                 
                    "fXX_XXXXX[8] = \n%2.4f\n%2.4f\n%2.4f\n%2.4f\n%2.4f\n%2.4f\n%2.4f\n%2.4f\n", 
                     pDataUser->fXX_XXXXX[0],
                     pDataUser->fXX_XXXXX[1],
                     pDataUser->fXX_XXXXX[2],
                     pDataUser->fXX_XXXXX[3],
                     pDataUser->fXX_XXXXX[4],
                     pDataUser->fXX_XXXXX[5],
                     pDataUser->fXX_XXXXX[6],
                     pDataUser->fXX_XXXXX[7]);
        }
        #endif
    }

    return E_D07_OK;

}

//XXX.XXX
eD07Err trans_d07_data_XXX_XXX(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    float fltXXX_XXX = 0.0;
    char acTmp[8] = {0};
    char strOut[8] = {0};
    char acXXX_XXX[8] = {0};    
    float * pfUserData  = NULL;
#ifdef D07_DEBUG_ON
    INT32 outLen = 0;
#endif

    if(E_D07_TRANS_U2F == flag)
    {
        
        fltXXX_XXX = *(float*)user;
        sprintf(acTmp, "%6.3f", fltXXX_XXX);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 8);
            printf("\t(XXX.XXX) << ");
            scanf("%s",(CHAR*)&acTmp);
        }
        #endif
        
        d07_remove_dot(6,acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 6);
    }
    else // frame to user
    {        
        d07_bcd2str(frame, acTmp, 3);
        d07_add_dot(6, acTmp, acXXX_XXX, 3);    
        pfUserData = (float*)user;
        * pfUserData = atof(acXXX_XXX);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_out_data_07,"XXXXXX\t=  %s\n", acTmp);
            outLen = sprintf(g_out_data_07 + outLen," XXX.XXX\t=  %6.3f\n", *pfUserData);
        }
        #endif
    }

    return E_D07_OK;

}

eD07Err trans_d07_data_YYMMDDhhmmss_XXX_XXX_YYMMDDhhmmss(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_YYMMDDhhmmss_XX_XXXX_YYMMDDhhmmss *pDataUser = (S_D07_YYMMDDhhmmss_XX_XXXX_YYMMDDhhmmss *)user;
    char str[300] = {0};
    char strOut[31] = {0};
    INT32 i = 0;
    
#ifdef D07_DEBUG_ON
    INT32 outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
        
    if(E_D07_TRANS_U2F == flag)
    {

        //测试数据
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            pDataUser->sBgnTime.YY = 11;
            pDataUser->sBgnTime.MM = 11;
            pDataUser->sBgnTime.DD = 11;
            pDataUser->sBgnTime.hh = 11;
            pDataUser->sBgnTime.mm = 11;
            pDataUser->sBgnTime.ss = 11;
            //sEndTime
            pDataUser->sEndTime.YY = 11;
            pDataUser->sEndTime.MM = 11;
            pDataUser->sEndTime.DD = 11;
            pDataUser->sEndTime.hh = 11;
            pDataUser->sEndTime.mm = 11;
            pDataUser->sEndTime.ss = 11;
            //fXXX_XXX
            pDataUser->fXXX_XXX = 111.111;
        }
        #endif
        
        sprintf(str, 
                "%02d%02d%02d%02d%02d%02d"        //sBgnTime
                "%02d%02d%02d%02d%02d%02d"        //sEndTime
                "%06.3f",        //fXXX_XXX
                //sBgnTime
                pDataUser->sBgnTime.YY,
                pDataUser->sBgnTime.MM,
                pDataUser->sBgnTime.DD,
                pDataUser->sBgnTime.hh,
                pDataUser->sBgnTime.mm,
                pDataUser->sBgnTime.ss,
                //sEndTime
                pDataUser->sEndTime.YY,
                pDataUser->sEndTime.MM,
                pDataUser->sEndTime.DD,
                pDataUser->sEndTime.hh,
                pDataUser->sEndTime.mm,
                pDataUser->sEndTime.ss,
                //fXXX_XXX
                pDataUser->fXXX_XXX);

        d07_remove_dot(30,str, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 30);
        
    }
    else// frame to user interface
    {
        
        d07_bcd2str(frame, str, 15);
        pDataUser->sBgnTime.YY = d07_str_to_num(&str[i],    2);
        pDataUser->sBgnTime.MM = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sBgnTime.DD = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sBgnTime.hh = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sBgnTime.mm = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sBgnTime.ss = d07_str_to_num(&str[i+=2], 2);
        //sEndTime
        pDataUser->sEndTime.YY = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.MM = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.DD = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.hh = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.mm = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.ss = d07_str_to_num(&str[i+=2], 2);
        //fXXX_XXX
        pDataUser->fXXX_XXX = d07_str_to_double(&str[i+=2], 6, 2);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
        
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                "sBgnTime = %02d %02d %02d %02d %02d %02d\n"        //sBgnTime
                "sEndTime = %02d %02d %02d %02d %02d %02d\n"        //sEndTime
                "fXXX_XXX  = %06.3f\n",        //sDemand
                //sBgnTime
                pDataUser->sBgnTime.YY,
                pDataUser->sBgnTime.MM,
                pDataUser->sBgnTime.DD,
                pDataUser->sBgnTime.hh,
                pDataUser->sBgnTime.mm,
                pDataUser->sBgnTime.ss,
                //sEndTime
                pDataUser->sEndTime.YY,
                pDataUser->sEndTime.MM,
                pDataUser->sEndTime.DD,
                pDataUser->sEndTime.hh,
                pDataUser->sEndTime.mm,
                pDataUser->sEndTime.ss,
                //sDemand
                pDataUser->fXXX_XXX);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_XXXXXX_2
 功能描述  : XXXXXX_2结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_XXXXXX_2(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_XXXXXX_2 * pUserData = (S_D07_XXXXXX_2*)user;
    char acTmp[13] = {0};
#ifdef D07_DEBUG_ON
    INT32 outLen = 0;
 #endif
 
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    if(E_D07_TRANS_U2F == flag)
    {
        sprintf(acTmp, "%06ld%06ld", 
                        (long int)((pUserData->ulTimes)   & 0xFFFFFF),
                        (long int)((pUserData->ulMinutes) & 0xFFFFFF));

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, (char)0x00, 12);
            printf("\t(XXXXXX_2) << ");
            scanf("%s", (char*)&acTmp);
            
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 12);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 6);        
        pUserData->ulTimes   = d07_str_to_num(&acTmp[0],  6);
        pUserData->ulMinutes = d07_str_to_num(&acTmp[6],  6);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {            
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen += sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", pUserData->ulTimes);
            outLen += sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", pUserData->ulMinutes);    
        }
       #endif

    }

    return E_D07_OK;

}



eD07Err trans_d07_data_XXXXXX_6(E_D07_TRANS_FLG flag, char *user, char *frame)
{    
    S_D07_XXXXXX_6 *pXXXXXX_6 = NULL;
    char acTmp[37] = {0};
    S_D07_XXXXXX_6 * pUserData = NULL;
#ifdef D07_DEBUG_ON
    INT32 outLen = 0;
#endif
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {
    
        pXXXXXX_6 = (S_D07_XXXXXX_6*)user;
    
        sprintf(acTmp, "%06ld%06ld%06ld%06ld%06ld%06ld", 
                        (long int)((pXXXXXX_6->a_times)   & 0xFFFFFF),
                        (long int)((pXXXXXX_6->a_seconds) & 0xFFFFFF),
                        (long int)((pXXXXXX_6->b_times)   & 0xFFFFFF),
                        (long int)((pXXXXXX_6->b_seconds) & 0xFFFFFF),
                        (long int)((pXXXXXX_6->c_times)   & 0xFFFFFF),
                        (long int)((pXXXXXX_6->c_seconds) & 0xFFFFFF));

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, (char)0x00, 37);
            printf("\t(XXXXXX_6) << ");
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 18);
    }
    else // frame to user
    {    
        d07_bcd2str(frame, acTmp, 18);        
        pUserData = (S_D07_XXXXXX_6*)user;
        pUserData->a_times   = d07_str_to_num(&acTmp[0],  6);
        pUserData->a_seconds = d07_str_to_num(&acTmp[6],  6);
        pUserData->b_times     = d07_str_to_num(&acTmp[12], 6);
        pUserData->b_seconds = d07_str_to_num(&acTmp[18], 6);
        pUserData->c_times   = d07_str_to_num(&acTmp[24], 6);
        pUserData->c_seconds = d07_str_to_num(&acTmp[30], 6);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {            
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen += sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", pUserData->a_times);
            outLen += sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", pUserData->a_seconds);
            outLen += sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", pUserData->b_times);
            outLen += sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", pUserData->b_seconds);
            outLen += sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", pUserData->c_times);
            outLen += sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", pUserData->c_seconds);
        }
       #endif

    }

    return E_D07_OK;

}

eD07Err trans_d07_data_XXXXXXXX(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    UINT32 ulXXXXXXXX = 0.0;
    char acTmp[9] = {0};
    UINT32 * pulUserData = NULL;
    
#ifdef D07_DEBUG_ON
    INT32 outLen = 0;
#endif 

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }


    if(E_D07_TRANS_U2F == flag)
    {        
        ulXXXXXXXX = *(UINT32*)user;
        sprintf(acTmp, "%08d", ulXXXXXXXX);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(XXXXXXXX) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else // frame to user
    {
        
        d07_bcd2str(frame, acTmp, 4);        
        pulUserData = (UINT32*)user;
        * pulUserData = atol(acTmp);
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {            
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_out_data_07,"XXXXXXXX\t=  %s\n", acTmp);
            outLen = sprintf(g_out_data_07 + outLen," XXXXXXXX\t=  %06ld\n", *pulUserData);
        }
       #endif

    }

    return E_D07_OK;

}

// XX.XX
eD07Err trans_d07_data_XX_XX(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    float fltXX_XX = 0.0;
    char acTmp[6] = {0};
    char strOut[6] = {0};
    char acXX_XX[6] = {0};
    float * pfUserData  = NULL;
    
    #ifdef D07_DEBUG_ON
    INT32 outLen = 0;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }


    if(E_D07_TRANS_U2F == flag)
    {
        
        fltXX_XX = *(float*)user;
    
        sprintf(acTmp, "%2.2f", fltXX_XX);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 6);
            printf("\t(XX.XX) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
    
        d07_remove_dot(4,acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 4);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 2);        
        d07_add_dot(4, acTmp, acXX_XX, 2);    
        pfUserData = (float*)user;
        * pfUserData = atof(acXX_XX);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {            
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_out_data_07,"XXXX\t=  %s\n", acTmp);
            outLen = sprintf(g_out_data_07 + outLen," XXX.X\t=  %2.2f\n", *pfUserData);
        }
       #endif

    }

    return E_D07_OK;

}

eD07Err trans_d07_data_XXX_X(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[6] = {0};
    char strOut[6] = {0};
    char acX_XXX[6] = {0};
    float fltXXX_X = 0.0;
    float * pfUserData = NULL;

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {
        fltXXX_X = *(float*)user;
        sprintf(acTmp, "%3.1f", fltXXX_X);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 6);
            printf("\t(XXX.X) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
    
        d07_remove_dot(4,acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 4);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 2);        
        d07_add_dot(4, acTmp, acX_XXX, 3);    
        pfUserData = (float*)user;
        *pfUserData = atof(acX_XXX);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                "XXXX\t=  %s\n"
                "XXX.X\t=  %4.1f\n",
                acTmp,
                *pfUserData);
        }
       #endif

    }
    return E_D07_OK;

}

eD07Err trans_d07_data_XXXXXX_XX(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char str[9] = {0};
    double dd;
    
    
#ifdef D07_DEBUG_ON
    char c;
    
    INT32 outLen = 0;
    char strOut[8] = {0};
    INT32 i = 0;
#endif    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            //float fInput = 0.0;
            
            printf("\t(XXXXXX.XX) << ");
            scanf("%s", (char*)&str);
            d07_remove_dot(8,str, strOut);

            d07_str2bcd(strOut, (UINT8*)frame, 8);
            
            for(i = 0; i < 4; i++)
            {    
                c = frame[i];
                //printf("%d%d",  (c >> 4)&0xF , c & 0xF);
            }
            //printf("\n");
            dd = BCD2Decimal((UINT8*)frame,4);

            printf("\tdouble = %6.2f\n\n", dd/100);
        }

        #endif
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, str,4);
        dd = d07_bcd_to_double((UINT8*)frame, 4, 2);
        *(float*)user = (float)dd;
       // printf("dd = %6.2f\n", dd);
        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen = sprintf(g_out_data_07,"XXXXXXXX\t=  %s\n", str);
            outLen = sprintf(g_out_data_07 + outLen," XXXXXX.XX\t=  %6.2f\n", dd);
        }
        #endif
    }
    return E_D07_OK;
}



eD07Err trans_d07_data_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm *pDataUser = NULL;
    char str[300] = {0};
    char strOut[41] = {0};
    INT32 i = 0;
    
    #ifdef D07_DEBUG_ON
    INT32 outLen = 0;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        #ifdef D07_DEBUG_ON
        //测试数据
        if(D07_ON == g_need_pack_07)
        {
            pDataUser->sBgnTime.YY = 11;
            pDataUser->sBgnTime.MM = 11;
            pDataUser->sBgnTime.DD = 11;
            pDataUser->sBgnTime.hh = 11;
            pDataUser->sBgnTime.mm = 11;
            pDataUser->sBgnTime.ss = 11;
            //sEndTime
            pDataUser->sEndTime.YY = 11;
            pDataUser->sEndTime.MM = 11;
            pDataUser->sEndTime.DD = 11;
            pDataUser->sEndTime.hh = 11;
            pDataUser->sEndTime.mm = 11;
            pDataUser->sEndTime.ss = 11;
            //sDemand
            pDataUser->sDemand.fXX_XXXX = 11.1111;
            pDataUser->sDemand.YY = 11;
            pDataUser->sDemand.MM = 11;
            pDataUser->sDemand.DD = 11;
            pDataUser->sDemand.hh = 11;
            pDataUser->sDemand.mm = 11;
        }
        #endif
        
        sprintf(str, 
                "%02d%02d%02d%02d%02d%02d"        //sBgnTime
                "%02d%02d%02d%02d%02d%02d"        //sEndTime
                "%06.4f%02d%02d%02d%02d%02d",        //sDemand
                //sBgnTime
                pDataUser->sBgnTime.YY,
                pDataUser->sBgnTime.MM,
                pDataUser->sBgnTime.DD,
                pDataUser->sBgnTime.hh,
                pDataUser->sBgnTime.mm,
                pDataUser->sBgnTime.ss,
                //sEndTime
                pDataUser->sEndTime.YY,
                pDataUser->sEndTime.MM,
                pDataUser->sEndTime.DD,
                pDataUser->sEndTime.hh,
                pDataUser->sEndTime.mm,
                pDataUser->sEndTime.ss,
                //sDemand
                pDataUser->sDemand.fXX_XXXX,
                pDataUser->sDemand.YY,
                pDataUser->sDemand.MM,
                pDataUser->sDemand.DD,
                pDataUser->sDemand.hh,
                pDataUser->sDemand.mm);

        d07_remove_dot(40,str, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 40);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, str, 20);
        pDataUser->sBgnTime.YY = d07_str_to_num(&str[i],    2);
        pDataUser->sBgnTime.MM = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sBgnTime.DD = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sBgnTime.hh = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sBgnTime.mm = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sBgnTime.ss = d07_str_to_num(&str[i+=2], 2);
        //sEndTime
        pDataUser->sEndTime.YY = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.MM = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.DD = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.hh = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.mm = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sEndTime.ss = d07_str_to_num(&str[i+=2], 2);
        //sDemand
        pDataUser->sDemand.fXX_XXXX = d07_str_to_double(&str[i+=2], 6, 2);
        pDataUser->sDemand.YY = d07_str_to_num(&str[i+=6], 2);
        pDataUser->sDemand.MM = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sDemand.DD = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sDemand.hh = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sDemand.mm = d07_str_to_num(&str[i+=2], 2);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                "sBgnTime = %02d %02d %02d %02d %02d %02d\n"        //sBgnTime
                "sEndTime = %02d %02d %02d %02d %02d %02d\n"        //sEndTime
                "sDemand  = %06.4f %02d %02d %02d %02d %02d\n",        //sDemand
                //sBgnTime
                pDataUser->sBgnTime.YY,
                pDataUser->sBgnTime.MM,
                pDataUser->sBgnTime.DD,
                pDataUser->sBgnTime.hh,
                pDataUser->sBgnTime.mm,
                pDataUser->sBgnTime.ss,
                //sEndTime
                pDataUser->sEndTime.YY,
                pDataUser->sEndTime.MM,
                pDataUser->sEndTime.DD,
                pDataUser->sEndTime.hh,
                pDataUser->sEndTime.mm,
                pDataUser->sEndTime.ss,
                //sDemand
                pDataUser->sDemand.fXX_XXXX,
                pDataUser->sDemand.YY,
                pDataUser->sDemand.MM,
                pDataUser->sDemand.DD,
                pDataUser->sDemand.hh,
                pDataUser->sDemand.mm);
        }
        #endif
    }

    return E_D07_OK;
}

//YYMMDDhhmmss_YYMMDDhhmmss
eD07Err trans_d07_data_YYMMDDhhmmss_2(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_YYMMDDhhmmss_2 *pYYMMDDhhmmss_2 = NULL;
    char acTmp[24] = {0};
    INT32 i = 0;
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pYYMMDDhhmmss_2 = (S_D07_YYMMDDhhmmss_2*)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        sprintf(acTmp, 
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d%02d%02d", 
                pYYMMDDhhmmss_2->sBegin.YY,
                pYYMMDDhhmmss_2->sBegin.MM,
                pYYMMDDhhmmss_2->sBegin.DD,
                pYYMMDDhhmmss_2->sBegin.hh,
                pYYMMDDhhmmss_2->sBegin.mm,
                pYYMMDDhhmmss_2->sBegin.ss,
                pYYMMDDhhmmss_2->sEnd.YY,
                pYYMMDDhhmmss_2->sEnd.MM,
                pYYMMDDhhmmss_2->sEnd.DD,
                pYYMMDDhhmmss_2->sEnd.hh,
                pYYMMDDhhmmss_2->sEnd.mm,
                pYYMMDDhhmmss_2->sEnd.ss);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 24);
            printf("\t(YYMMDDhhmmss_YYMMDDhhmmss) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 24);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 12);        
        
        pYYMMDDhhmmss_2->sBegin.YY = d07_str_to_num(&acTmp[i], 2);
        pYYMMDDhhmmss_2->sBegin.MM = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sBegin.DD = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sBegin.hh = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sBegin.mm = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sBegin.ss = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sEnd.YY = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sEnd.MM = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sEnd.DD = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sEnd.hh = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sEnd.mm = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss_2->sEnd.ss = d07_str_to_num(&acTmp[i+=2], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "YYMMDDhhmmss_1 = %02d%02d%02d%02d%02d%02d"
                    "YYMMDDhhmmss_2 = %02d%02d%02d%02d%02d%02d", 
                    pYYMMDDhhmmss_2->sBegin.YY,
                    pYYMMDDhhmmss_2->sBegin.MM,
                    pYYMMDDhhmmss_2->sBegin.DD,
                    pYYMMDDhhmmss_2->sBegin.hh,
                    pYYMMDDhhmmss_2->sBegin.mm,
                    pYYMMDDhhmmss_2->sBegin.ss,
                    pYYMMDDhhmmss_2->sEnd.YY,
                    pYYMMDDhhmmss_2->sEnd.MM,
                    pYYMMDDhhmmss_2->sEnd.DD,
                    pYYMMDDhhmmss_2->sEnd.hh,
                    pYYMMDDhhmmss_2->sEnd.mm,
                    pYYMMDDhhmmss_2->sEnd.ss);
        }
       #endif
    }

    return E_D07_OK;
}
/*****************************************************************************
 函 数 名  : trans_d07_data_YYMMDDhhmm
 功能描述  : YYMMDDhhmm
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_YYMMDDhhmm(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_YYMMDDhhmm *pYYMMDDhhmmNN = NULL;
    char acTmp[10] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pYYMMDDhhmmNN = (S_D07_YYMMDDhhmm*)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        sprintf(acTmp, 
                "%02d%02d%02d%02d%02d", 
                pYYMMDDhhmmNN->YY,
                pYYMMDDhhmmNN->MM,
                pYYMMDDhhmmNN->DD,
                pYYMMDDhhmmNN->hh,
                pYYMMDDhhmmNN->mm);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 12);
            printf("\t(YYMMDDhhmm) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 10);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 5);        
        
        pYYMMDDhhmmNN->YY = d07_str_to_num(&acTmp[i], 2);
        pYYMMDDhhmmNN->MM = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmNN->DD = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmNN->hh = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmNN->mm = d07_str_to_num(&acTmp[i+=2], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "YY = %02d\nMM = %02d\nDD = %02d\nhh = %02d\nmm = %02d\n", 
                    pYYMMDDhhmmNN->YY,
                    pYYMMDDhhmmNN->MM,
                    pYYMMDDhhmmNN->DD,
                    pYYMMDDhhmmNN->hh,
                    pYYMMDDhhmmNN->mm);
        }
       #endif
    }

    return E_D07_OK;
}

eD07Err trans_d07_data_YYMMDDhhmmNN(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_YYMMDDhhmmNN *pYYMMDDhhmmNN = NULL;
    char acTmp[12] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pYYMMDDhhmmNN = (S_D07_YYMMDDhhmmNN*)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        sprintf(acTmp, 
                "%02d%02d%02d%02d%02d%02d", 
                pYYMMDDhhmmNN->YY,
                pYYMMDDhhmmNN->MM,
                pYYMMDDhhmmNN->DD,
                pYYMMDDhhmmNN->hh,
                pYYMMDDhhmmNN->mm,
                pYYMMDDhhmmNN->NN);
#ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 12);
            printf("\t(YYMMDDhhmmNN) << ");
            scanf("%s",(char*)&acTmp);
        }
#endif
        d07_str2bcd(acTmp, (UINT8*)frame, 12);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 6);        
        
        pYYMMDDhhmmNN->YY = d07_str_to_num(&acTmp[i], 2);
        pYYMMDDhhmmNN->MM = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmNN->DD = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmNN->hh = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmNN->mm = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmNN->NN = d07_str_to_num(&acTmp[i+=2], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "YY = %02d\nMM = %02d\nDD = %02d\nhh = %02d\nmm = %02d\nNN = %02d\n", 
                    pYYMMDDhhmmNN->YY,
                    pYYMMDDhhmmNN->MM,
                    pYYMMDDhhmmNN->DD,
                    pYYMMDDhhmmNN->hh,
                    pYYMMDDhhmmNN->mm,
                    pYYMMDDhhmmNN->NN);
        }
     #endif
    }

    return E_D07_OK;
}


eD07Err trans_d07_data_YYMMDDhhmmss(E_D07_TRANS_FLG flag, char *user, char *frame)
{

    S_D07_YYMMDDhhmmss *pYYMMDDhhmmss = NULL;
    char acTmp[12] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pYYMMDDhhmmss = (S_D07_YYMMDDhhmmss*)user;

    if(E_D07_TRANS_U2F == flag)
    {
        sprintf(acTmp, 
                "%02d%02d%02d%02d%02d%02d", 
                pYYMMDDhhmmss->YY,
                pYYMMDDhhmmss->MM,
                pYYMMDDhhmmss->DD,
                pYYMMDDhhmmss->hh,
                pYYMMDDhhmmss->mm,
                pYYMMDDhhmmss->ss);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 12);
            printf("\t(YYMMDDhhmmss) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 12);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 6);        
    
        pYYMMDDhhmmss->YY = d07_str_to_num(&acTmp[i], 2);
        pYYMMDDhhmmss->MM = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss->DD = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss->hh = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss->mm = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDhhmmss->ss = d07_str_to_num(&acTmp[i+=2], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "YY = %02d\nMM = %02d\nDD = %02d\nhh = %02d\nmm = %02d\nss = %02d\n", 
                    pYYMMDDhhmmss->YY,
                    pYYMMDDhhmmss->MM,
                    pYYMMDDhhmmss->DD,
                    pYYMMDDhhmmss->hh,
                    pYYMMDDhhmmss->mm,
                    pYYMMDDhhmmss->ss);
        }
       #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_NN
 功能描述  : 数据结构NN转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_NN(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[3] = {0};    
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    printf("trans_d07_data_NN\n");
    
    
    if(E_D07_TRANS_U2F == flag)
    {
        sprintf(acTmp, "%02d", *user);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 3);
            printf("\t(NN) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 2);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 1);        
        *user = d07_str_to_num(&acTmp[0], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,"NN = %02d\n", *user);
        }
       #endif
    }

    return E_D07_OK;
}


eD07Err trans_d07_data_NNNN(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    return trans_d07_data_XXXX(flag, user, frame);
}


eD07Err trans_d07_data_NN_4(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    return trans_d07_data_XXXXXXXX(flag, user, frame);
}

eD07Err trans_d07_data_NN_6(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    return trans_d07_data_XX_6(flag, user, frame);
}

/*****************************************************************************
 函 数 名  : trans_d07_data_NN_32
 功能描述  : NN_32转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月30日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_NN_32(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[65] = {0};
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {
        memcpy((void*)acTmp, (void*)user, 64);
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 64);
            printf("\t(NN_32) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 64);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 32);        
        memcpy((void*)user, (void*)acTmp, 64);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "NN_32\t=  %s\n", acTmp);
        }
       #endif

    }
    
    return E_D07_OK;
}

// N.NNN
eD07Err trans_d07_data_N_NNN(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[6]   = {0};
    char acX_XXX[6] = {0};
    float fltX_XXX = 0.0;
    char strOut[6] = {0};
    float * pfUserData = NULL;

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    if(E_D07_TRANS_U2F == flag)
    {
        
        fltX_XXX = *(float*)user;
        sprintf(acTmp, "%4.3f", fltX_XXX);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 6);
            printf("\t(NN.NNN) << ");
            scanf("%s",(CHAR*)&acTmp);
        }
        #endif
        
        d07_remove_dot(4, acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 4);
    }
    else // frame to user
    {
        
        d07_bcd2str(frame, acTmp, 2);
        d07_add_dot(4, acTmp, acX_XXX, 1);    
        pfUserData = (float*)user;
        * pfUserData = atof(acX_XXX);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,"NNNN  =  %s\n N.NNN =  %4.3f\n", acTmp,*pfUserData);
        }
        #endif
    }

    return E_D07_OK;

}

/*****************************************************************************
 函 数 名  : trans_d07_data_NN_NNNN
 功能描述  : NN.NNNN结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月30日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_NN_NNNN(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[8] = {0};
    char strOut[6] = {0};
    float* pfNN_NNNN = (float*)user;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    if(E_D07_TRANS_U2F == flag)
    {
        sprintf(acTmp, "%6.4f", *pfNN_NNNN);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 8);
            printf("\t(NN.NNNN) << ");
            scanf("%s",(CHAR*)&acTmp);
        }
        #endif
        
        d07_remove_dot(6, acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 6);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 3);
        *pfNN_NNNN = d07_str_to_double(&acTmp[0], 6, 2);
        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,"NN.NNNN  =  %6.4f\n", *pfNN_NNNN);
        }
        #endif
    }

    return E_D07_OK;
}


/*****************************************************************************
 函 数 名  : trans_d07_data_NNN_N
 功能描述  : NNN.N结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月30日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_NNN_N(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[5] = {0};
    char strOut[4] = {0};
    float* pfNNN_N = (float*)user;

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {        
        sprintf(acTmp, "%4.1f", *pfNNN_N);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 5);
            printf("\t(NNN.N) << ");
            scanf("%s",(CHAR*)&acTmp);
        }
        #endif
        
        d07_remove_dot(4, acTmp, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 4);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 2);
        printf("acTmp = %s\n", acTmp);
        *pfNNN_N = (float)d07_str_to_double((char*)&acTmp[0], 4, 3);

        
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,"NNN.N  =  %4.1f\n", *pfNNN_N);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_MMDDNN_14
 功能描述  : MMDDNN_14结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月30日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_MMDDNN_14(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[84] = {0};
    UINT8 *pData = (UINT8*)user;
    UINT8 *pArray = (UINT8*)acTmp;
    INT32 j = -2;
    INT32 i = 0;
    
#ifdef D07_DEBUG_ON
    INT32 len = 0;
    S_D07_MMDDNN_14 *pMMDDNN = NULL;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    if(E_D07_TRANS_U2F == flag)
    {
        for(i = 0; i < 84; i++)
        {
            sprintf((char*)&pArray[i], "%02d", (UINT8)*(pData+i));
        }

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 84);
             for(i = 0; i < 84; i++)
             {
                 sprintf(pArray+i, "%02d", 0x12);
             }
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 84);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 42);    
        
        for(i = 0; i < 42; i++ )
        {
            *(pData + i) = d07_str_to_num(&acTmp[j+=2], 2);
        }
    
        #ifdef D07_DEBUG_ON
        pMMDDNN = (S_D07_MMDDNN_14*)user;
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            
            for(i = 0; i < 14; i++)
            {
                len += sprintf(g_out_data_07 + len,
                                "MMDDNN[%d] : MM = %02d\nDD = %02d\nNN = %02d\n", 
                                i+1,
                                pMMDDNN->MMDDNN[i].MM,
                                pMMDDNN->MMDDNN[i].DD,
                                pMMDDNN->MMDDNN[i].NN);
                
            }
        }
       #endif
    }
    return E_D07_OK;
}

eD07Err trans_d07_data_hhmmNN_14(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    char acTmp[84] = {0};
    INT32 i = 0;
    INT32 j = -2;
    UINT8 *pData = (UINT8*)user;
    UINT8 *pArray = (UINT8*)acTmp;
#ifdef D07_DEBUG_ON
    S_D07_hhmmNN_14 *phhmmNN = NULL;
    INT32 len = 0;
#endif
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    if(E_D07_TRANS_U2F == flag)
    {
        for(i = 0; i < 84; i++)
        {
            sprintf((char*)(pArray+i), "%02d", (UINT8)*(pData+i));
        }

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 84);
             for(i = 0; i < 84; i++)
             {
                 sprintf((char*)(pArray+i), "%02d", 0x12);
             }
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 84);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 42);    
        
        for(i = 0; i < 42; i++ )
        {
            *(pData + i) = d07_str_to_num(&acTmp[j+=2], 2);
        }
    
        #ifdef D07_DEBUG_ON
        phhmmNN = (S_D07_hhmmNN_14*)user;
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            
            for(i = 0; i < 14; i++)
            {
                len += sprintf(g_out_data_07 + len,
                                "phhmmNN[%d] : mm = %02d\nhh = %02d\nss = %02d\n", 
                                i+1,
                                phhmmNN->hhmmNN[i].hh,
                                phhmmNN->hhmmNN[i].mm,
                                phhmmNN->hhmmNN[i].NN);
                
            }
        }
       #endif
    }
    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_hhmmss
 功能描述  : hhmmss结构转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_hhmmss(E_D07_TRANS_FLG flag, char *user, char *frame)
{    
    S_D07_hhmmss *phhmmss = (S_D07_hhmmss*)user;
    char acTmp[7] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    if(E_D07_TRANS_U2F == flag)
    {
        
        sprintf(acTmp, 
                "%02d%02d%02d", 
                phhmmss->hh,
                phhmmss->mm,
                phhmmss->ss);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 7);
            printf("\t(hhmmss) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 6);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 3);        
        
        phhmmss->hh = d07_str_to_num(&acTmp[i], 2);
        phhmmss->mm = d07_str_to_num(&acTmp[i+=2], 2);
        phhmmss->ss = d07_str_to_num(&acTmp[i+=2], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "mm = %02d\nhh = %02d\nss = %02d\n", 
                    phhmmss->mm,
                    phhmmss->hh,
                    phhmmss->ss);
        }
       #endif
    }
    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_DDhh
 功能描述  : DDhh结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月29日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_DDhh(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_DDhh *pDDhh = NULL;
    char acTmp[5] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    pDDhh = (S_D07_DDhh*)user;
    if(E_D07_TRANS_U2F == flag)
    {        
        sprintf(acTmp, 
                "%02d%02d", 
                pDDhh->DD,
                pDDhh->hh);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 5);
            printf("\t(DDhh) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame,4);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 2);        
        
        pDDhh->DD = d07_str_to_num(&acTmp[i], 2);
        pDDhh->hh = d07_str_to_num(&acTmp[i+=2], 2);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "DD = %02d\nhh = %02d\n", 
                    pDDhh->DD,
                    pDDhh->hh);
        }
       #endif
    }
    return E_D07_OK;
}

//MMDDhhmm
eD07Err trans_d07_data_MMDDhhmm(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_MMDDhhmm *pMMDDhhmm = NULL;
    char acTmp[10] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pMMDDhhmm = (S_D07_MMDDhhmm*)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        
        sprintf(acTmp, 
                "%02d%02d%02d%02d", 
                pMMDDhhmm->MM,
                pMMDDhhmm->DD,
                pMMDDhhmm->hh,
                pMMDDhhmm->mm);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(MMDDhhmm) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 4);        
    
        pMMDDhhmm->MM = d07_str_to_num(&acTmp[i], 2);
        pMMDDhhmm->DD = d07_str_to_num(&acTmp[i+=2], 2);
        pMMDDhhmm->hh = d07_str_to_num(&acTmp[i+=2], 2);
        pMMDDhhmm->mm = d07_str_to_num(&acTmp[i+=2], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "MM = %02d\nDD = %02d\nhh = %02d\nmm = %02d\n", 
                    pMMDDhhmm->MM,
                    pMMDDhhmm->DD,
                    pMMDDhhmm->hh,
                    pMMDDhhmm->mm);
        }
       #endif
    }

    return E_D07_OK;
}

//YYMMDDWW 结构转换
eD07Err trans_d07_data_YYMMDDWW(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_YYMMDDWW *pYYMMDDWW = NULL;
    char acTmp[10] = {0};
    INT32 i = 0;
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pYYMMDDWW = (S_D07_YYMMDDWW*)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        
        sprintf(acTmp, 
                "%02d%02d%02d%02d", 
                pYYMMDDWW->YY,
                pYYMMDDWW->MM,
                pYYMMDDWW->DD,
                pYYMMDDWW->WW);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(YYMMDDWW) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 4);        

        pYYMMDDWW->YY = d07_str_to_num(&acTmp[i], 2);
        pYYMMDDWW->MM = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDWW->DD = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDWW->WW = d07_str_to_num(&acTmp[i+=2], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "YY = %02d\nMM = %02d\nDD = %02d\nWW = %02d\n", 
                    pYYMMDDWW->YY,
                    pYYMMDDWW->MM,
                    pYYMMDDWW->DD,
                    pYYMMDDWW->WW);
        }
       #endif
    }

    return E_D07_OK;
}

//YYMMDDNN
eD07Err trans_d07_data_YYMMDDNN(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_YYMMDDNN *pYYMMDDNN = NULL;
    char acTmp[10] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pYYMMDDNN = (S_D07_YYMMDDNN*)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        sprintf(acTmp, 
                "%02d%02d%02d%02d", 
                pYYMMDDNN->YY,
                pYYMMDDNN->MM,
                pYYMMDDNN->DD,
                pYYMMDDNN->NN);

        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void*)acTmp, 0x00, 9);
            printf("\t(YYMMDDNN) << ");
            scanf("%s",(char*)&acTmp);
        }
        #endif
        d07_str2bcd(acTmp, (UINT8*)frame, 8);
    }
    else // frame to user
    {
        d07_bcd2str(frame, acTmp, 4);        
    
        pYYMMDDNN->YY = d07_str_to_num(&acTmp[i], 2);
        pYYMMDDNN->MM = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDNN->DD = d07_str_to_num(&acTmp[i+=2], 2);
        pYYMMDDNN->NN = d07_str_to_num(&acTmp[i+=2], 2);
    
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07,
                    "YY = %02d\nMM = %02d\nDD = %02d\nNN = %02d\n", 
                    pYYMMDDNN->YY,
                    pYYMMDDNN->MM,
                    pYYMMDDNN->DD,
                    pYYMMDDNN->NN);
        }
       #endif
    }

    return E_D07_OK;
}


// XX.XXXXYYMMDDhhmm
eD07Err trans_d07_data_XX_XXXX_YYMMDDhhmm(E_D07_TRANS_FLG flag, char *user, char *frame)
{
    S_D07_XX_XXXX_YYMMDDhhmm *pDataUser = NULL;
    char str[17] = {0};
    char strOut[16] = {0};
    char acXX_XXXX[8] = {0};
    
#ifdef D07_DEBUG_ON
        INT32 outLen = 0;
#endif

    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    pDataUser = (S_D07_XX_XXXX_YYMMDDhhmm *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        
        sprintf(str, "%2.4f%d%d%d%d%d",
            pDataUser->fXX_XXXX,
            pDataUser->YY,
            pDataUser->MM,
            pDataUser->DD,
            pDataUser->hh,
            pDataUser->mm);
    
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            printf("\t(XX.XXXXYYMMDDhhmm) << ");
            scanf("%s", (char*)&str);
        }
        #endif
        
        d07_remove_dot(16,str, strOut);
        d07_str2bcd(strOut, (UINT8*)frame, 16);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, str,8);
        d07_add_dot(6, str, acXX_XXXX, 2);        
        pDataUser->fXX_XXXX = atof(acXX_XXXX);
        pDataUser->YY = d07_str_to_num(&str[6],2);
        pDataUser->MM = d07_str_to_num(&str[8],2);
        pDataUser->DD = d07_str_to_num(&str[10],2);
        pDataUser->hh = d07_str_to_num(&str[12],2);
        pDataUser->mm = d07_str_to_num(&str[14],2);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            outLen += sprintf(g_out_data_07 + outLen, "XX.XXXX =  %2.4f\n", pDataUser->fXX_XXXX);
            outLen += sprintf(g_out_data_07 + outLen, "      YY =   %02d\n", pDataUser->YY);
            outLen += sprintf(g_out_data_07 + outLen, "      MM =   %02d\n", pDataUser->MM);
            outLen += sprintf(g_out_data_07 + outLen, "      DD =   %02d\n", pDataUser->DD);
            outLen += sprintf(g_out_data_07 + outLen, "      hh =   %02d\n", pDataUser->hh);
            outLen += sprintf(g_out_data_07 + outLen, "      mm =   %02d\n", pDataUser->mm);
        }
        #endif
    }

    return E_D07_OK;
}
/*****************************************************************************
 函 数 名  : trans_d07_data_recd_loss_less_volt
 功能描述  : 失压或欠压记录数据结构用户侧到帧侧转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月22日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_loss_less_volt(E_D07_TRANS_FLG flag, char *user, char *frame)
{

    S_D07_LESS_LOSS_VOLT *pDataUser = NULL;

    #define BUF_SIZE     300
    #define BUF_OUT_BCD_SIZE 131
    #define BUF_OUT_SIZE    262
    INT32 i = 0;
    char strUsr[BUF_SIZE] = {0};
    char strBcd[BUF_SIZE] = {0};
    
    #ifdef D07_DEBUG_ON
    S_D07_LESS_LOSS_VOLT sTest ;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_LESS_LOSS_VOLT *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            
            pDataUser = (S_D07_LESS_LOSS_VOLT*)&sTest;
            
            sTest.sBgnTime.YY = 0x07;
            sTest.sBgnTime.MM = 0x07;
            sTest.sBgnTime.DD = 0x07;
            sTest.sBgnTime.hh = 0x07;
            sTest.sBgnTime.mm = 0x07;
            sTest.sBgnTime.ss = 0x07;
            
            sTest.sEndTime.YY = 0x08;
            sTest.sEndTime.MM = 0x08;
            sTest.sEndTime.DD = 0x08;
            sTest.sEndTime.hh = 0x08;
            sTest.sEndTime.mm = 0x08;
            sTest.sEndTime.ss = 0x08;
            
            //sPowerTotal
            sTest.sPowerTotal.dForthHavePowerIcr = 123456.78;
            sTest.sPowerTotal.dBackHavePowerIcr  = 123456.78;
            sTest.sPowerTotal.dCombNone1PowerIcr = 123456.78;
            sTest.sPowerTotal.dCombNone2PowerIcr = 123456.78;
            //sPowerA
            sTest.sPowerA.dForthHavePowerIcr = 123456.78;
            sTest.sPowerA.dBackHavePowerIcr = 123456.78;
            sTest.sPowerA.dCombNone1PowerIcr = 123456.78;
            sTest.sPowerA.dCombNone2PowerIcr = 123456.78;
            //sFactorA
            sTest.sFactorA.fVolt = 123.4;
            sTest.sFactorA.fElec= 123.456;
            sTest.sFactorA.fHavePowerRate = 12.3456;
            sTest.sFactorA.fNonePowerRate= 12.3456;
            sTest.sFactorA.fPowerRateFactor = 1.234;
            //sPowerB
            sTest.sPowerB.dForthHavePowerIcr= 123456.78;
            sTest.sPowerB.dBackHavePowerIcr= 123456.78;
            sTest.sPowerB.dCombNone1PowerIcr= 123456.78;
            sTest.sPowerB.dCombNone2PowerIcr= 123456.78;
            //sFactorB
            sTest.sFactorB.fVolt = 123.4;
            sTest.sFactorB.fElec= 123.456;
            sTest.sFactorB.fHavePowerRate = 12.3456;
            sTest.sFactorB.fNonePowerRate= 12.3456;
            sTest.sFactorB.fPowerRateFactor = 1.234;
            //sPowerA
            sTest.sPowerC.dForthHavePowerIcr= 123456.78;
            sTest.sPowerC.dBackHavePowerIcr= 123456.78;
            sTest.sPowerC.dCombNone1PowerIcr= 123456.78;
            sTest.sPowerC.dCombNone2PowerIcr= 123456.78;
            //sFactorA
            sTest.sFactorC.fVolt = 123.4;
            sTest.sFactorC.fElec= 123.456;
            sTest.sFactorC.fHavePowerRate = 12.3456;
            sTest.sFactorC.fNonePowerRate= 12.3456;
            sTest.sFactorC.fPowerRateFactor = 1.234;
            // sAtime
            sTest.sAtime.dAtimeTotal= 123456.78;
            sTest.sAtime.dAtimePhaseA= 123456.78;
            sTest.sAtime.dAtimePhaseB= 123456.78;
            sTest.sAtime.dAtimePhaseC = 123456.78;

        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"         //sBgnTime
                        "%02d%02d%02d%02d%02d%02d"        //end time
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerTotal
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerA
                        "%05.1f%07.3f%07.4f%07.4f%05.3f"//sFactorA
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerB
                        "%05.1f%07.3f%07.4f%07.4f%05.3f"//sFactorB
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerC
                        "%05.1f%07.3f%07.4f%07.4f%05.3f"//sFactorC
                        "%09.2f%09.2f%09.2f%09.2f",        //sAtime
                        //sBgnTime
                        pDataUser->sBgnTime.YY,
                        pDataUser->sBgnTime.MM,
                        pDataUser->sBgnTime.DD,    
                        pDataUser->sBgnTime.hh,
                        pDataUser->sBgnTime.mm,
                        pDataUser->sBgnTime.ss,
                        //sEndTime
                        pDataUser->sEndTime.YY,
                        pDataUser->sEndTime.MM,
                        pDataUser->sEndTime.DD,    
                        pDataUser->sEndTime.hh,
                        pDataUser->sEndTime.mm,
                        pDataUser->sEndTime.ss,
                        //sPowerTotal
                        pDataUser->sPowerTotal.dForthHavePowerIcr,
                        pDataUser->sPowerTotal.dBackHavePowerIcr,
                        pDataUser->sPowerTotal.dCombNone1PowerIcr,
                        pDataUser->sPowerTotal.dCombNone2PowerIcr,
                        //sPowerA
                        pDataUser->sPowerA.dForthHavePowerIcr,
                        pDataUser->sPowerA.dBackHavePowerIcr,
                        pDataUser->sPowerA.dCombNone1PowerIcr,
                        pDataUser->sPowerA.dCombNone2PowerIcr,
                        //sFactorA
                        pDataUser->sFactorA.fVolt,
                        pDataUser->sFactorA.fElec,
                        pDataUser->sFactorA.fHavePowerRate,
                        pDataUser->sFactorA.fNonePowerRate,
                        pDataUser->sFactorA.fPowerRateFactor,
                        //sPowerB
                        pDataUser->sPowerB.dForthHavePowerIcr,
                        pDataUser->sPowerB.dBackHavePowerIcr,
                        pDataUser->sPowerB.dCombNone1PowerIcr,
                        pDataUser->sPowerB.dCombNone2PowerIcr,
                        //sFactorB
                        pDataUser->sFactorB.fVolt,
                        pDataUser->sFactorB.fElec,
                        pDataUser->sFactorB.fHavePowerRate,
                        pDataUser->sFactorB.fNonePowerRate,
                        pDataUser->sFactorB.fPowerRateFactor,
                        //sPowerA
                        pDataUser->sPowerC.dForthHavePowerIcr,
                        pDataUser->sPowerC.dBackHavePowerIcr,
                        pDataUser->sPowerC.dCombNone1PowerIcr,
                        pDataUser->sPowerC.dCombNone2PowerIcr,
                        //sFactorA
                        pDataUser->sFactorC.fVolt,
                        pDataUser->sFactorC.fElec,
                        pDataUser->sFactorC.fHavePowerRate,
                        pDataUser->sFactorC.fNonePowerRate,
                        pDataUser->sFactorC.fPowerRateFactor,
                        // sAtime
                        pDataUser->sAtime.dAtimeTotal,
                        pDataUser->sAtime.dAtimePhaseA,
                        pDataUser->sAtime.dAtimePhaseB,
                        pDataUser->sAtime.dAtimePhaseC);//297
        
        d07_remove_dot(BUF_OUT_SIZE,strUsr, strBcd);
        d07_str2bcd(strBcd, (UINT8*)frame, BUF_OUT_SIZE);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr,BUF_OUT_BCD_SIZE);

        pDataUser->sBgnTime.YY = d07_str_to_num(&strUsr[i], 2);
        pDataUser->sBgnTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        
        pDataUser->sEndTime.YY = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sPowerTotal

        pDataUser->sPowerTotal.dForthHavePowerIcr = d07_str_to_double(&strUsr[i+=2],8,6);
        pDataUser->sPowerTotal.dBackHavePowerIcr  = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerTotal.dCombNone1PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerTotal.dCombNone2PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPowerA
        pDataUser->sPowerA.dForthHavePowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dBackHavePowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dCombNone1PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dCombNone2PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sFactorA
        pDataUser->sFactorA.fVolt = d07_str_to_double(&strUsr[i+=8], 4, 3);
        pDataUser->sFactorA.fElec= d07_str_to_double(&strUsr[i+=4], 6, 3);
        pDataUser->sFactorA.fHavePowerRate = d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorA.fNonePowerRate= d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorA.fPowerRateFactor = d07_str_to_double(&strUsr[i+=6], 4, 1);
        //sPowerB
        pDataUser->sPowerB.dForthHavePowerIcr= d07_str_to_double(&strUsr[i+=4], 8, 6);
        pDataUser->sPowerB.dBackHavePowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dCombNone1PowerIcr= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dCombNone2PowerIcr= d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sFactorB
        pDataUser->sFactorB.fVolt = d07_str_to_double(&strUsr[i+=8], 4, 3);
        pDataUser->sFactorB.fElec= d07_str_to_double(&strUsr[i+=4], 6, 3);
        pDataUser->sFactorB.fHavePowerRate = d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorB.fNonePowerRate= d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorB.fPowerRateFactor = d07_str_to_double(&strUsr[i+=6], 4, 1);
        //sPowerc
        pDataUser->sPowerC.dForthHavePowerIcr= d07_str_to_double(&strUsr[i+=4], 8, 6);
        pDataUser->sPowerC.dBackHavePowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dCombNone1PowerIcr= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dCombNone2PowerIcr= d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sFactorc
        pDataUser->sFactorC.fVolt = d07_str_to_double(&strUsr[i+=8], 4, 3);
        pDataUser->sFactorC.fElec= d07_str_to_double(&strUsr[i+=4], 6, 3);
        pDataUser->sFactorC.fHavePowerRate = d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorC.fNonePowerRate= d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorC.fPowerRateFactor = d07_str_to_double(&strUsr[i+=6], 4, 1);
        // sAtime
        pDataUser->sAtime.dAtimeTotal= d07_str_to_double(&strUsr[i+=4], 8, 6);
        pDataUser->sAtime.dAtimePhaseA= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sAtime.dAtimePhaseB= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sAtime.dAtimePhaseC =  d07_str_to_double(&strUsr[i+=8], 8, 6);
        

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "sBgnTime = %02d%02d%02d%02d%02d%02d\n"         //sBgnTime
                                    "sEndTime = %02d%02d%02d%02d%02d%02d\n"        //end time
                                    "sPowerTotal = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerTotal
                                    "sPowerA = %09.2f %09.2f %09.2f% 09.2f\n"        //sPowerA
                                    "sFactorA = %05.1f %07.3f %07.4f %07.4f %05.3f\n"//sFactorA
                                    "sPowerB = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerB
                                    "sFactorB = %05.1f %07.3f %07.4f %07.4f %05.3f\n"//sFactorB
                                    "sPowerC = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerC
                                    "sFactorC = %05.1f %07.3f %07.4f %07.4f %05.3f\n"//sFactorC
                                    "sAtime = %09.2f %09.2f %09.2f %09.2f\n",        //sAtime
                                    //sBgnTime
                                    pDataUser->sBgnTime.YY,
                                    pDataUser->sBgnTime.MM,
                                    pDataUser->sBgnTime.DD,    
                                    pDataUser->sBgnTime.hh,
                                    pDataUser->sBgnTime.mm,
                                    pDataUser->sBgnTime.ss,
                                    //sEndTime
                                    pDataUser->sEndTime.YY,
                                    pDataUser->sEndTime.MM,
                                    pDataUser->sEndTime.DD,    
                                    pDataUser->sEndTime.hh,
                                    pDataUser->sEndTime.mm,
                                    pDataUser->sEndTime.ss,
                                    //sPowerTotal
                                    pDataUser->sPowerTotal.dForthHavePowerIcr,
                                    pDataUser->sPowerTotal.dBackHavePowerIcr,
                                    pDataUser->sPowerTotal.dCombNone1PowerIcr,
                                    pDataUser->sPowerTotal.dCombNone2PowerIcr,
                                    //sPowerA
                                    pDataUser->sPowerA.dForthHavePowerIcr,
                                    pDataUser->sPowerA.dBackHavePowerIcr,
                                    pDataUser->sPowerA.dCombNone1PowerIcr,
                                    pDataUser->sPowerA.dCombNone2PowerIcr,
                                    //sFactorA
                                    pDataUser->sFactorA.fVolt,
                                    pDataUser->sFactorA.fElec,
                                    pDataUser->sFactorA.fHavePowerRate,
                                    pDataUser->sFactorA.fNonePowerRate,
                                    pDataUser->sFactorA.fPowerRateFactor,
                                    //sPowerB
                                    pDataUser->sPowerB.dForthHavePowerIcr,
                                    pDataUser->sPowerB.dBackHavePowerIcr,
                                    pDataUser->sPowerB.dCombNone1PowerIcr,
                                    pDataUser->sPowerB.dCombNone2PowerIcr,
                                    //sFactorB
                                    pDataUser->sFactorB.fVolt,
                                    pDataUser->sFactorB.fElec,
                                    pDataUser->sFactorB.fHavePowerRate,
                                    pDataUser->sFactorB.fNonePowerRate,
                                    pDataUser->sFactorB.fPowerRateFactor,
                                    //sPowerA
                                    pDataUser->sPowerC.dForthHavePowerIcr,
                                    pDataUser->sPowerC.dBackHavePowerIcr,
                                    pDataUser->sPowerC.dCombNone1PowerIcr,
                                    pDataUser->sPowerC.dCombNone2PowerIcr,
                                    //sFactorA
                                    pDataUser->sFactorC.fVolt,
                                    pDataUser->sFactorC.fElec,
                                    pDataUser->sFactorC.fHavePowerRate,
                                    pDataUser->sFactorC.fNonePowerRate,
                                    pDataUser->sFactorC.fPowerRateFactor,
                                    // sAtime
                                    pDataUser->sAtime.dAtimeTotal,
                                    pDataUser->sAtime.dAtimePhaseA,
                                    pDataUser->sAtime.dAtimePhaseB,
                                    pDataUser->sAtime.dAtimePhaseC);//297
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_anti_phase
 功能描述  : 电压逆序等数据结构用户侧和帧侧结构转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月23日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_anti_phase(E_D07_TRANS_FLG flag, char *user, char *frame)// 逆相序数据格式
{

    S_D07_EventRevPhase *pDataUser = NULL;
    INT32 i = 0;
    #define BUF_SIZE_ANTI_PHASE 300
    #define BUF_OUT_BCD_SIZE_ANTI_PHASE 76
    #define BUF_OUT_SIZE_ANTI_PHASE    152
    char strUsr[BUF_SIZE_ANTI_PHASE] = {0};
    char strBcd[BUF_SIZE_ANTI_PHASE] = {0};
    
#ifdef D07_DEBUG_ON
    S_D07_EventRevPhase sTest ;
#endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventRevPhase *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            
            pDataUser = (S_D07_EventRevPhase*)&sTest;
            //sBgnTime
            sTest.sBgnTime.YY = 0x07;
            sTest.sBgnTime.MM = 0x07;
            sTest.sBgnTime.DD = 0x07;
            sTest.sBgnTime.hh = 0x07;
            sTest.sBgnTime.mm = 0x07;
            sTest.sBgnTime.ss = 0x07;
            //sEndTime
            sTest.sEndTime.YY = 0x08;
            sTest.sEndTime.MM = 0x08;
            sTest.sEndTime.DD = 0x08;
            sTest.sEndTime.hh = 0x08;
            sTest.sEndTime.mm = 0x08;
            sTest.sEndTime.ss = 0x08;
            //sPowerTotal
            sTest.sPowerTotal.dForthHavePowerIcr = 123456.78;
            sTest.sPowerTotal.dBackHavePowerIcr  = 123456.78;
            sTest.sPowerTotal.dCombNone1PowerIcr = 123456.78;
            sTest.sPowerTotal.dCombNone2PowerIcr = 123456.78;
            //sPowerA
            sTest.sPowerA.dForthHavePowerIcr = 123456.78;
            sTest.sPowerA.dBackHavePowerIcr = 123456.78;
            sTest.sPowerA.dCombNone1PowerIcr = 123456.78;
            sTest.sPowerA.dCombNone2PowerIcr = 123456.78;
            //sPowerB
            sTest.sPowerB.dForthHavePowerIcr= 123456.78;
            sTest.sPowerB.dBackHavePowerIcr= 123456.78;
            sTest.sPowerB.dCombNone1PowerIcr= 123456.78;
            sTest.sPowerB.dCombNone2PowerIcr= 123456.78;
            //sPowerC
            sTest.sPowerC.dForthHavePowerIcr= 123456.78;
            sTest.sPowerC.dBackHavePowerIcr= 123456.78;
            sTest.sPowerC.dCombNone1PowerIcr= 123456.78;
            sTest.sPowerC.dCombNone2PowerIcr= 123456.78;

        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"         //sBgnTime
                        "%02d%02d%02d%02d%02d%02d"        //end time
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerTotal
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerA
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerB
                        "%09.2f%09.2f%09.2f%09.2f",        //sPowerC
                        //sBgnTime
                        pDataUser->sBgnTime.YY,
                        pDataUser->sBgnTime.MM,
                        pDataUser->sBgnTime.DD,    
                        pDataUser->sBgnTime.hh,
                        pDataUser->sBgnTime.mm,
                        pDataUser->sBgnTime.ss,
                        //sEndTime
                        pDataUser->sEndTime.YY,
                        pDataUser->sEndTime.MM,
                        pDataUser->sEndTime.DD,    
                        pDataUser->sEndTime.hh,
                        pDataUser->sEndTime.mm,
                        pDataUser->sEndTime.ss,
                        //sPowerTotal
                        pDataUser->sPowerTotal.dForthHavePowerIcr,
                        pDataUser->sPowerTotal.dBackHavePowerIcr,
                        pDataUser->sPowerTotal.dCombNone1PowerIcr,
                        pDataUser->sPowerTotal.dCombNone2PowerIcr,
                        //sPowerA
                        pDataUser->sPowerA.dForthHavePowerIcr,
                        pDataUser->sPowerA.dBackHavePowerIcr,
                        pDataUser->sPowerA.dCombNone1PowerIcr,
                        pDataUser->sPowerA.dCombNone2PowerIcr,
                        //sPowerB
                        pDataUser->sPowerB.dForthHavePowerIcr,
                        pDataUser->sPowerB.dBackHavePowerIcr,
                        pDataUser->sPowerB.dCombNone1PowerIcr,
                        pDataUser->sPowerB.dCombNone2PowerIcr,
                        //sPowerA
                        pDataUser->sPowerC.dForthHavePowerIcr,
                        pDataUser->sPowerC.dBackHavePowerIcr,
                        pDataUser->sPowerC.dCombNone1PowerIcr,
                        pDataUser->sPowerC.dCombNone2PowerIcr);//204
                        
        d07_remove_dot(BUF_OUT_SIZE_ANTI_PHASE,strUsr, strBcd);
        d07_str2bcd(strBcd, (UINT8*)frame, BUF_OUT_SIZE_ANTI_PHASE/2);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, BUF_OUT_BCD_SIZE_ANTI_PHASE);

        //sBgnTime
        pDataUser->sBgnTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sBgnTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sEndTime
        pDataUser->sEndTime.YY = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sPowerTotal
        pDataUser->sPowerTotal.dForthHavePowerIcr = d07_str_to_double(&strUsr[i+=2], 8, 6);
        pDataUser->sPowerTotal.dBackHavePowerIcr  = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerTotal.dCombNone1PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerTotal.dCombNone2PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPowerA
        pDataUser->sPowerA.dForthHavePowerIcr       = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dBackHavePowerIcr        = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dCombNone1PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dCombNone2PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPowerB
        pDataUser->sPowerB.dForthHavePowerIcr      = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dBackHavePowerIcr       = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dCombNone1PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dCombNone2PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPowerc
        pDataUser->sPowerC.dForthHavePowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dBackHavePowerIcr      = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dCombNone1PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dCombNone2PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "sBgnTime = %02d%02d%02d%02d%02d%02d\n"         //sBgnTime
                                    "sEndTime = %02d%02d%02d%02d%02d%02d\n"        //end time
                                    "sPowerTotal = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerTotal
                                    "sPowerA = %09.2f %09.2f %09.2f% 09.2f\n"        //sPowerA
                                    "sPowerB = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerB
                                    "sPowerC = %09.2f %09.2f %09.2f %09.2f\n",        //sPowerC
                                    //sBgnTime
                                    pDataUser->sBgnTime.YY,
                                    pDataUser->sBgnTime.MM,
                                    pDataUser->sBgnTime.DD,    
                                    pDataUser->sBgnTime.hh,
                                    pDataUser->sBgnTime.mm,
                                    pDataUser->sBgnTime.ss,
                                    //sEndTime
                                    pDataUser->sEndTime.YY,
                                    pDataUser->sEndTime.MM,
                                    pDataUser->sEndTime.DD,    
                                    pDataUser->sEndTime.hh,
                                    pDataUser->sEndTime.mm,
                                    pDataUser->sEndTime.ss,
                                    //sPowerTotal
                                    pDataUser->sPowerTotal.dForthHavePowerIcr,
                                    pDataUser->sPowerTotal.dBackHavePowerIcr,
                                    pDataUser->sPowerTotal.dCombNone1PowerIcr,
                                    pDataUser->sPowerTotal.dCombNone2PowerIcr,
                                    //sPowerA
                                    pDataUser->sPowerA.dForthHavePowerIcr,
                                    pDataUser->sPowerA.dBackHavePowerIcr,
                                    pDataUser->sPowerA.dCombNone1PowerIcr,
                                    pDataUser->sPowerA.dCombNone2PowerIcr,
                                    //sPowerB
                                    pDataUser->sPowerB.dForthHavePowerIcr,
                                    pDataUser->sPowerB.dBackHavePowerIcr,
                                    pDataUser->sPowerB.dCombNone1PowerIcr,
                                    pDataUser->sPowerB.dCombNone2PowerIcr,
                                    //sPowerA
                                    pDataUser->sPowerC.dForthHavePowerIcr,
                                    pDataUser->sPowerC.dBackHavePowerIcr,
                                    pDataUser->sPowerC.dCombNone1PowerIcr,
                                    pDataUser->sPowerC.dCombNone2PowerIcr);
        }
        #endif
    }

    return E_D07_OK;
}
/*****************************************************************************
 函 数 名  : trans_d07_data_recd_un_balance
 功能描述  : 电压不平衡记录事件结构用户侧到帧侧结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月23日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_un_balance(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventUnBalance *pDataUser = NULL;
    INT32 i = 0;
    #define BUF_SIZE_ANTI_PHASE 300
    #define BUF_OUT_BCD_SIZE_UN_BALANCE 78
    #define BUF_OUT_SIZE_UN_BALANCE   156
    
    char strUsr[BUF_SIZE_ANTI_PHASE] = {0};
    char strBcd[BUF_SIZE_ANTI_PHASE] = {0};
    #ifdef D07_DEBUG_ON
    S_D07_EventUnBalance sTest ;
    #endif
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventUnBalance *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
        
            pDataUser = (S_D07_EventUnBalance*)&sTest;
            //sBgnTime
            sTest.sBgnTime.YY = 0x07;
            sTest.sBgnTime.MM = 0x07;
            sTest.sBgnTime.DD = 0x07;
            sTest.sBgnTime.hh = 0x07;
            sTest.sBgnTime.mm = 0x07;
            sTest.sBgnTime.ss = 0x07;
            //sEndTime
            sTest.sEndTime.YY = 0x08;
            sTest.sEndTime.MM = 0x08;
            sTest.sEndTime.DD = 0x08;
            sTest.sEndTime.hh = 0x08;
            sTest.sEndTime.mm = 0x08;
            sTest.sEndTime.ss = 0x08;
            //sPowerTotal
            sTest.sPowerTotal.dForthHavePowerIcr = 123456.78;
            sTest.sPowerTotal.dBackHavePowerIcr  = 123456.78;
            sTest.sPowerTotal.dCombNone1PowerIcr = 123456.78;
            sTest.sPowerTotal.dCombNone2PowerIcr = 123456.78;
            //sPowerA
            sTest.sPowerA.dForthHavePowerIcr = 123456.78;
            sTest.sPowerA.dBackHavePowerIcr = 123456.78;
            sTest.sPowerA.dCombNone1PowerIcr = 123456.78;
            sTest.sPowerA.dCombNone2PowerIcr = 123456.78;
            //sPowerB
            sTest.sPowerB.dForthHavePowerIcr= 123456.78;
            sTest.sPowerB.dBackHavePowerIcr= 123456.78;
            sTest.sPowerB.dCombNone1PowerIcr= 123456.78;
            sTest.sPowerB.dCombNone2PowerIcr= 123456.78;
            //sPowerC
            sTest.sPowerC.dForthHavePowerIcr= 123456.78;
            sTest.sPowerC.dBackHavePowerIcr= 123456.78;
            sTest.sPowerC.dCombNone1PowerIcr= 123456.78;
            sTest.sPowerC.dCombNone2PowerIcr= 123456.78;

        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"         //sBgnTime
                        "%02d%02d%02d%02d%02d%02d"        //end time
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerTotal
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerA
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerB
                        "%09.2f%09.2f%09.2f%09.2f",        //sPowerC
                        //sBgnTime
                        pDataUser->sBgnTime.YY,
                        pDataUser->sBgnTime.MM,
                        pDataUser->sBgnTime.DD,    
                        pDataUser->sBgnTime.hh,
                        pDataUser->sBgnTime.mm,
                        pDataUser->sBgnTime.ss,
                        //sEndTime
                        pDataUser->sEndTime.YY,
                        pDataUser->sEndTime.MM,
                        pDataUser->sEndTime.DD,    
                        pDataUser->sEndTime.hh,
                        pDataUser->sEndTime.mm,
                        pDataUser->sEndTime.ss,
                        //sPowerTotal
                        pDataUser->sPowerTotal.dForthHavePowerIcr,
                        pDataUser->sPowerTotal.dBackHavePowerIcr,
                        pDataUser->sPowerTotal.dCombNone1PowerIcr,
                        pDataUser->sPowerTotal.dCombNone2PowerIcr,
                        //sPowerA
                        pDataUser->sPowerA.dForthHavePowerIcr,
                        pDataUser->sPowerA.dBackHavePowerIcr,
                        pDataUser->sPowerA.dCombNone1PowerIcr,
                        pDataUser->sPowerA.dCombNone2PowerIcr,
                        //sPowerB
                        pDataUser->sPowerB.dForthHavePowerIcr,
                        pDataUser->sPowerB.dBackHavePowerIcr,
                        pDataUser->sPowerB.dCombNone1PowerIcr,
                        pDataUser->sPowerB.dCombNone2PowerIcr,
                        //sPowerA
                        pDataUser->sPowerC.dForthHavePowerIcr,
                        pDataUser->sPowerC.dBackHavePowerIcr,
                        pDataUser->sPowerC.dCombNone1PowerIcr,
                        pDataUser->sPowerC.dCombNone2PowerIcr);//204
        
        d07_remove_dot(BUF_OUT_SIZE_UN_BALANCE,strUsr, strBcd);
        d07_str2bcd(strBcd, (UINT8*)frame, BUF_OUT_SIZE_UN_BALANCE/2);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, BUF_OUT_BCD_SIZE_UN_BALANCE);

        //sBgnTime
        pDataUser->sBgnTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sBgnTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sEndTime
        pDataUser->sEndTime.YY = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sPowerTotal
        pDataUser->sPowerTotal.dForthHavePowerIcr = d07_str_to_double(&strUsr[i+=2], 8, 6);
        pDataUser->sPowerTotal.dBackHavePowerIcr  = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerTotal.dCombNone1PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerTotal.dCombNone2PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPowerA
        pDataUser->sPowerA.dForthHavePowerIcr       = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dBackHavePowerIcr        = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dCombNone1PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dCombNone2PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPowerB
        pDataUser->sPowerB.dForthHavePowerIcr      = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dBackHavePowerIcr       = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dCombNone1PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dCombNone2PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPowerc
        pDataUser->sPowerC.dForthHavePowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dBackHavePowerIcr      = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dCombNone1PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dCombNone2PowerIcr     = d07_str_to_double(&strUsr[i+=8], 8, 6);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "sBgnTime = %02d%02d%02d%02d%02d%02d\n"         //sBgnTime
                                    "sEndTime = %02d%02d%02d%02d%02d%02d\n"        //end time
                                    "sPowerTotal = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerTotal
                                    "sPowerA = %09.2f %09.2f %09.2f% 09.2f\n"        //sPowerA
                                    "sPowerB = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerB
                                    "sPowerC = %09.2f %09.2f %09.2f %09.2f\n",        //sPowerC
                                    //sBgnTime
                                    pDataUser->sBgnTime.YY,
                                    pDataUser->sBgnTime.MM,
                                    pDataUser->sBgnTime.DD,    
                                    pDataUser->sBgnTime.hh,
                                    pDataUser->sBgnTime.mm,
                                    pDataUser->sBgnTime.ss,
                                    //sEndTime
                                    pDataUser->sEndTime.YY,
                                    pDataUser->sEndTime.MM,
                                    pDataUser->sEndTime.DD,    
                                    pDataUser->sEndTime.hh,
                                    pDataUser->sEndTime.mm,
                                    pDataUser->sEndTime.ss,
                                    //sPowerTotal
                                    pDataUser->sPowerTotal.dForthHavePowerIcr,
                                    pDataUser->sPowerTotal.dBackHavePowerIcr,
                                    pDataUser->sPowerTotal.dCombNone1PowerIcr,
                                    pDataUser->sPowerTotal.dCombNone2PowerIcr,
                                    //sPowerA
                                    pDataUser->sPowerA.dForthHavePowerIcr,
                                    pDataUser->sPowerA.dBackHavePowerIcr,
                                    pDataUser->sPowerA.dCombNone1PowerIcr,
                                    pDataUser->sPowerA.dCombNone2PowerIcr,
                                    //sPowerB
                                    pDataUser->sPowerB.dForthHavePowerIcr,
                                    pDataUser->sPowerB.dBackHavePowerIcr,
                                    pDataUser->sPowerB.dCombNone1PowerIcr,
                                    pDataUser->sPowerB.dCombNone2PowerIcr,
                                    //sPowerA
                                    pDataUser->sPowerC.dForthHavePowerIcr,
                                    pDataUser->sPowerC.dBackHavePowerIcr,
                                    pDataUser->sPowerC.dCombNone1PowerIcr,
                                    pDataUser->sPowerC.dCombNone2PowerIcr);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_zone_prog
 功能描述  : 时区表编程记录转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月26日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_zone_prog(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventZone *pDataUser = NULL;
    char strUsr[256] = {0};
    INT32 i = 0;
    #ifdef D07_DEBUG_ON
    S_D07_EventZone sTest ;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventZone *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {

            pDataUser = (S_D07_EventZone*)&sTest;
            //sOccurTime
            sTest.sOccurTime.YY = 0x07;
            sTest.sOccurTime.MM = 0x07;
            sTest.sOccurTime.DD = 0x07;
            sTest.sOccurTime.hh = 0x07;
            sTest.sOccurTime.mm = 0x07;
            sTest.sOccurTime.ss = 0x07;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;

            //sAftTime
            for(i = 0; i < 14; i++)
            {
                sTest.sZoneSet1[i].MM = i+1;
                sTest.sZoneSet1[i].DD = i+1;
                sTest.sZoneSet1[i].NN = i+1;
                
                sTest.sZoneSet2[i].MM = i+2;
                sTest.sZoneSet2[i].DD = i+2;
                sTest.sZoneSet2[i].NN = i+2;
            }
        }
        #endif
        
        sprintf(strUsr, 
                "%02d%02d%02d%02d%02d%02d"                     //sOccurTime
                "%02d%02d%02d%02d"                            //operator
                "%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d"  //sZoneSet1
                "%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d"  //sZoneSet1
                "%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d"  //sZoneSet1
                "%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d"  //sZoneSet2
                "%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d"  //sZoneSet2
                "%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d", //sZoneSet2
                //sBefTime
                pDataUser->sOccurTime.YY,
                pDataUser->sOccurTime.MM,
                pDataUser->sOccurTime.DD,    
                pDataUser->sOccurTime.hh,
                pDataUser->sOccurTime.mm,
                pDataUser->sOccurTime.ss,
                //operator
                pDataUser->operator[0],
                pDataUser->operator[1],
                pDataUser->operator[2],
                pDataUser->operator[3],

                //sZoneSet1
                pDataUser->sZoneSet1[0].MM,
                pDataUser->sZoneSet1[0].DD,
                pDataUser->sZoneSet1[0].NN,
                pDataUser->sZoneSet1[1].MM,
                pDataUser->sZoneSet1[1].DD,
                pDataUser->sZoneSet1[1].NN,
                pDataUser->sZoneSet1[2].MM,
                pDataUser->sZoneSet1[2].DD,
                pDataUser->sZoneSet1[2].NN,
                pDataUser->sZoneSet1[3].MM,
                pDataUser->sZoneSet1[3].DD,
                pDataUser->sZoneSet1[3].NN,
                pDataUser->sZoneSet1[4].MM,
                pDataUser->sZoneSet1[4].DD,
                pDataUser->sZoneSet1[4].NN,
                pDataUser->sZoneSet1[5].MM,
                pDataUser->sZoneSet1[5].DD,
                pDataUser->sZoneSet1[5].NN,
                pDataUser->sZoneSet1[6].MM,
                pDataUser->sZoneSet1[6].DD,
                pDataUser->sZoneSet1[6].NN,
                pDataUser->sZoneSet1[7].MM,
                pDataUser->sZoneSet1[7].DD,
                pDataUser->sZoneSet1[7].NN,
                pDataUser->sZoneSet1[8].MM,
                pDataUser->sZoneSet1[8].DD,
                pDataUser->sZoneSet1[8].NN,
                pDataUser->sZoneSet1[9].MM,
                pDataUser->sZoneSet1[9].DD,
                pDataUser->sZoneSet1[9].NN,
                pDataUser->sZoneSet1[10].MM,
                pDataUser->sZoneSet1[10].DD,
                pDataUser->sZoneSet1[10].NN,
                pDataUser->sZoneSet1[11].MM,
                pDataUser->sZoneSet1[11].DD,
                pDataUser->sZoneSet1[11].NN,
                pDataUser->sZoneSet1[12].MM,
                pDataUser->sZoneSet1[12].DD,
                pDataUser->sZoneSet1[12].NN,
                pDataUser->sZoneSet1[13].MM,
                pDataUser->sZoneSet1[13].DD,
                pDataUser->sZoneSet1[13].NN,
                //sZoneSet2
                pDataUser->sZoneSet2[0].MM,
                pDataUser->sZoneSet2[0].DD,
                pDataUser->sZoneSet2[0].NN,
                pDataUser->sZoneSet2[1].MM,
                pDataUser->sZoneSet2[1].DD,
                pDataUser->sZoneSet2[1].NN,
                pDataUser->sZoneSet2[2].MM,
                pDataUser->sZoneSet2[2].DD,
                pDataUser->sZoneSet2[2].NN,
                pDataUser->sZoneSet2[3].MM,
                pDataUser->sZoneSet2[3].DD,
                pDataUser->sZoneSet2[3].NN,
                pDataUser->sZoneSet2[4].MM,
                pDataUser->sZoneSet2[4].DD,
                pDataUser->sZoneSet2[4].NN,
                pDataUser->sZoneSet2[5].MM,
                pDataUser->sZoneSet2[5].DD,
                pDataUser->sZoneSet2[5].NN,
                pDataUser->sZoneSet2[6].MM,
                pDataUser->sZoneSet2[6].DD,
                pDataUser->sZoneSet2[6].NN,
                pDataUser->sZoneSet2[7].MM,
                pDataUser->sZoneSet2[7].DD,
                pDataUser->sZoneSet2[7].NN,
                pDataUser->sZoneSet2[8].MM,
                pDataUser->sZoneSet2[8].DD,
                pDataUser->sZoneSet2[8].NN,
                pDataUser->sZoneSet2[9].MM,
                pDataUser->sZoneSet2[9].DD,
                pDataUser->sZoneSet2[9].NN,
                pDataUser->sZoneSet2[10].MM,
                pDataUser->sZoneSet2[10].DD,
                pDataUser->sZoneSet2[10].NN,
                pDataUser->sZoneSet2[11].MM,
                pDataUser->sZoneSet2[11].DD,
                pDataUser->sZoneSet2[11].NN,
                pDataUser->sZoneSet2[12].MM,
                pDataUser->sZoneSet2[12].DD,
                pDataUser->sZoneSet2[12].NN,
                pDataUser->sZoneSet2[13].MM,
                pDataUser->sZoneSet2[13].DD,
                pDataUser->sZoneSet2[13].NN);

        d07_str2bcd(strUsr, (UINT8*)frame, 188);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, 94);

        //sBefTime
        pDataUser->sOccurTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sOccurTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //operator
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        
        //sZoneSet1
        pDataUser->sZoneSet1[0].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[0].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[0].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[1].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[1].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[1].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[2].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[2].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[2].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[3].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[3].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[3].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[4].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[4].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[4].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[5].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[5].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[5].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[6].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[6].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[6].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[7].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[7].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[7].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[8].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[8].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[8].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[9].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[9].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[9].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[10].MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[10].DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[10].NN = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[11].MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[11].DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[11].NN = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[12].MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[12].DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[12].NN = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[13].MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[13].DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet1[13].NN = d07_str_to_num(&strUsr[i+=2], 2);
        //sZoneSet2
        pDataUser->sZoneSet2[0].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[0].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[0].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[1].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[1].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[1].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[2].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[2].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[2].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[3].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[3].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[3].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[4].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[4].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[4].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[5].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[5].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[5].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[6].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[6].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[6].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[7].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[7].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[7].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[8].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[8].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[8].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[9].MM  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[9].DD  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[9].NN  = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[10].MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[10].DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[10].NN = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[11].MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[11].DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[11].NN = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[12].MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[12].DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[12].NN = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[13].MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[13].DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sZoneSet2[13].NN = d07_str_to_num(&strUsr[i+=2], 2);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "sOccurTime = %02d%02d%02d%02d%02d%02d\n"                     //sOccurTime
                    "operator = %02d%02d%02d%02d\n"                            //operator
                    "sZoneSet1 = %02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d\n"  //sZoneSet1
                    "sZoneSet1 = %02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d\n"  //sZoneSet1
                    "sZoneSet1 = %02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d\n"  //sZoneSet1
                    "sZoneSet2 = %02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d\n"  //sZoneSet2
                    "sZoneSet2 = %02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d\n"  //sZoneSet2
                    "sZoneSet2 = %02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d%02d\n", //sZoneSet2
                    //sBefTime
                    pDataUser->sOccurTime.YY,
                    pDataUser->sOccurTime.MM,
                    pDataUser->sOccurTime.DD,    
                    pDataUser->sOccurTime.hh,
                    pDataUser->sOccurTime.mm,
                    pDataUser->sOccurTime.ss,
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],

                    //sZoneSet1
                    pDataUser->sZoneSet1[0].MM,
                    pDataUser->sZoneSet1[0].DD,
                    pDataUser->sZoneSet1[0].NN,
                    pDataUser->sZoneSet1[1].MM,
                    pDataUser->sZoneSet1[1].DD,
                    pDataUser->sZoneSet1[1].NN,
                    pDataUser->sZoneSet1[2].MM,
                    pDataUser->sZoneSet1[2].DD,
                    pDataUser->sZoneSet1[2].NN,
                    pDataUser->sZoneSet1[3].MM,
                    pDataUser->sZoneSet1[3].DD,
                    pDataUser->sZoneSet1[3].NN,
                    pDataUser->sZoneSet1[4].MM,
                    pDataUser->sZoneSet1[4].DD,
                    pDataUser->sZoneSet1[4].NN,
                    pDataUser->sZoneSet1[5].MM,
                    pDataUser->sZoneSet1[5].DD,
                    pDataUser->sZoneSet1[5].NN,
                    pDataUser->sZoneSet1[6].MM,
                    pDataUser->sZoneSet1[6].DD,
                    pDataUser->sZoneSet1[6].NN,
                    pDataUser->sZoneSet1[7].MM,
                    pDataUser->sZoneSet1[7].DD,
                    pDataUser->sZoneSet1[7].NN,
                    pDataUser->sZoneSet1[8].MM,
                    pDataUser->sZoneSet1[8].DD,
                    pDataUser->sZoneSet1[8].NN,
                    pDataUser->sZoneSet1[9].MM,
                    pDataUser->sZoneSet1[9].DD,
                    pDataUser->sZoneSet1[9].NN,
                    pDataUser->sZoneSet1[10].MM,
                    pDataUser->sZoneSet1[10].DD,
                    pDataUser->sZoneSet1[10].NN,
                    pDataUser->sZoneSet1[11].MM,
                    pDataUser->sZoneSet1[11].DD,
                    pDataUser->sZoneSet1[11].NN,
                    pDataUser->sZoneSet1[12].MM,
                    pDataUser->sZoneSet1[12].DD,
                    pDataUser->sZoneSet1[12].NN,
                    pDataUser->sZoneSet1[13].MM,
                    pDataUser->sZoneSet1[13].DD,
                    pDataUser->sZoneSet1[13].NN,
                    //sZoneSet2
                    pDataUser->sZoneSet2[0].MM,
                    pDataUser->sZoneSet2[0].DD,
                    pDataUser->sZoneSet2[0].NN,
                    pDataUser->sZoneSet2[1].MM,
                    pDataUser->sZoneSet2[1].DD,
                    pDataUser->sZoneSet2[1].NN,
                    pDataUser->sZoneSet2[2].MM,
                    pDataUser->sZoneSet2[2].DD,
                    pDataUser->sZoneSet2[2].NN,
                    pDataUser->sZoneSet2[3].MM,
                    pDataUser->sZoneSet2[3].DD,
                    pDataUser->sZoneSet2[3].NN,
                    pDataUser->sZoneSet2[4].MM,
                    pDataUser->sZoneSet2[4].DD,
                    pDataUser->sZoneSet2[4].NN,
                    pDataUser->sZoneSet2[5].MM,
                    pDataUser->sZoneSet2[5].DD,
                    pDataUser->sZoneSet2[5].NN,
                    pDataUser->sZoneSet2[6].MM,
                    pDataUser->sZoneSet2[6].DD,
                    pDataUser->sZoneSet2[6].NN,
                    pDataUser->sZoneSet2[7].MM,
                    pDataUser->sZoneSet2[7].DD,
                    pDataUser->sZoneSet2[7].NN,
                    pDataUser->sZoneSet2[8].MM,
                    pDataUser->sZoneSet2[8].DD,
                    pDataUser->sZoneSet2[8].NN,
                    pDataUser->sZoneSet2[9].MM,
                    pDataUser->sZoneSet2[9].DD,
                    pDataUser->sZoneSet2[9].NN,
                    pDataUser->sZoneSet2[10].MM,
                    pDataUser->sZoneSet2[10].DD,
                    pDataUser->sZoneSet2[10].NN,
                    pDataUser->sZoneSet2[11].MM,
                    pDataUser->sZoneSet2[11].DD,
                    pDataUser->sZoneSet2[11].NN,
                    pDataUser->sZoneSet2[12].MM,
                    pDataUser->sZoneSet2[12].DD,
                    pDataUser->sZoneSet2[12].NN,
                    pDataUser->sZoneSet2[13].MM,
                    pDataUser->sZoneSet2[13].DD,
                    pDataUser->sZoneSet2[13].NN);
        }
        #endif
    }

    return E_D07_OK;
}


// 有些结构还是不明白
eD07Err trans_d07_data_recd_part_prog(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    #ifdef D07_DEBUG_ON
    

    #endif
    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_adjust_time
 功能描述  : 校时事件数据结构转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月26日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_adjust_time(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventTiming *pDataUser = NULL;
    char strUsr[128] = {0};
    INT32 i = 0;
    
    #ifdef D07_DEBUG_ON
    S_D07_EventTiming sTest ;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    pDataUser = (S_D07_EventTiming *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            pDataUser = (S_D07_EventTiming*)&sTest;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;
            //sOccurTime
            sTest.sBefTime.YY = 0x17;
            sTest.sBefTime.MM = 0x17;
            sTest.sBefTime.DD = 0x17;
            sTest.sBefTime.hh = 0x17;
            sTest.sBefTime.mm = 0x17;
            sTest.sBefTime.ss = 0x17;
            //sAftTime
            sTest.sAftTime.YY = 0x18;
            sTest.sAftTime.MM = 0x18;
            sTest.sAftTime.DD = 0x18;
            sTest.sAftTime.hh = 0x18;
            sTest.sAftTime.mm = 0x18;
            sTest.sAftTime.ss = 0x18;
            
        }
        #endif
        
        sprintf(strUsr, 
                "%02d%02d%02d%02d"                            //operator
                "%02d%02d%02d%02d%02d%02d"                     //sBefTime
                "%02d%02d%02d%02d%02d%02d",                 //sAftTime    
                //operator
                pDataUser->operator[0],
                pDataUser->operator[1],
                pDataUser->operator[2],
                pDataUser->operator[3],
                //sBefTime
                pDataUser->sBefTime.YY,
                pDataUser->sBefTime.MM,
                pDataUser->sBefTime.DD,    
                pDataUser->sBefTime.hh,
                pDataUser->sBefTime.mm,
                pDataUser->sBefTime.ss,
                //sAftTime
                pDataUser->sAftTime.YY,
                pDataUser->sAftTime.MM,
                pDataUser->sAftTime.DD,    
                pDataUser->sAftTime.hh,
                pDataUser->sAftTime.mm,
                pDataUser->sAftTime.ss);

        d07_str2bcd(strUsr, (UINT8*)frame, 32);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, 16);

        //operator
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        //sBefTime
        pDataUser->sBefTime.YY = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBefTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBefTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBefTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBefTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBefTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sAftTime
        pDataUser->sAftTime.YY = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sAftTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sAftTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sAftTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sAftTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sAftTime.ss = d07_str_to_num(&strUsr[i+=2], 2);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "operator = %02d%02d%02d%02d\n"                            //operator
                    "sBefTime = %02d%02d%02d%02d%02d%02d\n"                 //sBefTime
                    "sAftTime = %02d%02d%02d%02d%02d%02d\n",                 //sAftTime    
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],
                    //sBefTime
                    pDataUser->sBefTime.YY,
                    pDataUser->sBefTime.MM,
                    pDataUser->sBefTime.DD,    
                    pDataUser->sBefTime.hh,
                    pDataUser->sBefTime.mm,
                    pDataUser->sBefTime.ss,
                    //sAftTime
                    pDataUser->sAftTime.YY,
                    pDataUser->sAftTime.MM,
                    pDataUser->sAftTime.DD,    
                    pDataUser->sAftTime.hh,
                    pDataUser->sAftTime.mm,
                    pDataUser->sAftTime.ss);
        }
        #endif
    }

    return E_D07_OK;
}


/*****************************************************************************
 函 数 名  : trans_d07_data_recd_less_more_loss_elec
 功能描述  : 用户侧和帧侧转换，过流，失流，断流事件信息结构
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月23日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_less_more_loss_elec(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    INT32 nLenBCD =  115;
    INT32 nOutBCDLen =  230;
    INT32 i = 0;
    char strUsr[300] = {0};
    char strBcd[300] = {0};
    S_D07_LESS_MORE_LOSS_ELET *pDataUser = NULL;
    #ifdef D07_DEBUG_ON
    S_D07_LESS_MORE_LOSS_ELET sTest ;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_LESS_MORE_LOSS_ELET *)user;    
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {    
            pDataUser = (S_D07_LESS_MORE_LOSS_ELET*)&sTest;
            
            sTest.sBgnTime.YY = 0x07;
            sTest.sBgnTime.MM = 0x07;
            sTest.sBgnTime.DD = 0x07;
            sTest.sBgnTime.hh = 0x07;
            sTest.sBgnTime.mm = 0x07;
            sTest.sBgnTime.ss = 0x07;
            
            sTest.sEndTime.YY = 0x08;
            sTest.sEndTime.MM = 0x08;
            sTest.sEndTime.DD = 0x08;
            sTest.sEndTime.hh = 0x08;
            sTest.sEndTime.mm = 0x08;
            sTest.sEndTime.ss = 0x08;
            
            //sPowerTotal
            sTest.sPowerTotal.dForthHavePowerIcr = 123456.78;
            sTest.sPowerTotal.dBackHavePowerIcr  = 123456.78;
            sTest.sPowerTotal.dCombNone1PowerIcr = 123456.78;
            sTest.sPowerTotal.dCombNone2PowerIcr = 123456.78;
            //sPowerA
            sTest.sPowerA.dForthHavePowerIcr = 123456.78;
            sTest.sPowerA.dBackHavePowerIcr = 123456.78;
            sTest.sPowerA.dCombNone1PowerIcr = 123456.78;
            sTest.sPowerA.dCombNone2PowerIcr = 123456.78;
            //sFactorA
            sTest.sFactorA.fVolt = 123.4;
            sTest.sFactorA.fElec= 123.456;
            sTest.sFactorA.fHavePowerRate = 12.3456;
            sTest.sFactorA.fNonePowerRate= 12.3456;
            sTest.sFactorA.fPowerRateFactor = 1.234;
            //sPowerB
            sTest.sPowerB.dForthHavePowerIcr= 123456.78;
            sTest.sPowerB.dBackHavePowerIcr= 123456.78;
            sTest.sPowerB.dCombNone1PowerIcr= 123456.78;
            sTest.sPowerB.dCombNone2PowerIcr= 123456.78;
            //sFactorB
            sTest.sFactorB.fVolt = 123.4;
            sTest.sFactorB.fElec= 123.456;
            sTest.sFactorB.fHavePowerRate = 12.3456;
            sTest.sFactorB.fNonePowerRate= 12.3456;
            sTest.sFactorB.fPowerRateFactor = 1.234;
            //sPowerC
            sTest.sPowerC.dForthHavePowerIcr= 123456.78;
            sTest.sPowerC.dBackHavePowerIcr= 123456.78;
            sTest.sPowerC.dCombNone1PowerIcr= 123456.78;
            sTest.sPowerC.dCombNone2PowerIcr= 123456.78;
            //sFactorC
            sTest.sFactorC.fVolt = 123.4;
            sTest.sFactorC.fElec= 123.456;
            sTest.sFactorC.fHavePowerRate = 12.3456;
            sTest.sFactorC.fNonePowerRate= 12.3456;
            sTest.sFactorC.fPowerRateFactor = 1.234;

        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"         //sBgnTime
                        "%02d%02d%02d%02d%02d%02d"        //end time
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerTotal
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerA
                        "%05.1f%07.3f%07.4f%07.4f%05.3f"//sFactorA
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerB
                        "%05.1f%07.3f%07.4f%07.4f%05.3f"//sFactorB
                        "%09.2f%09.2f%09.2f%09.2f"        //sPowerC
                        "%05.1f%07.3f%07.4f%07.4f%05.3f",//sFactorC
                        //sBgnTime
                        pDataUser->sBgnTime.YY,
                        pDataUser->sBgnTime.MM,
                        pDataUser->sBgnTime.DD,    
                        pDataUser->sBgnTime.hh,
                        pDataUser->sBgnTime.mm,
                        pDataUser->sBgnTime.ss,
                        //sEndTime
                        pDataUser->sEndTime.YY,
                        pDataUser->sEndTime.MM,
                        pDataUser->sEndTime.DD,    
                        pDataUser->sEndTime.hh,
                        pDataUser->sEndTime.mm,
                        pDataUser->sEndTime.ss,
                        //sPowerTotal
                        pDataUser->sPowerTotal.dForthHavePowerIcr,
                        pDataUser->sPowerTotal.dBackHavePowerIcr,
                        pDataUser->sPowerTotal.dCombNone1PowerIcr,
                        pDataUser->sPowerTotal.dCombNone2PowerIcr,
                        //sPowerA
                        pDataUser->sPowerA.dForthHavePowerIcr,
                        pDataUser->sPowerA.dBackHavePowerIcr,
                        pDataUser->sPowerA.dCombNone1PowerIcr,
                        pDataUser->sPowerA.dCombNone2PowerIcr,
                        //sFactorA
                        pDataUser->sFactorA.fVolt,
                        pDataUser->sFactorA.fElec,
                        pDataUser->sFactorA.fHavePowerRate,
                        pDataUser->sFactorA.fNonePowerRate,
                        pDataUser->sFactorA.fPowerRateFactor,
                        //sPowerB
                        pDataUser->sPowerB.dForthHavePowerIcr,
                        pDataUser->sPowerB.dBackHavePowerIcr,
                        pDataUser->sPowerB.dCombNone1PowerIcr,
                        pDataUser->sPowerB.dCombNone2PowerIcr,
                        //sFactorB
                        pDataUser->sFactorB.fVolt,
                        pDataUser->sFactorB.fElec,
                        pDataUser->sFactorB.fHavePowerRate,
                        pDataUser->sFactorB.fNonePowerRate,
                        pDataUser->sFactorB.fPowerRateFactor,
                        //sPowerA
                        pDataUser->sPowerC.dForthHavePowerIcr,
                        pDataUser->sPowerC.dBackHavePowerIcr,
                        pDataUser->sPowerC.dCombNone1PowerIcr,
                        pDataUser->sPowerC.dCombNone2PowerIcr,
                        //sFactorA
                        pDataUser->sFactorC.fVolt,
                        pDataUser->sFactorC.fElec,
                        pDataUser->sFactorC.fHavePowerRate,
                        pDataUser->sFactorC.fNonePowerRate,
                        pDataUser->sFactorC.fPowerRateFactor);
        
        d07_remove_dot(nOutBCDLen, strUsr, strBcd);
        d07_str2bcd(strBcd, (UINT8*)frame, nOutBCDLen);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, nLenBCD);
        //sBgnTime
        pDataUser->sBgnTime.YY = d07_str_to_num(&strUsr[i], 2);
        pDataUser->sBgnTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sEndTime
        pDataUser->sEndTime.YY = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sPowerTotal
        pDataUser->sPowerTotal.dForthHavePowerIcr = d07_str_to_double(&strUsr[i+=2],8,6);
        pDataUser->sPowerTotal.dBackHavePowerIcr  = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerTotal.dCombNone1PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerTotal.dCombNone2PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPowerA
        pDataUser->sPowerA.dForthHavePowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dBackHavePowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dCombNone1PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerA.dCombNone2PowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sFactorA
        pDataUser->sFactorA.fVolt = d07_str_to_double(&strUsr[i+=8], 4, 3);
        pDataUser->sFactorA.fElec= d07_str_to_double(&strUsr[i+=4], 6, 3);
        pDataUser->sFactorA.fHavePowerRate = d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorA.fNonePowerRate= d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorA.fPowerRateFactor = d07_str_to_double(&strUsr[i+=6], 4, 1);
        //sPowerB
        pDataUser->sPowerB.dForthHavePowerIcr= d07_str_to_double(&strUsr[i+=4], 8, 6);
        pDataUser->sPowerB.dBackHavePowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dCombNone1PowerIcr= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerB.dCombNone2PowerIcr= d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sFactorB
        pDataUser->sFactorB.fVolt = d07_str_to_double(&strUsr[i+=8], 4, 3);
        pDataUser->sFactorB.fElec= d07_str_to_double(&strUsr[i+=4], 6, 3);
        pDataUser->sFactorB.fHavePowerRate = d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorB.fNonePowerRate= d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorB.fPowerRateFactor = d07_str_to_double(&strUsr[i+=6], 4, 1);
        //sPowerc
        pDataUser->sPowerC.dForthHavePowerIcr= d07_str_to_double(&strUsr[i+=4], 8, 6);
        pDataUser->sPowerC.dBackHavePowerIcr = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dCombNone1PowerIcr= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerC.dCombNone2PowerIcr= d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sFactorc
        pDataUser->sFactorC.fVolt = d07_str_to_double(&strUsr[i+=8], 4, 3);
        pDataUser->sFactorC.fElec= d07_str_to_double(&strUsr[i+=4], 6, 3);
        pDataUser->sFactorC.fHavePowerRate = d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorC.fNonePowerRate= d07_str_to_double(&strUsr[i+=6], 6, 2);
        pDataUser->sFactorC.fPowerRateFactor = d07_str_to_double(&strUsr[i+=6], 4, 1);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, "sBgnTime = %02d%02d%02d%02d%02d%02d\n"         //sBgnTime
                                    "sEndTime = %02d%02d%02d%02d%02d%02d\n"        //end time
                                    "sPowerTotal = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerTotal
                                    "sPowerA = %09.2f %09.2f %09.2f% 09.2f\n"        //sPowerA
                                    "sFactorA = %05.1f %07.3f %07.4f %07.4f %05.3f\n"//sFactorA
                                    "sPowerB = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerB
                                    "sFactorB = %05.1f %07.3f %07.4f %07.4f %05.3f\n"//sFactorB
                                    "sPowerC = %09.2f %09.2f %09.2f %09.2f\n"        //sPowerC
                                    "sFactorC = %05.1f %07.3f %07.4f %07.4f %05.3f\n",//sFactorC
                                    //sBgnTime
                                    pDataUser->sBgnTime.YY,
                                    pDataUser->sBgnTime.MM,
                                    pDataUser->sBgnTime.DD,    
                                    pDataUser->sBgnTime.hh,
                                    pDataUser->sBgnTime.mm,
                                    pDataUser->sBgnTime.ss,
                                    //sEndTime
                                    pDataUser->sEndTime.YY,
                                    pDataUser->sEndTime.MM,
                                    pDataUser->sEndTime.DD,    
                                    pDataUser->sEndTime.hh,
                                    pDataUser->sEndTime.mm,
                                    pDataUser->sEndTime.ss,
                                    //sPowerTotal
                                    pDataUser->sPowerTotal.dForthHavePowerIcr,
                                    pDataUser->sPowerTotal.dBackHavePowerIcr,
                                    pDataUser->sPowerTotal.dCombNone1PowerIcr,
                                    pDataUser->sPowerTotal.dCombNone2PowerIcr,
                                    //sPowerA
                                    pDataUser->sPowerA.dForthHavePowerIcr,
                                    pDataUser->sPowerA.dBackHavePowerIcr,
                                    pDataUser->sPowerA.dCombNone1PowerIcr,
                                    pDataUser->sPowerA.dCombNone2PowerIcr,
                                    //sFactorA
                                    pDataUser->sFactorA.fVolt,
                                    pDataUser->sFactorA.fElec,
                                    pDataUser->sFactorA.fHavePowerRate,
                                    pDataUser->sFactorA.fNonePowerRate,
                                    pDataUser->sFactorA.fPowerRateFactor,
                                    //sPowerB
                                    pDataUser->sPowerB.dForthHavePowerIcr,
                                    pDataUser->sPowerB.dBackHavePowerIcr,
                                    pDataUser->sPowerB.dCombNone1PowerIcr,
                                    pDataUser->sPowerB.dCombNone2PowerIcr,
                                    //sFactorB
                                    pDataUser->sFactorB.fVolt,
                                    pDataUser->sFactorB.fElec,
                                    pDataUser->sFactorB.fHavePowerRate,
                                    pDataUser->sFactorB.fNonePowerRate,
                                    pDataUser->sFactorB.fPowerRateFactor,
                                    //sPowerC
                                    pDataUser->sPowerC.dForthHavePowerIcr,
                                    pDataUser->sPowerC.dBackHavePowerIcr,
                                    pDataUser->sPowerC.dCombNone1PowerIcr,
                                    pDataUser->sPowerC.dCombNone2PowerIcr,
                                    //sFactorC
                                    pDataUser->sFactorC.fVolt,
                                    pDataUser->sFactorC.fElec,
                                    pDataUser->sFactorC.fHavePowerRate,
                                    pDataUser->sFactorC.fNonePowerRate,
                                    pDataUser->sFactorC.fPowerRateFactor);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_event_clear
 功能描述  : 事件清零记录结构转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月26日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_event_clear(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventClear *pDataUser = NULL;
    char strUsr[64] = {0};
    INT32 i = 0;
    
    #ifdef D07_DEBUG_ON
    S_D07_EventClear sTest ;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    pDataUser = (S_D07_EventClear *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            
            pDataUser = (S_D07_EventClear*)&sTest;
            //sOccurTime
            sTest.sOccurTime.YY = 0x07;
            sTest.sOccurTime.MM = 0x07;
            sTest.sOccurTime.DD = 0x07;
            sTest.sOccurTime.hh = 0x07;
            sTest.sOccurTime.mm = 0x07;
            sTest.sOccurTime.ss = 0x07;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;
            //sTotal
            sTest.ulEventID =  0x12345678;
        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"                     //sOccurTime
                        "%02d%02d%02d%02d"                            //operator
                        //ulEventID
                        "%08X",                
                        //sBgnTime
                        pDataUser->sOccurTime.YY,
                        pDataUser->sOccurTime.MM,
                        pDataUser->sOccurTime.DD,    
                        pDataUser->sOccurTime.hh,
                        pDataUser->sOccurTime.mm,
                        pDataUser->sOccurTime.ss,
                        //operator
                        pDataUser->operator[0],
                        pDataUser->operator[1],
                        pDataUser->operator[2],
                        pDataUser->operator[3],
                        //sTotal
                        (UINT32)pDataUser->ulEventID);
        
        d07_str2bcd(strUsr, (UINT8*)frame, 28);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, 14);

        //sOccurTime
        pDataUser->sOccurTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sOccurTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //operator
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        //ulEventID
        pDataUser->ulEventID = d07_hex_str_to_long(&strUsr[i+=2], 8);
        

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "OccurTime = %02d%02d%02d%02d%02d%02d\n" //sOccurTime
                    "operator %02d%02d%02d%02d\n"             //operator
                    //sTotal
                    "sTotal = 0x%08X\n",    
                    //sOccurTime
                    pDataUser->sOccurTime.YY,
                    pDataUser->sOccurTime.MM,
                    pDataUser->sOccurTime.DD,    
                    pDataUser->sOccurTime.hh,
                    pDataUser->sOccurTime.mm,
                    pDataUser->sOccurTime.ss,
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],
                    //sTotal
                    pDataUser->ulEventID);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_demand_clear
 功能描述  : 最在需量清零事件记录转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月26日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_demand_clear(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventDemandClear *pDataUser = NULL;
    char strUsr[512] = {0};
    char strBcd[512] = {0};
    INT32 i = 0;
    #ifdef D07_DEBUG_ON
    S_D07_EventDemandClear sTest ;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventDemandClear *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
        
            pDataUser = (S_D07_EventDemandClear*)&sTest;
            //sOccurTime
            sTest.sOccurTime.YY = 0x07;
            sTest.sOccurTime.MM = 0x07;
            sTest.sOccurTime.DD = 0x07;
            sTest.sOccurTime.hh = 0x07;
            sTest.sOccurTime.mm = 0x07;
            sTest.sOccurTime.ss = 0x07;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;
            //sTotal
            sTest.sTotal.sFortHaveDemand.fXX_XXXX = 12.3456;
            sTest.sTotal.sFortHaveDemand.YY = 01;
            sTest.sTotal.sFortHaveDemand.MM = 01;
            sTest.sTotal.sFortHaveDemand.DD = 01;
            sTest.sTotal.sFortHaveDemand.hh = 01;
            sTest.sTotal.sFortHaveDemand.mm = 01;
            sTest.sTotal.sBackHaveDemand.fXX_XXXX = 12.3456;
            sTest.sTotal.sBackHaveDemand.YY = 01;
            sTest.sTotal.sBackHaveDemand.MM = 01;
            sTest.sTotal.sBackHaveDemand.DD = 01;
            sTest.sTotal.sBackHaveDemand.hh = 01;
            sTest.sTotal.sBackHaveDemand.mm = 01;
            sTest.sTotal.sNoneDemandQuad1.fXX_XXXX = 12.3456;
            sTest.sTotal.sNoneDemandQuad1.YY = 01;
            sTest.sTotal.sNoneDemandQuad1.MM = 01;
            sTest.sTotal.sNoneDemandQuad1.DD = 01;
            sTest.sTotal.sNoneDemandQuad1.hh = 01;
            sTest.sTotal.sNoneDemandQuad1.mm = 01;
            sTest.sTotal.sNoneDemandQuad2.fXX_XXXX = 12.3456;
            sTest.sTotal.sNoneDemandQuad2.YY = 01;
            sTest.sTotal.sNoneDemandQuad2.MM = 01;
            sTest.sTotal.sNoneDemandQuad2.DD = 01;
            sTest.sTotal.sNoneDemandQuad2.hh = 01;
            sTest.sTotal.sNoneDemandQuad2.mm = 01;
            sTest.sTotal.sNoneDemandQuad3.fXX_XXXX = 12.3456;
            sTest.sTotal.sNoneDemandQuad3.YY = 01;
            sTest.sTotal.sNoneDemandQuad3.MM = 01;
            sTest.sTotal.sNoneDemandQuad3.DD = 01;
            sTest.sTotal.sNoneDemandQuad3.hh = 01;
            sTest.sTotal.sNoneDemandQuad3.mm = 01;
            sTest.sTotal.sNoneDemandQuad4.fXX_XXXX = 12.3456;
            sTest.sTotal.sNoneDemandQuad4.YY = 01;
            sTest.sTotal.sNoneDemandQuad4.MM = 01;
            sTest.sTotal.sNoneDemandQuad4.DD = 01;
            sTest.sTotal.sNoneDemandQuad4.hh = 01;
            sTest.sTotal.sNoneDemandQuad4.mm = 01;
            //sPhaseA
            sTest.sPhaseA.sFortHaveDemand.fXX_XXXX = 12.3456;
            sTest.sPhaseA.sFortHaveDemand.YY = 02;
            sTest.sPhaseA.sFortHaveDemand.MM = 02;
            sTest.sPhaseA.sFortHaveDemand.DD = 02;
            sTest.sPhaseA.sFortHaveDemand.hh = 02;
            sTest.sPhaseA.sFortHaveDemand.mm = 02;
            sTest.sPhaseA.sBackHaveDemand.fXX_XXXX = 12.3456;
            sTest.sPhaseA.sBackHaveDemand.YY = 02;
            sTest.sPhaseA.sBackHaveDemand.MM = 02;
            sTest.sPhaseA.sBackHaveDemand.DD = 02;
            sTest.sPhaseA.sBackHaveDemand.hh = 02;
            sTest.sPhaseA.sBackHaveDemand.mm = 02;
            sTest.sPhaseA.sNoneDemandQuad1.fXX_XXXX = 12.3456;
            sTest.sPhaseA.sNoneDemandQuad1.YY = 02;
            sTest.sPhaseA.sNoneDemandQuad1.MM = 02;
            sTest.sPhaseA.sNoneDemandQuad1.DD = 02;
            sTest.sPhaseA.sNoneDemandQuad1.hh = 02;
            sTest.sPhaseA.sNoneDemandQuad1.mm = 02;
            sTest.sPhaseA.sNoneDemandQuad2.fXX_XXXX = 12.3456;
            sTest.sPhaseA.sNoneDemandQuad2.YY = 02;
            sTest.sPhaseA.sNoneDemandQuad2.MM = 02;
            sTest.sPhaseA.sNoneDemandQuad2.DD = 02;
            sTest.sPhaseA.sNoneDemandQuad2.hh = 02;
            sTest.sPhaseA.sNoneDemandQuad2.mm = 02;
            sTest.sPhaseA.sNoneDemandQuad3.fXX_XXXX = 12.3456;
            sTest.sPhaseA.sNoneDemandQuad3.YY = 02;
            sTest.sPhaseA.sNoneDemandQuad3.MM = 02;
            sTest.sPhaseA.sNoneDemandQuad3.DD = 02;
            sTest.sPhaseA.sNoneDemandQuad3.hh = 02;
            sTest.sPhaseA.sNoneDemandQuad3.mm = 02;
            sTest.sPhaseA.sNoneDemandQuad4.fXX_XXXX = 12.3456;
            sTest.sPhaseA.sNoneDemandQuad4.YY = 02;
            sTest.sPhaseA.sNoneDemandQuad4.MM = 02;
            sTest.sPhaseA.sNoneDemandQuad4.DD = 02;
            sTest.sPhaseA.sNoneDemandQuad4.hh = 02;
            sTest.sPhaseA.sNoneDemandQuad4.mm = 02;
            //sPhaseB
            sTest.sPhaseB.sFortHaveDemand.fXX_XXXX = 12.3456;
            sTest.sPhaseB.sFortHaveDemand.YY = 03;
            sTest.sPhaseB.sFortHaveDemand.MM = 03;
            sTest.sPhaseB.sFortHaveDemand.DD = 03;
            sTest.sPhaseB.sFortHaveDemand.hh = 03;
            sTest.sPhaseB.sFortHaveDemand.mm = 03;
            sTest.sPhaseB.sBackHaveDemand.fXX_XXXX = 12.3456;
            sTest.sPhaseB.sBackHaveDemand.YY = 03;
            sTest.sPhaseB.sBackHaveDemand.MM = 03;
            sTest.sPhaseB.sBackHaveDemand.DD = 03;
            sTest.sPhaseB.sBackHaveDemand.hh = 03;
            sTest.sPhaseB.sBackHaveDemand.mm = 03;
            sTest.sPhaseB.sNoneDemandQuad1.fXX_XXXX = 12.3456;
            sTest.sPhaseB.sNoneDemandQuad1.YY = 03;
            sTest.sPhaseB.sNoneDemandQuad1.MM = 03;
            sTest.sPhaseB.sNoneDemandQuad1.DD = 03;
            sTest.sPhaseB.sNoneDemandQuad1.hh = 03;
            sTest.sPhaseB.sNoneDemandQuad1.mm = 03;
            sTest.sPhaseB.sNoneDemandQuad2.fXX_XXXX = 12.3456;
            sTest.sPhaseB.sNoneDemandQuad2.YY = 03;
            sTest.sPhaseB.sNoneDemandQuad2.MM = 03;
            sTest.sPhaseB.sNoneDemandQuad2.DD = 03;
            sTest.sPhaseB.sNoneDemandQuad2.hh = 03;
            sTest.sPhaseB.sNoneDemandQuad2.mm = 03;
            sTest.sPhaseB.sNoneDemandQuad3.fXX_XXXX = 12.3456;
            sTest.sPhaseB.sNoneDemandQuad3.YY = 03;
            sTest.sPhaseB.sNoneDemandQuad3.MM = 03;
            sTest.sPhaseB.sNoneDemandQuad3.DD = 03;
            sTest.sPhaseB.sNoneDemandQuad3.hh = 03;
            sTest.sPhaseB.sNoneDemandQuad3.mm = 03;
            sTest.sPhaseB.sNoneDemandQuad4.fXX_XXXX = 12.3456;
            sTest.sPhaseB.sNoneDemandQuad4.YY = 03;
            sTest.sPhaseB.sNoneDemandQuad4.MM = 03;
            sTest.sPhaseB.sNoneDemandQuad4.DD = 03;
            sTest.sPhaseB.sNoneDemandQuad4.hh = 03;
            sTest.sPhaseB.sNoneDemandQuad4.mm = 03;
            //sPhaseC
            sTest.sPhaseC.sFortHaveDemand.fXX_XXXX = 12.3456;
            sTest.sPhaseC.sFortHaveDemand.YY = 04;
            sTest.sPhaseC.sFortHaveDemand.MM = 04;
            sTest.sPhaseC.sFortHaveDemand.DD = 04;
            sTest.sPhaseC.sFortHaveDemand.hh = 04;
            sTest.sPhaseC.sFortHaveDemand.mm = 04;
            sTest.sPhaseC.sBackHaveDemand.fXX_XXXX = 12.3456;
            sTest.sPhaseC.sBackHaveDemand.YY = 04;
            sTest.sPhaseC.sBackHaveDemand.MM = 04;
            sTest.sPhaseC.sBackHaveDemand.DD = 04;
            sTest.sPhaseC.sBackHaveDemand.hh = 04;
            sTest.sPhaseC.sBackHaveDemand.mm = 04;
            sTest.sPhaseC.sNoneDemandQuad1.fXX_XXXX = 12.3456;
            sTest.sPhaseC.sNoneDemandQuad1.YY = 04;
            sTest.sPhaseC.sNoneDemandQuad1.MM = 04;
            sTest.sPhaseC.sNoneDemandQuad1.DD = 04;
            sTest.sPhaseC.sNoneDemandQuad1.hh = 04;
            sTest.sPhaseC.sNoneDemandQuad1.mm = 04;
            sTest.sPhaseC.sNoneDemandQuad2.fXX_XXXX = 12.3456;
            sTest.sPhaseC.sNoneDemandQuad2.YY = 04;
            sTest.sPhaseC.sNoneDemandQuad2.MM = 04;
            sTest.sPhaseC.sNoneDemandQuad2.DD = 04;
            sTest.sPhaseC.sNoneDemandQuad2.hh = 04;
            sTest.sPhaseC.sNoneDemandQuad2.mm = 04;
            sTest.sPhaseC.sNoneDemandQuad3.fXX_XXXX = 12.3456;
            sTest.sPhaseC.sNoneDemandQuad3.YY = 04;
            sTest.sPhaseC.sNoneDemandQuad3.MM = 04;
            sTest.sPhaseC.sNoneDemandQuad3.DD = 04;
            sTest.sPhaseC.sNoneDemandQuad3.hh = 04;
            sTest.sPhaseC.sNoneDemandQuad3.mm = 04;
            sTest.sPhaseC.sNoneDemandQuad4.fXX_XXXX = 12.3456;
            sTest.sPhaseC.sNoneDemandQuad4.YY = 04;
            sTest.sPhaseC.sNoneDemandQuad4.MM = 04;
            sTest.sPhaseC.sNoneDemandQuad4.DD = 04;
            sTest.sPhaseC.sNoneDemandQuad4.hh = 04;
            sTest.sPhaseC.sNoneDemandQuad4.mm = 04;

        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"                     //sOccurTime
                        "%02d%02d%02d%02d"                            //operator
                        //sTotal
                        "%06.4f%02d%02d%02d%02d%02d"    
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        //sPhaseA
                        "%06.4f%02d%02d%02d%02d%02d"    
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        //sTotal
                        "%06.4f%02d%02d%02d%02d%02d"    
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        //sPhaseA
                        "%06.4f%02d%02d%02d%02d%02d"    
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d"
                        "%06.4f%02d%02d%02d%02d%02d",                
                        //sBgnTime
                        pDataUser->sOccurTime.YY,
                        pDataUser->sOccurTime.MM,
                        pDataUser->sOccurTime.DD,    
                        pDataUser->sOccurTime.hh,
                        pDataUser->sOccurTime.mm,
                        pDataUser->sOccurTime.ss,
                        //operator
                        pDataUser->operator[0],
                        pDataUser->operator[1],
                        pDataUser->operator[2],
                        pDataUser->operator[3],
                        //sTotal
                        pDataUser->sTotal.sFortHaveDemand.fXX_XXXX,
                        pDataUser->sTotal.sFortHaveDemand.YY,
                        pDataUser->sTotal.sFortHaveDemand.MM,
                        pDataUser->sTotal.sFortHaveDemand.DD,
                        pDataUser->sTotal.sFortHaveDemand.hh,
                        pDataUser->sTotal.sFortHaveDemand.mm,
                        pDataUser->sTotal.sBackHaveDemand.fXX_XXXX,
                        pDataUser->sTotal.sBackHaveDemand.YY,
                        pDataUser->sTotal.sBackHaveDemand.MM,
                        pDataUser->sTotal.sBackHaveDemand.DD,
                        pDataUser->sTotal.sBackHaveDemand.hh,
                        pDataUser->sTotal.sBackHaveDemand.mm,
                        pDataUser->sTotal.sNoneDemandQuad1.fXX_XXXX,
                        pDataUser->sTotal.sNoneDemandQuad1.YY,
                        pDataUser->sTotal.sNoneDemandQuad1.MM,
                        pDataUser->sTotal.sNoneDemandQuad1.DD,
                        pDataUser->sTotal.sNoneDemandQuad1.hh,
                        pDataUser->sTotal.sNoneDemandQuad1.mm,
                        pDataUser->sTotal.sNoneDemandQuad2.fXX_XXXX,
                        pDataUser->sTotal.sNoneDemandQuad2.YY,
                        pDataUser->sTotal.sNoneDemandQuad2.MM,
                        pDataUser->sTotal.sNoneDemandQuad2.DD,
                        pDataUser->sTotal.sNoneDemandQuad2.hh,
                        pDataUser->sTotal.sNoneDemandQuad2.mm,
                        pDataUser->sTotal.sNoneDemandQuad3.fXX_XXXX,
                        pDataUser->sTotal.sNoneDemandQuad3.YY,
                        pDataUser->sTotal.sNoneDemandQuad3.MM,
                        pDataUser->sTotal.sNoneDemandQuad3.DD,
                        pDataUser->sTotal.sNoneDemandQuad3.hh,
                        pDataUser->sTotal.sNoneDemandQuad3.mm,
                        pDataUser->sTotal.sNoneDemandQuad4.fXX_XXXX,
                        pDataUser->sTotal.sNoneDemandQuad4.YY,
                        pDataUser->sTotal.sNoneDemandQuad4.MM,
                        pDataUser->sTotal.sNoneDemandQuad4.DD,
                        pDataUser->sTotal.sNoneDemandQuad4.hh,
                        pDataUser->sTotal.sNoneDemandQuad4.mm,
                        //sPhaseA
                        pDataUser->sPhaseA.sFortHaveDemand.fXX_XXXX,
                        pDataUser->sPhaseA.sFortHaveDemand.YY,
                        pDataUser->sPhaseA.sFortHaveDemand.MM,
                        pDataUser->sPhaseA.sFortHaveDemand.DD,
                        pDataUser->sPhaseA.sFortHaveDemand.hh,
                        pDataUser->sPhaseA.sFortHaveDemand.mm,
                        pDataUser->sPhaseA.sBackHaveDemand.fXX_XXXX,
                        pDataUser->sPhaseA.sBackHaveDemand.YY,
                        pDataUser->sPhaseA.sBackHaveDemand.MM,
                        pDataUser->sPhaseA.sBackHaveDemand.DD,
                        pDataUser->sPhaseA.sBackHaveDemand.hh,
                        pDataUser->sPhaseA.sBackHaveDemand.mm,
                        pDataUser->sPhaseA.sNoneDemandQuad1.fXX_XXXX,
                        pDataUser->sPhaseA.sNoneDemandQuad1.YY,
                        pDataUser->sPhaseA.sNoneDemandQuad1.MM,
                        pDataUser->sPhaseA.sNoneDemandQuad1.DD,
                        pDataUser->sPhaseA.sNoneDemandQuad1.hh,
                        pDataUser->sPhaseA.sNoneDemandQuad1.mm,
                        pDataUser->sPhaseA.sNoneDemandQuad2.fXX_XXXX,
                        pDataUser->sPhaseA.sNoneDemandQuad2.YY,
                        pDataUser->sPhaseA.sNoneDemandQuad2.MM,
                        pDataUser->sPhaseA.sNoneDemandQuad2.DD,
                        pDataUser->sPhaseA.sNoneDemandQuad2.hh,
                        pDataUser->sPhaseA.sNoneDemandQuad2.mm,
                        pDataUser->sPhaseA.sNoneDemandQuad3.fXX_XXXX,
                        pDataUser->sPhaseA.sNoneDemandQuad3.YY,
                        pDataUser->sPhaseA.sNoneDemandQuad3.MM,
                        pDataUser->sPhaseA.sNoneDemandQuad3.DD,
                        pDataUser->sPhaseA.sNoneDemandQuad3.hh,
                        pDataUser->sPhaseA.sNoneDemandQuad3.mm,
                        pDataUser->sPhaseA.sNoneDemandQuad4.fXX_XXXX,
                        pDataUser->sPhaseA.sNoneDemandQuad4.YY,
                        pDataUser->sPhaseA.sNoneDemandQuad4.MM,
                        pDataUser->sPhaseA.sNoneDemandQuad4.DD,
                        pDataUser->sPhaseA.sNoneDemandQuad4.hh,
                        pDataUser->sPhaseA.sNoneDemandQuad4.mm,
                        //sPhaseB
                        pDataUser->sPhaseB.sFortHaveDemand.fXX_XXXX,
                        pDataUser->sPhaseB.sFortHaveDemand.YY ,
                        pDataUser->sPhaseB.sFortHaveDemand.MM ,
                        pDataUser->sPhaseB.sFortHaveDemand.DD ,
                        pDataUser->sPhaseB.sFortHaveDemand.hh ,
                        pDataUser->sPhaseB.sFortHaveDemand.mm ,
                        pDataUser->sPhaseB.sBackHaveDemand.fXX_XXXX,
                        pDataUser->sPhaseB.sBackHaveDemand.YY ,
                        pDataUser->sPhaseB.sBackHaveDemand.MM ,
                        pDataUser->sPhaseB.sBackHaveDemand.DD ,
                        pDataUser->sPhaseB.sBackHaveDemand.hh ,
                        pDataUser->sPhaseB.sBackHaveDemand.mm ,
                        pDataUser->sPhaseB.sNoneDemandQuad1.fXX_XXXX,
                        pDataUser->sPhaseB.sNoneDemandQuad1.YY ,
                        pDataUser->sPhaseB.sNoneDemandQuad1.MM ,
                        pDataUser->sPhaseB.sNoneDemandQuad1.DD ,
                        pDataUser->sPhaseB.sNoneDemandQuad1.hh ,
                        pDataUser->sPhaseB.sNoneDemandQuad1.mm ,
                        pDataUser->sPhaseB.sNoneDemandQuad2.fXX_XXXX,
                        pDataUser->sPhaseB.sNoneDemandQuad2.YY ,
                        pDataUser->sPhaseB.sNoneDemandQuad2.MM ,
                        pDataUser->sPhaseB.sNoneDemandQuad2.DD ,
                        pDataUser->sPhaseB.sNoneDemandQuad2.hh ,
                        pDataUser->sPhaseB.sNoneDemandQuad2.mm ,
                        pDataUser->sPhaseB.sNoneDemandQuad3.fXX_XXXX,
                        pDataUser->sPhaseB.sNoneDemandQuad3.YY ,
                        pDataUser->sPhaseB.sNoneDemandQuad3.MM ,
                        pDataUser->sPhaseB.sNoneDemandQuad3.DD ,
                        pDataUser->sPhaseB.sNoneDemandQuad3.hh ,
                        pDataUser->sPhaseB.sNoneDemandQuad3.mm ,
                        pDataUser->sPhaseB.sNoneDemandQuad4.fXX_XXXX,
                        pDataUser->sPhaseB.sNoneDemandQuad4.YY ,
                        pDataUser->sPhaseB.sNoneDemandQuad4.MM ,
                        pDataUser->sPhaseB.sNoneDemandQuad4.DD ,
                        pDataUser->sPhaseB.sNoneDemandQuad4.hh ,
                        pDataUser->sPhaseB.sNoneDemandQuad4.mm ,
                        //sPhaseC
                        pDataUser->sPhaseC.sFortHaveDemand.fXX_XXXX,
                        pDataUser->sPhaseC.sFortHaveDemand.YY,
                        pDataUser->sPhaseC.sFortHaveDemand.MM,
                        pDataUser->sPhaseC.sFortHaveDemand.DD,
                        pDataUser->sPhaseC.sFortHaveDemand.hh,
                        pDataUser->sPhaseC.sFortHaveDemand.mm,
                        pDataUser->sPhaseC.sBackHaveDemand.fXX_XXXX,
                        pDataUser->sPhaseC.sBackHaveDemand.YY,
                        pDataUser->sPhaseC.sBackHaveDemand.MM,
                        pDataUser->sPhaseC.sBackHaveDemand.DD,
                        pDataUser->sPhaseC.sBackHaveDemand.hh,
                        pDataUser->sPhaseC.sBackHaveDemand.mm,
                        pDataUser->sPhaseC.sNoneDemandQuad1.fXX_XXXX,
                        pDataUser->sPhaseC.sNoneDemandQuad1.YY,
                        pDataUser->sPhaseC.sNoneDemandQuad1.MM,
                        pDataUser->sPhaseC.sNoneDemandQuad1.DD,
                        pDataUser->sPhaseC.sNoneDemandQuad1.hh,
                        pDataUser->sPhaseC.sNoneDemandQuad1.mm,
                        pDataUser->sPhaseC.sNoneDemandQuad2.fXX_XXXX,
                        pDataUser->sPhaseC.sNoneDemandQuad2.YY,
                        pDataUser->sPhaseC.sNoneDemandQuad2.MM,
                        pDataUser->sPhaseC.sNoneDemandQuad2.DD,
                        pDataUser->sPhaseC.sNoneDemandQuad2.hh,
                        pDataUser->sPhaseC.sNoneDemandQuad2.mm,
                        pDataUser->sPhaseC.sNoneDemandQuad3.fXX_XXXX,
                        pDataUser->sPhaseC.sNoneDemandQuad3.YY,
                        pDataUser->sPhaseC.sNoneDemandQuad3.MM,
                        pDataUser->sPhaseC.sNoneDemandQuad3.DD,
                        pDataUser->sPhaseC.sNoneDemandQuad3.hh,
                        pDataUser->sPhaseC.sNoneDemandQuad3.mm,
                        pDataUser->sPhaseC.sNoneDemandQuad4.fXX_XXXX,
                        pDataUser->sPhaseC.sNoneDemandQuad4.YY,
                        pDataUser->sPhaseC.sNoneDemandQuad4.MM,
                        pDataUser->sPhaseC.sNoneDemandQuad4.DD,
                        pDataUser->sPhaseC.sNoneDemandQuad4.hh,
                        pDataUser->sPhaseC.sNoneDemandQuad4.mm);
        
        d07_remove_dot(D07_DATA_LEN_RECD_DEAMD_CLEAR  * 2,strUsr, strBcd);
        d07_str2bcd(strBcd, (UINT8*)frame, D07_DATA_LEN_RECD_DEAMD_CLEAR *2);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, D07_DATA_LEN_RECD_DEAMD_CLEAR);

        //sOccurTime
        pDataUser->sOccurTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sOccurTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //operator
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        //sTotal
        pDataUser->sTotal.sFortHaveDemand.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sTotal.sFortHaveDemand.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sTotal.sFortHaveDemand.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sFortHaveDemand.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sFortHaveDemand.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sFortHaveDemand.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sBackHaveDemand.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sTotal.sBackHaveDemand.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sTotal.sBackHaveDemand.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sBackHaveDemand.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sBackHaveDemand.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sBackHaveDemand.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad1.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sTotal.sNoneDemandQuad1.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sTotal.sNoneDemandQuad1.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad1.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad1.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad1.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad2.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sTotal.sNoneDemandQuad2.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sTotal.sNoneDemandQuad2.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad2.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad2.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad2.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad3.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sTotal.sNoneDemandQuad3.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sTotal.sNoneDemandQuad3.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad3.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad3.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad3.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad4.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sTotal.sNoneDemandQuad4.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sTotal.sNoneDemandQuad4.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad4.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad4.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTotal.sNoneDemandQuad4.mm = d07_str_to_num(&strUsr[i+=2], 2);
        //sPhaseA
        pDataUser->sPhaseA.sFortHaveDemand.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseA.sFortHaveDemand.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseA.sFortHaveDemand.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sFortHaveDemand.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sFortHaveDemand.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sFortHaveDemand.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sBackHaveDemand.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseA.sBackHaveDemand.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseA.sBackHaveDemand.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sBackHaveDemand.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sBackHaveDemand.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sBackHaveDemand.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad1.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseA.sNoneDemandQuad1.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseA.sNoneDemandQuad1.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad1.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad1.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad1.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad2.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseA.sNoneDemandQuad2.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseA.sNoneDemandQuad2.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad2.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad2.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad2.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad3.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseA.sNoneDemandQuad3.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseA.sNoneDemandQuad3.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad3.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad3.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad3.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad4.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseA.sNoneDemandQuad4.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseA.sNoneDemandQuad4.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad4.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad4.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseA.sNoneDemandQuad4.mm = d07_str_to_num(&strUsr[i+=2], 2);
        //sPhaseB
        pDataUser->sPhaseB.sFortHaveDemand.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseB.sFortHaveDemand.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseB.sFortHaveDemand.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sFortHaveDemand.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sFortHaveDemand.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sFortHaveDemand.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sBackHaveDemand.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseB.sBackHaveDemand.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseB.sBackHaveDemand.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sBackHaveDemand.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sBackHaveDemand.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sBackHaveDemand.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad1.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseB.sNoneDemandQuad1.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseB.sNoneDemandQuad1.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad1.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad1.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad1.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad2.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseB.sNoneDemandQuad2.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseB.sNoneDemandQuad2.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad2.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad2.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad2.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad3.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseB.sNoneDemandQuad3.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseB.sNoneDemandQuad3.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad3.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad3.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad3.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad4.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseB.sNoneDemandQuad4.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseB.sNoneDemandQuad4.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad4.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad4.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseB.sNoneDemandQuad4.mm = d07_str_to_num(&strUsr[i+=2], 2);
        //sPhaseC
        pDataUser->sPhaseC.sFortHaveDemand.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseC.sFortHaveDemand.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseC.sFortHaveDemand.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sFortHaveDemand.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sFortHaveDemand.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sFortHaveDemand.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sBackHaveDemand.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseC.sBackHaveDemand.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseC.sBackHaveDemand.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sBackHaveDemand.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sBackHaveDemand.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sBackHaveDemand.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad1.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseC.sNoneDemandQuad1.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseC.sNoneDemandQuad1.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad1.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad1.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad1.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad2.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseC.sNoneDemandQuad2.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseC.sNoneDemandQuad2.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad2.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad2.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad2.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad3.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseC.sNoneDemandQuad3.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseC.sNoneDemandQuad3.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad3.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad3.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad3.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad4.fXX_XXXX = d07_str_to_double(&strUsr[i+=2], 6, 2);
        pDataUser->sPhaseC.sNoneDemandQuad4.YY = d07_str_to_num(&strUsr[i+=6], 2);
        pDataUser->sPhaseC.sNoneDemandQuad4.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad4.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad4.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sPhaseC.sNoneDemandQuad4.mm = d07_str_to_num(&strUsr[i+=2], 2);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "OccurTime = %02d%02d%02d%02d%02d%02d\n" //sOccurTime
                    "operator %02d%02d%02d%02d\n"             //operator
                    //sTotal
                    "sTotal\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"    
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    //sPhaseA
                    "sPhaseA\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"    
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    //sPhaseB
                    "sPhaseB\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"    
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    //sPhaseC
                    "sPhaseC\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"    
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n"
                    "%06.4f %02d%02d%02d%02d%02d\n",                
                    //sOccurTime
                    pDataUser->sOccurTime.YY,
                    pDataUser->sOccurTime.MM,
                    pDataUser->sOccurTime.DD,    
                    pDataUser->sOccurTime.hh,
                    pDataUser->sOccurTime.mm,
                    pDataUser->sOccurTime.ss,
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],
                    //sTotal
                    pDataUser->sTotal.sFortHaveDemand.fXX_XXXX,
                    pDataUser->sTotal.sFortHaveDemand.YY,
                    pDataUser->sTotal.sFortHaveDemand.MM,
                    pDataUser->sTotal.sFortHaveDemand.DD,
                    pDataUser->sTotal.sFortHaveDemand.hh,
                    pDataUser->sTotal.sFortHaveDemand.mm,
                    pDataUser->sTotal.sBackHaveDemand.fXX_XXXX,
                    pDataUser->sTotal.sBackHaveDemand.YY,
                    pDataUser->sTotal.sBackHaveDemand.MM,
                    pDataUser->sTotal.sBackHaveDemand.DD,
                    pDataUser->sTotal.sBackHaveDemand.hh,
                    pDataUser->sTotal.sBackHaveDemand.mm,
                    pDataUser->sTotal.sNoneDemandQuad1.fXX_XXXX,
                    pDataUser->sTotal.sNoneDemandQuad1.YY,
                    pDataUser->sTotal.sNoneDemandQuad1.MM,
                    pDataUser->sTotal.sNoneDemandQuad1.DD,
                    pDataUser->sTotal.sNoneDemandQuad1.hh,
                    pDataUser->sTotal.sNoneDemandQuad1.mm,
                    pDataUser->sTotal.sNoneDemandQuad2.fXX_XXXX,
                    pDataUser->sTotal.sNoneDemandQuad2.YY,
                    pDataUser->sTotal.sNoneDemandQuad2.MM,
                    pDataUser->sTotal.sNoneDemandQuad2.DD,
                    pDataUser->sTotal.sNoneDemandQuad2.hh,
                    pDataUser->sTotal.sNoneDemandQuad2.mm,
                    pDataUser->sTotal.sNoneDemandQuad3.fXX_XXXX,
                    pDataUser->sTotal.sNoneDemandQuad3.YY,
                    pDataUser->sTotal.sNoneDemandQuad3.MM,
                    pDataUser->sTotal.sNoneDemandQuad3.DD,
                    pDataUser->sTotal.sNoneDemandQuad3.hh,
                    pDataUser->sTotal.sNoneDemandQuad3.mm,
                    pDataUser->sTotal.sNoneDemandQuad4.fXX_XXXX,
                    pDataUser->sTotal.sNoneDemandQuad4.YY,
                    pDataUser->sTotal.sNoneDemandQuad4.MM,
                    pDataUser->sTotal.sNoneDemandQuad4.DD,
                    pDataUser->sTotal.sNoneDemandQuad4.hh,
                    pDataUser->sTotal.sNoneDemandQuad4.mm,
                    //sPhaseA
                    pDataUser->sPhaseA.sFortHaveDemand.fXX_XXXX,
                    pDataUser->sPhaseA.sFortHaveDemand.YY,
                    pDataUser->sPhaseA.sFortHaveDemand.MM,
                    pDataUser->sPhaseA.sFortHaveDemand.DD,
                    pDataUser->sPhaseA.sFortHaveDemand.hh,
                    pDataUser->sPhaseA.sFortHaveDemand.mm,
                    pDataUser->sPhaseA.sBackHaveDemand.fXX_XXXX,
                    pDataUser->sPhaseA.sBackHaveDemand.YY,
                    pDataUser->sPhaseA.sBackHaveDemand.MM,
                    pDataUser->sPhaseA.sBackHaveDemand.DD,
                    pDataUser->sPhaseA.sBackHaveDemand.hh,
                    pDataUser->sPhaseA.sBackHaveDemand.mm,
                    pDataUser->sPhaseA.sNoneDemandQuad1.fXX_XXXX,
                    pDataUser->sPhaseA.sNoneDemandQuad1.YY,
                    pDataUser->sPhaseA.sNoneDemandQuad1.MM,
                    pDataUser->sPhaseA.sNoneDemandQuad1.DD,
                    pDataUser->sPhaseA.sNoneDemandQuad1.hh,
                    pDataUser->sPhaseA.sNoneDemandQuad1.mm,
                    pDataUser->sPhaseA.sNoneDemandQuad2.fXX_XXXX,
                    pDataUser->sPhaseA.sNoneDemandQuad2.YY,
                    pDataUser->sPhaseA.sNoneDemandQuad2.MM,
                    pDataUser->sPhaseA.sNoneDemandQuad2.DD,
                    pDataUser->sPhaseA.sNoneDemandQuad2.hh,
                    pDataUser->sPhaseA.sNoneDemandQuad2.mm,
                    pDataUser->sPhaseA.sNoneDemandQuad3.fXX_XXXX,
                    pDataUser->sPhaseA.sNoneDemandQuad3.YY,
                    pDataUser->sPhaseA.sNoneDemandQuad3.MM,
                    pDataUser->sPhaseA.sNoneDemandQuad3.DD,
                    pDataUser->sPhaseA.sNoneDemandQuad3.hh,
                    pDataUser->sPhaseA.sNoneDemandQuad3.mm,
                    pDataUser->sPhaseA.sNoneDemandQuad4.fXX_XXXX,
                    pDataUser->sPhaseA.sNoneDemandQuad4.YY,
                    pDataUser->sPhaseA.sNoneDemandQuad4.MM,
                    pDataUser->sPhaseA.sNoneDemandQuad4.DD,
                    pDataUser->sPhaseA.sNoneDemandQuad4.hh,
                    pDataUser->sPhaseA.sNoneDemandQuad4.mm,
                    //sPhaseB
                    pDataUser->sPhaseB.sFortHaveDemand.fXX_XXXX,
                    pDataUser->sPhaseB.sFortHaveDemand.YY ,
                    pDataUser->sPhaseB.sFortHaveDemand.MM ,
                    pDataUser->sPhaseB.sFortHaveDemand.DD ,
                    pDataUser->sPhaseB.sFortHaveDemand.hh ,
                    pDataUser->sPhaseB.sFortHaveDemand.mm ,
                    pDataUser->sPhaseB.sBackHaveDemand.fXX_XXXX,
                    pDataUser->sPhaseB.sBackHaveDemand.YY ,
                    pDataUser->sPhaseB.sBackHaveDemand.MM ,
                    pDataUser->sPhaseB.sBackHaveDemand.DD ,
                    pDataUser->sPhaseB.sBackHaveDemand.hh ,
                    pDataUser->sPhaseB.sBackHaveDemand.mm ,
                    pDataUser->sPhaseB.sNoneDemandQuad1.fXX_XXXX,
                    pDataUser->sPhaseB.sNoneDemandQuad1.YY ,
                    pDataUser->sPhaseB.sNoneDemandQuad1.MM ,
                    pDataUser->sPhaseB.sNoneDemandQuad1.DD ,
                    pDataUser->sPhaseB.sNoneDemandQuad1.hh ,
                    pDataUser->sPhaseB.sNoneDemandQuad1.mm ,
                    pDataUser->sPhaseB.sNoneDemandQuad2.fXX_XXXX,
                    pDataUser->sPhaseB.sNoneDemandQuad2.YY ,
                    pDataUser->sPhaseB.sNoneDemandQuad2.MM ,
                    pDataUser->sPhaseB.sNoneDemandQuad2.DD ,
                    pDataUser->sPhaseB.sNoneDemandQuad2.hh ,
                    pDataUser->sPhaseB.sNoneDemandQuad2.mm ,
                    pDataUser->sPhaseB.sNoneDemandQuad3.fXX_XXXX,
                    pDataUser->sPhaseB.sNoneDemandQuad3.YY ,
                    pDataUser->sPhaseB.sNoneDemandQuad3.MM ,
                    pDataUser->sPhaseB.sNoneDemandQuad3.DD ,
                    pDataUser->sPhaseB.sNoneDemandQuad3.hh ,
                    pDataUser->sPhaseB.sNoneDemandQuad3.mm ,
                    pDataUser->sPhaseB.sNoneDemandQuad4.fXX_XXXX,
                    pDataUser->sPhaseB.sNoneDemandQuad4.YY ,
                    pDataUser->sPhaseB.sNoneDemandQuad4.MM ,
                    pDataUser->sPhaseB.sNoneDemandQuad4.DD ,
                    pDataUser->sPhaseB.sNoneDemandQuad4.hh ,
                    pDataUser->sPhaseB.sNoneDemandQuad4.mm ,
                    //sPhaseC
                    pDataUser->sPhaseC.sFortHaveDemand.fXX_XXXX,
                    pDataUser->sPhaseC.sFortHaveDemand.YY,
                    pDataUser->sPhaseC.sFortHaveDemand.MM,
                    pDataUser->sPhaseC.sFortHaveDemand.DD,
                    pDataUser->sPhaseC.sFortHaveDemand.hh,
                    pDataUser->sPhaseC.sFortHaveDemand.mm,
                    pDataUser->sPhaseC.sBackHaveDemand.fXX_XXXX,
                    pDataUser->sPhaseC.sBackHaveDemand.YY,
                    pDataUser->sPhaseC.sBackHaveDemand.MM,
                    pDataUser->sPhaseC.sBackHaveDemand.DD,
                    pDataUser->sPhaseC.sBackHaveDemand.hh,
                    pDataUser->sPhaseC.sBackHaveDemand.mm,
                    pDataUser->sPhaseC.sNoneDemandQuad1.fXX_XXXX,
                    pDataUser->sPhaseC.sNoneDemandQuad1.YY,
                    pDataUser->sPhaseC.sNoneDemandQuad1.MM,
                    pDataUser->sPhaseC.sNoneDemandQuad1.DD,
                    pDataUser->sPhaseC.sNoneDemandQuad1.hh,
                    pDataUser->sPhaseC.sNoneDemandQuad1.mm,
                    pDataUser->sPhaseC.sNoneDemandQuad2.fXX_XXXX,
                    pDataUser->sPhaseC.sNoneDemandQuad2.YY,
                    pDataUser->sPhaseC.sNoneDemandQuad2.MM,
                    pDataUser->sPhaseC.sNoneDemandQuad2.DD,
                    pDataUser->sPhaseC.sNoneDemandQuad2.hh,
                    pDataUser->sPhaseC.sNoneDemandQuad2.mm,
                    pDataUser->sPhaseC.sNoneDemandQuad3.fXX_XXXX,
                    pDataUser->sPhaseC.sNoneDemandQuad3.YY,
                    pDataUser->sPhaseC.sNoneDemandQuad3.MM,
                    pDataUser->sPhaseC.sNoneDemandQuad3.DD,
                    pDataUser->sPhaseC.sNoneDemandQuad3.hh,
                    pDataUser->sPhaseC.sNoneDemandQuad3.mm,
                    pDataUser->sPhaseC.sNoneDemandQuad4.fXX_XXXX,
                    pDataUser->sPhaseC.sNoneDemandQuad4.YY,
                    pDataUser->sPhaseC.sNoneDemandQuad4.MM,
                    pDataUser->sPhaseC.sNoneDemandQuad4.DD,
                    pDataUser->sPhaseC.sNoneDemandQuad4.hh,
                    pDataUser->sPhaseC.sNoneDemandQuad4.mm);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_meter_clear
 功能描述  : 电表清零事件数据结构
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月23日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_meter_clear(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventMeterClear *pDataUser = NULL;
    char strUsr[300] = {0};
    char strBcd[300] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }

    pDataUser = (S_D07_EventMeterClear *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            S_D07_EventMeterClear sTest ;
            pDataUser = (S_D07_EventMeterClear*)&sTest;
            //sOccurTime
            sTest.sOccurTime.YY = 0x07;
            sTest.sOccurTime.MM = 0x07;
            sTest.sOccurTime.DD = 0x07;
            sTest.sOccurTime.hh = 0x07;
            sTest.sOccurTime.mm = 0x07;
            sTest.sOccurTime.ss = 0x07;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;
            //sTotal
            sTest.sTotal.dForthHavePower = 123456.78;
            sTest.sTotal.dBackHavePower  = 123456.78;
            sTest.sTotal.dNonePowerQuad1 = 123456.78;
            sTest.sTotal.dNonePowerQuad2 = 123456.78;
            sTest.sTotal.dNonePowerQuad3 = 123456.78;
            sTest.sTotal.dNonePowerQuad4 = 123456.78;
            //sPhaseA
            sTest.sPhaseA.dForthHavePower = 123456.78;
            sTest.sPhaseA.dBackHavePower  = 123456.78;
            sTest.sPhaseA.dNonePowerQuad1 = 123456.78;
            sTest.sPhaseA.dNonePowerQuad2 = 123456.78;
            sTest.sPhaseA.dNonePowerQuad3 = 123456.78;
            sTest.sPhaseA.dNonePowerQuad4 = 123456.78;
            //sPhaseB
            sTest.sPhaseB.dForthHavePower = 123456.78;
            sTest.sPhaseB.dBackHavePower  = 123456.78;
            sTest.sPhaseB.dNonePowerQuad1 = 123456.78;
            sTest.sPhaseB.dNonePowerQuad2 = 123456.78;
            sTest.sPhaseB.dNonePowerQuad3 = 123456.78;
            sTest.sPhaseB.dNonePowerQuad4 = 123456.78;
            //sPhaseC
            sTest.sPhaseC.dForthHavePower = 123456.78;
            sTest.sPhaseC.dBackHavePower  = 123456.78;
            sTest.sPhaseC.dNonePowerQuad1 = 123456.78;
            sTest.sPhaseC.dNonePowerQuad2 = 123456.78;
            sTest.sPhaseC.dNonePowerQuad3 = 123456.78;
            sTest.sPhaseC.dNonePowerQuad4 = 123456.78;

        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"                     //sOccurTime
                        "%02d%02d%02d%02d"                            //operator
                        "%09.2f%09.2f%09.2f%09.2f%09.2f%09.2f"        //sTotal
                        "%09.2f%09.2f%09.2f%09.2f%09.2f%09.2f"        //sPhaseA
                        "%09.2f%09.2f%09.2f%09.2f%09.2f%09.2f"        //sPhaseB
                        "%09.2f%09.2f%09.2f%09.2f%09.2f%09.2f",        //sPhaseC        
                        //sBgnTime
                        pDataUser->sOccurTime.YY,
                        pDataUser->sOccurTime.MM,
                        pDataUser->sOccurTime.DD,    
                        pDataUser->sOccurTime.hh,
                        pDataUser->sOccurTime.mm,
                        pDataUser->sOccurTime.ss,
                        //operator
                        pDataUser->operator[0],
                        pDataUser->operator[1],
                        pDataUser->operator[2],
                        pDataUser->operator[3],
                        //sTotal
                        pDataUser->sTotal.dForthHavePower,
                        pDataUser->sTotal.dBackHavePower,
                        pDataUser->sTotal.dNonePowerQuad1,
                        pDataUser->sTotal.dNonePowerQuad2,
                        pDataUser->sTotal.dNonePowerQuad3,
                        pDataUser->sTotal.dNonePowerQuad4,
                        //sPhaseA
                        pDataUser->sPhaseA.dForthHavePower,
                        pDataUser->sPhaseA.dBackHavePower,
                        pDataUser->sPhaseA.dNonePowerQuad1,
                        pDataUser->sPhaseA.dNonePowerQuad2,
                        pDataUser->sPhaseA.dNonePowerQuad3,
                        pDataUser->sPhaseA.dNonePowerQuad4,
                        //sPhaseB
                        pDataUser->sPhaseB.dForthHavePower,
                        pDataUser->sPhaseB.dBackHavePower,
                        pDataUser->sPhaseB.dNonePowerQuad1,
                        pDataUser->sPhaseB.dNonePowerQuad2,
                        pDataUser->sPhaseB.dNonePowerQuad3,
                        pDataUser->sPhaseB.dNonePowerQuad4,
                        //sPhaseC
                        pDataUser->sPhaseC.dForthHavePower,
                        pDataUser->sPhaseC.dBackHavePower,
                        pDataUser->sPhaseC.dNonePowerQuad1,
                        pDataUser->sPhaseC.dNonePowerQuad2,
                        pDataUser->sPhaseC.dNonePowerQuad3,
                        pDataUser->sPhaseC.dNonePowerQuad4);
        
        d07_remove_dot(D07_DATA_LEN_RECD_METER_CLEAR  * 2,strUsr, strBcd);
        d07_str2bcd(strBcd, (UINT8*)frame, D07_DATA_LEN_RECD_METER_CLEAR *2);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, D07_DATA_LEN_RECD_METER_CLEAR);
        //sBgnTime
        pDataUser->sOccurTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sOccurTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sEndTime
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        //sTotal
        pDataUser->sTotal.dForthHavePower= d07_str_to_double(&strUsr[i+=2], 8, 6);
        pDataUser->sTotal.dBackHavePower = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sTotal.dNonePowerQuad1= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sTotal.dNonePowerQuad2= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sTotal.dNonePowerQuad3= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sTotal.dNonePowerQuad4= d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPhaseA
        pDataUser->sPhaseA.dForthHavePower= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseA.dBackHavePower = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseA.dNonePowerQuad1= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseA.dNonePowerQuad2= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseA.dNonePowerQuad3= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseA.dNonePowerQuad4= d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPhaseB
        pDataUser->sPhaseB.dForthHavePower= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseB.dBackHavePower = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseB.dNonePowerQuad1= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseB.dNonePowerQuad2= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseB.dNonePowerQuad3= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseB.dNonePowerQuad4= d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPhaseC
        pDataUser->sPhaseC.dForthHavePower= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseC.dBackHavePower = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseC.dNonePowerQuad1= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseC.dNonePowerQuad2= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseC.dNonePowerQuad3= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPhaseC.dNonePowerQuad4= d07_str_to_double(&strUsr[i+=8], 8, 6);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "sBgnTime = %02d%02d%02d%02d%02d%02d\n"                 //sBgnTime
                    "operator = %02d%02d%02d%02d\n"                            //operator
                    "sTotal  = %09.2f %09.2f %09.2f %09.2f %09.2f %09.2f\n"    //sPowerTotal
                    "sPhaseA = %09.2f %09.2f %09.2f% 09.2f %09.2f %09.2f\n"    //sPowerA
                    "sPhaseB = %09.2f %09.2f %09.2f %09.2f %09.2f %09.2f\n"    //sPowerB
                    "sPhaseC = %09.2f %09.2f %09.2f %09.2f %09.2f %09.2f\n",//sPowerC
                    //sBgnTime
                    pDataUser->sOccurTime.YY,
                    pDataUser->sOccurTime.MM,
                    pDataUser->sOccurTime.DD,    
                    pDataUser->sOccurTime.hh,
                    pDataUser->sOccurTime.mm,
                    pDataUser->sOccurTime.ss,
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],
                    //sTotal
                    pDataUser->sTotal.dForthHavePower,
                    pDataUser->sTotal.dBackHavePower ,
                    pDataUser->sTotal.dNonePowerQuad1,
                    pDataUser->sTotal.dNonePowerQuad2,
                    pDataUser->sTotal.dNonePowerQuad3,
                    pDataUser->sTotal.dNonePowerQuad4,
                    //sPhaseA
                    pDataUser->sPhaseA.dForthHavePower,
                    pDataUser->sPhaseA.dBackHavePower ,
                    pDataUser->sPhaseA.dNonePowerQuad1,
                    pDataUser->sPhaseA.dNonePowerQuad2,
                    pDataUser->sPhaseA.dNonePowerQuad3,
                    pDataUser->sPhaseA.dNonePowerQuad4,
                    //sPhaseB
                    pDataUser->sPhaseB.dForthHavePower,
                    pDataUser->sPhaseB.dBackHavePower ,
                    pDataUser->sPhaseB.dNonePowerQuad1,
                    pDataUser->sPhaseB.dNonePowerQuad2,
                    pDataUser->sPhaseB.dNonePowerQuad3,
                    pDataUser->sPhaseB.dNonePowerQuad4,
                    //sPhaseC
                    pDataUser->sPhaseC.dForthHavePower,
                    pDataUser->sPhaseC.dBackHavePower ,
                    pDataUser->sPhaseC.dNonePowerQuad1,
                    pDataUser->sPhaseC.dNonePowerQuad2,
                    pDataUser->sPhaseC.dNonePowerQuad3,
                    pDataUser->sPhaseC.dNonePowerQuad4);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_meter_prog
 功能描述  : 编程记录数据结构转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月23日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_meter_prog(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventMeterProgram *pDataUser = NULL;
    char str[256] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventMeterProgram *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            pDataUser->sProgTime.YY  = 0x11;
            pDataUser->sProgTime.MM  = 0x11;
            pDataUser->sProgTime.DD  = 0x11;
            pDataUser->sProgTime.hh  = 0x11;
            pDataUser->sProgTime.mm  = 0x11;
            pDataUser->sProgTime.ss  = 0x11;
            pDataUser->programer[0]  = 'A';
            pDataUser->programer[1]  = 'B';
            pDataUser->programer[2]  = 'C';
            pDataUser->programer[3]  = 'D';
            pDataUser->ulRulerID[0]  = 0x12345678;
            pDataUser->ulRulerID[1]  = 0x12345678;
            pDataUser->ulRulerID[2]  = 0x12345678;
            pDataUser->ulRulerID[3]  = 0x12345678;
            pDataUser->ulRulerID[4]  = 0x12345678;
            pDataUser->ulRulerID[5]  = 0x12345678;
            pDataUser->ulRulerID[6]  = 0x12345678;
            pDataUser->ulRulerID[7]  = 0x12345678;
            pDataUser->ulRulerID[8]  = 0x12345678;
            pDataUser->ulRulerID[9]  = 0x12345678;
        }
        #endif
        sprintf(str,
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d"
                "%08X%08X%08X%08X%08X"
                "%08X%08X%08X%08X%08X",
                pDataUser->sProgTime.YY,
                pDataUser->sProgTime.MM,
                pDataUser->sProgTime.DD,
                pDataUser->sProgTime.hh,
                pDataUser->sProgTime.mm,
                pDataUser->sProgTime.ss,
                (UINT32)pDataUser->programer[0],
                (UINT32)pDataUser->programer[1],
                (UINT32)pDataUser->programer[2],
                (UINT32)pDataUser->programer[3],
                (UINT32)pDataUser->ulRulerID[0],
                (UINT32)pDataUser->ulRulerID[1],
                (UINT32)pDataUser->ulRulerID[2],
                (UINT32)pDataUser->ulRulerID[3],
                (UINT32)pDataUser->ulRulerID[4],
                (UINT32)pDataUser->ulRulerID[5],
                (UINT32)pDataUser->ulRulerID[6],
                (UINT32)pDataUser->ulRulerID[7],
                (UINT32)pDataUser->ulRulerID[8],
                (UINT32)pDataUser->ulRulerID[9]);
        d07_str2bcd(str, (UINT8*)frame, D07_DATA_LEN_METER_PROG * 2);
        
    }
    else// frame to user interface
    {

        d07_bcd2str(frame,  str, D07_DATA_LEN_METER_PROG);
        pDataUser->sProgTime.YY = d07_str_to_num(&str[i], 2);
        pDataUser->sProgTime.MM = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sProgTime.DD = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sProgTime.hh = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sProgTime.mm = d07_str_to_num(&str[i+=2], 2);
        pDataUser->sProgTime.ss = d07_str_to_num(&str[i+=2], 2);
        pDataUser->programer[0] = d07_str_to_num(&str[i+=2], 2);
        pDataUser->programer[1] = d07_str_to_num(&str[i+=2], 2);
        pDataUser->programer[2] = d07_str_to_num(&str[i+=2], 2);
        pDataUser->programer[3] = d07_str_to_num(&str[i+=2], 2);
        pDataUser->ulRulerID[0] = (UINT32)d07_hex_str_to_long(&str[i+=2], 8);
        pDataUser->ulRulerID[1] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);
        pDataUser->ulRulerID[2] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);
        pDataUser->ulRulerID[3] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);
        pDataUser->ulRulerID[4] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);
        pDataUser->ulRulerID[5] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);
        pDataUser->ulRulerID[6] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);
        pDataUser->ulRulerID[7] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);
        pDataUser->ulRulerID[8] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);
        pDataUser->ulRulerID[9] = (UINT32)d07_hex_str_to_long(&str[i+=8], 8);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "sProgTime = %02d%02d%02d%02d%02d%02d\n"
                    "programer = %c%c%c%c\n"
                    "uRulerID[0~9]\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n",
                    pDataUser->sProgTime.YY,
                    pDataUser->sProgTime.MM,
                    pDataUser->sProgTime.DD,
                    pDataUser->sProgTime.hh,
                    pDataUser->sProgTime.mm,
                    pDataUser->sProgTime.ss,
                    (UINT32)pDataUser->programer[0],
                    (UINT32)pDataUser->programer[1],
                    (UINT32)pDataUser->programer[2],
                    (UINT32)pDataUser->programer[3],
                    (UINT32)pDataUser->ulRulerID[0],
                    (UINT32)pDataUser->ulRulerID[1],
                    (UINT32)pDataUser->ulRulerID[2],
                    (UINT32)pDataUser->ulRulerID[3],
                    (UINT32)pDataUser->ulRulerID[4],
                    (UINT32)pDataUser->ulRulerID[5],
                    (UINT32)pDataUser->ulRulerID[6],
                    (UINT32)pDataUser->ulRulerID[7],
                    (UINT32)pDataUser->ulRulerID[8],
                    (UINT32)pDataUser->ulRulerID[9]);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_volt_pass_percent
 功能描述  : 合格率事件数据结构
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月23日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_volt_pass_percent(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventPassPercent *pDataUser = NULL;
    INT32 nLenBCD = 27;
    INT32 nLenBcdStr = nLenBCD * 2;
    INT32 i = 0;
    char strUsr[300] = {0};
    char strBcd[300] = {0};
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventPassPercent *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            S_D07_EventPassPercent sTest ;
            pDataUser = (S_D07_EventPassPercent*)&sTest;

            sTest.ulMonitorVoltMinutes             = 111111;      //XXXXXX
            sTest.fVoltPassPercent                   = 1234.56;     //xxxx.xx            
            sTest.fVoltOverLimitPercent         = 1234.56;     //xxxx.xx    
            sTest.ulVoltOverUpperLimitMinutes     = 111111;    //XXXXXX
            sTest.ulVoltOverLowerLimitMinutes     = 111111;    //XXXXXX
            sTest.fVoltValueMax                 = 123.4;    //XXX.X
            sTest.sTimeVoltValueMax.MM             = 0x07;
            sTest.sTimeVoltValueMax.DD             = 0x07;
            sTest.sTimeVoltValueMax.hh             = 0x07;
            sTest.sTimeVoltValueMax.mm             = 0x07;
            sTest.fVoltValueMin                     = 123.4;    //XXX.X
            sTest.sTimeVoltValueMin.MM             = 0x07;
            sTest.sTimeVoltValueMin.DD             = 0x07;
            sTest.sTimeVoltValueMin.hh             = 0x07;
            sTest.sTimeVoltValueMin.mm             = 0x07;

        }
        #endif
        
        sprintf(strUsr, "%06d%06.2f%06.2f%06d%06d%04.1f%02d%02d%02d%02d%04.1f%02d%02d%02d%02d",        
                        (UINT32)pDataUser->ulMonitorVoltMinutes,
                        pDataUser->fVoltPassPercent,
                        pDataUser->fVoltOverLimitPercent,    
                        (UINT32)pDataUser->ulVoltOverUpperLimitMinutes,
                        (UINT32)pDataUser->ulVoltOverLowerLimitMinutes,
                        pDataUser->fVoltValueMax,
                        pDataUser->sTimeVoltValueMax.MM,
                        pDataUser->sTimeVoltValueMax.DD,
                        pDataUser->sTimeVoltValueMax.hh,
                        pDataUser->sTimeVoltValueMax.mm,
                        pDataUser->fVoltValueMin,
                        pDataUser->sTimeVoltValueMin.MM,
                        pDataUser->sTimeVoltValueMin.DD,
                        pDataUser->sTimeVoltValueMin.hh,
                        pDataUser->sTimeVoltValueMin.mm);
        d07_remove_dot(nLenBcdStr, strUsr, strBcd);
        d07_str2bcd(strBcd, (UINT8*)frame, nLenBcdStr);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, nLenBCD);

        pDataUser->ulMonitorVoltMinutes         = d07_str_to_num(&strUsr[0], 6);
        pDataUser->fVoltPassPercent             = d07_str_to_double(&strUsr[i+=6], 6, 4);
        pDataUser->fVoltOverLimitPercent        = d07_str_to_double(&strUsr[i+=6], 6, 4);
        pDataUser->ulVoltOverUpperLimitMinutes  = d07_str_to_num(&strUsr[i+=6], 6);
        pDataUser->ulVoltOverLowerLimitMinutes    = d07_str_to_num(&strUsr[i+=6], 6);
        pDataUser->fVoltValueMax                = d07_str_to_double(&strUsr[i+=6], 4, 3);
        pDataUser->sTimeVoltValueMax.MM            = d07_str_to_num(&strUsr[i+=4], 2);
        pDataUser->sTimeVoltValueMax.DD            = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTimeVoltValueMax.hh            = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTimeVoltValueMax.mm            = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->fVoltValueMin                = d07_str_to_double(&strUsr[i+=2], 4, 3);
        pDataUser->sTimeVoltValueMin.MM            = d07_str_to_num(&strUsr[i+=4], 2);
        pDataUser->sTimeVoltValueMin.DD            = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTimeVoltValueMin.hh            = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sTimeVoltValueMin.mm            = d07_str_to_num(&strUsr[i+=2], 2);

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    " XXXXXX  = %06d\n"
                    " XXXX.XX = %06.2f\n"
                    " XXXX.XX = %06.2f\n"
                    " XXXXXX  = %06d\n"
                    " XXXXXX  = %06d\n"
                    " XXX.X   = %04.1f\n"
                    " MM      = %02d\n"
                    " DD      = %02d\n"
                    " hh      = %02d\n"
                    " ss      = %02d\n"
                    " XXX.X   = %04.1f\n"
                    " MM      = %02d\n"
                    " DD      = %02d\n"
                    " hh      = %02d\n"
                    " ss      = %02d\n",        
                    (UINT32)pDataUser->ulMonitorVoltMinutes,
                    pDataUser->fVoltPassPercent,
                    pDataUser->fVoltOverLimitPercent,    
                    (UINT32)pDataUser->ulVoltOverUpperLimitMinutes,
                    (UINT32)pDataUser->ulVoltOverLowerLimitMinutes,
                    pDataUser->fVoltValueMax,
                    pDataUser->sTimeVoltValueMax.MM,
                    pDataUser->sTimeVoltValueMax.DD,
                    pDataUser->sTimeVoltValueMax.hh,
                    pDataUser->sTimeVoltValueMax.mm,
                    pDataUser->fVoltValueMin,
                    pDataUser->sTimeVoltValueMin.MM,
                    pDataUser->sTimeVoltValueMin.DD,
                    pDataUser->sTimeVoltValueMin.hh,
                    pDataUser->sTimeVoltValueMin.mm);
        }
        #endif
    }

    return E_D07_OK;
}
/*****************************************************************************
 函 数 名  : trans_d07_data_recd_weekday_prog
 功能描述  : 周休日编程事件记录数据结构转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月26日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_weekday_prog(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventWeekDay *pDataUser = NULL;
    char strUsr[64] = {0};
    INT32 i = 0;
    
    #ifdef D07_DEBUG_ON
    S_D07_EventWeekDay sTest ;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventWeekDay *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            
            pDataUser = (S_D07_EventWeekDay*)&sTest;
            //sOccurTime
            sTest.sOccurTime.YY = 0x07;
            sTest.sOccurTime.MM = 0x07;
            sTest.sOccurTime.DD = 0x07;
            sTest.sOccurTime.hh = 0x07;
            sTest.sOccurTime.mm = 0x07;
            sTest.sOccurTime.ss = 0x07;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;
            //sTotal
            sTest.ucTableNum =  0x12;
        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"                     //sOccurTime
                        "%02d%02d%02d%02d"                            //operator
                        //ulEventID
                        "%02d",                
                        //sBgnTime
                        pDataUser->sOccurTime.YY,
                        pDataUser->sOccurTime.MM,
                        pDataUser->sOccurTime.DD,    
                        pDataUser->sOccurTime.hh,
                        pDataUser->sOccurTime.mm,
                        pDataUser->sOccurTime.ss,
                        //operator
                        pDataUser->operator[0],
                        pDataUser->operator[1],
                        pDataUser->operator[2],
                        pDataUser->operator[3],
                        //ucTableNum
                        pDataUser->ucTableNum);
        
        d07_str2bcd(strUsr, (UINT8*)frame, 22);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, 11);

        //sOccurTime
        pDataUser->sOccurTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sOccurTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //operator
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        //ulEventID
        pDataUser->ucTableNum = d07_str_to_num(&strUsr[i+=2], 2);
        
        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "OccurTime = %02d%02d%02d%02d%02d%02d\n" //sOccurTime
                    "operator %02d%02d%02d%02d\n"             //operator
                    //sTotal
                    "sTotal = 0x%02X\n",    
                    //sOccurTime
                    pDataUser->sOccurTime.YY,
                    pDataUser->sOccurTime.MM,
                    pDataUser->sOccurTime.DD,    
                    pDataUser->sOccurTime.hh,
                    pDataUser->sOccurTime.mm,
                    pDataUser->sOccurTime.ss,
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],
                    //sTotal
                    pDataUser->ucTableNum);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_holiday_prog
 功能描述  : 节假日编程事件记录转换
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月26日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_holiday_prog(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventHoliday *pDataUser = NULL;
    char strUsr[128] = {0};
    INT32 j = 0;
    INT32 i = 0;    
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventHoliday *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            S_D07_EventHoliday sTest ;
            pDataUser = (S_D07_EventHoliday*)&sTest;
            //sOccurTime
            sTest.sOccurTime.YY = 0x07;
            sTest.sOccurTime.MM = 0x07;
            sTest.sOccurTime.DD = 0x07;
            sTest.sOccurTime.hh = 0x07;
            sTest.sOccurTime.mm = 0x07;
            sTest.sOccurTime.ss = 0x07;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;
            //sTotal
            INT32 i;
            for(i = 0; i < 12; i++)
            {
                sTest.sHoliday[i].YY = i;
                sTest.sHoliday[i].MM = i+1;
                sTest.sHoliday[i].DD = i+2;
                sTest.sHoliday[i].NN = i+3;
            }
        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr,
                "%02d%02d%02d%02d%02d%02d"                     //sOccurTime
                "%02d%02d%02d%02d"                            //operator
                //sHoliday
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d%02d%02d"
                "%02d%02d%02d%02d%02d%02d",                
                //sBgnTime
                pDataUser->sOccurTime.YY,
                pDataUser->sOccurTime.MM,
                pDataUser->sOccurTime.DD,    
                pDataUser->sOccurTime.hh,
                pDataUser->sOccurTime.mm,
                pDataUser->sOccurTime.ss,
                //operator
                pDataUser->operator[0],
                pDataUser->operator[1],
                pDataUser->operator[2],
                pDataUser->operator[3],
                //sHoliday
                pDataUser->sHoliday[0].YY,
                pDataUser->sHoliday[0].MM,
                pDataUser->sHoliday[0].DD,
                pDataUser->sHoliday[0].NN,
                pDataUser->sHoliday[1].YY,
                pDataUser->sHoliday[1].MM,
                pDataUser->sHoliday[1].DD,
                pDataUser->sHoliday[1].NN,
                pDataUser->sHoliday[2].YY,
                pDataUser->sHoliday[2].MM,
                pDataUser->sHoliday[2].DD,
                pDataUser->sHoliday[2].NN,
                pDataUser->sHoliday[3].YY,
                pDataUser->sHoliday[3].MM,
                pDataUser->sHoliday[3].DD,
                pDataUser->sHoliday[3].NN,
                pDataUser->sHoliday[4].YY,
                pDataUser->sHoliday[4].MM,
                pDataUser->sHoliday[4].DD,
                pDataUser->sHoliday[4].NN,
                pDataUser->sHoliday[5].YY,
                pDataUser->sHoliday[5].MM,
                pDataUser->sHoliday[5].DD,
                pDataUser->sHoliday[5].NN,
                pDataUser->sHoliday[6].YY,
                pDataUser->sHoliday[6].MM,
                pDataUser->sHoliday[6].DD,
                pDataUser->sHoliday[6].NN,
                pDataUser->sHoliday[7].YY,
                pDataUser->sHoliday[7].MM,
                pDataUser->sHoliday[7].DD,
                pDataUser->sHoliday[7].NN,
                pDataUser->sHoliday[8].YY,
                pDataUser->sHoliday[8].MM,
                pDataUser->sHoliday[8].DD,
                pDataUser->sHoliday[8].NN,
                pDataUser->sHoliday[9].YY,
                pDataUser->sHoliday[9].MM,
                pDataUser->sHoliday[9].DD,
                pDataUser->sHoliday[9].NN,
                pDataUser->sHoliday[10].YY,
                pDataUser->sHoliday[10].MM,
                pDataUser->sHoliday[10].DD,
                pDataUser->sHoliday[10].NN,
                pDataUser->sHoliday[11].YY,
                pDataUser->sHoliday[11].MM,
                pDataUser->sHoliday[11].DD,
                pDataUser->sHoliday[11].NN);
        
        d07_str2bcd(strUsr, (UINT8*)frame, 106);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, 58);

        //sOccurTime
        pDataUser->sOccurTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sOccurTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //operator
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        //ulEventID
        for(j = 0; j < 11; j++)
        {
            pDataUser->operator[j] = d07_str_to_num(&strUsr[i+=2], 2);
        }
        
        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "sOccurTime = %02d%02d%02d%02d%02d%02d\n"                     //sOccurTime
                    "operator = %02d%02d%02d%02d\n"                            //operator
                    //sHoliday
                    "%02d%02d%02d%02d%02d%02d"
                    "%02d%02d%02d%02d%02d%02d"
                    "%02d%02d%02d%02d%02d%02d"
                    "%02d%02d%02d%02d%02d%02d"
                    "%02d%02d%02d%02d%02d%02d"
                    "%02d%02d%02d%02d%02d%02d"
                    "%02d%02d%02d%02d%02d%02d"
                    "%02d%02d%02d%02d%02d%02d",                
                    //sBgnTime
                    pDataUser->sOccurTime.YY,
                    pDataUser->sOccurTime.MM,
                    pDataUser->sOccurTime.DD,    
                    pDataUser->sOccurTime.hh,
                    pDataUser->sOccurTime.mm,
                    pDataUser->sOccurTime.ss,
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],
                    //sHoliday
                    pDataUser->sHoliday[0].YY,
                    pDataUser->sHoliday[0].MM,
                    pDataUser->sHoliday[0].DD,
                    pDataUser->sHoliday[0].NN,
                    pDataUser->sHoliday[1].YY,
                    pDataUser->sHoliday[1].MM,
                    pDataUser->sHoliday[1].DD,
                    pDataUser->sHoliday[1].NN,
                    pDataUser->sHoliday[2].YY,
                    pDataUser->sHoliday[2].MM,
                    pDataUser->sHoliday[2].DD,
                    pDataUser->sHoliday[2].NN,
                    pDataUser->sHoliday[3].YY,
                    pDataUser->sHoliday[3].MM,
                    pDataUser->sHoliday[3].DD,
                    pDataUser->sHoliday[3].NN,
                    pDataUser->sHoliday[4].YY,
                    pDataUser->sHoliday[4].MM,
                    pDataUser->sHoliday[4].DD,
                    pDataUser->sHoliday[4].NN,
                    pDataUser->sHoliday[5].YY,
                    pDataUser->sHoliday[5].MM,
                    pDataUser->sHoliday[5].DD,
                    pDataUser->sHoliday[5].NN,
                    pDataUser->sHoliday[6].YY,
                    pDataUser->sHoliday[6].MM,
                    pDataUser->sHoliday[6].DD,
                    pDataUser->sHoliday[6].NN,
                    pDataUser->sHoliday[7].YY,
                    pDataUser->sHoliday[7].MM,
                    pDataUser->sHoliday[7].DD,
                    pDataUser->sHoliday[7].NN,
                    pDataUser->sHoliday[8].YY,
                    pDataUser->sHoliday[8].MM,
                    pDataUser->sHoliday[8].DD,
                    pDataUser->sHoliday[8].NN,
                    pDataUser->sHoliday[9].YY,
                    pDataUser->sHoliday[9].MM,
                    pDataUser->sHoliday[9].DD,
                    pDataUser->sHoliday[9].NN,
                    pDataUser->sHoliday[10].YY,
                    pDataUser->sHoliday[10].MM,
                    pDataUser->sHoliday[10].DD,
                    pDataUser->sHoliday[10].NN,
                    pDataUser->sHoliday[11].YY,
                    pDataUser->sHoliday[11].MM,
                    pDataUser->sHoliday[11].DD,
                    pDataUser->sHoliday[11].NN);
        }
        #endif
    }

    return E_D07_OK;
}


/*****************************************************************************
 函 数 名  : trans_d07_data_recd_power_prog
 功能描述  : 组合有功无功编程事件结构
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月27日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_power_prog(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventCombPower *pDataUser = NULL;
    char strUsr[64] = {0};
    INT32 i = 0;

    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventCombPower *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            S_D07_EventCombPower sTest ;
            pDataUser = (S_D07_EventCombPower*)&sTest;
            //sOccurTime
            sTest.sOccurTime.YY = 0x07;
            sTest.sOccurTime.MM = 0x07;
            sTest.sOccurTime.DD = 0x07;
            sTest.sOccurTime.hh = 0x07;
            sTest.sOccurTime.mm = 0x07;
            sTest.sOccurTime.ss = 0x07;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;
            //ucWord
            sTest.ucWord =  0x12;
        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"                     //sOccurTime
                        "%02d%02d%02d%02d"                            //operator
                        //ulEventID
                        "%02d",                
                        //sBgnTime
                        pDataUser->sOccurTime.YY,
                        pDataUser->sOccurTime.MM,
                        pDataUser->sOccurTime.DD,    
                        pDataUser->sOccurTime.hh,
                        pDataUser->sOccurTime.mm,
                        pDataUser->sOccurTime.ss,
                        //operator
                        pDataUser->operator[0],
                        pDataUser->operator[1],
                        pDataUser->operator[2],
                        pDataUser->operator[3],
                        //ucWord
                        pDataUser->ucWord);
        
        d07_str2bcd(strUsr, (UINT8*)frame, 22);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, 11);

        //sOccurTime
        pDataUser->sOccurTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sOccurTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //operator
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        //ucWord
        pDataUser->ucWord = d07_str_to_num(&strUsr[i+=2], 2);
        

        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "OccurTime = %02d%02d%02d%02d%02d%02d\n" //sOccurTime
                    "operator %02d%02d%02d%02d\n"             //operator
                    //sTotal
                    "sTotal = 0x%02X\n",    
                    //sOccurTime
                    pDataUser->sOccurTime.YY,
                    pDataUser->sOccurTime.MM,
                    pDataUser->sOccurTime.DD,    
                    pDataUser->sOccurTime.hh,
                    pDataUser->sOccurTime.mm,
                    pDataUser->sOccurTime.ss,
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],
                    //sTotal
                    pDataUser->ucWord);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_payoff_prog
 功能描述  : 结算日编程事件记录
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月27日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_payoff_prog(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventPayoff *pDataUser = NULL;
    char strUsr[64] = {0};
    INT32 i = 0;
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventPayoff *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            S_D07_EventPayoff sTest ;
            pDataUser = (S_D07_EventPayoff*)&sTest;
            //sOccurTime
            sTest.sOccurTime.YY = 0x07;
            sTest.sOccurTime.MM = 0x07;
            sTest.sOccurTime.DD = 0x07;
            sTest.sOccurTime.hh = 0x07;
            sTest.sOccurTime.mm = 0x07;
            sTest.sOccurTime.ss = 0x07;
            //operator
            sTest.operator[0] = 0x01;
            sTest.operator[1] = 0x02;
            sTest.operator[2] = 0x03;
            sTest.operator[3] = 0x04;
            //ucWord
            sTest.sDay1.DD =  0x12;
            sTest.sDay1.hh =  0x12;
            sTest.sDay2.DD =  0x12;
            sTest.sDay2.hh =  0x12;
            sTest.sDay3.DD =  0x12;
            sTest.sDay3.hh =  0x12;
        }
        #endif
        
        /* start time  字节总长 297*/
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"                     //sOccurTime
                        "%02d%02d%02d%02d"                            //operator
                        //ulEventID
                        "%02d%02d%02d%02d%02d%02d",                
                        //sBgnTime
                        pDataUser->sOccurTime.YY,
                        pDataUser->sOccurTime.MM,
                        pDataUser->sOccurTime.DD,    
                        pDataUser->sOccurTime.hh,
                        pDataUser->sOccurTime.mm,
                        pDataUser->sOccurTime.ss,
                        //operator
                        pDataUser->operator[0],
                        pDataUser->operator[1],
                        pDataUser->operator[2],
                        pDataUser->operator[3],
                        //ucWord
                        pDataUser->sDay1.DD,
                        pDataUser->sDay1.hh,
                        pDataUser->sDay2.DD,
                        pDataUser->sDay2.hh,
                        pDataUser->sDay3.DD,
                        pDataUser->sDay3.hh);
        
        d07_str2bcd(strUsr, (UINT8*)frame, 36);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, 18);
        //sOccurTime
        pDataUser->sOccurTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sOccurTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sOccurTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //operator
        pDataUser->operator[0] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[1] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[2] = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->operator[3] = d07_str_to_num(&strUsr[i+=2], 2);
        //sDay1
        pDataUser->sDay1.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sDay1.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sDay2.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sDay2.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sDay3.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sDay3.hh = d07_str_to_num(&strUsr[i+=2], 2);
        
        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "OccurTime = %02d%02d%02d%02d%02d%02d\n" //sOccurTime
                    "operator %02d%02d%02d%02d\n"             //operator
                    //sTotal
                    "sDay1 = 0x%02X 0x%02X\n"
                    "sDay2 = 0x%02X 0x%02X\n"
                    "sDay3 = 0x%02X 0x%02X\n",    
                    //sOccurTime
                    pDataUser->sOccurTime.YY,
                    pDataUser->sOccurTime.MM,
                    pDataUser->sOccurTime.DD,    
                    pDataUser->sOccurTime.hh,
                    pDataUser->sOccurTime.mm,
                    pDataUser->sOccurTime.ss,
                    //operator
                    pDataUser->operator[0],
                    pDataUser->operator[1],
                    pDataUser->operator[2],
                    pDataUser->operator[3],
                    //sTotal
                    pDataUser->sDay1.DD,
                    pDataUser->sDay1.hh,
                    pDataUser->sDay2.DD,
                    pDataUser->sDay2.hh,
                    pDataUser->sDay3.DD,
                    pDataUser->sDay3.hh);
        }
        #endif
    }

    return E_D07_OK;
}

/*****************************************************************************
 函 数 名  : trans_d07_data_recd_open_meter_cap
 功能描述  : 电表开盖事件记录结构
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年11月27日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_recd_open_meter_cap(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    S_D07_EventOpenCap *pDataUser = NULL;
    char strUsr[300] = {0};
    char strBcd[300] = {0};
    INT32 i = 0;

    #ifdef D07_DEBUG_ON
    S_D07_EventOpenCap sTest ;
    #endif
    
    if(NULL == user || NULL == frame)
    {
        return E_D07_ERRO_NULL;
    }
    
    pDataUser = (S_D07_EventOpenCap *)user;
    
    if(E_D07_TRANS_U2F == flag)
    {
        //显示数据信息
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
        
            pDataUser = (S_D07_EventOpenCap*)&sTest;
            //sBgnTime
            sTest.sBgnTime.YY = 0x07;
            sTest.sBgnTime.MM = 0x07;
            sTest.sBgnTime.DD = 0x07;
            sTest.sBgnTime.hh = 0x07;
            sTest.sBgnTime.mm = 0x07;
            sTest.sBgnTime.ss = 0x07;
            //sBgnTime
            sTest.sEndTime.YY = 0x07;
            sTest.sEndTime.MM = 0x07;
            sTest.sEndTime.DD = 0x07;
            sTest.sEndTime.hh = 0x07;
            sTest.sEndTime.mm = 0x07;
            sTest.sEndTime.ss = 0x07;

            //sPowerBeforOpen
            sTest.sPowerBeforOpen.dForthHavePower = 123456.78;
            sTest.sPowerBeforOpen.dBackHavePower  = 123456.78;
            sTest.sPowerBeforOpen.dNonePowerQuad1 = 123456.78;
            sTest.sPowerBeforOpen.dNonePowerQuad2 = 123456.78;
            sTest.sPowerBeforOpen.dNonePowerQuad3 = 123456.78;
            sTest.sPowerBeforOpen.dNonePowerQuad4 = 123456.78;
            //sPowerAfterOpen
            sTest.sPowerAfterOpen.dForthHavePower = 123456.78;
            sTest.sPowerAfterOpen.dBackHavePower  = 123456.78;
            sTest.sPowerAfterOpen.dNonePowerQuad1 = 123456.78;
            sTest.sPowerAfterOpen.dNonePowerQuad2 = 123456.78;
            sTest.sPowerAfterOpen.dNonePowerQuad3 = 123456.78;
            sTest.sPowerAfterOpen.dNonePowerQuad4 = 123456.78;

        }
        #endif
        
        sprintf(strUsr, "%02d%02d%02d%02d%02d%02d"                     //sBgnTime
                        "%02d%02d%02d%02d%02d%02d"                     //sBgnTime
                        "%09.2f%09.2f%09.2f%09.2f%09.2f%09.2f"        //sTotal
                        "%09.2f%09.2f%09.2f%09.2f%09.2f%09.2f",        //sPhaseA
                        //sBgnTime
                        pDataUser->sBgnTime.YY,
                        pDataUser->sBgnTime.MM,
                        pDataUser->sBgnTime.DD,    
                        pDataUser->sBgnTime.hh,
                        pDataUser->sBgnTime.mm,
                        pDataUser->sBgnTime.ss,
                        //sEndTime
                        pDataUser->sEndTime.YY,
                        pDataUser->sEndTime.MM,
                        pDataUser->sEndTime.DD,    
                        pDataUser->sEndTime.hh,
                        pDataUser->sEndTime.mm,
                        pDataUser->sEndTime.ss,
                        //sPowerBeforOpen
                        pDataUser->sPowerBeforOpen.dForthHavePower,
                        pDataUser->sPowerBeforOpen.dBackHavePower,
                        pDataUser->sPowerBeforOpen.dNonePowerQuad1,
                        pDataUser->sPowerBeforOpen.dNonePowerQuad2,
                        pDataUser->sPowerBeforOpen.dNonePowerQuad3,
                        pDataUser->sPowerBeforOpen.dNonePowerQuad4,
                        //sPowerAfterOpen
                        pDataUser->sPowerAfterOpen.dForthHavePower,
                        pDataUser->sPowerAfterOpen.dBackHavePower,
                        pDataUser->sPowerAfterOpen.dNonePowerQuad1,
                        pDataUser->sPowerAfterOpen.dNonePowerQuad2,
                        pDataUser->sPowerAfterOpen.dNonePowerQuad3,
                        pDataUser->sPowerAfterOpen.dNonePowerQuad4);
        
        d07_remove_dot(120,strUsr, strBcd);
        d07_str2bcd(strBcd, (UINT8*)frame, 120);
        
    }
    else// frame to user interface
    {
        d07_bcd2str(frame, strUsr, 60);
        
        //sBgnTime
        pDataUser->sBgnTime.YY = d07_str_to_num(&strUsr[ i  ], 2);
        pDataUser->sBgnTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sBgnTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sBgnTime
        pDataUser->sEndTime.YY = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.MM = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.DD = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.hh = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.mm = d07_str_to_num(&strUsr[i+=2], 2);
        pDataUser->sEndTime.ss = d07_str_to_num(&strUsr[i+=2], 2);
        //sTotal
        pDataUser->sPowerBeforOpen.dForthHavePower= d07_str_to_double(&strUsr[i+=2], 8, 6);
        pDataUser->sPowerBeforOpen.dBackHavePower = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerBeforOpen.dNonePowerQuad1= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerBeforOpen.dNonePowerQuad2= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerBeforOpen.dNonePowerQuad3= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerBeforOpen.dNonePowerQuad4= d07_str_to_double(&strUsr[i+=8], 8, 6);
        //sPhaseA
        pDataUser->sPowerAfterOpen.dForthHavePower= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerAfterOpen.dBackHavePower = d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerAfterOpen.dNonePowerQuad1= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerAfterOpen.dNonePowerQuad2= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerAfterOpen.dNonePowerQuad3= d07_str_to_double(&strUsr[i+=8], 8, 6);
        pDataUser->sPowerAfterOpen.dNonePowerQuad4= d07_str_to_double(&strUsr[i+=8], 8, 6);
        
        //数据输出
        #ifdef D07_DEBUG_ON
        if(D07_ON == g_need_pack_07)
        {
            memset((void *)g_out_data_07, 0x00, OUT_BUF_LEN);
            sprintf(g_out_data_07, 
                    "sBgnTime = %02d%02d%02d%02d%02d%02d\n"                 //sBgnTime
                    "sBgnTime = %02d%02d%02d%02d%02d%02d\n"                 //sEndTime
                    "sPowerBeforOpen  = %09.2f %09.2f %09.2f %09.2f %09.2f %09.2f\n"    //sPowerBeforOpen
                    "sPowerAfterOpen  = %09.2f %09.2f %09.2f% 09.2f %09.2f %09.2f\n",    //sPowerAfterOpen
                    pDataUser->sBgnTime.YY,
                    pDataUser->sBgnTime.MM,
                    pDataUser->sBgnTime.DD,    
                    pDataUser->sBgnTime.hh,
                    pDataUser->sBgnTime.mm,
                    pDataUser->sBgnTime.ss,
                    //sEndTime
                    pDataUser->sEndTime.YY,
                    pDataUser->sEndTime.MM,
                    pDataUser->sEndTime.DD,    
                    pDataUser->sEndTime.hh,
                    pDataUser->sEndTime.mm,
                    pDataUser->sEndTime.ss,
                    //sPowerBeforOpen
                    pDataUser->sPowerBeforOpen.dForthHavePower,
                    pDataUser->sPowerBeforOpen.dBackHavePower,
                    pDataUser->sPowerBeforOpen.dNonePowerQuad1,
                    pDataUser->sPowerBeforOpen.dNonePowerQuad2,
                    pDataUser->sPowerBeforOpen.dNonePowerQuad3,
                    pDataUser->sPowerBeforOpen.dNonePowerQuad4,
                    //sPowerAfterOpen
                    pDataUser->sPowerAfterOpen.dForthHavePower,
                    pDataUser->sPowerAfterOpen.dBackHavePower,
                    pDataUser->sPowerAfterOpen.dNonePowerQuad1,
                    pDataUser->sPowerAfterOpen.dNonePowerQuad2,
                    pDataUser->sPowerAfterOpen.dNonePowerQuad3,
                    pDataUser->sPowerAfterOpen.dNonePowerQuad4);
        }
        #endif
    }

    return E_D07_OK;
}

// 同开表盖
eD07Err trans_d07_data_recd_open_btn_box(E_D07_TRANS_FLG flag, char *user, char *frame)    
{
    return trans_d07_data_recd_open_meter_cap(flag, user, frame);
}


/*****************************************************************************
 函 数 名  : trans_d07_data_err
 功能描述  : 错误状态字转换函数
 输入参数  : E_D07_TRANS_FLG flag  
             char *user            
             char *frame           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月5日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eD07Err trans_d07_data_err(E_D07_TRANS_FLG flag, char *user, char *frame)
{

    UINT8 *pucErr;
    UINT8 ucErr = 0;
    S_D07_ERR *psErr = NULL;

    if(!user || !frame)
    {
        return E_D07_ERRO_NULL;
    }

    psErr = (S_D07_ERR*)user;
    pucErr = (UINT8 *)frame;
        
    if(flag == E_D07_TRANS_U2F)
    {
    
        (psErr->bRateErr  == TRUE) ? (ucErr |= 0x40) : (ucErr &= 0xBF);
        (psErr->bDayErr   == TRUE) ? (ucErr |= 0x20) : (ucErr &= 0xDF);
        (psErr->bYearErr  == TRUE) ? (ucErr |= 0x10) : (ucErr &= 0xEF);
        (psErr->bCommErr  == TRUE) ? (ucErr |= 0x08) : (ucErr &= 0xF7);
        (psErr->bPwdErr   == TRUE) ? (ucErr |= 0x04) : (ucErr &= 0xFB);
        (psErr->bNoneReq  == TRUE) ? (ucErr |= 0x02) : (ucErr &= 0xFD);
        (psErr->bOtherErr == TRUE) ? (ucErr |= 0x01) : (ucErr &= 0xFE);

        *pucErr = ucErr;

        // 显示信息
    }
    else
    {
        ucErr = *(UINT8 *)frame;
        psErr->bRateErr  = (ucErr & 0x40) ? TRUE : FALSE;
        psErr->bDayErr   = (ucErr & 0x20) ? TRUE : FALSE;
        psErr->bYearErr  = (ucErr & 0x10) ? TRUE : FALSE;
        psErr->bCommErr  = (ucErr & 0x08) ? TRUE : FALSE;
        psErr->bPwdErr   = (ucErr & 0x04) ? TRUE : FALSE;
        psErr->bNoneReq  = (ucErr & 0x02) ? TRUE : FALSE;
        psErr->bOtherErr = (ucErr & 0x01) ? TRUE : FALSE;

        //显示信息
        #ifdef D07_DEBUG_ON
        sprintf(g_out_data_07, 
        "费率数超: \t%s\n"
        "日时段数超: \t%s\n"
        "年时区数超: \t%s\n"
        "通讯速率不能更改: \t%s\n"
        "密码错/未授权: \t%s\n"
        "无请求数据: \t%s\n"
        "其他错误: \t%s\n", 
        psErr->bRateErr  == TRUE ? "有" : "无",
        psErr->bDayErr   == TRUE ? "有" : "无",
        psErr->bYearErr  == TRUE ? "有" : "无",
        psErr->bCommErr  == TRUE ? "有" : "无",
        psErr->bPwdErr   == TRUE ? "有" : "无",
        psErr->bNoneReq  == TRUE ? "有" : "无",
        psErr->bOtherErr == TRUE ? "有" : "无");
        #endif
    }
    
    return E_D07_OK;
}



/*****************************************************************************
 函 数 名  : get_d07_first_valid_frame
 功能描述  : DLT645 2007 找到第一个有效帧的位置及长度
 输入参数  : const UINT8 *pBuf    
             UINT16 usLenBuf      
             UINT8  **ppFirstPos  
             UINT16 *pusLenFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月25日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
int get_d07_first_valid_frame(const UINT8 *pBuf,         // 要解析的buf
                              UINT16 usLenBuf,           // 要解析的buf字节数
                              UINT8  **ppFirstPos,       // 输出帧buffer
                              UINT16 *pusLenFrame)      // 输出Frame字节数
{

    int i = 0, j = 0;
    UINT16 usLenLeft = 0; // 循环检测之的长度
    UINT8 ucDataLen = 0;
    UINT8 ucCheckSum = 0;
    int nLen = 0;
    INT32 nCheckSumPosStart, nCheckSumPos, nEndPos;
    
    if(!pBuf || !ppFirstPos || !pusLenFrame)
    {
        return E_D07_ERRO_NULL;
    }
    
    for(i = 0; i < usLenBuf; i++)
    {
        usLenLeft = usLenBuf - i;

        if(usLenLeft < D07_FRAME_LEN_MIN)
        {
            return E_D07_ERRO_FRAME_UNCOMP;
        }

        if(pBuf[i] == 0x68)
        {
            
          if (0x68 == pBuf[i+7])
          {
           
             nCheckSumPosStart = i;
             ucDataLen = pBuf[i+9];
             nLen = usLenLeft - 9 - ucDataLen;

             if(nLen < 0)
             {
                return E_D07_ERRO_FRAME_UNCOMP;
             }

             nCheckSumPos = nCheckSumPosStart + ucDataLen + 10;
             nEndPos = nCheckSumPos+1;

             // 计算校验和
             //查检checksum
            for(j = nCheckSumPosStart; j < nCheckSumPos; j++)
            {
                ucCheckSum +=  pBuf[j];
            }

            // 检查和出错
            if(ucCheckSum != pBuf[nCheckSumPos])
            {
                //return E_D07_ERRO_FRAME_CHECK_SUM;
                continue;
            }

            //结束符
            if(0x16 != pBuf[nEndPos])
            {
                //return E_D07_ERRO_FRAME_END_0x16;
                continue;
            }
            
            *ppFirstPos = (UINT8*)(pBuf + nCheckSumPosStart);
            *pusLenFrame = (UINT16)(ucDataLen + 12);
            return E_D07_OK;

          }
          
        }
    }
    
    return E_D07_ERRO_NONE_FRAME;
}

