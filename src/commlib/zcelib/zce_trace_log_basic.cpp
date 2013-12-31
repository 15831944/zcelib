

#include "zce_predefine.h"
#include "zce_os_adapt_predefine.h"
#include "zce_os_adapt_dirent.h"
#include "zce_os_adapt_thread.h"
#include "zce_os_adapt_process.h"
#include "zce_os_adapt_time.h"
#include "zce_os_adapt_string.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_file.h"
#include "zce_trace_log_basic.h"

const char ZCE_LogTrace_Basic::STR_LOG_POSTFIX[LEN_LOG_POSTFIX + 1] = ".log";

//���캯��
ZCE_LogTrace_Basic::ZCE_LogTrace_Basic():
    div_log_file_(LOGDEVIDE_NONE),
    output_way_(LOG_OUTPUT_FILE | LOG_OUTPUT_ERROUT),
    if_thread_synchro_(false),
    auto_new_line_(true),
    max_size_log_file_(DEFAULT_LOG_SIZE),
    reserve_file_num_(DEFAULT_RESERVE_FILENUM),
    record_info_(LOG_HEAD_RECORD_CURRENTTIME | LOG_HEAD_RECORD_LOGLEVEL),
    current_click_(1),
    out_level_mask_(RS_TRACE),
    size_log_file_(0),
    if_output_log_(true)
{
    //Ԥ�ȷ���ռ�
    log_file_name_.reserve(PATH_MAX + 32);
    log_file_dir_.reserve(PATH_MAX + 32);
    log_file_prefix_.reserve(PATH_MAX + 32);
}


//
ZCE_LogTrace_Basic::~ZCE_LogTrace_Basic()
{
    //ע��
    finalize();
}


//��ʼ������,����ʱ��ָ���־�Ĺ���
int ZCE_LogTrace_Basic::init_time_log(ZCE_LOGFILE_DEVIDE div_log_file,
                                      const char *log_file_prefix,
                                      bool if_thread_synchro,
                                      bool auto_new_line,
                                      size_t reserve_file_num,
                                      unsigned int output_way,
                                      unsigned int head_record)
{
    assert (LOGDEVIDE_NONE <= div_log_file  && LOGDEVIDE_BY_YEAR >= div_log_file  );
    return initialize( div_log_file,
                       log_file_prefix,
                       if_thread_synchro,
                       auto_new_line,
                       0,
                       reserve_file_num,
                       output_way,
                       head_record);
}

//��ʼ������,���ڳߴ�ָ���־�Ĺ��� ZCE_LOGFILE_DEVIDE = LOGDEVIDE_BY_SIZE
int ZCE_LogTrace_Basic::init_size_log(
    const char *log_file_prefix,
    bool if_thread_synchro,
    bool auto_new_line,
    size_t max_size_log_file,
    unsigned int reserve_file_num,
    unsigned int output_way,
    unsigned int head_record)
{

    ZCE_LOGFILE_DEVIDE div_log_file = LOGDEVIDE_BY_SIZE;

    //�������ʶ�ļ��ָ��С
    if ( 0 == max_size_log_file )
    {
        div_log_file = LOGDEVIDE_NONE;
    }

    return initialize( div_log_file,
                       log_file_prefix,
                       if_thread_synchro,
                       auto_new_line,
                       max_size_log_file,
                       reserve_file_num,
                       output_way,
                       head_record);
}

//��ʼ�����������ڱ�׼���
int ZCE_LogTrace_Basic::init_stdout(bool if_thread_synchro ,
                                    bool use_err_out,
                                    bool auto_new_line,
                                    unsigned int head_record)
{
    unsigned int output_way = 0;

    if (use_err_out)
    {
        output_way |= LOG_OUTPUT_ERROUT;
    }
    else
    {
        output_way |= LOG_OUTPUT_STDOUT;
    }

    return initialize( LOGDEVIDE_NONE,
                       "",
                       if_thread_synchro,
                       auto_new_line,
                       0,
                       0,
                       output_way,
                       head_record);
}

