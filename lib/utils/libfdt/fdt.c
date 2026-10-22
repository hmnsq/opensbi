// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	ଯଦି (can_assume(VALID_DTB))
		ଫେରନ୍ତୁ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ଯଦି ((uintptr_t)fdt & 7)
		ଫେରନ୍ତୁ -FDT_ERR_ALIGNMENT;

	ଯଦି (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ଯଦି (!can_assume(LATEST)) {
			ଯଦି (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ଫେରନ୍ତୁ -FDT_ERR_BADVERSION;
			ଯଦି (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ଫେରନ୍ତୁ -FDT_ERR_BADVERSION;
		}
	} ଅନ୍ୟଥା ଯଦି (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ଯଦି (!can_assume(VALID_ନିବେଶ) && fdt_size_dt_struct(fdt) == 0)
			ଫେରନ୍ତୁ -FDT_ERR_BADSTATE;
	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ -FDT_ERR_BADMAGIC;
	}

	ଯଦି (totalsize < INT32_MAX)
		ଫେରନ୍ତୁ totalsize;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ଫେରନ୍ତୁ (off >= hdrsize) && (off <= totalsize);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ଯଦି (!check_off_(hdrsize, totalsize, base))
		ଫେରନ୍ତୁ 0; /* block start out of bounds */
	ଯଦି ((base + size) < base)
		ଫେରନ୍ତୁ 0; /* overflow */
	ଯଦି (!check_off_(hdrsize, totalsize, base + size))
		ଫେରନ୍ତୁ 0; /* block end out of bounds */
	ଫେରନ୍ତୁ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ଯଦି (version <= 1)
		ଫେରନ୍ତୁ FDT_V1_SIZE;
	ଅନ୍ୟଥା ଯଦି (version <= 2)
		ଫେରନ୍ତୁ FDT_V2_SIZE;
	ଅନ୍ୟଥା ଯଦି (version <= 3)
		ଫେରନ୍ତୁ FDT_V3_SIZE;
	ଅନ୍ୟଥା ଯଦି (version <= 16)
		ଫେରନ୍ତୁ FDT_V16_SIZE;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ FDT_V17_SIZE;
}

size_t fdt_header_size(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	ଫେରନ୍ତୁ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_check_header(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ଯଦି ((uintptr_t)fdt & 7)
		ଫେରନ୍ତୁ -FDT_ERR_ALIGNMENT;

	ଯଦି (fdt_magic(fdt) != FDT_MAGIC)
		ଫେରନ୍ତୁ -FDT_ERR_BADMAGIC;
	ଯଦି (!can_assume(LATEST)) {
		ଯଦି ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ଫେରନ୍ତୁ -FDT_ERR_BADVERSION;
		ଯଦି (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ଫେରନ୍ତୁ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ଯଦି (!can_assume(VALID_DTB)) {

		ଯଦି ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ଯଦି (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;
	}

	ଯଦି (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ଯଦି (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ଯଦି (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;
		} ଅନ୍ୟଥା {
			ଯଦି (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ଯଦି (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;
	}

	ଫେରନ୍ତୁ 0;
}

ଅଚଳ ଶୂନ୍ୟ *fdt_offset_ptr(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uoffset = offset;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା absoffset = offset + fdt_off_dt_struct(fdt);

	ଯଦି (offset < 0)
		ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (!can_assume(VALID_ନିବେଶ))
		ଯଦି ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ଯଦି (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ଫେରନ୍ତୁ ଖାଲି;

	ଫେରନ୍ତୁ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startoffset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *nextoffset)
{
	ଅଚଳ fdt32_t *tagp, *lenp;
	uint32_t tag;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = startoffset;
	ଅଚଳ ଅକ୍ଷର *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ଯଦି (!can_assume(VALID_DTB) && !tagp)
		ଫେରନ୍ତୁ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	ବାଛ (tag) {
	ସ୍ଥିତି FDT_BEGIN_ଗଣ୍ଠି:
		/* skip name */
		କର {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ଯେତେବେଳେ (p && (*p != '\0'));
		ଯଦି (!can_assume(VALID_DTB) && !p)
			ଫେରନ୍ତୁ FDT_END; /* premature end */
		ଭାଙ୍ଗ;

	ସ୍ଥିତି FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, ଆକାର(*lenp));
		ଯଦି (!can_assume(VALID_DTB) && !lenp)
			ଫେରନ୍ତୁ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += ଆକାର(ସଂରଚନା fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ଯଦି (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି FDT_END:
	ସ୍ଥିତି FDT_END_ଗଣ୍ଠି:
	ସ୍ଥିତି FDT_NOP:
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ FDT_END;
	}

	ଯଦି (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ଫେରନ୍ତୁ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ଫେରନ୍ତୁ tag;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_check_ଗଣ୍ଠି_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
	ଯଦି (!can_assume(VALID_ନିବେଶ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;

	ଯଦି (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_ଗଣ୍ଠି)
		ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;

	ଫେରନ୍ତୁ offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_check_prop_offset_(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
	ଯଦି (!can_assume(VALID_ନିବେଶ)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;

	ଯଦି (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;

	ଫେରନ୍ତୁ offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_next_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *depth)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nextoffset = 0;
	uint32_t tag;

	ଯଦି (offset >= 0)
		ଯଦି ((nextoffset = fdt_check_ଗଣ୍ଠି_offset(fdt, offset)) < 0)
			ଫେରନ୍ତୁ nextoffset;

	କର {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ବାଛ (tag) {
		ସ୍ଥିତି FDT_PROP:
		ସ୍ଥିତି FDT_NOP:
			ଭାଙ୍ଗ;

		ସ୍ଥିତି FDT_BEGIN_ଗଣ୍ଠି:
			ଯଦି (depth)
				(*depth)++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି FDT_END_ଗଣ୍ଠି:
			ଯଦି (depth && ((--(*depth)) < 0))
				ଫେରନ୍ତୁ nextoffset;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି FDT_END:
			ଯଦି ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ nextoffset;
		}
	} ଯେତେବେଳେ (tag != FDT_BEGIN_ଗଣ୍ଠି);

	ଫେରନ୍ତୁ offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_first_subnode(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = 0;

	offset = fdt_next_ଗଣ୍ଠି(fdt, offset, &depth);
	ଯଦି (offset < 0 || depth != 1)
		ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;

	ଫେରନ୍ତୁ offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_next_subnode(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	କର {
		offset = fdt_next_ଗଣ୍ଠି(fdt, offset, &depth);
		ଯଦି (offset < 0 || depth < 1)
			ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;
	} ଯେତେବେଳେ (depth > 1);

	ଫେରନ୍ତୁ offset;
}

ଅଚଳ ଅକ୍ଷର *fdt_find_string_(ଅଚଳ ଅକ୍ଷର *strtab, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tabsize, ଅଚଳ ଅକ୍ଷର *s)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = strlen(s) + 1;
	ଅଚଳ ଅକ୍ଷର *last = strtab + tabsize - len;
	ଅଚଳ ଅକ୍ଷର *p;

	ପାଇଁ (p = strtab; p <= last; p++)
		ଯଦି (memcmp(p, s, len) == 0)
			ଫେରନ୍ତୁ p;
	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_move(ଅଚଳ ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize)
{
	ଯଦି (!can_assume(VALID_ନିବେଶ) && bufsize < 0)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ଯଦି (fdt_totalsize(fdt) > (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bufsize)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ଫେରନ୍ତୁ 0;
}
