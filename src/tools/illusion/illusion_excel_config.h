// illusion_doc.h :  CillusionDoc ��Ľӿ�
//


#pragma once




class Illusion_Excel_Config 
{
public:

    struct TABLE_CONFIG
    {
        ///�������
        CString table_name_;

        ///������ݴӵڼ��ж�ȡ
        unsigned int read_data_start_ = 3;

        ///����Ӧ��protobuf��message����
        CString  protobuf_message_;

        ///���ĵڼ��������ֶζ�Ӧ��protobuf
        unsigned int protobuf_cfg_line_ = 2;

        ///����ŵ����ݿ⣨SQLite���ļ�����
        CString sqlite3_db_name_;

        ///����Ӧ��table id
        unsigned int table_id_ = 0;
        ///����������ֶ�1���к�
        unsigned int index1_column_ = 0;
        ///����������ֶ�2���к�
        unsigned int index2_column_ = 0;


        ///
        std::vector<CString>  proto_tag_;

    };

    ///ö��ֵ�Ķ�Ӧ��ϵ��
    typedef std::map <CString, CString >  MAP_CSTRING_TO_CSTRING;

    ///
    typedef std::map <CString, TABLE_CONFIG> MAP_TABLE_TO_CONFIG;

    ///
    struct EXCEL_FILE_DATA
    {
        MAP_CSTRING_TO_CSTRING  file_enum_;

        MAP_TABLE_TO_CONFIG  file_table_cfg_;
    };

    typedef std::map <CString, EXCEL_FILE_DATA> MAP_FNAME_TO_CFGDATA;



protected: // �������л�����
    Illusion_Excel_Config();
protected:
    virtual ~Illusion_Excel_Config();


public:

    ///
    static Illusion_Excel_Config *instance();
    
    ///
    static void clean_instance();

protected:

    ///Excel�Ĵ������,EXCEL�Ĵ�����
    Illusion_ExcelFile illusion_excel_file_;

    ///�ļ���Ӧ���������ݣ������ҵĲ�ѯ
    MAP_FNAME_TO_CFGDATA   file_cfg_map_;

protected:
    //����ʵ��
    static Illusion_Excel_Config  *instance_;
    
public:
    //
    BOOL initialize();
    //
    void finalize();


    //����EXCEL�ļ�
    int read_excelconfig(const CString &open_file);

    //�������еĶ�ȡ����
    void clear();

protected:

    //��ö��ֵ
    int read_enum_data(EXCEL_FILE_DATA & file_cfg_data);

    //��ȡ�������
    int read_table_config(EXCEL_FILE_DATA & file_cfg_data);

    //��ȡ�������
    int read_table_data(TABLE_CONFIG & table_cfg);
};


