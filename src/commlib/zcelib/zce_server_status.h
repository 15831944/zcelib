/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_server_status.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2006��3��7��
* @brief
*
*
* @details    ��������ͳ���࣬
*             ����뷨����rong,sonicmao��C4A�ķ�����,��rong,sonicmao�¾�.
*
*
* @note       �޸ļ�¼���ܼ򵥵�һ���࣬���Ķ��ܶ�
*     1.Date  :2011��10��30��
*     Author  :����ZCELIB�Ĵ�������еĴ�������Ż�
*     Modification  :
*     2.Date  :2012��1��15��
*     Author  :Sailzeng
*     Modification  :��N�η������Լ���Ѫ�ĸ���2�κ��ҿ�ʼ������һ����򵥵ķ�ʽ������⣬
*     3.Date  :2012��5��10��
*     Author  :Sailzeng
*     Modification  :�ܶడͬ�»����²���Ҫ����classify�����뻹�Ǹ��˰ѡ����sandy��mandy
*     4.Date  :2013��10��1��
*     Author  :Sailzeng
*     Modification  :��һ����ֹͳ��ID�ظ������󣬽�ԭ����ARRAY�������һ��MAP��
*
*/

#ifndef ZCE_LIB_SERVER_STATUS_H_
#define ZCE_LIB_SERVER_STATUS_H_

#include "zce_share_mem_posix.h"
#include "zce_shm_vector.h"
#include "zce_lock_null_lock.h"
#include "zce_os_adapt_time.h"

/******************************************************************************************
״̬ͳ�Ƶķ�ʽ������5���Ӽ�¼һ�εķ�ʽ��һ��Сʱ��¼�ķ�ʽ��
******************************************************************************************/

//Ϊʲô�ŵ������棬��ǿ����Լ��
enum ZCE_STATUS_STATICS_TYPE
{
    //��ʶ��Χ
    STATICS_INVALID_TYPE       = 0,

    //ÿ5���ӽ���һ��ͳ�ƣ�������5���ӵ�ͳ�Ʒ�ʽ
    STATICS_PER_FIVE_MINTUES   = 1,

    //ÿСʱ����һ��ͳ��,:0����գ�����ͳ�Ʒ�ʽ��Ҫ����1��Сʱ�仯���ݣ����ڶԱȵȣ�
    STATICS_PER_HOUR           = 2,
    //ÿ�����һ��ͳ�ƣ�0:0����գ�������ͳ�Ʒ�ʽ��Ҫ����1��仯����
    STATICS_PER_DAYS           = 3,

    //����ֵ
    STATICS_ABSOLUTE_VALUE     = 11,

    //ԭ������һ��ÿ������ʱ����ֵ�Ƿ���յ�ѡ����ˣ�û��Ҫ������

};

/******************************************************************************************
struct ZCE_STATUS_ITEM_ID
******************************************************************************************/
struct ZCELIB_EXPORT ZCE_STATUS_ITEM_ID
{
public:

    //ͳ��ID
    unsigned int              statics_id_;
    //����ID��Ŀǰ������Ҫ��ҵ��ID,����ǿ��Ա仯��
    unsigned int              app_id_;
    //�ӷ���ID�����Ҳ�ǿ��Ա仯�ģ�
    unsigned int              classify_id_;

public:

    ZCE_STATUS_ITEM_ID(unsigned int statics_id,
                       unsigned int app_id,
                       unsigned int classify_id);
    ZCE_STATUS_ITEM_ID();
    ~ZCE_STATUS_ITEM_ID();

    bool operator == (const ZCE_STATUS_ITEM_ID &others) const;
};

///�õ�HASH���ӵĺ�����
struct ZCELIB_EXPORT HASH_ZCE_STATUS_ITEM_ID
{
public:
    //�Ͱ�3����ֵ��+��������ͻ�о���Сһ�㣬�����Ʒ����о����ã�
    //
    size_t operator()(const ZCE_STATUS_ITEM_ID &stat_item) const
    {
        return (size_t (stat_item.statics_id_
                        + (stat_item.app_id_ << 8)
                        + stat_item.classify_id_));
    }
};

/******************************************************************************************
struct ZCE_STATUS_ITEM ״̬��������
******************************************************************************************/
class ZCELIB_EXPORT ZCE_STATUS_ITEM
{
public:

