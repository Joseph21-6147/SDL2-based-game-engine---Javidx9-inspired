/* SGE_Sprite.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
 * ====================================================================
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

#include "SGE_Sprite.h"

#include  <cmath>
#include <string>

#include       <SDL.h>
#include <SDL_image.h>

#include  "SGE_FontData.h"
#include "SGE_Utilities.h"

// ==============================/ Class Sprite /==============================

//                           +------------------+                            //
// --------------------------+ CONSTRUCTORS ETC +--------------------------- //
//                           +------------------+                            //

// default constructor, creates an empty sprite object
flc::Sprite::Sprite() {
    m_SurfacePtr = nullptr;
    m_ColData    = nullptr;
    width        = 0;
    height       = 0;
}

// default destructor
flc::Sprite::~Sprite() {
    if (m_SurfacePtr != nullptr) {
        SDL_FreeSurface( m_SurfacePtr );
    }
    m_SurfacePtr = nullptr;
    m_ColData    = nullptr;
}

// Creates a sprite object based upon the file specified by sFilename.
// NOTE 1 - upon failure, IsEmpty() will be true, and width and height will be 0.
// NOTE 2 - for now the implementation is limited to sprite files that have uint32_t type pixels i.e.
//          BytesPerPixel in format is 4
flc::Sprite::Sprite( std::string sFileName ) {

    SDL_Surface *rawSurface = IMG_Load( sFileName.c_str() );
    if (rawSurface == nullptr) {
        std::cout << "WARNING: Sprite( filename ) with file: " << sFileName << " --> error loading: " << IMG_GetError() << std::endl;
        width  = 0;
        height = 0;
        m_ColData = nullptr;
    } else {

        //Convert surface to screen format to enhance performance
        m_SurfacePtr = SDL_ConvertSurface( rawSurface, glbPixelFormatPtr, 0 );
        //Get rid of old loaded surface
        SDL_FreeSurface( rawSurface );

        if (m_SurfacePtr == nullptr) {
            std::cout << "WARNING: Sprite( filename ) with file: " << sFileName << " --> couldn't optimize: " << IMG_GetError() << std::endl;
            width  = 0;
            height = 0;
            m_ColData = nullptr;
        } else {
            width  = m_SurfacePtr->w;
            height = m_SurfacePtr->h;
            if (m_SurfacePtr->format == nullptr) {
                std::cout << "ERROR: Sprite( fileName ) with file: " << sFileName << ": Can't get SDL_PixelFormat" << std::endl;
            } else if (m_SurfacePtr->format->BytesPerPixel != 4) {
                std::cout << "ERROR: Sprite( fileName ) with file: " << sFileName << ": Unexpected pixel format: " << std::to_string( m_SurfacePtr->format->BytesPerPixel ) << std::endl;
            } else {
                m_ColData = (uint32_t *)m_SurfacePtr->pixels;
            }
        }
    }
}

// create an empty sprite of the specified width and height
flc::Sprite::Sprite( int w, int h ) {

    // create a new surface as the sprite
    SDL_Surface *rawSurface = SDL_CreateRGBSurface(
                                  0,            // unused flags, should be set to 0
                                  w,            // width of surface
                                  h,            // height of surface
                                  32,           // depth of surface in bits (typically set to 32)
                                  glb_rmask,    // mask for r value
                                  glb_gmask,    //          g
                                  glb_bmask,    //          b
                                  glb_amask     //          a
                              );
    if (rawSurface == nullptr) {
        std::cout << "ERROR: Sprite( width, height ) during call to SDL_CreateRGBSurface(): " << SDL_GetError() << std::endl;
        m_ColData = nullptr;
        width  = 0;
        height = 0;
    } else {
        m_SurfacePtr = SDL_ConvertSurface( rawSurface, glbPixelFormatPtr, 0 );
        SDL_FreeSurface( rawSurface );
        rawSurface = nullptr;
        if (m_SurfacePtr == nullptr) {
            std::cout << "ERROR: Sprite( width, height ) during call to SDL_ConvertSurface(): " << SDL_GetError() << std::endl;
            m_ColData = nullptr;
            width  = 0;
            height = 0;
        } else {
            m_ColData = (uint32_t *)m_SurfacePtr->pixels;
            width  = m_SurfacePtr->w;
            height = m_SurfacePtr->h;
        }
    }
}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// returns the pixel at coordinates (x, y)
flc::Pixel flc::Sprite::GetPixel( int x, int y ) {
    // check whether coordinates are in range
    if (x < 0 || x >= width ) {
        std::cout << "WARNING: GetPixel() --> x value out of range: " << std::to_string( x ) << " (should be between 0 and " << std::to_string( width ) << ")" << std::endl;
        return flc::CYAN;
    }
    if (y < 0 || y >= height) {
        std::cout << "WARNING: GetPixel() --> y value out of range: " << std::to_string( y ) << " (should be between 0 and " << std::to_string( height ) << ")" << std::endl;
        return flc::CYAN;
    }
    // read the pixel value as a uint32_t from the sprite data
    SDL_LockSurface( m_SurfacePtr );
    uint32_t pixelValue = m_ColData[ y * width + x ];
    SDL_UnlockSurface( m_SurfacePtr );
    // build pixel and return it
    return flc::Pixel( pixelValue );
}

// sets the pixel at coordinate (x, y) at value pix
void flc::Sprite::SetPixel( int x, int y, flc::Pixel pix ) {
    // check whether coordinates are in range
    if (x < 0 || x >= width ) {
        std::cout << "WARNING: SetPixel() --> x value out of range: " << std::to_string( x ) << " (should be between 0 and " << std::to_string( width ) << ")" << std::endl;
    } else if (y < 0 || y >= height) {
        std::cout << "WARNING: SetPixel() --> y value out of range: " << std::to_string( y ) << " (should be between 0 and " << std::to_string( height ) << ")" << std::endl;
    } else {
        // encode the pixel into a uint32_t
        uint32_t pixelValue = pix.Encode();
        // write the pixel value as a uint32_t into the sprite data
        SDL_LockSurface( m_SurfacePtr );
        m_ColData[ y * height + x ] = pixelValue;
        SDL_UnlockSurface( m_SurfacePtr );
    }
}

// samples the sprite at normalized coordinates (u, v) (which must be in [0.0f, 1.0f])
flc::Pixel flc::Sprite::Sample( float u, float v ) {
    flc::Pixel result;
    if (u < 0.0f || u > 1.0f ||
        v < 0.0f || v > 1.0f)
        result = flc::MAGENTA;
    else {
//        int tmp_x = std::min( (int)round( u * (float)width  ), width  - 1 );
//        int tmp_y = std::min( (int)round( v * (float)height ), height - 1 );
        int tmp_x = std::max( 0, std::min( (int)( u * (float)width  ), width  - 1 ));
        int tmp_y = std::max( 0, std::min( (int)( v * (float)height ), height - 1 ));
        result = GetPixel( tmp_x, tmp_y );
    }

    return result;
}

// returns true if the sprite has no valid surface
bool flc::Sprite::IsEmpty() {
    return (m_SurfacePtr == nullptr);
}

SDL_Surface *flc::Sprite::GetSurfacePtr() {
    return m_SurfacePtr;
}

// handle with care!
void flc::Sprite::SetSurface( SDL_Surface *pSurf ) {
    if (pSurf == nullptr) {
        std::cout << "SetSurface() --> can't handle nullptr argument!" << std::endl;
    } else {
        m_SurfacePtr = pSurf;
        m_ColData    = (uint32_t *)pSurf->pixels;
        width  = pSurf->w;
        height = pSurf->h;
    }
}

// create an exact duplicate of this sprite and return a pointer to it
flc::Sprite* flc::Sprite::Duplicate() {

    flc::Sprite* pSpriteCopy = new flc::Sprite( width, height );
    SDL_BlitSurface( m_SurfacePtr, nullptr, pSpriteCopy->m_SurfacePtr, nullptr );

    return pSpriteCopy;
}

// create a sprite from a specified part of this sprite
flc::Sprite* flc::Sprite::Duplicate( flc::vi2d &vPos, flc::vi2d &vSize ) {

    flc::Sprite* pSpriteCopy = new flc::Sprite( vSize.x, vSize.y );
//    for (int y = 0; y < vSize.y; y++)
//        for (int x = 0; x < vSize.x; x++)
//            pSpriteCopy->SetPixel( x, y, GetPixel( vPos.x + x, vPos.y + y ));
    SDL_Rect tmp;
    InitSDL_Rect( tmp, vPos.x, vPos.y, vSize.x, vSize.y );
    SDL_BlitSurface( m_SurfacePtr, &tmp, pSpriteCopy->m_SurfacePtr, nullptr );

    return pSpriteCopy;
}

// ==============================/ Class Decal /==============================

//                           +------------------+                            //
// --------------------------+ CONSTRUCTORS ETC +--------------------------- //
//                           +------------------+                            //

// NOTE: in this implementation bFilter and bClamp are ignored
// spritePtr can't be nullptr
flc::Decal::Decal( flc::Sprite *spritePtr, bool bFilter, bool bClamp ) {
    if (spritePtr == nullptr) {
        std::cout << "ERROR: Decal() --> can't construct with a nullptr sprite pointer argument " << std::endl;
        m_sprite = nullptr;
        m_decal  = nullptr;
    } else {
        m_sprite = spritePtr;

        // color keying for transparency on the decal
        SDL_Surface *spriteSurface = m_sprite->GetSurfacePtr();
        SDL_SetColorKey( spriteSurface, SDL_TRUE, SDL_MapRGBA(spriteSurface->format, 0x00, 0x00, 0x00, 0x00 ));    // Hardcoded color keying on flc::BLANK
        m_decal  = SDL_CreateTextureFromSurface( glbRendererPtr, spriteSurface );
        if (m_decal == nullptr) {
            std::cout << "ERROR: Decal() --> failure in SDL_CreateTextureFromSurface(): " << SDL_GetError() << std::endl;
        }
    }
}

// NOTE: the associated sprite is not disposed by this destructor
flc::Decal::~Decal() {
    if (m_decal != nullptr) {
        SDL_DestroyTexture( m_decal );
        m_decal = nullptr;
    }
    m_sprite = nullptr;    // disconnect association with the sprite
}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// the width and height of a decal are given by the width and height of the
// associated sprite
int flc::Decal::GetWidth()  { return m_sprite->width;  }
int flc::Decal::GetHeight() { return m_sprite->height; }

flc::Decal::Decal(const uint32_t nExistingTextureResource, flc::Sprite* spr) {
    // no implementation (yet)
}

void flc::Decal::Update() {
    // NOTE: JavidX9 performs some actions on the scaling of the decal before Updating the sprite...?
    // this is not implemented yet - so Update() is identical to UpdateSprite()
    UpdateSprite();
}

void flc::Decal::UpdateSprite() {
    SDL_Surface *spriteSurface = m_sprite->GetSurfacePtr();
    SDL_UpdateTexture( m_decal, nullptr, spriteSurface->pixels, spriteSurface->pitch );
}

// ==============================/ Class SpriteFont /==============================

//                           +------------------+                            //
// --------------------------+ CONSTRUCTORS ETC +--------------------------- //
//                           +------------------+                            //

// default constructor
flc::SpriteFont::SpriteFont() {
    fontSprite = nullptr;
    fontDecal  = nullptr;
}

// default destructor
flc::SpriteFont::~SpriteFont() {
    delete fontSprite;
    delete fontDecal ;
    fontSprite = nullptr;
    fontDecal  = nullptr;
    vMargins.clear();
}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// select the sprite font indexed by the input parameter. If the index is out of range
// of [ 0, NR_OF_SPRITE_FONTS ] the default index 0 will be used
void flc::SpriteFont::SetFont( int index ) {

    if (index < 0 || index >= NR_OF_SPRITE_FONTS) {
        std::cout << "WARNING: SpriteFont( i ) - index out of range: " << index << ", using default font" << std::endl;
        index = 0;
    }

    // this function takes the index as input, and sets valid values for it's other arguments.
    // the font data is initialized from code and passed back in sFontDataString
    std::string sFontDataString;
    InitFontSprite( index, fontSpriteFile, nTilesX, nTilesY, nTileSizeX, nTileSizeY, nOffset, sFontDataString );
    // set the sprite sheet size, using the number of tiles * the tile size
    nFontSpriteSizeX = nTilesX * nTileSizeX;
    nFontSpriteSizeY = nTilesY * nTileSizeY;
    // convert the data string into a sprite, and dynamically calculate the margins for it.
    LoadFontFromDataString( nFontSpriteSizeX, nFontSpriteSizeY, sFontDataString );
    InitFontSpriteMargins();
}

// loads the font sprite file that is specified by sFileName, and makes it accessible
// via fontSprite. Returns true upon success, false otherwise.
bool flc::SpriteFont::LoadFontFromFile( std::string sFileName ) {
    if (fontSprite != nullptr) {
        delete fontSprite;
        fontSprite = nullptr;
    }
    if (fontDecal != nullptr) {
        delete fontDecal;
        fontDecal = nullptr;
    }
    fontSprite = new Sprite( sFileName );
    fontDecal  = new Decal( fontSprite );
    return (fontSprite->width > 0 && fontSprite->height > 0);
}

// for the complete sprite sheet the margins are determined per character.
// A margin is the maximum number of rows resp. columns that can be removed from the left, right, top or bottom
// without actually touching the character (i.e. containing only blank pixels).
void flc::SpriteFont::InitFontSpriteMargins() {

    // clear margins vector
    vMargins.clear();
    // get a pointer to the sprite data
    SDL_Surface *pSrfce = fontSprite->GetSurfacePtr();
    uint32_t *sprite = (uint32_t *)pSrfce->pixels;
    // process all tiles in the sprite and insert the margins per tile in the vSpacings vector
    int max_j = nTilesX * nTilesY;
    SDL_LockSurface( pSrfce );
    for (int j = 0; j < max_j; j++) {
        // get tile coordinates of current character tile
        int tx = j % nTilesX;
        int ty = j / nTilesX;
        // get pixel coordinates (origin = upper left corner) of character tile
        int px = tx * nTileSizeX;
        int py = ty * nTileSizeY;

        CharSpacing tmpSpacing;

        // process left side
        int x_left = 0;
        bool bFoundNonBlank = false;
        while (!bFoundNonBlank && x_left < nTileSizeX) {
            for (int yc = 0; yc < nTileSizeY; yc++) {
                // get the current pixel in uint32_t form
                uint32_t aux = sprite[ (py + yc) * fontSprite->width + (px + x_left) ];
                // check if it's blank or not
                bFoundNonBlank |= (aux != (uint32_t)0);
            }
            if (!bFoundNonBlank)   // if current column was completely blank
                x_left += 1;       // scan next column
        }
        tmpSpacing.lft = x_left;

        // process right side
        int x_right = nTileSizeX - 1;
        bFoundNonBlank = false;
        while (!bFoundNonBlank && x_right >= 0) {
            for (int yc = 0; yc < nTileSizeY; yc++) {
                uint32_t aux = sprite[ (py + yc) * fontSprite->width + (px + x_right) ];
                bFoundNonBlank |= (aux != (uint32_t)0);
            }
            if (!bFoundNonBlank)
                x_right -= 1;
        }
        tmpSpacing.rgt = nTileSizeX - 1 - x_right;

        // process top side
        int y_top = 0;
        bFoundNonBlank = false;
        while (!bFoundNonBlank && y_top < nTileSizeY) {
            for (int xc = 0; xc < nTileSizeX; xc++) {
                uint32_t aux = sprite[ (py + y_top) * fontSprite->width + (px + xc) ];
                bFoundNonBlank |= (aux != (uint32_t)0);
            }
            if (!bFoundNonBlank)
                y_top += 1;
        }
        tmpSpacing.top = y_top;

        // process bottom side
        int y_bottom = nTileSizeY - 1;
        bFoundNonBlank = false;
        while (!bFoundNonBlank && y_bottom >= 0) {
            for (int xc = 0; xc < nTileSizeX; xc++) {
                uint32_t aux = sprite[ (py + y_bottom) * fontSprite->width + (px + xc) ];
                bFoundNonBlank |= (aux != (uint32_t)0);
            }
            if (!bFoundNonBlank)
                y_bottom -= 1;
        }
        tmpSpacing.bot = nTileSizeY - 1 - y_bottom;

        vMargins.push_back( tmpSpacing );
    }
    SDL_UnlockSurface( pSrfce );

    // determine the width of the slimmest character and use that as nominal margin
    int nCharIx = (int)'|' - nOffset;
    nNominalMargin = nTileSizeX - (vMargins[nCharIx].lft + vMargins[nCharIx].rgt);
}

// Loads a font from sDataString, using the inverse algorithm that was used to write sprite file data to text.
// The sDataString data is provided from module SGE_FontData
void flc::SpriteFont::LoadFontFromDataString( int nSpriteSizeX, int nSpriteSizeY, std::string &sDataString ) {

    // if the fontPtr already contained some SDL_Texture, get rid of it.
    if (fontSprite != nullptr) {
        delete fontSprite;
        fontSprite = nullptr;
    }
    if (fontDecal != nullptr) {
        delete fontDecal;
        fontDecal = nullptr;
    }

    fontSprite = new Sprite( nSpriteSizeX, nSpriteSizeY );
    if (fontSprite == nullptr) {
        std::cout << "ERROR: LoadFontFromDataString() --> allocation/creation of font sprite failed" << std::endl;
    } else if (fontSprite->GetSurfacePtr() == nullptr) {
        std::cout << "ERROR: LoadFontFromDataString() --> can't get font sprite SDL_Surface pointer " << std::endl;
    } else if (fontSprite->GetSurfacePtr()->pixels == nullptr) {
        std::cout << "ERROR: LoadFontFromDataString() --> SDL_Surface has nullptr pixel pointer " << std::endl;
    }

    // The data string uses only 6 bits to prevent problem with difficult printable characters. The offset2Printable of 48
    // in combination with the useBits of 6 makes sure that all data is written in non complex printable
    // character subset of '0' (48) upto and including 'o' (48+64=112). As a consequence 2/8th
    // of each byte is spilled.
    const int useBits  = 6;
    const int offset2Printable = 48;

    // get a pointer to the sprite data
    SDL_Surface *pSrfce = fontSprite->GetSurfacePtr();
    uint32_t *sprite = (uint32_t *)pSrfce->pixels;
    // set the counter for reading the data string and read initial byte
    int dataCnt = 0;
    char nextBits = (char)(sDataString[dataCnt] - offset2Printable);

    SDL_LockSurface( pSrfce );
    for (int y = 0; y < fontSprite->height; y++) {
        for (int x = 0; x < fontSprite->width; x++) {

            // calculate which bit to process
            int bitCntr = x % useBits;
            // if the bit is not set, fill sprite with blank pixel, otherwise fill sprite with white pixel
            if ((nextBits & (0x20 >> bitCntr)) == 0x00)
                sprite[ y * fontSprite->width + x ] = (uint32_t)0x00000000;    // blank
            else
                sprite[ y * fontSprite->width + x ] = (uint32_t)0xFFFFFFFF;    // white

            // check if next data byte should be fetched. This is the case if either the current data byte
            // is completely processed, or wrap around the sprite width occurs
            if (bitCntr == (useBits - 1) || (x == fontSprite->width - 1)) {
                dataCnt += 1;
                nextBits = (char)(sDataString[dataCnt] - offset2Printable);
            }
        }
    }
    SDL_UnlockSurface( pSrfce );

    // now that the font sprite is initialized from the datastring, also create a decal from it
    fontDecal = new Decal( fontSprite );
    if (fontDecal == nullptr) {
        std::cout << "ERROR: LoadFontFromDataString() --> allocation/creation of font decal failed" << std::endl;
    }
}

//                       +------------------------+                                //
// ----------------------+ STRING DRAWING METHODS +------------------------------- //
//                       +------------------------+                                //

// Draws a string at specified location to the SDL_Surface pointed at by pSrfce, in the specified colour and scale
void flc::SpriteFont::DrawString( SDL_Surface *pSrfce, int x, int y, const std::string &sText, Pixel nColour, int nScale ) {

    // set colour mode and alpha mode for this font surface
    SDL_Surface *pFontSrfce = fontSprite->GetSurfacePtr();
    SDL_SetSurfaceColorMod( pFontSrfce, nColour.getR(), nColour.getG(), nColour.getB() );
    SDL_SetSurfaceAlphaMod( pFontSrfce, nColour.getA() );

    // do you want characters displayed in normalized width & height?
    int nUseCharWidth  = nTileSizeX;    // could also be nNormX resp. nNormY
    int nUseCharHeight = nTileSizeY;

    // draw the string by rendering the right partial sprites from the font sprite sheet
    int x_offset = 0;
    int y_offset = 0;
    for (int j = 0; j < (int)sText.length(); j++) {
        // get the correct partial sprite by using the character itself as index into the sprite sheet
        // the nOffset provides for sprite sheets that don't have the '0' on position 48, and 'A' on position 65 etc.
        int nCharIx = (int)sText[j] - nOffset;

        if (sText[j] == '\n') {
            y_offset += nUseCharHeight * nScale;
            x_offset = 0;
        } else {
            // source rectangle for the desired character - get the coords. in pixel space
            int nSrcX = (nCharIx % nTilesX) * nTileSizeX;
            int nSrcY = (nCharIx / nTilesX) * nTileSizeY;
            SDL_Rect part;
            InitSDL_Rect( part, nSrcX, nSrcY, nTileSizeX, nTileSizeY );

            // dest rectangle - where to put this character on the drawtarget surface
            int nDstX = x + x_offset;
            int nDstY = y + y_offset;
            SDL_Rect pos;
            InitSDL_Rect( pos, nDstX, nDstY, nUseCharWidth * nScale, nUseCharHeight * nScale );

            // now copy the subsprite to the drawtarget surface
            SDL_BlitScaled( fontSprite->GetSurfacePtr(), &part, pSrfce, &pos );

            x_offset += nUseCharWidth * nScale;
        }
    }
}

// This method very much resembles DrawString(). However, instead of directly blitting the character partial
// sprites to the draw target, it prepares a vector of information to render the partial decals in the main
// rendering loop.
void flc::SpriteFont::DrawStringDecal( int x, int y,
                                       const std::string &sText,
                                       Pixel nColour,
                                       float scaleX, float scaleY,
                                       std::vector<std::pair<SDL_Rect, SDL_Rect>> &drawInfo ) {

    // do you want characters displayed in normalized width & height?
    int nUseCharWidth  = nTileSizeX;    // could also be nNormX resp. nNormY
    int nUseCharHeight = nTileSizeY;

    // draw the string by rendering the right partial decals from the font decal sheet
    int x_offset = 0;
    int y_offset = 0;
    for (int j = 0; j < (int)sText.length(); j++) {
        // get the correct partial decal by using the character itself as index into the decal sheet
        // the nOffset provides for sprite sheets that don't have the '0' on position 48, and 'A' on position 65 etc.
        int nCharIx = (int)sText[j] - nOffset;

        if (sText[j] == '\n') {
            y_offset += nUseCharHeight * scaleY;
            x_offset = 0;
        } else {
            // source rectangle for the desired character - get the coords. in pixel space
            int nSrcX = (nCharIx % nTilesX) * nTileSizeX;
            int nSrcY = (nCharIx / nTilesX) * nTileSizeY;
            SDL_Rect part;
            InitSDL_Rect( part, nSrcX, nSrcY, nTileSizeX, nTileSizeY );

            // dest rectangle - where to put this character on the screen
            int nDstX = x + x_offset;
            int nDstY = y + y_offset;
            SDL_Rect pos;
            InitSDL_Rect( pos, nDstX, nDstY, nUseCharWidth * scaleX, nUseCharHeight * scaleY );

            drawInfo.push_back( std::make_pair( part, pos ));

            x_offset += nUseCharWidth * scaleX;
        }
    }
}

// Draws a string at specified location to the SDL_Surface pointed at by pSrfce, in the specified colour and scale
void flc::SpriteFont::DrawStringProp( SDL_Surface *pSrfce, int x, int y, const std::string &sText, Pixel nColour, int nScale ) {

    // set colour mode and alpha mode for this font surface
    SDL_Surface *pFontSrfce = fontSprite->GetSurfacePtr();
    SDL_SetSurfaceColorMod( pFontSrfce, nColour.getR(), nColour.getG(), nColour.getB() );
    SDL_SetSurfaceAlphaMod( pFontSrfce, nColour.getA() );

    // do you want characters displayed in normalized width & height?
    int nUseCharWidth  = nTileSizeX;    // could also be nNormX resp. nNormY
    int nUseCharHeight = nTileSizeY;

    // init accumulated margins counter - keeps track of accumulated spacing corrections
    int nAccSpacings = 0;
    // draw the string by rendering the right partial sprites from the font sprite sheet
    int x_offset = 0;
    int y_offset = 0;
    for (int j = 0; j < (int)sText.length(); j++) {
        // get the correct partial sprite by using the character itself as index into the sprite sheet
        // the nOffset provides for sprite sheets that don't have the '0' on position 48, and 'A' on position 65 etc.
        int nCharIx = (int)sText[j] - nOffset;

        if (sText[j] == '\n') {
            y_offset += nUseCharHeight * nScale;
            x_offset = 0;
            nAccSpacings = 0;
        } else {

            // check if charachter is space, this must be handled differently
            bool bIsSpace = sText[j] == ' ';

            // source rectangle - get the coords. in pixel space
            int nSrcX = (nCharIx % nTilesX) * nTileSizeX;
            int nSrcY = (nCharIx / nTilesX) * nTileSizeY;
            SDL_Rect part;
            InitSDL_Rect( part, nSrcX, nSrcY, nTileSizeX, nTileSizeY );

            // account for spacing on left side of character
            if (bIsSpace) // this is a space character
                nAccSpacings += nUseCharWidth - (nNominalMargin * nSpaceChar);
            else {
                // how many space is available in the sprite sheet vs how many is desired?
                int tmpSpc = vMargins[nCharIx].lft - (nNominalMargin * nInterChSpc);
                if (tmpSpc > 0)
                    nAccSpacings += tmpSpc;
            }

            // dest rectangle - where to put this character on the draw target buffer
            int nDstX = x + x_offset - (nAccSpacings * nScale);
            int nDstY = y + y_offset;
            SDL_Rect pos;
            InitSDL_Rect( pos, nDstX, nDstY, nUseCharWidth * nScale, nUseCharHeight * nScale );
            // now copy the subsprite to the draw target surface
            SDL_BlitScaled( fontSprite->GetSurfacePtr(), &part, pSrfce, &pos );

            // account for spacing on right side of character
            if (!bIsSpace) {
                // how many space is available in the sprite sheet vs how many is desired?
                int tmpSpc = vMargins[nCharIx].rgt - (nNominalMargin * nInterChSpc);
                if (tmpSpc > 0)
                    nAccSpacings += tmpSpc;
            }

            x_offset += nUseCharWidth * nScale;
        }
    }
}

// This method very much resembles DrawStringProp(). However, instead of directly blitting the character partial
// sprites to the draw target, it prepares a vector of information to render the partial decals in the main
// rendering loop.
void flc::SpriteFont::DrawStringPropDecal( int x, int y,
                                           const std::string &sText,
                                           Pixel nColour,
                                           float scaleX, float scaleY,
                                           std::vector<std::pair<SDL_Rect, SDL_Rect>> &drawInfo ) {

    // do you want characters displayed in normalized width & height?
    int nUseCharWidth  = nTileSizeX;    // could also be nNormX resp. nNormY
    int nUseCharHeight = nTileSizeY;

    // init accumulated margins counter - keeps track of accumulated spacing corrections
    int nAccSpacings = 0;
    // draw the string by rendering the right partial sprites from the font sprite sheet
    int x_offset = 0;
    int y_offset = 0;
    for (int j = 0; j < (int)sText.length(); j++) {
        // get the correct partial sprite by using the character itself as index into the sprite sheet
        // the nOffset provides for sprite sheets that don't have the '0' on position 48, and 'A' on position 65 etc.
        int nCharIx = (int)sText[j] - nOffset;

        if (sText[j] == '\n') {
            y_offset += nUseCharHeight * scaleY;
            x_offset = 0;
            nAccSpacings = 0;
        } else {

            // check if charachter is space, this must be handled differently
            bool bIsSpace = sText[j] == ' ';

            // source rectangle - get the coords. in pixel space
            int nSrcX = (nCharIx % nTilesX) * nTileSizeX;
            int nSrcY = (nCharIx / nTilesX) * nTileSizeY;
            SDL_Rect part;
            InitSDL_Rect( part, nSrcX, nSrcY, nTileSizeX, nTileSizeY );

            // account for spacing on left side of character
            if (bIsSpace) // this is a space character
                nAccSpacings += nUseCharWidth - (nNominalMargin * nSpaceChar);
            else {
                // how many space is available in the sprite sheet vs how many is desired?
                int tmpSpc = vMargins[nCharIx].lft - (nNominalMargin * nInterChSpc);
                if (tmpSpc > 0)
                    nAccSpacings += tmpSpc;
            }

            // dest rectangle - where to put this character on the screen
            int nDstX = x + x_offset - (nAccSpacings * scaleX);
            int nDstY = y + y_offset;
            SDL_Rect pos;
            InitSDL_Rect( pos, nDstX, nDstY, nUseCharWidth * scaleX, nUseCharHeight * scaleY );

            // now push the drawinfo into the vector
            drawInfo.push_back( std::make_pair( part, pos ));

            // account for spacing on right side of character
            if (!bIsSpace) {
                // how many space is available in the sprite sheet vs how many is desired?
                int tmpSpc = vMargins[nCharIx].rgt - (nNominalMargin * nInterChSpc);
                if (tmpSpc > 0)
                    nAccSpacings += tmpSpc;
            }

            x_offset += nUseCharWidth * scaleX;
        }
    }
}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
