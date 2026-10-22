// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_hartmask.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/irqchip/aplic.h>
#અન્તર્ભાવ <sbi_utils/irqchip/imsic.h>
#અન્તર્ભાવ <sbi_utils/irqchip/plic.h>

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

પૂર્ણાંક fdt_parse_phandle_with_args(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				અચલ અક્ષર *prop, અચલ અક્ષર *cells_prop,
				પૂર્ણાંક index, સંરચના fdt_phandle_args *out_args)
{
	u32 i, pcells;
	પૂર્ણાંક len, pnodeoff;
	અચલ fdt32_t *list, *list_end, *val;

	જો (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		પાછા SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	જો (!list)
		પાછા SBI_ENOENT;
	list_end = list + (len / કદ(*list));

	જ્યારે (list < list_end) {
		pnodeoff = fdt_ગાંઠ_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		જો (pnodeoff < 0)
			પાછા pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		જો (!val)
			પાછા SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		જો (FDT_MAX_PHANDLE_ARGS < pcells)
			પાછા SBI_EINVAL;
		જો (list + pcells > list_end)
			પાછા SBI_ENOENT;

		જો (index > 0) {
			list += pcells;
			index--;
		} નહીંતર {
			out_args->ગાંઠ_offset = pnodeoff;
			out_args->args_ગણતરી = pcells;
			માટે (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			પાછા 0;
		}
	}

	પાછા SBI_ENOENT;
}

સ્થિર પૂર્ણાંક fdt_translate_address(અચલ શૂન્ય *fdt, uint64_t reg, પૂર્ણાંક parent,
				 uint64_t *addr)
{
	પૂર્ણાંક i, rlen;
	પૂર્ણાંક cell_parent_addr, cell_child_addr, cell_size;
	અચલ fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	જો (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		જો (cell_child_addr < 1)
			પાછા SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		જો (cell_parent_addr < 1)
			પાછા SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		જો (cell_size < 0)
			પાછા SBI_ENODEV;

		માટે (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		માટે (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		માટે (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		જો (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			પાછા SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} નહીંતર {
		/* No translation required */
		*addr = reg;
	}

	પાછા 0;
}

પૂર્ણાંક fdt_get_ગાંઠ_addr_size(અચલ શૂન્ય *fdt, પૂર્ણાંક ગાંઠ, પૂર્ણાંક index,
			   uint64_t *addr, uint64_t *size)
{
	પૂર્ણાંક parent, len, i, rc;
	પૂર્ણાંક cell_addr, cell_size;
	અચલ fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	જો (!fdt || ગાંઠ < 0 || index < 0)
		પાછા SBI_EINVAL;

	parent = fdt_parent_offset(fdt, ગાંઠ);
	જો (parent < 0)
		પાછા parent;
	cell_addr = fdt_address_cells(fdt, parent);
	જો (cell_addr < 1)
		પાછા SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	જો (cell_size < 0)
		પાછા SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, ગાંઠ, "reg", &len);
	જો (!prop_addr)
		પાછા SBI_ENODEV;

	જો ((len / કદ(u32)) <= (index * (cell_addr + cell_size)))
		પાછા SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	જો (addr) {
		માટે (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		કરો {
			જો (parent < 0)
				તોડો;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			જો (rc)
				તોડો;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} જ્યારે (1);
	}
	temp = 0;

	જો (size) {
		માટે (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	પાછા 0;
}

પૂર્ણાંક fdt_get_ગાંઠ_addr_size_by_name(અચલ શૂન્ય *fdt, પૂર્ણાંક ગાંઠ, અચલ અક્ષર *name,
				   uint64_t *addr, uint64_t *size)
{
	પૂર્ણાંક i, j, ગણતરી;
	અચલ અક્ષર *val;
	અચલ અક્ષર *regname;

	જો (!fdt || ગાંઠ < 0 || !name)
		પાછા SBI_EINVAL;

	val = fdt_getprop(fdt, ગાંઠ, "reg-names", &ગણતરી);
	જો (!val)
		પાછા SBI_ENODEV;

	માટે (i = 0, j = 0; i < ગણતરી; i++, j++) {
		regname = val + i;

		જો (strcmp(name, regname) == 0)
			પાછા fdt_get_ગાંઠ_addr_size(fdt, ગાંઠ, j, addr, size);

		i += strlen(regname);
	}

	પાછા SBI_ENODEV;
}

bool fdt_ગાંઠ_is_enabled(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff)
{
	પૂર્ણાંક len;
	અચલ શૂન્ય *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	જો (!prop)
		પાછા true;

	જો (!strncmp(prop, "okay", strlen("okay")))
		પાછા true;

	જો (!strncmp(prop, "ok", strlen("ok")))
		પાછા true;

	પાછા false;
}

પૂર્ણાંક fdt_parse_hart_id(અચલ શૂન્ય *fdt, પૂર્ણાંક cpu_offset, u32 *hartid)
{
	પૂર્ણાંક len;
	અચલ શૂન્ય *prop;
	અચલ fdt32_t *val;

	જો (!fdt || cpu_offset < 0)
		પાછા SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	જો (!prop || !len)
		પાછા SBI_EINVAL;
	જો (strncmp (prop, "cpu", strlen ("cpu")))
		પાછા SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	જો (!val || len < કદ(fdt32_t))
		પાછા SBI_EINVAL;

	જો (len > કદ(fdt32_t))
		val++;

	જો (hartid)
		*hartid = fdt32_to_cpu(*val);

	પાછા 0;
}

પૂર્ણાંક fdt_parse_cbom_block_size(અચલ શૂન્ય *fdt, પૂર્ણાંક cpu_offset, unsigned long *cbom_block_size)
{
	પૂર્ણાંક len;
	અચલ શૂન્ય *prop;
	અચલ fdt32_t *val;

	જો (!fdt || cpu_offset < 0)
		પાછા SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	જો (!prop || !len)
		પાછા SBI_EINVAL;
	જો (strncmp (prop, "cpu", strlen ("cpu")))
		પાછા SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	જો (!val || len < કદ(fdt32_t))
		પાછા SBI_EINVAL;

	જો (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	પાછા 0;
}

પૂર્ણાંક fdt_parse_max_enabled_hart_id(અચલ શૂન્ય *fdt, u32 *max_hartid)
{
	u32 hartid;
	પૂર્ણાંક err, cpu_offset, cpus_offset;

	જો (!fdt)
		પાછા SBI_EINVAL;
	જો (!max_hartid)
		પાછા 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	જો (cpus_offset < 0)
		પાછા cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		જો (err)
			ચાલુ;

		જો (!fdt_ગાંઠ_is_enabled(fdt, cpu_offset))
			ચાલુ;

		જો (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	પાછા 0;
}

પૂર્ણાંક fdt_parse_timebase_frequency(અચલ શૂન્ય *fdt, unsigned long *freq)
{
	અચલ fdt32_t *val;
	પૂર્ણાંક len, cpus_offset;

	જો (!fdt || !freq)
		પાછા SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	જો (cpus_offset < 0)
		પાછા cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	જો (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	નહીંતર
		પાછા SBI_ENOENT;

	પાછા 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

સ્થિર પૂર્ણાંક fdt_parse_isa_one_hart(અચલ અક્ષર *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	અક્ષર mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	જો (isa[i] == 'r' || isa[i] == 'R')
		i++;
	નહીંતર
		પાછા SBI_EINVAL;

	જો (isa[i] == 'v' || isa[i] == 'V')
		i++;
	નહીંતર
		પાછા SBI_EINVAL;

	જો (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	નહીંતર જો (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	નહીંતર
		પાછા SBI_EINVAL;

	/* Skip base ISA extensions */
	માટે (; i < isa_len; i++) {
		જો (isa[i] == '_')
			તોડો;
	}

	જ્યારે (i < isa_len) {
		જો (isa[i] != '_') {
			i++;
			ચાલુ;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		જ્યારે ((i < isa_len) && (isa[i] != '_') &&
		       (j < (કદ(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		જો (!j)
			ચાલુ;

#define set_multi_letter_ext(name, bit)				\
			જો (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				ચાલુ;			\
			}

		માટે (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	પાછા 0;
}

સ્થિર શૂન્ય fdt_parse_isa_extensions_one_hart(અચલ અક્ષર *isa,
					      unsigned long *extensions,
					      પૂર્ણાંક len)
{
	size_t i;

	માટે (i = 0; i < SBI_HART_EXT_MAX; i++) {
		જો (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

પૂર્ણાંક fdt_parse_isa_extensions_all_harts(અચલ શૂન્ય *fdt)
{
	u32 hartid;
	અચલ fdt32_t *val;
	સંરચના sbi_scratch *scratch;
	સંરચના sbi_hart_features *hfeatures;
	પૂર્ણાંક err, cpu_offset, cpus_offset, len;

	જો (!fdt)
		પાછા SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	જો (cpus_offset < 0)
		પાછા cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		જો (err)
			ચાલુ;

		જો (!fdt_ગાંઠ_is_enabled(fdt, cpu_offset))
			ચાલુ;

		scratch = sbi_hartid_to_scratch(hartid);
		જો (!scratch)
			પાછા SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		જો (!hfeatures)
			પાછા SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		જો (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((અચલ અક્ષર *)val,
							  hfeatures->extensions, len);
			ચાલુ;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		જો (!val || len <= 0)
			પાછા SBI_ENOENT;

		err = fdt_parse_isa_one_hart((અચલ અક્ષર *)val, hfeatures->extensions);
		જો (err)
			પાછા err;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_parse_uart_ગાંઠ_common(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
				      સંરચના platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	પૂર્ણાંક len, rc;
	અચલ fdt32_t *val;
	uint64_t reg_addr, reg_size;

	જો (nodeoffset < 0 || !uart || !fdt)
		પાછા SBI_ENODEV;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	જો (rc < 0 || !reg_size)
		પાછા SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	જો (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	નહીંતર
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	જો (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	નહીંતર
		uart->baud = default_baud;

	પાછા 0;
}

પૂર્ણાંક fdt_parse_gaisler_uart_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
				સંરચના platform_uart_data *uart)
{
	પાછા fdt_parse_uart_ગાંઠ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

પૂર્ણાંક fdt_parse_renesas_scif_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
				સંરચના platform_uart_data *uart)
{
	પાછા fdt_parse_uart_ગાંઠ_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

પૂર્ણાંક fdt_parse_shakti_uart_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
			       સંરચના platform_uart_data *uart)
{
	પાછા fdt_parse_uart_ગાંઠ_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

પૂર્ણાંક fdt_parse_sifive_uart_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
			       સંરચના platform_uart_data *uart)
{
	પાછા fdt_parse_uart_ગાંઠ_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

પૂર્ણાંક fdt_parse_uart_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
			સંરચના platform_uart_data *uart)
{
	પૂર્ણાંક len, rc;
	અચલ fdt32_t *val;

	rc = fdt_parse_uart_ગાંઠ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	જો (rc)
		પાછા rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	જો (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	નહીંતર
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	જો (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	નહીંતર
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	જો (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	નહીંતર
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	પાછા 0;
}

પૂર્ણાંક fdt_parse_uart8250(અચલ શૂન્ય *fdt, સંરચના platform_uart_data *uart,
		   અચલ અક્ષર *compatible)
{
	પૂર્ણાંક nodeoffset;

	જો (!compatible || !uart || !fdt)
		પાછા SBI_ENODEV;

	nodeoffset = fdt_ગાંઠ_offset_by_compatible(fdt, -1, compatible);
	જો (nodeoffset < 0)
		પાછા nodeoffset;

	પાછા fdt_parse_uart_ગાંઠ(fdt, nodeoffset, uart);
}

પૂર્ણાંક fdt_parse_xlnx_uartlite_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
			       સંરચના platform_uart_data *uart)
{
	પાછા fdt_parse_uart_ગાંઠ_common(fdt, nodeoffset, uart, 0, 0);
}

સ્થિર પૂર્ણાંક fdt_aplic_find_imsic_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				     સંરચના imsic_data *imsic, bool mmode)
{
	અચલ fdt32_t *val;
	પૂર્ણાંક i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	જો (val && len >= કદ(fdt32_t)) {
		noff = fdt_ગાંઠ_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		જો (noff < 0)
			પાછા noff;

		rc = fdt_parse_imsic_ગાંઠ(fdt, noff, imsic);
		જો (rc)
			પાછા rc;

		rc = imsic_data_check(imsic);
		જો (rc)
			પાછા rc;

		જો (imsic->targets_mmode == mmode) {
			પાછા 0;
		}
	} નહીંતર {
		પાછા SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	જો (!val || len < કદ(fdt32_t))
		પાછા SBI_ENODEV;

	len /= કદ(fdt32_t);

	માટે (i = 0; i < len; i++) {
		noff = fdt_ગાંઠ_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		જો (noff < 0)
			પાછા noff;

		rc = fdt_aplic_find_imsic_ગાંઠ(fdt, noff, imsic, mmode);
		જો (!rc)
			તોડો;
	}

	પાછા rc;
}

પૂર્ણાંક fdt_parse_aplic_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, સંરચના aplic_data *aplic)
{
	bool child_found;
	અચલ fdt32_t *val;
	અચલ fdt32_t *del;
	સંરચના imsic_data imsic = { 0 };
	પૂર્ણાંક i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	સંરચના aplic_delegate_data *deleg;

	જો (nodeoff < 0 || !aplic || !fdt)
		પાછા SBI_ENODEV;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	જો (rc < 0 || !reg_size)
		પાછા SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	જો (len > 0)
		aplic->અંક_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	જો (val && len > કદ(fdt32_t)) {
		len = len / કદ(fdt32_t);
		માટે (i = 0; i < len; i += 2) {
			જો (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				તોડો;
			}
		}
		aplic->અંક_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_ગાંઠ(fdt, nodeoff, &imsic, true);
	જો (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		જો (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			પાછા SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_ગાંઠ(fdt, nodeoff, &imsic, false);
	જો (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		જો (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			પાછા SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	માટે (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	જો (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	જો (!del || len < (3 * કદ(fdt32_t)))
		જાઓ skip_delegate_parse;
	d = 0;
	dcnt = len / કદ(fdt32_t);
	માટે (i = 0; i < dcnt; i += 3) {
		જો (d >= APLIC_MAX_DELEGATE)
			તોડો;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		જો (!val || len < કદ(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			ચાલુ;
		}
		len = len / કદ(fdt32_t);
		માટે (j = 0; j < len; j++) {
			જો (del[i] != val[j])
				ચાલુ;
			deleg->child_index = j;
			child_found = true;
			તોડો;
		}

		જો (child_found) {
			d++;
		} નહીંતર {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	પાછા 0;
}

bool fdt_check_imsic_mlevel(અચલ શૂન્ય *fdt)
{
	અચલ fdt32_t *val;
	પૂર્ણાંક i, len, noff = 0;

	જો (!fdt)
		પાછા false;

	જ્યારે ((noff = fdt_ગાંઠ_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		જો (val && len > કદ(fdt32_t)) {
			len = len / કદ(fdt32_t);
			માટે (i = 0; i < len; i += 2) {
				જો (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					પાછા true;
			}
		}
	}

	પાછા false;
}

પૂર્ણાંક fdt_parse_imsic_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, સંરચના imsic_data *imsic)
{
	અચલ fdt32_t *val;
	સંરચના imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	પૂર્ણાંક i, rc, len, nr_parent_irqs;

	જો (nodeoff < 0 || !imsic || !fdt)
		પાછા SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	જો (val && len > કદ(fdt32_t)) {
		len = len / કદ(fdt32_t);
		nr_parent_irqs = len / 2;
		માટે (i = 0; i < len; i += 2) {
			જો (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				તોડો;
			}
		}
	} નહીંતર
		પાછા SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	જો (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	નહીંતર
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	જો (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} નહીંતર {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		જો ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	જો (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	નહીંતર
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	જો (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	નહીંતર
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	જો (val && len > 0)
		imsic->અંક_ids = fdt32_to_cpu(*val);
	નહીંતર
		પાછા SBI_EINVAL;

	માટે (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	માટે (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		જો (rc < 0 || !reg_size)
			તોડો;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	જો (!imsic->regs[0].size)
		પાછા SBI_EINVAL;

	પાછા 0;
}

પૂર્ણાંક fdt_parse_plic_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset, સંરચના plic_data *plic)
{
	પૂર્ણાંક len, rc;
	અચલ fdt32_t *val;
	uint64_t reg_addr, reg_size;

	જો (nodeoffset < 0 || !plic || !fdt)
		પાછા SBI_ENODEV;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	જો (rc < 0 || !reg_size)
		પાછા SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	જો (len > 0)
		plic->અંક_src = fdt32_to_cpu(*val);

	પાછા 0;
}

પૂર્ણાંક fdt_parse_plic(અચલ શૂન્ય *fdt, સંરચના plic_data *plic, અચલ અક્ષર *compat)
{
	પૂર્ણાંક nodeoffset;

	જો (!compat || !plic || !fdt)
		પાછા SBI_ENODEV;

	nodeoffset = fdt_ગાંઠ_offset_by_compatible(fdt, -1, compat);
	જો (nodeoffset < 0)
		પાછા nodeoffset;

	પાછા fdt_parse_plic_ગાંઠ(fdt, nodeoffset, plic);
}

સ્થિર પૂર્ણાંક fdt_get_aclint_addr_size_by_name(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	પૂર્ણાંક rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_ગાંઠ_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	જો (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_ગાંઠ_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	જો (rc < 0 || !reg_size)
		પાછા SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_get_aclint_addr_size(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	પૂર્ણાંક rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	જો (rc < 0 || !reg_size)
		પાછા SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	જો (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	જો (out_addr2)
		*out_addr2 = reg_addr;
	જો (out_size2)
		*out_size2 = reg_size;

	પાછા 0;
}

પૂર્ણાંક fdt_parse_aclint_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_ગણતરી)
{
	અચલ fdt32_t *val;
	પૂર્ણાંક i, rc, ગણતરી, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	જો (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_ગણતરી)
		પાછા SBI_EINVAL;

	જો (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", ખાલી))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	નહીંતર
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	જો (rc)
		પાછા rc;

	*out_first_hartid = 0;
	*out_hart_ગણતરી = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ગણતરી);
	જો (!val || ગણતરી < કદ(fdt32_t))
		પાછા 0;
	ગણતરી = ગણતરી / કદ(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	માટે (i = 0; i < (ગણતરી / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_ગાંઠ_offset_by_phandle(fdt, phandle);
		જો (cpu_intc_offset < 0)
			ચાલુ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		જો (cpu_offset < 0)
			ચાલુ;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		જો (rc)
			ચાલુ;

		જો (match_hwirq == hwirq) {
			જો (hartid < first_hartid)
				first_hartid = hartid;
			જો (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	જો ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_ગણતરી = last_hartid - first_hartid + 1;
	}
	પાછા 0;
}

પૂર્ણાંક fdt_parse_plmt_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_ગણતરી)
{
	અચલ fdt32_t *val;
	પૂર્ણાંક rc, i, ગણતરી;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	જો (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_ગણતરી || !plmt_size)
		પાછા SBI_EINVAL;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	જો (rc < 0)
		પાછા SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ગણતરી);
	જો (!val || ગણતરી < કદ(fdt32_t))
		પાછા 0;
	ગણતરી = ગણતરી / કદ(fdt32_t);

	hcount = 0;
	માટે (i = 0; i < (ગણતરી / 2); i++) {
		પૂર્ણાંક cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_ગાંઠ_offset_by_phandle(fdt, phandle);
		જો (cpu_intc_offset < 0)
			ચાલુ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		જો (cpu_offset < 0)
			ચાલુ;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		જો (rc)
			ચાલુ;

		જો (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			ચાલુ;

		જો (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_ગણતરી = hcount;

	પાછા 0;
}

પૂર્ણાંક fdt_parse_plicsw_ગાંઠ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_ગણતરી)
{
	અચલ fdt32_t *val;
	પૂર્ણાંક rc, i, ગણતરી;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	જો (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_ગણતરી || !size)
		પાછા SBI_EINVAL;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	જો (rc < 0)
		પાછા SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ગણતરી);
	જો (!val || ગણતરી < કદ(fdt32_t))
		પાછા 0;
	ગણતરી = ગણતરી / કદ(fdt32_t);

	hcount = 0;
	માટે (i = 0; i < (ગણતરી / 2); i++) {
		પૂર્ણાંક cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_ગાંઠ_offset_by_phandle(fdt, phandle);
		જો (cpu_intc_offset < 0)
			ચાલુ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		જો (cpu_offset < 0)
			ચાલુ;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		જો (rc)
			ચાલુ;

		જો (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			ચાલુ;

		જો (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_ગણતરી = hcount;

	પાછા 0;
}

પૂર્ણાંક fdt_parse_compat_addr(અચલ શૂન્ય *fdt, uint64_t *addr,
			  અચલ અક્ષર *compatible)
{
	પૂર્ણાંક nodeoffset, rc;

	nodeoffset = fdt_ગાંઠ_offset_by_compatible(fdt, -1, compatible);
	જો (nodeoffset < 0)
		પાછા nodeoffset;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoffset, 0, addr, ખાલી);
	જો (rc < 0 || !addr)
		પાછા SBI_ENODEV;

	પાછા 0;
}
