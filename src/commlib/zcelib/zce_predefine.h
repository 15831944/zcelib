/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_predefine.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2003-5-14
* @brief      ����Ԥ������Ϣ�������������е��ⲿͷ�ļ���ȫ��ʹ�õĺ꣬
*             ����ƽ̨�����Ե�һЩС��������ֵ��typedef��
* 
* 
*  @details   ��yunfei�Ľ�����ٸĽ�һ�£�����ÿ���˿������˼·���ǲ�һ���ġ�
*             Ҳ��ȡ��ѵ��дע�ͣ���ô�Ҳ����ΪɶҪ����
*             ������ϸ��һ��ÿ�εķָ��ߺ�˵��������Ϊ�ҵĻ����Ǻ������ģ���Ҫ
*             һ������Ϊ����궨����ң�
*             ���λ����֮ǰ�����㱼��Ұ��ʽ����˼ά����ϸ�Ķ�һ��ע��
*             ͷ�ļ��ֳɼ��������֣�����ϵͳ���壬ͷ�ļ���������ֵ���壬һЩ���ú꣬
* 
*             ͷ�ļ������������
*             1.WINDOWS���в��ֵģ���Ҫ��WINDOWS���ݶ����࣬���׳�ͻ������Windows
*               �����ļ��Ķ���˳��Ҳ���н�����
*             2.LINUX���в��ֵģ�
*             3.Cͷ�ļ�
*             4.C++���в��ֵģ�
*             5.�����ĵ�3���Ŀ��,����ز�Ҫ���ң����󲿷ֶ��ǿ��Դ򿪹رյģ�
* 
*             ��ֵ����typedef���ִ��룬�Լ���ص�ͷ�ļ���Ϣ,
*             ��Ķ����Ժ�Ϊ���ģ������ղ���ϵͳ�ֿ������������ʹ�࣬��Ҫ��ͼ���࣬
*             ���ı�˳�򣬷�������������⣬
* 
*             ��¼һ�㴿��YY���Ķ�����
*             һ���õ�������ǰ���͵�һ�λ�����¼������
*             ���ߣ�����Ҳ������Ҳ���ʲ��ģ����Σ����󣬲�������������������ȥ�������࣬
*             ��Ҳ�����Ƿ���Ҳ������Ҳ������������Ҳ��������������
*             �һظ���
*             ���������ƽ���� ȥ�������޷��ı�ġ�
*             ������������ȥ�ı����ܸı�ģ�
*             �����ǻۣ��ֱ������ߵ�����
*/

#ifndef ZCE_LIB_PREDEFINE_H_
#define ZCE_LIB_PREDEFINE_H_

//==================================================================================================
//���ݲ���ϵͳ����������������ͬ�Ķ��壬��Ϊ��Щ�����Ӱ��ȫ�֣��������ʼ�Ĳ���
//�ҵĿ�ֻ������Ӧ����������Windows(MSVC)��Linux(GCC)��

//WINDOWSƽ̨(MSVC)
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

#define ZCE_OS_WINDOWS 1

#if (defined(WIN64) || defined(_WIN64))
#define ZCE_WIN64 1
#endif //

#if (defined(WIN32) || defined(_WIN32)) && !(defined(WIN64) || defined(_WIN64))
#define ZCE_WIN32 1
#endif

#endif //#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

// LINUXƽ̨(GCC)��ֻ����֧��GCC��������˼
#if defined(__linux__) && defined (__GNUC__)
#define ZCE_OS_LINUX 1

#if defined(__LP64__)
#define ZCE_LINUX64 1
#else
#define ZCE_LINUX32 1
#endif

#endif

#if  ( defined (ZCE_WIN64) || defined(ZCE_LINUX64) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64__)  \
  || defined(__amd64) || defined(__ppc64__) || defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(__ia64__) )
#  define ZCE_OS64 1
#else
#  define ZCE_OS32 1
#endif

//�����ɶ�����ǻ���ɶ���ǣ��Ҳ����ˡ�
#if (!defined (ZCE_OS_WINDOWS) && !defined (ZCE_OS_LINUX)) \
    || (defined (ZCE_OS_WINDOWS) && defined (ZCE_OS_LINUX))
#error " ZCE_OS_WINDOWS and ZCE_OS_LINUX all defined or all undefined.  error."
#endif 

//Windows�±���ʹ��VS��Ϊ�༭���������Ҷ�GCC�ް�����̫̫�鷳�ˡ�
#if defined (ZCE_OS_WINDOWS)
#if !defined (_MSC_VER) || (_MSC_VER < 1310)
#error "Only support 2003 or upper visual studio version."
#endif
#endif

//==================================================================================================
//LINUX GCC�µĸ澯���ι��ܣ��������GCC 4.2�Ժ���У�����push,pop�Ĺ��ܣ�������GCC��4.6�Ժ���У�
//���������θ澯�Ķ���Ҫ�ȴ�GCC4.6�Ժ���С�������ˡ�
//#pragma GCC diagnostic push
//#pragma GCC diagnostic pop

//==================================================================================================

//WINDOWS������ͷ�ļ����֣�

#if defined(ZCE_OS_WINDOWS)

// ��WINDOWS�º�POSIX��׼���ݵĺ꣬VS2003���°汾��Σ���û�в��ԣ�2003�Ժ�Windows���ںܶ������ʹ��"_"ǰ׺��
#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_WARNINGS  1

