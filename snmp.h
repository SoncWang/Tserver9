#ifndef __SNMP_H__
#define __SNMP_H__

#include <string> 
#include "server.h" 

using namespace std; 

#define HWGANTRY_COUNT 25

int snmpInit(void);

int snmpInit(void);
int SendSnmpOid(string mSnmpOid);
int snmptrapInit(void);

#pragma pack(push, 1)
typedef enum
{
   //﮵�
   hwAcbGroupBatVolt=1,                //��ص�ѹ
   hwAcbGroupBatCurr=2,            //��ص���
   hwAcbGroupTotalCapacity=3,                //���������
   hwAcbGroupTotalRemainCapacity=4,               //���ʣ������
   hwAcbGroupBackupTime=5,              //��ر���ʱ��
   hwAcbGroupBatSoh=6,             //��� SOH
   //���ص�Դ
   hwApOrAblVoltage=7,                //A/AB ��ѹ
   hwBpOrBclVoltage=8,                //B/BC ��ѹ
   hwCpOrCalVoltage=9,                //C/CA ��ѹ
   hwAphaseCurrent=10,               //A �����
   hwBphaseCurrent=11,              //B �����
   hwCphaseCurrent=12,             //C �����
   hwDcOutputVoltage=13,             //DC �����ѹ
   hwDcOutputCurrent=14,               //DC �������
   //����������
   hwEnvTemperature=15,              //�����¶�ֵ
   hwEnvHumidity=16,             //����ʪ��ֵ
   //ֱ���յ�
	hwDcAirCtrlMode=17,			//�յ�����ģʽ
	hwDcAirRunStatus=18,			//�յ�����״̬
	hwDcAirCompressorRunStatus=19,		//�յ�ѹ��������״̬
	hwDcAirInnrFanSpeed=20,			//�յ��ڻ�ת��
	hwDcAirOuterFanSpeed=21,			//�յ�����ת��
	hwDcAirCompressorRunTime=22,		//�յ�ѹ��������ʱ��
	hwDcAirEnterChannelTemp=23,		//�յ��ط���¶�
	hwDcAirPowerOnTempPoint=24,		//�յ������¶ȵ�
	hwDcAirPowerOffTempPoint=25		//�յ��ػ��¶ȵ�
}EM_HUAWEIGantry;

typedef struct
{
   //﮵�
   string strhwAcbGroupBatVolt;                //��ص�ѹ "51.1"
   string strhwAcbGroupBatCurr;            //��ص���
   string strhwAcbGroupTotalCapacity;                //���������
   string strhwAcbGroupTotalRemainCapacity;               //���ʣ������
   string strhwAcbGroupBackupTime;              //��ر���ʱ��
   string strhwAcbGroupBatSoh;             //��� SOH
   //���ص�Դ
   string strhwApOrAblVoltage;                //A/AB ��ѹ
   string strhwBpOrBclVoltage;                //B/BC ��ѹ
   string strhwCpOrCalVoltage;                //C/CA ��ѹ
   string strhwAphaseCurrent;               //A �����
   string strhwBphaseCurrent;              //B �����
   string strhwCphaseCurrent;             //C �����
   string strhwDcOutputVoltage;             //DC �����ѹ
   string strhwDcOutputCurrent;               //DC �������
   //����������
   string strhwEnvTemperature;              //�����¶�ֵ
   string strhwEnvHumidity;            //����ʪ��ֵ
   //ֱ���յ�
	string strhwDcAirCtrlMode;			//�յ�����ģʽ
	string strhwDcAirRunStatus;			//�յ�����״̬
	string strhwDcAirCompressorRunStatus;		//�յ�ѹ��������״̬
	string strhwDcAirInnrFanSpeed;			//�յ��ڻ�ת��
	string strhwDcAirOuterFanSpeed;			//�յ�����ת��
	string strhwDcAirCompressorRunTime;		//�յ�ѹ��������ʱ��
	string strhwDcAirEnterChannelTemp;		//�յ��ط���¶�
	string strhwDcAirPowerOnTempPoint;		//�յ������¶ȵ�
	string strhwDcAirPowerOffTempPoint;		//�յ��ػ��¶ȵ�
}THUAWEIGantry;

typedef struct
{
	string hwEnvTempAlarmTraps;		//����/���¸澯
	string hwEnvTempAlarmResume;	//����/���¸澯�ָ�
	string hwEnvHumiAlarmTraps;		//��ʪ/��ʪ�澯
	string hwEnvHumiAlarmResume;	//��ʪ/��ʪ�澯�ָ�
	string hwSpareDigitalAlarmTraps;	//����ɽӵ�澯
	string hwSpareDigitalAlarmResume;	//����ɽӵ�澯�ָ�
	string hwDoorAlarmTraps;		//�Ž��澯
	string hwDoorAlarmResume;		//�Ž��澯�ָ�
	string hwWaterAlarmTraps;		//ˮ���澯
	string hwWaterAlarmResume;		//ˮ���澯�ָ�
	string hwSmokeAlarmTraps;		//�̸и澯
	string hwSmokeAlarmResume;		//�̸и澯�ָ�
}THUAWEIALARM;

#pragma pack(pop)

#endif

