#include "stdafx.h"
#include "illusion_excel_file.h"
#include "illusion_read_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//����ĵ���ʵ��
Illusion_Read_Config *Illusion_Read_Config::instance_ = NULL;

//
Illusion_Read_Config::Illusion_Read_Config()
{
    cvt_utf16_buf_ = new wchar_t[CONVERT_BUFFER_LEN + 1];
    cvt_utf8_buf_ = new char[CONVERT_BUFFER_LEN + 1];
    cvt_mbcs_buf_ = new char[CONVERT_BUFFER_LEN + 1];
}


Illusion_Read_Config::~Illusion_Read_Config()
{
    if (cvt_utf16_buf_)
    {
        delete cvt_utf16_buf_;
        cvt_utf16_buf_ = NULL;
    }
    if (cvt_utf8_buf_)
    {
        delete cvt_utf8_buf_;
        cvt_utf8_buf_ = NULL;
    }
    if (cvt_mbcs_buf_)
    {
        delete cvt_mbcs_buf_;
        cvt_mbcs_buf_ = NULL;
    }
}


Illusion_Read_Config *Illusion_Read_Config::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new Illusion_Read_Config();
    }
    return instance_;
}

void Illusion_Read_Config::clean_instance()
{
    if (instance_)
    {
        delete instance_;
        instance_ = NULL;
    }
}


//���ݵ�ǰĬ�ϵ��ַ����뷽ʽ��ת��ΪUTF8
int Illusion_Read_Config::convert_to_utf8(CString &src, std::string &dst)
{
#if defined UNICODE || defined _UNICODE

    DWORD ret = 0;
    ret = ::WideCharToMultiByte(CP_UTF8,
        NULL,
        (LPCTSTR)src,
        src.GetLength() + 1,
        cvt_utf8_buf_,
        CONVERT_BUFFER_LEN,
        NULL,
        0);
    if (ret == 0)
    {
        return -1;
    }
    dst = cvt_utf8_buf_;
    return 0;

#else
    // MBCS ===> UTF16 ===> UTF8

    // ��һ���Ȱ�MBCS��ת����UTF-16
    DWORD ret = 0;
    ret = ::MultiByteToWideChar(CP_ACP,
        0,
        (LPCTSTR)src,
        src.GetLength() + 1,
        cvt_utf16_buf_,
        CONVERT_BUFFER_LEN);
    if (ret == 0)
    {
        return -1;
    }
    int u16_buf_len = ret;
    // �ڶ����ٰ�UTF-16����ת��ΪUTF-8����
    ret = ::WideCharToMultiByte(CP_UTF8,
        NULL,
        cvt_utf16_buf_,
        u16_buf_len,
        cvt_utf8_buf_,
        CONVERT_BUFFER_LEN,
        NULL,
        0);
    if (ret == 0)
    {
        return -1;
    }
    dst = cvt_utf8_buf_;

    return 0;
#endif
}

//���ݵ�ǰĬ�ϵ��ַ����뷽ʽ��ת��ΪUTF16(UNICODE)
int Illusion_Read_Config::convert_to_utf16(CString &src, std::string &dst)
{
#if defined UNICODE || defined _UNICODE
    // UTF16 == UTF16
    if (src.GetLength() > CONVERT_BUFFER_LEN)
    {
        return -1;
    }
    dst.assign(((const char *)((LPCTSTR)src)), ((src.GetLength() + 1) * (sizeof(wchar_t))));
    return 0;
#else
    // MBCS ===> UTF16
    DWORD ret = 0;
    ret = ::MultiByteToWideChar(CP_ACP,
        0,
        (LPCTSTR)src,
        src.GetLength() + 1,
        cvt_utf16_buf_,
        CONVERT_BUFFER_LEN);
    if (ret == 0)
    {
        return -1;
    }
    dst.assign(((const char *)(cvt_utf16_buf_)), (ret * (sizeof(wchar_t))));
    return 0;
#endif
}

