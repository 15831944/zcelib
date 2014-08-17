#include "zce_predefine.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_socket.h"
#include "zce_os_adapt_file.h"
#include "zce_trace_log_debug.h"
#include "zce_inotify_event_base.h"
#include "zce_inotify_directory_reactor.h"


ZCE_INotify_Dir_Reactor::ZCE_INotify_Dir_Reactor(size_t max_watch_dir)
{

#if defined (ZCE_OS_WINDOWS)
    if ( max_watch_dir > MAXIMUM_WAIT_OBJECTS )
    {
        max_watch_dir =  MAXIMUM_WAIT_OBJECTS;
    }
    memset(watch_handle_ary_, 0, sizeof(watch_handle_ary_));
#elif defined (ZCE_OS_LINUX)
    inotify_handle_ = ZCE_INVALID_HANDLE;
    read_buffer_ = NULL;
#endif

    watch_event_map_.rehash(max_watch_dir);
}


//�������������Զ���������Դ�������Ҫ������Դ�������close������
ZCE_INotify_Dir_Reactor::~ZCE_INotify_Dir_Reactor()
{
}



//
int ZCE_INotify_Dir_Reactor::open()
{
#if defined (ZCE_OS_LINUX)

    //��LINUX��ʹ��INOTIFY�Ļ���
    inotify_handle_ = ::inotify_init();
    if (ZCE_INVALID_HANDLE == inotify_handle_ )
    {
        return -1;
    }
    read_buffer_ = new char [READ_BUFFER_LEN + 1];
    read_buffer_[READ_BUFFER_LEN] = '\0';
#endif

    return 0;
}

//�ر��ļ���ط�Ӧ��
int ZCE_INotify_Dir_Reactor::close()
{


    //������HASH MAP�ٶ��е���
    HANDLE_TO_EVENT_MAP::iterator iter_temp =  watch_event_map_.begin();
    for (; iter_temp != watch_event_map_.end();)
    {
        //�ر�֮
        ZCE_INotify_Event_Base *event_handler = (iter_temp->second).watch_event_;

        //��handle_close,
        event_handler->watch_close();

        //�õ������������ʼ����
        iter_temp = watch_event_map_.begin();
    }

    //�������������������ɣ����е����ݶ��ᱻ����

    return 0;

#if defined (ZCE_OS_LINUX)

    if (inotify_handle_ != ZCE_INVALID_HANDLE)
    {
        ::close(inotify_handle_);
        inotify_handle_ = ZCE_INVALID_HANDLE;
    }
    if (read_buffer_)
    {
        delete []read_buffer_;
        read_buffer_ = NULL;
    }

    return 0;

#endif

}


