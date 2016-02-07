/*
 * rdr area header file (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef	__RDR_AREA_H__
#define __RDR_AREA_H__

/*
 * Area size array: unit is BYTES, it should be 4 bytes align.
 * NOTES: before modify this file, you should sync the code from the lib first.
 * The followed macros r the area byte counts, if u modify the area size,
 * add notes: cpu-name/author/date, please.
 * If u'll modify the area which is not zero,please ask for the orig author.
 */
/* RDR area 0 begin: */
#define RDR_AREA_0 (0x400000 - 0x200)
/* RDR area 0 end.   */

/* RDR area 1 begin: */
#define RDR_AREA_1 (0x480000 - 0x400000)/*reserved,512KB,w00204535 2013.12.26*/
/* RDR area 1 end.   */

/* RDR area 2 begin: */
#define RDR_AREA_2 (0x600000 - 0x480000)/*CP-BBE16,1536KB,w00204535 2013.12.26*/
/* RDR area 2 end.   */

/* RDR area 3 begin: */
#define RDR_AREA_3 (0x680000 - 0x600000)/*TeeOS,512KB,w00204535 2013.12.26*/
/* RDR area 3 end.   */

/* RDR area 4 begin: */
#define RDR_AREA_4 (0x700000 - 0x680000)/*HIFI,512KB,w00204535 2013.12.26*/
/* RDR area 4 end.   */

/* RDR area 5 begin: */
#define RDR_AREA_5 (0x780000 - 0x700000)/*LPM3,512KB,w00204535 2013.12.26*/
/* RDR area 5 end.   */

/* RDR area 6 begin: */
#define RDR_AREA_6 (0x7F8000 - 0x780000)/*IOM3,480KB,w00204535 2013.12.26*/
/* RDR area 6 end.   */

/* RDR area 7 begin: */
#define RDR_AREA_7 0
/* RDR area 7 end.   */

#endif	/* End #define __RDR_AREA_H__ */
