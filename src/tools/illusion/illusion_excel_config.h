// illusion_doc.h :  CillusionDoc ��Ľӿ�
//


#pragma once


struct EXCEL_TABLE_CONFIG
{
    //�������
    CString table_name_;

    //������ݴӵڼ��ж�ȡ
    unsigned int read_data_start_ = 3;

    //����Ӧ��protobuf��message����
    CString  protobuf_message_;

    //���ĵڼ��������ֶζ�Ӧ��protobuf
    unsigned int protobuf_cfg_line_ = 2;

    //����ŵ����ݿ⣨SQLite���ļ�����
    CString sqlite3_db_name_;
    
    //����Ӧ��table id
    unsigned int table_id_ = 0;
    //����������ֶ�1���к�
    unsigned int index1_column_ = 0;
    
    //����������ֶ�2���к�
    unsigned int index2_column_ = 0;
    
    //���ֶε�Ĭ��ֵ

    
};

class Illusion_Excel_Config 
{
protected: 
    //ö��ֵ�Ķ�Ӧ��ϵ��
    typedef std::map <CString ,int >              MAP_STRING_TO_INT;

    ///
    


protected: // �������л�����
	Illusion_Excel_Config();
protected:
    virtual ~Illusion_Excel_Config();

    
protected:


    //ö��ֵ��MAP�������ҵĲ�ѯ
    MAP_STRING_TO_INT             map_enum_conf_;

    //Excel�Ĵ������,EXCEL�Ĵ�����
    Illusion_ExcelFile             illusion_excel_file_;

    //SQL���Ļ�����
    char                         *sql_string_;

protected:
    //����ʵ��
    static Illusion_Excel_Config  *instance_;
    
public:

    //����EXCEL�ļ�
    virtual int ProcessExcelFile(const CString &open_file) = 0;

    //�������еĶ�ȡ����
    virtual void ClearAllReadConf();

protected:

    //��ö��ֵ
    int ReadEnumTable();

};


