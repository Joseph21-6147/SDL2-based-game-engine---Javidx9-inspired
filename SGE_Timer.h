#ifndef MUTIMER_H
#define MUTIMER_H

/* SGE_Timer.h - part of the SDL2-based Game Engine (SGE) v.20221204
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

// This module implements a simple timer, and a simple profiler object that
// supports profiling of pieces of code.

#include <iostream>
#include <chrono>
#include <vector>

//                               +-----------+                               //
// ------------------------------+ CONSTANTS +------------------------------ //
//                               +-----------+                               //

// constants for timer state - very simple timer, either running or idle
#define MU_TMR_IDLE     0
#define MU_TMR_RUNNING  1

// defines max nr of probes per profiler
#define MU_NR_PROBES 100

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

// The timer object is initially idle, can be started, stopped or stopped and restarted
// immediately thereafter. Upon stop or stop/restart the time that is elapsed since the
// most recent Start() call is returned. The timings are in micro seconds, hence the
// name MuTimer.
// Inspiration: https://www.techiedelight.com/measure-elapsed-time-program-chrono-library/

    class MuTimer {
    public:
        MuTimer();
        ~MuTimer();

        // Start the timer
        void Start();
        // Stop the timer and return the elapsed time since the start in micro(!)seconds
        int Stop();
        // Stop the timer, report the elapsed time since the start in micro(!)seconds, and
        // start the timer for the next measurement.
        int StopAndStart();

    private:
        // state of timer
        int nState = MU_TMR_IDLE;
        // timing stuff
        std::chrono::time_point<std::chrono::system_clock> m_start_timing,  // to store start of timing period
                                                           m_stop_timing;   // to store end of timing period
    };

//                           +------------------+                            //
// --------------------------+ CLASS DEFINITION +--------------------------- //
//                           +------------------+                            //

// The profiler object has an internal timer, and uses that to measure (probe) elapsed
// time over pieces of code. There are max. 100 probes available, you can give them
// names as well.
// First call InitProbes() (with or without names), then time the pieces of code with
// their own probe id. After the measurements you can query each probe separately, or
// have all the probes statistics printed to screen.

    class MuProfiler {
    public:
        MuProfiler();
        ~MuProfiler();

        // struct to hold info for one probe
        struct sProbeInfo {
            int nProbeID;         // which probe is timed
            int nCumValue;        // cumulates the elapsed time measured
            int nFreq;            // holds the number of measurements
            std::string sName;    // to give the probe a name
        };

        // initialize the probes
        void InitProbes();
        // as before, and set names to the probes
        void InitProbes( std::vector<std::string> vProbeNames );
        // add timing for the specified probe
        void Probe( int nProbeIx );
        // getters / query methods for a specified probe
        int         GetProbeVal(  int nProbeIx );
        int         GetProbeFreq( int nProbeIx );
        std::string GetProbeName( int nProbeIx );
        // print the statistics for all the probes to screen
        void PrintStats( std::string sMsg, bool bVerbose = false );

    private:
        // internal profiler timer
        MuTimer cProbeTimer;
        // holds the number and list with probes
        int m_NrProbes;
        std::vector<sProbeInfo> vProbeData;
    };

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // MUTIMER_H
