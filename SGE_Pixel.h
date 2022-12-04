#ifndef PIXEL_H
#define PIXEL_H

/* SGE_Pixel.h - part of the SDL2-based Game Engine (SGE) v.20221204
 * =================================================================
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

//                          +--------------------+                           //
// -------------------------+ MODULE DESCRIPTION +-------------------------- //
//                          +--------------------+                           //

#include <SDL.h>

#include "SGE_Utilities.h"

//                               +-----------+                               //
// ------------------------------+ CONSTANTS +------------------------------ //
//                               +-----------+                               //

#define DEFAULT_R_NUM     0      // numeric default RGBA values for pixels
#define DEFAULT_G_NUM     0
#define DEFAULT_B_NUM     0
#define DEFAULT_A_NUM   255
#define DEFAULT_A_NRM     1.0f   // normalized default A value

namespace flc {

//                           +------------------+                            //
// --------------------------+ GLOBAL VARIABLES +--------------------------- //
//                           +------------------+                            //

    // global masks for getting RGBA values out of an uint32_t encoded pixel
    extern uint32_t glb_rmask,
                    glb_gmask,
                    glb_bmask,
                    glb_amask;
    // global shift values for getting RGBA values out of an uint32_t encoded pixel
    extern short    glb_rshift,
                    glb_gshift,
                    glb_bshift,
                    glb_ashift;
    // global loss values for getting RGBA values out of an uint32_t encoded pixel
    extern short    glb_rloss,
                    glb_gloss,
                    glb_bloss,
                    glb_aloss;

//                              +------------+                               //
// -----------------------------+ PROTOTYPES +------------------------------ //
//                              +------------+                               //

    // component getters on uint32_t encoded pixel
    uint8_t unpackR( uint32_t encodedCol );
    uint8_t unpackG( uint32_t encodedCol );
    uint8_t unpackB( uint32_t encodedCol );
    uint8_t unpackA( uint32_t encodedCol );

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

    class Pixel {
    public:
        uint8_t r, g, b, a;    // must be public for compliance with PGE

    public:
        // enumerates all possible pixel modes
        enum Mode {
            NORMAL,  // no transparency
            MASK,    // pixel with any transparency is not drawn
            ALPHA,   // regular alpha blending
            APROP,   // alpha blending with propagation to destination sprite
            CUSTOM   // custom blending (you must provide a custom blend function)
         };

    public:
        Pixel();
        ~Pixel();
        // initializer constructors
        Pixel( uint8_t uR, uint8_t uG, uint8_t uB, uint8_t uA = DEFAULT_A_NUM );
        Pixel( int     nR, int     nG, int     nB, int     nA = DEFAULT_A_NUM );
        Pixel( float   fR, float   fG, float   fB, float   fA = DEFAULT_A_NRM );
        Pixel( uint32_t encodedCol );
//        // copy constructor
//        Pixel( Pixel &p );

        // colour component setters
        void setR( uint8_t uR );
        void setG( uint8_t uG );
        void setB( uint8_t uB );
        void setA( uint8_t uA );
        // colour component getters
        uint8_t getR();
        uint8_t getG();
        uint8_t getB();
        uint8_t getA();
        // encodes the RGBA values of this pixel to a uint32_t
        uint32_t Encode();

        // operators on pixel and scalar
        Pixel operator *( const float rhs ) const {
            uint8_t rVal = (uint8_t)Clamp( float(this->r) * rhs, 0.0f, 255.0f );
            uint8_t gVal = (uint8_t)Clamp( float(this->g) * rhs, 0.0f, 255.0f );
            uint8_t bVal = (uint8_t)Clamp( float(this->b) * rhs, 0.0f, 255.0f );
            return Pixel( rVal, gVal, bVal );
        }
        Pixel operator /( const float rhs ) const {
            uint8_t rVal = (uint8_t)Clamp( float(this->r) / rhs, 0.0f, 255.0f );
            uint8_t gVal = (uint8_t)Clamp( float(this->g) / rhs, 0.0f, 255.0f );
            uint8_t bVal = (uint8_t)Clamp( float(this->b) / rhs, 0.0f, 255.0f );
            return Pixel( rVal, gVal, bVal );
        }
        // operators on pixel and pixel
        Pixel operator +( const Pixel &rhs ) {
            this->r = (int)(float(this->r) + rhs.r );
            this->g = (int)(float(this->g) + rhs.g );
            this->b = (int)(float(this->b) + rhs.b );
            this->PixelClamp();
            return *this;
        }
        Pixel operator -( const Pixel &rhs ) {
            this->r = (int)(float(this->r) - rhs.r );
            this->g = (int)(float(this->g) - rhs.g );
            this->b = (int)(float(this->b) - rhs.b );
            this->PixelClamp();
            return *this;
        }
        // operators on pixel and scalar
        Pixel &operator *=( const float rhs ) {
            this->r *= rhs;
            this->g *= rhs;
            this->b *= rhs;
            this->PixelClamp();
            return *this;
        }
        Pixel &operator /=( const float rhs ) {
            this->r /= rhs;
            this->g /= rhs;
            this->b /= rhs;
            this->PixelClamp();
            return *this;
        }
        // operators on pixel and pixel
        Pixel &operator +=( const Pixel &rhs ) {
            this->r += rhs.r;
            this->g += rhs.g;
            this->b += rhs.b;
            this->PixelClamp();
            return *this;
        }
        Pixel &operator -=( const Pixel &rhs ) {
            this->r -= rhs.r;
            this->g -= rhs.g;
            this->b -= rhs.b;
            this->PixelClamp();
            return *this;
        }

        // comparison operators on pixels
        bool operator ==( const Pixel & rhs ) {
            return (this->r == rhs.r && this->g == rhs.g && this->b == rhs.b && this->a == rhs.a );
        }
        bool operator !=( const Pixel & rhs ) {
            return (this->r != rhs.r || this->g != rhs.g || this->b != rhs.b || this->a != rhs.a );
        }
        bool operator <  (const Pixel &rhs ) const {
            return ( this->r <  rhs.r) ||
                   ((this->r == rhs.r) && (( this->g <  rhs.g) ||
                                           ((this->g == rhs.g) && (( this->b <  rhs.b) ||
                                                                   ((this->b == rhs.b) && (this->a < rhs.a))))));
        }
        bool operator >  (const Pixel &rhs ) const {
            return ( this->r >  rhs.r) ||
                   ((this->r == rhs.r) && (( this->g >  rhs.g) ||
                                           ((this->g == rhs.g) && (( this->b >  rhs.b) ||
                                                                   ((this->b == rhs.b) && (this->a > rhs.a))))));
        }

        // returns the inverse of current pixel (each rgb value is subtracted from 255)
        Pixel inv() const;
        // returns lerp (linear interpolation) of p1 and p2 w.r.t. percentage t
        Pixel PixelLerp( const Pixel& p1, const Pixel& p2, float t );

    private:
        // clamps all RGBA values within [0, 255]
        void PixelClamp();
    };

    // predefined colours from colour scheme
    static const Pixel
                                        // the shades on the right (DARK_ / VERY_DARK_) are inspired by JavidX9's PGE
        BLACK(   0.00f, 0.00f, 0.00f ),
        RED(     1.00f, 0.00f, 0.00f ),
        ORANGE(  1.00f, 0.50f, 0.00f ), DARK_ORANGE(  0.50f, 0.25f, 0.00f ), VERY_DARK_ORANGE(  0.25f, 0.13f, 0.00f ),
        YELLOW(  1.00f, 1.00f, 0.00f ),
        MINTGRN( 0.50f, 1.00f, 0.00f ), DARK_MINTGRN( 0.25f, 0.50f, 0.00f ), VERY_DARK_MINTGRN( 0.13f, 0.25f, 0.00f ),
        GREEN(   0.00f, 1.00f, 0.00f ),
        PALEGRN( 0.00f, 1.00f, 0.50f ), DARK_PALEGRN( 0.00f, 0.50f, 0.25f ), VERY_DARK_PALEGRN( 0.00f, 0.25f, 0.13f ),
        CYAN(    0.00f, 1.00f, 1.00f ),
        BLUEGRN( 0.00f, 0.50f, 1.00f ), DARK_BLUEGRN( 0.00f, 0.25f, 0.50f ), VERY_DARK_BLUEGRN( 0.00f, 0.13f, 0.25f ),
        BLUE(    0.00f, 0.00f, 1.00f ),
        PURPLE(  0.50f, 0.00f, 1.00f ), DARK_PURPLE(  0.25f, 0.00f, 0.50f ), VERY_DARK_PURPLE(  0.13f, 0.00f, 0.25f ),
        MAGENTA( 1.00f, 0.00f, 1.00f ),
        PINK(    1.00f, 0.00f, 0.50f ), DARK_PINK(    0.50f, 0.00f, 0.25f ), VERY_DARK_PINK(    0.25f, 0.00f, 0.13f ),
        GREY(    0.75f, 0.75f, 0.75f ),
        WHITE(   1.00f, 1.00f, 1.00f ),

        // these constants are to ensure compatibility with JavidX9's PGE
		DARK_GREY(    0.50f, 0.50f, 0.50f ), VERY_DARK_GREY(    0.25f, 0.25f, 0.25f ),
	    DARK_RED(     0.50f, 0.00f, 0.00f ), VERY_DARK_RED(     0.25f, 0.00f, 0.00f ),
	    DARK_YELLOW(  0.50f, 0.50f, 0.00f ), VERY_DARK_YELLOW(  0.25f, 0.25f, 0.00f ),
	    DARK_GREEN(   0.00f, 0.50f, 0.00f ), VERY_DARK_GREEN(   0.00f, 0.25f, 0.00f ),
	    DARK_CYAN(    0.00f, 0.50f, 0.50f ), VERY_DARK_CYAN(    0.00f, 0.25f, 0.25f ),
	    DARK_BLUE(    0.00f, 0.00f, 0.50f ), VERY_DARK_BLUE(    0.00f, 0.00f, 0.25f ),
	    DARK_MAGENTA( 0.50f, 0.00f, 0.50f ), VERY_DARK_MAGENTA( 0.25f, 0.00f, 0.25f ),
	    BLANK(        0.00f, 0.00f, 0.00f, 0.00f ),

	    // Special colours - thanks Loubitek!
        AMETHYST(       uint8_t(153), uint8_t(102), uint8_t(204)),
        AMBER(          uint8_t(255), uint8_t(191), uint8_t(  0)),
        GOLDEN(         uint8_t(255), uint8_t(215), uint8_t(  0)),
        SILVER(         uint8_t(192), uint8_t(192), uint8_t(192)),
        PLATINUM(       uint8_t(229), uint8_t(228), uint8_t(226)),
        ANTIQUE_BRONZE( uint8_t(102), uint8_t( 93), uint8_t( 30)),
        BRONZE(         uint8_t(205), uint8_t(127), uint8_t( 50)),
        COPPER(         uint8_t(184), uint8_t(115), uint8_t( 51)),
        VERMILION(      uint8_t(227), uint8_t( 66), uint8_t( 52)),
        INDIGO(         uint8_t( 63), uint8_t(  0), uint8_t(255)),
        ULTRAMARINE(    uint8_t( 18), uint8_t( 10), uint8_t(143));

} // namespace flc

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // PIXEL_H
