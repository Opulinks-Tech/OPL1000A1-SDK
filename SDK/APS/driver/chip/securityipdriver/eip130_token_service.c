#include "eip130_token_service.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_RegisterWrite
 *
 * This token can be used to write the DMA controller configuration register.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Address
 *     Address to write.
 *
 * Value
 *     Value to write.
 */
void
Eip130Token_Command_RegisterWrite(
        Eip130Token_Command_t * const CommandToken_p,
        const uint16_t Address,
        const uint32_t Value)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SERVICE << 24) |
                           (EIP130TOKEN_SUBCODE_REGISTERWRITE << 28);
    CommandToken_p->W[2] = 1;           // only one word
    CommandToken_p->W[3] = 0;           // Mask = 0 = Write all 32 bits
    CommandToken_p->W[4] = Address;     // Address to write to
    CommandToken_p->W[5] = Value;       // Value to write
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_RegisterRead
 *
 * This token can be used to read the DMA controller configuration register.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Address
 *     Address to read.
 */
void
Eip130Token_Command_RegisterRead(
        Eip130Token_Command_t * const CommandToken_p,
        const uint16_t Address)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SERVICE << 24) |
                           (EIP130TOKEN_SUBCODE_REGISTERREAD << 28);
    CommandToken_p->W[2] = 1;           // only one word
    CommandToken_p->W[3] = Address;     // Address to read from
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_RegisterRead
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * Value_p
 *      Pointer to the variable in which the value must be returned.
 */
void
Eip130Token_Result_RegisterRead(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const Value_p)
{
    *Value_p = ResultToken_p->W[1];
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_ZeroizeOutputMailbox
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_ZeroizeOutputMailbox(
        Eip130Token_Command_t * const CommandToken_p)
{
    //Eip130Token_Command_Clear(CommandToken_p);
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SERVICE << 24) |
                           (EIP130TOKEN_SUBCODE_ZEROOUTMAILBOX << 28);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_ClockSwitch
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * ClocksForcedOn
 *     Bitmask specifying (with 1b’s) the clocks that needs to be forced ON
 *     (always ON).
 *
 * ClocksForcedOff
 *     Bitmask specifying (with 1b’s) the clocks that needs to be forced OFF
 *     (always OFF).
 */
void
Eip130Token_Command_ClockSwitch(
        Eip130Token_Command_t * const CommandToken_p,
        const uint16_t ClocksForcedOn,
        const uint16_t ClocksForcedOff)
{
    //Eip130Token_Command_Clear(CommandToken_p);
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SERVICE << 24) |
                           (EIP130TOKEN_SUBCODE_CLOCKSWITCH << 28);
    CommandToken_p->W[2] = (((uint32_t)ClocksForcedOff) << 16) |
                           (uint32_t)ClocksForcedOn;
}


