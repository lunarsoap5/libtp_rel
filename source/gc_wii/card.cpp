#include "gc_wii/card.h"

#include <cstring>

namespace libtp::gc_wii::card
{
    int32_t __CARDFreeBlock( int32_t chan, uint16_t block, CARDCallback callback )
    {
        uint32_t card = reinterpret_cast<uint32_t>( &__CARDBlock[chan] );
        if ( *reinterpret_cast<int32_t*>( card ) == 0 )
        {
            return CARD_RESULT_NOCARD;
        }

        uint16_t cardVar = *reinterpret_cast<uint16_t*>( card + 0x10 );     // Maximum number of blocks?
        uint16_t* fatBlock = *reinterpret_cast<uint16_t**>( card + 0x88 );

        while ( block != 0xFFFF )
        {
            uint32_t tempBlock = static_cast<uint32_t>( block );
            if ( ( tempBlock < 5 ) || ( tempBlock >= cardVar ) )
            {
                return CARD_RESULT_BROKEN;
            }

            block = fatBlock[tempBlock];
            fatBlock[tempBlock] = 0;
            fatBlock[3] += 1;
        }

        return __CARDUpdateFatBlock( chan, fatBlock, callback );
    }

    void DeleteCallback( int32_t chan, int32_t result )
    {
        CARDBlock* card = &__CARDBlock[chan];

        CARDCallback cb = card->cardApiCb;
        card->cardApiCb = nullptr;

        int32_t ret = result;
        if ( ret >= CARD_RESULT_READY )
        {
            ret = __CARDFreeBlock( chan, card->currFileBlock, cb );
            if ( ret >= CARD_RESULT_READY )
            {
                return;
            }
        }

        __CARDPutControlBlock( card, ret );

        if ( cb )
        {
            cb( chan, ret );
        }
    }

    int32_t __CARDGetFileNo( CARDBlock* cardBlock, const char* fileName, int32_t* fileNo )
    {
        if ( cardBlock->attached == 0 )
        {
            return CARD_RESULT_NOCARD;
        }

        CARDDirEntry* dirBlock = __CARDGetDirBlock( cardBlock );

        int32_t i;
        for ( i = 0; i < CARD_MAX_FILE; i++ )
        {
            if ( !__CARDCompareFileName( &dirBlock[i], fileName ) )
            {
                continue;
            }

            if ( __CARDAccess( cardBlock, &dirBlock[i] ) < CARD_RESULT_READY )
            {
                continue;
            }

            *fileNo = i;
            break;
        }

        if ( i >= CARD_MAX_FILE )
        {
            return CARD_RESULT_NOFILE;
        }

        return CARD_RESULT_READY;
    }

    int32_t CARDDeleteAsync( int32_t chan, const char* fileName, CARDCallback callback )
    {
        CARDBlock* card = nullptr;

        int32_t ret = __CARDGetControlBlock( chan, &card );
        if ( ret < CARD_RESULT_READY )
        {
            return ret;
        }

        int32_t fileNo;
        ret = __CARDGetFileNo( card, fileName, &fileNo );
        if ( ret < CARD_RESULT_READY )
        {
            __CARDPutControlBlock( card, ret );
            return ret;
        }

        CARDDirEntry* dirBlock = __CARDGetDirBlock( card );
        CARDDirEntry& entry = dirBlock[fileNo];

        card->currFileBlock = entry.firstBlockIndex;

        memset( &entry, -1, 0x40 );

        CARDCallback cb = callback;
        if ( !cb )
        {
            cb = __CARDDefaultApiCallback;
        }

        card->cardApiCb = cb;

        ret = __CARDUpdateDir( chan, DeleteCallback );
        if ( ret >= CARD_RESULT_READY )
        {
            return ret;
        }

        __CARDPutControlBlock( card, ret );
        return ret;
    }

    int32_t CARDDelete( int32_t chan, const char* fileName )
    {
        int32_t ret = CARDDeleteAsync( chan, fileName, __CARDSyncCallback );
        if ( ret >= CARD_RESULT_READY )
        {
            ret = __CARDSync( chan );
        }
        return ret;
    }

    int32_t CARDGetDirEntries( int32_t chan, CARDDirEntries* dirEntries, int32_t* count, bool showAll )
    {
        int32_t numMatches = 0;
        int32_t ret = CARD_RESULT_READY;
        CARDBlock* cardBlock = nullptr;

        if ( chan < CARD_SLOT_A || chan > CARD_SLOT_B )
            return CARD_RESULT_NOCARD;

        ret = __CARDGetControlBlock( chan, &cardBlock );
        if ( ret < 0 )
            return ret;

        if ( cardBlock->attached )
        {
            CARDDirEntry* srcEntries = __CARDGetDirBlock( cardBlock );
            CARDDirEntry* outEntries = dirEntries->entries;

            // For example, "GZ2E".
            char* card_gamecode = reinterpret_cast<char*>( 0x80000000 );
            // For example, "01".
            char* card_company = reinterpret_cast<char*>( 0x80000004 );

            for ( int i = 0; i < CARD_MAX_FILE; i++ )
            {
                if ( srcEntries[i].gameCode[0] != 0xFF )
                {
                    if ( showAll ||
                         ( ( card_gamecode[0] != 0xFF && memcmp( srcEntries[i].gameCode, card_gamecode, 4 ) == 0 ) &&
                           ( card_company[0] != 0xFF && memcmp( srcEntries[i].publisherCode, card_company, 2 ) == 0 ) ) )
                    {
                        memcpy( (void*) &outEntries[numMatches], (void*) &srcEntries[i], sizeof( CARDDirEntry ) );
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

        __CARDPutControlBlock( cardBlock, ret );

        return ret;
    }

}     // namespace libtp::gc_wii::card