    //���캯������������
    ZCE_STATUS_ITEM();
    ZCE_STATUS_ITEM(unsigned int statics_id,
                    ZCE_STATUS_STATICS_TYPE statics_type);
    ~ZCE_STATUS_ITEM();

public:

    //ID��ʶ
    ZCE_STATUS_ITEM_ID        item_id_;

    //�������¼���
    ZCE_STATUS_STATICS_TYPE   statics_type_;

    //������
    uint64_t                  counter_;
};

/******************************************************************************************
class ZCE_STATUS_ITEM_WITHNAME ״̬������+���֣��������ã�DUMP�����
******************************************************************************************/
class ZCELIB_EXPORT ZCE_STATUS_ITEM_WITHNAME
{
public:

    //
    ZCE_STATUS_ITEM_WITHNAME(unsigned int ,
                             ZCE_STATUS_STATICS_TYPE ,
                             const char *);
    ZCE_STATUS_ITEM_WITHNAME();
    ~ZCE_STATUS_ITEM_WITHNAME();

public:
    //ͳ����Ŀ���Ƶĳ���
    static const size_t       MAX_COUNTER_NAME_LEN = 64;

public:

    ZCE_STATUS_ITEM           statics_item_;

    //����������
    char                      item_name_[MAX_COUNTER_NAME_LEN + 1];

};



//���ڰ����㶨�� ZCE_STATUS_ITEM_WITHNAME����
#ifndef DEF_ZCE_STATUS_ITEM
#define DEF_ZCE_STATUS_ITEM(_statics_id,_statics_type) ZCE_STATUS_ITEM_WITHNAME(_statics_id,_statics_type,(#_statics_id))
#endif

/******************************************************************************************
struct ZCE_STATUS_HEAD ״̬�ļ���ͷ��
******************************************************************************************/

struct ZCELIB_EXPORT ZCE_STATUS_HEAD
{
    //��ؿ�ʼʱ��
    uint32_t monitor_start_time_;

    //������ݸ��Ƶ�ʱ���
    uint32_t copy_time_;

    //��monitor�ϱ���ʱ��
    uint32_t report_monitor_time_;

    //����ʱ��
    uint32_t active_time_;
};

/******************************************************************************************
ʹ��Posix MMAP,��¼�����������һЩ������,״̬,
����ʹ�õ�����ģʽ��������ʹ����������ᵼ�����������������,
******************************************************************************************/
class ZCELIB_EXPORT ZCE_Server_Status : public ZCE_NON_Copyable
{
protected:

    //���ͳ�����ݵĹ����ڴ����飬
    typedef ZCE_LIB::shm_vector<ZCE_STATUS_ITEM>     ARRYA_OF_SHM_STATUS;
    //ͳ��ID��������±��hash map
    typedef unordered_map<ZCE_STATUS_ITEM_ID, size_t, HASH_ZCE_STATUS_ITEM_ID>     STATID_TO_INDEX_MAP;
    //statics_id_��key��ZCE_STATUS_ITEM_WITHNAME�Ľṹ
    typedef unordered_map<unsigned int , ZCE_STATUS_ITEM_WITHNAME>    STATUS_WITHNAME_MAP;

public:

    //ͳ�����ݵ����飬����dump��������ݽṹ
    typedef std::vector<ZCE_STATUS_ITEM_WITHNAME>     ARRAY_OF_STATUS_WITHNAME;

public:

    //���캯��,Ҳ���㵥��ʹ�õĻ��ᣬ���Բ���protected
    ZCE_Server_Status();
    //��������
    virtual ~ZCE_Server_Status();

protected:

    //��ʼ���ķ���,ͨ�õĵײ㣬
    //Param1: char* statfilename MMAPӰ���״̬�ļ�����
    //Param2: bool restore_mmap �Ƿ����ڻָ�MMAP������ǻָ����ļ������Ǵ��ڵ�,
    int initialize(const char *stat_filename,
                   bool restore_mmap,
                   bool multi_thread);

    //��sandy���������棬��������Ŀ
    int find_insert_idx(unsigned int statics_id,
                        unsigned int app_id,
                        unsigned int classify_id,
                        size_t *sandy_idx);

public:

    //����һ���Ѿ����ڵ��ļ����г�ʼ��,���ڻָ�������,�ļ������Ѿ����ڣ�
    //Param1: char* statfilename MMAPӰ���״̬�ļ�����
    int initialize(const char *stat_filename);

