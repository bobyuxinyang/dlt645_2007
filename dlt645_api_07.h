/*******************************************************************************
    File name:         dlt645_api_07.h
    Author:         李明
    Version:          1.0
    Date:            2012年10月22日
    Description:     dlt645 2007版规约解析头文件，用于智能采集终端与集中器、集中
                    器与智能电表通信的规约处理                 
    Others:       
    Function List: 
    1. History: 
                    
          1)  Date:
          2)  Author:
          3)  Modification:  Created
    2. ...
*******************************************************************************/
#ifndef _DLT645_API_2007_H_
#define _DLT645_API_2007_H_


#pragma pack(push)  // 采用一字节对齐
#pragma pack(1)

/* 调试开关 */
#define D07_DEBUG_ON
#undef D07_DEBUG_ON   // 正式版注释掉


#ifdef D07_DEBUG_ON

#define OUT_BUF_LEN 1024
#define D07_ON  0
#define D07_OFF 1

extern int g_need_pack_07;
extern char g_out_data_07[OUT_BUF_LEN];

#endif



#ifndef _TYPE_DEF_LM_
#define _TYPE_DEF_LM_
#ifndef __RTL_H__
#define __RTL_H__
typedef char    BOOL;
#endif
#define TRUE    (1)
#define FALSE   (0)
typedef unsigned char UCHAR;
typedef char    CHAR;
typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef _TYPE_DEF_
#define _TYPE_DEF_
typedef char            INT8;       //c
typedef short           INT16;      //s
typedef int             INT32;      //l
typedef unsigned char   UINT8;      //uc
typedef unsigned short  UINT16;     //us
typedef unsigned int    UINT32;     //ul

#ifdef MACHINE_64_BITS
typedef long             INT64;
typedef unsigned   long  UINT64;
#else
typedef long  long           INT64;
typedef unsigned long  long  UINT64;
#endif

typedef char*           STRING;     //str
typedef char            ISBOOL;     //b
typedef unsigned int    IPADDR;     //ip
typedef double          DOUBLE;
typedef void*  FRWK_HANDLE;

#endif
#endif



/* 宏定义 **/
#define NAME_LEN              128   
#define UNKNOWN_RULER_ID     0XFFFFFFFF
#define D07_ADDR_LEN         12                      /* 地址长 */
#define D07_DATA_MAX_NR      240                       /* dlt645 2007 最大数据域字节数算规约ID */
#define D07_DATA_MAX         (D07_DATA_MAX_NR + 4)   /* dlt645 2007 最大数据域字节数 */
#define D07_FRAME_LEN_MIN    12                        /* DLT645 2007 最小帧字节数 */  

/*功能码 2007*/
#define D07_CTRL_RESV                                0x00     // 保留
#define D07_CTRL_SYNC_TIME                            0x08     // 广播校时
#define D07_CTRL_READ_DATA                            0x11    // 读数据
#define D07_CTRL_READ_AFTERDATA                        0x12    // 读后续数据
#define D07_CTRL_READ_ADDR                            0x13    // 读通信地址
#define D07_CTRL_WRITE_DATA                            0x14    // 写数据
#define D07_CTRL_WRITE_ADDR                            0x15    // 写通信地址
#define D07_CTRL_FREEZ_COMM                            0x16    // 冻结命令
#define D07_CTRL_MODIFY_BAUD                        0x17    // 修改通信速率
#define D07_CTRL_MODIFY_PASSWORD                    0x18    // 修改密码
#define D07_CTRL_CLEAR_MAXDEMAND                    0x19    // 最大需量清零
#define D07_CTRL_CLEAR_METER                        0x1A    // 电表清零
#define D07_CTRL_CLEAR_EVENT                        0x1B    // 事件清零
#define D07_CTRL_COMM                                0x1C    // 控制命令




/** 宏定义 */


/* 返回值类型 列表*/
typedef enum
{
    E_D07_OK = 0,            /* 返回OK */
    E_D07_ERRO_NULL,        /* 指针为空 */
    E_D07_ERRO_UNKOWN_ID,    // 未知规约ID
    E_D07_ERRO_PARA_NONE,   /* 不存在 */
    E_D07_ERRO_PARA_OUT_RANGE,
    
    //check frame
    E_D07_ERRO_FRAME_UNCOMP,        /* 不完整的帧数据 */
    E_D07_ERRO_FRAME_0x68,          /* 测试帧帧起始符 0x68 的位置不对 */
    E_D07_ERRO_FRAME_CHECK_SUM,     /* 测试帧检验和不对 */
    E_D07_ERRO_FRAME_END_0x16,      /* 测试帧结束符 0x16 不对 */
    E_D07_ERRO_NONE_FRAME,          // 没有找到帧

}eD07Err;









