/**	@file card.h
 *	@brief Provides utility methods related to the GameCube memory card.
 *	Initially this file was named "card.h", but having multiple files named
 *	"card" made it such that the "card.cpp" was not used, so the function in
 *	this file was considered to not have an implementation.
 *
 *	@author Isaac
 *	@bug
 */
#ifndef UTIL_CARD_UTILS_H
#define UTIL_CARD_UTILS_H

#include <cstdint>

#include "gc_wii/card.h"

namespace libtp::util::card
{
    struct DirectoryEntry
    {
        /* 0x00 */ uint8_t gameCode[4];
        /* 0x04 */ uint8_t publisherCode[2];
        /* 0x06 */ uint8_t padding_06;
        /* 0x07 */ uint8_t imageFlags;
        /* 0x08 */ uint8_t filename[CARD_FILENAME_MAX];
        /* 0x28 */ uint32_t lastModified;
        /* 0x2C */ uint32_t imageDataOffset;
        /* 0x30 */ uint16_t iconFormats;
        /* 0x32 */ uint16_t iconAnimationSpeeds;
        /* 0x34 */ uint8_t permissions;
        /* 0x35 */ uint8_t copyCounter;
        /* 0x36 */ uint16_t firstBlockIndex;
        /* 0x38 */ uint16_t numBlocks;
        /* 0x3A */ uint16_t padding_3A;
        /* 0x3C */ uint32_t commentsOffset;
    };     // Size: 0x40

    int32_t GetDirectoryEntries( int32_t chn, DirectoryEntry* dirEntries, int32_t* count, bool showall );

}     // namespace libtp::util::card

#endif