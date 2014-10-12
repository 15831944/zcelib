#include "soar_predefine.h"
#include "soar_error_code.h"
#include "soar_zerg_svc_info.h"
#include "soar_server_ver_define.h"
#include "soar_svrd_app_base.h"
#include "soar_svrd_cfg_fsm.h"


Server_Config_FSM::Server_Config_FSM()
{
}

Server_Config_FSM::~Server_Config_FSM()
{
}

int Server_Config_FSM::read_cfgfile()
{
    // ����zerg ����
    int ret = 0;


    ret = Server_Config_Base::read_cfgfile();
    if (ret != 0)
    {
        return ret;
    }

    // ���ü��سɹ�
    ZLOG_INFO("Comm_Svrd_Config load framework config succ.");
    return 0;
}



