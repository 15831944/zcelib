
#ifndef _ZCE_LIB_PROTOBUF_REFLECT_H_
#define _ZCE_LIB_PROTOBUF_REFLECT_H_


#if defined ZCE_USE_PROTOBUF && ZCE_USE_PROTOBUF == 1

//
struct ZCE_PROTO_ERROR
{
    std::string file_name_;
    int line_;
    int column_;
    std::string message_;
};

typedef std::vector<ZCE_PROTO_ERROR> PROTO_ERROR_ARRAY;


//�����ռ�
class ZCE_Error_Collector :public google::protobuf::compiler::MultiFileErrorCollector
{
public:
    ZCE_Error_Collector();
    virtual ~ZCE_Error_Collector();

public:

    virtual void AddError(const std::string & filename, 
        int line, 
        int column, 
        const std::string & message);

public:
    //
    PROTO_ERROR_ARRAY error_array_;
};




/*!
* @brief      
*             
* @note       
*/
class ZCE_Protobuf_Reflect
{
public:
    //
    ZCE_Protobuf_Reflect();
    //
    ~ZCE_Protobuf_Reflect();

public:

    ///ӳ��һ��·��
    void map_path(const std::string &path);

    ///����һ��proto �ļ�
    int import_file(const std::string &file_name);

    //
    void error_info(PROTO_ERROR_ARRAY &error_ary);

    //�������ƴ���Message,new ��message �ᱣ��������ǰ�����message����
    int new_proc_mesage(const std::string &type_name);

    //
    void del_proc_message();

    //
    int set_proc_msg_field(const std::string &field_name,
        const std::string &set_data);

    //
public:

    //����һ��Message��field
    static int set_msg_field(google::protobuf::Message *msg,
        const google::protobuf::Descriptor *msg_desc,
        const std::string &field_name,
        const std::string &set_data);


    //��λһ���ӽṹ
    static int locate_sub_msg(google::protobuf::Message *msg,
        const google::protobuf::Descriptor *msg_desc,
        const std::string &submsg_field_name,
        google::protobuf::Message *&sub_msg,
        const google::protobuf::Descriptor *&submsg_desc);

protected:

    google::protobuf::compiler::Importer *protobuf_importer_;

    //
    google::protobuf::compiler::DiskSourceTree source_tree_;

    //
    google::protobuf::DynamicMessageFactory msg_factory_;

    //
    ZCE_Error_Collector error_collector_;


    ///
    google::protobuf::Message *proc_message_;
    ///
    const google::protobuf::Descriptor *proc_msg_desc_;


};

#endif 

#endif //# _ZCE_LIB_PROTOBUF_REFLECT_H_


