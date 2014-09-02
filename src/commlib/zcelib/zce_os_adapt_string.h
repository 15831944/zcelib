/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_os_adapt_string.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2002��06��22��
* @brief      ����ASNI string�����Ĳ���
*
*
* @details    �������LIB�����������һ��ʹ��
*
*
*
* @note       �������в���Ӧ����ZCELIB(ZENLIB)����Ĵ��롣
*             �����������δ��룬�������Լ��Ĺ�����Ϣ��������
*             2002�꣬��Ӧ���ڱ������������HW����ҵ���粿�Ÿոճ�����ʱ��
*             ��Ȼ��������������൱���ִ��뻹����д�ˣ�
*
*/


#ifndef ZCE_LIB_OS_ADAPT_STRING_H_
#define ZCE_LIB_OS_ADAPT_STRING_H_






namespace ZCE_LIB
{
//==========================================================================================================

//


/*!
* @brief      �ַ��Ĺ��ܵ���ȡ�࣬���ڼ���char��wchar_t�ַ��������ʱ����ͦ���õ�
*             yunfeiyang GG���һ�׶�����
* @tparam     char_type �ַ����ͣ�char ���� wchar_t
* @note       char_traits���滹�� length,move�Ⱥ���
*/
template <typename char_type>
struct yun_char_traits : public std::char_traits < char_type >
{
};

template <>
struct yun_char_traits < char > : public std::char_traits < char >
{
    typedef std::string string_type;

    static int tolower(int c)
    {
        return ::tolower(c);
    }
    static int toupper(int c)
    {
        return ::toupper(c);
    }

    static int isspace(int c)
    {
        return (isascii(c) && ::isspace(c));
    }

    static char_type *strend(char_type *s)
    {
        return ::strchr(s, 0);
    }
    static const char_type *strend(const char_type *s)
    {
        return ::strchr(s, 0);
    }

};

template <>
struct yun_char_traits < wchar_t > : public std::char_traits < wchar_t >
{
    typedef std::wstring    string_type;

    static int tolower(int c)
    {
        return ::towlower(c);
    }

    static int toupper(int c)
    {
        return ::towupper(c);
    }

    static int isspace(int c)
    {
        return ::iswspace(c);
    }

    static char_type *strend(char_type *s)
    {
        return ::wcschr(s, 0);
    }

