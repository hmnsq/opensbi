// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(અચલ શૂન્ય *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	જો (can_assume(VALID_DTB))
		પાછા totalsize;

	/* The device tree must be at an 8-byte aligned address */
	જો ((uintptr_t)fdt & 7)
		પાછા -FDT_ERR_ALIGNMENT;

	જો (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		જો (!can_assume(LATEST)) {
			જો (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				પાછા -FDT_ERR_BADVERSION;
			જો (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				પાછા -FDT_ERR_BADVERSION;
		}
	} નહીંતર જો (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		જો (!can_assume(VALID_આગત) && fdt_size_dt_struct(fdt) == 0)
			પાછા -FDT_ERR_BADSTATE;
	} નહીંતર {
		પાછા -FDT_ERR_BADMAGIC;
	}

	જો (totalsize < INT32_MAX)
		પાછા totalsize;
	નહીંતર
		પાછા -FDT_ERR_TRUNCATED;
}

સ્થિર પૂર્ણાંક check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	પાછા (off >= hdrsize) && (off <= totalsize);
}

સ્થિર પૂર્ણાંક check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	જો (!check_off_(hdrsize, totalsize, base))
		પાછા 0; /* block start out of bounds */
	જો ((base + size) < base)
		પાછા 0; /* overflow */
	જો (!check_off_(hdrsize, totalsize, base + size))
		પાછા 0; /* block end out of bounds */
	પાછા 1;
}

size_t fdt_header_size_(uint32_t version)
{
	જો (version <= 1)
		પાછા FDT_V1_SIZE;
	નહીંતર જો (version <= 2)
		પાછા FDT_V2_SIZE;
	નહીંતર જો (version <= 3)
		પાછા FDT_V3_SIZE;
	નહીંતર જો (version <= 16)
		પાછા FDT_V16_SIZE;
	નહીંતર
		પાછા FDT_V17_SIZE;
}

