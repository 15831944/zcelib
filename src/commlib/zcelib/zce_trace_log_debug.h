/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_trace_log_debug.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       2004��7��11��
* @brief      ���ٵ�����Ϣ������ZCE_Trace_LogMsg����
*             ���Թ����ߣ�������STDOUT,STDERR,Windos�ĵ��Դ��������
*             �����ļ���������ݲ�ͬ�궨�屣֤��ƽ̨����
* @details    
*             
*             
*             
* @note       
*             
*/


#ifndef ZCE_LIB_TRACE_LOGMSG_H_
#define ZCE_LIB_TRACE_LOGMSG_H_


#include "zce_trace_log_msg.h"



//==========================================================================================================



//�����
#define ZLOG_ENABLE           ZCE_Trace_LogMsg::enable_output_logmsg
//�ر����
#define ZLOG_DISABLE          ZCE_Trace_LogMsg::disable_output_logmsg
//���MASK����,С������������־��Ϣ�������
#define ZLOG_OUTLEVEL_MASK    ZCE_Trace_LogMsg::set_log_priority

//ZCE_LOGMSGΪʲôҪ����˫���ţ�GCC2.9�ǵû�,�����ˣ�����������Ϊ��,
//�Ҳ����������ô��汾������

//ʹ�õ��Լ��������־
#define ZCE_LOGMSG            ZCE_Trace_LogMsg::debug_output

#if _MSC_VER <= 1300

//�ṩһЩ��д�ķ�ʽ����Ȼ��Ҳ���ò����ر��
#define ZLOG_TRACE            ZCE_Trace_LogMsg::debug_traceex
#define ZLOG_DEBUG            ZCE_Trace_LogMsg::debug_debugEx
#define ZLOG_INFO             ZCE_Trace_LogMsg::debug_infoex
#define ZLOG_ERROR            ZCE_Trace_LogMsg::debug_errorex
#define ZLOG_ALERT            ZCE_Trace_LogMsg::debug_alertex
#define ZLOG_FATAL            ZCE_Trace_LogMsg::debug_fatalex

#else

#define ZLOG_TRACE(...)       ZCE_Trace_LogMsg::debug_output(RS_TRACE,__VA_ARGS__)
#define ZLOG_DEBUG(...)       ZCE_Trace_LogMsg::debug_output(RS_DEBUG,__VA_ARGS__)
#define ZLOG_INFO(...)        ZCE_Trace_LogMsg::debug_output(RS_INFO,__VA_ARGS__)
#define ZLOG_ERROR(...)       ZCE_Trace_LogMsg::debug_output(RS_ERROR,__VA_ARGS__)
#define ZLOG_ALERT(...)       ZCE_Trace_LogMsg::debug_output(RS_ALERT,__VA_ARGS__)
#define ZLOG_FATAL(...)       ZCE_Trace_LogMsg::debug_output(RS_FATAL,__VA_ARGS__)

#endif

//==========================================================================================================
//����һ���ṹ�Ĺ���������������к�������
class ZCE_Trace_Function
{
public:
    //��������
    const char        *func_name_;
    //�ļ�����
    const char        *file_name_;
    //�ļ����кţ��к��Ǻ������ڲ���λ�ã����Ǻ�����������ʼλ�ã������ֺη�
    int                file_line_;
    //
    ZCE_LOG_PRIORITY   log_priority_;

public:
    //���ù��캯����ʾ���뺯�������
    ZCE_Trace_Function(const char *func_name,
                       const char *file_name,
                       int file_line,
                       ZCE_LOG_PRIORITY   log_priority)
        : func_name_(func_name)
        , file_name_(file_name)
        , file_line_(file_line)
        , log_priority_(log_priority)
    {
        ZCE_LOGMSG(log_priority_, "[zcelib] [FUNCTION TRACE]%s entry,File %s|%u ", func_name_, file_name_, file_line_);
    }
    //��������������ʾ���뺯�������
    ~ZCE_Trace_Function()
    {
        ZCE_LOGMSG(log_priority_, "[zcelib] [FUNCTION TRACE]%s exit,File %s|%u .", func_name_, file_name_, file_line_);
    }

};

//ZCE_FUNCTION_TRACE(RS_DEBUG)�����ڸ��ٺ����Ľ���
//���ں����Ŀ�ʼʹ��ZCE_FUNCTION_TRACE(RS_DEBUG)����꣬�������ӷֺ�
#ifndef ZCE_TRACE_FUNCTION
#define ZCE_TRACE_FUNCTION(x)        ZCE_Trace_Function  ____tmp_func_trace_(__ZCE_FUNCTION__,__FILE__,__LINE__,(x))
#endif

//==========================================================================================================




