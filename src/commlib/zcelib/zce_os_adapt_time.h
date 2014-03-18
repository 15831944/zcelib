/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_os_adapt_time.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2011��5��1��
* @brief      ʱ��������������㣬��Ҫ������LINUX�¿�£
* 
* @details
* 
* @note       timeval�����׿��ڲ�ʹ�ý϶࣬�������㳹��������ʶ�ڲ���ʱ�䣬�����������������Լ���Time_Value�࣬
*             ��������Ϊ���д����ʱ��timeval�ĺܶ��������װд�������Ƿ�����
*/

#ifndef ZCE_LIB_OS_ADAPT_TIME_H_
#define ZCE_LIB_OS_ADAPT_TIME_H_

#include "zce_os_adapt_predefine.h"

//Ϊʲô��������ACE�������ޣ��������ޣ��������������������������ޣ���������������

//һ��Сʱ��ʱ��
static const time_t ONE_HOUR_SECONDS      = 3600;
//
static const time_t ONE_QUARTER_SECONDS   = 15 * 60;

static const time_t FIVE_MINUTE_SECONDS   = 5 * 60;

//һ�������
static const time_t ONE_DAY_SECONDS       = 86400;

#if defined ZCE_OS_WINDOWS

struct timezone
{
    //minutes west of Greenwich
    int tz_minuteswest;
    // type of DST correction ������ʱ�����ֵ��LINUX�¶�����ʹ���ˣ��������Ұ�
    //int tz_dsttime;
};

#endif

namespace ZCE_OS
{

/*!
* @brief      �Ǳ�׼�������õ����������������ڵ�ʱ�䣬���ʱ���Ǹ����Ե�����ֵ���������
* @detail     ԭ��ģ��ĺ�����gethrtime����Ϊ�����䷵�ص�λ�ȽϹ֣����Ƶõ�CPU�����������ڵ�TICK��ʱ�䣬
*             ����pascal���˸��˺������ƣ�Ҳ����Ϊ��һ������������
* @return     const timeval
* @note       WINDOWS (��֧��WIN2008�Ļ����ϣ���������) ʹ�õ���GetTickCount ����������������غ���������
*             49��ͻ�������ڲ�Ϊ����һ�ε��õ�ʱ�䣬����static ��������Ϊ�˱���static������������������Ҫ
*             ��֤����Ƶ�����⡣������ε���֮���ʱ���������������49�죬���޷���֤��õ�׼ȷ��ֵ
*             ���ϲ�Ҫ49���ֻ����һ���������ѽ�������ұ�֤�������TICK��Ч������������ÿ�����һ�ΰɡ�
*             LINUX �£����Ժ��ֺܶ�ϵͳû��gethrtime��������POSIX���º���clock_gettime���档
*/
const timeval  get_uptime(void);

/*!
* @brief      ȡ�õ�ǰ��ʱ�䣬��ǽ��ʱ�ӣ�
* @return     inline int  ==0��ʾ�ɹ�
* @param      [out]tv     ��ǰ��ʱ��
* @param      [out]tz     ʱ�����ڲ�û�д�������ʱ���⣬LINUX��ϵͳĿǰҲ��֧����
* @note       gettimeofday ���������POSIX���Ѿ������������Ǵ�����LINUX�������ʹ�ã�������ʱ��������
*             POSIX�Ƽ���clock_gettime����û�п�������������
*/
inline int gettimeofday(struct timeval *tv, struct timezone *tz = NULL);

//
/*!
* @brief      ȡ�õ�ǰ��ʱ��
* @return     inline const timeval ����ǰʱ��timeval��Ϊ�������
*/
inline const timeval gettimeofday();

///ʱ����ַ����ĳ��ȣ�������'\0',������ַ�����������������
static const size_t LEN_OF_TIME_STAMP = 26;

///ISO ʱ���ʽ���ַ����ĳ���
///����ַ������ȵ����ڵ��ַ�������
static const size_t LEN_OF_ISO_DAY_TIMESTRING  = 10;
///����ַ������ȵ�����ַ�������
static const size_t LEN_OF_ISO_SEC_TIMESTRING  = 19;
///����ַ������ȵ�΢����ַ�������
static const size_t LEN_OF_ISO_USEC_TIMESTRING = 26;

///US �׹�ʱ���ʽ�ַ����ĳ���
///����ַ������ȵ����ڵ��ַ�������
static const size_t LEN_OF_US_DAY_TIMESTRING  = 15;
///����ַ������ȵ�����ַ�������
static const size_t LEN_OF_US_SEC_TIMESTRING  = 24;
///����ַ������ȵ�΢����ַ�������
static const size_t LEN_OF_US_USEC_TIMESTRING = 31;

///ʱ���ʽ������ĸ�ʽ����
enum TIME_STR_FORMAT_TYPE
{
    ///��ӡ����ʱ��
    TIME_STRFFMT_LOCALTIME      = 0x1,
    ///��ӡUTCʱ��
    TIME_STRFFMT_UTCTIME        = 0x2,

