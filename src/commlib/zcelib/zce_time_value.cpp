﻿#include "zce_predefine.h"
#include "zce_time_value.h"

//ZCE_Time_Value默认的zero值
const ZCE_Time_Value ZCE_Time_Value::ZERO_TIME_VALUE(0, 0);
//
const ZCE_Time_Value ZCE_Time_Value::MAX_TIME_VALUE(0x7FFFFFFF, 0x7FFFFFFF);

//默认构造函数
ZCE_Time_Value::ZCE_Time_Value()
{
    zce_time_value_.tv_sec = 0;
    zce_time_value_.tv_usec = 0;
}
//析构函数
ZCE_Time_Value::~ZCE_Time_Value()
{
}

//构造函数，用timeval
ZCE_Time_Value::ZCE_Time_Value(const timeval &time_data)
{
    zce_time_value_ = time_data;
}

//构造函数，用::timespec
ZCE_Time_Value::ZCE_Time_Value(const ::timespec &timespec_val)
{
    zce_time_value_ = ZCE_LIB::make_timeval(&timespec_val);
}

//构造函数，几个时间数据数据
ZCE_Time_Value::ZCE_Time_Value(time_t sec, time_t usec)
{
#if defined ZCE_OS_WINDOWS
    zce_time_value_.tv_sec = static_cast<long>(sec);
    zce_time_value_.tv_usec = static_cast<long>(usec);
#elif defined ZCE_OS_LINUX
    zce_time_value_.tv_sec = sec;
    zce_time_value_.tv_usec = usec;
#endif

}

//构造函数，用time_t, usec被置为0
ZCE_Time_Value::ZCE_Time_Value(time_t sec)
{
#if defined ZCE_OS_WINDOWS
    zce_time_value_.tv_sec = static_cast<long>(sec);
#elif defined ZCE_OS_LINUX
    zce_time_value_.tv_sec = sec;
#endif

    zce_time_value_.tv_usec = 0;
}

#ifdef ZCE_OS_WINDOWS
//构造函数，用LPFILETIME,FILETIME
ZCE_Time_Value::ZCE_Time_Value(LPFILETIME file_time)
{
    zce_time_value_ = ZCE_LIB::make_timeval(file_time);
}

//构造函数，用LPSYSTEMTIME,SYSTEMTIME
ZCE_Time_Value::ZCE_Time_Value(LPSYSTEMTIME system_time)
{
    zce_time_value_ = ZCE_LIB::make_timeval(system_time);
}

#endif

//
void ZCE_Time_Value::set(const timeval &time_data)
{
    zce_time_value_ = time_data;
}
//
void ZCE_Time_Value::set(const ::timespec &timespec_val)
{
    zce_time_value_ = ZCE_LIB::make_timeval(&timespec_val);
}
//
void ZCE_Time_Value::set(time_t sec, time_t usec)
{
#if defined ZCE_OS_WINDOWS
    zce_time_value_.tv_sec = static_cast<long>(sec);
    zce_time_value_.tv_usec = static_cast<long>(usec);
#elif defined ZCE_OS_LINUX
    zce_time_value_.tv_sec = sec;
    zce_time_value_.tv_usec = usec;
#endif
}

//
void ZCE_Time_Value::set(time_t sec)
{
#if defined ZCE_OS_WINDOWS
    zce_time_value_.tv_sec = static_cast<long>(sec);
#elif defined ZCE_OS_LINUX
    zce_time_value_.tv_sec = sec;
#endif

    zce_time_value_.tv_usec = 0;
}

#ifdef ZCE_OS_WINDOWS
//设置ZCE_Time_Value, 用FILETIME
void ZCE_Time_Value::set(LPFILETIME file_time)
{
    zce_time_value_ = ZCE_LIB::make_timeval(file_time);
}
//设置ZCE_Time_Value, 用LPSYSTEMTIME
void ZCE_Time_Value::set(LPSYSTEMTIME system_time)
{
    zce_time_value_ = ZCE_LIB::make_timeval(system_time);
}

#endif

//用clock_t 设置时间，被迫用这个函数名字，避免和其他函数冲突
void ZCE_Time_Value::set_by_clock_t(clock_t time)
{
    zce_time_value_ = ZCE_LIB::make_timeval(time);
}

//得到总共多少毫秒
uint64_t ZCE_Time_Value::total_msec() const
{
    const long SEC_PER_MSEC = 1000;
    const long MSEC_PER_USEC = 1000;
    return static_cast<uint64_t>(this->zce_time_value_.tv_sec) * SEC_PER_MSEC
           + this->zce_time_value_.tv_usec / MSEC_PER_USEC;
}

