#ifndef SGE_SPRITE_H
#define SGE_SPRITE_H

/* SGE_Sprite.h - part of the SDL2-based Game Engine (SGE) v.20221204
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
 */

//                          +--------------------+                           //
// -------------------------+ MODULE DESCRIPTION +-------------------------- //
//                          +--------------------+                           //

/*
 * This module implements class Sprite, class Decal, class SpriteFont and
 * class DecalFrame:
 *   - Sprite     - a generic 2d surface like structure for drawing and rendering
 *   - SpriteFont - a specific application of font sprite files implemented as
 *                  code using datastrings.
 *   - Decal      - a generic 2d texture like structur for rendering by the GPU
 *   - DecalFrame - a structure needed for the rendering cycle in combination with Decals
 */

#include <iostream>
#include <vector>

#include "SGE_Utilities.h"
#include "SGE_Pixel.h"

namespace flc {

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

    class Sprite {
    public:
        // this enum denotes in what direction a sprite must be flipped (if at all)
        enum Flip {
            NONE  = 0,
            HORIZ,
            VERT,
            BOTH   // flip both horizontally and vertically
        };

    public:
        // default constructor, creates an empty sprite object
        Sprite();
        ~Sprite();
        // creates a sprite object loaded with the file specified by sFilename.
        // NOTE - upon failure, IsEmpty() will be true, and width and height will be 0.
        Sprite( std::string sFileName );
        // creates a sprite object with the specified width and height
        Sprite( int w, int h );

        // create an exact duplicate of this sprite and return a pointer to it
        Sprite* Duplicate();
        // create a sprite from a specified part of this sprite
        Sprite* Duplicate( flc::vi2d &pos, flc::vi2d &sze );

        // returns the pixel at sprite coordinates (x, y)
        Pixel GetPixel( int x, int y );
        Pixel GetPixel( flc::vi2d &a ) { return GetPixel( a.x, a.y ); }

        // sets the pixel at sprite coordinate (x, y) to value pix
        void SetPixel( int x, int y, Pixel pix );
        void SetPixel( flc::vi2d &a, Pixel pix ) { SetPixel( a.x, a.y, pix ); }

        // samples the sprite at normalized coordinates (u, v) (which must be in [0.0f, 1.0f])
        Pixel Sample( float u, float v );
        // returns true if the sprite has no valid surface
        bool IsEmpty();

        int width  = 0;
        int height = 0;

        SDL_Surface *GetSurfacePtr();
        void SetSurface( SDL_Surface *pSurf );

    private:
        SDL_Surface *m_SurfacePtr = nullptr;
        uint32_t    *m_ColData    = nullptr;
    };

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

    class Decal {
    public:
        // the signature of this constructor is kept compliant with PGE, however the parameters
        // filter and clamp are ignored / have no effect
        Decal( flc::Sprite* spr, bool filter = false, bool clamp = true );
        Decal( const uint32_t nExistingTextureResource, flc::Sprite* spr );
        virtual ~Decal();
        // use Update() if you want to associate a new / another sprite to the decal
        void Update();
        // use UpdateSprite() if the sprite that is already associated has changed, and you want to
        // put the changes in the GPU memory
        void UpdateSprite();
        // added functionality - the size of a Decal is identical to the size of the associated sprite
        int GetWidth();
        int GetHeight();

    public:
        flc::Sprite *m_sprite = nullptr;
        SDL_Texture *m_decal  = nullptr;
    };

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

/*
 * Options for rendering characters from partial sprites (or partial decals):
 *   1. absolute rendering   - render each character as a complete tile. Do not take margins into account.
 *   2. partial proportional - determine the minimum tile size that fits all characters from the sprite,
 *                             and apply this minimum tile size on all partial sprites equally.
 *   3. fully proportional   - take left and right margin for each individual character to determine how
 *                             that specific character is rendered.
 * At this moment (2022-04-18) I implemented options 1 and 3, but I'd like to implement option 2 as well.
 */

    struct CharSpacing {
        int lft = 0,
            rgt = 0,
            top = 0,
            bot = 0;
    };

