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

#ifndef __EFI_NVIDIA_I2C_LIB_H__
#define __EFI_NVIDIA_I2C_LIB_H__

#define I2C_1 0
#define I2C_2 1
#define I2C_3 2
#define I2C_4 3
#define I2C_5 4
#define I2C_6 5

VOID i2c_init(UINT32 idx);
UINT32 i2c_send_buf_small(UINT32 idx, UINT32 x, UINT32 y, UINT8 *buf, UINT32 size);
int i2c_recv_buf_small(UINT8 *buf, UINT32 size, UINT32 idx, UINT32 x, UINT32 y);
UINT32 i2c_send_byte(UINT32 idx, UINT32 x, UINT32 y, UINT8 b);
UINT8 i2c_recv_byte(UINT32 idx, UINT32 x, UINT32 y);

#endif