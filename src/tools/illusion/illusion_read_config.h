


#pragma once




class Illusion_Read_Config
{
public:

    struct TABLE_CONFIG
    {
        ///�������
        CString excel_table_name_;

        ///������ݴӵڼ��ж�ȡ
        long read_data_start_ = 3;

        ///����Ӧ��protobuf��message����
        std::string   protobuf_message_;

        ///���ĵڼ��������ֶζ�Ӧ��protobuf
        long protobuf_item_line_ = 2;

        ///����ŵ����ݿ⣨SQLite���ļ�����
        std::string sqlite3_db_name_;

        ///����Ӧ��table id
        unsigned int table_id_ = 0;
        ///����������ֶ�1���к�
        unsigned int index1_column_ = 0;
        ///����������ֶ�2���к�
        unsigned int index2_column_ = 0;


        ///Protobuf item���������
        std::vector<std::string>  proto_item_ary_;

    };

    ///ö��ֵ�Ķ�Ӧ��ϵ��
    typedef std::map <CString, CString >  MAP_CSTRING_TO_CSTRING;

    ///
    typedef std::map <CString, TABLE_CONFIG> MAP_TABLE_TO_CONFIG;

    ///
    struct EXCEL_FILE_DATA
    {
        MAP_CSTRING_TO_CSTRING  xls_enum_;

        MAP_TABLE_TO_CONFIG  xls_table_cfg_;
    };

    typedef std::map <CString, EXCEL_FILE_DATA> MAP_FNAME_TO_CFGDATA;


    enum CVT_CODING
    {
        CVT_UTF8,
        CVT_UTF16,
        CVT_MBCS,
    };


protected: // �������л�����
    Illusion_Read_Config();
protected:
    virtual ~Illusion_Read_Config();


public:

    ///
    static Illusion_Read_Config *instance();

    ///
    static void clean_instance();

public:
    //
    BOOL initialize();
    //
    void finalize();


    ///
    int read_excel_byucname(const CString &open_file);
    ///
    int read_excel(const std::string &proto_fname);


    ///
    void map_proto_path(const std::string &path_name);
    ///
    int read_proto(const std::string &mbcs_name);


    //�������еĶ�ȡ����
    void clear();

protected:

    //��ö��ֵ
    int read_table_enum(EXCEL_FILE_DATA &file_cfg_data);

    //��ȡ�������
    int read_table_config(EXCEL_FILE_DATA &file_cfg_data);

    //��ȡ�������
    int read_table_cfgdata(TABLE_CONFIG &table_cfg);


    //
    int convert_to_utf8(CString &src, std::string &dst);
    //
    int convert_to_utf16(CString &src, std::string &dst);
    //
    int convert_to_mbcs(CString &src, std::string &dst);

protected:
    //
    static const size_t CONVERT_BUFFER_LEN = 64 * 1024;

protected:

    //����ʵ��
    static Illusion_Read_Config  *instance_;

protected:

    ///Excel�Ĵ������,EXCEL�Ĵ�����
    Illusion_ExcelFile ils_excel_file_;

    ///
    ZCE_Protobuf_Reflect ils_proto_reflect_;

    ///�ļ���Ӧ���������ݣ������ҵĲ�ѯ
    MAP_FNAME_TO_CFGDATA   file_cfg_map_;

    ///
    CVT_CODING cur_cvt_coding_ = CVT_UTF8;

    ///
    wchar_t *cvt_utf16_buf_ = NULL;

    ///
    char *cvt_utf8_buf_ = NULL;

    ///
    char *cvt_mbcs_buf_ = NULL;

};


