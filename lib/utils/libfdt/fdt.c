// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ध्रुव शून्य *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	यदि (can_assume(VALID_DTB))
		प्रत्यागम totalsize;

	/* The device tree must be at an 8-byte aligned address */
	यदि ((uintptr_t)fdt & 7)
		प्रत्यागम -FDT_ERR_ALIGNMENT;

	यदि (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		यदि (!can_assume(LATEST)) {
			यदि (fdt_संस्करणम्(fdt) < FDT_FIRST_SUPPORTED_संस्करणम्)
				प्रत्यागम -FDT_ERR_BADVERSION;
			यदि (fdt_चरमः_comp_संस्करणम्(fdt) >
					FDT_चरमः_SUPPORTED_संस्करणम्)
				प्रत्यागम -FDT_ERR_BADVERSION;
		}
	} अन्यथा यदि (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		यदि (!can_assume(VALID_निवेशः) && fdt_आकार_dt_struct(fdt) == 0)
			प्रत्यागम -FDT_ERR_BADSTATE;
	} अन्यथा {
		प्रत्यागम -FDT_ERR_BADMAGIC;
	}

	यदि (totalsize < INT32_MAX)
		प्रत्यागम totalsize;
	अन्यथा
		प्रत्यागम -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णाङ्क check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	प्रत्यागम (off >= hdrsize) && (off <= totalsize);
}

स्थिर पूर्णाङ्क check_खण्डम्(uint32_t hdrsize, uint32_t totalsize,
			uint32_t आधारः_2, uint32_t आकार)
{
	यदि (!check_off_(hdrsize, totalsize, आधारः_2))
		प्रत्यागम 0; /* block start out of bounds */
	यदि ((आधारः_2 + आकार) < आधारः_2)
		प्रत्यागम 0; /*अतिप्रवाहः*/
	यदि (!check_off_(hdrsize, totalsize, आधारः_2 + आकार))
		प्रत्यागम 0; /* block end out of bounds */
	प्रत्यागम 1;
}

size_t fdt_शीर्षकम्_आकार(uint32_t संस्करणम्)
{
	यदि (संस्करणम् <= 1)
		प्रत्यागम FDT_V1_आकार;
	अन्यथा यदि (संस्करणम् <= 2)
		प्रत्यागम FDT_V2_आकार;
	अन्यथा यदि (संस्करणम् <= 3)
		प्रत्यागम FDT_V3_आकार;
	अन्यथा यदि (संस्करणम् <= 16)
		प्रत्यागम FDT_V16_आकार;
	अन्यथा
		प्रत्यागम FDT_V17_आकार;
}

size_t fdt_शीर्षकम्_आकार_2(ध्रुव शून्य *fdt)
{
	प्रत्यागम can_assume(LATEST) ? FDT_V17_आकार :
		fdt_शीर्षकम्_आकार(fdt_संस्करणम्(fdt));
}

