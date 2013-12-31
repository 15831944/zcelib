/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_server_toolkit.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       2006��3��7��
* @brief      ��������С���߼���,��Ϊ����������ʹ��
*             ���ܰ�����
*             1.���PID
              2.��ط��������ڴ棬CPUռ���ʵ�
* @details    
*             
*             
*             
* @note       
*             
*/

#ifndef ZCE_LIB_SERVER_TOOLKIT_H_
#define ZCE_LIB_SERVER_TOOLKIT_H_

#include "zce_os_adapt_process.h"
#include "zce_os_adapt_sysinfo.h"



/*********************************************************************************
class ZCE_Server_Toolkit
*********************************************************************************/
class ZCELIB_EXPORT ZCE_Server_Toolkit
{
protected:
    //���캯��,˽��,ʹ�õ������ʵ��,
    ZCE_Server_Toolkit();
    ~ZCE_Server_Toolkit();

public:

    //��ʼ��
    int socket_init();

    //��ӡ���PID File
    int out_pid_file(const char *pragramname,
                     bool lock_pid);

    //��ؽ��̵�����״̬
    int watch_dog_status(bool first_record);

    // ��ȡ����״̬ͳ��
    uint32_t get_sys_cpu_ratio(void) const;
    uint32_t get_app_cpu_ratio(void) const;
    uint32_t get_sys_mem_ratio(void) const;
    uint64_t get_can_use_mem_size(void) const;


protected:
    //ÿ�μ�¼�ڴ�й©�Ĵ���
    static const int      MAX_RECORD_MEMORY_NUMBER = 5;

    //�ڴ�й©����ֵ
    static const size_t   MEMORY_LEAK_THRESHOLD  = 300 * 1024 * 1024;

    //����CPU���ʹ���,��ʵ����һ��ĵ��߳�CPU�ﲻ�����ֵ
    static const size_t   PROCESS_CPU_RATIO_THRESHOLD = 600;
    //ϵͳCPU���ʹ���
    static const size_t   SYSTEM_CPU_RATIO_THRESHOLD = 750;

protected:

    //PID �ļ����
    ZCE_HANDLE             pid_handle_;

    //self��PID
    pid_t                  self_pid_;


    //�Ѿ���鵽���ڴ�й©�Ĵ�����ֻ��¼5��
    int                    check_leak_times_;

    //��ʼ��ʱ�򣨻��߼����ʱ���ڴ�ĳߴ�
    size_t                 mem_checkpoint_size_;
    //��ǰ�ڴ�ʹ��
    size_t                 cur_mem_usesize_;

    //���̵�CPU������,ǧ����
    uint32_t               process_cpu_ratio_;
    //ϵͳ��CPU������,ǧ����
    uint32_t               system_cpu_ratio_;

    //�����������õ��ڴ���ڴ��С
    uint64_t               can_use_size_;
    //ϵͳ���ڴ�ʹ����,ǧ����
    uint32_t               mem_use_ratio_;

    //��һ�εĽ�����������
    ZCE_PROCESS_PERFORM    last_process_perf_;
    //��ǰ��һ�εĽ�����������
    ZCE_PROCESS_PERFORM    now_process_perf_;

    //��һ�ε�ϵͳ����������
    ZCE_SYSTEM_PERFORMANCE last_system_perf_;
    //��ǰ��һ�ε�ϵͳ��������
    ZCE_SYSTEM_PERFORMANCE now_system_perf_;


};










#endif //_ZCE_LIB_SERVER_TOOLKIT_H_


