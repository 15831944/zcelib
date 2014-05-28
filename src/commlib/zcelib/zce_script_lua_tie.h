//��������ǲο�Tinkerʵ�ֵģ���Ȼ��лԭ����

#ifndef ZCE_LIB_SCRIPT_LUA_H_
#define ZCE_LIB_SCRIPT_LUA_H_

#if defined  lua_h

#if defined (ZCE_OS_WINDOWS)
#pragma warning ( push )
#pragma warning ( disable : 4127)
#endif

//Ϊʲô����ZCE_LIB�����ֿռ䣬
namespace ZCE_LUA
{
    ///ֻ����table��newdindex
    int newindex_onlyread(lua_State *state);

    ///dump lua���еĵĶ�ջ�����ڼ��lua����ʱ�����⣬�������
    int dump_luacall_stack(lua_State *state);

    ///dump C����lua�Ķ�ջ��
    int dump_clua_stack(lua_State *state);
    
    ///
    int on_error(lua_State *state);

    
    ///PUSHһ�����ݵ���ջ��
    //���������long�ı�������Ϊlong�޷���ֲ��
    template<typename val_type> static void push_stack(lua_State * /*state*/, val_type /*val*/)
    {
    }
    template<> void push_stack(lua_State *state, char val);
    template<> void push_stack(lua_State *state, unsigned char val);
    template<> void push_stack(lua_State *state, short val);
    template<> void push_stack(lua_State *state, unsigned short val);
    template<> void push_stack(lua_State *state, int val);
    template<> void push_stack(lua_State *state, unsigned int val);
    template<> void push_stack(lua_State *state, float val);
    template<> void push_stack(lua_State *state, double val);
    template<> void push_stack(lua_State *state, char *val);
    template<> void push_stack(lua_State *state, const char *val);
    template<> void push_stack(lua_State *state, bool val);
    template<> void push_stack(lua_State *state, int64_t val);
    template<> void push_stack(lua_State *state, uint64_t val);
    template<> void push_stack(lua_State *state, const std::string &val);

    ///
    template<typename val_type> static  val_type read_stack(lua_State *L, int index)
    {
        return *(val_type*)(L, index);
    }
    template<> char               *read_stack(lua_State *state, int index);
    template<> const char         *read_stack(lua_State *state, int index);
    template<> char                read_stack(lua_State *state, int index);
    template<> unsigned char       read_stack(lua_State *state, int index);
    template<> short               read_stack(lua_State *state, int index);
    template<> unsigned short      read_stack(lua_State *state, int index);
    template<> int                 read_stack(lua_State *state, int index);
    template<> unsigned int        read_stack(lua_State *state, int index);
    template<> float               read_stack(lua_State *state, int index);
    template<> double              read_stack(lua_State *state, int index);
    template<> bool                read_stack(lua_State *state, int index);
    template<> int64_t             read_stack(lua_State *state, int index);
    template<> uint64_t            read_stack(lua_State *state, int index);
    template<> std::string         read_stack(lua_State *state, int index);

    ///
    template<typename val_type>
    val_type pop_stack(lua_State *state)
    {
        val_type t = read_stack<val_type>(state, -1);
        lua_pop(L, 1);
        return t;
    }


    ///��C11������Ч�����ʵ��
    template<typename ret_type, typename... args_type>
    struct g_functor
    {
        static int invoke(lua_State *state)
        {
            //push�ǽ���������ջ
            void *upvalue_1 = lua_touserdata(state, lua_upvalueindex(1));
            ret_type(*fun_ptr)(args_type...) = (ret_type(*)(args_type...)) (upvalue_1);

            if (std::is_void<ret_type>::value)
            {
                int para_idx = 0;
                fun_ptr(read_stack<args_type>(state, para_idx++)...);
                return 0;
            }
            else
            {
                int para_idx = 0;
                push_stack<ret_type>(state, fun_ptr(read_stack<args_type>(state, para_idx++)...));
                return 1;
            }
        }
    };
};

//=======================================================================================================
//LUA Ь�������ڰ�����C++��
class ZCE_Lua_Tie
{

public:
    ZCE_Lua_Tie();
    ~ZCE_Lua_Tie();


    /*!
    * @brief      ��lua state
    * @return     int  
    * @param      open_libs  �Ƿ�򿪳��õ�һЩLUA��
    * @param      reg_common �Ƿ�ע��һЩ����
    */
    int open(bool open_libs,
        bool reg_common);
    ///�ر�lua state
    void close();


    ///ִ��һ��lua�ļ�
    int do_file(const char *filename);
    ///ִ��һ��LUA��buffer
    int do_buffer(const char *buff, size_t sz);