पूर्णाङ्क fdt_check_शीर्षकम्(ध्रुव शून्य *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	यदि ((uintptr_t)fdt & 7)
		प्रत्यागम -FDT_ERR_ALIGNMENT;

	यदि (fdt_magic(fdt) != FDT_MAGIC)
		प्रत्यागम -FDT_ERR_BADMAGIC;
	यदि (!can_assume(LATEST)) {
		यदि ((fdt_संस्करणम्(fdt) < FDT_FIRST_SUPPORTED_संस्करणम्)
		    || (fdt_चरमः_comp_संस्करणम्(fdt) >
			FDT_चरमः_SUPPORTED_संस्करणम्))
			प्रत्यागम -FDT_ERR_BADVERSION;
		यदि (fdt_संस्करणम्(fdt) < fdt_चरमः_comp_संस्करणम्(fdt))
			प्रत्यागम -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_शीर्षकम्_आकार_2(fdt);
	यदि (!can_assume(VALID_DTB)) {

		यदि ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > अधिकतम_पूर्णाङ्कः))
			प्रत्यागम -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		यदि (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			प्रत्यागम -FDT_ERR_TRUNCATED;
	}

	यदि (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		यदि (!can_assume(LATEST) && fdt_संस्करणम्(fdt) < 17) {
			यदि (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				प्रत्यागम -FDT_ERR_TRUNCATED;
		} अन्यथा {
			यदि (!check_खण्डम्(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_आकार_dt_struct(fdt)))
				प्रत्यागम -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		यदि (!check_खण्डम्(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_वाक्यानि(fdt),
				  fdt_आकार_dt_वाक्यानि(fdt)))
			प्रत्यागम -FDT_ERR_TRUNCATED;
	}

	प्रत्यागम 0;
}

ध्रुव शून्य *fdt_विस्थापनम्_ptr(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्, अचिह्नित पूर्णाङ्क len)
{
	अचिह्नित पूर्णाङ्क uoffset = विस्थापनम्;
	अचिह्नित पूर्णाङ्क absoffset = विस्थापनम् + fdt_off_dt_struct(fdt);

	यदि (विस्थापनम् < 0)
		प्रत्यागम रिक्त;

	यदि (!can_assume(VALID_निवेशः))
		यदि ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			प्रत्यागम रिक्त;

	यदि (can_assume(LATEST) || fdt_संस्करणम्(fdt) >= 0x11)
		यदि (((uoffset + len) < uoffset)
		    || ((विस्थापनम् + len) > fdt_आकार_dt_struct(fdt)))
			प्रत्यागम रिक्त;

	प्रत्यागम fdt_विस्थापनम्_ptr_2(fdt, विस्थापनम्);
}

uint32_t fdt_अग्र_चिह्नकम्(ध्रुव शून्य *fdt, पूर्णाङ्क startoffset, पूर्णाङ्क *nextoffset)
{
	ध्रुव fdt32_t *tagp, *lenp;
	uint32_t चिह्नकम्;
	पूर्णाङ्क विस्थापनम् = startoffset;
	ध्रुव वर्ण *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_विस्थापनम्_ptr(fdt, विस्थापनम्, FDT_TAGSIZE);
	यदि (!can_assume(VALID_DTB) && !tagp)
		प्रत्यागम FDT_अन्तः; /* premature end */
	चिह्नकम् = fdt32_गन्तव्यम्_cpu(*tagp);
	विस्थापनम् += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	विकल्प (चिह्नकम्) {
	पक्ष FDT_BEGIN_ग्रन्थिः:
		/* skip name */
		कुरु {
			p = fdt_विस्थापनम्_ptr(fdt, विस्थापनम्++, 1);
		} यावत् (p && (*p != '\0'));
		यदि (!can_assume(VALID_DTB) && !p)
			प्रत्यागम FDT_अन्तः; /* premature end */
		भङ्ग;

	पक्ष FDT_PROP:
		lenp = fdt_विस्थापनम्_ptr(fdt, विस्थापनम्, परिमाण(*lenp));
		यदि (!can_assume(VALID_DTB) && !lenp)
			प्रत्यागम FDT_अन्तः; /* premature end */
		/* skip-name offset, length and value */
		विस्थापनम् += परिमाण(संरचना fdt_property) - FDT_TAGSIZE
			+ fdt32_गन्तव्यम्_cpu(*lenp);
		यदि (!can_assume(LATEST) &&
		    fdt_संस्करणम्(fdt) < 0x10 && fdt32_गन्तव्यम्_cpu(*lenp) >= 8 &&
		    ((विस्थापनम् - fdt32_गन्तव्यम्_cpu(*lenp)) % 8) != 0)
			विस्थापनम् += 4;
		भङ्ग;

	पक्ष FDT_अन्तः:
	पक्ष FDT_अन्तः_ग्रन्थिः:
	पक्ष FDT_NOP:
		भङ्ग;

	स्वभाव:
		प्रत्यागम FDT_अन्तः;
	}

	यदि (!fdt_विस्थापनम्_ptr(fdt, startoffset, विस्थापनम् - startoffset))
		प्रत्यागम FDT_अन्तः; /* premature end */

	*nextoffset = FDT_TAGALIGN(विस्थापनम्);
	प्रत्यागम चिह्नकम्;
}

पूर्णाङ्क fdt_check_ग्रन्थिः_विस्थापनम्(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्)
{
	यदि (!can_assume(VALID_निवेशः)
	    && ((विस्थापनम् < 0) || (विस्थापनम् % FDT_TAGSIZE)))
		प्रत्यागम -FDT_ERR_BADOFFSET;

	यदि (fdt_अग्र_चिह्नकम्(fdt, विस्थापनम्, &विस्थापनम्) != FDT_BEGIN_ग्रन्थिः)
		प्रत्यागम -FDT_ERR_BADOFFSET;

	प्रत्यागम विस्थापनम्;
}

पूर्णाङ्क fdt_check_prop_विस्थापनम्(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्)
{
	यदि (!can_assume(VALID_निवेशः)
	    && ((विस्थापनम् < 0) || (विस्थापनम् % FDT_TAGSIZE)))
		प्रत्यागम -FDT_ERR_BADOFFSET;

	यदि (fdt_अग्र_चिह्नकम्(fdt, विस्थापनम्, &विस्थापनम्) != FDT_PROP)
		प्रत्यागम -FDT_ERR_BADOFFSET;

	प्रत्यागम विस्थापनम्;
}

पूर्णाङ्क fdt_अग्र_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क *गभीरता)
{
	पूर्णाङ्क nextoffset = 0;
	uint32_t चिह्नकम्;

	यदि (विस्थापनम् >= 0)
		यदि ((nextoffset = fdt_check_ग्रन्थिः_विस्थापनम्(fdt, विस्थापनम्)) < 0)
			प्रत्यागम nextoffset;

	कुरु {
		विस्थापनम् = nextoffset;
		चिह्नकम् = fdt_अग्र_चिह्नकम्(fdt, विस्थापनम्, &nextoffset);

		विकल्प (चिह्नकम्) {
		पक्ष FDT_PROP:
		पक्ष FDT_NOP:
			भङ्ग;

		पक्ष FDT_BEGIN_ग्रन्थिः:
			यदि (गभीरता)
				(*गभीरता)++;
			भङ्ग;

		पक्ष FDT_अन्तः_ग्रन्थिः:
			यदि (गभीरता && ((--(*गभीरता)) < 0))
				प्रत्यागम nextoffset;
			भङ्ग;

		पक्ष FDT_अन्तः:
			यदि ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !गभीरता))
				प्रत्यागम -FDT_ERR_NOTFOUND;
			अन्यथा
				प्रत्यागम nextoffset;
		}
	} यावत् (चिह्नकम् != FDT_BEGIN_ग्रन्थिः);

	प्रत्यागम विस्थापनम्;
}

