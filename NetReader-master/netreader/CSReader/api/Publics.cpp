// Publics.c

#include "Publics.h"
#include "Errors.h"

Publics::Publics(void)
{
}

// ascll�ַ�ת��ΪHEX�ֽ�
uint8_t Publics::char_to_half_hex(char ch)
{
#define SUB_CAPITAL			(uint8_t)('A' - 0x0A)			// ��д��ĸ�ַ����ӦHEX�Ĳ�ֵ
#define SUB_SMALL			(uint8_t)('a' - 0x0A)			// Сд��ĸ�ַ����ӦHEX�Ĳ�ֵ
#define SUB_NUMERIC			(uint8_t)0x30					// �����ַ����Ӧ��HEXֵ�Ĳ�

    uint8_t half_byte_hex = 0;
    if (ch >= '0' && ch <= '9')
        half_byte_hex = (uint8_t)(ch - SUB_NUMERIC);
    else if (ch >= 'a' && ch <= 'z')
        half_byte_hex = (uint8_t)(ch - SUB_SMALL);
    else if (ch >= 'A' && ch <= 'Z')
        half_byte_hex = (uint8_t)(ch - SUB_CAPITAL);

    return half_byte_hex;
}

// ��2�ֽ�charת��Ϊһ��BYTE��HEX
uint8_t Publics::two_char_to_hex(char * p_two_char)
{
    return ((char_to_half_hex(p_two_char[0]) << 4) + char_to_half_hex(p_two_char[1]));
}

// ���ֽ�Hexת1�ֽ�ascll
char Publics::half_hex_to_char(uint8_t half_hex)
{
    if (half_hex >= 0 && half_hex <= 9)
        return (char)(half_hex + '0');
    else if (half_hex >= 0x0A && half_hex <= 0x0F)
        return (char)(half_hex + 'A' - 0x0A);
    else
        return (char)(half_hex);
}

// 1�ֽ�Hexת2�ֽ�ascll
void Publics::hex_to_two_char(uint8_t byte_hex, char * p_two_char)
{
    p_two_char[0] = half_hex_to_char((byte_hex >> 4) & 0x0F);
    p_two_char[1] = half_hex_to_char(byte_hex & 0x0F);
}

// �ַ����Ƿ��ǿ�Ԥ�����ַ���
bool Publics::string_is_expected(char * p_string, int len_string, STRING_TYPE type, char expective_1/* = 0*/, char expective_2/* = 0*/)
{
	bool ret	= false;
	int i		= 0;

	if (type == FREE_SEQ)
	{
		ret = true;
	}
	else if (type == NUMERIC_SEQ)
	{
		for (i=0;i<len_string;i++)
		{
			if ((p_string[i] >= '0' && p_string[i] <= '9') ||
				(expective_1 != 0 && p_string[i] == expective_1) || 
				(expective_2 != 0 && p_string[i] == expective_2))
			{
				ret = true;
			}
			else
			{
				ret = false;
				break;
			}
		}
	}
	else if (type == HEX_SEQ)
	{
		for (i=0;i<len_string;i++)
		{
			if ((p_string[i] >= '0' && p_string[i] <= '9') || (p_string[i] >= 'A' && p_string[i] <= 'F') || (p_string[i] >= 'a' && p_string[i] <= 'f') ||
				(expective_1 != 0 && p_string[i] == expective_1) || (expective_2 != 0 && p_string[i] == expective_2))
			{
				ret = true;
			}
			else
			{
				ret = false;
				break;
			}
		}
	}
	
	return ret;
}

