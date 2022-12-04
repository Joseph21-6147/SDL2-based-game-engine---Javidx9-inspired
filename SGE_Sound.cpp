/* SGE_Sound.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
 * ===================================================================
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

#include "SGE_Sound.h"
#include "SGE_Utilities.h"

// ====================/   Music class   /==============================

// provide initial value for the static (shared) class variable
float flc::Music::m_MusicVolume = 0.25f;

//                           +-----------------+                             //
// --------------------------+ CONSTRUCTOR ETC +---------------------------- //
//                           +-----------------+                             //

flc::Music::Music( std::string sFileName ) {
    m_MusicFile = sFileName;  // used for testing / debugging / identification
    m_MusicPtr = Mix_LoadMUS( sFileName.c_str() );
    if (m_MusicPtr == nullptr)
        std::cout << "ERROR: Music() --> couldn't load file: " << sFileName << ": " << Mix_GetError() << std::endl;
    SetVolume();
}

flc::Music::~Music() {
    Mix_FreeMusic( m_MusicPtr );
    m_MusicPtr = nullptr;
}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// Starts the music. NOTE: if it was already playing this method has no effect.
void flc::Music::Start( bool repeat ) {
    int nLoopVal = repeat ? -1 : 1;
    if (!IsPlaying()) {
        if (Mix_PlayMusic( m_MusicPtr, nLoopVal ) == -1) {
            std::cout << "ERROR: Music::Start() --> " << Mix_GetError() << ": " << m_MusicFile << std::endl;
        }
    }
}

// Pause, resume or halt the music, but check if it's useful
void flc::Music::Pause() {  if (IsPlaying()              ) Mix_PauseMusic();  }
void flc::Music::Resume() { if (IsPaused()               ) Mix_ResumeMusic(); }
void flc::Music::Halt() {   if (IsPlaying() || IsPaused()) Mix_HaltMusic();   }

// return whether the music is playing or is paused
bool flc::Music::IsPlaying() { return Mix_PlayingMusic() == 1; }
bool flc::Music::IsPaused() {  return Mix_PausedMusic()  == 1; }

// sets the volume for all music objects to value
void flc::Music::SetVolume( float value ) {
    m_MusicVolume = Clamp( value, 0.0f, 1.0f );
    Mix_VolumeMusic( (int)((float)MIX_MAX_VOLUME * m_MusicVolume ));
}

// returns the current volume setting
float       flc::Music::GetVolume() {   return m_MusicVolume; }

// returns the name of the file that was used to load the music object
std::string flc::Music::GetFileName() { return m_MusicFile;   }


// ====================/   Chunk class   /==============================

//                           +-----------------+                             //
// --------------------------+ CONSTRUCTOR ETC +---------------------------- //
//                           +-----------------+                             //

// create a chunk object and load the sound effect from sFileName into it
flc::Chunk::Chunk( std::string sFileName ) {
    m_ChunkFile = sFileName;  // used for testing / debugging / identification
    m_ChunkPtr = Mix_LoadWAV( sFileName.c_str() );
    if (m_ChunkPtr == nullptr)
        std::cout << "ERROR: Chunk() --> couldn't load file: " << sFileName << ": " << Mix_GetError() << std::endl;
    SetVolume();
}

flc::Chunk::~Chunk() {
    Mix_FreeChunk( m_ChunkPtr );
    m_ChunkPtr = nullptr;
}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// play this sound effect. NOTE: there's no pause, resume or halt of chunks
void flc::Chunk::Play( int repeats ) {
    if (Mix_PlayChannel( -1, m_ChunkPtr, repeats) == -1)
        std::cout << "ERROR: Chunk::Play() --> " << Mix_GetError() << ": " << m_ChunkFile << std::endl;
}

// sets the volume for this chunk object to value
void flc::Chunk::SetVolume(float value) {
    m_ChunkVolume = Clamp( value, 0.0f, 1.0f );
    Mix_VolumeChunk( m_ChunkPtr, (int)((float)MIX_MAX_VOLUME * m_ChunkVolume ));
}

// returns the current volume setting
float       flc::Chunk::GetVolume() {   return m_ChunkVolume; }
// returns the name of the file that was used to load the chunk object
std::string flc::Chunk::GetFileName() { return m_ChunkFile;   }

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