    static const char_type *strend(const char_type *s)
    {
        return ::wcschr(s, 0);
    }

};


//==========================================================================================================
//ԭ���Ĵ��������˶�ASCII 255���ַ��������ж�����ASCII255��Non-breaking space��
//����ʱȥ���ˡ��о�������������������������Ϊ��ʱû��дע�ͣ�������ȫ���ǵó����ˡ�

/*!
* @brief      ������ַ�����ȥ���ַ�����ߵĿո񣬻��У��س���Tab
* @tparam     char_type �ַ����ͣ�char or wchar_t
* @return     char_type *
* @param      str
*/
template <typename char_type>
char_type *strtrimleft(char_type *str)
{
    ZCE_ASSERT(str);

    char_type *lstr = str;
    for (; *lstr != '\0'; lstr++)
    {
        //ע��isspace�Ĳ�����int,��WIN�£�Ҫ���ַ���0~255,
        if (yun_char_traits<char_type>::isspace(*lstr) != 0)
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
        yun_char_traits<char_type>::move(str, lstr, 
            yun_char_traits<char_type>::length(lstr) + 1);
    }

    return str;
}


/*!
* @brief      �ҹ����ַ�����ȥ���ַ����ұߵĿո񣬻��У��س���Tab
* @return     char*
* @param      str
*/
template <typename char_type>
char *strtrimright(char_type *str)
{
    ZCE_ASSERT(str);
    char_type *lstr = str + yun_char_traits<char_type>::length(str) - 1;

    for (; lstr >= str; lstr--)
    {
        //ע��isspace�Ĳ�����int,��WIN�£�Ҫ���ַ���0~255,
        if (yun_char_traits<char_type>::isspace(*lstr) != 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if (lstr != str + yun_char_traits<char_type>::length(str) - 1)
    {
        *++lstr = 0;
    }

    return str;
}


/*!
* @brief      �����ַ�����ȥ���ַ������ߵĿո񣬻��У��س���Tab
* @return     char*
* @param      str
*/
template <typename char_type>
inline char *strtrim(char_type *str)
{
    ZCE_LIB::strtrimleft<char_type>(str);
    ZCE_LIB::strtrimright<char_type>(str);
    return str;
}



template < typename char_type, typename char_traits_type, typename allocator_typ >
void trimleft_stdstr(std::basic_string<char_type, char_traits_type, allocator_typ> &str)
{
    typename std::basic_string<char_type, char_traits_type, allocator_typ>::iterator it = str.begin();

    while (it != str.end() && yun_char_traits<char_type>::isspace(*it))
    {
        ++it;
    }

    str.erase(str.begin(), it);
    return;
}

template < typename char_type, typename char_traits_type, typename allocator_typ >
void trimright_stdstr(std::basic_string<char_type, char_traits_type, allocator_typ> &str)
{
    typename std::basic_string<char_type, char_traits_type, allocator_typ>::iterator it = str.end();

    if (it == str.begin())
    {
        return;
    }

    while (it != str.begin() && yun_char_traits<char_type>::isspace(*--it));

    str.erase(it + 1, str.end());

    return;
}

template < typename char_type, typename char_traits_type, typename allocator_typ >
inline void trim(std::basic_string<char_type, char_traits_type, allocator_typ> &str)
{
    trimright_stdstr(str);
    trimleft_stdstr(str);

    return;
}

//==========================================================================================================
//�����⼸������Ŀǰ��ƽ̨�����У�����ʱ��ĳ��ƽ̨����ȱʧ����HP������IBM��SUN���޷�ȷ���ˣ�
//��ʱ�����ˡ�


/*!
* @brief      ���ַ���ȫ��ת��Ϊ��д�ַ�
*/
char *strupr(char *str);


/*!
* @brief      ���ַ���ȫ��ת��ΪСд�ַ�
*/
char *strlwr(char *str);


/*!
* @brief      �ַ����Ƚϣ����Ӵ�Сд
* @return     int
* @param      string1
* @param      string2
*/
int strcasecmp(const char *string1, const char *string2);

/*!
* @brief      �ַ��������Ƚϣ����Ӵ�Сд
* @return     int
* @param      string1
* @param      string2
* @param      maxlen
* @note
*/
int strncasecmp(const char *string1, const char *string2, size_t maxlen);




//==========================================================================================================
//����Ψһ���ֵ�һЩ����


/*!
* @brief      ͨ������ָ�룬ȡ��һ��Ψһ������,����һЩ��Ҫ���ֵĵط�,��֤����Ŀռ���48���ֽڣ�
* @return     char*           ���ص�����
* @param[in]  object_ptr  ����ָ�룬ĳ�������ָ���ַ��ͨ�����ָ���ַת���õ�Ψһ����
* @param[out] name        ���ֵ�buffer
* @param[in]  length      buffer�ĳ���
*/
char *object_unique_name (const void *object_ptr,
                          char *name,
                          size_t length);


/*!
* @brief      ͨ��ǰ׺ʽ���õ�һ��Ψһ������,Ψһ���ư�����ǰ׺+����ID+�ڲ�������
* @return     char*        ���ص�����
* @param[in]  prefix_name  ǰ׺���ַ���
* @param[out] name         ���ֵ�buffer
* @param[in]  length       buffer�ĳ���
*/
char *prefix_unique_name(const char *prefix_name,
                         char *name,
                         size_t length);






//==========================================================================================================
//����һЩ����ַ�����������

/*!
* @brief      ��Խ�հ׷���ָ�ո�ˮƽ�Ʊ���ֱ�Ʊ���ҳ���س��ͻ��з��������ַ�����Խ��
* @return     const char* ������ĵ�ַ
* @param      str         �����ַ���
* @note       Ϊ�˷���һЩ�����Ϣ�����ӵ�һ�麯����һ��һЩ��top�Ĵ���ʱ�����
*/
const char *skip_whitespace(const char *str);

//
/*!
* @brief      ��Խĳ��token������ĳ������
* @return     const char* ������ĵ�ַ
* @param      str         �����ַ���
*/
const char *skip_token(const char *str);


/*!
* @brief      ����һ��
* @return     const char* ������ĵ�ַ
* @param[in]  str         �����ַ���
* @note
*/
const char *skip_line(const char *str);


/*!
* @brief      ��Խĳ��token,ֱ���ָ����󣨻��߽�����
* @return     const char*    ������ĵ�ַ
* @param[in]  str            �����ַ���
* @param[in]  separator_char �ָ��ַ���Ҫ�ҵ�����ַ�Ϊֹ������������ַ����棩
*/
const char *skip_separator(const char *str, char separator_char);





//==========================================================================================================


/*!
* @brief      һ���򵥵Ĵ�ӡ���ĳ���ڴ��ַ��Ϣ�ĺ��������Ǽ򵥵��ڴ淭��Ϊ16�����ַ���
* @param      stream  �ļ�������stdout,stderrҲ������
* @param      mem     ���Ե��ڴ�ָ��
* @param      mem_len ָ�볤�ȣ�
* @note       �������־����ṩ�˷���ĸ����࣬��������ڲ���Ҫʹ����־�ĵط�
*/
void memory_debug(FILE *stream, const unsigned char *mem, size_t mem_len);



/*!
* @brief      �� 11 02 03 0E E0         ..... ��ʽ�������ָ����Ϣ�����Դ�ӡ�ڴ���Ϣ
*             ��ʽ��Ϣ���ӷḻ������������Ȼ��ӡҲ������
* @param      stream  �ļ�������stdout,stderrҲ������
* @param      mem     ���Ե��ڴ�ָ��
* @param      mem_len ָ�볤��
*/
void memory_debug_ex(FILE *stream, const unsigned char *mem, size_t mem_len);


//==========================================================================================================

/*!
* @brief      �����ڴ濽����������Ҫ���������ֽڶ��룬8�ֽڵĿ���������ֽڽ��У�
*             ��Ҫ���ܵ�����  http://blog.chinaunix.net/uid-20043340-id-154852.html ��Ӱ��
*             ����ʵ�����¿��ŶȲ������ֽ�ʵ�ʲ�����Ȼ��memcpy���죬���߿������ڷ��Ż������µĲ��ԡ�
*             ����Ȥ�Ĳο��Ķ���
*             http://www.cnblogs.com/fullsail/p/3160098.html
*             ����ο����Ķ��Ĳ���Ҳ�����Զ�һ�¡�
* @return     void*  ����dst
* @param      dst    Ŀ���ַ
* @param      src    Դ���ݵ�ַ
* @param      sz     �����ĳ���
* @note       �κ�ʱ�򶼣���������ѡ�� C���п�� memcpy,
*/
void *fast_memcpy(void *dst, const void *src, size_t sz);


void *fast_memcpy2(void *dst, const void *src, size_t sz);

};














#endif //ZCE_LIB_STRING_EX_H_