    //����һ���Ѿ����ڵ��ļ����г�ʼ��,���ڻָ�������,����ļ������Ѿ����ڣ������´���
    int initialize(const char *stat_filename,
                   size_t num_stat_item,
                   const ZCE_STATUS_ITEM_WITHNAME item_ary[],
                   bool multi_thread);

    //����һЩ�����Ŀ
    void add_status_item(size_t num_stat_item,
                         const ZCE_STATUS_ITEM_WITHNAME item_ary[]);

    //��ʼ���Ժ��޸��Ƿ���Ҫ���̱߳���
    void modify_multi_thread_guard(bool multi_thread);

    //���ֵ�޸�mandy����sandyͳ�Ƽ�����ʹ��ͳ��ID�ͷ���ID��Ϊkey,�ӿ�ʹ�÷���һ�㣬�㲻�ü�¼�ܶ��Ӧ��ϵ,���ٶ���һ��,
    int increase_by_statid(unsigned int statics_id,
                           unsigned int app_id,
                           unsigned int classify_id,
                           int64_t incre_value);

    //��Ҫ����classify_id�����������������
    inline int increase_by_statid(unsigned int statics_id,
                                  unsigned int app_id,
                                  int64_t incre_value);

    //��Ҫ����classify_id��app_id��������ֵ���
    inline int increase_by_statid(unsigned int statics_id,
                                  int64_t incre_value);

    //���ֵ�޸�mandy����sandyͳ�Ƽ�����ʹ��ͳ��ID�ͷ���ID��Ϊkey,�ӿ�ʹ�÷���һ�㣬�㲻�ü�¼�ܶ��Ӧ��ϵ,���ٶ���һ��,
    inline int increase_once(unsigned int statics_id,
                             unsigned int app_id,
                             unsigned int classify_id);

    //��Ҫ����classify_id�����������������
    inline int increase_once(unsigned int statics_id,
                             unsigned int app_id);

    //��Ҫ����classify_id��app_id��������ֵ���
    inline int increase_once(unsigned int statics_id);

    //����ֵ�޸ļ��ͳ����Ŀ
    int set_by_statid(unsigned int statics_id,
                      unsigned int app_id,
                      unsigned int classify_id,
                      uint64_t set_value);

    //��Ҫ����classify_id�����������������
    inline int set_by_statid(unsigned int statics_id,
                             unsigned int app_id,
                             uint64_t set_value);

    //��Ҫ����classify_id��app_id��������ֵ���
    inline int set_by_statid(unsigned int statics_id,
                             uint64_t set_value);

    //����ͳ��ID�ͷ���ID��Ϊkey���õ�ͳ����ֵ
    uint64_t get_counter(unsigned int statics_id,
                         unsigned int app_id,
                         unsigned int classify_id);

    //ȡ�ü������ĸ���
    size_t num_of_counter();

    //��ȡcopy_time
    uint32_t get_copy_time();

    //������ڵ����ݣ�����Ķ�ʱ������ʱ���ã���Ȼǰ�����Ӧ���ϱ��������ڽ�һЩ������0��
    //������ÿ5���ӵ���һ�ξ�OK
    void check_overtime(time_t now_time);

    //���ݼ�������Ϣ
    void copy_stat_counter();

    //���ڽ��ڲ�����ȫ��ȡ�����������ⲿ���֮��
    void dump_all(ARRAY_OF_STATUS_WITHNAME &array_status,
                  bool dump_copy = false);

    //Dump���е�����
    void dump_status_info(std::ostringstream &strstream,
                          bool dump_copy = false);

    //Dump���е�����
    void dump_status_info(ZCE_LOG_PRIORITY log_priority,
                          bool dump_copy = false);

    //�õ��ļ���ͷ����Ϣ
    void get_stat_head(ZCE_STATUS_HEAD *stat_head );

    //��¼��ص��ϱ�ʱ��
    void report_monitor_time(uint32_t report_time = static_cast<uint32_t>(time(NULL)));

    //���ӵĺ���Ⱥ�������Ҳ�֪��������BOOST��ģ��ʹ�õ��ӣ�����������ֱ����ˬ�����������޸�һЩ
    //�Ҳ���Ϊ�˵��ӿ�����ν�ı������⣬���Լ���֤��ĳ�ʼ��������������

private:
    //��Ҫ�����ļ�����Ƿ��Ѿ�����
    bool is_stat_id_exist(unsigned int stat_id);

