/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_inotify_directory_reactor.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2013��4��1��
* @brief      ���Ŀ¼�µ��ļ��仯�ķ�װ��
* 
* @details    Linux��ʹ�õ�INotify+SELECT���ƣ���Windows��ʹ��ReadDirectoryChangesW��OverLapped
*             ����Windows��LINUX�ڼ���ļ��仯�ϲ��ʵ���е��
*             Linux��ʹ�õ�INotify+SELECT���ƣ���Windows��ʹ��ReadDirectoryChangesW��OverLapped
*             1.Windows��һ���첽��OverLapped�����ù��̣���Linux��һ��IO���ù��̣�������ȫһ�¡�
*             2.Windows ��ֻ�ܼ���MAXIMUM_WAIT_OBJECTS��Ŀ¼����Linux û������
*             3.Windows ֻ�ܼ���Ŀ¼�������ܼ����ļ�
*             4.Windows ���Լ����Ŀ¼����Linux�����ԡ�
*             5.Windows ���Լ�ص����ͺ��١���������
*             6.Linux��ص���һ���豸�������Windows�Ƕ�ÿ��Ŀ¼�ľ�����д���
*             Ŀǰֻ�ܿ�����һ�����ߵĲ�ν��з�װ�ˣ�������OS�����ˡ�
* 
*             ����²�һ�£����˷�ָ��Windows API����ĩ���inotify��windows�·�װһ��
*             �����õ���ReadDirectoryChangesW + FindFirstChangeNotification ,
*             FindNextChangeNotification��һϵ�к�����
*             1.��������Windows IO���ƣ�ʹ�÷�ʽ�ͳ���TMD�Ķ��ˡ�����㲻��ʹ��������ģʽ�����Ҫ��
*             Overlapped������Ӵ򽻵����ð�Windows IO���������ģ����Ҳ���ˡ���
*             FindFirstChangeNotification�ֱ����ڳ��Ǿ��+WaitForSingleObjectģʽ������������
*             �θ㡣
*             2.ReadDirectoryChangesW ��������صĲ���dwNotifyFilter �����ͷ��ص�Action��ȫ����
*             ���¡����Ե���õ�Actionʱ�������ж��Ķ�����ʵ���١�����Inotify�ĵļ��ϸ�֣���ֱ��ֵһ�ᡣ
*             FindFirstChangeNotification �����͸��Ӳ�ֵһ���ˡ���֪�����鷢���ˣ�����ȫ��֪��������
*             ʲô��������
*             3.Windows ���Լ�����ļ��У���������Windows API����Ψһ��ɪһ�µĶ���
*             ��inotify�ĺ���һ�ȣ�����Windows API�������Ӧ��������¥��
* 
*             ZCE_INotify_Dir_ReactorΪ�˼��ݶ��ƽ̨���е��Ť�����ֻ��Linux��ʹ�ã�
*             �������Select ��Epoll Reactorһ��ʹ�ã�ZCE_Event_INotify�Ǹ��õ�ѡ��
* 
* �����Ź�������ȥ��10���꣬
* ��Ȼ�ǵ����죬2003���4��1���°࣬�ǵ��������ӣ���ҵ������
* �Һ�Linhai,Zhangke �ڿƼ�԰�Ĵ���й䣬Linhai����һ��
* ���ĵ�������ڶ�������������ج�ġ�
* 
* ���ҡ�
* �ݳ����Ź���  ��ʣ���Ϧ
* i am what i am
* ����Զ������������
* �����ǡ����ֵķ�ʽ��ֻһ��
* �������ǡ�˭���������ߵĹ���
* �������㡡Ϊ��ϲ�����������
* ���÷�ī����վ�ڹ����Ľ���
* #�Ҿ����ҡ�����ɫ��һ�����̻�
* ��պ�����Ҫ�����ǿ����ĭ
* ��ϲ���ҡ���Ǿޱ����һ�ֽ��
* �¶���ɳĮ�һ��ʢ�ŵĳ�����
* ��ô���ˡ����������п�������
* ������˵��ʲô�ǹ���������
* 
* 
*/


#ifndef ZCE_LIB_INOTIFY_DIRECTORY_REACTOR_H_
#define ZCE_LIB_INOTIFY_DIRECTORY_REACTOR_H_


#include "zce_os_adapt_predefine.h"

class ZCE_INotify_Event_Base;



/*!
* @brief      Inotify�ļ���ļ����ļ��仯�ķ�Ӧ��
*
*/
class ZCE_INotify_Dir_Reactor
{
public:

    ///���캯��
    ZCE_INotify_Dir_Reactor(size_t max_watch_dir = DEFUALT_MAX_WATCH_DIR_SIZE );

    ///��������
    ~ZCE_INotify_Dir_Reactor();

    ///�򿪷�Ӧ������ظ��ַ�ӳ
    int open();

    ///�ر��ļ���ط�Ӧ��,�ͷ������Դ��
    int close();


