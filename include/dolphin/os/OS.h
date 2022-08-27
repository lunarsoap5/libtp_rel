/**	@file OS.h
 *	@brief Defines OS structures. Based on
 *	https://github.com/zeldaret/tp/blob/master/include/dolphin/os/OS.h which
 *	will probably be broken into multiple files in the future.
 *
 *	@author Isaac
 *	@bug
 */
#ifndef DOLPHIN_OS_OS_H
#define DOLPHIN_OS_OS_H

#include <cstdint>

namespace libtp::os
{
    struct OSThread;
    struct OSMutex
    {
        uint8_t unk[24];
    };

    struct OSMutexQueue
    {
        struct OSMutex* prev;
        struct OSMutex* next;
    };

    struct OSContext
    {
        uint32_t gpr[32];
        uint32_t cr;
        uint32_t lr;
        uint32_t ctr;
        uint32_t xer;
        double fpr[32];
        uint32_t padding_1;
        uint32_t fpscr;
        uint32_t srr0;
        uint32_t srr1;
        uint16_t mode;
        uint16_t state;
        uint32_t gqr[8];
        uint32_t padding_2;
        double ps[32];
    };

    struct OSThreadLink
    {
        struct OSThread* prev;
        struct OSThread* next;
    };

    struct OSThreadQueue
    {
        struct OSThread* head;
        struct OSThread* tail;
    };

    struct OSCond
    {
        struct OSThreadQueue queue;
    };

    typedef uint16_t OSThreadState;
#define OS_THREAD_STATE_UNINITIALIZED 0
#define OS_THREAD_STATE_READY 1
#define OS_THREAD_STATE_RUNNING 2
#define OS_THREAD_STATE_WAITING 4
#define OS_THREAD_STATE_DEAD 8

    struct OSThread
    {
        OSContext context;
        OSThreadState state;
        uint16_t attributes;
        int32_t suspend_count;
        uint32_t effective_priority;
        uint32_t base_priority;
        void* exit_value;
        OSThreadQueue* queue;
        OSThreadLink link;
        OSThreadQueue join_queue;
        OSMutex* mutex;
        OSMutexQueue owned_mutexes;
        OSThreadLink active_threads_link;
        uint8_t* stack_base;
        uint8_t* stack_end;
        uint8_t* error_code;
        void* data[2];
    };

}     // namespace libtp::os

// OSMutex
// OSCond

#endif