//��ʼ������,��������ȫ��һ��
int ZCE_LogTrace_Basic::initialize(ZCE_LOGFILE_DEVIDE div_log_file,
                                   const char *log_file_prefix,
                                   bool if_thread_synchro,
                                   bool auto_new_line,
                                   size_t max_size_log_file,
                                   size_t reserve_file_num,
                                   unsigned int output_way,
                                   unsigned int head_record)
{
    div_log_file_ = div_log_file;
    output_way_ = output_way;

    if_thread_synchro_  = if_thread_synchro;
    auto_new_line_ = auto_new_line;

    max_size_log_file_ = max_size_log_file;

    reserve_file_num_ = reserve_file_num;
    record_info_ = head_record;
    current_click_ = 1;
    out_level_mask_ = RS_TRACE;
    size_log_file_ = 0;

    if_output_log_ = true;

    //���Լ���������
    if ( log_file_prefix != NULL)
    {
        log_file_prefix_ = log_file_prefix;
    }
    //����ļ���������ȫ
    else
    {
        assert(0 == ( output_way_ & (LOG_OUTPUT_FILE)) );
    }

    //
    make_configure();

    set_reserve_file_num((unsigned int)reserve_file_num);
    //�����Ҫ��־�ļ���������һ���ļ�
    if (output_way_ & LOG_OUTPUT_FILE )
    {
        make_new_logfile(time(NULL), true);
    }

    return 0;
}

//�ر���־��ע��رպ󣬱������³�ʼ��
void ZCE_LogTrace_Basic::finalize()
{
    if (log_file_handle_.is_open())
    {
        log_file_handle_.close();
    }

    current_click_ = 1;
    out_level_mask_ = RS_TRACE;

    div_log_file_ = LOGDEVIDE_NONE;
    output_way_ = LOG_OUTPUT_FILE | LOG_OUTPUT_ERROUT;

    size_log_file_ = 0;
    if_output_log_ = true;
}

//������־�ļ�
void ZCE_LogTrace_Basic::make_configure(void)
{
    int ret = 0;

    //���max_size_log_file_�Ĵ�С��Χ
    if (max_size_log_file_ < MIN_LOG_SIZE)
    {
        max_size_log_file_ = MIN_LOG_SIZE;
    }

    if (max_size_log_file_ > MAX_LOG_SIZE)
    {
        max_size_log_file_ = MAX_LOG_SIZE;
    }

    //�õ�Ŀ¼������
    char dir_name[PATH_MAX + 16];
    dir_name[PATH_MAX] = '\0';

    ZCE_OS::dirname(log_file_prefix_.c_str(), dir_name, PATH_MAX + 1);
    log_file_dir_ = dir_name;

    time_t cur_time = 0;

    // ���Ŀ¼�����ڣ��򴴽�
    if (ZCE_OS::mkdir_recurse(log_file_dir_.c_str()) != 0)
    {
        // ����ʧ�ܣ�
        printf("mkdir %s fail. err=%s\n", log_file_dir_.c_str(), strerror(errno));
    }

    //���ݷָ��ļ��ķ�������Ԥ����
    switch (div_log_file_)
    {
            //
        case LOGDEVIDE_NONE:
            log_file_name_ = log_file_prefix_ + STR_LOG_POSTFIX;
            break;

            //�õ����ں��·ݷָ���ļ�����
        case LOGDEVIDE_BY_FIVEMINUTE:
        case LOGDEVIDE_BY_QUARTER:
        case LOGDEVIDE_BY_HOUR:
        case LOGDEVIDE_BY_SIXHOUR:
        case LOGDEVIDE_BY_DATE:
        case LOGDEVIDE_BY_MONTH:
        case LOGDEVIDE_BY_YEAR:
            cur_time = time(NULL);
            current_click_ = cur_time / ONE_HOUR_SECONDS;
            create_time_logname(cur_time, log_file_name_);
            break;

            //ʹ��ID���ַָ���־��һ������.����ÿ�γ�ʼ��ʱҪ�õ�ID,����Ҫ����ԭ��
            //��־�ļ�,����Ҫ��ѯԭ�����һ����־�ļ�,��Ҫ�õ���ļ�¼�����Լ��ļ��ߴ�
            //ȷ����־�ļ�������Ѿ����˼�
        case LOGDEVIDE_BY_SIZE:

            //��¼��־�ļ����ļ�����
            create_id_logname(0, log_file_name_);

            //���Ҫ����SIZE������־,�õ���¼��־�ļ��ĳߴ�
            if (LOGDEVIDE_BY_SIZE == div_log_file_)
            {
                ret = ZCE_OS::filesize(log_file_name_.c_str(), &size_log_file_);

                if ( 0 != ret  )
                {
                    size_log_file_ = 0;
                }
            }

            break;

        default:
            break;
    }
}

