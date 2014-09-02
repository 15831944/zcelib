#include "soar_predefine.h"
#include "soar_stat_monitor.h"
#include "soar_error_code.h"
#include "soar_stat_define.h"

/******************************************************************************************
class Comm_Stat_Monitor ���̰߳汾��ʵ��
******************************************************************************************/
Comm_Stat_Monitor *Comm_Stat_Monitor::instance_ = NULL;

Comm_Stat_Monitor *Comm_Stat_Monitor::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new Comm_Stat_Monitor();
    }

    return instance_;
}

void Comm_Stat_Monitor::clean_instance()
{
    delete instance_;
    instance_ = NULL;
}

//��ʼ��,����С���ҵ�����������ͬID�Ĺ��棬��������һ��ǰ׺
int Comm_Stat_Monitor::initialize(bool if_zerg,
                                  const SERVICES_ID &service_info,
                                  size_t num_stat_item,
                                  const ZCE_STATUS_ITEM_WITHNAME item_ary[],
                                  bool mutli_thread)
{
    create_stat_fname(if_zerg, service_info);

    // ��stat_mmap_filename_ת��Ϊ��д
    ZCE_LIB::strupr(stat_mmap_filename_);

    int ret = ZCE_Server_Status::initialize(stat_mmap_filename_,
                                            num_stat_item,
                                            item_ary,
                                            mutli_thread);

    return ret;
}

//����stat�ļ�����
void Comm_Stat_Monitor::create_stat_fname(bool if_zerg,
                                          const SERVICES_ID &service_info)
{
    //�����Ƿ���С��õ��ļ�����
    if (if_zerg)
    {
        snprintf(stat_mmap_filename_, STAT_MMAP_FILENAME_LEN, "ZERG_%d_%d.STAT",
                 service_info.services_type_,
                 service_info.services_id_);
    }
    else
    {
        snprintf(stat_mmap_filename_, STAT_MMAP_FILENAME_LEN, "TSS_%d_%d.STAT",
                 service_info.services_type_,
                 service_info.services_id_);
    }

    stat_mmap_filename_[STAT_MMAP_FILENAME_LEN] = '\0';
}

Comm_Stat_Monitor::Comm_Stat_Monitor()
{
}

Comm_Stat_Monitor::~Comm_Stat_Monitor()
{
}

int Comm_Stat_Monitor::get_svr_info_from_stat_fname(const char *stat_file_name,
                                                    SERVICES_ID *service_info,
                                                    bool *if_zerg)
{
    ZCE_ASSERT(stat_file_name != NULL);
    ZCE_ASSERT(service_info != NULL);
    bool tmp_if_zerg = false;
    const char *scan_format = NULL;
    SERVICES_ID tmp_service_info;

    if (strncmp(stat_file_name, "ZERG", 4) == 0)
    {
        tmp_if_zerg = true;
        scan_format = "ZERG_%d_%d.STAT";
    }
    else if (strncmp(stat_file_name, "TSS", 3) == 0)
    {
        tmp_if_zerg = false;
        scan_format = "TSS_%d_%d.STAT";
    }
    else
    {
        // ������ļ�����
        return SOAR_RET::ERROR_BAD_STAT_FILE_NAME;
    }

    int ret = sscanf(stat_file_name, scan_format, &tmp_service_info.services_type_,
                     &tmp_service_info.services_id_);

    if (ret != 2)
    {
        // �ļ�������
        return SOAR_RET::ERROR_BAD_STAT_FILE_NAME;
    }

    if (if_zerg != NULL)
    {
        *if_zerg = tmp_if_zerg;
    }

    *service_info = tmp_service_info;

    return SOAR_RET::SOAR_RET_SUCC;
}

