// Publics.h

#pragma once

#include <math.h>
#include <stdint.h>
#include <string.h>

class Publics
{
public:
    Publics(void);

    // ascll�ַ�ת��ΪHEX�ֽڣ���'1'->0x01, 'a'->0x0a, 'A'->0x0a
    static uint8_t char_to_half_hex(char ch);

    // ��2�ֽ�charת��Ϊһ��uint8_t��HEX
    static uint8_t two_char_to_hex(char * p_two_char);

    // ���ֽ�Hexת1�ֽ�ascll
    static char half_hex_to_char(uint8_t half_hex);

    // 1�ֽ�Hexת2�ֽ�ascll
    static void hex_to_two_char(uint8_t byte_hex, char * p_two_char);

	// �ַ����Ƿ��ǿ�Ԥ�����ַ���
	enum STRING_TYPE{FREE_SEQ, NUMERIC_SEQ, HEX_SEQ};
	static bool string_is_expected(char * p_string, int len_string, STRING_TYPE type, char expective_1 = 0, char expective_2 = 0);

    // �ַ���ת��ΪHEX���У���"123456789A"->"\x12\x34\x56\x78\x9A"
    static void string_to_hexs(char * p_string, size_t len_string, uint8_t * p_hexs, size_t size_hexs);
    static void string_to_bcds(char * p_string, size_t len_string, uint8_t * p_bcds, size_t size_bcds);

    // HEX��ת��Ϊ�ַ���
    static void hexs_to_string(uint8_t * p_hexs, size_t len_hexs, char * p_string, size_t size_string);
    static void bcds_to_string(uint8_t * p_bcds, size_t len_bcds, char * p_string, size_t size_string);

    // �ж�һ��uint8_tֵ�Ƿ�BCD��ʽ����hex��ʽ���Ƿ���a��b��c��d��e��f
    static bool byte_bcd_valid(uint8_t bcd_byte);

    // ���ڴ浹��
    static int memory_reverse_order(char * p_memory, int len_memory, int len_reverse_unit);

    // �滻�ַ����е�ָ���ַ�����������Ŀ���ַ���
    static void truncat_char(char * p_src_string, size_t len_src, char ch_need_truncat, char ch_truncat_to, char * p_target_string);

	// ɾ���ַ�����ߵ�������ָ���ַ�
	// static char * trim_left(char * p_src, char c_to_trim);

	// ɾ���ַ����ұߵ�������ָ���ַ�
	// static char * trim_right(char * p_src, char c_to_trim);

    static bool char_to_bool(char ch);

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // ����ģ��

	// ������ָ���������Ƿ���һ����ֵ
	template <class T>
	static bool every_equal(T tag_val, T * src_buffer, int len)
	{
		for (int i=0;i<len;i++)
		{
			if (src_buffer[i] != tag_val)
				return false;
		}

		return true;
	};

    // �ۼӺͣ�����Ĳ��ֶ���
    template <class T>
    static T add_sum(T * p_data, int len_in_T)
    {
        T sum = 0;
        for (int i=0; i<len_in_T; i++)
        {
            sum += *p_data;
        }
        return sum;
    };

    // ����
    template <class T>
    static T xor_sum(T * p_data, int len_in_T)
    {
        T sum = 0;
        for (int i=0; i<len_in_T; i++)
        {
            sum ^= *p_data;
        }
        return sum;
    };

    // �ַ���ת��ΪHex��ֵ����"789A"->"\x789A"��T1���Ϊ����������������ͳ���
    template <class T>
    static T string_to_hex(char * p_string, int len_string)
    {
        T hex_val = 0;
        for (int i=0; i<len_string; i++)
        {
            hex_val = (hex_val << 4) + char_to_half_hex(p_string[i]);
        }

        return hex_val;
    };
    template <class T>
    static T string_to_bcd(char * p_string, int len_string)
    {
        return string_to_hex<T>(p_string, len_string);
    };

