/*
* Copyright (c) 2018 naehrwert
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public License,
* version 2, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _TYPES_H_
#define _TYPES_H_

typedef INT8 s8;
typedef INT16 s16;
typedef INT32 s32;
typedef INT64 s64;
typedef UINT8 u8;
typedef UINT16 u16;
typedef UINT32 u32;
typedef UINT64 u64;
typedef volatile UINT8 vu8;
typedef volatile UINT16 vu16;
typedef volatile UINT32 vu32;
typedef volatile UINT64 vu64;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef UINTN size_t;
typedef EFI_PHYSICAL_ADDRESS uintptr_t;
typedef BOOLEAN bool;

#define true TRUE
#define false FALSE

#define memcpy(s1, s2, n) CopyMem((s1), (s2), (n))
#define memmove(s1, s2, n) CopyMem((s1), (s2), (n))
#define memcmp(s1, s2, n) ((int)CompareMem((s1), (s2), (n)))

#define writel(v, a) MmioWrite32((UINTN)(a), (UINT32)(v))
#define readl(a) MmioRead32((UINTN)(a))
#define writeb(v, a) MmioWrite8((UINTN)(a), (UINT8)(v))
#define readb(a) MmioRead8((UINTN)(a))
#define writehw(v, a) MmioWrite16((UINTN)(a), (UINT16)(v))
#define readhw(a) MmioRead16((UINTN)(a))
#define writew(v, a) MmioWrite16((UINTN)(a), (UINT16)(v))
#define readw(a) MmioRead16((UINTN)(a))
#define RMWREG32(addr, startbit, width, val) writel((readl(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit)), addr)

#define do_div(n,base) ({					\
	UINT32 __base = (base);				\
	UINT32 __rem;						\
	__rem = ((UINT64)(n)) % __base;			\
	(n) = ((UINT64)(n)) / __base;				\
	__rem;							\
 })

/* Wrapper for do_div(). Doesn't modify dividend and returns
 * the result, not reminder.
 */
static inline UINT64 lldiv(UINT64 dividend, UINT64 divisor)
{
	UINT64 __res = dividend;
	do_div(__res, divisor);
	return(__res);
}

#ifndef BIT
#define BIT(nr)			(1UL << (nr))
#endif

#endif