// ���ڸ���VC++���Ҳ�ǿ��ʹ��_s���͵�API��_CRT_SECURE_NO_DEPRECATE��_CRT_SECURE_NO_WARNINGS���ϰ汾
#define _CRT_SECURE_NO_WARNINGS   1
#define _CRT_SECURE_NO_DEPRECATE  1

//==================================================================================================

//ΪʲôWindowsͷ�ļ��������ǰ�棬��Ϊ�����д����Ķ��壬����������������������������γɳ�ͻ��

//֧��WINSERVER2008,VISTA �����������1�������֧�����������0
#define ZCE_SUPPORT_WINSVR2008 1

//Windows ��Vista��WinServer2008�Ժ�֧���˺ܶ��µ�API,�����Ҫ֧�֣���Ҫ��֧�ֿ���
#if (defined _WIN32_WINNT) && (_WIN32_WINNT >=  0x0600) && defined (_MSC_VER) && (_MSC_VER >= 1400)
#ifndef ZCE_SUPPORT_WINSVR2008
#define ZCE_SUPPORT_WINSVR2008 1
#else
#define ZCE_SUPPORT_WINSVR2008 0
#endif
#endif

//�ܶ�ͷ�ļ��Լ���ֵ���屾�����Էŵ�����OS�������ļ�����ȥ�ģ����Ǹо��Ƚ���Ҫ�����Ƿŵ�����ط���
// pretend it's at least Windows XP or Win2003������������������ʱ�����һЩAPI�޷�ʹ�õ�����
#if !defined (_WIN32_WINNT)
#if (defined ZCE_SUPPORT_WINSVR2008) && (ZCE_SUPPORT_WINSVR2008 == 1)
# define _WIN32_WINNT 0x0600
#else
# define _WIN32_WINNT 0x0501
#endif
#endif

// ���¶���FD_SETSIZE����Ҫ��winsock2.hǰ�棬Ҳ��������ע��
#ifndef FD_SETSIZE
#define FD_SETSIZE   1024
#endif

//��Щ�ļ�ǰ׺�Ǵ�д�ģ��������ֵֹģ���Windows���������Ǵ�д�ġ�

#include <winsock2.h>
#include <MSWSock.h>
#include <winerror.h>
#include <windows.h>
#include <winnt.h>
#include <winbase.h>
#include <Psapi.h>
#include <windef.h>
#include <WTypes.h>
#include <process.h>
#include <ws2tcpip.h>
#include <direct.h>
#include <crtdbg.h>
#include <io.h>
#include <share.h>
#include <DbgHelp.h>
#include <intrin.h>

#endif //#ifdef ZCE_OS_WINDOWS

//==================================================================================================
//LINUX �����еĲ���ͷ�ļ�
#if defined(ZCE_OS_LINUX)

// Ϊ��ʹ��
#define _BSD_SOURCE

#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/io.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <time.h>
#include <netdb.h>
#include <pthread.h>
#include <iconv.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sysinfo.h>
#include <sys/inotify.h>

#if defined(__GNUC__)
#include <execinfo.h>
#endif //#if defined(__GNUC__)

#endif // #ifdef ZCE_OS_LINUX

//==================================================================================================

// C ͷ�ļ�
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>

//==================================================================================================
#if defined ZCE_OS_WINDOWS
#pragma warning ( push )
#pragma warning ( disable : 4702)
#pragma warning ( disable : 4267)
#endif

// c++ͷ�ļ�
#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>
#include <queue>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <utility>
#include <iostream>
#include <iomanip>
#include <functional>
#include <iomanip>
#include <typeinfo>
#include <algorithm>
#include <memory>
#include <limits>

//hash_map,hash_set�İ���
#if defined ZCE_OS_WINDOWS
//��VS2008�Ժ󣬲���unordered_map��unordered_set����������֮ǰ���������stlport��
//��Ȼ����stlport������ǿ��΢���Դ��������������汾Ҳ��������stlport
#if !defined _STLP_CONFIX_H && defined (_MSC_VER) && (_MSC_VER <= 1400)
#error " Please use stlport ,in Visual studio 2005, have not unordered_map and unordered_set ."
#endif

// hash��չ windows���õ�stlport
#include <unordered_map>
#include <unordered_set>
using std::tr1::unordered_map;
using std::tr1::unordered_set;

#elif defined ZCE_OS_LINUX

#include <unordered_set>
#include <unordered_map>
using std::unordered_map;
using std::unordered_set;

#endif  //#if defined ZCE_OS_WINDOWS

#if defined ZCE_OS_WINDOWS
#pragma warning ( pop )
#endif

//==================================================================================================
//����������ⲿ�⣬Ŀǰ����,rapidxml,MYSQL,SQLite,

//rapidxml XML�ļ���ͷ�ļ��Լ����أ���������Ŀ���rapidxml �⣬����������ֻ��ͷ�ļ���
#ifndef ZCE_USE_RAPIDXML
#define ZCE_USE_RAPIDXML 0
#endif

#if defined ZCE_USE_RAPIDXML && ZCE_USE_RAPIDXML == 1
#if defined (ZCE_OS_WINDOWS)
#pragma warning ( push )
#pragma warning ( disable : 4244)
#pragma warning ( disable : 4100)
#endif

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>

