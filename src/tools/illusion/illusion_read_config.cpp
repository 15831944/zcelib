#include "stdafx.h"
#include "illusion_excel_file.h"
#include "illusion_read_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//处理的单子实例
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


//根据当前默认的字符编码方式，转换为UTF8
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

    // 第一次先把MBCS码转换成UTF-16
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
    // 第二次再把UTF-16编码转换为UTF-8编码
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

//根据当前默认的字符编码方式，转换为UTF16(UNICODE)
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

//根据当前默认的字符编码方式，转换为MBCS
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


void Illusion_Read_Config::protobuf_output(const google::protobuf::Message *msg,
    std::ostream *out)
{
    const google::protobuf::Descriptor *msg_desc = msg->GetDescriptor();
    const google::protobuf::Reflection *reflection = msg->GetReflection();
    for (int i = 0; i < msg_desc->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor *field_desc =
            msg_desc->field(i);
        

        if (field_desc->label() == google::protobuf::FieldDescriptor::Label::LABEL_REQUIRED ||
            field_desc->label() == google::protobuf::FieldDescriptor::Label::LABEL_OPTIONAL)
        {
            *out <<"\t"<<field_desc->full_name() << ":";
            if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE)
            {
                *out << reflection->GetDouble(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_FLOAT)
            {
                *out << reflection->GetFloat(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_INT64)
            {
                *out << reflection->GetInt64(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_UINT64)
            {
                *out << reflection->GetUInt64(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_INT32)
            {
                *out << reflection->GetInt32(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_FIXED64)
            {
                *out << reflection->GetUInt64(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_FIXED32)
            {
                *out << reflection->GetUInt32(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_BOOL)
            {
                *out << reflection->GetBool(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_STRING)
            {
                *out << reflection->GetString(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE)
            {
                const google::protobuf::Message &sub_msg = reflection->GetMessage(*msg, field_desc);
                protobuf_output(&sub_msg, out);
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_BYTES)
            {
                *out << reflection->GetString(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_UINT32)
            {
                *out << reflection->GetUInt32(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_ENUM)
            {
                const google::protobuf::EnumValueDescriptor *enum_vale =
                    reflection->GetEnum(*msg, field_desc);
                
                *out << enum_vale->full_name() << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_SFIXED32)
            {
                *out << reflection->GetInt32(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_SFIXED64)
            {
                *out << reflection->GetInt64(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_SINT32)
            {
                *out << reflection->GetInt32(*msg, field_desc) << std::endl;
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_SINT64)
            {
                *out << reflection->GetInt64(*msg, field_desc) << std::endl;
            }
            else
            {
                ZCE_LOG(RS_ERROR, "I don't field_desc [%s] support this type.%d %s",
                    field_desc->full_name().c_str(),
                    field_desc->type(),
                    field_desc->type_name());
                ZCE_ASSERT(false);
            }
        }
        else if (field_desc->label() == google::protobuf::FieldDescriptor::Label::LABEL_REPEATED)
        {
            int field_size = reflection->FieldSize(*msg, field_desc);
            if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedDouble(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_FLOAT)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedFloat(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_INT64)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedInt64(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_UINT64)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedUInt64(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_INT32)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedInt32(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_FIXED64)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedUInt64(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_FIXED32)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedUInt32(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_BOOL)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedBool(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_STRING)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedString(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    const google::protobuf::Message &sub_msg = reflection->GetRepeatedMessage(*msg, field_desc,i);
                    protobuf_output(&sub_msg, out);
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_BYTES)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedString(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_UINT32)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedUInt32(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_ENUM)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    const google::protobuf::EnumValueDescriptor *enum_vale =
                        reflection->GetRepeatedEnum(*msg, field_desc, j);
                    *out << "\t" << field_desc->full_name() << ":" << enum_vale->full_name();
                }

            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_SFIXED32)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedInt32(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_SFIXED64)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedInt64(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_SINT32)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedInt32(*msg, field_desc, j) << std::endl;
                }
            }
            else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_SINT64)
            {
                for (int j = 0; j < field_size; ++j)
                {
                    *out << "\t" << field_desc->full_name() << ":" <<
                        reflection->GetRepeatedInt64(*msg, field_desc, j) << std::endl;
                }
            }
            else
            {
                ZCE_LOG(RS_ERROR, "I don't field_desc [%s] support this type.%d %s",
                    field_desc->full_name().c_str(),
                    field_desc->type(),
                    field_desc->type_name());
                ZCE_ASSERT(false);
                return;
            }
        }
        else
        {
            ZCE_ASSERT(false);
        }
    }
    return;
}

//
bool Illusion_Read_Config::initialize(bool need_open_excel,
                                      const std::string &config_path)
{
    config_path_ = config_path;

    //db3的路径没有可以创建
    sqlitedb_pah_ = config_path_;
    ZCE_LIB::path_string_cat(sqlitedb_pah_, "db3");
    if (false == ZCE_LIB::is_directory(sqlitedb_pah_.c_str()))
    {
        ZCE_LIB::mkdir_recurse(sqlitedb_pah_.c_str());
    }

    //log的路径没有可以创建
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

//清理所有的读取数据
void Illusion_Read_Config::clear()
{
    file_cfg_map_.clear();
}


int Illusion_Read_Config::read_excel_byucname(const CString &open_file)
{
    clear();

    BOOL bret = ils_excel_file_.open_excelfile(open_file);
    //Excel文件打开失败
    if (bret != TRUE)
    {
        return -1;
    }
    //
    TRACE("Dream excecl file have sheet num[%d].\n", ils_excel_file_.sheets_count());

    //表格错误
    if (ils_excel_file_.load_sheet(_T("TABLE_CONFIG"), FALSE) == FALSE ||
        ils_excel_file_.load_sheet(_T("ENUM_CONFIG"), FALSE) == FALSE)
    {
        //
        ::AfxMessageBox(_T("你选择的配置EXCEL不是能读取的配置表，请重现检查后打开。!"));
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
        ::AfxMessageBox(_T("你选择的配置EXCEL文件中的ENUM_CONFIG表不正确，请重现检查后打开。!"));
        return ret;
    }

    //
    ret = read_table_config(xls_data);
    if (0 != ret)
    {
        ::AfxMessageBox(_T("你选择的配置EXCEL文件中的TABLE_CONFIG表不正确，请重现检查后打开。!"));
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


//读取所有的枚举值
int Illusion_Read_Config::read_table_enum(EXCEL_FILE_DATA &file_cfg_data)
{
    //前面检查过了
    BOOL bret =  ils_excel_file_.load_sheet(_T("ENUM_CONFIG"), TRUE);
    if (bret == FALSE)
    {
        return -1;
    }

    //答应行列
    long row_count = ils_excel_file_.row_count();
    long col_count = ils_excel_file_.column_count();
    TRACE("ENUM_CONFIG table have col_count = %u row_count =%u\n", col_count, row_count);

    //注意行列的下标都是从1开始。
    const long COL_ENUM_KEY = 1;
    const long COL_ENUM_VALUE = 2;
    const TCHAR ENUM_FIRST_CHAR = _T('[');

    size_t read_enum = 0;
    //读取所有的行
    for (long i = 1; i <= row_count; ++i)
    {

        long   row_no = i;
        //字符串
        CString enum_key = ils_excel_file_.get_cell_cstring(row_no, COL_ENUM_KEY);

        //如果第一个字符是[
        if (enum_key[0] == ENUM_FIRST_CHAR )
        {
            CString enum_vlaue = ils_excel_file_.get_cell_cstring(row_no, COL_ENUM_VALUE);
            file_cfg_data.xls_enum_[enum_key] = enum_vlaue;

            ++read_enum;
        }
    }

    return 0;
}


//读取表格配置
int Illusion_Read_Config::read_table_config(EXCEL_FILE_DATA &file_cfg_data)
{
    //前面检查过了
    BOOL bret = ils_excel_file_.load_sheet(_T("TABLE_CONFIG"), TRUE);
    if (bret == FALSE)
    {
        return -1;
    }

    long row_count = ils_excel_file_.row_count();
    long col_count = ils_excel_file_.column_count();
    TRACE("TABLE_CONFIG table have col_count = %u row_count =%u\n", col_count, row_count);

    //注意行列的下标都是从1开始。
    const long COL_TC_KEY = 1;
    const long COL_TC_VALUE = 2;

    for (long row_no = 1; row_no <= row_count; ++row_no)
    {

        CString tc_key = ils_excel_file_.get_cell_cstring(row_no, COL_TC_KEY);

        CString temp_value;
        TABLE_CONFIG tc_data;

        if (tc_key == _T("表格名称"))
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
            //INDEX 2可以为0
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


//读取表格数据read_table_data
int Illusion_Read_Config::read_table_cfgdata(TABLE_CONFIG &tc_data,
                                             ARRARY_OF_AI_IIJIMA_BINARY *aiiijma_ary)
{
    int ret = 0;
    //检查EXCEL文件中是否有这个表格
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
        //如果为空表示不需要关注这列
        if (tc_data.proto_field_ary_[col_no - 1].length() == 0)
        {
            field_msg_ary.push_back(NULL);
            field_desc_ary.push_back(NULL);
            continue;
        }

        //取得字段的描述
        ret = ZCE_Protobuf_Reflect::get_fielddesc(new_msg.get(),
                                                  tc_data.proto_field_ary_[col_no - 1],
                                                  tc_data.item_msg_firstshow_[col_no - 1] == 1 ? true : false,
                                                  field_msg,
                                                  field_desc);
        if (0 != ret)
        {
            ZCE_LOG(RS_ERROR, "Message [%s] don't find field_desc [%s] field_desc name define in L/C[%d/%d]",
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




    //吧啦吧啦吧啦吧啦吧啦吧啦吧啦，这段啰嗦的代码只是为了搞个日志的名字,EXCEFILENAE_TABLENAME.log
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
        ZCE_LOG(RS_ERROR, "Read excel file data log file [%s] open fail.", out_log_file.c_str());
        return -1;
    }

    //什么？为啥不用google pb 的debugstring直接输出？为啥，自己考虑
    read_table_log << "Read excel file:" << xlsfile_stdstring.c_str() << " line count" << line_count
                   << "column count " << col_count << std::endl;
    read_table_log << "Read table:" << tablename_stdstring.c_str() << std::endl;

    ZCE_LOG(RS_INFO, "Read excel file:%s table :%s start. line count %u column %u.",
            xlsfile_stdstring.c_str(),
            tablename_stdstring.c_str(),
            line_count,
            col_count);

    int index_1 = 0, index_2 = 0;
    CString read_data;
    std::string set_data, show_data;

    //读取每一行的数据 ,+1是因为read_data_start_也要读取
    aiiijma_ary->resize(line_count - tc_data.read_data_start_ + 1);
    for (long line_no = tc_data.read_data_start_; line_no <= line_count; ++line_no)
    {
        new_msg->Clear();

        read_table_log << "Read line:" << line_no << std::endl << "{" << std::endl;
        ZCE_LOG(RS_INFO, "Read line：%u", line_no);

        for (long col_no = 1; col_no <= col_count; ++col_no)
        {
            //如果为空表示不需要关注这列
            if (tc_data.proto_field_ary_[col_no - 1].length() ==  0)
            {
                continue;
            }

            //读出EXCEL数据，注意这个地方是根据MFC的编码决定CString数据的编码
            read_data = ils_excel_file_.get_cell_cstring(line_no, col_no);

            //取得字段的描述
            field_msg = field_msg_ary[ col_no - 1 ];
            field_desc = field_desc_ary[col_no - 1];

            //如果是string 类型，Google PB之支持UTF8
            if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_STRING )
            {
                ret = convert_to_utf8(read_data, set_data);
                //
                ret = convert_to_mbcs(read_data, show_data);
            }
            //对于BYTES，
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
            //其他字段类型统一转换为UTF8的编码
            else
            {
                ret = convert_to_utf8(read_data, set_data);
                show_data = set_data;
            }
            //根据描述，设置字段的数据
            ret = ZCE_Protobuf_Reflect::set_fielddata(field_msg, field_desc, set_data);
            if (0 != ret)
            {
                ZCE_LOG(RS_ERROR, "Message [%s] field_desc [%s] type [%d][%s] set_fielddata fail. Line,Colmn[%d|%d]",
                        tc_data.pb_msg_name_.c_str(),
                        field_desc->full_name(),
                        field_desc->type(),
                        field_desc->type_name(),
                        line_no,
                        col_no
                       );
                return ret;
            }

            //读取索引
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

        //如果没有初始化
        if (!new_msg->IsInitialized())
        {
            ZCE_LOG(RS_ERROR, "Message [%s] is not IsInitialized, please check your excel or proto file.",
                    tc_data.pb_msg_name_.c_str());
            return -1;
        }

        read_table_log << "} index_1 :" << index_1 << "index_2" << index_2 << std::endl;

        if (!new_msg->IsInitialized())
        {
            ZCE_LOG(RS_ERROR, "Message [%s] protobuf_encode fail, please check your excel or proto file.",
                    tc_data.pb_msg_name_.c_str());
            return -1;
        }
        ret = (*aiiijma_ary)[line_no - tc_data.read_data_start_].protobuf_encode(index_1, index_2, new_msg.get());
    }

    ZCE_LOG(RS_INFO, "Read excel file:%s table :%s end.", xlsfile_stdstring.c_str(),
            tablename_stdstring.c_str());

    return 0;
}


int Illusion_Read_Config::save_to_sqlitedb(const TABLE_CONFIG &table_cfg,
                                           const ARRARY_OF_AI_IIJIMA_BINARY *aiiijma_ary)
{
    int ret = 0;
    std::string db3_file = sqlitedb_pah_;
    ZCE_LIB::path_string_cat(db3_file, table_cfg.sqlite3_db_name_);

    ret = sqlite_config_.open_dbfile(db3_file.c_str(), false, true);
    if (ret != 0)
    {
        return ret;
    }

    ret = sqlite_config_.create_table(table_cfg.table_id_);
    if (ret != 0)
    {
        return ret;
    }

    //更新数据库
    ret = sqlite_config_.replace_array(table_cfg.table_id_, aiiijma_ary);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}




///从DB3文件里面读取某个配置表的配置
int Illusion_Read_Config::read_db3_conftable(const std::string db3_fname,
                                             const std::string &conf_message_name,
                                             unsigned int table_id,
                                             unsigned int index_1,
                                             unsigned int index_2)
{
    int ret = 0;
    std::string db3_file = sqlitedb_pah_;
    ZCE_LIB::path_string_cat(db3_file, db3_fname);

    ret = sqlite_config_.open_dbfile(db3_file.c_str(), true, false);
    if (ret != 0)
    {
        return ret;
    }

    std::string log_file_name = db3_fname;
    log_file_name += "_";

    char table_id_buf[32];
    snprintf(table_id_buf,31 ,"%u", table_id);
    log_file_name += table_id_buf;
    log_file_name += ".log";
    std::string out_log_file = outlog_dir_path_;
    ZCE_LIB::path_string_cat(out_log_file, log_file_name.c_str());

    std::ofstream read_db3_log;
    read_db3_log.open(out_log_file.c_str());
    if (!read_db3_log.good())
    {
        ZCE_LOG(RS_ERROR, "Read excel file data log file [%s] open fail.", out_log_file.c_str());
        return -1;
    }
    google::protobuf::Message *temp_msg = NULL;
    ret = ils_proto_reflect_.new_mesage(conf_message_name, temp_msg);
    std::shared_ptr<google::protobuf::Message> new_msg(temp_msg);
    if (ret != 0)
    {
        return ret;
    }

    if (index_1 == 0 && index_2 == 0)
    {
        ARRARY_OF_AI_IIJIMA_BINARY aiiijma_ary;
        //更新数据库
        ret = sqlite_config_.select_array(table_id, 0, 0, &aiiijma_ary);
        if (ret != 0)
        {
            return ret;
        }

        //
        for (size_t i = 0; i < aiiijma_ary.size(); ++i)
        {
            new_msg->Clear();
            ret = aiiijma_ary[i].protobuf_decode(&index_1, &index_2, new_msg.get());
            if (ret != 0)
            {

            }

            protobuf_output(new_msg.get(),&read_db3_log);
        }
    }
    else
    {

    }


    return 0;
}
