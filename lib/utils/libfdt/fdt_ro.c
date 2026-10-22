// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static puur_naa_nka fdt_nodename_eq_(const void *fdt, puur_naa_nka offset,
			    const var_na *s, puur_naa_nka len)
{
	puur_naa_nka olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	ပေႃး (!p || olen < len)
		/* short match */
		ပွၵ်ႈ 0;

	ပေႃး (memcmp(p, s, len) != 0)
		ပွၵ်ႈ 0;

	ပေႃး (p[len] == '\0')
		ပွၵ်ႈ 1;
	ပေႃးဢမ်ႇ ပေႃး (!memchr(s, '@', len) && (p[len] == '@'))
		ပွၵ်ႈ 1;
	ပေႃးဢမ်ႇ
		ပွၵ်ႈ 0;
}

const var_na *fdt_get_string(const void *fdt, puur_naa_nka stroffset, puur_naa_nka *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	puur_naa_nka err;
	const var_na *s, *n;

	ပေႃး (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ပေႃး (lenp)
			*lenp = strlen(s);
		ပွၵ်ႈ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ပေႃး (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ပေႃး (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ပေႃး (fdt_magic(fdt) == FDT_MAGIC) {
		ပေႃး (stroffset < 0)
			goto fail;
		ပေႃး (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ပေႃး ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ပေႃး ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ပေႃးဢမ်ႇ ပေႃး (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned puur_naa_nka sw_stroffset = -stroffset;

		ပေႃး ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ပေႃး (sw_stroffset < len)
			len = sw_stroffset;
	} ပေႃးဢမ်ႇ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ပေႃး (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ပေႃး (lenp)
		*lenp = n - s;
	ပွၵ်ႈ s;

fail:
	ပေႃး (lenp)
		*lenp = err;
	ပွၵ်ႈ NULL;
}

const var_na *fdt_string(const void *fdt, puur_naa_nka stroffset)
{
	ပွၵ်ႈ fdt_get_string(fdt, stroffset, NULL);
}

static puur_naa_nka fdt_string_eq_(const void *fdt, puur_naa_nka stroffset,
			  const var_na *s, puur_naa_nka len)
{
	puur_naa_nka slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ပွၵ်ႈ p && (slen == len) && (memcmp(p, s, len) == 0);
}

puur_naa_nka fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	puur_naa_nka offset = -1;

	ၶၢဝ်း (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ပေႃး (offset < 0) {
			ပေႃး (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ပွၵ်ႈ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ပေႃး (value > max)
			max = value;
	}

	ပေႃး (phandle)
		*phandle = max;

	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	puur_naa_nka err;

	err = fdt_find_max_phandle(fdt, &max);
	ပေႃး (err < 0)
		ပွၵ်ႈ err;

	ပေႃး (max == FDT_MAX_PHANDLE)
		ပွၵ်ႈ -FDT_ERR_NOPHANDLES;

	ပေႃး (phandle)
		*phandle = max + 1;

	ပွၵ်ႈ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, puur_naa_nka n)
{
	unsigned puur_naa_nka offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned puur_naa_nka absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ပေႃး (!can_assume(VALID_INPUT)) {
		ပေႃး (absoffset < fdt_off_mem_rsvmap(fdt))
			ပွၵ်ႈ NULL;
		ပေႃး (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ပွၵ်ႈ NULL;
	}
	ပွၵ်ႈ fdt_mem_rsv_(fdt, n);
}

puur_naa_nka fdt_get_mem_rsv(const void *fdt, puur_naa_nka n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ပေႃး (!can_assume(VALID_INPUT) && !re)
		ပွၵ်ႈ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_num_mem_rsv(const void *fdt)
{
	puur_naa_nka i;
	const sa_nracanaa fdt_reserve_entry *re;

	for (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ပေႃး (fdt64_ld_(&re->size) == 0)
			ပွၵ်ႈ i;
	}
	ပွၵ်ႈ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka nextprop_(const void *fdt, puur_naa_nka offset)
{
	uint32_t tag;
	puur_naa_nka nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ပေႃး (nextoffset >= 0)
				ပွၵ်ႈ -FDT_ERR_BADSTRUCTURE;
			ပေႃးဢမ်ႇ
				ပွၵ်ႈ nextoffset;

		pak_sa FDT_PROP:
			ပွၵ်ႈ offset;
		}
		offset = nextoffset;
	} ၶၢဝ်း (tag == FDT_NOP);

	ပွၵ်ႈ -FDT_ERR_NOTFOUND;
}

puur_naa_nka fdt_subnode_offset_namelen(const void *fdt, puur_naa_nka offset,
			       const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka depth;

	FDT_RO_PROBE(fdt);

	for (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ပေႃး ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ပွၵ်ႈ offset;

	ပေႃး (depth < 0)
		ပွၵ်ႈ -FDT_ERR_NOTFOUND;
	ပွၵ်ႈ offset; /* error */
}

puur_naa_nka fdt_subnode_offset(const void *fdt, puur_naa_nka parentoffset,
		       const var_na *name)
{
	ပွၵ်ႈ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_path_offset_namelen(const void *fdt, const var_na *path, puur_naa_nka namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	puur_naa_nka offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ပေႃး (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ပေႃး (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ပေႃး (!p)
			ပွၵ်ႈ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ၶၢဝ်း (p < end) {
		const var_na *q;

		ၶၢဝ်း (*p == '/') {
			p++;
			ပေႃး (p == end)
				ပွၵ်ႈ offset;
		}
		q = memchr(p, '/', end - p);
		ပေႃး (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ပေႃး (offset < 0)
			ပွၵ်ႈ offset;

		p = q;
	}

	ပွၵ်ႈ offset;
}

puur_naa_nka fdt_path_offset(const void *fdt, const var_na *path)
{
	ပွၵ်ႈ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, puur_naa_nka nodeoffset, puur_naa_nka *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	puur_naa_nka err;

	ပေႃး (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ပေႃး (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ပေႃး (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ပေႃး (len)
		*len = strlen(nameptr);

	ပွၵ်ႈ nameptr;

 fail:
	ပေႃး (len)
		*len = err;
	ပွၵ်ႈ NULL;
}

puur_naa_nka fdt_first_property_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka offset;

	ပေႃး ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ပွၵ်ႈ offset;

	ပွၵ်ႈ nextprop_(fdt, offset);
}

puur_naa_nka fdt_next_property_offset(const void *fdt, puur_naa_nka offset)
{
	ပေႃး ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ပွၵ်ႈ offset;

	ပွၵ်ႈ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              puur_naa_nka offset,
						              puur_naa_nka *lenp)
{
	puur_naa_nka err;
	const sa_nracanaa fdt_property *prop;

	ပေႃး (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ပေႃး (lenp)
			*lenp = err;
		ပွၵ်ႈ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ပေႃး (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ပွၵ်ႈ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      puur_naa_nka offset,
						      puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ပေႃး (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ပေႃး (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ပွၵ်ႈ NULL;
	}

	ပွၵ်ႈ fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            puur_naa_nka offset,
						            const var_na *name,
						            puur_naa_nka namelen,
							    puur_naa_nka *lenp,
							    puur_naa_nka *poffset)
{
	for (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ပေႃး (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ပေႃး (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ပေႃး (poffset)
				*poffset = offset;
			ပွၵ်ႈ prop;
		}
	}

	ပေႃး (lenp)
		*lenp = offset;
	ပွၵ်ႈ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    puur_naa_nka offset,
						    const var_na *name,
						    puur_naa_nka namelen, puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ပေႃး (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ပေႃး (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ပွၵ်ႈ NULL;
	}

	ပွၵ်ႈ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    puur_naa_nka nodeoffset,
					    const var_na *name, puur_naa_nka *lenp)
{
	ပွၵ်ႈ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, puur_naa_nka nodeoffset,
				const var_na *name, puur_naa_nka namelen, puur_naa_nka *lenp)
{
	puur_naa_nka poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ပေႃး (!prop)
		ပွၵ်ႈ NULL;

	/* Handle realignment */
	ပေႃး (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ပွၵ်ႈ prop->data + 4;
	ပွၵ်ႈ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, puur_naa_nka offset,
				  const var_na **namep, puur_naa_nka *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ပေႃး (!prop)
		ပွၵ်ႈ NULL;
	ပေႃး (namep) {
		const var_na *name;
		puur_naa_nka namelen;

		ပေႃး (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ပေႃး (!name) {
				ပေႃး (lenp)
					*lenp = namelen;
				ပွၵ်ႈ NULL;
			}
			*namep = name;
		} ပေႃးဢမ်ႇ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ပေႃး (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ပွၵ်ႈ prop->data + 4;
	ပွၵ်ႈ prop->data;
}

const void *fdt_getprop(const void *fdt, puur_naa_nka nodeoffset,
			const var_na *name, puur_naa_nka *lenp)
{
	ပွၵ်ႈ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, puur_naa_nka nodeoffset)
{
	const fdt32_t *php;
	puur_naa_nka len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ပေႃး (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ပေႃး (!php || (len != parimaa_na(*php)))
			ပွၵ်ႈ 0;
	}

	ပွၵ်ႈ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ပေႃး (aliasoffset < 0)
		ပွၵ်ႈ NULL;

	ပွၵ်ႈ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ပွၵ်ႈ fdt_get_alias_namelen(fdt, name, strlen(name));
}

puur_naa_nka fdt_get_path(const void *fdt, puur_naa_nka nodeoffset, var_na *buf, puur_naa_nka buflen)
{
	puur_naa_nka pdepth = 0, p = 0;
	puur_naa_nka offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ပေႃး (buflen < 2)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ၶၢဝ်း (pdepth > depth) {
			do {
				p--;
			} ၶၢဝ်း (buf[p-1] != '/');
			pdepth--;
		}

		ပေႃး (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ပေႃး (!name)
				ပွၵ်ႈ namelen;
			ပေႃး ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ပေႃး (offset == nodeoffset) {
			ပေႃး (pdepth < (depth + 1))
				ပွၵ်ႈ -FDT_ERR_NOSPACE;

			ပေႃး (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ပွၵ်ႈ 0;
		}
	}

	ပေႃး ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ပွၵ်ႈ -FDT_ERR_BADOFFSET;
	ပေႃးဢမ်ႇ ပေႃး (offset == -FDT_ERR_BADOFFSET)
		ပွၵ်ႈ -FDT_ERR_BADSTRUCTURE;

	ပွၵ်ႈ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_supernode_atdepth_offset(const void *fdt, puur_naa_nka nodeoffset,
				 puur_naa_nka supernodedepth, puur_naa_nka *nodedepth)
{
	puur_naa_nka offset, depth;
	puur_naa_nka supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ပေႃး (supernodedepth < 0)
		ပွၵ်ႈ -FDT_ERR_NOTFOUND;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ပေႃး (depth == supernodedepth)
			supernodeoffset = offset;

		ပေႃး (offset == nodeoffset) {
			ပေႃး (nodedepth)
				*nodedepth = depth;

			ပေႃး (supernodedepth > depth)
				ပွၵ်ႈ -FDT_ERR_NOTFOUND;
			ပေႃးဢမ်ႇ
				ပွၵ်ႈ supernodeoffset;
		}
	}

	ပေႃး (!can_assume(VALID_INPUT)) {
		ပေႃး ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ပွၵ်ႈ -FDT_ERR_BADOFFSET;
		ပေႃးဢမ်ႇ ပေႃး (offset == -FDT_ERR_BADOFFSET)
			ပွၵ်ႈ -FDT_ERR_BADSTRUCTURE;
	}

	ပွၵ်ႈ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_depth(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth;
	puur_naa_nka err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ပေႃး (err)
		ပွၵ်ႈ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ပွၵ်ႈ nodedepth;
}

puur_naa_nka fdt_parent_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth = fdt_node_depth(fdt, nodeoffset);

	ပေႃး (nodedepth < 0)
		ပွၵ်ႈ nodedepth;
	ပွၵ်ႈ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

puur_naa_nka fdt_node_offset_by_prop_value(const void *fdt, puur_naa_nka startoffset,
				  const var_na *propname,
				  const void *propval, puur_naa_nka proplen)
{
	puur_naa_nka offset;
	const void *val;
	puur_naa_nka len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	for (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ပေႃး (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ပွၵ်ႈ offset;
	}

	ပွၵ်ႈ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	puur_naa_nka offset;

	ပေႃး ((phandle == 0) || (phandle == ~0U))
		ပွၵ်ႈ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	for (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		ပေႃး (fdt_get_phandle(fdt, offset) == phandle)
			ပွၵ်ႈ offset;
	}

	ပွၵ်ႈ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_stringlist_contains(const var_na *strlist, puur_naa_nka listlen, const var_na *str)
{
	puur_naa_nka len = strlen(str);
	const var_na *p;

	ၶၢဝ်း (listlen >= len) {
		ပေႃး (memcmp(str, strlist, len+1) == 0)
			ပွၵ်ႈ 1;
		p = memchr(strlist, '\0', listlen);
		ပေႃး (!p)
			ပွၵ်ႈ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_stringlist_count(const void *fdt, puur_naa_nka nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	puur_naa_nka length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ပေႃး (!list)
		ပွၵ်ႈ length;

	end = list + length;

	ၶၢဝ်း (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ပေႃး (list + length > end)
			ပွၵ်ႈ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ပွၵ်ႈ count;
}

puur_naa_nka fdt_stringlist_search(const void *fdt, puur_naa_nka nodeoffset, const var_na *property,
			  const var_na *string)
{
	puur_naa_nka length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ပေႃး (!list)
		ပွၵ်ႈ length;

	len = strlen(string) + 1;
	end = list + length;

	ၶၢဝ်း (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ပေႃး (list + length > end)
			ပွၵ်ႈ -FDT_ERR_BADVALUE;

		ပေႃး (length == len && memcmp(list, string, length) == 0)
			ပွၵ်ႈ idx;

		list += length;
		idx++;
	}

	ပွၵ်ႈ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, puur_naa_nka nodeoffset,
			       const var_na *property, puur_naa_nka idx,
			       puur_naa_nka *lenp)
{
	const var_na *list, *end;
	puur_naa_nka length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ပေႃး (!list) {
		ပေႃး (lenp)
			*lenp = length;

		ပွၵ်ႈ NULL;
	}

	end = list + length;

	ၶၢဝ်း (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ပေႃး (list + length > end) {
			ပေႃး (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ပွၵ်ႈ NULL;
		}

		ပေႃး (idx == 0) {
			ပေႃး (lenp)
				*lenp = length - 1;

			ပွၵ်ႈ list;
		}

		list += length;
		idx--;
	}

	ပေႃး (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ပွၵ်ႈ NULL;
}

puur_naa_nka fdt_node_check_compatible(const void *fdt, puur_naa_nka nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	puur_naa_nka len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ပေႃး (!prop)
		ပွၵ်ႈ len;

	ပွၵ်ႈ !fdt_stringlist_contains(prop, len, compatible);
}

puur_naa_nka fdt_node_offset_by_compatible(const void *fdt, puur_naa_nka startoffset,
				  const var_na *compatible)
{
	puur_naa_nka offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	for (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		ပေႃး ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ပွၵ်ႈ err;
		ပေႃးဢမ်ႇ ပေႃး (err == 0)
			ပွၵ်ႈ offset;
	}

	ပွၵ်ႈ offset; /* error from fdt_next_node() */
}
