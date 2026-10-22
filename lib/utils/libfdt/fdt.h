/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ifndef FDT_H
#define FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ifndef __ASSEMBLY__

संरचना fdt_header {
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
	fdt32_t size_dt_strings;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t size_dt_struct;		 /* size of the structure block */
};

संरचना fdt_reserve_entry {
	fdt64_t address;
	fdt64_t size;
};

संरचना fdt_ग्रन्थिः_header {
	fdt32_t tag;
	वर्ण name[0];
};

संरचना fdt_property {
	fdt32_t tag;
	fdt32_t len;
	fdt32_t nameoff;
	वर्ण data[0];
};

#endif /* !__ASSEMBLY */

#define FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#define FDT_TAGSIZE	परिमाण(fdt32_t)

#define FDT_BEGIN_ग्रन्थिः	0x1		/* Start node: full name */
#define FDT_END_ग्रन्थिः	0x2		/* End node */
#define FDT_PROP	0x3		/* Property: name off,
					   size, content */
#define FDT_NOP		0x4		/* nop */
#define FDT_END		0x9

#define FDT_V1_SIZE	(7*परिमाण(fdt32_t))
#define FDT_V2_SIZE	(FDT_V1_SIZE + परिमाण(fdt32_t))
#define FDT_V3_SIZE	(FDT_V2_SIZE + परिमाण(fdt32_t))
#define FDT_V16_SIZE	FDT_V3_SIZE
#define FDT_V17_SIZE	(FDT_V16_SIZE + परिमाण(fdt32_t))

#endif /* FDT_H */
