/* SGE_Draw.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
 * ==================================================================
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
 *
 * Change trace:
 * 09/29/2023 - bug fixed in DrawPartialSprite()
 */

#include "SGE_Core.h"

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// screen stuff - size queries and clearing =====

// returns the dimensions of the current active window
int flc::SDL_GameEngine::ScreenWidth() {  return vWindows[nActiveWindowIx]->GetWidth();  }
int flc::SDL_GameEngine::ScreenHeight() { return vWindows[nActiveWindowIx]->GetHeight(); }

// works on currently acive draw target!!
void flc::SDL_GameEngine::Clear( Pixel colour ) {
    FillRect( 0, 0, GetDrawTargetWidth(), GetDrawTargetHeight(), colour );
}

// pixel drawing =====

// internal method - lowest level pixel drawing. The mask and alpha blending are implemented in here!
// Parameters:
//   * (x, y)     - the location in the draw target to draw the pixel
//   * encodedCol - the colour (pixel) encoded as a uint32_t
//   * pixelPtr   - a pointer to the pixels field of the SDL_Surface (i.e. the draw target)
// NOTE - this method assumes that the SDL_Surface is locked already!
void flc::SDL_GameEngine::ClampedDraw( int x, int y, uint32_t encodedCol, uint32_t *pixelPtr ) {

    // grab the draw target width, it's needed in addressing of the pixels
    int nDTwidth = GetDrawTargetWidth();

    switch (m_PixelMode) {
        case flc::Pixel::NORMAL: {
                // unconditionally write the pixel value to the draw target
                pixelPtr[ y * nDTwidth + x ] = encodedCol;
            }
            break;
        case flc::Pixel::MASK: {
                // write the pixel value only if the alpha component has no transparency
                if (unpackA( encodedCol ) == 255) {
                    pixelPtr[ y * nDTwidth + x ] = encodedCol;
                }
            }
            break;
        case flc::Pixel::ALPHA:
        case flc::Pixel::APROP: {
                // blend the source and the destination value according to alpha blending calculations
                // see: https://en.wikipedia.org/wiki/Alpha_compositing
                flc::Pixel srcPix = Pixel( encodedCol );
                flc::Pixel dstPix = Pixel( pixelPtr[ y * nDTwidth + x ] );
                // lerp new alpha value from src and dst alphas
                float fAlpha_src = float( srcPix.getA()) / 255.0f * m_BlendFactor;
                float fAlpha_dst = float( dstPix.getA()) / 255.0f;
                float fAlpha_new = fAlpha_src + fAlpha_dst * ( 1.0f - fAlpha_src );
                // lerp new rgb values using src and dst alpha, and divide by new alpha value
                int nR_new = int(( float( srcPix.getR() ) * fAlpha_src + float( dstPix.getR() ) * fAlpha_dst * (1.0f - fAlpha_src) ) / fAlpha_new);
                int nG_new = int(( float( srcPix.getG() ) * fAlpha_src + float( dstPix.getG() ) * fAlpha_dst * (1.0f - fAlpha_src) ) / fAlpha_new);
                int nB_new = int(( float( srcPix.getB() ) * fAlpha_src + float( dstPix.getB() ) * fAlpha_dst * (1.0f - fAlpha_src) ) / fAlpha_new);
                int nA_new = int( fAlpha_new * 255 );
                flc::Pixel newPixel = Pixel( (uint8_t)nR_new, (uint8_t)nG_new, (uint8_t)nB_new, (uint8_t)nA_new );
                // write the calculated pixel value to the draw target
                pixelPtr[ y * nDTwidth + x ] = newPixel.Encode();
            }
            break;
        case flc::Pixel::CUSTOM: {
                // use a user provide function to blend the src and dst pixel
                flc::Pixel srcPixel = flc::Pixel( encodedCol );
                flc::Pixel dstPixel = flc::Pixel( pixelPtr[ y * nDTwidth + x ] );
                flc::Pixel newPixel = m_BlendFunc( x, y, srcPixel, dstPixel );
                // write the calculated pixel value to the draw target
                pixelPtr[ y * nDTwidth + x ] = newPixel.Encode();
            }
            break;
        default: {
                std::cout << "WARNING: ClampedDraw() --> invalid blend mode: " << m_PixelMode << std::endl;
            }
    }
}

// Draw a pixel of 'colour' to the drawtarget at location (x, y ). If this location is out of bounds for the draw target, nothing is drawn.
void flc::SDL_GameEngine::Draw( int x, int y, Pixel colour ) {
    Draw( x, y, colour.Encode() );
}