//���ݵ�ǰĬ�ϵ��ַ����뷽ʽ��ת��ΪMBCS
int Illusion_Read_Config::convert_to_mbcs(CString &src, std::string &dst)
{
#if defined UNICODE || defined _UNICODE
    //UTF16 == > MBCS
    DWORD ret = 0;
    ret = ::WideCharToMultiByte(CP_ACP,
        NULL,
        (LPCTSTR)src,
        src.GetLength() + 1,
        cvt_mbcs_buf_,
        CONVERT_BUFFER_LEN,
        NULL,
        0);
    if (ret == 0)
    {
        return -1;
    }
    dst = cvt_mbcs_buf_;
    return 0;
#else
    // MBCS ===> MBCS
    if (src.GetLength() > CONVERT_BUFFER_LEN)
    {
        return -1;
    }
    dst = ((LPCTSTR)src);
    return 0;
#endif
}

//
bool Illusion_Read_Config::initialize(bool need_open_excel,
                                      const std::string &config_path)
{
    config_path_ = config_path;

    //db3��·��û�п��Դ���
    sqlitedb_pah_ = config_path_;
    ZCE_LIB::path_string_cat(sqlitedb_pah_, "db3");
    if (false == ZCE_LIB::is_directory(sqlitedb_pah_.c_str()))
    {
        ZCE_LIB::mkdir_recurse(sqlitedb_pah_.c_str());
    }

    //log��·��û�п��Դ���
    outlog_dir_path_ = config_path_;
    ZCE_LIB::path_string_cat(outlog_dir_path_, "log");
    if (false == ZCE_LIB::is_directory(outlog_dir_path_.c_str()))
    {
        ZCE_LIB::mkdir_recurse(outlog_dir_path_.c_str());
    }

    need_open_excel_ = need_open_excel;
    if (need_open_excel_)
    {
        BOOL bret = ils_excel_file_.init_excel();
        if (FALSE == bret)
        {
            return false;
        }
    }
    return true;
}

//
void Illusion_Read_Config::finalize()
{
    clear();

    if (need_open_excel_)
    {
        ils_excel_file_.close_excelfile(FALSE);
        Illusion_ExcelFile::release_excel();
    }
    
    return;
}

//�������еĶ�ȡ����
void Illusion_Read_Config::clear()
{
    file_cfg_map_.clear();
}


int Illusion_Read_Config::read_excel_byucname(const CString &open_file)
{
    clear();

    BOOL bret = ils_excel_file_.open_excelfile(open_file);
    //Excel�ļ���ʧ��
    if (bret != TRUE)
    {
        return -1;
    }
    //
    TRACE("Dream excecl file have sheet num[%d].\n", ils_excel_file_.sheets_count());

    //������
    if (ils_excel_file_.load_sheet(_T("TABLE_CONFIG"), FALSE) == FALSE ||
        ils_excel_file_.load_sheet(_T("ENUM_CONFIG"), FALSE) == FALSE)
    {
        //
        ::AfxMessageBox(_T("��ѡ�������EXCEL�����ܶ�ȡ�����ñ������ּ���򿪡�!"));
        return -1;
    }

    //file_cfg_map_[open_file] = excel_data;
    EXCEL_FILE_DATA excel_data;
    auto result = file_cfg_map_.insert(make_pair(open_file, excel_data));
    if (!result.second)
    {
        return -1;
    }

    //
    EXCEL_FILE_DATA &xls_data = (*result.first).second;
    int ret = read_table_enum(xls_data);
    if (0 != ret)
    {
        ::AfxMessageBox(_T("��ѡ�������EXCEL�ļ��е�ENUM_CONFIG����ȷ�������ּ���򿪡�!"));
        return ret;
    }

    //
    ret = read_table_config(xls_data);
    if (0 != ret)
    {
        ::AfxMessageBox(_T("��ѡ�������EXCEL�ļ��е�TABLE_CONFIG����ȷ�������ּ���򿪡�!"));
        return ret;
    }

    ///
    ARRARY_OF_AI_IIJIMA_BINARY  fandaoai_ary;

    auto iter_tmp = xls_data.xls_table_cfg_.begin();
    for (; iter_tmp != xls_data.xls_table_cfg_.end(); ++iter_tmp)
    {

        ret = read_table_cfgdata(iter_tmp->second, &fandaoai_ary);
        if (0 != ret)
        {
            return ret;
        }

        ret = save_to_sqlitedb(iter_tmp->second, &fandaoai_ary);
        if (0 != ret)
        {
            return ret;
        }
    }

    return 0;
}

