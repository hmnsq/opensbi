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
ಸ್ಥಿರ uint32_t overlay_get_ಗುರಿ_phandle(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdto, ಪೂರ್ಣಾಂಕ fragment)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	ಆದರೆ (!val)
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ ((len != ಗಾತ್ರ(*val)) || (fdt32_ಗಮ್ಯ_cpu(*val) == (uint32_t)-1))
		ಹಿಂತಿರುಗಿ (uint32_t)-1;

	ಹಿಂತಿರುಗಿ fdt32_ಗಮ್ಯ_cpu(*val);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_get_ಗುರಿ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdto,
			      ಪೂರ್ಣಾಂಕ fragment, ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ **pathp)
{
	uint32_t phandle;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_ಗುರಿ_phandle(fdto, fragment);
	ಆದರೆ (phandle == (uint32_t)-1)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	ಆದರೆ (!phandle) {
		/* And then a path based lookup */
		ಮಾರ್ಗ = fdt_getprop(fdto, fragment, "target-path", &ಮಾರ್ಗ_len);
		ಆದರೆ (ಮಾರ್ಗ)
			ret = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, ಮಾರ್ಗ);
		ಇಲ್ಲದಿದ್ದರೆ
			ret = ಮಾರ್ಗ_len;
	} ಇಲ್ಲದಿದ್ದರೆ
		ret = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	ಆದರೆ (ret < 0 && ಮಾರ್ಗ_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	ಆದರೆ (ret < 0)
		ಹಿಂತಿರುಗಿ ret;

	/* return pointer to path (if available) */
	ಆದರೆ (pathp)
		*pathp = ಮಾರ್ಗ ? ಮಾರ್ಗ : ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ ret;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_phandle_ಕೂಡಿಸು_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಗಂಟು,
				      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, uint32_t delta)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	uint32_t adj_val;
	ಪೂರ್ಣಾಂಕ len;

	val = fdt_getprop(fdt, ಗಂಟು, name, &len);
	ಆದರೆ (!val)
		ಹಿಂತಿರುಗಿ len;

	ಆದರೆ (len != ಗಾತ್ರ(*val))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_ಗಮ್ಯ_cpu(*val);
	ಆದರೆ ((adj_val + delta) < adj_val)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	ಆದರೆ (adj_val == (uint32_t)-1)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOPHANDLES;

	ಹಿಂತಿರುಗಿ fdt_setprop_inplace_u32(fdt, ಗಂಟು, name, adj_val);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_adjust_ಗಂಟು_phandles(ಶೂನ್ಯ *fdto, ಪೂರ್ಣಾಂಕ ಗಂಟು,
					uint32_t delta)
{
	ಪೂರ್ಣಾಂಕ ಮಗು;
	ಪೂರ್ಣಾಂಕ ret;

	ret = overlay_phandle_ಕೂಡಿಸು_ಸ್ಥಾನಾಂತರ(fdto, ಗಂಟು, "phandle", delta);
	ಆದರೆ (ret && ret != -FDT_ERR_NOTFOUND)
		ಹಿಂತಿರುಗಿ ret;

	ret = overlay_phandle_ಕೂಡಿಸು_ಸ್ಥಾನಾಂತರ(fdto, ಗಂಟು, "linux,phandle", delta);
	ಆದರೆ (ret && ret != -FDT_ERR_NOTFOUND)
		ಹಿಂತಿರುಗಿ ret;

	fdt_for_each_subnode(ಮಗು, fdto, ಗಂಟು) {
		ret = overlay_adjust_ಗಂಟು_phandles(fdto, ಮಗು, delta);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_adjust_local_phandles(ಶೂನ್ಯ *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	ಹಿಂತಿರುಗಿ overlay_adjust_ಗಂಟು_phandles(fdto, 0, delta);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_update_local_ಗಂಟು_references(ಶೂನ್ಯ *fdto,
						ಪೂರ್ಣಾಂಕ ವೃಕ್ಷ_ಗಂಟು,
						ಪೂರ್ಣಾಂಕ fixup_ಗಂಟು,
						uint32_t delta)
{
	ಪೂರ್ಣಾಂಕ fixup_prop;
	ಪೂರ್ಣಾಂಕ fixup_ಮಗು;
	ಪೂರ್ಣಾಂಕ ret;

	fdt_for_each_property_ಸ್ಥಾನಾಂತರ(fixup_prop, fdto, fixup_ಗಂಟು) {
		ಸ್ಥಿರಾಂಕ fdt32_t *fixup_val;
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವೃಕ್ಷ_val;
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
		ಪೂರ್ಣಾಂಕ fixup_len;
		ಪೂರ್ಣಾಂಕ ವೃಕ್ಷ_len;
		ಪೂರ್ಣಾಂಕ i;

		fixup_val = fdt_getprop_by_ಸ್ಥಾನಾಂತರ(fdto, fixup_prop,
						  &name, &fixup_len);
		ಆದರೆ (!fixup_val)
			ಹಿಂತಿರುಗಿ fixup_len;

		ಆದರೆ (fixup_len % ಗಾತ್ರ(uint32_t))
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;
		fixup_len /= ಗಾತ್ರ(uint32_t);

		ವೃಕ್ಷ_val = fdt_getprop(fdto, ವೃಕ್ಷ_ಗಂಟು, name, &ವೃಕ್ಷ_len);
		ಆದರೆ (!ವೃಕ್ಷ_val) {
			ಆದರೆ (ವೃಕ್ಷ_len == -FDT_ERR_NOTFOUND)
				ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;

			ಹಿಂತಿರುಗಿ ವೃಕ್ಷ_len;
		}

		ಗಾಗಿ (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_ಗಮ್ಯ_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			ಸ್ಮರಣೆ_ನಕಲು(&adj_val, ವೃಕ್ಷ_val + poffset, ಗಾತ್ರ(adj_val));

			adj_val = cpu_ಗಮ್ಯ_fdt32(fdt32_ಗಮ್ಯ_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  ವೃಕ್ಷ_ಗಂಟು,
								  name,
								  ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name),
								  poffset,
								  &adj_val,
								  ಗಾತ್ರ(adj_val));
			ಆದರೆ (ret == -FDT_ERR_NOSPACE)
				ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;

			ಆದರೆ (ret)
				ಹಿಂತಿರುಗಿ ret;
		}
	}

	fdt_for_each_subnode(fixup_ಮಗು, fdto, fixup_ಗಂಟು) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fixup_ಮಗು_name = fdt_get_name(fdto, fixup_ಮಗು,
							    ಖಾಲಿ);
		ಪೂರ್ಣಾಂಕ ವೃಕ್ಷ_ಮಗು;

		ವೃಕ್ಷ_ಮಗು = fdt_subnode_ಸ್ಥಾನಾಂತರ(fdto, ವೃಕ್ಷ_ಗಂಟು,
						fixup_ಮಗು_name);
		ಆದರೆ (ವೃಕ್ಷ_ಮಗು == -FDT_ERR_NOTFOUND)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;
		ಆದರೆ (ವೃಕ್ಷ_ಮಗು < 0)
			ಹಿಂತಿರುಗಿ ವೃಕ್ಷ_ಮಗು;

		ret = overlay_update_local_ಗಂಟು_references(fdto,
							   ವೃಕ್ಷ_ಮಗು,
							   fixup_ಮಗು,
							   delta);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_update_local_references(ಶೂನ್ಯ *fdto, uint32_t delta)
{
	ಪೂರ್ಣಾಂಕ fixups;

	fixups = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdto, "/__local_fixups__");
	ಆದರೆ (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		ಆದರೆ (fixups == -FDT_ERR_NOTFOUND)
			ಹಿಂತಿರುಗಿ 0;

		ಹಿಂತಿರುಗಿ fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	ಹಿಂತಿರುಗಿ overlay_update_local_ಗಂಟು_references(fdto, 0, fixups,
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_fixup_one_phandle(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto,
				     ಪೂರ್ಣಾಂಕ symbols_off,
				     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uint32_t ಮಾರ್ಗ_len,
				     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, uint32_t name_len,
				     ಪೂರ್ಣಾಂಕ poffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಚಿಹ್ನೆ_ಮಾರ್ಗ;
	uint32_t phandle;
	fdt32_t phandle_prop;
	ಪೂರ್ಣಾಂಕ ಚಿಹ್ನೆ_off, fixup_off;
	ಪೂರ್ಣಾಂಕ prop_len;

	ಆದರೆ (symbols_off < 0)
		ಹಿಂತಿರುಗಿ symbols_off;

	ಚಿಹ್ನೆ_ಮಾರ್ಗ = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	ಆದರೆ (!ಚಿಹ್ನೆ_ಮಾರ್ಗ)
		ಹಿಂತಿರುಗಿ prop_len;

	ಚಿಹ್ನೆ_off = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, ಚಿಹ್ನೆ_ಮಾರ್ಗ);
	ಆದರೆ (ಚಿಹ್ನೆ_off < 0)
		ಹಿಂತಿರುಗಿ ಚಿಹ್ನೆ_off;

	phandle = fdt_get_phandle(fdt, ಚಿಹ್ನೆ_off);
	ಆದರೆ (!phandle)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;

	fixup_off = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ_namelen(fdto, ಮಾರ್ಗ, ಮಾರ್ಗ_len);
	ಆದರೆ (fixup_off == -FDT_ERR_NOTFOUND)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;
	ಆದರೆ (fixup_off < 0)
		ಹಿಂತಿರುಗಿ fixup_off;

	phandle_prop = cpu_ಗಮ್ಯ_fdt32(phandle);
	ಹಿಂತಿರುಗಿ fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   ಗಾತ್ರ(phandle_prop));
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_fixup_phandle(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto, ಪೂರ್ಣಾಂಕ symbols_off,
				 ಪೂರ್ಣಾಂಕ property)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label;
	ಪೂರ್ಣಾಂಕ len;

	ಮೌಲ್ಯ = fdt_getprop_by_ಸ್ಥಾನಾಂತರ(fdto, property,
				      &label, &len);
	ಆದರೆ (!ಮೌಲ್ಯ) {
		ಆದರೆ (len == -FDT_ERR_NOTFOUND)
			ಹಿಂತಿರುಗಿ -FDT_ERR_ಆಂತರಿಕ;

		ಹಿಂತಿರುಗಿ len;
	}

	ಮಾಡು {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, *name, *fixup_ಕೊನೆ;
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fixup_str = ಮೌಲ್ಯ;
		uint32_t ಮಾರ್ಗ_len, name_len;
		uint32_t fixup_len;
		ಅಕ್ಷರ *sep, *endptr;
		ಪೂರ್ಣಾಂಕ poffset, ret;

		fixup_ಕೊನೆ = memchr(ಮೌಲ್ಯ, '\0', len);
		ಆದರೆ (!fixup_ಕೊನೆ)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_ಕೊನೆ - fixup_str;

		len -= fixup_len + 1;
		ಮೌಲ್ಯ += fixup_len + 1;

		ಮಾರ್ಗ = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		ಆದರೆ (!sep || *sep != ':')
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;

		ಮಾರ್ಗ_len = sep - ಮಾರ್ಗ;
		ಆದರೆ (ಮಾರ್ಗ_len == (fixup_len - 1))
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;

		fixup_len -= ಮಾರ್ಗ_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		ಆದರೆ (!sep || *sep != ':')
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		ಆದರೆ (!name_len)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		ಆದರೆ ((*endptr != '\0') || (endptr <= (sep + 1)))
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						ಮಾರ್ಗ, ಮಾರ್ಗ_len, name, name_len,
						poffset, label);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	} ಆಗ (len > 0);

	ಹಿಂತಿರುಗಿ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_fixup_phandles(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto)
{
	ಪೂರ್ಣಾಂಕ fixups_off, symbols_off;
	ಪೂರ್ಣಾಂಕ property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdto, "/__fixups__");
	ಆದರೆ (fixups_off == -FDT_ERR_NOTFOUND)
		ಹಿಂತಿರುಗಿ 0; /* nothing to do */
	ಆದರೆ (fixups_off < 0)
		ಹಿಂತಿರುಗಿ fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/__symbols__");
	ಆದರೆ ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		ಹಿಂತಿರುಗಿ symbols_off;

	fdt_for_each_property_ಸ್ಥಾನಾಂತರ(property, fdto, fixups_off) {
		ಪೂರ್ಣಾಂಕ ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_apply_ಗಂಟು(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಗುರಿ,
			      ಶೂನ್ಯ *fdto, ಪೂರ್ಣಾಂಕ ಗಂಟು)
{
	ಪೂರ್ಣಾಂಕ property;
	ಪೂರ್ಣಾಂಕ subnode;

	fdt_for_each_property_ಸ್ಥಾನಾಂತರ(property, fdto, ಗಂಟು) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
		ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *prop;
		ಪೂರ್ಣಾಂಕ prop_len;
		ಪೂರ್ಣಾಂಕ ret;

		prop = fdt_getprop_by_ಸ್ಥಾನಾಂತರ(fdto, property, &name,
					     &prop_len);
		ಆದರೆ (prop_len == -FDT_ERR_NOTFOUND)
			ಹಿಂತಿರುಗಿ -FDT_ERR_ಆಂತರಿಕ;
		ಆದರೆ (prop_len < 0)
			ಹಿಂತಿರುಗಿ prop_len;

		ret = fdt_setprop(fdt, ಗುರಿ, name, prop, prop_len);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	fdt_for_each_subnode(subnode, fdto, ಗಂಟು) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = fdt_get_name(fdto, subnode, ಖಾಲಿ);
		ಪೂರ್ಣಾಂಕ nnode;
		ಪೂರ್ಣಾಂಕ ret;

		nnode = fdt_ಕೂಡಿಸು_subnode(fdt, ಗುರಿ, name);
		ಆದರೆ (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_ಸ್ಥಾನಾಂತರ(fdt, ಗುರಿ, name);
			ಆದರೆ (nnode == -FDT_ERR_NOTFOUND)
				ಹಿಂತಿರುಗಿ -FDT_ERR_ಆಂತರಿಕ;
		}

		ಆದರೆ (nnode < 0)
			ಹಿಂತಿರುಗಿ nnode;

		ret = overlay_apply_ಗಂಟು(fdt, nnode, fdto, subnode);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_ವಿಲೀನ(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto)
{
	ಪೂರ್ಣಾಂಕ fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		ಪೂರ್ಣಾಂಕ overlay;
		ಪೂರ್ಣಾಂಕ ಗುರಿ;
		ಪೂರ್ಣಾಂಕ ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_ಸ್ಥಾನಾಂತರ(fdto, fragment, "__overlay__");
		ಆದರೆ (overlay == -FDT_ERR_NOTFOUND)
			ಮುಂದುವರಿ;

		ಆದರೆ (overlay < 0)
			ಹಿಂತಿರುಗಿ overlay;

		ಗುರಿ = overlay_get_ಗುರಿ(fdt, fdto, fragment, ಖಾಲಿ);
		ಆದರೆ (ಗುರಿ < 0)
			ಹಿಂತಿರುಗಿ ಗುರಿ;

		ret = overlay_apply_ಗಂಟು(fdt, ಗುರಿ, fdto, overlay);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_ಮಾರ್ಗ_len(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಪೂರ್ಣಾಂಕ len = 0, namelen;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;

	FDT_RO_PROBE(fdt);

	ಗಾಗಿ (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		ಆದರೆ (!name)
			ಹಿಂತಿರುಗಿ namelen;

		/* root? we're done */
		ಆದರೆ (namelen == 0)
			ನಿಲ್ಲಿಸು;

		nodeoffset = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, nodeoffset);
		ಆದರೆ (nodeoffset < 0)
			ಹಿಂತಿರುಗಿ nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	ಆದರೆ (len == 0)
		len++;
	ಹಿಂತಿರುಗಿ len;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ overlay_ಚಿಹ್ನೆ_update(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto)
{
	ಪೂರ್ಣಾಂಕ ಬೇರು_sym, ov_sym, prop, ಮಾರ್ಗ_len, fragment, ಗುರಿ;
	ಪೂರ್ಣಾಂಕ len, frag_name_len, ret, rel_ಮಾರ್ಗ_len;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, *e;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *frag_name;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rel_ಮಾರ್ಗ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಮಾರ್ಗ;
	ಅಕ್ಷರ *buf;
	ಶೂನ್ಯ *p;

	ov_sym = fdt_subnode_ಸ್ಥಾನಾಂತರ(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	ಆದರೆ (ov_sym < 0)
		ಹಿಂತಿರುಗಿ 0;

	ಬೇರು_sym = fdt_subnode_ಸ್ಥಾನಾಂತರ(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	ಆದರೆ (ಬೇರು_sym == -FDT_ERR_NOTFOUND)
		ಬೇರು_sym = fdt_ಕೂಡಿಸು_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	ಆದರೆ (ಬೇರು_sym < 0)
		ಹಿಂತಿರುಗಿ ಬೇರು_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_ಸ್ಥಾನಾಂತರ(prop, fdto, ov_sym) {
		ಮಾರ್ಗ = fdt_getprop_by_ಸ್ಥಾನಾಂತರ(fdto, prop, &name, &ಮಾರ್ಗ_len);
		ಆದರೆ (!ಮಾರ್ಗ)
			ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_len;

		/* verify it's a string property (terminated by a single \0) */
		ಆದರೆ (ಮಾರ್ಗ_len < 1 || memchr(ಮಾರ್ಗ, '\0', ಮಾರ್ಗ_len) != &ಮಾರ್ಗ[ಮಾರ್ಗ_len - 1])
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = ಮಾರ್ಗ + ಮಾರ್ಗ_len;

		ಆದರೆ (*ಮಾರ್ಗ != '/')
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(ಮಾರ್ಗ + 1, '/');
		ಆದರೆ (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ಮುಂದುವರಿ;
		}

		frag_name = ಮಾರ್ಗ + 1;
		frag_name_len = s - ಮಾರ್ಗ - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = ಗಾತ್ರ("/__overlay__/") - 1;
		ಆದರೆ ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_ಮಾರ್ಗ = s + len;
			rel_ಮಾರ್ಗ_len = e - rel_ಮಾರ್ಗ - 1;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_ಮಾರ್ಗ = "";
			rel_ಮಾರ್ಗ_len = 0;
		} ಇಲ್ಲದಿದ್ದರೆ {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ಮುಂದುವರಿ;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_ಸ್ಥಾನಾಂತರ_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		ಆದರೆ (ret < 0)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_ಸ್ಥಾನಾಂತರ(fdto, fragment, "__overlay__");
		ಆದರೆ (ret < 0)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_ಗುರಿ(fdt, fdto, fragment, &ಗುರಿ_ಮಾರ್ಗ);
		ಆದರೆ (ret < 0)
			ಹಿಂತಿರುಗಿ ret;
		ಗುರಿ = ret;

		/* if we have a target path use */
		ಆದರೆ (!ಗುರಿ_ಮಾರ್ಗ) {
			ret = get_ಮಾರ್ಗ_len(fdt, ಗುರಿ);
			ಆದರೆ (ret < 0)
				ಹಿಂತಿರುಗಿ ret;
			len = ret;
		} ಇಲ್ಲದಿದ್ದರೆ {
			len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಗುರಿ_ಮಾರ್ಗ);
		}

		ret = fdt_setprop_placeholder(fdt, ಬೇರು_sym, name,
				len + (len > 1) + rel_ಮಾರ್ಗ_len + 1, &p);
		ಆದರೆ (ret < 0)
			ಹಿಂತಿರುಗಿ ret;

		ಆದರೆ (!ಗುರಿ_ಮಾರ್ಗ) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_ಗುರಿ(fdt, fdto, fragment, &ಗುರಿ_ಮಾರ್ಗ);
			ಆದರೆ (ret < 0)
				ಹಿಂತಿರುಗಿ ret;
			ಗುರಿ = ret;
		}

		buf = p;
		ಆದರೆ (len > 1) { /* target is not root */
			ಆದರೆ (!ಗುರಿ_ಮಾರ್ಗ) {
				ret = fdt_get_ಮಾರ್ಗ(fdt, ಗುರಿ, buf, len + 1);
				ಆದರೆ (ret < 0)
					ಹಿಂತಿರುಗಿ ret;
			} ಇಲ್ಲದಿದ್ದರೆ
				ಸ್ಮರಣೆ_ನಕಲು(buf, ಗುರಿ_ಮಾರ್ಗ, len + 1);

		} ಇಲ್ಲದಿದ್ದರೆ
			len--;

		buf[len] = '/';
		ಸ್ಮರಣೆ_ನಕಲು(buf + len + 1, rel_ಮಾರ್ಗ, rel_ಮಾರ್ಗ_len);
		buf[len + 1 + rel_ಮಾರ್ಗ_len] = '\0';
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_overlay_apply(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *fdto)
{
	uint32_t delta;
	ಪೂರ್ಣಾಂಕ ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_ಕಂಡುಹಿಡಿ_max_phandle(fdt, &delta);
	ಆದರೆ (ret)
		ಹೋಗು err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	ಆದರೆ (ret)
		ಹೋಗು err;

	ret = overlay_update_local_references(fdto, delta);
	ಆದರೆ (ret)
		ಹೋಗು err;

	ret = overlay_fixup_phandles(fdt, fdto);
	ಆದರೆ (ret)
		ಹೋಗು err;

	ret = overlay_ವಿಲೀನ(fdt, fdto);
	ಆದರೆ (ret)
		ಹೋಗು err;

	ret = overlay_ಚಿಹ್ನೆ_update(fdt, fdto);
	ಆದರೆ (ret)
		ಹೋಗು err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_ಗಣ_magic(fdto, ~0);

	ಹಿಂತಿರುಗಿ 0;

err:
	/*
	 * The overlay might have been damaged, erase its magic.
	 */
	fdt_ಗಣ_magic(fdto, ~0);

	/*
	 * The base device tree might have been damaged, erase its
	 * magic.
	 */
	fdt_ಗಣ_magic(fdt, ~0);

	ಹಿಂತಿರುಗಿ ret;
}
