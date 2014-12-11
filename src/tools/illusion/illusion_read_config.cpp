#include "stdafx.h"
#include "illusion_excel_file.h"
#include "illusion_read_config.h"


//����ĵ���ʵ��
Illusion_Read_Config *Illusion_Read_Config::instance_ = NULL;

//
Illusion_Read_Config::Illusion_Read_Config()
{
    cvt_utf16_buf_ = new wchar_t[64 * 1024];
    cvt_utf8_buf_ = new char [64 * 1024];
    cvt_mbcs_buf_ = new char[64 * 1024];
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

//
BOOL Illusion_Read_Config::initialize()
{
    return ils_excel_file_.init_excel();
}
//
void Illusion_Read_Config::finalize()
{
    return ils_excel_file_.release_excel();
}

//�������еĶ�ȡ����
void Illusion_Read_Config::clear()
{
    file_cfg_map_.clear();


}


int Illusion_Read_Config::read_excel_byucname(const CString &open_file)
{
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

    auto iter_tmp = xls_data.xls_table_cfg_.begin();
    for (; iter_tmp != xls_data.xls_table_cfg_.end(); ++iter_tmp)
    {
        read_table_cfgdata(iter_tmp->second);
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
            convert_to_utf8(temp_value, tc_data.protobuf_message_);
            ++row_no;
            if (row_no > row_count)
            {
                return -1;
            }
            tc_data.protobuf_item_line_ = ils_excel_file_.get_cell_int(row_no, COL_TC_VALUE);
            if (tc_data.protobuf_item_line_ <= 0)
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
int Illusion_Read_Config::read_table_cfgdata(TABLE_CONFIG &tc_data)
{
    int ret = 0;
    //���EXCEL�ļ����Ƿ���������
    if (ils_excel_file_.load_sheet(tc_data.excel_table_name_, TRUE) == FALSE)
    {
        return -3;
    }

    google::protobuf::Message *new_msg = NULL;
    ret = ils_proto_reflect_.new_mesage(tc_data.protobuf_message_, new_msg);
    if (ret != 0)
    {
        return ret;
    }

    long line_count = ils_excel_file_.row_count();
    long col_count = ils_excel_file_.column_count();
    TRACE("%s table have col_count = %u row_count =%u\n", tc_data.excel_table_name_, col_count, line_count);

    CString proto_item_name;
    std::string std_item_name;

    if (tc_data.protobuf_item_line_ > line_count || tc_data.read_data_start_ > line_count )
    {
        return -4;
    }

    for (long col_no = 1; col_no <= col_count; ++col_no)
    {
        proto_item_name = ils_excel_file_.get_cell_cstring(tc_data.protobuf_item_line_, col_no);


#if defined UNICODE || defined _UNICODE
        convert_to_utf8(proto_item_name, std_item_name);
#else
        std_item_name = (LPCTSTR)proto_item_name;
#endif
        tc_data.proto_item_ary_.push_back(std_item_name);

    }

    CString read_data;
    //��ȡÿһ�е�����
    for (long line_no = tc_data.read_data_start_; line_no <= line_count; ++line_no)
    {
        new_msg->Clear();
        for (long col_no = 1; col_no <= col_count; ++col_no)
        {
            read_data = ils_excel_file_.get_cell_cstring(line_no, col_no);
        }
    }

    return 0;
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

    // �ڶ����ٰ�UTF-16����ת��ΪUTF-8����
    ret = ::WideCharToMultiByte(CP_UTF8,
                                NULL,
                                cvt_utf16_buf_,
                                -1,
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
    dst.assign(((const char *)((LPCTSTR)src)), (src.GetLength() * ( sizeof(wchar_t))) );
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
    dst  = ((LPCTSTR)src);
    return 0;
#endif
}

