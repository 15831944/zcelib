/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2013��11��27��
* @brief      Э�̵�OS����㣬
* 
* @details    �ʼ�ο��������������
*             http://www.codeproject.com/Articles/4225/Unix-ucontext_t-Operations-on-Windows-Platforms
*             ��������ʵ��������ȷ��
*             1.CONTEXT�Ǹ���CPU�ṹ����ͬ�ģ�
*             2.Windows �µ�API GetThreadContext,SetThreadContext ����64λ�Ļ�������û���õġ�
*             3.GetThreadContext�Ե�ǰ�߳�����Ч�ģ�
*             ���ԣ�
* 
*             ����ֻ����Windows ��Fibers��ģ��Linux�µ�getcontext�Ⱥ���������Щ�����ӿ��Ǵ���
*             �޷���ȫ�ںϵĲ���ģ�����ֻ�ܸı���һЩ�ӿڵ����������䣬��ԭ�������в�һ�µĵط���
*             ����Fibers������˵������ο������ĵ�������д�ķǳ������
*             http://www.cnblogs.com/wz19860913/archive/2008/08/26/1276816.html
* @note       ConvertFiberToThread
* 
* 
* 
*/

#ifndef ZCE_LIB_OS_ADAPT_CORROUTINE_H_
#define ZCE_LIB_OS_ADAPT_CORROUTINE_H_

#include "zce_os_adapt_predefine.h"

#if defined ZCE_OS_WINDOWS

typedef  void * ucontext_t;

#endif 


typedef   void(*ZCE_COROUTINE_FUN_PTR) (void *left_para,void *right_para);


namespace ZCE_OS
{


/*!
* @brief      �Ǳ�׼��������LINUX�µ�makecontext���ƣ����ڶ��������Ƕ�ջ��С��
*             ����ָ���ʶ�ĺ���ֻ����ʹ��2�����������ܺ�LINUXһ��ʹ�ñ��
*             LINUXԭ����makecontext ��ucct ʹ��ǰҪ��ʼ����ָ��ͳ��ȣ�
*             �ҵ�ʵ���ڲ����Զ�����һ�������ݣ���Ϊucct�Ķ�ջ���������㲻���Լ����䣩 
*             ���Ա������deletecontext��WINDOWS�����������Fibers���ã�
*             ����������Ҫԭ���Ǽ���Windows ��Fibersʵ�֡�
* @return     int ����0��ʶ�ɹ���
* @param      ucct         ucontext_t��CONTEXT�����
* @param      slack_size   ��ջ��С
* @param      fun_ptr      ����ָ�룬����ֻ��������void *��ָ�롣
* @param      left_para    �����
* @param      right_para   �Ҳ���
* @note       
*/
int makecontext(ucontext_t *ucct,
    size_t slack_size, 
    ZCE_COROUTINE_FUN_PTR fun_ptr,
    void *left_para,
    void *right_para = NULL);


/*!
* @brief      �Ǳ�׼������LINUX�µĻ����ԵĿռ�
* @return     void
* @param      ucct  Ҫ�����ͷŵ�CONTEXT�����
* @note       LINUX���ͷ����Զ������ucct�Ŀռ䣬Windows�µ��õ���DeleteFiber
*/
void deletecontext(ucontext_t *ucct);


/*!
* @brief      ȡ�õ�ǰջ�ĵ�CONTEXT,
* @return     int ����0��ʶ�ɹ�
* @param      ucct CONTEXT����������õ�CONTEXT
* @note       WINDOWS�¼����ˣ�ConvertThreadToFiberEx��GetCurrentFiber
*/
int getcontext(ucontext_t *ucct);

/*!
* @brief      ���õ�ǰջ�ĵ�CONTEXT,
* @return     int   ����0��ʶ�ɹ�
* @param      ucct  CONTEXT�����

*/
int setcontext(const ucontext_t *ucct);

/*!
* @brief      ����CONTEXT
* @return     int ����0��ʶ�ɹ�
* @param      get_uctt ȡ�õ�CONTEXT
* @param      set_uctt ���õ�CONTEXT
* @note       
*/
int swapcontext(ucontext_t *get_uctt, const ucontext_t *set_uctt);

};

#endif //ZCE_LIB_OS_ADAPT_CORROUTINE_H_

