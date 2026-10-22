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

	إذا (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		إرجاع SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	إذا (!list)
		إرجاع SBI_ENOENT;
	list_end = list + (len / حجم(*list));

	بينما (list < list_end) {
		pnodeoff = fdt_عقدة_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		إذا (pnodeoff < 0)
			إرجاع pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		إذا (!val)
			إرجاع SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		إذا (FDT_MAX_PHANDLE_ARGS < pcells)
			إرجاع SBI_EINVAL;
		إذا (list + pcells > list_end)
			إرجاع SBI_ENOENT;

		إذا (index > 0) {
			list += pcells;
			index--;
		} وإلا {
			out_args->عقدة_offset = pnodeoff;
			out_args->args_تعداد = pcells;
			لكل (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			إرجاع 0;
		}
	}

	إرجاع SBI_ENOENT;
}

ساكن صحيح fdt_translate_address(ثابت فراغ *fdt, uint64_t reg, صحيح parent,
				 uint64_t *addr)
{
	صحيح i, rlen;
	صحيح cell_parent_addr, cell_child_addr, cell_size;
	ثابت fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	إذا (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		إذا (cell_child_addr < 1)
			إرجاع SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		إذا (cell_parent_addr < 1)
			إرجاع SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		إذا (cell_size < 0)
			إرجاع SBI_ENODEV;

		لكل (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		لكل (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		لكل (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		إذا (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			إرجاع SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} وإلا {
		/* No translation required */
		*addr = reg;
	}

	إرجاع 0;
}

صحيح fdt_get_عقدة_addr_size(ثابت فراغ *fdt, صحيح عقدة, صحيح index,
			   uint64_t *addr, uint64_t *size)
{
	صحيح parent, len, i, rc;
	صحيح cell_addr, cell_size;
	ثابت fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	إذا (!fdt || عقدة < 0 || index < 0)
		إرجاع SBI_EINVAL;

	parent = fdt_parent_offset(fdt, عقدة);
	إذا (parent < 0)
		إرجاع parent;
	cell_addr = fdt_address_cells(fdt, parent);
	إذا (cell_addr < 1)
		إرجاع SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	إذا (cell_size < 0)
		إرجاع SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, عقدة, "reg", &len);
	إذا (!prop_addr)
		إرجاع SBI_ENODEV;

	إذا ((len / حجم(u32)) <= (index * (cell_addr + cell_size)))
		إرجاع SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	إذا (addr) {
		لكل (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		افعل {
			إذا (parent < 0)
				اكسر;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			إذا (rc)
				اكسر;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} بينما (1);
	}
	temp = 0;

	إذا (size) {
		لكل (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	إرجاع 0;
}

صحيح fdt_get_عقدة_addr_size_by_name(ثابت فراغ *fdt, صحيح عقدة, ثابت حرف *name,
				   uint64_t *addr, uint64_t *size)
{
	صحيح i, j, تعداد_2;
	ثابت حرف *val;
	ثابت حرف *regname;

	إذا (!fdt || عقدة < 0 || !name)
		إرجاع SBI_EINVAL;

	val = fdt_getprop(fdt, عقدة, "reg-names", &تعداد_2);
	إذا (!val)
		إرجاع SBI_ENODEV;

	لكل (i = 0, j = 0; i < تعداد_2; i++, j++) {
		regname = val + i;

		إذا (strcmp(name, regname) == 0)
			إرجاع fdt_get_عقدة_addr_size(fdt, عقدة, j, addr, size);

		i += strlen(regname);
	}

	إرجاع SBI_ENODEV;
}

bool fdt_عقدة_is_enabled(ثابت فراغ *fdt, صحيح nodeoff)
{
	صحيح len;
	ثابت فراغ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	إذا (!prop)
		إرجاع true;

	إذا (!strncmp(prop, "okay", strlen("okay")))
		إرجاع true;

	إذا (!strncmp(prop, "ok", strlen("ok")))
		إرجاع true;

	إرجاع false;
}

صحيح fdt_parse_hart_id(ثابت فراغ *fdt, صحيح cpu_offset, u32 *hartid)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_offset < 0)
		إرجاع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	إذا (!prop || !len)
		إرجاع SBI_EINVAL;
	إذا (strncmp (prop, "cpu", strlen ("cpu")))
		إرجاع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	إذا (!val || len < حجم(fdt32_t))
		إرجاع SBI_EINVAL;

	إذا (len > حجم(fdt32_t))
		val++;

	إذا (hartid)
		*hartid = fdt32_to_cpu(*val);

	إرجاع 0;
}

صحيح fdt_parse_cbom_block_size(ثابت فراغ *fdt, صحيح cpu_offset, unsigned long *cbom_block_size)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_offset < 0)
		إرجاع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	إذا (!prop || !len)
		إرجاع SBI_EINVAL;
	إذا (strncmp (prop, "cpu", strlen ("cpu")))
		إرجاع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	إذا (!val || len < حجم(fdt32_t))
		إرجاع SBI_EINVAL;

	إذا (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	إرجاع 0;
}

صحيح fdt_parse_max_enabled_hart_id(ثابت فراغ *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحيح err, cpu_offset, cpus_offset;

	إذا (!fdt)
		إرجاع SBI_EINVAL;
	إذا (!max_hartid)
		إرجاع 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	إذا (cpus_offset < 0)
		إرجاع cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_عقدة_is_enabled(fdt, cpu_offset))
			استمر;

		إذا (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	إرجاع 0;
}

صحيح fdt_parse_timebase_frequency(ثابت فراغ *fdt, unsigned long *freq)
{
	ثابت fdt32_t *val;
	صحيح len, cpus_offset;

	إذا (!fdt || !freq)
		إرجاع SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	إذا (cpus_offset < 0)
		إرجاع cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	إذا (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	وإلا
		إرجاع SBI_ENOENT;

	إرجاع 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

ساكن صحيح fdt_parse_isa_one_hart(ثابت حرف *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	حرف mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	إذا (isa[i] == 'r' || isa[i] == 'R')
		i++;
	وإلا
		إرجاع SBI_EINVAL;

	إذا (isa[i] == 'v' || isa[i] == 'V')
		i++;
	وإلا
		إرجاع SBI_EINVAL;

	إذا (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	وإلا إذا (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	وإلا
		إرجاع SBI_EINVAL;

	/* Skip base ISA extensions */
	لكل (; i < isa_len; i++) {
		إذا (isa[i] == '_')
			اكسر;
	}

	بينما (i < isa_len) {
		إذا (isa[i] != '_') {
			i++;
			استمر;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		بينما ((i < isa_len) && (isa[i] != '_') &&
		       (j < (حجم(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		إذا (!j)
			استمر;

#define set_multi_letter_ext(name, bit)				\
			إذا (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				استمر;			\
			}

		لكل (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	إرجاع 0;
}

ساكن فراغ fdt_parse_isa_extensions_one_hart(ثابت حرف *isa,
					      unsigned long *extensions,
					      صحيح len)
{
	size_t i;

	لكل (i = 0; i < SBI_HART_EXT_MAX; i++) {
		إذا (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
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

	إذا (!fdt)
		إرجاع SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	إذا (cpus_offset < 0)
		إرجاع cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_عقدة_is_enabled(fdt, cpu_offset))
			استمر;

		scratch = sbi_hartid_to_scratch(hartid);
		إذا (!scratch)
			إرجاع SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		إذا (!hfeatures)
			إرجاع SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		إذا (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت حرف *)val,
							  hfeatures->extensions, len);
			استمر;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		إذا (!val || len <= 0)
			إرجاع SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت حرف *)val, hfeatures->extensions);
		إذا (err)
			إرجاع err;
	}

	إرجاع 0;
}

ساكن صحيح fdt_parse_uart_عقدة_common(ثابت فراغ *fdt, صحيح nodeoffset,
				      بنية platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_size;

	إذا (nodeoffset < 0 || !uart || !fdt)
		إرجاع SBI_ENODEV;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	إذا (rc < 0 || !reg_size)
		إرجاع SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	إذا (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	وإلا
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	إذا (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	وإلا
		uart->baud = default_baud;

	إرجاع 0;
}

صحيح fdt_parse_gaisler_uart_عقدة(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_data *uart)
{
	إرجاع fdt_parse_uart_عقدة_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحيح fdt_parse_renesas_scif_عقدة(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_data *uart)
{
	إرجاع fdt_parse_uart_عقدة_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحيح fdt_parse_shakti_uart_عقدة(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	إرجاع fdt_parse_uart_عقدة_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحيح fdt_parse_sifive_uart_عقدة(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	إرجاع fdt_parse_uart_عقدة_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

صحيح fdt_parse_uart_عقدة(ثابت فراغ *fdt, صحيح nodeoffset,
			بنية platform_uart_data *uart)
{
	صحيح len, rc;
	ثابت fdt32_t *val;

	rc = fdt_parse_uart_عقدة_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	إذا (rc)
		إرجاع rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	إذا (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	وإلا
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	إذا (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	وإلا
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	إذا (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	وإلا
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	إرجاع 0;
}

صحيح fdt_parse_uart8250(ثابت فراغ *fdt, بنية platform_uart_data *uart,
		   ثابت حرف *compatible)
{
	صحيح nodeoffset;

	إذا (!compatible || !uart || !fdt)
		إرجاع SBI_ENODEV;

	nodeoffset = fdt_عقدة_offset_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		إرجاع nodeoffset;

	إرجاع fdt_parse_uart_عقدة(fdt, nodeoffset, uart);
}

صحيح fdt_parse_xlnx_uartlite_عقدة(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_data *uart)
{
	إرجاع fdt_parse_uart_عقدة_common(fdt, nodeoffset, uart, 0, 0);
}

ساكن صحيح fdt_aplic_find_imsic_عقدة(ثابت فراغ *fdt, صحيح nodeoff,
				     بنية imsic_data *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	إذا (val && len >= حجم(fdt32_t)) {
		noff = fdt_عقدة_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		إذا (noff < 0)
			إرجاع noff;

		rc = fdt_parse_imsic_عقدة(fdt, noff, imsic);
		إذا (rc)
			إرجاع rc;

		rc = imsic_data_check(imsic);
		إذا (rc)
			إرجاع rc;

		إذا (imsic->targets_mmode == mmode) {
			إرجاع 0;
		}
	} وإلا {
		إرجاع SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	إذا (!val || len < حجم(fdt32_t))
		إرجاع SBI_ENODEV;

	len /= حجم(fdt32_t);

	لكل (i = 0; i < len; i++) {
		noff = fdt_عقدة_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		إذا (noff < 0)
			إرجاع noff;

		rc = fdt_aplic_find_imsic_عقدة(fdt, noff, imsic, mmode);
		إذا (!rc)
			اكسر;
	}

	إرجاع rc;
}

صحيح fdt_parse_aplic_عقدة(ثابت فراغ *fdt, صحيح nodeoff, بنية aplic_data *aplic)
{
	bool child_found;
	ثابت fdt32_t *val;
	ثابت fdt32_t *del;
	بنية imsic_data imsic = { 0 };
	صحيح i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	بنية aplic_delegate_data *deleg;

	إذا (nodeoff < 0 || !aplic || !fdt)
		إرجاع SBI_ENODEV;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	إذا (rc < 0 || !reg_size)
		إرجاع SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	إذا (len > 0)
		aplic->رقم_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	إذا (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		لكل (i = 0; i < len; i += 2) {
			إذا (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				اكسر;
			}
		}
		aplic->رقم_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_عقدة(fdt, nodeoff, &imsic, true);
	إذا (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		إذا (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			إرجاع SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_عقدة(fdt, nodeoff, &imsic, false);
	إذا (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		إذا (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			إرجاع SBI_EINVAL;
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
	إذا (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	إذا (!del || len < (3 * حجم(fdt32_t)))
		اذهب skip_delegate_parse;
	d = 0;
	dcnt = len / حجم(fdt32_t);
	لكل (i = 0; i < dcnt; i += 3) {
		إذا (d >= APLIC_MAX_DELEGATE)
			اكسر;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		إذا (!val || len < حجم(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			استمر;
		}
		len = len / حجم(fdt32_t);
		لكل (j = 0; j < len; j++) {
			إذا (del[i] != val[j])
				استمر;
			deleg->child_index = j;
			child_found = true;
			اكسر;
		}

		إذا (child_found) {
			d++;
		} وإلا {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	إرجاع 0;
}

bool fdt_check_imsic_mlevel(ثابت فراغ *fdt)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff = 0;

	إذا (!fdt)
		إرجاع false;

	بينما ((noff = fdt_عقدة_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		إذا (val && len > حجم(fdt32_t)) {
			len = len / حجم(fdt32_t);
			لكل (i = 0; i < len; i += 2) {
				إذا (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					إرجاع true;
			}
		}
	}

	إرجاع false;
}

صحيح fdt_parse_imsic_عقدة(ثابت فراغ *fdt, صحيح nodeoff, بنية imsic_data *imsic)
{
	ثابت fdt32_t *val;
	بنية imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	صحيح i, rc, len, nr_parent_irqs;

	إذا (nodeoff < 0 || !imsic || !fdt)
		إرجاع SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	إذا (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		nr_parent_irqs = len / 2;
		لكل (i = 0; i < len; i += 2) {
			إذا (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				اكسر;
			}
		}
	} وإلا
		إرجاع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	إذا (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	وإلا
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	إذا (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} وإلا {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		إذا ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	إذا (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	وإلا
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	إذا (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	وإلا
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	إذا (val && len > 0)
		imsic->رقم_ids = fdt32_to_cpu(*val);
	وإلا
		إرجاع SBI_EINVAL;

	لكل (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	لكل (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_عقدة_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		إذا (rc < 0 || !reg_size)
			اكسر;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	إذا (!imsic->regs[0].size)
		إرجاع SBI_EINVAL;

	إرجاع 0;
}

صحيح fdt_parse_plic_عقدة(ثابت فراغ *fdt, صحيح nodeoffset, بنية plic_data *plic)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_size;

	إذا (nodeoffset < 0 || !plic || !fdt)
		إرجاع SBI_ENODEV;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	إذا (rc < 0 || !reg_size)
		إرجاع SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	إذا (len > 0)
		plic->رقم_src = fdt32_to_cpu(*val);

	إرجاع 0;
}

صحيح fdt_parse_plic(ثابت فراغ *fdt, بنية plic_data *plic, ثابت حرف *compat)
{
	صحيح nodeoffset;

	إذا (!compat || !plic || !fdt)
		إرجاع SBI_ENODEV;

	nodeoffset = fdt_عقدة_offset_by_compatible(fdt, -1, compat);
	إذا (nodeoffset < 0)
		إرجاع nodeoffset;

	إرجاع fdt_parse_plic_عقدة(fdt, nodeoffset, plic);
}

ساكن صحيح fdt_get_aclint_addr_size_by_name(ثابت فراغ *fdt, صحيح nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_عقدة_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	إذا (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_عقدة_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	إذا (rc < 0 || !reg_size)
		إرجاع SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	إرجاع 0;
}

ساكن صحيح fdt_get_aclint_addr_size(ثابت فراغ *fdt, صحيح nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	إذا (rc < 0 || !reg_size)
		إرجاع SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	إذا (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	إذا (out_addr2)
		*out_addr2 = reg_addr;
	إذا (out_size2)
		*out_size2 = reg_size;

	إرجاع 0;
}

صحيح fdt_parse_aclint_عقدة(ثابت فراغ *fdt, صحيح nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_تعداد)
{
	ثابت fdt32_t *val;
	صحيح i, rc, تعداد_2, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	إذا (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_تعداد)
		إرجاع SBI_EINVAL;

	إذا (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", عدم))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	وإلا
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	إذا (rc)
		إرجاع rc;

	*out_first_hartid = 0;
	*out_hart_تعداد = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &تعداد_2);
	إذا (!val || تعداد_2 < حجم(fdt32_t))
		إرجاع 0;
	تعداد_2 = تعداد_2 / حجم(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	لكل (i = 0; i < (تعداد_2 / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_عقدة_offset_by_phandle(fdt, phandle);
		إذا (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		إذا (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		إذا (rc)
			استمر;

		إذا (match_hwirq == hwirq) {
			إذا (hartid < first_hartid)
				first_hartid = hartid;
			إذا (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	إذا ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_تعداد = last_hartid - first_hartid + 1;
	}
	إرجاع 0;
}

صحيح fdt_parse_plmt_عقدة(ثابت فراغ *fdt, صحيح nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_تعداد)
{
	ثابت fdt32_t *val;
	صحيح rc, i, تعداد_2;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_تعداد || !plmt_size)
		إرجاع SBI_EINVAL;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	إذا (rc < 0)
		إرجاع SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &تعداد_2);
	إذا (!val || تعداد_2 < حجم(fdt32_t))
		إرجاع 0;
	تعداد_2 = تعداد_2 / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (تعداد_2 / 2); i++) {
		صحيح cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_عقدة_offset_by_phandle(fdt, phandle);
		إذا (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		إذا (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_تعداد = hcount;

	إرجاع 0;
}

صحيح fdt_parse_plicsw_عقدة(ثابت فراغ *fdt, صحيح nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_تعداد)
{
	ثابت fdt32_t *val;
	صحيح rc, i, تعداد_2;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_تعداد || !size)
		إرجاع SBI_EINVAL;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	إذا (rc < 0)
		إرجاع SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &تعداد_2);
	إذا (!val || تعداد_2 < حجم(fdt32_t))
		إرجاع 0;
	تعداد_2 = تعداد_2 / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (تعداد_2 / 2); i++) {
		صحيح cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_عقدة_offset_by_phandle(fdt, phandle);
		إذا (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		إذا (cpu_offset < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_تعداد = hcount;

	إرجاع 0;
}

صحيح fdt_parse_compat_addr(ثابت فراغ *fdt, uint64_t *addr,
			  ثابت حرف *compatible)
{
	صحيح nodeoffset, rc;

	nodeoffset = fdt_عقدة_offset_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		إرجاع nodeoffset;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoffset, 0, addr, عدم);
	إذا (rc < 0 || !addr)
		إرجاع SBI_ENODEV;

	إرجاع 0;
}
