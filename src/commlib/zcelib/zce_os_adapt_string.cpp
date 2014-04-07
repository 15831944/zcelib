//
//Author:ZengXing
//����ASNI string�����Ĳ���
//2002-06-22
//


#include "zce_predefine.h"
#include "zce_os_adapt_process.h"
#include "zce_trace_log_debug.h"
#include "zce_os_adapt_string.h"












//==========================================================================================================
//ȡ��һ��Ψһ������,����һЩ��ҪȡΨһ���ֵĵط���objectһ��ѡȡһЩָ�뿼��
char *ZCE_OS::object_unique_name (const void *object_ptr,
                                  char *name,
                                  size_t length)
{
    snprintf (name,
              length,
              "%u.%p",
              static_cast <int> (ZCE_OS::getpid ()),
              object_ptr
             );

    return name;
}


//ͨ��ǰ׺ʽ���õ�һ��Ψһ������,
char *ZCE_OS::prefix_unique_name(const char *prefix_name,
                                 char *name,
                                 size_t length)
{
    static unsigned int uniqueid_builder = 0;

    ++uniqueid_builder;

    snprintf (name,
              length,
              "%s.%u.%x",
              prefix_name,
              static_cast <int> (ZCE_OS::getpid ()),
              uniqueid_builder
             );

    return name;
}



//==========================================================================================================
//���ַ���ȫ��ת��Ϊ��д�ַ�
char *ZCE_OS::strupr(char *str)
{

    assert(str);
    char *lstr = str;

    while (*lstr != '\0')
    {
        *lstr = static_cast<char> (::toupper(*lstr));
        ++lstr;
    }

    return str;
}

//���ַ���ȫ��ת��ΪСд�ַ�
char *ZCE_OS::strlwr(char *str)
{

    assert(str);
    char *lstr = str;

    while (*lstr++ != '\0')
    {
        *lstr = static_cast<char>(::tolower(*lstr));
    }

    return str;
}

//�ַ����Ƚϣ����Ӵ�Сд
//��Ч�汾
int ZCE_OS::strcasecmp(const char *string1, const char *string2)
{
#if defined (ZCE_OS_WINDOWS)
    return ::strcasecmp(string1, string2);
#elif defined (ZCE_OS_LINUX)
    return ::strcasecmp(string1, string2);
#endif

}

//�ַ��������Ƚϣ����Ӵ�Сд
//��Ч�汾
int ZCE_OS::strncasecmp(const char *string1, const char *string2, size_t maxlen)
{
#if defined (ZCE_OS_WINDOWS)
    return ::strncasecmp(string1, string2, maxlen);
#elif defined (ZCE_OS_LINUX)
    return ::strncasecmp(string1, string2, maxlen);
#endif
}

//==========================================================================================================
//ԭ���Ĵ��������˶�ASCII 255���ַ��������ж�����ASCII255��Non-breaking space��
//����ʱȥ���ˡ��о�������������������������Ϊ��ʱû��дע�ͣ�������ȫ���ǵó����ˡ�

