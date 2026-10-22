/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#جيڪڏهن_وصف_نه_ٿيل FDT_H
#وصف_ڪريو FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

بنية fdt_سرو {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_لڙيون;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t نسخو;		 /* format version */
	fdt32_t آخري_comp_نسخو;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t سائيز_dt_لڙيون;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t سائيز_dt_struct;		 /* size of the structure block */
};

بنية fdt_reserve_داخلا {
	fdt64_t پتو;
	fdt64_t سائيز;
};

بنية fdt_ڳنڍ_سرو {
	fdt32_t ٽيگ;
	حرف name[0];
};

بنية fdt_property {
	fdt32_t ٽيگ;
	fdt32_t len;
	fdt32_t nameoff;
	حرف معلومات[0];
};

#جيڪڏهن_پورو /* !__ASSEMBLY */

#وصف_ڪريو FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#وصف_ڪريو FDT_TAGSIZE	حجم(fdt32_t)

#وصف_ڪريو FDT_BEGIN_ڳنڍ	0x1		/* Start node: full name */
#وصف_ڪريو FDT_پڇاڙي_ڳنڍ	0x2		/* End node */
#وصف_ڪريو FDT_PROP	0x3		/* Property: name off,
					   size, content */
#وصف_ڪريو FDT_NOP		0x4		/* nop */
#وصف_ڪريو FDT_پڇاڙي		0x9

#وصف_ڪريو FDT_V1_سائيز	(7*حجم(fdt32_t))
#وصف_ڪريو FDT_V2_سائيز	(FDT_V1_سائيز + حجم(fdt32_t))
#وصف_ڪريو FDT_V3_سائيز	(FDT_V2_سائيز + حجم(fdt32_t))
#وصف_ڪريو FDT_V16_سائيز	FDT_V3_سائيز
#وصف_ڪريو FDT_V17_سائيز	(FDT_V16_سائيز + حجم(fdt32_t))

#جيڪڏهن_پورو /* FDT_H */
