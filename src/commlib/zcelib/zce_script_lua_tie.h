//��������ǲο�Tinkerʵ�ֵģ���Ȼ��лԭ����

#ifndef ZCE_LIB_SCRIPT_LUA_H_
#define ZCE_LIB_SCRIPT_LUA_H_

//LUAĿǰ�İ�װ����ʹ��C11������Ч���������µı�����
#if defined  ZCE_USE_LUA && defined ZCE_SUPPORT_CPP11

#if LUA_VERSION_NUM != 501
#error "[Error] please check your lua libary version,Only support 501. LUA_VERSION_NUM != 501."
#endif

#if defined (ZCE_OS_WINDOWS)
#pragma warning ( push )
#pragma warning ( disable : 4127)
#endif

//�����ʹ������ZCE_LUA�����ֿռ䣬��Ϊ��ZCE_LIB�о�������ȫ�����ͻ��
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


    ///Ϊ���ģ�庯���ݹ�׼���ĺ���
    template<typename val_type, typename... val_tlist>
    void push_stack(lua_State * state, val_type val, val_tlist ... val_s)
    {
        push_stack(state, val, val_s...);

    }

    
    ///PUSHһ�����ݵ���ջ��
    //���������long�ı�������Ϊlong�޷���ֲ��
    template<typename val_type> 
    void push_stack(lua_State * /*state*/, val_type /*val*/)
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
        lua_pop(state, 1);
        return t;
    }


    ///��C++11������Ч�����ʵ��
    template<typename ret_type, typename... args_type>
    struct g_cfunctor
    {
        static int invoke(lua_State *state)
        {
            //push�ǽ���������ջ
            void *upvalue_1 = lua_touserdata(state, lua_upvalueindex(1));
            ret_type(*fun_ptr)(args_type...) = (ret_type(*)(args_type...)) (upvalue_1);

            if (std::is_void<ret_type>::value)
            {
                int para_idx = 0;
                fun_ptr(read_stack<args_type>(state, ++para_idx)...);
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
    
    //��ģ�庯��������æʵ��һ������������ͨ��class �ҵ���Ӧ�������ƣ�ע�ᵽLUA�����ƣ���
    template<typename class_type>
    struct class_name
    {
        // global name
        static const char *name(const char *name = NULL)
        {
            const size_t MAX_LEN = 255;
            static char temp[MAX_LEN+1] = "";
            if (name)
            {
                strncpy(temp, name, MAX_LEN);
                temp[MAX_LEN] = '\0';
            }
            return temp;
        }
    };

    //��װ��Ĺ��캯����LUAʹ��
    template<typename class_type, typename... args_type>
    int constructor(lua_State *state)
    {
        //new һ��user data����<T>�Ĵ�С,ͬʱ��ͬʱ��placement new �ķ�ʽ��
        //��ָ��ʽlua_newuserdata����ģ���ɹ��캯��
        int para_idx = 1;
        new(lua_newuserdata(state,
            sizeof(class_type)))
            class_type(read_stack<args_type>(state, ++para_idx)...);

        lua_pushstring(state, class_name<class_type>::name());
        lua_gettable(state, LUA_GLOBALSINDEX);

        lua_setmetatable(state, -2);

        return 1;
    }

    //����������LUA��װ
    template<typename class_type>
    int destroyer(lua_State *state)
    {
        (class_type *)(lua_touserdata(state, 1))->~class_type();
        return 0;
    }
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

    
    /*!
    * @brief      ��LUA����һ������LUA���ԣ�ȫ�ֱ��������ƺͱ�����Ӧֵ�Ŀ�����
    * @tparam     val_type
    * @param      name
    * @param      val
    */
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
    typename val_type get_gval(const char *name)
    {
        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);
        return pop_stack<val_type>(lua_state_);
    }

    ///��LUA����һ��ȫ�ֵ�����
    template<typename val_type>
    void set_gary(const char *name, 
        size_t ary_num,
        typename const val_type ary_data[],
        bool read_only = false)
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        lua_createtable(lua_state_, ary_num, 0);

        for (size_t i = 0; i<ary_num; ++i)
        {
            //�൱��lua_rawseti.ֻ��lua_rawseti���ڲ���ʵ�����˶�ջ˳�����Ӵ��֣�
            //����,����ȷ��֪��lua_rawseti�Ƿ���һЩ�Ż�������Ϊ�о�lua��hashtable����һЩ���⴦��ģ�
            lua_pushnumber(lua_state_,static_cast<int>(i) );
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
    int get_gary(const char *name,
        size_t ary_num,
        typename const ary_type ary_data[]  )
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        //�������һ��table������Ŷ
        if (!lua_istable(lua_state_, -1))
        {
            lua_pop(lua_state_, 1);
            return -1;
        }

        for (size_t i = 0; i < ary_num; ++i)
        {
            lua_pushnumber(lua_state_, static_cast<int>(i));
            lua_gettable(lua_state_,-2)
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
        lua_pushcclosure(lua_state_, ZCE_LUA::g_cfunctor<ret_type, args_type...>::invoke, 1);
        //�������ȫ�ֻ�������
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }
        
    /*!
    * @brief      ����LUA�ĺ�����û�з���ֵ���������ݲ�ȡ��һ������ֵ��
    * @tparam     args_type ���������б�
    * @return     int == 0 ��ʾ�ɹ�  
    * @param      name ��������
    * @param      args �����б�
    */
    template<typename... args_type>
    int call_luafun_0(const char *name, args_type... args)
    {
        int ret = 0;
        //���������ĺ��������Ҽ�¼��ջ�ĵ�ַ
        lua_pushcclosure(lua_state_, ZCE_LUA::on_error, 0);
        int errfunc = lua_gettop(lua_state_);

        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);
        //������Ƿ��Ǻ���
        if (!lua_isfunction(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] call_luafun() attempt to call global `%s' (not a function)", name);
            lua_pop(lua_state_,1);
            return -1;
        }

        //�����ջ������args
        ZCE_LUA::push_stack(lua_state_, args...);

        size_t arg_num = sizeof...(args);
        //����lua�ĺ�����Ĭ��ֻ��һ������ֵ��
        ret = lua_pcall(lua_state_,
            static_cast<int>(arg_num),
            ,
            errfunc);
        if (ret != 0)
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] lua_pcall ret = %d", ret);
        }

        //�ڶ�ջɾ����������ĺ���
        lua_remove(lua_state_, errfunc);

        //ע���������û��ȡ������ֵ���������������û�з���ֵ

        return 0;
    }
    
    ///����LUA�ĺ�����ֻ��һ������ֵ
    template<typename ret_type1, typename... args_type>
    int call_luafun_1(const char *name, ret_type1 ret_val1, args_type... args)
    {
        int ret = 0;
        ret = call_luafun_0(args...);
        if (ret != 0)
        {
            return ret;
        }
        //�ڶ�ջ��������ֵ
        ret_val1 = ZCE_LUA::pop_stack<ret_type1>(lua_state_);
        return 0;
    }

    ///����LUA�ĺ���������������ֵ
    template<typename ret_type1, typename ret_type2, typename... args_type>
    int call_luafun_2(const char *name, ret_type1 ret_val1, ret_type2 ret_val2, args_type... args)
    {
        int ret = 0;
        ret = call_luafun_0(args...);
        if (ret != 0)
        {
            return ret;
        }
        //�ڶ�ջ��������ֵ
        ret_val1 = ZCE_LUA::pop_stack<ret_type1>(lua_state_);
        ret_val2 = ZCE_LUA::pop_stack<ret_type2>(lua_state_);
        return 0;
    }

    // class init
    //��ĳ�ʼ������class����lua��ʹ��
    //�������metatable�ı�����˵ԭ�͵ı�
    template<typename class_type>
    void reg_class(const char *name)
    {
        //��T������
        ZCE_LUA::class_name<class_type>::name(name);

        //�������
        lua_pushstring(lua_state_, name);
        //new һ��table�����table����Ϊ���������metatable�ģ�ĳ�̶ֳ���Ҳ����˵��ԭ�ͣ���
        lua_newtable(state);

        //__name���Ǳ�׼��Ԫ������������������ʹ��
        lua_pushstring(lua_state_, "__name");
        lua_pushstring(lua_state_, name);
        lua_rawset(lua_state_, -3);

        //��meta_get������Ϊ__index����
        lua_pushstring(lua_state_, "__index");
        lua_pushcclosure(lua_state_, ZCE_LUA::meta_get, 0);
        lua_rawset(lua_state_, -3);

        //��meta_set������Ϊ__index����
        lua_pushstring(lua_state_, "__newindex");
        lua_pushcclosure(lua_state_, ZCE_LUA::meta_set, 0);
        lua_rawset(lua_state_, -3);

        //�������պ���
        lua_pushstring(lua_state_, "__gc");
        lua_pushcclosure(lua_state_, destroyer<class_type>, 0);
        lua_rawset(lua_state_, -3);

        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }


    //template<typename ret_type1, typename ret_type2, typename... args_type>

    // class_type ����
    // constructor_fun �ǹ��캯���ķ�װ��ZCE_LUA::constructor
    template<typename class_type, typename constructor_fun>
    int class_constructor(constructor_fun func)
    {
        //����������ƣ�ȡ�����metatable�ı�����˵ԭ�͡�
        lua_pushstring(lua_state_, class_name<class_type>::name());
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        //���ջ����һ����
        if (lua_istable(lua_state_, -1))
        {
            //��������metatable�ı�����һ��metatable������������һ��__call�Ķ�Ӧ����
            //������Ŀ���������ģ�__call�Ƕ�Ӧһ��()���ã���ʵ�岻�Ǻ���ʽ���ĵ��ú���
            //LUA�г��������ĵ��ã�
            //object =class_name()
            lua_newtable(lua_state_);

            lua_pushstring(lua_state_, "__call");
            lua_pushcclosure(lua_state_, func, 0);
            lua_rawset(lua_state_, -3);
            //�������table��Ϊclass ԭ�͵�metatable.
            //����˵�������table��Ϊclass metatable��metatable.
            lua_setmetatable(lua_state_, -2);

            lua_pop(lua_state_, 1);
            return 0;
        }
        else
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] class name[%s] is not tie to lua.",
                class_name<class_type>::name());
            ZCE_ASSERT(false);
            lua_pop(lua_state_, 1);
            return -1;
        }
    }


protected:

    //
    lua_State   *lua_state_;
};

#if defined (ZCE_OS_WINDOWS)
#pragma warning ( pop )
#endif

#endif //#if defined  ZCE_USE_LUA && defined ZCE_SUPPORT_CPP11

#endif // ZCE_LIB_SCRIPT_LUA_H_

