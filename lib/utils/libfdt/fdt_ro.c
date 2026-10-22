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

	ꦪꦼꦤ꧀ (!p || olen < len)
		/* short match */
		ꦮꦭꦶ 0;

	ꦪꦼꦤ꧀ (memcmp(p, s, len) != 0)
		ꦮꦭꦶ 0;

	ꦪꦼꦤ꧀ (p[len] == '\0')
		ꦮꦭꦶ 1;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (!memchr(s, '@', len) && (p[len] == '@'))
		ꦮꦭꦶ 1;
	ꦭꦶꦪ
		ꦮꦭꦶ 0;
}

const var_na *fdt_get_string(const void *fdt, puur_naa_nka stroffset, puur_naa_nka *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	puur_naa_nka err;
	const var_na *s, *n;

	ꦪꦼꦤ꧀ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ꦪꦼꦤ꧀ (lenp)
			*lenp = strlen(s);
		ꦮꦭꦶ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ꦪꦼꦤ꧀ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ꦪꦼꦤ꧀ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ꦪꦼꦤ꧀ (fdt_magic(fdt) == FDT_MAGIC) {
		ꦪꦼꦤ꧀ (stroffset < 0)
			goto fail;
		ꦪꦼꦤ꧀ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ꦪꦼꦤ꧀ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ꦪꦼꦤ꧀ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ꦭꦶꦪ ꦪꦼꦤ꧀ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned puur_naa_nka sw_stroffset = -stroffset;

		ꦪꦼꦤ꧀ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ꦪꦼꦤ꧀ (sw_stroffset < len)
			len = sw_stroffset;
	} ꦭꦶꦪ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ꦪꦼꦤ꧀ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ꦪꦼꦤ꧀ (lenp)
		*lenp = n - s;
	ꦮꦭꦶ s;

fail:
	ꦪꦼꦤ꧀ (lenp)
		*lenp = err;
	ꦮꦭꦶ NULL;
}

const var_na *fdt_string(const void *fdt, puur_naa_nka stroffset)
{
	ꦮꦭꦶ fdt_get_string(fdt, stroffset, NULL);
}

static puur_naa_nka fdt_string_eq_(const void *fdt, puur_naa_nka stroffset,
			  const var_na *s, puur_naa_nka len)
{
	puur_naa_nka slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ꦮꦭꦶ p && (slen == len) && (memcmp(p, s, len) == 0);
}

puur_naa_nka fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	puur_naa_nka offset = -1;

	ꦱꦼꦢꦼꦭ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ꦪꦼꦤ꧀ (offset < 0) {
			ꦪꦼꦤ꧀ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ꦮꦭꦶ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ꦪꦼꦤ꧀ (value > max)
			max = value;
	}

	ꦪꦼꦤ꧀ (phandle)
		*phandle = max;

	ꦮꦭꦶ 0;
}

puur_naa_nka fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	puur_naa_nka err;

	err = fdt_find_max_phandle(fdt, &max);
	ꦪꦼꦤ꧀ (err < 0)
		ꦮꦭꦶ err;

	ꦪꦼꦤ꧀ (max == FDT_MAX_PHANDLE)
		ꦮꦭꦶ -FDT_ERR_NOPHANDLES;

	ꦪꦼꦤ꧀ (phandle)
		*phandle = max + 1;

	ꦮꦭꦶ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, puur_naa_nka n)
{
	unsigned puur_naa_nka offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned puur_naa_nka absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT)) {
		ꦪꦼꦤ꧀ (absoffset < fdt_off_mem_rsvmap(fdt))
			ꦮꦭꦶ NULL;
		ꦪꦼꦤ꧀ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ꦮꦭꦶ NULL;
	}
	ꦮꦭꦶ fdt_mem_rsv_(fdt, n);
}

puur_naa_nka fdt_get_mem_rsv(const void *fdt, puur_naa_nka n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT) && !re)
		ꦮꦭꦶ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ꦮꦭꦶ 0;
}

