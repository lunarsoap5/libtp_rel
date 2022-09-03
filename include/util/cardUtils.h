/**	@file cardUtils.h
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
    /**
     *  @brief Gets the DirEntries for a memory card. Based on params, copies
     *  data into dirEntries for each matching DirEntry. The number of copied
     *  entries is written to `count`. Note this should be called after the
     *  memory card has been mounted (at the same time you might call CARDOpen,
     *  for example). This function is based off of CARD_GetDirectory from
     *  https://github.com/devkitPro/libogc/blob/master/gc/ogc/card.h
     *
     *  @param chan EXI channel number (must be memory card slot A or B)
     *  @param dirEntries pointer to a fresh __DirEntries. This will have enough
     *  space to store data for the maximum of 127 files on the memory card.
     *  @param count pointer to which the number of matching DirEntry is
     *  written.
     *  @param showAll If true, every DirEntry for any game on the memory card
     *  will be copied to `dirEntries`. If false, will only copy ones which
     *  match the publisherCode and gameCode of the game being played (for
     *  example, DirEntry which start with "GZ2E01").
     *  @return Code indicating success or reason for failure.
     */
    int32_t GetDirEntries( int32_t chan, libtp::gc_wii::card::__DirEntries* dirEntries, int32_t* count, bool showAll );

}     // namespace libtp::util::card

#endif