#if defined (ZCE_OS_WINDOWS)
#pragma warning ( pop )
#endif
#endif

// mysql�����Լ�ͷ�ļ�����
#ifndef ZCE_USE_MYSQL
#define ZCE_USE_MYSQL 1
#endif

#if defined ZCE_USE_MYSQL && ZCE_USE_MYSQL == 1
#include <mysql.h>
#endif

//SQLite3��ͷ�ļ�
#ifndef ZCE_USE_SQLITE3
#define ZCE_USE_SQLITE3 0
#endif

#if defined ZCE_USE_SQLITE3 && ZCE_USE_SQLITE3 == 1
#include <sqlite3.h>
#endif

//IPV6�ĺ꣬��ʱ����
//#if !defined ZCE_HAS_IPV6    1
//#define ZCE_HAS_IPV6
//#endif

//==================================================================================================
//�ֽ����Сͷ�ʹ�ͷ������
#define ZCE_LITTLE_ENDIAN  0x1234
#define ZCE_BIG_ENDIAN     0x4321

//Ŀǰ���ִ����ǿ�����Сͷ���ʹ�ͷ�������⣬��֪������֮�����״����Ƿ񻹻�Ϊ��ͷ������һ�Σ�
//��Ҫ��hash�ͼ��ܲ��ֵĴ��룬�ǿ��ǹ��ֽ������������ġ�
// Little Endian or Big Endian ?
// Overwrite the #define below if you know your architecture endianess
#if defined (__GLIBC__)
#  include <endian.h>
#endif

#ifndef ZCE_BYTES_ORDER
#if ( (defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)) || defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN) ) && !(defined(__LITTLE_ENDIAN__) || defined(__LITTLE_ENDIAN) || defined(_LITTLE_ENDIAN))
#  define ZCE_BYTES_ORDER ZCE_BIG_ENDIAN
#elif defined(__sparc) || defined(__sparc__) \
    || defined(__ppc__) || defined(_POWER) || defined(__powerpc__) || defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC) || defined(PPC) \
    || defined(__powerpc__) || defined(__powerpc) || defined(powerpc) \
    || defined(__hpux)  || defined(__hppa)  || defined(_MIPSEB) || defined(__s390__)
#  define  ZCE_BYTES_ORDER ZCE_BIG_ENDIAN
#else
// Little Endian assumed. PDP Endian and other very rare endian format are unsupported.
//��ʵ���˴�ͷ����Сͷ�����⣬���������ɱ������渴�ӡ�
#define ZCE_BYTES_ORDER    ZCE_LITTLE_ENDIAN
#endif
#endif  //#ifndef ZCE_BYTES_ORDER

//==================================================================================================

//��׼������ֵ���岿�֣�����VS2010ǰ�Ŀ�û�а���C99�ı�׼ִ�У�����������������û�С�
//���еĴ�����,(�������ڼ���API)����׼����long��longlong���ֶ��壬��׼�����־͵�СJJ . ע��������

//�������Ͷ���,�Ƽ�ʹ�ã��ر���64λ��uint64_t,

//LINUX���Ѿ�����صĶ����ˣ�����
#if defined(ZCE_OS_LINUX)
#include <stdint.h>
#include <inttypes.h>
#endif //#if defined(ZCE_OS_LINUX)

//WINDOWS�£����ֲ�ͬ�����ֲ�һ�£���ֻ���Լ���
#if defined(ZCE_OS_WINDOWS)

//��VS2010Ϊֹ��ssize_t��û�б�֧��
#if defined (ZCE_WIN64)
typedef SSIZE_T ssize_t;
#else
typedef int ssize_t;
#endif

//VC++ 2010���Լ����������׼��
#if _MSC_VER >= 1500
#include <stdint.h>
//VC++ 2005
#else

//The stdint declaras
typedef  signed char        int8_t;
typedef  short              int16_t;
typedef  int                int32_t;
typedef  unsigned char      uint8_t;
typedef  unsigned short     uint16_t;
typedef  unsigned int       uint32_t;

//
#if _MSC_VER >= 1300
typedef unsigned long long  uint64_t;
typedef long long           int64_t;
#else
typedef unsigned __int64    uint64_t;
typedef __int64             int64_t;
#endif //#if _MSC_VER >= 1300

#endif //#if _MSC_VER >= 1500

#endif //#if defined(ZCE_OS_WINDOWS)

//==================================================================================================
//���ֺ궨�壬���붨�壬һЩ�Ƚϳ��õĺ꣬�������ʡһЩ����

//�ⲿͷ�ļ�����������ǰ��ѽ������Ԥ����ͷ�ļ��Ͳ���������
//��������ڶ�����߱�����������Ĵ���Ͳ���Ԥ�����ˣ���Ҫ���ڼӿ������룬��LINUXĿǰ����֧��

#if defined ZCE_OS_WINDOWS
#pragma hdrstop
#endif //#if defined ZCE_OS_WINDOWS

//���붯̬���õĶ���
#if defined ZCE_OS_WINDOWS && defined ZCELIB_HASDLL
#  ifdef BUILD_ZCELIB_DLL
#    define ZCELIB_EXPORT __declspec (dllexport)
#  else
#    define ZCELIB_EXPORT __declspec (dllimport)
#  endif //#if defined ZCE_OS_WINDOWS && defined ZCELIB_HASDLL
#else
#  define ZCELIB_EXPORT
#endif //#if defined ZCE_OS_WINDOWS && defined ZCELIB_HASDLL