// Draw a pixel of encodedCol to the drawtarget at location (x, y ). If this location is out of bounds for the draw target, nothing is drawn.
void flc::SDL_GameEngine::Draw( int x, int y, uint32_t encodedCol ) {
    if (x >= 0 && x < GetDrawTargetWidth() && y >= 0 && y < GetDrawTargetHeight()) {
        SDL_Surface *pSrfce = pEngineDrawTarget->GetSurfacePtr();
        uint32_t *aux = (uint32_t *)pSrfce->pixels;
        SDL_LockSurface( pSrfce );
        ClampedDraw( x, y, encodedCol, aux );
        SDL_UnlockSurface( pSrfce );
    }
}

// DrawLine() method and aux functions =====

// This method draws any line from (x0, y0) to (x1, y1) using colour and pattern.
// Two cases of horizontal resp. vertical lines are handled separately.
// All other lines are distinguished for their gradient.
void flc::SDL_GameEngine::DrawLine( int x0, int y0, int x1, int y1, Pixel colour, uint32_t nLinePattern ) {

    // lambda for drawing patterns - The 'cur' dot of the line that is drawn is mapped onto pattern.
    // If a 1 bit isfound, this lambda returns true.
    auto pattern_active = [=] ( int fst, int lst, int cur, uint32_t pattern ) -> bool {
        int nBitIx = (fst - cur) % 32;
        uint32_t mask = 0x00000001 << nBitIx;
        return ((pattern & mask) != 0);
    };

    auto plot_horizontal_line = [=] ( int x0, int x1, int y, Pixel colour, uint32_t linePattern ) -> void {
        if (x0 > x1)
            std::swap( x0, x1 );
        for (int x = x0; x <= x1; x++)
            if (pattern_active( x0, x1, x, linePattern ))
                Draw( x, y, colour );
    };

    auto plot_vertical_line = [=] ( int x, int y0, int y1, Pixel colour, uint32_t linePattern ) -> void {
        if (y0 > y1)
            std::swap( y0, y1 );
        for (int y = y0; y <= y1; y++)
            if (pattern_active( y0, y1, y, linePattern ))
                Draw( x, y, colour );
    };

    // low gradient line - m = dy/dx in [-1, 1]: per 1 x step there's < 1 y step
    auto plot_line_low_gradient = [=] ( int x0, int y0, int x1, int y1, Pixel colour, uint32_t linePattern ) -> void {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int yi = 1;
        if (dy < 0) {
            yi = -1;
            dy = -dy;
        }
        int D = (2 * dy) - dx;
        int y = y0;
        for (int x = x0; x <= x1; x++) {
            if (pattern_active( x0, x1, x, linePattern ))
                Draw( x, y, colour );
            if (D > 0) {
                y += yi;
                D += 2 * (dy - dx);
            } else {
                D = D + 2 * dy;
            }
        }
    };

    // high gradient line - m = dy/dx outside of [-1, 1]: per 1 y step there's < 1 x step
    auto plot_line_high_gradient = [=] ( int x0, int y0, int x1, int y1, Pixel colour, uint32_t linePattern ) -> void {
        int dx = x1 - x0;
            int dy = y1 - y0;
        int xi = 1;
        if (dx < 0) {
            xi = -1;
            dx = -dx;
        }
        int D = (2 * dx) - dy;
        int x = x0;
        for (int y = y0; y <= y1; y++) {
            if (pattern_active( y0, y1, y, linePattern ))
                Draw( x, y, colour );
            if (D > 0) {
                x += xi;
                D += 2 * (dx - dy);
            } else {
                D += 2 * dx;
            }
        }
    };

    // implementation of bresenham line plotting
    // See: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    if (x0 == x1) {
        plot_vertical_line(   x0, y0, y1, colour, nLinePattern );
    } else if (y0 == y1) {
        plot_horizontal_line( x0, x1, y0, colour, nLinePattern );
    } else {
        if (abs(y1 - y0) < abs(x1 - x0)) {
            if (x0 > x1) {
                std::swap( x0, x1 );
                std::swap( y0, y1 );
            }
            plot_line_low_gradient( x0, y0, x1, y1, colour, nLinePattern );
        } else {
            if (y0 > y1) {
                std::swap( y0, y1 );
                std::swap( x0, x1 );
            }
            plot_line_high_gradient( x0, y0, x1, y1, colour, nLinePattern );
        }
    }
}

// rectangle drawing =====

