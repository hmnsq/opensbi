// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

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
ਸਥਿਰ uint32_t overlay_get_ਨਿਸ਼ਾਨਾ_phandle(ਅਚਲ ਖਾਲੀ *fdto, ਪੂਰਨ fragment)
{
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	ਜੇ (!val)
		ਵਾਪਸ 0;

	ਜੇ ((len != ਆਕਾਰ(*val)) || (fdt32_ਮੰਜ਼ਿਲ_cpu(*val) == (uint32_t)-1))
		ਵਾਪਸ (uint32_t)-1;

	ਵਾਪਸ fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
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
ਸਥਿਰ ਪੂਰਨ overlay_get_ਨਿਸ਼ਾਨਾ(ਅਚਲ ਖਾਲੀ *fdt, ਅਚਲ ਖਾਲੀ *fdto,
			      ਪੂਰਨ fragment, ਅੱਖਰ ਅਚਲ **pathp)
{
	uint32_t phandle;
	ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;
	ਪੂਰਨ ਰਸਤਾ_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_ਨਿਸ਼ਾਨਾ_phandle(fdto, fragment);
	ਜੇ (phandle == (uint32_t)-1)
		ਵਾਪਸ -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	ਜੇ (!phandle) {
		/* And then a path based lookup */
		ਰਸਤਾ = fdt_getprop(fdto, fragment, "target-path", &ਰਸਤਾ_len);
		ਜੇ (ਰਸਤਾ)
			ret = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, ਰਸਤਾ);
		ਨਹੀਂਤਾਂ
			ret = ਰਸਤਾ_len;
	} ਨਹੀਂਤਾਂ
		ret = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	ਜੇ (ret < 0 && ਰਸਤਾ_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	ਜੇ (ret < 0)
		ਵਾਪਸ ret;

	/* return pointer to path (if available) */
	ਜੇ (pathp)
		*pathp = ਰਸਤਾ ? ਰਸਤਾ : ਨਿਰਾ;

	ਵਾਪਸ ret;
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
ਸਥਿਰ ਪੂਰਨ overlay_phandle_ਜੋੜੋ_ਵਿੱਥ(ਖਾਲੀ *fdt, ਪੂਰਨ ਗੰਢ,
				      ਅਚਲ ਅੱਖਰ *name, uint32_t delta)
{
	ਅਚਲ fdt32_t *val;
	uint32_t adj_val;
	ਪੂਰਨ len;

	val = fdt_getprop(fdt, ਗੰਢ, name, &len);
	ਜੇ (!val)
		ਵਾਪਸ len;

	ਜੇ (len != ਆਕਾਰ(*val))
		ਵਾਪਸ -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਜੇ ((adj_val + delta) < adj_val)
		ਵਾਪਸ -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	ਜੇ (adj_val == (uint32_t)-1)
		ਵਾਪਸ -FDT_ERR_NOPHANDLES;

	ਵਾਪਸ fdt_setprop_inplace_u32(fdt, ਗੰਢ, name, adj_val);
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
ਸਥਿਰ ਪੂਰਨ overlay_adjust_ਗੰਢ_phandles(ਖਾਲੀ *fdto, ਪੂਰਨ ਗੰਢ,
					uint32_t delta)
{
	ਪੂਰਨ ਬੱਚਾ;
	ਪੂਰਨ ret;

	ret = overlay_phandle_ਜੋੜੋ_ਵਿੱਥ(fdto, ਗੰਢ, "phandle", delta);
	ਜੇ (ret && ret != -FDT_ERR_NOTFOUND)
		ਵਾਪਸ ret;

	ret = overlay_phandle_ਜੋੜੋ_ਵਿੱਥ(fdto, ਗੰਢ, "linux,phandle", delta);
	ਜੇ (ret && ret != -FDT_ERR_NOTFOUND)
		ਵਾਪਸ ret;

	fdt_for_each_subnode(ਬੱਚਾ, fdto, ਗੰਢ) {
		ret = overlay_adjust_ਗੰਢ_phandles(fdto, ਬੱਚਾ, delta);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
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
ਸਥਿਰ ਪੂਰਨ overlay_adjust_local_phandles(ਖਾਲੀ *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	ਵਾਪਸ overlay_adjust_ਗੰਢ_phandles(fdto, 0, delta);
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
ਸਥਿਰ ਪੂਰਨ overlay_update_local_ਗੰਢ_references(ਖਾਲੀ *fdto,
						ਪੂਰਨ ਰੁੱਖ_ਗੰਢ,
						ਪੂਰਨ fixup_ਗੰਢ,
						uint32_t delta)
{
	ਪੂਰਨ fixup_prop;
	ਪੂਰਨ fixup_ਬੱਚਾ;
	ਪੂਰਨ ret;

	fdt_for_each_property_ਵਿੱਥ(fixup_prop, fdto, fixup_ਗੰਢ) {
		ਅਚਲ fdt32_t *fixup_val;
		ਅਚਲ ਅੱਖਰ *ਰੁੱਖ_val;
		ਅਚਲ ਅੱਖਰ *name;
		ਪੂਰਨ fixup_len;
		ਪੂਰਨ ਰੁੱਖ_len;
		ਪੂਰਨ i;

		fixup_val = fdt_getprop_by_ਵਿੱਥ(fdto, fixup_prop,
						  &name, &fixup_len);
		ਜੇ (!fixup_val)
			ਵਾਪਸ fixup_len;

		ਜੇ (fixup_len % ਆਕਾਰ(uint32_t))
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;
		fixup_len /= ਆਕਾਰ(uint32_t);

		ਰੁੱਖ_val = fdt_getprop(fdto, ਰੁੱਖ_ਗੰਢ, name, &ਰੁੱਖ_len);
		ਜੇ (!ਰੁੱਖ_val) {
			ਜੇ (ਰੁੱਖ_len == -FDT_ERR_NOTFOUND)
				ਵਾਪਸ -FDT_ERR_BADOVERLAY;

			ਵਾਪਸ ਰੁੱਖ_len;
		}

		ਲਈ (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_ਮੰਜ਼ਿਲ_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(&adj_val, ਰੁੱਖ_val + poffset, ਆਕਾਰ(adj_val));

			adj_val = cpu_ਮੰਜ਼ਿਲ_fdt32(fdt32_ਮੰਜ਼ਿਲ_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  ਰੁੱਖ_ਗੰਢ,
								  name,
								  ਲੜੀ_ਲੰਬਾਈ(name),
								  poffset,
								  &adj_val,
								  ਆਕਾਰ(adj_val));
			ਜੇ (ret == -FDT_ERR_NOSPACE)
				ਵਾਪਸ -FDT_ERR_BADOVERLAY;

			ਜੇ (ret)
				ਵਾਪਸ ret;
		}
	}

	fdt_for_each_subnode(fixup_ਬੱਚਾ, fdto, fixup_ਗੰਢ) {
		ਅਚਲ ਅੱਖਰ *fixup_ਬੱਚਾ_name = fdt_get_name(fdto, fixup_ਬੱਚਾ,
							    ਨਿਰਾ);
		ਪੂਰਨ ਰੁੱਖ_ਬੱਚਾ;

		ਰੁੱਖ_ਬੱਚਾ = fdt_subnode_ਵਿੱਥ(fdto, ਰੁੱਖ_ਗੰਢ,
						fixup_ਬੱਚਾ_name);
		ਜੇ (ਰੁੱਖ_ਬੱਚਾ == -FDT_ERR_NOTFOUND)
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;
		ਜੇ (ਰੁੱਖ_ਬੱਚਾ < 0)
			ਵਾਪਸ ਰੁੱਖ_ਬੱਚਾ;

		ret = overlay_update_local_ਗੰਢ_references(fdto,
							   ਰੁੱਖ_ਬੱਚਾ,
							   fixup_ਬੱਚਾ,
							   delta);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
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
ਸਥਿਰ ਪੂਰਨ overlay_update_local_references(ਖਾਲੀ *fdto, uint32_t delta)
{
	ਪੂਰਨ fixups;

	fixups = fdt_ਰਸਤਾ_ਵਿੱਥ(fdto, "/__local_fixups__");
	ਜੇ (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		ਜੇ (fixups == -FDT_ERR_NOTFOUND)
			ਵਾਪਸ 0;

		ਵਾਪਸ fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	ਵਾਪਸ overlay_update_local_ਗੰਢ_references(fdto, 0, fixups,
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
ਸਥਿਰ ਪੂਰਨ overlay_fixup_one_phandle(ਖਾਲੀ *fdt, ਖਾਲੀ *fdto,
				     ਪੂਰਨ symbols_off,
				     ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uint32_t ਰਸਤਾ_len,
				     ਅਚਲ ਅੱਖਰ *name, uint32_t name_len,
				     ਪੂਰਨ poffset, ਅਚਲ ਅੱਖਰ *label)
{
	ਅਚਲ ਅੱਖਰ *ਚਿੰਨ੍ਹ_ਰਸਤਾ;
	uint32_t phandle;
	fdt32_t phandle_prop;
	ਪੂਰਨ ਚਿੰਨ੍ਹ_off, fixup_off;
	ਪੂਰਨ prop_len;

	ਜੇ (symbols_off < 0)
		ਵਾਪਸ symbols_off;

	ਚਿੰਨ੍ਹ_ਰਸਤਾ = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	ਜੇ (!ਚਿੰਨ੍ਹ_ਰਸਤਾ)
		ਵਾਪਸ prop_len;

	ਚਿੰਨ੍ਹ_off = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, ਚਿੰਨ੍ਹ_ਰਸਤਾ);
	ਜੇ (ਚਿੰਨ੍ਹ_off < 0)
		ਵਾਪਸ ਚਿੰਨ੍ਹ_off;

	phandle = fdt_get_phandle(fdt, ਚਿੰਨ੍ਹ_off);
	ਜੇ (!phandle)
		ਵਾਪਸ -FDT_ERR_NOTFOUND;

	fixup_off = fdt_ਰਸਤਾ_ਵਿੱਥ_namelen(fdto, ਰਸਤਾ, ਰਸਤਾ_len);
	ਜੇ (fixup_off == -FDT_ERR_NOTFOUND)
		ਵਾਪਸ -FDT_ERR_BADOVERLAY;
	ਜੇ (fixup_off < 0)
		ਵਾਪਸ fixup_off;

	phandle_prop = cpu_ਮੰਜ਼ਿਲ_fdt32(phandle);
	ਵਾਪਸ fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   ਆਕਾਰ(phandle_prop));
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
ਸਥਿਰ ਪੂਰਨ overlay_fixup_phandle(ਖਾਲੀ *fdt, ਖਾਲੀ *fdto, ਪੂਰਨ symbols_off,
				 ਪੂਰਨ property)
{
	ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;
	ਅਚਲ ਅੱਖਰ *label;
	ਪੂਰਨ len;

	ਮੁੱਲ = fdt_getprop_by_ਵਿੱਥ(fdto, property,
				      &label, &len);
	ਜੇ (!ਮੁੱਲ) {
		ਜੇ (len == -FDT_ERR_NOTFOUND)
			ਵਾਪਸ -FDT_ERR_ਅੰਦਰੂਨੀ;

		ਵਾਪਸ len;
	}

	ਕਰੋ {
		ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, *name, *fixup_ਸਿਰਾ;
		ਅਚਲ ਅੱਖਰ *fixup_str = ਮੁੱਲ;
		uint32_t ਰਸਤਾ_len, name_len;
		uint32_t fixup_len;
		ਅੱਖਰ *sep, *endptr;
		ਪੂਰਨ poffset, ret;

		fixup_ਸਿਰਾ = memchr(ਮੁੱਲ, '\0', len);
		ਜੇ (!fixup_ਸਿਰਾ)
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_ਸਿਰਾ - fixup_str;

		len -= fixup_len + 1;
		ਮੁੱਲ += fixup_len + 1;

		ਰਸਤਾ = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		ਜੇ (!sep || *sep != ':')
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;

		ਰਸਤਾ_len = sep - ਰਸਤਾ;
		ਜੇ (ਰਸਤਾ_len == (fixup_len - 1))
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;

		fixup_len -= ਰਸਤਾ_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		ਜੇ (!sep || *sep != ':')
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		ਜੇ (!name_len)
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		ਜੇ ((*endptr != '\0') || (endptr <= (sep + 1)))
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						ਰਸਤਾ, ਰਸਤਾ_len, name, name_len,
						poffset, label);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	} ਜਦੋਂ (len > 0);

	ਵਾਪਸ 0;
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
ਸਥਿਰ ਪੂਰਨ overlay_fixup_phandles(ਖਾਲੀ *fdt, ਖਾਲੀ *fdto)
{
	ਪੂਰਨ fixups_off, symbols_off;
	ਪੂਰਨ property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_ਰਸਤਾ_ਵਿੱਥ(fdto, "/__fixups__");
	ਜੇ (fixups_off == -FDT_ERR_NOTFOUND)
		ਵਾਪਸ 0; /* nothing to do */
	ਜੇ (fixups_off < 0)
		ਵਾਪਸ fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/__symbols__");
	ਜੇ ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		ਵਾਪਸ symbols_off;

	fdt_for_each_property_ਵਿੱਥ(property, fdto, fixups_off) {
		ਪੂਰਨ ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
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
ਸਥਿਰ ਪੂਰਨ overlay_apply_ਗੰਢ(ਖਾਲੀ *fdt, ਪੂਰਨ ਨਿਸ਼ਾਨਾ,
			      ਖਾਲੀ *fdto, ਪੂਰਨ ਗੰਢ)
{
	ਪੂਰਨ property;
	ਪੂਰਨ subnode;

	fdt_for_each_property_ਵਿੱਥ(property, fdto, ਗੰਢ) {
		ਅਚਲ ਅੱਖਰ *name;
		ਅਚਲ ਖਾਲੀ *prop;
		ਪੂਰਨ prop_len;
		ਪੂਰਨ ret;

		prop = fdt_getprop_by_ਵਿੱਥ(fdto, property, &name,
					     &prop_len);
		ਜੇ (prop_len == -FDT_ERR_NOTFOUND)
			ਵਾਪਸ -FDT_ERR_ਅੰਦਰੂਨੀ;
		ਜੇ (prop_len < 0)
			ਵਾਪਸ prop_len;

		ret = fdt_setprop(fdt, ਨਿਸ਼ਾਨਾ, name, prop, prop_len);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	fdt_for_each_subnode(subnode, fdto, ਗੰਢ) {
		ਅਚਲ ਅੱਖਰ *name = fdt_get_name(fdto, subnode, ਨਿਰਾ);
		ਪੂਰਨ nnode;
		ਪੂਰਨ ret;

		nnode = fdt_ਜੋੜੋ_subnode(fdt, ਨਿਸ਼ਾਨਾ, name);
		ਜੇ (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_ਵਿੱਥ(fdt, ਨਿਸ਼ਾਨਾ, name);
			ਜੇ (nnode == -FDT_ERR_NOTFOUND)
				ਵਾਪਸ -FDT_ERR_ਅੰਦਰੂਨੀ;
		}

		ਜੇ (nnode < 0)
			ਵਾਪਸ nnode;

		ret = overlay_apply_ਗੰਢ(fdt, nnode, fdto, subnode);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
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
ਸਥਿਰ ਪੂਰਨ overlay_ਮਿਲਾਓ(ਖਾਲੀ *fdt, ਖਾਲੀ *fdto)
{
	ਪੂਰਨ fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		ਪੂਰਨ overlay;
		ਪੂਰਨ ਨਿਸ਼ਾਨਾ;
		ਪੂਰਨ ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_ਵਿੱਥ(fdto, fragment, "__overlay__");
		ਜੇ (overlay == -FDT_ERR_NOTFOUND)
			ਜਾਰੀ;

		ਜੇ (overlay < 0)
			ਵਾਪਸ overlay;

		ਨਿਸ਼ਾਨਾ = overlay_get_ਨਿਸ਼ਾਨਾ(fdt, fdto, fragment, ਨਿਰਾ);
		ਜੇ (ਨਿਸ਼ਾਨਾ < 0)
			ਵਾਪਸ ਨਿਸ਼ਾਨਾ;

		ret = overlay_apply_ਗੰਢ(fdt, ਨਿਸ਼ਾਨਾ, fdto, overlay);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ get_ਰਸਤਾ_len(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਪੂਰਨ len = 0, namelen;
	ਅਚਲ ਅੱਖਰ *name;

	FDT_RO_PROBE(fdt);

	ਲਈ (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		ਜੇ (!name)
			ਵਾਪਸ namelen;

		/* root? we're done */
		ਜੇ (namelen == 0)
			ਰੋਕੋ;

		nodeoffset = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, nodeoffset);
		ਜੇ (nodeoffset < 0)
			ਵਾਪਸ nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	ਜੇ (len == 0)
		len++;
	ਵਾਪਸ len;
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
ਸਥਿਰ ਪੂਰਨ overlay_ਚਿੰਨ੍ਹ_update(ਖਾਲੀ *fdt, ਖਾਲੀ *fdto)
{
	ਪੂਰਨ ਜੜ੍ਹ_sym, ov_sym, prop, ਰਸਤਾ_len, fragment, ਨਿਸ਼ਾਨਾ;
	ਪੂਰਨ len, frag_name_len, ret, rel_ਰਸਤਾ_len;
	ਅਚਲ ਅੱਖਰ *s, *e;
	ਅਚਲ ਅੱਖਰ *ਰਸਤਾ;
	ਅਚਲ ਅੱਖਰ *name;
	ਅਚਲ ਅੱਖਰ *frag_name;
	ਅਚਲ ਅੱਖਰ *rel_ਰਸਤਾ;
	ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਰਸਤਾ;
	ਅੱਖਰ *buf;
	ਖਾਲੀ *p;

	ov_sym = fdt_subnode_ਵਿੱਥ(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	ਜੇ (ov_sym < 0)
		ਵਾਪਸ 0;

	ਜੜ੍ਹ_sym = fdt_subnode_ਵਿੱਥ(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	ਜੇ (ਜੜ੍ਹ_sym == -FDT_ERR_NOTFOUND)
		ਜੜ੍ਹ_sym = fdt_ਜੋੜੋ_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	ਜੇ (ਜੜ੍ਹ_sym < 0)
		ਵਾਪਸ ਜੜ੍ਹ_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_ਵਿੱਥ(prop, fdto, ov_sym) {
		ਰਸਤਾ = fdt_getprop_by_ਵਿੱਥ(fdto, prop, &name, &ਰਸਤਾ_len);
		ਜੇ (!ਰਸਤਾ)
			ਵਾਪਸ ਰਸਤਾ_len;

		/* verify it's a string property (terminated by a single \0) */
		ਜੇ (ਰਸਤਾ_len < 1 || memchr(ਰਸਤਾ, '\0', ਰਸਤਾ_len) != &ਰਸਤਾ[ਰਸਤਾ_len - 1])
			ਵਾਪਸ -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = ਰਸਤਾ + ਰਸਤਾ_len;

		ਜੇ (*ਰਸਤਾ != '/')
			ਵਾਪਸ -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(ਰਸਤਾ + 1, '/');
		ਜੇ (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ਜਾਰੀ;
		}

		frag_name = ਰਸਤਾ + 1;
		frag_name_len = s - ਰਸਤਾ - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = ਆਕਾਰ("/__overlay__/") - 1;
		ਜੇ ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_ਰਸਤਾ = s + len;
			rel_ਰਸਤਾ_len = e - rel_ਰਸਤਾ - 1;
		} ਨਹੀਂਤਾਂ ਜੇ ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_ਰਸਤਾ = "";
			rel_ਰਸਤਾ_len = 0;
		} ਨਹੀਂਤਾਂ {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ਜਾਰੀ;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_ਵਿੱਥ_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		ਜੇ (ret < 0)
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_ਵਿੱਥ(fdto, fragment, "__overlay__");
		ਜੇ (ret < 0)
			ਵਾਪਸ -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_ਨਿਸ਼ਾਨਾ(fdt, fdto, fragment, &ਨਿਸ਼ਾਨਾ_ਰਸਤਾ);
		ਜੇ (ret < 0)
			ਵਾਪਸ ret;
		ਨਿਸ਼ਾਨਾ = ret;

		/* if we have a target path use */
		ਜੇ (!ਨਿਸ਼ਾਨਾ_ਰਸਤਾ) {
			ret = get_ਰਸਤਾ_len(fdt, ਨਿਸ਼ਾਨਾ);
			ਜੇ (ret < 0)
				ਵਾਪਸ ret;
			len = ret;
		} ਨਹੀਂਤਾਂ {
			len = ਲੜੀ_ਲੰਬਾਈ(ਨਿਸ਼ਾਨਾ_ਰਸਤਾ);
		}

		ret = fdt_setprop_placeholder(fdt, ਜੜ੍ਹ_sym, name,
				len + (len > 1) + rel_ਰਸਤਾ_len + 1, &p);
		ਜੇ (ret < 0)
			ਵਾਪਸ ret;

		ਜੇ (!ਨਿਸ਼ਾਨਾ_ਰਸਤਾ) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_ਨਿਸ਼ਾਨਾ(fdt, fdto, fragment, &ਨਿਸ਼ਾਨਾ_ਰਸਤਾ);
			ਜੇ (ret < 0)
				ਵਾਪਸ ret;
			ਨਿਸ਼ਾਨਾ = ret;
		}

		buf = p;
		ਜੇ (len > 1) { /* target is not root */
			ਜੇ (!ਨਿਸ਼ਾਨਾ_ਰਸਤਾ) {
				ret = fdt_get_ਰਸਤਾ(fdt, ਨਿਸ਼ਾਨਾ, buf, len + 1);
				ਜੇ (ret < 0)
					ਵਾਪਸ ret;
			} ਨਹੀਂਤਾਂ
				ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, ਨਿਸ਼ਾਨਾ_ਰਸਤਾ, len + 1);

		} ਨਹੀਂਤਾਂ
			len--;

		buf[len] = '/';
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + len + 1, rel_ਰਸਤਾ, rel_ਰਸਤਾ_len);
		buf[len + 1 + rel_ਰਸਤਾ_len] = '\0';
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_overlay_apply(ਖਾਲੀ *fdt, ਖਾਲੀ *fdto)
{
	uint32_t delta;
	ਪੂਰਨ ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_ਲੱਭੋ_max_phandle(fdt, &delta);
	ਜੇ (ret)
		ਜਾਓ err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	ਜੇ (ret)
		ਜਾਓ err;

	ret = overlay_update_local_references(fdto, delta);
	ਜੇ (ret)
		ਜਾਓ err;

	ret = overlay_fixup_phandles(fdt, fdto);
	ਜੇ (ret)
		ਜਾਓ err;

	ret = overlay_ਮਿਲਾਓ(fdt, fdto);
	ਜੇ (ret)
		ਜਾਓ err;

	ret = overlay_ਚਿੰਨ੍ਹ_update(fdt, fdto);
	ਜੇ (ret)
		ਜਾਓ err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_ਸੈੱਟ_magic(fdto, ~0);

	ਵਾਪਸ 0;

err:
	/*
	 * The overlay might have been damaged, erase its magic.
	 */
	fdt_ਸੈੱਟ_magic(fdto, ~0);

	/*
	 * The base device tree might have been damaged, erase its
	 * magic.
	 */
	fdt_ਸੈੱਟ_magic(fdt, ~0);

	ਵਾਪਸ ret;
}