    //���������
    int add_stat_id(unsigned int stat_id);

public:

    //�õ�����ʵ��
    static ZCE_Server_Status *instance();
    //����ʵ����ֵ
    static void instance(ZCE_Server_Status *);
    //������ʵ��
    static void clean_instance();

protected:

    //���ļ����ĿID,
    static const size_t MAX_MONITOR_STAT_ITEM = 64 * 1024;

    //����ӵ�����
    static const time_t FIVE_MINTUE_SECONDS   = 300;
    //һСʱ������
    static const time_t ONE_HOURS_SECONDS     = 3600;
    //һ���ʱ��
    static const time_t ONE_DAY_SECONDS       = 86400;

    //ͳ����Ŀ��ֵ����1
    static const uint64_t INCREASE_VALUE_ONCE = 1;

protected:

    //��̬����,
    ZCE_Lock_Base            *stat_lock_;

    //��һ�������ʱ��
    time_t                    clear_time_;

    //MMAP�ڴ�Ӱ��������ļ�
    ZCE_ShareMem_Posix        stat_file_;

    //�ڴ��ļ�ͷ
    ZCE_STATUS_HEAD          *stat_file_head_;

    //mandy��sandy��ԭ�������м�Ϊ�����������鶨λ��hash��λ�����������Ķ�����
    //��������ȫ����Ϊ����hansh��λ��������˼������СMM����������ǲ��Ź����ǣ�
    //http://t.qq.com/angelbaby22

    // ���״̬������������
    ARRYA_OF_SHM_STATUS      *status_stat_sandy_;

    //״̬��������һ�ݿ��������ݷ��ڹ����ڴ��У����ǿ����ÿ�����mandy��ȡ���ݣ����Կ��Խ�ÿ��
    ARRYA_OF_SHM_STATUS      *status_copy_mandy_;

    //��¼���õĵ�ͳ������SET�����ڼ�¼���õ�ͳ����Ŀ��Ҳ���ڷ�ֹ�ظ������dump ���ʱ��������Ϣ
    STATUS_WITHNAME_MAP       conf_stat_map_;

    //STAT_ID to idx������MAP
    STATID_TO_INDEX_MAP       statid_to_index_;

    //�Ƿ���ж��̱߳���
    bool                      multi_thread_guard_;

    // �Ƿ��Ѿ���ʼ��
    bool                      initialized_;

protected:

    //����ʵ��
    static ZCE_Server_Status *instance_;
};


//�����Ŀ��������1
int ZCE_Server_Status::increase_once(unsigned int statics_id,
                                     unsigned int app_id,
                                     unsigned int classify_id)
{
    return increase_by_statid(statics_id, app_id, classify_id, INCREASE_VALUE_ONCE);
}

//��Ҫ����classify_id������ļ����Ŀ��������1
int ZCE_Server_Status::increase_once(unsigned int statics_id,
                                     unsigned int app_id)
{
    return increase_by_statid(statics_id, app_id, 0, INCREASE_VALUE_ONCE);
}

//��Ҫ����classify_id��app_id�ļ����Ŀ��������1
int ZCE_Server_Status::increase_once(unsigned int statics_id)
{
    return increase_by_statid(statics_id, 0, 0, INCREASE_VALUE_ONCE);
}


//��Ҫ����classify_id�����������������
int ZCE_Server_Status::increase_by_statid(unsigned int statics_id,
                                          unsigned int app_id,
                                          int64_t incre_value)
{
    return increase_by_statid(statics_id, app_id, 0, incre_value);
}

//��Ҫ����classify_id��app_id��������ֵ���
int ZCE_Server_Status::increase_by_statid(unsigned int statics_id,
                                          int64_t incre_value)
{
    return increase_by_statid(statics_id, 0, 0, incre_value);
}

//��Ҫ����classify_id�����������������
int ZCE_Server_Status::set_by_statid(unsigned int statics_id,
                                     unsigned int app_id,
                                     uint64_t set_value)
{
    return set_by_statid(statics_id, app_id, 0, set_value);
}

//��Ҫ����classify_id��app_id��������ֵ���
int ZCE_Server_Status::set_by_statid(unsigned int statics_id,
                                     uint64_t set_value)
{
    return set_by_statid(statics_id, 0, 0, set_value);
}

#endif //_ZCE_LIB_SERVER_STATUS_H_