// Draw a (non filled) rectangle. The parameters are the upper left resp. lower right corner.
void flc::SDL_GameEngine::DrawRect( int x, int y, int w, int h, Pixel colour ) {

    auto plot_horizontal_line = [=]( int x0, int x1, int y, Pixel colour ) -> void {
        if (x0 > x1)
            std::swap( x0, x1 );
        for (int x = x0; x <= x1; x++)
            Draw( x, y, colour );
    };

    auto plot_vertical_line = [=]( int x, int y0, int y1, Pixel colour ) -> void {
        if (y0 > y1)
            std::swap( y0, y1 );
        for (int y = y0; y <= y1; y++)
            Draw( x, y, colour );
    };

    plot_horizontal_line( x    , x + w, y    , colour );
    plot_horizontal_line( x    , x + w, y + h, colour );
    plot_vertical_line(   x    , y    , y + h, colour );
    plot_vertical_line(   x + w, y    , y + h, colour );
}

// Draw a filled rectangle. The parameters are the upper left resp. lower right corner.
void flc::SDL_GameEngine::FillRect( int x, int y, int w, int h, Pixel colour ) {
    // Clamp the corner points of the rectangle to fill within the boundaries of the drawtarget
    int aux_x1 = Clamp( x    , 0, GetDrawTargetWidth()  );
    int aux_y1 = Clamp( y    , 0, GetDrawTargetHeight() );
    int aux_x2 = Clamp( x + w, 0, GetDrawTargetWidth()  );
    int aux_y2 = Clamp( y + h, 0, GetDrawTargetHeight() );

    // Fill the rectangle with calls to ClampedDraw()
    uint32_t auxCol = colour.Encode();
    SDL_Surface *pSrfce = pEngineDrawTarget->GetSurfacePtr();
    uint32_t *aux = (uint32_t *)pSrfce->pixels;
    SDL_LockSurface( pSrfce );
    for (int i = aux_x1; i < aux_x2; i++) {
        for (int j = aux_y1; j < aux_y2; j++) {
            ClampedDraw( i, j, auxCol, aux );
        }
    }
    SDL_UnlockSurface( pSrfce );
}

// DrawTriangle() method =====

void flc::SDL_GameEngine::DrawTriangle( int x0, int y0, int x1, int y1, int x2, int y2, Pixel colour ) {
    DrawLine( x0, y0, x1, y1, colour );
    DrawLine( x1, y1, x2, y2, colour );
    DrawLine( x2, y2, x0, y0, colour );
}

// FillTriangle() method and aux functions =====

// https://www.avrfreaks.net/sites/default/files/triangles.c
void flc::SDL_GameEngine::FillTriangle( int x1, int y1, int x2, int y2, int x3, int y3, Pixel c ) {

    auto plot_horizontal_line = [ = ]( int x0, int x1, int y, Pixel colour ) -> void {
        if (x0 > x1)
            std::swap( x0, x1 );
        for (int x = x0; x <= x1; x++)
            Draw( x, y, colour );
    };

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    int changed1 = false;
    int changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;

    // Sort vertices
    if (y1 > y2) {
        std::swap( y1, y2 );
        std::swap( x1, x2 );
    }
    if (y1 > y3) {
        std::swap( y1, y3 );
        std::swap( x1, x3 );
    }
    if (y2 > y3) {
        std::swap( y2, y3 );
        std::swap( x2, x3 );
    }

    t1x = t2x = x1;
    y = y1;   // Starting points
    dx1 = (int)(x2 - x1);
    if (dx1 < 0) {
        dx1 = -dx1;
        signx1 = -1;
    } else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1);
    if (dx2 < 0) {
        dx2 = -dx2;
        signx2 = -1;
    } else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
        std::swap( dx1, dy1 );
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
        std::swap( dy2, dx2 );
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 != y2) {

        e1 = (int)(dx1 >> 1);

        for (int i = 0; i < dx1;) {
            t1xp = 0;
            t2xp = 0;
            if (t1x < t2x) {
                minx = t1x;
                maxx = t2x;
            } else {
                minx = t2x;
                maxx = t1x;
            }
            // process first line until y value is about to change
            while (i < dx1) {
                i++;
                e1 += dy1;
                while (e1 >= dx1) {
                    e1 -= dx1;
                    if (changed1) t1xp = signx1;//t1x += signx1;
                    else          goto next1;
                }
                if (changed1) break;
                else t1x += signx1;
            }
            // Move line
next1:
            // process second line until y value is about to change
            while (1) {
                e2 += dy2;
                while (e2 >= dx2) {
                    e2 -= dx2;
                    if (changed2) t2xp = signx2;//t2x += signx2;
                    else          goto next2;
                }
                if (changed2)     break;
                else              t2x += signx2;
            }
next2:
            if (minx > t1x) minx = t1x;
            if (minx > t2x) minx = t2x;
            if (maxx < t1x) maxx = t1x;
            if (maxx < t2x) maxx = t2x;
            plot_horizontal_line( minx, maxx, y, c );    // Draw line from min to max points found on the y
            // Now increase y
            if (!changed1) t1x += signx1;
            t1x += t1xp;
            if (!changed2) t2x += signx2;
            t2x += t2xp;
            y += 1;
            if (y == y2) break;

        }
    }

    // Second half
    dx1 = (int)(x3 - x2);
    if (dx1 < 0) {
        dx1 = -dx1;
        signx1 = -1;
    } else signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1) {   // swap values
        std::swap( dy1, dx1 );
        changed1 = true;
    } else changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++) {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x) {
            minx = t1x;
            maxx = t2x;
        } else {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1) {
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) {
                    t1xp = signx1;    //t1x += signx1;
                    break;
                } else          goto next3;
            }
            if (changed1) break;
            else   	   	  t1x += signx1;
            if (i < dx1) i++;
        }
