#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "sam.h"
#include "iso14443.h"
#include "ticket.h"



static void DRVAPI_TEST_UseTip(void)
{
    printf("==========================\n");
	printf("0:exit \n"); 			//对交通部SAM进行初始化 
    printf("1:octjtb_saminit \n"); 			//对交通部SAM进行初始化 
	printf("2:searchcard \n"); 				//寻卡
//    printf("3:svt_active \n");				//CPU 卡复位,此前必须做过searchcard函数
//	printf("4:svt_getofflineserial \n");	//取脱机交易计数
//    printf("5:svt_read \n");				//读取地铁票的数据内容
//	printf("6:jtb_selectppse \n");			//通过AID的方式选择互联互通电子钱包应用
	printf("7:search_jtbcpu \n");			//通过AID的方式选择互联互通电子钱包应用
//   printf("6:svt_recharge \n");			//对地铁svt 卡进行充值
 //   printf("7:svt_consum \n");				//对地铁票进行脱机消费
	printf("8:change to main nfc device \n");
	printf("9:change to sub nfc device \n");
	printf("==========================\n");    
}
static int DRVAPI_TEST_GetCmdChoice(void)
{
    int choice = -1;
    char cmd[1024];

    scanf("%s", cmd);
    choice = atoi(cmd);

    return choice;
}

int main(int argc, char *argv[])
{
    int ret;
    int choice = 0;

	int nsamsck = 0;
	unsigned char p_len_sam_id;
	unsigned char p_sam_id[256];
	unsigned char p_len_terminal_id;
	unsigned char p_terminal_id[256];
	unsigned char lpcardno[256];
	unsigned char lpinf[256];
	unsigned char lpserial[256];
	int opertion = 1;
	unsigned char lpcardinf[512];
	unsigned char val =0x0A;
	unsigned char pid[256];
	unsigned char type;
	
	samdev_init();
	rfdev_init();
//	ISO14443A_Init(val);

    while (1)
    {
        DRVAPI_TEST_UseTip();
        printf("please input your choice:\n");
        choice = DRVAPI_TEST_GetCmdChoice();
		printf("choice: %d\n",choice);
        switch (choice)
        {
        case 1:
        {
			sam_hst_active(nsamsck, &p_len_sam_id, p_sam_id, &p_len_terminal_id, p_terminal_id);
			//octjtb_saminit(nsamsck, &p_len_sam_id, p_sam_id, &p_len_terminal_id, p_terminal_id);
            break;
        }
        case 2:
        {
			searchcard(lpcardno);
            break;
        }
        case 3:
        {
			svt_active(lpinf);
            break;
        }
        case 4:
        {
			svt_getofflineserial(nsamsck,lpserial);
            break;
        }
        case 5:
        {	
			svt_read(opertion, lpcardinf);
            break;
        }
		case 6:
        {
			jtb_selectppse();
            break;
        }
		case 7:
        {
			search_jtbcpu(pid,type);
            break;
        }
       case 0:
        {
			goto exit_app;
            break;
        }
	   case 8:
	   {
		   printf("***** case 8 antenna_switch******\n");
			
			antenna_switch(0);
			printf("***** case 8 ISO14443A_Init******\n");
			ISO14443A_Init(val);
			break;
	   }
		case 9:
	   {
		   printf("***** case 9 antenna_switch******\n");
			antenna_switch(1);
			printf("***** case 9 ISO14443A_Init******\n");
			ISO14443A_Init(val);
			break;
	   }
       default:
        {
            printf("***************************\n");
            printf("***** incorrect numuber !!!******\n");
            printf("***************************\n");
            break;
        }
        }
    }
exit_app:
	samdev_destory();
	rfdev_destory();

}
