#include "CNC_Fanuc.hpp"
#include "fwlib32.h"
#include <string>
#include <sstream>
#include <any>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

//Função de extração de dados mensagem de ALM________________________________________________________________________
int *getALMmsg(unsigned short mHandle){
	long alm;
	cnc_alarm2(mHandle, &alm);
	unsigned short idx;
	unsigned short idx2=0;
	int *alm_list = new int[40];	// Existem apenas 32 tipos de alarmes,
								// Mas é bom deixar uma folga de memória.
	if (alm == 0)
	{
		alm_list[0]=32;
		alm_list[33]=0;
	}
	else
	{

		for (idx = 0; idx <= 31 ; idx++)
		{
    		if (alm & 0x0001)
    		{
				alm_list[idx2]=idx;
				idx2++;
			}
			alm >>= 1 ;
    	}
    	alm_list[33]=idx2;
	}
	
	if (alm_list[33]<3)
	{
		for (idx = alm_list[33];idx<=2;idx++)
		{
			alm_list[idx]=32;
		}
	}
	return alm_list;
}

long int getRate(unsigned short mHandle){
    IODBPMC *pmcrng;
    char rate_hex[20],rate[20];
    short ret,idx;
    short adr_type, data_type;
    unsigned short length, start, end;
    long int percent;
    start = 10;
    end = 10;
    adr_type = 0;                       // In case that kind of PMC address is G
    data_type = 0;                      // In case that type of PMC data is Byte
    length = 8 + (end - start + 1);
    pmcrng = (IODBPMC *)malloc( length );
    ret = pmc_rdpmcrng( mHandle, adr_type, data_type, start, end, length, pmcrng );
    if ( !ret )
    {
		for ( idx = 0 ; idx <= end-start ; idx++ )
		{
			sprintf(rate_hex,"%02X",(unsigned char)pmcrng->u.cdata[idx]);
		}
    }
    else
    {
    	sprintf(rate_hex,"ERROR!(%d)\n",ret);
    }

    char *r=rate_hex;
    r[3]='\0';
    strcpy(rate,r);

    if (rate[0]=='D' && rate[1]=='F')
    {
    	percent=200;
    }
    else if (rate[0]=='C' && rate[1]=='7')
    {
    	percent=190;
    }
    else if (rate[0]=='A' && rate[1]=='F')
    {
    	percent=180;
    }
    else if (rate[0]=='9' && rate[1]=='7')
    {
    	percent=170;
    }
    else if (rate[0]=='7' && rate[1]=='F')
    {
    	percent=160;
    }
    else if (rate[0]=='6' && rate[1]=='7')
    {
    	percent=150;
    }
    else if (rate[0]=='4' && rate[1]=='F')
    {
    	percent=140;
    }
    else if (rate[0]=='3' && rate[1]=='7')
    {
    	percent=130;
    }
    else if (rate[0]=='1' && rate[1]=='F')
    {
    	percent=120;
    }
    else if (rate[0]=='0' && rate[1]=='7')
    {
    	percent=110;
    }
    else if (rate[0]=='E' && rate[1]=='F')
    {
    	percent=100;
    }
    else if (rate[0]=='D' && rate[1]=='7')
    {
    	percent=90;
    }
    else if (rate[0]=='B' && rate[1]=='F')
    {
    	percent=80;
    }
    else if (rate[0]=='A' && rate[1]=='7')
    {
    	percent=70;
    }
    else if (rate[0]=='8' && rate[1]=='F')
    {
    	percent=60;
    }
    else if (rate[0]=='7' && rate[1]=='7')
    {
    	percent=50;
    }
    else if (rate[0]=='5' && rate[1]=='F')
    {
    	percent=40;
    }
    else if (rate[0]=='4' && rate[1]=='7')
    {
    	percent=30;
    }
    else if (rate[0]=='2' && rate[1]=='F')
    {
    	percent=20;
    }
    else if (rate[0]=='1' && rate[1]=='7')
    {
    	percent=10;
    }
    else if (rate[0]=='F' && rate[1]=='F')
    {
    	percent=0;
    }
    else
    {
    	percent=-1;
    }

    return percent;
}

