/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#که_نه_تعریف_شوی FDT_H
#تعریف_کړه FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#که_نه_تعریف_شوی __ASSEMBLY__

بنية fdt_سرلیک {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_مزي_ګانې;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t نسخه;		 /* format version */
	fdt32_t وروستی_comp_نسخه;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t اندازه_dt_مزي_ګانې;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t اندازه_dt_struct;		 /* size of the structure block */
};

بنية fdt_reserve_ثبت {
	fdt64_t پته;
	fdt64_t اندازه;
};

بنية fdt_غوټه_سرلیک {
	fdt32_t نښان;
	حرف name[0];
};

بنية fdt_property {
	fdt32_t نښان;
	fdt32_t len;
	fdt32_t nameoff;
	حرف معلومات[0];
};

#د_که_پای /* !__ASSEMBLY */

#تعریف_کړه FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#تعریف_کړه FDT_TAGSIZE	حجم(fdt32_t)

#تعریف_کړه FDT_BEGIN_غوټه	0x1		/* Start node: full name */
#تعریف_کړه FDT_پای_ټکی_غوټه	0x2		/* End node */
#تعریف_کړه FDT_PROP	0x3		/* Property: name off,
					   size, content */
#تعریف_کړه FDT_NOP		0x4		/* nop */
#تعریف_کړه FDT_پای_ټکی		0x9

#تعریف_کړه FDT_V1_اندازه	(7*حجم(fdt32_t))
#تعریف_کړه FDT_V2_اندازه	(FDT_V1_اندازه + حجم(fdt32_t))
#تعریف_کړه FDT_V3_اندازه	(FDT_V2_اندازه + حجم(fdt32_t))
#تعریف_کړه FDT_V16_اندازه	FDT_V3_اندازه
#تعریف_کړه FDT_V17_اندازه	(FDT_V16_اندازه + حجم(fdt32_t))

#د_که_پای /* FDT_H */
