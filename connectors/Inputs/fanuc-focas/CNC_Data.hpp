#pragma once
#include <string>
#include<iostream>
#include <any>

using namespace std; 

enum CNCDataType {
        tALARM,
        tSTATUS,
        tAXIS,
        tPROG,
        tSYSTEM
    };

class CNCData {
	public:
		string key;
		any value;
		CNCDataType dType;

	CNCData(string key, any value, CNCDataType type){
		this->key = key;
		this->value = value;
		this->dType = type;
	}

 //    static bool is_int(const boost::any & operand)
	// {
	//     return operand.type() == typeid(int);
	// }

	// static bool is_string(const boost::any & operand)
	// {
	//     return any_cast<std::string>(&operand);
	// }

	// static bool is_float(const boost::any & operand)
	// {
	//     return operand.type() == typeid(float);
	// }

};


// struct CNCDataHash {
//     std::size_t operator()(const CNCData& data) const {
//     	std::string str = data.key() + data.value();
//         return std::hash<std::string>()(str);
//     }
// };

// struct CNCDataEqual{
// 	bool operator()(const CNCData &a, const CNCData &b) const 
// 	{
// 	    return a.key() == b.key();
// 	}
// };

// class StrData : public CNC_Data
// {
// 	public:
// 		StrData(std::string property, std::string data, dType type){
// 			_property = property;
// 			_data = data;
// 			_type = type;
// 		};
// };

// class IntData : public CNC_Data
// {
// 	public:
// 		IntData(std::string property, int data, dType type){
// 			_property = property;
// 			_data = std::to_string(data);
// 			_type = type;
// 		};
// };

// class LintData : public CNC_Data
// {
// 	public:
// 		LintData(std::string property, long int data, dType type){
// 			_property = property;
// 			_data = std::to_string(data);
// 			_type = type;
// 		};
// };