//����һ��Ҫ���м�ص��ļ�����
int ZCE_INotify_Dir_Reactor::add_watch(ZCE_INotify_Event_Base *event_base,
                                       const char *pathname,
                                       uint32_t mask,
                                       ZCE_HANDLE *watch_handle,
                                       bool watch_sub_dir)
{
    //�������Ƿ���Ч
    ZCE_ASSERT(event_base
               &&  pathname
               &&  mask
               && (NULL == event_base->inotify_reactor_ || this == event_base->inotify_reactor_) );

    if (event_base == NULL
        || pathname == NULL
        || mask == 0
        || (NULL != event_base->inotify_reactor_ && this != event_base->inotify_reactor_) )
    {
        errno = EINVAL;
        return -1;
    }

    *watch_handle = ZCE_INVALID_HANDLE;
    ZCE_INOTIFY_NODE watch_note;
    ZCE_HANDLE hdl_dir = ZCE_INVALID_HANDLE;

#if defined (ZCE_OS_WINDOWS)
    size_t watch_ary_size = watch_event_map_.size();
    //Windows��ֻ�ܴ���MAXIMUM_WAIT_OBJECTS��Ŀ¼���
    if ( watch_ary_size >= MAXIMUM_WAIT_OBJECTS )
    {
        ZLOG_ERROR("[%s] in Windows ,watch handle number must less than"
            " MAXIMUM_WAIT_OBJECTS,please check you code.",
            __ZCE_FUNCTION__);
        return -1;
    }

    event_base->inotify_reactor_ = this;



    //����OVERLAPPED�ķ�ʽ���ļ���ע��FILE_LIST_DIRECTORY��FILE_FLAG_OVERLAPPED
    hdl_dir = ::CreateFileA( pathname, // pointer to the file name
                             FILE_LIST_DIRECTORY,                // access (read/write) mode
                             FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // share mode
                             NULL,                               // security descriptor
                             OPEN_EXISTING,                      // how to create
                             FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,       // file attributes
                             NULL                                // file with attributes to copy
                           );

    if (hdl_dir == ZCE_INVALID_HANDLE)
    {
        ZLOG_ERROR("ZCE_INotify_Dir_Reactor::add_watch invoke ::CreateFile [%s] fail,error [%u].",
                   pathname,
                   ZCE_OS::last_error());
        return -1;
    }

    watch_note.read_buffer_ = new char [READ_BUFFER_LEN + 1];
    watch_note.read_buffer_[READ_BUFFER_LEN] = '\0';
    watch_note.watch_sub_dir_ = watch_sub_dir;

#elif defined (ZCE_OS_LINUX)

    //Linux ��֧�ּ����Ŀ¼
    ZCE_ASSERT(watch_sub_dir == false);
    if (watch_sub_dir)
    {
        errno = EINVAL;
        ZLOG_ERROR("[%s] in linux ,watch sub dir is not support,please check you code.", __ZCE_FUNCTION__);
        return -1;
    }

    ZCE_ASSERT(inotify_handle_ != ZCE_INVALID_HANDLE);
    hdl_dir =  ::inotify_add_watch(inotify_handle_, pathname, mask);
    if (hdl_dir == -1)
    {
        ZLOG_ERROR("[%s] invoke ::inotify_add_watch fail,error [%u].",
                   __ZCE_FUNCTION__,
                   ZCE_OS::last_error());
        return -1;
    }
    //

#endif

    watch_note.watch_handle_ = hdl_dir;
    watch_note.watch_mask_ = mask;
    strncpy(watch_note.watch_path_, pathname, MAX_PATH);
    watch_note.watch_event_ = event_base;

    std::pair<HANDLE_TO_EVENT_MAP::iterator, bool>
    ins_ret = watch_event_map_.insert(HANDLE_TO_EVENT_MAP::value_type(hdl_dir, watch_note));

    //������벻�ɹ������и����ѹ�������
    if (ins_ret.second == false)
    {

#if defined (ZCE_OS_WINDOWS)
        ::CloseHandle(hdl_dir);
        delete [] watch_note.read_buffer_;
        watch_note.read_buffer_ = NULL;
#endif

        //������δ������ε�ԭ���ǣ�LInux�£����inotify_add_watch ͬһ��Ŀ¼��handle��һ���ġ�
        //#if defined (ZCE_OS_LINUX)
        //        ::inotify_rm_watch(inotify_handle_, hdl_dir);
        //#endif

        //��������־��ӡ���ܻᶪʧ����,��ΪHANDLE��ָ�룬����ʱ����ˣ�
        ZLOG_ERROR("[%s] insert code node to map fail. code error or map already haved one equal HANDLE[%u].",
                   __ZCE_FUNCTION__,
                   hdl_dir);
        return -1;
    }


#if defined (ZCE_OS_WINDOWS)
    //û���ӣ��ʺ�Windows API�������ĸ��������ȼ�����е�NOTIFY�ѡ�
    DWORD bytes_returned = 0;
    BOOL bret = ::ReadDirectoryChangesW(
                    hdl_dir,                                  // handle to directory
                    watch_note.read_buffer_,                                    // read results buffer
                    READ_BUFFER_LEN,                               // length of buffer
                    watch_note.watch_sub_dir_,                                 // monitoring option
                    FILE_NOTIFY_CHANGE_SECURITY |
                    FILE_NOTIFY_CHANGE_CREATION |
                    FILE_NOTIFY_CHANGE_LAST_ACCESS |
                    FILE_NOTIFY_CHANGE_LAST_WRITE |
                    FILE_NOTIFY_CHANGE_SIZE |
                    FILE_NOTIFY_CHANGE_ATTRIBUTES |
                    FILE_NOTIFY_CHANGE_DIR_NAME |
                    FILE_NOTIFY_CHANGE_FILE_NAME,          // filter conditions
                    &bytes_returned,                       // bytes returned
                    & ((ins_ret.first)->second.over_lapped_),   // overlapped buffer
                    NULL // completion routine
                );

    //�����ȡʧ�ܣ�һ����ԣ�������δ���������
    if (bret == FALSE)
    {
        ZLOG_ERROR("[%s] ::ReadDirectoryChangesW fail,error [%u].",
                   __ZCE_FUNCTION__,
                   ZCE_OS::last_error());
        delete [] (ins_ret.first)->second.read_buffer_;
        (ins_ret.first)->second.read_buffer_ = NULL;
        ::CloseHandle(hdl_dir);

        watch_event_map_.erase(ins_ret.first);

        return -1;
    }

    //�ڼ����������Ӽ�ص���Ϣ
    watch_handle_ary_[watch_ary_size] = hdl_dir;
#endif

    *watch_handle = hdl_dir;

    return 0;
}