string translateAlarmNo(long aNum, int aAxis) {
    switch (aNum) {
        case 0: // Parameter Switch Off
            return "Parameter Switch Off";

        case 2: // I/O
        case 7: // Data I/O
            return "I/O";

        case 4: // Overtravel
            if (aAxis > 0)
                return "Overview";
            else
                return "System";

        case 5: // Overheat
            if (aAxis > 0)
                return "Overheat";
            else
                return "System";

        case 6: // Servo
            if (aAxis > 0)
                return "Axis Servo";
            else
                return "Servo";

        case 12: // Background P/S
        case 3: // Forground P/S
        case 8: // Macro
            return "Motion";

        case 9: // Spindle
            return "Spindle";

        case 19: // PMC
            return "PMC Logic";

        case 10: // 10, 11, 13, 15.
        case 11:
        case 13:
        case 15: 
            return "System";

        default:
        	return "";
    }

    return "";
}

string CNC_Fanuc::getMachineType(string mt){
	string retval;

	if(mt==" M" || mt=="M")
		retval = "Machining Center";
	else if (mt==" T" || mt=="T")
		retval = "Lathe";
	else if (mt=="MM")
		retval = "M series with 2 path control";
	else if (mt=="TT")
		retval = "T series with 2/3 path control";
	else if (mt=="MT")
		retval = "T series with compound machining function";
	else if (mt==" P" || mt=="P")
		retval = "Punch press";
	else if (mt==" L" || mt=="L")
		retval = "Laser";
	else if (mt==" W" || mt=="W")
		retval = "Wire cut";
	return retval;
}

void CNC_Fanuc::buildAlarmsData(){
	int *alm_type_num;
	alm_type_num=getALMmsg(mHandle);
	ODBALMMSG2 almmsg;

	ODBDY2 dyn;
    memset(&dyn, 0xEF, sizeof(dyn));
    short int ret = cnc_rddynamic2(mHandle, ALL_AXES, sizeof(dyn), &dyn);

    int idx = 0;

    if (dyn.alarm != 0) {
        for (int i = 0; i < 31; i++) {
            if (dyn.alarm & (0x1 << i)) {
                ODBALMMSG2 alarms[MAX_AXIS];
                short num = MAX_AXIS;

                short ret = cnc_rdalmmsg2(mHandle, i, &num, alarms);
                if (ret != EW_OK)
                    continue;

                for (int j = 0; j < num; j++) {
                    ODBALMMSG2 &alarm = alarms[j];
                    char num[16];

                    string almType = translateAlarmNo(i, alarm.axis);
                    if (almType == "")
                        continue;

                    
                    string s(alarm.alm_msg);
                    std::istringstream stream(s);

                    std::getline(stream, s);
					idx++;

					CNCData msg("Msg_" + std::to_string(idx), s, CNCDataType::tALARM);
					CNCData type("Type_" + std::to_string(idx), almType, CNCDataType::tALARM);
					mDataset.push_back(msg);
					mDataset.push_back(type);
                }
            }
        }
    }
}

void CNC_Fanuc::buildPMCalarms(){
	OPMSG3 opmsg;
	short int read_num=5;
	short int ret;
	for (int idx=0; idx<=16; idx++)
	{
		ret = cnc_rdopmsg3(mHandle,idx,&read_num,&opmsg);
		if(ret == EW_OK && opmsg.datano!=-1){
			CNCData pmcalrm("PMC_" + std::to_string(idx),std::to_string(opmsg.datano)/*+ " - " + *opmsg.data*/, CNCDataType::tALARM);
			mDataset.push_back(pmcalrm);
		}		
	}
}

void CNC_Fanuc::buildPMCparams(){
	//Ciclo para extrair os parâmetros de PMC_______________________
	IODBPSD param;
	cnc_rdparam(mHandle, 6750, -1, 4+1*MAX_AXIS, &param); // Power On Timer (Minutes)
	CNCData powerOn("Power_On_Timer", param.u.ldata, CNCDataType::tSTATUS);				
	cnc_rdparam(mHandle, 6752, -1, 4+1*MAX_AXIS, &param); // Operation Time on Automatic Operation (Minutes)
	CNCData opertime("Auto_Operation_Timer", param.u.ldata, CNCDataType::tSTATUS);
	cnc_rdparam(mHandle, 6754, -1, 4+1*MAX_AXIS, &param); // Cutting Time (G01 / G02 / G03)) (Minutes)
	CNCData cuttime("Cutting_Timer", param.u.ldata, CNCDataType::tSTATUS);
	cnc_rdparam(mHandle, 6712, -1, 4+1*MAX_AXIS, &param); // Total Number of machined parts
	CNCData totalmachined("Total_Machined_Parts", param.u.ldata, CNCDataType::tSTATUS);
	cnc_rdparam(mHandle, 6713, -1, 4+1*MAX_AXIS, &param); // Number of required machined parts
	CNCData reqmachined("Required_Machined_Parts", param.u.ldata, CNCDataType::tSTATUS);
	cnc_rdparam(mHandle, 6711, 0, 8, &param); // Number of machined parts
	CNCData machined("Machined_Parts", param.u.ldata, CNCDataType::tSTATUS);

	mDataset.push_back(powerOn);
	mDataset.push_back(opertime);
	mDataset.push_back(cuttime);
	mDataset.push_back(totalmachined);
	mDataset.push_back(reqmachined);
	mDataset.push_back(machined);

	// cnc_rdparam(mHandle, 6756, -1, 4+1*MAX_AXIS, &param);
	// long timer_run = param.u.ldata;
}

