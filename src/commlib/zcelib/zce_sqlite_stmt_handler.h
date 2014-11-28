#ifndef ZCE_LIB_SQLITE_STMT_HANDLER_H_
#define ZCE_LIB_SQLITE_STMT_HANDLER_H_

//Ŀǰ�汾����ֻ����һ��
#if SQLITE_VERSION_NUMBER >= 3005000

/*!
@brief      SQlite STMT�ľ��
            ����SQL�Ĵ���ȣ�STMT�Ǹ��ö���������������鷳һ�㡣
*/
class ZCELIB_EXPORT SQLite_STMT_Handler
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
    SQLite_STMT_Handler(SQLite_DB_Handler *sqlite3_handler);
    /*!
    * @brief      ��������
    */
    ~SQLite_STMT_Handler();

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
    * @brief      ִ��SQL����һ����ִ��SQL�������ƶ��α�
    * @return     int         0�ɹ�������ʧ��
    * @param[out] hash_reuslt ����ֵ,�Ƿ��н��
    * note        Ҫִ�ж�Σ���һ�εõ�������ϣ������ƶ��αꡣ
    */
    int execute_stmt_sql(bool &hash_reuslt);



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
    inline const char *error_message()
    {
        return sqlite_handler_->error_message();
    }
    ///DB���صĴ���ID
    inline  unsigned int error_code()
    {
        return sqlite_handler_->error_code();
    }

    //
    /*!
    * @brief      �õ���ǰ�����еĳ���
    * @return     unsigned int
    * @note
    */
    inline int column_bytes()
    {
        return ::sqlite3_column_bytes(sqlite3_stmt_handler_, current_col_);
    }

    /*!
    * @brief      ȡ���е�����
    * @return     int
    * @param      num_col
    */
    int column_count()
    {
        return ::sqlite3_column_count(sqlite3_stmt_handler_);
    }

    //SQLite STMT��MYSQL��API������һЩ�������𣬿������ĺ���,����û������,
    //SQLite��Bind�������õ�ʱ���ȡ����ֵ�����ٴӺ����Ĳ����Ͽ�����������
    //����Ҫbind blob���ݣ�ʹ��BINARY
    template <class bind_type>
    int bind(bind_type val);

    ///�����Ƶ�����Ҫ�ر���һ��,�ַ������ر�+1��,�����������ݲ�Ҫ��������
    template <class value_type>
    void column(value_type val);


    ///�������
    template <class value_type>
    SQLite_STMT_Handler &operator >> (value_type val)
    {
        column<value_type>(val);
        return *this;
    }


protected:

    ///SQLite��DB���
    SQLite_DB_Handler    *sqlite_handler_;

    ///SQLiteԭ����STMT�ľ��
    sqlite3_stmt          *sqlite3_stmt_handler_;

    ///��ǰ�󶨵�,����<<����,��1��ʼ
    int                    current_bind_;
    ///��ǰ����,����>>����,��0��ʼ
    int                    current_col_;
};

#endif //SQLITE_VERSION_NUMBER >= 3005000

#endif //ZCE_LIB_SQLITE3_STMT_HANDLER_H_

