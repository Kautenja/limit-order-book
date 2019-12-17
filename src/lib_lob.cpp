//  Program:      lob
//  File:         lib_lob.cpp
//  Description:  file describes the outward facing ctypes API for Python
//
//  Copyright (c) 2019 Christian Kauten. All rights reserved.
//

#include <string>
#include "limit_order_book.hpp"

// Windows-base systems
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
    // setup the module initializer. required to link visual studio C++ ctypes
    void PyInit_lib_lob() { }
    // setup the function modifier to export in the DLL
    #define EXP __declspec(dllexport)
// Unix-like systems
#else
    // setup the modifier as a dummy
    #define EXP
#endif

using namespace LOB;

// definitions of functions for the Python interface to access
extern "C" {
    /// Initialize a new limit order book and return a pointer to it
    EXP LimitOrderBook* Initialize() { return new LimitOrderBook(); }

    /// Delete the limit order book, i.e., purge it from memory
    EXP void Delete(LimitOrderBook* book) { delete book; }

    // /// Return a pointer to a controller on the machine
    // EXP NES::NES_Byte* Controller(LimitOrderBook* book, int port) {
    //     return emu->get_controller(port);
    // }

    // /// Return the pointer to the screen buffer
    // EXP NES::NES_Pixel* Screen(LimitOrderBook* book) {
    //     return emu->get_screen_buffer();
    // }

    // /// Return the pointer to the memory buffer
    // EXP NES::NES_Byte* Memory(LimitOrderBook* book) {
    //     return emu->get_memory_buffer();
    // }

    // /// Reset the emulator
    // EXP void Reset(LimitOrderBook* book) {
    //     emu->reset();
    // }

    // /// Perform a discrete step in the emulator (i.e., 1 frame)
    // EXP void Step(LimitOrderBook* book) {
    //     emu->step();
    // }

    // /// Create a deep copy (i.e., a clone) of the given emulator
    // EXP void Backup(LimitOrderBook* book) {
    //     emu->backup();
    // }

    // /// Create a deep copy (i.e., a clone) of the given emulator
    // EXP void Restore(LimitOrderBook* book) {
    //     emu->restore();
    // }

    // /// Close the emulator, i.e., purge it from memory
    // EXP void Close(LimitOrderBook* book) {
    //     delete emu;
    // }
}

// un-define the macro
#undef EXP