// �ַ���ת��ΪHEX����
void Publics::string_to_hexs(char * p_string, size_t len_string, uint8_t * p_hexs, size_t size_hexs)
{
    size_t pos_string, pos_hexs;

    pos_hexs = pos_string = 0;

    if (len_string > size_hexs * 2)
        len_string = size_hexs * 2;

    if (len_string % 2 != 0)
    {
        p_hexs[pos_hexs++] = char_to_half_hex(p_string[pos_string++]);
    }

    for (; pos_string<len_string; pos_string+=2,pos_hexs++)
    {
        p_hexs[pos_hexs] = (char_to_half_hex(p_string[pos_string])<<4) + char_to_half_hex(p_string[pos_string+1]);
    }
}
void Publics::string_to_bcds(char * p_string, size_t len_string, uint8_t * p_bcds, size_t size_bcds)
{
    string_to_hexs(p_string, len_string, p_bcds, size_bcds);
}

// HEX��ת��Ϊ�ַ���
void Publics::hexs_to_string(uint8_t * p_hexs, size_t len_hexs, char * p_string, size_t size_string)
{
    size_t len;
    if (size_string < len_hexs * 2)
        len = size_string / 2;
    else
        len = len_hexs;

    for (size_t i=0; i<len; i++)
    {
        hex_to_two_char(p_hexs[i], p_string + 2 * i);
    }
}

void Publics::bcds_to_string(uint8_t * p_bcds, size_t len_bcds, char * p_string, size_t size_string)
{
    hexs_to_string(p_bcds, len_bcds, p_string, size_string);
}

// �ж�һ��BYTEֵ�Ƿ�BCD��ʽ����hex��ʽ���Ƿ���a��b��c��d��e��f
bool Publics::byte_bcd_valid(uint8_t bcd_byte)
{
    return ((bcd_byte & 0xF0) < 0xA0 && (bcd_byte & 0x0F) < 0x0A);
}

// ���ַ���������Ҫ����Э���н����ֽ���ǰ���ֽ��ں�����
int memory_reverse_order(char * p_memory, int len_memory, int len_reverse_unit/* = 2*/)
{
    char sz_temp[256]	= {0};

    if (len_memory % len_reverse_unit != 0 || len_memory < len_reverse_unit || len_memory > (int)sizeof(sz_temp))
        return ERR_INPUT_INVALID;

    for (int i=0; i<len_memory; i+=len_reverse_unit)
    {
        memcpy(sz_temp + i, p_memory + len_memory - i - len_reverse_unit, len_reverse_unit);
    }

    memcpy(p_memory, sz_temp, len_memory);

    return 0;
}

// �滻�ַ����е�ָ���ַ�����������Ŀ���ַ���
void Publics::truncat_char(char * p_src_string, size_t len_src, char ch_need_truncat, char ch_truncat_to, char * p_target_string)
{
    for (size_t i=0; i<len_src; i++)
    {
        p_target_string[i] = (p_src_string[i] == ch_need_truncat ? ch_truncat_to : p_src_string[i]);
    }
}

bool Publics::char_to_bool(char ch)
{
    return (ch - '0' > 0 ? true : false);
}

uint8_t * Publics::getUpper(uint8_t * lower)
{
	for(int i = 0;lower[i]!='\0';i++)
	{
		if(lower[i]>='a'&&lower[i]<='z')
            lower[i]-=0x20;
	}
	return lower;
}

bool Publics::substr_index(uint8_t * substr, uint8_t * str, int length)
{
	bool flag = false;
	int sizeOfSubstr = sizeof(substr)/sizeof(substr[0]);
	int sizeOfStr = length;

	//dbg_formatvar("sizeOfSubstr:%d",sizeOfSubstr);
	//dbg_formatvar("sizeOfStr:%d",sizeOfStr);

	for (int i = 0; i < sizeOfStr; i++)
	{
		if (str[i] == substr[0])
		{
			flag = true;
			for (int j = 1; j < sizeOfSubstr; j++)
			{
				if (str[i + j] != substr[j])
				{
					flag = false;
					break;
				}
			}
		}
		if (flag == true)
			break;
	}
	return flag;
}

/*char * Publics::string_to_char(string data)
{
	int length = data.length();
	char p[length];
	int i;
	for( i=0;i<data.length();i++)
	{
		p[i] = data[i];
	}
	p[i] = '\0';

	return p;
}*/
 