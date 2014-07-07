/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_uuid_64_generator.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2013��2��4��
* @brief      64bit��UUID����Ȼ��ʵUUID��ʵ��128bit�ģ�������ǲ���һ��
*             64λ��Ψһ��ʾ��
*             ����ط���Ҫ��Ϊ���ṩ�����һ�ַ���������һ��Ψһ��ʶ��
*             ����Ϸ��Ŀ�飬��Ҫ������Ϸ���߱�ʶ�ȷ�Χ��Ҫ�����Ķ�������Ҫ����
*             ����ĳ����Ʒ����ת�����Ҹ��˶Ե���ʹ�����������ΪID��Ч���ֱ��������
*             Ч����������
*
* @details    ֧�����ֲ�����ʽ�����������ʱ��Ϊ�����ķ�ʽ
*
* @note
*
*/
#ifndef _ZCE_LIB_UUID_64_GENERATOR_H_
#define _ZCE_LIB_UUID_64_GENERATOR_H_

#include "zce_boost_random.h"

/************************************************************************************************************
Class           : ZCE_UUID64
************************************************************************************************************/

#pragma pack(push,1)

/*!
* @brief      UUID64��һ�ֱ�ʾ����
*             ZCE_UUID64_16_48 ��1��16bit����ֵ+48λ����ֵ
*             ��ʾUUID64����������һЩ���ⳡ�ϲ���Ψһ��ʾ��
*/
struct ZCELIB_EXPORT ZCE_UUID64_16_48
{
    uint64_t       data1_: 16;
    uint64_t       data2_: 48;
};

/*!
* @brief      64λ��UUID���ڲ��ֳ�3�ֱ�ʾ������Ҳ�ṩ�˱Ƚ�==��<�Ȳ���
*             @li 64λ������
*             @li 16λ����+48Ϊ������
*             @li 2��32λ������
*
*/
class ZCELIB_EXPORT ZCE_UUID64
{
public:
    ///���캯��
    ZCE_UUID64();
    ///��������
    ~ZCE_UUID64();

    /// < ���������
    bool operator < (const ZCE_UUID64 &others) const;
    /// == �����
    bool operator == (const ZCE_UUID64 &others) const;

    /// ת�Ƴ�һ��uint64_t�Ľṹ
    operator uint64_t();

    /// ת��Ϊ�ַ���
    const char *to_string(char *buffer, size_t buf_len) const;

    /// ת��Ϊ�ַ�����ͬʱ��������ַ����ĳ���
    inline const char *to_string(char *buffer, size_t buf_len, size_t &use_buf) const
    {
        const char *ret_ptr = to_string(buffer, buf_len);
        if (ret_ptr)
        {
            use_buf = LEN_OF_ZCE_UUID64_STR;
        }
        else
        {
            use_buf = 0;
        }
        return 0;
    }

public:

    ///�ṹ���ݵļ��ֲ�ͬ�ı�﷽ʽ
    union
    {
        ///����64�ֽڵı�ʾ����
        uint64_t          u_uint64_;
        ///2��32�ֽڵı�ʾ����
        uint32_t          u_2uint32_[2];
        ///16bit+48Bit�ı�ʾ����
        ZCE_UUID64_16_48  u_16_48_;
    };

public:

    ///UUID����ַ�������󳤶ȣ�������'\0',��ʽXXXXXXXX-XXXXXXXX
    static const size_t   LEN_OF_ZCE_UUID64_STR = 17;

};

#pragma pack(pop)

/************************************************************************************************************
Class           : ZCE_UUID64_Generator
************************************************************************************************************/

/*!
* @brief      UUID64�Ĳ��������ṩ�������������ʱ��Ϊ��������������ķ���
*             @li ���������������һ�������uint64_t������,��Ȼ��ҷ�������ظ��ɺܵ�
*             @li ��ʱ��Ϊ�����Ĳ���������16λ��ֵ���Լ����壬����48λ��������ʱ��Ϊ����
*                 ��Ȼ�����ÿ�β������ۼ�һ�Σ����ַ����ʺ�С��ģ�ķֲ���16λ��ʾ�ֲ���
*                 ��������ÿ���������Լ�����UUID�������ֻҪ1s������UUID������������16
*                 λ���ͱȽϺܰ�ȫ���������ʼ��Ӽ��Ӧ���� sonicmao������Ը���UUID����
*                 ��ʲô����������������
*
*/
class ZCELIB_EXPORT ZCE_UUID64_Generator
{
    //UUID����������
    enum UUID64_GENERATOR_TYPE
    {
        ///��Ч�Ĳ�����ʽ
        UUID64_GENERATOR_INVALID,

        ///��������ķ�������
        UUID64_GENERATOR_RANDOM,
        ///���¼���Ϊ��������
        UUID64_GENERATOR_TIME,
    };

public:

    /*!
    * @brief      ���캯��
    */
    ZCE_UUID64_Generator();

    /*!
    * @brief      ��������
    */
    ~ZCE_UUID64_Generator();

public:

    /*!
    * @brief      �����������,�������������
    *             �ͻ�ȷ�ϲ�����ʽ��UUID64_GENERATOR_RANDOM�����ܸ����ˡ�
    * @param      seed �����������,
    */
    void random_seed(uint32_t seed);
    /*!
    @brief      ͨ�����������һ��UUID64
    @return     ZCE_UUID64 ������UUID
    */
    ZCE_UUID64 random_gen();

    /*!
    * @brief      ��ʱ��Ϊ�����ĳ�ʼ����radixһ������õ�ǰʱ��time_t
    *             ������������󣬾ͻ�ȷ�ϲ�����ʽ��UUID64_GENERATOR_TIME�����ܸ����ˡ�
    * @param      identity ��ǰ��Ψһ��ʾ�����������ID����Ϣ
    * @param      radix    ʱ�����
    */
    void time_radix(uint16_t identity, uint32_t radix = static_cast<uint32_t> (time(NULL)));

    /*!
    * @brief      ��ʱ��Ϊ��������UUID64
    * @return     ZCE_UUID64 ������UUID64
    */
    ZCE_UUID64 timeradix_gen();

protected:

    /*!
    * @brief      ���Ӻ���
    * @return     ZCE_UUID64_Generator* ���ص�ʵ��ָ��
    */
    static ZCE_UUID64_Generator *instance();
    /*!
    * @brief      ����ʵ��ָ��
    */
    static void clean_instance();

protected:

    ///������ʹ��ʲô������ʽ
    UUID64_GENERATOR_TYPE         generator_type_;

    ///���������1
    ZCE_LIB::random_mt19937       mt_19937_random_;
    ///���������2
    ZCE_LIB::random_mt11213b      mt_11213b_random_;

    ///�����������
    ZCE_UUID64                    time_radix_seed_;

protected:

    ///������ʵ��ָ��
    static ZCE_UUID64_Generator  *instance_;

};

#endif //# ZCE_LIB_UUID_64_GENERATOR_H_

