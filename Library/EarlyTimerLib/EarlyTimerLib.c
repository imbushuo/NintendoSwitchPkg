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

#include <PiDxe.h>

#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>

#include <Foundation/Types.h>
#include <Library/EarlyTimerLib.h>
#include <Device/T210.h>

u32 get_tmr()
{
	return TMR(TMR_US_OFFS);
}

void sleep(u32 ticks)
{
	u32 start = TMR(TMR_US_OFFS);
	while (TMR(TMR_US_OFFS) - start <= ticks) {}
}