///
int Illusion_Read_Config::read_excel(const std::string &mbcs_name)
{
#if defined UNICODE || defined _UNICODE
    DWORD ret = 0;
    ret = ::MultiByteToWideChar(CP_ACP,
                                0,
                                mbcs_name.c_str(),
                                mbcs_name.length() + 1,
                                cvt_utf16_buf_,
                                CONVERT_BUFFER_LEN);
    if (ret == 0)
    {
        return -1;
    }

    return read_excel_byucname(cvt_utf16_buf_);
#else
    return read_excel_byucname(mbcs_name.c_str());
#endif
}


//
void Illusion_Read_Config::map_proto_path(const std::string &path_name)
{
    ils_proto_reflect_.map_path(path_name);
}

///
int Illusion_Read_Config::read_proto(const std::string &proto_fname)
{

    int ret = ils_proto_reflect_.import_file(proto_fname);
    if (ret == 0)
    {
        return -1;
    }
    return 0;
}


//��ȡ���е�ö��ֵ
int Illusion_Read_Config::read_table_enum(EXCEL_FILE_DATA &file_cfg_data)
{
    //ǰ�������
    BOOL bret =  ils_excel_file_.load_sheet(_T("ENUM_CONFIG"), TRUE);
    if (bret == FALSE)
    {
        return -1;
    }

    //��Ӧ����
    long row_count = ils_excel_file_.row_count();
    long col_count = ils_excel_file_.column_count();
    TRACE("ENUM_CONFIG table have col_count = %u row_count =%u\n", col_count, row_count);

    //ע�����е��±궼�Ǵ�1��ʼ��
    const long COL_ENUM_KEY = 1;
    const long COL_ENUM_VALUE = 2;
    const TCHAR ENUM_FIRST_CHAR = _T('[');

    size_t read_enum = 0;
    //��ȡ���е���
    for (long i = 1; i <= row_count; ++i)
    {

        long   row_no = i;
        //�ַ���
        CString enum_key = ils_excel_file_.get_cell_cstring(row_no, COL_ENUM_KEY);

        //�����һ���ַ���[
        if (enum_key[0] == ENUM_FIRST_CHAR )
        {
            CString enum_vlaue = ils_excel_file_.get_cell_cstring(row_no, COL_ENUM_VALUE);
            file_cfg_data.xls_enum_[enum_key] = enum_vlaue;

            ++read_enum;
        }
    }

    return 0;
}


//��ȡ�������
int Illusion_Read_Config::read_table_config(EXCEL_FILE_DATA &file_cfg_data)
{
    //ǰ�������
    BOOL bret = ils_excel_file_.load_sheet(_T("TABLE_CONFIG"), TRUE);
    if (bret == FALSE)
    {
        return -1;
    }

    long row_count = ils_excel_file_.row_count();
    long col_count = ils_excel_file_.column_count();
    TRACE("TABLE_CONFIG table have col_count = %u row_count =%u\n", col_count, row_count);

    //ע�����е��±궼�Ǵ�1��ʼ��
    const long COL_TC_KEY = 1;
    const long COL_TC_VALUE = 2;

    for (long row_no = 1; row_no <= row_count; ++row_no)
    {

        CString tc_key = ils_excel_file_.get_cell_cstring(row_no, COL_TC_KEY);

        CString temp_value;
        TABLE_CONFIG tc_data;

        if (tc_key == _T("�������"))
        {

            tc_data.excel_table_name_ = ils_excel_file_.get_cell_cstring(row_no, COL_TC_VALUE);
            if (tc_data.excel_table_name_.IsEmpty())
            {
                return -1;
            }

            ++row_no;
            if (row_no > row_count)
            {
                return -1;
            }
            tc_data.read_data_start_ = ils_excel_file_.get_cell_int(row_no, COL_TC_VALUE);
            if (tc_data.read_data_start_ <= 0)
            {
                return -1;
            }

            ++row_no;
            if (row_no > row_count)
            {
                return -1;
            }

            temp_value  = ils_excel_file_.get_cell_cstring(row_no, COL_TC_VALUE);
            if (temp_value.IsEmpty())
            {
                return -1;
            }
            convert_to_utf8(temp_value, tc_data.pb_msg_name_);
            ++row_no;
            if (row_no > row_count)
            {
                return -1;
            }
            tc_data.pb_fieldname_line_ = ils_excel_file_.get_cell_int(row_no, COL_TC_VALUE);
            if (tc_data.pb_fieldname_line_ <= 0)
            {
                return -1;
            }

            ++row_no;
            if (row_no > row_count)
            {
                return -1;
            }
            temp_value = ils_excel_file_.get_cell_cstring(row_no, COL_TC_VALUE);
            if (temp_value.IsEmpty())
            {
                return -1;
            }
            convert_to_utf8(temp_value, tc_data.sqlite3_db_name_);

            ++row_no;
            if (row_no > row_count)
            {
                return -1;
            }
            tc_data.table_id_ = ils_excel_file_.get_cell_int(row_no, COL_TC_VALUE);
            if (tc_data.table_id_ <= 0)
            {
                return -1;
            }

            ++row_no;
            if (row_no > row_count)
            {
                return -1;
            }
            tc_data.index1_column_ = ils_excel_file_.get_cell_int(row_no, COL_TC_VALUE);
            if (tc_data.index1_column_ <= 0)
            {
                return -1;
            }

            ++row_no;
            if (row_no > row_count)
            {
                return -1;
            }
            tc_data.index2_column_ = ils_excel_file_.get_cell_int(row_no, COL_TC_VALUE);
            //INDEX 2����Ϊ0
            //if (tc_data.index2_column_ <= 0)

            auto result = file_cfg_data.xls_table_cfg_.insert(std::make_pair(tc_data.excel_table_name_, tc_data));
            if (false == result.second)
            {
                return -2;
            }
        }
        else
        {
            continue;
        }
    }

    return 0;
}


