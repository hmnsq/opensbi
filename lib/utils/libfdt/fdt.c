// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ثابت تهی *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	اگر (can_assume(VALID_DTB))
		بازگشت totalsize;

	/* The device tree must be at an 8-byte aligned address */
	اگر ((uintptr_t)fdt & 7)
		بازگشت -FDT_ERR_ALIGNMENT;

	اگر (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		اگر (!can_assume(LATEST)) {
			اگر (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				بازگشت -FDT_ERR_BADVERSION;
			اگر (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				بازگشت -FDT_ERR_BADVERSION;
		}
	} وگرنه اگر (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		اگر (!can_assume(VALID_درون_داد) && fdt_size_dt_struct(fdt) == 0)
			بازگشت -FDT_ERR_BADSTATE;
	} وگرنه {
		بازگشت -FDT_ERR_BADMAGIC;
	}

	اگر (totalsize < INT32_MAX)
		بازگشت totalsize;
	وگرنه
		بازگشت -FDT_ERR_TRUNCATED;
}

ایستا صحیح check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	بازگشت (off >= hdrsize) && (off <= totalsize);
}

ایستا صحیح check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	اگر (!check_off_(hdrsize, totalsize, base))
		بازگشت 0; /* block start out of bounds */
	اگر ((base + size) < base)
		بازگشت 0; /* overflow */
	اگر (!check_off_(hdrsize, totalsize, base + size))
		بازگشت 0; /* block end out of bounds */
	بازگشت 1;
}

size_t fdt_header_size_(uint32_t version)
{
	اگر (version <= 1)
		بازگشت FDT_V1_SIZE;
	وگرنه اگر (version <= 2)
		بازگشت FDT_V2_SIZE;
	وگرنه اگر (version <= 3)
		بازگشت FDT_V3_SIZE;
	وگرنه اگر (version <= 16)
		بازگشت FDT_V16_SIZE;
	وگرنه
		بازگشت FDT_V17_SIZE;
}

size_t fdt_header_size(ثابت تهی *fdt)
{
	بازگشت can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

صحیح fdt_check_header(ثابت تهی *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	اگر ((uintptr_t)fdt & 7)
		بازگشت -FDT_ERR_ALIGNMENT;

	اگر (fdt_magic(fdt) != FDT_MAGIC)
		بازگشت -FDT_ERR_BADMAGIC;
	اگر (!can_assume(LATEST)) {
		اگر ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			بازگشت -FDT_ERR_BADVERSION;
		اگر (fdt_version(fdt) < fdt_last_comp_version(fdt))
			بازگشت -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	اگر (!can_assume(VALID_DTB)) {

		اگر ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			بازگشت -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		اگر (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			بازگشت -FDT_ERR_TRUNCATED;
	}

	اگر (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		اگر (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			اگر (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				بازگشت -FDT_ERR_TRUNCATED;
		} وگرنه {
			اگر (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				بازگشت -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		اگر (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			بازگشت -FDT_ERR_TRUNCATED;
	}

	بازگشت 0;
}

ثابت تهی *fdt_offset_ptr(ثابت تهی *fdt, صحیح offset, unsigned صحیح len)
{
	unsigned صحیح uoffset = offset;
	unsigned صحیح absoffset = offset + fdt_off_dt_struct(fdt);

	اگر (offset < 0)
		بازگشت پوچ;

	اگر (!can_assume(VALID_درون_داد))
		اگر ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			بازگشت پوچ;

	اگر (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		اگر (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			بازگشت پوچ;

	بازگشت fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ثابت تهی *fdt, صحیح startoffset, صحیح *nextoffset)
{
	ثابت fdt32_t *tagp, *lenp;
	uint32_t tag;
	صحیح offset = startoffset;
	ثابت نویسه *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	اگر (!can_assume(VALID_DTB) && !tagp)
		بازگشت FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	انتخاب (tag) {
	حالت FDT_BEGIN_گره:
		/* skip name */
		انجامبده {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} وقتی (p && (*p != '\0'));
		اگر (!can_assume(VALID_DTB) && !p)
			بازگشت FDT_END; /* premature end */
		بشکن;

	حالت FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, اندازه(*lenp));
		اگر (!can_assume(VALID_DTB) && !lenp)
			بازگشت FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += اندازه(ساختار fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		اگر (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		بشکن;

	حالت FDT_END:
	حالت FDT_END_گره:
	حالت FDT_NOP:
		بشکن;

	پیشفرض:
		بازگشت FDT_END;
	}

	اگر (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		بازگشت FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	بازگشت tag;
}

صحیح fdt_check_گره_offset(ثابت تهی *fdt, صحیح offset)
{
	اگر (!can_assume(VALID_درون_داد)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		بازگشت -FDT_ERR_BADOFFSET;

	اگر (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_گره)
		بازگشت -FDT_ERR_BADOFFSET;

	بازگشت offset;
}

صحیح fdt_check_prop_offset_(ثابت تهی *fdt, صحیح offset)
{
	اگر (!can_assume(VALID_درون_داد)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		بازگشت -FDT_ERR_BADOFFSET;

	اگر (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		بازگشت -FDT_ERR_BADOFFSET;

	بازگشت offset;
}

صحیح fdt_next_گره(ثابت تهی *fdt, صحیح offset, صحیح *depth)
{
	صحیح nextoffset = 0;
	uint32_t tag;

	اگر (offset >= 0)
		اگر ((nextoffset = fdt_check_گره_offset(fdt, offset)) < 0)
			بازگشت nextoffset;

	انجامبده {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		انتخاب (tag) {
		حالت FDT_PROP:
		حالت FDT_NOP:
			بشکن;

		حالت FDT_BEGIN_گره:
			اگر (depth)
				(*depth)++;
			بشکن;

		حالت FDT_END_گره:
			اگر (depth && ((--(*depth)) < 0))
				بازگشت nextoffset;
			بشکن;

		حالت FDT_END:
			اگر ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				بازگشت -FDT_ERR_NOTFOUND;
			وگرنه
				بازگشت nextoffset;
		}
	} وقتی (tag != FDT_BEGIN_گره);

	بازگشت offset;
}

صحیح fdt_first_subnode(ثابت تهی *fdt, صحیح offset)
{
	صحیح depth = 0;

	offset = fdt_next_گره(fdt, offset, &depth);
	اگر (offset < 0 || depth != 1)
		بازگشت -FDT_ERR_NOTFOUND;

	بازگشت offset;
}

صحیح fdt_next_subnode(ثابت تهی *fdt, صحیح offset)
{
	صحیح depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	انجامبده {
		offset = fdt_next_گره(fdt, offset, &depth);
		اگر (offset < 0 || depth < 1)
			بازگشت -FDT_ERR_NOTFOUND;
	} وقتی (depth > 1);

	بازگشت offset;
}

ثابت نویسه *fdt_find_string_(ثابت نویسه *strtab, صحیح tabsize, ثابت نویسه *s)
{
	صحیح len = strlen(s) + 1;
	ثابت نویسه *last = strtab + tabsize - len;
	ثابت نویسه *p;

	برای (p = strtab; p <= last; p++)
		اگر (memcmp(p, s, len) == 0)
			بازگشت p;
	بازگشت پوچ;
}

صحیح fdt_move(ثابت تهی *fdt, تهی *buf, صحیح bufsize)
{
	اگر (!can_assume(VALID_درون_داد) && bufsize < 0)
		بازگشت -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	اگر (fdt_totalsize(fdt) > (unsigned صحیح)bufsize)
		بازگشت -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	بازگشت 0;
}
