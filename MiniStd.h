#ifndef MINISTD_H
#define MINISTD_H

#include "phnt.h"


INT MemCompare( _In_ PVOID m1, _In_ PVOID m2, _In_ UINT len );
PVOID CopyMemoryEx( _Inout_ PVOID Destination, _In_ PVOID Source, _In_ SIZE_T Length );

SIZE_T StrLenA( _In_ LPCSTR String );
SIZE_T StrLenW( _In_ LPCWSTR String );

#endif // MINISTD_H