    class SpriteFont {
    public:
        SpriteFont();
        ~SpriteFont();

        // select the sprite font indexed by the input parameter.
        // NOTE: index 0 is the default
        void SetFont( int index = 0 );
        // draw a string in the specified colour with the specified scale
        void DrawString(     SDL_Surface *screen, int x, int y, const std::string &sText, Pixel nColour = WHITE, int nScale = 1 );
        // like DrawString() but with variable = proportional (horizontal) character spacing
        void DrawStringProp( SDL_Surface *screen, int x, int y, const std::string &sText, Pixel nColour = WHITE, int nScale = 1 );

        // resembles DrawString(), but instead of blitting or copying to render, it returns info to draw the partial decals in the engine
        void DrawStringDecal(     int x, int y, const std::string &sText, Pixel nColour, float scaleX, float scaleY, std::vector<std::pair<SDL_Rect, SDL_Rect>> &drawInfo );
        // similar, but to DrawStringProp()
        void DrawStringPropDecal( int x, int y, const std::string &sText, Pixel nColour, float scaleX, float scaleY, std::vector<std::pair<SDL_Rect, SDL_Rect>> &drawInfo );

        // return a pointer to the sprite font for this object
        Sprite *GetSprite() { return fontSprite; }
        Decal  *GetDecal() {  return fontDecal;  }

    private:
        // save the name of the sprite file the sprite was loaded from (for testing/debugging)
        std::string fontSpriteFile = "un-initialized";
        // internal pointers to the sprite and decal objects.
        Sprite *fontSprite = nullptr;
        Decal  *fontDecal  = nullptr;

        int nTilesX    = -1;             // font sprite info - how much tiles horizontally
        int nTilesY    = -1;             //                  - vertically
        int nTileSizeX = -1;             //                  - horizontal size per tile in pixels
        int nTileSizeY = -1;             //                  - vertical size per tile in pixels
        int nOffset    =  0;             // not all sprites are 1 on 1 with ascii values

        int nFontSpriteSizeX;
        int nFontSpriteSizeY;

        int nNominalMargin = 1;          // this is reset per font to be 1/8 of the nTileSizeX
        int nInterChSpc    = 1;          // inter character spacing is ... times nNormSpc

    private:

        int nSpaceChar  = 4;             // the spacing for the "space" character is ... times nNormSpace

        // contains margins for all characters in the same order as the sprite
        std::vector<CharSpacing> vMargins;

    private:
        // loads the font sprite from a file that is specified by sFileName, and makes it accessible
        // via class variable fontPtr. Returns true upon success, false otherwise.
        bool LoadFontFromFile( std::string sFileName );
        // for all characters in the sprite the margins are determined per character.
        // A spacing is the maximum number of pixel rows resp. pixel columns that can be removed from the
        // left, right, top or bottom without actually touching the character (i.e. that contain only
        // blank pixels). The margins are stored in std::vector vmargins
        void InitFontSpriteMargins();
        // loads the font sprite from data that is stored as data strings in the code. Makes it accessible
        // via fontPtr.
        void LoadFontFromDataString( int nSpriteSizeX, int nSpriteSizeY, std::string &sDataString );
    };

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

	// This class represents an instance of a decal for a specific frame. Decals are redrawn each frame
	// from a vector containing info on the part of the decal to draw, the location on the screen where
	// to draw it, the angle and rotation point and a colour for tinting the decal.
	class DecalFrame {
        public:
            DecalFrame() {}
            ~DecalFrame() {}

        public:
            SDL_Texture *m_decal = nullptr;

            SDL_Rect     m_rect_src;   // this rect determines what part of the source decal is drawn
            SDL_Rect     m_rect_dst;   // the (x, y) component is the screen position of the decal origin
            // NOTE: SDL needs angle in degrees but the interface expects angles to be in radians (for PGE compatibility)
            double       m_angle_degrees = 0.0;
            SDL_Point    m_point_rot;

            flc::Pixel   m_tint = flc::WHITE;
	};

} // namespace flc

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // SGE_SPRITE_H