//四舍五入得到总共多少毫秒，其实不是真正的四舍五入，而是如果微秒有数据，就返回1毫秒，
uint64_t ZCE_Time_Value::total_msec_round() const
{
    uint64_t ret_msec = total_msec();
    //可能微秒数据上有数据，但<1000,
    if ( 0 == ret_msec  && this->zce_time_value_.tv_usec > 0 )
    {
        ret_msec = 1;
    }
    return ret_msec;
}

//用毫秒作为单位设置TimeValue
void ZCE_Time_Value::total_msec(uint64_t set_msec)
{
    const int SEC_PER_MESC = 1000;
    const long MSEC_PER_USEC = 1000;

#if defined ZCE_OS_WINDOWS
    zce_time_value_.tv_sec = static_cast<long>(set_msec / SEC_PER_MESC);
    zce_time_value_.tv_usec = static_cast<long>((set_msec % SEC_PER_MESC) * MSEC_PER_USEC);
#elif defined ZCE_OS_LINUX
    zce_time_value_.tv_sec = static_cast<time_t>(set_msec / SEC_PER_MESC);
    zce_time_value_.tv_usec = static_cast<time_t>((set_msec % SEC_PER_MESC) * MSEC_PER_USEC);
#endif
}

//得到总共多少微秒
uint64_t ZCE_Time_Value::total_usec() const
{
    const long SEC_PER_USEC = 1000000;
    return static_cast<uint64_t>(zce_time_value_.tv_sec) * SEC_PER_USEC + zce_time_value_.tv_usec;
}

//用微秒作为单位，设置TimeValue，注意这个函数和usec函数的区别，usec函数是设置timeval的usec部分，
void ZCE_Time_Value::total_usec(uint64_t set_usec)
{
    const int SEC_PER_UESC = 1000000;
#if defined ZCE_OS_WINDOWS
    zce_time_value_.tv_sec = static_cast<long>(set_usec / SEC_PER_UESC);
    zce_time_value_.tv_usec = static_cast<long>(set_usec % SEC_PER_UESC);
#elif defined ZCE_OS_LINUX
    zce_time_value_.tv_sec = static_cast<time_t>(set_usec / SEC_PER_UESC);
    zce_time_value_.tv_usec = static_cast<time_t>(set_usec % SEC_PER_UESC);
#endif
}

//得到Timevalue的秒部分
time_t ZCE_Time_Value::sec (void) const
{
    return zce_time_value_.tv_sec;
}

// 设置Timevalue的秒部分
void ZCE_Time_Value::sec (time_t sec)
{
#if defined ZCE_OS_WINDOWS
    zce_time_value_.tv_sec = static_cast<long>(sec);
#elif defined ZCE_OS_LINUX
    zce_time_value_.tv_sec = sec;
#endif
}

//得到Timevalue的微秒部分
time_t ZCE_Time_Value::usec (void) const
{
    return zce_time_value_.tv_usec;
}

//设置Timevalue的微秒部分
void ZCE_Time_Value::usec (time_t usec)
{
#if defined ZCE_OS_WINDOWS
    zce_time_value_.tv_usec = static_cast<long>(usec);
#elif defined ZCE_OS_LINUX
    zce_time_value_.tv_usec = usec;
#endif
}

//获取当前时间
void ZCE_Time_Value::gettimeofday()
{
    zce_time_value_ = ZCE_LIB::gettimeofday();
}

//
ZCE_Time_Value &ZCE_Time_Value::operator += (const ZCE_Time_Value &tv)
{
    zce_time_value_ = ZCE_LIB::timeval_add(zce_time_value_, tv.zce_time_value_);
    return *this;
}
// Assign @ tv to this
ZCE_Time_Value &ZCE_Time_Value::operator = (const ZCE_Time_Value &tv)
{
    zce_time_value_ = tv.zce_time_value_;
    return *this;
}

// Subtract @a tv to this.
ZCE_Time_Value &ZCE_Time_Value::operator -= (const ZCE_Time_Value &tv)
{
    //保证返回值里面的时钟>=0
    zce_time_value_ = ZCE_LIB::timeval_sub(zce_time_value_, tv.zce_time_value_, true);
    return *this;
}

