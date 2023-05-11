//
// Created by flpkp on 2020/3/22.
//

#include "TicketElectDepositBase.h"

void TicketElectDepositBase::getDealData(ETYTKOPER operType,uint16_t& len_data,uint8_t * dataBack,long amount){
    int length = 0;
    char StationCode[2+1] = {0};
    char LineCode[2+1] = {0};
    char DeviceType[2+1] = {0};
    char DeviceCode[3+1] = {0};
    char time[14+1] = {0};
    char exittime[14+1] = {0};
    long penalty_val	= 0;
    uint8_t tempTime[7] = {0};
    uint8_t validexitTime[7] = {0};

    char dealData[256] = {0};
    sprintf(LineCode, "%02x", ((Api::current_station_id >>8) & 0x00FF));
    sprintf(StationCode, "%02x", (Api::current_station_id & 0xFF));
    sprintf(DeviceType, "%02x", (uint8_t)(Api::current_device_type));
    sprintf(DeviceCode, "%03x", Api::current_device_id);
    
    memcpy(tempTime, CmdSort::m_time_now, 7);
    sprintf(time, "%02x%02x%02x%02x%02x%02x%02x", tempTime[0], tempTime[1], tempTime[2], tempTime[3], tempTime[4], tempTime[5], tempTime[6]);
    g_Parameter.get_validexittime(Api::current_station_id,tempTime, validexitTime, &penalty_val);
    sprintf(exittime, "%02x%02x%02x%02x%02x%02x%02x", validexitTime[0], validexitTime[1], validexitTime[2], validexitTime[3], validexitTime[4], validexitTime[5], validexitTime[6]);
    memcpy(m_ticket_data.qr_ticketdata.validOutTime, exittime, 14);
    switch (operType)
    {
        case operEntry:
        {
            memcpy(dealData,"01",2);
            length += 2;
            memcpy(dealData + length, time, 14);
            length += 14;
            memcpy(dealData + length, exittime, 14);
            length += 14;
            memcpy(dealData + length, LineCode, 2);//��·����
            length += 2;
            memcpy(dealData + length, StationCode, 2);//վ�����
            length += 2;
            memcpy(dealData + length, DeviceType, 2);//�豸����
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);//�豸���
            length += 3;
            memcpy(dealData + length, "112233445566", 12);//��ʱ��д
            length += 12;
            memcpy(dealData + length, "00", 2);
            length += 2;
            memcpy(dealData + length, "00000000", 8);
            length += 8;
            len_data = length;

        }
            break;
        case operExit:
        {
            memcpy(dealData,"02",2);
            length += 2;
            memcpy(dealData + length, time, 14);
            length += 14;
            memcpy(dealData + length, LineCode, 2);//��·����
            length += 2;
            memcpy(dealData + length, StationCode, 2);//վ�����
            length += 2;
            memcpy(dealData + length, DeviceType, 2);//�豸����
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);//�豸���
            length += 3;
            memcpy(dealData + length, "112233445566", 12);
            length += 12;
            //Ӧ�۽��
            sprintf(dealData + length,"%08X",amount);
            length += 8;

            memcpy(dealData + length, "00", 2);
            length += 2;
            memcpy(dealData + length, "00000000", 8);
            length += 8;
            len_data = length;

        }
            break;

        case operPenaltyRes:										//����֪ͨ
        {
            memcpy(dealData, "07", 2);								//������ 07����
            length += 2;
            memcpy(dealData + length, time, 14);					//ʱ��
            length += 14;
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);			//����ԱID operator_id������10λ��BOM����ԱIDΪ6λǰ��4λ0
            length += 6;
            memcpy(dealData + length, LineCode, 2);				    //��·����
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//վ�����
            length += 2;
            memcpy(dealData + length, "01", 2);						//�������� 01-��ʱ����
            length += 2;
            sprintf(dealData + length, "%08X", amount);				//���
            length += 8;
            memcpy(dealData + length, exittime, 14);				//��Ч��վʱ��
            length += 14;
            // ��������û�д� ���뻪�ڹ�ͨ����������
            memcpy(dealData + length, "00000000", 8);				//��֤��
            length += 8;
            len_data = length;
        }
            break;
        case operClearEntry:										//���վ
        {
            memcpy(dealData, "21", 2);								//������ 21���վ
            length += 2;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.entryTime, 14);//��վʱ�� TODO �Ҿ������Ӧ�����Ѽ�¼�Ľ�վʱ��
            length += 14;
            memcpy(dealData + length, exittime, 14);				//��Ч��վʱ��
            length += 14;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.startStation, 4);//��վ�ĳ�վid
            length += 4;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.entryDeviceType, 2);	//�豸����
            length += 2;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.entryDeviceCode, 3);//�豸���
            length += 3;
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);				//����ԱID
            length += 6;
            memcpy(dealData + length, LineCode, 2);				        //���վ��·����
            length += 2;
            memcpy(dealData + length, StationCode, 2);					//���վվ�����
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				    //���վ�豸����
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				    //���վ�豸���
            length += 3;
            memcpy(dealData + length, time, 14);					    //���վ��ʱ��
            length += 14;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		    //�ն˺�
            length += 12;
            memcpy(dealData + length, "00", 2);						    //״̬��0��������1��ʧ��
            length += 2;
            memcpy(dealData + length, "00000000", 8);				    //��֤��
            length += 8;
            len_data = length;
        }
            break;
        case operClearExit:												      //���վ
        {
            memcpy(dealData, "22", 2);								            //������ 22���վ
            length += 2;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.exitTime, 14);//��վʱ�� TODO �Ҿ������Ӧ�����Ѽ�¼�ĳ�վʱ��
            length += 14;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.exitStation, 4);//��վ�ĳ�վid
            length += 4;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.exitDeviceType, 2);//�豸����
            length += 2;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.exitDeviceCode, 3);//�豸���
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		//�ն˺�
            length += 12;
            //Ӧ�۽��
            sprintf(dealData + length, "%08X", amount);
            length += 8;
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);			//����ԱID
            length += 6;
            memcpy(dealData + length, LineCode, 2);				    //���վ��·����
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//���վվ�����
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				//���վ�豸����
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				//���վ�豸���
            length += 3;
            memcpy(dealData + length, time, 14);					//���վ��ʱ��
            length += 14;
            memcpy(dealData + length, "00", 2);						//״̬��0��������1��ʧ��
            length += 2;
            memcpy(dealData + length, "00000000", 8);				//��֤��
            length += 8;
            len_data = length;
        }
            break;
        case operAddEntry:											//����վ
        {
            memcpy(dealData, "23", 2);								//������ 23����վ
            length += 2;
            memcpy(dealData + length,time, 14);						//����վʱ�� TODO �õ�����ʱ��
            length += 14;
            memcpy(dealData + length, exittime, 14);				//��Ч��վʱ��
            length += 14;
            memcpy(dealData + length, m_ticket_data.qr_ticketdata.startStation, 4);//��վ�ĳ�վid
            length += 4;
            //��վ�豸����  Ϊ�̶�ֵ00
            memcpy(dealData + length, "00", 2);
            length += 2;
            //��վ�豸���   �̶�ֵΪ000
            memcpy(dealData + length, "000", 3);
            length += 3;
            //����ԱID  һ��10λ
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);
            //����վ�豸����
            length += 6;
            memcpy(dealData + length, LineCode, 2);			    	//����վ��·����
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//����վվ�����
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				//����վ�豸����
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				//����վ�豸���
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		//�ն˺�
            length += 12;
            memcpy(dealData + length, "00", 2);						//״̬��0��������1��ʧ��
            length += 2;
            memcpy(dealData + length, "00000000", 8);				//��֤��
            length += 8;
            len_data = length;
        }
            break;
        case operAddExit:											//����վ
        {
            memcpy(dealData, "24", 2);								//������ 24����վ
            length += 2;
            memcpy(dealData + length, time, 14);					//����վʱ�� TODO �õ�����ʱ��
            length += 14;
            memcpy(dealData + length, LineCode, 2);				    //����վ��·����
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//����վ��·����
            length += 2;
            memcpy(dealData + length, "00", 2);						//�豸���� �̶�Ϊ00
            length += 2;
            memcpy(dealData + length, "000", 3);					//�豸��� �̶�Ϊ000
            length += 3;
            sprintf(dealData + length, "%08X", amount);				//Ӧ�۽��
            length += 8;
            //10λ����ԱID
            memcpy(dealData + length, "0000", 4);
            length += 4;
            memcpy(dealData + length, Api::operator_id, 6);			//����ԱID
            length += 6;
            memcpy(dealData + length, LineCode, 2);				    //����վ��·����
            length += 2;
            memcpy(dealData + length, StationCode, 2);				//����վ��·����
            length += 2;
            memcpy(dealData + length, DeviceType, 2);				//����վ��·����
            length += 2;
            memcpy(dealData + length, DeviceCode, 3);				//����վ��·����
            length += 3;
            memcpy(dealData + length, m_p_current_sam + 4, 12);		    //�ն˺�
            length += 12;
            memcpy(dealData + length, "00", 2);						    //״̬��0��������1��ʧ��
            length += 2;    
            memcpy(dealData + length, "00000000", 8);			    	//��֤��
            length += 8;
            len_data = length;
        }
            break;
        default:
            break;
    }
    memcpy(dataBack, (uint8_t * )dealData, length);
    //g_Record.log_buffer("blue write ", dataBack,len_data);


}
