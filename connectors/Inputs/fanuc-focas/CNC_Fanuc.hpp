#pragma once
#include <stdio.h>
#include <exception>
#include <list>
#include "CNC_Data.hpp"
#include "json.hpp"

using json = nlohmann::json;

const std::string aut_msg[2][11] = {{"NO SELECTION", "MDI", "TAPE/DNC","MEMORY","EDIT","TEACH IN"},
						   {"MDI", "MEMORY", "NO SELECTION","EDIT","HANDlE","JOG",
						   "TEACH IN JOG","TEACH IN HANDLE","INC FEED","REFERENCE","REMOTE"}};


const std::string run_msg[2][14] = {{"STOP", "HOLD", "START","MSTR(jog mdi)",
						   "RESTART NOT BLINKING","PROGRAM RESTART","SEQUENCE NUMBER SEARCH",
						   "RESTART BLINKING","RESET","unnused","unnused","unnused",
						   "unnused","HPCC"},{"****(RESET)","STOP","HOLD","START","MSTR"}};


const std::string edit_msg[3][61] = {{"NOT EDITING","EDIT","SEARCH","VERIFY",
						  "CONDENSE","READ","PUNCH"},{"NOT EDITING","EDIT","SEARCH","OUTPUT","INPUT","COMPARE",
							"LABEL SKIP","RESTART","HPCC","PTRR","RVRS","RTRY","RVED",
							"HANDLE","OFFSET","WORK OFFSET","AICC/AI-APC","MEMORY-CHECK",
							"CUSTOMER BOARD","SAVE","AI NANO","AI APC","MBL APC","AICC 2",
							"AI HPCC","5AXIS","LEN","RAD","WZR","Empty","Empty","Empty",
							"Empty","Empty","Empty","Empty","Empty","Empty","Empty",
							"TCP","TWP","TCP+TWP","APC","PRG-CHK","APC","S-TCP","AICC 2",
							"Empty","Empty","Empty","Empty","Empty","Empty","Empty","Empty",
							"Empty","Empty","Empty","Empty","ALL SAVE","NOTSAVE"},{"Not editting","EDIT","SeaRCH","OUTPUT","INPUT","COMPARE",
							"Label SKip","OFSeT","Work ShiFT","ReSTaRt","RVRS","RTRY","RVED",
							"Empty","PTRR","Empty","AICC/AI-APC","MEmory-CHecK",
							"Empty","SAVE","AI NANO","HPCC","Empty","AICC 2",
							"AI HPCC","5AXIS","OFSX","OFSZ","WZR","OFSY","Empty","TOFS",
							"Empty","Empty","Empty","Empty","Empty","Empty","Empty",
							"TCP","TWP","TCP+TWP","APC","PRG-CHK","APC","S-TCP","Empty",
							"Empty","Empty","Empty","Empty","Empty","Empty","Empty","Empty",
							"Empty","Empty","Empty","Empty","ALL SAVE","NOTSAVE"}};


const std::string motion_msg[2][4] = {{"****","MOTION","DWELI","WAIT"},{"****","MOTION","DWELI"}};

const std::string emergency_msg[2][4] = {{"NO EMERGENCY","EMERGENCY"},{"NO EMERGENCY","EMERGENCY","RESET","WAIT"}};

const std::string alarm_msg[2][9] = {{"NO ALARM","ALARM"},{"****","ALARM","BATTERY LOW","FAN (NC/SERVO AMPLIFIER)","PS WARNING",
							 "FSSB WARNING","INSULATE WARNING","ENCODER WARNING","PMC ALARM"}};

const std::string speed_unit[5] = {"mm/min","inch/min","rpm","mm/rev","inch/rev"};


class CNC_Exception: public std::exception
{
	private:
   		std::string _msg_;
	public:
   		CNC_Exception(const std::string& msg) : _msg_(msg) {}
  		~CNC_Exception() {}
	 	virtual const char* what() const throw()
	  	{
	    	return _msg_.c_str();
	  	}
};


class CNC_Fanuc
{
	// CNC_DataSet;
	std::list <CNCData> mDataset; // CNCDataHash, CNCDataEqual> dataset;
	enum QueryType{
		eALL,
		eSTATUS,
		eALARMS,
		eAXIS,
		ePROGRAM,
		eSYSTEM
	};

	private:
		unsigned short mHandle;
		short int mNumAxes;
		std::string ipAddress;
		std::string mCNCType = "";
		std::string mMachineType;
		std::string getMachineType(std::string cnctype);
		void buildStatData();
		void buildAlarmsData();
		void buildAxisData();
		void buildFeedData();
		void buildProgramData();
		void buildPMCparams();
		void buildPMCalarms();
		
		void buildGdata();
		
		short buildSystemData();
		short UpdateData();
		short UpdateData(std::string type);	
		short UpdateData(QueryType type);
		std::string dataToString();
		json dataToJSON();

	public:
		CNC_Fanuc(std::string ip, int port = 8193, int timeout = 10);
		~CNC_Fanuc();
		short getData(std::string type, std::string* data);
		short getData(QueryType type, std::string* data);
		short getData(std::string type, json* data);
		short getData(QueryType type, json* data);
};
