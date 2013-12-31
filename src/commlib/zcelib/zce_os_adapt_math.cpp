/******************************************************************************************
Copyright           : 2002-2003, FXL Technology (Shenzhen) Company Limited.
FileName            : zce_os_adapt_layer.h
Author              : Sail (ZENGXING)/Author name here
Version             :
Date Of Creation    : 2011-5-1
Description         : ʱ��������������㣬��Ҫ������LINUX�¿�£

Others              :
Function List       :
1.  ......
Modification History:
1.Date  :
Author  :
Modification  :
******************************************************************************************/
#include "zce_predefine.h"
#include "zce_os_adapt_predefine.h"
#include "zce_os_adapt_math.h"

// Euclid's greatest common divisor algorithm.
uint32_t ZCE_OS::gcd (uint32_t x, uint32_t y)
{
    while (y != 0)
    {
        u_long r = x % y;
        x = y;
        y = r;
    }

    return x;
}

