#include "Utils.h"

static unsigned char Write_Buffer[MAX_WRITE_LEN];

WRITE_info Write_Type = PLATFORM_WRITE_ALL;

void board_write(const char *fmt, ... )
{
    va_list 	arg_ptr;
	uint32_t	length;

    /* Initial board_write to temporary buffer.. at least try some sort of sanity check so we don't write all over
     * memory if the print is too large.
     */
    if (strlen(fmt) > MAX_WRITE_LEN)
    	return;

    va_start( arg_ptr, fmt );
    length = vsprintf( (char *)Write_Buffer, fmt, arg_ptr );
    va_end( arg_ptr );

	if ((Write_Type == PLATFORM_WRITE_PRINTF) || (Write_Type == PLATFORM_WRITE_ALL))
	{
		printf( "%s", Write_Buffer );
	}

	if ((Write_Type == PLATFORM_WRITE_UART) || (Write_Type == PLATFORM_WRITE_ALL))
	{
		KeyStone_UART_write(Write_Buffer, length, UART_NUM);
		KeyStone_UART_TX_wait(UART_NUM);
	}

	return;
}
