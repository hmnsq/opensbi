/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#నిర్వచితం_కాకపోతే FDT_H
#నిర్వచించు FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#నిర్వచితం_కాకపోతే __ASSEMBLY__

నిర్మాణం fdt_శీర్షిక {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_అక్షరశ్రేణులు;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t సంస్కరణ;		 /* format version */
	fdt32_t చివరి_comp_సంస్కరణ;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t కొలత_dt_అక్షరశ్రేణులు;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t కొలత_dt_struct;		 /* size of the structure block */
};

నిర్మాణం fdt_reserve_నమోదు {
	fdt64_t చిరునామా;
	fdt64_t కొలత;
};

నిర్మాణం fdt_కణుపు_శీర్షిక {
	fdt32_t ట్యాగ్;
	అక్షరం name[0];
};

నిర్మాణం fdt_property {
	fdt32_t ట్యాగ్;
	fdt32_t len;
	fdt32_t nameoff;
	అక్షరం దత్తాంశం[0];
};

#అయితే_ముగింపు /* !__ASSEMBLY */

#నిర్వచించు FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#నిర్వచించు FDT_TAGSIZE	పరిమాణం(fdt32_t)

#నిర్వచించు FDT_BEGIN_కణుపు	0x1		/* Start node: full name */
#నిర్వచించు FDT_చివర_కణుపు	0x2		/* End node */
#నిర్వచించు FDT_PROP	0x3		/* Property: name off,
					   size, content */
#నిర్వచించు FDT_NOP		0x4		/* nop */
#నిర్వచించు FDT_చివర		0x9

#నిర్వచించు FDT_V1_కొలత	(7*పరిమాణం(fdt32_t))
#నిర్వచించు FDT_V2_కొలత	(FDT_V1_కొలత + పరిమాణం(fdt32_t))
#నిర్వచించు FDT_V3_కొలత	(FDT_V2_కొలత + పరిమాణం(fdt32_t))
#నిర్వచించు FDT_V16_కొలత	FDT_V3_కొలత
#నిర్వచించు FDT_V17_కొలత	(FDT_V16_కొలత + పరిమాణం(fdt32_t))

#అయితే_ముగింపు /* FDT_H */
