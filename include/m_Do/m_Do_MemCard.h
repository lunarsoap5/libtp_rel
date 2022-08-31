/**	@file m_Do_MemCard.h
 *	@brief Helper for interacting with the memory card.
 *
 *	@author Isaac
 *	@bug
 */
#ifndef M_DO_M_DO_MEMCARD_H
#define M_DO_M_DO_MEMCARD_H
#include <cstdint>

#include "dolphin/os/OS.h"

namespace libtp::m_Do_MemCard
{
    using libtp::os::OSCond;
    using libtp::os::OSMutex;

    class mDoMemCd_Ctrl_c
    {
       public:
        enum CardCommand
        {
            CARD_NO_COMMAND,
            CARD_RESTORE,
            CARD_STORE,
            CARD_FORMAT,
            CARD_ATTACH,
            CARD_DETACH,
        };

        /* 0x0000 */ uint8_t mData[0x1FBC];
        /* 0x1FBC */ uint8_t mChannel;
        /* 0x1FBD */ uint8_t mCopyToPos;
        /* 0x1FBE */ uint8_t mProbeStat;
        /* 0x1FC0 */ int32_t mCardCommand;
        /* 0x1FC4 */ int32_t mCardState;
        /* 0x1FC8 */ int32_t field_0x1fc8;
        /* 0x1FCC */ OSMutex mMutex;
        /* 0x1FE4 */ OSCond mCond;
        /* 0x1FEC */ uint32_t mNandState;
        /* 0x1FF0 */ uint64_t mSerialNo;
        /* 0x1FF8 */ uint32_t mDataVersion;
    };     // Size: 0x2000

    extern "C"
    {
        void mDoMemCd_Ctrl_c__detach( mDoMemCd_Ctrl_c* _this );
        // Decomp says this function returns an s32. Need to confirm with the map.
        bool mDoMemCd_Ctrl_c__loadfile( mDoMemCd_Ctrl_c* _this );
    }

}     // namespace libtp::m_Do_MemCard

#endif