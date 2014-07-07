/*!
* @copyright  2002-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_config_ini_file.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2002��1��3��
* @brief      INI �ļ��Ķ�д�Ļ��ɰ棬��WINDOWS��API�������ƣ�
*             ��������д�ص�ʱ���лᱣ֤�ļ�ԭ���ĸ�ʽ��ע�ͣ������ƻ��ļ��ĸ�ʽ��
*             ���⣬���API��һЩ���⹦�ܣ������ȡһ��section�����е��ֶ����ݵȣ�
* @details    ����INI�ļ���һ�е���󳤶���8K
*
* @note       ��Ӧ����2002�꣬2003���ڼƷ���Ŀʱ���Լ��ڼ���д�Ĵ��롣ĳ�ֳ�
*             ���ϣ������������ǲ��ٻ�ʹ�õ��ˣ�
*             ������ĳ�̶ֳ��Ǹ���Ļ��ɡ��������Ҳ���ǰ���Щ��������������µ�
*             ����淶
*             ����ע�ͷ������滹��WIN95����ʾ���Ǻ�
*/

#ifndef ZCE_LIB_CONFIG_INI_PASTTIME_H_
#define ZCE_LIB_CONFIG_INI_PASTTIME_H_


/*!
* @brief      INI�ļ���д�Ĵ����ӿڶ�ģ��WIndows��API
*/
class ZCELIB_EXPORT ZCE_CfgFile_INI_Pt
{

public:

    ///���캯��,
    ///@param file_name �ļ�����
    ZCE_CfgFile_INI_Pt(const char *file_name);
    //��������
    ~ZCE_CfgFile_INI_Pt();

public:

    ///�õ�keyֵ��String ,��������ֵ�ο�get_privateprofile_string
    size_t get_private_str(const char *sec_name,
                           const char *key_name,
                           const char *default_str,
                           char *return_str,
                           const size_t size_ret_str);

    ///�õ����е�section,��'\0'�ָ�����'\0\0'��������������ֵ�ο�get_privateprofile_string
    size_t get_private_allsection(char *return_str,
                                  const size_t size_ret_str);

    ///�õ�ĳ��Section�����е�Keyֵ����������ֵ�ο�get_privateprofile_string
    size_t getprivate_allkey(const char *sec_name,
                             char *return_str,
                             const size_t size_ret_str);

    ///�õ�Int�� Keyֵ����������ֵ�ο�get_privateprofile_string
    int get_private_int(const char *sec_name,
                        const char *key_name,
                        const int default_int);

    ///�õ�bool�� Keyֵ����������ֵ�ο�get_privateprofile_string
    bool   get_private_bool(const char *sec_name,
                            const char *key_name,
                            const bool default_int);

    ///д��String
    bool   write_private_string(const char *sec_name,
                                const char *key_name,
                                const char *write_string);

    ///�޸�Section
    bool   write_private_section(const char *sec_name,
                                 const char *write_string);
    //д��Int
    bool   write_private_int(const char *sec_name,
                             const char *key_name,
                             const int nvalue);
    ///д��Bool����
    bool   write_private_bool(const char *sec_name,
                              const char *key_name,
                              const bool bvalue);

    ///ɾ��Key
    bool   del_private_key(const char *sec_name,
                           const char *key_name);
    ///ɾ��Section
    bool   del_private_section(const char *sec_name);

public:

    /*!
    * @brief      ��̬��������ȡINI�ļ����ֶΣ����䣬ģ��WIN32�ӿ�GetPrivateProfileString,
    *             ���в����Ҿ�ֱ�Ӹ���MSDN��
    * @return     size_t
    *             If the function succeeds, the return value is the number of characters
    *             copied to the buffer, not including the terminating null character.
    *             If neither sec_name nor key_name is NULL and the supplied destination
    *             buffer is too small to hold the requested string, the string is
    *             truncated and followed by a null character, and the return value is
    *             equal to size_ret_str minus one.
    *             If either sec_name or key_name is NULL and the supplied destination
    *             buffer is too small to hold all the strings, the last string is
    *             truncated and followed by two null characters. In this case, the return
    *             value is equal to size_ret_str minus two.
    * @param      sec_name
    *             Points to a null-terminated string that specifies the section containing
    *             the key name. If this parameter is NULL, the get_privateprofile_string
    *             function copies all section names in the file to the supplied buffer.
    * @param      key_name
    *             Pointer to the null-terminated string containing the key name whose
    *             associated string is to be retrieved. If this parameter is NULL, all key
    *             names in the section specified by the sec_name parameter are copied to
    *             the buffer specified by the return_str parameter.
    * @param      default_str
    *             Pointer to a null-terminated default string. If the key_name key cannot
    *             be found in the initialization file, get_privateprofile_string copies the
    *             default string to the return_str buffer. This parameter cannot be NULL.
    *             Avoid specifying a default string with trailing blank characters. The
    *             function inserts a null character in the return_str * buffer to strip any
    *             trailing blanks.
    * @param      return_str
    *             Pointer to the buffer that receives the retrieved string.
    * @param      size_ret_str
    *             Specifies the size, in characters, of the buffer pointed to by the return_str
    *             parameter.
    * @param      file_name
    *             Pointer to a null-terminated string that names the initialization file. If
    *             this parameter does not contain a full path to the file, Windows searches
    *             for the file in the Windows directory.
    */
    static size_t get_privateprofile_string(const char *sec_name,
                                            const char *key_name,
                                            const char *default_str,
                                            char *return_str,
                                            const size_t size_ret_str,
                                            const char *file_name);

