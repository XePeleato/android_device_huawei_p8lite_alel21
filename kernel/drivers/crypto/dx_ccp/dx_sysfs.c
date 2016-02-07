/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents, and distributed under multiple licenses.   *
* Any use of this Software as part of the Discretix CryptoCell or  *
* Packet Engine products requires a commercial license.            *
* Copies of this Software that are distributed with the Discretix  *
* CryptoCell or Packet Engine product drivers, may be used in      *
* accordance with a commercial license, or at the user's option,   *
* used and redistributed under the terms and conditions of the GNU *
* General Public License ("GPL") version 2, as published by the    *
* Free Software Foundation.                                        *
* This program is distributed in the hope that it will be useful,  *
* but WITHOUT ANY LIABILITY AND WARRANTY; without even the implied *
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
* See the GNU General Public License version 2 for more details.   *
* You should have received a copy of the GNU General Public        *
* License version 2 along with this Software; if not, please write *
* to the Free Software Foundation, Inc., 59 Temple Place - Suite   *
* 330, Boston, MA 02111-1307, USA.                                 *
* Any copy or reproduction of this Software, as permitted under    *
* the GNU General Public License version 2, must include this      *
* Copyright Notice as well as any other notices provided under     *
* the said license.                                                *
********************************************************************/

#include <linux/kernel.h>
#include <asm/timex.h>

#include "dx_config.h"
#include "dx_driver.h"
#include "dx_sysfs.h"

#ifdef ENABLE_CYCLE_COUNT

static DEFINE_SPINLOCK(stat_lock);

struct stat_item {
	unsigned int min;
	unsigned int max;
	cycles_t sum;
	unsigned int count;
};

struct stat_name {
	const char *op_type_name;
	const char *stat_phase_name[MAX_STAT_PHASES];
};

static struct stat_name stat_name_db[MAX_STAT_OP_TYPES] =
{
	{
		.op_type_name = "Encode",
		.stat_phase_name[STAT_PHASE_0] = "Init and sanity checks",
		.stat_phase_name[STAT_PHASE_1] = "Map buffers",
		.stat_phase_name[STAT_PHASE_2] = "Create sequence",
		.stat_phase_name[STAT_PHASE_3] = "Send Request",
		.stat_phase_name[STAT_PHASE_4] = "HW-Q push",
		.stat_phase_name[STAT_PHASE_5] = "Sequence completion",
	},
	{	.op_type_name = "Decode",
		.stat_phase_name[STAT_PHASE_0] = "Init and sanity checks",
		.stat_phase_name[STAT_PHASE_1] = "Map buffers",
		.stat_phase_name[STAT_PHASE_2] = "Create sequence",
		.stat_phase_name[STAT_PHASE_3] = "Send Request",
		.stat_phase_name[STAT_PHASE_4] = "HW-Q push",
		.stat_phase_name[STAT_PHASE_5] = "Sequence completion",
	},
	{	.op_type_name = "Setkey",
		.stat_phase_name[STAT_PHASE_0] = "Init and sanity checks",
		.stat_phase_name[STAT_PHASE_1] = "Copy key to ctx",
		.stat_phase_name[STAT_PHASE_2] = "Create sequence",
		.stat_phase_name[STAT_PHASE_3] = "Send Request",
		.stat_phase_name[STAT_PHASE_4] = "HW-Q push",
		.stat_phase_name[STAT_PHASE_5] = "Sequence completion",
	},
	{
		.op_type_name = "Generic",
		.stat_phase_name[STAT_PHASE_0] = "Interrupt",
		.stat_phase_name[STAT_PHASE_1] = "ISR-to-Tasklet",
		.stat_phase_name[STAT_PHASE_2] = "Tasklet start-to-end",
		.stat_phase_name[STAT_PHASE_3] = "Tasklet:user_cb()",
		.stat_phase_name[STAT_PHASE_4] = "Tasklet:dx_X_complete() - w/o X_complete()",
		.stat_phase_name[STAT_PHASE_5] = "",
	}
};

static struct stat_item stat_db[MAX_STAT_OP_TYPES][MAX_STAT_PHASES];

#endif


void init_stat_db(void)
{
#ifdef ENABLE_CYCLE_COUNT

	unsigned int i,j;

	/* Clear all DB */
	for (i=0; i<MAX_STAT_OP_TYPES; i++) {
		for (j=0; j<MAX_STAT_PHASES; j++) {
			stat_db[i][j].min = 0xFFFFFFFF;
			stat_db[i][j].max = 0;
			stat_db[i][j].sum = 0;
			stat_db[i][j].count = 0;
		}
	}
#endif
}

void update_stat(unsigned int op_type, unsigned int phase, cycles_t result)
{
#ifdef ENABLE_CYCLE_COUNT
	unsigned long flags;

	spin_lock_irqsave(&stat_lock, flags);
	stat_db[op_type][phase].count++;
	stat_db[op_type][phase].sum += result;
	if ( (unsigned int)result < stat_db[op_type][phase].min )
		stat_db[op_type][phase].min = (unsigned int)result;
	if ( (unsigned int)result > stat_db[op_type][phase].max )
		stat_db[op_type][phase].max = (unsigned int)result;
	spin_unlock_irqrestore(&stat_lock, flags);
#endif
}

void display_stat_db(void)
{
#ifdef ENABLE_CYCLE_COUNT

	unsigned int i,j;
	uint64_t avg;

	for (i=0; i<MAX_STAT_OP_TYPES; i++) {
		for (j=0; j<MAX_STAT_PHASES; j++) {
			if (stat_db[i][j].count) {
				avg = (uint64_t)stat_db[i][j].sum;
				do_div(avg, stat_db[i][j].count);
				DX_LOG_ERR("%s, %s: min=%d avg=%d max=%d sum=%lld count=%d\n",
					stat_name_db[i].op_type_name, stat_name_db[i].stat_phase_name[j],
					stat_db[i][j].min, (int)avg, stat_db[i][j].max, (long long)stat_db[i][j].sum, stat_db[i][j].count);
			}
		}
	}
#endif
}
