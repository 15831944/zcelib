/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_uuid_128_generator.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2013��2��4��
* @brief      128Ϊ��UUID��������UUID��
*             Universally Unique Identifier ��дUUID��
* 
* @details    ��2�ֲ���������
*             ������ķ���
*             �����ķ�ʽ����������������Ҳ���ԣ�����ʱ�������Ҳ���ԡ�
* 
* ����
* �ʣ����� ��������
* 
* �ҷԸ����ҵ������������ǣ������
* ����û�������ҵĻ����ұ��Լ��ߵ����
* �����ð���������ȥ��
* Զ�������˺Ž�����������������ʲô��˼��
* ��˵��֪������ʲôҲû������ʲôҲû������
* 
* �ڴ��ſڣ�����ס�ң�
* �ʣ������������Ķ�ȥ�أ��ҵ����ˣ���
* ���Ҳ�֪��������˵����ֻ���뿪������뿪�����
* �뿪�����ǰ�ߣ���ǰ�ߣ�������ҴﵽĿ���Ψһ�취����
* ����ô��֪������Ŀ���ˣ������ʡ�
* ���ǵġ��һش�
* ���Ҹոո������ˣ��뿪������뿪�����������ҵ�Ŀ�ꡣ��
* ������û�д��Ͽ����أ�����˵����ʲô������Ҳ��Ҫ������˵��
* ����;����ô�������������һ·���ҵò���������
* ����һ���ᣬ���ġ�
* 
* ʲô����Ҳ���ܴ���ң�
* ���˵��ǣ������һ�Σ�����û�о�ͷ���ó̰�����
* 
*/
#ifndef ZCE_LIB_UUID_128_GENERATOR_H_
#define ZCE_LIB_UUID_128_GENERATOR_H_

#include "zce_boost_random.h"

/************************************************************************************************************
Class           : ZCE_UUID128
************************************************************************************************************/
///32bit����+32����+64λ�����ı�ʾ����
struct ZCELIB_EXPORT ZCE_UUID128_32_32_64
{

    //
    uint32_t       data1_;
    uint32_t       data2_;
    uint64_t       data3_;
};

///��׼��UUID�ĸ�ʽ
struct ZCELIB_EXPORT ZCE_UUID128_32_16_16_16_48
{
    uint64_t       data1_: 32;
    uint64_t       data2_: 16;
    uint64_t       data3_: 16;
    uint64_t       data4_: 16;
    uint64_t       data5_: 48;
};

///΢���GUID�ĸ�ʽ
struct ZCELIB_EXPORT ZCE_UUID128_32_16_16_64
{
    uint64_t       data1_: 32;
    uint64_t       data2_: 16;
    uint64_t       data3_: 16;
    uint64_t       data4_;

};

/*!
* @brief      UUID128��128λ��Ψһ��ʶ��
*             ��5�б�ʾ������
*             @li 16���ֽ�
*             @li 4��32Ϊ������
*             @li 32λ����+32λ����+64λ�����ı�ʾ����
*             @li 32λ����+16λ����+16λ����+16λ����+48λ��������׼��UUID��ʾ����
*             @li 32λ����+16λ����+16λ����+48λ������΢���UUID��ʾ������GUID��
* 
*/
class ZCELIB_EXPORT ZCE_UUID128
{
public:
    ///���캯��
    ZCE_UUID128();
    ///��������
    ~ZCE_UUID128();

    /// < ���������
    bool operator < (const ZCE_UUID128 &others) const;
    /// == �����
    bool operator == (const ZCE_UUID128 &others) const;

    /// ��UUID8-4-4-4-12�ĸ�ʽ����ת��Ϊ�ַ���
    const char *to_string(char *buffer, size_t buf_len) const;

    /// ת��Ϊ�ַ�����ͬʱ��������ַ����ĳ���
    inline const char *to_string(char *buffer, size_t buf_len, size_t &use_buf) const
    {
        const char *ret_ptr = to_string(buffer, buf_len);
        if (ret_ptr)
        {
            use_buf = LEN_OF_ZCE_UUID128_STR;
        }
        else
        {
            use_buf = 0;
        }
        return 0;
    }

public:

    ///5�����ݱ�ʾ����
    union
    {
        ///16���ֽڵ����
        uint8_t                      u_16uint8_[16];
        ///4��32Ϊ�ֽڵ����
        uint32_t                     u_4uint32_[4];
        ///32λ����+32λ����+64λ����
        ZCE_UUID128_32_32_64         u_32_32_64_;
        ///��׼��UUID�ı�ʶ����
        ZCE_UUID128_32_16_16_16_48   u_32_16_16_16_48_;
        ///΢���GUID�ı�ʶ����
        ZCE_UUID128_32_16_16_64      u_32_16_16_64_;

    };

public:
    ///UUID���ַ�����ʾ�ĳ���
    static const size_t LEN_OF_ZCE_UUID128_STR = 36;
};

/************************************************************************************************************
Class           : ZCE_UUID128_Generator UUID�ķ�����
************************************************************************************************************/

/*!
* @brief      ZCE_UUID128_Generator UUID�ķ�����
* 
*/
class ZCELIB_EXPORT ZCE_UUID128_Generator
{
    //
    enum UUID128_GENERATOR_TYPE
    {
        //
        UUID128_GENERATOR_INVALID,

        ///��������ķ�������
        UUID128_GENERATOR_RANDOM,
        ///���¼���Ϊ��������
        UUID128_GENERATOR_TIME,
    };

public:

    ///���캯��
    ZCE_UUID128_Generator();
    ///��������
    ~ZCE_UUID128_Generator();

public:

    /*!
    * @brief      ע�������������
    * @param      seed ����
    */
    void random_seed(uint32_t seed);
    /*!
    * @brief      ����һ��UUID128��
    * @return     ZCE_UUID128
    */
    ZCE_UUID128 random_gen();

    /*!
    * @brief      ��ʱ��Ϊ�����ĳ�ʼ����radixһ������õ�ǰʱ��time_t����ȻҲ���Բ��ã���������һ����ʶ��
    * @param      identity
    * @param      radix
    */
    void time_radix(uint32_t identity, uint32_t radix = static_cast<uint32_t> (time(NULL)));

    /*!
    * @brief      ��ʱ��Ϊ��������UUID64
    * @return     ZCE_UUID128
    */
    ZCE_UUID128 timeradix_gen();

protected:

    ///
    static ZCE_UUID128_Generator *instance();
    ///
    static void clean_instance();

protected:

    ///������ʹ��ʲô������ʽ
    UUID128_GENERATOR_TYPE        generator_type_;

    ///���������1
    ZCE_LIB::random_mt19937       mt_19937_random_;
    ///���������2
    ZCE_LIB::random_mt11213b      mt_11213b_random_;

    ///
    ZCE_UUID128                   time_radix_seed_;

protected:

    //������ʵ��ָ��
    static ZCE_UUID128_Generator *instance_;

};

#endif // ZCE_LIB_UUID_128_GENERATOR_H_