next3:
        // process second line until y value is about to change
        while (t2x != x3) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;
                else          goto next4;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
next4:

        if (minx > t1x) minx = t1x;
        if (minx > t2x) minx = t2x;
        if (maxx < t1x) maxx = t1x;
        if (maxx < t2x) maxx = t2x;
        plot_horizontal_line( minx, maxx, y, c );

        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3) return;
    }
}

// DrawCircle() and FillCircle() method =====

// Function for circle-generation, using Bresenham's algorithm
// see: https://cppsecrets.com/users/100741121141051219710912197115104485164103109971051084699111109/Bresenham-Circle-Drawing-Algorithm.php
void flc::SDL_GameEngine::DrawCircle( int xc, int yc, int r, Pixel colour ) {

    // this aux. lambda exploits the full potential of symmetry of a circle so that only
    // 1/8 of a circle points need to be calculated.
    auto copy_circle_pixels = [=]( int xc, int yc, int x, int y, Pixel colour ) {
        Draw( xc + x, yc + y, colour );
        Draw( xc - x, yc + y, colour );
        Draw( xc + x, yc - y, colour );
        Draw( xc - x, yc - y, colour );
        Draw( xc + y, yc + x, colour );
        Draw( xc - y, yc + x, colour );
        Draw( xc + y, yc - x, colour );
        Draw( xc - y, yc - x, colour );
    };

    int pk, x, y;
    pk = 3 - 2 * r;
    x = 0;
    y = r;
    copy_circle_pixels( xc, yc, x, y, colour );

    while (x < y) {
        if (pk <= 0) {
            pk = pk + (4 * x) + 6;
            copy_circle_pixels( xc, yc, ++x, y, colour );
        } else {
            pk = pk + (4 * (x - y)) + 10;
            copy_circle_pixels(xc, yc, ++x, --y, colour );
        }
    }
}

// Function for circle-generation, using Bresenham's algorithm
void flc::SDL_GameEngine::FillCircle( int xc, int yc, int r, Pixel colour ) {

    auto plot_horizontal_line = [=]( int x0, int x1, int y, Pixel colour ) -> void {
        if (x0 > x1)
            std::swap( x0, x1 );
        for (int x = x0; x <= x1; x++)
            Draw( x, y, colour );
    };

    int pk, x, y;
    pk = 3 - 2 * r;
    x = 0;
    y = r;

    while (x <= y) {
        plot_horizontal_line( xc - y, xc + y, yc - x, colour );
        if (x > 0)
            plot_horizontal_line( xc - y, xc + y, yc + x, colour );
        if (pk < 0) {
            pk = pk + (4 * x) + 6;
            x++;
        } else {
            if (x != y) {
                plot_horizontal_line( xc - x, xc + x, yc - y, colour );
                plot_horizontal_line( xc - x, xc + x, yc + y, colour );
            }
            pk = pk + (4 * (x - y)) + 10;
            x++;
            y--;
        }
    }
}

// text drawing stuff =====

// Draws a string at specified location, in specified colour and scale
void flc::SDL_GameEngine::DrawString( int x, int y, const std::string &sText, Pixel nColour, int nScale ) {
    cFont.DrawString( pEngineDrawTarget->GetSurfacePtr(), x, y, sText, nColour, nScale );
}

