// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

/**
 * overlay_get_target_phandle - retrieves the target phandle of a fragment
 * @fdto: pointer to the device tree overlay blob
 * @fragment: node offset of the fragment in the overlay
 *
 * overlay_get_target_phandle() retrieves the target phandle of an
 * overlay fragment when that fragment uses a phandle (target
 * property) instead of a path (target-path property).
 *
 * returns:
 *      the phandle pointed by the target property
 *      0, if the phandle was not found
 *	-1, if the phandle was malformed
 */
ساکن uint32_t overlay_get_ہدف_phandle(ثابت خالی *fdto, صحیح fragment)
{
	ثابت fdt32_t *val;
	صحیح len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	اگر (!val)
		واپس 0;

	اگر ((len != حجم(*val)) || (fdt32_مقصد_cpu(*val) == (uint32_t)-1))
		واپس (uint32_t)-1;

	واپس fdt32_مقصد_cpu(*val);
}

/**
 * overlay_get_target - retrieves the offset of a fragment's target
 * @fdt: Base device tree blob
 * @fdto: Device tree overlay blob
 * @fragment: node offset of the fragment in the overlay
 * @pathp: pointer which receives the path of the target (or NULL)
 *
 * overlay_get_target() retrieves the target offset in the base
 * device tree of a fragment, no matter how the actual targeting is
 * done (through a phandle or a path)
 *
 * returns:
 *      the targeted node offset in the base device tree
 *      Negative error code on error
 */
