// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hartmask.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/aplic.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/imsic.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/plic.h>

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

ಪೂರ್ಣಾಙ್ಕ fdt_parse_phandle_with_args(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				ಧ್ರುವ ವರ್ಣ *prop, ಧ್ರುವ ವರ್ಣ *cells_prop,
				ಪೂರ್ಣಾಙ್ಕ index, ಸಂರಚನಾ fdt_phandle_args *out_args)
{
	u32 i, pcells;
	ಪೂರ್ಣಾಙ್ಕ len, pnodeoff;
	ಧ್ರುವ fdt32_t *list, *list_end, *val;

	ಯದಿ (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ಯದಿ (!list)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;
	list_end = list + (len / ಪರಿಮಾಣ(*list));

	ಯಾವತ್ (list < list_end) {
		pnodeoff = fdt_ಗಂಟು_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		ಯದಿ (pnodeoff < 0)
			ಪ್ರತ್ಯಾಗಮ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ಯದಿ (!val)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		ಯದಿ (FDT_MAX_PHANDLE_ARGS < pcells)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		ಯದಿ (list + pcells > list_end)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

		ಯದಿ (index > 0) {
			list += pcells;
			index--;
		} ಅನ್ಯಥಾ {
			out_args->ಗಂಟು_offset = pnodeoff;
			out_args->args_ಎಣಿಕೆ = pcells;
			ಪ್ರತಿ (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			ಪ್ರತ್ಯಾಗಮ 0;
		}
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_translate_address(ಧ್ರುವ ಶೂನ್ಯ *fdt, uint64_t reg, ಪೂರ್ಣಾಙ್ಕ parent,
				 uint64_t *addr)
{
	ಪೂರ್ಣಾಙ್ಕ i, rlen;
	ಪೂರ್ಣಾಙ್ಕ cell_parent_addr, cell_child_addr, cell_size;
	ಧ್ರುವ fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	ಯದಿ (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		ಯದಿ (cell_child_addr < 1)
			ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		ಯದಿ (cell_parent_addr < 1)
			ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		ಯದಿ (cell_size < 0)
			ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

		ಪ್ರತಿ (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		ಪ್ರತಿ (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		ಪ್ರತಿ (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		ಯದಿ (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} ಅನ್ಯಥಾ {
		/* No translation required */
		*addr = reg;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_get_ಗಂಟು_addr_size(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ ಗಂಟು, ಪೂರ್ಣಾಙ್ಕ index,
			   uint64_t *addr, uint64_t *size)
{
	ಪೂರ್ಣಾಙ್ಕ parent, len, i, rc;
	ಪೂರ್ಣಾಙ್ಕ cell_addr, cell_size;
	ಧ್ರುವ fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	ಯದಿ (!fdt || ಗಂಟು < 0 || index < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	parent = fdt_parent_offset(fdt, ಗಂಟು);
	ಯದಿ (parent < 0)
		ಪ್ರತ್ಯಾಗಮ parent;
	cell_addr = fdt_address_cells(fdt, parent);
	ಯದಿ (cell_addr < 1)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	ಯದಿ (cell_size < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, ಗಂಟು, "reg", &len);
	ಯದಿ (!prop_addr)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯದಿ ((len / ಪರಿಮಾಣ(u32)) <= (index * (cell_addr + cell_size)))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	ಯದಿ (addr) {
		ಪ್ರತಿ (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		ಕುರು {
			ಯದಿ (parent < 0)
				ಭಙ್ಗ;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			ಯದಿ (rc)
				ಭಙ್ಗ;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} ಯಾವತ್ (1);
	}
	temp = 0;

	ಯದಿ (size) {
		ಪ್ರತಿ (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_get_ಗಂಟು_addr_size_by_name(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ ಗಂಟು, ಧ್ರುವ ವರ್ಣ *name,
				   uint64_t *addr, uint64_t *size)
{
	ಪೂರ್ಣಾಙ್ಕ i, j, ಎಣಿಕೆ;
	ಧ್ರುವ ವರ್ಣ *val;
	ಧ್ರುವ ವರ್ಣ *regname;

	ಯದಿ (!fdt || ಗಂಟು < 0 || !name)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	val = fdt_getprop(fdt, ಗಂಟು, "reg-names", &ಎಣಿಕೆ);
	ಯದಿ (!val)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಪ್ರತಿ (i = 0, j = 0; i < ಎಣಿಕೆ; i++, j++) {
		regname = val + i;

		ಯದಿ (strcmp(name, regname) == 0)
			ಪ್ರತ್ಯಾಗಮ fdt_get_ಗಂಟು_addr_size(fdt, ಗಂಟು, j, addr, size);

		i += strlen(regname);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
}

bool fdt_ಗಂಟು_is_enabled(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff)
{
	ಪೂರ್ಣಾಙ್ಕ len;
	ಧ್ರುವ ಶೂನ್ಯ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ಯದಿ (!prop)
		ಪ್ರತ್ಯಾಗಮ true;

	ಯದಿ (!strncmp(prop, "okay", strlen("okay")))
		ಪ್ರತ್ಯಾಗಮ true;

	ಯದಿ (!strncmp(prop, "ok", strlen("ok")))
		ಪ್ರತ್ಯಾಗಮ true;

	ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_hart_id(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ cpu_offset, u32 *hartid)
{
	ಪೂರ್ಣಾಙ್ಕ len;
	ಧ್ರುವ ಶೂನ್ಯ *prop;
	ಧ್ರುವ fdt32_t *val;

	ಯದಿ (!fdt || cpu_offset < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ಯದಿ (!prop || !len)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (strncmp (prop, "cpu", strlen ("cpu")))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	ಯದಿ (!val || len < ಪರಿಮಾಣ(fdt32_t))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (len > ಪರಿಮಾಣ(fdt32_t))
		val++;

	ಯದಿ (hartid)
		*hartid = fdt32_to_cpu(*val);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_cbom_block_size(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ cpu_offset, unsigned long *cbom_block_size)
{
	ಪೂರ್ಣಾಙ್ಕ len;
	ಧ್ರುವ ಶೂನ್ಯ *prop;
	ಧ್ರುವ fdt32_t *val;

	ಯದಿ (!fdt || cpu_offset < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ಯದಿ (!prop || !len)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (strncmp (prop, "cpu", strlen ("cpu")))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	ಯದಿ (!val || len < ಪರಿಮಾಣ(fdt32_t))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_max_enabled_hart_id(ಧ್ರುವ ಶೂನ್ಯ *fdt, u32 *max_hartid)
{
	u32 hartid;
	ಪೂರ್ಣಾಙ್ಕ err, cpu_offset, cpus_offset;

	ಯದಿ (!fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!max_hartid)
		ಪ್ರತ್ಯಾಗಮ 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ಯದಿ (cpus_offset < 0)
		ಪ್ರತ್ಯಾಗಮ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ಯದಿ (err)
			ಅನುವರ್ತ;

		ಯದಿ (!fdt_ಗಂಟು_is_enabled(fdt, cpu_offset))
			ಅನುವರ್ತ;

		ಯದಿ (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_timebase_frequency(ಧ್ರುವ ಶೂನ್ಯ *fdt, unsigned long *freq)
{
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ len, cpus_offset;

	ಯದಿ (!fdt || !freq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ಯದಿ (cpus_offset < 0)
		ಪ್ರತ್ಯಾಗಮ cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	ಯದಿ (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	ಪ್ರತ್ಯಾಗಮ 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_parse_isa_one_hart(ಧ್ರುವ ವರ್ಣ *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	ವರ್ಣ mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	ಯದಿ (isa[i] == 'r' || isa[i] == 'R')
		i++;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (isa[i] == 'v' || isa[i] == 'V')
		i++;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	ಅನ್ಯಥಾ ಯದಿ (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Skip base ISA extensions */
	ಪ್ರತಿ (; i < isa_len; i++) {
		ಯದಿ (isa[i] == '_')
			ಭಙ್ಗ;
	}

	ಯಾವತ್ (i < isa_len) {
		ಯದಿ (isa[i] != '_') {
			i++;
			ಅನುವರ್ತ;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ಯಾವತ್ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (ಪರಿಮಾಣ(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ಯದಿ (!j)
			ಅನುವರ್ತ;

#define set_multi_letter_ext(name, bit)				\
			ಯದಿ (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				ಅನುವರ್ತ;			\
			}

		ಪ್ರತಿ (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_parse_isa_extensions_one_hart(ಧ್ರುವ ವರ್ಣ *isa,
					      unsigned long *extensions,
					      ಪೂರ್ಣಾಙ್ಕ len)
{
	size_t i;

	ಪ್ರತಿ (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ಯದಿ (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_isa_extensions_all_harts(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	u32 hartid;
	ಧ್ರುವ fdt32_t *val;
	ಸಂರಚನಾ sbi_scratch *scratch;
	ಸಂರಚನಾ sbi_hart_features *hfeatures;
	ಪೂರ್ಣಾಙ್ಕ err, cpu_offset, cpus_offset, len;

	ಯದಿ (!fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ಯದಿ (cpus_offset < 0)
		ಪ್ರತ್ಯಾಗಮ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ಯದಿ (err)
			ಅನುವರ್ತ;

		ಯದಿ (!fdt_ಗಂಟು_is_enabled(fdt, cpu_offset))
			ಅನುವರ್ತ;

		scratch = sbi_hartid_to_scratch(hartid);
		ಯದಿ (!scratch)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ಯದಿ (!hfeatures)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		ಯದಿ (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ಧ್ರುವ ವರ್ಣ *)val,
							  hfeatures->extensions, len);
			ಅನುವರ್ತ;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		ಯದಿ (!val || len <= 0)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ಧ್ರುವ ವರ್ಣ *)val, hfeatures->extensions);
		ಯದಿ (err)
			ಪ್ರತ್ಯಾಗಮ err;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_parse_uart_ಗಂಟು_common(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
				      ಸಂರಚನಾ platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	ಪೂರ್ಣಾಙ್ಕ len, rc;
	ಧ್ರುವ fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ಯದಿ (nodeoffset < 0 || !uart || !fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_size)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ಯದಿ (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ಯದಿ (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		uart->baud = default_baud;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_gaisler_uart_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
				ಸಂರಚನಾ platform_uart_data *uart)
{
	ಪ್ರತ್ಯಾಗಮ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_renesas_scif_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
				ಸಂರಚನಾ platform_uart_data *uart)
{
	ಪ್ರತ್ಯಾಗಮ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_shakti_uart_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			       ಸಂರಚನಾ platform_uart_data *uart)
{
	ಪ್ರತ್ಯಾಗಮ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_sifive_uart_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			       ಸಂರಚನಾ platform_uart_data *uart)
{
	ಪ್ರತ್ಯಾಗಮ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_uart_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			ಸಂರಚನಾ platform_uart_data *uart)
{
	ಪೂರ್ಣಾಙ್ಕ len, rc;
	ಧ್ರುವ fdt32_t *val;

	rc = fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ಯದಿ (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ಯದಿ (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ಯದಿ (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_uart8250(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಸಂರಚನಾ platform_uart_data *uart,
		   ಧ್ರುವ ವರ್ಣ *compatible)
{
	ಪೂರ್ಣಾಙ್ಕ nodeoffset;

	ಯದಿ (!compatible || !uart || !fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	nodeoffset = fdt_ಗಂಟು_offset_by_compatible(fdt, -1, compatible);
	ಯದಿ (nodeoffset < 0)
		ಪ್ರತ್ಯಾಗಮ nodeoffset;

	ಪ್ರತ್ಯಾಗಮ fdt_parse_uart_ಗಂಟು(fdt, nodeoffset, uart);
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_xlnx_uartlite_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			       ಸಂರಚನಾ platform_uart_data *uart)
{
	ಪ್ರತ್ಯಾಗಮ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart, 0, 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_aplic_find_imsic_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				     ಸಂರಚನಾ imsic_data *imsic, bool mmode)
{
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ಯದಿ (val && len >= ಪರಿಮಾಣ(fdt32_t)) {
		noff = fdt_ಗಂಟು_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		ಯದಿ (noff < 0)
			ಪ್ರತ್ಯಾಗಮ noff;

		rc = fdt_parse_imsic_ಗಂಟು(fdt, noff, imsic);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		rc = imsic_data_check(imsic);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		ಯದಿ (imsic->targets_mmode == mmode) {
			ಪ್ರತ್ಯಾಗಮ 0;
		}
	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ಯದಿ (!val || len < ಪರಿಮಾಣ(fdt32_t))
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	len /= ಪರಿಮಾಣ(fdt32_t);

	ಪ್ರತಿ (i = 0; i < len; i++) {
		noff = fdt_ಗಂಟು_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		ಯದಿ (noff < 0)
			ಪ್ರತ್ಯಾಗಮ noff;

		rc = fdt_aplic_find_imsic_ಗಂಟು(fdt, noff, imsic, mmode);
		ಯದಿ (!rc)
			ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_aplic_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಸಂರಚನಾ aplic_data *aplic)
{
	bool child_found;
	ಧ್ರುವ fdt32_t *val;
	ಧ್ರುವ fdt32_t *del;
	ಸಂರಚನಾ imsic_data imsic = { 0 };
	ಪೂರ್ಣಾಙ್ಕ i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	ಸಂರಚನಾ aplic_delegate_data *deleg;

	ಯದಿ (nodeoff < 0 || !aplic || !fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_size)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ಯದಿ (len > 0)
		aplic->ಅಂಕೆ_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ಯದಿ (val && len > ಪರಿಮಾಣ(fdt32_t)) {
		len = len / ಪರಿಮಾಣ(fdt32_t);
		ಪ್ರತಿ (i = 0; i < len; i += 2) {
			ಯದಿ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				ಭಙ್ಗ;
			}
		}
		aplic->ಅಂಕೆ_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_ಗಂಟು(fdt, nodeoff, &imsic, true);
	ಯದಿ (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		ಯದಿ (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_ಗಂಟು(fdt, nodeoff, &imsic, false);
	ಯದಿ (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		ಯದಿ (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	ಪ್ರತಿ (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ಯದಿ (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ಯದಿ (!del || len < (3 * ಪರಿಮಾಣ(fdt32_t)))
		ಗಚ್ಛ skip_delegate_parse;
	d = 0;
	dcnt = len / ಪರಿಮಾಣ(fdt32_t);
	ಪ್ರತಿ (i = 0; i < dcnt; i += 3) {
		ಯದಿ (d >= APLIC_MAX_DELEGATE)
			ಭಙ್ಗ;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ಯದಿ (!val || len < ಪರಿಮಾಣ(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			ಅನುವರ್ತ;
		}
		len = len / ಪರಿಮಾಣ(fdt32_t);
		ಪ್ರತಿ (j = 0; j < len; j++) {
			ಯದಿ (del[i] != val[j])
				ಅನುವರ್ತ;
			deleg->child_index = j;
			child_found = true;
			ಭಙ್ಗ;
		}

		ಯದಿ (child_found) {
			d++;
		} ಅನ್ಯಥಾ {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	ಪ್ರತ್ಯಾಗಮ 0;
}

bool fdt_check_imsic_mlevel(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ i, len, noff = 0;

	ಯದಿ (!fdt)
		ಪ್ರತ್ಯಾಗಮ false;

	ಯಾವತ್ ((noff = fdt_ಗಂಟು_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ಯದಿ (val && len > ಪರಿಮಾಣ(fdt32_t)) {
			len = len / ಪರಿಮಾಣ(fdt32_t);
			ಪ್ರತಿ (i = 0; i < len; i += 2) {
				ಯದಿ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					ಪ್ರತ್ಯಾಗಮ true;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_imsic_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಸಂರಚನಾ imsic_data *imsic)
{
	ಧ್ರುವ fdt32_t *val;
	ಸಂರಚನಾ imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	ಪೂರ್ಣಾಙ್ಕ i, rc, len, nr_parent_irqs;

	ಯದಿ (nodeoff < 0 || !imsic || !fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ಯದಿ (val && len > ಪರಿಮಾಣ(fdt32_t)) {
		len = len / ಪರಿಮಾಣ(fdt32_t);
		nr_parent_irqs = len / 2;
		ಪ್ರತಿ (i = 0; i < len; i += 2) {
			ಯದಿ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				ಭಙ್ಗ;
			}
		}
	} ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ಯದಿ (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ಯದಿ (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} ಅನ್ಯಥಾ {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		ಯದಿ ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ಯದಿ (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ಯದಿ (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ಯದಿ (val && len > 0)
		imsic->ಅಂಕೆ_ids = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತಿ (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	ಪ್ರತಿ (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		ಯದಿ (rc < 0 || !reg_size)
			ಭಙ್ಗ;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	ಯದಿ (!imsic->regs[0].size)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_plic_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಸಂರಚನಾ plic_data *plic)
{
	ಪೂರ್ಣಾಙ್ಕ len, rc;
	ಧ್ರುವ fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ಯದಿ (nodeoffset < 0 || !plic || !fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_size)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ಯದಿ (len > 0)
		plic->ಅಂಕೆ_src = fdt32_to_cpu(*val);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_plic(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಸಂರಚನಾ plic_data *plic, ಧ್ರುವ ವರ್ಣ *compat)
{
	ಪೂರ್ಣಾಙ್ಕ nodeoffset;

	ಯದಿ (!compat || !plic || !fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	nodeoffset = fdt_ಗಂಟು_offset_by_compatible(fdt, -1, compat);
	ಯದಿ (nodeoffset < 0)
		ಪ್ರತ್ಯಾಗಮ nodeoffset;

	ಪ್ರತ್ಯಾಗಮ fdt_parse_plic_ಗಂಟು(fdt, nodeoffset, plic);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_get_aclint_addr_size_by_name(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_ಗಂಟು_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_ಗಂಟು_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_size)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_get_aclint_addr_size(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_size)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	ಯದಿ (out_addr2)
		*out_addr2 = reg_addr;
	ಯದಿ (out_size2)
		*out_size2 = reg_size;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_aclint_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_ಎಣಿಕೆ)
{
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ i, rc, ಎಣಿಕೆ, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ಯದಿ (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", ರಿಕ್ತ))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	ಅನ್ಯಥಾ
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	*out_first_hartid = 0;
	*out_hart_ಎಣಿಕೆ = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ಎಣಿಕೆ);
	ಯದಿ (!val || ಎಣಿಕೆ < ಪರಿಮಾಣ(fdt32_t))
		ಪ್ರತ್ಯಾಗಮ 0;
	ಎಣಿಕೆ = ಎಣಿಕೆ / ಪರಿಮಾಣ(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	ಪ್ರತಿ (i = 0; i < (ಎಣಿಕೆ / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_ಗಂಟು_offset_by_phandle(fdt, phandle);
		ಯದಿ (cpu_intc_offset < 0)
			ಅನುವರ್ತ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ಯದಿ (cpu_offset < 0)
			ಅನುವರ್ತ;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ಯದಿ (rc)
			ಅನುವರ್ತ;

		ಯದಿ (match_hwirq == hwirq) {
			ಯದಿ (hartid < first_hartid)
				first_hartid = hartid;
			ಯದಿ (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	ಯದಿ ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_ಎಣಿಕೆ = last_hartid - first_hartid + 1;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_plmt_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_ಎಣಿಕೆ)
{
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ rc, i, ಎಣಿಕೆ;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ಯದಿ (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_ಎಣಿಕೆ || !plmt_size)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ಯದಿ (rc < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ಎಣಿಕೆ);
	ಯದಿ (!val || ಎಣಿಕೆ < ಪರಿಮಾಣ(fdt32_t))
		ಪ್ರತ್ಯಾಗಮ 0;
	ಎಣಿಕೆ = ಎಣಿಕೆ / ಪರಿಮಾಣ(fdt32_t);

	hcount = 0;
	ಪ್ರತಿ (i = 0; i < (ಎಣಿಕೆ / 2); i++) {
		ಪೂರ್ಣಾಙ್ಕ cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_ಗಂಟು_offset_by_phandle(fdt, phandle);
		ಯದಿ (cpu_intc_offset < 0)
			ಅನುವರ್ತ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ಯದಿ (cpu_offset < 0)
			ಅನುವರ್ತ;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ಯದಿ (rc)
			ಅನುವರ್ತ;

		ಯದಿ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			ಅನುವರ್ತ;

		ಯದಿ (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_ಎಣಿಕೆ = hcount;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_plicsw_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_ಎಣಿಕೆ)
{
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ rc, i, ಎಣಿಕೆ;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ಯದಿ (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_ಎಣಿಕೆ || !size)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ಯದಿ (rc < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ಎಣಿಕೆ);
	ಯದಿ (!val || ಎಣಿಕೆ < ಪರಿಮಾಣ(fdt32_t))
		ಪ್ರತ್ಯಾಗಮ 0;
	ಎಣಿಕೆ = ಎಣಿಕೆ / ಪರಿಮಾಣ(fdt32_t);

	hcount = 0;
	ಪ್ರತಿ (i = 0; i < (ಎಣಿಕೆ / 2); i++) {
		ಪೂರ್ಣಾಙ್ಕ cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_ಗಂಟು_offset_by_phandle(fdt, phandle);
		ಯದಿ (cpu_intc_offset < 0)
			ಅನುವರ್ತ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ಯದಿ (cpu_offset < 0)
			ಅನುವರ್ತ;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ಯದಿ (rc)
			ಅನುವರ್ತ;

		ಯದಿ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			ಅನುವರ್ತ;

		ಯದಿ (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_ಎಣಿಕೆ = hcount;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parse_compat_addr(ಧ್ರುವ ಶೂನ್ಯ *fdt, uint64_t *addr,
			  ಧ್ರುವ ವರ್ಣ *compatible)
{
	ಪೂರ್ಣಾಙ್ಕ nodeoffset, rc;

	nodeoffset = fdt_ಗಂಟು_offset_by_compatible(fdt, -1, compatible);
	ಯದಿ (nodeoffset < 0)
		ಪ್ರತ್ಯಾಗಮ nodeoffset;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoffset, 0, addr, ರಿಕ್ತ);
	ಯದಿ (rc < 0 || !addr)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಪ್ರತ್ಯಾಗಮ 0;
}
