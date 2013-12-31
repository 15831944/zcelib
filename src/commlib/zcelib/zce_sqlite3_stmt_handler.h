#ifndef ZCE_LIB_SQLITE3_STMT_HANDLER_H_
#define ZCE_LIB_SQLITE3_STMT_HANDLER_H_

//Ŀǰ�汾����ֻ����һ��
#if SQLITE_VERSION_NUMBER >= 3003000

/*!
@brief      SQlite STMT�ľ��
            ����SQL�Ĵ���ȣ�STMT�Ǹ��ö���������������鷳һ�㡣
*/
class ZCELIB_EXPORT SQLite3_STMT_Handler
{
public:

    ///������������ݽṹ������BLOB���͵�����
    struct BINARY
    {
        ///2�������ݵ�ָ��
        void         *binary_data_;
        ///���������ݵĳ���
        int           binary_len_;
        //
        BINARY(void *binary_data, int binary_len)
            : binary_data_(binary_data)
            , binary_len_(binary_len)
        {
        }
        ~BINARY()
        {
        }
    };

public:
    /*!
    * @brief      ���캯��
    * @param      sqlite3_handler  SQlite3��DB��װ�����
    */
    SQLite3_STMT_Handler(SQLite3_DB_Handler *sqlite3_handler);
    /*!
    * @brief      ��������
    */
    ~SQLite3_STMT_Handler();

public:

    /*!
    * @brief      Ԥ����SQL���
    * @return     int
    * @param      sql_string
    */
    int prepare_sql_string(const char *sql_string);

    /*!
    * @brief      ���³�ʼ��STMT��Handler
    * @return     int
    */
    int reset_stmt_handler();

    /*!
    * @brief      ����SQLITE3��STMT HANDLER,�ָ���ʼ��ֵ�ȡ�
    * @return     int
    */
    int finalize_stmt_handler();

    /*!
    * @brief      ִ��SQL
    * @return     int         0�ɹ�������ʧ��
    * @param[out] hash_reuslt ����ֵ,�Ƿ��н��
    */
    int execute_stmt_sql(bool &hash_reuslt);

    /*!
    * @brief      ȡ���е�����
    * @param      num_col
    */
    void get_column_count(int &num_col);

    ///��ʼһ������
    int begin_transaction();
    ///�ύһ������
    int commit_transction();
    ///��ͬ��ѡ��رգ����鲻Ҫʹ��
    int turn_off_synch();

    ///
    sqlite3_stmt *get_sqlite3_stmt_handler()
    {
        return sqlite3_stmt_handler_;
    }

    ///�������Str
    const char *get_dbret_errstr()
    {
        return sqlite3_db_handler_->get_dbret_errstr();
    }
    ///DB���صĴ���ID
    unsigned int get_dbret_errid()
    {
        return sqlite3_db_handler_->get_dbret_errid();
    }

    //�õ���ǰ�����еĳ���
    unsigned int get_cur_field_length();

    //SQLite STMT��MYSQL��API������һЩ�������𣬿������ĺ���,����û������,
    //SQLite��Bind�������õ�ʱ���ȡ����ֵ�����ٴӺ����Ĳ����Ͽ�����������

    SQLite3_STMT_Handler &operator << (char );
    SQLite3_STMT_Handler &operator << (short );
    SQLite3_STMT_Handler &operator << (int );
    SQLite3_STMT_Handler &operator << (long );
    SQLite3_STMT_Handler &operator << (long long );

    SQLite3_STMT_Handler &operator << (unsigned char );
    SQLite3_STMT_Handler &operator << (unsigned short );
    SQLite3_STMT_Handler &operator << (unsigned int );
    SQLite3_STMT_Handler &operator << (unsigned long );
    SQLite3_STMT_Handler &operator << (unsigned long long );

    SQLite3_STMT_Handler &operator << (float );
    SQLite3_STMT_Handler &operator << (double );

    SQLite3_STMT_Handler &operator << (const char *);
    SQLite3_STMT_Handler &operator << (const std::string &);

    //
    SQLite3_STMT_Handler &operator << (const BINARY &);

    SQLite3_STMT_Handler &operator >> (char &);
    SQLite3_STMT_Handler &operator >> (short &);
    SQLite3_STMT_Handler &operator >> (int &);
    SQLite3_STMT_Handler &operator >> (long &);
    SQLite3_STMT_Handler &operator >> (long long &);

    SQLite3_STMT_Handler &operator >> (unsigned char &);
    SQLite3_STMT_Handler &operator >> (unsigned short &);
    SQLite3_STMT_Handler &operator >> (unsigned int &);
    SQLite3_STMT_Handler &operator >> (unsigned long &);
    SQLite3_STMT_Handler &operator >> (unsigned long long &);

    SQLite3_STMT_Handler &operator >> (float &);
    SQLite3_STMT_Handler &operator >> (double &);

    SQLite3_STMT_Handler &operator >> (char *);
    SQLite3_STMT_Handler &operator >> (std::string &);

    ///�����Ƶ�����Ҫ�ر���һ��,�ַ������ر�+1��,�����������ݲ�Ҫ��������
    SQLite3_STMT_Handler &operator >> (BINARY &);

protected:

    ///SQLite��DB���
    SQLite3_DB_Handler    *sqlite3_db_handler_;

    ///SQLiteԭ����STMT�ľ��
    sqlite3_stmt          *sqlite3_stmt_handler_;

    ///��ǰ�󶨵�,����<<����,��1��ʼ
    int                    current_bind_;
    ///��ǰ����,����>>����,��0��ʼ
    int                    current_col_;
};

#endif //SQLITE_VERSION_NUMBER >= 3003000

#endif //ZCE_LIB_SQLITE3_STMT_HANDLER_H_

