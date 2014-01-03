#include "zce_predefine.h"
#include "zce_shm_lockfree_deque.h"
#include "zce_os_adapt_file.h"
#include "zce_os_adapt_error.h"
#include "zce_share_mem_mmap.h"
#include "zce_trace_log_debug.h"
#include "zce_bus_mmap_pipe.h"

/****************************************************************************************************
class  ZCE_Bus_MMAPPipe::ZCE_BUS_PIPE_HEAD
****************************************************************************************************/
//PIPE HEAD�Ĺ��캯��
ZCE_Bus_MMAPPipe::ZCE_BUS_PIPE_HEAD::ZCE_BUS_PIPE_HEAD():
    size_of_sizet_(sizeof(size_t)),
    number_of_pipe_(0)
{
    memset(size_of_pipe_, 0, sizeof(size_of_pipe_));
    memset(size_of_room_, 0, sizeof(size_of_room_));
}

ZCE_Bus_MMAPPipe::ZCE_BUS_PIPE_HEAD::~ZCE_BUS_PIPE_HEAD()
{
}

/****************************************************************************************************
class  ZCE_Bus_MMAPPipe
****************************************************************************************************/
ZCE_Bus_MMAPPipe *ZCE_Bus_MMAPPipe::instance_ = NULL;

//���캯��
ZCE_Bus_MMAPPipe::ZCE_Bus_MMAPPipe()
{
    bus_mmap_name_[0] = '\0';
    bus_mmap_name_[PATH_MAX] = '\0';

    memset(bus_pipe_pointer_, 0, sizeof(bus_pipe_pointer_));
}

ZCE_Bus_MMAPPipe::~ZCE_Bus_MMAPPipe()
{

    mmap_file_.flush();

    for (size_t i = 0; i < bus_head_.number_of_pipe_; ++i)
    {

        delete bus_pipe_pointer_[i];
        bus_pipe_pointer_[i] = NULL;

        bus_head_.size_of_pipe_[i] = 0;
        bus_head_.size_of_room_[i] = 0;
    }

}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��4��23��
Function        : ZCE_Bus_MMAPPipe::initialize
Return          : int
Parameter List  :
  Param1: const char *bus_mmap_name
  Param2: size_t number_of_pipe �ܵ�������
  Param3: size_t size_of_pipe   �ܵ��Ĵ�С
  Param3: size_t max_frame_len
  Param5: bool if_restore �Ƿ���лָ�
Description     : ��ʼ��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int ZCE_Bus_MMAPPipe::initialize(const char *bus_mmap_name,
                                 size_t number_of_pipe,
                                 size_t size_of_pipe[],
                                 size_t max_frame_len,
                                 bool if_restore)
{

    int ret = 0;

    assert(number_of_pipe > 0);

    if (number_of_pipe == 0 )
    {
        return -1;
    }

    strncpy(bus_mmap_name_, bus_mmap_name, PATH_MAX);
    bus_head_.number_of_pipe_ = number_of_pipe;

    for (size_t i = 0; i < bus_head_.number_of_pipe_; ++i)
    {
        bus_head_.size_of_pipe_[i] = size_of_pipe[i];
    }


    //�Ƿ�Թܵ����м���

    //Malloc������,

    //������ָ�,�ɴ�ɾ��ԭ�е�MMAP�ļ�,����ʹ�õ�ʱ���������.
    if ( if_restore == false )
    {
        ZCE_OS::unlink(bus_mmap_name_);
    }
    //���û������ļ�,��ôֻ���ؽ�
    else
    {
        zce_os_stat mmapfile_stat;
        ret = ZCE_OS::stat(bus_mmap_name_, &mmapfile_stat);

        if (ret != 0 )
        {
            if_restore = false;
        }
    }

    size_t sz_malloc = 0;
    sz_malloc += sizeof (ZCE_BUS_PIPE_HEAD);

    for (size_t i = 0; i < bus_head_.number_of_pipe_; ++i)
    {
        size_t sz_room = ZCE_LIB::shm_dequechunk::getallocsize(bus_head_.size_of_pipe_[i]);
        bus_head_.size_of_room_[i] = sz_room;
        sz_malloc += sz_room;
    }


    //���������ڴ�Ĳ�����ʽ

    //MAPһ���ļ�
    ret = mmap_file_.open(bus_mmap_name_,
                          sz_malloc,
                          if_restore);

    if (0  != ret)
    {
        ZLOG_ERROR("[zcelib] MMAP map a file (%s) to share memory fail,ret =%d, last error=%d|%s.",
                   bus_mmap_name_,
                   ret,
                   ZCE_OS::last_error(),
                   strerror(ZCE_OS::last_error()) );
        return -1;
    }



    if (if_restore)
    {
        ZCE_BUS_PIPE_HEAD *pipe_head = static_cast<ZCE_BUS_PIPE_HEAD *>( mmap_file_.addr() );

        //���ڸ��ֳ��Ƚ��м��
        if (pipe_head->size_of_sizet_ != bus_head_.size_of_sizet_
            || pipe_head->number_of_pipe_ != bus_head_.number_of_pipe_)
        {
            ZLOG_ERROR("[zcelib] ZCE_Bus_MMAPPipe::initialize pipe fail. ZCE_BUS_PIPE_HEAD old size_t_len[%u] numpipe[%u],new size_t_len[%u],numpipe[%u] ",
                       pipe_head->size_of_sizet_,
                       pipe_head->number_of_pipe_,
                       bus_head_.size_of_sizet_,
                       bus_head_.number_of_pipe_);
            return -1;
        }

        for (size_t i = 0; i < bus_head_.number_of_pipe_; ++i)
        {
            if (pipe_head->size_of_pipe_[i] != bus_head_.size_of_pipe_[i]
                || pipe_head->size_of_room_[i] != bus_head_.size_of_room_[i])
            {
                ZLOG_ERROR("[zcelib] ZCE_Bus_MMAPPipe::initialize pipe fail. ZCE_BUS_PIPE_HEAD <%u> old size_t_len[%u] numpipe[%u],new size_t_len[%u],numpipe[%u] .",
                           i,
                           pipe_head->size_of_pipe_[i],
                           pipe_head->size_of_room_[i],
                           bus_head_.size_of_pipe_[i],
                           bus_head_.size_of_room_[i]);
                return -1;
            }
        }
    }

    memcpy(mmap_file_.addr(), &bus_head_, sizeof(ZCE_BUS_PIPE_HEAD));

    //��ʼ�����еĹܵ�
    ret = init_all_pipe(max_frame_len, if_restore);

    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

//��ʼ����ֻ�����ļ����г�ʼ��������ĳЩ���߶�MMAP�ļ����д�����ʱ��
//size_t max_frame_len�����е����ᣬ�������������������ײ�ܶ����Ҫ�ģ�
//���Ҷ���һ����Ŀ�����ֵӦ��Ӧ����һ������
int ZCE_Bus_MMAPPipe::initialize(const char *bus_mmap_name,
                                 size_t max_frame_len)
{
    int ret = 0;

    strncpy(bus_mmap_name_, bus_mmap_name, PATH_MAX);

    zce_os_stat mmapfile_stat;
    ret = ZCE_OS::stat(bus_mmap_name_, &mmapfile_stat);

    if (ret != 0 )
    {
        return -1;
    }

    if ((size_t)mmapfile_stat.st_size <= sizeof(ZCE_BUS_PIPE_HEAD) )
    {
        return -1;
    }

    //MAPһ���ļ�
    ret = mmap_file_.open(bus_mmap_name_,
                          static_cast<size_t>(mmapfile_stat.st_size),
                          true);

    if (ret != 0)
    {
        ZLOG_ERROR("[zcelib] MMAP map a file (%s) to share memory fail,ret =%d, last error=%d|%s.",
                   bus_mmap_name_,
                   ret,
                   ZCE_OS::last_error(),
                   strerror(ZCE_OS::last_error()) );
        return -1;
    }

    ZCE_BUS_PIPE_HEAD *pipe_head = static_cast<ZCE_BUS_PIPE_HEAD *>( mmap_file_.addr() );
    bus_head_ = *pipe_head;

    //��ʼ�����еĹܵ�
    ret = init_all_pipe(max_frame_len, true);

    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

//��ʼ�����е����ݹܵ�
int ZCE_Bus_MMAPPipe::init_all_pipe(size_t max_frame_len,
                                    bool if_restore)
{
    size_t file_offset = 0;
    //ƫ��һ��ͷ��
    file_offset = sizeof(ZCE_BUS_PIPE_HEAD);

    //ѭ����ʼ��ÿ��PIPE
    for (size_t i = 0; i < bus_head_.number_of_pipe_; ++i)
    {
        char *pt_pipe = static_cast<char *>( mmap_file_.addr() ) + file_offset ;

        //��ʼ���ڴ�
        bus_pipe_pointer_[i] = ZCE_LIB::shm_dequechunk::initialize(bus_head_.size_of_pipe_[i],
                                                                   max_frame_len,
                                                                   pt_pipe,
                                                                   if_restore
                                                                  );

        //�ܵ������Լ�Ҳ�����Ƿ��ָܻ�
        if (bus_pipe_pointer_[i] == NULL)
        {
            ZLOG_ERROR("[zcelib] ZCE_Bus_MMAPPipe::initialize pipe[%u] size[%u] room[%u] fail.",
                       i,
                       bus_head_.size_of_pipe_[i],
                       bus_head_.size_of_room_[i]);
            return -1;
        }

        ZCE_ASSERT( bus_pipe_pointer_[i] != NULL );

        size_t sz_room = ZCE_LIB::shm_dequechunk::getallocsize(bus_head_.size_of_pipe_[i]);
        file_offset += sz_room;
    }

    return 0;
}

//�õ�Ψһ�ĵ���ʵ��
ZCE_Bus_MMAPPipe *ZCE_Bus_MMAPPipe::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new ZCE_Bus_MMAPPipe();
    }

    return instance_;
}

//��ֵΨһ�ĵ���ʵ��
void ZCE_Bus_MMAPPipe::instance(ZCE_Bus_MMAPPipe *pinstatnce)
{
    clean_instance();
    instance_ = pinstatnce;
    return;
}

//�������ʵ��
void ZCE_Bus_MMAPPipe::clean_instance()
{
    if (instance_)
    {
        delete instance_;
    }

    instance_ = NULL;
    return;
}