    ///��ISO�ĸ�ʽ����ʱ�����
    TIME_STRFFMT_ISO            = 0x10,
    ///��������ʱ���ʽ�������
    TIME_STRFFMT_US             = 0x20,

    ///��ӡʱ�侫�ȵ�����
    TIME_STRFFMT_PRECISION_DAY  = 0x100,
    ///��ӡʱ�侫�ȵ�����
    TIME_STRFFMT_PRECISION_SEC  = 0x200,
    ///��ӡʱ�侫�ȵ�΢��
    TIME_STRFFMT_PRECISION_USEC = 0x400,

};

/*!
* @brief      ������timeval��ֵ��Ϊ��ʱ����ʽ���������ӡ����
*             ʱ�����ӡ����,�ַ����������ʽ��ISO-8601 format. ���� 2010-09-10 10:03:18.100190
*             �����ʱ�����ʽΪ2010-09-10 10:03:18.100190       ע��ĩβ����һ��\0
*                            123456789012345678901234567890
* @return     const char*         �õ���ʱ���ַ���
* @param[out] str_date_time  �ַ���
* @param[in]  datetime_strlen �ַ�������
* @note
*/
const char *timestamp (const timeval *timeval,
                       char *str_date_time,
                       size_t datetime_strlen);

/*!
* @brief      �õ���ǰ��ϵͳʱ���ַ������
* @return     const char*
* @param      str_date_time
* @param      datetime_strlen
* @note
*/
const char *timestamp (char *str_date_time,
                       size_t datetime_strlen);

/*!
* @brief      ������timeval��ֵ��Ϊ��ʱ����ʽ���������ӡ���������Կ��Ƹ��ָ�ʽ���
* @return     const char*
* @param[in]  timeval         �õ���ʱ���ַ���
* @param[out] str_date_time
* @param[in]  datetime_strlen
* @param      fromat_type     ������ο�@ref TIME_STR_FORMAT_TYPE ��������|�򣬿���������ֳ��ȡ����룬���ָ�ʽ
* @note       ʱ�����ӡ��ʽ˵��,
*             �ַ����������ʽ��ISO-8601 format. @ref TIME_STRFFMT_ISO
*             �����ʱ�����ʽΪ2010-09-10 10:03:18.100190       ע��ĩβ����һ��\0
*                           123456789012345678901234567890
*             �������ʽʱ���ʽ����ô��ʵ�� ����-��-��-�� ʱ���֣���.΢��   @ref TIME_STRFFMT_US
*             �����ʱ�����ʽΪFri Aug 24 2002 07:43:05.100190       ע��ĩβ����һ��\0
*                            12345678901234567890123456789012
*/
const char *timestamp_ex(const timeval *timeval,
                         char *str_date_time,
                         size_t datetime_strlen,
                         int fromat_type = ZCE_OS::TIME_STRFFMT_LOCALTIME |
                         ZCE_OS::TIME_STRFFMT_ISO | 
                         ZCE_OS::TIME_STRFFMT_PRECISION_USEC);

/*!
* @brief      ����timeval�ڲ��ܼ��Ƕ��ٺ���
* @return     uint64_t
* @param      tv  Ҫ�����timeval
*/
uint64_t total_milliseconds(const timeval &tv);

/*!
* @brief      ����timeval�ڲ��ܼ��Ƕ���΢��
* @return     uint64_t
* @param      tv  Ҫ�����timeval
*/
uint64_t total_microseconds(const timeval &tv);

//��չtimeval��һЩ������û��֮�����ز������ŵ�ԭ���Ǳ�����Ⱦ�����ռ�

/*!
* @brief      ����һ��0,0�ĵ�timeval�Ľṹ��
* @return     const timeval Ҫ�����timeval
*/
const timeval timeval_zero();

/*!
* @brief      ��tv����Ϊ0
* @param      tv  ��0��timeval
*/
void timeval_clear(timeval &tv);

/*!
* @brief      �Ƚ�ʱ���Ƿ�һ��,
* @return     int     ���һ�·���0��left�󣬷���������right�󷵻ظ���,
* @param      left    ��ֵ
* @param      right   ��ֵ
* @note       ע������timercmp�ǲ�һ���Ĺ��ܣ�timercmp��Ҫ���������һ������������ɱȽ�����
*/
int timeval_compare(const timeval &left, const timeval &right);

/*!
* @brief      ������ʱ�������ӣ���������أ��Ǳ�׼����
* @return     const timeval
* @param      left          ��ֵ
* @param      right         ��ֵ
* @note
*/
const timeval timeval_add(const timeval &left, const timeval &right);

/*!
* @brief      ������ʱ������������������أ��Ǳ�׼����,safe==true���С��0������0
* @return     const timeval ����Ľ��
* @param      left          ��
* @param      right         ��
* @param      safe          �Ƿ���а�ȫ������������б��������С��0ʱ������0
* @note
*/
const timeval timeval_sub(const timeval &left, const timeval &right, bool safe = true);

/*!
* @brief      ��������õ�usec �ܳ���>1s���Ұ�����������Ǳ�׼����
* @return     void
* @param      tv      ������timeval
*/
void timeval_adjust(timeval &tv);

/*!
* @brief      ������TIMEVALUE�Ƿ���ʣ���ʱ�䣬�Ǳ�׼����
* @return     bool
* @param      tv
*/
bool timeval_havetime(const timeval &tv);

/*!
* @brief      ����timeval����ṹ
* @return     const timeval
* @param      sec           ��
* @param      usec          ΢��
*/
const timeval make_timeval(time_t sec, time_t usec);

/*!
* @brief      ������Ϊstd::clock_tֵ ת���õ�timeval����ṹ
* @return     const timeval ת�����timeval���
* @param      clock_value   ����ת���Ĳ���
*/
const timeval make_timeval(std::clock_t clock_value);

/*!
* @brief      ��timespec�ṹת���õ�timeval�ṹ
* @return     const timeval ת�����timeval���
* @param      timespec_val  ����ת���Ĳ���
*/
const timeval make_timeval(const ::timespec *timespec_val);

//WINDOWS API���õļ�������
#if defined (ZCE_OS_WINDOWS)

/*!
* @brief      ��FILETIME�Ĳ�����Ϊһ��ʱ�䣨����ʱ�� ��2013-01-01 01:53:29����ת���õ�timeval
* @return     const timeval
* @param      file_time
*/
const timeval make_timeval(const FILETIME *file_time);

/*!
* @brief      ת��SYSTEMTIME��timeval
* @return     const timeval
* @param      system_time
* @note
*/
const timeval make_timeval(const SYSTEMTIME *system_time);

/*!
* @brief      ��FILETIME�Ĳ�����Ϊһ��ʱ�������ʱ�� ��25s����ת��FILETIME��timeval ��
* @return     const timeval
* @param      file_time
*/
const timeval make_timeval2(const FILETIME *file_time);

#endif

//�������timespec������̫��֧�֣�

/*!
* @brief
* @return     int
* @param      clk_id
* @param      ts
*/
int clock_gettime (clockid_t clk_id, timespec *ts);

/*!
* @brief      ����timespec�ڲ��ܼ��Ƕ��ٺ���
* @return     uint64_t
* @param      ts
*/
uint64_t total_milliseconds(const timespec &ts);

/*!
* @brief      ��timespec �ṹ��ת���õ�timeval����ṹ
* @return     const ::timespec
* @param      timeval_val
*/
const ::timespec make_timespec(const timeval *timeval_val);

//-------------------------------------------------------------------------------
//��Ϣһ�£���Ϣһ��

/*!
* @brief      SLEEP ��
* @return     int     0�ɹ���-1ʧ��
* @param      seconds ��Ϣ������
* @note
*/
int sleep (uint32_t seconds);

/*!
* @brief      SLEEP timeval��ʱ��
* @return     int  0�ɹ���-1ʧ��
* @param      tv   ��Ϣ��timevalʱ�䳤�ȵ�ʱ��
* @note
*/
int sleep (const timeval &tv);

/*!
* @brief      ��Ϣ΢�룬��ʵ����Ϣʱ�䣬�϶�����������Ǻ�
* @return     int   0�ɹ���-1ʧ��
* @param      usec  ΢��ʱ�䳤��
*/
int usleep (unsigned long usec);

//-------------------------------------------------------------------------------
//���밲ȫ��ʱ��ת�������ĺ���

/*!
* @brief      �õ�����ʱ���tm�ṹ
* @return     inline struct tm*
* @param      timep
* @param      result
* @note       ���밲ȫ
*/
inline struct tm *localtime_r(const time_t *timep, struct tm *result);

/*!
* @brief      ����GMʱ�䣬�õ�tm�ṹ
* @return     inline struct tm*
* @param      timep
* @param      result
* @note
*/
inline struct tm *gmtime_r(const time_t *timep, struct tm *result);

/*!
* @brief      ��ӡTM�ڲ���ʱ����Ϣ
* @return     inline char*
* @param      tm
* @param      buf            ������ַ������ȣ��������Լ���֤����26��
* @note       ����ַ����ĸ�ʽ��Wed Jan 02 02:03:55 1980\n\0.ǿ�Ҳ�����ʹ��,
*             ������ʵ�ڲ�֪��������ʱ�������ɶ�ô�����ȫ��Ϊ�˼��ݲ�д������������
*/
inline char *asctime_r(const struct tm *tm, char *buf);

/*!
* @brief      ��ӡtime_t����ʾ��ʱ����Ϣ
* @return     inline char* ���ص�ʱ���ַ���
* @param      timep        ʱ��
* @param      buf          ������ַ������ȣ��������Լ���֤����26��
* @note       ����ַ����ĸ�ʽ��Wed Jan 02 02:03:55 1980\n\0.ǿ�Ҳ�����ʹ��,
*             ������ʵ�ڲ�֪��������ʱ�������ɶ�ô�����ȫ��Ϊ�˼��ݲ�д������������
*/
inline char *ctime_r(const time_t *timep, char *buf);

//-------------------------------------------------------------------------------

//��ȡTSC��Read Time-Stamp Counter
uint64_t rdtsc();

//-------------------------------------------------------------------------------
};

