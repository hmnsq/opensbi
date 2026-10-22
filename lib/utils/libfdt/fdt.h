/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#यदि_अपरिभाषितम् FDT_H
#परिभाषय FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#यदि_अपरिभाषितम् __ASSEMBLY__

संरचना fdt_शीर्षकम् {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_वाक्यानि;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t संस्करणम्;		 /* format version */
	fdt32_t चरमः_comp_संस्करणम्;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t आकार_dt_वाक्यानि;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t आकार_dt_struct;		 /* size of the structure block */
};

संरचना fdt_reserve_प्रविष्टिः {
	fdt64_t स्थानाङ्कः;
	fdt64_t आकार;
};

संरचना fdt_ग्रन्थिः_शीर्षकम् {
	fdt32_t चिह्नकम्;
	वर्ण name[0];
};

संरचना fdt_property {
	fdt32_t चिह्नकम्;
	fdt32_t len;
	fdt32_t nameoff;
	वर्ण दत्त[0];
};

#यदि_अन्तः /* !__ASSEMBLY */

#परिभाषय FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#परिभाषय FDT_TAGSIZE	परिमाण(fdt32_t)

#परिभाषय FDT_BEGIN_ग्रन्थिः	0x1		/* Start node: full name */
#परिभाषय FDT_अन्तः_ग्रन्थिः	0x2		/* End node */
#परिभाषय FDT_PROP	0x3		/* Property: name off,
					   size, content */
#परिभाषय FDT_NOP		0x4		/* nop */
#परिभाषय FDT_अन्तः		0x9

#परिभाषय FDT_V1_आकार	(7*परिमाण(fdt32_t))
#परिभाषय FDT_V2_आकार	(FDT_V1_आकार + परिमाण(fdt32_t))
#परिभाषय FDT_V3_आकार	(FDT_V2_आकार + परिमाण(fdt32_t))
#परिभाषय FDT_V16_आकार	FDT_V3_आकार
#परिभाषय FDT_V17_आकार	(FDT_V16_आकार + परिमाण(fdt32_t))

#यदि_अन्तः /* FDT_H */
