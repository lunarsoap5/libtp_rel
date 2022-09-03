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
    int32_t GetDirEntries( int32_t chn, libtp::gc_wii::card::__DirEntry* dirEntries, int32_t* count, bool showall );

}     // namespace libtp::util::card

#endif