puur_naa_nka fdt_num_mem_rsv(const void *fdt)
{
	puur_naa_nka i;
	const sa_nracanaa fdt_reserve_entry *re;

	ꦏꦁꦒꦺꦴ (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ꦪꦼꦤ꧀ (fdt64_ld_(&re->size) == 0)
			ꦮꦭꦶ i;
	}
	ꦮꦭꦶ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka nextprop_(const void *fdt, puur_naa_nka offset)
{
	uint32_t tag;
	puur_naa_nka nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ꦪꦼꦤ꧀ (nextoffset >= 0)
				ꦮꦭꦶ -FDT_ERR_BADSTRUCTURE;
			ꦭꦶꦪ
				ꦮꦭꦶ nextoffset;

		pak_sa FDT_PROP:
			ꦮꦭꦶ offset;
		}
		offset = nextoffset;
	} ꦱꦼꦢꦼꦭ (tag == FDT_NOP);

	ꦮꦭꦶ -FDT_ERR_NOTFOUND;
}

puur_naa_nka fdt_subnode_offset_namelen(const void *fdt, puur_naa_nka offset,
			       const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka depth;

	FDT_RO_PROBE(fdt);

	ꦏꦁꦒꦺꦴ (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ꦪꦼꦤ꧀ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ꦮꦭꦶ offset;

	ꦪꦼꦤ꧀ (depth < 0)
		ꦮꦭꦶ -FDT_ERR_NOTFOUND;
	ꦮꦭꦶ offset; /* error */
}

puur_naa_nka fdt_subnode_offset(const void *fdt, puur_naa_nka parentoffset,
		       const var_na *name)
{
	ꦮꦭꦶ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_path_offset_namelen(const void *fdt, const var_na *path, puur_naa_nka namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	puur_naa_nka offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ꦪꦼꦤ꧀ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ꦪꦼꦤ꧀ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ꦪꦼꦤ꧀ (!p)
			ꦮꦭꦶ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ꦱꦼꦢꦼꦭ (p < end) {
		const var_na *q;

		ꦱꦼꦢꦼꦭ (*p == '/') {
			p++;
			ꦪꦼꦤ꧀ (p == end)
				ꦮꦭꦶ offset;
		}
		q = memchr(p, '/', end - p);
		ꦪꦼꦤ꧀ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ꦪꦼꦤ꧀ (offset < 0)
			ꦮꦭꦶ offset;

		p = q;
	}

	ꦮꦭꦶ offset;
}

puur_naa_nka fdt_path_offset(const void *fdt, const var_na *path)
{
	ꦮꦭꦶ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, puur_naa_nka nodeoffset, puur_naa_nka *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	puur_naa_nka err;

	ꦪꦼꦤ꧀ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ꦪꦼꦤ꧀ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ꦪꦼꦤ꧀ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ꦪꦼꦤ꧀ (len)
		*len = strlen(nameptr);

	ꦮꦭꦶ nameptr;

 fail:
	ꦪꦼꦤ꧀ (len)
		*len = err;
	ꦮꦭꦶ NULL;
}

puur_naa_nka fdt_first_property_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka offset;

	ꦪꦼꦤ꧀ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ꦮꦭꦶ offset;

	ꦮꦭꦶ nextprop_(fdt, offset);
}

puur_naa_nka fdt_next_property_offset(const void *fdt, puur_naa_nka offset)
{
	ꦪꦼꦤ꧀ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ꦮꦭꦶ offset;

	ꦮꦭꦶ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              puur_naa_nka offset,
						              puur_naa_nka *lenp)
{
	puur_naa_nka err;
	const sa_nracanaa fdt_property *prop;

	ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ꦪꦼꦤ꧀ (lenp)
			*lenp = err;
		ꦮꦭꦶ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ꦪꦼꦤ꧀ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ꦮꦭꦶ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      puur_naa_nka offset,
						      puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ꦪꦼꦤ꧀ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ꦪꦼꦤ꧀ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ꦮꦭꦶ NULL;
	}

	ꦮꦭꦶ fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            puur_naa_nka offset,
						            const var_na *name,
						            puur_naa_nka namelen,
							    puur_naa_nka *lenp,
							    puur_naa_nka *poffset)
{
	ꦏꦁꦒꦺꦴ (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ꦪꦼꦤ꧀ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ꦪꦼꦤ꧀ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ꦪꦼꦤ꧀ (poffset)
				*poffset = offset;
			ꦮꦭꦶ prop;
		}
	}

	ꦪꦼꦤ꧀ (lenp)
		*lenp = offset;
	ꦮꦭꦶ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    puur_naa_nka offset,
						    const var_na *name,
						    puur_naa_nka namelen, puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ꦪꦼꦤ꧀ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ꦪꦼꦤ꧀ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ꦮꦭꦶ NULL;
	}

	ꦮꦭꦶ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    puur_naa_nka nodeoffset,
					    const var_na *name, puur_naa_nka *lenp)
{
	ꦮꦭꦶ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, puur_naa_nka nodeoffset,
				const var_na *name, puur_naa_nka namelen, puur_naa_nka *lenp)
{
	puur_naa_nka poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ꦪꦼꦤ꧀ (!prop)
		ꦮꦭꦶ NULL;

	/* Handle realignment */
	ꦪꦼꦤ꧀ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ꦮꦭꦶ prop->data + 4;
	ꦮꦭꦶ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, puur_naa_nka offset,
				  const var_na **namep, puur_naa_nka *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ꦪꦼꦤ꧀ (!prop)
		ꦮꦭꦶ NULL;
	ꦪꦼꦤ꧀ (namep) {
		const var_na *name;
		puur_naa_nka namelen;

		ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ꦪꦼꦤ꧀ (!name) {
				ꦪꦼꦤ꧀ (lenp)
					*lenp = namelen;
				ꦮꦭꦶ NULL;
			}
			*namep = name;
		} ꦭꦶꦪ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ꦪꦼꦤ꧀ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ꦮꦭꦶ prop->data + 4;
	ꦮꦭꦶ prop->data;
}

