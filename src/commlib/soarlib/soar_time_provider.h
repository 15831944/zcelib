/// @file       soar_time_provider.h
/// @date       2012/08/13 11:58
///
/// @author     yunfeiyang
///
/// @brief      ʱ���ṩ���
///
#ifndef SOARING_LIB_TIME_PROVIDER_H_
#define SOARING_LIB_TIME_PROVIDER_H_

// ʱ���ṩ��, ��Ҫ����������£����ܺã����Ȳ��ߣ���Ҫ��Ϊ��
// �����ʱ�侫��Ҫ�󲻸ߵ�����ҪƵ��ȡʱ�������
class Comm_Time_Provider
{
public:
    // ����
    static Comm_Time_Provider *instance();
    static void clean_instance();

    // ��ȡ��ǰʱ��
    inline time_t time() const
    {
        return cur_time_.sec();
    }

    inline const char *get_str_time() const
    {
        return str_time_[str_time_idx_];
    }

    // ��ȡ��ǰʱ��
    inline timeval gettimeofday() const
    {
        return cur_time_;
    }

    // ��ܸ���ʱ��
    void update(const ZCE_Time_Value &now);

private:
    Comm_Time_Provider();
    ~Comm_Time_Provider();
    // ��ǰʱ��
    ZCE_Time_Value cur_time_;

    static const unsigned int STR_TIME_LEN = 32;

    char str_time_[2][STR_TIME_LEN];
    // ��volatile��Ϊ�˷�ֹ�Ĵ�������
    volatile unsigned int str_time_idx_;

    // ����
    static Comm_Time_Provider *instance_;
};

#endif // SOARING_LIB_TIME_PROVIDER_H_

