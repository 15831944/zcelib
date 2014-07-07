/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_os_adapt_inotify.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2013��3��28��
* @brief      ���Ŀ¼�仯���¼�������
*
*
* @details
*
*
*
* @note       �����Ƿ�ʹ��ZCE_INotify_Event_Base�ǿ���ͬʱ��ض��Ŀ¼������
*             ֻ�ܼ��һ��Ŀ¼����ԥ�˰��졣
*             �ҿ����˰��죬���û��Ǽ���event���Լ�ض��Ŀ¼���ˣ���Ϊ��������
*             ���򵥡��ؼ������eventֻ��һ���������Ľ�ɫ����ʵ������Ҫ����Ŀ¼��
*             Ҳ�Ͳ���Ҫ֪��watch_hadnle���ණ����
*
*
*/


#ifndef ZCE_LIB_INOTIFY_EVENT_BASE_H_
#define ZCE_LIB_INOTIFY_EVENT_BASE_H_


#include "zce_os_adapt_predefine.h"




/*!
* @brief      ���Ŀ¼�仯���¼������࣬���������ֻص�������
*             ZCE_INotify_Dir_Reactor ����ȴ�������
*
*/

class ZCE_INotify_Dir_Reactor;

class ZCE_INotify_Event_Base
{
    //ֻ�з�Ӧ�����Ե���
    friend class ZCE_INotify_Dir_Reactor;

    //
public:

    ///���캯��
    ZCE_INotify_Event_Base();
    ///��������
    virtual ~ZCE_INotify_Event_Base();

    ///���÷�Ӧ��
    void set_reactor(ZCE_INotify_Dir_Reactor *reactor);
    ///ȡ�÷�Ӧ��
    ZCE_INotify_Dir_Reactor *get_reactor();

    //��Ӧ���Ļص�����������ʽ����¼�ʱ����Ӧ�����лص�
protected:


    /*!
    @brief      �رռ������Ļص�������
    @return     int
    */
    virtual int watch_close();

    //===================================================================================================
    //�������飨5������Windows �¿���֧�ֵļ����Ŀ��LINUXҲ�У�
    //���ϣ��ͨ�ã����ֻ����5������������ǰ3����

    /*!
    * @brief      ��⵽���ļ���Ŀ¼�������Ļص��������������Ҫ����������Ϊ����̳����أ�
    *             ��Ӧ����IN_CREATE�����溯���Ĳ��������ƣ���ο�inotify_create��
    * @return     int          ����0��ʾ�ɹ�������-1��ʾ
    * @param[in]  watch_handle ����ļ������ע���Ǽ�صľ�������ǲ����ļ��ľ��
    * @param[in]  watch_mask   ��ط�������Ϊ�����룬����ͨ�������ж����ļ�����Ŀ¼
    * @param[in]  watch_path   ��ص�·��
    * @param[in]  active_path  ������������Ϊ���ļ�����Ŀ¼��·��
    */
    virtual int inotify_create(ZCE_HANDLE watch_handle,
                               uint32_t watch_mask,
                               const char *watch_path,
                               const char *active_path)
    {
        ZCE_UNUSED_ARG(watch_handle);
        ZCE_UNUSED_ARG(watch_mask);
        ZCE_UNUSED_ARG(watch_path);
        ZCE_UNUSED_ARG(active_path);
        return 0;
    }

    ///��⵽��ɾ���ļ�����Ŀ¼,��Ӧ����IN_DELETE������˵���ο�@fun inotify_create
    virtual int inotify_delete(ZCE_HANDLE /*watch_handle*/,
                               uint32_t /*watch_mask*/,
                               const char * /*watch_path*/,
                               const char * /*active_path*/)
    {
        return 0;
    }

    ///��⵽���ļ����޸�,��Ӧ����IN_MODIFY������˵���ο�@fun inotify_create
    virtual int inotify_modify(ZCE_HANDLE /*watch_handle*/,
                               uint32_t /*watch_mask*/,
                               const char * /*watch_path*/,
                               const char * /*active_path*/)
    {
        return 0;
    }

    ///����ļ���ĳ��Ŀ¼�ƶ���ȥ��IN_MOVED_FROM,����˵���ο�@fun inotify_create
    virtual int inotify_moved_from(ZCE_HANDLE /*watch_handle*/,
                                   uint32_t /*watch_mask*/,
                                   const char * /*watch_path*/,
                                   const char * /*active_path*/)
    {
        return 0;
    }

    ///����ļ��ƶ���ĳ��Ŀ¼��IN_MOVED_TO,(���Լ�����ֻ���ڼ��Ŀ¼���ƶ��Żᷢ������¼�),
    ///����˵���ο�@fun inotify_create
    virtual int inotify_moved_to(ZCE_HANDLE /*watch_handle*/,
                                 uint32_t /*watch_mask*/,
                                 const char * /*watch_path*/,
                                 const char * /*active_path*/)
    {
        return 0;
    }

    //===================================================================================================
    //������Щ��LINUX���е�һЩ�����Ŀ��������굽����Ϊ�˱��룬�Ǳ�������ʹ��

#if defined (ZCE_OS_LINUX)

    ///�������Ŀ¼���ļ�������ʱ���ص���IN_ACCESS,����˵���ο�@fun inotify_create
    virtual int inotify_access(int /*watch_handle*/,
                               uint32_t /*watch_mask*/,
                               const char * /*watch_path*/,
                               const char * /*active_path*/)
    {
        return 0;
    }

    ///�������Ŀ¼���ļ�����ʱ���ص���IN_OPEN,����˵���ο�@fun inotify_create
    virtual int inotify_open(int /*watch_handle*/,
                             uint32_t /*watch_mask*/,
                             const char * /*watch_path*/,
                             const char * /*active_path*/)
    {
        return 0;
    }

    ///�������Ŀ¼���ļ����ر��¼�ʱ���ص���IN_CLOSE_WRITE,IN_CLOSE_NOWRITE,
    ///����˵���ο�@fun inotify_create
    virtual int inotify_close(int /*watch_handle*/,
                              uint32_t /*watch_mask*/,
                              const char * /*watch_path*/,
                              const char * /*active_path*/)
    {
        return 0;
    }

    ///����Ŀ¼�����ļ�����Ŀ¼���Ա��޸��¼�ʱ���ص���IN_ATTRIB�� permissions, timestamps,
    ///extended attributes, link count (since Linux 2.6.25), UID, GID,
    ///����˵���ο�@fun inotify_create
    virtual int inotify_attrib(int /*watch_handle*/,
                               uint32_t /*watch_mask*/,
                               const char * /*watch_path*/,
                               const char * /*active_path*/)
    {
        return 0;
    }

    ///������ص�Ŀ¼���ƶ�ʱ���ص���IN_MOVE_SELF,����˵���ο�@fun inotify_create
    virtual int inotify_move_slef(int /*watch_handle*/,
                                  uint32_t /*watch_mask*/,
                                  const char * /*watch_path*/,
                                  const char * /*active_path*/)
    {
        return 0;
    }

    ///������ص�Ŀ¼��ɾ��ʱ���ص���IN_DELETE_SELF,����˵���ο�@fun inotify_create
    virtual int inotify_delete_slef(int /*watch_handle*/,
                                    uint32_t /*watch_mask*/,
                                    const char * /*watch_path*/,
                                    const char * /*active_path*/)
    {
        return 0;
    }

#endif


protected:

    //
    ZCE_INotify_Dir_Reactor  *inotify_reactor_;
};






#endif //ZCE_LIB_INOTIFY_EVENT_BASE_H_