/* dlt645 2007规约类型 **/
typedef enum
{
    E_D07_RULER_TYPE_UNKNOWN  =  0,                         // 未知的规约类型

    /* 标准协议 */
    E_D07_RULER_TYPE_A1_MIN,
    
    /* [00] [00] [(00~FF)] [00] */
    E_D07_RULER_TYPE_COMB_HAVE_POWER_TOTAL = E_D07_RULER_TYPE_A1_MIN,// 组合有功总电能
    E_D07_RULER_TYPE_COMB_HAVE_POWER_RATE,            // 组合有功费率电能
    E_D07_RULER_TYPE_COMB_HAVE_POWER_BLOCK,            // 组合有电能数据块

    /* [00] [01] [(00~FF)] [00] */
    E_D07_RULER_TYPE_FORTH_HAVE_POWER_TOTAL,        // 正向有功总电能
    E_D07_RULER_TYPE_FORTH_HAVE_POWER_RATE,            // 正向有功费率电能
    E_D07_RULER_TYPE_FORTH_HAVE_POWER_BLOCK,        // 正向有功电能数据块

    /* [00] [02] [(00~FF)] [00] */
    E_D07_RULER_TYPE_BACK_HAVE_POWER_TOTAL,            // 反向有功总电能
    E_D07_RULER_TYPE_BACK_HAVE_POWER_RATE,            // 反向有功费率电能
    E_D07_RULER_TYPE_BACK_HAVE_POWER_BLOCK,            // 反向有功电能数据块

    /* [00] [03] [(00~FF)] [00] */
    E_D07_RULER_TYPE_COMB_NONE_1_POWER_TOTAL,        // 组合无功1总电能
    E_D07_RULER_TYPE_COMB_NONE_1_POWER_RATE,        // 组合无功1费率电能
    E_D07_RULER_TYPE_COMB_NONE_1_POWER_BLOCK,        // 组合无功1电能数据块

    /* [00] [04] [(00~FF)] [00] */
    E_D07_RULER_TYPE_COMB_NONE_2_POWER_TOTAL,        // 组合无功2总电能
    E_D07_RULER_TYPE_COMB_NONE_2_POWER_RATE,        // 组合无功2费率电能
    E_D07_RULER_TYPE_COMB_NONE_2_POWER_BLOCK,        // 组合无功2电能数据块

    /* [00] [05] [(00~FF)] [00] */
    E_D07_RULER_TYPE_QUAD_1_NONE_POWER_TOTAL,        // 第一象限无功总电能
    E_D07_RULER_TYPE_QUAD_1_NONE_POWER_RATE,        // 第一象限无功费率电能
    E_D07_RULER_TYPE_QUAD_1_NONE_POWER_BLOCK,        // 第一象限无功电能数据块

    /* [00] [06] [(00~FF)] [00] */
    E_D07_RULER_TYPE_QUAD_2_NONE_POWER_TOTAL,        // 第二象限无功总电能
    E_D07_RULER_TYPE_QUAD_2_NONE_POWER_RATE,        // 第二象限无功费率电能
    E_D07_RULER_TYPE_QUAD_2_NONE_POWER_BLOCK,        // 第二象限无功电能数据块

    /* [00] [07] [(00~FF)] [00] */
    E_D07_RULER_TYPE_QUAD_3_NONE_POWER_TOTAL,        // 第三象限无功总电能
    E_D07_RULER_TYPE_QUAD_3_NONE_POWER_RATE,        // 第三象限无功费率电能
    E_D07_RULER_TYPE_QUAD_3_NONE_POWER_BLOCK,        // 第三象限无功电能数据块

    /* [00] [08] [(00~FF)] [00] */
    E_D07_RULER_TYPE_QUAD_4_NONE_POWER_TOTAL,        // 第四象限无功总电能
    E_D07_RULER_TYPE_QUAD_4_NONE_POWER_RATE,        // 第四象限无功费率电能
    E_D07_RULER_TYPE_QUAD_4_NONE_POWER_BLOCK,        // 第四象限无功电能数据块

    /* [00] [09] [(00~FF)] [00] */
    E_D07_RULER_TYPE_FORTH_APPARENT_POWER_TOTAL,    // 正向视在总电能
    E_D07_RULER_TYPE_FORTH_APPARENT_POWER_RATE,        // 正向视在费率电能
    E_D07_RULER_TYPE_FORTH_APPARENT_POWER_BLOCK,    // 正向视在电能数据块

    /* [00] [0A] [(00~FF)] [00] */
    E_D07_RULER_TYPE_BACK_APPARENT_POWER_TOTAL,        // 反向视在总电能
    E_D07_RULER_TYPE_BACK_APPARENT_POWER_RATE,        // 反向视在费率电能
    E_D07_RULER_TYPE_BACK_APPARENT_POWER_BLOCK,        // 反向视在电能数据块

    /* [00] [80][00] [00] */
    E_D07_RULER_TYPE_ASSO_POWER_TOTAL,                // 关联总电能

    /* [00] [81][00] [00] */
    E_D07_RULER_TYPE_FORTH_FUND_HAVE_POWER_TOTAL,    // 正向有功基波总电能

    /* [00] [82][00] [00] */
    E_D07_RULER_TYPE_BACK_FUND_HAVE_POWER_TOTAL,    // 反向有功基波总电能

    /* [00] [83][00] [00] */
    E_D07_RULER_TYPE_FORTH_HARM_HAVE_POWER_TOTAL,    // 正向有功谐波总电能

    /* [00] [84][00] [00] */
    E_D07_RULER_TYPE_BACK_HARM_HAVE_POWER_TOTAL,    // 反向有功谐波总电能

    /* [00] [85][00] [00] */
    E_D07_RULER_TYPE_COPR_LOSS_HAVE_POWER_COMP_TOTAL,       // 铜损有功总电能补偿量

    /* [00] [86][00] [00] */
    E_D07_RULER_TYPE_CORE_LOSS_HAVE_POWER_COMP_TOTAL,        // 铁损有功总电能补偿量

    /* [00] [(15~9A)][00] [00] */
    E_D07_RULER_TYPE_PHASE_A_FORTH_HAVE_POWER,                // A相正向有功电能
    E_D07_RULER_TYPE_PHASE_A_BACK_HAVE_POWER,                // A相反向有功电能
    E_D07_RULER_TYPE_PHASE_A_COMB_NONE_1_POWER,              // A相组合无功1电能
    E_D07_RULER_TYPE_PHASE_A_COMB_NONE_2_POWER,                // A相组合无功2电能
    E_D07_RULER_TYPE_PHASE_A_QUAD_1_NONE_POWER,              // A相第一象限无功电能
    E_D07_RULER_TYPE_PHASE_A_QUAD_2_NONE_POWER,                // A相第二象限无功电能
    E_D07_RULER_TYPE_PHASE_A_QUAD_3_NONE_POWER,                // A相第三象限无功电能
    E_D07_RULER_TYPE_PHASE_A_QUAD_4_NONE_POWER,                // A相第四象限无功电能
    E_D07_RULER_TYPE_PHASE_A_FORTH_APPARENT_POWER,            // A相正向视在电能
    E_D07_RULER_TYPE_PHASE_A_BACK_APPARENT_POWER,            // A相反向视在电能
    E_D07_RULER_TYPE_PHASE_A_ASSO_POWER,                    // A相关联电能
    E_D07_RULER_TYPE_PHASE_A_FORTH_FUND_HAVE_POWER,            // A相正向有功基波电能
    E_D07_RULER_TYPE_PHASE_A_BACK_FUND_HAVE_POWER,            // A相反向有功基波电能
    E_D07_RULER_TYPE_PHASE_A_FORTH_HARM_HAVE_POWER,            // A相正向有功谐波电能
    E_D07_RULER_TYPE_PHASE_A_BACK_HARM_HAVE_POWER,            // A相反向有功谐波电能
    E_D07_RULER_TYPE_PHASE_A_COPR_LOSS_HAVE_POWER_COMP,       // A相铜损有功电能补偿量
    E_D07_RULER_TYPE_PHASE_A_CORE_LOSS_HAVE_POWER_COMP,        // A相铁损有功电能补偿量


    /* [00] [(29~AE)][00] [00] */
    E_D07_RULER_TYPE_PHASE_B_FORTH_HAVE_POWER,                // B相正向有功电能
    E_D07_RULER_TYPE_PHASE_B_BACK_HAVE_POWER,                // B相反向有功电能
    E_D07_RULER_TYPE_PHASE_B_COMB_NONE_1_POWER,              // B相组合无功1电能
    E_D07_RULER_TYPE_PHASE_B_COMB_NONE_2_POWER,                // B相组合无功2电能
    E_D07_RULER_TYPE_PHASE_B_QUAD_1_NONE_POWER,              // B相第一象限无功电能
    E_D07_RULER_TYPE_PHASE_B_QUAD_2_NONE_POWER,                // B相第二象限无功电能
    E_D07_RULER_TYPE_PHASE_B_QUAD_3_NONE_POWER,                // B相第三象限无功电能
    E_D07_RULER_TYPE_PHASE_B_QUAD_4_NONE_POWER,                // B相第四象限无功电能
    E_D07_RULER_TYPE_PHASE_B_FORTH_APPARENT_POWER,            // B相正向视在电能
    E_D07_RULER_TYPE_PHASE_B_BACK_APPARENT_POWER,            // B相反向视在电能
    E_D07_RULER_TYPE_PHASE_B_ASSO_POWER,                    // B相关联电能
    E_D07_RULER_TYPE_PHASE_B_FORTH_FUND_HAVE_POWER,            // B相正向有功基波电能
    E_D07_RULER_TYPE_PHASE_B_BACK_FUND_HAVE_POWER,            // B相反向有功基波电能
    E_D07_RULER_TYPE_PHASE_B_FORTH_HARM_HAVE_POWER,            // B相正向有功谐波电能
    E_D07_RULER_TYPE_PHASE_B_BACK_HARM_HAVE_POWER,            // B相反向有功谐波电能
    E_D07_RULER_TYPE_PHASE_B_COPR_LOSS_HAVE_POWER_COMP,       // B相铜损有功电能补偿量
    E_D07_RULER_TYPE_PHASE_B_CORE_LOSS_HAVE_POWER_COMP,        // B相铁损有功电能补偿量


    /* [00] [(3D~C2)][00] [00] */
    E_D07_RULER_TYPE_PHASE_C_FORTH_HAVE_POWER,                // C相正向有功电能
    E_D07_RULER_TYPE_PHASE_C_BACK_HAVE_POWER,                // C相反向有功电能
    E_D07_RULER_TYPE_PHASE_C_COMB_NONE_1_POWER,              // C相组合无功1电能
    E_D07_RULER_TYPE_PHASE_C_COMB_NONE_2_POWER,                // C相组合无功2电能
    E_D07_RULER_TYPE_PHASE_C_QUAD_1_NONE_POWER,              // C相第一象限无功电能
    E_D07_RULER_TYPE_PHASE_C_QUAD_2_NONE_POWER,                // C相第二象限无功电能
    E_D07_RULER_TYPE_PHASE_C_QUAD_3_NONE_POWER,                // C相第三象限无功电能
    E_D07_RULER_TYPE_PHASE_C_QUAD_4_NONE_POWER,                // C相第四象限无功电能
    E_D07_RULER_TYPE_PHASE_C_FORTH_APPARENT_POWER,            // C相正向视在电能
    E_D07_RULER_TYPE_PHASE_C_BACK_APPARENT_POWER,            // C相反向视在电能
    E_D07_RULER_TYPE_PHASE_C_ASSO_POWER,                    // C相关联电能
    E_D07_RULER_TYPE_PHASE_C_FORTH_FUND_HAVE_POWER,            // C相正向有功基波电能
    E_D07_RULER_TYPE_PHASE_C_BACK_FUND_HAVE_POWER,            // C相反向有功基波电能
    E_D07_RULER_TYPE_PHASE_C_FORTH_HARM_HAVE_POWER,            // C相正向有功谐波电能
    E_D07_RULER_TYPE_PHASE_C_BACK_HARM_HAVE_POWER,            // C相反向有功谐波电能
    E_D07_RULER_TYPE_PHASE_C_COPR_LOSS_HAVE_POWER_COMP,       // C相铜损有功电能补偿量
    E_D07_RULER_TYPE_PHASE_C_CORE_LOSS_HAVE_POWER_COMP,        // C相铁损有功电能补偿量


    // 对应表A.1电能数据标识编码表最大标识 */
    E_D07_RULER_TYPE_A1_MAX = E_D07_RULER_TYPE_PHASE_C_CORE_LOSS_HAVE_POWER_COMP,        

    /* 对应表A.2 */
    E_D07_RULER_TYPE_A2_MIN,
    
    /* [01] [00] [(00~FF)] [00] */

    /* [01] [01] [(00~FF)] [00] */
    E_D07_RULER_TYPE_FORTH_HAVE_DEMAND_TOTAL = E_D07_RULER_TYPE_A2_MIN,        // 正向有功总最大需量及发生时间
    E_D07_RULER_TYPE_FORTH_HAVE_DEMAND_RATE,                // 正向有功费率最大需量及发生时间
    E_D07_RULER_TYPE_FORTH_HAVE_DEMAND_BLOCK,                // 正向有功最大需量及发生时间数据块

    /* [01] [02] [(00~FF)] [00] */
    E_D07_RULER_TYPE_BACK_HAVE_DEMAND_TOTAL,                // 反向有功总最大需量及发生时间
    E_D07_RULER_TYPE_BACK_HAVE_DEMAND_RATE,                    // 反向有功费率最大需量及发生时间
    E_D07_RULER_TYPE_BACK_HAVE_DEMAND_BLOCK,                // 反向有功最大需量及发生时间数据块

    /* [01] [03] [(00~FF)] [00] */
    E_D07_RULER_TYPE_COMB_NONE_1_DEMAND_TOTAL,                // 组合无功1总最大需量及发生时间
    E_D07_RULER_TYPE_COMB_NONE_1_DEMAND_RATE,                // 组合无功1费率最大需量及发生时间
    E_D07_RULER_TYPE_COMB_NONE_1_DEMAND_BLOCK,                // 组合无功1最大需量及发生时间数据块

    /* [01] [04] [(00~FF)] [00] */
    E_D07_RULER_TYPE_COMB_NONE_2_DEMAND_TOTAL,                // 组合无功2总最大需量及发生时间
    E_D07_RULER_TYPE_COMB_NONE_2_DEMAND_RATE,                // 组合无功2费率最大需量及发生时间
    E_D07_RULER_TYPE_COMB_NONE_2_DEMAND_BLOCK,                // 组合无功2最大需量及发生时间数据块

    /* [01] [05] [(00~FF)] [00] */
    E_D07_RULER_TYPE_QUAD_1_NONE_DEMAND_TOTAL,                // 第一象限无功总最大需量及发生时间
    E_D07_RULER_TYPE_QUAD_1_NONE_DEMAND_RATE,                // 第一象限无功费率最大需量及发生时间
    E_D07_RULER_TYPE_QUAD_1_NONE_DEMAND_BLOCK,                // 第一象限无功最大需量及发生时间数据块

    /* [01] [06] [(00~FF)] [00] */
    E_D07_RULER_TYPE_QUAD_2_NONE_DEMAND_TOTAL,                // 第二象限无功总最大需量及发生时间
    E_D07_RULER_TYPE_QUAD_2_NONE_DEMAND_RATE,                // 第二象限无功费率最大需量及发生时间
    E_D07_RULER_TYPE_QUAD_2_NONE_DEMAND_BLOCK,                // 第二象限无功最大需量及发生时间数据块

    /* [01] [07] [(00~FF)] [00] */
    E_D07_RULER_TYPE_QUAD_3_NONE_DEMAND_TOTAL,                // 第三象限无功总最大需量及发生时间
    E_D07_RULER_TYPE_QUAD_3_NONE_DEMAND_RATE,                // 第三象限无功费率最大需量及发生时间
    E_D07_RULER_TYPE_QUAD_3_NONE_DEMAND_BLOCK,                // 第三象限无功最大需量及发生时间数据块

    /* [01] [08] [(00~FF)] [00] */
    E_D07_RULER_TYPE_QUAD_4_NONE_DEMAND_TOTAL,                // 第四象限无功总最大需量及发生时间
    E_D07_RULER_TYPE_QUAD_4_NONE_DEMAND_RATE,                // 第四象限无功费率最大需量及发生时间
    E_D07_RULER_TYPE_QUAD_4_NONE_DEMAND_BLOCK,                // 第四象限无功最大需量及发生时间数据块

    /* [01] [09] [(00~FF)] [00] */
    E_D07_RULER_TYPE_FORTH_APPARENT_DEMAND_TOTAL,            // 正向视在总最大需量及发生时间
    E_D07_RULER_TYPE_FORTH_APPARENT_DEMAND_RATE,            // 正向视在费率最大需量及发生时间
    E_D07_RULER_TYPE_FORTH_APPARENT_DEMAND_BLOCK,            // 正向视在最大需量及发生时间数据块

    /* [01] [0A] [(00~FF)] [00] */
    E_D07_RULER_TYPE_BACK_APPARENT_DEMAND_TOTAL,            // 反向视在总最大需量及发生时间
    E_D07_RULER_TYPE_BACK_APPARENT_DEMAND_RATE,                // 反向视在费率最大需量及发生时间
    E_D07_RULER_TYPE_BACK_APPARENT_DEMAND_BLOCK,            // 反向视在最大需量及发生时间数据块

    /* [01] [(15~9A)][00] [00] */
    E_D07_RULER_TYPE_PHASE_A_FORTH_HAVE_DEMAND,                // A相正向有功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_BACK_HAVE_DEMAND,                // A相反向有功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_COMB_NONE_1_DEMAND,              // A相组合无功1最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_COMB_NONE_2_DEMAND,            // A相组合无功2最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_QUAD_1_NONE_DEMAND,              // A相第一象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_QUAD_2_NONE_DEMAND,            // A相第二象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_QUAD_3_NONE_DEMAND,            // A相第三象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_QUAD_4_NONE_DEMAND,            // A相第四象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_FORTH_APPARENT_DEMAND,            // A相正向视在最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_A_BACK_APPARENT_DEMAND,            // A相反向视在最大需量及发生时间


    /* [01] [(29~AE)][00] [00] */
    E_D07_RULER_TYPE_PHASE_B_FORTH_HAVE_DEMAND,                // B相正向有功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_BACK_HAVE_DEMAND,                // B相反向有功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_COMB_NONE_1_DEMAND,              // B相组合无功1最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_COMB_NONE_2_DEMAND,            // B相组合无功2最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_QUAD_1_NONE_DEMAND,              // B相第一象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_QUAD_2_NONE_DEMAND,            // B相第二象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_QUAD_3_NONE_DEMAND,            // B相第三象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_QUAD_4_NONE_DEMAND,            // B相第四象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_FORTH_APPARENT_DEMAND,            // B相正向视在最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_B_BACK_APPARENT_DEMAND,            // B相反向视在最大需量及发生时间

    /* [01] [(3D~C2)][00] [00] */
    E_D07_RULER_TYPE_PHASE_C_FORTH_HAVE_DEMAND,                // C相正向有功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_BACK_HAVE_DEMAND,                // C相反向有功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_COMB_NONE_1_DEMAND,              // C相组合无功1最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_COMB_NONE_2_DEMAND,            // C相组合无功2最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_QUAD_1_NONE_DEMAND,              // C相第一象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_QUAD_2_NONE_DEMAND,            // C相第二象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_QUAD_3_NONE_DEMAND,            // C相第三象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_QUAD_4_NONE_DEMAND,            // C相第四象限无功最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_FORTH_APPARENT_DEMAND,            // C相正向视在最大需量及发生时间
    E_D07_RULER_TYPE_PHASE_C_BACK_APPARENT_DEMAND,            // C相反向视在最大需量及发生时间
    

    /* 对应表A.2最大需量及发生时间数据标识编码表 */
    E_D07_RULER_TYPE_A2_MAX = E_D07_RULER_TYPE_PHASE_C_BACK_APPARENT_DEMAND,    

    /* 对应表A.3 变量数据标识编码表 */
    E_D07_RULER_TYPE_A3_MIN,
    E_D07_RULER_TYPE_PHASE_A_VOLT = E_D07_RULER_TYPE_A3_MIN,// A相电压
    E_D07_RULER_TYPE_PHASE_B_VOLT,                            // B相电压
    E_D07_RULER_TYPE_PHASE_C_VOLT,                            // C相电压
    E_D07_RULER_TYPE_VOLT_BLOCK,                            // 电压数据块
    
    E_D07_RULER_TYPE_PHASE_A_ELEC,                            // A相电流
    E_D07_RULER_TYPE_PHASE_B_ELEC,                            // B相电流
    E_D07_RULER_TYPE_PHASE_C_ELEC,                            // C相电流
    E_D07_RULER_TYPE_ELEC_BLOCK,                            // 电流数据块

    E_D07_RULER_TYPE_INSTANT_HAVE_POWER_RATE_TOTAL,            // 瞬时总有功功率    
    E_D07_RULER_TYPE_INSTANT_PHASE_A_HAVE_POWER_RATE,        // 瞬时A相有功功率
    E_D07_RULER_TYPE_INSTANT_PHASE_B_HAVE_POWER_RATE,        // 瞬时B相有功功率
    E_D07_RULER_TYPE_INSTANT_PHASE_C_HAVE_POWER_RATE,        // 瞬时C相有功功率
    E_D07_RULER_TYPE_INSTANT_HAVE_POWER_RATE_BLOCK,            // 瞬时有功功率数据块
    
    E_D07_RULER_TYPE_INSTANT_NONE_POWER_RATE_TOTAL,            // 瞬时总无功功率    
    E_D07_RULER_TYPE_INSTANT_PHASE_A_NONE_POWER_RATE,        // 瞬时A相无功功率
    E_D07_RULER_TYPE_INSTANT_PHASE_B_NONE_POWER_RATE,        // 瞬时B相无功功率
    E_D07_RULER_TYPE_INSTANT_PHASE_C_NONE_POWER_RATE,        // 瞬时C相无功功率
    E_D07_RULER_TYPE_INSTANT_NONE_POWER_RATE_BLOCK,            // 瞬时无功功率数据块

    E_D07_RULER_TYPE_INSTANT_APPARENT_POWER_RATE_TOTAL,        // 瞬时总视在功率    
    E_D07_RULER_TYPE_INSTANT_PHASE_A_APPARENT_POWER_RATE,    // 瞬时A相视在功率
    E_D07_RULER_TYPE_INSTANT_PHASE_B_APPARENT_POWER_RATE,    // 瞬时B相视在功率
    E_D07_RULER_TYPE_INSTANT_PHASE_C_APPARENT_POWER_RATE,    // 瞬时C相视在功率
    E_D07_RULER_TYPE_INSTANT_APPARENT_POWER_RATE_BLOCK,        // 瞬时视在功率数据块

    E_D07_RULER_TYPE_POWER_RATE_FACTOR_TOTAL,                // 总功率因数
    E_D07_RULER_TYPE_PHASE_A_POWER_RATE_FACTOR,                // A相功率因数
    E_D07_RULER_TYPE_PHASE_B_POWER_RATE_FACTOR,                // B相功率因数
    E_D07_RULER_TYPE_PHASE_C_POWER_RATE_FACTOR,                // C相功率因数
    E_D07_RULER_TYPE_POWER_RATE_FACTOR_BLOCK,                // 功率因数数据块
    
    E_D07_RULER_TYPE_PHASE_A_ANGLE,                            // A相相角
    E_D07_RULER_TYPE_PHASE_B_ANGLE,                            // B相相角
    E_D07_RULER_TYPE_PHASE_C_ANGLE,                            // C相相角
    E_D07_RULER_TYPE_ANGLE_BLOCK,                            // 相角数据块
    
    E_D07_RULER_TYPE_PHASE_A_VOLT_WAVEFORM_DISTORTION,        // A相电压波形失真度
    E_D07_RULER_TYPE_PHASE_B_VOLT_WAVEFORM_DISTORTION,        // B相电压波形失真度
    E_D07_RULER_TYPE_PHASE_C_VOLT_WAVEFORM_DISTORTION,        // C相电压波形失真度
    E_D07_RULER_TYPE_VOLT_WAVEFORM_DISTORTION_BLOCK,        // 电压波形失真度数据块
    
    E_D07_RULER_TYPE_PHASE_A_ELEC_WAVEFORM_DISTORTION,        // A相电流波形失真度
    E_D07_RULER_TYPE_PHASE_B_ELEC_WAVEFORM_DISTORTION,        // B相电流波形失真度
    E_D07_RULER_TYPE_PHASE_C_ELEC_WAVEFORM_DISTORTION,        // C相电流波形失真度
    E_D07_RULER_TYPE_ELEC_WAVEFORM_DISTORTION_BLOCK,        // 电流波形失真度数据块
    
    E_D07_RULER_TYPE_PHASE_A_VOLT_HARMONIC_CONTENT_T,        // A相电压(t)次谐波含量
    E_D07_RULER_TYPE_PHASE_A_VOLT_HARMONIC_CONTENT_BLOCK,    // A相电压次谐波含量数据块
    
    E_D07_RULER_TYPE_PHASE_B_VOLT_HARMONIC_CONTENT_T,        // B相电压(t)次谐波含量
    E_D07_RULER_TYPE_PHASE_B_VOLT_HARMONIC_CONTENT_BLOCK,   // B相电压谐波含量数据块
    
    E_D07_RULER_TYPE_PHASE_C_VOLT_HARMONIC_CONTENT_T,        // C相电压(t)次谐波含量
    E_D07_RULER_TYPE_PHASE_C_VOLT_HARMONIC_CONTENT_BLOCK,   // C相电压谐波含量数据块

    E_D07_RULER_TYPE_PHASE_A_ELEC_HARMONIC_CONTENT_T,        // A相电流(t)次谐波含量
    E_D07_RULER_TYPE_PHASE_A_ELEC_HARMONIC_CONTENT_BLOCK,    // A相电流谐波含量数据块
    
    E_D07_RULER_TYPE_PHASE_B_ELEC_HARMONIC_CONTENT_T,        // C相电流(t)次谐波含量
    E_D07_RULER_TYPE_PHASE_B_ELEC_HARMONIC_CONTENT_BLOCK,    // C相电流谐波含量数据块
    
    E_D07_RULER_TYPE_PHASE_C_ELEC_HARMONIC_CONTENT_T,        // C相电流(t)次谐波含量
    E_D07_RULER_TYPE_PHASE_C_ELEC_HARMONIC_CONTENT_BLOCK,    // C相电流谐波含量数据块
    
    E_D07_RULER_TYPE_ZERO_LINE_ELEC,                        // 零线电流
    E_D07_RULER_TYPE_POWER_GRID_HZ,                            // 电网频率
    E_D07_RULER_TYPE_1M_AVER_HAVE_POWER_RATE_TOTAL,            // 一分钟有功总平均功率
    
    E_D07_RULER_TYPE_CURRENT_HAVE_POWER_DEMAND,                // 当前有功需量
    E_D07_RULER_TYPE_CURRENT_NONE_POWER_DEMAND,                // 当前无功需量
    E_D07_RULER_TYPE_CURRENT_APPARENT_POWER_DEMAND,            // 当前视在需量
    
    E_D07_RULER_TYPE_METER_TEMP,                            // 表内温度
    E_D07_RULER_TYPE_INTER_CLOCK_CELL_VOLT,                    // 时钟电池电压(内部)
    E_D07_RULER_TYPE_READ_METER_CELL_VOLT,                    // 停电抄表电池电压(外部)
    E_D07_RULER_TYPE_INTER_CLOCK_WORK_TIME,                    // 内部电池工作时间
    
    E_D07_RULER_TYPE_A3_MAX = E_D07_RULER_TYPE_INTER_CLOCK_WORK_TIME,
    
    /* 对应表 A.4 事件记录数据标识编码表 */
    E_D07_RULER_TYPE_A4_MIN,                                /* 表A.4规约最小值 */
    E_D07_RULER_TYPE_PHASE_ABC_LOSS_VOLT_TIMES = E_D07_RULER_TYPE_A4_MIN,// ABC相失压次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_LOSS_VOLT_RECORD,                // 上(n)次A相失压记录
    E_D07_RULER_TYPE_PHASE_B_LOSS_VOLT_RECORD,                // 上(n)次B相失压记录
    E_D07_RULER_TYPE_PHASE_C_LOSS_VOLT_RECORD,                // 上(n)次C相失压记录
        
    E_D07_RULER_TYPE_PHASE_ABC_LESS_VOLT_TIMES,                // ABC相欠压次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_LESS_VOLT_RECORD,                // 上(n)次A相欠压记录
    E_D07_RULER_TYPE_PHASE_B_LESS_VOLT_RECORD,                // 上(n)次B相欠压记录
    E_D07_RULER_TYPE_PHASE_C_LESS_VOLT_RECORD,                // 上(n)次C相欠压记录
    
    E_D07_RULER_TYPE_PHASE_ABC_MORE_VOLT_TIMES,                // ABC相过压次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_MORE_VOLT_RECORD,                // 上(n)次A相过压记录
    E_D07_RULER_TYPE_PHASE_B_MORE_VOLT_RECORD,                // 上(n)次B相过压记录
    E_D07_RULER_TYPE_PHASE_C_MORE_VOLT_RECORD,                // 上(n)次C相过压记录
    
    E_D07_RULER_TYPE_PHASE_ABC_DROP_TIMES,                    // ABC相断相次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_DROP_RECORD,                    // 上(n)次A相断相记录
    E_D07_RULER_TYPE_PHASE_B_DROP_RECORD,                    // 上(n)次B相断相记录
    E_D07_RULER_TYPE_PHASE_C_DROP_RECORD,                    // 上(n)次C相断相记录
    
    E_D07_RULER_TYPE_LOSS_VOLT_TIMES_TOTAL,                    // 全失压次数，总累计时间
    E_D07_RULER_TYPE_LOSS_ALL_VOLT_TIME_N,                    // 上(n)次全失压发生时刻，电流值，结束时刻
    E_D07_RULER_TYPE_APS_LOSS_ELEC_TIMES_TOTAL,                // 辅助电源失电发生次数，总累计时间
    E_D07_RULER_TYPE_APS_LOSS_ELEC_TIME_N,                    // 上(n)次辅助电源失电发生时刻，结束时刻
    
    E_D07_RULER_TYPE_VOLT_ANTI_PHASE_TIMES_TOTAL,            // 电压逆相序总次数，总累计时间
    E_D07_RULER_TYPE_VOLT_ANTI_PHASE_RECORD_N,                // 上(n)次电压逆相序记录

    E_D07_RULER_TYPE_ELEC_ANTI_PHASE_TIMES_TOTAL,            // 电流逆相序总次数，总累计时间
    E_D07_RULER_TYPE_ELEC_ANTI_PHASE_RECORD_N,                // 上(n)次电流逆相序记录
    
    E_D07_RULER_TYPE_VOLT_UNBALANCE_TIMES_TOTAL,            // 电压不平衡总次数，总累计时间
    E_D07_RULER_TYPE_VOLT_UNBALANCE_RECORD_N,                // 上(n)次电压不平衡记录
    
    E_D07_RULER_TYPE_ELEC_UNBALANCE_TIMES_TOTAL,            // 电流不平衡总次数，总累计时间
    E_D07_RULER_TYPE_ELEC_UNBALANCE_RECORD_N,                // 上(n)次电流不平衡记录
    
    E_D07_RULER_TYPE_PHASE_ABC_LOSS_ELEC_TIMES,                // ABC相失流次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_LOSS_ELEC_RECORD,                // 上(n)次A相失流记录
    E_D07_RULER_TYPE_PHASE_B_LOSS_ELEC_RECORD,                // 上(n)次B相失流记录
    E_D07_RULER_TYPE_PHASE_C_LOSS_ELEC_RECORD,                // 上(n)次C相失流记录
    
    E_D07_RULER_TYPE_PHASE_ABC_MORE_ELEC_TIMES,                // ABC相过流次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_MORE_ELEC_RECORD,                // 上(n)次A相过流记录
    E_D07_RULER_TYPE_PHASE_B_MORE_ELEC_RECORD,                // 上(n)次B相过流记录
    E_D07_RULER_TYPE_PHASE_C_MORE_ELEC_RECORD,                // 上(n)次C相过流记录
    
    E_D07_RULER_TYPE_PHASE_ABC_DROP_ELEC_TIMES,                // ABC相断流次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_DROP_ELEC_RECORD,                // 上(n)次A相断流记录
    E_D07_RULER_TYPE_PHASE_B_DROP_ELEC_RECORD,                // 上(n)次B相断流记录
    E_D07_RULER_TYPE_PHASE_C_DROP_ELEC_RECORD,                // 上(n)次C相断流记录
    
    E_D07_RULER_TYPE_PHASE_ABC_BACKWARD_ELEC_TIMES,            // ABC相潮流反向次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_BACKWARD_ELEC_RECORD,            // 上(n)次A相潮流反向记录
    E_D07_RULER_TYPE_PHASE_B_BACKWARD_ELEC_RECORD,            // 上(n)次B相潮流反向记录
    E_D07_RULER_TYPE_PHASE_C_BACKWARD_ELEC_RECORD,            // 上(n)次C相潮流反向记录
    
    E_D07_RULER_TYPE_PHASE_ABC_OVERLOAD_TIMES,                // ABC相过载次数，总累计时间
    E_D07_RULER_TYPE_PHASE_A_OVERLOAD_RECORD,                // 上(n)次A相过载记录
    E_D07_RULER_TYPE_PHASE_B_OVERLOAD_RECORD,                // 上(n)次B相过载记录
    E_D07_RULER_TYPE_PHASE_C_OVERLOAD_RECORD,                // 上(n)次C相过载记录
    
    E_D07_RULER_TYPE_VOLT_PASS_PERCENT_M,                    // 上(n)月电压合格率统计数据
    E_D07_RULER_TYPE_PHASE_A_VOLT_PASS_PERCENT_M,            // 上(n)月A相电压合格率统计数据
    E_D07_RULER_TYPE_PHASE_B_VOLT_PASS_PERCENT_M,            // 上(n)月B相电压合格率统计数据
    E_D07_RULER_TYPE_PHASE_C_VOLT_PASS_PERCENT_M,            // 上(n)月C相电压合格率统计数据
    
    E_D07_RULER_TYPE_POWER_DOWN_TIMES,                        // 掉电总次数
    E_D07_RULER_TYPE_POWER_DOWN_TIME_T,                        // 上(t)次掉电发生时刻，结束时刻
    
    E_D07_RULER_TYPE_OVER_LIMIT_MEMAND_TIMES,                // 需量超限总次数记录
    E_D07_RULER_TYPE_FORTH_OVER_LIMIT_MEMAND_TIME_T,        // 上(t)次正向有功需量超限记录    
    E_D07_RULER_TYPE_BACK_OVER_LIMIT_MEMAND_TIME_T,            // 上(t)次反向有功需量超限记录
    
    E_D07_RULER_TYPE_QUAD_1_OVER_LIMIT_MEMAND_TIME_T,        // 上(t)次第1象限无功需量超限记录    
    E_D07_RULER_TYPE_QUAD_2_OVER_LIMIT_MEMAND_TIME_T,        // 上(t)次第2象限无功需量超限记录
    E_D07_RULER_TYPE_QUAD_3_OVER_LIMIT_MEMAND_TIME_T,        // 上(t)次第3象限无功需量超限记录
    E_D07_RULER_TYPE_QUAD_4_OVER_LIMIT_MEMAND_TIME_T,        // 上(t)次第4象限无功需量超限记录
    
    E_D07_RULER_TYPE_METER_PROG_TIMES_TOTAL,                // 编程总次数    
    E_D07_RULER_TYPE_METER_PROG_RECORD_T,                    // 上(t)次编程记录
    
    E_D07_RULER_TYPE_METER_CLEAR_TIMES_TOTAL,                // 电表清零总次数    
    E_D07_RULER_TYPE_METER_CLEAR_RECORD_T,                    // 上(t)次电表清零记录
    
    E_D07_RULER_TYPE_DEMAND_CLEAR_TIMES_TOTAL,                // 需量清零总次数    
    E_D07_RULER_TYPE_DEMAND_CLEAR_RECORD_T,                    // 上(t)次需量清零记录
    
    E_D07_RULER_TYPE_EVENT_CLEAR_TIMES_TOTAL,                // 事件清零总次数    
    E_D07_RULER_TYPE_EVENT_CLEAR_RECORD_T,                    // 上(t)次事件清零记录
    
    E_D07_RULER_TYPE_ADJUST_TIME_TIMES_TOTAL,                // 校时总次数    
    E_D07_RULER_TYPE_ADJUST_TIME_RECORD_T,                    // 上(t)次校时记录
    
    E_D07_RULER_TYPE_TIME_PART_PROG_TIMES_TOTAL,            // 时段表编程总次数    
    E_D07_RULER_TYPE_TIME_PART_PROG_RECORD_T,                // 上(t)次时段表编程记录
    
    E_D07_RULER_TYPE_TIME_ZONE_PROG_TIMES_TOTAL,            // 时区表编程总次数    
    E_D07_RULER_TYPE_TIME_ZONE_PROG_RECORD_T,                // 上(t)次时区表编程记录
    
    E_D07_RULER_TYPE_WEEKDAY_PROG_TIMES_TOTAL,                // 周休日编程总次数    
    E_D07_RULER_TYPE_WEEKDAY_PROG_RECORD_T,                    // 上(t)次周休日编程记录
    
    E_D07_RULER_TYPE_HOLIDAY_PROG_TIMES_TOTAL,                // 节假日编程总次数    
    E_D07_RULER_TYPE_HOLIDAY_PROG_RECORD_T,                    // 上(t)次节假日编程记录
    
    E_D07_RULER_TYPE_HAVE_COMB_PROG_TIMES_TOTAL,            // 有功组合方式编程总次数    
    E_D07_RULER_TYPE_HAVE_COMB_PROG_RECORD_T,                // 上(t)次有功组合方式编程记录
    
    E_D07_RULER_TYPE_NONE_COMB_1_PROG_TIMES_TOTAL,            // 无功组合方式1编程总次数    
    E_D07_RULER_TYPE_NONE_COMB_1_PROG_RECORD_T,                // 上(t)次无功组合方式1编程记录
    
    E_D07_RULER_TYPE_NONE_COMB_2_PROG_TIMES_TOTAL,            // 无功组合方式2编程总次数    
    E_D07_RULER_TYPE_NONE_COMB_2_PROG_RECORD_T,                // 上(t)次无功组合方式2编程记录
    
    E_D07_RULER_TYPE_PAYOFF_DAY_PROG_TIMES_TOTAL,            // 结算日编程总次数    
    E_D07_RULER_TYPE_PAYOFF_DAY_PROG_RECORD_T,                // 上(t)次结算日编程记录
    
    E_D07_RULER_TYPE_OPEN_METER_CAP_TIMES_TOTAL,            // 开表盖总次数    
    E_D07_RULER_TYPE_OPEN_METER_CAP_RECORD_T,                // 上(t)次开表盖记录
    
    E_D07_RULER_TYPE_OPEN_BTN_BOX_TIMES_TOTAL,                // 开端钮盒总次数    
    E_D07_RULER_TYPE_OPEN_BTN_BOX_RECORD_T,                    // 上(t)次开端钮盒记录
    
    E_D07_RULER_TYPE_A4_MAX = E_D07_RULER_TYPE_OPEN_BTN_BOX_RECORD_T, /* 表 A.4 规约最大值 */
    
    /* 对应表 A.5 参变量数据标识编码表 */
    E_D07_RULER_TYPE_A5_MIN,                                /* 表 A.5 规约最小值 */
    E_D07_RULER_TYPE_DATE = E_D07_RULER_TYPE_A5_MIN,        // 日期及星期（其中0代表星期天）
    E_D07_RULER_TYPE_TIME,                                    // 时间
    E_D07_RULER_TYPE_DEMAND_PERIOD,                            // 最大需量周期
    E_D07_RULER_TYPE_SLIT_TIME,                                // 滑差时间
    E_D07_RULER_TYPE_ADJUST_PULSE_WEDTH,                    // 校表脉冲宽度
    E_D07_RULER_TYPE_SWITCH_ZONE_TIME,                        // 两套时区表切换时间
    E_D07_RULER_TYPE_SWITCH_PART_TIME,                        // 两套日时段表切换时间
    E_D07_RULER_TYPE_YEAR_ZONE_P,                            // 年时区数 p <= 14
    E_D07_RULER_TYPE_DAY_PART_Q,                            // 日时段表数 q <= 8
    E_D07_RULER_TYPE_DAY_PART_M,                            // 日时段数(每日切换数)m <=14
    E_D07_RULER_TYPE_RATE_K,                                // 费率数 k <= 63
    E_D07_RULER_TYPE_HOLIDAY_N,                                // 公共假日数n <= 254
    E_D07_RULER_TYPE_HARMONIC_ANALY_TIMES,                    // 谐波分析次数
    E_D07_RULER_TYPE_AUTO_DISPLAY_SCREENS,                    // 自动循环显示屏数
    E_D07_RULER_TYPE_DISPLAY_TIME,                            // 每屏显示时间
    E_D07_RULER_TYPE_DISPLAY_POWER_DECIMAL_DIGITS,            // 显示电能小数位数
    E_D07_RULER_TYPE_DISPLAY_DEMAND_DECIMAL_DIGITS,            // 显示最大需量小数位数
    E_D07_RULER_TYPE_PRESS_DISPLAY_SCREENS,                    // 按键循环显示屏数
    E_D07_RULER_TYPE_ADDRESS,                                // 通信地址
    E_D07_RULER_TYPE_METER_ID,                                // 表号
    E_D07_RULER_TYPE_ASSETS_CODE,                            // 资产管理编码
    E_D07_RULER_TYPE_RATED_VOLT,                            // 额定电压
    E_D07_RULER_TYPE_RATED_ELEC,                            // 额定电流
    E_D07_RULER_TYPE_MAX_ELEC,                                // 最大电流

    E_D07_RULER_TYPE_HAVE_ACCURACY_CLASS,                    // 有功准确度等级
    E_D07_RULER_TYPE_NONE_ACCURACY_CLASS,                    // 无功准确度等级
        
    E_D07_RULER_TYPE_METER_HAVE_CONSTANT,                    // 电表有功常数
    E_D07_RULER_TYPE_METER_NONE_CONSTANT,                    // 电表无功常数
    
    E_D07_RULER_TYPE_METER_MODEL_NUM,                        // 电表型号
    E_D07_RULER_TYPE_METER_PROD_DATE,                        // 生产日期
    E_D07_RULER_TYPE_METER_RULER_VERSION,                    // 协议版本号
    
    E_D07_RULER_TYPE_METER_RUN_STWD_1,                        // 电表运行状态字 1
    E_D07_RULER_TYPE_METER_RUN_STWD_2,                        // 电表运行状态字 2
    E_D07_RULER_TYPE_METER_RUN_STWD_3,                        // 电表运行状态字 3
    E_D07_RULER_TYPE_METER_RUN_STWD_4,                        // 电表运行状态字 4
    E_D07_RULER_TYPE_METER_RUN_STWD_5,                        // 电表运行状态字 5
    E_D07_RULER_TYPE_METER_RUN_STWD_6,                        // 电表运行状态字 6
    E_D07_RULER_TYPE_METER_RUN_STWD_7,                        // 电表运行状态字 7
    E_D07_RULER_TYPE_METER_RUN_STWD_BLOCK,                    // 电表运行状态字数据块
    E_D07_RULER_TYPE_HAVE_COMB_STWD,                        // 有功组合方式特征字
    E_D07_RULER_TYPE_NONE_COMB_1_STWD,                        // 无功组合方式1特征字
    E_D07_RULER_TYPE_NONE_COMB_2_STWD,                        // 无功组合方式2特征字
    
    E_D07_RULER_TYPE_MODEM_IRDA_RATE_STWD,                    // 调制型红外光口通信速率特征字
    E_D07_RULER_TYPE_TOUCH_IRDA_RATE_STWD,                    // 接触式红外光口通信速率特征字
    E_D07_RULER_TYPE_PART_1_RATE_STWD,                        // 通信口 1 通信速率特征字    
    E_D07_RULER_TYPE_PART_2_RATE_STWD,                        // 通信口 2 通信速率特征字    
    E_D07_RULER_TYPE_PART_3_RATE_STWD,                        // 通信口 3 通信速率特征字    
    
    E_D07_RULER_TYPE_WEEKDAY_STWD,                            // 周休日特征字
    E_D07_RULER_TYPE_WEEKDAY_TBL,                            // 周休日采用的日时段表号
    E_D07_RULER_TYPE_OVERLOAD_MODEL_STWD,                    // 负荷记录模式字
    E_D07_RULER_TYPE_FREEZE_MODEL_STWD,                        // 冻结数据模式字
    E_D07_RULER_TYPE_OVERLOAD_START_TIME,                    // 负荷记录起始时间
    E_D07_RULER_TYPE_OVERLOAD_INTRV_TIME_1,                    // 第 1 类负荷记录间隔时间
    E_D07_RULER_TYPE_OVERLOAD_INTRV_TIME_2,                    // 第 2 类负荷记录间隔时间
    E_D07_RULER_TYPE_OVERLOAD_INTRV_TIME_3,                    // 第 3 类负荷记录间隔时间
    E_D07_RULER_TYPE_OVERLOAD_INTRV_TIME_4,                    // 第 4 类负荷记录间隔时间
    E_D07_RULER_TYPE_OVERLOAD_INTRV_TIME_5,                    // 第 5 类负荷记录间隔时间
    E_D07_RULER_TYPE_OVERLOAD_INTRV_TIME_6,                    // 第 6 类负荷记录间隔时间
    E_D07_RULER_TYPE_PAYOFF_DAY_1,                            // 每月第 1 结算日
    E_D07_RULER_TYPE_PAYOFF_DAY_2,                            // 每月第 2 结算日
    E_D07_RULER_TYPE_PAYOFF_DAY_3,                            // 每月第 3 结算日
    
    E_D07_RULER_TYPE_PASSWORD_LV0,                            // 0 级密码
    E_D07_RULER_TYPE_PASSWORD_LV1,                            // 1 级密码
    E_D07_RULER_TYPE_PASSWORD_LV2,                            // 2 级密码
    E_D07_RULER_TYPE_PASSWORD_LV3,                            // 3 级密码
    E_D07_RULER_TYPE_PASSWORD_LV4,                            // 4 级密码
    E_D07_RULER_TYPE_PASSWORD_LV5,                            // 5 级密码
    E_D07_RULER_TYPE_PASSWORD_LV6,                            // 6 级密码
    E_D07_RULER_TYPE_PASSWORD_LV7,                            // 7 级密码
    E_D07_RULER_TYPE_PASSWORD_LV8,                            // 8 级密码
    E_D07_RULER_TYPE_PASSWORD_LV9,                            // 9 级密码
    
    E_D07_RULER_TYPE_PHASE_A_CONDUCT_FACTOR,                // A相电导系数
    E_D07_RULER_TYPE_PHASE_A_SUSCEPT_FACTOR,                // A相电纳系数
    E_D07_RULER_TYPE_PHASE_A_RESIST_FACTOR,                    // A相电阻系数
    E_D07_RULER_TYPE_PHASE_A_REACTANCE_FACTOR,                // A相电抗系数
    
    E_D07_RULER_TYPE_PHASE_B_CONDUCT_FACTOR,                // B相电导系数
    E_D07_RULER_TYPE_PHASE_B_SUSCEPT_FACTOR,                // B相电纳系数
    E_D07_RULER_TYPE_PHASE_B_RESIST_FACTOR,                    // B相电阻系数
    E_D07_RULER_TYPE_PHASE_B_REACTANCE_FACTOR,                // B相电抗系数
    
    E_D07_RULER_TYPE_PHASE_C_CONDUCT_FACTOR,                // C相电导系数
    E_D07_RULER_TYPE_PHASE_C_SUSCEPT_FACTOR,                // C相电纳系数
    E_D07_RULER_TYPE_PHASE_C_RESIST_FACTOR,                    // C相电阻系数
    E_D07_RULER_TYPE_PHASE_C_REACTANCE_FACTOR,                // C相电抗系数
    
    E_D07_RULER_TYPE_FORTH_HAVE_POWER_MAX,                    // 正向有功功率上限值
    E_D07_RULER_TYPE_BACK_HAVE_POWER_MAX,                    // 反向有功功率上限值
    
    E_D07_RULER_TYPE_VOLT_MAX,                                // 电压上限值
    E_D07_RULER_TYPE_VOLT_MIN,                                // 电压下限值
    
    E_D07_RULER_TYPE_DAY_PART_DATA_1,                        // 第一套时区表数据    
    E_D07_RULER_TYPE_DAY_PART_DATA_1_T,                        // 第一套第(t)日时段表数据 t(1~8)
    E_D07_RULER_TYPE_DAY_PART_DATA_2,                        // 第二套时区表数据    
    E_D07_RULER_TYPE_DAY_PART_DATA_2_T,                        // 第二套第(t)日时段表数据 t(1~8)
    
    E_D07_RULER_TYPE_HOLIDAY_DATE_T,                        // 第 (t)公共假日日期及日时段表号 t(1~254)

    E_D07_RULER_TYPE_AUTO_DISPLAY_SCREEN_T,                    // 自动循环显示第(t)屏显示数据项 t(1~254)
    E_D07_RULER_TYPE_PREES_DISPLAY_SCREEN_T,                // 按键循环显示第(t)屏显示数据项 t(1~254)
    E_D07_RULER_TYPE_FACTORY_SOFTWARE_VERSION,                // 厂家软件版本号
    E_D07_RULER_TYPE_FACTORY_HARDWARE_VERSION,                // 厂家硬件版本号
    E_D07_RULER_TYPE_FACTORY_SERIAL_NUM,                    // 厂家编号
    E_D07_RULER_TYPE_A5_MAX = E_D07_RULER_TYPE_FACTORY_SERIAL_NUM,/* 表 A.5 规约最大值 */
    
    /* 对应表 A.6 冻结数据标识编码表 */
    /* t(1~12)*/
    E_D07_RULER_TYPE_A6_MIN,                                /* 表 A.6 规约最小值 */    
    E_D07_RULER_TYPE_FIX_FREZ_TIME_T = E_D07_RULER_TYPE_A6_MIN,    // 上(t)次定时冻结时间
    E_D07_RULER_TYPE_FIX_FREZ_FORTH_HAVE_POWER_T,            // 上(t)次定时冻结正向有功电能数据
    E_D07_RULER_TYPE_FIX_FREZ_BACK_HAVE_POWER_T,            // 上(t)次定时冻结反向有功电能数据
    E_D07_RULER_TYPE_FIX_FREZ_COMB_1_NONE_POWER_T,            // 上(t)次定时冻结组合无功1电能数据
    E_D07_RULER_TYPE_FIX_FREZ_COMB_2_NONE_POWER_T,            // 上(t)次定时冻结组合无功2电能数据
    E_D07_RULER_TYPE_FIX_FREZ_PHASE_1_NONE_POWER_T,            // 上(t)次定时冻结第一象限无功电能数据
    E_D07_RULER_TYPE_FIX_FREZ_PHASE_2_NONE_POWER_T,            // 上(t)次定时冻结第二象限无功电能数据
    E_D07_RULER_TYPE_FIX_FREZ_PHASE_3_NONE_POWER_T,            // 上(t)次定时冻结第三象限无功电能数据
    E_D07_RULER_TYPE_FIX_FREZ_PHASE_4_NONE_POWER_T,            // 上(t)次定时冻结第四象限无功电能数据
    E_D07_RULER_TYPE_FIX_FREZ_FORTH_HAVE_DEMAND_T,            // 上(t)次定时冻结正向有功最大需量数据及发生时间数据
    E_D07_RULER_TYPE_FIX_FREZ_BACK_HAVE_DEMAND_T,            // 上(t)次定时冻结反向有功最大需量数据及发生时间数据
    E_D07_RULER_TYPE_FIX_FREZ_VAR_T,                        // 上(t)次定时冻结变量数据
    E_D07_RULER_TYPE_FIX_FREZ_BLOCK_T,                        // 上(t)次定时冻结数据块
    
    /* t (1~3)*/
    E_D07_RULER_TYPE_INST_FREZ_TIME_T,                        // 上(t)次瞬时冻结时间
    E_D07_RULER_TYPE_INST_FREZ_FORTH_HAVE_POWER_T,            // 上(t)次瞬时冻结正向有功电能数据
    E_D07_RULER_TYPE_INST_FREZ_BACK_HAVE_POWER_T,            // 上(t)次瞬时冻结反向有功电能数据
    E_D07_RULER_TYPE_INST_FREZ_COMB_1_NONE_POWER_T,            // 上(t)次瞬时冻结组合无功电能数据
    E_D07_RULER_TYPE_INST_FREZ_COMB_2_NONE_POWER_T,            // 上(t)次瞬时冻结组合无功电能数据
    E_D07_RULER_TYPE_INST_FREZ_PHASE_1_NONE_POWER_T,        // 上(t)次瞬时冻结第一象限无功电能数据
    E_D07_RULER_TYPE_INST_FREZ_PHASE_2_NONE_POWER_T,        // 上(t)次瞬时冻结第二象限无功电能数据
    E_D07_RULER_TYPE_INST_FREZ_PHASE_3_NONE_POWER_T,        // 上(t)次瞬时冻结第三象限无功电能数据
    E_D07_RULER_TYPE_INST_FREZ_PHASE_4_NONE_POWER_T,        // 上(t)次瞬时冻结第四象限无功电能数据
    E_D07_RULER_TYPE_INST_FREZ_FORTH_HAVE_DEMAND_T,            // 上(t)次瞬时冻结正向有功最大需量数据及发生时间数据
    E_D07_RULER_TYPE_INST_FREZ_BACK_HAVE_DEMAND_T,            // 上(t)次瞬时冻结反向有功最大需量数据及发生时间数据
    E_D07_RULER_TYPE_INST_FREZ_BLOCK_T,                        // 上(t)次瞬时冻结数据块

    /* t (1~2) */
    E_D07_RULER_TYPE_SWITCH_ZONE_TIME_T,                    // 上(t)次 两套时区表切换时间
    E_D07_RULER_TYPE_SWITCH_ZONE_FORTH_HAVE_POWER_T,        // 上(t)次 两套时区表切换正向有功电能数据
    E_D07_RULER_TYPE_SWITCH_ZONE_BACK_HAVE_POWER_T,            // 上(t)次 两套时区表切换反向有功电能数据
    E_D07_RULER_TYPE_SWITCH_ZONE_COMB_1_NONE_POWER_T,        // 上(t)次 两套时区表切换组合无功电能数据
    E_D07_RULER_TYPE_SWITCH_ZONE_COMB_2_NONE_POWER_T,        // 上(t)次 两套时区表切换组合无功电能数据
    E_D07_RULER_TYPE_SWITCH_ZONE_QUAD_1_NONE_POWER_T,        // 上(t)次 两套时区表切换第一象限无功电能数据
    E_D07_RULER_TYPE_SWITCH_ZONE_QUAD_2_NONE_POWER_T,        // 上(t)次 两套时区表切换第二象限无功电能数据
    E_D07_RULER_TYPE_SWITCH_ZONE_QUAD_3_NONE_POWER_T,        // 上(t)次 两套时区表切换第三象限无功电能数据
    E_D07_RULER_TYPE_SWITCH_ZONE_QUAD_4_NONE_POWER_T,        // 上(t)次 两套时区表切换第四象限无功电能数据
    E_D07_RULER_TYPE_SWITCH_ZONE_FORTH_HAVE_DEMAND_T,        // 上(t)次 两套时区表切换正向有功最大需量及发生时间数据
    E_D07_RULER_TYPE_SWITCH_ZONE_BACK_HAVE_DEMAND_T,           // 上(t)次 两套时区表切换反向有功最大需量及发生时间数据
    E_D07_RULER_TYPE_SWITCH_ZONE_VAR_DATA_T,                   // 上(t)次 两套时区表切换变量数据
    E_D07_RULER_TYPE_SWITCH_ZONE_BLOCK_T,                       // 上(t)次 两套时区表切换数据块
    
    /* t (1~2) */
    E_D07_RULER_TYPE_SWITCH_PART_TIME_T,                    // 上(t)次 两套日时段表切换时间
    E_D07_RULER_TYPE_SWITCH_PART_FORTH_HAVE_POWER_T,        // 上(t)次 两套日时段表切换正向有功电能数据
    E_D07_RULER_TYPE_SWITCH_PART_BACK_HAVE_POWER_T,            // 上(t)次 两套日时段表切换反向有功电能数据
    E_D07_RULER_TYPE_SWITCH_PART_COMB_1_NONE_POWER_T,        // 上(t)次 两套日时段表切换组合无功电能数据
    E_D07_RULER_TYPE_SWITCH_PART_COMB_2_NONE_POWER_T,        // 上(t)次 两套日时段表切换组合无功电能数据
    E_D07_RULER_TYPE_SWITCH_PART_QUAD_1_NONE_POWER_T,        // 上(t)次 两套日时段表切换第一象限无功电能数据
    E_D07_RULER_TYPE_SWITCH_PART_QUAD_2_NONE_POWER_T,        // 上(t)次 两套日时段表切换第二象限无功电能数据
    E_D07_RULER_TYPE_SWITCH_PART_QUAD_3_NONE_POWER_T,        // 上(t)次 两套日时段表切换第三象限无功电能数据
    E_D07_RULER_TYPE_SWITCH_PART_QUAD_4_NONE_POWER_T,        // 上(t)次 两套日时段表切换第四象限无功电能数据
    E_D07_RULER_TYPE_SWITCH_PART_FORTH_HAVE_DEMAND_T,        // 上(t)次 两套日时段表切换正向有功最大需量及发生时间数据
    E_D07_RULER_TYPE_SWITCH_PART_BACK_HAVE_DEMAND_T,           // 上(t)次 两套日时段表切换反向有功最大需量及发生时间数据
    E_D07_RULER_TYPE_SWITCH_PART_VAR_DATA_T,                   // 上(t)次 两套日时段表切换变量数据
    E_D07_RULER_TYPE_SWITCH_PART_BLOCK_T,                       // 上(t)次 两套日时段表切换数据块

    E_D07_RULER_TYPE_A6_MAX = E_D07_RULER_TYPE_SWITCH_PART_BLOCK_T,    /* 表 A.6 规约最大值 */    
    
    /* 对应表 A.7 负荷记录数据编码表 */
    E_D07_RULER_TYPE_A7_MIN,                                /* 表 A.7 规约最小值 */    
    E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD = E_D07_RULER_TYPE_A7_MIN,    // 最早记录块
    E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD,                    // 给定时间记录块
    E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD,                    // 最近一个记录块
    
    E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD_1,                // 第一类负荷 最早记录块
    E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD_1,                // 第一类负荷 给定时间记录块
    E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD_1,                // 第一类负荷 最近一个记录块

    E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD_2,                // 第二类负荷 最早记录块
    E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD_2,                // 第二类负荷 给定时间记录块
    E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD_2,                // 第二类负荷 最近一个记录块

    E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD_3,                // 第三类负荷 最早记录块
    E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD_3,                // 第三类负荷 给定时间记录块
    E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD_3,                // 第三类负荷 最近一个记录块

    E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD_4,                // 第四类负荷 最早记录块
    E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD_4,                // 第四类负荷 给定时间记录块
    E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD_4,                // 第四类负荷 最近一个记录块

    E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD_5,                // 第五类负荷 最早记录块
    E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD_5,                // 第五类负荷 给定时间记录块
    E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD_5,                // 第五类负荷 最近一个记录块

    E_D07_RULER_TYPE_EARLY_OVERLOAD_RECORD_6,                // 第六类负荷 最早记录块
    E_D07_RULER_TYPE_GIVEN_OVERLOAD_RECORD_6,                // 第六类负荷 给定时间记录块
    E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD_6,                // 第六类负荷 最近一个记录块
    E_D07_RULER_TYPE_A7_MAX = E_D07_RULER_TYPE_RECNT_OVERLOAD_RECORD_6,    /* 表 A.7 规约最大值 */
    

    /* 用户扩展 */
    





}E_D07_RULER_TYPE;
/** dlt645 2007规约类型 */