//����־�������
void ZCE_LogTrace_Basic::enable_output_logmsg(void)
{
    if_output_log_ = true;
}
//�ر���־�������
void ZCE_LogTrace_Basic::disable_output_logmsg(void)
{
    if_output_log_ = false;
}

//������־���Mask
ZCE_LOG_PRIORITY ZCE_LogTrace_Basic::set_log_priority( ZCE_LOG_PRIORITY outlevel )
{
    ZCE_LOG_PRIORITY oldlevel = out_level_mask_;
    out_level_mask_ = outlevel;
    return oldlevel;
}
//ȡ�����Mask
ZCE_LOG_PRIORITY ZCE_LogTrace_Basic::get_log_priority(void )
{
    return out_level_mask_;
}

//����Ĭ���������Ϣ����
unsigned int ZCE_LogTrace_Basic::set_log_head(unsigned int recdinfo)
{
    unsigned int tmprecdinfo = recdinfo;
    record_info_ = recdinfo;
    return tmprecdinfo;
}
//ȡ��Ĭ���������Ϣ����
unsigned int ZCE_LogTrace_Basic::get_log_head(void)
{
    return record_info_;
}

//����ͬ������ı�ʾ
//�����ʼû�������ļ�ͬ�����,���治����.
unsigned int ZCE_LogTrace_Basic::set_output_way(unsigned int output_way)
{
    //
    unsigned int tmpsynchr = output_way_;
    output_way_ = output_way;

    return tmpsynchr;
}

//ȡ��ͬ������ı�ʾ
unsigned int ZCE_LogTrace_Basic::get_output_way(void)
{
    return output_way_;
}

//�����Ƿ��߳�ͬ��
bool ZCE_LogTrace_Basic::set_thread_synchro(bool if_thread_synchro)
{
    bool old_synchro = if_thread_synchro_;
    if_thread_synchro_ = if_thread_synchro;
    return old_synchro;

}
//ȡ���Ƿ�����߳�ͬ��
bool ZCE_LogTrace_Basic::get_thread_synchro(void)
{
    return if_thread_synchro_;
}

