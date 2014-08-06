/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_config_property_tree.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2011��10��5��
* @brief      һ������������Ϣ������ͨ��path����node�ϵ���Ϣ��
*
*
* @details    ����������ʵ��Ŀ����ʵ������BOOST ��Property Tree �� ACE_Configuration_Heap
*             ���Ŀ�꣬��������Ϣ��֯��һ�������
*
*
*
* @note      ����IPhone 5û�з��������Ƿ�����һ��Iphone 4S,�������е�ʧ��
*
*
*/


#ifndef ZCE_LIB_CONFIG_PROPERTY_TREE_H_
#define ZCE_LIB_CONFIG_PROPERTY_TREE_H_

//
//
class ZCE_Sockaddr_In;
class ZCE_Sockaddr_In6;
class ZCE_Time_Value;





/*!
* @brief      �����ļ���ȡ���ŵ���
*             ������ȡ���߶�ȡ�������ݺ����ɷ���ZCE_Conf_PropertyTree�У�
*             ˼·���Ǻ�Property Tree����һ�¡�
* @note       2013���ͷ��ϸ����BOOST��˵���������˼ҵ�ʵ�ֱ��ҿ��ǵĻ���������
*             �٣���������俿£�ˡ�
*/
class ZCE_Conf_PropertyTree
{
protected:

    ///Ҷ�ӽڵ�
    typedef std::string  LEAF_NOTE_TYPE;

    ///�����Ľڵ������,�������map�����Բ��Ǹ�Чʵ�֣���Ϊɶ����map�أ��ҹ�����
    ///��Ϊ��ʵmap���²�����˳�������ڻ�ԭ��ʱ�򣬻���ȫ����ԭ�������ݣ�����Ȼ
    ///������������
    typedef std::list< std::pair<std::string, ZCE_Conf_PropertyTree> > CHILDREN_NOTE_TYPE;

    //
public:

    ///���캯��
    ZCE_Conf_PropertyTree();
    ~ZCE_Conf_PropertyTree();


    ///����·���õ�һ��CHILD ������
    int path_get_child(const std::string &path_str,
                       ZCE_Conf_PropertyTree *& child_data);

    ///����·���õ�һ��const CHILD ������
    int path_get_child(const std::string &path_str,
                       const ZCE_Conf_PropertyTree *& child_data) const;

    //ȡ��Ҷ�ӽڵ��string
    int get_leafptr(const std::string &path_str,
                    std::string *& leaf_data);

    int get_leafptr(const std::string &path_str,
                    const std::string *& leaf_data) const;

    /*!
    * @brief      ���������ػ���ģ��ߵ���һ�麯��,ģ�庯��,�����ػ�ʵ��,
    * @tparam     val_type ���ػ��� ZCE_Sockaddr_In��ZCE_Sockaddr_In6��ZCE_Time_Value
    *             int32_t ,int64_t,std::string, �ȡ�
    * @return     int      �Ƿ������Ķ�ȡ��������
    * @param      path_str ��ȡ��·��
    * @param      val      ��ȡ���ص�ֵ
    */
    template<typename val_type>
    int path_get_leaf(const std::string &path_str,
                      val_type &val) const;


    template<>
    int path_get_leaf(const std::string &path_str,
                      ZCE_Sockaddr_In &val) const;

    ///ȡ��IPV6�ĵ�ַ��
    template<>
    int path_get_leaf(const std::string &path_str,
                      ZCE_Sockaddr_In6 &val) const;

    ///ʱ����ַ�����ʹ��ISO�ĸ�ʽ
    template<>
    int path_get_leaf(const std::string &path_str,
                      ZCE_Time_Value &val) const;


    ///����һ���µ�CHILD,��Ȼ����ȫ������ΪNULL,���ҷ��������Ľڵ�
    void add_child(const std::string &key_str,
                  ZCE_Conf_PropertyTree *&new_child_note);

    ///���������ػ���ģ��ߵ���һ�麯��,ģ�庯��,ֻ���岻ʵ��
    template<typename val_type>
    void set_leaf(val_type val);


    //����һ��Ҷ�ӽڵ㣬string
    template<>
    void set_leaf(const std::string &value_data);

    //����һ��Ҷ�ӽڵ㣬int
    template<>
    void set_leaf(int value_int);

    //����һ��Ҷ�ӽڵ㣬bool
    template<>
    void set_leaf(bool value_bool);


    //
    template<typename val_type>
    void add_child_leaf(const std::string &key_str,
        val_type val)
    {
        ZCE_Conf_PropertyTree *tree_node = NULL;
        add_child(key_str, tree_node);
        tree_node->set_leaf<val_type>(val);
    }

public:
    ///���÷ָ����,������������
    static void set_separator(char separator_char);

protected:

    ///Ĭ��ʹ��.��Ϊ�����ţ����ǵ�name������ܻ�ʹ��������ţ�
    ///�������Ҳ���������,û����const �������ṩ���޸ĺ���
    static char  SEPARATOR_STRING[2];

protected:

    ///Ҷ�ӽڵ��MAP
    LEAF_NOTE_TYPE   leaf_node_;

    ///�����ڵ��MAP
    CHILDREN_NOTE_TYPE     child_node_;

};

#endif //ZCE_LIB_CONFIG_PROPERTY_H_

