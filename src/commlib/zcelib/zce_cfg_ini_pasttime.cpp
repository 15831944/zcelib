
#include "zce_predefine.h"
#include "zce_os_adapt_string.h"
#include "zce_cfg_ini_pasttime.h"

//
ZCE_CfgFile_INI_Pt::ZCE_CfgFile_INI_Pt(const char *szfname)
{
    file_name_.assign(szfname);
}

ZCE_CfgFile_INI_Pt::~ZCE_CfgFile_INI_Pt()
{

}

//�õ�keyֵ��String
size_t ZCE_CfgFile_INI_Pt::get_private_str(const char *sec_name,
                                           const char *key_name,
                                           const char *default_str,
                                           char *return_str,
                                           const size_t size_ret_str)
{
    //����static get_privateprofile_string�������,ע�����
    return get_privateprofile_string(sec_name,
                                     key_name,
                                     default_str,
                                     return_str,
                                     size_ret_str, file_name_.c_str());
}

//�õ����е�section,��'\0'�ָ�����'\0\0'����
size_t ZCE_CfgFile_INI_Pt::get_private_allsection(char *return_str,
                                                  const size_t size_ret_str)
{
    //����static get_privateprofile_string�������,ע�����
    return get_privateprofile_string(NULL,
                                     NULL,
                                     NULL,
                                     return_str,
                                     size_ret_str, file_name_.c_str());
}

//�õ�ĳ��Section�����е�Key = stringֵ ��'\0'�ָ�����'\0\0'����
size_t ZCE_CfgFile_INI_Pt::getprivate_allkey(const char *sec_name,
                                             char *return_str, const size_t size_ret_str)
{
    //����static get_privateprofile_string�������,ע�����
    return get_privateprofile_string(sec_name, NULL, NULL, return_str, size_ret_str, file_name_.c_str());
}

//�õ�Int�� Keyֵ
int ZCE_CfgFile_INI_Pt::get_private_int(const char *sec_name,
                                        const char *key_name,
                                        const int default_int)
{
    //����static get_privateprofile_string�������,ע�����
    const size_t TMP_BUFFER_LEN = 64;
    char default_str[TMP_BUFFER_LEN], return_str[TMP_BUFFER_LEN];
    snprintf(default_str, TMP_BUFFER_LEN, "%d", default_int);
    get_privateprofile_string(sec_name,
                              key_name,
                              default_str,
                              return_str,
                              64,
                              file_name_.c_str());
    return atoi(return_str);
}

//�õ�bool�� Keyֵ
bool ZCE_CfgFile_INI_Pt::get_private_bool(const char *sec_name,
                                          const char *key_name,
                                          bool bDefault)
{
    //����static get_privateprofile_string�������,ע�����
    const size_t TMP_BUFFER_LEN = 64;
    char default_str[TMP_BUFFER_LEN + 1], return_str[TMP_BUFFER_LEN + 1];

    if (bDefault)
    {
        snprintf(default_str, TMP_BUFFER_LEN, "%s", "TRUE");
    }
    else
    {
        snprintf(default_str, TMP_BUFFER_LEN, "%s", "FALSE");
    }

    get_privateprofile_string(sec_name, key_name, default_str, return_str, 64, file_name_.c_str());

    if (ZCE_OS::strcasecmp(return_str, "TRUE"))
    {
        return false;
    }
    else
    {
        return true;
    }
}

//д��String
bool ZCE_CfgFile_INI_Pt::write_private_string(const char *sec_name,
                                              const char *key_name,
                                              const char *write_string)
{
    //����static WritePrivateProfileString�������,ע�����
    return write_privateprofile_string(sec_name, key_name, write_string, file_name_.c_str());
}

//�޸�Section
bool ZCE_CfgFile_INI_Pt::write_private_section(const char *sec_name,
                                               const char *write_string)
{
    //����static WritePrivateProfileSection�������,ע�����
    return   write_privateprofile_section(sec_name, write_string, file_name_.c_str());
}

//д��Int
bool ZCE_CfgFile_INI_Pt::write_private_int(const char *sec_name,
                                           const char *key_name,
                                           const int nvalue)
{
    //����static write_privateprofile_string�������,ע�����
    const size_t TMP_BUFFER_LEN = 64;
    char write_string[TMP_BUFFER_LEN + 1];
    snprintf(write_string, TMP_BUFFER_LEN, "%d", nvalue);
    return write_privateprofile_string(sec_name,
                                       key_name,
                                       write_string,
                                       file_name_.c_str());
}

