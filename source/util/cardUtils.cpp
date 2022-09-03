#include "util/cardutils.h"

#include <cstdint>
#include <cstring>

#include "gc_wii/card.h"

namespace libtp::util::card
{
    using libtp::gc_wii::card::__DirEntries;
    using libtp::gc_wii::card::__DirEntry;

    // This is at least 0x88 bytes long since offset 0x84 is a pointer to the
    // DirectoryEntry data, but we don't need the exact details since we only
    // care about the first 8 bytes.
    struct card_block
    {
        int32_t attached;
        int32_t cardResult;
    };

    int32_t GetDirEntries( int32_t chan, __DirEntries* dirEntries, int32_t* count, bool showAll )
    {
        int32_t numMatches = 0;
        int32_t ret = CARD_RESULT_READY;
        card_block* cardBlock = nullptr;

        if ( chan < CARD_SLOT_A || chan > CARD_SLOT_B )
            return CARD_RESULT_NOCARD;

        ret = libtp::gc_wii::card::__CARDGetControlBlock( chan, (void**) &cardBlock );
        if ( ret < 0 )
            return ret;

        if ( cardBlock->attached )
        {
            __DirEntries* dirblock = libtp::gc_wii::card::__CARDGetDirBlock( (void*) cardBlock );
            __DirEntry* entries = dirblock->entries;
            __DirEntry* outEntries = dirEntries->entries;

            // For example, "GZ2E".
            char* card_gamecode = reinterpret_cast<char*>( 0x80000000 );
            // For example, "01".
            char* card_company = reinterpret_cast<char*>( 0x80000004 );

            for ( int i = 0; i < CARD_MAX_FILE; i++ )
            {
                if ( entries[i].gameCode[0] != 0xFF )
                {
                    if ( showAll ||
                         ( ( card_gamecode[0] != 0xFF && memcmp( entries[i].gameCode, card_gamecode, 4 ) == 0 ) &&
                           ( card_company[0] != 0xFF && memcmp( entries[i].publisherCode, card_company, 2 ) == 0 ) ) )
                    {
                        memcpy( (void*) &outEntries[numMatches], (void*) &entries[i], sizeof( __DirEntry ) );
                        numMatches += 1;
                    }
                }
            }

            if ( count != nullptr )
                *count = numMatches;

            if ( numMatches == 0 )
                ret = CARD_RESULT_NOFILE;
        }
        else
        {
            ret = CARD_RESULT_NOCARD;
        }

        libtp::gc_wii::card::__CARDPutControlBlock( cardBlock, ret );

        return ret;
    }
}     // namespace libtp::util::card
