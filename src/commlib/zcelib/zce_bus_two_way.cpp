

#include "zce_predefine.h"
#include "zce_shm_lockfree_deque.h"
#include "zce_os_adapt_file.h"
#include "zce_share_mem_mmap.h"
#include "zce_trace_log_debug.h"
#include "zce_bus_mmap_pipe.h"
#include "zce_bus_two_way.h"


ZCE_BusPipe_TwoWay *ZCE_BusPipe_TwoWay::two_way_instance_ = NULL;

const char  ZCE_BusPipe_TwoWay::BUS_PIPE_NAME[NUM_OF_PIPE][16] =
{
    "RECV_PIPE",
    "SEND_PIPE",
};

//���캯��
ZCE_BusPipe_TwoWay::ZCE_BusPipe_TwoWay()
{
}

ZCE_BusPipe_TwoWay::~ZCE_BusPipe_TwoWay()
{
}




/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��4��23��
Function        : ZCE_BusPipe_TwoWay::initialize
Return          : int
Parameter List  :
  Param1: const char *bus_mmap_name
  Param2: size_t size_recv_pipe
  Param3: size_t size_send_pipe
  Param3: size_t max_frame_len
  Param5: bool if_restore �Ƿ���лָ�
Description     : ��ʼ��,ֻ��ʼ�������ܵ���1��1��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int ZCE_BusPipe_TwoWay::initialize(const char *bus_mmap_name,
                                   size_t size_recv_pipe,
                                   size_t size_send_pipe,
                                   size_t max_frame_len,
                                   bool if_restore)
{
    int ret = 0;
    size_t size_of_pipe[NUM_OF_PIPE];
    size_of_pipe[RECV_PIPE_ID] = size_recv_pipe;
    size_of_pipe[SEND_PIPE_ID] = size_send_pipe;

    ret = ZCE_Bus_MMAPPipe::initialize(bus_mmap_name,
                                       NUM_OF_PIPE,
                                       size_of_pipe,
                                       max_frame_len,
                                       if_restore);

    if (ret != 0)
    {
        return ret;
    }

    return 0;
}



//�õ�Ψһ�ĵ���ʵ��
ZCE_BusPipe_TwoWay *ZCE_BusPipe_TwoWay::instance()
{
    if (two_way_instance_ == NULL)
    {
        two_way_instance_ = new ZCE_BusPipe_TwoWay();
    }

    return two_way_instance_;
}

//��ֵΨһ�ĵ���ʵ��
void ZCE_BusPipe_TwoWay::instance(ZCE_BusPipe_TwoWay *pinstatnce)
{
    clean_instance();
    two_way_instance_ = pinstatnce;
    return;
}

//�������ʵ��
void ZCE_BusPipe_TwoWay::clean_instance()
{
    if (two_way_instance_)
    {
        delete two_way_instance_;
    }

    two_way_instance_ = NULL;
    return;
}



