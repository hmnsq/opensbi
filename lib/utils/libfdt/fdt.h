/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ifndef FDT_H
#define FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ifndef __ASSEMBLY__

কাঠামো fdt_header {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_strings;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t version;		 /* format version */
	fdt32_t last_comp_version;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t আকার_dt_strings;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t আকার_dt_struct;		 /* size of the structure block */
};

কাঠামো fdt_reserve_entry {
	fdt64_t address;
	fdt64_t আকার_2;
};

কাঠামো fdt_গ্রন্থি_header {
	fdt32_t tag;
	অক্ষর name[0];
};

কাঠামো fdt_property {
	fdt32_t tag;
	fdt32_t len;
	fdt32_t nameoff;
	অক্ষর তথ্য[0];
};

#endif /* !__ASSEMBLY */

#define FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#define FDT_TAGSIZE	আকার(fdt32_t)

#define FDT_BEGIN_গ্রন্থি	0x1		/* Start node: full name */
#define FDT_END_গ্রন্থি	0x2		/* End node */
#define FDT_PROP	0x3		/* Property: name off,
					   size, content */
#define FDT_NOP		0x4		/* nop */
#define FDT_END		0x9

#define FDT_V1_আকার	(7*আকার(fdt32_t))
#define FDT_V2_আকার	(FDT_V1_আকার + আকার(fdt32_t))
#define FDT_V3_আকার	(FDT_V2_আকার + আকার(fdt32_t))
#define FDT_V16_আকার	FDT_V3_আকার
#define FDT_V17_আকার	(FDT_V16_আকার + আকার(fdt32_t))

#endif /* FDT_H */