//����LINUX�µ�gettimeofday
inline int ZCE_OS::gettimeofday(struct timeval *tv, struct timezone *tz)
{
    //
#if defined ZCE_OS_WINDOWS
    //
    if (tv == NULL && tz == NULL)
    {
        return 0;
    }

    //���������ʱ��
    if (tv)
    {
        //�õ�ϵͳʱ��,
        //���ԱȽ�::GetSystemTime �����������ʱ������������е����
        FILETIME   tfile;
        ::GetSystemTimeAsFileTime (&tfile);

        ULARGE_INTEGER ui;
        ui.LowPart = tfile.dwLowDateTime;
        ui.HighPart = tfile.dwHighDateTime;

        //The FILETIME structure is a 64-bit value representing the number of
        //100-nanosecond intervals since January 1, 1601.

        //�õ�time_t����
        tv->tv_sec = static_cast<long>((ui.QuadPart - 116444736000000000) / 10000000);
        //�õ�΢�벿�֣�FILETIME��ŵ���100-nanosecond
        tv->tv_usec = static_cast<long>(((ui.QuadPart - 116444736000000000) % 10000000) / 10);
    }

    //�õ�ʱ���������
    if (tz)
    {
        //�õ�ʱ����Ϣ
        TIME_ZONE_INFORMATION tzone;
        DWORD  ret = ::GetTimeZoneInformation(&tzone);

        if (ret == TIME_ZONE_ID_INVALID)
        {
            return -1;
        }

        //
        tz->tz_minuteswest = tzone.Bias;
        //����ʱ����رܣ�LINUX���Ѿ����������������

    }

    return 0;
#endif //

    //LINUX�µõ�ʱ��
#if defined ZCE_OS_LINUX
    //ֱ�ӵ���ϵͳ��
    return ::gettimeofday(tv, tz);
#endif //

}