/* DLT645 2007版 规约编码表类型 */
typedef enum
{
    E_DLT64507_RULER_TABEL_POWER,          /* 对应表A.1 电能量数据标识编码表 */
    E_DLT64507_RULER_TABEL_DEMAND,      /* 对应表A.2 最大需量及发生时间数据标识编码表 */
    E_DLT64507_RULER_TABEL_VAR,            /* 对应表A.3 变量数据标识编码表 */
    E_DLT64507_RULER_TABEL_EVENT,        /* 对应表A.4 事件记录数据标识编码表 */
    E_DLT64507_RULER_TABEL_PARA,         /* 对应表A.5 参变量数据标识编码表 */
    E_DLT64507_RULER_TABEL_FREEZE,      /* 对应表A.6 冻结数据标识编码表 */
    E_DLT64507_RULER_TABEL_LOAD,        /* 对应表A.7 负荷记录数据标识编码表 */
    E_DLT64507_RULER_TABEL_EXT,            /* 对应表A.8 用户自定义数据标识编码表 */

}E_DLT645_07_RULER_TABEL_TYPE;



/* 结算日类型 */
typedef enum
{
    E_D07_PAYOFF_NULL,      // 不关心该参数
    E_D07_PAYOFF_CURRENT, /* 当前 */
    E_D07_PAYOFF_PRE_1,      /* 上 1 结算日 */
    E_D07_PAYOFF_PRE_2,      /* 上 2 结算日 */
    E_D07_PAYOFF_PRE_3,      /* 上 3 结算日 */
    E_D07_PAYOFF_PRE_4,      /* 上 4 结算日 */
    E_D07_PAYOFF_PRE_5,      /* 上 5 结算日 */
    E_D07_PAYOFF_PRE_6,      /* 上 6 结算日 */
    E_D07_PAYOFF_PRE_7,      /* 上 7 结算日 */
    E_D07_PAYOFF_PRE_8,      /* 上 8 结算日 */
    E_D07_PAYOFF_PRE_9,      /* 上 9 结算日 */
    E_D07_PAYOFF_PRE_10,  /* 上 10 结算日 */
    E_D07_PAYOFF_PRE_11,  /* 上 11 结算日 */
    E_D07_PAYOFF_PRE_12,  /* 上 12 结算日 */
     
}E_D07_PARA_PAYOFF;