//���ǳ�ACE_UNUSED_ARG��ѽ���ҳ���ѽ��windows��Ҳ��Ҳ���Կ��Ƕ����__noopѽ��
#if !defined (ZCE_UNUSED_ARG)
#if defined ZCE_OS_LINUX
#  if defined (__GNUC__) && ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2)))
#    define ZCE_UNUSED_ARG(a) (void) (a)
#  else
#    define ZCE_UNUSED_ARG(a) do {/* null */} while (&a == 0)
#  endif
#elif defined ZCE_OS_WINDOWS
#    define ZCE_UNUSED_ARG(a)    UNREFERENCED_PARAMETER(a)
#  endif
#endif //#if !defined (ZCE_UNUSED_ARG)

//BOOL����
#ifndef FALSE
#  define FALSE               0
#endif
#ifndef TRUE
#  define TRUE                1
#endif

//NULL
#ifndef NULL
#  ifdef __cplusplus
#    define NULL    0
#  else
#    define NULL    ((void *)0)
#  endif
#endif

//__FUNCTION__������滻���������ܰ������������ϸ�ĺ���������Ϣ
#if defined ZCE_OS_WINDOWS
#define __ZCE_FUNCTION__   __FUNCSIG__
#elif defined ZCE_OS_LINUX
#define __ZCE_FUNCTION__   __PRETTY_FUNCTION__
#else
#define __ZCE_FUNCTION__   __FUNCTION__
#endif

//
#if defined(ZCE_OS_LINUX) && defined(__GNUC__)
#define ZCE_LIKELY(x)      __builtin_expect (x, 1)
#define ZCE_UNLIKELY(x)    __builtin_expect (x, 0)
#else
#define ZCE_LIKELY(x)      (x)
#define ZCE_UNLIKELY(x)    (x)
#endif

//BITλ��һЩ���������

//���û���������ֵĳ��λ���ϵ�bitλ
//_value û����()�����������ɵģ���ú����룬�Ǻ�
#if !defined(ZCE_SET_BITS)
#  define ZCE_SET_BITS(set_value, bits) (set_value |= (bits))
#endif
#if !defined(ZCE_CLR_BITS)
#  define ZCE_CLR_BITS(clr_value, bits) (clr_value &= ~(bits))
#endif

//���ĳ��bitλ�Ƿ�������
#if !defined(ZCE_BIT_IS_SET)
#  define ZCE_BIT_IS_SET(compare_value, bits) (((compare_value) & (bits)) != 0)
#endif
#if !defined(ZCE_BIT_ISNOT_SET)
#  define ZCE_BIT_ISNOT_SET(compare_value, bits) (((compare_value) & (bits)) == 0)
#endif

///�����������,ע�⴫�ݵ��ͱ��
#ifndef ZCE_ARRAY_SIZE
#define ZCE_ARRAY_SIZE(ary) (sizeof(ary)/sizeof((ary)[0]))
#endif

//һЩ�ֽ��򽻻��ĺ�
//#if (_MSC_VER > 1300) && (defined(CPU_IA32) || defined(CPU_X64)) /* MS VC */
//_MSC_VER > 1300  _byteswap_ushort,_byteswap_ulong,_byteswap_uint64
//#if defined(__GNUC__) && (__GNUC__ >= 4) && (__GNUC__ > 4 || __GNUC_MINOR__ >= 3)
//GCC 4.3__builtin_bswap64,__builtin_bswap16,__builtin_bswap32

#ifndef ZCE_SWAP_UINT16
#define ZCE_SWAP_UINT16(x)  ((((x) & 0xff00) >>  8) | (((x) & 0x00ff) <<  8))
#endif
#ifndef ZCE_SWAP_UINT32
#define ZCE_SWAP_UINT32(x)  ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
                             (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#endif
#ifndef ZCE_SWAP_UINT64
#define ZCE_SWAP_UINT64(x)   ((((x) & 0xff00000000000000ULL) >> 56) | (((x) & 0x00ff000000000000ULL) >>  40) | \
                              (((x) & 0x0000ff0000000000ULL) >> 24) | (((x) & 0x000000ff00000000ULL) >>  8) |   \
                              (((x) & 0x00000000ff000000ULL) << 8 ) | (((x) & 0x0000000000ff0000ULL) <<  24) |  \
                              (((x) & 0x000000000000ff00ULL) << 40 ) | (((x) & 0x00000000000000ffULL) <<  56))
#endif

//����һ��ntol ntos,nothll�ȣ���Ҫ������64��ת������������ϵͳ���У�
#if (ZCE_BYTES_ORDER == ZCE_LITTLE_ENDIAN)
# define ZCE_HTONS(x)  ZCE_SWAP_UINT16 (x)
# define ZCE_NTOHS(x)  ZCE_SWAP_UINT16 (x)
# define ZCE_HTONL(x)  ZCE_SWAP_UINT32 (x)
# define ZCE_NTOHL(x)  ZCE_SWAP_UINT32 (x)
# define ZCE_HTONLL(x) ZCE_SWAP_UINT64 (x)
# define ZCE_NTOHLL(x) ZCE_SWAP_UINT64 (x)
#else
# define ZCE_HTONS(x)  (x)
# define ZCE_NTOHS(x)  (x)
# define ZCE_HTONL(x)  (x)
# define ZCE_NTOHL(x)  (x)
# define ZCE_HTONLL(x) (x)
# define ZCE_NTOHLL(x) (x)
#endif /* end if (ZCE_BYTES_ORDER == ZCE_LITTLE_ENDIAN) */


