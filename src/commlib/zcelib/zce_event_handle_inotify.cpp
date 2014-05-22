#include "zce_predefine.h"


#if defined (ZCE_OS_LINUX)

#include "zce_os_adapt_predefine.h"
#include "zce_os_adapt_file.h"
#include "zce_os_adapt_error.h"
#include "zce_event_handle_inotify.h"
#include "zce_trace_log_debug.h"
#include "zce_event_reactor_base.h"



//���캯������������
ZCE_Event_INotify::ZCE_Event_INotify():
    ZCE_Event_Handler(),
    inotify_handle_(ZCE_INVALID_SOCKET),
    read_buffer_(NULL)
{
    //�������LINUX������ʹ�ã������㿴��
#if !defined ZCE_OS_LINUX
    ZCE_ASSERT_ALL(false);
#endif
     
    read_buffer_ = new char [READ_BUFFER_LEN + 1];
    read_buffer_[READ_BUFFER_LEN] = '\0';
}

ZCE_Event_INotify::~ZCE_Event_INotify()
{
    if (read_buffer_)
    {
        delete []read_buffer_;
        read_buffer_ = NULL;
    }
}

//�򿪼�ؾ���ȣ���reactor��
int ZCE_Event_INotify::open(ZCE_Reactor *reactor_base)
{
    int ret = 0;
    
    //����Ѿ���ʼ������
    if (ZCE_INVALID_HANDLE != inotify_handle_ )
    {
        return -1;
    }

    //��LINUX��ʹ��INOTIFY�Ļ���
    inotify_handle_ = ::inotify_init();
    if (ZCE_INVALID_HANDLE == inotify_handle_ )
    {
        ZLOG_ERROR("[%s] invoke ::inotify_init fail,error [%u].",
            __ZCE_FUNCTION__,
            ZCE_OS::last_error());
        return -1;
    }

    reactor(reactor_base);
    ret = reactor()->register_handler(this,READ_MASK);
    if (ret != 0)
    {
        ::close(inotify_handle_);
        return ret;
    }

    return 0;
}


//�رռ�ؾ���ȣ������reactor��
int ZCE_Event_INotify::close()
{
    int ret = 0;

    //������HASH MAP�ٶ��е���
    HDL_TO_EIN_MAP::iterator iter_temp =  watch_event_map_.begin();
    for (; iter_temp != watch_event_map_.end();)
    {
        rm_watch(iter_temp->second.watch_handle_);
        //�õ������������ʼ����
        iter_temp = watch_event_map_.begin();
    }

    //�رռ�ؾ��
    if (inotify_handle_ != ZCE_INVALID_HANDLE)
    {
        //�ӷ�Ӧ���Ƴ�
        ret = reactor()->remove_handler(this,false);
        if (ret != 0)
        {
            return ret;
        }

        ::close(inotify_handle_);
        inotify_handle_ = ZCE_INVALID_HANDLE;
    }

    return 0;
}



//��Ӽ��                                 
int ZCE_Event_INotify::add_watch(const char *pathname,
                                 uint32_t mask,
                                 ZCE_HANDLE *watch_handle)
{

    //�������Ƿ���Ч��������Ƿ��Ѿ���ʼ��
    ZCE_ASSERT( pathname
        &&  mask 
        && inotify_handle_ != ZCE_INVALID_HANDLE );

    if ( pathname == NULL
        || mask == 0
        ||  inotify_handle_ == ZCE_INVALID_HANDLE )
    {
        errno = EINVAL;
        return -1;
    }

    *watch_handle = ZCE_INVALID_HANDLE;
    EVENT_INOTIFY_NODE watch_note;
    ZCE_HANDLE hdl_dir = ZCE_INVALID_HANDLE;

    hdl_dir =  ::inotify_add_watch(inotify_handle_, pathname, mask);
    if (hdl_dir == ZCE_INVALID_HANDLE )
    {
        ZLOG_ERROR("[%s] invoke ::inotify_add_watch fail,error [%u].",
            __ZCE_FUNCTION__,
            ZCE_OS::last_error());
        return -1;
    }
    
    watch_note.watch_handle_ = hdl_dir;
    watch_note.watch_mask_ = mask;
    strncpy(watch_note.watch_path_, pathname, MAX_PATH);

    std::pair<HDL_TO_EIN_MAP::iterator, bool>
        ins_ret = watch_event_map_.insert(HDL_TO_EIN_MAP::value_type(hdl_dir, watch_note));

    //������벻�ɹ������и����ѹ�������
    if (ins_ret.second == false)
    {

        //������δ������ε�ԭ���ǣ���LInux�£����inotify_add_watch ͬһ��Ŀ¼��handle��һ���ġ�
        //::inotify_rm_watch(inotify_handle_, hdl_dir);

        ZLOG_ERROR("[%s] insert code node to map fail. code error or map already haved one equal HANDLE[%u].",
            __ZCE_FUNCTION__,
            hdl_dir);
        return -1;
    }

    return 0;
}