// like DrawString() but with variable (horizontal) character spacing
void flc::SDL_GameEngine::DrawStringProp( int x, int y, const std::string &sText, Pixel nColour, int nScale ) {
    cFont.DrawStringProp( pEngineDrawTarget->GetSurfacePtr(), x, y, sText, nColour, nScale );
}

// Select one of the available fonts
// NOTE: error checking is done by the FontSprite object.
void flc::SDL_GameEngine::SetFont( int nFontIndex ) {
    cFont.SetFont( nFontIndex );
}

// Sprite drawing stuff =====

// inspired by Lazy Foo, thanks!
uint32_t get_pixel32( SDL_Surface *surface, int x, int y ) {
    uint32_t *pixelPtr = (uint32_t *)surface->pixels;
    return pixelPtr[ (y * surface->w) + x ];
}

// these four auxiliary lambda's are used in DrawSprite(). Using this construction I can set a function pointer to the right
// pixel getter, instead of doing the same check for each pixel using a switch statement (or duplicating a lot of code)
auto p_getter1_none = []( SDL_Surface *pSrfc, int x, int y ) { return get_pixel32( pSrfc,                x,                y ); };
auto p_getter1_hor  = []( SDL_Surface *pSrfc, int x, int y ) { return get_pixel32( pSrfc, pSrfc->w - 1 - x,                y ); };
auto p_getter1_ver  = []( SDL_Surface *pSrfc, int x, int y ) { return get_pixel32( pSrfc,                x, pSrfc->h - 1 - y ); };
auto p_getter1_both = []( SDL_Surface *pSrfc, int x, int y ) { return get_pixel32( pSrfc, pSrfc->w - 1 - x, pSrfc->h - 1 - y ); };

// Draws an entire sprite at location (x, y) - the scale must be integer > 0.
// I work with a function pointer to select the right 'pixel getter' depending on the value of the flip argument. This prevents
// either having to do the same check for all pixels, or copying a lot of code.
void flc::SDL_GameEngine::DrawSprite( int x, int y, Sprite* sprite, int scale, Sprite::Flip flip ) {

    if (scale >= 1) {
        // select the correct pixel getting function from the flip mode
        uint32_t (*pixel_getter)( SDL_Surface *, int, int ) = nullptr;
        switch (flip) {
            case Sprite::NONE:  pixel_getter = p_getter1_none; break;
            case Sprite::HORIZ: pixel_getter = p_getter1_hor;  break;
            case Sprite::VERT:  pixel_getter = p_getter1_ver;  break;
            case Sprite::BOTH:  pixel_getter = p_getter1_both; break;
            default:            pixel_getter = p_getter1_none;
        }
        // grab a pointer to the sprite's SDL_Surface object
        SDL_Surface *pSrfce = sprite->GetSurfacePtr();

        // I decided to replace the call to SDL_BlitScaled with my own code, so that I could implement flipping
        // xs and ys iterate over the source rectangle
        for (int ys = 0; ys < sprite->height; ys++) {
            for (int xs = 0; xs < sprite->width; xs++) {
                // get the correct pixel using the right pixel_getter function
                uint32_t tmp_pixel = pixel_getter( pSrfce, xs, ys );
                // scale in integer numbers if so required
                for (int y_scale = 0; y_scale < scale; y_scale++) {
                    for (int x_scale = 0; x_scale < scale; x_scale++) {
                        Draw( x + (xs * scale) + x_scale, y + (ys * scale) + y_scale, tmp_pixel );
                    }
                }
            }
        }
    }
}

// these four auxiliary lambda's are used in DrawSprite(). Using this construction I can set a function pointer to the right
// pixel getter, instead of doing the same check for each pixel using a switch statement (or duplicating a lot of code)
auto p_getter2_none = []( SDL_Surface *pSrfc, int _ox, int _oy, int _w, int _h, int _xs, int _ys ) { return get_pixel32( pSrfc, _ox +           _xs , _oy +           _ys  ); };
auto p_getter2_hor  = []( SDL_Surface *pSrfc, int _ox, int _oy, int _w, int _h, int _xs, int _ys ) { return get_pixel32( pSrfc, _ox + (_w - 1 - _xs), _oy +           _ys  ); };
auto p_getter2_ver  = []( SDL_Surface *pSrfc, int _ox, int _oy, int _w, int _h, int _xs, int _ys ) { return get_pixel32( pSrfc, _ox +           _xs , _oy + (_h - 1 - _ys) ); };
auto p_getter2_both = []( SDL_Surface *pSrfc, int _ox, int _oy, int _w, int _h, int _xs, int _ys ) { return get_pixel32( pSrfc, _ox + (_w - 1 - _xs), _oy + (_h - 1 - _ys) ); };