//����д��Ϊ�˱���BUS ERROR���⣬�����Լ�Ҳ����100%�϶���������ܽ��BUS ERROR��
//��Ϊ��û�л���������صĲ��ԡ�
//ע��#pragma pack(push, 1) ��__attribute__ ((packed)) ��������ģ�
//���²ο�֮ ����
//ǰ�߸��߱������ṹ������ڲ��ĳ�Ա��������ڵ�һ�������ĵ�ַ��ƫ�����Ķ��뷽ʽ��
//ȱʡ����£�������������Ȼ�߽���룬�������������Ȼ����߽��n�� ʱ������n���룬
//��������Ȼ�߽���룻���߸��߱�����һ���ṹ�������������ϻ���һ�����͵ı���
//(����)�����ַ�ռ�ʱ�ĵ�ַ���뷽ʽ��Ҳ�������������__attribute__((aligned(m)))
//������һ�����ͣ���ô�����͵ı����ڷ����ַ�ռ�ʱ�����ŵĵ�ַһ������m�ֽڶ���
//(m�� ����2���ݴη�)��������ռ�õĿռ䣬����С,Ҳ��m�����������Ա�֤�����������洢�ռ��ʱ��
//ÿһ��Ԫ�صĵ�ַҲ�ǰ���m�ֽڶ��롣 __attribute__((aligned(m)))Ҳ����������һ�������ı�����
//���Ͽ��Կ���__attribute__((aligned(m)))�Ĺ��ܸ�ȫ��


#if defined ZCE_OS_WINDOWS

#pragma pack(push, 1)

struct ZU16_STRUCT
{
    uint16_t value_;
};
struct ZU32_STRUCT
{
    uint32_t value_;
};
struct ZU64_STRUCT
{
    uint64_t value_;
};

#pragma pack(pop)

#elif defined ZCE_OS_LINUX
//__attribute__ ((packed)) �������Ŀ�������������Ա���BUS ERROR����
struct ZU16_STRUCT
{
    uint16_t value_;
} __attribute__ ((packed));
struct ZU32_STRUCT
{
    uint32_t value_;
} __attribute__ ((packed));
struct ZU64_STRUCT
{
    uint64_t value_;
} __attribute__ ((packed));

#endif


///��һ��(char *)ָ���ڶ�ȡ(Ҳ��������д��)һ��uint16_t,or uint32_t or uint64_t
# define ZBYTE_TO_UINT16(ptr)  ((ZU16_STRUCT *)(ptr))->value_
# define ZBYTE_TO_UINT32(ptr)  ((ZU32_STRUCT *)(ptr))->value_
# define ZBYTE_TO_UINT64(ptr)  ((ZU64_STRUCT *)(ptr))->value_

///��һ��(char *)ָ���ڶ�ȡuint16_t,or uint32_t or uint64_t �������ڵ�ary_index��Ԫ��ע�������±���ֵ�������ε��±꣬(������ptr���±�)
# define ZINDEX_TO_UINT16(ptr,ary_index)  (((ZU16_STRUCT *)(ptr))+(ary_index))->value_
# define ZINDEX_TO_UINT32(ptr,ary_index)  (((ZU32_STRUCT *)(ptr))+(ary_index))->value_
# define ZINDEX_TO_UINT64(ptr,ary_index)  (((ZU64_STRUCT *)(ptr))+(ary_index))->value_

///��һ��(char *)ָ����д��һ��uint16_t,or uint32_t or uint64_t
# define ZUINT16_TO_BYTE(ptr,wr_data)  ((ZU16_STRUCT *)(ptr))->value_ = (wr_data)
# define ZUINT32_TO_BYTE(ptr,wr_data)  ((ZU32_STRUCT *)(ptr))->value_ = (wr_data)
# define ZUINT64_TO_BYTE(ptr,wr_data)  ((ZU64_STRUCT *)(ptr))->value_ = (wr_data)

//��һ��(char *)ָ����д��һ��uint16_t,or uint32_t or uint64_t�������ڲ���ary_index��Ԫ��ע�������±���ֵ�������ε��±꣬(������ptr���±�)
# define ZUINT16_TO_INDEX(ptr,ary_index,wr_data)  (((((ZU16_STRUCT *)(ptr))+(ary_index))->value_) = (wr_data))
# define ZUINT32_TO_INDEX(ptr,ary_index,wr_data)  (((((ZU32_STRUCT *)(ptr))+(ary_index))->value_) = (wr_data))
# define ZUINT64_TO_INDEX(ptr,ary_index,wr_data)  (((((ZU64_STRUCT *)(ptr))+(ary_index))->value_) = (wr_data))


