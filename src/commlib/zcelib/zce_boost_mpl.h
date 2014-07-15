#ifndef ZCE_LIB_BOOST_MPL_H_
#define ZCE_LIB_BOOST_MPL_H_

//���쿴��һ�Σ��׷���ǧ�ɣ�����д���У���֪������δ���������

namespace ZCE_LIB
{

//�����������boost ��
//JiangweiSun  �������blog �� http://www.suninf.net/SFINAE-and-enable_if/ �ܺõ�����
template<typename T>
struct helper
{
    typedef void type;
};

//==================================================================
// �������T�Ƿ�����ڲ�����Ϊtype��typedef

// ��Ĭ��ģ�����ָʾĬ�����
template<typename T, typename U = void> 
struct has_def_type_impl
{
    static const bool value = false;
};

//ƫ�ػ�������ȥ�����T��ȡ���ͣ� T�����õ�typeʱѡ��
template<typename T> 
struct has_def_type_impl < T, typename helper<typename T::type>::type >
{
    static const bool value = true;
};

// �������T�Ƿ�����ڲ�����Ϊtype��typedef
template<typename T>
struct has_def_type : has_def_type_impl < T >
{
}; 


//==================================================================
//�������T�Ƿ�����ڲ�����Ϊfirst_type��typedef

template<typename T, typename U = void>
struct has_def_first_type_impl
{
    static const bool value = false;
};

template<typename T>
struct has_def_first_type_impl < T, typename helper<typename T::first_type>::type >
{
    static const bool value = true;
};


template<typename T>
struct has_def_first_type : has_def_first_type_impl < T >
{
};


//==================================================================
// �������T�Ƿ�����ڲ�����Ϊkey_type��typedef

template<typename T, typename U = void>
struct has_def_key_type_impl
{
    static const bool value = false;
};


template<typename T>
struct has_def_key_type_impl < T, typename helper<typename T::key_type>::type >
{
    static const bool value = true;
};


template<typename T>
struct has_def_key_type : has_def_key_type_impl < T >
{
};


};





#endif //ZCE_LIB_BOOST_MPL_H_
