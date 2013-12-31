#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_uuid_64_generator.h"

/************************************************************************************************************
Class           : ZCE_UUID64
************************************************************************************************************/
ZCE_UUID64::ZCE_UUID64():
    u_uint64_(0)
{
}

ZCE_UUID64::~ZCE_UUID64()
{
}

// < �����
bool ZCE_UUID64::operator < (const ZCE_UUID64 &others) const
{
    if (this->u_uint64_ < others.u_uint64_)
    {
        return true;
    }

    return false;
}

// == �����
bool ZCE_UUID64::operator == (const ZCE_UUID64 &others) const
{
    if (this->u_uint64_ == others.u_uint64_)
    {
        return true;
    }

    return false;
}

//ת��Ϊ�ַ���
const char *ZCE_UUID64::to_string(char *buffer, size_t buf_len) const
{

    //������ݵ�BUFFER�ռ䲻����ֱ�ӷ���NULL
    if (buf_len < LEN_OF_ZCE_UUID64_STR + 1)
    {
        return NULL;
    }

    int ret = snprintf(buffer, buf_len, "%08x-%08x", this->u_2uint32_[1], this->u_2uint32_[0]);

    if (ret < 0 || ret > static_cast<int>( buf_len) )
    {
        return NULL;
    }

    return buffer;
}

// ת�Ƴ�һ��uint64_t�Ľṹ
ZCE_UUID64::operator uint64_t ()
{
    return this->u_uint64_;
}

/************************************************************************************************************
Class           : ZCE_UUID64_Generator
************************************************************************************************************/

//ʵ��ָ��
ZCE_UUID64_Generator *ZCE_UUID64_Generator::instance_ = NULL;

//���캯��
ZCE_UUID64_Generator::ZCE_UUID64_Generator():
    generator_type_(UUID64_GENERATOR_INVALID)
{
}
ZCE_UUID64_Generator::~ZCE_UUID64_Generator()
{
}

//����������ӣ�ע�����������һ���ֲ�ʽϵͳ�������ӣ�
//ʱ��δ���ر����Ҳ����IP��ַ����õ�һ�����Ӹ���һЩ
void ZCE_UUID64_Generator::random_seed(uint32_t seed)
{
    generator_type_ = UUID64_GENERATOR_RANDOM;
    mt_19937_random_.srand(seed);
    mt_11213b_random_.srand(seed);
}

//����һ��UUID64
ZCE_UUID64 ZCE_UUID64_Generator::random_gen()
{
    ZCE_ASSERT(UUID64_GENERATOR_RANDOM == generator_type_);

    //�ֱ���2��������������õ������
    ZCE_UUID64 uuid64_ret;

    uuid64_ret.u_2uint32_[0] = mt_19937_random_.rand();
    uuid64_ret.u_2uint32_[1] = mt_11213b_random_.rand();

    return uuid64_ret;
}

//��ʱ��Ϊ�����ĳ�ʼ����
void ZCE_UUID64_Generator::time_radix(uint16_t identity, uint32_t radix )
{
    generator_type_ = UUID64_GENERATOR_TIME;

    time_radix_seed_.u_16_48_.data1_ = identity;
    time_radix_seed_.u_16_48_.data2_ = radix * 0x10;
}

//��ʱ��Ϊ��������UUID64
ZCE_UUID64 ZCE_UUID64_Generator::timeradix_gen()
{
    ZCE_ASSERT(UUID64_GENERATOR_TIME == generator_type_);

    ++time_radix_seed_.u_16_48_.data2_;
    return time_radix_seed_;
}

