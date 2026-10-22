// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ចំនួនគត់ fdt_nodename_eq_(const void *fdt, ចំនួនគត់ offset,
			    const var_na *s, ចំនួនគត់ len)
{
	ចំនួនគត់ olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	ប្រសិនបើ (!p || olen < len)
		/* short match */
		ត្រឡប់ 0;

	ប្រសិនបើ (memcmp(p, s, len) != 0)
		ត្រឡប់ 0;

	ប្រសិនបើ (p[len] == '\0')
		ត្រឡប់ 1;
	បើមិនដូច្នេះ ប្រសិនបើ (!memchr(s, '@', len) && (p[len] == '@'))
		ត្រឡប់ 1;
	បើមិនដូច្នេះ
		ត្រឡប់ 0;
}

const var_na *fdt_get_string(const void *fdt, ចំនួនគត់ stroffset, ចំនួនគត់ *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ចំនួនគត់ err;
	const var_na *s, *n;

	ប្រសិនបើ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ប្រសិនបើ (lenp)
			*lenp = strlen(s);
		ត្រឡប់ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ប្រសិនបើ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ប្រសិនបើ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ប្រសិនបើ (fdt_magic(fdt) == FDT_MAGIC) {
		ប្រសិនបើ (stroffset < 0)
			goto fail;
		ប្រសិនបើ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ប្រសិនបើ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ប្រសិនបើ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} បើមិនដូច្នេះ ប្រសិនបើ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned ចំនួនគត់ sw_stroffset = -stroffset;

		ប្រសិនបើ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ប្រសិនបើ (sw_stroffset < len)
			len = sw_stroffset;
	} បើមិនដូច្នេះ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ប្រសិនបើ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ប្រសិនបើ (lenp)
		*lenp = n - s;
	ត្រឡប់ s;

fail:
	ប្រសិនបើ (lenp)
		*lenp = err;
	ត្រឡប់ NULL;
}

const var_na *fdt_string(const void *fdt, ចំនួនគត់ stroffset)
{
	ត្រឡប់ fdt_get_string(fdt, stroffset, NULL);
}

static ចំនួនគត់ fdt_string_eq_(const void *fdt, ចំនួនគត់ stroffset,
			  const var_na *s, ចំនួនគត់ len)
{
	ចំនួនគត់ slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ត្រឡប់ p && (slen == len) && (memcmp(p, s, len) == 0);
}

ចំនួនគត់ fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ចំនួនគត់ offset = -1;

	ខណៈពេល (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ប្រសិនបើ (offset < 0) {
			ប្រសិនបើ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ត្រឡប់ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ប្រសិនបើ (value > max)
			max = value;
	}

	ប្រសិនបើ (phandle)
		*phandle = max;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	ចំនួនគត់ err;

	err = fdt_find_max_phandle(fdt, &max);
	ប្រសិនបើ (err < 0)
		ត្រឡប់ err;

	ប្រសិនបើ (max == FDT_MAX_PHANDLE)
		ត្រឡប់ -FDT_ERR_NOPHANDLES;

	ប្រសិនបើ (phandle)
		*phandle = max + 1;

	ត្រឡប់ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, ចំនួនគត់ n)
{
	unsigned ចំនួនគត់ offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned ចំនួនគត់ absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ប្រសិនបើ (!can_assume(VALID_INPUT)) {
		ប្រសិនបើ (absoffset < fdt_off_mem_rsvmap(fdt))
			ត្រឡប់ NULL;
		ប្រសិនបើ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ត្រឡប់ NULL;
	}
	ត្រឡប់ fdt_mem_rsv_(fdt, n);
}

ចំនួនគត់ fdt_get_mem_rsv(const void *fdt, ចំនួនគត់ n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ប្រសិនបើ (!can_assume(VALID_INPUT) && !re)
		ត្រឡប់ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_num_mem_rsv(const void *fdt)
{
	ចំនួនគត់ i;
	const sa_nracanaa fdt_reserve_entry *re;

	សម្រាប់ (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ប្រសិនបើ (fdt64_ld_(&re->size) == 0)
			ត្រឡប់ i;
	}
	ត្រឡប់ -FDT_ERR_TRUNCATED;
}

static ចំនួនគត់ nextprop_(const void *fdt, ចំនួនគត់ offset)
{
	uint32_t tag;
	ចំនួនគត់ nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ប្រសិនបើ (nextoffset >= 0)
				ត្រឡប់ -FDT_ERR_BADSTRUCTURE;
			បើមិនដូច្នេះ
				ត្រឡប់ nextoffset;

		pak_sa FDT_PROP:
			ត្រឡប់ offset;
		}
		offset = nextoffset;
	} ខណៈពេល (tag == FDT_NOP);

	ត្រឡប់ -FDT_ERR_NOTFOUND;
}