//����Ĵ���ӹ����ϵ�ͬ����Ĵ��롣������Щд�ᵼ��BUS ERROR���⡣
//# define ZBYTE_TO_UINT32(ptr)  (*(uint32_t *)(ptr))
//# define ZINDEX_TO_UINT32(ptr,ary_index)  (*(((uint32_t *)(ptr))+(ary_index)))
//# define ZUINT32_TO_BYTE(ptr,wr_data)  ((*(uint32_t *)(ptr)) = (wr_data))
//# define ZUINT32_TO_INDEX(ptr,ary_index,wr_data)  ((*(((uint32_t *)(ptr))+(ary_index))) = (wr_data))

//�����ֽ���Ķ�ȡ�����ͣ����Σ�64λ�����εķ�ʽ
//������˼����ʹ��ˣ�����������е�������ˣ��м���д��ζ��������벻���׼��ܣ�MD5�ȴ�����ΪɶҪ��
//���ֽ������أ�
//��һ��Ϊ�˱����㷨��һ���ԣ�һ̨Сͷ�ֽ�������ϼ���ĵõ������ģ�������һ̨��ͷ��Ļ���Ҫ�ܽ⿪��
//������˵��ͷ��Сͷ�Ļ�����ͬ����һ��buffer�����ܵõ�������Ӧ����һ�µģ�
//���Ҫ���������ֽ�����ȡ��������������һ�µġ����Ծ�Ҫ�����ֽ��������ˡ�������ؼ������ء�
//�ڶ�����Щ�㷨��Ϊ�˼ӿ촦������������������ģ���ʵ�����棬�Ǳ��뿼���ֽ�˳��ģ��������ǵ�
//CRC32�㷨������������⣬���������㷨������뿼���ֽ������⣬Сͷ�������������������Ҫ����ת��
//��������Щ�㷨��ʵ��ȷҪ����������ô�ͷ�򣬻���Сͷ��������ͱ��뿼����Ļ�����
//
//��Ĭ�ϱ���������ϣ��һ���������Сͷ��һ�����ҵĴ������99.99%��������X86�ܹ�֮�£���һ����
//Сͷ���Ǵӵ͵��ߣ����ֽ�˳��һ�£�������Ҳ����һ�㡣

#if (ZCE_BYTES_ORDER == ZCE_LITTLE_ENDIAN)

///��һ��(char *)ָ���ڶ�ȡСͷ�ֽ����uint16_t,or uint32_t or uint64_t����Сͷ�ֽ���Ļ����ϲ������ı�
# define ZBYTE_TO_LEUINT16(ptr)    ZBYTE_TO_UINT16(ptr)
# define ZBYTE_TO_LEUINT32(ptr)    ZBYTE_TO_UINT32(ptr)
# define ZBYTE_TO_LEUINT64(ptr)    ZBYTE_TO_UINT64(ptr)

///��һ��(char *)ָ���ڶ�ȡСͷ�ֽ����uint16_t,or uint32_t or uint64_t �������ڵ�ary_index��Ԫ��ע�������±���ֵ�������ε��±꣬(������ptr���±�)
# define ZINDEX_TO_LEUINT16(ptr,ary_index)  ZINDEX_TO_UINT16(ptr,ary_index)
# define ZINDEX_TO_LEUINT32(ptr,ary_index)  ZINDEX_TO_UINT32(ptr,ary_index)
# define ZINDEX_TO_LEUINT64(ptr,ary_index)  ZINDEX_TO_UINT64(ptr,ary_index)

///��һ��(char *)ָ����д��һ��Сͷ�ֽ����uint16_t,or uint32_t or uint64_t����Сͷ�ֽ���Ļ����ϲ������ı�
# define ZLEUINT16_TO_BYTE(ptr,wr_data)  ZUINT16_TO_BYTE(ptr,wr_data)
# define ZLEUINT32_TO_BYTE(ptr,wr_data)  ZUINT32_TO_BYTE(ptr,wr_data)
# define ZLEUINT64_TO_BYTE(ptr,wr_data)  ZUINT64_TO_BYTE(ptr,wr_data)

//��һ��(char *)ָ����д��һ��Сͷ�ֽ����uuint16_t,or uint32_t or uint64_t�������ڲ���ary_index��Ԫ��ע�������±���ֵ�������ε��±꣬(������ptr���±�)
# define ZLEUINT16_TO_INDEX(ptr,ary_index,wr_data)  ZUINT16_TO_INDEX(ptr,ary_index,wr_data)
# define ZLEUINT32_TO_INDEX(ptr,ary_index,wr_data)  ZUINT32_TO_INDEX(ptr,ary_index,wr_data)
# define ZLEUINT64_TO_INDEX(ptr,ary_index,wr_data)  ZUINT64_TO_INDEX(ptr,ary_index,wr_data)

#else

# define ZBYTE_TO_LEUINT16(ptr)  ZCE_SWAP_UINT16(((ZU16_STRUCT *)(ptr))->value_)
# define ZBYTE_TO_LEUINT32(ptr)  ZCE_SWAP_UINT32(((ZU32_STRUCT *)(ptr))->value_)
# define ZBYTE_TO_LEUINT64(ptr)  ZCE_SWAP_UINT64(((ZU64_STRUCT *)(ptr))->value_)

