/// @file       arbiter_flow_stat.h
/// @date       2012/07/19
///
/// @author     stefzhou
///
/// @brief      ͳ�ƴ���
///

#ifndef ARBITER_FLOW_STAT_H_
#define ARBITER_FLOW_STAT_H_

// ����svc_type��svr_id����64λkey, ͬ������ͳ��
#define MAKE_STAT_KEY(svc_type, svc_id) ((((uint64_t)svc_type) << 32) + ((uint64_t)(svc_id)))

/******************************************************************************************
�˵���Ϣ
******************************************************************************************/
enum ProxyLogType
{
    LOGTYPE_CHANNEL_FLOW_STAT = 1,
};

/******************************************************************************************
ͨ����Ϣ, ÿ��ͨ����<from_type, to_type>ȷ��
******************************************************************************************/
struct ChannelInfo
{
public:
    // ���ݰ�Դtype
    unsigned short  from_type_;

    // ���ݰ�Ŀ��type
    unsigned short  to_type_;

public:
    ChannelInfo(unsigned short from_type, unsigned short to_type);
    ~ChannelInfo();

    // ��Ҫ����==������
    bool operator ==(const ChannelInfo &others) const;

    // ��Ҫ����!=������
    bool operator !=(const ChannelInfo &others) const;

    // ��Ҫ����<������
    bool operator <(const ChannelInfo &others) const;
};

// �õ�KEY��HASH����
class KeyofChannelInfo
{
public:
    // ����������ĳ�size_t���б���澯��
    size_t operator()(const ChannelInfo &channel_info) const
    {
        return (((unsigned int)(channel_info.from_type_)) << 16) + channel_info.to_type_;
    }
};

typedef hash_map<ChannelInfo, size_t, KeyofChannelInfo> ChannelFlowInfo;

class ArbiterFlowStat
{
public:
    // ����ʵ������
    static ArbiterFlowStat *instance();

    // ������ʵ��
    static void clean_instance();

private:
    static ArbiterFlowStat *instance_;

    ArbiterFlowStat();
    ~ArbiterFlowStat();

public:
    // stat�˵����
    void write_flow_stat();

    // ����ͨ�����ݰ���
    void increase_channel_pkg_num(unsigned short from_type, unsigned short to_type);

public:
    // ͨ������ͳ��
    ChannelFlowInfo  channel_flow_info_;
};



// ����ͳ��ÿ������id��������
class ArbiterSendStat
{
public:
    // ����ʵ������
    static ArbiterSendStat *instance();

    // ������ʵ��
    static void clean_instance();


public:
    // ����������
    void send_flow_increase_once(uint32_t svc_type, uint32_t svc_id);

    // ͳ�����
    void show_flow_info(void);

private:
    static ArbiterSendStat *instance_;

    ArbiterSendStat();
    ~ArbiterSendStat();

private:
    // ����ͳ��ͨ�����ز�
    struct SendIdStatInfo
    {
        uint64_t max_val_;
        uint64_t min_val_;

        SendIdStatInfo()
        {
            max_val_ = 0ULL;
            min_val_ = -1ULL;
        }
    };

    // ����ͨ��ͳ��
    typedef hash_map<uint64_t, uint64_t> SendFlotStatType;

    // ���ͳ��
    typedef hash_map<uint64_t, SendIdStatInfo> SendFlotCalType;

    // ͳ�ƴ洢�ṹ
    SendFlotStatType flow_stat_;

    SendFlotCalType cal_stat_;

};




#endif // ARBITER_FLOW_STAT_H_