/* 参数 谐波次数 */
typedef enum
{

    E_D07_HARM_NULL,        // 不关心谐波次数
    E_D07_HARM_1,            // 1次谐波
    E_D07_HARM_2,            // 2次谐波
    E_D07_HARM_3,            // 3次谐波
    E_D07_HARM_4,            // 4次谐波
    E_D07_HARM_5,            // 5次谐波
    E_D07_HARM_6,            // 6次谐波
    E_D07_HARM_7,            // 7次谐波
    E_D07_HARM_8,            // 8次谐波
    E_D07_HARM_9,            // 9次谐波
    E_D07_HARM_10,            // 10次谐波
    E_D07_HARM_11,            // 11次谐波
    E_D07_HARM_12,            // 12次谐波
    E_D07_HARM_13,            // 13次谐波
    E_D07_HARM_14,            // 14次谐波
    E_D07_HARM_15,            // 15次谐波
    E_D07_HARM_16,            // 16次谐波
    E_D07_HARM_17,            // 17次谐波
    E_D07_HARM_18,            // 18次谐波
    E_D07_HARM_19,            // 19次谐波
    E_D07_HARM_20,            // 20次谐波
    E_D07_HARM_21,            // 21次谐波

}E_D07_PARA_HARM;

/* 参数上N次数 */
typedef enum
{
    E_D07_LAST_NULL,        // 上不关次数
    E_D07_LAST_1,            // 上1次
    E_D07_LAST_2,            // 上2次
    E_D07_LAST_3,            // 上3次
    E_D07_LAST_4,            // 上4次
    E_D07_LAST_5,            // 上5次
    E_D07_LAST_6,            // 上6次
    E_D07_LAST_7,            // 上7次
    E_D07_LAST_8,            // 上8次
    E_D07_LAST_9,            // 上9次
    E_D07_LAST_10,            // 上10次    
    E_D07_LAST_11,            // 上11次
    E_D07_LAST_12,            // 上12次    
}E_D07_PARA_LAST;

