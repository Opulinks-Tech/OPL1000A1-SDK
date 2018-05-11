#include "eip130_token_nop.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Nop
 *
 * CommandToken_p
 *      Pointer to the command token buffer.
 *
 * InputDataAddress
 *     Address of the input data buffer.
 *
 * InputDataLengthInBytes
 *     Size of the input data buffer being the number of bytes to copy.
 *     Must be a multiple of 4.
 *
 * OutputDataAddress
 *     Address of the output data buffer.
 *
 * OutputDataLengthInBytes
 *     Size of the output data buffer.
 *     Must be a multiple of 4.
 */
void
Eip130Token_Command_Nop(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t InputDataAddress,
        const uint32_t InputDataLengthInBytes,
        const uint64_t OutputDataAddress,
        const uint32_t OutputDataLengthInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_NOP << 24);
    CommandToken_p->W[2] = InputDataLengthInBytes;
    CommandToken_p->W[3] = (uint32_t)(InputDataAddress);
    CommandToken_p->W[4] = (uint32_t)(InputDataAddress >> 32);
    CommandToken_p->W[5] = InputDataLengthInBytes;
    CommandToken_p->W[6] = (uint32_t)(OutputDataAddress);
    CommandToken_p->W[7] = (uint32_t)(OutputDataAddress >> 32);
    CommandToken_p->W[8] = OutputDataLengthInBytes;
}



