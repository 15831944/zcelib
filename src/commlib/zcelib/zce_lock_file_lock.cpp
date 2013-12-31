#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_file.h"
#include "zce_os_adapt_flock.h"
#include "zce_lock_file_lock.h"

//���캯��
ZCE_File_Lock::ZCE_File_Lock(ZCE_HANDLE file_handle):
    lock_file_hadle_(file_handle),
    open_by_self_(false)
{
}

ZCE_File_Lock::ZCE_File_Lock():
    lock_file_hadle_(ZCE_INVALID_HANDLE),
    open_by_self_(false)
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
    //�����ظ��򿪣��ö��Ա���
    ZCE_ASSERT(lock_file_hadle_ == ZCE_INVALID_HANDLE);

    //������ļ�
    lock_file_hadle_ = ZCE_OS::open ( file_name,
                                      open_mode,
                                      perms);

    if (lock_file_hadle_ == ZCE_INVALID_HANDLE)
    {
        return -1;
    }

    //��ʶ���Լ��򿪵�
    open_by_self_ = true;

    return 0;
}

//ͨ���ļ������ʼ���ļ���
int ZCE_File_Lock::open(ZCE_HANDLE file_handle)
{
    lock_file_hadle_ = file_handle;
    return 0;
}

//�ر��ļ���
int ZCE_File_Lock::close()
{
    unlock();

    //������Լ��򿪵ģ��ر�֮
    if (open_by_self_)
    {
        ZCE_OS::close(lock_file_hadle_);
    }

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
    ret = ZCE_OS::flock (lock_file_hadle_, LOCK_SH);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::flock LOCK_SH", ret);
        return;
    }

    return;
}
//���Զ�ȡ��
bool ZCE_File_Lock::try_lock_read()
{
    int ret = 0;

    ret = ZCE_OS::flock (lock_file_hadle_, LOCK_SH | LOCK_NB);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::flock LOCK_SH|LOCK_NB", ret);
        return false;
    }

    return true;
}

//д����
void ZCE_File_Lock::lock_write()
{
    int ret = 0;
    ret = ZCE_OS::flock (lock_file_hadle_, LOCK_EX);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::flock LOCK_EX", ret);
        return;
    }
}
//���Զ�ȡ��
bool ZCE_File_Lock::try_lock_write()
{
    int ret = 0;

    ret = ZCE_OS::flock (lock_file_hadle_, LOCK_EX | LOCK_NB);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::flock LOCK_EX|LOCK_NB", ret);
        return false;
    }

    return true;
}

//����,����Ƕ�д��Ҳֻ��Ҫ��һ������
void ZCE_File_Lock::unlock()
{
    int ret = 0;

    ret = ZCE_OS::flock (lock_file_hadle_, LOCK_UN);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::flock LOCK_UN", ret);
        return;
    }
}