/* 规约参数费率 */
typedef enum
{
    E_D07_RATE_NULL,        // 不关心费率参数
    E_D07_RATE_1,            // 费率
    E_D07_RATE_2,            // 费率2
    E_D07_RATE_3,            // 费率3
    E_D07_RATE_4,            // 费率4
    E_D07_RATE_5,            // 费率5
    E_D07_RATE_6,            // 费率6
    E_D07_RATE_7,            // 费率7
    E_D07_RATE_8,            // 费率8
    E_D07_RATE_9,            // 费率9
    E_D07_RATE_10,            // 费率10
    E_D07_RATE_11,            // 费率11
    E_D07_RATE_12,            // 费率12
    E_D07_RATE_13,            // 费率13
    E_D07_RATE_14,            // 费率14
    E_D07_RATE_15,            // 费率15
    E_D07_RATE_16,            // 费率16
    E_D07_RATE_17,            // 费率17
    E_D07_RATE_18,            // 费率18
    E_D07_RATE_19,            // 费率19
    E_D07_RATE_20,            // 费率20
    E_D07_RATE_21,            // 费率21
    E_D07_RATE_22,            // 费率22
    E_D07_RATE_23,            // 费率23
    E_D07_RATE_24,            // 费率24
    E_D07_RATE_25,            // 费率25
    E_D07_RATE_26,            // 费率26
    E_D07_RATE_27,            // 费率27
    E_D07_RATE_28,            // 费率28
    E_D07_RATE_29,            // 费率29
    E_D07_RATE_30,            // 费率30
    E_D07_RATE_31,            // 费率31
    E_D07_RATE_32,            // 费率32
    E_D07_RATE_33,            // 费率33
    E_D07_RATE_34,            // 费率34
    E_D07_RATE_35,            // 费率35
    E_D07_RATE_36,            // 费率36
    E_D07_RATE_37,            // 费率37
    E_D07_RATE_38,            // 费率38
    E_D07_RATE_39,            // 费率39
    E_D07_RATE_40,            // 费率40
    E_D07_RATE_41,            // 费率41
    E_D07_RATE_42,            // 费率42
    E_D07_RATE_43,            // 费率43
    E_D07_RATE_44,            // 费率44
    E_D07_RATE_45,            // 费率45
    E_D07_RATE_46,            // 费率46
    E_D07_RATE_47,            // 费率47
    E_D07_RATE_48,            // 费率48
    E_D07_RATE_49,            // 费率49
    E_D07_RATE_50,            // 费率50
    E_D07_RATE_51,            // 费率51
    E_D07_RATE_52,            // 费率52
    E_D07_RATE_53,            // 费率53
    E_D07_RATE_54,            // 费率54
    E_D07_RATE_55,            // 费率55
    E_D07_RATE_56,            // 费率56
    E_D07_RATE_57,            // 费率57
    E_D07_RATE_58,            // 费率58
    E_D07_RATE_59,            // 费率59
    E_D07_RATE_60,            // 费率60
    E_D07_RATE_61,            // 费率61
    E_D07_RATE_62,            // 费率62
    E_D07_RATE_63,            // 费率63
    E_D07_RATE_64,            // 费率64
}E_D07_PARA_RATE;


