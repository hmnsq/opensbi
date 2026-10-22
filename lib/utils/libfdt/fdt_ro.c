// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাৱ "libfdt_env.h"

#অন্তর্ভাৱ <fdt.h>
#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ "libfdt_internal.h"

স্থির পূর্ণাঙ্ক fdt_nodename_eq_(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset,
			    ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক len)
{
	পূর্ণাঙ্ক olen;
	ধ্রুৱ ৱর্ণ *p = fdt_get_name(fdt, offset, &olen);

	যদি (!p || olen < len)
		/* short match */
		ঘূৰাই 0;

	যদি (memcmp(p, s, len) != 0)
		ঘূৰাই 0;

	যদি (p[len] == '\0')
		ঘূৰাই 1;
	অন্যথা যদি (!memchr(s, '@', len) && (p[len] == '@'))
		ঘূৰাই 1;
	অন্যথা
		ঘূৰাই 0;
}

ধ্রুৱ ৱর্ণ *fdt_get_string(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক stroffset, পূর্ণাঙ্ক *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	পূর্ণাঙ্ক err;
	ধ্রুৱ ৱর্ণ *s, *n;

	যদি (can_assume(VALID_প্ৰবেশ)) {
		s = (ধ্রুৱ ৱর্ণ *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		যদি (lenp)
			*lenp = strlen(s);
		ঘূৰাই s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	যদি (totalsize < 0)
		গচ্ছ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	যদি (absoffset >= (unsigned)totalsize)
		গচ্ছ fail;
	len = totalsize - absoffset;

	যদি (fdt_magic(fdt) == FDT_MAGIC) {
		যদি (stroffset < 0)
			গচ্ছ fail;
		যদি (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			যদি ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				গচ্ছ fail;
			যদি ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} অন্যথা যদি (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned পূর্ণাঙ্ক sw_stroffset = -stroffset;

		যদি ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			গচ্ছ fail;
		যদি (sw_stroffset < len)
			len = sw_stroffset;
	} অন্যথা {
		err = -FDT_ERR_INTERNAL;
		গচ্ছ fail;
	}

	s = (ধ্রুৱ ৱর্ণ *)fdt + absoffset;
	n = memchr(s, '\0', len);
	যদি (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		গচ্ছ fail;
	}

	যদি (lenp)
		*lenp = n - s;
	ঘূৰাই s;

fail:
	যদি (lenp)
		*lenp = err;
	ঘূৰাই রিক্ত;
}

ধ্রুৱ ৱর্ণ *fdt_string(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক stroffset)
{
	ঘূৰাই fdt_get_string(fdt, stroffset, রিক্ত);
}

স্থির পূর্ণাঙ্ক fdt_string_eq_(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক stroffset,
			  ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক len)
{
	পূর্ণাঙ্ক slen;
	ধ্রুৱ ৱর্ণ *p = fdt_get_string(fdt, stroffset, &slen);

	ঘূৰাই p && (slen == len) && (memcmp(p, s, len) == 0);
}

পূর্ণাঙ্ক fdt_find_max_phandle(ধ্রুৱ শূন্য *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	পূর্ণাঙ্ক offset = -1;

	যেতিয়ালৈ (true) {
		uint32_t মান;

		offset = fdt_next_গ্ৰন্থি(fdt, offset, রিক্ত);
		যদি (offset < 0) {
			যদি (offset == -FDT_ERR_NOTFOUND)
				ভঙ্গ;

			ঘূৰাই offset;
		}

		মান = fdt_get_phandle(fdt, offset);

		যদি (মান > max)
			max = মান;
	}

	যদি (phandle)
		*phandle = max;

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_generate_phandle(ধ্রুৱ শূন্য *fdt, uint32_t *phandle)
{
	uint32_t max;
	পূর্ণাঙ্ক err;

	err = fdt_find_max_phandle(fdt, &max);
	যদি (err < 0)
		ঘূৰাই err;

	যদি (max == FDT_MAX_PHANDLE)
		ঘূৰাই -FDT_ERR_NOPHANDLES;

	যদি (phandle)
		*phandle = max + 1;

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_reserve_entry *fdt_mem_rsv(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক n)
{
	unsigned পূর্ণাঙ্ক offset = n * পরিমাণ(সংরচনা fdt_reserve_entry);
	unsigned পূর্ণাঙ্ক absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	যদি (!can_assume(VALID_প্ৰবেশ)) {
		যদি (absoffset < fdt_off_mem_rsvmap(fdt))
			ঘূৰাই রিক্ত;
		যদি (absoffset > fdt_totalsize(fdt) -
		    পরিমাণ(সংরচনা fdt_reserve_entry))
			ঘূৰাই রিক্ত;
	}
	ঘূৰাই fdt_mem_rsv_(fdt, n);
}

পূর্ণাঙ্ক fdt_get_mem_rsv(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক n, uint64_t *address, uint64_t *size)
{
	ধ্রুৱ সংরচনা fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	যদি (!can_assume(VALID_প্ৰবেশ) && !re)
		ঘূৰাই -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_অঙ্ক_mem_rsv(ধ্রুৱ শূন্য *fdt)
{
	পূর্ণাঙ্ক i;
	ধ্রুৱ সংরচনা fdt_reserve_entry *re;

	বাবে (i = 0; (re = fdt_mem_rsv(fdt, i)) != রিক্ত; i++) {
		যদি (fdt64_ld_(&re->size) == 0)
			ঘূৰাই i;
	}
	ঘূৰাই -FDT_ERR_TRUNCATED;
}

স্থির পূর্ণাঙ্ক nextprop_(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	uint32_t tag;
	পূর্ণাঙ্ক nextoffset;

	কৰা {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ৱিকল্প (tag) {
		পক্ষ FDT_END:
			যদি (nextoffset >= 0)
				ঘূৰাই -FDT_ERR_BADSTRUCTURE;
			অন্যথা
				ঘূৰাই nextoffset;

		পক্ষ FDT_PROP:
			ঘূৰাই offset;
		}
		offset = nextoffset;
	} যেতিয়ালৈ (tag == FDT_NOP);

	ঘূৰাই -FDT_ERR_NOTFOUND;
}

পূর্ণাঙ্ক fdt_subnode_offset_namelen(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset,
			       ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক namelen)
{
	পূর্ণাঙ্ক depth;

	FDT_RO_PROBE(fdt);

	বাবে (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_গ্ৰন্থি(fdt, offset, &depth))
		যদি ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ঘূৰাই offset;

	যদি (depth < 0)
		ঘূৰাই -FDT_ERR_NOTFOUND;
	ঘূৰাই offset; /* error */
}

পূর্ণাঙ্ক fdt_subnode_offset(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক parentoffset,
		       ধ্রুৱ ৱর্ণ *name)
{
	ঘূৰাই fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

পূর্ণাঙ্ক fdt_path_offset_namelen(ধ্রুৱ শূন্য *fdt, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক namelen)
{
	ধ্রুৱ ৱর্ণ *end = path + namelen;
	ধ্রুৱ ৱর্ণ *p = path;
	পূর্ণাঙ্ক offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	যদি (*path != '/') {
		ধ্রুৱ ৱর্ণ *q = memchr(path, '/', end - p);

		যদি (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		যদি (!p)
			ঘূৰাই -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	যেতিয়ালৈ (p < end) {
		ধ্রুৱ ৱর্ণ *q;

		যেতিয়ালৈ (*p == '/') {
			p++;
			যদি (p == end)
				ঘূৰাই offset;
		}
		q = memchr(p, '/', end - p);
		যদি (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		যদি (offset < 0)
			ঘূৰাই offset;

		p = q;
	}

	ঘূৰাই offset;
}

পূর্ণাঙ্ক fdt_path_offset(ধ্রুৱ শূন্য *fdt, ধ্রুৱ ৱর্ণ *path)
{
	ঘূৰাই fdt_path_offset_namelen(fdt, path, strlen(path));
}

ধ্রুৱ ৱর্ণ *fdt_get_name(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, পূর্ণাঙ্ক *len)
{
	ধ্রুৱ সংরচনা fdt_গ্ৰন্থি_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ধ্রুৱ ৱর্ণ *nameptr;
	পূর্ণাঙ্ক err;

	যদি (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_গ্ৰন্থি_offset(fdt, nodeoffset)) < 0))
			গচ্ছ fail;

	nameptr = nh->name;

	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ধ্রুৱ ৱর্ণ *leaf;
		leaf = strrchr(nameptr, '/');
		যদি (leaf == রিক্ত) {
			err = -FDT_ERR_BADSTRUCTURE;
			গচ্ছ fail;
		}
		nameptr = leaf+1;
	}

	যদি (len)
		*len = strlen(nameptr);

	ঘূৰাই nameptr;

 fail:
	যদি (len)
		*len = err;
	ঘূৰাই রিক্ত;
}

পূর্ণাঙ্ক fdt_first_property_offset(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক offset;

	যদি ((offset = fdt_check_গ্ৰন্থি_offset(fdt, nodeoffset)) < 0)
		ঘূৰাই offset;

	ঘূৰাই nextprop_(fdt, offset);
}

পূর্ণাঙ্ক fdt_next_property_offset(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	যদি ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ঘূৰাই offset;

	ঘূৰাই nextprop_(fdt, offset);
}

স্থির ধ্রুৱ সংরচনা fdt_property *fdt_get_property_by_offset_(ধ্রুৱ শূন্য *fdt,
						              পূর্ণাঙ্ক offset,
						              পূর্ণাঙ্ক *lenp)
{
	পূর্ণাঙ্ক err;
	ধ্রুৱ সংরচনা fdt_property *prop;

	যদি (!can_assume(VALID_প্ৰবেশ) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		যদি (lenp)
			*lenp = err;
		ঘূৰাই রিক্ত;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	যদি (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ঘূৰাই prop;
}

ধ্রুৱ সংরচনা fdt_property *fdt_get_property_by_offset(ধ্রুৱ শূন্য *fdt,
						      পূর্ণাঙ্ক offset,
						      পূর্ণাঙ্ক *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		যদি (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ঘূৰাই রিক্ত;
	}

	ঘূৰাই fdt_get_property_by_offset_(fdt, offset, lenp);
}

স্থির ধ্রুৱ সংরচনা fdt_property *fdt_get_property_namelen_(ধ্রুৱ শূন্য *fdt,
						            পূর্ণাঙ্ক offset,
						            ধ্রুৱ ৱর্ণ *name,
						            পূর্ণাঙ্ক namelen,
							    পূর্ণাঙ্ক *lenp,
							    পূর্ণাঙ্ক *poffset)
{
	বাবে (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ধ্রুৱ সংরচনা fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		যদি (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			ভঙ্গ;
		}
		যদি (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			যদি (poffset)
				*poffset = offset;
			ঘূৰাই prop;
		}
	}

	যদি (lenp)
		*lenp = offset;
	ঘূৰাই রিক্ত;
}


ধ্রুৱ সংরচনা fdt_property *fdt_get_property_namelen(ধ্রুৱ শূন্য *fdt,
						    পূর্ণাঙ্ক offset,
						    ধ্রুৱ ৱর্ণ *name,
						    পূর্ণাঙ্ক namelen, পূর্ণাঙ্ক *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		যদি (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ঘূৰাই রিক্ত;
	}

	ঘূৰাই fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 রিক্ত);
}


ধ্রুৱ সংরচনা fdt_property *fdt_get_property(ধ্রুৱ শূন্য *fdt,
					    পূর্ণাঙ্ক nodeoffset,
					    ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক *lenp)
{
	ঘূৰাই fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ধ্রুৱ শূন্য *fdt_getprop_namelen(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
				ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক namelen, পূর্ণাঙ্ক *lenp)
{
	পূর্ণাঙ্ক poffset;
	ধ্রুৱ সংরচনা fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	যদি (!prop)
		ঘূৰাই রিক্ত;

	/* Handle realignment */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + পরিমাণ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ঘূৰাই prop->data + 4;
	ঘূৰাই prop->data;
}

ধ্রুৱ শূন্য *fdt_getprop_by_offset(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক offset,
				  ধ্রুৱ ৱর্ণ **namep, পূর্ণাঙ্ক *lenp)
{
	ধ্রুৱ সংরচনা fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	যদি (!prop)
		ঘূৰাই রিক্ত;
	যদি (namep) {
		ধ্রুৱ ৱর্ণ *name;
		পূর্ণাঙ্ক namelen;

		যদি (!can_assume(VALID_প্ৰবেশ)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			যদি (!name) {
				যদি (lenp)
					*lenp = namelen;
				ঘূৰাই রিক্ত;
			}
			*namep = name;
		} অন্যথা {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + পরিমাণ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ঘূৰাই prop->data + 4;
	ঘূৰাই prop->data;
}

ধ্রুৱ শূন্য *fdt_getprop(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক *lenp)
{
	ঘূৰাই fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	ধ্রুৱ fdt32_t *php;
	পূর্ণাঙ্ক len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	যদি (!php || (len != পরিমাণ(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		যদি (!php || (len != পরিমাণ(*php)))
			ঘূৰাই 0;
	}

	ঘূৰাই fdt32_ld_(php);
}

ধ্রুৱ ৱর্ণ *fdt_get_alias_namelen(ধ্রুৱ শূন্য *fdt,
				  ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক namelen)
{
	পূর্ণাঙ্ক aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	যদি (aliasoffset < 0)
		ঘূৰাই রিক্ত;

	ঘূৰাই fdt_getprop_namelen(fdt, aliasoffset, name, namelen, রিক্ত);
}

ধ্রুৱ ৱর্ণ *fdt_get_alias(ধ্রুৱ শূন্য *fdt, ধ্রুৱ ৱর্ণ *name)
{
	ঘূৰাই fdt_get_alias_namelen(fdt, name, strlen(name));
}

পূর্ণাঙ্ক fdt_get_path(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ৱর্ণ *buf, পূর্ণাঙ্ক buflen)
{
	পূর্ণাঙ্ক pdepth = 0, p = 0;
	পূর্ণাঙ্ক offset, depth, namelen;
	ধ্রুৱ ৱর্ণ *name;

	FDT_RO_PROBE(fdt);

	যদি (buflen < 2)
		ঘূৰাই -FDT_ERR_NOSPACE;

	বাবে (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_গ্ৰন্থি(fdt, offset, &depth)) {
		যেতিয়ালৈ (pdepth > depth) {
			কৰা {
				p--;
			} যেতিয়ালৈ (buf[p-1] != '/');
			pdepth--;
		}

		যদি (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			যদি (!name)
				ঘূৰাই namelen;
			যদি ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		যদি (offset == nodeoffset) {
			যদি (pdepth < (depth + 1))
				ঘূৰাই -FDT_ERR_NOSPACE;

			যদি (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ঘূৰাই 0;
		}
	}

	যদি ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ঘূৰাই -FDT_ERR_BADOFFSET;
	অন্যথা যদি (offset == -FDT_ERR_BADOFFSET)
		ঘূৰাই -FDT_ERR_BADSTRUCTURE;

	ঘূৰাই offset; /* error from fdt_next_node() */
}

পূর্ণাঙ্ক fdt_supernode_atdepth_offset(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
				 পূর্ণাঙ্ক supernodedepth, পূর্ণাঙ্ক *nodedepth)
{
	পূর্ণাঙ্ক offset, depth;
	পূর্ণাঙ্ক supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	যদি (supernodedepth < 0)
		ঘূৰাই -FDT_ERR_NOTFOUND;

	বাবে (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_গ্ৰন্থি(fdt, offset, &depth)) {
		যদি (depth == supernodedepth)
			supernodeoffset = offset;

		যদি (offset == nodeoffset) {
			যদি (nodedepth)
				*nodedepth = depth;

			যদি (supernodedepth > depth)
				ঘূৰাই -FDT_ERR_NOTFOUND;
			অন্যথা
				ঘূৰাই supernodeoffset;
		}
	}

	যদি (!can_assume(VALID_প্ৰবেশ)) {
		যদি ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ঘূৰাই -FDT_ERR_BADOFFSET;
		অন্যথা যদি (offset == -FDT_ERR_BADOFFSET)
			ঘূৰাই -FDT_ERR_BADSTRUCTURE;
	}

	ঘূৰাই offset; /* error from fdt_next_node() */
}

পূর্ণাঙ্ক fdt_গ্ৰন্থি_depth(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক nodedepth;
	পূর্ণাঙ্ক err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	যদি (err)
		ঘূৰাই (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ঘূৰাই nodedepth;
}

পূর্ণাঙ্ক fdt_parent_offset(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক nodedepth = fdt_গ্ৰন্থি_depth(fdt, nodeoffset);

	যদি (nodedepth < 0)
		ঘূৰাই nodedepth;
	ঘূৰাই fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, রিক্ত);
}

পূর্ণাঙ্ক fdt_গ্ৰন্থি_offset_by_prop_মান(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক startoffset,
				  ধ্রুৱ ৱর্ণ *propname,
				  ধ্রুৱ শূন্য *propval, পূর্ণাঙ্ক proplen)
{
	পূর্ণাঙ্ক offset;
	ধ্রুৱ শূন্য *val;
	পূর্ণাঙ্ক len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	বাবে (offset = fdt_next_গ্ৰন্থি(fdt, startoffset, রিক্ত);
	     offset >= 0;
	     offset = fdt_next_গ্ৰন্থি(fdt, offset, রিক্ত)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		যদি (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ঘূৰাই offset;
	}

	ঘূৰাই offset; /* error from fdt_next_node() */
}

পূর্ণাঙ্ক fdt_গ্ৰন্থি_offset_by_phandle(ধ্রুৱ শূন্য *fdt, uint32_t phandle)
{
	পূর্ণাঙ্ক offset;

	যদি ((phandle == 0) || (phandle == ~0U))
		ঘূৰাই -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	বাবে (offset = fdt_next_গ্ৰন্থি(fdt, -1, রিক্ত);
	     offset >= 0;
	     offset = fdt_next_গ্ৰন্থি(fdt, offset, রিক্ত)) {
		যদি (fdt_get_phandle(fdt, offset) == phandle)
			ঘূৰাই offset;
	}

	ঘূৰাই offset; /* error from fdt_next_node() */
}

পূর্ণাঙ্ক fdt_stringlist_contains(ধ্রুৱ ৱর্ণ *strlist, পূর্ণাঙ্ক listlen, ধ্রুৱ ৱর্ণ *str)
{
	পূর্ণাঙ্ক len = strlen(str);
	ধ্রুৱ ৱর্ণ *p;

	যেতিয়ালৈ (listlen >= len) {
		যদি (memcmp(str, strlist, len+1) == 0)
			ঘূৰাই 1;
		p = memchr(strlist, '\0', listlen);
		যদি (!p)
			ঘূৰাই 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_stringlist_গুনতি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুৱ ৱর্ণ *property)
{
	ধ্রুৱ ৱর্ণ *list, *end;
	পূর্ণাঙ্ক length, গুনতি = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	যদি (!list)
		ঘূৰাই length;

	end = list + length;

	যেতিয়ালৈ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + length > end)
			ঘূৰাই -FDT_ERR_BADVALUE;

		list += length;
		গুনতি++;
	}

	ঘূৰাই গুনতি;
}

পূর্ণাঙ্ক fdt_stringlist_অনুসন্ধান(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুৱ ৱর্ণ *property,
			  ধ্রুৱ ৱর্ণ *string)
{
	পূর্ণাঙ্ক length, len, idx = 0;
	ধ্রুৱ ৱর্ণ *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	যদি (!list)
		ঘূৰাই length;

	len = strlen(string) + 1;
	end = list + length;

	যেতিয়ালৈ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + length > end)
			ঘূৰাই -FDT_ERR_BADVALUE;

		যদি (length == len && memcmp(list, string, length) == 0)
			ঘূৰাই idx;

		list += length;
		idx++;
	}

	ঘূৰাই -FDT_ERR_NOTFOUND;
}

ধ্রুৱ ৱর্ণ *fdt_stringlist_get(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			       ধ্রুৱ ৱর্ণ *property, পূর্ণাঙ্ক idx,
			       পূর্ণাঙ্ক *lenp)
{
	ধ্রুৱ ৱর্ণ *list, *end;
	পূর্ণাঙ্ক length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	যদি (!list) {
		যদি (lenp)
			*lenp = length;

		ঘূৰাই রিক্ত;
	}

	end = list + length;

	যেতিয়ালৈ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + length > end) {
			যদি (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ঘূৰাই রিক্ত;
		}

		যদি (idx == 0) {
			যদি (lenp)
				*lenp = length - 1;

			ঘূৰাই list;
		}

		list += length;
		idx--;
	}

	যদি (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ঘূৰাই রিক্ত;
}

পূর্ণাঙ্ক fdt_গ্ৰন্থি_check_compatible(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			      ধ্রুৱ ৱর্ণ *compatible)
{
	ধ্রুৱ শূন্য *prop;
	পূর্ণাঙ্ক len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	যদি (!prop)
		ঘূৰাই len;

	ঘূৰাই !fdt_stringlist_contains(prop, len, compatible);
}

পূর্ণাঙ্ক fdt_গ্ৰন্থি_offset_by_compatible(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক startoffset,
				  ধ্রুৱ ৱর্ণ *compatible)
{
	পূর্ণাঙ্ক offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	বাবে (offset = fdt_next_গ্ৰন্থি(fdt, startoffset, রিক্ত);
	     offset >= 0;
	     offset = fdt_next_গ্ৰন্থি(fdt, offset, রিক্ত)) {
		err = fdt_গ্ৰন্থি_check_compatible(fdt, offset, compatible);
		যদি ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ঘূৰাই err;
		অন্যথা যদি (err == 0)
			ঘূৰাই offset;
	}

	ঘূৰাই offset; /* error from fdt_next_node() */
}
