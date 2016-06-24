
#include "zce_predefine.h"
#include "zce_trace_debugging.h"
#include "zce_trace_aid_debug.h"


//=====================================================================================================================



//=====================================================================================================================

int ZCE_LIB::backtrace_stack(ZCE_LOG_PRIORITY dbg_lvl,
                             const char *dbg_info)
{

    //���ٺ����Ĳ���
    const size_t SIZE_OF_BACKTRACE_FUNC = 100;

#if defined(ZCE_OS_LINUX)
    ZCE_LOG(dbg_lvl, "[BACKTRACE]This program compiled by Linux GCC. %s", dbg_info);
    //Windows �±�����2008����VISTA֮���SDK��֧�֣�
#elif defined(ZCE_OS_WINDOWS) && ZCE_SUPPORT_WINSVR2008 == 1
    ZCE_LOG(dbg_lvl, "[BACKTRACE]This program compiled by Windows Visual studio .%s", dbg_info);
#else
    ZCE_UNUSED_ARG(SIZE_OF_BACKTRACE_FUNC);
    ZCE_LOG(dbg_lvl, "[BACKTRACE]back_trace_stack don't support this system.%s", dbg_info);
#endif


    //����������ṩ��Linux �µ�GCCʹ�õ�
#if defined(ZCE_OS_LINUX)


    void *backtrace_buffer[SIZE_OF_BACKTRACE_FUNC];
    char **symbols_strings;

    int sz_of_stack = ::backtrace(backtrace_buffer, SIZE_OF_BACKTRACE_FUNC);

    //
    symbols_strings = ::backtrace_symbols(backtrace_buffer, sz_of_stack);

    if (symbols_strings == NULL)
    {
        ZCE_LOG(dbg_lvl, "%s", "[BACKTRACE] backtrace_symbols return fail.");
    }

    //��ӡ���еĶ�ջ��Ϣ,��Щʱ����Ϣ�޷���ʾ���ű�����ʹ��
    for (int j = 0; j < sz_of_stack; j++)
    {
        ZCE_LOG(dbg_lvl, "[BACKTRACE] %u, %s.", j + 1, symbols_strings[j]);
    }

    //�ͷſռ�
    ::free(symbols_strings);

#elif defined(ZCE_OS_WINDOWS) && ZCE_SUPPORT_WINSVR2008 == 1

    //�һ�û��ʱ�俴��dbghelp���еĶ���,Ŀǰ�Ĵ���ο���һ���汾�Ӷ�,Ŀǰ�������������pdb�ļ���
    //http://blog.csdn.net/skies457/article/details/7201185

    // Max length of symbols' name.
    const size_t MAX_NAME_LENGTH = 256;

    // Store register addresses.
    CONTEXT context;
    // Call stack.
    STACKFRAME64 stackframe;
    // Handle to current process & thread.
    HANDLE process, cur_thread;
    // Generally it can be subsitituted with 0xFFFFFFFF & 0xFFFFFFFE.
    // Debugging symbol's information.
    PSYMBOL_INFO symbol;
    // Source information (file name & line number)
    IMAGEHLP_LINE64 source_info;
    // Source line displacement.
    DWORD displacement;

    // Initialize PSYMBOL_INFO structure.
    // Allocate a properly-sized block.
    symbol = (PSYMBOL_INFO)malloc(sizeof(SYMBOL_INFO) + (MAX_NAME_LENGTH) * sizeof(char));
    memset(symbol, 0, sizeof(SYMBOL_INFO) + (MAX_NAME_LENGTH) * sizeof(TCHAR));
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);  // SizeOfStruct *MUST BE* set to sizeof(SYMBOL_INFO).
    symbol->MaxNameLen = MAX_NAME_LENGTH;

    // Initialize IMAGEHLP_LINE64 structure.
    memset(&source_info, 0, sizeof(IMAGEHLP_LINE64));
    source_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    // Initialize STACKFRAME64 structure.
    RtlCaptureContext(&context);            // Get context.
    memset(&stackframe, 0, sizeof(STACKFRAME64));

    // Fill in register addresses (EIP, ESP, EBP).

    stackframe.AddrPC.Mode = AddrModeFlat;
    stackframe.AddrStack.Mode = AddrModeFlat;
    stackframe.AddrFrame.Mode = AddrModeFlat;

#if defined ZCE_WIN32
    stackframe.AddrPC.Offset = context.Eip;
    stackframe.AddrStack.Offset = context.Esp;
    stackframe.AddrFrame.Offset = context.Ebp;
#elif defined ZCE_WIN64
    stackframe.AddrPC.Offset = context.Rip;
    stackframe.AddrStack.Offset = context.Rsp;
    stackframe.AddrFrame.Offset = context.Rbp;