//==========================================================================================================

///������ӡһ��ָ���ڲ����ݵĺ�������16���Ƶķ�ʽ��ӡ
namespace ZCE_LIB
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
#define ZCE_BACKTRACE_STACK(x)            ZCE_LIB::backtrace_stack(x,__ZCE_FUNCTION__)
#endif

//��־��ӡ��ջ��Ϣ�ĺ꣬x����־���������,y��ָ�룬z��ָ�볤��
#ifndef ZCE_TRACE_POINTER_DATA
#define ZCE_TRACE_POINTER_DATA(x,y,z)     ZCE_LIB::debug_pointer((x),__ZCE_FUNCTION__,(y),(z))
#endif

//==========================================================================================================



///���ڳ������е��ĵط���
#ifndef ZCE_TRACE_FILELINE
#define ZCE_TRACE_FILELINE(x)        ZCE_LOGMSG(x,"[FILELINE TRACE]goto File %s|%d,function:%s.",__FILE__,__LINE__,__ZCE_FUNCTION__)
#endif

///
#ifndef ZCE_TRACE_FAIL_RETURN
#define ZCE_TRACE_FAIL_RETURN(x,y,z) ZCE_LOGMSG(x,"[FAIL RETRUN]Fail in file [%s|%d],function:%s,fail info:%s,return %d,last error %d.",__FILE__,__LINE__,__ZCE_FUNCTION__,y,z,ZCE_OS::last_error())
#endif

#ifndef ZCE_RECORD_FAIL_API
#define ZCE_RECORD_FAIL_API(x,y)     ZCE_LOGMSG(x,"[API FAIL ]API fail in file [%s|%d],function:%s,fail api:%s,last error %d.",__FILE__,__LINE__,__ZCE_FUNCTION__,y,ZCE_OS::last_error())
#endif


//TRACE���Բ��ֵĺ궨��

//----------------------------------------------------------------------------------
//DEBUG �汾���е�һЩ��
//ZCE_ASSERTԭ����д���� while(!(f)){ do something,��������WINDOWS�»��������и澯�����ȸ���

#if defined DEBUG || defined _DEBUG

#ifndef ZCE_LOGMSG_DBG
#define ZCE_LOGMSG_DBG        ZCE_LOGMSG
#endif

#ifndef ZCE_ASSERT_DGB
#define ZCE_ASSERT_DGB         ZCE_ASSERT
#endif

#ifndef ZCE_FUNCTION_TRACE_DBG
#define ZCE_FUNCTION_TRACE_DBG   ZCE_TRACE_FUNCTION
#endif


#ifndef ZCE_FILELINE_TRACE_DBG
#define ZCE_FILELINE_TRACE_DBG   ZCE_TRACE_FILELINE
#endif

#ifndef ZCE_STACK_BACKTRACE_DBG
#define ZCE_STACK_BACKTRACE_DBG  ZCE_STACK_BACKTRACE
#endif



//�ǵ��԰汾�ĺ궨�壬
#else //DEBUG

//������ǵ��Ա���,����Щ����Ϊ��


#ifndef ZCE_LOGMSG_DBG
#if defined ZCE_OS_WINDOWS
#define ZCE_LOGMSG_DBG             __noop
#else
#define ZCE_LOGMSG_DBG(...)          do  {} while (0)
#endif
#endif

#ifndef ZCE_ASSERT_DGB
#define ZCE_ASSERT_DGB(...)       ((void)0)
#endif

#ifndef ZCE_FUNCTION_TRACE_DBG
#define ZCE_FUNCTION_TRACE_DBG(...) ((void)0)
#endif


#ifndef ZCE_FILELINE_TRACE_DBG
#define ZCE_FILELINE_TRACE_DBG(...) ((void)0)
#endif

#ifndef ZCE_STACK_BACKTRACE_DBG
#define ZCE_STACK_BACKTRACE_DBG(...) ((void)0)
#endif

#endif //DEBUG

//------------------------------------------------------------------------------------------------------

//��ΪWindow��LINUX������ʵ��ʹ�õĺ�������ʵ�ڷ�DEBUG�汾��ʵ��û�б�©���������Ա�����������Щ�������ʹ�á�

#if defined (ZCE_OS_WINDOWS)
_CRTIMP int __cdecl _CrtDbgReport(
    int _ReportType,
    const char *_Filename,
    int _Linenumber,
    const char *_ModuleName,
    const char *_Format,
    ...);
#endif

#if defined (ZCE_OS_LINUX)
//__assert_fail���������extern�ģ����������extern���ã�linux�Լ�ֻ�ڷ�NDEBUG�汾�²���extern����
extern "C"  void __assert_fail (__const char *__assertion, __const char *__file,
                                unsigned int __line, __const char *__function)