//д��Bool����
bool ZCE_CfgFile_INI_Pt::write_private_bool(const char *sec_name,
                                            const char *key_name,
                                            const bool bvalue)
{
    //����static write_privateprofile_string�������,ע�����
    const size_t TMP_BUFFER_LEN = 64;
    char  write_string[TMP_BUFFER_LEN + 1];

    if (bvalue)
    {
        strncpy(write_string, "TRUE", TMP_BUFFER_LEN);
    }
    else
    {
        strncpy(write_string, "FASLE", TMP_BUFFER_LEN);
    }

    return write_privateprofile_string(sec_name, key_name, write_string, file_name_.c_str());
}

//ɾ��Key
bool ZCE_CfgFile_INI_Pt::del_private_key(const char *sec_name, const char *key_name)
{

    return write_privateprofile_string(sec_name, key_name, NULL, file_name_.c_str());
}

//ɾ��Section
bool ZCE_CfgFile_INI_Pt::del_private_section(const char *sec_name)
{

    return write_privateprofile_string(sec_name,
                                       NULL,
                                       NULL,
                                       file_name_.c_str());
}

size_t ZCE_CfgFile_INI_Pt::get_privateprofile_string(
    const char  *sec_name,
    const char  *key_name,
    const char  *default_str,
    char  *return_str,
    const size_t size_ret_str,
    const char  *file_name)
{
    //�����ַ�����ָ��Ϊ��,���ش���
    if (return_str == NULL || file_name == NULL)
    {
        assert(0);
    }

    *return_str = '\0';

    //�����ַ����ĳߴ�С��1,
    if (size_ret_str <= 1)
    {
        *return_str = '\0';
        return size_ret_str;
    }

    int operate = 0;

    //���App��Keyȫ��Ϊ�գ��򷵻����е�Section����'\0'�ָ������һ��������'\0'��ʶ
    if (sec_name == NULL && key_name == NULL)
    {
        operate = 1;
    }

    //���KeyΪ�գ�App���գ��򷵻����е�Section�µ�Keyֵ����'\0'�ָ������һ��������'\0'��ʶ
    if (sec_name != NULL && key_name == NULL)
    {
        operate = 2;
    }

    //AppΪ�գ�Key��Ϊ��,�������е�Key������һ��ƥ��ļ�ֵ����
    if (sec_name == NULL && key_name != NULL)
    {
        operate = 3;
    }

    //App��Key����Ϊ�գ��ⷵ��App��Key��ƥ��ļ�ֵ
    if (sec_name != NULL && key_name != NULL)
    {
        operate = 4;
    }

    std::ifstream cfgfile(file_name);

    //�ļ��򲻿�������Ĭ��ֵ
    if (!cfgfile)
    {
        strncpy(return_str, default_str, size_ret_str - 1);
        *(return_str + size_ret_str - 1) = '\0';
        return strlen(return_str);
    }

    //8*1024,1�е����ֵ
    char choneline[LINE_BUFFER_LEN + 1], chkey[LINE_BUFFER_LEN + 1], chstring[LINE_BUFFER_LEN + 1];
    choneline[LINE_BUFFER_LEN] = '\0';
    chkey[LINE_BUFFER_LEN] = '\0';
    chstring[LINE_BUFFER_LEN] = '\0';

    char     *pstrtmp;
    size_t ntmp;
    bool bApp;

    //���App��Keyȫ��Ϊ�գ��򷵻����е�Section����'\0'�ָ������һ��������'\0'��ʶ
    if (operate == 1)
    {
        //��������ַ�������С��2,ȫ�����ؿ��ַ�
        if (size_ret_str <= 2)
        {
            *return_str = '\0';
            *(return_str + 1) = '\0';
            return size_ret_str;
        }

        bApp = false;
        pstrtmp = return_str;
        ntmp = size_ret_str - 1;

        while (cfgfile)
        {
            cfgfile.getline(choneline, LINE_BUFFER_LEN);
            //fgets(choneline,LINE_BUFFER_LEN,pfile);
            //����
            ZCE_OS::strtrim(choneline);

            //ע����
            if (choneline[0] == ';' || choneline[0] == '#')
            {
                continue;
            }

            if (choneline[0] == '[' && choneline[strlen(choneline) - 1] == ']')
            {
                //ȥ��'[',']'
                memmove(choneline, choneline + 1, strlen(choneline) - 1);
                choneline[strlen(choneline) - 2] = '\0';

                strncpy(pstrtmp, choneline, ntmp - 1);
                *(pstrtmp + ntmp - 1) = '\0';

                ntmp = ntmp - strlen(pstrtmp) - 1;

                //ͬʱ���ǽ����ַ�������'\0'
                if (ntmp > 1)
                {
                    //������һ��д�ĵط������Ȱ���һ��'\0',
                    pstrtmp += strlen(pstrtmp) + 1;
                }
                //
                else
                {
                    *(pstrtmp + strlen(pstrtmp)) = '\0';
                    //�������ַ�Ҫ��˫'\0'
                    *(return_str + strlen(pstrtmp) + 1) = '\0';
                    return size_ret_str - ntmp - 2;
                }
            }
        }

        //
        if (*return_str != '\0')
        {
            *pstrtmp = '\0';
            return size_ret_str - ntmp - 2;
        }
        else
        {
            //����Ĭ��ֵ
            if (default_str == NULL)
            {
                return_str[0] = '\0';
                return 0;
            }
            else
            {
                strncpy(return_str, default_str, size_ret_str - 2);
                *(return_str + strlen(return_str)) = '\0';
                *(return_str + strlen(return_str) + 1) = '\0';
                return strlen(return_str);
            }
        }

    }

    //���KeyΪ�գ�App���գ��򷵻����е�Section�µ�Keyֵ����'\0'�ָ������һ��������'\0'��ʶ
    if (operate == 2)
    {
        //��������ַ�������С��2,ȫ�����ؿ��ַ�
        if (size_ret_str <= 2)
        {
            *return_str = '\0';
            *(return_str + 1) = '\0';
            return size_ret_str;
        }

        bApp = false;
        pstrtmp = return_str;
        ntmp = size_ret_str - 1;

        while (cfgfile)
        {

            cfgfile.getline(choneline, LINE_BUFFER_LEN);
            //����
            ZCE_OS::strtrim(choneline);

            //ע����
            if (choneline[0] == ';' || choneline[0] == '#')
            {
                continue;
            }

            //��ƥ���Section
            if (choneline[0] == '[' && choneline[strlen(choneline) - 1] == ']')
            {
                //�Ѿ��ҵ���һ��Section,û�з�����ص�Key�����ز�ѯ������Keyֵ
                if (bApp == true)
                {
                    *pstrtmp = '\0';
                    return size_ret_str - ntmp - 2;
                }

                //ȥ��'[',']'
                memmove(choneline, choneline + 1, strlen(choneline) - 1);
                choneline[strlen(choneline) - 2] = '\0';
                //����
                ZCE_OS::strtrim(choneline);

                if (ZCE_OS::strcasecmp(choneline, sec_name) == 0)
                {
                    bApp = true;
                    pstrtmp = return_str;
                    ntmp = size_ret_str - 1;
                    continue;
                }
            }

            //��key
            if (bApp == true)
            {
                char *str = strstr(choneline, "=");

                if (str != NULL)
                {

                    strncpy(pstrtmp, choneline, ntmp - 1);
                    //��ӽ�����
                    *(pstrtmp + strlen(pstrtmp)) = '\0';
                    //���Ȱ���һ��'\0'��
                    ntmp = ntmp - strlen(pstrtmp) - 1;

                    //ͬʱ���ǽ����ַ�������'\0'
                    if (ntmp > 1)
                    {
                        //������һ��д�ĵط������Ȱ���һ��'\0',
                        pstrtmp += strlen(pstrtmp) + 1;
                    }
                    //
                    else
                    {
                        *(pstrtmp + strlen(pstrtmp)) = '\0';
                        //�������ַ�Ҫ��˫'\0'
                        *(return_str + strlen(pstrtmp) + 1) = '\0';
                        return size_ret_str - ntmp - 2;
                    }
                }
            }
        }

        //
        if (*return_str != '\0')
        {
            *pstrtmp = '\0';
            return size_ret_str - ntmp - 2;
        }
        else
        {
            //����Ĭ��ֵ
            if (default_str == NULL)
            {
                return_str[0] = '\0';
                return 0;
            }
            else
            {
                strncpy(return_str, default_str, size_ret_str - 2);
                *(return_str + strlen(return_str)) = '\0';
                *(return_str + strlen(return_str) + 1) = '\0';
                return strlen(return_str);
            }
        }

    }

    //AppΪ�գ�Key��Ϊ��,�������е�Key������һ��ƥ��ļ�ֵ����
    if (operate == 3)
    {
        while (cfgfile)
        {
            cfgfile.getline(choneline, LINE_BUFFER_LEN);
            //����
            ZCE_OS::strtrim(choneline);

            //ע����
            if (choneline[0] == ';' || choneline[0] == '#')
            {
                continue;
            }

            char *str = strstr(choneline, "=");

            if (str != NULL)
            {
                char *snext = str + 1;
                *str = '\0';
                strncpy(chkey, choneline, LINE_BUFFER_LEN);
                strncpy(chstring, snext, LINE_BUFFER_LEN);

                ////
                //�ҵ����ء�
                if (ZCE_OS::strcasecmp(chkey, key_name) == 0)
                {
                    strncpy(return_str, chstring, size_ret_str - 1);
                    *(return_str + size_ret_str - 1) = '\0';
                    return strlen(return_str);
                }
            }
        }

        //����Ĭ��ֵ
        if (default_str == NULL)
        {
            return_str = '\0';
            return 0;
        }
        else
        {
            strncpy(return_str, default_str, size_ret_str - 1);
            *(return_str + size_ret_str - 1) = '\0';
            return strlen(return_str);
        }
    }

    //App��Key����Ϊ�գ��ⷵ��App��Key��ƥ��ļ�ֵ
    if (operate == 4)
    {
        bApp = false;

        while (cfgfile)
        {

            cfgfile.getline(choneline, LINE_BUFFER_LEN);
            //����
            ZCE_OS::strtrim(choneline);

            //ע����
            if (choneline[0] == ';' || choneline[0] == '#')
            {
                continue;
            }

            if (choneline[0] == '[' && choneline[strlen(choneline) - 1] == ']')
            {
                //�Ѿ��ҵ���һ��Section,û�з�����ص�Key������Ĭ��ֵ
                if (bApp == true)
                {
                    //����Ĭ��ֵ
                    if (default_str == NULL)
                    {
                        return_str = '\0';
                        return 0;
                    }
                    else
                    {
                        strncpy(return_str, default_str, size_ret_str - 1);
                        *(return_str + size_ret_str - 1) = '\0';
                        return strlen(return_str);
                    }
                }

                //ȥ��'[',']'
                memmove(choneline, choneline + 1, strlen(choneline) - 1);
                choneline[strlen(choneline) - 2] = '\0';
                //
                ZCE_OS::strtrim(choneline);

                //
                if (ZCE_OS::strcasecmp(choneline, sec_name) == 0)
                {
                    bApp = true;
                    continue;
                }
            }

            if (bApp == true)
            {
                char *str = strstr(choneline, "=");

                if (str != NULL)
                {
                    char *snext = str + 1;
                    *str = '\0';
                    strncpy(chkey, choneline, LINE_BUFFER_LEN);
                    strncpy(chstring, snext, LINE_BUFFER_LEN);
                    ////
                    ZCE_OS::strtrim(chkey);
                    ZCE_OS::strtrim(chstring);

                    //�ҵ����ء�
                    if (ZCE_OS::strcasecmp(chkey, key_name) == 0)
                    {
                        strncpy(return_str, chstring, size_ret_str - 1);
                        *(return_str + size_ret_str - 1) = '\0';
                        return strlen(return_str);
                    }
                }
            }
        }

        //
        if (default_str == NULL)
        {
            return_str = '\0';
            return 0;
        }
        else
        {
            strncpy(return_str, default_str, size_ret_str - 1);
            *(return_str + size_ret_str - 1) = '\0';
            return strlen(return_str);
        }
    }

    //No rearch
    return 0;
}

