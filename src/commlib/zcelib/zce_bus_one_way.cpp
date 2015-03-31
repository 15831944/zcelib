
#include "zce_predefine.h"
#include "zce_lockfree_kfifo.h"
#include "zce_os_adapt_file.h"
#include "zce_share_mem_mmap.h"
#include "zce_trace_log_debug.h"
#include "zce_bus_mmap_pipe.h"
#include "zce_bus_one_way.h"

ZCE_BusPipe_OneWay *ZCE_BusPipe_OneWay::one_way_instance_ = NULL;

//���캯��
ZCE_BusPipe_OneWay::ZCE_BusPipe_OneWay()
{
}

ZCE_BusPipe_OneWay::~ZCE_BusPipe_OneWay()
{
}

//�õ�Ψһ�ĵ���ʵ��
ZCE_BusPipe_OneWay *ZCE_BusPipe_OneWay::instance()
{
    if (one_way_instance_ == NULL)
    {
        one_way_instance_ = new ZCE_BusPipe_OneWay();
    }

    return one_way_instance_;
}

//��ֵΨһ�ĵ���ʵ��
void ZCE_BusPipe_OneWay::instance(ZCE_BusPipe_OneWay *pinstatnce)
{
    clean_instance();
    one_way_instance_ = pinstatnce;
    return;
}

//�������ʵ��
void ZCE_BusPipe_OneWay::clean_instance()
{
    if (one_way_instance_)
    {
        delete one_way_instance_;
    }

    one_way_instance_ = NULL;
    return;
}

