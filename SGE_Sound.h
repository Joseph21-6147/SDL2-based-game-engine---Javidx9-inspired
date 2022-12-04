#ifndef SGE_SOUND_H
#define SGE_SOUND_H

/* SGE_Sound.h - part of the SDL2-based Game Engine (SGE) v.20221204
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

// This module is a wrapper around the SDL functionality for producing sounds.
// At this point the functionality is limited to music (SDL_Music) and
// short sound effects (SDL_Chunks).

#include <iostream>

#include <SDL_mixer.h>
//                               +-----------+                               //
// ------------------------------+ CONSTANTS +------------------------------ //
//                               +-----------+                               //

// constants for initializing SDL_mixer
#define STD_FREQ        44100   // Nyquist: for the sample rate, take double the
                                //          human upper hearing limit in Hz
#define CHUNK_SIZE       2048   // packet size
#define NR_OF_CHANNELS      2   // stereo

namespace flc {

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

    class Music {
    public:
        // creates a Music object and loads the music from file
        Music( std::string sFileName );
        virtual ~Music();

        // you can play from the start, pause, resume or terminate (halt) the music
        void Start( bool repeat = true );
        void Halt();
        void Pause();
        void Resume();

        // queries whether music is playing or is paused
        bool IsPlaying();
        bool IsPaused();

        // the volume setting is shared among all Music objects. There may be more
        // than one Music object, but only 1 can be playing at any time. The value
        // of the volume is in [0.0f, 1.0f]
        static void  SetVolume( float value = 0.25f );
        static float GetVolume();

        // returns the name of the file that was used to load this music object
        std::string GetFileName();

    private:
        // hide the default constructor - make only the constructor with the file name
        // parameter available
        Music() {}

        std::string m_MusicFile;           // contains filename after loading
        Mix_Music *m_MusicPtr = nullptr;   // pointer to the SDL music object

        static float m_MusicVolume;        // in [0.0f, 1.0f]
    };

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

    class Chunk {
    public:
        // creates a Chunck (sound effect) object and loads it from file
        Chunk( std::string sFileName );
        virtual ~Chunk();

        // you can only *play* sound effects. There's no pausing or halting
        // repeats = 0 means it's only played once :)
        void Play( int repeats = 0 );

        // the volume setting is dedicated per Chunk objects. The value of the volume
        // is in [0.0f, 1.0f]
        void SetVolume( float value = 0.25f );
        float GetVolume();

        // returns the name of the file that was used to load this chunk object
        std::string GetFileName();

    private:
        // hide the default constructor - make only the constructor with the file name
        // parameter available
        Chunk() {}

        std::string m_ChunkFile;           // contains filename after loading
        Mix_Chunk *m_ChunkPtr = nullptr;   // pointer to the SDL chunk object

        float m_ChunkVolume;               // in [0.0f, 1.0f] - each chunk has its own volume
    };

}  // namespace flc

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // SGE_SOUND_H
