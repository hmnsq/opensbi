// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static จำนวนเต็ม fdt_nodename_eq_(const void *fdt, จำนวนเต็ม offset,
			    const var_na *s, จำนวนเต็ม len)
{
	จำนวนเต็ม olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	ถ้า (!p || olen < len)
		/* short match */
		คืน 0;

	ถ้า (memcmp(p, s, len) != 0)
		คืน 0;

	ถ้า (p[len] == '\0')
		คืน 1;
	มิฉะนั้น ถ้า (!memchr(s, '@', len) && (p[len] == '@'))
		คืน 1;
	มิฉะนั้น
		คืน 0;
}

const var_na *fdt_get_string(const void *fdt, จำนวนเต็ม stroffset, จำนวนเต็ม *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	จำนวนเต็ม err;
	const var_na *s, *n;

	ถ้า (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ถ้า (lenp)
			*lenp = strlen(s);
		คืน s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ถ้า (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ถ้า (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ถ้า (fdt_magic(fdt) == FDT_MAGIC) {
		ถ้า (stroffset < 0)
			goto fail;
		ถ้า (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ถ้า ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ถ้า ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} มิฉะนั้น ถ้า (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned จำนวนเต็ม sw_stroffset = -stroffset;

		ถ้า ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ถ้า (sw_stroffset < len)
			len = sw_stroffset;
	} มิฉะนั้น {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ถ้า (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ถ้า (lenp)
		*lenp = n - s;
	คืน s;

fail:
	ถ้า (lenp)
		*lenp = err;
	คืน NULL;
}

const var_na *fdt_string(const void *fdt, จำนวนเต็ม stroffset)
{
	คืน fdt_get_string(fdt, stroffset, NULL);
}

static จำนวนเต็ม fdt_string_eq_(const void *fdt, จำนวนเต็ม stroffset,
			  const var_na *s, จำนวนเต็ม len)
{
	จำนวนเต็ม slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	คืน p && (slen == len) && (memcmp(p, s, len) == 0);
}

จำนวนเต็ม fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	จำนวนเต็ม offset = -1;

	ขณะ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ถ้า (offset < 0) {
			ถ้า (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			คืน offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ถ้า (value > max)
			max = value;
	}

	ถ้า (phandle)
		*phandle = max;

	คืน 0;
}

จำนวนเต็ม fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	จำนวนเต็ม err;

	err = fdt_find_max_phandle(fdt, &max);
	ถ้า (err < 0)
		คืน err;

	ถ้า (max == FDT_MAX_PHANDLE)
		คืน -FDT_ERR_NOPHANDLES;

	ถ้า (phandle)
		*phandle = max + 1;

	คืน 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, จำนวนเต็ม n)
{
	unsigned จำนวนเต็ม offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned จำนวนเต็ม absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ถ้า (!can_assume(VALID_INPUT)) {
		ถ้า (absoffset < fdt_off_mem_rsvmap(fdt))
			คืน NULL;
		ถ้า (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			คืน NULL;
	}
	คืน fdt_mem_rsv_(fdt, n);
}

จำนวนเต็ม fdt_get_mem_rsv(const void *fdt, จำนวนเต็ม n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ถ้า (!can_assume(VALID_INPUT) && !re)
		คืน -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	คืน 0;
}

จำนวนเต็ม fdt_num_mem_rsv(const void *fdt)
{
	จำนวนเต็ม i;
	const sa_nracanaa fdt_reserve_entry *re;

	สำหรับ (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ถ้า (fdt64_ld_(&re->size) == 0)
			คืน i;
	}
	คืน -FDT_ERR_TRUNCATED;
}

static จำนวนเต็ม nextprop_(const void *fdt, จำนวนเต็ม offset)
{
	uint32_t tag;
	จำนวนเต็ม nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ถ้า (nextoffset >= 0)
				คืน -FDT_ERR_BADSTRUCTURE;
			มิฉะนั้น
				คืน nextoffset;

		pak_sa FDT_PROP:
			คืน offset;
		}
		offset = nextoffset;
	} ขณะ (tag == FDT_NOP);

	คืน -FDT_ERR_NOTFOUND;
}

จำนวนเต็ม fdt_subnode_offset_namelen(const void *fdt, จำนวนเต็ม offset,
			       const var_na *name, จำนวนเต็ม namelen)
{
	จำนวนเต็ม depth;

	FDT_RO_PROBE(fdt);

	สำหรับ (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ถ้า ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			คืน offset;

	ถ้า (depth < 0)
		คืน -FDT_ERR_NOTFOUND;
	คืน offset; /* error */
}

จำนวนเต็ม fdt_subnode_offset(const void *fdt, จำนวนเต็ม parentoffset,
		       const var_na *name)
{
	คืน fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

จำนวนเต็ม fdt_path_offset_namelen(const void *fdt, const var_na *path, จำนวนเต็ม namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	จำนวนเต็ม offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ถ้า (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ถ้า (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ถ้า (!p)
			คืน -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ขณะ (p < end) {
		const var_na *q;

		ขณะ (*p == '/') {
			p++;
			ถ้า (p == end)
				คืน offset;
		}
		q = memchr(p, '/', end - p);
		ถ้า (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ถ้า (offset < 0)
			คืน offset;

		p = q;
	}

	คืน offset;
}

จำนวนเต็ม fdt_path_offset(const void *fdt, const var_na *path)
{
	คืน fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, จำนวนเต็ม nodeoffset, จำนวนเต็ม *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	จำนวนเต็ม err;

	ถ้า (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ถ้า (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ถ้า (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ถ้า (len)
		*len = strlen(nameptr);

	คืน nameptr;

 fail:
	ถ้า (len)
		*len = err;
	คืน NULL;
}

จำนวนเต็ม fdt_first_property_offset(const void *fdt, จำนวนเต็ม nodeoffset)
{
	จำนวนเต็ม offset;

	ถ้า ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		คืน offset;

	คืน nextprop_(fdt, offset);
}

จำนวนเต็ม fdt_next_property_offset(const void *fdt, จำนวนเต็ม offset)
{
	ถ้า ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		คืน offset;

	คืน nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              จำนวนเต็ม offset,
						              จำนวนเต็ม *lenp)
{
	จำนวนเต็ม err;
	const sa_nracanaa fdt_property *prop;

	ถ้า (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ถ้า (lenp)
			*lenp = err;
		คืน NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ถ้า (lenp)
		*lenp = fdt32_ld_(&prop->len);

	คืน prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      จำนวนเต็ม offset,
						      จำนวนเต็ม *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ถ้า (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ถ้า (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		คืน NULL;
	}

	คืน fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            จำนวนเต็ม offset,
						            const var_na *name,
						            จำนวนเต็ม namelen,
							    จำนวนเต็ม *lenp,
							    จำนวนเต็ม *poffset)
{
	สำหรับ (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ถ้า (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ถ้า (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ถ้า (poffset)
				*poffset = offset;
			คืน prop;
		}
	}

	ถ้า (lenp)
		*lenp = offset;
	คืน NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    จำนวนเต็ม offset,
						    const var_na *name,
						    จำนวนเต็ม namelen, จำนวนเต็ม *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ถ้า (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ถ้า (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		คืน NULL;
	}

	คืน fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    จำนวนเต็ม nodeoffset,
					    const var_na *name, จำนวนเต็ม *lenp)
{
	คืน fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, จำนวนเต็ม nodeoffset,
				const var_na *name, จำนวนเต็ม namelen, จำนวนเต็ม *lenp)
{
	จำนวนเต็ม poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ถ้า (!prop)
		คืน NULL;

	/* Handle realignment */
	ถ้า (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		คืน prop->data + 4;
	คืน prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, จำนวนเต็ม offset,
				  const var_na **namep, จำนวนเต็ม *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ถ้า (!prop)
		คืน NULL;
	ถ้า (namep) {
		const var_na *name;
		จำนวนเต็ม namelen;

		ถ้า (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ถ้า (!name) {
				ถ้า (lenp)
					*lenp = namelen;
				คืน NULL;
			}
			*namep = name;
		} มิฉะนั้น {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ถ้า (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		คืน prop->data + 4;
	คืน prop->data;
}

const void *fdt_getprop(const void *fdt, จำนวนเต็ม nodeoffset,
			const var_na *name, จำนวนเต็ม *lenp)
{
	คืน fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, จำนวนเต็ม nodeoffset)
{
	const fdt32_t *php;
	จำนวนเต็ม len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ถ้า (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ถ้า (!php || (len != parimaa_na(*php)))
			คืน 0;
	}

	คืน fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, จำนวนเต็ม namelen)
{
	จำนวนเต็ม aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ถ้า (aliasoffset < 0)
		คืน NULL;

	คืน fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	คืน fdt_get_alias_namelen(fdt, name, strlen(name));
}

จำนวนเต็ม fdt_get_path(const void *fdt, จำนวนเต็ม nodeoffset, var_na *buf, จำนวนเต็ม buflen)
{
	จำนวนเต็ม pdepth = 0, p = 0;
	จำนวนเต็ม offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ถ้า (buflen < 2)
		คืน -FDT_ERR_NOSPACE;

	สำหรับ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ขณะ (pdepth > depth) {
			do {
				p--;
			} ขณะ (buf[p-1] != '/');
			pdepth--;
		}

		ถ้า (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ถ้า (!name)
				คืน namelen;
			ถ้า ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ถ้า (offset == nodeoffset) {
			ถ้า (pdepth < (depth + 1))
				คืน -FDT_ERR_NOSPACE;

			ถ้า (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			คืน 0;
		}
	}

	ถ้า ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		คืน -FDT_ERR_BADOFFSET;
	มิฉะนั้น ถ้า (offset == -FDT_ERR_BADOFFSET)
		คืน -FDT_ERR_BADSTRUCTURE;

	คืน offset; /* error from fdt_next_node() */
}

จำนวนเต็ม fdt_supernode_atdepth_offset(const void *fdt, จำนวนเต็ม nodeoffset,
				 จำนวนเต็ม supernodedepth, จำนวนเต็ม *nodedepth)
{
	จำนวนเต็ม offset, depth;
	จำนวนเต็ม supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ถ้า (supernodedepth < 0)
		คืน -FDT_ERR_NOTFOUND;

	สำหรับ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ถ้า (depth == supernodedepth)
			supernodeoffset = offset;

		ถ้า (offset == nodeoffset) {
			ถ้า (nodedepth)
				*nodedepth = depth;

			ถ้า (supernodedepth > depth)
				คืน -FDT_ERR_NOTFOUND;
			มิฉะนั้น
				คืน supernodeoffset;
		}
	}

	ถ้า (!can_assume(VALID_INPUT)) {
		ถ้า ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			คืน -FDT_ERR_BADOFFSET;
		มิฉะนั้น ถ้า (offset == -FDT_ERR_BADOFFSET)
			คืน -FDT_ERR_BADSTRUCTURE;
	}

	คืน offset; /* error from fdt_next_node() */
}

จำนวนเต็ม fdt_node_depth(const void *fdt, จำนวนเต็ม nodeoffset)
{
	จำนวนเต็ม nodedepth;
	จำนวนเต็ม err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ถ้า (err)
		คืน (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	คืน nodedepth;
}

จำนวนเต็ม fdt_parent_offset(const void *fdt, จำนวนเต็ม nodeoffset)
{
	จำนวนเต็ม nodedepth = fdt_node_depth(fdt, nodeoffset);

	ถ้า (nodedepth < 0)
		คืน nodedepth;
	คืน fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

จำนวนเต็ม fdt_node_offset_by_prop_value(const void *fdt, จำนวนเต็ม startoffset,
				  const var_na *propname,
				  const void *propval, จำนวนเต็ม proplen)
{
	จำนวนเต็ม offset;
	const void *val;
	จำนวนเต็ม len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	สำหรับ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ถ้า (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			คืน offset;
	}

	คืน offset; /* error from fdt_next_node() */
}

จำนวนเต็ม fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	จำนวนเต็ม offset;

	ถ้า ((phandle == 0) || (phandle == ~0U))
		คืน -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	สำหรับ (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		ถ้า (fdt_get_phandle(fdt, offset) == phandle)
			คืน offset;
	}

	คืน offset; /* error from fdt_next_node() */
}

จำนวนเต็ม fdt_stringlist_contains(const var_na *strlist, จำนวนเต็ม listlen, const var_na *str)
{
	จำนวนเต็ม len = strlen(str);
	const var_na *p;

	ขณะ (listlen >= len) {
		ถ้า (memcmp(str, strlist, len+1) == 0)
			คืน 1;
		p = memchr(strlist, '\0', listlen);
		ถ้า (!p)
			คืน 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	คืน 0;
}

จำนวนเต็ม fdt_stringlist_count(const void *fdt, จำนวนเต็ม nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	จำนวนเต็ม length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ถ้า (!list)
		คืน length;

	end = list + length;

	ขณะ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ถ้า (list + length > end)
			คืน -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	คืน count;
}

จำนวนเต็ม fdt_stringlist_search(const void *fdt, จำนวนเต็ม nodeoffset, const var_na *property,
			  const var_na *string)
{
	จำนวนเต็ม length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ถ้า (!list)
		คืน length;

	len = strlen(string) + 1;
	end = list + length;

	ขณะ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ถ้า (list + length > end)
			คืน -FDT_ERR_BADVALUE;

		ถ้า (length == len && memcmp(list, string, length) == 0)
			คืน idx;

		list += length;
		idx++;
	}

	คืน -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, จำนวนเต็ม nodeoffset,
			       const var_na *property, จำนวนเต็ม idx,
			       จำนวนเต็ม *lenp)
{
	const var_na *list, *end;
	จำนวนเต็ม length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ถ้า (!list) {
		ถ้า (lenp)
			*lenp = length;

		คืน NULL;
	}

	end = list + length;

	ขณะ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ถ้า (list + length > end) {
			ถ้า (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			คืน NULL;
		}

		ถ้า (idx == 0) {
			ถ้า (lenp)
				*lenp = length - 1;

			คืน list;
		}

		list += length;
		idx--;
	}

	ถ้า (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	คืน NULL;
}

จำนวนเต็ม fdt_node_check_compatible(const void *fdt, จำนวนเต็ม nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	จำนวนเต็ม len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ถ้า (!prop)
		คืน len;

	คืน !fdt_stringlist_contains(prop, len, compatible);
}

จำนวนเต็ม fdt_node_offset_by_compatible(const void *fdt, จำนวนเต็ม startoffset,
				  const var_na *compatible)
{
	จำนวนเต็ม offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	สำหรับ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		ถ้า ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			คืน err;
		มิฉะนั้น ถ้า (err == 0)
			คืน offset;
	}

	คืน offset; /* error from fdt_next_node() */
}
