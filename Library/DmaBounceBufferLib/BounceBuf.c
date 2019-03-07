#include <PiDxe.h>

#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/Utc/ErrNo.h>
#include <Library/Utc/BounceBuf.h>
#include <Foundation/Types.h>
#include <Shim/Kernel.h>
#include <Shim/DebugLib.h>
#include <Shim/MemAlign.h>

STATIC UINTN LowMemoryTop = FixedPcdGet64(PcdSystemMemoryBase) + SIZE_1GB;

/**
  Allocates one or more 4KB pages of a certain memory type at a specified alignment within 2GB memory space.

  Allocates the number of 4KB pages specified by Pages of a certain memory type with an alignment
  specified by Alignment.  The allocated buffer is returned.  If Pages is 0, then NULL is returned.
  If there is not enough memory at the specified alignment remaining to satisfy the request, then
  NULL is returned.
  If Alignment is not a power of two and Alignment is not zero, then ASSERT().
  If Pages plus EFI_SIZE_TO_PAGES (Alignment) overflows, then ASSERT().

  @param  MemoryType            The type of memory to allocate.
  @param  Pages                 The number of 4 KB pages to allocate.
  @param  Alignment             The requested alignment of the allocation.  Must be a power of two.
                                If Alignment is zero, then byte alignment is used.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
InternalAllocateAlignedPages32 (
	IN EFI_MEMORY_TYPE  MemoryType,  
	IN UINTN            Pages,
	IN UINTN            Alignment
)
{
	EFI_STATUS            Status;
	EFI_PHYSICAL_ADDRESS  Memory;
	UINTN                 AlignedMemory;
	UINTN                 AlignmentMask;
	UINTN                 UnalignedPages;
	UINTN                 RealPages;

	Memory = LowMemoryTop;

	//
	// Alignment must be a power of two or zero.
	//
	ASSERT ((Alignment & (Alignment - 1)) == 0);
	
	if (Pages == 0) return NULL;
	if (Alignment > EFI_PAGE_SIZE) 
	{
		//
		// Calculate the total number of pages since alignment is larger than page size.
		//
		AlignmentMask  = Alignment - 1;
		RealPages      = Pages + EFI_SIZE_TO_PAGES(Alignment);
		//
		// Make sure that Pages plus EFI_SIZE_TO_PAGES (Alignment) does not overflow.
		//
		ASSERT (RealPages > Pages);
	
		Status = gBS->AllocatePages (AllocateMaxAddress, MemoryType, RealPages, &Memory);
		if (EFI_ERROR (Status)) return NULL;
		AlignedMemory  = ((UINTN) Memory + AlignmentMask) & ~AlignmentMask;
		UnalignedPages = EFI_SIZE_TO_PAGES (AlignedMemory - (UINTN) Memory);
		if (UnalignedPages > 0) 
		{
			//
			// Free first unaligned page(s).
			//
			Status = gBS->FreePages (Memory, UnalignedPages);
			ASSERT_EFI_ERROR (Status);
		}
		Memory         = AlignedMemory + EFI_PAGES_TO_SIZE (Pages);
		UnalignedPages = RealPages - Pages - UnalignedPages;
		if (UnalignedPages > 0) 
		{
			//
			// Free last unaligned page(s).
			//
			Status = gBS->FreePages (Memory, UnalignedPages);
			ASSERT_EFI_ERROR (Status);
		}
	} 
	else 
	{
		//
		// Do not over-allocate pages in this case.
		//
		Status = gBS->AllocatePages(AllocateMaxAddress, MemoryType, Pages, &Memory);
		if (EFI_ERROR (Status)) return NULL;
		AlignedMemory  = (UINTN) Memory;
	}
	return (VOID *) AlignedMemory;
}

/**
  Allocates one or more 4KB pages of type EfiBootServicesData at a specified alignment within 2GB space.

  Allocates the number of 4KB pages specified by Pages of type EfiBootServicesData with an
  alignment specified by Alignment.  The allocated buffer is returned.  If Pages is 0, then NULL is
  returned.  If there is not enough memory at the specified alignment remaining to satisfy the
  request, then NULL is returned.
  
  If Alignment is not a power of two and Alignment is not zero, then ASSERT().
  If Pages plus EFI_SIZE_TO_PAGES (Alignment) overflows, then ASSERT().

  @param  Pages                 The number of 4 KB pages to allocate.
  @param  Alignment             The requested alignment of the allocation.  Must be a power of two.
                                If Alignment is zero, then byte alignment is used.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocateAlignedPages32 (
  IN UINTN  Pages,
  IN UINTN  Alignment
  )
{
	VOID  *Buffer;

	Buffer = InternalAllocateAlignedPages32(EfiBootServicesData, Pages, Alignment);
	return Buffer;
}

/**
	Frees one or more 4KB pages that were previously allocated with one of the aligned page
	allocation functions in the Memory Allocation Library.

	Frees the number of 4KB pages specified by Pages from the buffer specified by Buffer.  Buffer
	must have been allocated on a previous call to the aligned page allocation services of the Memory
	Allocation Library.  If it is not possible to free allocated pages, then this function will 
	perform no actions.
	
	If Buffer was not allocated with an aligned page allocation function in the Memory Allocation
	Library, then ASSERT().
	If Pages is zero, then ASSERT().
	
	@param  Buffer                The pointer to the buffer of pages to free.
	@param  Pages                 The number of 4 KB pages to free.

**/
VOID
EFIAPI
FreeAlignedPages32(
	IN VOID   *Buffer,
	IN UINTN  Pages
)
{
	EFI_STATUS  Status;

	ASSERT (Pages != 0);
	Status = gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) Buffer, Pages);
	ASSERT_EFI_ERROR (Status);
}

static BOOLEAN addr_aligned(struct bounce_buffer *state)
{
	const ulong align_mask = ARCH_DMA_MINALIGN - 1;

	/* Check if start is aligned */
	if ((ulong) state->user_buffer & align_mask) {
		debug("Unaligned buffer address \n");
		return FALSE;
	}

	/* Check if length is aligned */
	if (state->len != state->len_aligned) {
		debug("Unaligned buffer length \n");
		return FALSE;
	}

	/* Aligned */
	return TRUE;
}

static BOOLEAN addr_lower_32bit(struct bounce_buffer *state)
{
	if (((UINTN) state->bounce_buffer) < LowMemoryTop)
	{
		return TRUE;
	}

	return FALSE;
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

	if (!addr_aligned(state) || !addr_lower_32bit(state)) 
	{
		state->bounce_buffer = AllocateAlignedPages32(
			EFI_SIZE_TO_PAGES(state->len_aligned), ARCH_DMA_MINALIGN);
		if (!state->bounce_buffer) return -ENOMEM;

		if (state->flags & GEN_BB_READ)
			CopyMem(state->bounce_buffer, state->user_buffer, state->len);
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

	FreeAlignedPages32(state->bounce_buffer, EFI_SIZE_TO_PAGES(state->len_aligned));

	return 0;
}