//�Ƴ�һ����صľ��
int ZCE_INotify_Dir_Reactor::rm_watch(ZCE_HANDLE watch_handle)
{
    //���þ����ѯ
    HANDLE_TO_EVENT_MAP::iterator iter_del = watch_event_map_.find(watch_handle);
    if (iter_del == watch_event_map_.end())
    {
        return -1;
    }

    //������������
#if defined (ZCE_OS_WINDOWS)

    delete iter_del->second.read_buffer_;
    iter_del->second.read_buffer_ = NULL;
    ::CloseHandle(iter_del->second.watch_handle_);
    iter_del->second.watch_handle_ = ZCE_INVALID_HANDLE;

    size_t watch_ary_size = watch_event_map_.size();
    //������������������
    for (size_t i = 0; i < watch_ary_size; ++i)
    {
        if (watch_handle_ary_[i] == watch_handle && i < watch_ary_size)
        {
            //�����һ����ض����ƶ���ɾ����λ��
            watch_handle_ary_[i] = watch_handle_ary_[watch_ary_size - 1];
            break;
        }
    }

#elif defined (ZCE_OS_LINUX)
    int ret =  ::inotify_rm_watch(inotify_handle_, iter_del->second.watch_handle_);
    if (ret != 0)
    {
        return -1;
    }
#endif

    //��MAP��ɾ�����NODe
    watch_event_map_.erase(iter_del);

    return 0;
}

//ȡ�������ZCE_INotify_Event_Base��ע������Ŀ¼�ļ��
int ZCE_INotify_Dir_Reactor::rm_watch(const ZCE_INotify_Event_Base *event_base)
{
    int del_num = 0;
    HANDLE_TO_EVENT_MAP::iterator iter_temp =  watch_event_map_.begin();
    for (; iter_temp != watch_event_map_.end();)
    {

        ZCE_INotify_Event_Base *event_handler = (iter_temp->second).watch_event_;
        if (event_handler == event_base )
        {
            rm_watch((iter_temp->second).watch_handle_);
            del_num ++;
            //�õ������������ʼ����
            iter_temp = watch_event_map_.begin();
        }
    }

    //û��ɾ�����κ�NODE�����ش���
    if (0 == del_num )
    {
        return 0;
    }
    return 0;
}