ចំនួនគត់ fdt_subnode_offset_namelen(const void *fdt, ចំនួនគត់ offset,
			       const var_na *name, ចំនួនគត់ namelen)
{
	ចំនួនគត់ depth;

	FDT_RO_PROBE(fdt);

	សម្រាប់ (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ប្រសិនបើ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ត្រឡប់ offset;

	ប្រសិនបើ (depth < 0)
		ត្រឡប់ -FDT_ERR_NOTFOUND;
	ត្រឡប់ offset; /* error */
}

ចំនួនគត់ fdt_subnode_offset(const void *fdt, ចំនួនគត់ parentoffset,
		       const var_na *name)
{
	ត្រឡប់ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

ចំនួនគត់ fdt_path_offset_namelen(const void *fdt, const var_na *path, ចំនួនគត់ namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	ចំនួនគត់ offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ប្រសិនបើ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ប្រសិនបើ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ប្រសិនបើ (!p)
			ត្រឡប់ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ខណៈពេល (p < end) {
		const var_na *q;

		ខណៈពេល (*p == '/') {
			p++;
			ប្រសិនបើ (p == end)
				ត្រឡប់ offset;
		}
		q = memchr(p, '/', end - p);
		ប្រសិនបើ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ប្រសិនបើ (offset < 0)
			ត្រឡប់ offset;

		p = q;
	}

	ត្រឡប់ offset;
}

ចំនួនគត់ fdt_path_offset(const void *fdt, const var_na *path)
{
	ត្រឡប់ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, ចំនួនគត់ nodeoffset, ចំនួនគត់ *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	ចំនួនគត់ err;

	ប្រសិនបើ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ប្រសិនបើ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ប្រសិនបើ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ប្រសិនបើ (len)
		*len = strlen(nameptr);

	ត្រឡប់ nameptr;

 fail:
	ប្រសិនបើ (len)
		*len = err;
	ត្រឡប់ NULL;
}

ចំនួនគត់ fdt_first_property_offset(const void *fdt, ចំនួនគត់ nodeoffset)
{
	ចំនួនគត់ offset;

	ប្រសិនបើ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ត្រឡប់ offset;

	ត្រឡប់ nextprop_(fdt, offset);
}

ចំនួនគត់ fdt_next_property_offset(const void *fdt, ចំនួនគត់ offset)
{
	ប្រសិនបើ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ត្រឡប់ offset;

	ត្រឡប់ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              ចំនួនគត់ offset,
						              ចំនួនគត់ *lenp)
{
	ចំនួនគត់ err;
	const sa_nracanaa fdt_property *prop;

	ប្រសិនបើ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ប្រសិនបើ (lenp)
			*lenp = err;
		ត្រឡប់ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ប្រសិនបើ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ត្រឡប់ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      ចំនួនគត់ offset,
						      ចំនួនគត់ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ប្រសិនបើ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ប្រសិនបើ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ត្រឡប់ NULL;
	}

	ត្រឡប់ fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            ចំនួនគត់ offset,
						            const var_na *name,
						            ចំនួនគត់ namelen,
							    ចំនួនគត់ *lenp,
							    ចំនួនគត់ *poffset)
{
	សម្រាប់ (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ប្រសិនបើ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ប្រសិនបើ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ប្រសិនបើ (poffset)
				*poffset = offset;
			ត្រឡប់ prop;
		}
	}

	ប្រសិនបើ (lenp)
		*lenp = offset;
	ត្រឡប់ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    ចំនួនគត់ offset,
						    const var_na *name,
						    ចំនួនគត់ namelen, ចំនួនគត់ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ប្រសិនបើ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ប្រសិនបើ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ត្រឡប់ NULL;
	}

	ត្រឡប់ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    ចំនួនគត់ nodeoffset,
					    const var_na *name, ចំនួនគត់ *lenp)
{
	ត្រឡប់ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, ចំនួនគត់ nodeoffset,
				const var_na *name, ចំនួនគត់ namelen, ចំនួនគត់ *lenp)
{
	ចំនួនគត់ poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ប្រសិនបើ (!prop)
		ត្រឡប់ NULL;

	/* Handle realignment */
	ប្រសិនបើ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ត្រឡប់ prop->data + 4;
	ត្រឡប់ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, ចំនួនគត់ offset,
				  const var_na **namep, ចំនួនគត់ *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ប្រសិនបើ (!prop)
		ត្រឡប់ NULL;
	ប្រសិនបើ (namep) {
		const var_na *name;
		ចំនួនគត់ namelen;

		ប្រសិនបើ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ប្រសិនបើ (!name) {
				ប្រសិនបើ (lenp)
					*lenp = namelen;
				ត្រឡប់ NULL;
			}
			*namep = name;
		} បើមិនដូច្នេះ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ប្រសិនបើ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ត្រឡប់ prop->data + 4;
	ត្រឡប់ prop->data;
}

