#ifndef __NETRSU_H__
#define __NETRSU_H__

#include "registers.h"

#define NETWORK_FILE "/opt/config"

//***B9��������������֡����****
typedef struct AntennaInfoN_n{
			char Status;			//����״̬
			char Power;			//���߹���
			char Channel;			//�����ŵ�
			char Control_state;		//���߿���״̬
}ANTENNAINFON_N;				
typedef struct control_S{
	int Control_datetime[7];//����ʱ��
	char ControlCount;	//����������
	char ControlStatusN[4];	//������״̬
	char AntennaCount;	//��������
	AntennaInfoN_n  AntennaInfoN[8];	//��������
}RSUCONTROLER;
//************B0���豸״̬������֡����******************
typedef struct PSAMInfoN_n{
 char Psam_ID[8];
}PSAMINFON_N;
typedef struct RSU_data_init{
	char RSUManuID;		//·�൥Ԫ���̴���
	char RSUID[3];		//·�൥Ԫ���
	char RSUVersion[2];	//·�൥Ԫ����汾��
	char ControlId;		//��ǰ���������߿��ƺ�Id
	char PSAMCount;		//PSAM����
	PSAMInfoN_n PSAMInfoN[8];	//PSAM��Ϣ
}RSU_DATA_INIT;
//**********************D1���豸����������֡����***************
typedef struct RSU_reset{
	char AntennaCount;	//�������������
	char RSUState;		//�����Ƿ�����״̬  0��û������  1��ϵͳ�Ѿ�����
}RSU_RESET;

void init_net_rsu(void);
void send_RSU(char command,bool ReSend,char state,int num);


#endif

