#include "MiniStd.h"

// functions are from https://github.com/vxunderground/VX-API

// https://github.com/xrombar/flower/blob/06ba9f1eca599c1ea157e81868ab61ab57d35c8a/src/Flower.c#L1181
INT
MemCompare(
    _In_ PVOID m1,
    _In_ PVOID m2,
    _In_ UINT  len
) {
    PUCHAR p = m1;
    PUCHAR q = m2;
    INT charCompareStatus = 0;

    if ( m1 == m2 ) {
        return charCompareStatus;
    }

    while ( len > 0 ) 
    {
        if ( *p != *q )
        {
            charCompareStatus = ( *p > *q ) ? 1 : -1;
            break;
        }
        len--;
        p++;
        q++;
    }

    return charCompareStatus;
}

PVOID
CopyMemoryEx( 
    _Inout_    PVOID  Destination, 
    _In_ CONST PVOID  Source, 
    _In_       SIZE_T Length
) {
    PBYTE Dst = ( PBYTE )Destination;
    PBYTE Src = ( PBYTE )Source;

    while ( Length-- ) 
    {
        *Dst++ = *Src++;
    }

    return Destination;
}

// https://github.com/vxunderground/VX-API/blob/main/VX-API/StringLength.cpp
SIZE_T
StrLenA( 
    _In_ LPCSTR String
) {
    LPCSTR String2;
    
    for ( String2 = String; *String2; ++String2 );
    
    return ( String2 - String );
}

SIZE_T
StrLenW( 
    _In_ LPCWSTR String
) {
    LPCWSTR String2;

    for ( String2 = String; *String2; ++String2 );
    
    return ( String2 - String );
}