// Draws an area of a sprite on the screen at location (x, y), where the selected area is within the specified
// sprite is (ox, oy) to (ox + w, oy + h). The scale must be integer > 0.
// I work with a function pointer to select the right 'pixel getter' depending on the value of the flip argument. This prevents
// either having to do the same check for all pixels, or copying a lot of code.
void flc::SDL_GameEngine::DrawPartialSprite( int x, int y, Sprite* sprite, int ox, int oy, int w, int h, int scale, Sprite::Flip flip ) {

    if (scale >= 1) {
        // select the correct pixel getting function from the flip mode
        uint32_t (*pixel_getter)( SDL_Surface *, int, int, int, int, int, int ) = nullptr;
        switch (flip) {
            case Sprite::NONE:  pixel_getter = p_getter2_none; break;
            case Sprite::HORIZ: pixel_getter = p_getter2_hor;  break;
            case Sprite::VERT:  pixel_getter = p_getter2_ver;  break;
            case Sprite::BOTH:  pixel_getter = p_getter2_both; break;
            default:            pixel_getter = p_getter2_none;
        }
        // grab a pointer to the sprite's SDL_Surface object
        SDL_Surface *pSrfce = sprite->GetSurfacePtr();

        // I decided to replace the call to SDL_BlitScaled with my own code, so that I could implement flipping
        // xs and ys iterate over the source rectangle
        for (int ys = 0; ys < h; ys++) {
            for (int xs = 0; xs < w; xs++) {
                // get the correct pixel using the right pixel_getter function
                uint32_t tmp_pixel = pixel_getter( pSrfce, ox, oy, w, h, xs, ys );
                // scale in integer numbers if so required
                for (int y_scale = 0; y_scale < scale; y_scale++) {
                    for (int x_scale = 0; x_scale < scale; x_scale++) {
                        Draw( x + (xs * scale) + x_scale, y + (ys * scale) + y_scale, tmp_pixel );
                    }
                }
            }
        }
    }
}

// Decal drawing stuff =====

// Draws a whole decal, with optional scale and tinting
void flc::SDL_GameEngine::DrawDecal( const flc::vf2d &pos, flc::Decal *decal, const flc::vf2d &scale, const flc::Pixel &tint ) {
    DecalFrame dec;
    dec.m_decal = decal->m_decal;
    dec.m_tint  = tint;

    // draw the whole decal - this could be a nullptr
    InitSDL_Rect( dec.m_rect_src,  0, 0, decal->m_sprite->width, decal->m_sprite->height );
    // destination rect (x, y) is screen position of upper left corner of decal
    InitSDL_Rect( dec.m_rect_dst, int( pos.x ), int( pos.y ),
                                  int( (float)dec.m_rect_src.w * scale.x ), int( (float)dec.m_rect_src.h * scale.y ));
    // no rotation, so angle is 0
    dec.m_angle_degrees = 0;
    InitSDL_Point( dec.m_point_rot, 0, 0 );

    vWindows[nActiveWindowIx]->vLayers[nEngineDrawTargetIx].vDecals.push_back( dec );
}

// Draws a region of a decal, with optional scale and tinting
void flc::SDL_GameEngine::DrawPartialDecal(
    const flc::vf2d &pos,
    flc::Decal *decal,
    const flc::vf2d &source_pos,
    const flc::vf2d &source_size,
    const flc::vf2d &scale,
    const flc::Pixel &tint
) {
    DecalFrame dec;
    dec.m_decal = decal->m_decal;
    dec.m_tint  = tint;

    // draw the part of the decal that is specified by source_pos and source_size
    InitSDL_Rect(  dec.m_rect_src, int( source_pos.x  ),
                                   int( source_pos.y  ),
                                   int( source_size.x ),
                                   int( source_size.y ));
    // destination rect (x, y) is screen position of upper left corner of decal
    InitSDL_Rect(  dec.m_rect_dst, int( pos.x ), int( pos.y ),
                                   int( (float)dec.m_rect_src.w * scale.x ), int( (float)dec.m_rect_src.h * scale.y ));
    // no rotation, so angle is 0
    dec.m_angle_degrees = 0;
    InitSDL_Point( dec.m_point_rot, 0, 0 );

    vWindows[nActiveWindowIx]->vLayers[nEngineDrawTargetIx].vDecals.push_back( dec );
}