/* 结构 */












/* 数据转换方向 */
typedef enum
{
    E_D07_TRANS_U2F,    // 数据格式从用户侧转换到帧侧
    E_D07_TRANS_F2U,    // 数据格式从帧侧转换到用户侧
    
}E_D07_TRANS_FLG;

typedef eD07Err (*F_D07_RULER_TRANS)(E_D07_TRANS_FLG, char*, char*);

/* 规约的格式类型 */
typedef enum
{    
    E_D07_FMT_UNKOWN,                // 未知 
    E_D07_FMT_XXXX,                    // XXXX
    E_D07_FMT_XX_2 = E_D07_FMT_XXXX,// XXXX
    E_D07_FMT_XXXXXX,                // XXXXXX
    E_D07_FMT_XX_3 = E_D07_FMT_XXXXXX,
    E_D07_FMT_XX_4,                    // XX 重复4次
    E_D07_FMT_XX_6,                    // XX 重复6次
    E_D07_FMT_XX_8,                    // XX 重复8次
    E_D07_FMT_XX_10,                // XX 重复10次
    E_D07_FMT_XX_16,                // XX 重复16次
    E_D07_FMT_X_XXX,                // X.XXX
    E_D07_FMT_XX_XX,                // XX.XX
    E_D07_FMT_XX_XXXX,                // XX.XXXX
    E_D07_FMT_XX_XXXX_8,            // XX.XXXX
    E_D07_FMT_XXX_X,                // XXX.X
    E_D07_FMT_XXX_XXX,                // XXX.XXX
    E_D07_FMT_XXXXXX_XX,            // XXXXXX.XX
    E_D07_FMT_XXXXXXXX,                // XXXXXXXX
    E_D07_FMT_XXXXXX_2,                // XXXXXX 重复2次
    E_D07_FMT_XXXXXX_6,                // XXXXXX 重复6次
    E_D07_FMT_NN,                     // NN 
    E_D07_FMT_NNNN,                 // NNNN 
    E_D07_FMT_NN_4,                 // NN 
    E_D07_FMT_NN_6,                 // NN 
    E_D07_FMT_NN_32,                 // NN 
    E_D07_FMT_N_NNN,                 // N.NNN 
    E_D07_FMT_NN_NNNN,                 // NN.NNNN 
    E_D07_FMT_NNN_N,                 // NNN.N 
    E_D07_FMT_hhmmss,                // hhmmss 
    E_D07_FMT_MMDDhhmm,                // MMDDhhmm
    E_D07_FMT_MMDDNN_14,            // MMDDNN 重复14次
    E_D07_FMT_hhmmNN_14,            // hhmmNN 重复14次
    E_D07_FMT_DDhh,                    // DDhh
    E_D07_FMT_YYMMDDWW,                // YYMMDDWW 
    E_D07_FMT_YYMMDDNN,                // YYMMDDWW 
    E_D07_FMT_YYMMDDhhmm,            // YYMMDDhhmm 
    E_D07_FMT_YYMMDDhhmmNN,            // YYMMDDhhmm
    E_D07_FMT_YYMMDDhhmmss,            // YYMMDDhhmmss 
    E_D07_FMT_YYMMDDhhmmss_2,        // YYMMDDhhmmss YYMMDDhhmmss
    E_D07_FMT_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm,        // YYMMDDhhmmss YYMMDDhhmmss xx.xxxx YYMMDDhhmm
    E_D07_FMT_XX_XXXX_YYMMDDhhmm,    /* XX.XXXX
                                       YYMMDDhhmm */
    E_D07_FMT_YYMMDDhhmmss_XXX_XXX_YYMMDDhhmmss,    /* YYMMDDhhmmss
                                                        XXX.XXXX
                                                        YYMMDDhhmmss */

    // 复杂结合数据格式类型
    E_D07_FMT_RECD_LOSS_LESS_VOLT,        // 失压 欠压数据格式    
    E_D07_FMT_RECD_ANTI_PHASE,    // 逆相序数据格式
    E_D07_FMT_RECD_UN_BALANCE,    // 不平衡数据格式    
    E_D07_FMT_RECD_LESS_MORE_LOSS_ELEC,        // LESS MORE LOSS 失流、过流、断流数据记录格式
    E_D07_FMT_RECD_VOLT_PASS_PERCENT,        // 电压合格率数据统计
    E_D07_FMT_RECD_METER_PROG,        // 电压合格率数据统计
    E_D07_FMT_RECD_METER_CLEAR,        // 电压合格率数据统计
    E_D07_FMT_RECD_DEAMD_CLEAR,        // 电压合格率数据统计
    E_D07_FMT_RECD_EVENT_CLEAR,        // 电压合格率数据统计
    E_D07_FMT_RECD_ADJUST_TIME,        // 电压合格率数据统计
    E_D07_FMT_RECD_PART_PROG,        // 时段表编程记录
    E_D07_FMT_RECD_ZONE_PROG,        // 时区表编程记录
    E_D07_FMT_RECD_WEEKDAY_PROG,
    E_D07_FMT_RECD_HOLIDAY_PROG,
    E_D07_FMT_RECD_POWER_PROG,
    E_D07_FMT_RECD_PAYOFF_PROG,
    E_D07_FMT_RECD_OPEN_METER_CAP,
    E_D07_FMT_RECD_OPEN_BTN_BOX,
}E_D07_RULER_FORMAT;

/* 规约读写类型 */
typedef enum
{
    E_D07_RDWR_READ_ONLY,        /* 只读 */
    E_D07_RDWR_WRITE_ONLY,        /* 只写 */
    E_D07_RDWR_READ_WRITE,        /* 读写 */

}E_D07_RULER_RDWR;


/*数据结构*/


