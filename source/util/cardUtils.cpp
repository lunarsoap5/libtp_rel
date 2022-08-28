#include "util/cardutils.h"

#include <cstdint>
#include <cstring>

#include "gc_wii/card.h"

namespace libtp::util::card
{
    struct DirectoryEntries
    {     // dir allocation table
        DirectoryEntry entries[CARD_MAX_FILE];
    };

    /*! \typedef struct _card_file card_file
    \brief structure to hold the fileinformations upon open and for later use.
    \param chn CARD slot.
    \param filenum file index in the card directory structure.
    \param offset offset into the file.
    \param len length of file.
    \param iblock block index on memory card.
    */
    typedef struct _card_file
    {
        int32_t chn;
        int32_t filenum;
        int32_t offset;
        int32_t len;
        uint16_t iblock;
    } card_file;

    /*! \typedef void (*cardcallback)(s32 chan,s32 result)
    \brief function pointer typedef for the user's operation callback
    \param chan CARD slot
    \param result result of operation upon call of callback.
    */
    typedef void ( *cardcallback )( int32_t chan, int32_t result );

    // typedef struct _card_block
    // {
    //     uint8_t cmd[9];
    //     uint32_t cmd_len;
    //     uint32_t cmd_mode;
    //     uint32_t cmd_blck_cnt;
    //     uint32_t cmd_sector_addr;
    //     uint32_t cmd_retries;
    //     uint32_t attached;
    //     int32_t result;
    //     uint32_t cid;
    //     uint16_t card_size;
    //     uint32_t mount_step;
    //     uint32_t format_step;
    //     uint32_t sector_size;
    //     uint16_t blocks;
    //     uint32_t latency;
    //     uint32_t cipher;
    //     uint32_t key[3];
    //     uint32_t transfer_cnt;
    //     uint16_t curr_fileblock;
    //     card_file* curr_file;
    //     struct card_dat* curr_dir;
    //     struct card_bat* curr_fat;
    //     void* workarea;
    //     void* cmd_usr_buf;
    //     uint32_t wait_sync_queue;     // type is lwpq_t
    //     uint32_t timeout_svc;         // type is syswd_t
    //     uint8_t dsp_task[0x40];       // type is dsptask_t (struct, not an array)

    //     cardcallback card_ext_cb;
    //     cardcallback card_tx_cb;
    //     cardcallback card_exi_cb;
    //     cardcallback card_api_cb;
    //     cardcallback card_xfer_cb;
    //     cardcallback card_erase_cb;
    //     cardcallback card_unlock_cb;
    // } card_block;

    typedef struct _card_block
    {
        int32_t attached;
        int32_t cardResult;
    } card_block;
    // ^ This is at least 0x88 bytes long since offset 0x84 is a pointer to the
    // DirectoryEntry data.

    /*! \fn s32 CARD_GetDirectory(s32 chn, card_dir *dir_entries, s32 *count, bool showall)
    \brief Returns the directory entries. size of entries is max. 128.
    \param[in] chn CARD slot
    \param[out] dir_entries pointer to card_dir structure to receive the result set.
    \param[out] count pointer to an integer to receive the counted entries.
    \param[in] showall Whether to show all files of the memory card or only those which are identified by the company and
    gamecode string. \return \ref card_errors "card error codes"
    */
    int32_t GetDirectoryEntries( int32_t chn, DirectoryEntry* dirEntries, int32_t* count, bool showall )
    {
        int32_t numMatches = 0;
        int32_t ret = CARD_RESULT_READY;
        card_block* cardBlock = nullptr;

        if ( chn < CARD_SLOT_A || chn > CARD_SLOT_B )
            return CARD_RESULT_NOCARD;

        ret = libtp::gc_wii::card::__CARDGetControlBlock( chn, (void**) &cardBlock );
        if ( ret < 0 )
            return ret;

        if ( cardBlock->attached )
        {
            DirectoryEntries* dirblock =
                static_cast<DirectoryEntries*>( libtp::gc_wii::card::__CARDGetDirBlock( (void*) cardBlock ) );

            DirectoryEntry* entries = dirblock->entries;

            // update these to read from correct address
            char card_gamecode[4] = { 'G', 'Z', '2', 'E' };
            char card_company[2] = { '0', '1' };

            for ( int i = 0; i < CARD_MAX_FILE; i++ )
            {
                if ( entries[i].gameCode[0] != 0xFF )
                {
                    if ( showall ||
                         ( ( card_gamecode[0] != 0xFF && memcmp( entries[i].gameCode, card_gamecode, 4 ) == 0 ) &&
                           ( card_company[0] != 0xFF && memcmp( entries[i].publisherCode, card_company, 2 ) == 0 ) ) )
                    {
                        memcpy( (void*) &dirEntries[numMatches], (void*) &entries[i], sizeof( DirectoryEntry ) );
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
