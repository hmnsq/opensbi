/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FDT_H
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

ರಚನೆ fdt_ಶೀರ್ಷಿಕೆ {
	fdt32_t magic;			 /* magic word FDT_MAGIC */
	fdt32_t totalsize;		 /* total size of DT block */
	fdt32_t off_dt_struct;		 /* offset to structure */
	fdt32_t off_dt_ಅಕ್ಷರಸರಣಿಗಳು;		 /* offset to strings */
	fdt32_t off_mem_rsvmap;		 /* offset to memory reserve map */
	fdt32_t ಆವೃತ್ತಿ;		 /* format version */
	fdt32_t ಕೊನೆಯ_comp_ಆವೃತ್ತಿ;	 /* last compatible version */

	/* version 2 fields below */
	fdt32_t boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	fdt32_t ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು;	 /* size of the strings block */

	/* version 17 fields below */
	fdt32_t ಗಾತ್ರ_dt_struct;		 /* size of the structure block */
};

ರಚನೆ fdt_reserve_ನಮೂದು {
	fdt64_t ವಿಳಾಸ;
	fdt64_t ಗಾತ್ರ_2;
};

ರಚನೆ fdt_ಗಂಟು_ಶೀರ್ಷಿಕೆ {
	fdt32_t ಟ್ಯಾಗ್;
	ಅಕ್ಷರ name[0];
};

ರಚನೆ fdt_property {
	fdt32_t ಟ್ಯಾಗ್;
	fdt32_t len;
	fdt32_t nameoff;
	ಅಕ್ಷರ ದತ್ತಾಂಶ[0];
};

#ಆದರೆ_ಅಂತ್ಯ /* !__ASSEMBLY */

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_TAGSIZE	ಗಾತ್ರ(fdt32_t)

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_BEGIN_ಗಂಟು	0x1		/* Start node: full name */
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_ಕೊನೆ_ಗಂಟು	0x2		/* End node */
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_PROP	0x3		/* Property: name off,
					   size, content */
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_NOP		0x4		/* nop */
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_ಕೊನೆ		0x9

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_V1_ಗಾತ್ರ	(7*ಗಾತ್ರ(fdt32_t))
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_V2_ಗಾತ್ರ	(FDT_V1_ಗಾತ್ರ + ಗಾತ್ರ(fdt32_t))
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_V3_ಗಾತ್ರ	(FDT_V2_ಗಾತ್ರ + ಗಾತ್ರ(fdt32_t))
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_V16_ಗಾತ್ರ	FDT_V3_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_V17_ಗಾತ್ರ	(FDT_V16_ಗಾತ್ರ + ಗಾತ್ರ(fdt32_t))

#ಆದರೆ_ಅಂತ್ಯ /* FDT_H */