//
# define ZINDEX_TO_LEUINT16(ptr,ary_index)  ZCE_SWAP_UINT16((((ZU16_STRUCT *)(ptr))+(ary_index))->value_)
# define ZINDEX_TO_LEUINT32(ptr,ary_index)  ZCE_SWAP_UINT32((((ZU32_STRUCT *)(ptr))+(ary_index))->value_)
# define ZINDEX_TO_LEUINT64(ptr,ary_index)  ZCE_SWAP_UINT64((((ZU64_STRUCT *)(ptr))+(ary_index))->value_)

//
# define ZLEUINT16_TO_BYTE(ptr,wr_data)  ZBYTE_TO_UINT16(ptr) = ZCE_SWAP_UINT16(wr_data))
# define ZLEUINT32_TO_BYTE(ptr,wr_data)  ZBYTE_TO_UINT32(ptr) = ZCE_SWAP_UINT32(wr_data))
# define ZLEUINT64_TO_BYTE(ptr,wr_data)  ZBYTE_TO_UINT64(ptr) = ZCE_SWAP_UINT64(wr_data))

//
# define ZLEUINT16_TO_INDEX(ptr,ary_index,wr_data)  ZINDEX_TO_UINT16(ptr) = ZCE_SWAP_UINT16(wr_data))
# define ZLEUINT32_TO_INDEX(ptr,ary_index,wr_data)  ZINDEX_TO_UINT32(ptr) = ZCE_SWAP_UINT32(wr_data))
# define ZLEUINT64_TO_INDEX(ptr,ary_index,wr_data)  ZINDEX_TO_UINT64(ptr) = ZCE_SWAP_UINT64(wr_data))

#endif /* end if (ZCE_BYTES_ORDER == ZCE_LITTLE_ENDIAN) */

//������ת����λ��������ͬλ����ʾ����Զ�����������64λ�����Ĳ�����
//ע��n��Ҫ����һ���������ֵ������32λ��ZCE_ROTL32��nҪС��31������
#ifndef ZCE_ROTL16
#define ZCE_ROTL16(word, n)  (((word) <<  ((n)&15))  | ((word) >>  (16 - ((n)&15))))
#endif
#ifndef ZCE_ROTR16
#define ZCE_ROTR16(word, n)  (((word) >>  ((n)&15))  | ((word) <<  (16 - ((n)&15))))
#endif
#ifndef ZCE_ROTL32
#define ZCE_ROTL32(dword, n) (((dword) << ((n)&31)) |  ((dword) >> (32 - ((n)&31))))
#endif
#ifndef ZCE_ROTR32
#define ZCE_ROTR32(dword, n) (((dword) >> ((n)&31)) |  ((dword) << (32 - ((n)&31))))
#endif
#ifndef ZCE_ROTL64
#define ZCE_ROTL64(qword, n) (((qword) << ((n)&63)) |  ((qword) >> (64 - ((n)&63))))
#endif
#ifndef ZCE_ROTR64
#define ZCE_ROTR64(qword, n) (((qword) >> ((n)&63)) |  ((qword) << (64 - ((n)&63))))
#endif

#define ZUINT16_0BYTE(data)    ((data) & 0xFF)
#define ZUINT16_1BYTE(data)    ((data) >> 8)

#define ZUINT32_0BYTE(data)    ((data)  & 0xFF)
#define ZUINT32_1BYTE(data)    (((data) >> 8)  & 0xFF)
#define ZUINT32_2BYTE(data)    (((data) >> 16) & 0xFF)
#define ZUINT32_3BYTE(data)    ((data)  >> 24)

#define ZUINT64_0BYTE(data)    ((data)  & 0xFF)
#define ZUINT64_1BYTE(data)    (((data) >> 8) & 0xFF)
#define ZUINT64_2BYTE(data)    (((data) >> 16) & 0xFF)
#define ZUINT64_3BYTE(data)    (((data) >> 24) & 0xFF)
#define ZUINT64_4BYTE(data)    (((data) >> 32) & 0xFF)
#define ZUINT64_5BYTE(data)    (((data) >> 40) & 0xFF)
#define ZUINT64_6BYTE(data)    (((data) >> 48) & 0xFF)
#define ZUINT64_7BYTE(data)    ((data)  >> 56)

///���ָ���Ƿ�32λ�������64λ����
#ifndef ZCE_IS_ALIGNED_32
#define ZCE_IS_ALIGNED_32(p) (0 == (0x3 & ((const char*)(p) - (const char*)0)))
#endif
#ifndef ZCE_IS_ALIGNED_64
#define ZCE_IS_ALIGNED_64(p) (0 == (0x7 & ((const char*)(p) - (const char*)0)))
#endif

// ȡ��Сֵ, min max��linux��û�ж���
#define ZCE_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define ZCE_MIN(a,b) (((a) < (b)) ? (a) : (b))

// ���Ƶ���󳤶�
#if !defined (NAME_MAX)
#  if defined (FILENAME_MAX)
#    define NAME_MAX FILENAME_MAX
#  elif defined (_MAX_FNAME)
#    define NAME_MAX _MAX_FNAME
#  else /* _MAX_FNAME */
#    define NAME_MAX 256
#  endif /* MAXNAMLEN */
#endif /* !NAME_MAX */

// �������Ƶ���󳤶�
#if !defined (HOST_NAME_MAX)
#  define HOST_NAME_MAX 256
#endif /* !HOST_NAME_MAX */