const void *fdt_getprop(const void *fdt, puur_naa_nka nodeoffset,
			const var_na *name, puur_naa_nka *lenp)
{
	ꦮꦭꦶ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, puur_naa_nka nodeoffset)
{
	const fdt32_t *php;
	puur_naa_nka len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ꦪꦼꦤ꧀ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ꦪꦼꦤ꧀ (!php || (len != parimaa_na(*php)))
			ꦮꦭꦶ 0;
	}

	ꦮꦭꦶ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ꦪꦼꦤ꧀ (aliasoffset < 0)
		ꦮꦭꦶ NULL;

	ꦮꦭꦶ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ꦮꦭꦶ fdt_get_alias_namelen(fdt, name, strlen(name));
}

puur_naa_nka fdt_get_path(const void *fdt, puur_naa_nka nodeoffset, var_na *buf, puur_naa_nka buflen)
{
	puur_naa_nka pdepth = 0, p = 0;
	puur_naa_nka offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ꦪꦼꦤ꧀ (buflen < 2)
		ꦮꦭꦶ -FDT_ERR_NOSPACE;

	ꦏꦁꦒꦺꦴ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ꦱꦼꦢꦼꦭ (pdepth > depth) {
			do {
				p--;
			} ꦱꦼꦢꦼꦭ (buf[p-1] != '/');
			pdepth--;
		}

		ꦪꦼꦤ꧀ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ꦪꦼꦤ꧀ (!name)
				ꦮꦭꦶ namelen;
			ꦪꦼꦤ꧀ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ꦪꦼꦤ꧀ (offset == nodeoffset) {
			ꦪꦼꦤ꧀ (pdepth < (depth + 1))
				ꦮꦭꦶ -FDT_ERR_NOSPACE;

			ꦪꦼꦤ꧀ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ꦮꦭꦶ 0;
		}
	}

	ꦪꦼꦤ꧀ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ꦮꦭꦶ -FDT_ERR_BADOFFSET;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (offset == -FDT_ERR_BADOFFSET)
		ꦮꦭꦶ -FDT_ERR_BADSTRUCTURE;

	ꦮꦭꦶ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_supernode_atdepth_offset(const void *fdt, puur_naa_nka nodeoffset,
				 puur_naa_nka supernodedepth, puur_naa_nka *nodedepth)
{
	puur_naa_nka offset, depth;
	puur_naa_nka supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ꦪꦼꦤ꧀ (supernodedepth < 0)
		ꦮꦭꦶ -FDT_ERR_NOTFOUND;

	ꦏꦁꦒꦺꦴ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ꦪꦼꦤ꧀ (depth == supernodedepth)
			supernodeoffset = offset;

		ꦪꦼꦤ꧀ (offset == nodeoffset) {
			ꦪꦼꦤ꧀ (nodedepth)
				*nodedepth = depth;

			ꦪꦼꦤ꧀ (supernodedepth > depth)
				ꦮꦭꦶ -FDT_ERR_NOTFOUND;
			ꦭꦶꦪ
				ꦮꦭꦶ supernodeoffset;
		}
	}

	ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT)) {
		ꦪꦼꦤ꧀ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ꦮꦭꦶ -FDT_ERR_BADOFFSET;
		ꦭꦶꦪ ꦪꦼꦤ꧀ (offset == -FDT_ERR_BADOFFSET)
			ꦮꦭꦶ -FDT_ERR_BADSTRUCTURE;
	}

	ꦮꦭꦶ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_depth(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth;
	puur_naa_nka err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ꦪꦼꦤ꧀ (err)
		ꦮꦭꦶ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ꦮꦭꦶ nodedepth;
}