//��ȡINI�ļ���ı���ֶ���Ϊ��������
int ZCE_CfgFile_INI_Pt::get_privateprofile_int(
    const char *sec_name,
    const char *key_name,
    int default_int,
    const char *file_name)
{
    const size_t TMP_BUFFER_LEN = 64;
    char default_str[TMP_BUFFER_LEN + 1], return_str[TMP_BUFFER_LEN + 1];
    snprintf(default_str, TMP_BUFFER_LEN, "%d", default_int);
    ZCE_CfgFile_INI_Pt::get_privateprofile_string(sec_name, key_name, default_str, return_str, 64, file_name);
    return atoi(return_str);
}

//дINI�ļ���ı���ֶΣ�
bool ZCE_CfgFile_INI_Pt::write_privateprofile_string(
    const char *sec_name,
    const char *key_name,
    const char *write_string,
    const char *file_name)
{

    int operate(0);

    if (sec_name == NULL && key_name == NULL && write_string == NULL)
    {
        return false;
    }

    if (sec_name == NULL)
    {
        operate = 1;
    }
    else
    {
        operate = 2;
    }

    FILE *pfile;
    pfile = fopen(file_name, "r+t");

    //�ļ��򲻿���
    if (pfile == NULL)
    {
        pfile = fopen(file_name, "w+t");

        if (pfile == NULL)
        {
            return false;
        }

        if (sec_name != NULL)
        {
            fprintf(pfile, "[%s]\n", sec_name);
        }

        fprintf(pfile, "%s=%s", key_name, write_string);
        fclose(pfile);
        return true;
    }

    //1�е����ֵ
    char choneline[LINE_BUFFER_LEN + 1], line[LINE_BUFFER_LEN + 1], chkey[LINE_BUFFER_LEN + 1], chstring[LINE_BUFFER_LEN + 1];

    if (operate == 1)
    {
        //���û����ص�App����section=string д��ĩβ
        fseek(pfile, 0, SEEK_END);
        fprintf(pfile, "%s", "\n");
        fprintf(pfile, "%s=%s", key_name, write_string);
        return true;
    }

    if (operate == 2)
    {
        //���ļ�����������
        std::vector<std::string> strarytmp;
        std::string strtmp;
        line[0] = '\0';
        //��ʾ�Ƿ��ҵ�App,key
        bool bApp = false, bkey = false;

        while (!feof(pfile))
        {
            //��д����ʱ�ļ�
            if (line[0] != '\0')
            {
                std::string strtmp(line);
                strarytmp.push_back(line);
            }

            if (fgets(choneline, LINE_BUFFER_LEN, pfile) == NULL)
            {
                break;
            }

            memmove(line, choneline, strlen(choneline) + 1);
            //����
            ZCE_OS::strtrim(choneline);

            //ע����,����
            if (choneline[0] == ';' || choneline[0] == '#' || choneline[0] == '\0')
            {
                continue;
            }

            //[Section]
            if (choneline[0] == '[' && choneline[strlen(choneline) - 1] == ']')
            {
                //�Ѿ��ҵ���һ��Section,û�з�����ص�Key������Ĭ��ֵ
                if (bApp == true)
                {
                    if (key_name != NULL && write_string != NULL)
                    {
                        snprintf(choneline, LINE_BUFFER_LEN, "%s=%s\n", key_name, write_string);
                        choneline[LINE_BUFFER_LEN] = '\0';
                        std::string strtmp(choneline);
                        strarytmp.push_back(choneline);
                    }

                    break;
                }

                choneline[strlen(choneline) - 1] = '\0';
                //����
                ZCE_OS::strtrimleft(choneline);
                //ȥ��'[',']'
                memmove(choneline, choneline + 1, strlen(choneline));
                ZCE_OS::strtrimright(choneline);

                //�Ƚ�Section����
                if (ZCE_OS::strcasecmp(choneline, sec_name) == 0)
                {
                    bApp = true;

                    //Ҫɾ�����App
                    if (key_name == NULL)
                    {
                        line[0] = '\0';
                    }

                    continue;
                }
            }

            //���[Section] �Ѿ��ҵ�
            if (bApp == true)
            {
                //���Key==NULL,���ò�ѯ������
                if (key_name == NULL)
                {
                    line[0] = '\0';
                    continue;
                }

                //�ҵ���һ��'='
                char *str = strstr(choneline, "=");

                //����ҵ�
                if (str != NULL)
                {
                    char *snext = str + 1;
                    *str = '\0';
                    strncpy(chkey, choneline, LINE_BUFFER_LEN);
                    strncpy(chstring, snext, LINE_BUFFER_LEN);
                    //����
                    ZCE_OS::strtrim(chkey);
                    ZCE_OS::strtrim(chstring);

                    //�ҵ���Ӧ��Key
                    if (ZCE_OS::strcasecmp(chkey, key_name) == 0)
                    {
                        //��ʾ�ҵ���ֵ
                        bkey = true;

                        //���Keystring==NULL��ɾ��Key�����Keystring!=NULL,��д�롣
                        if (write_string != NULL)
                        {
                            snprintf(line, LINE_BUFFER_LEN, "%s=%s\n", key_name, write_string);
                            break;
                        }
                        else
                        {
                            line[0] = '\0';
                            break;
                        }
                    }
                }
            }
        }

        //
        if (line[0] != '\0')
        {
            std::string strtmp(line);
            strarytmp.push_back(line);
        }

        //����ҵ�App ,�����ҵ�Key����key == NUL ,��ʾ�����Ѿ����,
        if (bApp == true)
        {

            if (key_name != NULL && write_string != NULL && feof(pfile) && bkey != true)
            {
                snprintf(line, LINE_BUFFER_LEN, "\n%s=%s", key_name, write_string);
                std::string strtmp(line);
                strarytmp.push_back(line);
            }

            while (!feof(pfile))
            {

                if (fgets(line, LINE_BUFFER_LEN, pfile) == NULL)
                {
                    break;
                }

                std::string strtmp(line);
                strarytmp.push_back(line);

            }

            fclose(pfile);
            //����Ϣд���ļ�
            pfile = fopen(file_name, "w+t");

            for (size_t j = 0; j < strarytmp.size(); ++j)
            {
                fputs(strarytmp[j].c_str(), pfile);
            }

            //�ر��ļ�
            fclose(pfile);
            return true;
        }
        else if (bApp == false)
        {
            //��App��Key Stringȫ��д��
            if (bkey == false && key_name != NULL && write_string != NULL)
            {
                fseek(pfile, 0, SEEK_END);
                fprintf(pfile, "\n[%s]", sec_name);
                fprintf(pfile, "\n%s=%s", key_name, write_string);
                fclose(pfile);
                return true;
            }
            else
            {
                fclose(pfile);
                return false;
            }
        }

        fclose(pfile);
        return false;
    }

    //No rearch
    return false;
}