typedef struct
{
    E_D07_PARA_PAYOFF   payoff;        // 结算日(不关心，当前，上(1-12)结算日)
    E_D07_PARA_RATE      rate;         // 费率(不关心，1~63)
    E_D07_PARA_HARM      harm;         // 谐波次(不关心，1~21)
    E_D07_PARA_LAST      last;         // 上(n)次记录(不关心，1-10)
    
}S_D07_RULER_PARA;
typedef struct
{
    E_D07_RULER_TYPE     type;            // 规约类型
    E_D07_RULER_RDWR    rdwr;            // 规约数据的读写属性
    E_D07_RULER_FORMAT     format;            // 规约数据的格式
    F_D07_RULER_TRANS    func;            // 数据域转换函数指针
    S_D07_RULER_PARA     para;            // 规约类型的额外数据
    int                 len;            // 数据域字节长度 
    char         name[NAME_LEN];        // 该条规约数据项名称

}S_D07_RULER_INFO;



typedef struct
{
    UINT32 ruler_id;     /* 规约标签符 C0 C1  型如 0x40E3 */
    UINT8  ctrl_code;    /* 控制码 */
    UINT8  data_len;     /* 数据域字节数 包括规约和其它数据 */
    char   address[13];  /* 地址 */
    char* data;          /* 数据 */

}S_D07_PACK_FRAME;

/* 控制码域 C 帧传送方向 */
typedef enum
{
    E_D07_CTRL_DIR_M2S, // 主站到从站
    E_D07_CTRL_DIR_S2M, // 从站到主站
    
}E_D07_CTRL_DIR;

/* 从站异常标志 */
typedef enum
{
    E_D07_CTRL_SR_OK, // 从站正常应答
    E_D07_CTRL_SR_NO, // 从站异常应答
    
}E_D07_CTRL_SR;

/* 有无后续帧标志 */
typedef enum
{
    E_D07_CTRL_FLW_NONE, //无后续
    E_D07_CTRL_FLW_HAVE, //有后续
    
}E_D07_CTRL_FLW;
/*  解析包数据域的结果标志 */
typedef enum
{
    E_D07_UNPD_FLG_OK,          /* 解析正确 数据可用 且数据是正常的答复 */
    E_D07_UNPD_FLG_ERROR_OK,    /* 解析正确 数据可用 且数据是异常的答复 */
    E_D07_UNPD_FLG_NONE_DATA,   /* 没有对应除规约 数据 */
    E_D07_UNPD_FLG_FUNC_NULL,   /* 没有解析函数 */
    E_D07_UNPD_FLG_FUNC_ERROR,  /* 有解析函数，但解析过程出错 */
    E_D07_UNPD_FLG_NONE_STRUCT, /* 没有解析该类型的用户侧数据结构 */

}E_D07_UNPD_FLG;

/* 控制域C 功能码 */
typedef enum
{
    E_D07_CTRL_RESV                  = 0x00,// 保留
    E_D07_CTRL_SYNC_TIME          = 0x08,// 广播校时
    E_D07_CTRL_READ_DATA          = 0x11,// 读数据
    E_D07_CTRL_READ_AFTERDATA      = 0x12,// 读后续数据
    E_D07_CTRL_READ_ADDR          = 0x13,// 读通信地址
    E_D07_CTRL_WRITE_DATA          = 0x14,// 写数据
    E_D07_CTRL_WRITE_ADDR          = 0x15,// 写通信地址
    E_D07_CTRL_FREEZ_COMM          = 0x16,// 冻结命令
    E_D07_CTRL_MODIFY_BAUD          = 0x17,// 修改通信速率
    E_D07_CTRL_MODIFY_PASSWORD      = 0x18,// 修改密码
    E_D07_CTRL_CLEAR_MAXDEMAND      = 0x19,// 最大需量清零
    E_D07_CTRL_CLEAR_METER          = 0x1A,// 电表清零
    E_D07_CTRL_CLEAR_EVENT          = 0x1B,// 事件清零
    E_D07_CTRL_COMM                  = 0x1C // 控制命令
    
}E_D07_CTRL_FNC;
/* 控制域C 结构定义 */
typedef struct
{
    E_D07_CTRL_DIR direct;    // 帧传送方向 
    E_D07_CTRL_SR  reply;      // 从站应答异常标志
    E_D07_CTRL_FLW follow;      // 有无后续帧
    E_D07_CTRL_FNC funcode;      // 功能码
    
}S_D07_CTRL_CODE;



typedef struct
{
    float fXX_XXXX;
    UINT8 YY;
    UINT8 MM;
    UINT8 DD;
    UINT8 hh;
    UINT8 mm;
        
}S_D07_XX_XXXX_YYMMDDhhmm;


typedef struct
{
    UINT32 a_times;
    UINT32 a_seconds;
    UINT32 b_times;
    UINT32 b_seconds;
    UINT32 c_times;
    UINT32 c_seconds;
}S_D07_XXXXXX_6;

typedef struct
{
    UINT8 MM;    // month
    UINT8 DD;    // day
    UINT8 hh;    // hour
    UINT8 mm;    // minute
}S_D07_MMDDhhmm;


typedef struct
{
    UINT32 ulTimes;   // XXXXXX
    UINT32 ulMinutes; // XXXXXX
}S_D07_XXXXXX_2;


typedef struct
{
    float fXX_XXXXX[8];
}S_D07_XX_XXXX_8;

typedef struct
{
    UINT8 hh;
    UINT8 mm;
    UINT8 ss;    
}S_D07_hhmmss;

typedef struct
{
    UINT8 hh;
    UINT8 mm;
    UINT8 NN;

}S_D07_hhmmNN;

typedef struct
{
    S_D07_hhmmNN hhmmNN[14];
}S_D07_hhmmNN_14;




typedef struct
{
    UINT8 YY;   // year
    UINT8 MM;    // month
    UINT8 DD;    // day
    UINT8 hh;    // hour
    UINT8 mm;    // minute
    UINT8 ss;    // second

}S_D07_YYMMDDhhmmss;

typedef struct
{
    S_D07_YYMMDDhhmmss sBegin;
    S_D07_YYMMDDhhmmss sEnd;
}S_D07_YYMMDDhhmmss_2;

typedef struct
{
    UINT8 YY;   // year
    UINT8 MM;    // month
    UINT8 DD;    // day
    UINT8 hh;    // hour
    UINT8 mm;    // minute

}S_D07_YYMMDDhhmm;

typedef struct
{
    S_D07_YYMMDDhhmmss            sBgnTime;      // 开始时刻         6 * 1
    S_D07_YYMMDDhhmmss            sEndTime;      // 结束时刻         6 * 1 
    S_D07_XX_XXXX_YYMMDDhhmm   sDemand;        // 最大需量及发生时间 
}S_D07_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm;

typedef struct
{
    S_D07_YYMMDDhhmmss            sBgnTime;      // 开始时刻         6 * 1
    S_D07_YYMMDDhhmmss            sEndTime;      // 结束时刻         6 * 1 
    float fXXX_XXX;
}S_D07_YYMMDDhhmmss_XX_XXXX_YYMMDDhhmmss;





typedef struct
{
    double dForthHavePowerIcr;     // 事件发生期间某相 正向有功电能增量     XXXXXX.XX
    double dBackHavePowerIcr;   // 事件发生期间某相 反向有功电能增量      XXXXXX.XX
    double dCombNone1PowerIcr;    // 事件发生期间某相 组合无功1电能增量     XXXXXX.XX
    double dCombNone2PowerIcr;    // 事件发生期间某相 组合无功2电能增量     XXXXXX.XX
    
}S_D07_EventRecordVoltPhase;

typedef struct
{
    float fVolt;                // 事件发生期间某相 电压 形如 XXX.X
    float fElec;                // 事件发生期间某相    电流      XXX.XXX
    float fHavePowerRate;        // 事件发生期间某相 有功功率  XX.XXXX
    float fNonePowerRate;        // 事件发生期间某相 无功功率  XX.XXXX    
    float fPowerRateFactor;        // 事件发生期间某相 功率因数  X.XXX
}S_D07_EventRecordFactor;


typedef struct
{
    double    dAtimeTotal;        // 事件发生期间 总安时数    XXXXXX.XX
    double    dAtimePhaseA;        // 事件发生期间 A相安时数   XXXXXX.XX
    double    dAtimePhaseB;        // 事件发生期间 B相安时数   XXXXXX.XX
    double    dAtimePhaseC;        // 事件发生期间 C相安时数   XXXXXX.XX
}S_D07_EventRecordAtime; // 安时数

typedef struct
{
    S_D07_YYMMDDhhmmss            sBgnTime;      // 开始时刻         6 * 1
    S_D07_YYMMDDhhmmss            sEndTime;      // 结束时刻         6 * 1
    S_D07_EventRecordVoltPhase sPowerTotal; // 总电能增量       4 * 8
    S_D07_EventRecordVoltPhase sPowerA;         // A相电能增量         4 * 8
    S_D07_EventRecordFactor       sFactorA;    // A相因数            5 * 4
    S_D07_EventRecordVoltPhase sPowerB;         // B相电能增量        4 * 8 
    S_D07_EventRecordFactor       sFactorB;    // B相因数             5 * 4
    S_D07_EventRecordVoltPhase sPowerC;     // C相电能增量         4 * 8
    S_D07_EventRecordFactor    sFactorC;    // C相因数            5 * 4
    S_D07_EventRecordAtime     sAtime;      // 安时数              4 * 8
    
}S_D07_LESS_LOSS_VOLT;// 失压欠压过压，断相事件发生记录数据  35 * 4 + 12 = 152字节



typedef struct
{
    S_D07_YYMMDDhhmmss            sBgnTime;       // 开始时刻    6 * 1    
    S_D07_YYMMDDhhmmss            sEndTime;       // 结束时刻    6 * 1
    S_D07_EventRecordVoltPhase sPowerTotal;    // 总电能增量  4 * 8
    S_D07_EventRecordVoltPhase sPowerA;           // A相电能增量 4 * 8
    S_D07_EventRecordVoltPhase sPowerB;           // B相电能增量 4 * 8
    S_D07_EventRecordVoltPhase sPowerC;        // C相电能增量 4 * 8
    
}S_D07_EventRevPhase; // 电压 电流逆相    共(140)字节

typedef struct
{
    S_D07_YYMMDDhhmmss            sBgnTime;              // 开始时刻
    S_D07_YYMMDDhhmmss            sEndTime;              // 结束时刻
    float                        fMaxUnbalanceFator;     // 最大不平衡率
    S_D07_EventRecordVoltPhase sPowerTotal;          // 总电能增量  4 * 8
    S_D07_EventRecordVoltPhase sPowerA;                   // A相电能增量 4 * 8
    S_D07_EventRecordVoltPhase sPowerB;                   // B相电能增量 4 * 8
    S_D07_EventRecordVoltPhase sPowerC;               // C相电能增量 4 * 8
    
}S_D07_EventUnBalance; //不平衡事件记录


typedef struct
{
    S_D07_YYMMDDhhmmss            sBgnTime;      // 开始时刻         6 * 1
    S_D07_YYMMDDhhmmss            sEndTime;      // 结束时刻         6 * 1
    S_D07_EventRecordVoltPhase sPowerTotal; // 总电能增量       4 * 8
    S_D07_EventRecordVoltPhase sPowerA;         // A相电能增量         4 * 8
    S_D07_EventRecordFactor       sFactorA;    // A相因数            5 * 4
    S_D07_EventRecordVoltPhase sPowerB;         // B相电能增量        4 * 8 
    S_D07_EventRecordFactor       sFactorB;    // B相因数             5 * 4
    S_D07_EventRecordVoltPhase sPowerC;     // C相电能增量         4 * 8
    S_D07_EventRecordFactor    sFactorC;    // C相因数            5 * 4
    //S_D07_EventRecordAtime     sAtime;      // 安时数              4 * 8
    
}S_D07_LESS_MORE_LOSS_ELET;// 失流，断流，过流事件发生记录数据  35 * 4 + 12 = 152字节


typedef struct
{
    UINT32 ulMonitorVoltMinutes;  // 监测时间 单位分
    float fVoltPassPercent;      // 电压合格率
    float fVoltOverLimitPercent; // 电压超限率
    UINT32 ulVoltOverUpperLimitMinutes;  // 电压超上限时间
    UINT32 ulVoltOverLowerLimitMinutes;  // 电压超下限时间
    float fVoltValueMax;                // 电压最大值
    float fVoltValueMin;                // 电压最小值
    S_D07_MMDDhhmm sTimeVoltValueMax;       // 最高电压发生时间      
    S_D07_MMDDhhmm sTimeVoltValueMin;         // 最低电压发生时间
}S_D07_EventPassPercent; // 合格率事件数据



typedef struct
{
    S_D07_YYMMDDhhmmss sProgTime;         // 事件发生时刻
    UINT8              programer[4];      // 编程者标识 4字节
    UINT32               ulRulerID[10];     // 编程的前十个数据标识码，型如 0xFFFFFFFF 不足用0xFFFFFFFF补足
}S_D07_EventMeterProgram; // 编程事件记录 


