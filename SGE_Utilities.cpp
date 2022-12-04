/* SGE_Utilities.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
 * =======================================================================
 *
 * The SGE was developed by Joseph21 and is heavily inspired bij the Pixel Game Engine (PGE) by Javidx9
 * (see: https://github.com/OneLoneCoder/olcPixelGameEngine). It's interface is deliberately kept very
 * close to that of the PGE, so that programs can be ported from the one to the other quite easily.
 *
 * License
 * -------
 * This code is completely free to use, change, rewrite or get inspiration from. At the same time, there's
 * no warranty that this code is free of bugs. If you use (any part of) this code, you accept each and any
 * risk or consequence thereof.
 *
 * Although there is no obligation to mention or shout out to the creator, I wouldn't mind if you did :)
 *
 * Have fun with it!
 *
 * Joseph21
 * december 4, 2022
 */

#include "SGE_Utilities.h"

#include <iostream>
#include <deque>

//                           +------------------+                            //
// --------------------------+ GLOBAL VARIABLES +--------------------------- //
//                           +------------------+                            //

std::ofstream debugFile;

SDL_PixelFormat *glbPixelFormatPtr = nullptr;
SDL_Renderer    *glbRendererPtr    = nullptr;

//                              +------------+                               //
// -----------------------------+ FUNCTIONS  +------------------------------ //
//                              +------------+                               //

char to_hex_position( uint8_t val ) {
    char result = '?';
    if (val >= 16) {
        std::cout << "ERROR: to_hex_position() --> argument > 0xF: " << val << std::endl;
    } else {
        result = (val < 10) ? '0' + val : 'A' + (val - 10);
    }
    return result;
}

std::string to_hex( uint32_t val ) {
    // Break the 32 bit int up into 8 groups of 4 bits and push these to the front of the
    // auxiliary queue. Least significant will be at the back, most significant at the front.
    std::deque<char> hexQueue;
    uint32_t input = val;
    std::string output;
    for (int i = 0; i < 8; i++) {
        uint8_t byteVal = (input % 16);
        hexQueue.push_front( byteVal );
        input = input / 16;
    }
    // Build the result string by walking the auxiliary queue front to back and convering the
    // 4 bit values into their char hexadecmial representations.
    output.append( "0x" );
    for (auto e : hexQueue) {
        output.append( std::string( 1, to_hex_position( e )));
    }
    return output;
}


// returns a clamp of the first parameter between the second and third
uint8_t Clamp( uint8_t a, uint8_t a_start, uint8_t a_end ) {
    uint8_t retVal = a;
    if (retVal < a_start) retVal = a_start;
    if (retVal > a_end  ) retVal = a_end;
    return retVal;
}

// returns a clamp of the first parameter between the second and third
int Clamp( int a, int a_start, int a_end ) {
    int retVal = a;
    if (retVal < a_start) retVal = a_start;
    if (retVal > a_end  ) retVal = a_end;
    return retVal;
}

float Clamp( float a, float a_start, float a_end ) {
    float retVal = a;
    if (retVal < a_start) retVal = a_start;
    if (retVal > a_end  ) retVal = a_end;
    return retVal;
}

// function produces a right aligned string of length positions containing value
std::string right_align( int value, int positions ) {
    std::string aux = std::to_string( value );
    return right_align( aux, positions );
}

std::string right_align( std::string &s, int positions ) {
    std::string result;

    for (int i = 0; i < (positions - (int)s.length()); i++)
        result.append( " " );
    result.append( s );

    return result;
}

// function produces a left aligned string of length positions containing value
std::string left_align( int value, int positions ) {
    std::string aux = std::to_string( value );
    return left_align( aux, positions );
}

std::string left_align( std::string &s, int positions ) {
    std::string result;

    result.append( s );
    for (int i = 0; i < (positions - (int)s.length()); i++)
        result.append( " " );

    return result;
}

// float f resp. string s contains a floating point number which may (or may not) have a dot in it.
// align the dot of this number on dotPosition, creating a string of totalPositions
std::string dot_align( float f, int dotPosition, int totalPositions ) {
    std::string aux = std::to_string( f );
    return dot_align( aux, dotPosition, totalPositions );
}

std::string dot_align( std::string &s, int dotPosition, int totalPositions ) {
    std::string result;
    size_t dotIndex = s.find( "." );
    std::string sBeforeDot, sAfterDot, sDot;
    if (dotIndex == std::string::npos) {     // string contained no '.'
        sBeforeDot = s;
        sDot = "";
        sAfterDot = "";
    } else {
        sBeforeDot = s.substr( 0, dotIndex );
        sDot = ".";
        sAfterDot  = s.substr( dotIndex + 1 );
    }
    result.append( right_align( sBeforeDot, dotPosition - 1 ));
    result.append( sDot );
    result.append( left_align( sAfterDot, totalPositions - dotPosition ));
    return result;
}



// fills an SDL_Rect structure using the input parameters
void flc::InitSDL_Rect( SDL_Rect &rect, int x, int y, int w, int h ) {
    rect = { x, y, w, h };
}

// fills an SDL_Rect structure using the input parameters
void flc::InitSDL_Point( SDL_Point &point, int x, int y ) {
    point = { x, y };
}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