//��ȡ�������read_table_data
int Illusion_Read_Config::read_table_cfgdata(TABLE_CONFIG &tc_data,
                                             ARRARY_OF_AI_IIJIMA_BINARY *aiiijma_ary)
{
    int ret = 0;
    //���EXCEL�ļ����Ƿ���������
    if (ils_excel_file_.load_sheet(tc_data.excel_table_name_, TRUE) == FALSE)
    {
        return -3;
    }

    google::protobuf::Message *temp_msg = NULL;
    ret = ils_proto_reflect_.new_mesage(tc_data.pb_msg_name_, temp_msg);
    std::shared_ptr<google::protobuf::Message> new_msg(temp_msg);
    if (ret != 0)
    {
        return ret;
    }
    //std::shared_ptr <google::protobuf::Message> new_msg(tmp_msg);

    long line_count = ils_excel_file_.row_count();
    long col_count = ils_excel_file_.column_count();
    TRACE("%s table have col_count = %u row_count =%u\n", tc_data.excel_table_name_, col_count, line_count);

    CString field_name_cstring;
    std::string field_name_stdstr;

    if (tc_data.pb_fieldname_line_ > line_count || tc_data.read_data_start_ > line_count )
    {
        return -4;
    }

    for (long col_no = 1; col_no <= col_count; ++col_no)
    {
        field_name_cstring = ils_excel_file_.get_cell_cstring(tc_data.pb_fieldname_line_, col_no);

        ret = convert_to_mbcs(field_name_cstring, field_name_stdstr);

        tc_data.proto_field_ary_.push_back(field_name_stdstr);


        size_t find_pos = tc_data.proto_field_ary_[col_no - 1].find_last_of('.');
        if (find_pos != std::string::npos)
        {
            if (0 == strcmp(tc_data.firstshow_field_.c_str(),
                            field_name_stdstr.c_str()))
            {
                tc_data.item_msg_firstshow_.push_back(true);
            }
            else
            {
                if (0 == strncmp(tc_data.firstshow_msg_.c_str(),
                                 field_name_stdstr.c_str(),
                                 tc_data.firstshow_msg_.length()) )
                {
                    tc_data.item_msg_firstshow_.push_back(false);
                }
                else
                {
                    tc_data.firstshow_field_ = field_name_stdstr;
                    tc_data.firstshow_msg_.assign(field_name_stdstr, 0, find_pos + 1);
                    tc_data.item_msg_firstshow_.push_back(true);
                }
            }
        }
        else
        {
            tc_data.item_msg_firstshow_.push_back(false);
        }

    }

    std::vector<google::protobuf::Message *> field_msg_ary;
    std::vector<const google::protobuf::FieldDescriptor *> field_desc_ary;

    google::protobuf::Message *field_msg = NULL;
    const google::protobuf::FieldDescriptor *field_desc = NULL;
    for (long col_no = 1; col_no <= col_count; ++col_no)
    {
        //���Ϊ�ձ�ʾ����Ҫ��ע����
        if (tc_data.proto_field_ary_[col_no - 1].length() == 0)
        {
            field_msg_ary.push_back(NULL);
            field_desc_ary.push_back(NULL);
            continue;
        }

        //ȡ���ֶε�����
        ret = ZCE_Protobuf_Reflect::get_fielddesc(new_msg.get(),
                                                  tc_data.proto_field_ary_[col_no - 1],
                                                  tc_data.item_msg_firstshow_[col_no - 1] == 1 ? true : false,
                                                  field_msg,
                                                  field_desc);
        if (0 != ret)
        {
            ZCE_LOGMSG(RS_ERROR, "Message [%s] don't find field [%s] field name define in L/C[%d/%d]",
                       tc_data.pb_msg_name_.c_str(),
                       tc_data.proto_field_ary_[col_no - 1].c_str(),
                       tc_data.pb_fieldname_line_,
                       col_no
                      );
            return ret;
        }
        field_msg_ary.push_back(field_msg);
        field_desc_ary.push_back(field_desc);
    }




    //��������������������������������Ά��µĴ���ֻ��Ϊ�˸����־������,EXCEFILENAE_TABLENAME.log
    std::string xlsfile_stdstring;
    convert_to_mbcs(ils_excel_file_.open_filename(), xlsfile_stdstring);
    ZCE_LIB::basename(xlsfile_stdstring.c_str(), cvt_utf8_buf_, CONVERT_BUFFER_LEN);
    std::string log_file_name = cvt_utf8_buf_;
    log_file_name += "_";

    std::string tablename_stdstring;
    convert_to_mbcs(tc_data.excel_table_name_, tablename_stdstring);
    log_file_name += tablename_stdstring;
    log_file_name += ".log";
    std::string out_log_file = outlog_dir_path_;
    ZCE_LIB::path_string_cat(out_log_file, log_file_name.c_str());

    std::ofstream read_table_log;
    read_table_log.open(out_log_file.c_str());

    if (!read_table_log.good())
    {
        return -1;
    }

    //ʲô��Ϊɶ����google pb ��debugstringֱ�������Ϊɶ���Լ�����
    read_table_log << "Read excel file:" << xlsfile_stdstring.c_str() << " line count" << line_count
                   << "column count " << col_count << std::endl;
    read_table_log << "Read table:" << tablename_stdstring.c_str() << std::endl;

    ZCE_LOGMSG(RS_INFO, "Read excel file:%s table :%s start. line count %u column %u.",
               xlsfile_stdstring.c_str(),
               tablename_stdstring.c_str(),
               line_count,
               col_count);

    int index_1 = 0, index_2 = 0;
    CString read_data;
    std::string set_data, show_data;

    //��ȡÿһ�е����� ,+1����Ϊread_data_start_ҲҪ��ȡ
    aiiijma_ary->resize(line_count - tc_data.read_data_start_+1);
    for (long line_no = tc_data.read_data_start_; line_no <= line_count; ++line_no)
    {
        new_msg->Clear();

        read_table_log << "Read line:" << line_no << std::endl << "{" << std::endl;
        ZCE_LOGMSG(RS_INFO, "Read line��%u", line_no);

        for (long col_no = 1; col_no <= col_count; ++col_no)
        {
            //���Ϊ�ձ�ʾ����Ҫ��ע����
            if (tc_data.proto_field_ary_[col_no - 1].length() ==  0)
            {
                continue;
            }

            //����EXCEL���ݣ�ע������ط��Ǹ���MFC�ı������CString���ݵı���
            read_data = ils_excel_file_.get_cell_cstring(line_no, col_no);

            //ȡ���ֶε�����
            field_msg = field_msg_ary[ col_no - 1 ];
            field_desc = field_desc_ary[col_no - 1];

            //�����string ���ͣ�Google PB֧֮��UTF8
            if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_STRING )
            {
                ret = convert_to_utf8(read_data, set_data);
                //
                ret = convert_to_mbcs(read_data, show_data);
            }
            //����BYTES��
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_BYTES)
            {
                if (cur_cvt_coding_ == CVT_UTF8)
                {
                    ret = convert_to_utf8(read_data, set_data);
                }
                else if (cur_cvt_coding_ == CVT_UTF16)
                {
                    ret = convert_to_utf16(read_data, set_data);
                }
                else if (cur_cvt_coding_ == CVT_MBCS)
                {
                    ret = convert_to_mbcs(read_data, set_data);
                }
                else
                {
                    ZCE_ASSERT(false);
                }
                show_data = set_data;
            }
            //�����ֶ�����ͳһת��ΪUTF8�ı���
            else
            {
                ret = convert_to_utf8(read_data, set_data);
                show_data = set_data;
            }
            //���������������ֶε�����
            ret = ZCE_Protobuf_Reflect::set_fielddata(field_msg, field_desc, set_data);
            if (0 != ret)
            {
                ZCE_LOGMSG(RS_ERROR, "Message [%s] field [%s] type [%d][%s] set_fielddata fail. Line,Colmn[%d|%d]",
                           tc_data.pb_msg_name_.c_str(),
                           field_desc->full_name(),
                           field_desc->type(),
                           field_desc->type_name(),
                           line_no,
                           col_no
                          );
                return ret;
            }

            //��ȡ����
            if (col_no == tc_data.index1_column_)
            {
                index_1 = std::stol(set_data, 0, 10 );
            }
            if (tc_data.index2_column_ != 0 && col_no == tc_data.index2_column_)
            {
                index_2 = std::stol(set_data, 0, 10);
            }

            read_table_log << "\t" << tc_data.proto_field_ary_[col_no - 1].c_str() << ":" << show_data.c_str()
                           << std::endl;
        }

        //���û�г�ʼ��
        if (!new_msg->IsInitialized())
        {
            ZCE_LOGMSG(RS_ERROR, "Message [%s] is not IsInitialized, please check your excel or proto file.",
                       tc_data.pb_msg_name_.c_str());
            return -1;
        }

        read_table_log << "} index_1 :" << index_1 << "index_2" << index_2 << std::endl;

        if (!new_msg->IsInitialized())
        {
            ZCE_LOGMSG(RS_ERROR, "Message [%s] protobuf_encode fail, please check your excel or proto file.",
                       tc_data.pb_msg_name_.c_str());
            return -1;
        }
        ret = (*aiiijma_ary)[line_no - tc_data.read_data_start_].protobuf_encode(index_1, index_2, new_msg.get());
    }

    ZCE_LOGMSG(RS_INFO, "Read excel file:%s table :%s end.", xlsfile_stdstring.c_str(),
               tablename_stdstring.c_str());

    return 0;
}