// Draws a region of a decal, with optional scale and tinting - the scaling is specified as a size parameter
void flc::SDL_GameEngine::DrawPartialDecal(
    const flc::vf2d &pos,
    const flc::vf2d &size,
    flc::Decal *decal,
    const flc::vf2d &source_pos,
    const flc::vf2d &source_size,
    const flc::Pixel &tint
) {
    DecalFrame dec;
    dec.m_decal = decal->m_decal;
    dec.m_tint  = tint;

    // draw the part of the decal that is specified by source_pos and source_size
    InitSDL_Rect(  dec.m_rect_src, int( source_pos.x  ),
                                   int( source_pos.y  ),
                                   int( source_size.x ),
                                   int( source_size.y ));
    // destination rect (x, y) is screen position of upper left corner of decal
    InitSDL_Rect(  dec.m_rect_dst, int(  pos.x ), int(  pos.y ),
                                   int( size.x ), int( size.y ));
    // no rotation, so angle is 0
    dec.m_angle_degrees = 0;
    InitSDL_Point( dec.m_point_rot, 0, 0 );

    vWindows[nActiveWindowIx]->vLayers[nEngineDrawTargetIx].vDecals.push_back( dec );
}

// Draws a decal rotated to specified angle, with point of rotation offset
void flc::SDL_GameEngine::DrawRotatedDecal(
    const flc::vf2d &pos,
    flc::Decal *decal,
    const float fAngle,
    const flc::vf2d &center,
    const flc::vf2d &scale,
    const flc::Pixel &tint
) {
    DecalFrame dec;
    dec.m_decal = decal->m_decal;
    dec.m_tint  = tint;

    // draw the whole decal - this could be a nullptr
    InitSDL_Rect(  dec.m_rect_src, 0, 0, decal->m_sprite->width, decal->m_sprite->height );
    // destination rect (x, y) is screen position of upper left corner of decal
    InitSDL_Rect(  dec.m_rect_dst, int( pos.x - center.x * scale.x ), int( pos.y - center.y * scale.y ),
                                   int( (float)dec.m_rect_src.w * scale.x ), int( (float)dec.m_rect_src.h * scale.y ));
    // this is a rotated decal, so give the angle and the rotation center their values
    InitSDL_Point( dec.m_point_rot, int( center.x * scale.x ), int( center.y * scale.y ));
    dec.m_angle_degrees = double( fAngle ) * 360.0 / (2.0 * M_PI);

    vWindows[nActiveWindowIx]->vLayers[nEngineDrawTargetIx].vDecals.push_back( dec );
}

void flc::SDL_GameEngine::DrawPartialRotatedDecal(
    const flc::vf2d &pos,
    flc::Decal *decal,
    const float fAngle,
    const flc::vf2d &center,
    const flc::vf2d &source_pos,
    const flc::vf2d &source_size,
    const flc::vf2d &scale,
    const flc::Pixel &tint
) {
    DecalFrame dec;
    dec.m_decal = decal->m_decal;
    dec.m_tint  = tint;

    // draw the part of the decal that is specified by source_pos and source_size
    InitSDL_Rect(  dec.m_rect_src, int( source_pos.x ),
                                   int( source_pos.y ),
                                   int( source_size.x ),
                                   int( source_size.y ));
    // destination rect (x, y) is screen position of upper left corner of decal
    InitSDL_Rect(  dec.m_rect_dst, int( pos.x - center.x * scale.x ), int( pos.y - center.y * scale.y ),
                                   int( (float)dec.m_rect_src.w * scale.x ), int( (float)dec.m_rect_src.h * scale.y ));
    // this is a rotated decal, so give the angle and the rotation center their values
    InitSDL_Point( dec.m_point_rot, int( center.x * scale.x ),
                                    int( center.y * scale.y ));
    dec.m_angle_degrees = double( fAngle ) * 360.0 / (2.0 * M_PI);

    vWindows[nActiveWindowIx]->vLayers[nEngineDrawTargetIx].vDecals.push_back( dec );
}