void getProgramInfo(unsigned short handle) {
    int ret;
    char buf[1024];
    unsigned short len = sizeof(buf);
    short num;
    ret = cnc_rdexecprog(handle, (unsigned short *) &len, &num, buf);
}

void CNC_Fanuc::buildProgramData(){
	//getProgramInfo(mHandle);
	if(mCNCType == "15" || mCNCType == "16" || mCNCType == "18" || mCNCType =="21" || mCNCType == "PM"){
		CNCData prgname("Name", "Not Supported", CNCDataType::tPROG);
		mDataset.push_back(prgname);
	}
	else { 
		char fullexeprg[256];
		ODBEXEPRG exeprg;
		cnc_exeprgname(mHandle,&exeprg);
		cnc_exeprgname2(mHandle, fullexeprg);

		CNCData prgname("Name", std::string(exeprg.name), CNCDataType::tPROG);
		CNCData fullprgname("Fullname", std::string(fullexeprg), CNCDataType::tPROG);
		mDataset.push_back(prgname);
		mDataset.push_back(fullprgname);
	}
}

void CNC_Fanuc::buildFeedData(){
	// Retorna o "FEED RATE" da ferramenta_________________________
	ODBSPEED speed;
	cnc_rdspeed(mHandle,-1, &speed);

	CNCData fdr_unit("Feed_Rate_Unit", speed_unit[speed.actf.unit], CNCDataType::tAXIS);
	CNCData spd_unit("Spindle_Unit", speed_unit[speed.acts.unit], CNCDataType::tAXIS);
	CNCData fdr("Feed_Rate", speed.actf.data, CNCDataType::tAXIS);
	//CNCData spd("Spindle Speed", speed.acts.data);
    long int spdrate = getRate(mHandle);
	CNCData speedrate("Speed_Rate", spdrate , CNCDataType::tAXIS);

	mDataset.push_back(fdr_unit);
	mDataset.push_back(spd_unit);
	mDataset.push_back(fdr);
	// mDataset.push_back(spd);
	mDataset.push_back(speedrate);

    ODBSPDLNAME spindles[MAX_SPINDLE];
    short int spindleCount = MAX_SPINDLE;
    short ret = cnc_rdspdlname(mHandle, &spindleCount, spindles);

    ODBACT2 speeds;
    ret = cnc_acts2(mHandle, ALL_SPINDLES, &speeds);

    ODBSPLOAD spLoad[MAX_SPINDLE];
    short num = MAX_SPINDLE;
    ret = cnc_rdspmeter(mHandle, 0, &num, spLoad);

	std::vector<char *> mSpindles;

	if (ret == EW_OK) {
        int i = 0;
        for (i = 0; i < spindleCount; i++) {
            char name[12];
            int j = 0;
            name[j++] = spindles[i].name;
            if (spindles[i].suff1 > 0)
                name[j++] = spindles[i].suff1;
            name[j] = '\0';

            float load = spLoad[i].spload.data / pow((long double) 10.0, (long double) spLoad[i].spload.dec);
            float speed = ((int)(speeds.data[i]*1000))/1000.F;
            load = ((int)(load*1000))/1000.0F;
            CNCData sload("Spindle_Load", load, CNCDataType::tAXIS);
            CNCData sspeed("Spindle_Speed", speed, CNCDataType::tAXIS);
            mDataset.push_back(sload);
            mDataset.push_back(sspeed);
         }
    }
}
/*
void CNC_Fanuc::buildGdata(){
    ODGBCD gdt;
    string G_code = cnc_rdgcode(mHandle,0);
    CNCData Gcode("Gcode", G_code, CNCDataType::tAXIS);
    mDataset.push_back(Gcode);
};*/
void CNC_Fanuc::buildAxisData(){
    ODBDY2 dyn;
    memset(&dyn, 0xEF, sizeof(dyn));
    short ret = cnc_rddynamic2(mHandle, -1, sizeof(dyn), &dyn);

    // current line = dyn.seqnum
    ODBSVLOAD axLoad[MAX_AXIS];
    short num = MAX_AXIS;
    ret = cnc_rdsvmeter(mHandle, &num, axLoad);

	num = 1;
    short types[num] = {1 /* actual position */ };
    short len = MAX_AXIS;

    ODBAXDT axisData[MAX_AXIS * num];
    ret = cnc_rdaxisdata(mHandle, 1 /* Position Value */, (short *) types, num, &len, axisData);
    
    ODBAXISNAME axes[MAX_AXIS];
    ret = cnc_rdaxisname(mHandle, &mNumAxes, axes); // get axes name
  
    short count, inprec[MAX_AXIS], outprec[MAX_AXIS];
    ret = cnc_getfigure(mHandle, 0, &count, inprec, outprec);

	for(int i=0; i<mNumAxes; i++){
		if ((axisData[i].flag & 0x01) != 0){
        	float abspos = dyn.pos.faxis.absolute[i] / pow((long double) 10.0, (long double) inprec[i]);
        	float actpos = dyn.pos.faxis.machine[i] / pow((long double) 10.0, (long double) inprec[i]);
        	float load = axLoad[i].svload.data / pow((long double) 10.0, (long double) axLoad[i].svload.dec);

            abspos = ((int)(abspos*1000))/1000.0F;
            actpos = ((int)(actpos*1000))/1000.0F;
            load = ((int)(load*1000))/1000.0F;

        	CNCData sabspos("AbsPos_" + string(1,axes[i].name), abspos, CNCDataType::tAXIS);
        	CNCData sactpos("ActPos_" + string(1,axes[i].name), actpos, CNCDataType::tAXIS);
        	CNCData sload("Load_" + string(1,axes[i].name), load, CNCDataType::tAXIS);
        	mDataset.push_back(sabspos);
        	mDataset.push_back(sactpos);
        	mDataset.push_back(sload);
		}
	}           
}

