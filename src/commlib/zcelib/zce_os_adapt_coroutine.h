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

//Ϊʲô���ѡ��3�������ĺ�����Ϊ֧�ֵ����ͣ��������Ϊά�������ӣ����ұ���һֱ��Ϊ2�������㹻�ˣ�
typedef   void(*ZCE_COROUTINE_3PARA) (void *para1,
    void *para2,
    void *para3);


namespace ZCE_OS
{


/*!
* @brief      �Ǳ�׼��������LINUX�µ�makecontext���ƣ�������Ҳ���٣�����������Ҫԭ
*             ���Ǽ���Windows ��Fibersʵ�֡��������£�
*             1.����ָ���ʶ�ĺ���ֻ����ʹ��2�����������ܺ�LINUXһ��ʹ�ñ��
*             2.LINUXԭ����::makecontext��ucontext_t����Ҫ����::getcontext��
*             ��ȡ�ģ�  ��ʹ�ô˺������ã��ڲ������ˣ�
*             3.�ڶ��������Ƕ�ջ��С���һ�������ָ������ѿռ䣬��Ϊ���COROUTINE
*             ��ջ����LINUXԭ����makecontext ��ucontext_t����ʹ��ǰҪ��ʼ����ָ
*             ��ͳ��ȣ�
*             4.�����󣬱������deletecontext����ucontext_t��deletecontext��
*             WINDOWS�µ���DeleteFiber��������Linux��������ucontext_t�����ջ
*             ָ�루�Ӷ��Ϸ���Ŀռ䣩
*             5.LINUXԭ����::makecontextû�з���ֵ�����˷�װ�У���ΪWindows����
*             ������CreateFiberEx��Linux�������ڲ�������::getcontext����Щ����
*             ������ʧ��
*             6.LINUX��::makecontext����ʹ�ñ�Σ��������û�п��ǲ�������Ȼ�����
*             Ҳ���з��գ�����ʵʹ�ò���������ֲ���յģ�������������ĵ���˵����
*             http://en.wikipedia.org/wiki/Setcontext   
*             http://pubs.opengroup.org/onlinepubs/009695399/functions/makecontext.html
*             ����������Ǽ����ԣ�����ʹ��ǰ���Ǹ������ȽϺ�
*             ���⣬Ϊ�˰�װ�������ݣ���Windows�£����������newһ���ṹ
* @return     int ����0��ʶ�ɹ���
* @param      uctt         ucontext_t�����ɵ�CONTEXT����� 
* @param      stack_size   ջ��С
* @param      back_uc      ucontext_t���������أ��ص���ucontext_t
* @param      fun_ptr      ����ָ�룬����3��ָ�����
* @param      para1        ָ�����1
* @param      para2        ָ�����2
* @param      para3        ָ�����3
* @note       
*/
    int makecontext(ucontext_t *uctt,
    size_t stack_size,
    ucontext_t *back_uc,
    ZCE_COROUTINE_3PARA fun_ptr,
    void *para1,
    void *para2,
    void *para3);


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