#else
#endif
    // Get current process & thread.
    process = GetCurrentProcess();
    cur_thread = GetCurrentThread();

    // Initialize dbghelp library.
    if (!SymInitialize(process, NULL, TRUE))
    {
        return -1;
    }

    //��Щ�ռ��Ǿ����㹻�ģ���Ҳ������ϸ�ļ����
    const size_t LINE_OUTLEN = 1024;
    char line_out[LINE_OUTLEN];
    int use_len = 0;

    uint32_t k = 0;
    // Enumerate call stack frame.
    while (StackWalk64(IMAGE_FILE_MACHINE_I386,
                       process,
                       cur_thread,
                       &stackframe,
                       &context,
                       NULL,
                       SymFunctionTableAccess64,
                       SymGetModuleBase64,
                       NULL))
    {
        use_len = 0;
        // End reaches.
        if (stackframe.AddrFrame.Offset == 0 || k > SIZE_OF_BACKTRACE_FUNC)
        {
            break;
        }

        // Get symbol.
        if (SymFromAddr(process, stackframe.AddrPC.Offset, NULL, symbol))
        {
            use_len += snprintf(line_out + use_len, LINE_OUTLEN - use_len, " %s", symbol->Name);

        }

        if (SymGetLineFromAddr64(process, stackframe.AddrPC.Offset,
                                 &displacement,
                                 &source_info))
        {
            // Get source information.
            use_len += snprintf(line_out + use_len, LINE_OUTLEN - use_len, "\t[ %s: %d] at addr 0x %08llX",
                                source_info.FileName,
                                source_info.LineNumber,
                                stackframe.AddrPC.Offset);
        }
        else
        {
            // If err_code == 0x1e7, no symbol was found.
            if (GetLastError() == 0x1E7)
            {
                use_len += snprintf(line_out + use_len, LINE_OUTLEN - use_len, "%s", "\tNo debug symbol loaded for this function.");
            }
        }
        ZCE_LOG(dbg_lvl, "[BACKTRACE] %u, %s.", k + 1, line_out);
        ++k;
    }

    SymCleanup(process);    // Clean up and exit.
    free(symbol);

#endif

    //
    return 0;

}



//������ӡһ��ָ���ڲ����ݵĺ�������16���Ƶķ�ʽ��ӡ
//�� 11 02 03 0E E0         ..... ��ʽ�������ָ����Ϣ��
void ZCE_LIB::debug_pointer(ZCE_LOG_PRIORITY dbg_lvl,
                            const char *dbg_info,
                            const unsigned char  *debug_ptr,
                            size_t data_len)
{
    ZCE_LOG(dbg_lvl, "[DEBUG_POINTER] out pointer address[%p] [%s].", dbg_info, dbg_info);
    //60���ַ�����
    const unsigned int LINE_OUT_NUM = 60;

    //��Щ�ռ��Ǿ����㹻�ģ���Ҳ������ϸ�ļ����
    const size_t LINE_OUTLEN = 1024;
    char line_out[LINE_OUTLEN];
    int use_len = 0;

    unsigned char ascii_str[LINE_OUT_NUM + 1];
    ascii_str[LINE_OUT_NUM] = '\0';

    size_t j = 0;
    for (size_t i = 0; i < data_len ; ++i, ++j)
    {

        //����
        if (i % LINE_OUT_NUM == 0 && i != 0  )
        {
            use_len += snprintf(line_out + use_len, LINE_OUTLEN - use_len, "  %s", ascii_str);

            ZCE_LOG(dbg_lvl, "[DEBUG_POINTER] %s.", line_out);
            //��ͷ��ʼ��¼
            j = 0;
            use_len = 0;

        }
        //����ط�Ч���е�ͣ�������Կ����Ż�һ��
        unsigned char bytmp = *(debug_ptr + i);
        use_len += snprintf(line_out + use_len, LINE_OUTLEN - use_len, " %02X ", bytmp);

        //ֻ��������ʾ���ַ��������ַ�����Ϊ'.'
        if (bytmp <= 0x20 || bytmp >= 0xFA )
        {
            bytmp = '.';
        }
        ascii_str [j] = bytmp;
    }

    //�������LINE_OUT_NUM ����������Ҫ���룬��������ַ���
    if (data_len % LINE_OUT_NUM != 0 )
    {
        //Ϊ�˶��룬��ӡ�ո�
        for (size_t k = 0; k < LINE_OUT_NUM - data_len % LINE_OUT_NUM; k++)
        {
            use_len += snprintf(line_out + use_len, LINE_OUTLEN - use_len, "%s", "    ");
        }

        ascii_str[j] = '\0';
        use_len += snprintf(line_out + use_len, LINE_OUTLEN - use_len, "  %s\n", ascii_str);
    }
    ZCE_LOG(dbg_lvl, "[DEBUG_POINTER] %s", line_out);

    return;
}