const void *fdt_getprop(const void *fdt, ចំនួនគត់ nodeoffset,
			const var_na *name, ចំនួនគត់ *lenp)
{
	ត្រឡប់ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, ចំនួនគត់ nodeoffset)
{
	const fdt32_t *php;
	ចំនួនគត់ len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ប្រសិនបើ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ប្រសិនបើ (!php || (len != parimaa_na(*php)))
			ត្រឡប់ 0;
	}

	ត្រឡប់ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, ចំនួនគត់ namelen)
{
	ចំនួនគត់ aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ប្រសិនបើ (aliasoffset < 0)
		ត្រឡប់ NULL;

	ត្រឡប់ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ត្រឡប់ fdt_get_alias_namelen(fdt, name, strlen(name));
}

ចំនួនគត់ fdt_get_path(const void *fdt, ចំនួនគត់ nodeoffset, var_na *buf, ចំនួនគត់ buflen)
{
	ចំនួនគត់ pdepth = 0, p = 0;
	ចំនួនគត់ offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ប្រសិនបើ (buflen < 2)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	សម្រាប់ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ខណៈពេល (pdepth > depth) {
			do {
				p--;
			} ខណៈពេល (buf[p-1] != '/');
			pdepth--;
		}

		ប្រសិនបើ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ប្រសិនបើ (!name)
				ត្រឡប់ namelen;
			ប្រសិនបើ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ប្រសិនបើ (offset == nodeoffset) {
			ប្រសិនបើ (pdepth < (depth + 1))
				ត្រឡប់ -FDT_ERR_NOSPACE;

			ប្រសិនបើ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ត្រឡប់ 0;
		}
	}

	ប្រសិនបើ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ត្រឡប់ -FDT_ERR_BADOFFSET;
	បើមិនដូច្នេះ ប្រសិនបើ (offset == -FDT_ERR_BADOFFSET)
		ត្រឡប់ -FDT_ERR_BADSTRUCTURE;

	ត្រឡប់ offset; /* error from fdt_next_node() */
}

ចំនួនគត់ fdt_supernode_atdepth_offset(const void *fdt, ចំនួនគត់ nodeoffset,
				 ចំនួនគត់ supernodedepth, ចំនួនគត់ *nodedepth)
{
	ចំនួនគត់ offset, depth;
	ចំនួនគត់ supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ប្រសិនបើ (supernodedepth < 0)
		ត្រឡប់ -FDT_ERR_NOTFOUND;

	សម្រាប់ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ប្រសិនបើ (depth == supernodedepth)
			supernodeoffset = offset;

		ប្រសិនបើ (offset == nodeoffset) {
			ប្រសិនបើ (nodedepth)
				*nodedepth = depth;

			ប្រសិនបើ (supernodedepth > depth)
				ត្រឡប់ -FDT_ERR_NOTFOUND;
			បើមិនដូច្នេះ
				ត្រឡប់ supernodeoffset;
		}
	}

	ប្រសិនបើ (!can_assume(VALID_INPUT)) {
		ប្រសិនបើ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ត្រឡប់ -FDT_ERR_BADOFFSET;
		បើមិនដូច្នេះ ប្រសិនបើ (offset == -FDT_ERR_BADOFFSET)
			ត្រឡប់ -FDT_ERR_BADSTRUCTURE;
	}

	ត្រឡប់ offset; /* error from fdt_next_node() */
}

