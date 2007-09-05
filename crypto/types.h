#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char u_int8_t;
typedef unsigned int u_int32_t;
typedef unsigned long long u_int64_t;

#ifndef LITTLE_ENDIAN
    #define LITTLE_ENDIAN 1234
#endif

#ifndef BIG_ENDIAN
    #define BIG_ENDIAN    4321
#endif

#ifndef BYTE_ORDER
    #define BYTE_ORDER LITTLE_ENDIAN
#endif

#endif
