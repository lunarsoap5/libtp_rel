#include "tools.h"

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "display/console.h"
#include "gc/card.h"
#include "memory.h"
#include "tp/JFWSystem.h"
#include "tp/d_com_inf_game.h"
#include "tp/d_stage.h"
#include "tp/dzx.h"
#include "tp/f_op_actor_mng.h"

namespace libtp::tools
{
    uint16_t fletcher16( uint8_t* data, int32_t length )
    {
        uint16_t sum1 = 0;
        uint16_t sum2 = 0;

        for ( int32_t index = 0; index < length; ++index )
        {
            sum1 = ( sum1 + data[index] ) % 0xFF;
            sum2 = ( sum2 + sum1 ) % 0xFF;
        }

        return ( sum2 << 8 ) | sum1;
    }

    //This entire function will need to be re-looked at now that a lot of defintions are different. UPDATE: should be better. updated data types in libtp
    void TriggerSaveLoad( const char* stage, uint8_t room, uint8_t spawn, uint8_t state, uint8_t event )
    {
        using namespace libtp::tp::d_com_inf_game;

        dComIfG_inf_c* gameInfo = &dComIfG_gameInfo;
        strcpy( gameInfo->play.mNextStage.mStage, stage );
        gameInfo->play.mNextStage.mRoomNo = room;
        gameInfo->play.mNextStage.mPoint = spawn;
        gameInfo->play.mNextStage.mLayer = state;

        gameInfo->play.mEvent.mOrder[0].mEventInfoIdx = event;
        gameInfo->save.restart.mLastMode = 0;
        gameInfo->play.mNextStage.mPoint = 0;
        gameInfo->save.restart.mRoomParam = 0;
        gameInfo->play.mEvent.mOrder[0].mEventId = 0xFFFF; //immediateControl
        gameInfo->play.mNextStage.wipe_speed = 0x13;

        gameInfo->play.mNextStage.wipe = true;
    }

    void SpawnActor( uint8_t roomID, tp::dzx::ACTR& actor )
    {
        using namespace libtp::tp::dzx;
        using namespace libtp::tp::f_op_actor_mng;

        ActorPRMClass* actorMemoryPtr = CreateAppend();

        actorMemoryPtr->params = actor.parameters;

        actorMemoryPtr->pos[0] = actor.pos[0];
        actorMemoryPtr->pos[1] = actor.pos[1];
        actorMemoryPtr->pos[2] = actor.pos[2];

        actorMemoryPtr->xRot = actor.rot[0];
        actorMemoryPtr->yRot = actor.rot[1];

        actorMemoryPtr->flag = actor.flag;
        actorMemoryPtr->enemy_id = actor.enemyID;
        actorMemoryPtr->room_id = roomID;

        tp::d_stage::ActorCreate( &actor, actorMemoryPtr );
    }

    int32_t ReadGCI( int32_t chan, const char* fileName, int32_t length, int32_t offset, void* buffer )
    {
        using namespace libtp::gc::card;

        CARDFileInfo* fileInfo = new CARDFileInfo();
        uint8_t* workArea = new uint8_t[CARD_WORKAREA_SIZE];
        int32_t result;

        // Since we can only read in and at increments of CARD_READ_SIZE do this to calculate the region we require

        int32_t adjustedOffset = ( offset / CARD_READ_SIZE ) * CARD_READ_SIZE;
        int32_t adjustedLength = ( 1 + ( ( offset - adjustedOffset + length - 1 ) / CARD_READ_SIZE ) ) * CARD_READ_SIZE;

        // Buffer might not be adjusted to the new length so create a temporary data buffer
        uint8_t* data = new uint8_t[adjustedLength];

        // Check if card is valid
        result = CARDProbeEx( chan, NULL, NULL );

        if ( result == CARD_RESULT_READY )
        {
            result = CARDMount( chan,
                                workArea,
                                []( int32_t chan, int32_t result )
                                {
                                    // S
                                    tp::jfw_system::ConsoleLine* line =
                                        &tp::jfw_system::systemConsole->consoleLine[JFW_DEBUG_LINE];

                                    line->showLine = true;
                                    sprintf( line->line, "ReadGCI::CARDERR; Chan: %" PRId32 " Result: %" PRId32, chan, result );
                                } );

            if ( result == CARD_RESULT_READY )
            {
                // Read data
                result = CARDOpen( chan, const_cast<char*>( fileName ), fileInfo );

                if ( result == CARD_RESULT_READY )
                {
                    result = CARDRead( fileInfo, data, adjustedLength, adjustedOffset );
                    CARDClose( fileInfo );

                    // Copy data to the user's buffer
                    memcpy( buffer, data + ( offset - adjustedOffset ), length );
                }
                // CARDOpen
                CARDUnmount( chan );
            }
            // CARDMount
        }
        // CARDProbeEx

        // Clean up
        delete fileInfo;
        delete[] workArea;
        delete[] data;

        return result;
    }

    uint32_t getRandom( uint64_t* seed, uint32_t max )
    {
        uint64_t z = ( *seed += 0x9e3779b97f4a7c15 );
        z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
        z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;

        return ( z % max );
    }
}     // namespace libtp::tools