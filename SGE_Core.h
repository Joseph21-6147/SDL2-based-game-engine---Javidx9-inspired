#ifndef SGE_CORE_H
#define SGE_CORE_H

/* SGE_Core.h - part of the SDL2-based Game Engine (SGE) v.20221204
 * ================================================================
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
 * The core of the engine consists of two methods (Construct() and Start()) and three overridables (OnUserCreate(),
 * OnUserUpdate(), OnUserDestroy().
 * In addition there are some test functions to output SDL/Rendering/Pixelformat detail info, some methods on the
 * timing of the engine, as well as some methods to manipulate th drawtarget. Furthermore there are methods on layer
 * manipulation and on adding and activating a window.
 *
 * Core methods
 * ============
 *     Construct()
 *         * Sets global variables wrt pixelformat used throughout the engine;
 *         * Initializes SDL environment, SDL image support and SDL audio support;
 *         * Creates (and opens) main window for the engine;
 *     Start()
 *         * Activates default window, initializes default font sprite, sets default pixelmode;
 *         * Initializes keyboard and mouse state buffers;
 *         * performs user initialisation by calling OnUserCreate();
 *         * Sets up timer and profiler;
 *         * Starts game loop, with in it:
 *              - polling and update of mouse and keyboard state
 *              - timing
 *              - user frame updates by calling OnUserUpdate();
 *              - rendering [ see SGE_Window for description of the rendercycle ]
 *         * After the game loop is finished, user finalization by calling OnUserDestroy()
 *         * Closes all windows and disposes all objects associated with it
 *         * Closes SDL audio, SDL image support and SDL environment, and displays profiling output.
 *
 * Overridables (in compliance with olcPixelGameEngine)
 * ============
 *     OnUserCreate()      - is called once before the game loop is entered, and can be used to create and/or
 *                           initialize assets you need for your game.
 *     OnUserUpdate(float) - is called during the game loop, once per frame. Put all your user input, game logic
 *                           and frame drawing into this method.
 *     OnUserDestroy()     - is called once after the game loop has finished. Use this method to finalize all
 *                           user created stuff.
 */

#include   <iostream>                 // C++ libraries
#include     <vector>
#include <functional>

#include       <SDL.h>                // SDL libraries
#include <SDL_image.h>
#include <SDL_mixer.h>


#include  "SGE_Utilities.h"           // application dependencies
#include      "SGE_Pixel.h"
#include     "SGE_Sprite.h"
#include "SGE_Periferals.h"
#include       "SGE_Draw.h"
#include      "SGE_Sound.h"
#include     "SGE_Window.h"
#include      "SGE_Timer.h"

//                               +-----------+                               //
// ------------------------------+ CONSTANTS +------------------------------ //
//                               +-----------+                               //

#define DIAG_TITLE1   "SDL Game Engine (SGE) v.20221204 - by Joseph21"
#define DIAG_TITLE2   "=============================================="

#define DIAG_OUTPUT          true        // do you want the engine to produce diagnostic output in console?
#define PROF_OUTPUT          true        // same for profiler output (note: DIAG_OUTPUT must be true as well)

#define DEFAULT_RNDRR        1           // default renderer index
#define WIN_RESIZABLE        false       // are windows created as resizable or not?

namespace flc {

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

    class SDL_GameEngine {
        public:
            SDL_GameEngine();
            virtual ~SDL_GameEngine();

            // ========== SGE_Core methods ====================

            // engine core methods (see header comment)
            bool Construct(
                int nLogicalWinX, int nLogicalWinY,     // initial engine window logical window size (expressed in logical pixels)
                int nLogicalPixX, int nLogicalPixY,     // initial engine window logical pixel size (expressed in physical  pixels)
                bool bFullScreen = false,
                bool bVsynced = false
            );
            void Start();

            // user game control methods - all virtual, must be overridden by the user (see header comment)
            virtual bool OnUserCreate();
            virtual bool OnUserUpdate( float fElapsedTime );
            virtual bool OnUserDestroy();

            // methods on frame timing
            int GetFPS();                   // last frame accurate fps
            int GetFPS_mean();              // mean value over 0.5 seconds

            float GetElapsedTime();         // last frame accurate elapsed time
            float GetElapsedTime_mean();    // mean value over 0.5 seconds

            // ========== SGE_Draw methods ====================

            // screen - size interrogation and cleaning
            int ScreenWidth();
            int ScreenHeight();
            void Clear( Pixel colour = BLACK );

            // draw a single pixel in the specified colour
            void Draw( int x, int y, Pixel colour = WHITE );
            void Draw( int x, int y, uint32_t encodedCol );
            void Draw( const flc::vi2d &pos, Pixel colour = WHITE ) {
                Draw( pos.x, pos.y, colour );
            }
            void Draw( const flc::vi2d &pos, uint32_t encodedCol ) {
                Draw( pos.x, pos.y, encodedCol );
            }

