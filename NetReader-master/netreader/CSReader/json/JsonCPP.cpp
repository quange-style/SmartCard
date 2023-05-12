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

//1010—进站验票 2010—出站验票 3010—PITM 验票 4010—取票验票
string JsonCPP::GetRespJson(Value json_ticket,Value json_ticket_add)
{
	Json::Value json_temp;      // 临时对象，供如下代码使用
	json_temp["version"] = Json::Value("01");//报文头-版本号
	json_temp["encMode"] = Json::Value("00");//报文头-加密方式 0—明文，1—3DES 加密，2—国密
	json_temp["length"] = Json::Value("00");
	json_temp["result"] = Json::Value("00");
	json_temp["type"] = Json::Value("1011");
	Json::Value root; // 表示整个 json 对象
	root["key_string"] = Json::Value("value_string"); // 新建一个 Key(名为：key_string),
	//赋予字符串值:"value_string"
	root["key_number"] = Json::Value(12345); // 新建一个 Key(名为:key_number),
	//赋予数值:12345。
	root["key_boolean"] = Json::Value(false);  // 新建一个 Key(名为:key_boolean),
	//赋予bool值:false。
	root["key_double"] = Json::Value(12.345); // 新建一个 Key(名为:key_double),
	//赋予 double 值:12.345
	root["key_object"] = json_temp; // 新建一个 Key(名为:key_object)，
	//赋予 json::Value 对象值。
	//root["key_array"].append("array_string");// 新建一个 Key(名为:key_array),
	//类型为数组,对第一个元素赋值为字符串:"array_string"
	//root["key_array"].append(1234);  // 为数组 key_array 赋值,对第二个元素赋值为:1234。
	
	//Json::ValueType type = root.type();// 获得 root 的类型，此处为 objectValue 类型。

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
	
	std::cout << "名字：" << json_object["name"] << std::endl;
	std::cout << "年龄：" << json_object["age"] << std::endl;
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

