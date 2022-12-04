/* MuTimer.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
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

#include <iostream>

#include "MuTimer.h"

// ==============================/ Class MuTimer /==============================

//                           +-----------------+                             //
// --------------------------+ CONSTRUCTOR ETC +---------------------------- //
//                           +-----------------+                             //

// the constructor sets timer state to idle
MuTimer::MuTimer() {
    nState = MU_TMR_IDLE;
}

// default destructor
MuTimer::~MuTimer() {}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// Start the timer
void MuTimer::Start() {
    if (nState != MU_TMR_IDLE)
        std::cout << "WARNING: MuTimer::Start() --> timer state is not idle: " << nState << std::endl;

    // set state
    nState = MU_TMR_RUNNING;
    // get current time stamp in start variable
    m_start_timing = std::chrono::system_clock::now();
}

// Stop the timer and return the elapsed time since the start in micro(!)seconds
int MuTimer::Stop() {
    if (nState != MU_TMR_RUNNING)
        std::cout << "WARNING: MuTimer::Stop() --> timer state is not running: " << nState << std::endl;

    // get current time stamp in stop variable
    m_stop_timing  = std::chrono::system_clock::now();
    // calculate the time elapsed between last start and stop timings
    int nMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(m_stop_timing - m_start_timing).count();
    // set state
    nState = MU_TMR_IDLE;
    return nMicroSeconds;
}

// Stop the timer, report the elapsed time since the start in micro(!)seconds, and
// start the timer for the next measurement.
int MuTimer::StopAndStart() {
    // stop timer and record elapsed time
    int nMicroSeconds = Stop();
    // start timer for next frame
    Start();
    return nMicroSeconds;
}

// ==============================/ Class MuProfiler /==============================

//                           +-----------------+                             //
// --------------------------+ CONSTRUCTOR ETC +---------------------------- //
//                           +-----------------+                             //

// default constructor
MuProfiler::MuProfiler() {}

// default destructor
MuProfiler::~MuProfiler() {}

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

void MuProfiler::InitProbes() {
    std::vector<std::string> vEmptyDummy;
    InitProbes( vEmptyDummy );
}

void MuProfiler::InitProbes( std::vector<std::string> vNames ) {
    vProbeData.clear();
    int nrNames = (int)vNames.size();
    for (int i = 0; i < MU_NR_PROBES; i++) {
        sProbeInfo aux;
        aux.nProbeID  = i;
        aux.nCumValue = 0;
        aux.nFreq     = 0;
        aux.sName     = (i < nrNames) ? vNames[i] : "";
        vProbeData.push_back( aux );
    }
    m_NrProbes = (vNames.empty() ? MU_NR_PROBES : (int)vNames.size());
    cProbeTimer.Start();
}

// get the timing and add it to the statistics for this probe
void MuProfiler::Probe( int nProbeIx ) {
    if (nProbeIx < 0 || nProbeIx >= MU_NR_PROBES) std::cout << "ERROR: MuProfiler::Probe() --> index out of range!" << std::endl;
    vProbeData[nProbeIx].nCumValue += cProbeTimer.StopAndStart();
    vProbeData[nProbeIx].nFreq     += 1;
}

// returns the cumulated elapsed time for this probe
int MuProfiler::GetProbeVal( int nProbeIx ) {
    if (nProbeIx < 0 || nProbeIx >= MU_NR_PROBES) std::cout << "ERROR: MuProfiler::GetProbeVal() --> index out of range!" << std::endl;
    return vProbeData[nProbeIx].nCumValue;
}

// returns the nr of measurements for this probe
int MuProfiler::GetProbeFreq( int nProbeIx ) {
    if (nProbeIx < 0 || nProbeIx >= MU_NR_PROBES) std::cout << "ERROR: MuProfiler::GetProbeFreq() --> index out of range!" << std::endl;
    return vProbeData[nProbeIx].nFreq;
}

// returns the name for this probe
std::string MuProfiler::GetProbeName( int nProbeIx ) {
    if (nProbeIx < 0 || nProbeIx >= MU_NR_PROBES) std::cout << "ERROR: MuProfiler::GetProbeName() --> index out of range!" << std::endl;
    return vProbeData[nProbeIx].sName;
}

// function produces a right aligned string of length positions containing value

std::string right_align( std::string &s, int positions ) {
    std::string result;

    for (int i = 0; i < (positions - (int)s.length()); i++)
        result.append( " " );
    result.append( s );

    return result;
}

std::string right_align( int value, int positions ) {
    std::string aux = std::to_string( value );
    return right_align( aux, positions );
}

// function produces a left aligned string of length positions containing value

std::string left_align( std::string &s, int positions ) {
    std::string result;

    result.append( s );
    for (int i = 0; i < (positions - (int)s.length()); i++)
        result.append( " " );

    return result;
}

std::string left_align( int value, int positions ) {
    std::string aux = std::to_string( value );
    return left_align( aux, positions );
}

// float f resp. string s contains a floating point number which may (or may not) have a dot in it.
// align the dot of this number on dotPosition, creating a string of totalPositions

std::string dot_align( std::string &s, int dotPosition, int totalPositions ) {
    std::string result;
    size_t dotIndex = s.find( "." );
    std::string sBeforeDot, sAfterDot, sDot;
    if (dotIndex == std::string::npos) {     // string contained no '.'
        sBeforeDot = s;
        sDot = "";
        sAfterDot = "";
    } else {
        sBeforeDot = s.substr( 0, dotIndex );
        sDot = ".";
        sAfterDot  = s.substr( dotIndex + 1 );
    }
    result.append( right_align( sBeforeDot, dotPosition - 1 ));
    result.append( sDot );
    result.append( left_align( sAfterDot, totalPositions - dotPosition ));
    return result;
}

std::string dot_align( float f, int dotPosition, int totalPositions ) {
    std::string aux = std::to_string( f );
    return dot_align( aux, dotPosition, totalPositions );
}

// print all the statistics for this profiler
void MuProfiler::PrintStats( std::string sMsg, bool bVerbose ) {
    // first get cumulated statistics
    int nTotalValue = 0;
    int nMaxValue   = 0;
    int nMaxFreq    = INT_MIN;
    int nMaxNameLen = 0;
    for (int i = 0; i < (int)vProbeData.size(); i++) {
        sProbeInfo aux = vProbeData[i];
        nTotalValue += aux.nCumValue;
        if (aux.nCumValue > nMaxValue) nMaxValue = aux.nCumValue;
        if (aux.nFreq     > nMaxFreq ) nMaxFreq  = aux.nFreq;
        int len = aux.sName.length();
        if (len > nMaxNameLen) nMaxNameLen = len;
    }

    auto get_positions = []( int value ) {
        int factor    = 10;
        int positions = 10;
        bool bFound = false;
        for (int i = 1; i < 9 && !bFound; i++) {
            if (value < factor) {
                bFound = true;
                positions = i;
            }
            factor *= 10;
        }
        return positions;
    };

    float fTotalPercentage = 0.0f;
    float fTotalMeans      = 0.0f;
    std::cout << sMsg << std::endl;
    for (int i = 0; i < (int)sMsg.length(); i++) std::cout << "-";
    std::cout << std::endl;

    for (int i = 0; i < m_NrProbes; i++) {
        std::string s = GetProbeName( i );
        int         f = GetProbeFreq( i );
        int         v = GetProbeVal( i );

        fTotalPercentage += 100.0f * float( v ) / float( nTotalValue );
        fTotalMeans      +=          float( v ) / float( f           );

        std::cout << "Probe nr: " << right_align( i, 2           )
                  << " name: "    <<  left_align( s, nMaxNameLen );
        if (bVerbose)
            std::cout << " frequency: "  << right_align( f, get_positions( nMaxFreq  ))
                      << " cum. musec: " << right_align( v, get_positions( nMaxValue ));
        std::cout << " mean musec: " << dot_align(          float( v ) / float( f )          , 6, 11 )
                  << " ( "           << dot_align( 100.0f * float( v ) / float( nTotalValue ), 6, 11 ) << " % )" << std::endl;
    }
    std::cout << std::endl;
    std::cout << "total means (musec): " << dot_align( fTotalMeans             , 6, 11 )           << std::endl
              << "            (msec) : " << dot_align( fTotalMeans / 1000.0f   , 6, 11 )           << std::endl
              << "                   ( " << dot_align( fTotalPercentage        , 6, 11 ) << " % )" << std::endl
              << "total mean fps     : " << dot_align( 1000000.0f / fTotalMeans, 6, 11 )           << std::endl;
}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
