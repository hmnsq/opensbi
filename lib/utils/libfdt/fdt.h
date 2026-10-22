/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#جے_بنا_تعریف FDT_H
#تعریف_کرو FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#جے_بنا_تعریف __ASSEMBLY__

بنية fdt_سرخی {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_لڑیاں;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t نسخہ;		 /* format version */
	fdt32_t آخری_comp_نسخہ;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t ناپ_dt_لڑیاں;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t ناپ_dt_struct;		 /* size of the structure block */
};

بنية fdt_reserve_اندراج {
	fdt64_t پتہ;
	fdt64_t ناپ;
};

بنية fdt_گرہ_سرخی {
	fdt32_t ٹیگ;
	حرف name[0];
};

بنية fdt_property {
	fdt32_t ٹیگ;
	fdt32_t len;
	fdt32_t nameoff;
	حرف انکڑے[0];
};

#جے_انت /* !__ASSEMBLY */

#تعریف_کرو FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#تعریف_کرو FDT_TAGSIZE	حجم(fdt32_t)

#تعریف_کرو FDT_BEGIN_گرہ	0x1		/* Start node: full name */
#تعریف_کرو FDT_سرا_گرہ	0x2		/* End node */
#تعریف_کرو FDT_PROP	0x3		/* Property: name off,
					   size, content */
#تعریف_کرو FDT_NOP		0x4		/* nop */
#تعریف_کرو FDT_سرا		0x9

#تعریف_کرو FDT_V1_ناپ	(7*حجم(fdt32_t))
#تعریف_کرو FDT_V2_ناپ	(FDT_V1_ناپ + حجم(fdt32_t))
#تعریف_کرو FDT_V3_ناپ	(FDT_V2_ناپ + حجم(fdt32_t))
#تعریف_کرو FDT_V16_ناپ	FDT_V3_ناپ
#تعریف_کرو FDT_V17_ناپ	(FDT_V16_ناپ + حجم(fdt32_t))

#جے_انت /* FDT_H */
