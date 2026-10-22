// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_hartmask.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/irqchip/aplic.h>
#ضمن <sbi_utils/irqchip/imsic.h>
#ضمن <sbi_utils/irqchip/plic.h>

#define DEFAULT_UART_FREQ		0
#define DEFAULT_UART_BAUD		115200
#define DEFAULT_UART_REG_SHIFT		0
#define DEFAULT_UART_REG_IO_WIDTH	1
#define DEFAULT_UART_REG_OFFSET		0

#define DEFAULT_RENESAS_SCIF_FREQ		100000000
#define DEFAULT_RENESAS_SCIF_BAUD		115200

#define DEFAULT_SIFIVE_UART_FREQ		0
#define DEFAULT_SIFIVE_UART_BAUD		115200

#define DEFAULT_SHAKTI_UART_FREQ		50000000
#define DEFAULT_SHAKTI_UART_BAUD		115200

صحيح fdt_parse_phandle_with_args(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت حرف *prop, ثابت حرف *cells_prop,
				صحيح index, بنية fdt_phandle_args *out_args)
{
	u32 i, pcells;
	صحيح len, pnodeoff;
	ثابت fdt32_t *list, *list_end, *val;

	אם (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		החזר SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	אם (!list)
		החזר SBI_ENOENT;
	list_end = list + (len / حجم(*list));

	כלעוד (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		אם (pnodeoff < 0)
			החזר pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		אם (!val)
			החזר SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		אם (FDT_MAX_PHANDLE_ARGS < pcells)
			החזר SBI_EINVAL;
		אם (list + pcells > list_end)
			החזר SBI_ENOENT;

		אם (index > 0) {
			list += pcells;
			index--;
		} אחרת {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			עבור (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			החזר 0;
		}
	}

	החזר SBI_ENOENT;
}

ساكن صحيح fdt_translate_address(ثابت فراغ *fdt, uint64_t reg, صحيح parent,
				 uint64_t *addr)
{
	صحيح i, rlen;
	صحيح cell_parent_addr, cell_child_addr, cell_size;
	ثابت fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	אם (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		אם (cell_child_addr < 1)
			החזר SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		אם (cell_parent_addr < 1)
			החזר SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		אם (cell_size < 0)
			החזר SBI_ENODEV;

		עבור (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		עבור (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		עבור (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		אם (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			החזר SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} אחרת {
		/* No translation required */
		*addr = reg;
	}

	החזר 0;
}

صحيح fdt_get_node_addr_size(ثابت فراغ *fdt, صحيح node, صحيح index,
			   uint64_t *addr, uint64_t *size)
{
	صحيح parent, len, i, rc;
	صحيح cell_addr, cell_size;
	ثابت fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	אם (!fdt || node < 0 || index < 0)
		החזר SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	אם (parent < 0)
		החזר parent;
	cell_addr = fdt_address_cells(fdt, parent);
	אם (cell_addr < 1)
		החזר SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	אם (cell_size < 0)
		החזר SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	אם (!prop_addr)
		החזר SBI_ENODEV;

	אם ((len / حجم(u32)) <= (index * (cell_addr + cell_size)))
		החזר SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	אם (addr) {
		עבור (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		עשה {
			אם (parent < 0)
				اكسر;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			אם (rc)
				اكسر;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} כלעוד (1);
	}
	temp = 0;

	אם (size) {
		עבור (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	החזר 0;
}

صحيح fdt_get_node_addr_size_by_name(ثابت فراغ *fdt, صحيح node, ثابت حرف *name,
				   uint64_t *addr, uint64_t *size)
{
	صحيح i, j, count;
	ثابت حرف *val;
	ثابت حرف *regname;

	אם (!fdt || node < 0 || !name)
		החזר SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	אם (!val)
		החזר SBI_ENODEV;

	עבור (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		אם (strcmp(name, regname) == 0)
			החזר fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	החזר SBI_ENODEV;
}

bool fdt_node_is_enabled(ثابت فراغ *fdt, صحيح nodeoff)
{
	صحيح len;
	ثابت فراغ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	אם (!prop)
		החזר true;

	אם (!strncmp(prop, "okay", strlen("okay")))
		החזר true;

	אם (!strncmp(prop, "ok", strlen("ok")))
		החזר true;

	החזר false;
}

صحيح fdt_parse_hart_id(ثابت فراغ *fdt, صحيح cpu_offset, u32 *hartid)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	אם (!fdt || cpu_offset < 0)
		החזר SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	אם (!prop || !len)
		החזר SBI_EINVAL;
	אם (strncmp (prop, "cpu", strlen ("cpu")))
		החזר SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	אם (!val || len < حجم(fdt32_t))
		החזר SBI_EINVAL;

	אם (len > حجم(fdt32_t))
		val++;

	אם (hartid)
		*hartid = fdt32_to_cpu(*val);

	החזר 0;
}

صحيح fdt_parse_cbom_block_size(ثابت فراغ *fdt, صحيح cpu_offset, unsigned long *cbom_block_size)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	אם (!fdt || cpu_offset < 0)
		החזר SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	אם (!prop || !len)
		החזר SBI_EINVAL;
	אם (strncmp (prop, "cpu", strlen ("cpu")))
		החזר SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	אם (!val || len < حجم(fdt32_t))
		החזר SBI_EINVAL;

	אם (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	החזר 0;
}

صحيح fdt_parse_max_enabled_hart_id(ثابت فراغ *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحيح err, cpu_offset, cpus_offset;

	אם (!fdt)
		החזר SBI_EINVAL;
	אם (!max_hartid)
		החזר 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	אם (cpus_offset < 0)
		החזר cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		אם (err)
			استمر;

		אם (!fdt_node_is_enabled(fdt, cpu_offset))
			استمر;

		אם (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	החזר 0;
}

صحيح fdt_parse_timebase_frequency(ثابت فراغ *fdt, unsigned long *freq)
{
	ثابت fdt32_t *val;
	صحيح len, cpus_offset;

	אם (!fdt || !freq)
		החזר SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	אם (cpus_offset < 0)
		החזר cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	אם (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	אחרת
		החזר SBI_ENOENT;

	החזר 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

ساكن صحيح fdt_parse_isa_one_hart(ثابت حرف *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	حرف mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	אם (isa[i] == 'r' || isa[i] == 'R')
		i++;
	אחרת
		החזר SBI_EINVAL;

	אם (isa[i] == 'v' || isa[i] == 'V')
		i++;
	אחרת
		החזר SBI_EINVAL;

	אם (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	אחרת אם (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	אחרת
		החזר SBI_EINVAL;

	/* Skip base ISA extensions */
	עבור (; i < isa_len; i++) {
		אם (isa[i] == '_')
			اكسر;
	}

	כלעוד (i < isa_len) {
		אם (isa[i] != '_') {
			i++;
			استمر;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		כלעוד ((i < isa_len) && (isa[i] != '_') &&
		       (j < (حجم(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		אם (!j)
			استمر;

#define set_multi_letter_ext(name, bit)				\
			אם (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				استمر;			\
			}

		עבור (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	החזר 0;
}

ساكن فراغ fdt_parse_isa_extensions_one_hart(ثابت حرف *isa,
					      unsigned long *extensions,
					      صحيح len)
{
	size_t i;

	עבור (i = 0; i < SBI_HART_EXT_MAX; i++) {
		אם (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

صحيح fdt_parse_isa_extensions_all_harts(ثابت فراغ *fdt)
{
	u32 hartid;
	ثابت fdt32_t *val;
	بنية sbi_scratch *scratch;
	بنية sbi_hart_features *hfeatures;
	صحيح err, cpu_offset, cpus_offset, len;

	אם (!fdt)
		החזר SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	אם (cpus_offset < 0)
		החזר cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		אם (err)
			استمر;

		אם (!fdt_node_is_enabled(fdt, cpu_offset))
			استمر;

		scratch = sbi_hartid_to_scratch(hartid);
		אם (!scratch)
			החזר SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		אם (!hfeatures)
			החזר SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		אם (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت حرف *)val,
							  hfeatures->extensions, len);
			استمر;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		אם (!val || len <= 0)
			החזר SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت حرف *)val, hfeatures->extensions);
		אם (err)
			החזר err;
	}

	החזר 0;
}

ساكن صحيح fdt_parse_uart_node_common(ثابت فراغ *fdt, صحيح nodeoffset,
				      بنية platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_size;

	אם (nodeoffset < 0 || !uart || !fdt)
		החזר SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	אם (rc < 0 || !reg_size)
		החזר SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	אם (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	אחרת
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	אם (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	אחרת
		uart->baud = default_baud;

	החזר 0;
}

صحيح fdt_parse_gaisler_uart_node(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_data *uart)
{
	החזר fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحيح fdt_parse_renesas_scif_node(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_data *uart)
{
	החזר fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحيح fdt_parse_shakti_uart_node(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	החזר fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحيح fdt_parse_sifive_uart_node(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	החזר fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

صحيح fdt_parse_uart_node(ثابت فراغ *fdt, صحيح nodeoffset,
			بنية platform_uart_data *uart)
{
	صحيح len, rc;
	ثابت fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	אם (rc)
		החזר rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	אם (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	אחרת
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	אם (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	אחרת
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	אם (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	אחרת
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	החזר 0;
}

صحيح fdt_parse_uart8250(ثابت فراغ *fdt, بنية platform_uart_data *uart,
		   ثابت حرف *compatible)
{
	صحيح nodeoffset;

	אם (!compatible || !uart || !fdt)
		החזר SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	אם (nodeoffset < 0)
		החזר nodeoffset;

	החזר fdt_parse_uart_node(fdt, nodeoffset, uart);
}

صحيح fdt_parse_xlnx_uartlite_node(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	החזר fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

ساكن صحيح fdt_aplic_find_imsic_node(ثابت فراغ *fdt, صحيح nodeoff,
				     بنية imsic_data *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	אם (val && len >= حجم(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		אם (noff < 0)
			החזר noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		אם (rc)
			החזר rc;

		rc = imsic_data_check(imsic);
		אם (rc)
			החזר rc;

		אם (imsic->targets_mmode == mmode) {
			החזר 0;
		}
	} אחרת {
		החזר SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	אם (!val || len < حجم(fdt32_t))
		החזר SBI_ENODEV;

	len /= حجم(fdt32_t);

	עבור (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		אם (noff < 0)
			החזר noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		אם (!rc)
			اكسر;
	}

	החזר rc;
}

صحيح fdt_parse_aplic_node(ثابت فراغ *fdt, صحيح nodeoff, بنية aplic_data *aplic)
{
	bool child_found;
	ثابت fdt32_t *val;
	ثابت fdt32_t *del;
	بنية imsic_data imsic = { 0 };
	صحيح i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	بنية aplic_delegate_data *deleg;

	אם (nodeoff < 0 || !aplic || !fdt)
		החזר SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	אם (rc < 0 || !reg_size)
		החזר SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	אם (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	אם (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		עבור (i = 0; i < len; i += 2) {
			אם (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				اكسر;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	אם (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		אם (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			החזר SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	אם (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		אם (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			החזר SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	עבור (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	אם (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	אם (!del || len < (3 * حجم(fdt32_t)))
		اذهب skip_delegate_parse;
	d = 0;
	dcnt = len / حجم(fdt32_t);
	עבור (i = 0; i < dcnt; i += 3) {
		אם (d >= APLIC_MAX_DELEGATE)
			اكسر;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		אם (!val || len < حجم(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			استمر;
		}
		len = len / حجم(fdt32_t);
		עבור (j = 0; j < len; j++) {
			אם (del[i] != val[j])
				استمر;
			deleg->child_index = j;
			child_found = true;
			اكسر;
		}

		אם (child_found) {
			d++;
		} אחרת {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	החזר 0;
}

bool fdt_check_imsic_mlevel(ثابت فراغ *fdt)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff = 0;

	אם (!fdt)
		החזר false;

	כלעוד ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		אם (val && len > حجم(fdt32_t)) {
			len = len / حجم(fdt32_t);
			עבור (i = 0; i < len; i += 2) {
				אם (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					החזר true;
			}
		}
	}

	החזר false;
}

صحيح fdt_parse_imsic_node(ثابت فراغ *fdt, صحيح nodeoff, بنية imsic_data *imsic)
{
	ثابت fdt32_t *val;
	بنية imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	صحيح i, rc, len, nr_parent_irqs;

	אם (nodeoff < 0 || !imsic || !fdt)
		החזר SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	אם (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		nr_parent_irqs = len / 2;
		עבור (i = 0; i < len; i += 2) {
			אם (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				اكسر;
			}
		}
	} אחרת
		החזר SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	אם (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	אחרת
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	אם (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} אחרת {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		אם ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	אם (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	אחרת
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	אם (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	אחרת
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	אם (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	אחרת
		החזר SBI_EINVAL;

	עבור (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	עבור (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		אם (rc < 0 || !reg_size)
			اكسر;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	אם (!imsic->regs[0].size)
		החזר SBI_EINVAL;

	החזר 0;
}

صحيح fdt_parse_plic_node(ثابت فراغ *fdt, صحيح nodeoffset, بنية plic_data *plic)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_size;

	אם (nodeoffset < 0 || !plic || !fdt)
		החזר SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	אם (rc < 0 || !reg_size)
		החזר SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	אם (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	החזר 0;
}

صحيح fdt_parse_plic(ثابت فراغ *fdt, بنية plic_data *plic, ثابت حرف *compat)
{
	صحيح nodeoffset;

	אם (!compat || !plic || !fdt)
		החזר SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	אם (nodeoffset < 0)
		החזר nodeoffset;

	החזר fdt_parse_plic_node(fdt, nodeoffset, plic);
}

ساكن صحيح fdt_get_aclint_addr_size_by_name(ثابت فراغ *fdt, صحيح nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	אם (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	אם (rc < 0 || !reg_size)
		החזר SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	החזר 0;
}

ساكن صحيح fdt_get_aclint_addr_size(ثابت فراغ *fdt, صحيح nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	אם (rc < 0 || !reg_size)
		החזר SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	אם (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	אם (out_addr2)
		*out_addr2 = reg_addr;
	אם (out_size2)
		*out_size2 = reg_size;

	החזר 0;
}

صحيح fdt_parse_aclint_node(ثابت فراغ *fdt, صحيح nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	ثابت fdt32_t *val;
	صحيح i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	אם (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		החזר SBI_EINVAL;

	אם (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", عدم))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	אחרת
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	אם (rc)
		החזר rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	אם (!val || count < حجم(fdt32_t))
		החזר 0;
	count = count / حجم(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	עבור (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		אם (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		אם (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		אם (rc)
			استمر;

		אם (match_hwirq == hwirq) {
			אם (hartid < first_hartid)
				first_hartid = hartid;
			אם (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	אם ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	החזר 0;
}

صحيح fdt_parse_plmt_node(ثابت فراغ *fdt, صحيح nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	ثابت fdt32_t *val;
	صحيح rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	אם (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		החזר SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	אם (rc < 0)
		החזר SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	אם (!val || count < حجم(fdt32_t))
		החזר 0;
	count = count / حجم(fdt32_t);

	hcount = 0;
	עבור (i = 0; i < (count / 2); i++) {
		صحيح cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		אם (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		אם (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		אם (rc)
			استمر;

		אם (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			استمر;

		אם (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	החזר 0;
}

صحيح fdt_parse_plicsw_node(ثابت فراغ *fdt, صحيح nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	ثابت fdt32_t *val;
	صحيح rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	אם (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		החזר SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	אם (rc < 0)
		החזר SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	אם (!val || count < حجم(fdt32_t))
		החזר 0;
	count = count / حجم(fdt32_t);

	hcount = 0;
	עבור (i = 0; i < (count / 2); i++) {
		صحيح cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		אם (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		אם (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		אם (rc)
			استمر;

		אם (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			استمر;

		אם (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	החזר 0;
}

صحيح fdt_parse_compat_addr(ثابت فراغ *fdt, uint64_t *addr,
			  ثابت حرف *compatible)
{
	صحيح nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	אם (nodeoffset < 0)
		החזר nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, عدم);
	אם (rc < 0 || !addr)
		החזר SBI_ENODEV;

	החזר 0;
}