    // �ַ���ת��Ϊʮ������ֵ����"7890"->7890��T1���Ϊ����������������ͳ���
    template <class T>
    static T string_to_val(char * p_string, int len_string)
    {
        T val = 0;
        for (int i=0; i<len_string; i++)
        {
            if (p_string[i] != 0x20)
                val = val * 10 + (p_string[i] - '0');
        }
        return val;
    };

    // �����hex�ֽ����һ����ֵ����ֵ�ĳ���Ϊ���������õ���󳤶�,����VC��MAKEuint16_t��MAKEuint32_t
    // bOrder=true\false��ʾת������ֽ���ǰ\�ں�
    template <class T>
    static void make_val(uint8_t * p_hexs, size_t len_hexs, T& var, bool low_byte_front)
    {
        size_t size_var = sizeof(T);
        if (len_hexs < size_var) size_var = len_hexs;

        var = 0;
        if (low_byte_front)
        {
            for (size_t i=0; i<size_var; i++)
            {
                var = (var << 8) + p_hexs[i];
            }
        }
        else
        {
            for (size_t i=size_var; i>0; i--)
            {
                var = (var << 8) + p_hexs[i-1];
            }
        }
    }

    // ����ֵת��ΪBCD���У���101->"\x01\x01"
    template <class T>
    static void val_to_bcds(T val, uint8_t * p_bcds, int size_bcds)
    {
        uint8_t val_byte;
        int pos_bcd = size_bcds - 1;

        do
        {
            val_byte = (uint8_t)(val % 100);
            val = (val - val_byte) / 100;

            p_bcds[pos_bcd] = val_to_bcd(val_byte);
            pos_bcd--;

        }
        while(pos_bcd >= 0 && val != 0);
    }

    // ����ֵת��ΪBCD��ֵ����101->"\x0101"
    template <class T>
    static T val_to_bcd(T val)
    {
        T ret				= 0;
        int bits_to_move	= 0;
        uint8_t half_byte	= 0;

        do
        {
            half_byte = val % 10;
            val = (val - half_byte) /10;

            ret += half_byte << bits_to_move;
            bits_to_move += 4;

        }
        while (val != 0);

        return ret;
    }

    // ��BCDת��Ϊ��ֵ����"\x0101"->101
    template <class T>
    static T bcd_to_val(T bcd)
    {
        T ret				= 0;
        T to_multiple		= 1;
        uint8_t half_byte	= 0;

        do
        {
            half_byte = bcd % 0x10;
            bcd >>= 4;

            ret += (T)(half_byte * to_multiple);
            to_multiple *= 10;

        }
        while (bcd != 0);

        return ret;
    }

    template <class T>
    static T hexs_to_val(uint8_t * p_hex, int len_hex)
    {
        T ret = 0;

        for (int i=0; i<len_hex; i++)
        {
            ret = (ret * 100) + bcd_to_val(p_hex[i]);
        }
        return ret;
    }
    template <class T>
    static T bcds_to_val(uint8_t * p_bcd, int len_bcd)
    {
        return hexs_to_val<T>(p_bcd, len_bcd);
    }

    // ȡһ����������λ�������Ҷ���
    template<class T>
    static T bits_get_align_right(T src_val, int start_bit, int end_bit)
    {
        int typesize = sizeof(T) * 8 - 1;

        if (start_bit >= end_bit)
        {
            src_val <<= (typesize - start_bit);
            src_val >>= typesize - start_bit + end_bit;
        }
        else if (end_bit > start_bit)
        {
            src_val <<= (typesize - end_bit);
            src_val >>= typesize - end_bit + start_bit;
        }
        return src_val;
    }

    // ȡһ������ֵ��������֧�ֵ�����ͣ�һ��byte
    template<class T>
    static uint8_t byte_get(T src_val, int byte_pos)
    {
        return (uint8_t)((src_val >> (byte_pos * 8)) & 0xFF);
    }

};
