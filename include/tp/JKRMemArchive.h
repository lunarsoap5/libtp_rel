/**	@file JKRMemArchive.h
 *	@brief See https://github.com/zeldaret/tp/blob/master/include/JSystem/JKernel/JKRMemArchive.h
 *
 *	@author Isaac
 *	@bug
 */
#ifndef TP_JKRMEMARCHIVE_H
#define TP_JKRMEMARCHIVE_H

#include <cstdint>

#include "tp/JKRArchive.h"
#include "tp/JKRCompression.h"

namespace libtp::tp
{
    class JKRMemArchive: public JKRArchive
    {
       public:
        /* 0x00 */     // vtable
        /* 0x04 */     // JKRArchive
        /* 0x5C */ JKRCompression mCompression;
        /* 0x60 */ EMountDirection mMountDirection;
        /* 0x64 */ SArcHeader* mArcHeader;
        /* 0x68 */ uint8_t* mArchiveData;
        /* 0x6C */ bool mIsOpen;
        /* 0x6D */ uint8_t field_0x6d[3];
    };
}     // namespace libtp::tp

#endif