bool ZCE_CfgFile_INI_Pt::write_privateprofile_section(
    const char *sec_name,
    const char *write_string,
    const char *file_name)
{

    if (sec_name == NULL || write_string == NULL || write_string == NULL)
    {
        return false;
    }

    FILE *pfile;
    pfile = fopen(file_name, "r+t");

    //�ļ��򲻿���
    if (pfile == NULL)
    {
        return false;
    }

    //8*1024,1�е����ֵ

    char choneline[LINE_BUFFER_LEN + 1], line[LINE_BUFFER_LEN + 1], *read_ret = NULL;

    //���ļ�����������
    std::vector<std::string> strarytmp;
    std::string strtmp;
    line[0] = '\0';

    //��ʾ�Ƿ��ҵ�App,key
    bool bApp = false;

    while (!feof(pfile))
    {
        //��д����ʱ������
        if (line[0] != '\0')
        {
            std::string strtmp(line);
            strarytmp.push_back(line);
        }

        read_ret = fgets(choneline, LINE_BUFFER_LEN, pfile);
        if (NULL == read_ret)
        {
            return false;
        }
        memmove(line, choneline, strlen(choneline) + 1);
        //����
        ZCE_OS::strtrim(choneline);

        //ע����,����
        if (choneline[0] == ';' || choneline[0] == '#' || choneline[0] == '\0')
        {
            continue;
        }

        //[Section]
        if (choneline[0] == '[' && choneline[strlen(choneline) - 1] == ']')
        {
            //ȥ��'[',']'
            memmove(choneline, choneline + 1, strlen(choneline) - 1);
            choneline[strlen(choneline) - 2] = '\0';
            //����
            ZCE_OS::strtrim(choneline);

            //�Ƚ�Section����
            if (ZCE_OS::strcasecmp(choneline, sec_name) == 0)
            {
                bApp = true;
                //Ҫ�޸����App
                snprintf(line, LINE_BUFFER_LEN, "[%s]\n", write_string);
                break;
            }
        }
    }

    //
    if (line[0] != '\0')
    {
        std::string strtmp(line);
        strarytmp.push_back(line);
    }

    //����ҵ�App ,�����ҵ�Key����key == NUL ,��ʾ�����Ѿ����,
    if (bApp == true)
    {

        while (!feof(pfile))
        {
            read_ret = fgets(line, LINE_BUFFER_LEN, pfile);
            if (NULL == read_ret)
            {
                return false;
            }
            std::string strtmp(line);
            strarytmp.push_back(line);
        }

        fclose(pfile);
        //����Ϣд���ļ�
        pfile = fopen(file_name, "w+t");

        for (size_t j = 0; j < strarytmp.size(); ++j)
        {
            fputs(strarytmp[j].c_str(), pfile);
        }

        //�ر��ļ�
        fclose(pfile);
        return true;
    }

    return false;
}