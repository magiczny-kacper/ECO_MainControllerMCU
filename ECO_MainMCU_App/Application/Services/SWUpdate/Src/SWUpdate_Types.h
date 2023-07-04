#ifndef SWUPDATE_TYPES_H_
#define SWUPDATE_TYPES_H_

/**
 * @brief SWUpdated block to be updated identificator
 * 
 */
typedef enum SWUpdate_Block
{
    SWUPDATE_BLOCK_APP =            0x00, /** MCU application */
    SWUPDATE_BLOCK_NEXTION_SMALL =  0x01, /** Nextion small application */
    SWUPDATE_BLOCK_NEXTION_BIG =    0x02, /** Nextion big application */
    SWUPDATE_BLOCK_COUNT                  /** Count of blocks that could be updated */
} SWUpdate_Block_t;

#endif /* SWUPDATE_TYPES_H_ */
