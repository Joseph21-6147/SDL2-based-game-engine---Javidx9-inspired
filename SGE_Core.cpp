/* SGE_Core.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
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

#include <cmath>
#include <iostream>
#include <fstream>

#include      "SGE_Core.h"
#include  "SGE_FontData.h"

//                              +------------+                               //
// -----------------------------+ FUNCTIONS  +------------------------------ //
//                              +------------+                               //

// Test output producing functions  =====

// for testing - prints characteristics of a surface pixelformat to screen
void PrintSurfaceFormat( SDL_Surface *ps ) {
    if (ps == nullptr) {
        std::cout << "ERROR: PrintSurfaceFormat() --> can't print nullptr argument!" << std::endl;
    } else if (ps->format == nullptr) {
        std::cout << "ERROR: PrintSurfaceFormat() --> can't print nullptr format!" << std::endl;
    } else {
        SDL_PixelFormat *pf = ps->format;
        std::cout << "SURFACE PIXEL FORMAT"                                        << std::endl;
        std::cout << "    Format ID:   "   << pf->format                           << std::endl;
        std::cout << "    Format name: "   << SDL_GetPixelFormatName( pf->format ) << std::endl;
        std::cout << "    BitsPerPixel:  " << std::to_string( pf->BitsPerPixel  )  << std::endl;
        std::cout << "    BytesPerPixel: " << std::to_string( pf->BytesPerPixel )  << std::endl;

        std::cout << "    Rmask: " << to_hex( pf->Rmask  ) << " Rshift: " << to_hex( pf->Rshift ) << " Rloss: " << to_hex( pf->Rloss ) << std::endl;
        std::cout << "    Gmask: " << to_hex( pf->Gmask  ) << " Gshift: " << to_hex( pf->Gshift ) << " Gloss: " << to_hex( pf->Gloss ) << std::endl;
        std::cout << "    Bmask: " << to_hex( pf->Bmask  ) << " Bshift: " << to_hex( pf->Bshift ) << " Bloss: " << to_hex( pf->Bloss ) << std::endl;
        std::cout << "    Amask: " << to_hex( pf->Amask  ) << " Ashift: " << to_hex( pf->Ashift ) << " Aloss: " << to_hex( pf->Aloss ) << std::endl << std::endl;
    }
}

// for testing - print the values of global masks, shift and loss values in hex form to screen
void PrintGlobalMasks() {
    std::cout << "GLOBAL MASK VALUEs" << std::endl;
    std::cout << "    glb_rmask: " << to_hex( flc::glb_rmask ) << " glb_rshift: " << to_hex( flc::glb_rshift ) << " glb_rloss: " << to_hex( flc::glb_rloss ) << std::endl;
    std::cout << "    glb_gmask: " << to_hex( flc::glb_gmask ) << " glb_gshift: " << to_hex( flc::glb_gshift ) << " glb_rloss: " << to_hex( flc::glb_gloss ) << std::endl;
    std::cout << "    glb_bmask: " << to_hex( flc::glb_bmask ) << " glb_bshift: " << to_hex( flc::glb_bshift ) << " glb_rloss: " << to_hex( flc::glb_bloss ) << std::endl;
    std::cout << "    glb_amask: " << to_hex( flc::glb_amask ) << " glb_ashift: " << to_hex( flc::glb_ashift ) << " glb_rloss: " << to_hex( flc::glb_aloss ) << std::endl << std::endl;
}

// for testing - prints characteristics of a renderer to screen
void PrintRendererInfo( SDL_RendererInfo &ri ) {

    std::cout << "RENDERER INFO" << std::endl;

    std::cout << "    name:  "               << ri.name                << std::endl;
    std::cout << "    flags: "               << ri.flags               << std::endl;
    std::cout << "    num_texture_formats: " << ri.num_texture_formats << std::endl;
    for (int i = 0; i < (int)ri.num_texture_formats; i++) {
        std::cout << "        texture_formats[ " << i;
        std::cout << " ]: " << right_align( ri.texture_formats[i], 10 ) << " - name: " << SDL_GetPixelFormatName( ri.texture_formats[i] ) << std::endl;
    }
    std::cout << "    max_texture_width:  " << ri.max_texture_width  << std::endl;
    std::cout << "    max_texture_height: " << ri.max_texture_height << std::endl << std::endl;
}

// for testing - print the renderer driver characteristics of all graphics drivers that are found by SDL
void PrintGraphicsCapabilities() {
    int nNrOfDrivers = SDL_GetNumRenderDrivers();
    std::cout << "Nr of render drivers: " << nNrOfDrivers << std::endl;
    for (int i = 0; i < nNrOfDrivers; i++) {
        SDL_RendererInfo ri;
        if (SDL_GetRenderDriverInfo( i, &ri ) != 0) {
            std::cout << "couldn't get render info for index: " << i << std::endl;
        } else {
            std::cout << "render info for index: " << i << " = ";
            PrintRendererInfo( ri );
        }
    }
}

void PrintPixelFormat( std::string sTitle, uint32_t nPixelFormat ) {
    std::cout << sTitle << std::endl;
    std::cout << "    format: " << nPixelFormat << std::endl;
    std::cout << "    name:   " << SDL_GetPixelFormatName( nPixelFormat ) << std::endl << std::endl;
}

// ==============================/ Class SDL_GameEngine /==============================

//                           +------------------+                            //
// --------------------------+ CONSTRUCTORS ETC +--------------------------- //
//                           +------------------+                            //

flc::SDL_GameEngine::SDL_GameEngine() {}
flc::SDL_GameEngine::~SDL_GameEngine() {}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// Construct() method =====

// Parameters:
//   * (nWinSizeX, nWinSizeY) = logical window size
//   * (nPixSizeX, nPixSizeY) = logical pixelsize
//   *     --> so the physical window size is the logical window size * logical pixelsize !!
//   * The two bools control full screen resp. screen sync'ed rendering (at approx 60 fps)
bool flc::SDL_GameEngine::Construct( int nWinSizeX, int nWinSizeY,
                                     int nPixSizeX, int nPixSizeY,
                                     bool bFullScreen, bool bVsynced ) {

    if (DIAG_OUTPUT) {
        std::cout << "SDL Game Engine (SGE) - by Joseph21" << std::endl;
        std::cout << "===================================" << std::endl << std::endl;
        std::cout << "Construct() --> setting up global pixel format info, ";
    }

    // global rgba masks and shift values - these match the ARGB8888 pixel format
    glb_amask = 0xFF000000; glb_ashift = 24;
    glb_rmask = 0x00FF0000; glb_rshift = 16;
    glb_gmask = 0x0000FF00; glb_gshift =  8;
    glb_bmask = 0x000000FF; glb_bshift =  0;

    if (DIAG_OUTPUT) std::cout << "SDL environment, ";

     // setup SDL environment
    if (SDL_Init( SDL_INIT_EVERYTHING ) != 0) {
        std::cout << "ERROR: Construct() --> SDL_Init() failure: " << SDL_GetError() << std::endl;
        return false;
    }

    if (DIAG_OUTPUT) std::cout << "SDL image load support, ";

    // load support for the JPG and PNG image formats
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init( flags );
    if((initted & flags) != flags) {
        std::cout << "ERROR: Construct() --> IMG_Init() failure: " << IMG_GetError() << std::endl;
        return false;
    }

    if (DIAG_OUTPUT) std::cout << "done!" << std::endl;
    if (DIAG_OUTPUT) std::cout << "Construct() --> creating window (including renderer, canvas sprite, canvas texture and default layer), ";
    AddWindow( sAppName, nWinSizeX, nWinSizeY, nPixSizeX, nPixSizeY, bFullScreen, WIN_RESIZABLE, bVsynced, DEFAULT_RNDRR );

    SGE_Window *winPtr = vWindows[0];
    // class Sprite & Decal need this info. For now solved with global vars... :|
    glbRendererPtr = winPtr->GetRendererPtr();
    // class Sprite & Decal need this info. For now solved with global vars... :|
    glbPixelFormatPtr = winPtr->GetCanvasPtr()->GetSurfacePtr()->format;
    // the glb_Xmask and glb_Xshift are inited before the Surface of window[0] was created,
    // now obtain the glb_Xloss values from that surface
    glb_aloss = glbPixelFormatPtr->Aloss;
    glb_rloss = glbPixelFormatPtr->Rloss;
    glb_gloss = glbPixelFormatPtr->Gloss;
    glb_bloss = glbPixelFormatPtr->Bloss;

    if (DIAG_OUTPUT) std::cout << "done! " << std::endl;

    if (DEBUG_MODE) {
        // Output the pixel formats for the window
        uint32_t nWindowFormat = SDL_GetWindowPixelFormat( winPtr->GetWindowPtr() );
        PrintPixelFormat( "WINDOW PIXEL FORMAT ", nWindowFormat );

        // Output the pixel formats for the renderer
        SDL_RendererInfo rInfo;
        SDL_GetRendererInfo( winPtr->GetRendererPtr(), &rInfo );
        PrintRendererInfo( rInfo );

        // Output the pixel formats for the screen surface, screen texture and renderer - for testing...!!!
        PrintSurfaceFormat( winPtr->GetCanvasPtr()->GetSurfacePtr() );
        PrintGlobalMasks();

        // Output the pixel formats for the texture associated with the screen surface
        uint32_t nTextureFormat;
        SDL_QueryTexture( winPtr->GetTexturePtr(), &nTextureFormat, nullptr, nullptr, nullptr );
        PrintPixelFormat( "TEXTURE PIXEL FORMAT ", nTextureFormat );

        // this code checks and outputs the renderer indices and details
        PrintGraphicsCapabilities();
    }

    if (DIAG_OUTPUT) std::cout << "Construct() --> setting up SDL sound support, ";

    // Open audio mixer for music and sound effects
    if (Mix_OpenAudio( STD_FREQ, MIX_DEFAULT_FORMAT, NR_OF_CHANNELS, CHUNK_SIZE ) < 0) {
        std::cout << "ERROR: Construct() --> Mix_OpenAudio() failure: " << Mix_GetError() << std::endl;
        return false;
    }

    if (DIAG_OUTPUT) std::cout << "done!" << std::endl << "Construct() --> setup finished succesfully!" << std::endl << std::endl;

    return true;
}

// Start() method =====

void flc::SDL_GameEngine::Start() {

    if (DEBUG_MODE)
        debugFile.open( DEBUG_FILE_NAME );

    // set default window to active
    ActivateWindow( 0 );

    // clear keyboard and mouse state buffers
    if (DIAG_OUTPUT) std::cout << "Start()     --> init keyboard & mouse " << std::endl;
    InitKeyboardState();
    InitMouseState();

    // initialize default font sprite
    if (DIAG_OUTPUT) std::cout << "Start()     --> setting default font and pixel mode " << std::endl;
    cFont.SetFont( 0 );
    SetPixelMode( flc::Pixel::NORMAL );    // start with default pixel mode

    // do user initialisation stuff
    if (DIAG_OUTPUT) std::cout << "Start()     --> calling OnUserCreate() " << std::endl;
    bool bContinueGameLoop = OnUserCreate();

    // setup the profiler for timing the main components of the game loop
    cEngineProfiler.InitProbes(
        {
            "Update KBD/Mouse",
            "SDL event polling",
            "frame timing",
            "OnUserUpdate()",
            "Rendering - texture update",
            "Rendering - SDL_RenderCopy()",
            "Rendering - all drawn decals",
            "Rendering - SDL_RenderPresent()",
        }
    );

    // setup the frame timer - first start timing is done upon creation of FrameTimer object
    cFrameTimer.Start();
    m_TimingCntr = 0;    // variables to accumulate mean FPS and elapsed time (in mseconds)
    m_MuSecCum = 0;

    // start game loop
    if (DIAG_OUTPUT) std::cout << "Start()     --> starting game loop" << std::endl;
    while (bContinueGameLoop) {

        // KEYBOARD - get new keyboard button state vector
        GetUpdateKeyboardState();
        // MOUSE - get new mouse button states and position
        GetUpdateMouseState();
        // update windows' local mouse coordinates if the window has mouse focus
        for (int i = 0; i < (int)vWindows.size(); i++) {
            if (vWindows[i]->IsMouseFocused()) {
                vWindows[i]->SetMouseCoordinates( nMouseX_logical, nMouseY_logical, nMouseX_physical, nMouseY_physical );
            }
        }

        cEngineProfiler.Probe( 0 );  // -------------------------------------------------------------------

        // OTHER inputs - poll all other relevant events
        nMouseWheel = 0;
        SDL_Event ev;    // define an SDL event struct to poll events
        while (SDL_PollEvent( &ev )) {   // Poll until event queue is empty
            switch (ev.type) {           // do stuff depending on the event
                case SDL_QUIT:
                    bContinueGameLoop = false;
                    break;
                case SDL_WINDOWEVENT:
                    for (auto &w : vWindows) {
                        w->HandleEvent( ev );
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    nMouseWheel = ev.wheel.y;
                    break;
                default:
                    break;
            }
        }
        // if the main window is x'out, it will be hidden (= not shown)
        // Quit the game loop if that is the case
        if (!vWindows[0]->IsShown())
            bContinueGameLoop = false;

        cEngineProfiler.Probe( 1 );  // -------------------------------------------------------------------

        // TIMING - determine and display elapsed time
        if (bContinueGameLoop) {
            // get end timing for current frame and start timing for next one
            int nElapsedMuSec = cFrameTimer.StopAndStart();
            m_MuSecCum += nElapsedMuSec;
            m_TimingCntr += 1;

            m_MuSecCurElapsed = float( nElapsedMuSec );
            m_CurFPS = 1000000.0f / m_MuSecCurElapsed;

            // update FPS and microsec values in window title every 0.5 seconds
            if (m_MuSecCum >= 500000.0f) {
                float f_muSec_mean = (float)m_MuSecCum / (float)m_TimingCntr;
                m_mSec_mean = (int)f_muSec_mean;
                float f_FPS_mean  = 1000000.0f / f_muSec_mean;     // frames per second, so divide by 1 million
                m_FPS_mean = (int)f_FPS_mean;

                m_MuSecCum -= 500000.0f;
                m_TimingCntr = 0;

                vWindows[0]->UpdateCaption(
                    "flc::SGE: "  + sAppName +
                    " - FPS: "    + std::to_string( m_FPS_mean  ) +
                    ", elapsed: " + std::to_string( m_mSec_mean ) + " microsec"
                );
            }

            cEngineProfiler.Probe( 2 );  // -------------------------------------------------------------------

            // UPDATE - do the user game logic and drawing
            bContinueGameLoop = OnUserUpdate( nElapsedMuSec / 1000000.0f );

            cEngineProfiler.Probe( 3 );  // -------------------------------------------------------------------

// ====================   RENDERING CYCLE STARTS HERE   ==============================

            // iterate all windows
            for (int winIx = 0; winIx < (int)vWindows.size(); winIx++) {

                // render only if the window is visible
                if (vWindows[winIx]->IsShown()) {

                    // Layer 0 must always be drawn and is therefore always enabled and dirty
                    vWindows[winIx]->vLayers[0].bDirty   = true;
                    vWindows[winIx]->vLayers[0].bEnabled = true;

                    // clear the renderer before building up the new frame
                    SDL_SetRenderDrawColor( vWindows[winIx]->GetRendererPtr(), 0, 0, 0, 255 );
                    SDL_RenderClear( vWindows[winIx]->GetRendererPtr() );

                    // iterate all layers of this window
                    for (int layIx = (int)vWindows[winIx]->vLayers.size() - 1; layIx >= 0; layIx -= 1) {

                        // only render enabled layers
                        if (vWindows[winIx]->vLayers[layIx].bEnabled) {

                            // 1. render the layer internal surface via it's internal texture to the current windows' renderer
                            // -----------------------------------------------------------------------------------------------

                            // texture needs no update if the canvas is not "dirty" (i.e. altered since last frame)
                            if (vWindows[winIx]->vLayers[layIx].bDirty) {

                                // update the layer texture from the layer surface
                                SDL_UpdateTexture(
                                    vWindows[winIx]->vLayers[layIx].pRenderTexture, nullptr,
                                    vWindows[winIx]->vLayers[layIx].pLayerCanvas->GetSurfacePtr()->pixels,
                                    vWindows[winIx]->vLayers[layIx].pLayerCanvas->GetSurfacePtr()->pitch
                                );
                                vWindows[winIx]->vLayers[layIx].bDirty = false;
                            }

                            cEngineProfiler.Probe( 4 );  // -------------------------------------------------------------------

                            // Tinting - 1 of 2: set the color modulation for this texture, to get the tinting effect
                            SDL_SetTextureColorMod( vWindows[winIx]->vLayers[layIx].pRenderTexture, vWindows[winIx]->vLayers[layIx].tint.getR(),
                                                                                                    vWindows[winIx]->vLayers[layIx].tint.getG(),
                                                                                                    vWindows[winIx]->vLayers[layIx].tint.getB());
                            // Tinting - 2 of 2: set the alpha mode for this texture
                            SDL_SetTextureAlphaMod( vWindows[winIx]->vLayers[layIx].pRenderTexture, vWindows[winIx]->vLayers[layIx].tint.getA());
                            // render the texture using the parameters from the DecalFrame instance

                            // NOTE - the offset must be interpreted as a normalised factor on the scaled draw target size
                            float fLayerDecalWidth  = float( vWindows[winIx]->vLayers[layIx].pLayerCanvas->width  ) * vWindows[winIx]->vLayers[layIx].vScale.x;
                            float fLayerDecalHeight = float( vWindows[winIx]->vLayers[layIx].pLayerCanvas->height ) * vWindows[winIx]->vLayers[layIx].vScale.y;
                            SDL_Rect  tmpDst = {
                                0 + int( vWindows[winIx]->vLayers[layIx].vOffset.x * fLayerDecalWidth  ),        // put vOffset into dest rect for RenderCopyEx() call
                                0 + int( vWindows[winIx]->vLayers[layIx].vOffset.y * fLayerDecalHeight ),
                                int( fLayerDecalWidth  ),                                                        // put scale into dest rect for RenderCopyEx() call
                                int( fLayerDecalHeight )
                            };
                            double    tmpAngle = 0.0;    // for layer canvas rendering we don't use rotation
                            SDL_Point tmpPnt = { 0, 0 };

                            SDL_RenderCopyEx(
                                 vWindows[winIx]->GetRendererPtr(),
                                 vWindows[winIx]->vLayers[layIx].pRenderTexture,
                                 nullptr,
                                 &tmpDst,
                                 tmpAngle,
                                 &tmpPnt,
                                 SDL_FLIP_NONE
                             );

                            cEngineProfiler.Probe( 5 );  // -------------------------------------------------------------------

                            // 2. if there are decals for this layer/frame, render them on top of the layer texture
                            // ------------------------------------------------------------------------------------

                            for (auto &elt : vWindows[winIx]->vLayers[layIx].vDecals) {
                                // Tinting - 1 of 2: set the color modulation for this texture, to get the tinting effect
                                SDL_SetTextureColorMod( elt.m_decal, elt.m_tint.getR(), elt.m_tint.getG(), elt.m_tint.getB());
                                // Tinting - 2 of 2: set the alpha mode for this texture
                                SDL_SetTextureAlphaMod( elt.m_decal, elt.m_tint.getA());

                                // render the texture using the parameters from the DecalFrame instance
                                SDL_RenderCopyEx(
                                     vWindows[winIx]->GetRendererPtr(),
                                     elt.m_decal,
                                     &elt.m_rect_src,
                                     &elt.m_rect_dst,
                                     elt.m_angle_degrees,
                                     &elt.m_point_rot,
                                     SDL_FLIP_NONE
                                 );
                            }
                        }
                        // clear the decal instance vector at the end of the render cycle - regardless if it's used or not
                        // otherwise disabled layers would pile up their decals
                        vWindows[winIx]->vLayers[layIx].vDecals.clear();
                    } // iterate layers

                    cEngineProfiler.Probe( 6 );  // -------------------------------------------------------------------

                    // 3. update the altered renderer contents to the screen
                    // -----------------------------------------------------
                    SDL_RenderPresent( vWindows[winIx]->GetRendererPtr() );

                    cEngineProfiler.Probe( 7 );  // -------------------------------------------------------------------

                } else { // if window is not shown ...
                    // ... clear the decals fromt he layers anyway, to prevent piling up
                    for (int layIx = (int)vWindows[winIx]->vLayers.size() - 1; layIx >= 0; layIx -= 1) {
                        vWindows[winIx]->vLayers[layIx].vDecals.clear();
                    }
                }
            } // iterate windows
        }
    }  // game loop

    // If the game loop is finished, finalize according to user wishes...
    if (DIAG_OUTPUT) std::cout << "Start()     --> game loop finished, calling OnUserDestroy()" << std::endl;
    OnUserDestroy();

    // ... dispose all dynamically allocated objects ...
    if (DIAG_OUTPUT) std::cout << "Start()     --> shutting down..." << std::endl << std::endl;

    for (auto &w : vWindows) {
        w->CloseWindow();
        delete w;
    }
    vWindows.clear();

    pActiveWindow       = nullptr;
    nActiveWindowIx     = -1;
    pEngineDrawTarget   = nullptr;
    nEngineDrawTargetIx = -1;

    // ... and close the SDL environment
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    // print profiler statistics
    if (DIAG_OUTPUT && PROF_OUTPUT) cEngineProfiler.PrintStats( "Game Loop profiler data" );

    if (DEBUG_MODE)
        debugFile.close();
}

// Overridables: OnUserCreate(), -Update(), -Destroy() methods =====

// user must override these methods
bool flc::SDL_GameEngine::OnUserCreate() { return true; }
bool flc::SDL_GameEngine::OnUserUpdate( float fElapsedTime ) { return true; }
bool flc::SDL_GameEngine::OnUserDestroy() { return true; }

// FPS and Elapsed Time getters ==========

int flc::SDL_GameEngine::GetFPS() {      return m_CurFPS;  }   // FPS based on last elapsed time
int flc::SDL_GameEngine::GetFPS_mean() { return m_FPS_mean; }   // mean FPS over 0.5 sec

float flc::SDL_GameEngine::GetElapsedTime() {      return m_MuSecCurElapsed  / 1000.0f; }   // actual last elapsed time
float flc::SDL_GameEngine::GetElapsedTime_mean() { return m_mSec_mean / 1000.0f; }   // mean elapsed time over 0.5 sec

// Draw Target functions ==========

// Returns width and height of current draw target
int flc::SDL_GameEngine::GetDrawTargetWidth() {
    if (pEngineDrawTarget == nullptr) std::cout << "ERROR: GetDrawTargetWidth() --> nullptr drawtarget!" << std::endl;
    return pEngineDrawTarget->width;
}

int flc::SDL_GameEngine::GetDrawTargetHeight() {
    if (pEngineDrawTarget == nullptr) std::cout << "ERROR: GetDrawTargetHeight() --> nullptr drawtarget!" << std::endl;
    return pEngineDrawTarget->height;
}

// Returns sprite pointer of the current draw target
flc::Sprite* flc::SDL_GameEngine::GetDrawTarget() { return pEngineDrawTarget; }

// Set the draw target to be the parameter sprite pointer
// NOTE if you specify 0 (nullptr, nullptr), the screen (= layer[0]) should be selected as the draw target
void flc::SDL_GameEngine::SetDrawTarget( flc::Sprite *pDT ) {
    if (pDT == nullptr) {
        pEngineDrawTarget = vWindows[nActiveWindowIx]->vLayers[0].pLayerCanvas;
        nEngineDrawTargetIx = 0;
    } else {
        pEngineDrawTarget = pDT;
    }
}

// =============== Layering =============================================

// NOTE - The methods below are all implemented to work on the current active window.

// create a new layer and return the id of it
int flc::SDL_GameEngine::CreateLayer() {
    return vWindows[nActiveWindowIx]->CreateLayer();
}

// set layer as the new drawtarget of the currently active window
void flc::SDL_GameEngine::SetDrawTarget( uint8_t layer ) {
    vWindows[nActiveWindowIx]->SetDrawTarget( layer );
    pEngineDrawTarget = vWindows[nActiveWindowIx]->GetDrawTarget();
    nEngineDrawTargetIx = layer;
}

// enable or disable layer for rendering on the current window
void flc::SDL_GameEngine::EnableLayer( uint8_t layer, bool bEnable ) {
    vWindows[nActiveWindowIx]->EnableLayer( layer, bEnable );
}

// set an offset for layer on the current window
void flc::SDL_GameEngine::SetLayerOffset( uint8_t layer, float x, float y ) {
    vWindows[nActiveWindowIx]->SetLayerOffset( layer, x, y );
}

// set a scale for layer on the current window
void flc::SDL_GameEngine::SetLayerScale( uint8_t layer, float x, float y ) {
    vWindows[nActiveWindowIx]->SetLayerScale( layer, x, y );
}

// set an inverted scale for layer on the current window
void flc::SDL_GameEngine::SetLayerScaleInv( uint8_t layer, float x, float y ) {
    vWindows[nActiveWindowIx]->SetLayerScaleInv( layer, x, y );
}

// set a tint for layer on the current window
void flc::SDL_GameEngine::SetLayerTint( uint8_t layer, const flc::Pixel& tint ) {
    vWindows[nActiveWindowIx]->SetLayerTint( layer, tint );
}

// ========== Window selection / activation ====================

// Make window with index nWinID the active window. As a side effect the draw target of the new
// active window becomes the active draw target. This need not be the canvas of layer[0]!!
void flc::SDL_GameEngine::ActivateWindow( int nWinID ) {
    if (nWinID < 0 || nWinID >= (int)vWindows.size()) {
        std::cout << "ERROR: ActivateWindow() --> window index out of range: " << nWinID << std::endl;
        nWinID = 0;
    }
    nActiveWindowIx = nWinID;
    pActiveWindow   = vWindows[nActiveWindowIx];
    pEngineDrawTarget   = pActiveWindow->GetDrawTarget();
    nEngineDrawTargetIx = pActiveWindow->GetDrawTargetIndex();
    // I must make sure that the glbRendererPtr is updated to the renderer * that belongs the the activated window!!
    glbRendererPtr = pActiveWindow->GetRendererPtr();
}

// Creates a new window with the specifications in compliance to the parameters, adds it to
// the vWindows container and returns an index of the new window.
int flc::SDL_GameEngine::AddWindow(
    const std::string &sCaption,
    int widthInPixels, int heightInPixels,
    int pixelSizeX,    int pixelSizeY,
    bool bFullScreen, bool bResizable, bool bVsynced, int nRenderIx
) {
    // dynamically allocate the SGE_Window object
    SGE_Window *winPtr = new SGE_Window();
    // initialize it
    winPtr->CreateWindow(
        sCaption,
        widthInPixels, heightInPixels,
        pixelSizeX, pixelSizeY,
        bFullScreen, bResizable, bVsynced, nRenderIx
    );
    // set the ID - this is also the index in the vWindows container
    winPtr->nWinID = (int)vWindows.size();
    // add to the windows vector
    vWindows.push_back( winPtr );

    return winPtr->nWinID;
}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
