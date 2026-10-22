// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static පූර්ණ_සංඛ්‍යා fdt_nodename_eq_(const void *fdt, පූර්ණ_සංඛ්‍යා offset,
			    const var_na *s, පූර්ණ_සංඛ්‍යා len)
{
	පූර්ණ_සංඛ්‍යා olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	නම් (!p || olen < len)
		/* short match */
		ආපසු 0;

	නම් (memcmp(p, s, len) != 0)
		ආපසු 0;

	නම් (p[len] == '\0')
		ආපසු 1;
	නැතහොත් නම් (!memchr(s, '@', len) && (p[len] == '@'))
		ආපසු 1;
	නැතහොත්
		ආපසු 0;
}

const var_na *fdt_get_string(const void *fdt, පූර්ණ_සංඛ්‍යා stroffset, පූර්ණ_සංඛ්‍යා *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	පූර්ණ_සංඛ්‍යා err;
	const var_na *s, *n;

	නම් (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		නම් (lenp)
			*lenp = strlen(s);
		ආපසු s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	නම් (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	නම් (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	නම් (fdt_magic(fdt) == FDT_MAGIC) {
		නම් (stroffset < 0)
			goto fail;
		නම් (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			නම් ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			නම් ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} නැතහොත් නම් (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned පූර්ණ_සංඛ්‍යා sw_stroffset = -stroffset;

		නම් ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		නම් (sw_stroffset < len)
			len = sw_stroffset;
	} නැතහොත් {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	නම් (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	නම් (lenp)
		*lenp = n - s;
	ආපසු s;

fail:
	නම් (lenp)
		*lenp = err;
	ආපසු NULL;
}

const var_na *fdt_string(const void *fdt, පූර්ණ_සංඛ්‍යා stroffset)
{
	ආපසු fdt_get_string(fdt, stroffset, NULL);
}

static පූර්ණ_සංඛ්‍යා fdt_string_eq_(const void *fdt, පූර්ණ_සංඛ්‍යා stroffset,
			  const var_na *s, පූර්ණ_සංඛ්‍යා len)
{
	පූර්ණ_සංඛ්‍යා slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ආපසු p && (slen == len) && (memcmp(p, s, len) == 0);
}

පූර්ණ_සංඛ්‍යා fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	පූර්ණ_සංඛ්‍යා offset = -1;

	අතරතුර (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		නම් (offset < 0) {
			නම් (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ආපසු offset;
		}

		value = fdt_get_phandle(fdt, offset);

		නම් (value > max)
			max = value;
	}

	නම් (phandle)
		*phandle = max;

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	පූර්ණ_සංඛ්‍යා err;

	err = fdt_find_max_phandle(fdt, &max);
	නම් (err < 0)
		ආපසු err;

	නම් (max == FDT_MAX_PHANDLE)
		ආපසු -FDT_ERR_NOPHANDLES;

	නම් (phandle)
		*phandle = max + 1;

	ආපසු 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, පූර්ණ_සංඛ්‍යා n)
{
	unsigned පූර්ණ_සංඛ්‍යා offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned පූර්ණ_සංඛ්‍යා absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	නම් (!can_assume(VALID_INPUT)) {
		නම් (absoffset < fdt_off_mem_rsvmap(fdt))
			ආපසු NULL;
		නම් (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ආපසු NULL;
	}
	ආපසු fdt_mem_rsv_(fdt, n);
}

පූර්ණ_සංඛ්‍යා fdt_get_mem_rsv(const void *fdt, පූර්ණ_සංඛ්‍යා n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	නම් (!can_assume(VALID_INPUT) && !re)
		ආපසු -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_num_mem_rsv(const void *fdt)
{
	පූර්ණ_සංඛ්‍යා i;
	const sa_nracanaa fdt_reserve_entry *re;

	සඳහා (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		නම් (fdt64_ld_(&re->size) == 0)
			ආපසු i;
	}
	ආපසු -FDT_ERR_TRUNCATED;
}

static පූර්ණ_සංඛ්‍යා nextprop_(const void *fdt, පූර්ණ_සංඛ්‍යා offset)
{
	uint32_t tag;
	පූර්ණ_සංඛ්‍යා nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			නම් (nextoffset >= 0)
				ආපසු -FDT_ERR_BADSTRUCTURE;
			නැතහොත්
				ආපසු nextoffset;

		pak_sa FDT_PROP:
			ආපසු offset;
		}
		offset = nextoffset;
	} අතරතුර (tag == FDT_NOP);

	ආපසු -FDT_ERR_NOTFOUND;
}

පූර්ණ_සංඛ්‍යා fdt_subnode_offset_namelen(const void *fdt, පූර්ණ_සංඛ්‍යා offset,
			       const var_na *name, පූර්ණ_සංඛ්‍යා namelen)
{
	පූර්ණ_සංඛ්‍යා depth;

	FDT_RO_PROBE(fdt);

	සඳහා (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		නම් ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ආපසු offset;

	නම් (depth < 0)
		ආපසු -FDT_ERR_NOTFOUND;
	ආපසු offset; /* error */
}

පූර්ණ_සංඛ්‍යා fdt_subnode_offset(const void *fdt, පූර්ණ_සංඛ්‍යා parentoffset,
		       const var_na *name)
{
	ආපසු fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

පූර්ණ_සංඛ්‍යා fdt_path_offset_namelen(const void *fdt, const var_na *path, පූර්ණ_සංඛ්‍යා namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	පූර්ණ_සංඛ්‍යා offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	නම් (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		නම් (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		නම් (!p)
			ආපසු -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	අතරතුර (p < end) {
		const var_na *q;

		අතරතුර (*p == '/') {
			p++;
			නම් (p == end)
				ආපසු offset;
		}
		q = memchr(p, '/', end - p);
		නම් (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		නම් (offset < 0)
			ආපසු offset;

		p = q;
	}

	ආපසු offset;
}

පූර්ණ_සංඛ්‍යා fdt_path_offset(const void *fdt, const var_na *path)
{
	ආපසු fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, පූර්ණ_සංඛ්‍යා *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	පූර්ණ_සංඛ්‍යා err;

	නම් (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	නම් (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		නම් (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	නම් (len)
		*len = strlen(nameptr);

	ආපසු nameptr;

 fail:
	නම් (len)
		*len = err;
	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා fdt_first_property_offset(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset)
{
	පූර්ණ_සංඛ්‍යා offset;

	නම් ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ආපසු offset;

	ආපසු nextprop_(fdt, offset);
}

පූර්ණ_සංඛ්‍යා fdt_next_property_offset(const void *fdt, පූර්ණ_සංඛ්‍යා offset)
{
	නම් ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ආපසු offset;

	ආපසු nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              පූර්ණ_සංඛ්‍යා offset,
						              පූර්ණ_සංඛ්‍යා *lenp)
{
	පූර්ණ_සංඛ්‍යා err;
	const sa_nracanaa fdt_property *prop;

	නම් (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		නම් (lenp)
			*lenp = err;
		ආපසු NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	නම් (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ආපසු prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      පූර්ණ_සංඛ්‍යා offset,
						      පූර්ණ_සංඛ්‍යා *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	නම් (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		නම් (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ආපසු NULL;
	}

	ආපසු fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            පූර්ණ_සංඛ්‍යා offset,
						            const var_na *name,
						            පූර්ණ_සංඛ්‍යා namelen,
							    පූර්ණ_සංඛ්‍යා *lenp,
							    පූර්ණ_සංඛ්‍යා *poffset)
{
	සඳහා (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		නම් (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		නම් (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			නම් (poffset)
				*poffset = offset;
			ආපසු prop;
		}
	}

	නම් (lenp)
		*lenp = offset;
	ආපසු NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    පූර්ණ_සංඛ්‍යා offset,
						    const var_na *name,
						    පූර්ණ_සංඛ්‍යා namelen, පූර්ණ_සංඛ්‍යා *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	නම් (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		නම් (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ආපසු NULL;
	}

	ආපසු fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    පූර්ණ_සංඛ්‍යා nodeoffset,
					    const var_na *name, පූර්ණ_සංඛ්‍යා *lenp)
{
	ආපසු fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset,
				const var_na *name, පූර්ණ_සංඛ්‍යා namelen, පූර්ණ_සංඛ්‍යා *lenp)
{
	පූර්ණ_සංඛ්‍යා poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	නම් (!prop)
		ආපසු NULL;

	/* Handle realignment */
	නම් (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ආපසු prop->data + 4;
	ආපසු prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, පූර්ණ_සංඛ්‍යා offset,
				  const var_na **namep, පූර්ණ_සංඛ්‍යා *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	නම් (!prop)
		ආපසු NULL;
	නම් (namep) {
		const var_na *name;
		පූර්ණ_සංඛ්‍යා namelen;

		නම් (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			නම් (!name) {
				නම් (lenp)
					*lenp = namelen;
				ආපසු NULL;
			}
			*namep = name;
		} නැතහොත් {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	නම් (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ආපසු prop->data + 4;
	ආපසු prop->data;
}

const void *fdt_getprop(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset,
			const var_na *name, පූර්ණ_සංඛ්‍යා *lenp)
{
	ආපසු fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset)
{
	const fdt32_t *php;
	පූර්ණ_සංඛ්‍යා len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	නම් (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		නම් (!php || (len != parimaa_na(*php)))
			ආපසු 0;
	}

	ආපසු fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, පූර්ණ_සංඛ්‍යා namelen)
{
	පූර්ණ_සංඛ්‍යා aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	නම් (aliasoffset < 0)
		ආපසු NULL;

	ආපසු fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ආපසු fdt_get_alias_namelen(fdt, name, strlen(name));
}

පූර්ණ_සංඛ්‍යා fdt_get_path(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, var_na *buf, පූර්ණ_සංඛ්‍යා buflen)
{
	පූර්ණ_සංඛ්‍යා pdepth = 0, p = 0;
	පූර්ණ_සංඛ්‍යා offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	නම් (buflen < 2)
		ආපසු -FDT_ERR_NOSPACE;

	සඳහා (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		අතරතුර (pdepth > depth) {
			do {
				p--;
			} අතරතුර (buf[p-1] != '/');
			pdepth--;
		}

		නම් (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			නම් (!name)
				ආපසු namelen;
			නම් ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		නම් (offset == nodeoffset) {
			නම් (pdepth < (depth + 1))
				ආපසු -FDT_ERR_NOSPACE;

			නම් (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ආපසු 0;
		}
	}

	නම් ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ආපසු -FDT_ERR_BADOFFSET;
	නැතහොත් නම් (offset == -FDT_ERR_BADOFFSET)
		ආපසු -FDT_ERR_BADSTRUCTURE;

	ආපසු offset; /* error from fdt_next_node() */
}

පූර්ණ_සංඛ්‍යා fdt_supernode_atdepth_offset(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset,
				 පූර්ණ_සංඛ්‍යා supernodedepth, පූර්ණ_සංඛ්‍යා *nodedepth)
{
	පූර්ණ_සංඛ්‍යා offset, depth;
	පූර්ණ_සංඛ්‍යා supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	නම් (supernodedepth < 0)
		ආපසු -FDT_ERR_NOTFOUND;

	සඳහා (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		නම් (depth == supernodedepth)
			supernodeoffset = offset;

		නම් (offset == nodeoffset) {
			නම් (nodedepth)
				*nodedepth = depth;

			නම් (supernodedepth > depth)
				ආපසු -FDT_ERR_NOTFOUND;
			නැතහොත්
				ආපසු supernodeoffset;
		}
	}

	නම් (!can_assume(VALID_INPUT)) {
		නම් ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ආපසු -FDT_ERR_BADOFFSET;
		නැතහොත් නම් (offset == -FDT_ERR_BADOFFSET)
			ආපසු -FDT_ERR_BADSTRUCTURE;
	}

	ආපසු offset; /* error from fdt_next_node() */
}

පූර්ණ_සංඛ්‍යා fdt_node_depth(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset)
{
	පූර්ණ_සංඛ්‍යා nodedepth;
	පූර්ණ_සංඛ්‍යා err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	නම් (err)
		ආපසු (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ආපසු nodedepth;
}

පූර්ණ_සංඛ්‍යා fdt_parent_offset(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset)
{
	පූර්ණ_සංඛ්‍යා nodedepth = fdt_node_depth(fdt, nodeoffset);

	නම් (nodedepth < 0)
		ආපසු nodedepth;
	ආපසු fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

පූර්ණ_සංඛ්‍යා fdt_node_offset_by_prop_value(const void *fdt, පූර්ණ_සංඛ්‍යා startoffset,
				  const var_na *propname,
				  const void *propval, පූර්ණ_සංඛ්‍යා proplen)
{
	පූර්ණ_සංඛ්‍යා offset;
	const void *val;
	පූර්ණ_සංඛ්‍යා len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	සඳහා (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		නම් (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ආපසු offset;
	}

	ආපසු offset; /* error from fdt_next_node() */
}

පූර්ණ_සංඛ්‍යා fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	පූර්ණ_සංඛ්‍යා offset;

	නම් ((phandle == 0) || (phandle == ~0U))
		ආපසු -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	සඳහා (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		නම් (fdt_get_phandle(fdt, offset) == phandle)
			ආපසු offset;
	}

	ආපසු offset; /* error from fdt_next_node() */
}

පූර්ණ_සංඛ්‍යා fdt_stringlist_contains(const var_na *strlist, පූර්ණ_සංඛ්‍යා listlen, const var_na *str)
{
	පූර්ණ_සංඛ්‍යා len = strlen(str);
	const var_na *p;

	අතරතුර (listlen >= len) {
		නම් (memcmp(str, strlist, len+1) == 0)
			ආපසු 1;
		p = memchr(strlist, '\0', listlen);
		නම් (!p)
			ආපසු 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_stringlist_count(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	පූර්ණ_සංඛ්‍යා length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	නම් (!list)
		ආපසු length;

	end = list + length;

	අතරතුර (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		නම් (list + length > end)
			ආපසු -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ආපසු count;
}

පූර්ණ_සංඛ්‍යා fdt_stringlist_search(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *property,
			  const var_na *string)
{
	පූර්ණ_සංඛ්‍යා length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	නම් (!list)
		ආපසු length;

	len = strlen(string) + 1;
	end = list + length;

	අතරතුර (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		නම් (list + length > end)
			ආපසු -FDT_ERR_BADVALUE;

		නම් (length == len && memcmp(list, string, length) == 0)
			ආපසු idx;

		list += length;
		idx++;
	}

	ආපසු -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset,
			       const var_na *property, පූර්ණ_සංඛ්‍යා idx,
			       පූර්ණ_සංඛ්‍යා *lenp)
{
	const var_na *list, *end;
	පූර්ණ_සංඛ්‍යා length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	නම් (!list) {
		නම් (lenp)
			*lenp = length;

		ආපසු NULL;
	}

	end = list + length;

	අතරතුර (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		නම් (list + length > end) {
			නම් (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ආපසු NULL;
		}

		නම් (idx == 0) {
			නම් (lenp)
				*lenp = length - 1;

			ආපසු list;
		}

		list += length;
		idx--;
	}

	නම් (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා fdt_node_check_compatible(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	පූර්ණ_සංඛ්‍යා len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	නම් (!prop)
		ආපසු len;

	ආපසු !fdt_stringlist_contains(prop, len, compatible);
}

පූර්ණ_සංඛ්‍යා fdt_node_offset_by_compatible(const void *fdt, පූර්ණ_සංඛ්‍යා startoffset,
				  const var_na *compatible)
{
	පූර්ණ_සංඛ්‍යා offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	සඳහා (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		නම් ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ආපසු err;
		නැතහොත් නම් (err == 0)
			ආපසු offset;
	}

	ආපසු offset; /* error from fdt_next_node() */
}