//�õ��µ���־�ļ��ļ�����
void ZCE_LogTrace_Basic::make_new_logfile(time_t cur_time, bool init)
{
    //�Ƿ�Ҫ�����µ��ļ�����
    bool to_new_file = false;

    time_t cur_click = 0;

    switch (div_log_file_)
    {
        case LOGDEVIDE_NONE:
            log_file_name_ = log_file_prefix_ + STR_LOG_POSTFIX;
            break;

        case LOGDEVIDE_BY_FIVEMINUTE:
            cur_click = cur_time / FIVE_MINUTE_SECONDS;
            break;

        case LOGDEVIDE_BY_QUARTER:
            cur_click = cur_time / ONE_QUARTER_SECONDS;
            break;

        case LOGDEVIDE_BY_HOUR:
        case LOGDEVIDE_BY_SIXHOUR:
        case LOGDEVIDE_BY_DATE:
        case LOGDEVIDE_BY_MONTH:
        case LOGDEVIDE_BY_YEAR:
            //�õ�Сʱ���ȵ�ʱ���¼,
            cur_click = cur_time / ONE_HOUR_SECONDS;

            //ÿСʱ���һ��,�԰������շָ���񲻹�ƽ,���������Խ�ʡ��������,����Ҳû��ʲôЧ������
            //�����������Ա���ʱ�����ж�
            if (current_click_ !=  cur_click)
            {
                current_click_ = cur_click;
                std::string new_file_name;
                new_file_name.reserve(512);
                create_time_logname(cur_time, new_file_name);

                //�����־�ļ������Ѿ�����,��ʾҪ����һ�����ļ�,
                //String�ıȽ��ǱȽϺ�ʱ��,��ǰ����޶���֤1�����Ƚ�24��,Сcase
                if (log_file_name_ != new_file_name)
                {
                    to_new_file = true;
                    log_file_name_ = new_file_name;
                }
            }

            break;

        case LOGDEVIDE_BY_SIZE:

            //�����־�ļ��ĳߴ��Ѿ�����
            if (size_log_file_ > max_size_log_file_)
            {
                to_new_file = true;
            }

            break;

        default:
            break;
    }

    //����ļ��Ǵ�״̬,���ļ���������������´�һ��
    if (log_file_handle_.is_open())
    {
        if (log_file_handle_.good() == false)
        {
            to_new_file = true;
        }
    }
    //����ļ�û�д�
    else
    {
        //�����´��ļ����򿪾��е��ļ������ֳߴ�
        if (to_new_file == false)
        {
            log_file_handle_.clear();
            log_file_handle_.open(log_file_name_.c_str(), std::ios::out | std::ios::app);
        }
    }

    //���������һ���µ��ļ�����
    if (to_new_file == true )
    {
        //�ر�ԭ�е��ļ�.
        if (log_file_handle_.is_open())
        {
            log_file_handle_.close();
        }

        //�������������־�����߷ָ���־��ʱ��Ϊ �� ���� ��
        del_old_logfile(cur_time, init);

        log_file_handle_.clear();
        //��������ļ�,��֮,������
        log_file_handle_.open(log_file_name_.c_str(), std::ios::out);

        size_log_file_ = 0;
    }
}

//�����ڵ�ʱ��ָ���־�ļ�,���ܻ�ɾ��,
//��ʼ����ʱ��������־Ŀ¼��ɾ�������ļ�
void ZCE_LogTrace_Basic::del_old_time_logfile(time_t cur_time, bool init)
{
    //���ȷ����ֻ����һ����������־�ļ�,�����ļ�����,ɾ����������ļ�

    //
    std::string del_logfile_name;
    del_logfile_name.reserve(PATH_MAX + 16);

    time_t del_file_time = 0;

    switch (div_log_file_)
    {
        case LOGDEVIDE_NONE:
            log_file_name_ = log_file_prefix_ + STR_LOG_POSTFIX;
            break;

        case LOGDEVIDE_BY_FIVEMINUTE:
            del_file_time = cur_time - reserve_file_num_ * FIVE_MINUTE_SECONDS;
            break;

        case LOGDEVIDE_BY_QUARTER:
            del_file_time = cur_time - reserve_file_num_ * ONE_QUARTER_SECONDS;
            break;

        case LOGDEVIDE_BY_HOUR:
            del_file_time = cur_time - reserve_file_num_ * ONE_HOUR_SECONDS;
            break;

        case LOGDEVIDE_BY_SIXHOUR:
            del_file_time = cur_time - reserve_file_num_ * 6 * ONE_HOUR_SECONDS;
            break;

        case LOGDEVIDE_BY_DATE:
            del_file_time = cur_time - reserve_file_num_ * 24 * ONE_HOUR_SECONDS;
            break;

            //ʹ�����һ���µ�ʱ������жϣ�
        case LOGDEVIDE_BY_MONTH:
            del_file_time = cur_time - reserve_file_num_ * 31 * ONE_DAY_SECONDS;
            break;

            //ʹ�����һ�������ʱ������ж�
        case LOGDEVIDE_BY_YEAR:
            del_file_time = cur_time - reserve_file_num_ * 366 * ONE_DAY_SECONDS;
            break;

        default:
            assert(false);
            break;
    }

    create_time_logname(del_file_time, del_logfile_name);

    //������ǳ�ʼ����ֻ����ɾ��һ�����ڵ��ļ�
    if (!init)
    {
        //����ɾ������ļ���
        ::remove(del_logfile_name.c_str());
    }
    //����ǳ�ʼ����ɨ����־Ŀ¼����������ļ�
    else
    {
        //����Ŀ¼
        DIR *dir_handle = ZCE_OS::opendir(log_file_dir_.c_str());

        if (NULL == dir_handle)
        {
            return ;
        }

        dirent *find_file = NULL;

        for ( find_file = ZCE_OS::readdir(dir_handle);
              find_file != NULL;
              find_file = ZCE_OS::readdir(dir_handle))
        {
            int ret_cmp = 0;
            std::string process_file_name(log_file_dir_);
            process_file_name += find_file->d_name;

            //���ǰ׺
            ret_cmp = strncmp(process_file_name.c_str(),
                              log_file_prefix_.c_str(),
                              log_file_prefix_.length());

            if (ret_cmp != 0)
            {
                continue;
            }

            size_t filename_len = strlen(find_file->d_name);

            if ( filename_len <= LEN_LOG_POSTFIX)
            {
                continue;
            }

            //ǰ����жϱ�֤������������
            //�����־��׺��β
            if (0 != strcmp(find_file->d_name + filename_len - LEN_LOG_POSTFIX,
                            STR_LOG_POSTFIX))
            {
                continue;
            }

            //����ж��е�򵥴ֱ���������Ϊ�Ѿ�����˺�׺��ǰ׺�����ⲻ��
            if (strcmp(process_file_name.c_str(), del_logfile_name.c_str()) <= 0)
            {
                ::remove(process_file_name.c_str());
            }
        }

        ZCE_OS::closedir(dir_handle);

    }
}



