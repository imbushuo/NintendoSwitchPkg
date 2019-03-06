#include <PiDxe.h>

#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/Utc/ErrNo.h>
#include <Library/Utc/BounceBuf.h>
#include <Foundation/Types.h>
#include <Shim/Kernel.h>
#include <Shim/DebugLib.h>
#include <Shim/MemAlign.h>

static int addr_aligned(struct bounce_buffer *state)
{
	const ulong align_mask = ARCH_DMA_MINALIGN - 1;

	/* Check if start is aligned */
	if ((ulong) state->user_buffer & align_mask) {
		debug("Unaligned buffer address %p\n", state->user_buffer);
		return 0;
	}

	/* Check if length is aligned */
	if (state->len != state->len_aligned) {
		debug("Unaligned buffer length %zu\n", state->len);
		return 0;
	}

	/* Aligned */
	return 1;
}

int bounce_buffer_start(
    struct bounce_buffer *state, void *data,
    UINTN len, unsigned int flags)
{
	state->user_buffer = data;
	state->bounce_buffer = data;
	state->len = len;
	state->len_aligned = roundup(len, ARCH_DMA_MINALIGN);
	state->flags = flags;

	if (!addr_aligned(state)) {
		state->bounce_buffer = AllocateAlignedPages(
            EFI_SIZE_TO_PAGES(state->len_aligned), ARCH_DMA_MINALIGN);
		if (!state->bounce_buffer)
			return -ENOMEM;

		if (state->flags & GEN_BB_READ)
			CopyMem(state->bounce_buffer, state->user_buffer,
				state->len);
	}

	/*
	 * Flush data to RAM so DMA reads can pick it up,
	 * and any CPU writebacks don't race with DMA writes
	 */
	WriteBackDataCacheRange(
        state->bounce_buffer,
		state->len_aligned
    );

	return 0;
}

int bounce_buffer_stop(struct bounce_buffer *state)
{
	if (state->flags & GEN_BB_WRITE) {
		/* Invalidate cache so that CPU can see any newly DMA'd data */
		InvalidateDataCacheRange(
            state->bounce_buffer,
			state->len_aligned
        );
	}

	if (state->bounce_buffer == state->user_buffer)
		return 0;

	if (state->flags & GEN_BB_WRITE)
		CopyMem(state->user_buffer, state->bounce_buffer, state->len);

	FreeAlignedPages(state->bounce_buffer, EFI_SIZE_TO_PAGES(state->len_aligned));

	return 0;
}
