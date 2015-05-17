#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_file.h"
#include "zce_os_adapt_flock.h"
#include "zce_lock_file_lock.h"

//���캯��
ZCE_File_Lock::ZCE_File_Lock():
    lock_file_hadle_(ZCE_INVALID_HANDLE),
    open_by_self_(false),
    file_len_(0)
{
}

//��������
ZCE_File_Lock::~ZCE_File_Lock()
{
    close();
}

//ͨ���ļ����Ʋ�����ʼ���ļ������������ļ�
int ZCE_File_Lock::open(const char *file_name,
                        int open_mode,
                        mode_t perms)
{
    int ret = 0;
    //�����ظ��򿪣��ö��Ա���
    ZCE_ASSERT(lock_file_hadle_ == ZCE_INVALID_HANDLE);

    //������ļ�
    lock_file_hadle_ = ZCE_LIB::open (file_name,
                                      open_mode,
                                      perms);

    if (lock_file_hadle_ == ZCE_INVALID_HANDLE)
    {
        return -1;
    }

    ret = ZCE_LIB::filesize(lock_file_hadle_, &file_len_);
    if (0 != ret)
    {
        return ret;
    }

    ZCE_LIB::flock_init(&file_lock_hdl_, lock_file_hadle_);

    //��ʶ���Լ��򿪵�
    open_by_self_ = true;

    return 0;
}

//ͨ���ļ������ʼ���ļ���
int ZCE_File_Lock::open(ZCE_HANDLE file_handle)
{
    int ret = 0;
    lock_file_hadle_ = file_handle;
    ret = ZCE_LIB::filesize(lock_file_hadle_, &file_len_);
    if (0 != ret)
    {
        return ret;
    }

    ZCE_LIB::flock_init(&file_lock_hdl_, lock_file_hadle_);

    return 0;
}

//�ر��ļ���
int ZCE_File_Lock::close()
{
    unlock();

    //������Լ��򿪵ģ��ر�֮
    if (open_by_self_)
    {
        ZCE_LIB::close(lock_file_hadle_);
    }
    ZCE_LIB::flock_destroy(&file_lock_hdl_);
    return 0;
}

//�õ����ļ��ľ��
ZCE_HANDLE ZCE_File_Lock::get_file_handle()
{
    return lock_file_hadle_;
}

//��ȡ��
void ZCE_File_Lock::lock_read()
{
    int ret = 0;
    ret = ZCE_LIB::flock_rdlock(&file_lock_hdl_, SEEK_SET, 0, file_len_);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::flock LOCK_SH", ret);
        return;
    }

    return;
}
//���Զ�ȡ��
bool ZCE_File_Lock::try_lock_read()
{
    int ret = 0;

    ret = ZCE_LIB::flock_tryrdlock(&file_lock_hdl_, SEEK_SET, 0, file_len_);
    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::flock LOCK_SH|LOCK_NB", ret);
        return false;
    }

    return true;
}

//д����
void ZCE_File_Lock::lock_write()
{
    int ret = 0;
    ret = ZCE_LIB::flock_wrlock(&file_lock_hdl_, SEEK_SET, 0, file_len_);
    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::flock LOCK_EX", ret);
        return;
    }
}
//���Զ�ȡ��
bool ZCE_File_Lock::try_lock_write()
{
    int ret = 0;

    ret = ZCE_LIB::flock_trywrlock(&file_lock_hdl_, SEEK_SET, 0, file_len_);
    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::flock LOCK_EX|LOCK_NB", ret);
        return false;
    }

    return true;
}

//����,����Ƕ�д��Ҳֻ��Ҫ��һ������
void ZCE_File_Lock::unlock()
{
    int ret = 0;

    ret = ZCE_LIB::flock_unlock(&file_lock_hdl_, SEEK_SET, 0, file_len_);
    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::flock LOCK_UN", ret);
        return;
    }
}