ساکن صحیح overlay_get_ہدف(ثابت خالی *fdt, ثابت خالی *fdto,
			      صحیح fragment, حرف ثابت **pathp)
{
	uint32_t phandle;
	ثابت حرف *راستہ = تہی;
	صحیح راستہ_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_ہدف_phandle(fdto, fragment);
	اگر (phandle == (uint32_t)-1)
		واپس -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	اگر (!phandle) {
		/* And then a path based lookup */
		راستہ = fdt_getprop(fdto, fragment, "target-path", &راستہ_len);
		اگر (راستہ)
			ret = fdt_راستہ_انحراف(fdt, راستہ);
		ورنہ
			ret = راستہ_len;
	} ورنہ
		ret = fdt_گرہ_انحراف_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	اگر (ret < 0 && راستہ_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	اگر (ret < 0)
		واپس ret;

	/* return pointer to path (if available) */
	اگر (pathp)
		*pathp = راستہ ? راستہ : تہی;

	واپس ret;
}

/**
 * overlay_phandle_add_offset - Increases a phandle by an offset
 * @fdt: Base device tree blob
 * @node: Device tree overlay blob
 * @name: Name of the property to modify (phandle or linux,phandle)
 * @delta: offset to apply
 *
 * overlay_phandle_add_offset() increments a node phandle by a given
 * offset.
 *
 * returns:
 *      0 on success.
 *      Negative error code on error
 */
ساکن صحیح overlay_phandle_جوڑو_انحراف(خالی *fdt, صحیح گرہ,
				      ثابت حرف *name, uint32_t delta)
{
	ثابت fdt32_t *val;
	uint32_t adj_val;
	صحیح len;

	val = fdt_getprop(fdt, گرہ, name, &len);
	اگر (!val)
		واپس len;

	اگر (len != حجم(*val))
		واپس -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_مقصد_cpu(*val);
	اگر ((adj_val + delta) < adj_val)
		واپس -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	اگر (adj_val == (uint32_t)-1)
		واپس -FDT_ERR_NOPHANDLES;

	واپس fdt_setprop_inplace_u32(fdt, گرہ, name, adj_val);
}

/**
 * overlay_adjust_node_phandles - Offsets the phandles of a node
 * @fdto: Device tree overlay blob
 * @node: Offset of the node we want to adjust
 * @delta: Offset to shift the phandles of
 *
 * overlay_adjust_node_phandles() adds a constant to all the phandles
 * of a given node. This is mainly use as part of the overlay
 * application process, when we want to update all the overlay
 * phandles to not conflict with the overlays of the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_adjust_گرہ_phandles(خالی *fdto, صحیح گرہ,
					uint32_t delta)
{
	صحیح بچہ;
	صحیح ret;

	ret = overlay_phandle_جوڑو_انحراف(fdto, گرہ, "phandle", delta);
	اگر (ret && ret != -FDT_ERR_NOTFOUND)
		واپس ret;

	ret = overlay_phandle_جوڑو_انحراف(fdto, گرہ, "linux,phandle", delta);
	اگر (ret && ret != -FDT_ERR_NOTFOUND)
		واپس ret;

	fdt_for_each_subnode(بچہ, fdto, گرہ) {
		ret = overlay_adjust_گرہ_phandles(fdto, بچہ, delta);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

/**
 * overlay_adjust_local_phandles - Adjust the phandles of a whole overlay
 * @fdto: Device tree overlay blob
 * @delta: Offset to shift the phandles of
 *
 * overlay_adjust_local_phandles() adds a constant to all the
 * phandles of an overlay. This is mainly use as part of the overlay
 * application process, when we want to update all the overlay
 * phandles to not conflict with the overlays of the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_adjust_local_phandles(خالی *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	واپس overlay_adjust_گرہ_phandles(fdto, 0, delta);
}

/**
 * overlay_update_local_node_references - Adjust the overlay references
 * @fdto: Device tree overlay blob
 * @tree_node: Node offset of the node to operate on
 * @fixup_node: Node offset of the matching local fixups node
 * @delta: Offset to shift the phandles of
 *
 * overlay_update_local_nodes_references() update the phandles
 * pointing to a node within the device tree overlay by adding a
 * constant delta.
 *
 * This is mainly used as part of a device tree application process,
 * where you want the device tree overlays phandles to not conflict
 * with the ones from the base device tree before merging them.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_update_local_گرہ_references(خالی *fdto,
						صحیح درخت_گرہ,
						صحیح fixup_گرہ,
						uint32_t delta)
{
	صحیح fixup_prop;
	صحیح fixup_بچہ;
	صحیح ret;

	fdt_for_each_property_انحراف(fixup_prop, fdto, fixup_گرہ) {
		ثابت fdt32_t *fixup_val;
		ثابت حرف *درخت_val;
		ثابت حرف *name;
		صحیح fixup_len;
		صحیح درخت_len;
		صحیح i;

		fixup_val = fdt_getprop_by_انحراف(fdto, fixup_prop,
						  &name, &fixup_len);
		اگر (!fixup_val)
			واپس fixup_len;

		اگر (fixup_len % حجم(uint32_t))
			واپس -FDT_ERR_BADOVERLAY;
		fixup_len /= حجم(uint32_t);

		درخت_val = fdt_getprop(fdto, درخت_گرہ, name, &درخت_len);
		اگر (!درخت_val) {
			اگر (درخت_len == -FDT_ERR_NOTFOUND)
				واپس -FDT_ERR_BADOVERLAY;

			واپس درخت_len;
		}

		برائے (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_مقصد_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			حافظہ_نقل(&adj_val, درخت_val + poffset, حجم(adj_val));

			adj_val = cpu_مقصد_fdt32(fdt32_مقصد_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  درخت_گرہ,
								  name,
								  لڑی_لمبائی(name),
								  poffset,
								  &adj_val,
								  حجم(adj_val));
			اگر (ret == -FDT_ERR_NOSPACE)
				واپس -FDT_ERR_BADOVERLAY;

			اگر (ret)
				واپس ret;
		}
	}

	fdt_for_each_subnode(fixup_بچہ, fdto, fixup_گرہ) {
		ثابت حرف *fixup_بچہ_name = fdt_get_name(fdto, fixup_بچہ,
							    تہی);
		صحیح درخت_بچہ;

		درخت_بچہ = fdt_subnode_انحراف(fdto, درخت_گرہ,
						fixup_بچہ_name);
		اگر (درخت_بچہ == -FDT_ERR_NOTFOUND)
			واپس -FDT_ERR_BADOVERLAY;
		اگر (درخت_بچہ < 0)
			واپس درخت_بچہ;

		ret = overlay_update_local_گرہ_references(fdto,
							   درخت_بچہ,
							   fixup_بچہ,
							   delta);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

/**
 * overlay_update_local_references - Adjust the overlay references
 * @fdto: Device tree overlay blob
 * @delta: Offset to shift the phandles of
 *
 * overlay_update_local_references() update all the phandles pointing
 * to a node within the device tree overlay by adding a constant
 * delta to not conflict with the base overlay.
 *
 * This is mainly used as part of a device tree application process,
 * where you want the device tree overlays phandles to not conflict
 * with the ones from the base device tree before merging them.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_update_local_references(خالی *fdto, uint32_t delta)
{
	صحیح fixups;

	fixups = fdt_راستہ_انحراف(fdto, "/__local_fixups__");
	اگر (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		اگر (fixups == -FDT_ERR_NOTFOUND)
			واپس 0;

		واپس fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	واپس overlay_update_local_گرہ_references(fdto, 0, fixups,
						    delta);
}

/**
 * overlay_fixup_one_phandle - Set an overlay phandle to the base one
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 * @symbols_off: Node offset of the symbols node in the base device tree
 * @path: Path to a node holding a phandle in the overlay
 * @path_len: number of path characters to consider
 * @name: Name of the property holding the phandle reference in the overlay
 * @name_len: number of name characters to consider
 * @poffset: Offset within the overlay property where the phandle is stored
 * @label: Label of the node referenced by the phandle
 *
 * overlay_fixup_one_phandle() resolves an overlay phandle pointing to
 * a node in the base device tree.
 *
 * This is part of the device tree overlay application process, when
 * you want all the phandles in the overlay to point to the actual
 * base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_fixup_one_phandle(خالی *fdt, خالی *fdto,
				     صحیح symbols_off,
				     ثابت حرف *راستہ, uint32_t راستہ_len,
				     ثابت حرف *name, uint32_t name_len,
				     صحیح poffset, ثابت حرف *label)
{
	ثابت حرف *علامت_راستہ;
	uint32_t phandle;
	fdt32_t phandle_prop;
	صحیح علامت_off, fixup_off;
	صحیح prop_len;

	اگر (symbols_off < 0)
		واپس symbols_off;

	علامت_راستہ = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	اگر (!علامت_راستہ)
		واپس prop_len;

	علامت_off = fdt_راستہ_انحراف(fdt, علامت_راستہ);
	اگر (علامت_off < 0)
		واپس علامت_off;

	phandle = fdt_get_phandle(fdt, علامت_off);
	اگر (!phandle)
		واپس -FDT_ERR_NOTFOUND;

	fixup_off = fdt_راستہ_انحراف_namelen(fdto, راستہ, راستہ_len);
	اگر (fixup_off == -FDT_ERR_NOTFOUND)
		واپس -FDT_ERR_BADOVERLAY;
	اگر (fixup_off < 0)
		واپس fixup_off;

	phandle_prop = cpu_مقصد_fdt32(phandle);
	واپس fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   حجم(phandle_prop));
}

/**
 * overlay_fixup_phandle - Set an overlay phandle to the base one
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 * @symbols_off: Node offset of the symbols node in the base device tree
 * @property: Property offset in the overlay holding the list of fixups
 *
 * overlay_fixup_phandle() resolves all the overlay phandles pointed
 * to in a __fixups__ property, and updates them to match the phandles
 * in use in the base device tree.
 *
 * This is part of the device tree overlay application process, when
 * you want all the phandles in the overlay to point to the actual
 * base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_fixup_phandle(خالی *fdt, خالی *fdto, صحیح symbols_off,
				 صحیح property)
{
	ثابت حرف *قدر;
	ثابت حرف *label;
	صحیح len;

	قدر = fdt_getprop_by_انحراف(fdto, property,
				      &label, &len);
	اگر (!قدر) {
		اگر (len == -FDT_ERR_NOTFOUND)
			واپس -FDT_ERR_اندرونی;

		واپس len;
	}

	کرو {
		ثابت حرف *راستہ, *name, *fixup_انتہا;
		ثابت حرف *fixup_str = قدر;
		uint32_t راستہ_len, name_len;
		uint32_t fixup_len;
		حرف *sep, *endptr;
		صحیح poffset, ret;

		fixup_انتہا = memchr(قدر, '\0', len);
		اگر (!fixup_انتہا)
			واپس -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_انتہا - fixup_str;

		len -= fixup_len + 1;
		قدر += fixup_len + 1;

		راستہ = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		اگر (!sep || *sep != ':')
			واپس -FDT_ERR_BADOVERLAY;

		راستہ_len = sep - راستہ;
		اگر (راستہ_len == (fixup_len - 1))
			واپس -FDT_ERR_BADOVERLAY;

		fixup_len -= راستہ_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		اگر (!sep || *sep != ':')
			واپس -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		اگر (!name_len)
			واپس -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		اگر ((*endptr != '\0') || (endptr <= (sep + 1)))
			واپس -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						راستہ, راستہ_len, name, name_len,
						poffset, label);
		اگر (ret)
			واپس ret;
	} جب (len > 0);

	واپس 0;
}

/**
 * overlay_fixup_phandles - Resolve the overlay phandles to the base
 *                          device tree
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_fixup_phandles() resolves all the overlay phandles pointing
 * to nodes in the base device tree.
 *
 * This is one of the steps of the device tree overlay application
 * process, when you want all the phandles in the overlay to point to
 * the actual base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_fixup_phandles(خالی *fdt, خالی *fdto)
{
	صحیح fixups_off, symbols_off;
	صحیح property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_راستہ_انحراف(fdto, "/__fixups__");
	اگر (fixups_off == -FDT_ERR_NOTFOUND)
		واپس 0; /* nothing to do */
	اگر (fixups_off < 0)
		واپس fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_راستہ_انحراف(fdt, "/__symbols__");
	اگر ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		واپس symbols_off;

	fdt_for_each_property_انحراف(property, fdto, fixups_off) {
		صحیح ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

/**
 * overlay_apply_node - Merges a node into the base device tree
 * @fdt: Base Device Tree blob
 * @target: Node offset in the base device tree to apply the fragment to
 * @fdto: Device tree overlay blob
 * @node: Node offset in the overlay holding the changes to merge
 *
 * overlay_apply_node() merges a node into a target base device tree
 * node pointed.
 *
 * This is part of the final step in the device tree overlay
 * application process, when all the phandles have been adjusted and
 * resolved and you just have to merge overlay into the base device
 * tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_apply_گرہ(خالی *fdt, صحیح ہدف,
			      خالی *fdto, صحیح گرہ)
{
	صحیح property;
	صحیح subnode;

	fdt_for_each_property_انحراف(property, fdto, گرہ) {
		ثابت حرف *name;
		ثابت خالی *prop;
		صحیح prop_len;
		صحیح ret;

		prop = fdt_getprop_by_انحراف(fdto, property, &name,
					     &prop_len);
		اگر (prop_len == -FDT_ERR_NOTFOUND)
			واپس -FDT_ERR_اندرونی;
		اگر (prop_len < 0)
			واپس prop_len;

		ret = fdt_setprop(fdt, ہدف, name, prop, prop_len);
		اگر (ret)
			واپس ret;
	}

	fdt_for_each_subnode(subnode, fdto, گرہ) {
		ثابت حرف *name = fdt_get_name(fdto, subnode, تہی);
		صحیح nnode;
		صحیح ret;

		nnode = fdt_جوڑو_subnode(fdt, ہدف, name);
		اگر (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_انحراف(fdt, ہدف, name);
			اگر (nnode == -FDT_ERR_NOTFOUND)
				واپس -FDT_ERR_اندرونی;
		}

		اگر (nnode < 0)
			واپس nnode;

		ret = overlay_apply_گرہ(fdt, nnode, fdto, subnode);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

/**
 * overlay_merge - Merge an overlay into its base device tree
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_merge() merges an overlay into its base device tree.
 *
 * This is the next to last step in the device tree overlay application
 * process, when all the phandles have been adjusted and resolved and
 * you just have to merge overlay into the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_ملاؤ(خالی *fdt, خالی *fdto)
{
	صحیح fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		صحیح overlay;
		صحیح ہدف;
		صحیح ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_انحراف(fdto, fragment, "__overlay__");
		اگر (overlay == -FDT_ERR_NOTFOUND)
			جاری;

		اگر (overlay < 0)
			واپس overlay;

		ہدف = overlay_get_ہدف(fdt, fdto, fragment, تہی);
		اگر (ہدف < 0)
			واپس ہدف;

		ret = overlay_apply_گرہ(fdt, ہدف, fdto, overlay);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

ساکن صحیح get_راستہ_len(ثابت خالی *fdt, صحیح nodeoffset)
{
	صحیح len = 0, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	برائے (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		اگر (!name)
			واپس namelen;

		/* root? we're done */
		اگر (namelen == 0)
			توڑو;

		nodeoffset = fdt_والد_انحراف(fdt, nodeoffset);
		اگر (nodeoffset < 0)
			واپس nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	اگر (len == 0)
		len++;
	واپس len;
}

/**
 * overlay_symbol_update - Update the symbols of base tree after a merge
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_symbol_update() updates the symbols of the base tree with the
 * symbols of the applied overlay
 *
 * This is the last step in the device tree overlay application
 * process, allowing the reference of overlay symbols by subsequent
 * overlay operations.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ساکن صحیح overlay_علامت_update(خالی *fdt, خالی *fdto)
{
	صحیح جڑ_sym, ov_sym, prop, راستہ_len, fragment, ہدف;
	صحیح len, frag_name_len, ret, rel_راستہ_len;
	ثابت حرف *s, *e;
	ثابت حرف *راستہ;
	ثابت حرف *name;
	ثابت حرف *frag_name;
	ثابت حرف *rel_راستہ;
	ثابت حرف *ہدف_راستہ;
	حرف *buf;
	خالی *p;

	ov_sym = fdt_subnode_انحراف(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	اگر (ov_sym < 0)
		واپس 0;

	جڑ_sym = fdt_subnode_انحراف(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	اگر (جڑ_sym == -FDT_ERR_NOTFOUND)
		جڑ_sym = fdt_جوڑو_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	اگر (جڑ_sym < 0)
		واپس جڑ_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_انحراف(prop, fdto, ov_sym) {
		راستہ = fdt_getprop_by_انحراف(fdto, prop, &name, &راستہ_len);
		اگر (!راستہ)
			واپس راستہ_len;

		/* verify it's a string property (terminated by a single \0) */
		اگر (راستہ_len < 1 || memchr(راستہ, '\0', راستہ_len) != &راستہ[راستہ_len - 1])
			واپس -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = راستہ + راستہ_len;

		اگر (*راستہ != '/')
			واپس -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(راستہ + 1, '/');
		اگر (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			جاری;
		}

		frag_name = راستہ + 1;
		frag_name_len = s - راستہ - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = حجم("/__overlay__/") - 1;
		اگر ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_راستہ = s + len;
			rel_راستہ_len = e - rel_راستہ - 1;
		} ورنہ اگر ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_راستہ = "";
			rel_راستہ_len = 0;
		} ورنہ {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			جاری;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_انحراف_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		اگر (ret < 0)
			واپس -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_انحراف(fdto, fragment, "__overlay__");
		اگر (ret < 0)
			واپس -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_ہدف(fdt, fdto, fragment, &ہدف_راستہ);
		اگر (ret < 0)
			واپس ret;
		ہدف = ret;

		/* if we have a target path use */
		اگر (!ہدف_راستہ) {
			ret = get_راستہ_len(fdt, ہدف);
			اگر (ret < 0)
				واپس ret;
			len = ret;
		} ورنہ {
			len = لڑی_لمبائی(ہدف_راستہ);
		}

		ret = fdt_setprop_placeholder(fdt, جڑ_sym, name,
				len + (len > 1) + rel_راستہ_len + 1, &p);
		اگر (ret < 0)
			واپس ret;

		اگر (!ہدف_راستہ) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_ہدف(fdt, fdto, fragment, &ہدف_راستہ);
			اگر (ret < 0)
				واپس ret;
			ہدف = ret;
		}

		buf = p;
		اگر (len > 1) { /* target is not root */
			اگر (!ہدف_راستہ) {
				ret = fdt_get_راستہ(fdt, ہدف, buf, len + 1);
				اگر (ret < 0)
					واپس ret;
			} ورنہ
				حافظہ_نقل(buf, ہدف_راستہ, len + 1);

		} ورنہ
			len--;

		buf[len] = '/';
		حافظہ_نقل(buf + len + 1, rel_راستہ, rel_راستہ_len);
		buf[len + 1 + rel_راستہ_len] = '\0';
	}

	واپس 0;
}

صحیح fdt_overlay_apply(خالی *fdt, خالی *fdto)
{
	uint32_t delta;
	صحیح ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_ڈھونڈو_max_phandle(fdt, &delta);
	اگر (ret)
		جاؤ err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	اگر (ret)
		جاؤ err;

	ret = overlay_update_local_references(fdto, delta);
	اگر (ret)
		جاؤ err;

	ret = overlay_fixup_phandles(fdt, fdto);
	اگر (ret)
		جاؤ err;

	ret = overlay_ملاؤ(fdt, fdto);
	اگر (ret)
		جاؤ err;

	ret = overlay_علامت_update(fdt, fdto);
	اگر (ret)
		جاؤ err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_سیٹ_magic(fdto, ~0);

	واپس 0;

err:
	/*
	 * The overlay might have been damaged, erase its magic.
	 */
	fdt_سیٹ_magic(fdto, ~0);

	/*
	 * The base device tree might have been damaged, erase its
	 * magic.
	 */
	fdt_سیٹ_magic(fdt, ~0);

	واپس ret;
}