__THROW __attribute__ ((__noreturn__));
#endif //#if defined (ZCE_OS_LINUX)

//��REALSE�汾��һЩ��
#if !defined NDEBUG

#ifndef ZCE_ASSERT
#if defined (ZCE_OS_WINDOWS)
#define ZCE_ASSERT(expr) \
    (void) ((!!(expr)) || \
            (ZCE_Trace_LogMsg::debug_assert(__FILE__,__LINE__,__ZCE_FUNCTION__,#expr),0) || \
            (1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __ZCE_FUNCTION__, #expr)) || \
            (_CrtDbgBreak(), 0))
#else
#define ZCE_ASSERT(expr) \
    (void) ((!!(expr)) || \
            (ZCE_Trace_LogMsg::debug_assert(__FILE__,__LINE__,__ZCE_FUNCTION__,#expr),0) || \
            (__assert_fail (#expr, __FILE__, __LINE__, __ZCE_FUNCTION__),0))
#endif
#endif

#ifndef ZCE_ASSERT_EX
#if defined (ZCE_OS_WINDOWS)
#define ZCE_ASSERT_EX(expr,str) \
    (void) ((!!(expr)) || \
            (ZCE_Trace_LogMsg::debug_assert_ex(__FILE__,__LINE__,__ZCE_FUNCTION__,#expr,str),0) || \
            (1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __ZCE_FUNCTION__, #expr)) || \
            (_CrtDbgBreak(), 0))
#else
#define ZCE_ASSERT_EX(expr,str) \
    (void) ((!!(expr)) || \
            (ZCE_Trace_LogMsg::debug_assert_ex(__FILE__,__LINE__,__ZCE_FUNCTION__,#expr,str),0) || \
            (__assert_fail (#expr, __FILE__, __LINE__, __ZCE_FUNCTION__),0))
#endif
#endif

#ifndef ZCE_VERIFY
#define ZCE_VERIFY(expr)       ZCE_ASSERT(expr)
#endif

//#if !defined NDEBUG
#else

//ע������ZCE_ASSERT��VERIFYZCE_VERIFY������,VERIFY�ڲ��ı��ʽʽ��Ȼ������
#ifndef ZCE_ASSERT
#define ZCE_ASSERT(...)             ((void)0)
#endif

#ifndef ZCE_ASSERT_EX
#define ZCE_ASSERT_EX(...)          ((void)0)
#endif


#ifndef ZCE_VERIFY
#define ZCE_VERIFY(expr)             ((void)(expr))
#endif

#endif


//----------------------------------------------------------------------------------
//���ۣ�DEBUG�汾��REALSE�汾Ҳ�����õ�һЩ���ԣ���Щ�������а汾�������ã�����һЩ������ʱ��ҲҪ�жϵĶ���
//ALL���κ�ʱ��

#ifndef ZCE_ASSERT_ALL
#if defined (ZCE_OS_WINDOWS)
#define ZCE_ASSERT_ALL(expr) \
    (void) ((!!(expr)) || \
            (ZCE_Trace_LogMsg::debug_assert(__FILE__,__LINE__,__ZCE_FUNCTION__,#expr),0) || \
            (1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __ZCE_FUNCTION__, #expr)) || \
            (_CrtDbgBreak(), 0))
#else
#define ZCE_ASSERT_ALL(expr) \
    (void) ((!!(expr)) || \
            (ZCE_Trace_LogMsg::debug_assert(__FILE__,__LINE__,__ZCE_FUNCTION__,#expr),0) || \
            (__assert_fail (#expr, __FILE__, __LINE__, __ZCE_FUNCTION__),0))
#endif
#endif


#ifndef ZCE_ASSERT_ALL_EX
#if defined (ZCE_OS_WINDOWS)
#define ZCE_ASSERT_ALL_EX(expr,str) \
    (void) ((!!(expr)) || \
            (ZCE_Trace_LogMsg::debug_assert_ex(__FILE__,__LINE__,__ZCE_FUNCTION__,#expr,str),0) || \
            (1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, __ZCE_FUNCTION__, #expr)) || \
            (_CrtDbgBreak(), 0))
#else
#define ZCE_ASSERT_ALL_EX(expr,str) \
    (void) ((!!(expr)) || \
            (ZCE_Trace_LogMsg::debug_assert_ex(__FILE__,__LINE__,__ZCE_FUNCTION__,#expr,str),0) || \
            (__assert_fail (#expr, __FILE__, __LINE__, __ZCE_FUNCTION__),0))
#endif
#endif


#endif //ZCE_LIB_TRACE_LOGMSG_H_