//������ַ�����ȥ���ַ�����ߵĿո񣬻��У��س���Tab
char *ZCE_OS::strtrimleft(char   *str)
{
    ZCE_ASSERT(str);

    char *lstr = str;

    for (; *lstr != '\0'; lstr++)
    {
        //ע��isspace�Ĳ�����int,��WIN�£�Ҫ���ַ���0~255,
        if (::isspace(static_cast<unsigned char>(*lstr)) != 0 )
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if (lstr != str)
    {
        memmove(str, lstr, strlen(lstr) + 1);
    }

    return str;
}

//�ҹ����ַ�����ȥ���ַ����ұߵĿո񣬻��У��س���Tab
char *ZCE_OS::strtrimright(char *str)
{
    ZCE_ASSERT(str);
    char *lstr = str + strlen(str) - 1;

    for (; lstr >= str; lstr--)
    {
        //ע��isspace�Ĳ�����int,��WIN�£�Ҫ���ַ���0~255,
        if (::isspace(static_cast<unsigned char> (*lstr)) != 0 )
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if (lstr != str + strlen(str) - 1)
    {
        *++lstr = '\0';
    }

    return str;
}

//�����ַ�����ȥ���ַ������ߵĿո񣬻��У��س���Tab
char *ZCE_OS::strtrim(char *str)
{
    ZCE_OS::strtrimleft(str);
    ZCE_OS::strtrimright(str);
    return str;
}


//-------------------------------------------------------------------------------------------------------------------
//��Խ�հ׷���ָ�ո�ˮƽ�Ʊ���ֱ�Ʊ���ҳ���س��ͻ��з��������ַ�����Խ��
const char *ZCE_OS::skip_whitespace (const char *str)
{
    while (::isspace(static_cast<unsigned char>(*str)))
    {
        ++str;
    }

    return str;
}

//��Խĳ��token
const  char *ZCE_OS::skip_token(const char *str)
{
    while (::isspace(static_cast<unsigned char>(*str)))
    {
        ++str;
    }

    while ( *str && 0 == ::isspace(static_cast<unsigned char>(*str)))
    {
        ++str;
    }

    //����Ŀո�Ҫ��Ҫ���������ˣ�������һ����
    //while (::isspace(static_cast<unsigned char>(*str)))
    //{
    //    ++str;
    //}
    return str;
}

//��Խһ��
const char *ZCE_OS::skip_line(const char *str)
{
    while ( ('\n' != (*str)) && ('\0' != (*str)) )
    {
        ++str;
    }

    //����ǻ��з���ǰ��һ��
    if ('\n' == (*str) )
    {
        ++str;
    }

    return str;
}



//��Խı���ָ�����
const char *ZCE_OS::skip_separator(const char *str, char separator_char)
{
    while ( (separator_char != (*str)) && ('\0' != (*str)) )
    {
        ++str;
    }

    //����ǻ��з���ǰ��һ��
    if (separator_char == (*str) )
    {
        ++str;
    }

    return str;
}




//==========================================================================================================

//���Դ�ӡ�ڴ���Ϣ�����Ǽ򵥵��ڴ淭��Ϊ16�����ַ���
void ZCE_OS::memory_debug(FILE *stream, const unsigned char *mem, size_t mem_len)
{
    fprintf(stream, "DEBUG memory[%p][%lu]", mem, mem_len);
    for (size_t i = 0; i < mem_len; ++i)
    {
        fprintf(stream, "%02x", mem[i]);
    }
    fprintf(stream, "\n");
}


//�� 11 02 03 0E E0         ..... ��ʽ�������ָ����Ϣ�����Դ�ӡ�ڴ���Ϣ
void ZCE_OS::memory_debug_ex(FILE *stream, const unsigned char *mem, size_t mem_len)
{
    //60���ַ�����
    const unsigned int LINE_OUT_NUM = 60;

    unsigned char ascii_str[LINE_OUT_NUM + 1];
    ascii_str[LINE_OUT_NUM] = '\0';
    size_t j = 0;
    for (size_t i = 0; i < mem_len ; ++i, ++j)
    {
        //����
        if (i % LINE_OUT_NUM == 0 && i != 0  )
        {
            fprintf(stream, "  %s\n", ascii_str);
            //��ͷ��ʼ��¼
            j = 0;
        }
        unsigned char bytmp = *(mem + i);
        fprintf(stream, "%02X ", bytmp);


        //ֻ��������ʾ���ַ��������ַ�����Ϊ'.'
        if (bytmp <= 0x20 || bytmp >= 0xFA )
        {
            bytmp = '.';
        }
        ascii_str [j] = bytmp;
    }

    //�������LINE_OUT_NUM ����������Ҫ���룬��������ַ���
    if (mem_len % LINE_OUT_NUM != 0 )
    {
        //Ϊ�˶��룬��ӡ�ո�
        for (size_t k = 0; k < LINE_OUT_NUM - mem_len % LINE_OUT_NUM; k++)
        {
            fprintf(stream, "%s", "   ");
        }

        ascii_str[j] = '\0';
        fprintf(stream, "  %s\n", ascii_str);
    }
}


//==========================================================================================================

//�����ڴ濽������Ȼ��ʵ��������飬
//�����������ģ��������ԣ�����ʵ��û��memcpy�죬���Բ�����ʹ��
void *ZCE_OS::fast_memcpy(void *dst, const void *src, size_t sz)
{
    void *r = dst;

    //�Ƚ���uint64_t���ȵĿ�����һ����ԣ��ڴ��ַ���Ƕ���ģ�
    size_t n = sz & ~(sizeof(uint64_t) - 1);
    uint64_t *src_u64 = (uint64_t *) src;
    uint64_t *dst_u64 = (uint64_t *) dst;

    while (n)
    {
        *dst_u64++ = *src_u64++;
        n -= sizeof(uint64_t);
    }

    //��û�з�8�ֽ��ֳ�ȡ���Ĳ���copy
    n = sz & (sizeof(uint64_t) - 1);
    uint8_t *src_u8 = (uint8_t *) src;
    uint8_t *dst_u8 = (uint8_t *) dst;
    while (n-- )
    {
        (*dst_u8++ = *src_u8++);
    }

    return r;
}

//�����ڴ濽���ĵڶ����汾����ʵ�����ڸ��Ƶ�ʱ��������һ�θ��ƣ������Ż�һ��
//���Ҳû��memcpy��
void *ZCE_OS::fast_memcpy2(void *dst, const void *src, size_t sz)
{
    void *r = dst;

    //�Ƚ���uint64_t���ȵĿ�����һ����ԣ��ڴ��ַ���Ƕ���ģ�
    size_t n = sz & ~((sizeof(uint64_t)<< 1)  - 1);
    uint64_t *src_u64 = (uint64_t *) src;
    uint64_t *dst_u64 = (uint64_t *) dst;

    while (n)
    {
        *dst_u64++ = *src_u64++;
        *dst_u64++ = *src_u64++;
        n -= sizeof(uint64_t)*2;
    }

    //��û�з�8�ֽ��ֳ��Ĳ���copy
    n = sz & ((sizeof(uint64_t)<<1) - 1);
    uint8_t *src_u8 = (uint8_t *) src;
    uint8_t *dst_u8 = (uint8_t *) dst;
    while (n-- )
    {
        (*dst_u8++ = *src_u8++);
    }
    return r;
}