void CNC_Fanuc::buildStatData(){
	ODBST buf;	
	// Testando dados gerais do status da máquina em questão
	cnc_statinfo(mHandle, &buf);
	int index, edit_index;

	if(mCNCType=="15"){
		index = 0;
		edit_index = 0;
	} else {
		index = 1;
		edit_index = 1;		
		if (mMachineType == "T" || mMachineType == "MT" || mMachineType == "TT")
		{
			edit_index = 2;
		}
	}

	CNCData auto_stat("Automatic_Mode", aut_msg[index][buf.aut], CNCDataType::tSTATUS);
	mDataset.push_back(auto_stat);	

	CNCData alm_stat("Alarm_Status", alarm_msg[index][buf.alarm], CNCDataType::tSTATUS);
	mDataset.push_back(alm_stat); 

	CNCData run_stat("Automatic_Operation", run_msg[index][buf.run], CNCDataType::tSTATUS);
	mDataset.push_back(run_stat); 	

	CNCData motion_stat("Motion_Status", motion_msg[index][buf.motion], CNCDataType::tSTATUS);
	mDataset.push_back(motion_stat); 	

	CNCData emg_stat("Emergency_Status", emergency_msg[index][buf.emergency], CNCDataType::tSTATUS);
	mDataset.push_back(emg_stat); 	

	CNCData edit_stat("Edit_Status", edit_msg[edit_index][buf.edit], CNCDataType::tSTATUS);
	mDataset.push_back(edit_stat); 	

	ODBST2 status;
    memset(&status, 0, sizeof(status));

    string execution = "";
    string emergency = "";
    string controller = "";

    int ret = cnc_statinfo2(mHandle, &status);
    if (ret == EW_OK) {
        if (status.run == 3 || status.run == 4) // STaRT
            execution = "Active";
        else {
            if (status.run == 2 || status.motion == 2 || status.mstb != 0) // HOLD or motion is Wait
                execution = "Interrupted"; 
            else if (status.run == 0) // STOP
                execution = "Stopped"; 
            else
                execution = "Ready"; 
        }

        // This will take care of JOG
        if (status.aut == 5 || status.aut == 6)
            controller = "Manual"; 
        else if (status.aut == 0 || status.aut == 3) // MDI and EDIT
            controller = "Manual Data Input"; 
        else // Otherwise AUTOMATIC
        	controller = "Automatic";

        if (status.emergency == 1)
        	emergency = "Emergency";
        else
        	emergency = "Armed";
    } 

    CNCData _emergency("Emergency", emergency, CNCDataType::tSTATUS);
	mDataset.push_back(_emergency);
	CNCData _controller("Controller_Mode", controller, CNCDataType::tSTATUS);
	mDataset.push_back(_controller);
	CNCData _execution("Execution_Mode", execution, CNCDataType::tSTATUS);
	mDataset.push_back(_execution);
}