    /*!
    * @brief      ����һ��Ҫ���м�ص��ļ�����
    * @return     int           ����0��ʾ�ɹ�������-1��ʾʧ��
    * @param[in]  event_base    ����¼������󣬶�Ӧ�Ļص�������
    * @param[in]  pathname      ��ص�·��
    * @param[in]  mask          ��ص�ѡ��
    * @param[out] watch_handle  ���صļ�ض�Ӧ�ľ��
    * @param[in]  watch_sub_dir �Ƿ�����Ŀ¼���������ֻ��Windows����Ч��LINUX��ûЧ��
    */
    int add_watch(ZCE_INotify_Event_Base *event_base,
                  const char *pathname,
                  uint32_t mask,
                  ZCE_HANDLE *watch_handle,
                  bool watch_sub_dir = false);


    /*!
    * @brief      ͨ���ļ�������Ƴ�һ��Ҫ��ص���Ŀ��
    * @return     int          ����0��ʾ�ɹ�������-1��ʾʧ��
    * @param[in]  watch_handle ���Ŀ¼���ļ����
    */
    int rm_watch(ZCE_HANDLE watch_handle);

    /*!
    * @brief      ͨ���¼������Ƴ�������ص�Ҫ��ص���Ŀ���˺����õ�������Hash�ڲ��������ٶȻ�Ƚ�����
    * @return     int        ����0��ʾ�ɹ�������-1��ʾʧ��
    * @param[in]  event_base Ҫȡ�����ע����¼�ָ�룬
    */
    int rm_watch(const ZCE_INotify_Event_Base *event_base);


    /*!
    * @brief      ������Ӧ����Ӧ�ĺ�����������¼����������������ص���Ӧ��ZCE_INotify_Event_Base
    * @return     int               ����0��ʾ����
    * @param[in]  time_out          ��Ӧ�ȴ��ĳ�ʱʱ��
    * @param[in]  watch_event_num   ��ص��¼�����
    */
    int watch_event(ZCE_Time_Value *time_out, size_t *watch_event_num);


protected:


    ///BUFFER�ĳ���
    static const size_t     READ_BUFFER_LEN = 16 * 1024 - 1;

#if defined (ZCE_OS_WINDOWS)
    ///Ĭ�ϵļ�ص�Ŀ¼��������Windows ���ֻ����MAXIMUM_WAIT_OBJECTS��
    static  const size_t    DEFUALT_MAX_WATCH_DIR_SIZE = MAXIMUM_WAIT_OBJECTS;
#elif defined (ZCE_OS_LINUX)
    ///Ĭ�ϵļ�ص�Ŀ¼������
    static  const size_t    DEFUALT_MAX_WATCH_DIR_SIZE = 256;
#endif

    ///�����ļ���صĽڵ�
    struct ZCE_INOTIFY_NODE
    {
        ZCE_INOTIFY_NODE():
            watch_handle_(ZCE_INVALID_HANDLE),
            watch_mask_(0),
            watch_event_(NULL)
        {

#if defined (ZCE_OS_WINDOWS)
            memset((void *)&over_lapped_, 0, sizeof(OVERLAPPED));
            read_buffer_ = NULL;
            watch_sub_dir_ = FALSE;
#endif
        }
        ~ZCE_INOTIFY_NODE()
        {
        }

        ///��صľ��
        ZCE_HANDLE              watch_handle_;
        ///���ӵ��ļ�·��
        char                    watch_path_[MAX_PATH];
        ///����������
        uint32_t                watch_mask_;

        ///����¼��Ļص��¼���
        ZCE_INotify_Event_Base *watch_event_;

#if defined (ZCE_OS_WINDOWS)
        ///��ȡ��Buffer��
        char                   *read_buffer_;
        ///over lapped ʹ�õĶ���
        OVERLAPPED              over_lapped_;
        ///�Ƿ�����Ŀ¼
        BOOL                    watch_sub_dir_;
#endif

    };

    typedef unordered_map<ZCE_HANDLE, ZCE_INOTIFY_NODE >  HANDLE_TO_EVENT_MAP;

    ///��Ӧ�������Ŀ¼�ڵ���Ϣ��MAP,
    HANDLE_TO_EVENT_MAP watch_event_map_;


#if defined (ZCE_OS_WINDOWS)
    ///WaitForMultipleObjects�ȴ���Ŀ¼���
    ///ΪʲôҪ������ظ��Ľṹ����Ҫ��Ϊ�˷���ʹ��WaitForMultipleObjects���ٶ�
    ZCE_HANDLE          watch_handle_ary_[MAXIMUM_WAIT_OBJECTS];

#elif defined (ZCE_OS_LINUX)
    ///inotify_init ��ʼ���õ��ľ��
    ZCE_HANDLE          inotify_handle_;

    char               *read_buffer_;
#endif


};



#endif //ZCE_LIB_INOTIFY_DIRECTORY_REACTOR_H_


