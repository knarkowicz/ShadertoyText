#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <memory.h>
#include <algorithm>

unsigned const MAX_LINE_LEN = 2048;
unsigned const MAX_LINE_NUM = 64;

int main( int argc, char *argv[] )
{
    if ( argc < 3 )
    {
        printf( "Usage: ShadertoyText.exe src.txt dst.txt\n" );
        return 0;
    }

    FILE* fi = fopen( argv[ 1 ], "r" );
    if ( !fi )
    {
        printf( "Can't open src file: %s\n", argv[ 1 ] );
        return 0;
    }

    FILE* fo = fopen( argv[ 2 ], "w" );
    if ( !fo )
    {
        printf( "Can't open dst file: %s\n", argv[ 2 ] );
        return 0;
    }


    unsigned char line[ MAX_LINE_LEN ];
    unsigned code[ MAX_LINE_NUM ][ MAX_LINE_LEN / 4 ];
    unsigned lineCharNum[ MAX_LINE_NUM ];
    memset( line, 0, sizeof( line ) );
    memset( lineCharNum, 0, sizeof( lineCharNum ) );
    memset( code, 0, sizeof( code ) );
    unsigned lineId = 0;
    while( fgets( (char*) line, sizeof( line ), fi ) )
    {
        unsigned charId = 0;
        while ( line[ charId ] && line[ charId ] != '\n' )
        {
            code[ lineId ][ charId / 4 ] |= ( line[ charId ] << ( charId % 4 ) * 8 );
            ++charId;
        }
        lineCharNum[ lineId ] = charId;
        ++lineId;
    }

    unsigned maxUIntNum = 0;
    for ( unsigned iLine = 0; iLine < lineId; ++iLine )
    {
        maxUIntNum = std::max( maxUIntNum, ( lineCharNum[ iLine ] + 3 ) / 4 );
    }

    for ( unsigned iLine = 0; iLine < lineId; ++iLine )
    {
        unsigned uintNum = ( lineCharNum[ iLine ] + 3 ) / 4;
        if ( uintNum != maxUIntNum )
        {
            ++uintNum;
        }

        fprintf( fo, "  v = t.y == %u. ? ", lineId - iLine - 1, 4 );
        for ( unsigned j = 1; j < uintNum; ++j )
        {
            fprintf( fo, "( t.x < %u. ? %uu : ", j * 4, code[ iLine ][ j - 1 ] );
        }

        fprintf( fo, "%uu", code[ iLine ][ uintNum - 1 ] );
        for ( unsigned j = 1; j < uintNum; ++j )
        {
            fprintf( fo, " )" );
        }
        fprintf( fo, " : v;\n" );
    }

    fprintf( fo, "  v = t.x >= 0. && t.x < %u. ? v : 0u;", maxUIntNum * 4 );

    return 0;
}
