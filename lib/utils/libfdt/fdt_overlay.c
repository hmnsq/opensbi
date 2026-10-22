// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

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
ಸ್ಥಿರ uint32_t overlay_get_target_phandle(ಧ್ರುವ ಶೂನ್ಯ *fdto, ಪೂರ್ಣಾಙ್ಕ fragment)
{
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	ಯದಿ (!val)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ ((len != ಪರಿಮಾಣ(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		ಪ್ರತ್ಯಾಗಮ (uint32_t)-1;

	ಪ್ರತ್ಯಾಗಮ fdt32_to_cpu(*val);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_get_target(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಧ್ರುವ ಶೂನ್ಯ *fdto,
			      ಪೂರ್ಣಾಙ್ಕ fragment, ವರ್ಣ ಧ್ರುವ **pathp)
{
	uint32_t phandle;
	ಧ್ರುವ ವರ್ಣ *path = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	ಯದಿ (phandle == (uint32_t)-1)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	ಯದಿ (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		ಯದಿ (path)
			ret = fdt_path_offset(fdt, path);
		ಅನ್ಯಥಾ
			ret = path_len;
	} ಅನ್ಯಥಾ
		ret = fdt_ಗಂಟು_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	ಯದಿ (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	ಯದಿ (ret < 0)
		ಪ್ರತ್ಯಾಗಮ ret;

	/* return pointer to path (if available) */
	ಯದಿ (pathp)
		*pathp = path ? path : ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ ret;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_phandle_add_offset(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ ಗಂಟು,
				      ಧ್ರುವ ವರ್ಣ *name, uint32_t delta)
{
	ಧ್ರುವ fdt32_t *val;
	uint32_t adj_val;
	ಪೂರ್ಣಾಙ್ಕ len;

	val = fdt_getprop(fdt, ಗಂಟು, name, &len);
	ಯದಿ (!val)
		ಪ್ರತ್ಯಾಗಮ len;

	ಯದಿ (len != ಪರಿಮಾಣ(*val))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	ಯದಿ ((adj_val + delta) < adj_val)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	ಯದಿ (adj_val == (uint32_t)-1)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOPHANDLES;

	ಪ್ರತ್ಯಾಗಮ fdt_setprop_inplace_u32(fdt, ಗಂಟು, name, adj_val);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_adjust_ಗಂಟು_phandles(ಶೂನ್ಯ *fdto, ಪೂರ್ಣಾಙ್ಕ ಗಂಟು,
					uint32_t delta)
{
	ಪೂರ್ಣಾಙ್ಕ child;
	ಪೂರ್ಣಾಙ್ಕ ret;

	ret = overlay_phandle_add_offset(fdto, ಗಂಟು, "phandle", delta);
	ಯದಿ (ret && ret != -FDT_ERR_NOTFOUND)
		ಪ್ರತ್ಯಾಗಮ ret;

	ret = overlay_phandle_add_offset(fdto, ಗಂಟು, "linux,phandle", delta);
	ಯದಿ (ret && ret != -FDT_ERR_NOTFOUND)
		ಪ್ರತ್ಯಾಗಮ ret;

	fdt_for_each_subnode(child, fdto, ಗಂಟು) {
		ret = overlay_adjust_ಗಂಟು_phandles(fdto, child, delta);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_adjust_local_phandles(ಶೂನ್ಯ *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	ಪ್ರತ್ಯಾಗಮ overlay_adjust_ಗಂಟು_phandles(fdto, 0, delta);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_update_local_ಗಂಟು_references(ಶೂನ್ಯ *fdto,
						ಪೂರ್ಣಾಙ್ಕ ವೃಕ್ಷ_ಗಂಟು,
						ಪೂರ್ಣಾಙ್ಕ fixup_ಗಂಟು,
						uint32_t delta)
{
	ಪೂರ್ಣಾಙ್ಕ fixup_prop;
	ಪೂರ್ಣಾಙ್ಕ fixup_child;
	ಪೂರ್ಣಾಙ್ಕ ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_ಗಂಟು) {
		ಧ್ರುವ fdt32_t *fixup_val;
		ಧ್ರುವ ವರ್ಣ *ವೃಕ್ಷ_val;
		ಧ್ರುವ ವರ್ಣ *name;
		ಪೂರ್ಣಾಙ್ಕ fixup_len;
		ಪೂರ್ಣಾಙ್ಕ ವೃಕ್ಷ_len;
		ಪೂರ್ಣಾಙ್ಕ i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		ಯದಿ (!fixup_val)
			ಪ್ರತ್ಯಾಗಮ fixup_len;

		ಯದಿ (fixup_len % ಪರಿಮಾಣ(uint32_t))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;
		fixup_len /= ಪರಿಮಾಣ(uint32_t);

		ವೃಕ್ಷ_val = fdt_getprop(fdto, ವೃಕ್ಷ_ಗಂಟು, name, &ವೃಕ್ಷ_len);
		ಯದಿ (!ವೃಕ್ಷ_val) {
			ಯದಿ (ವೃಕ್ಷ_len == -FDT_ERR_NOTFOUND)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;

			ಪ್ರತ್ಯಾಗಮ ವೃಕ್ಷ_len;
		}

		ಪ್ರತಿ (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, ವೃಕ್ಷ_val + poffset, ಪರಿಮಾಣ(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  ವೃಕ್ಷ_ಗಂಟು,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  ಪರಿಮಾಣ(adj_val));
			ಯದಿ (ret == -FDT_ERR_NOSPACE)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;

			ಯದಿ (ret)
				ಪ್ರತ್ಯಾಗಮ ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_ಗಂಟು) {
		ಧ್ರುವ ವರ್ಣ *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    ರಿಕ್ತ);
		ಪೂರ್ಣಾಙ್ಕ ವೃಕ್ಷ_child;

		ವೃಕ್ಷ_child = fdt_subnode_offset(fdto, ವೃಕ್ಷ_ಗಂಟು,
						fixup_child_name);
		ಯದಿ (ವೃಕ್ಷ_child == -FDT_ERR_NOTFOUND)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;
		ಯದಿ (ವೃಕ್ಷ_child < 0)
			ಪ್ರತ್ಯಾಗಮ ವೃಕ್ಷ_child;

		ret = overlay_update_local_ಗಂಟು_references(fdto,
							   ವೃಕ್ಷ_child,
							   fixup_child,
							   delta);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_update_local_references(ಶೂನ್ಯ *fdto, uint32_t delta)
{
	ಪೂರ್ಣಾಙ್ಕ fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	ಯದಿ (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		ಯದಿ (fixups == -FDT_ERR_NOTFOUND)
			ಪ್ರತ್ಯಾಗಮ 0;

		ಪ್ರತ್ಯಾಗಮ fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	ಪ್ರತ್ಯಾಗಮ overlay_update_local_ಗಂಟು_references(fdto, 0, fixups,
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_fixup_one_phandle(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto,
				     ಪೂರ್ಣಾಙ್ಕ symbols_off,
				     ಧ್ರುವ ವರ್ಣ *path, uint32_t path_len,
				     ಧ್ರುವ ವರ್ಣ *name, uint32_t name_len,
				     ಪೂರ್ಣಾಙ್ಕ poffset, ಧ್ರುವ ವರ್ಣ *label)
{
	ಧ್ರುವ ವರ್ಣ *ಚಿಹ್ನೆ_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	ಪೂರ್ಣಾಙ್ಕ ಚಿಹ್ನೆ_off, fixup_off;
	ಪೂರ್ಣಾಙ್ಕ prop_len;

	ಯದಿ (symbols_off < 0)
		ಪ್ರತ್ಯಾಗಮ symbols_off;

	ಚಿಹ್ನೆ_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	ಯದಿ (!ಚಿಹ್ನೆ_path)
		ಪ್ರತ್ಯಾಗಮ prop_len;

	ಚಿಹ್ನೆ_off = fdt_path_offset(fdt, ಚಿಹ್ನೆ_path);
	ಯದಿ (ಚಿಹ್ನೆ_off < 0)
		ಪ್ರತ್ಯಾಗಮ ಚಿಹ್ನೆ_off;

	phandle = fdt_get_phandle(fdt, ಚಿಹ್ನೆ_off);
	ಯದಿ (!phandle)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	ಯದಿ (fixup_off == -FDT_ERR_NOTFOUND)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;
	ಯದಿ (fixup_off < 0)
		ಪ್ರತ್ಯಾಗಮ fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	ಪ್ರತ್ಯಾಗಮ fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   ಪರಿಮಾಣ(phandle_prop));
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_fixup_phandle(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto, ಪೂರ್ಣಾಙ್ಕ symbols_off,
				 ಪೂರ್ಣಾಙ್ಕ property)
{
	ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;
	ಧ್ರುವ ವರ್ಣ *label;
	ಪೂರ್ಣಾಙ್ಕ len;

	ಮೌಲ್ಯ = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	ಯದಿ (!ಮೌಲ್ಯ) {
		ಯದಿ (len == -FDT_ERR_NOTFOUND)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_INTERNAL;

		ಪ್ರತ್ಯಾಗಮ len;
	}

	ಕುರು {
		ಧ್ರುವ ವರ್ಣ *path, *name, *fixup_end;
		ಧ್ರುವ ವರ್ಣ *fixup_str = ಮೌಲ್ಯ;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		ವರ್ಣ *sep, *endptr;
		ಪೂರ್ಣಾಙ್ಕ poffset, ret;

		fixup_end = memchr(ಮೌಲ್ಯ, '\0', len);
		ಯದಿ (!fixup_end)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		ಮೌಲ್ಯ += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		ಯದಿ (!sep || *sep != ':')
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		ಯದಿ (path_len == (fixup_len - 1))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		ಯದಿ (!sep || *sep != ':')
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		ಯದಿ (!name_len)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		ಯದಿ ((*endptr != '\0') || (endptr <= (sep + 1)))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	} ಯಾವತ್ (len > 0);

	ಪ್ರತ್ಯಾಗಮ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_fixup_phandles(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto)
{
	ಪೂರ್ಣಾಙ್ಕ fixups_off, symbols_off;
	ಪೂರ್ಣಾಙ್ಕ property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	ಯದಿ (fixups_off == -FDT_ERR_NOTFOUND)
		ಪ್ರತ್ಯಾಗಮ 0; /* nothing to do */
	ಯದಿ (fixups_off < 0)
		ಪ್ರತ್ಯಾಗಮ fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	ಯದಿ ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		ಪ್ರತ್ಯಾಗಮ symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		ಪೂರ್ಣಾಙ್ಕ ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_apply_ಗಂಟು(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ target,
			      ಶೂನ್ಯ *fdto, ಪೂರ್ಣಾಙ್ಕ ಗಂಟು)
{
	ಪೂರ್ಣಾಙ್ಕ property;
	ಪೂರ್ಣಾಙ್ಕ subnode;

	fdt_for_each_property_offset(property, fdto, ಗಂಟು) {
		ಧ್ರುವ ವರ್ಣ *name;
		ಧ್ರುವ ಶೂನ್ಯ *prop;
		ಪೂರ್ಣಾಙ್ಕ prop_len;
		ಪೂರ್ಣಾಙ್ಕ ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		ಯದಿ (prop_len == -FDT_ERR_NOTFOUND)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_INTERNAL;
		ಯದಿ (prop_len < 0)
			ಪ್ರತ್ಯಾಗಮ prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	fdt_for_each_subnode(subnode, fdto, ಗಂಟು) {
		ಧ್ರುವ ವರ್ಣ *name = fdt_get_name(fdto, subnode, ರಿಕ್ತ);
		ಪೂರ್ಣಾಙ್ಕ nnode;
		ಪೂರ್ಣಾಙ್ಕ ret;

		nnode = fdt_add_subnode(fdt, target, name);
		ಯದಿ (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			ಯದಿ (nnode == -FDT_ERR_NOTFOUND)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_INTERNAL;
		}

		ಯದಿ (nnode < 0)
			ಪ್ರತ್ಯಾಗಮ nnode;

		ret = overlay_apply_ಗಂಟು(fdt, nnode, fdto, subnode);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_merge(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto)
{
	ಪೂರ್ಣಾಙ್ಕ fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		ಪೂರ್ಣಾಙ್ಕ overlay;
		ಪೂರ್ಣಾಙ್ಕ target;
		ಪೂರ್ಣಾಙ್ಕ ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		ಯದಿ (overlay == -FDT_ERR_NOTFOUND)
			ಅನುವರ್ತ;

		ಯದಿ (overlay < 0)
			ಪ್ರತ್ಯಾಗಮ overlay;

		target = overlay_get_target(fdt, fdto, fragment, ರಿಕ್ತ);
		ಯದಿ (target < 0)
			ಪ್ರತ್ಯಾಗಮ target;

		ret = overlay_apply_ಗಂಟು(fdt, target, fdto, overlay);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_path_len(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಪೂರ್ಣಾಙ್ಕ len = 0, namelen;
	ಧ್ರುವ ವರ್ಣ *name;

	FDT_RO_PROBE(fdt);

	ಪ್ರತಿ (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		ಯದಿ (!name)
			ಪ್ರತ್ಯಾಗಮ namelen;

		/* root? we're done */
		ಯದಿ (namelen == 0)
			ಭಙ್ಗ;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		ಯದಿ (nodeoffset < 0)
			ಪ್ರತ್ಯಾಗಮ nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	ಯದಿ (len == 0)
		len++;
	ಪ್ರತ್ಯಾಗಮ len;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ overlay_ಚಿಹ್ನೆ_update(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto)
{
	ಪೂರ್ಣಾಙ್ಕ ಬೇರು_sym, ov_sym, prop, path_len, fragment, target;
	ಪೂರ್ಣಾಙ್ಕ len, frag_name_len, ret, rel_path_len;
	ಧ್ರುವ ವರ್ಣ *s, *e;
	ಧ್ರುವ ವರ್ಣ *path;
	ಧ್ರುವ ವರ್ಣ *name;
	ಧ್ರುವ ವರ್ಣ *frag_name;
	ಧ್ರುವ ವರ್ಣ *rel_path;
	ಧ್ರುವ ವರ್ಣ *target_path;
	ವರ್ಣ *buf;
	ಶೂನ್ಯ *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	ಯದಿ (ov_sym < 0)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಬೇರು_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	ಯದಿ (ಬೇರು_sym == -FDT_ERR_NOTFOUND)
		ಬೇರು_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	ಯದಿ (ಬೇರು_sym < 0)
		ಪ್ರತ್ಯಾಗಮ ಬೇರು_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		ಯದಿ (!path)
			ಪ್ರತ್ಯಾಗಮ path_len;

		/* verify it's a string property (terminated by a single \0) */
		ಯದಿ (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		ಯದಿ (*path != '/')
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		ಯದಿ (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ಅನುವರ್ತ;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = ಪರಿಮಾಣ("/__overlay__/") - 1;
		ಯದಿ ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} ಅನ್ಯಥಾ ಯದಿ ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} ಅನ್ಯಥಾ {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ಅನುವರ್ತ;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		ಯದಿ (ret < 0)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		ಯದಿ (ret < 0)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		ಯದಿ (ret < 0)
			ಪ್ರತ್ಯಾಗಮ ret;
		target = ret;

		/* if we have a target path use */
		ಯದಿ (!target_path) {
			ret = get_path_len(fdt, target);
			ಯದಿ (ret < 0)
				ಪ್ರತ್ಯಾಗಮ ret;
			len = ret;
		} ಅನ್ಯಥಾ {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, ಬೇರು_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		ಯದಿ (ret < 0)
			ಪ್ರತ್ಯಾಗಮ ret;

		ಯದಿ (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			ಯದಿ (ret < 0)
				ಪ್ರತ್ಯಾಗಮ ret;
			target = ret;
		}

		buf = p;
		ಯದಿ (len > 1) { /* target is not root */
			ಯದಿ (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				ಯದಿ (ret < 0)
					ಪ್ರತ್ಯಾಗಮ ret;
			} ಅನ್ಯಥಾ
				memcpy(buf, target_path, len + 1);

		} ಅನ್ಯಥಾ
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_overlay_apply(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto)
{
	uint32_t delta;
	ಪೂರ್ಣಾಙ್ಕ ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	ಯದಿ (ret)
		ಗಚ್ಛ err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	ಯದಿ (ret)
		ಗಚ್ಛ err;

	ret = overlay_update_local_references(fdto, delta);
	ಯದಿ (ret)
		ಗಚ್ಛ err;

	ret = overlay_fixup_phandles(fdt, fdto);
	ಯದಿ (ret)
		ಗಚ್ಛ err;

	ret = overlay_merge(fdt, fdto);
	ಯದಿ (ret)
		ಗಚ್ಛ err;

	ret = overlay_ಚಿಹ್ನೆ_update(fdt, fdto);
	ಯದಿ (ret)
		ಗಚ್ಛ err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	ಪ್ರತ್ಯಾಗಮ 0;

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

	ಪ್ರತ್ಯಾಗಮ ret;
}
