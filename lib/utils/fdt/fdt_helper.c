// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hartmask.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/aplic.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/imsic.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/plic.h>

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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_phandle_with_args(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				ଅଚଳ ଅକ୍ଷର *prop, ଅଚଳ ଅକ୍ଷର *cells_prop,
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, ସଂରଚନା fdt_phandle_args *out_args)
{
	u32 i, pcells;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, pnodeoff;
	ଅଚଳ fdt32_t *list, *list_end, *val;

	ଯଦି (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		ଫେରନ୍ତୁ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ଯଦି (!list)
		ଫେରନ୍ତୁ SBI_ENOENT;
	list_end = list + (len / ଆକାର(*list));

	ଯେତେବେଳେ (list < list_end) {
		pnodeoff = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		ଯଦି (pnodeoff < 0)
			ଫେରନ୍ତୁ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ଯଦି (!val)
			ଫେରନ୍ତୁ SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		ଯଦି (FDT_MAX_PHANDLE_ARGS < pcells)
			ଫେରନ୍ତୁ SBI_EINVAL;
		ଯଦି (list + pcells > list_end)
			ଫେରନ୍ତୁ SBI_ENOENT;

		ଯଦି (index > 0) {
			list += pcells;
			index--;
		} ଅନ୍ୟଥା {
			out_args->ଗଣ୍ଠି_offset = pnodeoff;
			out_args->args_ଗଣନା = pcells;
			ପାଇଁ (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			ଫେରନ୍ତୁ 0;
		}
	}

	ଫେରନ୍ତୁ SBI_ENOENT;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_translate_address(ଅଚଳ ଶୂନ୍ୟ *fdt, uint64_t reg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା parent,
				 uint64_t *addr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, rlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cell_parent_addr, cell_child_addr, cell_size;
	ଅଚଳ fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	ଯଦି (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		ଯଦି (cell_child_addr < 1)
			ଫେରନ୍ତୁ SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		ଯଦି (cell_parent_addr < 1)
			ଫେରନ୍ତୁ SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		ଯଦି (cell_size < 0)
			ଫେରନ୍ତୁ SBI_ENODEV;

		ପାଇଁ (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		ପାଇଁ (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		ପାଇଁ (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		ଯଦି (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ଫେରନ୍ତୁ SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} ଅନ୍ୟଥା {
		/* No translation required */
		*addr = reg;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_get_ଗଣ୍ଠି_addr_size(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
			   uint64_t *addr, uint64_t *size)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା parent, len, i, rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cell_addr, cell_size;
	ଅଚଳ fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	ଯଦି (!fdt || ଗଣ୍ଠି < 0 || index < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	parent = fdt_parent_offset(fdt, ଗଣ୍ଠି);
	ଯଦି (parent < 0)
		ଫେରନ୍ତୁ parent;
	cell_addr = fdt_address_cells(fdt, parent);
	ଯଦି (cell_addr < 1)
		ଫେରନ୍ତୁ SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	ଯଦି (cell_size < 0)
		ଫେରନ୍ତୁ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, ଗଣ୍ଠି, "reg", &len);
	ଯଦି (!prop_addr)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯଦି ((len / ଆକାର(u32)) <= (index * (cell_addr + cell_size)))
		ଫେରନ୍ତୁ SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	ଯଦି (addr) {
		ପାଇଁ (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		କର {
			ଯଦି (parent < 0)
				ଭାଙ୍ଗ;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			ଯଦି (rc)
				ଭାଙ୍ଗ;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} ଯେତେବେଳେ (1);
	}
	temp = 0;

	ଯଦି (size) {
		ପାଇଁ (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_get_ଗଣ୍ଠି_addr_size_by_name(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି, ଅଚଳ ଅକ୍ଷର *name,
				   uint64_t *addr, uint64_t *size)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j, ଗଣନା_2;
	ଅଚଳ ଅକ୍ଷର *val;
	ଅଚଳ ଅକ୍ଷର *regname;

	ଯଦି (!fdt || ଗଣ୍ଠି < 0 || !name)
		ଫେରନ୍ତୁ SBI_EINVAL;

	val = fdt_getprop(fdt, ଗଣ୍ଠି, "reg-names", &ଗଣନା_2);
	ଯଦି (!val)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ପାଇଁ (i = 0, j = 0; i < ଗଣନା_2; i++, j++) {
		regname = val + i;

		ଯଦି (strcmp(name, regname) == 0)
			ଫେରନ୍ତୁ fdt_get_ଗଣ୍ଠି_addr_size(fdt, ଗଣ୍ଠି, j, addr, size);

		i += strlen(regname);
	}

	ଫେରନ୍ତୁ SBI_ENODEV;
}

bool fdt_ଗଣ୍ଠି_is_enabled(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	ଅଚଳ ଶୂନ୍ୟ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ଯଦି (!prop)
		ଫେରନ୍ତୁ true;

	ଯଦି (!strncmp(prop, "okay", strlen("okay")))
		ଫେରନ୍ତୁ true;

	ଯଦି (!strncmp(prop, "ok", strlen("ok")))
		ଫେରନ୍ତୁ true;

	ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_hart_id(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_offset, u32 *hartid)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	ଅଚଳ ଶୂନ୍ୟ *prop;
	ଅଚଳ fdt32_t *val;

	ଯଦି (!fdt || cpu_offset < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ଯଦି (!prop || !len)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (strncmp (prop, "cpu", strlen ("cpu")))
		ଫେରନ୍ତୁ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	ଯଦି (!val || len < ଆକାର(fdt32_t))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (len > ଆକାର(fdt32_t))
		val++;

	ଯଦି (hartid)
		*hartid = fdt32_to_cpu(*val);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_cbom_block_size(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_offset, unsigned long *cbom_block_size)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	ଅଚଳ ଶୂନ୍ୟ *prop;
	ଅଚଳ fdt32_t *val;

	ଯଦି (!fdt || cpu_offset < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ଯଦି (!prop || !len)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (strncmp (prop, "cpu", strlen ("cpu")))
		ଫେରନ୍ତୁ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	ଯଦି (!val || len < ଆକାର(fdt32_t))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_max_enabled_hart_id(ଅଚଳ ଶୂନ୍ୟ *fdt, u32 *max_hartid)
{
	u32 hartid;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, cpu_offset, cpus_offset;

	ଯଦି (!fdt)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!max_hartid)
		ଫେରନ୍ତୁ 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ଯଦି (cpus_offset < 0)
		ଫେରନ୍ତୁ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ଯଦି (err)
			ଜାରି;

		ଯଦି (!fdt_ଗଣ୍ଠି_is_enabled(fdt, cpu_offset))
			ଜାରି;

		ଯଦି (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_timebase_frequency(ଅଚଳ ଶୂନ୍ୟ *fdt, unsigned long *freq)
{
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, cpus_offset;

	ଯଦି (!fdt || !freq)
		ଫେରନ୍ତୁ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ଯଦି (cpus_offset < 0)
		ଫେରନ୍ତୁ cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	ଯଦି (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_ENOENT;

	ଫେରନ୍ତୁ 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_isa_one_hart(ଅଚଳ ଅକ୍ଷର *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	ଅକ୍ଷର mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	ଯଦି (isa[i] == 'r' || isa[i] == 'R')
		i++;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (isa[i] == 'v' || isa[i] == 'V')
		i++;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	ଅନ୍ୟଥା ଯଦି (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Skip base ISA extensions */
	ପାଇଁ (; i < isa_len; i++) {
		ଯଦି (isa[i] == '_')
			ଭାଙ୍ଗ;
	}

	ଯେତେବେଳେ (i < isa_len) {
		ଯଦି (isa[i] != '_') {
			i++;
			ଜାରି;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ଯେତେବେଳେ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (ଆକାର(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ଯଦି (!j)
			ଜାରି;

#define set_multi_letter_ext(name, bit)				\
			ଯଦି (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				ଜାରି;			\
			}

		ପାଇଁ (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ fdt_parse_isa_extensions_one_hart(ଅଚଳ ଅକ୍ଷର *isa,
					      unsigned long *extensions,
					      ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	size_t i;

	ପାଇଁ (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ଯଦି (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_isa_extensions_all_harts(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	u32 hartid;
	ଅଚଳ fdt32_t *val;
	ସଂରଚନା sbi_scratch *scratch;
	ସଂରଚନା sbi_hart_features *hfeatures;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, cpu_offset, cpus_offset, len;

	ଯଦି (!fdt)
		ଫେରନ୍ତୁ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ଯଦି (cpus_offset < 0)
		ଫେରନ୍ତୁ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ଯଦି (err)
			ଜାରି;

		ଯଦି (!fdt_ଗଣ୍ଠି_is_enabled(fdt, cpu_offset))
			ଜାରି;

		scratch = sbi_hartid_to_scratch(hartid);
		ଯଦି (!scratch)
			ଫେରନ୍ତୁ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ଯଦି (!hfeatures)
			ଫେରନ୍ତୁ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		ଯଦି (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ଅଚଳ ଅକ୍ଷର *)val,
							  hfeatures->extensions, len);
			ଜାରି;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		ଯଦି (!val || len <= 0)
			ଫେରନ୍ତୁ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ଅଚଳ ଅକ୍ଷର *)val, hfeatures->extensions);
		ଯଦି (err)
			ଫେରନ୍ତୁ err;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_uart_ଗଣ୍ଠି_common(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
				      ସଂରଚନା platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, rc;
	ଅଚଳ fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ଯଦି (nodeoffset < 0 || !uart || !fdt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_size)
		ଫେରନ୍ତୁ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ଯଦି (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ଯଦି (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		uart->baud = default_baud;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_gaisler_uart_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
				ସଂରଚନା platform_uart_data *uart)
{
	ଫେରନ୍ତୁ fdt_parse_uart_ଗଣ୍ଠି_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_renesas_scif_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
				ସଂରଚନା platform_uart_data *uart)
{
	ଫେରନ୍ତୁ fdt_parse_uart_ଗଣ୍ଠି_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_shakti_uart_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			       ସଂରଚନା platform_uart_data *uart)
{
	ଫେରନ୍ତୁ fdt_parse_uart_ଗଣ୍ଠି_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_sifive_uart_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			       ସଂରଚନା platform_uart_data *uart)
{
	ଫେରନ୍ତୁ fdt_parse_uart_ଗଣ୍ଠି_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_uart_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			ସଂରଚନା platform_uart_data *uart)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, rc;
	ଅଚଳ fdt32_t *val;

	rc = fdt_parse_uart_ଗଣ୍ଠି_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ଯଦି (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ଯଦି (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ଯଦି (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_uart8250(ଅଚଳ ଶୂନ୍ୟ *fdt, ସଂରଚନା platform_uart_data *uart,
		   ଅଚଳ ଅକ୍ଷର *compatible)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset;

	ଯଦି (!compatible || !uart || !fdt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	nodeoffset = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, -1, compatible);
	ଯଦି (nodeoffset < 0)
		ଫେରନ୍ତୁ nodeoffset;

	ଫେରନ୍ତୁ fdt_parse_uart_ଗଣ୍ଠି(fdt, nodeoffset, uart);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_xlnx_uartlite_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			       ସଂରଚନା platform_uart_data *uart)
{
	ଫେରନ୍ତୁ fdt_parse_uart_ଗଣ୍ଠି_common(fdt, nodeoffset, uart, 0, 0);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_aplic_find_imsic_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				     ସଂରଚନା imsic_data *imsic, bool mmode)
{
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ଯଦି (val && len >= ଆକାର(fdt32_t)) {
		noff = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		ଯଦି (noff < 0)
			ଫେରନ୍ତୁ noff;

		rc = fdt_parse_imsic_ଗଣ୍ଠି(fdt, noff, imsic);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		rc = imsic_data_check(imsic);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		ଯଦି (imsic->targets_mmode == mmode) {
			ଫେରନ୍ତୁ 0;
		}
	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ଯଦି (!val || len < ଆକାର(fdt32_t))
		ଫେରନ୍ତୁ SBI_ENODEV;

	len /= ଆକାର(fdt32_t);

	ପାଇଁ (i = 0; i < len; i++) {
		noff = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		ଯଦି (noff < 0)
			ଫେରନ୍ତୁ noff;

		rc = fdt_aplic_find_imsic_ଗଣ୍ଠି(fdt, noff, imsic, mmode);
		ଯଦି (!rc)
			ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ rc;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_aplic_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ସଂରଚନା aplic_data *aplic)
{
	bool child_found;
	ଅଚଳ fdt32_t *val;
	ଅଚଳ fdt32_t *del;
	ସଂରଚନା imsic_data imsic = { 0 };
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	ସଂରଚନା aplic_delegate_data *deleg;

	ଯଦି (nodeoff < 0 || !aplic || !fdt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_size)
		ଫେରନ୍ତୁ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ଯଦି (len > 0)
		aplic->ଅଙ୍କ_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ଯଦି (val && len > ଆକାର(fdt32_t)) {
		len = len / ଆକାର(fdt32_t);
		ପାଇଁ (i = 0; i < len; i += 2) {
			ଯଦି (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				ଭାଙ୍ଗ;
			}
		}
		aplic->ଅଙ୍କ_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_ଗଣ୍ଠି(fdt, nodeoff, &imsic, true);
	ଯଦି (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		ଯଦି (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ଫେରନ୍ତୁ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_ଗଣ୍ଠି(fdt, nodeoff, &imsic, false);
	ଯଦି (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		ଯଦି (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ଫେରନ୍ତୁ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	ପାଇଁ (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ଯଦି (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ଯଦି (!del || len < (3 * ଆକାର(fdt32_t)))
		ଯାଅ skip_delegate_parse;
	d = 0;
	dcnt = len / ଆକାର(fdt32_t);
	ପାଇଁ (i = 0; i < dcnt; i += 3) {
		ଯଦି (d >= APLIC_MAX_DELEGATE)
			ଭାଙ୍ଗ;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ଯଦି (!val || len < ଆକାର(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			ଜାରି;
		}
		len = len / ଆକାର(fdt32_t);
		ପାଇଁ (j = 0; j < len; j++) {
			ଯଦି (del[i] != val[j])
				ଜାରି;
			deleg->child_index = j;
			child_found = true;
			ଭାଙ୍ଗ;
		}

		ଯଦି (child_found) {
			d++;
		} ଅନ୍ୟଥା {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	ଫେରନ୍ତୁ 0;
}

bool fdt_check_imsic_mlevel(ଅଚଳ ଶୂନ୍ୟ *fdt)
{
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, len, noff = 0;

	ଯଦି (!fdt)
		ଫେରନ୍ତୁ false;

	ଯେତେବେଳେ ((noff = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ଯଦି (val && len > ଆକାର(fdt32_t)) {
			len = len / ଆକାର(fdt32_t);
			ପାଇଁ (i = 0; i < len; i += 2) {
				ଯଦି (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					ଫେରନ୍ତୁ true;
			}
		}
	}

	ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_imsic_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ସଂରଚନା imsic_data *imsic)
{
	ଅଚଳ fdt32_t *val;
	ସଂରଚନା imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, rc, len, nr_parent_irqs;

	ଯଦି (nodeoff < 0 || !imsic || !fdt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ଯଦି (val && len > ଆକାର(fdt32_t)) {
		len = len / ଆକାର(fdt32_t);
		nr_parent_irqs = len / 2;
		ପାଇଁ (i = 0; i < len; i += 2) {
			ଯଦି (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				ଭାଙ୍ଗ;
			}
		}
	} ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ଯଦି (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ଯଦି (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} ଅନ୍ୟଥା {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		ଯଦି ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ଯଦି (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ଯଦି (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ଯଦି (val && len > 0)
		imsic->ଅଙ୍କ_ids = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EINVAL;

	ପାଇଁ (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	ପାଇଁ (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		ଯଦି (rc < 0 || !reg_size)
			ଭାଙ୍ଗ;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	ଯଦି (!imsic->regs[0].size)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_plic_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ସଂରଚନା plic_data *plic)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, rc;
	ଅଚଳ fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ଯଦି (nodeoffset < 0 || !plic || !fdt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_size)
		ଫେରନ୍ତୁ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ଯଦି (len > 0)
		plic->ଅଙ୍କ_src = fdt32_to_cpu(*val);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_plic(ଅଚଳ ଶୂନ୍ୟ *fdt, ସଂରଚନା plic_data *plic, ଅଚଳ ଅକ୍ଷର *compat)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset;

	ଯଦି (!compat || !plic || !fdt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	nodeoffset = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, -1, compat);
	ଯଦି (nodeoffset < 0)
		ଫେରନ୍ତୁ nodeoffset;

	ଫେରନ୍ତୁ fdt_parse_plic_ଗଣ୍ଠି(fdt, nodeoffset, plic);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_get_aclint_addr_size_by_name(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_ଗଣ୍ଠି_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_ଗଣ୍ଠି_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_size)
		ଫେରନ୍ତୁ SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_get_aclint_addr_size(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_size)
		ଫେରନ୍ତୁ SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	ଯଦି (out_addr2)
		*out_addr2 = reg_addr;
	ଯଦି (out_size2)
		*out_size2 = reg_size;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_aclint_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_ଗଣନା)
{
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, rc, ଗଣନା_2, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ଯଦି (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_ଗଣନା)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", ଖାଲି))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	ଅନ୍ୟଥା
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	*out_first_hartid = 0;
	*out_hart_ଗଣନା = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ଗଣନା_2);
	ଯଦି (!val || ଗଣନା_2 < ଆକାର(fdt32_t))
		ଫେରନ୍ତୁ 0;
	ଗଣନା_2 = ଗଣନା_2 / ଆକାର(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	ପାଇଁ (i = 0; i < (ଗଣନା_2 / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, phandle);
		ଯଦି (cpu_intc_offset < 0)
			ଜାରି;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ଯଦି (cpu_offset < 0)
			ଜାରି;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ଯଦି (rc)
			ଜାରି;

		ଯଦି (match_hwirq == hwirq) {
			ଯଦି (hartid < first_hartid)
				first_hartid = hartid;
			ଯଦି (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	ଯଦି ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_ଗଣନା = last_hartid - first_hartid + 1;
	}
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_plmt_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_ଗଣନା)
{
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, i, ଗଣନା_2;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ଯଦି (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_ଗଣନା || !plmt_size)
		ଫେରନ୍ତୁ SBI_EINVAL;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ଯଦି (rc < 0)
		ଫେରନ୍ତୁ SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ଗଣନା_2);
	ଯଦି (!val || ଗଣନା_2 < ଆକାର(fdt32_t))
		ଫେରନ୍ତୁ 0;
	ଗଣନା_2 = ଗଣନା_2 / ଆକାର(fdt32_t);

	hcount = 0;
	ପାଇଁ (i = 0; i < (ଗଣନା_2 / 2); i++) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, phandle);
		ଯଦି (cpu_intc_offset < 0)
			ଜାରି;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ଯଦି (cpu_offset < 0)
			ଜାରି;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ଯଦି (rc)
			ଜାରି;

		ଯଦି (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			ଜାରି;

		ଯଦି (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_ଗଣନା = hcount;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_plicsw_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_ଗଣନା)
{
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, i, ଗଣନା_2;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ଯଦି (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_ଗଣନା || !size)
		ଫେରନ୍ତୁ SBI_EINVAL;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ଯଦି (rc < 0)
		ଫେରନ୍ତୁ SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ଗଣନା_2);
	ଯଦି (!val || ଗଣନା_2 < ଆକାର(fdt32_t))
		ଫେରନ୍ତୁ 0;
	ଗଣନା_2 = ଗଣନା_2 / ଆକାର(fdt32_t);

	hcount = 0;
	ପାଇଁ (i = 0; i < (ଗଣନା_2 / 2); i++) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, phandle);
		ଯଦି (cpu_intc_offset < 0)
			ଜାରି;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ଯଦି (cpu_offset < 0)
			ଜାରି;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ଯଦି (rc)
			ଜାରି;

		ଯଦି (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			ଜାରି;

		ଯଦି (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_ଗଣନା = hcount;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_parse_compat_addr(ଅଚଳ ଶୂନ୍ୟ *fdt, uint64_t *addr,
			  ଅଚଳ ଅକ୍ଷର *compatible)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, rc;

	nodeoffset = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, -1, compatible);
	ଯଦି (nodeoffset < 0)
		ଫେରନ୍ତୁ nodeoffset;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoffset, 0, addr, ଖାଲି);
	ଯଦି (rc < 0 || !addr)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଫେରନ୍ତୁ 0;
}