puur_naa_nka fdt_parent_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth = fdt_node_depth(fdt, nodeoffset);

	ꦪꦼꦤ꧀ (nodedepth < 0)
		ꦮꦭꦶ nodedepth;
	ꦮꦭꦶ fdt_supernode_atdepth_offset(fdt, nodeoffset,
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
	ꦏꦁꦒꦺꦴ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ꦪꦼꦤ꧀ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ꦮꦭꦶ offset;
	}

	ꦮꦭꦶ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	puur_naa_nka offset;

	ꦪꦼꦤ꧀ ((phandle == 0) || (phandle == ~0U))
		ꦮꦭꦶ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	ꦏꦁꦒꦺꦴ (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		ꦪꦼꦤ꧀ (fdt_get_phandle(fdt, offset) == phandle)
			ꦮꦭꦶ offset;
	}

	ꦮꦭꦶ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_stringlist_contains(const var_na *strlist, puur_naa_nka listlen, const var_na *str)
{
	puur_naa_nka len = strlen(str);
	const var_na *p;

	ꦱꦼꦢꦼꦭ (listlen >= len) {
		ꦪꦼꦤ꧀ (memcmp(str, strlist, len+1) == 0)
			ꦮꦭꦶ 1;
		p = memchr(strlist, '\0', listlen);
		ꦪꦼꦤ꧀ (!p)
			ꦮꦭꦶ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ꦮꦭꦶ 0;
}

puur_naa_nka fdt_stringlist_count(const void *fdt, puur_naa_nka nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	puur_naa_nka length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ꦪꦼꦤ꧀ (!list)
		ꦮꦭꦶ length;

	end = list + length;

	ꦱꦼꦢꦼꦭ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ꦪꦼꦤ꧀ (list + length > end)
			ꦮꦭꦶ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ꦮꦭꦶ count;
}

puur_naa_nka fdt_stringlist_search(const void *fdt, puur_naa_nka nodeoffset, const var_na *property,
			  const var_na *string)
{
	puur_naa_nka length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ꦪꦼꦤ꧀ (!list)
		ꦮꦭꦶ length;

	len = strlen(string) + 1;
	end = list + length;

	ꦱꦼꦢꦼꦭ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ꦪꦼꦤ꧀ (list + length > end)
			ꦮꦭꦶ -FDT_ERR_BADVALUE;

		ꦪꦼꦤ꧀ (length == len && memcmp(list, string, length) == 0)
			ꦮꦭꦶ idx;

		list += length;
		idx++;
	}

	ꦮꦭꦶ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, puur_naa_nka nodeoffset,
			       const var_na *property, puur_naa_nka idx,
			       puur_naa_nka *lenp)
{
	const var_na *list, *end;
	puur_naa_nka length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ꦪꦼꦤ꧀ (!list) {
		ꦪꦼꦤ꧀ (lenp)
			*lenp = length;

		ꦮꦭꦶ NULL;
	}

	end = list + length;

	ꦱꦼꦢꦼꦭ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ꦪꦼꦤ꧀ (list + length > end) {
			ꦪꦼꦤ꧀ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ꦮꦭꦶ NULL;
		}

		ꦪꦼꦤ꧀ (idx == 0) {
			ꦪꦼꦤ꧀ (lenp)
				*lenp = length - 1;

			ꦮꦭꦶ list;
		}

		list += length;
		idx--;
	}

	ꦪꦼꦤ꧀ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ꦮꦭꦶ NULL;
}

puur_naa_nka fdt_node_check_compatible(const void *fdt, puur_naa_nka nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	puur_naa_nka len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ꦪꦼꦤ꧀ (!prop)
		ꦮꦭꦶ len;

	ꦮꦭꦶ !fdt_stringlist_contains(prop, len, compatible);
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
	ꦏꦁꦒꦺꦴ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		ꦪꦼꦤ꧀ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ꦮꦭꦶ err;
		ꦭꦶꦪ ꦪꦼꦤ꧀ (err == 0)
			ꦮꦭꦶ offset;
	}

	ꦮꦭꦶ offset; /* error from fdt_next_node() */
}
