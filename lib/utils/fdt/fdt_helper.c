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

	𐡄𐡍 (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		أرجع SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	𐡄𐡍 (!list)
		أرجع SBI_ENOENT;
	list_end = list + (len / حجم(*list));

	طالما (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		𐡄𐡍 (pnodeoff < 0)
			أرجع pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		𐡄𐡍 (!val)
			أرجع SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		𐡄𐡍 (FDT_MAX_PHANDLE_ARGS < pcells)
			أرجع SBI_EINVAL;
		𐡄𐡍 (list + pcells > list_end)
			أرجع SBI_ENOENT;

		𐡄𐡍 (index > 0) {
			list += pcells;
			index--;
		} وإلا {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			لكل (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			أرجع 0;
		}
	}

	أرجع SBI_ENOENT;
}

ساكن صحيح fdt_translate_address(ثابت فراغ *fdt, uint64_t reg, صحيح parent,
				 uint64_t *addr)
{
	صحيح i, rlen;
	صحيح cell_parent_addr, cell_child_addr, cell_size;
	ثابت fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	𐡄𐡍 (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		𐡄𐡍 (cell_child_addr < 1)
			أرجع SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		𐡄𐡍 (cell_parent_addr < 1)
			أرجع SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		𐡄𐡍 (cell_size < 0)
			أرجع SBI_ENODEV;

		لكل (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		لكل (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		لكل (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		𐡄𐡍 (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			أرجع SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} وإلا {
		/* No translation required */
		*addr = reg;
	}

	أرجع 0;
}

صحيح fdt_get_node_addr_size(ثابت فراغ *fdt, صحيح node, صحيح index,
			   uint64_t *addr, uint64_t *size)
{
	صحيح parent, len, i, rc;
	صحيح cell_addr, cell_size;
	ثابت fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	𐡄𐡍 (!fdt || node < 0 || index < 0)
		أرجع SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	𐡄𐡍 (parent < 0)
		أرجع parent;
	cell_addr = fdt_address_cells(fdt, parent);
	𐡄𐡍 (cell_addr < 1)
		أرجع SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	𐡄𐡍 (cell_size < 0)
		أرجع SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	𐡄𐡍 (!prop_addr)
		أرجع SBI_ENODEV;

	𐡄𐡍 ((len / حجم(u32)) <= (index * (cell_addr + cell_size)))
		أرجع SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	𐡄𐡍 (addr) {
		لكل (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		𐡏𐡁𐡃 {
			𐡄𐡍 (parent < 0)
				اكسر;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			𐡄𐡍 (rc)
				اكسر;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} طالما (1);
	}
	temp = 0;

	𐡄𐡍 (size) {
		لكل (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	أرجع 0;
}

صحيح fdt_get_node_addr_size_by_name(ثابت فراغ *fdt, صحيح node, ثابت حرف *name,
				   uint64_t *addr, uint64_t *size)
{
	صحيح i, j, count;
	ثابت حرف *val;
	ثابت حرف *regname;

	𐡄𐡍 (!fdt || node < 0 || !name)
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	𐡄𐡍 (!val)
		أرجع SBI_ENODEV;

	لكل (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		𐡄𐡍 (strcmp(name, regname) == 0)
			أرجع fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	أرجع SBI_ENODEV;
}

bool fdt_node_is_enabled(ثابت فراغ *fdt, صحيح nodeoff)
{
	صحيح len;
	ثابت فراغ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	𐡄𐡍 (!prop)
		أرجع true;

	𐡄𐡍 (!strncmp(prop, "okay", strlen("okay")))
		أرجع true;

	𐡄𐡍 (!strncmp(prop, "ok", strlen("ok")))
		أرجع true;

	أرجع false;
}

صحيح fdt_parse_hart_id(ثابت فراغ *fdt, صحيح cpu_offset, u32 *hartid)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	𐡄𐡍 (!fdt || cpu_offset < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	𐡄𐡍 (!prop || !len)
		أرجع SBI_EINVAL;
	𐡄𐡍 (strncmp (prop, "cpu", strlen ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	𐡄𐡍 (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	𐡄𐡍 (len > حجم(fdt32_t))
		val++;

	𐡄𐡍 (hartid)
		*hartid = fdt32_to_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_cbom_block_size(ثابت فراغ *fdt, صحيح cpu_offset, unsigned long *cbom_block_size)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	𐡄𐡍 (!fdt || cpu_offset < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	𐡄𐡍 (!prop || !len)
		أرجع SBI_EINVAL;
	𐡄𐡍 (strncmp (prop, "cpu", strlen ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	𐡄𐡍 (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	𐡄𐡍 (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	أرجع 0;
}

صحيح fdt_parse_max_enabled_hart_id(ثابت فراغ *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحيح err, cpu_offset, cpus_offset;

	𐡄𐡍 (!fdt)
		أرجع SBI_EINVAL;
	𐡄𐡍 (!max_hartid)
		أرجع 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	𐡄𐡍 (cpus_offset < 0)
		أرجع cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		𐡄𐡍 (err)
			استمر;

		𐡄𐡍 (!fdt_node_is_enabled(fdt, cpu_offset))
			استمر;

		𐡄𐡍 (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	أرجع 0;
}

صحيح fdt_parse_timebase_frequency(ثابت فراغ *fdt, unsigned long *freq)
{
	ثابت fdt32_t *val;
	صحيح len, cpus_offset;

	𐡄𐡍 (!fdt || !freq)
		أرجع SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	𐡄𐡍 (cpus_offset < 0)
		أرجع cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	𐡄𐡍 (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	وإلا
		أرجع SBI_ENOENT;

	أرجع 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

ساكن صحيح fdt_parse_isa_one_hart(ثابت حرف *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	حرف mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	𐡄𐡍 (isa[i] == 'r' || isa[i] == 'R')
		i++;
	وإلا
		أرجع SBI_EINVAL;

	𐡄𐡍 (isa[i] == 'v' || isa[i] == 'V')
		i++;
	وإلا
		أرجع SBI_EINVAL;

	𐡄𐡍 (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	وإلا 𐡄𐡍 (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	وإلا
		أرجع SBI_EINVAL;

	/* Skip base ISA extensions */
	لكل (; i < isa_len; i++) {
		𐡄𐡍 (isa[i] == '_')
			اكسر;
	}

	طالما (i < isa_len) {
		𐡄𐡍 (isa[i] != '_') {
			i++;
			استمر;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		طالما ((i < isa_len) && (isa[i] != '_') &&
		       (j < (حجم(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		𐡄𐡍 (!j)
			استمر;

#define set_multi_letter_ext(name, bit)				\
			𐡄𐡍 (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				استمر;			\
			}

		لكل (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	أرجع 0;
}

ساكن فراغ fdt_parse_isa_extensions_one_hart(ثابت حرف *isa,
					      unsigned long *extensions,
					      صحيح len)
{
	size_t i;

	لكل (i = 0; i < SBI_HART_EXT_MAX; i++) {
		𐡄𐡍 (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
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

	𐡄𐡍 (!fdt)
		أرجع SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	𐡄𐡍 (cpus_offset < 0)
		أرجع cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		𐡄𐡍 (err)
			استمر;

		𐡄𐡍 (!fdt_node_is_enabled(fdt, cpu_offset))
			استمر;

		scratch = sbi_hartid_to_scratch(hartid);
		𐡄𐡍 (!scratch)
			أرجع SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		𐡄𐡍 (!hfeatures)
			أرجع SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		𐡄𐡍 (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت حرف *)val,
							  hfeatures->extensions, len);
			استمر;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		𐡄𐡍 (!val || len <= 0)
			أرجع SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت حرف *)val, hfeatures->extensions);
		𐡄𐡍 (err)
			أرجع err;
	}

	أرجع 0;
}

ساكن صحيح fdt_parse_uart_node_common(ثابت فراغ *fdt, صحيح nodeoffset,
				      بنية platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_size;

	𐡄𐡍 (nodeoffset < 0 || !uart || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	𐡄𐡍 (rc < 0 || !reg_size)
		أرجع SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	𐡄𐡍 (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	وإلا
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	𐡄𐡍 (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	وإلا
		uart->baud = default_baud;

	أرجع 0;
}

صحيح fdt_parse_gaisler_uart_node(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_data *uart)
{
	أرجع fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحيح fdt_parse_renesas_scif_node(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_data *uart)
{
	أرجع fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحيح fdt_parse_shakti_uart_node(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	أرجع fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحيح fdt_parse_sifive_uart_node(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	أرجع fdt_parse_uart_node_common(fdt, nodeoffset, uart,
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
	𐡄𐡍 (rc)
		أرجع rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	𐡄𐡍 (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	وإلا
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	𐡄𐡍 (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	وإلا
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	𐡄𐡍 (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	وإلا
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	أرجع 0;
}

صحيح fdt_parse_uart8250(ثابت فراغ *fdt, بنية platform_uart_data *uart,
		   ثابت حرف *compatible)
{
	صحيح nodeoffset;

	𐡄𐡍 (!compatible || !uart || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	𐡄𐡍 (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_uart_node(fdt, nodeoffset, uart);
}

صحيح fdt_parse_xlnx_uartlite_node(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	أرجع fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

ساكن صحيح fdt_aplic_find_imsic_node(ثابت فراغ *fdt, صحيح nodeoff,
				     بنية imsic_data *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	𐡄𐡍 (val && len >= حجم(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		𐡄𐡍 (noff < 0)
			أرجع noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		𐡄𐡍 (rc)
			أرجع rc;

		rc = imsic_data_check(imsic);
		𐡄𐡍 (rc)
			أرجع rc;

		𐡄𐡍 (imsic->targets_mmode == mmode) {
			أرجع 0;
		}
	} وإلا {
		أرجع SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	𐡄𐡍 (!val || len < حجم(fdt32_t))
		أرجع SBI_ENODEV;

	len /= حجم(fdt32_t);

	لكل (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		𐡄𐡍 (noff < 0)
			أرجع noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		𐡄𐡍 (!rc)
			اكسر;
	}

	أرجع rc;
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

	𐡄𐡍 (nodeoff < 0 || !aplic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	𐡄𐡍 (rc < 0 || !reg_size)
		أرجع SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	𐡄𐡍 (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	𐡄𐡍 (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		لكل (i = 0; i < len; i += 2) {
			𐡄𐡍 (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				اكسر;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	𐡄𐡍 (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		𐡄𐡍 (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			أرجع SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	𐡄𐡍 (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		𐡄𐡍 (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			أرجع SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	لكل (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	𐡄𐡍 (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	𐡄𐡍 (!del || len < (3 * حجم(fdt32_t)))
		اذهب skip_delegate_parse;
	d = 0;
	dcnt = len / حجم(fdt32_t);
	لكل (i = 0; i < dcnt; i += 3) {
		𐡄𐡍 (d >= APLIC_MAX_DELEGATE)
			اكسر;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		𐡄𐡍 (!val || len < حجم(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			استمر;
		}
		len = len / حجم(fdt32_t);
		لكل (j = 0; j < len; j++) {
			𐡄𐡍 (del[i] != val[j])
				استمر;
			deleg->child_index = j;
			child_found = true;
			اكسر;
		}

		𐡄𐡍 (child_found) {
			d++;
		} وإلا {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	أرجع 0;
}

bool fdt_check_imsic_mlevel(ثابت فراغ *fdt)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff = 0;

	𐡄𐡍 (!fdt)
		أرجع false;

	طالما ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		𐡄𐡍 (val && len > حجم(fdt32_t)) {
			len = len / حجم(fdt32_t);
			لكل (i = 0; i < len; i += 2) {
				𐡄𐡍 (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					أرجع true;
			}
		}
	}

	أرجع false;
}

صحيح fdt_parse_imsic_node(ثابت فراغ *fdt, صحيح nodeoff, بنية imsic_data *imsic)
{
	ثابت fdt32_t *val;
	بنية imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	صحيح i, rc, len, nr_parent_irqs;

	𐡄𐡍 (nodeoff < 0 || !imsic || !fdt)
		أرجع SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	𐡄𐡍 (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		nr_parent_irqs = len / 2;
		لكل (i = 0; i < len; i += 2) {
			𐡄𐡍 (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				اكسر;
			}
		}
	} وإلا
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	𐡄𐡍 (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	وإلا
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	𐡄𐡍 (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} وإلا {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		𐡄𐡍 ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	𐡄𐡍 (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	وإلا
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	𐡄𐡍 (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	وإلا
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	𐡄𐡍 (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	وإلا
		أرجع SBI_EINVAL;

	لكل (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	لكل (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		𐡄𐡍 (rc < 0 || !reg_size)
			اكسر;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	𐡄𐡍 (!imsic->regs[0].size)
		أرجع SBI_EINVAL;

	أرجع 0;
}

صحيح fdt_parse_plic_node(ثابت فراغ *fdt, صحيح nodeoffset, بنية plic_data *plic)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_size;

	𐡄𐡍 (nodeoffset < 0 || !plic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	𐡄𐡍 (rc < 0 || !reg_size)
		أرجع SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	𐡄𐡍 (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_plic(ثابت فراغ *fdt, بنية plic_data *plic, ثابت حرف *compat)
{
	صحيح nodeoffset;

	𐡄𐡍 (!compat || !plic || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	𐡄𐡍 (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_plic_node(fdt, nodeoffset, plic);
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
	𐡄𐡍 (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	𐡄𐡍 (rc < 0 || !reg_size)
		أرجع SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	أرجع 0;
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
	𐡄𐡍 (rc < 0 || !reg_size)
		أرجع SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	𐡄𐡍 (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	𐡄𐡍 (out_addr2)
		*out_addr2 = reg_addr;
	𐡄𐡍 (out_size2)
		*out_size2 = reg_size;

	أرجع 0;
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

	𐡄𐡍 (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		أرجع SBI_EINVAL;

	𐡄𐡍 (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", عدم))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	وإلا
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	𐡄𐡍 (rc)
		أرجع rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	𐡄𐡍 (!val || count < حجم(fdt32_t))
		أرجع 0;
	count = count / حجم(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	لكل (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		𐡄𐡍 (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		𐡄𐡍 (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		𐡄𐡍 (rc)
			استمر;

		𐡄𐡍 (match_hwirq == hwirq) {
			𐡄𐡍 (hartid < first_hartid)
				first_hartid = hartid;
			𐡄𐡍 (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	𐡄𐡍 ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	أرجع 0;
}

صحيح fdt_parse_plmt_node(ثابت فراغ *fdt, صحيح nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	ثابت fdt32_t *val;
	صحيح rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	𐡄𐡍 (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		أرجع SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	𐡄𐡍 (rc < 0)
		أرجع SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	𐡄𐡍 (!val || count < حجم(fdt32_t))
		أرجع 0;
	count = count / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (count / 2); i++) {
		صحيح cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		𐡄𐡍 (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		𐡄𐡍 (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		𐡄𐡍 (rc)
			استمر;

		𐡄𐡍 (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			استمر;

		𐡄𐡍 (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	أرجع 0;
}

صحيح fdt_parse_plicsw_node(ثابت فراغ *fdt, صحيح nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	ثابت fdt32_t *val;
	صحيح rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	𐡄𐡍 (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		أرجع SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	𐡄𐡍 (rc < 0)
		أرجع SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	𐡄𐡍 (!val || count < حجم(fdt32_t))
		أرجع 0;
	count = count / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (count / 2); i++) {
		صحيح cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		𐡄𐡍 (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		𐡄𐡍 (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		𐡄𐡍 (rc)
			استمر;

		𐡄𐡍 (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			استمر;

		𐡄𐡍 (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	أرجع 0;
}

صحيح fdt_parse_compat_addr(ثابت فراغ *fdt, uint64_t *addr,
			  ثابت حرف *compatible)
{
	صحيح nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	𐡄𐡍 (nodeoffset < 0)
		أرجع nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, عدم);
	𐡄𐡍 (rc < 0 || !addr)
		أرجع SBI_ENODEV;

	أرجع 0;
}