inline const timeval ZCE_OS::gettimeofday()
{
    timeval now_time;
    ZCE_OS::gettimeofday(&now_time);
    return now_time;
}

//�õ�����ʱ��
inline struct tm *ZCE_OS::localtime_r(const time_t *timep, struct tm *result)
{
#if defined (ZCE_OS_WINDOWS)
    //WINDOWS��ʹ��Ĭ�ϵ�_s ϵ�е�API
    errno_t convert_err =  ::localtime_s(result, timep);

    if (convert_err)
    {
        return NULL;
    }
    else
    {
        return result;
    }

#endif //#if defined (ZCE_OS_WINDOWS)

#if defined (ZCE_OS_LINUX)
    return ::localtime_r(timep, result);
#endif //#if defined (ZCE_OS_LINUX)
}

//GMʱ��
inline struct tm *ZCE_OS::gmtime_r(const time_t *timep, struct tm *result)
{
#if defined (ZCE_OS_WINDOWS)
    //WINDOWS��ʹ��Ĭ�ϵ�_s ϵ�е�API
    errno_t convert_err =  ::gmtime_s(result, timep);

    if (convert_err)
    {
        return NULL;
    }
    else
    {
        return result;
    }

#endif //#if defined (ZCE_OS_WINDOWS)

#if defined (ZCE_OS_LINUX)
    return ::gmtime_r(timep, result);
#endif //#if defined (ZCE_OS_LINUX)
}

