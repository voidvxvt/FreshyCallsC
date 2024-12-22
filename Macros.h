#ifndef MACROS_H
#define MACROS_H


//
// Relative Virtual Address To Virtual Address
//
#define RVA2VA( TYPE, BASE, RVA ) ( TYPE )( ( UINT_PTR ) BASE + RVA )

//
// memory macros
//
#define MemCopy         __builtin__memcpy
#define MemSet          __stosb
#define MemZero( p, l ) __stosb( p, 0, l )

//
// casting macros
// 
#define C_PTR( x )   ( ( PVOID    ) ( x ) )
#define U_PTR( x )   ( ( UINT_PTR ) ( x ) )
#define B_PTR( x )   ( ( UINT8    ) ( x ) )
#define U_PTR16( x ) ( ( UINT16   ) ( x ) )
#define U_PTR32( x ) ( ( UINT32   ) ( x ) )
#define U_PTR64( x ) ( ( UINT64   ) ( x ) )

//
// dereference macros
//
#define DEREF( x )     ( ( PVOID ) *( PVOID*  ) ( x ) )
#define DEREF_U8( x )  ( ( BYTE  ) *( UINT8*  ) ( x ) )
#define DEREF_U16( x ) ( ( WORD  ) *( UINT16* ) ( x ) )
#define DEREF_U32( x ) ( ( DWORD ) *( UINT32* ) ( x ) )
#define DEREF_U64( x ) ( ( QWORD ) *( UINT64* ) ( x ) )

#define MAX( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )


#include <stdio.h>
#define PUTS( s )           { Ctx->MsvCrt.printf( s "\n" ); }
#define PRINTF( f, ... )    { Ctx->MsvCrt.printf( f "\n", __VA_ARGS__ ); }
#define GETCHAR()           { Ctx->MsvCrt.getchar(); }


#endif//MACROS_H
