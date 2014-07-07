
#ifndef ZCE_LIB_CONFIG_INI_IMPLEMENTION_H_
#define ZCE_LIB_CONFIG_INI_IMPLEMENTION_H_

#include "zce_boost_non_copyable.h"
#include "zce_config_property_tree.h"

/******************************************************************************************
class ZCE_INI_Implemention INI�ļ������ö�ȡ��д��ʵ����
******************************************************************************************/
class ZCE_INI_Implemention : public ZCE_NON_Copyable
{
public:
    ZCE_INI_Implemention();
    ~ZCE_INI_Implemention();

    //��ȡ�������������ZCE_INI_PropertyTree���ݽṹ����
    int read(const char *file_name, ZCE_Conf_PropertyTree &propertytree);

    //д�룬��ʱû��ʵ�֣�ʵ����©��̫�࣬10.1�ڼ��е�̰�࣬
    int write(const char *file_name, ZCE_Conf_PropertyTree &propertytree);

protected:
    //ÿ�е���󳤶�
    static const size_t LINE_BUFFER_LEN = 8192;

};

/******************************************************************************************
class ZCE_INI_PropertyTree INI�ļ������ö�ȡ���ŵ��������������չ��Ҫ�Ǹ�һЩ���﷨�Ǹ����
******************************************************************************************/

//����INI�ļ��������ԣ�ΪZCE_Conf_PropertyTree �����˼�������
//�����˼����������������ǽ��в���
class ZCE_INI_PropertyTree : public ZCE_Conf_PropertyTree
{
public:
    //��ȡ����1
    int get_string_value(const std::string &sectionname,
                         const std::string &keyname,
                         std::string &value) const;

    //��ȡ���ã�
    int get_string_value(const std::string &sectionname,
                         const std::string &keyname,
                         char *buf,
                         size_t size_buf) const;
    //
    int get_int32_value(const std::string &sectionname,
                        const std::string &keyname,
                        int32_t &value) const;

    int get_uint32_value(const std::string &sectionname,
                         const std::string &keyname,
                         uint32_t &value) const;

    //
    int get_bool_value(const std::string &sectionname,
                       const std::string &keyname,
                       bool &value) const;
};

#endif //ZCE_LIB_CONFIG_INI_IMPLEMENTION_H_