// 两个时间进行比较 <
bool ZCE_Time_Value::operator < (const ZCE_Time_Value &tv)
{
    //先比较秒，
    if (zce_time_value_.tv_sec < tv.zce_time_value_.tv_sec)
    {
        return true;
    }

    //再比较微秒
    if (zce_time_value_.tv_sec == tv.zce_time_value_.tv_sec &&
        zce_time_value_.tv_usec < tv.zce_time_value_.tv_usec )
    {
        return true;
    }

    return false;
}

// 两个时间进行比较 >
bool ZCE_Time_Value::operator > (const ZCE_Time_Value &tv)
{
    //先比较秒，
    if (zce_time_value_.tv_sec > tv.zce_time_value_.tv_sec)
    {
        return true;
    }

    //再比较微秒
    if (zce_time_value_.tv_sec == tv.zce_time_value_.tv_sec &&
        zce_time_value_.tv_usec > tv.zce_time_value_.tv_usec )
    {
        return true;
    }

    return false;
}

//
bool ZCE_Time_Value::operator <= (const ZCE_Time_Value &tv)
{
    //先比较秒，
    if (zce_time_value_.tv_sec <= tv.zce_time_value_.tv_sec)
    {
        return true;
    }

    //再比较微秒
    if (zce_time_value_.tv_sec == tv.zce_time_value_.tv_sec &&
        zce_time_value_.tv_usec <= tv.zce_time_value_.tv_usec )
    {
        return true;
    }

    return false;
}

/// True if @a tv1 >= @a tv2.
bool ZCE_Time_Value::operator >= (const ZCE_Time_Value &tv)
{
    //先比较秒，
    if (zce_time_value_.tv_sec > tv.zce_time_value_.tv_sec)
    {
        return true;
    }

    //再比较微秒
    if (zce_time_value_.tv_sec == tv.zce_time_value_.tv_sec &&
        zce_time_value_.tv_usec >= tv.zce_time_value_.tv_usec )
    {
        return true;
    }

    return false;
}

/// True if @a tv1 == @a tv2.
bool ZCE_Time_Value::operator == (const ZCE_Time_Value &tv)
{
    if (zce_time_value_.tv_sec == tv.zce_time_value_.tv_sec &&
        zce_time_value_.tv_usec == tv.zce_time_value_.tv_usec )
    {
        return true;
    }

    return false;
}

/// True if @a tv1 != @a tv2.
bool ZCE_Time_Value::operator != (const ZCE_Time_Value &tv)
{
    return !(*this == tv);
}

/// Adds two ZCE_Time_Value objects together, returns the sum.
ZCE_Time_Value operator + (const ZCE_Time_Value &tv1,
                           const ZCE_Time_Value &tv2)
{
    return ZCE_LIB::timeval_add(tv1.zce_time_value_, tv2.zce_time_value_);
}

/// Subtracts two ZCE_Time_Value objects, returns the difference.
ZCE_Time_Value operator - (const ZCE_Time_Value &tv1,
                           const ZCE_Time_Value &tv2)
{
    //保证返回值里面的时钟>=0
    return ZCE_LIB::timeval_sub(tv1.zce_time_value_, tv2.zce_time_value_, true);
}

/// Returns the value of the object as a timeval.
ZCE_Time_Value::operator timeval () const
{
    return zce_time_value_;
}

//
ZCE_Time_Value::operator const timeval *() const
{
    return &zce_time_value_;
}

//
ZCE_Time_Value::operator timeval *()
{
    return &zce_time_value_;
}

//将时间打印出来
const char *ZCE_Time_Value::to_string(char *str_date_time,
                                      size_t datetime_strlen,
                                      bool utc_time,
                                      ZCE_LIB::TIME_STR_FORMAT_TYPE fmt) const
{
    return ZCE_LIB::timeval_to_str(&(this->zce_time_value_),
                                   str_date_time,
                                   datetime_strlen,
                                   utc_time,
                                   fmt);
}


//从字符串中得到时间
int ZCE_Time_Value::from_string(const char *strtm,
                                bool uct_time,
                                ZCE_LIB::TIME_STR_FORMAT_TYPE fmt)
{
    return ZCE_LIB::str_to_timeval(strtm,
                                   fmt,
                                   uct_time,
                                   &zce_time_value_
                                   );
}

const char *ZCE_Time_Value::timestamp(char *str_date_time,
                                      size_t datetime_strlen
                                     ) const
{
    return ZCE_LIB::timestamp(&(this->zce_time_value_),
                              str_date_time,
                              datetime_strlen);
}