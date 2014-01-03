/******************************************************************************************
Copyright           : 2000-2004, Fullsail Technology (Shenzhen) Company Limited.
FileName            : soar_predefine.h
Author              : Sail(ZENGXING)
Version             :
Date Of Creation    : 2006��6��22��
Description         : ���ģ�����ļ��Ļ���ͷ�ļ������һ�����������.

Others              : Ϊ�˱���һЩ���ֲ��ִ���Ĵ��࣬
Function List       :
    1.  ......
Modification History:
    1.Date  :
      Author  :
      Modification  :
******************************************************************************************/

#ifndef SOARING_LIB_PER_DEFINE_H_
#define SOARING_LIB_PER_DEFINE_H_

//��WINDOWS�º�POSIX��׼���ݵĺ꣬2003������Σ���û�в���
#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS   1

//------------------------------------------------------------------------------------
//�ͻ��˷��������õ�ͷ�ļ�,��������ͷ�ļ�����������

#if defined ZCE_OS_WINDOWS && defined TSSCOMM_HASDLL
#ifdef BUILD_SOARING_MODULE_DLL
#define SOARING_EXPORT __declspec (dllexport)
#else
#define SOARING_EXPORT __declspec (dllimport)
#endif 
#else
#define SOARING_EXPORT
#endif

//---------------------------------------------------------------------------------------------

//ZCElib�Ŀ��ļ�
#include <zce_predefine.h>
#include <zce_id_to_string.h>
#include <zce_time_value.h>
#include <zce_mysql_connect.h>
#include <zce_mysql_command.h>
#include <zce_mysql_process.h>
#include <zce_shm_cache_chunk.h>
#include <zce_shm_lockfree_deque.h>
#include <zce_server_toolkit.h>
#include <zce_shm_hash_table.h>
#include <zce_boost_non_copyable.h>
#include <zce_boost_lord_rings.h>
#include <zce_os_adapt_string.h>
#include <zce_os_adapt_thread.h>
#include <zce_os_adapt_socket.h>
#include <zce_os_adapt_thread.h>
#include <zce_os_adapt_error.h>
#include <zce_os_adapt_process.h>
#include <zce_os_adapt_socket.h>
#include <zce_os_adapt_getopt.h>
#include <zce_os_adapt_file.h>
#include <zce_os_adapt_dirent.h>
#include <zce_share_mem_mmap.h>
#include <zce_share_mem_posix.h>
#include <zce_trace_log_debug.h>
#include <zce_trace_log_msg.h>
#include <zce_lock_null_lock.h>
#include <zce_lock_thread_mutex.h>
#include <zce_config_property_tree.h>
#include <zce_config_ini_implement.h>
#include <zce_thread_msgque_sema.h>
#include <zce_thread_msgque_nonlock.h>
#include <zce_timer_handler_base.h>
#include <zce_timer_queue_base.h>
#include <zce_timer_queue_wheel.h>
#include <zce_bus_two_way.h>
#include <zce_thread_task.h>
#include <zce_socket_addr_base.h>
#include <zce_socket_addr_in.h>
#include <zce_socket_base.h>
#include <zce_socket_stream.h>
#include <zce_socket_datagram.h>
#include <zce_socket_connector.h>
#include <zce_event_handle_base.h>
#include <zce_event_reactor_base.h>
#include <zce_event_reactor_select.h>
#include <zce_event_reactor_epoll.h>
#include <zce_thread_task.h>
#include <zce_thread_wait_mgr.h>
#include <zce_config_getopt.h>
#include <zce_server_status.h>
#include <zce_thread_bus_pipe.h>
#include <zce_lock_file_lock.h>
#include <zce_bytes_hash.h>
#include <zce_bytes_encode.h>




//����Ƕ����Ϊ���ÿͻ��˺ͷ�������ʹ�ò���ͳһ������

#if  defined(DEBUG) || defined(_DEBUG)
#define TESTCONFIG(condition,outstring)   if (!(condition) ) ZLOG_ERROR("[framework] Get configure file error. %s.",outstring); \
    ZCE_ASSERT(condition)
#else
#define TESTCONFIG(condition,outstring)   if(!(condition))\
    { \
        ZLOG_ERROR("[framework] Get configure file error. %s.",outstring); \
        return SOAR_RET::ERROR_GET_CFGFILE_CONFIG_FAIL; \
    }
#endif


#if !defined ZCE_LIB_LIBARY_NAME
#define ZCE_LIB_LIBARY_NAME   "soarlib-"ZCE_PLAT_TOOLSET_CONF".lib"
#endif

#endif //SOARING_LIB_PER_DEFINE_H_