//·������󳤶ȣ�
//�ռ�һ��С֪ʶ��ע��һ����ʵMAX_PATHδ���������ã�MSһ��Ķ�����260�����ǣ���ʵ����Գ���,
#ifndef MAX_PATH
#  define MAX_PATH 512
#endif
#if !defined (PATH_MAX)
#  if defined (_MAX_PATH)
#    define PATH_MAX _MAX_PATH
#  elif defined (MAX_PATH)
#    define PATH_MAX MAX_PATH
#  elif defined (_POSIX_PATH_MAX)
#     define PATH_MAX _POSIX_PATH_MAX
#  else /* !_MAX_PATH */
#    define PATH_MAX 512
#  endif /* _MAX_PATH */
#endif /* !PATH_MAX */

// һЩC�����������������ּ��ݣ������������
#if defined ZCE_OS_WINDOWS

#if _MSC_VER <= 1300
#define snprintf     _snprintf
#define vsnprintf    _vsnprintf
#else
//��VS2005�Ժ�ʹ�ð�ȫAPI,��֤WINDOWS�¸��ӽӽ���LINUX����֤ĩβ�����'\0'
#define snprintf(buffer,buf_count,fmt,...) _snprintf_s((buffer),(buf_count),((buf_count)-1),(fmt),__VA_ARGS__)
#define vsnprintf(buffer,buf_count,fmt,argptr_list)  _vsnprintf_s((buffer),((buf_count)-1),(buf_count),(fmt),(argptr_list))
#endif

#define strtoull _strtoui64
#define strcasecmp   _stricmp
#define strncasecmp  _strnicmp

#endif

//==================================================================================================
//Windows �µ��Զ�����
#if defined (ZCE_OS_WINDOWS) && defined (_MSC_VER)

// ���������������
#  pragma comment(lib, "ws2_32.lib")
#  pragma comment(lib, "mswsock.lib")
#  pragma comment(lib, "psapi.lib")
#  pragma comment(lib, "dbghelp.lib")
#  if defined (ZCE_HAS_IPV6) && (ZCE_HAS_IPV6==1)
#    pragma comment(lib, "iphlpapi.lib")
#  endif
#  if defined (ZCE_USE_MYSQL) && (ZCE_USE_MYSQL==1)
#    pragma comment(lib, "libmysql.lib")
#  endif

//������ܵ�2008��֧��
#if defined ZCE_SUPPORT_WINSVR2008 
#  pragma comment(lib, "dbghelp.lib")
#endif



#if defined ZCE_WIN32
// vc71(2003):
#if defined(_MSC_VER) && (_MSC_VER == 1310)
#  define ZCE_PLATFORM_TOOLSET "win32-v71"
// vc80(2005):
#elif defined(_MSC_VER) && (_MSC_VER == 1400)
#  define ZCE_PLATFORM_TOOLSET "win32-v80"
// vc90(VS2008):
#elif defined(_MSC_VER) && (_MSC_VER == 1500)
#  define ZCE_PLATFORM_TOOLSET "win32-v90"
// vc10(VS2010):
#elif defined(_MSC_VER) && (_MSC_VER == 1600)
#  define ZCE_PLATFORM_TOOLSET "win32-v100"
// vc11(VS2012):
#elif defined(_MSC_VER) && (_MSC_VER == 1700)
#  define ZCE_PLATFORM_TOOLSET "win32-v110"
#elif defined(_MSC_VER) && (_MSC_VER == 1800)
#  define ZCE_PLATFORM_TOOLSET "win32-v120"
#endif
#elif defined ZCE_WIN64
// vc71(2003):
#if defined(_MSC_VER) && (_MSC_VER == 1310)
#  define ZCE_PLATFORM_TOOLSET "x64-v71"
// vc80(2005):
#elif defined(_MSC_VER) && (_MSC_VER == 1400)
#  define ZCE_PLATFORM_TOOLSET "x64-v80"
// vc90(VS2008):
#elif defined(_MSC_VER) && (_MSC_VER == 1500)
#  define ZCE_PLATFORM_TOOLSET "x64-v90"
// vc10(VS2010):
#elif defined(_MSC_VER) && (_MSC_VER == 1600)
#  define ZCE_PLATFORM_TOOLSET "x64-v100"
// vc11(VS2012):
#elif defined(_MSC_VER) && (_MSC_VER == 1700)
#  define ZCE_PLATFORM_TOOLSET "x64-v110"
#elif defined(_MSC_VER) && (_MSC_VER == 1800)
#  define ZCE_PLATFORM_TOOLSET "x64-v120"
#endif
#endif

//�����ڲ�����ͳһʹ��ZCE_PLAT_TOOLSET_CONF����꣬�򻯴�ҵı��빤������
#if defined _DEBUG || defined DEBUG
#  define ZCE_PLAT_TOOLSET_CONF ZCE_PLATFORM_TOOLSET"-Debug"
#else
#  define ZCE_PLAT_TOOLSET_CONF ZCE_PLATFORM_TOOLSET"-Release"
#endif

#if !defined ZCE_LIB_LIBARY_NAME
#define ZCE_LIB_LIBARY_NAME   "zcelib-"ZCE_PLAT_TOOLSET_CONF".lib"
#endif

//�Զ������İ������ӣ�����Ĳ���
#pragma comment(lib, ZCE_LIB_LIBARY_NAME  )

#endif





#endif //ZCE_LIB_PREDEFINE_H_