string c2s(char* a, int size){ 
    int i; 
    string s = ""; 
    for (i = 0; i < size; i++) { 
        s = s + a[i]; 
    } 
    return s; 
} 

short CNC_Fanuc::buildSystemData(){
	ODBSYS sysinfo;	
	memset(&sysinfo, 0, sizeof(sysinfo));
	short int ret = cnc_sysinfo(mHandle,&sysinfo);
	
    if(ret== EW_OK){
    	CNCData series("Series",c2s(sysinfo.series,4), CNCDataType::tSYSTEM);
    	CNCData version("Version",c2s(sysinfo.version,4), CNCDataType::tSYSTEM);
    	CNCData axes("Axes",c2s(sysinfo.axes,2), CNCDataType::tSYSTEM);
        CNCData ip("IPAddress", ipAddress, CNCDataType::tSYSTEM);

    	mNumAxes = atoi(sysinfo.axes);
    	mCNCType = c2s(sysinfo.cnc_type,2);
    	mMachineType = c2s(sysinfo.mt_type,2);

    	mDataset.push_back(series);
    	mDataset.push_back(version);
    	mDataset.push_back(axes);
        mDataset.push_back(ip);
    } 
    return ret;
}

short CNC_Fanuc::UpdateData(){
	if(mDataset.size()){
		mDataset.clear();
	}
	
	if(mHandle){
		short int ret = buildSystemData();
		if (ret == EW_OK)
		{
			buildStatData();
			buildAlarmsData();
			buildAxisData();
			buildFeedData();
			buildProgramData();
			buildPMCparams();
			buildPMCalarms();
		}
	}
	return 0;
}

short CNC_Fanuc::UpdateData(string type){
    std::for_each(type.begin(), type.end(), [](char & c) {
        c = ::tolower(c);
    });
    
    QueryType query;
    

    if(type == "all") query = QueryType::eALL;
    else if(type == "status") query = QueryType::eSTATUS;
    else if(type == "alarms") query = QueryType::eALARMS;
    else if(type == "axis") query = QueryType::eAXIS;
    else if(type == "program") query = QueryType::ePROGRAM;
    else if(type == "system") query = QueryType::eSYSTEM;
    else query = QueryType::eALL;
    
    return UpdateData(query);
}

short CNC_Fanuc::UpdateData(QueryType type){
    if(mHandle){
        if(mDataset.size()){
            mDataset.clear();
        }
        short int ret = buildSystemData();
        if(type == QueryType::eSYSTEM){
            return 0;
        } else if (ret == EW_OK && type == QueryType::eSTATUS){
            buildStatData();
            buildPMCparams();
        } else if (ret == EW_OK && type == QueryType::eALARMS){
            buildAlarmsData();
            buildPMCalarms();
        } else if (ret == EW_OK && type == QueryType::eAXIS){
            buildAxisData();
            buildFeedData();
        } else if (ret == EW_OK && type == QueryType::ePROGRAM){
            buildProgramData();
        } else if (ret == EW_OK){
            buildStatData();
            buildAlarmsData();
            buildAxisData();
            buildFeedData();
            buildProgramData();
            buildPMCparams();
            buildPMCalarms();
        }
        return ret;
    }
    return 1;
}

string CNC_Fanuc::dataToString(){
	string str = "";
	for(auto  &elem : mDataset){
		// str += elem.key + " = " + elem.value + "\n";
	}
	return str;
}