//�����ڵ�ID��־�ļ�
void ZCE_LogTrace_Basic::del_old_id_logfile()
{

    std::string dellogfname;
    dellogfname.reserve(PATH_MAX + 16);
    create_id_logname(reserve_file_num_ - 1, dellogfname);
    ::remove(dellogfname.c_str());

    std::string oldlogfilename , renamefilename;
    oldlogfilename.reserve(512);
    renamefilename.reserve(512);

    // ��������Ҫ�Ӻ��濪ʼ��ǰ������
    for (size_t i = reserve_file_num_ - 1; i > 0; -- i)
    {
        create_id_logname(i - 1, oldlogfilename);
        create_id_logname(i, renamefilename);
        ::rename(oldlogfilename.c_str(), renamefilename.c_str());
    }
}




//�������ڵõ��ļ�����
void ZCE_LogTrace_Basic::create_time_logname(time_t tmt, std::string &logfilename)
{
    tm curtm = *localtime(&tmt);
    char tmpbuf[65] = {0};
    size_t buflen = sizeof(tmpbuf) - 1;

    switch (div_log_file_)
    {
            //�Է���Ϊ��λ�õ��ļ�����
        case LOGDEVIDE_BY_FIVEMINUTE:
        case LOGDEVIDE_BY_QUARTER:
        {
            strftime( tmpbuf, buflen, "_%Y%m%d_%H%M", &curtm);
            ::strcat(tmpbuf, STR_LOG_POSTFIX);
            break;
        }

        //��СʱΪ��λ�õ��ļ�����
        case LOGDEVIDE_BY_HOUR:
        case LOGDEVIDE_BY_SIXHOUR:
        {
            strftime( tmpbuf, buflen, "_%Y%m%d_%H", &curtm);
            ::strcat(tmpbuf, STR_LOG_POSTFIX);
            break;
        }

        //����Ϊ��λ
        case LOGDEVIDE_BY_DATE:
            strftime( tmpbuf, buflen, "_%Y%m%d", &curtm);
            ::strcat(tmpbuf, STR_LOG_POSTFIX);
            break;

            //
        case LOGDEVIDE_BY_MONTH:
            strftime( tmpbuf, buflen, "_%Y%m", &curtm);
            ::strcat(tmpbuf, STR_LOG_POSTFIX);
            break;

        case LOGDEVIDE_BY_YEAR:
            strftime( tmpbuf, buflen, "_%Y", &curtm);
            ::strcat(tmpbuf, STR_LOG_POSTFIX);
            break;

            //Never goto here.
        default:
            ::strcat(tmpbuf, STR_LOG_POSTFIX);
            break;
    }

    logfilename = log_file_prefix_;
    logfilename += tmpbuf;
}

