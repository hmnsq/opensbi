// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/irqchip/aplic.h>
#شامل <sbi_utils/irqchip/imsic.h>
#شامل <sbi_utils/irqchip/plic.h>

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

صحیح fdt_parse_phandle_with_args(ثابت تهی *fdt, صحیح nodeoff,
				ثابت نویسه *prop, ثابت نویسه *cells_prop,
				صحیح index, ساختار fdt_phandle_args *out_args)
{
	u32 i, pcells;
	صحیح len, pnodeoff;
	ثابت fdt32_t *list, *list_end, *val;

	اگر (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		بازگشت SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	اگر (!list)
		بازگشت SBI_ENOENT;
	list_end = list + (len / اندازه(*list));

	وقتی (list < list_end) {
		pnodeoff = fdt_گره_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		اگر (pnodeoff < 0)
			بازگشت pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		اگر (!val)
			بازگشت SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		اگر (FDT_MAX_PHANDLE_ARGS < pcells)
			بازگشت SBI_EINVAL;
		اگر (list + pcells > list_end)
			بازگشت SBI_ENOENT;

		اگر (index > 0) {
			list += pcells;
			index--;
		} وگرنه {
			out_args->گره_offset = pnodeoff;
			out_args->args_شمار = pcells;
			برای (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			بازگشت 0;
		}
	}

	بازگشت SBI_ENOENT;
}

ایستا صحیح fdt_translate_address(ثابت تهی *fdt, uint64_t reg, صحیح parent,
				 uint64_t *addr)
{
	صحیح i, rlen;
	صحیح cell_parent_addr, cell_child_addr, cell_size;
	ثابت fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	اگر (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		اگر (cell_child_addr < 1)
			بازگشت SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		اگر (cell_parent_addr < 1)
			بازگشت SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		اگر (cell_size < 0)
			بازگشت SBI_ENODEV;

		برای (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		برای (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		برای (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		اگر (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			بازگشت SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} وگرنه {
		/* No translation required */
		*addr = reg;
	}

	بازگشت 0;
}

صحیح fdt_get_گره_addr_size(ثابت تهی *fdt, صحیح گره, صحیح index,
			   uint64_t *addr, uint64_t *size)
{
	صحیح parent, len, i, rc;
	صحیح cell_addr, cell_size;
	ثابت fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	اگر (!fdt || گره < 0 || index < 0)
		بازگشت SBI_EINVAL;

	parent = fdt_parent_offset(fdt, گره);
	اگر (parent < 0)
		بازگشت parent;
	cell_addr = fdt_address_cells(fdt, parent);
	اگر (cell_addr < 1)
		بازگشت SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	اگر (cell_size < 0)
		بازگشت SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, گره, "reg", &len);
	اگر (!prop_addr)
		بازگشت SBI_ENODEV;

	اگر ((len / اندازه(u32)) <= (index * (cell_addr + cell_size)))
		بازگشت SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	اگر (addr) {
		برای (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		انجامبده {
			اگر (parent < 0)
				بشکن;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			اگر (rc)
				بشکن;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} وقتی (1);
	}
	temp = 0;

	اگر (size) {
		برای (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	بازگشت 0;
}

صحیح fdt_get_گره_addr_size_by_name(ثابت تهی *fdt, صحیح گره, ثابت نویسه *name,
				   uint64_t *addr, uint64_t *size)
{
	صحیح i, j, شمار;
	ثابت نویسه *val;
	ثابت نویسه *regname;

	اگر (!fdt || گره < 0 || !name)
		بازگشت SBI_EINVAL;

	val = fdt_getprop(fdt, گره, "reg-names", &شمار);
	اگر (!val)
		بازگشت SBI_ENODEV;

	برای (i = 0, j = 0; i < شمار; i++, j++) {
		regname = val + i;

		اگر (strcmp(name, regname) == 0)
			بازگشت fdt_get_گره_addr_size(fdt, گره, j, addr, size);

		i += strlen(regname);
	}

	بازگشت SBI_ENODEV;
}

bool fdt_گره_is_enabled(ثابت تهی *fdt, صحیح nodeoff)
{
	صحیح len;
	ثابت تهی *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	اگر (!prop)
		بازگشت true;

	اگر (!strncmp(prop, "okay", strlen("okay")))
		بازگشت true;

	اگر (!strncmp(prop, "ok", strlen("ok")))
		بازگشت true;

	بازگشت false;
}

صحیح fdt_parse_hart_id(ثابت تهی *fdt, صحیح cpu_offset, u32 *hartid)
{
	صحیح len;
	ثابت تهی *prop;
	ثابت fdt32_t *val;

	اگر (!fdt || cpu_offset < 0)
		بازگشت SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	اگر (!prop || !len)
		بازگشت SBI_EINVAL;
	اگر (strncmp (prop, "cpu", strlen ("cpu")))
		بازگشت SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	اگر (!val || len < اندازه(fdt32_t))
		بازگشت SBI_EINVAL;

	اگر (len > اندازه(fdt32_t))
		val++;

	اگر (hartid)
		*hartid = fdt32_to_cpu(*val);

	بازگشت 0;
}

صحیح fdt_parse_cbom_block_size(ثابت تهی *fdt, صحیح cpu_offset, unsigned long *cbom_block_size)
{
	صحیح len;
	ثابت تهی *prop;
	ثابت fdt32_t *val;

	اگر (!fdt || cpu_offset < 0)
		بازگشت SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	اگر (!prop || !len)
		بازگشت SBI_EINVAL;
	اگر (strncmp (prop, "cpu", strlen ("cpu")))
		بازگشت SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	اگر (!val || len < اندازه(fdt32_t))
		بازگشت SBI_EINVAL;

	اگر (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	بازگشت 0;
}

صحیح fdt_parse_max_enabled_hart_id(ثابت تهی *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحیح err, cpu_offset, cpus_offset;

	اگر (!fdt)
		بازگشت SBI_EINVAL;
	اگر (!max_hartid)
		بازگشت 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	اگر (cpus_offset < 0)
		بازگشت cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		اگر (err)
			ادامه;

		اگر (!fdt_گره_is_enabled(fdt, cpu_offset))
			ادامه;

		اگر (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	بازگشت 0;
}

صحیح fdt_parse_timebase_frequency(ثابت تهی *fdt, unsigned long *freq)
{
	ثابت fdt32_t *val;
	صحیح len, cpus_offset;

	اگر (!fdt || !freq)
		بازگشت SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	اگر (cpus_offset < 0)
		بازگشت cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	اگر (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	وگرنه
		بازگشت SBI_ENOENT;

	بازگشت 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

ایستا صحیح fdt_parse_isa_one_hart(ثابت نویسه *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	نویسه mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	اگر (isa[i] == 'r' || isa[i] == 'R')
		i++;
	وگرنه
		بازگشت SBI_EINVAL;

	اگر (isa[i] == 'v' || isa[i] == 'V')
		i++;
	وگرنه
		بازگشت SBI_EINVAL;

	اگر (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	وگرنه اگر (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	وگرنه
		بازگشت SBI_EINVAL;

	/* Skip base ISA extensions */
	برای (; i < isa_len; i++) {
		اگر (isa[i] == '_')
			بشکن;
	}

	وقتی (i < isa_len) {
		اگر (isa[i] != '_') {
			i++;
			ادامه;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		وقتی ((i < isa_len) && (isa[i] != '_') &&
		       (j < (اندازه(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		اگر (!j)
			ادامه;

#define set_multi_letter_ext(name, bit)				\
			اگر (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				ادامه;			\
			}

		برای (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	بازگشت 0;
}

ایستا تهی fdt_parse_isa_extensions_one_hart(ثابت نویسه *isa,
					      unsigned long *extensions,
					      صحیح len)
{
	size_t i;

	برای (i = 0; i < SBI_HART_EXT_MAX; i++) {
		اگر (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

صحیح fdt_parse_isa_extensions_all_harts(ثابت تهی *fdt)
{
	u32 hartid;
	ثابت fdt32_t *val;
	ساختار sbi_scratch *scratch;
	ساختار sbi_hart_features *hfeatures;
	صحیح err, cpu_offset, cpus_offset, len;

	اگر (!fdt)
		بازگشت SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	اگر (cpus_offset < 0)
		بازگشت cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		اگر (err)
			ادامه;

		اگر (!fdt_گره_is_enabled(fdt, cpu_offset))
			ادامه;

		scratch = sbi_hartid_to_scratch(hartid);
		اگر (!scratch)
			بازگشت SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		اگر (!hfeatures)
			بازگشت SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		اگر (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت نویسه *)val,
							  hfeatures->extensions, len);
			ادامه;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		اگر (!val || len <= 0)
			بازگشت SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت نویسه *)val, hfeatures->extensions);
		اگر (err)
			بازگشت err;
	}

	بازگشت 0;
}

ایستا صحیح fdt_parse_uart_گره_common(ثابت تهی *fdt, صحیح nodeoffset,
				      ساختار platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	صحیح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_size;

	اگر (nodeoffset < 0 || !uart || !fdt)
		بازگشت SBI_ENODEV;

	rc = fdt_get_گره_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_size)
		بازگشت SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	اگر (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	وگرنه
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	اگر (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	وگرنه
		uart->baud = default_baud;

	بازگشت 0;
}

صحیح fdt_parse_gaisler_uart_گره(ثابت تهی *fdt, صحیح nodeoffset,
				ساختار platform_uart_data *uart)
{
	بازگشت fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحیح fdt_parse_renesas_scif_گره(ثابت تهی *fdt, صحیح nodeoffset,
				ساختار platform_uart_data *uart)
{
	بازگشت fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحیح fdt_parse_shakti_uart_گره(ثابت تهی *fdt, صحیح nodeoffset,
			       ساختار platform_uart_data *uart)
{
	بازگشت fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحیح fdt_parse_sifive_uart_گره(ثابت تهی *fdt, صحیح nodeoffset,
			       ساختار platform_uart_data *uart)
{
	بازگشت fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

صحیح fdt_parse_uart_گره(ثابت تهی *fdt, صحیح nodeoffset,
			ساختار platform_uart_data *uart)
{
	صحیح len, rc;
	ثابت fdt32_t *val;

	rc = fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	اگر (rc)
		بازگشت rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	اگر (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	وگرنه
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	اگر (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	وگرنه
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	اگر (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	وگرنه
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	بازگشت 0;
}

صحیح fdt_parse_uart8250(ثابت تهی *fdt, ساختار platform_uart_data *uart,
		   ثابت نویسه *compatible)
{
	صحیح nodeoffset;

	اگر (!compatible || !uart || !fdt)
		بازگشت SBI_ENODEV;

	nodeoffset = fdt_گره_offset_by_compatible(fdt, -1, compatible);
	اگر (nodeoffset < 0)
		بازگشت nodeoffset;

	بازگشت fdt_parse_uart_گره(fdt, nodeoffset, uart);
}

صحیح fdt_parse_xlnx_uartlite_گره(ثابت تهی *fdt, صحیح nodeoffset,
			       ساختار platform_uart_data *uart)
{
	بازگشت fdt_parse_uart_گره_common(fdt, nodeoffset, uart, 0, 0);
}

ایستا صحیح fdt_aplic_find_imsic_گره(ثابت تهی *fdt, صحیح nodeoff,
				     ساختار imsic_data *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحیح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	اگر (val && len >= اندازه(fdt32_t)) {
		noff = fdt_گره_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		اگر (noff < 0)
			بازگشت noff;

		rc = fdt_parse_imsic_گره(fdt, noff, imsic);
		اگر (rc)
			بازگشت rc;

		rc = imsic_data_check(imsic);
		اگر (rc)
			بازگشت rc;

		اگر (imsic->targets_mmode == mmode) {
			بازگشت 0;
		}
	} وگرنه {
		بازگشت SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	اگر (!val || len < اندازه(fdt32_t))
		بازگشت SBI_ENODEV;

	len /= اندازه(fdt32_t);

	برای (i = 0; i < len; i++) {
		noff = fdt_گره_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		اگر (noff < 0)
			بازگشت noff;

		rc = fdt_aplic_find_imsic_گره(fdt, noff, imsic, mmode);
		اگر (!rc)
			بشکن;
	}

	بازگشت rc;
}

صحیح fdt_parse_aplic_گره(ثابت تهی *fdt, صحیح nodeoff, ساختار aplic_data *aplic)
{
	bool child_found;
	ثابت fdt32_t *val;
	ثابت fdt32_t *del;
	ساختار imsic_data imsic = { 0 };
	صحیح i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	ساختار aplic_delegate_data *deleg;

	اگر (nodeoff < 0 || !aplic || !fdt)
		بازگشت SBI_ENODEV;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_size)
		بازگشت SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	اگر (len > 0)
		aplic->رقم_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	اگر (val && len > اندازه(fdt32_t)) {
		len = len / اندازه(fdt32_t);
		برای (i = 0; i < len; i += 2) {
			اگر (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				بشکن;
			}
		}
		aplic->رقم_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_گره(fdt, nodeoff, &imsic, true);
	اگر (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		اگر (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			بازگشت SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_گره(fdt, nodeoff, &imsic, false);
	اگر (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		اگر (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			بازگشت SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	برای (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	اگر (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	اگر (!del || len < (3 * اندازه(fdt32_t)))
		برو skip_delegate_parse;
	d = 0;
	dcnt = len / اندازه(fdt32_t);
	برای (i = 0; i < dcnt; i += 3) {
		اگر (d >= APLIC_MAX_DELEGATE)
			بشکن;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		اگر (!val || len < اندازه(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			ادامه;
		}
		len = len / اندازه(fdt32_t);
		برای (j = 0; j < len; j++) {
			اگر (del[i] != val[j])
				ادامه;
			deleg->child_index = j;
			child_found = true;
			بشکن;
		}

		اگر (child_found) {
			d++;
		} وگرنه {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	بازگشت 0;
}

bool fdt_check_imsic_mlevel(ثابت تهی *fdt)
{
	ثابت fdt32_t *val;
	صحیح i, len, noff = 0;

	اگر (!fdt)
		بازگشت false;

	وقتی ((noff = fdt_گره_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		اگر (val && len > اندازه(fdt32_t)) {
			len = len / اندازه(fdt32_t);
			برای (i = 0; i < len; i += 2) {
				اگر (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					بازگشت true;
			}
		}
	}

	بازگشت false;
}

صحیح fdt_parse_imsic_گره(ثابت تهی *fdt, صحیح nodeoff, ساختار imsic_data *imsic)
{
	ثابت fdt32_t *val;
	ساختار imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	صحیح i, rc, len, nr_parent_irqs;

	اگر (nodeoff < 0 || !imsic || !fdt)
		بازگشت SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	اگر (val && len > اندازه(fdt32_t)) {
		len = len / اندازه(fdt32_t);
		nr_parent_irqs = len / 2;
		برای (i = 0; i < len; i += 2) {
			اگر (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				بشکن;
			}
		}
	} وگرنه
		بازگشت SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	اگر (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	وگرنه
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	اگر (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} وگرنه {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		اگر ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	اگر (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	وگرنه
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	اگر (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	وگرنه
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	اگر (val && len > 0)
		imsic->رقم_ids = fdt32_to_cpu(*val);
	وگرنه
		بازگشت SBI_EINVAL;

	برای (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	برای (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_گره_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		اگر (rc < 0 || !reg_size)
			بشکن;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	اگر (!imsic->regs[0].size)
		بازگشت SBI_EINVAL;

	بازگشت 0;
}

صحیح fdt_parse_plic_گره(ثابت تهی *fdt, صحیح nodeoffset, ساختار plic_data *plic)
{
	صحیح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_size;

	اگر (nodeoffset < 0 || !plic || !fdt)
		بازگشت SBI_ENODEV;

	rc = fdt_get_گره_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_size)
		بازگشت SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	اگر (len > 0)
		plic->رقم_src = fdt32_to_cpu(*val);

	بازگشت 0;
}

صحیح fdt_parse_plic(ثابت تهی *fdt, ساختار plic_data *plic, ثابت نویسه *compat)
{
	صحیح nodeoffset;

	اگر (!compat || !plic || !fdt)
		بازگشت SBI_ENODEV;

	nodeoffset = fdt_گره_offset_by_compatible(fdt, -1, compat);
	اگر (nodeoffset < 0)
		بازگشت nodeoffset;

	بازگشت fdt_parse_plic_گره(fdt, nodeoffset, plic);
}

ایستا صحیح fdt_get_aclint_addr_size_by_name(ثابت تهی *fdt, صحیح nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	صحیح rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_گره_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_گره_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_size)
		بازگشت SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	بازگشت 0;
}

ایستا صحیح fdt_get_aclint_addr_size(ثابت تهی *fdt, صحیح nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	صحیح rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_گره_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_size)
		بازگشت SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_گره_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	اگر (out_addr2)
		*out_addr2 = reg_addr;
	اگر (out_size2)
		*out_size2 = reg_size;

	بازگشت 0;
}

صحیح fdt_parse_aclint_گره(ثابت تهی *fdt, صحیح nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_شمار)
{
	ثابت fdt32_t *val;
	صحیح i, rc, شمار, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	اگر (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_شمار)
		بازگشت SBI_EINVAL;

	اگر (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", پوچ))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	وگرنه
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	اگر (rc)
		بازگشت rc;

	*out_first_hartid = 0;
	*out_hart_شمار = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمار);
	اگر (!val || شمار < اندازه(fdt32_t))
		بازگشت 0;
	شمار = شمار / اندازه(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	برای (i = 0; i < (شمار / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_گره_offset_by_phandle(fdt, phandle);
		اگر (cpu_intc_offset < 0)
			ادامه;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		اگر (cpu_offset < 0)
			ادامه;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		اگر (rc)
			ادامه;

		اگر (match_hwirq == hwirq) {
			اگر (hartid < first_hartid)
				first_hartid = hartid;
			اگر (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	اگر ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_شمار = last_hartid - first_hartid + 1;
	}
	بازگشت 0;
}

صحیح fdt_parse_plmt_گره(ثابت تهی *fdt, صحیح nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_شمار)
{
	ثابت fdt32_t *val;
	صحیح rc, i, شمار;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	اگر (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_شمار || !plmt_size)
		بازگشت SBI_EINVAL;

	rc = fdt_get_گره_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	اگر (rc < 0)
		بازگشت SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمار);
	اگر (!val || شمار < اندازه(fdt32_t))
		بازگشت 0;
	شمار = شمار / اندازه(fdt32_t);

	hcount = 0;
	برای (i = 0; i < (شمار / 2); i++) {
		صحیح cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_گره_offset_by_phandle(fdt, phandle);
		اگر (cpu_intc_offset < 0)
			ادامه;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		اگر (cpu_offset < 0)
			ادامه;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		اگر (rc)
			ادامه;

		اگر (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			ادامه;

		اگر (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_شمار = hcount;

	بازگشت 0;
}

صحیح fdt_parse_plicsw_گره(ثابت تهی *fdt, صحیح nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_شمار)
{
	ثابت fdt32_t *val;
	صحیح rc, i, شمار;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	اگر (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_شمار || !size)
		بازگشت SBI_EINVAL;

	rc = fdt_get_گره_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	اگر (rc < 0)
		بازگشت SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمار);
	اگر (!val || شمار < اندازه(fdt32_t))
		بازگشت 0;
	شمار = شمار / اندازه(fdt32_t);

	hcount = 0;
	برای (i = 0; i < (شمار / 2); i++) {
		صحیح cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_گره_offset_by_phandle(fdt, phandle);
		اگر (cpu_intc_offset < 0)
			ادامه;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		اگر (cpu_offset < 0)
			ادامه;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		اگر (rc)
			ادامه;

		اگر (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			ادامه;

		اگر (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_شمار = hcount;

	بازگشت 0;
}

صحیح fdt_parse_compat_addr(ثابت تهی *fdt, uint64_t *addr,
			  ثابت نویسه *compatible)
{
	صحیح nodeoffset, rc;

	nodeoffset = fdt_گره_offset_by_compatible(fdt, -1, compatible);
	اگر (nodeoffset < 0)
		بازگشت nodeoffset;

	rc = fdt_get_گره_addr_size(fdt, nodeoffset, 0, addr, پوچ);
	اگر (rc < 0 || !addr)
		بازگشت SBI_ENODEV;

	بازگشت 0;
}