//
int ZCE_INotify_Dir_Reactor::watch_event(ZCE_Time_Value *time_out, size_t *watch_event_num)
{
    *watch_event_num = 0;
    int detect_ret = 0;

#if defined (ZCE_OS_WINDOWS)

    DWORD wait_msec = static_cast<DWORD>( time_out->total_msec());
    wait_msec = 0;
    DWORD watch_ary_size = static_cast<DWORD>(watch_event_map_.size());
    DWORD wait_status = WaitForMultipleObjects(watch_ary_size,
                                               watch_handle_ary_,
                                               FALSE,
                                               INFINITE);

    if ( wait_status == WAIT_FAILED )
    {
        ZLOG_ERROR("[%s] ::WaitForMultipleObjects fail,error [%u].",
                   __ZCE_FUNCTION__,
                   ZCE_OS::last_error());
        return -1;
    }

    //����ǳ�ʱ
    if ( wait_status == WAIT_TIMEOUT )
    {
        errno = ETIMEDOUT;
        return -1;
    }

    size_t activate_id = wait_status - WAIT_OBJECT_0;
    HANDLE_TO_EVENT_MAP::iterator active_iter = watch_event_map_.find(watch_handle_ary_[activate_id]);
    if (active_iter == watch_event_map_.end())
    {
        ZLOG_ALERT("You code error,a handle not in map, please check you code. handle[%u]", watch_handle_ary_[activate_id]);
        return -1;
    }

    ZCE_INOTIFY_NODE *node_ptr = &(active_iter->second);

    DWORD num_read = 0;

    BOOL bret = ::GetOverlappedResult(watch_handle_ary_[activate_id],
                                      &(node_ptr->over_lapped_),
                                      &num_read,
                                      FALSE);

    //��ȡ���ʧ��
    if (FALSE == bret )
    {
        ZLOG_ERROR("[%s] ::GetOverlappedResult fail,error [%u].",
                   __ZCE_FUNCTION__,
                   ZCE_OS::last_error());
        return -1;
    }


    //��¼��ǰ����ľ����
    ZCE_HANDLE cur_proc_handle = watch_handle_ary_[activate_id];

    FILE_NOTIFY_INFORMATION *read_ptr = NULL;
    DWORD next_entry_offset = 0;
    do
    {
        detect_ret = 0;
        read_ptr = (FILE_NOTIFY_INFORMATION *) (node_ptr->read_buffer_ + next_entry_offset);

        //�ļ����ƽ���ת��,�ӿ��ֽ�ת��Ϊ���ֽ�,
        //��ɱ��Windows����Щ�ط�����������FILE_NOTIFY_INFORMATION����ĳ���FileNameLength���ֽڳ���
        //��WideCharToMultiByte������Ҫ�ĳ������ֳ��ȣ�����TMDͳһһ����
        int length_of_ws = ::WideCharToMultiByte( CP_ACP,
                                                  0,
                                                  read_ptr->FileName ,
                                                  read_ptr->FileNameLength / sizeof(wchar_t),
                                                  NULL,
                                                  0,
                                                  NULL,
                                                  NULL);
        //Windows ��Ŀ¼������󳤶ȿ��Ե�3K����û����Ȥȥ��һ�������
        if (length_of_ws >= MAX_PATH)
        {
            ZLOG_ALERT("My God ,your path length [%u] more than MAX_PATH [%u],I don't process this.",
                       length_of_ws,
                       MAX_PATH);
            continue;
        }
        char active_path[MAX_PATH + 1];
        ::WideCharToMultiByte( CP_ACP,
                               0,
                               read_ptr->FileName,
                               read_ptr->FileNameLength / sizeof(wchar_t),
                               active_path,
                               length_of_ws,
                               NULL,
                               NULL);
        active_path[length_of_ws] = '\0';

        //����Action��maskȷ�����ú���
        switch (read_ptr->Action)
        {
            case FILE_ACTION_ADDED:
                if (node_ptr->watch_mask_ | IN_CREATE)
                {
                    detect_ret = node_ptr->watch_event_->inotify_create(node_ptr->watch_handle_,
                                                                        node_ptr->watch_mask_,
                                                                        node_ptr->watch_path_,
                                                                        active_path);
                }
                break;
            case FILE_ACTION_REMOVED:
                if (node_ptr->watch_mask_ | IN_DELETE)
                {
                    detect_ret = node_ptr->watch_event_->inotify_delete(node_ptr->watch_handle_,
                                                                        node_ptr->watch_mask_,
                                                                        node_ptr->watch_path_,
                                                                        active_path);
                }
                break;
                //ע��Windows �µ�������ͣ����������Ը���
            case FILE_ACTION_MODIFIED:
                if (node_ptr->watch_mask_ | IN_MODIFY)
                {
                    detect_ret = node_ptr->watch_event_->inotify_modify(node_ptr->watch_handle_,
                                                                        node_ptr->watch_mask_,
                                                                        node_ptr->watch_path_,
                                                                        active_path);
                }
                break;
            case FILE_ACTION_RENAMED_OLD_NAME:
                if (node_ptr->watch_mask_ | IN_MOVED_FROM)
                {
                    detect_ret = node_ptr->watch_event_->inotify_moved_from(node_ptr->watch_handle_,
                                                                            node_ptr->watch_mask_,
                                                                            node_ptr->watch_path_,
                                                                            active_path);
                }
                break;
            case FILE_ACTION_RENAMED_NEW_NAME:
                if (node_ptr->watch_mask_ | IN_MOVED_TO)
                {
                    detect_ret = node_ptr->watch_event_->inotify_moved_to(node_ptr->watch_handle_,
                                                                          node_ptr->watch_mask_,
                                                                          node_ptr->watch_path_,
                                                                          active_path);
                }
                break;
        }

        //�ۼ�ƫ�Ƴ���
        next_entry_offset += read_ptr->NextEntryOffset;
        //�ۼƷ����¼�����
        ++(*watch_event_num);

        //����-1���ر�֮
        if (detect_ret == -1)
        {
            node_ptr->watch_event_->watch_close();
        }

        //ΪʲôҪ����������Ϊ����Ĵ�����̣����������Ѿ�������rm_watch������handle_close��
        if (watch_handle_ary_[activate_id] != cur_proc_handle)
        {
            return 0;
        }

    }
    while (read_ptr->NextEntryOffset != 0);

    DWORD bytes_returned = 0;

    //�������м�ش���
    bret = ::ReadDirectoryChangesW(
               watch_handle_ary_[activate_id],            // handle to directory
               node_ptr->read_buffer_, // read results buffer
               READ_BUFFER_LEN,                               // length of buffer
               node_ptr->watch_sub_dir_,                                 // monitoring option
               FILE_NOTIFY_CHANGE_SECURITY |
               FILE_NOTIFY_CHANGE_CREATION |
               FILE_NOTIFY_CHANGE_LAST_ACCESS |
               FILE_NOTIFY_CHANGE_LAST_WRITE |
               FILE_NOTIFY_CHANGE_SIZE |
               FILE_NOTIFY_CHANGE_ATTRIBUTES |
               FILE_NOTIFY_CHANGE_DIR_NAME |
               FILE_NOTIFY_CHANGE_FILE_NAME,          // filter conditions
               &bytes_returned,                       // bytes returned
               & (node_ptr->over_lapped_),  // overlapped buffer
               NULL                                   // completion routine
           );
    if (FALSE == bret )
    {
        ZLOG_ERROR("[%s] ::ReadDirectoryChangesW fail,error [%u].",
                   __ZCE_FUNCTION__,
                   ZCE_OS::last_error());
    }

    return 0;

#elif defined (ZCE_OS_LINUX)

    const int HANDLE_READY_ONE = 1;

    int result_watch = ZCE_OS::handle_ready(inotify_handle_, time_out, ZCE_OS::HANDLE_READY_READ);
    if (result_watch != HANDLE_READY_ONE)
    {
        return result_watch;
    }

    //
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
        read_len -= (sizeof(inotify_event) + ne_ptr->len);
        next_entry_offset += sizeof(inotify_event) + ne_ptr->len;

        HANDLE_TO_EVENT_MAP::iterator active_iter = watch_event_map_.find(ne_ptr->wd);
        if (active_iter == watch_event_map_.end())
        {
            //ĳ��FD��MAP�м��޷��ҵ������Ŀ�����
            ZCE_LOGMSG(RS_DEBUG,
                       "You code error or a handle not in map (delete in this do while), please check you code. handle[%u]",
                       ne_ptr->wd);
            continue;
        }
        ZCE_INOTIFY_NODE *node_ptr = &(active_iter->second);
        const char *active_path = ne_ptr->name;
        //���ݷ��ص�mask������δ���

        //ע������Ĵ����֧�õ�if else if ,������if if���ҵĳ�����������Щ�¼�����һ�𴥷�����Ҳ���ԡ�
        //����5���Ǻ�Windows ���еģ�
        if (ne_ptr->mask & IN_CREATE )
        {
            detect_ret = node_ptr->watch_event_->inotify_create(node_ptr->watch_handle_,
                                                                node_ptr->watch_mask_,
                                                                node_ptr->watch_path_,
                                                                active_path);
        }
        else if (ne_ptr->mask & IN_DELETE  )
        {
            detect_ret = node_ptr->watch_event_->inotify_delete(node_ptr->watch_handle_,
                                                                node_ptr->watch_mask_,
                                                                node_ptr->watch_path_,
                                                                active_path);
        }
        else if ( ne_ptr->mask & IN_MODIFY )
        {
            detect_ret = node_ptr->watch_event_->inotify_modify(node_ptr->watch_handle_,
                                                                node_ptr->watch_mask_,
                                                                node_ptr->watch_path_,
                                                                active_path);
        }
        else if ( ne_ptr->mask & IN_MOVED_FROM)
        {
            detect_ret = node_ptr->watch_event_->inotify_moved_from(node_ptr->watch_handle_,
                                                                    node_ptr->watch_mask_,
                                                                    node_ptr->watch_path_,
                                                                    active_path);
        }
        else if ( ne_ptr->mask & IN_MOVED_TO)
        {
            detect_ret = node_ptr->watch_event_->inotify_moved_to(node_ptr->watch_handle_,
                                                                  node_ptr->watch_mask_,
                                                                  node_ptr->watch_path_,
                                                                  active_path);
        }

#if defined ZCE_OS_LINUX
        //������Щ��LINUX�Լ����е�
        else if ( ne_ptr->mask & IN_ACCESS)
        {
            detect_ret = node_ptr->watch_event_->inotify_access(node_ptr->watch_handle_,
                                                                node_ptr->watch_mask_,
                                                                node_ptr->watch_path_,
                                                                active_path);
        }
        else if (ne_ptr->mask & IN_OPEN)
        {
            detect_ret = node_ptr->watch_event_->inotify_open(node_ptr->watch_handle_,
                                                              node_ptr->watch_mask_,
                                                              node_ptr->watch_path_,
                                                              active_path);
        }
        else if (ne_ptr->mask & IN_CLOSE_WRITE || ne_ptr->mask & IN_CLOSE_NOWRITE)
        {
            detect_ret = node_ptr->watch_event_->inotify_close(node_ptr->watch_handle_,
                                                               node_ptr->watch_mask_,
                                                               node_ptr->watch_path_,
                                                               active_path);
        }
        else if (ne_ptr->mask & IN_ATTRIB)
        {
            detect_ret = node_ptr->watch_event_->inotify_attrib(node_ptr->watch_handle_,
                                                                node_ptr->watch_mask_,
                                                                node_ptr->watch_path_,
                                                                active_path);
        }
        else if (ne_ptr->mask & IN_MOVE_SELF)
        {
            detect_ret = node_ptr->watch_event_->inotify_move_slef(node_ptr->watch_handle_,
                                                                   node_ptr->watch_mask_,
                                                                   node_ptr->watch_path_,
                                                                   active_path);
        }
        else if (ne_ptr->mask & IN_DELETE_SELF)
        {
            detect_ret = node_ptr->watch_event_->inotify_delete_slef(node_ptr->watch_handle_,
                                                                     node_ptr->watch_mask_,
                                                                     node_ptr->watch_path_,
                                                                     active_path);
        }
#endif

        //����-1���ر�֮,
        if (detect_ret == -1)
        {
            node_ptr->watch_event_->watch_close();
        }

        //�ۼƷ����¼�����
        ++(*watch_event_num);

    }
    while (read_len > 0);

    return 0;
#endif
}