//����ID�õ��ļ�����f
void ZCE_LogTrace_Basic::create_id_logname(size_t logfileid, std::string &idlogfilename)
{
    char tmpbuf[32];

    if (logfileid == 0)
    {
        snprintf(tmpbuf, sizeof(tmpbuf) - 1, "%s",
                 STR_LOG_POSTFIX);
    }
    else
    {
        snprintf(tmpbuf, sizeof(tmpbuf) - 1, "%s.%u",
                 STR_LOG_POSTFIX,
                 (unsigned int)logfileid);
    }

    idlogfilename = log_file_prefix_;
    idlogfilename += tmpbuf;
}

//����־��ͷ����Ϣ�����һ��Stringbuf��
void ZCE_LogTrace_Basic::stringbuf_loghead(ZCE_LOG_PRIORITY outlevel,
                                           const timeval &now_time,
                                           char *log_tmp_buffer,
                                           size_t sz_buf_len,
                                           size_t &sz_use_len)
{

    sz_use_len = 0;

    //�����¼ʱ��
    if (record_info_ & LOG_HEAD_RECORD_CURRENTTIME)
    {
        //ת��Ϊ���
        ZCE_OS::timestamp(&now_time, log_tmp_buffer + sz_use_len, sz_buf_len);

        //������ˣ����
        sz_use_len = ZCE_OS::LEN_OF_TIME_STAMP;

        sz_buf_len -= sz_use_len;
    }

    //�����¼��־����
    if (record_info_ & LOG_HEAD_RECORD_LOGLEVEL)
    {
        switch (outlevel)
        {
            case RS_TRACE:
                sz_use_len += snprintf(log_tmp_buffer + sz_use_len, sz_buf_len, "%s", "[TRACE]");
                sz_buf_len -= sz_use_len;
                break;

            case RS_DEBUG:
                sz_use_len += snprintf(log_tmp_buffer + sz_use_len, sz_buf_len, "%s", "[DEBUG]");
                sz_buf_len -= sz_use_len;
                break;

            case RS_INFO:
                sz_use_len += snprintf(log_tmp_buffer + sz_use_len, sz_buf_len, "%s", "[INFO]");
                sz_buf_len -= sz_use_len;
                break;

            case RS_ERROR:
                sz_use_len += snprintf(log_tmp_buffer + sz_use_len, sz_buf_len, "%s", "[ERROR]");
                sz_buf_len -= sz_use_len;
                break;

            case RS_FATAL:
                sz_use_len += snprintf(log_tmp_buffer + sz_use_len, sz_buf_len, "%s", "[FATAL]");
                sz_buf_len -= sz_use_len;
                break;

            default:
                break;
        }
    }

    //�����¼��ǰ��PID
    if (record_info_ & LOG_HEAD_RECORD_PROCESSID)
    {
        sz_use_len += snprintf(log_tmp_buffer + sz_use_len, sz_buf_len, "[PID:%u]", static_cast<unsigned int>(ZCE_OS::getpid()));
        sz_buf_len -= sz_use_len;
    }

    if (record_info_ & LOG_HEAD_RECORD_THREADID)
    {
        sz_use_len += snprintf(log_tmp_buffer + sz_use_len, sz_buf_len, "[TID:%u]", static_cast<unsigned int>(ZCE_OS::pthread_self()));
        sz_buf_len -= sz_use_len;
    }
}

