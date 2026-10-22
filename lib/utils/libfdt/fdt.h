/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FDT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

ਬਣਤਰ fdt_ਸਿਰਲੇਖ {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_ਲੜੀਆਂ;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t ਸੰਸਕਰਣ;		 /* format version */
	fdt32_t ਆਖਰੀ_comp_ਸੰਸਕਰਣ;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t ਮਾਪ_dt_ਲੜੀਆਂ;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t ਮਾਪ_dt_struct;		 /* size of the structure block */
};

ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ {
	fdt64_t ਪਤਾ;
	fdt64_t ਮਾਪ;
};

ਬਣਤਰ fdt_ਗੰਢ_ਸਿਰਲੇਖ {
	fdt32_t ਟੈਗ;
	ਅੱਖਰ name[0];
};

ਬਣਤਰ fdt_property {
	fdt32_t ਟੈਗ;
	fdt32_t len;
	fdt32_t nameoff;
	ਅੱਖਰ ਅੰਕੜੇ[0];
};

#ਜੇ_ਅੰਤ /* !__ASSEMBLY */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_TAGSIZE	ਆਕਾਰ(fdt32_t)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_BEGIN_ਗੰਢ	0x1		/* Start node: full name */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_ਸਿਰਾ_ਗੰਢ	0x2		/* End node */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_PROP	0x3		/* Property: name off,
					   size, content */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_NOP		0x4		/* nop */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_ਸਿਰਾ		0x9

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_V1_ਮਾਪ	(7*ਆਕਾਰ(fdt32_t))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_V2_ਮਾਪ	(FDT_V1_ਮਾਪ + ਆਕਾਰ(fdt32_t))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_V3_ਮਾਪ	(FDT_V2_ਮਾਪ + ਆਕਾਰ(fdt32_t))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_V16_ਮਾਪ	FDT_V3_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_V17_ਮਾਪ	(FDT_V16_ਮਾਪ + ਆਕਾਰ(fdt32_t))

#ਜੇ_ਅੰਤ /* FDT_H */
