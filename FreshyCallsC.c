#include "phnt.h"
#include "Macros.h"
#include "MiniStd.h"


#include <stdio.h>


typedef struct _Ctx {

    struct {
        typeof( & printf ) printf;
        typeof( & getchar ) getchar;
    } MsvCrt;

} CTX, *PCTX;

PCTX Ctx;


#define NT_API_NUM_MAX 512
#define NT_API_NAME_MAX 68

typedef struct _VX_TABLE {
    PVOID FuncAddr;                     // Virtual Address
    UCHAR FuncName[ NT_API_NAME_MAX ];  // name of function
    DWORD FuncHash;                     // hashed function name
    PVOID SysAddr;                      // virtual address of `syscall` instruction in syscall stub
} VX_TABLE, *PVX_TABLE;

VX_TABLE g_NtDllSyscallArray[ NT_API_NUM_MAX ] = { 0 };
///KEYVALUE g_Win32uArray[ WIN32U_API_NUM_MAX ] = { 0 };


void Swap( PVX_TABLE x, PVX_TABLE y ) 
{
    VX_TABLE tmp = *x;
    *x = *y;
    *y = tmp;
}


void SortVXTableArrayByAddr( SIZE_T SSN_MAX ) 
{
    for ( SIZE_T i = 0; i < SSN_MAX - 1; i++ )
    {
        for ( SIZE_T j = 0; j < SSN_MAX - i - 1; j++ ) 
        {
            if ( g_NtDllSyscallArray[ j ].FuncAddr > g_NtDllSyscallArray[ j + 1 ].FuncAddr ) 
            {
                Swap( & g_NtDllSyscallArray[ j ], & g_NtDllSyscallArray[ j + 1 ] );
            }
        }
    }
}


PVOID
SysExtractSsnFreshyCalls(
    IN PVOID ModuleBase
) {
    PIMAGE_NT_HEADERS       ImgNtHdrs     = { 0 };
    PIMAGE_EXPORT_DIRECTORY ImgExpDir     = { 0 };
    SIZE_T                  ImgExpDirSize = { 0 };
    PDWORD                  FuncNameArray = { 0 };
    PDWORD                  FuncAddrArray = { 0 };
    PWORD                   FuncOrdnArray = { 0 };
    PVOID                   FuncAddr      = { 0 };
    PCHAR                   FuncName      = { 0 };

    ImgNtHdrs = RVA2VA( PVOID, ModuleBase, ( ( PIMAGE_DOS_HEADER ) ModuleBase )->e_lfanew );
    if ( ImgNtHdrs->Signature != IMAGE_NT_SIGNATURE ) {
        return NULL;
    }

    //
    // parse EDT from optional header
    //
    ImgExpDir     = RVA2VA( PVOID, ModuleBase, ( ( PIMAGE_OPTIONAL_HEADER ) & ImgNtHdrs->OptionalHeader )->DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress );
    ImgExpDirSize = RVA2VA( DWORD, ModuleBase, ( ( PIMAGE_OPTIONAL_HEADER ) & ImgNtHdrs->OptionalHeader )->DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].Size );

    FuncNameArray = RVA2VA( PVOID, ModuleBase, ImgExpDir->AddressOfNames );
    FuncAddrArray = RVA2VA( PVOID, ModuleBase, ImgExpDir->AddressOfFunctions );
    FuncOrdnArray = RVA2VA( PVOID, ModuleBase, ImgExpDir->AddressOfNameOrdinals );

    WORD sysidx = 0; // will be SSN_MAX

    //
    // iterate over EAT
    //
    for ( DWORD idx = 0; idx < ImgExpDir->NumberOfFunctions; idx++ )
    {
        //
        // retrieve current function name
        //
        FuncName = RVA2VA( PCHAR, ModuleBase, FuncNameArray[ idx ] );

        //
        // if function name starts with `Zw`, function is a syscall stub
        //
        if ( MemCompare( C_PTR( FuncName ), "Zw", 2 ) == 0 )
        {
            FuncAddr = RVA2VA( PVOID, ModuleBase, FuncAddrArray[ FuncOrdnArray[ idx ] ] );
            
            //
            // save function virtual address and function name
            //
            g_NtDllSyscallArray[ sysidx ].FuncAddr = FuncAddr;
            CopyMemoryEx( g_NtDllSyscallArray[ sysidx ].FuncName, FuncName, StrLenA( FuncName ) );

            sysidx++;
        }
    }

    SortVXTableArrayByAddr( sysidx );

    for ( SIZE_T Ssn = 0; Ssn < sysidx ; Ssn++ ) {
        PRINTF( "FuncAddr [ 0x%p ] Ssn [ %3zX ] FuncName [ %s ]\n", g_NtDllSyscallArray[ Ssn ].FuncAddr, Ssn, g_NtDllSyscallArray[ Ssn ].FuncName );
    }

    return NULL;
}



WORD GetSsnByFunctionName( PUCHAR FuncName ) 
{
    //
    // iterate over KV array
    //
    for ( SIZE_T Ssn = 0; Ssn < NT_API_NUM_MAX; Ssn++ ) 
    {
        //
        // find function name in array and return index as SSN
        //
        if ( MemCompare( g_NtDllSyscallArray[ Ssn ].FuncName, FuncName, StrLenA( FuncName ) ) == 0 ) 
        {
            return Ssn;
        }
    }
    return 0xFFFF; // invalid SSN
}

#pragma comment( linker, "/ENTRY:WinMain" )
INT WinMain( VOID ) 
{
    CTX Context = { 0 };
    Ctx = & Context;

    PVOID MsvCrt = LoadLibraryA( "MsvCrt.dll" );

    Ctx->MsvCrt.printf = GetProcAddress( MsvCrt, "printf" );
    Ctx->MsvCrt.getchar = GetProcAddress( MsvCrt, "getchar" );

    PVOID NtDll = GetModuleHandleA( "ntdll.dll" );

    SysExtractSsnFreshyCalls( NtDll );

    PUCHAR SysNtFunctionName = "ZwCreateThreadEx";
    WORD SystemServiceNumber = GetSsnByFunctionName( SysNtFunctionName );

    PRINTF( "%s : [ %X ]", SysNtFunctionName, SystemServiceNumber );

    GETCHAR()

    return 0;
}