typedef struct
{    
    double dForthHavePower; // XXXXXX.XX 正向 有功总电能
    double dBackHavePower;  // XXXXXX.XX 反向 有功总电能
    double dNonePowerQuad1; // XXXXXX.XX 事件发生时第 1 象限无功电能
    double dNonePowerQuad2; // XXXXXX.XX 事件发生时第 2 象限无功电能
    double dNonePowerQuad3; // XXXXXX.XX 事件发生时第 3 象限无功电能
    double dNonePowerQuad4; // XXXXXX.XX 事件发生时第 4 象限无功电能
}S_D07_EventPower;

typedef struct
{
    S_D07_XX_XXXX_YYMMDDhhmm sFortHaveDemand;   // 正向有功最大需量及发生时间     XX.XXXX YYMMDDhhmm
    S_D07_XX_XXXX_YYMMDDhhmm sBackHaveDemand;   // 正向有功最大需量及发生时间    XX.XXXX    YYMMDDhhmm
    S_D07_XX_XXXX_YYMMDDhhmm sNoneDemandQuad1;  // 正向有功最大需量及发生时间    XX.XXXX    YYMMDDhhmm
    S_D07_XX_XXXX_YYMMDDhhmm sNoneDemandQuad2;  // 正向有功最大需量及发生时间    XX.XXXX    YYMMDDhhmm
    S_D07_XX_XXXX_YYMMDDhhmm sNoneDemandQuad3;  // 正向有功最大需量及发生时间    XX.XXXX    YYMMDDhhmm
    S_D07_XX_XXXX_YYMMDDhhmm sNoneDemandQuad4;  // 正向有功最大需量及发生时间    XX.XXXX    YYMMDDhhmm

}S_D07_EventDemand; //最大需量清零事件某项最大需量及发生时间

typedef struct
{
    S_D07_YYMMDDhhmmss sOccurTime;       // 事件发生时刻
    UINT8              operator[4];      // 操作者标识 4字节

    S_D07_EventDemand    sTotal;             // 电表清零前 总 需量数据
    S_D07_EventDemand    sPhaseA;         // 电表清零前 A相需量数据
    S_D07_EventDemand    sPhaseB;         // 电表清零前 B相需量数据
    S_D07_EventDemand    sPhaseC;         // 电表清零前 C相需量数据
}S_D07_EventDemandClear;

typedef struct
{
    S_D07_YYMMDDhhmmss sOccurTime;       // 事件发生时刻
    UINT8              operator[4];      // 操作者标识 4字节

    S_D07_EventPower    sTotal;             // 电表清零前 总 电能数据
    S_D07_EventPower    sPhaseA;         // 电表清零前 A相电能数据
    S_D07_EventPower    sPhaseB;         // 电表清零前 B相电能数据
    S_D07_EventPower    sPhaseC;         // 电表清零前 C相电能数据

}S_D07_EventMeterClear; // 电表清0事件记录


typedef struct
{
    S_D07_YYMMDDhhmmss sOccurTime;       // 事件发生时刻
    UINT8              operator[4];      // 操作者标识 4字节
    UINT32               ulEventID;         // 清零事件标记
}S_D07_EventClear;         // 事件清零记录


typedef struct
{
    UINT8              operator[4]; // 操作者标识 4字节
    S_D07_YYMMDDhhmmss sBefTime;    // 校时前时间     
    S_D07_YYMMDDhhmmss sAftTime;     // 校时后时间
    
}S_D07_EventTiming;        // 校时事件记录

typedef struct
{
    UINT8 MM;
    UINT8 DD;
    UINT8 NN;    
}S_D07_MMDDNN;

typedef struct
{
    S_D07_YYMMDDhhmmss sOccurTime;       // 事件发生时刻
    UINT8              operator[4];      // 操作者标识 4字节
    S_D07_MMDDNN       sZoneSet1[14];    // 时区表编程前第一套第(1~14)时区表数据
    S_D07_MMDDNN       sZoneSet2[14];    // 时区表编程前第二套第(1~14)时区表数据
}S_D07_EventZone; // 时区表编程事件记录

typedef struct
{
    S_D07_YYMMDDhhmmss sOccurTime;       // 事件发生时刻
    UINT8              operator[4];      // 操作者标识 4字节
    UINT8               ucTableNum;         // 周休是编程前采用的日段表号
}S_D07_EventWeekDay;   // 周休日编程记录


typedef struct
{
    UINT8 DD;
    UINT8 hh;
}S_D07_DDhh;

typedef struct
{
    UINT8 YY;
    UINT8 MM;
    UINT8 DD;
    UINT8 NN;
    
}S_D07_YYMMDDNN;

typedef struct
{
    UINT8 YY;
    UINT8 MM;
    UINT8 DD;
    UINT8 hh;
    UINT8 mm;
    UINT8 NN;    
}S_D07_YYMMDDhhmmNN;

typedef struct
{

    S_D07_YYMMDDhhmmss sOccurTime;       // 事件发生时刻
    UINT8              operator[4];      // 操作者标识 4字节

    S_D07_YYMMDDNN  sHoliday[12];        
    // 中国法定节假日11天 元旦一天。春节三天。清明一天。五一一天。端午一天。中秋一天。国庆三天。
    // 编程前第N节假日数据 标准协议支持254个节假日
    // 
    
}S_D07_EventHoliday; // 节假日编程事件记录


typedef struct
{
    S_D07_YYMMDDhhmmss sOccurTime;       // 事件发生时刻
    UINT8              operator[4];      // 操作者标识 4字节
    UINT8               ucWord;             // 功组合方式编程前的有功组合方式特征字
}S_D07_EventCombPower; // 有功/无功组合方式编程记录

typedef struct
{
    S_D07_YYMMDDhhmmss sOccurTime;       // 事件发生时刻
    UINT8              operator[4];      // 操作者标识 4字节
    S_D07_DDhh         sDay1;
    S_D07_DDhh         sDay2;
    S_D07_DDhh         sDay3;
}S_D07_EventPayoff;

typedef struct
{
    S_D07_YYMMDDhhmmss sBgnTime;       // 开始时刻
    S_D07_YYMMDDhhmmss sEndTime;       // 结束时刻 
    S_D07_EventPower sPowerBeforOpen;  // 开盖前电能数据
    S_D07_EventPower sPowerAfterOpen;  // 开盖后电能数据

}S_D07_EventOpenCap;        // 开表盖记录

typedef struct
{
    UINT8 YY;
    UINT8 MM;
    UINT8 DD;
    UINT8 WW;
}S_D07_YYMMDDWW;

typedef struct
{
    S_D07_MMDDNN MMDDNN[14];
    
}S_D07_MMDDNN_14;




typedef struct
{
    BOOL bRateErr;  // 费率数超
    BOOL bDayErr;   // 日时段数超
    BOOL bYearErr;  // 年时区数超
    BOOL bCommErr;  // 通讯速率不能更改
    BOOL bPwdErr;   // 密码错或未授权
    BOOL bNoneReq;  // 没有请求数据
    BOOL bOtherErr; // 其它错误
}S_D07_ERR;


/* 用户接口侧数据结构 */
typedef union
{

    float   fPower; 
    S_D07_YYMMDDWW sYYMMDDWW;
    S_D07_ERR      sError;
}U_D07_DATA_UNPACK;

/* 解析帧信息数据结构 */
typedef struct
{
    
    UINT8                  ctrl_c;           /* 控制码 字节型 */
    UINT8                  lead_num;         /* 前导字符 0xFE的个数 */
    UINT32                 ruler_id;         /* 规约ID */
   
    unsigned short           data_len;         /* 数据域长 */
    unsigned short         frame_len;         /* 整个帧长*/
      
    E_D07_RULER_TYPE       type;             /* 规约类型 */
    E_D07_RULER_FORMAT     format;           /* 帧中数据格式 */
    E_D07_UNPD_FLG         flag;             /* 解析数据到用户侧数据的标志，说明解析状态 决定data_unpack 是否可用 */
    S_D07_CTRL_CODE        ctrl_s;           /* 控制码结构 */
    S_D07_RULER_PARA       para;             /* 规约类型的额外数据 */ 
    UINT8                  data_pack[D07_DATA_MAX_NR];   /* 数据域 不包括规约ID的部分 没有做转换 */
    UINT8                  address[D07_ADDR_LEN+1];     //12位地址域数据
    U_D07_DATA_UNPACK      data_unpack;               /* 转换为用户侧的数据 */ 
    //UINT8   *pDataUser;
}S_D07_UNPACK;


/* 功能函数 */

/* 根据规约ID 获取对应规约的数据类型信息 */
int get_d07_ruler_info(UINT32 rulerID, S_D07_RULER_INFO *outRulerInfo);
int get_d07_ruler_id(E_D07_RULER_TYPE type,
                     S_D07_RULER_PARA para,
                     UINT32 *poutRulerID);
/* 通过地址 规约ID 数据域数据及长度 封帧 */
int pack_d07_frame_by_data(S_D07_PACK_FRAME *inPara, char *outBuffer, int *outLength); 

//解析 dlt645 07帧 
int unpack_d07_frame(void *inpBuffer, int inBufferLength, S_D07_UNPACK *outpFrame);


/* 从一段数据中找出第一个有效dlt645 2007帧的位置及长度 */
int get_d07_first_valid_frame(const UINT8 *pBuf,         // 要解析的buf
                              UINT16 usLenBuf,           // 要解析的buf字节数
                              UINT8  **ppFirstPos,       // 输出帧buffer
                              UINT16 *pusLenFrame);      // 输出Frame字节数
/* 判断一个帧是否为正确的 */
int is_right_d07_frame(void *inpBuffer, int inBufferLength);
/* */
void d07_str2bcd(const char *pstr, UINT8 *pbcd, int len);
void d07_bcd2str(const char* inBCD, char *outStr, int len);
double d07_bcd_to_double(UINT8* pBcd, int len,int LeftDigit);
int d07_remove_dot(int len, char*inArray, char *outArray); // 去掉字符串格式中去的小数点

int d07_add_dot(int len, char*inArray, char *outArray,  int dotpos); // 加上小数点

/* 解析控制码 */
eD07Err trans_d07ctrl_struct2char(UINT8 *outChar, S_D07_CTRL_CODE *inStruct); //将结构封装成字节
eD07Err trans_d07ctrl_char2struct(UINT8 inChar,  S_D07_CTRL_CODE *outStruct); //将字节封装成结构




/* 数据解析函数 */
/*XX*/
eD07Err trans_d07_data_XX_2(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_3(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_4(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_6(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_8(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_10(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_16(E_D07_TRANS_FLG flag, char *user, char *frame);

eD07Err trans_d07_data_XXXXXX_2(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XXXXXX_6(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XXXX(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XXXXXX(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XXX_X(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XXX_XXX(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_XXXX(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XXXXXXXX(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XXXXXX_XX(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_XXXX_YYMMDDhhmm(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_XXXX_8(E_D07_TRANS_FLG flag, char *user, char *frame);
/*NN*/
eD07Err trans_d07_data_NN(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_NNNN(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_NN_4(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_NN_6(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_NN_32(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_N_NNN(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_NN_NNNN(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_NNN_N(E_D07_TRANS_FLG flag, char *user, char *frame);


/*X.X*/
eD07Err trans_d07_data_X_XXX(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_XX_XX(E_D07_TRANS_FLG flag, char *user, char *frame);

/*YYMMDDWWhhmmss*/
eD07Err trans_d07_data_hhmmss(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_MMDDhhmm(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_DDhh(E_D07_TRANS_FLG flag, char *user, char *frame);

eD07Err trans_d07_data_YYMMDDWW(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_YYMMDDNN(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_YYMMDDhhmm(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_YYMMDDhhmmNN(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_YYMMDDhhmmss(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_YYMMDDhhmmss_2(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_YYMMDDhhmmss_2_XX_XXXX_YYMMDDhhmm(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_YYMMDDhhmmss_XXX_XXX_YYMMDDhhmmss(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_MMDDNN_14(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_hhmmNN_14(E_D07_TRANS_FLG flag, char *user, char *frame);
/* 组合数据 */
eD07Err trans_d07_data_recd_loss_less_volt(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_anti_phase(E_D07_TRANS_FLG flag, char *user, char *frame);    // 逆相序数据格式
eD07Err trans_d07_data_recd_un_balance(E_D07_TRANS_FLG flag, char *user, char *frame);    // 不平衡数据格式    
eD07Err trans_d07_data_recd_less_more_loss_elec(E_D07_TRANS_FLG flag, char *user, char *frame);        // less more loss 失流、过流、断流数据记录格式
eD07Err trans_d07_data_recd_volt_pass_percent(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_meter_prog(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_meter_clear(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_demand_clear(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_event_clear(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_adjust_time(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_part_prog(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_zone_prog(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_weekday_prog(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_holiday_prog(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_power_prog(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_payoff_prog(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_open_meter_cap(E_D07_TRANS_FLG flag, char *user, char *frame);
eD07Err trans_d07_data_recd_open_btn_box(E_D07_TRANS_FLG flag, char *user, char *frame);

//错误状态字
eD07Err trans_d07_data_err(E_D07_TRANS_FLG flag, char *user, char *frame);


#pragma pack(pop) 
#endif //__DLT645_2007_RULERS_H__

