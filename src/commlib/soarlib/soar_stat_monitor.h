#ifndef SOARING_LIB_MONITOR_STAT_H_
#define SOARING_LIB_MONITOR_STAT_H_

#include "soar_zerg_svc_info.h"

//ԭ����ʹ��ģ�����������������ģ�����׽��������󻯣�
//�ĳɶ�̬����
class Comm_Stat_Monitor : public ZCE_Server_Status
{

    // ���mmap�ļ�����󳤶�
    static const size_t STAT_MMAP_FILENAME_LEN = 64;

protected:

    //����stat�ļ�����
    void create_stat_fname(bool if_zerg, const SERVICES_ID &service_info);

public:
    //gunner�����н��ն��cgiͳ���ϱ�������
    //�����޷�֧��
    Comm_Stat_Monitor();

    ~Comm_Stat_Monitor();

    //��ʼ��,����С���ҵ�����������ͬID�Ĺ��棬��������һ��ǰ׺
    int initialize(bool if_zerg,
                   const SERVICES_ID &service_info,
                   size_t num_stat_item,
                   const ZCE_STATUS_ITEM_WITHNAME item_ary[],
                   bool mutli_thread);

    // ��stat�ļ���ȡ��service_info
    static int get_svr_info_from_stat_fname(const char *stat_file_name,
                                            SERVICES_ID *service_info, bool *if_zerg = NULL);

    //���ӵĺ���Ⱥ�������Ҳ�֪��������BOOST��ģ��ʹ�õ��ӣ�����������ֱ����ˬ�����������޸�һЩ
    //�Ҳ���Ϊ�˵��ӿ�����ν�ı������⣬���Լ���֤��ĳ�ʼ��������������
public:
    //
    static Comm_Stat_Monitor *instance();
    //
    static void clean_instance();

protected:
    // ���mmap�ļ���
    char                      stat_mmap_filename_[STAT_MMAP_FILENAME_LEN + 1];

protected:
    //����ʵ��
    static Comm_Stat_Monitor *instance_;
};

#endif //SOARING_LIB_MONITOR_STAT_H_

