/* SGE_Window.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
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

#include "SGE_Window.h"

//                           +------------------+                            //
// --------------------------+ CONSTRUCTORS ETC +--------------------------- //
//                           +------------------+                            //

// constructor - sets all internal and public class variables to their initial values
flc::SGE_Window::SGE_Window() {
    m_Window          = nullptr;    // init pointers
    m_Renderer        = nullptr;
    nWinID            = -1;
    nSDLWinID         = -1;
    bHasMouseFocus    = false;	    // init flags
    bHasKeybdFocus    = false;
    bIsMaximized      = false;
    bIsMinimized      = false;
    bIsShown          = false;
    m_PixelSizeX      = 0;          // init size variables
    m_PixelSizeY      = 0;
    m_WidthLogical    = 0;
    m_HeightLogical   = 0;
    m_WidthPhysical   = 0;
    m_HeightPhysical  = 0;
    m_MouseX_physical = 0;          // mouse coord's local to this window
    m_MouseY_physical = 0;
    m_MouseX_logical  = 0;
    m_MouseY_logical  = 0;

    pScreenCanvas       = nullptr;
    pWindowDrawTarget   = nullptr;
    nWindowDrawTargetIx = 0;
}

// destructor - the call to CloseWindow() will dispose all dyn. allocated objects
flc::SGE_Window::~SGE_Window() {
    CloseWindow();
}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// CreateWindow() performs the following actions:
//   1. Creates an SDL_Window object (from scratch);
//   2. Creates an SDL_Renderer, associated with the window object
//   3. Sets logical size of the renderer
//   4. Creates an empty flc::Sprite object
//   5. Creates an SDL_Surface object and associates it with the sprite object
//   6. Creates an SDL_Texture object (that is needed in the render cycle)
//   7. Creates layer[0] using both the above created Sprite and SDL_Texture
bool flc::SGE_Window::CreateWindow(
    const std::string &sCaption,
    int widthInPixels, int heightInPixels,
    int pixelSizeX   , int pixelSizeY,
    bool bFullScreen, bool bResizable, bool bVsynced, int nRenderIx )
{
    m_PixelSizeX     = pixelSizeX;
    m_PixelSizeY     = pixelSizeY;
    m_WidthLogical   = widthInPixels;
    m_HeightLogical  = heightInPixels;
    m_WidthPhysical  = m_WidthLogical  * m_PixelSizeX;
    m_HeightPhysical = m_HeightLogical * m_PixelSizeY;

    // set flags for window creation
	uint32_t win_flags = SDL_WINDOW_SHOWN;
	if (bResizable ) win_flags = win_flags | SDL_WINDOW_RESIZABLE;
	if (bFullScreen) win_flags = win_flags | SDL_WINDOW_FULLSCREEN_DESKTOP;
	// create SDL_Window object
	m_Window = SDL_CreateWindow(
        sCaption.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        m_WidthPhysical, m_HeightPhysical,
        win_flags
    );
	if (m_Window == nullptr) {
        std::cout << "ERROR: CreateWindow() --> failed call to SDL_CreateWindow(): " << SDL_GetError() << std::endl;
        return false;
	}

    // set flags for renderer creation
    uint32_t rnd_flags = SDL_RENDERER_ACCELERATED;
    if (bVsynced) rnd_flags = rnd_flags | SDL_RENDERER_PRESENTVSYNC;
    // create the renderer associated with this window
    m_Renderer = SDL_CreateRenderer( m_Window, nRenderIx, rnd_flags );
    if (m_Renderer == nullptr) {
        std::cout << "ERROR: CreateWindow() --> failed call to SDL_CreateRenderer(): " << SDL_GetError() << std::endl;
        SDL_DestroyWindow( m_Window );
        m_Window = nullptr;
        return false;
    }

    // [ This hint is from the SDL2 migration guide ]  The call to SDL_RenderSetLogicalSize() implements the
    // difference between the physical and the logical window size - and hence implements the pixel size.
    SDL_RenderSetLogicalSize( m_Renderer, m_WidthLogical, m_HeightLogical );
    // grab window identifier - needed for event handling
    nSDLWinID = SDL_GetWindowID( m_Window );
    // flag the window as open
    bIsShown = true;

    // Create a sprite that represents the screen (or actually the layer[0] canvas for this window)
    // NOTE - since some of the Sprite constructors need the pixel format for the screen sprite, and that pixel
    // format is not yet know at this point, an empty sprite and the associated SDL_Surface are created
    // explicitly here (and the pixel format is derived thereof).
    pScreenCanvas = new Sprite();
    // create a new surface for the screen sprite
    SDL_Surface *auxSurface = SDL_CreateRGBSurface(
        0,                        // unused flags, should be set to 0
        m_WidthLogical,           // width of surface
        m_HeightLogical,          // height of surface
        32,                       // depth of surface in bits (typically set to 32)
        glb_rmask,                // mask for r value
        glb_gmask,                //          g
        glb_bmask,                //          b
        glb_amask                 //          a
    );
    if (auxSurface == nullptr) {
        std::cout << "ERROR: CreateWindow() --> failed call to SDL_CreateRGBSurface(): " << SDL_GetError() << std::endl;
        return false;
    }

    pScreenCanvas->SetSurface( auxSurface );  // other (internal) variables of the pScreenCanvas sprite are also set in this call
    auxSurface = nullptr;

    // create texture to update from the pScreenCanvas each frame
    // NOTE 1 - the SDL_TEXTUREACCESS_STREAMING is mandatory in this create call!
    // NOTE 2 - the ARGB8888 pixel format appears to be faster than others... ? (see: https://gamedev.stackexchange.com/a/87770)
    SDL_Texture *auxTexture = SDL_CreateTexture(
        m_Renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        m_WidthLogical,
        m_HeightLogical
    );
    if (auxTexture == nullptr) {
        std::cout << "ERROR: CreateWindow() --> failed call to SDL_CreateTexture(): " << SDL_GetError() << std::endl;
        return false;
    }

    if (CreateScreenLayer( pScreenCanvas, auxTexture ) != 0) {
        std::cout << "ERROR: CreateWindow() --> screen layer has index != 0..." << std::endl;
        return false;
    }

    // for this window, set the draw target on the layer 0 canvas
    this->SetDrawTarget( 0 );

    return true;
}

// this method lets each window handle its own events
void flc::SGE_Window::HandleEvent( SDL_Event& event ) {

	//only process if it's a window event AND it's for this window
	if (event.type == SDL_WINDOWEVENT && event.window.windowID == (uint32_t)nSDLWinID) {
        switch (event.window.event) {
            case SDL_WINDOWEVENT_SHOWN:             // window appeared
                bIsShown = true;
                break;
            case SDL_WINDOWEVENT_HIDDEN:            // window disappeared
                bIsShown = false;
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:		// load new sizes and repaint on window size change
                m_WidthPhysical  = event.window.data1;
                m_HeightPhysical = event.window.data2;

                // if the window (physical) size is changed, the logical size must change as well, and
                // the logical size used by the renderer must be adapted
                m_WidthLogical  = m_WidthPhysical  / m_PixelSizeX;
                m_HeightLogical = m_HeightPhysical / m_PixelSizeY;
                SDL_RenderSetLogicalSize( m_Renderer, m_WidthLogical, m_HeightLogical );
                SDL_RenderPresent( m_Renderer );
                break;
            case SDL_WINDOWEVENT_EXPOSED:			// repaint on exposure
                SDL_RenderPresent( m_Renderer );
                break;
            case SDL_WINDOWEVENT_ENTER:			    // mouse entered window
                bHasMouseFocus = true;
                break;
            case SDL_WINDOWEVENT_LEAVE:			    // mouse left window
                bHasMouseFocus = false;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:		// window got keyboard focus
                bHasKeybdFocus = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:		// window lost keyboard focus
                bHasKeybdFocus = false;
                break;
            case SDL_WINDOWEVENT_MINIMIZED:			// window is minimized
                bIsMinimized = true;
                bIsMaximized = false;
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:			// window is maximized
                bIsMinimized = false;
                bIsMaximized = true;
                break;
            case SDL_WINDOWEVENT_RESTORED:			// window is restored
                bIsMinimized = false;
                bIsMaximized = false;
                break;

// When you have multiple windows, Xing out the window doesn't necessarily mean quitting the program.
// What is implemented instead is have each window hide when Xed out. So we'll need keep track of when
// the window is hidden/shown by checking for SDL_WINDOWEVENT_SHOWN/SDL_WINDOWEVENT_HIDDEN events.

// NOTE if the window[0] is hidden (Xed out), this will be interpreted as a quit event in the engine
// game loop.

            case SDL_WINDOWEVENT_CLOSE:             // hide window when x'ed out
                SDL_HideWindow( m_Window );         // ig this generates an SDL_WINDOWEVENT_HIDDEN event, so the
                break;                              // bIsShown flag need not be set here.
		}
	}
}

// This method is invoked from the game loop, and only for the window that has mouse focus.
void flc::SGE_Window::SetMouseCoordinates( int nLogicalX, int nLogicalY, int nPhysicalX, int nPhysicalY ) {
    m_MouseX_logical  = nLogicalX;
    m_MouseY_logical  = nLogicalY;
    m_MouseX_physical = nPhysicalX;
    m_MouseY_physical = nPhysicalY;
}

// make a window visible if it wasn't and put it on top of other windows
void flc::SGE_Window::Focus() {
    // restore window if needed
    if (!bIsShown) {
        SDL_ShowWindow( m_Window );
    }
    // move window on top of others
    SDL_RaiseWindow( m_Window );
}

// change the text in the title bar of the window
void flc::SGE_Window::UpdateCaption( const std::string &sCaption ) {
    SDL_SetWindowTitle( m_Window, sCaption.c_str() );
}

// close the window and dispose it, together with it's associated renderer.
void flc::SGE_Window::CloseWindow() {
    if (m_Window   != nullptr) SDL_DestroyWindow(   m_Window   );
    if (m_Renderer != nullptr) SDL_DestroyRenderer( m_Renderer );

    m_Window   = nullptr;   // reset to initial state
    m_Renderer = nullptr;

    ClearLayers();

    bHasMouseFocus = false;
    bHasKeybdFocus = false;
    bIsMaximized   = false;
    bIsMinimized   = false;
    bIsShown       = false;

    m_PixelSizeX     = 0;
    m_PixelSizeY     = 0;
    m_WidthLogical   = 0;
    m_HeightLogical  = 0;
    m_WidthPhysical  = 0;
    m_HeightPhysical = 0;

    m_MouseX_physical = 0;
    m_MouseY_physical = 0;
    m_MouseX_logical  = 0;
    m_MouseY_logical  = 0;

    pScreenCanvas       = nullptr;
    pWindowDrawTarget   = nullptr;
    nWindowDrawTargetIx = 0;
}

// getters on the pointers to the SDL window resp. Renderer objects
SDL_Window   *flc::SGE_Window::GetWindowPtr() {      return m_Window;         }
SDL_Renderer *flc::SGE_Window::GetRendererPtr() {    return m_Renderer;       }
// getters to the pixel and window sizes
int           flc::SGE_Window::GetPixelWidth() {     return m_PixelSizeX;     }
int           flc::SGE_Window::GetPixelHeight() {    return m_PixelSizeY;     }
int           flc::SGE_Window::GetWidth() {          return m_WidthLogical;   }
int           flc::SGE_Window::GetHeight() {         return m_HeightLogical;  }
int           flc::SGE_Window::GetPhysicalWidth() {  return m_WidthPhysical;  }
int           flc::SGE_Window::GetPhysicalHeight() { return m_HeightPhysical; }
// queries on the visibility characteristics of this window
bool          flc::SGE_Window::IsMouseFocused() {    return bHasMouseFocus;   }
bool          flc::SGE_Window::IsKeybdFocused() {    return bHasKeybdFocus;   }
bool          flc::SGE_Window::IsMinimized() {       return bIsMinimized;     }
bool          flc::SGE_Window::IsMaximized() {       return bIsMaximized;     }

// =============== Window layer implementation =============================================

// create a new layer and return the id of it
// NOTE: DON'T USE THIS ONE FOR LAYER 0 !!
int flc::SGE_Window::CreateLayer() {
    if (vLayers.size() == 0) std::cout << "ERROR: CreateLayer() --> screen layer is missing..." << std::endl;
    sLayer sl;
    sl.nID = (int)vLayers.size();
    sl.pLayerCanvas = new flc::Sprite( m_WidthLogical, m_HeightLogical );
    // set blendmode on the surface to retain transparency when doing TextureUpdates() with it
    SDL_SetSurfaceBlendMode( sl.pLayerCanvas->GetSurfacePtr(), SDL_BLENDMODE_BLEND );

    // in the render cycle of the game engine loop a texture is needed per layer
    // create this texture to update from the layers internal surface each frame
    sl.pRenderTexture = SDL_CreateTexture( m_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_WidthLogical, m_HeightLogical );
    // set blendmode on the texture to retain transparancy when doing RenderCopy()
    SDL_SetTextureBlendMode( sl.pRenderTexture, SDL_BLENDMODE_BLEND );

    vLayers.push_back( sl );
    return sl.nID;
}

// create a new layer 0 for this window and return the id of it
// NOTE: ONLY TO BE USED FOR LAYER 0 (SCREEN or CANVAS LAYER) CREATION !!
int flc::SGE_Window::CreateScreenLayer( flc::Sprite *pScreenSprite, SDL_Texture *pScreenTexture ) {
    if (vLayers.size() > 0) std::cout << "ERROR: CreateScreenLayer() --> there's already content in vLayer..." << std::endl;
    sLayer sl;
    sl.nID = (int)vLayers.size();
    // for layer 0 the canvas sprite and render texture are created in CreateWindow()
    sl.pLayerCanvas = pScreenSprite;
    SDL_SetSurfaceBlendMode( sl.pLayerCanvas->GetSurfacePtr(), SDL_BLENDMODE_BLEND );
    sl.pRenderTexture = pScreenTexture;
    SDL_SetTextureBlendMode( sl.pRenderTexture, SDL_BLENDMODE_BLEND );
    vLayers.push_back( sl );
    return sl.nID;
}

// each layer contains a sprite and a SDL_Texture. This function disposes these for all layers
// and clears the vLayers std::vector.
void flc::SGE_Window::ClearLayers() {
    for (auto &e : vLayers) {
        delete e.pLayerCanvas;
        e.pLayerCanvas = nullptr;
        SDL_DestroyTexture( e.pRenderTexture );
        e.pRenderTexture = nullptr;
    }
    vLayers.clear();
}

void flc::SGE_Window::SetDrawTarget( uint8_t layer ) {
    if (layer < 0 || layer >= (int)vLayers.size()) std::cout << "ERROR: SetDrawTarget() --> layer index out of range: " << layer << std::endl;
    nWindowDrawTargetIx = layer;
    pWindowDrawTarget   = vLayers[layer].pLayerCanvas;
    vLayers[layer].bDirty = true;
}

void flc::SGE_Window::EnableLayer( uint8_t layer, bool bEnable ) {
    if (layer < 0 || layer >= (int)vLayers.size()) std::cout << "ERROR: EnableLayer() --> layer index out of range: " << layer << std::endl;
    vLayers[layer].bEnabled = bEnable;
    vLayers[layer].bDirty   = bEnable;    // if a layer is enabled, it's also dirty
}

// NOTE - see the notes on SetLayerOffset() and SetLayerScale() - they don't behave as you might expect...
// "The offset must be interpreted as a normalised factor on the scaled decal width / height."

void flc::SGE_Window::SetLayerOffset( uint8_t layer, float x, float y ) {
    if (layer < 0 || layer >= (int)vLayers.size()) std::cout << "ERROR: SetLayerOffset() --> layer index out of range: " << layer << std::endl;
    vLayers[layer].vOffset.x = -x;
    vLayers[layer].vOffset.y = -y;
    vLayers[layer].bDirty = true;
}

void flc::SGE_Window::SetLayerScale( uint8_t layer, float x, float y ) {
    if (layer < 0 || layer >= (int)vLayers.size()) std::cout << "ERROR: SetLayerScale() --> layer index out of range: " << layer << std::endl;
    vLayers[layer].vScale.x = 1.0f / x;
    vLayers[layer].vScale.y = 1.0f / y;
    vLayers[layer].bDirty = true;
}

void flc::SGE_Window::SetLayerScaleInv( uint8_t layer, float x, float y ) {
    if (layer < 0 || layer >= (int)vLayers.size()) std::cout << "ERROR: SetLayerScaleInv() --> layer index out of range: " << layer << std::endl;
    vLayers[layer].vScale.x = x;
    vLayers[layer].vScale.y = y;
    vLayers[layer].bDirty = true;
}

void flc::SGE_Window::SetLayerTint( uint8_t layer, const flc::Pixel& tint ) {
    if (layer < 0 || layer >= (int)vLayers.size()) std::cout << "ERROR: SetLayerTint() --> layer index out of range: " << layer << std::endl;
    vLayers[layer].tint = tint;
    vLayers[layer].bDirty = true;
}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