//��ӡTM�ڲ���ʱ����Ϣ
inline char *ZCE_OS::asctime_r(const struct tm *tm_data, char *buf)
{
#if defined (ZCE_OS_WINDOWS)

    //������Ҫ��֤buf������26���ַ�,����д��ʵ�����Σ���ΪLINUX��API��û�б�֤�����İ�ȫ��ֻ�ǽ������������
    //�����еķ���WINDOWS��API����LINUX
    const size_t I_GUESS_BUF_HAVE_ROOM_FOR_AT_LEAST_26_BYTES = 26;

    //WINDOWS��ʹ��Ĭ�ϵ�_s ϵ�е�API
    errno_t convert_err =  ::asctime_s(buf, I_GUESS_BUF_HAVE_ROOM_FOR_AT_LEAST_26_BYTES, tm_data);

    if (convert_err)
    {
        return NULL;
    }
    else
    {
        return buf;
    }

#endif //#if defined (ZCE_OS_WINDOWS)

#if defined (ZCE_OS_LINUX)
    return ::asctime_r(tm_data, buf);
#endif //#if defined (ZCE_OS_LINUX)
}

//��ӡtime_t����ʶ��ʱ����Ϣ
inline char *ctime_r(const time_t *timep, char *buf)
{
#if defined (ZCE_OS_WINDOWS)

    //������Ҫ��֤buf������26���ַ�,����ΪLINUX��API��û�б�֤�����İ�ȫ��ֻ�ǽ������������
    const size_t I_GUESS_BUF_HAVE_ROOM_FOR_AT_LEAST_26_BYTES = 26;

    //WINDOWS��ʹ��Ĭ�ϵ�_s ϵ�е�API
    errno_t convert_err =  ::ctime_s(buf, I_GUESS_BUF_HAVE_ROOM_FOR_AT_LEAST_26_BYTES, timep);

    if (convert_err)
    {
        return NULL;
    }
    else
    {
        return buf;
    }

#endif //#if defined (ZCE_OS_WINDOWS)

#if defined (ZCE_OS_LINUX)
    return ::ctime_r(timep, buf);
#endif //#if defined (ZCE_OS_LINUX)
}

#endif //ZCE_LIB_SHARE_MEM_PRE_DEFINE_H_