पूर्णाङ्क fdt_first_subnode(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्)
{
	पूर्णाङ्क गभीरता = 0;

	विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, &गभीरता);
	यदि (विस्थापनम् < 0 || गभीरता != 1)
		प्रत्यागम -FDT_ERR_NOTFOUND;

	प्रत्यागम विस्थापनम्;
}

पूर्णाङ्क fdt_अग्र_subnode(ध्रुव शून्य *fdt, पूर्णाङ्क विस्थापनम्)
{
	पूर्णाङ्क गभीरता = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	कुरु {
		विस्थापनम् = fdt_अग्र_ग्रन्थिः(fdt, विस्थापनम्, &गभीरता);
		यदि (विस्थापनम् < 0 || गभीरता < 1)
			प्रत्यागम -FDT_ERR_NOTFOUND;
	} यावत् (गभीरता > 1);

	प्रत्यागम विस्थापनम्;
}

ध्रुव वर्ण *fdt_find_string_(ध्रुव वर्ण *strtab, पूर्णाङ्क tabsize, ध्रुव वर्ण *s)
{
	पूर्णाङ्क len = वाक्यदैर्घ्यम्(s) + 1;
	ध्रुव वर्ण *चरमः = strtab + tabsize - len;
	ध्रुव वर्ण *p;

	प्रति (p = strtab; p <= चरमः; p++)
		यदि (memcmp(p, s, len) == 0)
			प्रत्यागम p;
	प्रत्यागम रिक्त;
}

पूर्णाङ्क fdt_move(ध्रुव शून्य *fdt, शून्य *buf, पूर्णाङ्क bufsize)
{
	यदि (!can_assume(VALID_निवेशः) && bufsize < 0)
		प्रत्यागम -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	यदि (fdt_totalsize(fdt) > (अचिह्नित पूर्णाङ्क)bufsize)
		प्रत्यागम -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	प्रत्यागम 0;
}
