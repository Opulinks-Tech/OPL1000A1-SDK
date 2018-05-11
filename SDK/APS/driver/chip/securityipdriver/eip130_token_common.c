/* eip130_token_common.c
 *
 * Security Module Token helper functions
 * - Common byte array related functions
 *
 * This module can convert a set of parameters into a Security Module Command
 * token, or parses a set of parameters from a Security Module Result token.
 */

/*****************************************************************************
* Copyright (c) 2014-2017 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#include "c_eip130_token.h"         // configuration options

#include "basic_defs.h"             // uint8_t, IDENTIFIER_NOT_USED, etc.
#include "clib.h"                   // memset, memcpy

#include "eip130_token_common.h"    // the API to implement


/*----------------------------------------------------------------------------
 * Eip130Token_Command_WriteByteArray
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * StartWord
 *     The word index of read.
 *
 * Data_p
 *     Pointer to the buffer.
 *
 * DataLenInBytes
 *     The size of the buffer to copy to the token.
 */
void
Eip130Token_Command_WriteByteArray(
        Eip130Token_Command_t * const CommandToken_p,
        unsigned int StartWord,
        const uint8_t * Data_p,
        const unsigned int DataLenInBytes)
{
    const uint8_t * const Stop_p = Data_p + DataLenInBytes;

    if (CommandToken_p == NULL || Data_p == NULL)
    {
        return;
    }

    while (Data_p < Stop_p)
    {
        uint32_t W;

        if (StartWord >= EIP130TOKEN_RESULT_WORDS)
        {
            return;
        }

        // LSB-first
        W = (*Data_p++);
        if (Data_p < Stop_p)
        {
            W |= (*Data_p++) << 8;
            if (Data_p < Stop_p)
            {
                W |= (*Data_p++) << 16;
                if (Data_p < Stop_p)
                {
                    W |= (*Data_p++) << 24;
                }
            }
        }

        // Write word
        CommandToken_p->W[StartWord++] = W;
    }
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_ReadByteArray
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * StartWord
 *     The word index of read.
 *
 * DestLenInBytes
 *     The size of the buffer to copy the read info to.
 *
 * Dest_p
 *     Pointer to the buffer.
 */
void
Eip130Token_Result_ReadByteArray(
        const Eip130Token_Result_t * const ResultToken_p,
        unsigned int StartWord,
        unsigned int DestLenOutBytes,
        uint8_t * Dest_p)
{
    uint8_t * const Stop_p = Dest_p + DestLenOutBytes;

    if (ResultToken_p == NULL || Dest_p == NULL)
    {
        return;
    }

    while (Dest_p < Stop_p)
    {
        uint32_t W;

        if (StartWord >= EIP130TOKEN_RESULT_WORDS)
        {
            return;
        }

        // Read word
        W = ResultToken_p->W[StartWord++];

        // LSB-first
        *Dest_p++ = (uint8_t)W;
        if (Dest_p < Stop_p)
        {
            W >>= 8;
            *Dest_p++ = (uint8_t)W;
            if (Dest_p < Stop_p)
            {
                W >>= 8;
                *Dest_p++ = (uint8_t)W;
                if (Dest_p < Stop_p)
                {
                    W >>= 8;
                    *Dest_p++ = (uint8_t)W;
                }
            }
        }
    }
}


/* end of file eip130_token_common.c */

/*----------------------------------------------------------------------------
 * Eip130Token_Command_Clear
 *
 * This function initializes a Eip130Token_Command_t array with a known
 * pattern. This helps debugging.
 *
 * CommandToken_p
 *      Pointer to the command token buffer.
 */
void
Eip130Token_Command_Clear(
        Eip130Token_Command_t * const CommandToken_p)
{
    unsigned int i;

    for (i = 0; i < EIP130TOKEN_COMMAND_WORDS; i++)
    {
        CommandToken_p->W[i] = 0xAAAAAAAA;
    }
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Identity
 *
 * This function sets the token identity.
 *
 * CommandToken_p
 *      Pointer to the command token buffer.
 *
 * Identity
 *      A 32-bit value to identify the user.
 */
void
Eip130Token_Command_Identity(
        Eip130Token_Command_t * const CommandToken_p,
        uint32_t Identity)
{
    CommandToken_p->W[1] = Identity;
}



/*----------------------------------------------------------------------------
 * Eip130Token_Command_SetTokenID
 *
 * This function sets the TokenID related field in the Command Token. The SM
 * will copy this value to the Result Token.
 * This can also be used for data stream synchronization: the TokenID is
 * appended to the end of the DMA stream. This word must be initialized to a
 * different value and then polled until the expected TokenID value/pattern
 * shows up.
 *
 * CommandToken_p
 *      Pointer to the command token buffer.
 *
 * TokenIDValue
 *      The 16 bit TokenID value that the SM will write to the Result Token
 *      and optionally appends to the end of the output DMA data stream.
 *
 * fWriteTokenID
 *      false  do not append TokenID to DMA data stream.
 *      true   write TokenID at end of DMA data stream. The stream will first
 *             be padded with zeros until a complete 32bit word before an extra
 *             32bit word is output with the TokenID in it.
 */
void
Eip130Token_Command_SetTokenID(
        Eip130Token_Command_t * const CommandToken_p,
        const uint16_t TokenIDValue,
        const bool fWriteTokenID)
{
    // Replace TokenID field (word 0, lowest 16 bits) with TokenIDValue aand
    // reset Write Token ID indication
    CommandToken_p->W[0] &= ((MASK_16_BITS << 16) - BIT_18);
    CommandToken_p->W[0] |= TokenIDValue;
    if (fWriteTokenID)
    {
        // Set Write Token ID field (word 0, bit 18)
        CommandToken_p->W[0] |= BIT_18;
    }
}



/*----------------------------------------------------------------------------
 * Eip130Token_Result_Code
 *
 * This function extracts the error or warning details from the result token.
 * This function can be used on any result token.
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * Return Value
 *     0    No error or warning
 *     <0   Error code
 *     >0   warning code
 */
int
Eip130Token_Result_Code(
        Eip130Token_Result_t * const ResultToken_p)
{
    int rv = (int)(ResultToken_p->W[0] >> 24);
    if (rv & BIT_7)
    {
        return -(rv & MASK_7_BITS);
    }
    return rv;
}



