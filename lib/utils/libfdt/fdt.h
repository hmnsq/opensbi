/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#اگر_غیر_تعریف_شدہ FDT_H
#تعریف_کرو FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#اگر_غیر_تعریف_شدہ __ASSEMBLY__

ساخت fdt_سرخی {
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
	fdt32_t سائز_dt_لڑیاں;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t سائز_dt_struct;		 /* size of the structure block */
};

ساخت fdt_reserve_اندراج {
	fdt64_t پتہ;
	fdt64_t سائز;
};

ساخت fdt_گرہ_سرخی {
	fdt32_t لیبل;
	حرف name[0];
};

ساخت fdt_property {
	fdt32_t لیبل;
	fdt32_t len;
	fdt32_t nameoff;
	حرف ڈیٹا[0];
};

#اگر_ختم /* !__ASSEMBLY */

#تعریف_کرو FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#تعریف_کرو FDT_TAGSIZE	حجم(fdt32_t)

#تعریف_کرو FDT_BEGIN_گرہ	0x1		/* Start node: full name */
#تعریف_کرو FDT_انتہا_گرہ	0x2		/* End node */
#تعریف_کرو FDT_PROP	0x3		/* Property: name off,
					   size, content */
#تعریف_کرو FDT_NOP		0x4		/* nop */
#تعریف_کرو FDT_انتہا		0x9

#تعریف_کرو FDT_V1_سائز	(7*حجم(fdt32_t))
#تعریف_کرو FDT_V2_سائز	(FDT_V1_سائز + حجم(fdt32_t))
#تعریف_کرو FDT_V3_سائز	(FDT_V2_سائز + حجم(fdt32_t))
#تعریف_کرو FDT_V16_سائز	FDT_V3_سائز
#تعریف_کرو FDT_V17_سائز	(FDT_V16_سائز + حجم(fdt32_t))

#اگر_ختم /* FDT_H */
