#ifndef SGE_WINDOW_H
#define SGE_WINDOW_H

/* SGE_Window.h - part of the SDL2-based Game Engine (SGE) v.20221204
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
 * Canvas    - is a sprite and can be drawn onto [ after SetDrawTarget() ] with primitives of the engine.
 *             The primitives (dot, line, circle, rectangle, triangle, text etc) are drawn onto the
 *             canvas in the execution order.
 * Layer     - holds exactly 1 canvas and 0 or more decals (so the decals are optional). In the rendering
 *             order all decals are rendered in the order they were drawn, and on top of the canvas (!)
 * Layering  - Layer [0] is the default layer, and always exists. Layers can be added, and all of them
 *             constitute 1 canvas and 0 or more decals. The rendering order is reversed LIFO or reversed
 *             creation order: layers are rendered in reversed order they are created.
 * Window    - can hold 1 or more layers. Layers are relative to the window they belong to. There is always
 *             at least 1 window, window [0], which is default. If that window is closed, a quit event is
 *             generated and the engine shuts down.
 * Windowing - Additional windows can be added. The rendering order is in the order the windows are
 *             created. However that's not so relevant. More relevant is whether windows are visible (shown)
 *             an/or have focus.
 *
 * The rendering cycle looks like this (pseudocode):
 *
 *     ITERATE (all windows (that are visible/shown) [ in order of creation ]) {
 *         ITERATE (all layers of current window (that are enabled for rendering) [ in reversed order of creation ]) {
 *             RENDER the sprite (canvas) of the current layer of the current window;
 *             ITERATE (all decals for current layer of current window [ in order they were drawn ])
 *                 RENDER current decal;
 *         }
 *     }
 *
 */

#include <iostream>
#include <string>

#include <SDL.h>

#include "SGE_Sprite.h"
#include "vector_types.h"

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

namespace flc {

    class SGE_Window {
    public:
        // Constructor
        SGE_Window();
        // Destructor
        ~SGE_Window();

        // Create window of specified size, including associated renderer
        bool CreateWindow(
            const std::string &sCaption,             // text in window title bar
            int widthInPixels, int heightInPixels,   // window size is expressed in logical pixels
            int pixelSizeX,    int pixelSizeY,       // logical pixel size is expressed in physical (screen) pixels
            bool bFullScreen = false,                // visibility flags - create window as full screen
            bool bResizable  = true,                 //                    allow resizing of window
            bool bVsynced    = false,                //                    forces synced rendering
            int  nRenderIx   = -1                    //                    -1 means default render driver
        );

        // get a pointer to resp. index of the windows current draw target
        flc::Sprite *GetDrawTarget() {      return pWindowDrawTarget; }
        int          GetDrawTargetIndex() { return nWindowDrawTargetIx; }

        // Get a pointer to the window
        SDL_Window *GetWindowPtr();
        // Get a pointer to the windows' renderer
        SDL_Renderer *GetRendererPtr();
        // get a pointer to the windows canvas
        flc::Sprite *GetCanvasPtr() {  return pScreenCanvas;     }
        // get a pointer to the windows render texture
        SDL_Texture *GetTexturePtr() { return vLayers[0].pRenderTexture; }

        // Let the window handle its window events
        void HandleEvent( SDL_Event& e );
        // Change the caption (text in title bar of the window)
        void UpdateCaption( const std::string &sCaption );
        // Bring back window to be visible on screen
        void Focus();
        // Delete the window including the associated renderer
        void CloseWindow();

        // Getters on window size and pixel size
        int GetPixelWidth();      // logical pixel size
        int GetPixelHeight();
        int GetWidth();           // logical window size
        int GetHeight();
        int GetPhysicalWidth();   // physical window size
        int GetPhysicalHeight();

        // queries on focus state of window wrt keyboard / mouse
        bool IsMouseFocused();
        bool IsKeybdFocused();
        bool IsFocused() { return IsKeybdFocused(); }   // for PGE compatibility

        // queries on visibility status of window
        bool IsMinimized();
        bool IsMaximized();
        bool IsShown() { return bIsShown; }

        // queries on mouse position within this window
        int GetMouseX() {          return m_MouseX_logical;  }
        int GetMouseY() {          return m_MouseY_logical;  }
        int GetMouseX_physical() { return m_MouseX_physical; }
        int GetMouseY_physical() { return m_MouseY_physical; }

