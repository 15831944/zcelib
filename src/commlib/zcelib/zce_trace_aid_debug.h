/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_trace_aid_debug.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2004��7��11��
* @brief      ���ٵ��Եĸ���������������ȡ�
*
*             �����ļ���������ݲ�ͬ�궨�屣֤��ƽ̨����
* @details
*
*
*
* @note
*
*/


#pragma once


//==========================================================================================================

///������ӡһ��ָ���ڲ����ݵĺ�������16���Ƶķ�ʽ��ӡ
namespace zce
{

///�������Ķ�ջ��Ϣ�����ڵ���
int backtrace_stack(ZCE_LOG_PRIORITY dbg_lvl,
                    const char *dbg_info);

///��־��ӡ��ջ��Ϣ
///���Դ�ӡ��ָ��
///ָ�����ݵĳ���
void debug_pointer(ZCE_LOG_PRIORITY dbg_lvl,
                   const char *dbg_info,
                   const unsigned char  *debug_ptr,
                   size_t data_len);

};

//��־��ӡ��ջ��Ϣ�ĺ꣬x����־���������
#ifndef ZCE_BACKTRACE_STACK
#define ZCE_BACKTRACE_STACK(x)            zce::backtrace_stack(x,__ZCE_FUNC__)
#endif

//��־��ӡ��ջ��Ϣ�ĺ꣬x����־���������,y��ָ�룬z��ָ�볤��
#ifndef ZCE_TRACE_POINTER_DATA
#define ZCE_TRACE_POINTER_DATA(x,y,z)     zce::debug_pointer((x),__ZCE_FUNC__,(y),(z))
#endif


#if defined DEBUG || defined _DEBUG

#ifndef ZCE_BACKTRACE_STACK_DBG
#define ZCE_BACKTRACE_STACK_DBG    ZCE_BACKTRACE_STACK
#endif

#else

#ifndef ZCE_BACKTRACE_STACK_DBG
#define ZCE_BACKTRACE_STACK_DBG(...) ((void)0)
#endif

#endif

//==========================================================================================================