size_t fdt_header_size(અચલ શૂન્ય *fdt)
{
	પાછા can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

પૂર્ણાંક fdt_check_header(અચલ શૂન્ય *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	જો ((uintptr_t)fdt & 7)
		પાછા -FDT_ERR_ALIGNMENT;

	જો (fdt_magic(fdt) != FDT_MAGIC)
		પાછા -FDT_ERR_BADMAGIC;
	જો (!can_assume(LATEST)) {
		જો ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			પાછા -FDT_ERR_BADVERSION;
		જો (fdt_version(fdt) < fdt_last_comp_version(fdt))
			પાછા -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	જો (!can_assume(VALID_DTB)) {

		જો ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			પાછા -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		જો (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			પાછા -FDT_ERR_TRUNCATED;
	}

	જો (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		જો (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			જો (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				પાછા -FDT_ERR_TRUNCATED;
		} નહીંતર {
			જો (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				પાછા -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		જો (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			પાછા -FDT_ERR_TRUNCATED;
	}

	પાછા 0;
}

અચલ શૂન્ય *fdt_offset_ptr(અચલ શૂન્ય *fdt, પૂર્ણાંક offset, unsigned પૂર્ણાંક len)
{
	unsigned પૂર્ણાંક uoffset = offset;
	unsigned પૂર્ણાંક absoffset = offset + fdt_off_dt_struct(fdt);

	જો (offset < 0)
		પાછા ખાલી;

	જો (!can_assume(VALID_આગત))
		જો ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			પાછા ખાલી;

	જો (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		જો (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			પાછા ખાલી;

	પાછા fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(અચલ શૂન્ય *fdt, પૂર્ણાંક startoffset, પૂર્ણાંક *nextoffset)
{
	અચલ fdt32_t *tagp, *lenp;
	uint32_t tag;
	પૂર્ણાંક offset = startoffset;
	અચલ અક્ષર *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	જો (!can_assume(VALID_DTB) && !tagp)
		પાછા FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	પસંદગી (tag) {
	સ્થિતિ FDT_BEGIN_ગાંઠ:
		/* skip name */
		કરો {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} જ્યારે (p && (*p != '\0'));
		જો (!can_assume(VALID_DTB) && !p)
			પાછા FDT_END; /* premature end */
		તોડો;

	સ્થિતિ FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, કદ(*lenp));
		જો (!can_assume(VALID_DTB) && !lenp)
			પાછા FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += કદ(સંરચના fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		જો (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		તોડો;

	સ્થિતિ FDT_END:
	સ્થિતિ FDT_END_ગાંઠ:
	સ્થિતિ FDT_NOP:
		તોડો;

	મૂળભૂત:
		પાછા FDT_END;
	}

	જો (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		પાછા FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	પાછા tag;
}

પૂર્ણાંક fdt_check_ગાંઠ_offset(અચલ શૂન્ય *fdt, પૂર્ણાંક offset)
{
	જો (!can_assume(VALID_આગત)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		પાછા -FDT_ERR_BADOFFSET;

	જો (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_ગાંઠ)
		પાછા -FDT_ERR_BADOFFSET;

	પાછા offset;
}

પૂર્ણાંક fdt_check_prop_offset_(અચલ શૂન્ય *fdt, પૂર્ણાંક offset)
{
	જો (!can_assume(VALID_આગત)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		પાછા -FDT_ERR_BADOFFSET;

	જો (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		પાછા -FDT_ERR_BADOFFSET;

	પાછા offset;
}

પૂર્ણાંક fdt_next_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક offset, પૂર્ણાંક *depth)
{
	પૂર્ણાંક nextoffset = 0;
	uint32_t tag;

	જો (offset >= 0)
		જો ((nextoffset = fdt_check_ગાંઠ_offset(fdt, offset)) < 0)
			પાછા nextoffset;

	કરો {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		પસંદગી (tag) {
		સ્થિતિ FDT_PROP:
		સ્થિતિ FDT_NOP:
			તોડો;

		સ્થિતિ FDT_BEGIN_ગાંઠ:
			જો (depth)
				(*depth)++;
			તોડો;

		સ્થિતિ FDT_END_ગાંઠ:
			જો (depth && ((--(*depth)) < 0))
				પાછા nextoffset;
			તોડો;

		સ્થિતિ FDT_END:
			જો ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				પાછા -FDT_ERR_NOTFOUND;
			નહીંતર
				પાછા nextoffset;
		}
	} જ્યારે (tag != FDT_BEGIN_ગાંઠ);

	પાછા offset;
}

પૂર્ણાંક fdt_first_subnode(અચલ શૂન્ય *fdt, પૂર્ણાંક offset)
{
	પૂર્ણાંક depth = 0;

	offset = fdt_next_ગાંઠ(fdt, offset, &depth);
	જો (offset < 0 || depth != 1)
		પાછા -FDT_ERR_NOTFOUND;

	પાછા offset;
}

પૂર્ણાંક fdt_next_subnode(અચલ શૂન્ય *fdt, પૂર્ણાંક offset)
{
	પૂર્ણાંક depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	કરો {
		offset = fdt_next_ગાંઠ(fdt, offset, &depth);
		જો (offset < 0 || depth < 1)
			પાછા -FDT_ERR_NOTFOUND;
	} જ્યારે (depth > 1);

	પાછા offset;
}

અચલ અક્ષર *fdt_find_string_(અચલ અક્ષર *strtab, પૂર્ણાંક tabsize, અચલ અક્ષર *s)
{
	પૂર્ણાંક len = strlen(s) + 1;
	અચલ અક્ષર *last = strtab + tabsize - len;
	અચલ અક્ષર *p;

	માટે (p = strtab; p <= last; p++)
		જો (memcmp(p, s, len) == 0)
			પાછા p;
	પાછા ખાલી;
}

પૂર્ણાંક fdt_move(અચલ શૂન્ય *fdt, શૂન્ય *buf, પૂર્ણાંક bufsize)
{
	જો (!can_assume(VALID_આગત) && bufsize < 0)
		પાછા -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	જો (fdt_totalsize(fdt) > (unsigned પૂર્ણાંક)bufsize)
		પાછા -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	પાછા 0;
}