void ZCE_LogTrace_Basic::output_log_info(const timeval &now_time,
                                         char *log_tmp_buffer,
                                         size_t sz_use_len)
{
    //���Ҫ�߳�ͬ����������ط�����������ʹ���������ж��Ƿ������������ģ�棬��������ط�û����GRUAD��
    if (if_thread_synchro_)
    {
        protect_lock_.lock();
    }

    //��¼���ļ���
    if ((output_way_ & LOG_OUTPUT_FILE) )
    {
        //�õ��µ��ļ�����
        make_new_logfile(now_time.tv_sec);

        //����ļ�״̬OK
        if (log_file_handle_)
        {
            log_file_handle_.write(log_tmp_buffer, static_cast<std::streamsize>(sz_use_len));

            //�������flush�������,��Ϊ����л�����Ͳ�������������־����ˣ�
            //����������ף���ʵ�û��������־���ٶ��½��ܶ�ܶ�,�ܶ�ܶ�,
            //�Ƿ�����Ż��أ�����һ����������
            log_file_handle_.flush();

            //size_log_file_ = static_cast<unsigned long>( log_file_handle_.tellp());
            size_log_file_ += sz_use_len;
        }
    }

    //�����ͬ��Ҫ������ĵط�
    if (output_way_ & LOG_OUTPUT_STDOUT)
    {
        //cout���л���
        std::cout.write(log_tmp_buffer, static_cast<std::streamsize>(sz_use_len));
    }

    if (output_way_ & LOG_OUTPUT_ERROUT)
    {
        //cerrû�л��壬�Ʒ�˵��
        std::cerr.write(log_tmp_buffer, static_cast<std::streamsize>( sz_use_len));
    }

    //WIN32 �µĵ������,����Դ������
#ifdef ZCE_OS_WINDOWS

    if (output_way_ & LOG_OUTPUT_WINDBG)
    {
        ::OutputDebugStringA(log_tmp_buffer);
    }

#endif

    //������߳�ͬ����������ط�����
    if (if_thread_synchro_)
    {
        protect_lock_.unlock();
    }
}

//ͨ���ַ����õ���Ӧ����־����,
ZCE_LOG_PRIORITY ZCE_LogTrace_Basic::log_priorities(const char *str_priority)
{
    if (strcasecmp (str_priority,  ("TRACE")) == 0)
    {
        return RS_TRACE;
    }
    else if (strcasecmp (str_priority,  ("DEBUG")) == 0)
    {
        return RS_DEBUG;
    }
    else if (strcasecmp (str_priority, ("INFO")) == 0)
    {
        return RS_INFO;
    }
    else if (strcasecmp (str_priority, ("ERROR")) == 0)
    {
        return RS_ERROR;
    }
    else if (strcasecmp (str_priority,  ("ALERT")) == 0)
    {
        return RS_ALERT;
    }
    else if (strcasecmp (str_priority, ("FATAL")) == 0)
    {
        return RS_FATAL;
    }
    else
    {
        return RS_DEBUG;
    }

    //Never Goto Here.
    //return RS_DEBUG;
}

size_t  ZCE_LogTrace_Basic::set_reserve_file_num(size_t file_num)
{
    size_t old_reserve = reserve_file_num_;
    reserve_file_num_ = file_num;
    // ���¼�����Ҫ����ľ��ļ�
    del_old_logfile(time(NULL), true);
    return old_reserve;
}

unsigned int ZCE_LogTrace_Basic::get_reserve_file_num() const
{
    return (unsigned int)reserve_file_num_;
}

void ZCE_LogTrace_Basic::del_old_logfile(time_t cur_time, bool init)
{
    //�������������־�����߷ָ���־��ʱ��Ϊ �� ���� ��
    if ( reserve_file_num_ > 0 )
    {
        //����ǰ���ʱ����зָ��ļ���
        if ( div_log_file_ > LOGDEVIDE_BY_TIMEBEGIN && div_log_file_ < LOGDEVIDE_BY_TIMEEND)
        {
            del_old_time_logfile(cur_time, init);
        }

        //����ǰ�װ�ߴ����ļ���
        if (LOGDEVIDE_BY_SIZE == div_log_file_ )
        {
            del_old_id_logfile();
        }

        ZCE_OS::clear_last_error();
    }
}
