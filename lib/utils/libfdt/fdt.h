/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#اگر_تعریف_نشده FDT_H
#تعریف_کن FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#اگر_تعریف_نشده __ASSEMBLY__

بنية fdt_سرآیند {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_رشتهها;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t نسخه;		 /* format version */
	fdt32_t آخرین_comp_نسخه;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t اندازه_dt_رشتهها;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t اندازه_dt_struct;		 /* size of the structure block */
};

بنية fdt_reserve_مدخل {
	fdt64_t آدرس;
	fdt64_t اندازه;
};

بنية fdt_گره_سرآیند {
	fdt32_t برچسب;
	حرف name[0];
};

بنية fdt_property {
	fdt32_t برچسب;
	fdt32_t len;
	fdt32_t nameoff;
	حرف داده[0];
};

#پایان_اگر /* !__ASSEMBLY */

#تعریف_کن FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#تعریف_کن FDT_TAGSIZE	حجم(fdt32_t)

#تعریف_کن FDT_BEGIN_گره	0x1		/* Start node: full name */
#تعریف_کن FDT_انتها_گره	0x2		/* End node */
#تعریف_کن FDT_PROP	0x3		/* Property: name off,
					   size, content */
#تعریف_کن FDT_NOP		0x4		/* nop */
#تعریف_کن FDT_انتها		0x9

#تعریف_کن FDT_V1_اندازه	(7*حجم(fdt32_t))
#تعریف_کن FDT_V2_اندازه	(FDT_V1_اندازه + حجم(fdt32_t))
#تعریف_کن FDT_V3_اندازه	(FDT_V2_اندازه + حجم(fdt32_t))
#تعریف_کن FDT_V16_اندازه	FDT_V3_اندازه
#تعریف_کن FDT_V17_اندازه	(FDT_V16_اندازه + حجم(fdt32_t))

#پایان_اگر /* FDT_H */