int Illusion_Read_Config::save_to_sqlitedb(const TABLE_CONFIG &table_cfg,
                                           const ARRARY_OF_AI_IIJIMA_BINARY *aiiijma_ary)
{
    int ret = 0;
    std::string db3_file = sqlitedb_pah_;
    ZCE_LIB::path_string_cat(db3_file, table_cfg.sqlite3_db_name_);

    ret = sqlite_config_.open_dbfile(db3_file.c_str(),false,true);
    if (ret != 0)
    {
        return ret;
    }

    ret = sqlite_config_.create_table(table_cfg.table_id_);
    if (ret != 0)
    {
        return ret;
    }

    //�������ݿ�
    ret = sqlite_config_.replace_array(table_cfg.table_id_, aiiijma_ary);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}




///��DB3�ļ������ȡĳ�����ñ������
int Illusion_Read_Config::read_db3_conftable(const std::string db3_fname,
    unsigned int table_id,
    const std::string &conf_message_name)
{
    int ret = 0;
    std::string db3_file = sqlitedb_pah_;
    ZCE_LIB::path_string_cat(db3_file, db3_fname);

    ret = sqlite_config_.open_dbfile(db3_file.c_str(), true, false);
    if (ret != 0)
    {
        return ret;
    }

    ARRARY_OF_AI_IIJIMA_BINARY aiiijma_ary;
    //�������ݿ�
    ret = sqlite_config_.select_array(table_id, 0, 0 ,&aiiijma_ary);
    if (ret != 0)
    {
        return ret;
    }

    google::protobuf::Message *temp_msg = NULL;
    ret = ils_proto_reflect_.new_mesage(conf_message_name, temp_msg);
    std::shared_ptr<google::protobuf::Message> new_msg(temp_msg);
    if (ret != 0)
    {
        return ret;
    }



    return 0;
}