// this is the decal version of DrawString()
void flc::SDL_GameEngine::DrawStringDecal( const flc::vf2d &pos, const std::string &sText, Pixel nColour, const flc::vf2d &scale ) {

    std::vector<std::pair<SDL_Rect, SDL_Rect>> decalDrawInfo;

    // let the character font object work out the set of decals from this text
    cFont.DrawStringDecal( pos.x, pos.y, sText, nColour, scale.x, scale.y, decalDrawInfo );

    // set colour and alpha mode for this font texture
    SDL_SetTextureColorMod( cFont.GetDecal()->m_decal, nColour.getR(), nColour.getG(), nColour.getB() );
    SDL_SetTextureAlphaMod( cFont.GetDecal()->m_decal, nColour.getA() );

    for (int i = 0; i < (int)decalDrawInfo.size(); i++) {
        SDL_Rect src = decalDrawInfo[i].first;
        SDL_Rect dst = decalDrawInfo[i].second;

        DrawPartialDecal( vf2d( dst.x, dst.y ), cFont.GetDecal(), vf2d( src.x, src.y ), vf2d( src.w, src.h ), scale, nColour );
    }
}

// this is the decal version of DrawStringProp()
void flc::SDL_GameEngine::DrawStringPropDecal( const flc::vf2d &pos, const std::string &sText, Pixel nColour, const flc::vf2d &scale ) {

    std::vector<std::pair<SDL_Rect, SDL_Rect>> decalDrawInfo;

    // let the character font object work out the set of decals from this text
    cFont.DrawStringPropDecal( pos.x, pos.y, sText, nColour, scale.x, scale.y, decalDrawInfo );

    // set colour mode for this font texture
    SDL_SetTextureColorMod( cFont.GetDecal()->m_decal, nColour.getR(), nColour.getG(), nColour.getB() );
    SDL_SetTextureAlphaMod( cFont.GetDecal()->m_decal, nColour.getA() );

    for (int i = 0; i < (int)decalDrawInfo.size(); i++) {
        SDL_Rect src = decalDrawInfo[i].first;
        SDL_Rect dst = decalDrawInfo[i].second;

        DrawPartialDecal( vf2d( dst.x, dst.y ), cFont.GetDecal(), vf2d( src.x, src.y ), vf2d( src.w, src.h ), scale, nColour );
    }
}

// Pixel mode & alpha blending stuff =====

SDL_BlendMode flc::SDL_GameEngine::TranslateBlendMode( flc::Pixel::Mode blendMode ) {
    SDL_BlendMode result = SDL_BLENDMODE_NONE;
    switch (blendMode) {
        case flc::Pixel::NORMAL: result = SDL_BLENDMODE_NONE;  break;
        case flc::Pixel::MASK:   result = SDL_BLENDMODE_NONE;  break;
        case flc::Pixel::ALPHA:  result = SDL_BLENDMODE_BLEND; break;
        case flc::Pixel::APROP:  result = SDL_BLENDMODE_BLEND; break;
        case flc::Pixel::CUSTOM: result = SDL_BLENDMODE_BLEND; break;
        default: std::cout << "ERROR: TranslateBlendMode() --> invalid blendmode parameter: " << blendMode << std::endl;
    }
    return result;
}

void flc::SDL_GameEngine::SetPixelMode( Pixel::Mode mode ) {
    m_PixelMode = mode;
//    SDL_SetSurfaceBlendMode( pDrawTarget->GetSurface(), TranslateBlendMode( m_PixelMode ));
}

// set your own custom blending function - NOTE this will set the PixelMode to CUSTOM
void flc::SDL_GameEngine::SetPixelMode( std::function<flc::Pixel( const int x, const int y, const flc::Pixel& pSource, const flc::Pixel& pDest)> blendFunc ) {
    m_PixelMode = flc::Pixel::CUSTOM;
//    SDL_SetSurfaceBlendMode( pDrawTarget->GetSurface(), TranslateBlendMode( m_PixelMode ));
    m_BlendFunc = blendFunc;
}

flc::Pixel::Mode flc::SDL_GameEngine::GetPixelMode() {
    return m_PixelMode;
}

// set a new blend factor for the screen / draw target - fBlend must be in [ 0.0f, 1.0f ]
// NOT COMPLETE - this works for Sprites (SDL_Surfaces) that are drawn with the DrawClamped() function,
//                but is IS NOT IMPLEMENTED YET for Decals (SDL_Textures). However it could be that the
//                textures copy the surface attributes when created from them... ???
void flc::SDL_GameEngine::SetPixelBlend( float fBlend ) {
    m_BlendFactor = fBlend;
}

// return the blend factor, this should be in [ 0.0f, 1.0f ]
float flc::SDL_GameEngine::GetPixelBlend() {
    return m_BlendFactor;
}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
