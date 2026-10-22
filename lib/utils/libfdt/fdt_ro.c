// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_nodename_eq_(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset,
			    ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା olen;
	ଅଚଳ ଅକ୍ଷର *p = fdt_get_name(fdt, offset, &olen);

	ଯଦି (!p || olen < len)
		/* short match */
		ଫେରନ୍ତୁ 0;

	ଯଦି (memcmp(p, s, len) != 0)
		ଫେରନ୍ତୁ 0;

	ଯଦି (p[len] == '\0')
		ଫେରନ୍ତୁ 1;
	ଅନ୍ୟଥା ଯଦି (!memchr(s, '@', len) && (p[len] == '@'))
		ଫେରନ୍ତୁ 1;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ 0;
}

ଅଚଳ ଅକ୍ଷର *fdt_get_string(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା stroffset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	ଅଚଳ ଅକ୍ଷର *s, *n;

	ଯଦି (can_assume(VALID_ନିବେଶ)) {
		s = (ଅଚଳ ଅକ୍ଷର *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ଯଦି (lenp)
			*lenp = strlen(s);
		ଫେରନ୍ତୁ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ଯଦି (totalsize < 0)
		ଯାଅ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ଯଦି (absoffset >= (unsigned)totalsize)
		ଯାଅ fail;
	len = totalsize - absoffset;

	ଯଦି (fdt_magic(fdt) == FDT_MAGIC) {
		ଯଦି (stroffset < 0)
			ଯାଅ fail;
		ଯଦି (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ଯଦି ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				ଯାଅ fail;
			ଯଦି ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ଅନ୍ୟଥା ଯଦି (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sw_stroffset = -stroffset;

		ଯଦି ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			ଯାଅ fail;
		ଯଦି (sw_stroffset < len)
			len = sw_stroffset;
	} ଅନ୍ୟଥା {
		err = -FDT_ERR_INTERNAL;
		ଯାଅ fail;
	}

	s = (ଅଚଳ ଅକ୍ଷର *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ଯଦି (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		ଯାଅ fail;
	}

	ଯଦି (lenp)
		*lenp = n - s;
	ଫେରନ୍ତୁ s;

fail:
	ଯଦି (lenp)
		*lenp = err;
	ଫେରନ୍ତୁ ଖାଲି;
}

ଅଚଳ ଅକ୍ଷର *fdt_string(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା stroffset)
{
	ଫେରନ୍ତୁ fdt_get_string(fdt, stroffset, ଖାଲି);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_string_eq_(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା stroffset,
			  ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା slen;
	ଅଚଳ ଅକ୍ଷର *p = fdt_get_string(fdt, stroffset, &slen);

	ଫେରନ୍ତୁ p && (slen == len) && (memcmp(p, s, len) == 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_find_max_phandle(ଅଚଳ ଶୂନ୍ୟ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = -1;

	ଯେତେବେଳେ (true) {
		uint32_t ମୂଲ୍ୟ;

		offset = fdt_next_ଗଣ୍ଠି(fdt, offset, ଖାଲି);
		ଯଦି (offset < 0) {
			ଯଦି (offset == -FDT_ERR_NOTFOUND)
				ଭାଙ୍ଗ;

			ଫେରନ୍ତୁ offset;
		}

		ମୂଲ୍ୟ = fdt_get_phandle(fdt, offset);

		ଯଦି (ମୂଲ୍ୟ > max)
			max = ମୂଲ୍ୟ;
	}

	ଯଦି (phandle)
		*phandle = max;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_generate_phandle(ଅଚଳ ଶୂନ୍ୟ *fdt, uint32_t *phandle)
{
	uint32_t max;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	err = fdt_find_max_phandle(fdt, &max);
	ଯଦି (err < 0)
		ଫେରନ୍ତୁ err;

	ଯଦି (max == FDT_MAX_PHANDLE)
		ଫେରନ୍ତୁ -FDT_ERR_NOPHANDLES;

	ଯଦି (phandle)
		*phandle = max + 1;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_reserve_entry *fdt_mem_rsv(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = n * ଆକାର(ସଂରଚନା fdt_reserve_entry);
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ଯଦି (!can_assume(VALID_ନିବେଶ)) {
		ଯଦି (absoffset < fdt_off_mem_rsvmap(fdt))
			ଫେରନ୍ତୁ ଖାଲି;
		ଯଦି (absoffset > fdt_totalsize(fdt) -
		    ଆକାର(ସଂରଚନା fdt_reserve_entry))
			ଫେରନ୍ତୁ ଖାଲି;
	}
	ଫେରନ୍ତୁ fdt_mem_rsv_(fdt, n);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_get_mem_rsv(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, uint64_t *address, uint64_t *size)
{
	ଅଚଳ ସଂରଚନା fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ଯଦି (!can_assume(VALID_ନିବେଶ) && !re)
		ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_ଅଙ୍କ_mem_rsv(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ଅଚଳ ସଂରଚନା fdt_reserve_entry *re;

	ପାଇଁ (i = 0; (re = fdt_mem_rsv(fdt, i)) != ଖାଲି; i++) {
		ଯଦି (fdt64_ld_(&re->size) == 0)
			ଫେରନ୍ତୁ i;
	}
	ଫେରନ୍ତୁ -FDT_ERR_TRUNCATED;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nextprop_(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
	uint32_t tag;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nextoffset;

	କର {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ବାଛ (tag) {
		ସ୍ଥିତି FDT_END:
			ଯଦି (nextoffset >= 0)
				ଫେରନ୍ତୁ -FDT_ERR_BADSTRUCTURE;
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ nextoffset;

		ସ୍ଥିତି FDT_PROP:
			ଫେରନ୍ତୁ offset;
		}
		offset = nextoffset;
	} ଯେତେବେଳେ (tag == FDT_NOP);

	ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_subnode_offset_namelen(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset,
			       ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth;

	FDT_RO_PROBE(fdt);

	ପାଇଁ (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_ଗଣ୍ଠି(fdt, offset, &depth))
		ଯଦି ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ଫେରନ୍ତୁ offset;

	ଯଦି (depth < 0)
		ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;
	ଫେରନ୍ତୁ offset; /* error */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_subnode_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା parentoffset,
		       ଅଚଳ ଅକ୍ଷର *name)
{
	ଫେରନ୍ତୁ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_path_offset_namelen(ଅଚଳ ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen)
{
	ଅଚଳ ଅକ୍ଷର *end = path + namelen;
	ଅଚଳ ଅକ୍ଷର *p = path;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ଯଦି (*path != '/') {
		ଅଚଳ ଅକ୍ଷର *q = memchr(path, '/', end - p);

		ଯଦି (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ଯଦି (!p)
			ଫେରନ୍ତୁ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ଯେତେବେଳେ (p < end) {
		ଅଚଳ ଅକ୍ଷର *q;

		ଯେତେବେଳେ (*p == '/') {
			p++;
			ଯଦି (p == end)
				ଫେରନ୍ତୁ offset;
		}
		q = memchr(p, '/', end - p);
		ଯଦି (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ଯଦି (offset < 0)
			ଫେରନ୍ତୁ offset;

		p = q;
	}

	ଫେରନ୍ତୁ offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_path_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *path)
{
	ଫେରନ୍ତୁ fdt_path_offset_namelen(fdt, path, strlen(path));
}

ଅଚଳ ଅକ୍ଷର *fdt_get_name(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *len)
{
	ଅଚଳ ସଂରଚନା fdt_ଗଣ୍ଠି_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ଅଚଳ ଅକ୍ଷର *nameptr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	ଯଦି (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ଗଣ୍ଠି_offset(fdt, nodeoffset)) < 0))
			ଯାଅ fail;

	nameptr = nh->name;

	ଯଦି (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ଅଚଳ ଅକ୍ଷର *leaf;
		leaf = strrchr(nameptr, '/');
		ଯଦି (leaf == ଖାଲି) {
			err = -FDT_ERR_BADSTRUCTURE;
			ଯାଅ fail;
		}
		nameptr = leaf+1;
	}

	ଯଦି (len)
		*len = strlen(nameptr);

	ଫେରନ୍ତୁ nameptr;

 fail:
	ଯଦି (len)
		*len = err;
	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_first_property_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;

	ଯଦି ((offset = fdt_check_ଗଣ୍ଠି_offset(fdt, nodeoffset)) < 0)
		ଫେରନ୍ତୁ offset;

	ଫେରନ୍ତୁ nextprop_(fdt, offset);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_next_property_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
	ଯଦି ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ଫେରନ୍ତୁ offset;

	ଫେରନ୍ତୁ nextprop_(fdt, offset);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_property *fdt_get_property_by_offset_(ଅଚଳ ଶୂନ୍ୟ *fdt,
						              ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset,
						              ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	ଅଚଳ ସଂରଚନା fdt_property *prop;

	ଯଦି (!can_assume(VALID_ନିବେଶ) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ଯଦି (lenp)
			*lenp = err;
		ଫେରନ୍ତୁ ଖାଲି;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ଯଦି (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ଫେରନ୍ତୁ prop;
}

ଅଚଳ ସଂରଚନା fdt_property *fdt_get_property_by_offset(ଅଚଳ ଶୂନ୍ୟ *fdt,
						      ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset,
						      ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ଯଦି (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ଯଦି (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଫେରନ୍ତୁ fdt_get_property_by_offset_(fdt, offset, lenp);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_property *fdt_get_property_namelen_(ଅଚଳ ଶୂନ୍ୟ *fdt,
						            ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset,
						            ଅଚଳ ଅକ୍ଷର *name,
						            ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen,
							    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp,
							    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *poffset)
{
	ପାଇଁ (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ଅଚଳ ସଂରଚନା fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ଯଦି (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			ଭାଙ୍ଗ;
		}
		ଯଦି (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ଯଦି (poffset)
				*poffset = offset;
			ଫେରନ୍ତୁ prop;
		}
	}

	ଯଦି (lenp)
		*lenp = offset;
	ଫେରନ୍ତୁ ଖାଲି;
}


ଅଚଳ ସଂରଚନା fdt_property *fdt_get_property_namelen(ଅଚଳ ଶୂନ୍ୟ *fdt,
						    ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset,
						    ଅଚଳ ଅକ୍ଷର *name,
						    ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ଯଦି (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ଯଦି (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଫେରନ୍ତୁ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 ଖାଲି);
}


ଅଚଳ ସଂରଚନା fdt_property *fdt_get_property(ଅଚଳ ଶୂନ୍ୟ *fdt,
					    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
					    ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	ଫେରନ୍ତୁ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ଅଚଳ ଶୂନ୍ୟ *fdt_getprop_namelen(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
				ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା poffset;
	ଅଚଳ ସଂରଚନା fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ଯଦି (!prop)
		ଫେରନ୍ତୁ ଖାଲି;

	/* Handle realignment */
	ଯଦି (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + ଆକାର(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ଫେରନ୍ତୁ prop->data + 4;
	ଫେରନ୍ତୁ prop->data;
}

ଅଚଳ ଶୂନ୍ୟ *fdt_getprop_by_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset,
				  ଅଚଳ ଅକ୍ଷର **namep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	ଅଚଳ ସଂରଚନା fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ଯଦି (!prop)
		ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (namep) {
		ଅଚଳ ଅକ୍ଷର *name;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen;

		ଯଦି (!can_assume(VALID_ନିବେଶ)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ଯଦି (!name) {
				ଯଦି (lenp)
					*lenp = namelen;
				ଫେରନ୍ତୁ ଖାଲି;
			}
			*namep = name;
		} ଅନ୍ୟଥା {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ଯଦି (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + ଆକାର(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ଫେରନ୍ତୁ prop->data + 4;
	ଫେରନ୍ତୁ prop->data;
}

ଅଚଳ ଶୂନ୍ୟ *fdt_getprop(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	ଫେରନ୍ତୁ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ଅଚଳ fdt32_t *php;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ଯଦି (!php || (len != ଆକାର(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ଯଦି (!php || (len != ଆକାର(*php)))
			ଫେରନ୍ତୁ 0;
	}

	ଫେରନ୍ତୁ fdt32_ld_(php);
}

ଅଚଳ ଅକ୍ଷର *fdt_get_alias_namelen(ଅଚଳ ଶୂନ୍ୟ *fdt,
				  ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ଯଦି (aliasoffset < 0)
		ଫେରନ୍ତୁ ଖାଲି;

	ଫେରନ୍ତୁ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, ଖାଲି);
}

ଅଚଳ ଅକ୍ଷର *fdt_get_alias(ଅଚଳ ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *name)
{
	ଫେରନ୍ତୁ fdt_get_alias_namelen(fdt, name, strlen(name));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_get_path(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା buflen)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pdepth = 0, p = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, depth, namelen;
	ଅଚଳ ଅକ୍ଷର *name;

	FDT_RO_PROBE(fdt);

	ଯଦି (buflen < 2)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	ପାଇଁ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ଗଣ୍ଠି(fdt, offset, &depth)) {
		ଯେତେବେଳେ (pdepth > depth) {
			କର {
				p--;
			} ଯେତେବେଳେ (buf[p-1] != '/');
			pdepth--;
		}

		ଯଦି (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ଯଦି (!name)
				ଫେରନ୍ତୁ namelen;
			ଯଦି ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ଯଦି (offset == nodeoffset) {
			ଯଦି (pdepth < (depth + 1))
				ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

			ଯଦି (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ଫେରନ୍ତୁ 0;
		}
	}

	ଯଦି ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;
	ଅନ୍ୟଥା ଯଦି (offset == -FDT_ERR_BADOFFSET)
		ଫେରନ୍ତୁ -FDT_ERR_BADSTRUCTURE;

	ଫେରନ୍ତୁ offset; /* error from fdt_next_node() */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_supernode_atdepth_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
				 ପୂର୍ଣ୍ଣସଂଖ୍ୟା supernodedepth, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *nodedepth)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, depth;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ଯଦି (supernodedepth < 0)
		ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;

	ପାଇଁ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ଗଣ୍ଠି(fdt, offset, &depth)) {
		ଯଦି (depth == supernodedepth)
			supernodeoffset = offset;

		ଯଦି (offset == nodeoffset) {
			ଯଦି (nodedepth)
				*nodedepth = depth;

			ଯଦି (supernodedepth > depth)
				ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ supernodeoffset;
		}
	}

	ଯଦି (!can_assume(VALID_ନିବେଶ)) {
		ଯଦି ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;
		ଅନ୍ୟଥା ଯଦି (offset == -FDT_ERR_BADOFFSET)
			ଫେରନ୍ତୁ -FDT_ERR_BADSTRUCTURE;
	}

	ଫେରନ୍ତୁ offset; /* error from fdt_next_node() */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_ଗଣ୍ଠି_depth(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodedepth;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ଯଦି (err)
		ଫେରନ୍ତୁ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ଫେରନ୍ତୁ nodedepth;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parent_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodedepth = fdt_ଗଣ୍ଠି_depth(fdt, nodeoffset);

	ଯଦି (nodedepth < 0)
		ଫେରନ୍ତୁ nodedepth;
	ଫେରନ୍ତୁ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_ଗଣ୍ଠି_offset_by_prop_ମୂଲ୍ୟ(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startoffset,
				  ଅଚଳ ଅକ୍ଷର *propname,
				  ଅଚଳ ଶୂନ୍ୟ *propval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା proplen)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
	ଅଚଳ ଶୂନ୍ୟ *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	ପାଇଁ (offset = fdt_next_ଗଣ୍ଠି(fdt, startoffset, ଖାଲି);
	     offset >= 0;
	     offset = fdt_next_ଗଣ୍ଠି(fdt, offset, ଖାଲି)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ଯଦି (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ଫେରନ୍ତୁ offset;
	}

	ଫେରନ୍ତୁ offset; /* error from fdt_next_node() */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_ଗଣ୍ଠି_offset_by_phandle(ଅଚଳ ଶୂନ୍ୟ *fdt, uint32_t phandle)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;

	ଯଦି ((phandle == 0) || (phandle == ~0U))
		ଫେରନ୍ତୁ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	ପାଇଁ (offset = fdt_next_ଗଣ୍ଠି(fdt, -1, ଖାଲି);
	     offset >= 0;
	     offset = fdt_next_ଗଣ୍ଠି(fdt, offset, ଖାଲି)) {
		ଯଦି (fdt_get_phandle(fdt, offset) == phandle)
			ଫେରନ୍ତୁ offset;
	}

	ଫେରନ୍ତୁ offset; /* error from fdt_next_node() */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_stringlist_contains(ଅଚଳ ଅକ୍ଷର *strlist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listlen, ଅଚଳ ଅକ୍ଷର *str)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = strlen(str);
	ଅଚଳ ଅକ୍ଷର *p;

	ଯେତେବେଳେ (listlen >= len) {
		ଯଦି (memcmp(str, strlist, len+1) == 0)
			ଫେରନ୍ତୁ 1;
		p = memchr(strlist, '\0', listlen);
		ଯଦି (!p)
			ଫେରନ୍ତୁ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_stringlist_ଗଣନା(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *property)
{
	ଅଚଳ ଅକ୍ଷର *list, *end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା length, ଗଣନା_2 = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ଯଦି (!list)
		ଫେରନ୍ତୁ length;

	end = list + length;

	ଯେତେବେଳେ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ଯଦି (list + length > end)
			ଫେରନ୍ତୁ -FDT_ERR_BADVALUE;

		list += length;
		ଗଣନା_2++;
	}

	ଫେରନ୍ତୁ ଗଣନା_2;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_stringlist_ସନ୍ଧାନ(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *property,
			  ଅଚଳ ଅକ୍ଷର *string)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା length, len, idx = 0;
	ଅଚଳ ଅକ୍ଷର *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ଯଦି (!list)
		ଫେରନ୍ତୁ length;

	len = strlen(string) + 1;
	end = list + length;

	ଯେତେବେଳେ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ଯଦି (list + length > end)
			ଫେରନ୍ତୁ -FDT_ERR_BADVALUE;

		ଯଦି (length == len && memcmp(list, string, length) == 0)
			ଫେରନ୍ତୁ idx;

		list += length;
		idx++;
	}

	ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;
}

ଅଚଳ ଅକ୍ଷର *fdt_stringlist_get(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			       ଅଚଳ ଅକ୍ଷର *property, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx,
			       ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
	ଅଚଳ ଅକ୍ଷର *list, *end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ଯଦି (!list) {
		ଯଦି (lenp)
			*lenp = length;

		ଫେରନ୍ତୁ ଖାଲି;
	}

	end = list + length;

	ଯେତେବେଳେ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ଯଦି (list + length > end) {
			ଯଦି (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ଫେରନ୍ତୁ ଖାଲି;
		}

		ଯଦି (idx == 0) {
			ଯଦି (lenp)
				*lenp = length - 1;

			ଫେରନ୍ତୁ list;
		}

		list += length;
		idx--;
	}

	ଯଦି (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_ଗଣ୍ଠି_check_compatible(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			      ଅଚଳ ଅକ୍ଷର *compatible)
{
	ଅଚଳ ଶୂନ୍ୟ *prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ଯଦି (!prop)
		ଫେରନ୍ତୁ len;

	ଫେରନ୍ତୁ !fdt_stringlist_contains(prop, len, compatible);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_ଗଣ୍ଠି_offset_by_compatible(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startoffset,
				  ଅଚଳ ଅକ୍ଷର *compatible)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	ପାଇଁ (offset = fdt_next_ଗଣ୍ଠି(fdt, startoffset, ଖାଲି);
	     offset >= 0;
	     offset = fdt_next_ଗଣ୍ଠି(fdt, offset, ଖାଲି)) {
		err = fdt_ଗଣ୍ଠି_check_compatible(fdt, offset, compatible);
		ଯଦି ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ଫେରନ୍ତୁ err;
		ଅନ୍ୟଥା ଯଦି (err == 0)
			ଫେରନ୍ତୁ offset;
	}

	ଫେରନ୍ତୁ offset; /* error from fdt_next_node() */
}
