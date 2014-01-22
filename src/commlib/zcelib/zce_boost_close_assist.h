/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_boost_close_assist.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2011��7��1��
* @brief      
*             
*
* @details
*
* @note
*
*/

#ifndef ZCE_LIB_BOOST_CLOSE_ASSIST_H_
#define ZCE_LIB_BOOST_CLOSE_ASSIST_H_

namespace ZCE_LIB
{

/*!
* @brief     �����ļ�������̵��Զ��ͷ�
*/
template <class _value_type >
class close_assist
{
public:

    ///���캯�õ��ļ����
    close_assist(const _value_type to_close) :
        to_close_(to_close)
    {
    }

    //�������������ر��ļ�
    ~close_assist();

protected:

    ///�Զ�������ļ����
    _value_type    to_close_;

};



};

#endif //ZCE_LIB_BOOST_CLOSE_ASSIST_H_