ចំនួនគត់ fdt_node_depth(const void *fdt, ចំនួនគត់ nodeoffset)
{
	ចំនួនគត់ nodedepth;
	ចំនួនគត់ err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ប្រសិនបើ (err)
		ត្រឡប់ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ត្រឡប់ nodedepth;
}

ចំនួនគត់ fdt_parent_offset(const void *fdt, ចំនួនគត់ nodeoffset)
{
	ចំនួនគត់ nodedepth = fdt_node_depth(fdt, nodeoffset);

	ប្រសិនបើ (nodedepth < 0)
		ត្រឡប់ nodedepth;
	ត្រឡប់ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

ចំនួនគត់ fdt_node_offset_by_prop_value(const void *fdt, ចំនួនគត់ startoffset,
				  const var_na *propname,
				  const void *propval, ចំនួនគត់ proplen)
{
	ចំនួនគត់ offset;
	const void *val;
	ចំនួនគត់ len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	សម្រាប់ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ប្រសិនបើ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ត្រឡប់ offset;
	}

	ត្រឡប់ offset; /* error from fdt_next_node() */
}

ចំនួនគត់ fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	ចំនួនគត់ offset;

	ប្រសិនបើ ((phandle == 0) || (phandle == ~0U))
		ត្រឡប់ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	សម្រាប់ (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		ប្រសិនបើ (fdt_get_phandle(fdt, offset) == phandle)
			ត្រឡប់ offset;
	}

	ត្រឡប់ offset; /* error from fdt_next_node() */
}

ចំនួនគត់ fdt_stringlist_contains(const var_na *strlist, ចំនួនគត់ listlen, const var_na *str)
{
	ចំនួនគត់ len = strlen(str);
	const var_na *p;

	ខណៈពេល (listlen >= len) {
		ប្រសិនបើ (memcmp(str, strlist, len+1) == 0)
			ត្រឡប់ 1;
		p = memchr(strlist, '\0', listlen);
		ប្រសិនបើ (!p)
			ត្រឡប់ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_stringlist_count(const void *fdt, ចំនួនគត់ nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	ចំនួនគត់ length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ប្រសិនបើ (!list)
		ត្រឡប់ length;

	end = list + length;

	ខណៈពេល (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ប្រសិនបើ (list + length > end)
			ត្រឡប់ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ត្រឡប់ count;
}

ចំនួនគត់ fdt_stringlist_search(const void *fdt, ចំនួនគត់ nodeoffset, const var_na *property,
			  const var_na *string)
{
	ចំនួនគត់ length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ប្រសិនបើ (!list)
		ត្រឡប់ length;

	len = strlen(string) + 1;
	end = list + length;

	ខណៈពេល (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ប្រសិនបើ (list + length > end)
			ត្រឡប់ -FDT_ERR_BADVALUE;

		ប្រសិនបើ (length == len && memcmp(list, string, length) == 0)
			ត្រឡប់ idx;

		list += length;
		idx++;
	}

	ត្រឡប់ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, ចំនួនគត់ nodeoffset,
			       const var_na *property, ចំនួនគត់ idx,
			       ចំនួនគត់ *lenp)
{
	const var_na *list, *end;
	ចំនួនគត់ length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ប្រសិនបើ (!list) {
		ប្រសិនបើ (lenp)
			*lenp = length;

		ត្រឡប់ NULL;
	}

	end = list + length;

	ខណៈពេល (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ប្រសិនបើ (list + length > end) {
			ប្រសិនបើ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ត្រឡប់ NULL;
		}

		ប្រសិនបើ (idx == 0) {
			ប្រសិនបើ (lenp)
				*lenp = length - 1;

			ត្រឡប់ list;
		}

		list += length;
		idx--;
	}

	ប្រសិនបើ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ត្រឡប់ NULL;
}

ចំនួនគត់ fdt_node_check_compatible(const void *fdt, ចំនួនគត់ nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	ចំនួនគត់ len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ប្រសិនបើ (!prop)
		ត្រឡប់ len;

	ត្រឡប់ !fdt_stringlist_contains(prop, len, compatible);
}

ចំនួនគត់ fdt_node_offset_by_compatible(const void *fdt, ចំនួនគត់ startoffset,
				  const var_na *compatible)
{
	ចំនួនគត់ offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	សម្រាប់ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		ប្រសិនបើ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ត្រឡប់ err;
		បើមិនដូច្នេះ ប្រសិនបើ (err == 0)
			ត្រឡប់ offset;
	}

	ត្រឡប់ offset; /* error from fdt_next_node() */
}
