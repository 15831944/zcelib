


#pragma once




class Illusion_Read_Config
{
public:

    enum CVT_CODING
    {
        CVT_UTF8,
        CVT_UTF16,
        CVT_MBCS,
    };


    struct TABLE_CONFIG
    {
        ///�������
        CString excel_table_name_;

        ///������ݴӵڼ��ж�ȡ
        long read_data_start_ = 3;

        ///����Ӧ��protobuf��message����
        std::string   pb_msg_name_;

        ///���ĵڼ��������ֶζ�Ӧ��protobuf
        long pb_fieldname_line_ = 2;

        ///����ŵ����ݿ⣨SQLite���ļ�����
        std::string sqlite3_db_name_;

        ///����Ӧ��table id
        unsigned int table_id_ = 0;
        ///����������ֶ�1���к�
        unsigned int index1_column_ = 0;
        ///����������ֶ�2���к�
        unsigned int index2_column_ = 0;


        ///Protobuf item���������
        std::vector<std::string>  proto_field_ary_;

        ///����ṹ���£�record��һ��repeated ��message��
        ///phonebook.master
        ///phonebook.record.name
        ///phonebook.record.tele_number
        ///phonebook.record.email
        ///phonebook.record.name
        ///phonebook.record.tele_number
        ///phonebook.record.email
        ///��ôphonebook.record.name���ֵ�λ�ûᱻ��ʶΪitem_msg_firstshow_ Ϊ1
        std::vector<int> item_msg_firstshow_;

        ///�����������  �ᱻ��¼Ϊphonebook.record.name
        std::string firstshow_field_;
        ///����������� �ᱻ��¼Ϊphonebook.record
        std::string firstshow_msg_;

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

    ///��ʼ��
    bool initialize(bool need_open_excel,
                    const std::string &config_path);
    //
    void finalize();

    /*!
    * @brief
    * @return     int
    * @param      open_file �򿪵�EXCEL�ļ����ƣ�����MFC
    */
    int read_excel_byucname(const CString &open_file);

    /*!
    * @brief
    * @return     int
    * @param      proto_fname EXCEL�ļ����ơ�
    */
    int read_excel(const std::string &excel_fname);

    /*!
    * @brief
    * @param      path_name
    */
    void map_proto_path(const std::string &path_name);

    /*!
    * @brief
    * @return     int
    * @param      mbcs_name
    * @note
    */
    int read_proto(const std::string &mbcs_name);


    //�������еĶ�ȡ����
    void clear();

    ///
    void set_string_coding(CVT_CODING cvt_coding);

    ///��DB3�ļ������ȡĳ�����ñ������
    int read_db3_conftable(const std::string db3_fname, 
        unsigned int table_id,
        const std::string &conf_message_name);

protected:

    //��ö��ֵ
    int read_table_enum(EXCEL_FILE_DATA &file_cfg_data);

    ///��ȡ�������
    int read_table_config(EXCEL_FILE_DATA &file_cfg_data);

    ///��ȡ�������
    int read_table_cfgdata(TABLE_CONFIG &table_cfg,
                           ARRARY_OF_AI_IIJIMA_BINARY *aiiijma_ary);


    int save_to_sqlitedb(const TABLE_CONFIG &table_cfg,
                         const ARRARY_OF_AI_IIJIMA_BINARY *aiiijma_ary);

    /*!
    * @brief      ���ݵ�ǰĬ�ϵ��ַ����뷽ʽ��ת��ΪUTF8
    * @return     int == 0��ʾת���ɹ�
    * @param      src Դ�ַ�����CString�ṹ������MFC���ַ������뼯����
    * @param      dst ת��ĵ��ַ������������Ĭ��ת��ΪUTF8���ַ�����
    * @note
    */
    int convert_to_utf8(CString &src, std::string &dst);

    //
    int convert_to_utf16(CString &src, std::string &dst);
    //
    int convert_to_mbcs(CString &src, std::string &dst);

protected:
    //
    static const size_t CONVERT_BUFFER_LEN = 64 * 1024 - 1;

protected:

    //����ʵ��
    static Illusion_Read_Config  *instance_;

protected:

    ///����·��
    std::string config_path_;

    ///��־�����Ŀ¼
    std::string outlog_dir_path_;

    ///DB3�ļ������Ŀ¼
    std::string sqlitedb_pah_;

    bool need_open_excel_ = false;

    ///Excel�Ĵ������,EXCEL�Ĵ�����
    Illusion_ExcelFile ils_excel_file_;

    ///
    ZCE_Protobuf_Reflect ils_proto_reflect_;

    ///�ļ���Ӧ���������ݣ������ҵĲ�ѯ
    MAP_FNAME_TO_CFGDATA   file_cfg_map_;

    ///
    ZCE_General_Config_Table sqlite_config_;

    ///
    CVT_CODING cur_cvt_coding_ = CVT_MBCS;

    ///
    wchar_t *cvt_utf16_buf_ = NULL;
    ///
    char *cvt_utf8_buf_ = NULL;
    ///
    char *cvt_mbcs_buf_ = NULL;



};


