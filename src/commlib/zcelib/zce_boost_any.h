/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_boost_any.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2014��5��30��
* @brief      ��Boost ��any��һ��ʵ�֣�����һЩ������Ҫ��̫�ϸ񣬵�����Ҫ����
*             ����Եĵط���
* 
* @details
* 
* @note       ��Ӣ�ģ����ڵ�̨�壬û��һ���˻���Ϊ������Ȩ����������ѹ����
*             Ҳû��һ���˻���Ϊ�����������Ա�������������һ��·�������ߵú�
*             �������ߵú����ࡣ����������̨��׷˼����.���ˡ�����������һ����
*             �����й�Ҳ����ͬ�����ص�������׷˼�Ǹ����ӡ�̨���ܣ���½û����
*             �ɲ��ܣ�
*/

#ifndef ZCE_LIB_BOOST_ANY_H_
#define ZCE_LIB_BOOST_ANY_H_

namespace ZCE_LIB
{

    class any
    {
    public:
        // construct/copy/destruct
        any();
        any(const any &);
        any(any &&);
        template<typename ValueType> any(const ValueType &);
        template<typename ValueType> any(ValueType &&);
        any & operator=(const any &);
        any & operator=(any &&);
        template<typename value_type> any & operator=(const value_type &);
        template<typename value_type> any & operator=(value_type &&);
        ~any();

        // modifiers
        any & swap(any &);

        // queries
        bool empty() const;
        const std::type_info & type() const;
    };


};

#endif //ZCE_LIB_BOOST_ANY_H_
