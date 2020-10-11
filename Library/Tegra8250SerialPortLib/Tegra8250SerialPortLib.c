// Tegra8250SerialPortLib.c: Coreboot-ported serial library
// before I figuring out how to use the builtin 16550 serial library.

#include <PiDxe.h>

#include <Library/ArmLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/SerialPortLib.h>

#include "Serial.h"

static struct tegra210_uart * const uart_ptr = (void *) CONFIG_CONSOLE_SERIAL_TEGRA210_UART_ADDRESS;

static void tegra210_uart_tx_flush(void);
static int tegra210_uart_tst_byte(void);

static void tegra210_uart_init(void)
{
	// Use a hardcoded divisor for now.
	const unsigned divisor = 221;
	const UINT8 line_config = UART8250_LCR_WLS_8; // 8n1

	tegra210_uart_tx_flush();

	// Disable interrupts.
	write8(&uart_ptr->ier, 0);
	// Force DTR and RTS to high.
	write8(&uart_ptr->mcr, UART8250_MCR_DTR | UART8250_MCR_RTS);
	// Set line configuration, access divisor latches.
	write8(&uart_ptr->lcr, UART8250_LCR_DLAB | line_config);
	// Set the divisor.
	write8(&uart_ptr->dll, divisor & 0xff);
	write8(&uart_ptr->dlm, (divisor >> 8) & 0xff);
	// Hide the divisor latches.
	write8(&uart_ptr->lcr, line_config);
	// Enable FIFOs, and clear receive and transmit.
	write8(&uart_ptr->fcr,
		UART8250_FCR_FIFO_EN |
		UART8250_FCR_CLEAR_RCVR |
		UART8250_FCR_CLEAR_XMIT);
}

static void tegra210_uart_tx_byte(unsigned char data)
{
	while (!(read8(&uart_ptr->lsr) & UART8250_LSR_THRE));
	write8(&uart_ptr->thr, data);
}

static void tegra210_uart_tx_flush(void)
{
	while (!(read8(&uart_ptr->lsr) & UART8250_LSR_TEMT));
}

static unsigned char tegra210_uart_rx_byte(void)
{
	if (!tegra210_uart_tst_byte())
		return 0;
	return read8(&uart_ptr->rbr);
}

static int tegra210_uart_tst_byte(void)
{
	return (read8(&uart_ptr->lsr) & UART8250_LSR_DR) == UART8250_LSR_DR;
}

RETURN_STATUS
EFIAPI
SerialPortInitialize
(
	VOID
)
{
	tegra210_uart_init();
	return RETURN_SUCCESS;
}

UINTN
EFIAPI
SerialPortWrite
(
	IN UINT8     *Buffer,
	IN UINTN     NumberOfBytes
)
{
	UINT8* CONST Final = &Buffer[NumberOfBytes];

	while (Buffer < Final)
	{
		tegra210_uart_tx_byte(*Buffer++);
	}

	return NumberOfBytes;
}

UINTN
EFIAPI
SerialPortRead
(
	OUT UINT8     *Buffer,
	IN  UINTN     NumberOfBytes
)
{
	UINTN  Result;

	if (NULL == Buffer) {
    	return 0;
  	}

	for (Result = 0; NumberOfBytes-- != 0; Result++, Buffer++) {
		//
    	// Wait for the serial port to have some data.
    	//
		while (tegra210_uart_tst_byte() == 0) {
		}
		
		//
    	// Read byte from the receive buffer.
    	//
    	*Buffer = read8(&uart_ptr->rbr);
	}

	return Result;
}

BOOLEAN
EFIAPI
SerialPortPoll
(
	VOID
)
{
	if (tegra210_uart_tst_byte() != 0) {
		return TRUE;
	}

	return FALSE;
}

RETURN_STATUS
EFIAPI
SerialPortSetControl
(
	IN UINT32 Control
)
{
	return RETURN_UNSUPPORTED;
}

RETURN_STATUS
EFIAPI
SerialPortGetControl
(
	OUT UINT32 *Control
)
{
	return RETURN_UNSUPPORTED;
}

RETURN_STATUS
EFIAPI
SerialPortSetAttributes
(
	IN OUT UINT64             *BaudRate,
	IN OUT UINT32             *ReceiveFifoDepth,
	IN OUT UINT32             *Timeout,
	IN OUT EFI_PARITY_TYPE    *Parity,
	IN OUT UINT8              *DataBits,
	IN OUT EFI_STOP_BITS_TYPE *StopBits
)
{
	return RETURN_UNSUPPORTED;
}

UINTN SerialPortFlush(VOID)
{
	tegra210_uart_tx_flush();
	return 0;
}

VOID
EnableSynchronousSerialPortIO(VOID)
{
	// Already synchronous
}