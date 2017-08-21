/*
 * Copyright (C) 2012 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef _ET4007_H_
#define _ET4007_H_

#define MAX_SIZE 1052
#define MAX_INDEX 64
#define MAX_SAMPLE 16
#define MAX_SAMPLE_INDEX 32
#define MAX_DATA 310
#define MAX_SEND_DATA 380

struct ir_remocon_data {
	struct mutex mutex;
	u32 clk_gpio;
	u32 sda_gpio;
	u32 busy_gpio;
	char *name;
	uint8_t signal[MAX_SEND_DATA+2];
	unsigned int original[MAX_SIZE];
	uint16_t sample[MAX_INDEX];
	uint8_t zp_sample[MAX_INDEX*2];
	uint8_t data[MAX_DATA*2];
	uint32_t orig_freq;
	uint8_t freq;
	int length;
	int count;
	int couple;
	int data_count;
	int	index;
	uint8_t	crc;
	int ir_sum;
	uint8_t send_flag;
};

struct remote_data {
	uint16_t high_level;
	uint16_t low_level;
};

struct remote_desc {
	int gpio;
	char *name;
	struct timer_list timer;
};

#endif /* _IR_REMOTE_ET4007_H_ */