json CNC_Fanuc::dataToJSON(){
    json j;
    for(auto &elem: mDataset){
        if(elem.value.type() == typeid(string)){
            string data(any_cast<std::string>(elem.value));
            if(elem.dType == CNCDataType::tALARM){
                j["Alarm"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tAXIS){
                j["Axis"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tSTATUS){
                j["Status"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tPROG){
                j["Program"][elem.key] = data;
            } if(elem.dType == CNCDataType::tSYSTEM){
                j["System"][elem.key] = data;
            }
        } else if(elem.value.type() == typeid(float)){
            float data = any_cast<float>(elem.value);
            if(elem.dType == CNCDataType::tALARM){
                j["Alarm"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tAXIS){
                j["Axis"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tSTATUS){
                j["Status"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tPROG){
                j["Program"][elem.key] = data;
            } if(elem.dType == CNCDataType::tSYSTEM){
                j["System"][elem.key] = data;
            }
        } else if (elem.value.type() == typeid(long int)){
            int data = any_cast<long int>(elem.value);
             if(elem.dType == CNCDataType::tALARM){
                j["Alarm"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tAXIS){
                j["Axis"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tSTATUS){
                j["Status"][elem.key] = data;
            } else if(elem.dType == CNCDataType::tPROG){
                j["Program"][elem.key] = data;
            } if(elem.dType == CNCDataType::tSYSTEM){
                j["System"][elem.key] = data;
            }
        } else { 
            cout << "Nao detectou tipo -- " << elem.key << std::endl;
        }
        
    }
    return j;
}

short CNC_Fanuc::getData(string type, string* data){
    short ret;
    ret = UpdateData(type);
    if(ret == EW_OK)
        *data = dataToString();
    return ret;
}

short CNC_Fanuc::getData(QueryType type, string* data){
    short ret;
    ret = UpdateData(type);
    if(ret == EW_OK)
        *data = dataToString();
    return ret;
}

short CNC_Fanuc::getData(string type, json* data){
    short ret;
    ret = UpdateData(type);
    if(ret == EW_OK)
        *data = dataToJSON();
    return ret;
}

short CNC_Fanuc::getData(QueryType type, json* data){
    short ret;
    ret = UpdateData(type);
    if(ret == EW_OK)
        *data = dataToJSON();
    return ret;
}

CNC_Fanuc::CNC_Fanuc(string ip, int port, int timeout){
    mDataset.clear();
    string log = "fanuc_"+ip+".log";
	short ret = cnc_startupprocess(0, log.c_str());

    if(ret != EW_OK)
        throw CNC_Exception("Could not open log file");
    else {
        ret = cnc_allclibhndl3(ip.c_str(), port, timeout, &mHandle);
        if(ret != EW_OK){
            cnc_exitprocess();
            throw CNC_Exception("Could not connect to IP address: error code: " + std::to_string(ret));
        } else {
            ipAddress.assign(ip);
        }
    }   
}

CNC_Fanuc::~CNC_Fanuc(){
	cnc_exitprocess();
}
/*
bool FanucPath::getToolData(unsigned short aFlibhndl) {
     if (mToolManagementEnabled) {
        ODBTLIFE4 toolId2;
        short ret = cnc_toolnum(aFlibhndl, 0, 0, &toolId2);

        ODBTLIFE3 toolId;
        ret = cnc_rdntool(aFlibhndl, 0, &toolId);
        if (ret == EW_OK && toolId.data != 0) {
            mToolId.setValue(toolId.data);
            mToolGroup.setValue(toolId.datano);
        } else {
            gLogger->warning("Cannot cnc_rdntool for path %d: %d", mPathNumber, ret);
            mToolManagementEnabled = false;
            gLogger->warning("Trying modal tool number", mPathNumber, ret);
            mUseModalToolData = true;
        }
    }
    if (mUseModalToolData) {
        ODBMDL command;
        short ret = cnc_modal(aFlibhndl, 108, 1, &command);
        if (ret == EW_OK) {
            gLogger->debug("cnc_modal returned: datano %d and type %d: %d %X %X",
            command.datano, command.type, command.modal.aux.aux_data, command.modal.aux.flag1,
            command.modal.aux.flag2);
            mToolId.setValue(command.modal.aux.aux_data);
        } else {
            gLogger->warning("cnc_modal failed for T on path %d: %d", mPathNumber, ret);
            mUseModalToolData = false;
        }
    }
    return true;
}
*/