    /*!
    * @brief      ��̬������дĳ���ֶΣ�ģ��WIN32�ӿ�WritePrivateProfileString
    * @return     bool
    *             If the function successfully copies the string to the initialization
    *             file, the return value is nonzero.
    *             If the function fails, or if it flushes the cached version of the most
    *             recently accessed initialization file, the return value is zero. To get
    *             extended error information, call GetLastError.
    * @param      sec_name
    *             Points to a null-terminated string containing the name of the section
    *             to which the string will be copied. If the section does not exist, it
    *             is created. The name of the section is case-independent; the string
    *             can be any combination of uppercase and lowercase letters.
    * @param      key_name��
    *             Points to the null-terminated string containing the name of the key
    *             to be associated with a string. If the key does not exist in the
    *             specified section, it is created. If this parameter is NULL, the entire
    *             section, including all entries within the section, is deleted.
    * @param      write_string
    *             Points to a null-terminated string to be written to the file. If this
    *             parameter is NULL, the key pointed to by the key_name parameter is deleted.
    * @param      file_name
    *             Points to a null-terminated string that names the initialization file.
    */
    static bool   write_privateprofile_string(const char *sec_name,
                                              const char *key_name,
                                              const char *write_string,
                                              const char *file_name);

    /*��
    * @brief   ��̬���������ֶ��м�ȡ��ĳ��INT�����ã��ο�WINDOWS API GetPrivateProfileInt
    * @param   sec_name
    *          Points to a null-terminated string containing the section name in the
    *           initialization file.
    * @param   key_name
    *          Points to the null-terminated string containing the key name whose
    *          value is to be retrieved. This value is in the form of a string;
    *          the GetPrivateProfileInt function converts the string into an integer
    *          and returns the integer.
    * @param   default_int
    *          Specifies the default value to return if the key name cannot be found
    *          in the initialization file.
    * @param   file_name
    *          Points to a null-terminated string that names the initialization file.
    *          If this parameter does not contain a full path to the file, Windows
    *          searches for the file in the Windows directory.
    * @return  bool
    *          If the function succeeds, the return value is the integer equivalent
    *          of the string following the specified key name in the specified
    *          initialization file. If the key is not found, the return value is the
    *          specified default value. If the value of the key is less than zero,
    *          the return value is zero.
    * Remarks
    *          The function searches the file for a key that matches the name specified
    *          by the key_name parameter under the section name specified by the sec_name
    *          parameter. A section in the initialization file must have the following
    *           form:
    *          [section]
    *          key=value
    *          .
    *          The GetPrivateProfileInt function is not case-sensitive; the strings in
    *          sec_name and key_name can be a combination of uppercase and lowercase letters.
    */
    static int get_privateprofile_int(const char *sec_name,
                                      const char *key_name,
                                      int default_int,
                                      const char *file_name);

    /*!
    * @brief   ��̬������дĳ�����䣬ģ��WIN32�ӿ�WritePrivateProfileSection
    * @param   sec_name
    *          Points to a null-terminated string containing the name of the section in
    *          which data is written. This section name is typically the name of the
    *          calling application.
    * @param   write_string
    *          Points to a buffer containing the new key names and associated values
    *           that are written to the named section.
    * @param   file_name
    *          Points to a null-terminated string containing the name of the initialization
    *          file. If this parameter does not contain a full path for the file, the
    *          function searches the Windows directory for the file. If the file does not exist
    *          and file_name does not contain a full path, the function creates the file in
    *          the Windows directory. The function does not create a file if file_name contains
    *          the full path and filename of a file that does not exist.
    * @return  bool
    *          If the function succeeds, the return value is nonzero.
    */
    static bool   write_privateprofile_section(const char *sec_name,
                                               const  char *write_string,
                                               const char *file_name);


protected:
    //ÿ�е���󳤶�
    static const size_t LINE_BUFFER_LEN = 8192;

protected:

    ///������ļ�����
    std::string            file_name_;

};

#endif //ZCE_LIB_CONFIG_INI_PASTTIME_H_