    ///dump C����lua�Ķ�ջ��
    void dump_clua_stack();
    ///dump lua���еĵĶ�ջ�����ڼ��lua����ʱ�����⣬�������
    void dump_luacall_stack();


    ///��LUAע��int64_t�����ͣ���ΪLUA�ڲ���numberĬ����double��������ʵ�޷���ʾ������Ҫע�����
    void reg_int64();
    ///��LUAע��uint64_t������
    void reg_uint64();
    ///��LUAע��std::string������
    void reg_stdstring();

    ///��LUAע��ö��ֵ
    void reg_enum(const char *name, size_t item_num, ...);

    ///��LUA����һ����ȫ�֣����������ƺͱ�����Ӧֵ�ÿ�����
    template<typename val_type>
    void set_gval(const char *name, typename val_type val)
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        //ģ�庯��������val_type�������push
        push_stack(lua_state_, val);
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    ///�������ƣ���LUA��ȡһ������
    template<typename val_type>
    typename val_type get_val(const char *name)
    {
        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);
        return pop_stack<val_type>(lua_state_);
    }

    ///��LUA����һ��ȫ�ֵ�����
    template<typename val_type>
    void set_ary(const char *name, 
        size_t ary_num,
        typename const val_type ary_data[],
        bool read_only = false)
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        lua_createtable(lua_state_, ary_num, 0);

        char num_key[64];
        for (size_t i = 0; i<ary_num; ++i)
        {
            lua_pushstring(lua_state_, itoa(i, num_key,10));
            push_stack(lua_state_, ary_data[i]);
            lua_rawset(lua_state_, -3);
        }
        //���ϣ����ֻ��
        if (read_only)
        {
            //��������ֻ��
            lua_newtable(lua_state_);

            lua_pushstring(lua_state_, "__newindex");
            lua_pushcclosure(lua_state_, ZCE_LUA::newindex_onlyread, 0);
            lua_rawset(lua_state_, -3);

            lua_setmetatable(lua_state_, -2);
        }

        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    ///��LUA�л�ȡһ��ȫ�ֵ�����
    template<typename ary_type>
    int get_ary(const char *name,
        size_t ary_num,
        typename const ary_type ary_data[]  )
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        //�������һ��table������Ŷ
        if (!lua_istable(lua_state_, -1))
        {
            return -1;
        }

        char num_key[64];
        for (size_t i = 0; i < ary_num; ++i)
        {
            lua_getfield(lua_state_,-1�� itoa(i, num_key, 10));
            if (lua_isnil(lua_state_,-1))
            {
                return -1;
            }
            ary_data[i] = pop_stack<ary_type>(lua_state_ );
        }
    }



    /*!
    * @brief      ��LUAע��һ��ȫ�ֺ�����������ľ�̬������lua����
    * @tparam     ret_type  ���ز�������
    * @tparam     args_type �����Ĳ������ͣ����
    * @param      name      ��LUAע��ĺ�������
    * @param      func      ע���C����
    */
    template<typename ret_type, typename... args_type>
    void reg_gfun(const char *name, ret_type(*func)(args_type...))
    {
        //��������
        lua_pushstring(lua_state_, name);
        //������ָ��ת��Ϊvoid * ����Ϊlightuserdata �����ջ����Ϊclosure��upvalue����
        lua_pushlightuserdata(lua_state_, (void *)func);
        //functorģ�庯��������closure,
        lua_pushcclosure(lua_state_, ZCE_LUA::g_functor<ret_type, args_type...>::invoke, 1);
        //�������ȫ�ֻ�������
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    ///����LUA�ĺ�����ȡ��һ������ֵ
    template<typename ret_type, typename... args_type>
    int call_luafun(const char *name, ret_type ret, args_type... args)
    {
        //���������ĺ��������Ҽ�¼��ջ�ĵ�ַ
        lua_pushcclosure(lua_state_, ZCE_LUA::on_error, 0);
        int errfunc = lua_gettop(lua_state_);

        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);
        //������Ƿ��Ǻ���
        if (!lua_isfunction(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "call_luafun() attempt to call global `%s' (not a function)", name);
            return -1;
        }
        
        //�����ջ������args
        ZCE_LUA::push_stack(lua_state_, args...);

        //����lua�ĺ�����Ĭ��ֻ��һ������ֵ��
        lua_pcall(L, 2, 1, errfunc);

        //�ڶ�ջɾ����������ĺ���
        lua_remove(lua_state_, errfunc);

        //�ڶ�ջ��������ֵ
        ZCE_LUA::pop_stack<ret_type>(lua_state_);
        
        return 0;
    }

protected:

    //
    lua_State   *lua_state_;
};

#if defined (ZCE_OS_WINDOWS)
#pragma warning ( pop )
#endif

#endif

#endif // ZCE_LIB_SCRIPT_LUA_H_