int ZCE_Event_INotify::rm_watch(ZCE_HANDLE watch_handle)
{

    //���þ����ѯ
    HDL_TO_EIN_MAP::iterator iter_del = watch_event_map_.find(watch_handle);
    if (iter_del == watch_event_map_.end())
    {
        return -1;
    }

    int ret =  ::inotify_rm_watch(inotify_handle_, iter_del->second.watch_handle_);
    if (ret != 0)
    {
        return -1;
    }

    //��MAP��ɾ�����NODe
    watch_event_map_.erase(iter_del);
    return 0;
}


//��ȡ�¼��������ú���
int ZCE_Event_INotify::handle_input ()
{
    ZCE_LOGMSG(RS_DEBUG, "ZCE_Event_INotify::handle_input");
    int detect_ret = 0;
    size_t watch_event_num = 0;

    //��ȡ
    ssize_t read_ret = ZCE_OS::read(inotify_handle_, read_buffer_, READ_BUFFER_LEN);
    if (read_ret <= 0)
    {
        return -1;
    }
    
    uint32_t read_len = static_cast<uint32_t>(read_ret);
    uint32_t next_entry_offset = 0;

    //����һ�ζ�ȡ�������inotify_event���ݣ�����Ҫѭ������
    do
    {
        detect_ret = 0;

        ::inotify_event *ne_ptr = (::inotify_event *) (read_buffer_ + next_entry_offset);

        //����ȡ�������Ƿ���һ����
        read_len -= (sizeof(::inotify_event) + ne_ptr->len);
        next_entry_offset += sizeof(::inotify_event) + ne_ptr->len;

        HDL_TO_EIN_MAP::iterator active_iter = watch_event_map_.find(ne_ptr->wd);
        if (active_iter == watch_event_map_.end())
        {
            //ĳ��FD��MAP�м��޷��ҵ������Ŀ�����
            ZCE_LOGMSG(RS_DEBUG, 
                "You code error or a handle not in map (delete in this do while), please check you code. handle[%u]", 
                ne_ptr->wd);
            continue;
        }
        EVENT_INOTIFY_NODE *node_ptr = &(active_iter->second);
        const char *active_path = ne_ptr->name;
        //���ݷ��ص�mask������δ���

        //ע������Ĵ����֧�õ�if else if ,������if if���ҵĳ�����������Щ�¼�����һ�𴥷�����Ҳ���ԡ�
        //����5���Ǻ�Windows ���еģ�
        uint32_t event_mask = ne_ptr->mask;
        if (event_mask & IN_CREATE )
        {
            detect_ret = inotify_create(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if (event_mask & IN_DELETE  ) 
        {
            detect_ret = inotify_delete(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if ( event_mask & IN_MODIFY )
        {
            detect_ret = inotify_modify(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if ( event_mask & IN_MOVED_FROM)
        {
            detect_ret = inotify_moved_from(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if ( event_mask & IN_MOVED_TO)
        {
            detect_ret = inotify_moved_to(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        //������Щ��LINUX�Լ����е�
        else if ( event_mask & IN_ACCESS)
        {
            detect_ret = inotify_access(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if (event_mask | IN_OPEN)
        {
            detect_ret = inotify_open(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if (event_mask | IN_CLOSE_WRITE || event_mask | IN_CLOSE_NOWRITE)
        {
            detect_ret = inotify_close(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if (event_mask | IN_ATTRIB)
        {
            detect_ret = inotify_attrib(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if (event_mask | IN_MOVE_SELF)
        {
            detect_ret = inotify_move_slef(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }
        else if (event_mask | IN_DELETE_SELF)
        {
            detect_ret = inotify_delete_slef(node_ptr->watch_handle_,
                event_mask,
                node_ptr->watch_path_, 
                active_path);
        }

        //����-1���ر�֮,
        if (detect_ret == -1)
        {
            rm_watch(node_ptr->watch_handle_);
        }

        //�ۼƷ����¼�����
        ++(watch_event_num);

    }
    while (read_len > 0);

    return 0;
}


//�رռ�ؾ��
int ZCE_Event_INotify::handle_close ()
{
    return close();
}




#endif //#if defined (ZCE_OS_LINUX)


