/**	@file m_Do_dvd_thread.h
 *	@brief See https://github.com/zeldaret/tp/blob/master/include/m_Do/m_Do_dvd_thread.h
 *
 *	@author Isaac
 *	@bug
 */
#ifndef TP_M_DO_DVD_THREAD_H
#define TP_M_DO_DVD_THREAD_H

#include <cstdint>

#include "tp/JKRArchive.h"
#include "tp/JKRHeap.h"
#include "tp/JKRMemArchive.h"
#include "tp/c_list.h"
#include "tp/c_node.h"
#include "gc_wii/OS.h"

namespace libtp::tp
{
    typedef void* ( *mDoDvdThd_callback_func )( void* );

    class mDoDvdThd_command_c: public node_class
    {
       public:
        /* 0x0C */ bool mIsDone;
        /* 0x10 */ void* vtable;
    };     // Size = 0x14

    class mDoDvdThd_toMainRam_c: public mDoDvdThd_command_c
    {
       public:
        /* 0x14 */ uint8_t mMountDirection;
        /* 0x18 */ int32_t mEntryNum;
        /* 0x1C */ void* mData;
        /* 0x20 */ int32_t mDataSize;
        /* 0x24 */ JKRHeap* mHeap;
    };     // Size = 0x28

    class mDoDvdThd_param_c
    {
       public:
        /* 0x00 */ libtp::gc_wii::os::OSMessageQueue mMessageQueue;
        /* 0x20 */ void* mMessageQueueMessages;
        /* 0x24 */ node_list_class mNodeList;
        /* 0x30 */ libtp::gc_wii::os::OSMutex mMutext;
    };     // Size = 0x48

    class mDoDvdThd_mountXArchive_c: public mDoDvdThd_command_c
    {
       public:
        /* 0x14 */ uint8_t mMountDirection;
        /* 0x18 */ int32_t mEntryNum;
        /* 0x1C */ JKRArchive* mArchive;
        /* 0x20 */ JKRArchive::EMountMode mMountMode;
        /* 0x24 */ JKRHeap* mHeap;
    };     // Size = 0x28

    class mDoDvdThd_mountArchive_c: public mDoDvdThd_command_c
    {
       public:
        /* 0x14 */ uint8_t mMountDirection;
        /* 0x18 */ int32_t mEntryNumber;
        /* 0x1C */ JKRMemArchive* mArchive;
        /* 0x20 */ JKRHeap* mHeap;
    };     // Size = 0x24

    class mDoDvdThd_callback_c: public mDoDvdThd_command_c
    {
       public:
        /* 80015BB8 */ virtual ~mDoDvdThd_callback_c();
        /* 80015C18 */ mDoDvdThd_callback_c( mDoDvdThd_callback_func, void* );
        /* 80015C74 */ static mDoDvdThd_callback_c* create( mDoDvdThd_callback_func, void* );
        /* 80015CF0 */ virtual int32_t execute();

       private:
        /* 0x14 */ mDoDvdThd_callback_func mFunction;
        /* 0x18 */ void* mData;
        /* 0x1C */ void* mResult;
    };
}     // namespace libtp::tp

#endif