            // draw a line from (x0, y0) to (x1, y1) in the specified colour and pattern
            void DrawLine( int x0, int y0, int x1, int y1, Pixel colour = WHITE, uint32_t linePattern = 0xFFFFFFFF );
            void DrawLine( const flc::vi2d &p1, const flc::vi2d &p2, Pixel colour = flc::WHITE, uint32_t linePattern = 0xFFFFFFFF ) {
                DrawLine( p1.x, p1.y, p2.x, p2.y, colour, linePattern );
            }

            // draw resp. fill a rectangle in the specified colour
            void DrawRect( int x, int y, int w, int h, Pixel colour = flc::WHITE );
            void FillRect( int x, int y, int w, int h, Pixel colour = flc::WHITE );
            void DrawRect( const flc::vi2d &pos, const flc::vi2d &size, Pixel colour = flc::WHITE ) {
                DrawRect( pos.x, pos.y, size.x, size.y, colour );
            }
            void FillRect( const flc::vi2d &pos, const flc::vi2d &size, Pixel colour ) {
                FillRect( pos.x, pos.y, size.x, size.y, colour );
            }

            // draw resp. fill a triangle in the specified colour
            void DrawTriangle( int x0, int y0, int x1, int y1, int x2, int y2, Pixel colour = flc::WHITE );
            void FillTriangle( int x0, int y0, int x1, int y1, int x2, int y2, Pixel colour = flc::WHITE );
            void DrawTriangle( flc::vi2d &p0, flc::vi2d &p1, flc::vi2d &p2, Pixel colour = flc::WHITE ) {
                DrawTriangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, colour );
            }
            void FillTriangle( flc::vi2d &p0, flc::vi2d &p1, flc::vi2d &p2, Pixel colour = flc::WHITE ) {
                FillTriangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, colour );
            }

            // draw a circle in the specified colour
            void DrawCircle( int   xc, int   yc, int   r, Pixel colour = flc::WHITE );
