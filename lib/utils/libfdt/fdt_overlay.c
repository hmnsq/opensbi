// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

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
ସ୍ଥିର uint32_t overlay_get_target_phandle(ଅଚଳ ଶୂନ୍ୟ *fdto, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fragment)
{
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	ଯଦି (!val)
		ଫେରନ୍ତୁ 0;

	ଯଦି ((len != ଆକାର(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		ଫେରନ୍ତୁ (uint32_t)-1;

	ଫେରନ୍ତୁ fdt32_to_cpu(*val);
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_get_target(ଅଚଳ ଶୂନ୍ୟ *fdt, ଅଚଳ ଶୂନ୍ୟ *fdto,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା fragment, ଅକ୍ଷର ଅଚଳ **pathp)
{
	uint32_t phandle;
	ଅଚଳ ଅକ୍ଷର *path = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	ଯଦି (phandle == (uint32_t)-1)
		ଫେରନ୍ତୁ -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	ଯଦି (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		ଯଦି (path)
			ret = fdt_path_offset(fdt, path);
		ଅନ୍ୟଥା
			ret = path_len;
	} ଅନ୍ୟଥା
		ret = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	ଯଦି (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	ଯଦି (ret < 0)
		ଫେରନ୍ତୁ ret;

	/* return pointer to path (if available) */
	ଯଦି (pathp)
		*pathp = path ? path : ଖାଲି;

	ଫେରନ୍ତୁ ret;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_phandle_add_offset(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି,
				      ଅଚଳ ଅକ୍ଷର *name, uint32_t delta)
{
	ଅଚଳ fdt32_t *val;
	uint32_t adj_val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	val = fdt_getprop(fdt, ଗଣ୍ଠି, name, &len);
	ଯଦି (!val)
		ଫେରନ୍ତୁ len;

	ଯଦି (len != ଆକାର(*val))
		ଫେରନ୍ତୁ -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	ଯଦି ((adj_val + delta) < adj_val)
		ଫେରନ୍ତୁ -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	ଯଦି (adj_val == (uint32_t)-1)
		ଫେରନ୍ତୁ -FDT_ERR_NOPHANDLES;

	ଫେରନ୍ତୁ fdt_setprop_inplace_u32(fdt, ଗଣ୍ଠି, name, adj_val);
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_adjust_ଗଣ୍ଠି_phandles(ଶୂନ୍ୟ *fdto, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି,
					uint32_t delta)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା child;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ret = overlay_phandle_add_offset(fdto, ଗଣ୍ଠି, "phandle", delta);
	ଯଦି (ret && ret != -FDT_ERR_NOTFOUND)
		ଫେରନ୍ତୁ ret;

	ret = overlay_phandle_add_offset(fdto, ଗଣ୍ଠି, "linux,phandle", delta);
	ଯଦି (ret && ret != -FDT_ERR_NOTFOUND)
		ଫେରନ୍ତୁ ret;

	fdt_for_each_subnode(child, fdto, ଗଣ୍ଠି) {
		ret = overlay_adjust_ଗଣ୍ଠି_phandles(fdto, child, delta);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_adjust_local_phandles(ଶୂନ୍ୟ *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	ଫେରନ୍ତୁ overlay_adjust_ଗଣ୍ଠି_phandles(fdto, 0, delta);
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_update_local_ଗଣ୍ଠି_references(ଶୂନ୍ୟ *fdto,
						ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବୃକ୍ଷ_ଗଣ୍ଠି,
						ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixup_ଗଣ୍ଠି,
						uint32_t delta)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixup_prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixup_child;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_ଗଣ୍ଠି) {
		ଅଚଳ fdt32_t *fixup_val;
		ଅଚଳ ଅକ୍ଷର *ବୃକ୍ଷ_val;
		ଅଚଳ ଅକ୍ଷର *name;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixup_len;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବୃକ୍ଷ_len;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		ଯଦି (!fixup_val)
			ଫେରନ୍ତୁ fixup_len;

		ଯଦି (fixup_len % ଆକାର(uint32_t))
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;
		fixup_len /= ଆକାର(uint32_t);

		ବୃକ୍ଷ_val = fdt_getprop(fdto, ବୃକ୍ଷ_ଗଣ୍ଠି, name, &ବୃକ୍ଷ_len);
		ଯଦି (!ବୃକ୍ଷ_val) {
			ଯଦି (ବୃକ୍ଷ_len == -FDT_ERR_NOTFOUND)
				ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;

			ଫେରନ୍ତୁ ବୃକ୍ଷ_len;
		}

		ପାଇଁ (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, ବୃକ୍ଷ_val + poffset, ଆକାର(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  ବୃକ୍ଷ_ଗଣ୍ଠି,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  ଆକାର(adj_val));
			ଯଦି (ret == -FDT_ERR_NOSPACE)
				ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;

			ଯଦି (ret)
				ଫେରନ୍ତୁ ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_ଗଣ୍ଠି) {
		ଅଚଳ ଅକ୍ଷର *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    ଖାଲି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବୃକ୍ଷ_child;

		ବୃକ୍ଷ_child = fdt_subnode_offset(fdto, ବୃକ୍ଷ_ଗଣ୍ଠି,
						fixup_child_name);
		ଯଦି (ବୃକ୍ଷ_child == -FDT_ERR_NOTFOUND)
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;
		ଯଦି (ବୃକ୍ଷ_child < 0)
			ଫେରନ୍ତୁ ବୃକ୍ଷ_child;

		ret = overlay_update_local_ଗଣ୍ଠି_references(fdto,
							   ବୃକ୍ଷ_child,
							   fixup_child,
							   delta);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_update_local_references(ଶୂନ୍ୟ *fdto, uint32_t delta)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	ଯଦି (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		ଯଦି (fixups == -FDT_ERR_NOTFOUND)
			ଫେରନ୍ତୁ 0;

		ଫେରନ୍ତୁ fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	ଫେରନ୍ତୁ overlay_update_local_ଗଣ୍ଠି_references(fdto, 0, fixups,
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_fixup_one_phandle(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *fdto,
				     ପୂର୍ଣ୍ଣସଂଖ୍ୟା symbols_off,
				     ଅଚଳ ଅକ୍ଷର *path, uint32_t path_len,
				     ଅଚଳ ଅକ୍ଷର *name, uint32_t name_len,
				     ପୂର୍ଣ୍ଣସଂଖ୍ୟା poffset, ଅଚଳ ଅକ୍ଷର *label)
{
	ଅଚଳ ଅକ୍ଷର *ପ୍ରତୀକ_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ପ୍ରତୀକ_off, fixup_off;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା prop_len;

	ଯଦି (symbols_off < 0)
		ଫେରନ୍ତୁ symbols_off;

	ପ୍ରତୀକ_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	ଯଦି (!ପ୍ରତୀକ_path)
		ଫେରନ୍ତୁ prop_len;

	ପ୍ରତୀକ_off = fdt_path_offset(fdt, ପ୍ରତୀକ_path);
	ଯଦି (ପ୍ରତୀକ_off < 0)
		ଫେରନ୍ତୁ ପ୍ରତୀକ_off;

	phandle = fdt_get_phandle(fdt, ପ୍ରତୀକ_off);
	ଯଦି (!phandle)
		ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	ଯଦି (fixup_off == -FDT_ERR_NOTFOUND)
		ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;
	ଯଦି (fixup_off < 0)
		ଫେରନ୍ତୁ fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	ଫେରନ୍ତୁ fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   ଆକାର(phandle_prop));
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_fixup_phandle(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *fdto, ପୂର୍ଣ୍ଣସଂଖ୍ୟା symbols_off,
				 ପୂର୍ଣ୍ଣସଂଖ୍ୟା property)
{
	ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;
	ଅଚଳ ଅକ୍ଷର *label;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	ମୂଲ୍ୟ = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	ଯଦି (!ମୂଲ୍ୟ) {
		ଯଦି (len == -FDT_ERR_NOTFOUND)
			ଫେରନ୍ତୁ -FDT_ERR_INTERNAL;

		ଫେରନ୍ତୁ len;
	}

	କର {
		ଅଚଳ ଅକ୍ଷର *path, *name, *fixup_end;
		ଅଚଳ ଅକ୍ଷର *fixup_str = ମୂଲ୍ୟ;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		ଅକ୍ଷର *sep, *endptr;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା poffset, ret;

		fixup_end = memchr(ମୂଲ୍ୟ, '\0', len);
		ଯଦି (!fixup_end)
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		ମୂଲ୍ୟ += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		ଯଦି (!sep || *sep != ':')
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		ଯଦି (path_len == (fixup_len - 1))
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		ଯଦି (!sep || *sep != ':')
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		ଯଦି (!name_len)
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		ଯଦି ((*endptr != '\0') || (endptr <= (sep + 1)))
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	} ଯେତେବେଳେ (len > 0);

	ଫେରନ୍ତୁ 0;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_fixup_phandles(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *fdto)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixups_off, symbols_off;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	ଯଦି (fixups_off == -FDT_ERR_NOTFOUND)
		ଫେରନ୍ତୁ 0; /* nothing to do */
	ଯଦି (fixups_off < 0)
		ଫେରନ୍ତୁ fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	ଯଦି ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		ଫେରନ୍ତୁ symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_apply_ଗଣ୍ଠି(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା target,
			      ଶୂନ୍ୟ *fdto, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା property;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା subnode;

	fdt_for_each_property_offset(property, fdto, ଗଣ୍ଠି) {
		ଅଚଳ ଅକ୍ଷର *name;
		ଅଚଳ ଶୂନ୍ୟ *prop;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା prop_len;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		ଯଦି (prop_len == -FDT_ERR_NOTFOUND)
			ଫେରନ୍ତୁ -FDT_ERR_INTERNAL;
		ଯଦି (prop_len < 0)
			ଫେରନ୍ତୁ prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	fdt_for_each_subnode(subnode, fdto, ଗଣ୍ଠି) {
		ଅଚଳ ଅକ୍ଷର *name = fdt_get_name(fdto, subnode, ଖାଲି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା nnode;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

		nnode = fdt_add_subnode(fdt, target, name);
		ଯଦି (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			ଯଦି (nnode == -FDT_ERR_NOTFOUND)
				ଫେରନ୍ତୁ -FDT_ERR_INTERNAL;
		}

		ଯଦି (nnode < 0)
			ଫେରନ୍ତୁ nnode;

		ret = overlay_apply_ଗଣ୍ଠି(fdt, nnode, fdto, subnode);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_merge(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *fdto)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା target;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		ଯଦି (overlay == -FDT_ERR_NOTFOUND)
			ଜାରି;

		ଯଦି (overlay < 0)
			ଫେରନ୍ତୁ overlay;

		target = overlay_get_target(fdt, fdto, fragment, ଖାଲି);
		ଯଦି (target < 0)
			ଫେରନ୍ତୁ target;

		ret = overlay_apply_ଗଣ୍ଠି(fdt, target, fdto, overlay);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_path_len(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 0, namelen;
	ଅଚଳ ଅକ୍ଷର *name;

	FDT_RO_PROBE(fdt);

	ପାଇଁ (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		ଯଦି (!name)
			ଫେରନ୍ତୁ namelen;

		/* root? we're done */
		ଯଦି (namelen == 0)
			ଭାଙ୍ଗ;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		ଯଦି (nodeoffset < 0)
			ଫେରନ୍ତୁ nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	ଯଦି (len == 0)
		len++;
	ଫେରନ୍ତୁ len;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlay_ପ୍ରତୀକ_update(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *fdto)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_sym, ov_sym, prop, path_len, fragment, target;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, frag_name_len, ret, rel_path_len;
	ଅଚଳ ଅକ୍ଷର *s, *e;
	ଅଚଳ ଅକ୍ଷର *path;
	ଅଚଳ ଅକ୍ଷର *name;
	ଅଚଳ ଅକ୍ଷର *frag_name;
	ଅଚଳ ଅକ୍ଷର *rel_path;
	ଅଚଳ ଅକ୍ଷର *target_path;
	ଅକ୍ଷର *buf;
	ଶୂନ୍ୟ *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	ଯଦି (ov_sym < 0)
		ଫେରନ୍ତୁ 0;

	ମୂଳ_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	ଯଦି (ମୂଳ_sym == -FDT_ERR_NOTFOUND)
		ମୂଳ_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	ଯଦି (ମୂଳ_sym < 0)
		ଫେରନ୍ତୁ ମୂଳ_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		ଯଦି (!path)
			ଫେରନ୍ତୁ path_len;

		/* verify it's a string property (terminated by a single \0) */
		ଯଦି (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			ଫେରନ୍ତୁ -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		ଯଦି (*path != '/')
			ଫେରନ୍ତୁ -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		ଯଦି (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ଜାରି;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = ଆକାର("/__overlay__/") - 1;
		ଯଦି ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} ଅନ୍ୟଥା ଯଦି ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} ଅନ୍ୟଥା {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ଜାରି;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		ଯଦି (ret < 0)
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		ଯଦି (ret < 0)
			ଫେରନ୍ତୁ -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		ଯଦି (ret < 0)
			ଫେରନ୍ତୁ ret;
		target = ret;

		/* if we have a target path use */
		ଯଦି (!target_path) {
			ret = get_path_len(fdt, target);
			ଯଦି (ret < 0)
				ଫେରନ୍ତୁ ret;
			len = ret;
		} ଅନ୍ୟଥା {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, ମୂଳ_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		ଯଦି (ret < 0)
			ଫେରନ୍ତୁ ret;

		ଯଦି (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			ଯଦି (ret < 0)
				ଫେରନ୍ତୁ ret;
			target = ret;
		}

		buf = p;
		ଯଦି (len > 1) { /* target is not root */
			ଯଦି (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				ଯଦି (ret < 0)
					ଫେରନ୍ତୁ ret;
			} ଅନ୍ୟଥା
				memcpy(buf, target_path, len + 1);

		} ଅନ୍ୟଥା
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_overlay_apply(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *fdto)
{
	uint32_t delta;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	ଯଦି (ret)
		ଯାଅ err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	ଯଦି (ret)
		ଯାଅ err;

	ret = overlay_update_local_references(fdto, delta);
	ଯଦି (ret)
		ଯାଅ err;

	ret = overlay_fixup_phandles(fdt, fdto);
	ଯଦି (ret)
		ଯାଅ err;

	ret = overlay_merge(fdt, fdto);
	ଯଦି (ret)
		ଯାଅ err;

	ret = overlay_ପ୍ରତୀକ_update(fdt, fdto);
	ଯଦି (ret)
		ଯାଅ err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	ଫେରନ୍ତୁ 0;

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

	ଫେରନ୍ତୁ ret;
}
