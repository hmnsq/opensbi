// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

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
സ്ഥിതം uint32_t overlay_get_target_phandle(സ്ഥിരം ശൂന്യം *fdto, പൂർണ്ണസംഖ്യ fragment)
{
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	എങ്കിൽ (!val)
		തിരികെ 0;

	എങ്കിൽ ((len != വലുപ്പം(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		തിരികെ (uint32_t)-1;

	തിരികെ fdt32_to_cpu(*val);
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_get_target(സ്ഥിരം ശൂന്യം *fdt, സ്ഥിരം ശൂന്യം *fdto,
			      പൂർണ്ണസംഖ്യ fragment, അക്ഷരം സ്ഥിരം **pathp)
{
	uint32_t phandle;
	സ്ഥിരം അക്ഷരം *path = ശൂന്യ;
	പൂർണ്ണസംഖ്യ path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	എങ്കിൽ (phandle == (uint32_t)-1)
		തിരികെ -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	എങ്കിൽ (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		എങ്കിൽ (path)
			ret = fdt_path_offset(fdt, path);
		അല്ലെങ്കിൽ
			ret = path_len;
	} അല്ലെങ്കിൽ
		ret = fdt_കണ്ണി_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	എങ്കിൽ (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	എങ്കിൽ (ret < 0)
		തിരികെ ret;

	/* return pointer to path (if available) */
	എങ്കിൽ (pathp)
		*pathp = path ? path : ശൂന്യ;

	തിരികെ ret;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_phandle_add_offset(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ കണ്ണി,
				      സ്ഥിരം അക്ഷരം *name, uint32_t delta)
{
	സ്ഥിരം fdt32_t *val;
	uint32_t adj_val;
	പൂർണ്ണസംഖ്യ len;

	val = fdt_getprop(fdt, കണ്ണി, name, &len);
	എങ്കിൽ (!val)
		തിരികെ len;

	എങ്കിൽ (len != വലുപ്പം(*val))
		തിരികെ -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	എങ്കിൽ ((adj_val + delta) < adj_val)
		തിരികെ -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	എങ്കിൽ (adj_val == (uint32_t)-1)
		തിരികെ -FDT_ERR_NOPHANDLES;

	തിരികെ fdt_setprop_inplace_u32(fdt, കണ്ണി, name, adj_val);
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_adjust_കണ്ണി_phandles(ശൂന്യം *fdto, പൂർണ്ണസംഖ്യ കണ്ണി,
					uint32_t delta)
{
	പൂർണ്ണസംഖ്യ child;
	പൂർണ്ണസംഖ്യ ret;

	ret = overlay_phandle_add_offset(fdto, കണ്ണി, "phandle", delta);
	എങ്കിൽ (ret && ret != -FDT_ERR_NOTFOUND)
		തിരികെ ret;

	ret = overlay_phandle_add_offset(fdto, കണ്ണി, "linux,phandle", delta);
	എങ്കിൽ (ret && ret != -FDT_ERR_NOTFOUND)
		തിരികെ ret;

	fdt_for_each_subnode(child, fdto, കണ്ണി) {
		ret = overlay_adjust_കണ്ണി_phandles(fdto, child, delta);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_adjust_local_phandles(ശൂന്യം *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	തിരികെ overlay_adjust_കണ്ണി_phandles(fdto, 0, delta);
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_update_local_കണ്ണി_references(ശൂന്യം *fdto,
						പൂർണ്ണസംഖ്യ വൃക്ഷ_കണ്ണി,
						പൂർണ്ണസംഖ്യ fixup_കണ്ണി,
						uint32_t delta)
{
	പൂർണ്ണസംഖ്യ fixup_prop;
	പൂർണ്ണസംഖ്യ fixup_child;
	പൂർണ്ണസംഖ്യ ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_കണ്ണി) {
		സ്ഥിരം fdt32_t *fixup_val;
		സ്ഥിരം അക്ഷരം *വൃക്ഷം_val;
		സ്ഥിരം അക്ഷരം *name;
		പൂർണ്ണസംഖ്യ fixup_len;
		പൂർണ്ണസംഖ്യ വൃക്ഷം_len;
		പൂർണ്ണസംഖ്യ i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		എങ്കിൽ (!fixup_val)
			തിരികെ fixup_len;

		എങ്കിൽ (fixup_len % വലുപ്പം(uint32_t))
			തിരികെ -FDT_ERR_BADOVERLAY;
		fixup_len /= വലുപ്പം(uint32_t);

		വൃക്ഷം_val = fdt_getprop(fdto, വൃക്ഷ_കണ്ണി, name, &വൃക്ഷം_len);
		എങ്കിൽ (!വൃക്ഷം_val) {
			എങ്കിൽ (വൃക്ഷം_len == -FDT_ERR_NOTFOUND)
				തിരികെ -FDT_ERR_BADOVERLAY;

			തിരികെ വൃക്ഷം_len;
		}

		വേണ്ടി (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, വൃക്ഷം_val + poffset, വലുപ്പം(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  വൃക്ഷ_കണ്ണി,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  വലുപ്പം(adj_val));
			എങ്കിൽ (ret == -FDT_ERR_NOSPACE)
				തിരികെ -FDT_ERR_BADOVERLAY;

			എങ്കിൽ (ret)
				തിരികെ ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_കണ്ണി) {
		സ്ഥിരം അക്ഷരം *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    ശൂന്യ);
		പൂർണ്ണസംഖ്യ വൃക്ഷം_child;

		വൃക്ഷം_child = fdt_subnode_offset(fdto, വൃക്ഷ_കണ്ണി,
						fixup_child_name);
		എങ്കിൽ (വൃക്ഷം_child == -FDT_ERR_NOTFOUND)
			തിരികെ -FDT_ERR_BADOVERLAY;
		എങ്കിൽ (വൃക്ഷം_child < 0)
			തിരികെ വൃക്ഷം_child;

		ret = overlay_update_local_കണ്ണി_references(fdto,
							   വൃക്ഷം_child,
							   fixup_child,
							   delta);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_update_local_references(ശൂന്യം *fdto, uint32_t delta)
{
	പൂർണ്ണസംഖ്യ fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	എങ്കിൽ (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		എങ്കിൽ (fixups == -FDT_ERR_NOTFOUND)
			തിരികെ 0;

		തിരികെ fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	തിരികെ overlay_update_local_കണ്ണി_references(fdto, 0, fixups,
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_fixup_one_phandle(ശൂന്യം *fdt, ശൂന്യം *fdto,
				     പൂർണ്ണസംഖ്യ symbols_off,
				     സ്ഥിരം അക്ഷരം *path, uint32_t path_len,
				     സ്ഥിരം അക്ഷരം *name, uint32_t name_len,
				     പൂർണ്ണസംഖ്യ poffset, സ്ഥിരം അക്ഷരം *label)
{
	സ്ഥിരം അക്ഷരം *ചിഹ്നം_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	പൂർണ്ണസംഖ്യ ചിഹ്നം_off, fixup_off;
	പൂർണ്ണസംഖ്യ prop_len;

	എങ്കിൽ (symbols_off < 0)
		തിരികെ symbols_off;

	ചിഹ്നം_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	എങ്കിൽ (!ചിഹ്നം_path)
		തിരികെ prop_len;

	ചിഹ്നം_off = fdt_path_offset(fdt, ചിഹ്നം_path);
	എങ്കിൽ (ചിഹ്നം_off < 0)
		തിരികെ ചിഹ്നം_off;

	phandle = fdt_get_phandle(fdt, ചിഹ്നം_off);
	എങ്കിൽ (!phandle)
		തിരികെ -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	എങ്കിൽ (fixup_off == -FDT_ERR_NOTFOUND)
		തിരികെ -FDT_ERR_BADOVERLAY;
	എങ്കിൽ (fixup_off < 0)
		തിരികെ fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	തിരികെ fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   വലുപ്പം(phandle_prop));
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_fixup_phandle(ശൂന്യം *fdt, ശൂന്യം *fdto, പൂർണ്ണസംഖ്യ symbols_off,
				 പൂർണ്ണസംഖ്യ property)
{
	സ്ഥിരം അക്ഷരം *മൂല്യം;
	സ്ഥിരം അക്ഷരം *label;
	പൂർണ്ണസംഖ്യ len;

	മൂല്യം = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	എങ്കിൽ (!മൂല്യം) {
		എങ്കിൽ (len == -FDT_ERR_NOTFOUND)
			തിരികെ -FDT_ERR_INTERNAL;

		തിരികെ len;
	}

	ചെയ്യുക {
		സ്ഥിരം അക്ഷരം *path, *name, *fixup_end;
		സ്ഥിരം അക്ഷരം *fixup_str = മൂല്യം;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		അക്ഷരം *sep, *endptr;
		പൂർണ്ണസംഖ്യ poffset, ret;

		fixup_end = memchr(മൂല്യം, '\0', len);
		എങ്കിൽ (!fixup_end)
			തിരികെ -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		മൂല്യം += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		എങ്കിൽ (!sep || *sep != ':')
			തിരികെ -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		എങ്കിൽ (path_len == (fixup_len - 1))
			തിരികെ -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		എങ്കിൽ (!sep || *sep != ':')
			തിരികെ -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		എങ്കിൽ (!name_len)
			തിരികെ -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		എങ്കിൽ ((*endptr != '\0') || (endptr <= (sep + 1)))
			തിരികെ -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		എങ്കിൽ (ret)
			തിരികെ ret;
	} ആകുമ്പോൾ (len > 0);

	തിരികെ 0;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_fixup_phandles(ശൂന്യം *fdt, ശൂന്യം *fdto)
{
	പൂർണ്ണസംഖ്യ fixups_off, symbols_off;
	പൂർണ്ണസംഖ്യ property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	എങ്കിൽ (fixups_off == -FDT_ERR_NOTFOUND)
		തിരികെ 0; /* nothing to do */
	എങ്കിൽ (fixups_off < 0)
		തിരികെ fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	എങ്കിൽ ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		തിരികെ symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		പൂർണ്ണസംഖ്യ ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_apply_കണ്ണി(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ target,
			      ശൂന്യം *fdto, പൂർണ്ണസംഖ്യ കണ്ണി)
{
	പൂർണ്ണസംഖ്യ property;
	പൂർണ്ണസംഖ്യ subnode;

	fdt_for_each_property_offset(property, fdto, കണ്ണി) {
		സ്ഥിരം അക്ഷരം *name;
		സ്ഥിരം ശൂന്യം *prop;
		പൂർണ്ണസംഖ്യ prop_len;
		പൂർണ്ണസംഖ്യ ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		എങ്കിൽ (prop_len == -FDT_ERR_NOTFOUND)
			തിരികെ -FDT_ERR_INTERNAL;
		എങ്കിൽ (prop_len < 0)
			തിരികെ prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	fdt_for_each_subnode(subnode, fdto, കണ്ണി) {
		സ്ഥിരം അക്ഷരം *name = fdt_get_name(fdto, subnode, ശൂന്യ);
		പൂർണ്ണസംഖ്യ nnode;
		പൂർണ്ണസംഖ്യ ret;

		nnode = fdt_add_subnode(fdt, target, name);
		എങ്കിൽ (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			എങ്കിൽ (nnode == -FDT_ERR_NOTFOUND)
				തിരികെ -FDT_ERR_INTERNAL;
		}

		എങ്കിൽ (nnode < 0)
			തിരികെ nnode;

		ret = overlay_apply_കണ്ണി(fdt, nnode, fdto, subnode);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_merge(ശൂന്യം *fdt, ശൂന്യം *fdto)
{
	പൂർണ്ണസംഖ്യ fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		പൂർണ്ണസംഖ്യ overlay;
		പൂർണ്ണസംഖ്യ target;
		പൂർണ്ണസംഖ്യ ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		എങ്കിൽ (overlay == -FDT_ERR_NOTFOUND)
			തുടരുക;

		എങ്കിൽ (overlay < 0)
			തിരികെ overlay;

		target = overlay_get_target(fdt, fdto, fragment, ശൂന്യ);
		എങ്കിൽ (target < 0)
			തിരികെ target;

		ret = overlay_apply_കണ്ണി(fdt, target, fdto, overlay);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ get_path_len(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	പൂർണ്ണസംഖ്യ len = 0, namelen;
	സ്ഥിരം അക്ഷരം *name;

	FDT_RO_PROBE(fdt);

	വേണ്ടി (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		എങ്കിൽ (!name)
			തിരികെ namelen;

		/* root? we're done */
		എങ്കിൽ (namelen == 0)
			നിർത്തുക;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		എങ്കിൽ (nodeoffset < 0)
			തിരികെ nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	എങ്കിൽ (len == 0)
		len++;
	തിരികെ len;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ overlay_ചിഹ്നം_update(ശൂന്യം *fdt, ശൂന്യം *fdto)
{
	പൂർണ്ണസംഖ്യ വേര്_sym, ov_sym, prop, path_len, fragment, target;
	പൂർണ്ണസംഖ്യ len, frag_name_len, ret, rel_path_len;
	സ്ഥിരം അക്ഷരം *s, *e;
	സ്ഥിരം അക്ഷരം *path;
	സ്ഥിരം അക്ഷരം *name;
	സ്ഥിരം അക്ഷരം *frag_name;
	സ്ഥിരം അക്ഷരം *rel_path;
	സ്ഥിരം അക്ഷരം *target_path;
	അക്ഷരം *buf;
	ശൂന്യം *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	എങ്കിൽ (ov_sym < 0)
		തിരികെ 0;

	വേര്_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	എങ്കിൽ (വേര്_sym == -FDT_ERR_NOTFOUND)
		വേര്_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	എങ്കിൽ (വേര്_sym < 0)
		തിരികെ വേര്_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		എങ്കിൽ (!path)
			തിരികെ path_len;

		/* verify it's a string property (terminated by a single \0) */
		എങ്കിൽ (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			തിരികെ -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		എങ്കിൽ (*path != '/')
			തിരികെ -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		എങ്കിൽ (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			തുടരുക;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = വലുപ്പം("/__overlay__/") - 1;
		എങ്കിൽ ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} അല്ലെങ്കിൽ എങ്കിൽ ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} അല്ലെങ്കിൽ {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			തുടരുക;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		എങ്കിൽ (ret < 0)
			തിരികെ -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		എങ്കിൽ (ret < 0)
			തിരികെ -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		എങ്കിൽ (ret < 0)
			തിരികെ ret;
		target = ret;

		/* if we have a target path use */
		എങ്കിൽ (!target_path) {
			ret = get_path_len(fdt, target);
			എങ്കിൽ (ret < 0)
				തിരികെ ret;
			len = ret;
		} അല്ലെങ്കിൽ {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, വേര്_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		എങ്കിൽ (ret < 0)
			തിരികെ ret;

		എങ്കിൽ (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			എങ്കിൽ (ret < 0)
				തിരികെ ret;
			target = ret;
		}

		buf = p;
		എങ്കിൽ (len > 1) { /* target is not root */
			എങ്കിൽ (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				എങ്കിൽ (ret < 0)
					തിരികെ ret;
			} അല്ലെങ്കിൽ
				memcpy(buf, target_path, len + 1);

		} അല്ലെങ്കിൽ
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_overlay_apply(ശൂന്യം *fdt, ശൂന്യം *fdto)
{
	uint32_t delta;
	പൂർണ്ണസംഖ്യ ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	എങ്കിൽ (ret)
		പോകുക err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	എങ്കിൽ (ret)
		പോകുക err;

	ret = overlay_update_local_references(fdto, delta);
	എങ്കിൽ (ret)
		പോകുക err;

	ret = overlay_fixup_phandles(fdt, fdto);
	എങ്കിൽ (ret)
		പോകുക err;

	ret = overlay_merge(fdt, fdto);
	എങ്കിൽ (ret)
		പോകുക err;

	ret = overlay_ചിഹ്നം_update(fdt, fdto);
	എങ്കിൽ (ret)
		പോകുക err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	തിരികെ 0;

err:
	/*
	 * The overlay might have been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	/*
	 * The base device tree might have been damaged, erase its
	 * magic.
	 */
	fdt_set_magic(fdt, ~0);

	തിരികെ ret;
}