//            void DrawCircle( float xc, float yc, float r, Pixel colour = flc::WHITE ) { DrawCircle( int(xc), int(yc), int(r), colour ); }
            void DrawCircle( flc::vi2d c, int r, Pixel colour ) {
                DrawCircle( c.x, c.y, r, colour );
            }

            // fill a circle in the specified colour
            void FillCircle( int   xc, int   yc, int   r, Pixel colour = flc::WHITE );
            void FillCircle( float xc, float yc, float r, Pixel colour = flc::WHITE ) { FillCircle( int(xc), int(yc), int(r), colour ); }
            void FillCircle( flc::vi2d c, int r, Pixel colour ) {
                FillCircle( c.x, c.y, r, colour );
            }

            // draw a string in the specified colour with the specified scale
            void DrawString( int x, int y, const std::string &sText, Pixel nColour = WHITE, int nScale = 1 );
            void DrawString( const flc::vi2d &pos, const std::string &sText, Pixel nColour = WHITE, int nScale = 1 ) {
                DrawString( pos.x, pos.y, sText, nColour, nScale );
            }
            // like DrawString() but with variable (horizontal) character spacing
            void DrawStringProp( int x, int y, const std::string &sText, Pixel nColour = WHITE, int nScale = 1 );
            void DrawStringProp( const flc::vi2d &pos, const std::string &sText, Pixel nColour = WHITE, int nScale = 1 ) {
                DrawStringProp( pos.x, pos.y, sText, nColour, nScale );
            }

            // select one of the available fonts
            void SetFont( int nFontIndex = 0 );
            // Grabs a pointer to the font sprite
            flc::Sprite* GetFontSprite() { return cFont.GetSprite(); }

            // Draws a sprite to the draw target at location (x, y)
            void DrawSprite( int x, int y, Sprite* sprite, int scale = 1, Sprite::Flip flip = Sprite::NONE );
            void DrawSprite( vi2d pos,     Sprite* sprite, int scale = 1, Sprite::Flip flip = Sprite::NONE ) {
                DrawSprite( pos.x, pos.y, sprite, scale, flip );
            }
            // Draws an area of a sprite at location (x, y), where the selected area is (ox, oy) to (ox + w, oy + h)
            void DrawPartialSprite( int x, int y, Sprite* sprite, int ox, int oy, int w, int h, int scale = 1, Sprite::Flip flip = Sprite::NONE );
            void DrawPartialSprite( const flc::vi2d &pos, Sprite* sprite, const flc::vi2d &sourcepos, const flc::vi2d &size, int scale = 1, Sprite::Flip flip = Sprite::NONE ) {
                DrawPartialSprite( pos.x, pos.y, sprite, sourcepos.x, sourcepos.y, size.x, size.y, scale, flip );
            }

            // Draws a decal to the draw target at location (x, y), with optional scale and tinting
            void DrawDecal( const vf2d &pos, Decal *decal, const vf2d &scale = { 1.0f,1.0f }, const Pixel &tint = WHITE );
            // Draws an area of a decal, with optional scaling and tinting
            void DrawPartialDecal( const vf2d &pos, Decal *decal, const vf2d &src_pos, const vf2d &src_size, const vf2d &scale = { 1.0f,1.0f }, const Pixel &tint = WHITE );
            // same function - but now the scaling is done by specifying a size parameter instead of a scale parameter
            // [ NOTE that the parameter ordering had to be altered, otherwise the signatures would be identical, and the compiler can't see the difference ]
            void DrawPartialDecal( const vf2d &pos, const vf2d &size, Decal *decal, const vf2d &src_pos, const vf2d &src_size, const Pixel &tint = WHITE );
            // Draws a decal rotated to the specified angle, with point of rotation offset
            void DrawRotatedDecal( const vf2d &pos, Decal *decal, const float fAngle, const vf2d &center = { 0.0f, 0.0f }, const vf2d &scale = { 1.0f,1.0f }, const Pixel &tint = WHITE );
            // same function - only an area of the decal is drawn
            void DrawPartialRotatedDecal( const vf2d &pos, Decal *decal, const float fAngle, const vf2d &center, const vf2d &src_pos, const vf2d &src_size, const vf2d &scale = { 1.0f, 1.0f }, const Pixel &tint = WHITE );

            // DrawString() and DrawStringProp() are based on Sprites - these are the Decal versions
            void DrawStringDecal(     const flc::vf2d &pos, const std::string &sText, Pixel col = flc::WHITE, const flc::vf2d &scale = { 1.0f, 1.0f } );
            void DrawStringPropDecal( const flc::vf2d &pos, const std::string &sText, Pixel col = flc::WHITE, const flc::vf2d &scale = { 1.0f, 1.0f } );

            // Alpha blending and pixel modes

            // Pixel mode can be one of
            //    * NORMAL - no masking or alpha blending
            //    * MASK   - if pixels A component < 255, then don't draw pixel
            //    * ALPHA  - blend pixel from source sprite with destination sprite
            //    * APROP  - as ALPHA, but with propagation of the alpha value to the destination sprite
            //    * CUSTOM - you can pass your own alpha blending function
            void SetPixelMode( Pixel::Mode mode );
            Pixel::Mode GetPixelMode();
            // set your own custom blending function - NOTE this will set the PixelMode to CUSTOM
            void SetPixelMode( std::function<flc::Pixel( const int x, const int y, const flc::Pixel& pSource, const flc::Pixel& pDest)> pixelMode );
            // set a new blend factor - fBlend must be in [ 0.0f, 1.0f ]
            void  SetPixelBlend( float fBlend );
            float GetPixelBlend();

            // ========== SGE_periferals (I/O) methods) ====================

            // Returns true if active window has keyboard or mouse focus
            bool IsFocused() const;
            bool IsKeybdFocused() const;
            bool IsMouseFocused() const;

            // queries for keyboard and mouse state
            KeyState GetKey( Key eKeyIndex );
            KeyState MouseKey( int nKeyIndex );
            KeyState GetMouse( int nKeyIndex ) { return MouseKey( nKeyIndex ); }  // for compatibility with PGE

            // queries for mouse position
            int MouseX();
            int MouseY();
            int GetMouseX() { return MouseX(); }   // for compatibility with PGE
            int GetMouseY() { return MouseY(); }
            const vi2d &GetMousePos();
            const vi2d &GetWindowMouse();
            // Returns 0 if mouse wheel not touched, returns < 0 or > 0 if mouse wheel scrolled down resp. up
            int GetMouseWheel();

            // cursor setting and testing - this is central (i.e. for all windows the same)
            void SetCursorOff();
            void SetCursorOn();
            bool IsCursorOn();

        private:
            // called each frame to update the state of the keyboard and mouse keys and mouse positions
            void GetUpdateKeyboardState();
            void GetUpdateMouseState();

        public:
            MuTimer cFrameTimer;
            // used to profile the execution time for the main parts of the game loop
            MuProfiler cEngineProfiler;

            // Draw target setting and getting

            // Returns sprite pointer of the current draw target
            flc::Sprite* GetDrawTarget();
            // Set the draw target to be the parameter sprite pointer
            // NOTE if you specify 0 (or nullptr), the canvas of the current active window is selected as the draw target
            void SetDrawTarget( flc::Sprite *pDrawTarget );
            // Returns width resp. height of current draw target
            int GetDrawTargetWidth();
            int GetDrawTargetHeight();

        protected:
            std::string sAppName;             // contains title bar text

        private:

            // ========== SGE_periferals (I/O) variables and private methods) ====================

            KeyState sKeybdStates[NUM_KEYBD_KEYS];       // contains states for all keyboard keys
            KeyState sMouseStates[NUM_MOUSE_BUTTONS];    // contains states for all mouse keys
            int nMouseWheel = 0;                         // contains state of mouse wheel scrolling

            int nMouseX_physical;    // mouse position in physical pixels
            int nMouseY_physical;
            vi2d vMouse_physical;

            int nMouseX_logical;     // idem in logical pixels
            int nMouseY_logical;
            vi2d vMouse_logical;

            // used at startup of engine
            void InitKeyboardState();
            void InitMouseState();
            // auxiliary setter for KeyState record
            void SetKeyState( KeyState &kState, int kVal );

            // ========== SGE_Draw private methods ====================

            // internal function, only use if x, and y are guaranteed to be within boundaries of drawable object
            inline void ClampedDraw( int x, int y, uint32_t colour, uint32_t *pixelPtr );

        private:
            // At all times during execution of the engine exactly 1 window will be active. This is kept track of by both
            // a pointer to the window object, and an index into the vWindows container
            SGE_Window   *pActiveWindow       = nullptr;   // Always points to the current window
            int           nActiveWindowIx     = 0;         // Always indexes the current active window in vWindows

            // Similarly, exactly 1 draw target will be active within the active window. This is kept track of by both
            // a pointer to the draw target, and an index into the vLayers container of the active window
            flc::Sprite  *pEngineDrawTarget   = nullptr;   // Always points to the current draw target
            int           nEngineDrawTargetIx = 0;         // Always set to the current layer of drawing (for Decal Drawing)

            // Exactly 1 font will be active (in current implementation). You can switch between fonts, but there can only
            // be one active at any time.
            SpriteFont cFont;                              // sprite font object for text drawing

        private:
            // internal class variables for frame timing
            int   m_TimingCntr      = 0;
            int   m_CurFPS          = 0;
            int   m_FPS_cumulative  = 0;
            int   m_FPS_mean        = 0;
            float m_MuSecCurElapsed        = 0.0f;
            int   m_MuSecCum = 0;
            int   m_mSec_mean       = 0;

            // internal class variables for alpha blending and pixel mode. So these are central within the engine class
            Pixel::Mode m_PixelMode = Pixel::Mode::NORMAL;
            std::function<flc::Pixel( const int x, const int y, const flc::Pixel& pSource, const flc::Pixel& pDest)> m_BlendFunc = nullptr;
            float m_BlendFactor = 1.0f;
            // translate SGE blendmode value to SDL usable constant
            SDL_BlendMode TranslateBlendMode( Pixel::Mode blendMode );

        public:
            // This container holds all the active windows (and each window will have it's own draw target)
            std::vector<SGE_Window *> vWindows;

            // Layer targeting functions - relative to active current window
            int  CreateLayer();                                                // add a layer to the active window
            void SetDrawTarget(    uint8_t layer );                            // set this layer to be the draw target
            void EnableLayer(      uint8_t layer, bool bEnable = true );       // enable/disable this layer (for rendering)
            // give the layer an offset, a scale or a tint
            void SetLayerOffset(   uint8_t layer, float x, float y );
            void SetLayerScale(    uint8_t layer, float x, float y);
            void SetLayerScaleInv( uint8_t layer, float x, float y);
            void SetLayerOffset(   uint8_t layer, const flc::vf2d &offset ) { SetLayerOffset(   layer, offset.x, offset.y ); }
            void SetLayerScale(    uint8_t layer, const flc::vf2d &scale ) {  SetLayerScale(    layer,  scale.x,  scale.y ); }
            void SetLayerScaleInv( uint8_t layer, const flc::vf2d &scale ) {  SetLayerScaleInv( layer,  scale.x,  scale.y ); }
            void SetLayerTint(     uint8_t layer, const flc::Pixel &tint );

        public:
            // Create an additional window with the specified characteristics, and add it to the vWindows container
            int  AddWindow(
                const std::string &sCaption,
                int widthInPixels, int heightInPixels,
                int pixelSizeX,    int pixelSizeY,
                bool bFullScreen = false,
                bool bResizable  = WIN_RESIZABLE,
                bool bVsynced    = false,
                int  nRenderIx   = DEFAULT_RNDRR
            );
            // Set the window with nWinID as the currently active window.
            void ActivateWindow( int nWinID );
    };

} // namespace flc

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // SGE_CORE_H