        // internal method: The mouse position is updated per frame in the game loop, but it's only
        // updated for the window that has mouse focus
        void SetMouseCoordinates( int nLogicalX, int nLogicalY, int nPhysicalX, int nPhysicalY );

    public:
        int nWinID = -1;      // will be the index in the std::vector of windows

    private:
        int nSDLWinID = -1;   // SDL windows ID, needed for event handling

        // Internal pointers to SDL_Window and SDL_Renderer objects
        SDL_Window   *m_Window   = nullptr;
        SDL_Renderer *m_Renderer = nullptr;

        // the "canvas" part of the window
        flc::Sprite  *pScreenCanvas       = nullptr;
        flc::Sprite  *pWindowDrawTarget   = nullptr;   // Always points to the current draw target - for this window
        int           nWindowDrawTargetIx = 0;         // Always set to the current layer of drawing (for Decal Drawing)

        // size
        int m_PixelSizeX     = 0;      // logical pixel size (expressed in physical (= screen) pixels
        int m_PixelSizeY     = 0;
        int m_WidthLogical   = 0;      // logical window size (expressed in logical pixels)
        int m_HeightLogical  = 0;
        int m_WidthPhysical  = 0;      // phyiscial window size (expressed in physical (= screen) pixels
        int m_HeightPhysical = 0;

        // flags for focus state of window
        bool bHasMouseFocus = false;
        bool bHasKeybdFocus = false;

        // flags for visibility state of window
        bool bIsMaximized = false;
        bool bIsMinimized = false;
        bool bIsShown     = false;

        int m_MouseX_physical;    // mouse position in physical pixels (refreshed each frame)
        int m_MouseY_physical;
        int m_MouseX_logical;     // same in logical pixels
        int m_MouseY_logical;

    private:
        // Each window has at least 1 layer (the default layer with index 0), possibly more.
        struct sLayer {
            int nID = 0;    // layer 0 is default, counting up (1, 2, 3...)

            flc::vf2d vOffset = { 0.0f, 0.0f };      // determine HOW layer is rendered
            flc::vf2d vScale  = { 1.0f, 1.0f };
            flc::Pixel tint   = flc::WHITE;

            bool bEnabled = false;                   // determine WHETHER layer is rendered
            bool bDirty   = false;

            flc::Sprite *pLayerCanvas   = nullptr;   // each layer contains a canvas (= implemented as a sprite)
            SDL_Texture *pRenderTexture = nullptr;   // the canvas and all decals are converted into an SDL_Texture in the render cycle

            std::vector<DecalFrame> vDecals;         // to hold all the decals that are drawn to this layer
        };

    public:
        // layers container
        std::vector<sLayer> vLayers;

        // Layer targeting functions - local to "this" window: layers can be made the draw target (i.e. their canvas sprite will be
        // the draw target), they can be enabled / disabled for display, they can get an offset, a scale or a tint.
        void SetDrawTarget(    uint8_t layer );
        void EnableLayer(      uint8_t layer, bool bEnable = true );
        void SetLayerOffset(   uint8_t layer, float x, float y );
        void SetLayerScale(    uint8_t layer, float x, float y );
        void SetLayerScaleInv( uint8_t layer, float x, float y );
        void SetLayerTint(     uint8_t layer, const flc::Pixel &tint );
        // for compatibility with PGE
        void SetLayerOffset(   uint8_t layer, const flc::vf2d &offset ) { SetLayerOffset(   layer, offset.x, offset.y ); }
        void SetLayerScale(    uint8_t layer, const flc::vf2d &scale  ) { SetLayerScale(    layer,  scale.x,  scale.y ); }
        void SetLayerScaleInv( uint8_t layer, const flc::vf2d &scale  ) { SetLayerScaleInv( layer,  scale.x,  scale.y ); }

        // creates an additional layer, puts it in the vLayers container and returns its index
        int CreateLayer();

    private:
        // these functions are only for internal use: For the first (the default) layer (per window) some additional initialisation
        // and action is required. Therefore the first layer is created when the window is created, by calling this method.
        int CreateScreenLayer( flc::Sprite *pScreenSprite, SDL_Texture *pScreenTexture );
        // disposes all dynamically created layer objects. Internal method, is called by CloseWindow.
        void ClearLayers();
    };

} // end of namespace flc

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // SGE_WINDOW_H
