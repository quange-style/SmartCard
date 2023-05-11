// JsonCPP.cpp : Defines the entry point for the console application.
//

#include "../json/json.h"
#include <string>
#include "../link/myprintf.h"
#include "JsonCPP.h"

using namespace Json;
using namespace std;

JsonCPP::JsonCPP(void)
{
}

/*int Parse()
{
	std::string strValue = "{\"key1\":\"value1\",\"array\":[{\"key2\":\"value2\"},{\"key2\":\"value3\"},{\"key2\":\"value4\"}]}";
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(strValue, value))
	{
		std::string out = value["key1"].asString();
		std::cout << out << std::endl;
		const Json::Value arrayObj = value["array"];
		for (int i=0; i<arrayObj.size(); i++)
		{
			out = arrayObj[i]["key2"].asString();
			std::cout << out;
			if (i != arrayObj.size() - 1 )
				std::cout << std::endl;
		}
	}
	std::cout << std::endl << std::endl << std::endl;
	return 0;
}
int Parse1()
{
	std::string strValue = "{\"key1\":\"value1\",\"key2\":\"value2\",\"array\":{\"key2\":\"value2\"}}";
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(strValue, value))
	{
		std::string out = value["key1"].toStyledString();
		std::cout << out << std::endl;

		std::string out0 = value["key2"].toStyledString();
		std::cout << out0 << std::endl;

		const Json::Value arrayObj = value["array"];
		

		std::string out1 = arrayObj.toStyledString();
		std::cout << out1 << std::endl;

		
	}
	std::cout << std::endl << std::endl << std::endl;
	return 0;
}

int Build()
{
	Json::Value root;
	root["key1"] = "value1";
	root["key2"] = "value2";
	
	Json::Value arrayObj;
	Json::Value item;
	for (int i=0; i<10; i++)
	{
		item["key"] = i;
		arrayObj.append(item);
	}
	root["array"] = arrayObj;
	root.toStyledString();
	std::string out = root.toStyledString();
	std::cout << out << std::endl;
	return 0;
}*/

//1010����վ��Ʊ 2010����վ��Ʊ 3010��PITM ��Ʊ 4010��ȡƱ��Ʊ
string JsonCPP::GetRespJson(Value json_ticket,Value json_ticket_add)
{
	Json::Value json_temp;      // ��ʱ���󣬹����´���ʹ��
	json_temp["version"] = Json::Value("01");//����ͷ-�汾��
	json_temp["encMode"] = Json::Value("00");//����ͷ-���ܷ�ʽ 0�����ģ�1��3DES ���ܣ�2������
	json_temp["length"] = Json::Value("00");
	json_temp["result"] = Json::Value("00");
	json_temp["type"] = Json::Value("1011");
	Json::Value root; // ��ʾ���� json ����
	root["key_string"] = Json::Value("value_string"); // �½�һ�� Key(��Ϊ��key_string),
	//�����ַ���ֵ:"value_string"
	root["key_number"] = Json::Value(12345); // �½�һ�� Key(��Ϊ:key_number),
	//������ֵ:12345��
	root["key_boolean"] = Json::Value(false);  // �½�һ�� Key(��Ϊ:key_boolean),
	//����boolֵ:false��
	root["key_double"] = Json::Value(12.345); // �½�һ�� Key(��Ϊ:key_double),
	//���� double ֵ:12.345
	root["key_object"] = json_temp; // �½�һ�� Key(��Ϊ:key_object)��
	//���� json::Value ����ֵ��
	//root["key_array"].append("array_string");// �½�һ�� Key(��Ϊ:key_array),
	//����Ϊ����,�Ե�һ��Ԫ�ظ�ֵΪ�ַ���:"array_string"
	//root["key_array"].append(1234);  // Ϊ���� key_array ��ֵ,�Եڶ���Ԫ�ظ�ֵΪ:1234��
	
	//Json::ValueType type = root.type();// ��� root �����ͣ��˴�Ϊ objectValue ���͡�

	Json::FastWriter fast_writer;
	string jsonRoot = fast_writer.write(root);
	dbg_formatvar("jsonRoot=%s",jsonRoot.c_str());
	
	//Json::StyledWriter styled_writer;
	//std::cout << styled_writer.write(root) << std::endl;

	//root.

	return jsonRoot;
}
/*
int ReadJson()
{
	Json::Reader reader;
	Json::Value json_object;
	const char* json_document = "{\"age\" : \"26\",\"name\" : \"qinhuang\"}";
	if (!reader.parse(json_document, json_object))
		return 0;
	
	std::cout << "���֣�" << json_object["name"] << std::endl;
	std::cout << "���䣺" << json_object["age"] << std::endl;
	string sst;
	sst=json_object["name"].asString();
	cout<<"dddd   "<<sst<<endl;
	string st;
	st=json_object["age"].asString();
	cout<<"ssss   "<<st<<endl;
	
	return 1;
}*/

/*void main()
{
	Parse1();
	//Build();
	//WriteJson();
	//ReadJson();
	system("pause");
}*/

