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

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_UART_FREQ		0
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_UART_BAUD		115200
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_UART_REG_ಸರಿಸು		0
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_UART_REG_IO_ಅಗಲ	1
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_UART_REG_ಸ್ಥಾನಾಂತರ		0

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_RENESAS_SCIF_FREQ		100000000
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_RENESAS_SCIF_BAUD		115200

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_SIFIVE_UART_FREQ		0
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_SIFIVE_UART_BAUD		115200

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_SHAKTI_UART_FREQ		50000000
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_SHAKTI_UART_BAUD		115200

ಪೂರ್ಣಾಂಕ fdt_parse_phandle_with_ವಾದಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prop, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cells_prop,
				ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_2, ರಚನೆ fdt_phandle_ವಾದಗಳು *ಹೊರಹೋಗು_ವಾದಗಳು)
{
	u32 i, pcells;
	ಪೂರ್ಣಾಂಕ len, pnodeoff;
	ಸ್ಥಿರಾಂಕ fdt32_t *list, *list_ಕೊನೆ, *val;

	ಆದರೆ (!fdt || (nodeoff < 0) || !prop || !cells_prop || !ಹೊರಹೋಗು_ವಾದಗಳು)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ಆದರೆ (!list)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;
	list_ಕೊನೆ = list + (len / ಗಾತ್ರ(*list));

	ಆಗ (list < list_ಕೊನೆ) {
		pnodeoff = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt,
						fdt32_ಗಮ್ಯ_cpu(*list));
		ಆದರೆ (pnodeoff < 0)
			ಹಿಂತಿರುಗಿ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ಆದರೆ (!val)
			ಹಿಂತಿರುಗಿ SBI_ENOENT;
		pcells = fdt32_ಗಮ್ಯ_cpu(*val);
		ಆದರೆ (FDT_MAX_PHANDLE_ವಾದಗಳು < pcells)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		ಆದರೆ (list + pcells > list_ಕೊನೆ)
			ಹಿಂತಿರುಗಿ SBI_ENOENT;

		ಆದರೆ (ಸೂಚ್ಯಂಕ_2 > 0) {
			list += pcells;
			ಸೂಚ್ಯಂಕ_2--;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಹೊರಹೋಗು_ವಾದಗಳು->ಗಂಟು_ಸ್ಥಾನಾಂತರ = pnodeoff;
			ಹೊರಹೋಗು_ವಾದಗಳು->ವಾದಗಳು_ಎಣಿಕೆ = pcells;
			ಗಾಗಿ (i = 0; i < pcells; i++)
				ಹೊರಹೋಗು_ವಾದಗಳು->ವಾದಗಳು[i] = fdt32_ಗಮ್ಯ_cpu(list[i]);
			ಹಿಂತಿರುಗಿ 0;
		}
	}

	ಹಿಂತಿರುಗಿ SBI_ENOENT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_translate_ವಿಳಾಸ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, uint64_t reg, ಪೂರ್ಣಾಂಕ ಪೋಷಕ,
				 uint64_t *addr)
{
	ಪೂರ್ಣಾಂಕ i, rlen;
	ಪೂರ್ಣಾಂಕ cell_ಪೋಷಕ_addr, cell_ಮಗು_addr, cell_ಗಾತ್ರ;
	ಸ್ಥಿರಾಂಕ fdt32_t *ranges;
	uint64_t ಸ್ಥಾನಾಂತರ, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, ಪೋಷಕ, "ranges", &rlen);
	ಆದರೆ (ranges && rlen > 0) {
		cell_ಮಗು_addr = fdt_ವಿಳಾಸ_cells(fdt, ಪೋಷಕ);
		ಆದರೆ (cell_ಮಗು_addr < 1)
			ಹಿಂತಿರುಗಿ SBI_ENODEV;

		cell_ಪೋಷಕ_addr = fdt_ವಿಳಾಸ_cells(fdt, fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, ಪೋಷಕ));
		ಆದರೆ (cell_ಪೋಷಕ_addr < 1)
			ಹಿಂತಿರುಗಿ SBI_ENODEV;

		cell_ಗಾತ್ರ = fdt_ಗಾತ್ರ_cells(fdt, ಪೋಷಕ);
		ಆದರೆ (cell_ಗಾತ್ರ < 0)
			ಹಿಂತಿರುಗಿ SBI_ENODEV;

		ಗಾಗಿ (i = 0; i < cell_ಮಗು_addr; i++)
			caddr = (caddr << 32) | fdt32_ಗಮ್ಯ_cpu(*ranges++);
		ಗಾಗಿ (i = 0; i < cell_ಪೋಷಕ_addr; i++)
			paddr = (paddr << 32) | fdt32_ಗಮ್ಯ_cpu(*ranges++);
		ಗಾಗಿ (i = 0; i < cell_ಗಾತ್ರ; i++)
			rsize = (rsize << 32) | fdt32_ಗಮ್ಯ_cpu(*ranges++);
		ಆದರೆ (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ಹಿಂತಿರುಗಿ SBI_ENODEV;
		}
		ಸ್ಥಾನಾಂತರ = reg - caddr;
		*addr = paddr + ಸ್ಥಾನಾಂತರ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* No translation required */
		*addr = reg;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಗಂಟು, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_2,
			   uint64_t *addr, uint64_t *ಗಾತ್ರ_2)
{
	ಪೂರ್ಣಾಂಕ ಪೋಷಕ, len, i, rc;
	ಪೂರ್ಣಾಂಕ cell_addr, cell_ಗಾತ್ರ;
	ಸ್ಥಿರಾಂಕ fdt32_t *prop_addr, *prop_ಗಾತ್ರ;
	uint64_t ತಾತ್ಕಾಲಿಕ = 0;

	ಆದರೆ (!fdt || ಗಂಟು < 0 || ಸೂಚ್ಯಂಕ_2 < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಪೋಷಕ = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, ಗಂಟು);
	ಆದರೆ (ಪೋಷಕ < 0)
		ಹಿಂತಿರುಗಿ ಪೋಷಕ;
	cell_addr = fdt_ವಿಳಾಸ_cells(fdt, ಪೋಷಕ);
	ಆದರೆ (cell_addr < 1)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	cell_ಗಾತ್ರ = fdt_ಗಾತ್ರ_cells(fdt, ಪೋಷಕ);
	ಆದರೆ (cell_ಗಾತ್ರ < 0)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, ಗಂಟು, "reg", &len);
	ಆದರೆ (!prop_addr)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆದರೆ ((len / ಗಾತ್ರ(u32)) <= (ಸೂಚ್ಯಂಕ_2 * (cell_addr + cell_ಗಾತ್ರ)))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	prop_addr = prop_addr + (ಸೂಚ್ಯಂಕ_2 * (cell_addr + cell_ಗಾತ್ರ));
	prop_ಗಾತ್ರ = prop_addr + cell_addr;

	ಆದರೆ (addr) {
		ಗಾಗಿ (i = 0; i < cell_addr; i++)
			ತಾತ್ಕಾಲಿಕ = (ತಾತ್ಕಾಲಿಕ << 32) | fdt32_ಗಮ್ಯ_cpu(*prop_addr++);
		ಮಾಡು {
			ಆದರೆ (ಪೋಷಕ < 0)
				ನಿಲ್ಲಿಸು;
			rc  = fdt_translate_ವಿಳಾಸ(fdt, ತಾತ್ಕಾಲಿಕ, ಪೋಷಕ, addr);
			ಆದರೆ (rc)
				ನಿಲ್ಲಿಸು;
			ಪೋಷಕ = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, ಪೋಷಕ);
			ತಾತ್ಕಾಲಿಕ = *addr;
		} ಆಗ (1);
	}
	ತಾತ್ಕಾಲಿಕ = 0;

	ಆದರೆ (ಗಾತ್ರ_2) {
		ಗಾಗಿ (i = 0; i < cell_ಗಾತ್ರ; i++)
			ತಾತ್ಕಾಲಿಕ = (ತಾತ್ಕಾಲಿಕ << 32) | fdt32_ಗಮ್ಯ_cpu(*prop_ಗಾತ್ರ++);
		*ಗಾತ್ರ_2 = ತಾತ್ಕಾಲಿಕ;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_get_ಗಂಟು_addr_ಗಾತ್ರ_by_name(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಗಂಟು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
				   uint64_t *addr, uint64_t *ಗಾತ್ರ_2)
{
	ಪೂರ್ಣಾಂಕ i, j, ಎಣಿಕೆ_2;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *regname;

	ಆದರೆ (!fdt || ಗಂಟು < 0 || !name)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	val = fdt_getprop(fdt, ಗಂಟು, "reg-names", &ಎಣಿಕೆ_2);
	ಆದರೆ (!val)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಗಾಗಿ (i = 0, j = 0; i < ಎಣಿಕೆ_2; i++, j++) {
		regname = val + i;

		ಆದರೆ (strcmp(name, regname) == 0)
			ಹಿಂತಿರುಗಿ fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, ಗಂಟು, j, addr, ಗಾತ್ರ_2);

		i += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(regname);
	}

	ಹಿಂತಿರುಗಿ SBI_ENODEV;
}

bool fdt_ಗಂಟು_is_enabled(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff)
{
	ಪೂರ್ಣಾಂಕ len;
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ಆದರೆ (!prop)
		ಹಿಂತಿರುಗಿ true;

	ಆದರೆ (!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(prop, "okay", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("okay")))
		ಹಿಂತಿರುಗಿ true;

	ಆದರೆ (!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(prop, "ok", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("ok")))
		ಹಿಂತಿರುಗಿ true;

	ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ fdt_parse_hart_id(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ cpu_ಸ್ಥಾನಾಂತರ, u32 *hartid)
{
	ಪೂರ್ಣಾಂಕ len;
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *prop;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;

	ಆದರೆ (!fdt || cpu_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_ಸ್ಥಾನಾಂತರ, "device_type", &len);
	ಆದರೆ (!prop || !len)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ (prop, "cpu", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ ("cpu")))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_ಸ್ಥಾನಾಂತರ, "reg", &len);
	ಆದರೆ (!val || len < ಗಾತ್ರ(fdt32_t))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (len > ಗಾತ್ರ(fdt32_t))
		val++;

	ಆದರೆ (hartid)
		*hartid = fdt32_ಗಮ್ಯ_cpu(*val);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_cbom_ಖಂಡ_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ cpu_ಸ್ಥಾನಾಂತರ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *cbom_ಖಂಡ_ಗಾತ್ರ)
{
	ಪೂರ್ಣಾಂಕ len;
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *prop;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;

	ಆದರೆ (!fdt || cpu_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_ಸ್ಥಾನಾಂತರ, "device_type", &len);
	ಆದರೆ (!prop || !len)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ (prop, "cpu", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ ("cpu")))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_ಸ್ಥಾನಾಂತರ, "riscv,cbom-block-size", &len);
	ಆದರೆ (!val || len < ಗಾತ್ರ(fdt32_t))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (cbom_ಖಂಡ_ಗಾತ್ರ)
		*cbom_ಖಂಡ_ಗಾತ್ರ = fdt32_ಗಮ್ಯ_cpu(*val);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_max_enabled_hart_id(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, u32 *max_hartid)
{
	u32 hartid;
	ಪೂರ್ಣಾಂಕ err, cpu_ಸ್ಥಾನಾಂತರ, cpus_ಸ್ಥಾನಾಂತರ;

	ಆದರೆ (!fdt)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!max_hartid)
		ಹಿಂತಿರುಗಿ 0;

	*max_hartid = 0;

	cpus_ಸ್ಥಾನಾಂತರ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/cpus");
	ಆದರೆ (cpus_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ cpus_ಸ್ಥಾನಾಂತರ;

	fdt_for_each_subnode(cpu_ಸ್ಥಾನಾಂತರ, fdt, cpus_ಸ್ಥಾನಾಂತರ) {
		err = fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &hartid);
		ಆದರೆ (err)
			ಮುಂದುವರಿ;

		ಆದರೆ (!fdt_ಗಂಟು_is_enabled(fdt, cpu_ಸ್ಥಾನಾಂತರ))
			ಮುಂದುವರಿ;

		ಆದರೆ (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_timebase_frequency(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *freq)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ len, cpus_ಸ್ಥಾನಾಂತರ;

	ಆದರೆ (!fdt || !freq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	cpus_ಸ್ಥಾನಾಂತರ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/cpus");
	ಆದರೆ (cpus_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ cpus_ಸ್ಥಾನಾಂತರ;

	val = fdt_getprop(fdt, cpus_ಸ್ಥಾನಾಂತರ, "timebase-frequency", &len);
	ಆದರೆ (len > 0 && val)
		*freq = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_ISA_EXT_NAME_LEN_MAX	32

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_parse_isa_one_hart(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *isa, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *extensions)
{
	size_t i, j, isa_len;
	ಅಕ್ಷರ mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(isa);

	ಆದರೆ (isa[i] == 'r' || isa[i] == 'R')
		i++;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (isa[i] == 'v' || isa[i] == 'V')
		i++;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Skip base ISA extensions */
	ಗಾಗಿ (; i < isa_len; i++) {
		ಆದರೆ (isa[i] == '_')
			ನಿಲ್ಲಿಸು;
	}

	ಆಗ (i < isa_len) {
		ಆದರೆ (isa[i] != '_') {
			i++;
			ಮುಂದುವರಿ;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ಆಗ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (ಗಾತ್ರ(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ಆದರೆ (!j)
			ಮುಂದುವರಿ;

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_multi_letter_ext(name, ಬಿಟ್_2)				\
			ಆದರೆ (!strcmp(mstr, name)) {		\
				__set_bit(ಬಿಟ್_2, extensions);	\
				ಮುಂದುವರಿ;			\
			}

		ಗಾಗಿ (j = 0; j < SBI_HART_EXT_MAX; j++) {
			ಗಣ_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಗಣ_multi_letter_ext
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_parse_isa_extensions_one_hart(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *isa,
					      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *extensions,
					      ಪೂರ್ಣಾಂಕ len)
{
	size_t i;

	ಗಾಗಿ (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ಆದರೆ (fdt_stringlist_ಒಳಗೊಂಡಿದೆ(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

ಪೂರ್ಣಾಂಕ fdt_parse_isa_extensions_all_harts(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	u32 hartid;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ರಚನೆ sbi_scratch *scratch;
	ರಚನೆ sbi_hart_features *hfeatures;
	ಪೂರ್ಣಾಂಕ err, cpu_ಸ್ಥಾನಾಂತರ, cpus_ಸ್ಥಾನಾಂತರ, len;

	ಆದರೆ (!fdt)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	cpus_ಸ್ಥಾನಾಂತರ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/cpus");
	ಆದರೆ (cpus_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ cpus_ಸ್ಥಾನಾಂತರ;

	fdt_for_each_subnode(cpu_ಸ್ಥಾನಾಂತರ, fdt, cpus_ಸ್ಥಾನಾಂತರ) {
		err = fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &hartid);
		ಆದರೆ (err)
			ಮುಂದುವರಿ;

		ಆದರೆ (!fdt_ಗಂಟು_is_enabled(fdt, cpu_ಸ್ಥಾನಾಂತರ))
			ಮುಂದುವರಿ;

		scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
		ಆದರೆ (!scratch)
			ಹಿಂತಿರುಗಿ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ಆದರೆ (!hfeatures)
			ಹಿಂತಿರುಗಿ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_ಸ್ಥಾನಾಂತರ, "riscv,isa-extensions", &len);
		ಆದರೆ (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)val,
							  hfeatures->extensions, len);
			ಮುಂದುವರಿ;
		}

		val = fdt_getprop(fdt, cpu_ಸ್ಥಾನಾಂತರ, "riscv,isa", &len);
		ಆದರೆ (!val || len <= 0)
			ಹಿಂತಿರುಗಿ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)val, hfeatures->extensions);
		ಆದರೆ (err)
			ಹಿಂತಿರುಗಿ err;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_parse_uart_ಗಂಟು_common(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
				      ರಚನೆ platform_uart_ದತ್ತಾಂಶ *uart,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ default_freq,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ default_baud)
{
	ಪೂರ್ಣಾಂಕ len, rc;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	uint64_t reg_addr, reg_ಗಾತ್ರ;

	ಆದರೆ (nodeoffset < 0 || !uart || !fdt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ಆದರೆ (len > 0 && val)
		uart->freq = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ಆದರೆ (len > 0 && val)
		uart->baud = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		uart->baud = default_baud;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_gaisler_uart_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
				ರಚನೆ platform_uart_ದತ್ತಾಂಶ *uart)
{
	ಹಿಂತಿರುಗಿ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

ಪೂರ್ಣಾಂಕ fdt_parse_renesas_scif_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
				ರಚನೆ platform_uart_ದತ್ತಾಂಶ *uart)
{
	ಹಿಂತಿರುಗಿ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

ಪೂರ್ಣಾಂಕ fdt_parse_shakti_uart_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
			       ರಚನೆ platform_uart_ದತ್ತಾಂಶ *uart)
{
	ಹಿಂತಿರುಗಿ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

ಪೂರ್ಣಾಂಕ fdt_parse_sifive_uart_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
			       ರಚನೆ platform_uart_ದತ್ತಾಂಶ *uart)
{
	ಹಿಂತಿರುಗಿ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

ಪೂರ್ಣಾಂಕ fdt_parse_uart_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
			ರಚನೆ platform_uart_ದತ್ತಾಂಶ *uart)
{
	ಪೂರ್ಣಾಂಕ len, rc;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;

	rc = fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ಆದರೆ (len > 0 && val)
		uart->reg_ಸರಿಸು = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		uart->reg_ಸರಿಸು = DEFAULT_UART_REG_ಸರಿಸು;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ಆದರೆ (len > 0 && val)
		uart->reg_io_ಅಗಲ = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		uart->reg_io_ಅಗಲ = DEFAULT_UART_REG_IO_ಅಗಲ;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ಆದರೆ (len > 0 && val)
		uart->reg_ಸ್ಥಾನಾಂತರ = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		uart->reg_ಸ್ಥಾನಾಂತರ = DEFAULT_UART_REG_ಸ್ಥಾನಾಂತರ;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_uart8250(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ರಚನೆ platform_uart_ದತ್ತಾಂಶ *uart,
		   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compatible)
{
	ಪೂರ್ಣಾಂಕ nodeoffset;

	ಆದರೆ (!compatible || !uart || !fdt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	nodeoffset = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, -1, compatible);
	ಆದರೆ (nodeoffset < 0)
		ಹಿಂತಿರುಗಿ nodeoffset;

	ಹಿಂತಿರುಗಿ fdt_parse_uart_ಗಂಟು(fdt, nodeoffset, uart);
}

ಪೂರ್ಣಾಂಕ fdt_parse_xlnx_uartlite_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
			       ರಚನೆ platform_uart_ದತ್ತಾಂಶ *uart)
{
	ಹಿಂತಿರುಗಿ fdt_parse_uart_ಗಂಟು_common(fdt, nodeoffset, uart, 0, 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_aplic_ಕಂಡುಹಿಡಿ_imsic_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				     ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic, bool mmode)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ಆದರೆ (val && len >= ಗಾತ್ರ(fdt32_t)) {
		noff = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, fdt32_ಗಮ್ಯ_cpu(*val));
		ಆದರೆ (noff < 0)
			ಹಿಂತಿರುಗಿ noff;

		rc = fdt_parse_imsic_ಗಂಟು(fdt, noff, imsic);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		rc = imsic_ದತ್ತಾಂಶ_check(imsic);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		ಆದರೆ (imsic->targets_mmode == mmode) {
			ಹಿಂತಿರುಗಿ 0;
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ಆದರೆ (!val || len < ಗಾತ್ರ(fdt32_t))
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	len /= ಗಾತ್ರ(fdt32_t);

	ಗಾಗಿ (i = 0; i < len; i++) {
		noff = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, fdt32_ಗಮ್ಯ_cpu(val[i]));
		ಆದರೆ (noff < 0)
			ಹಿಂತಿರುಗಿ noff;

		rc = fdt_aplic_ಕಂಡುಹಿಡಿ_imsic_ಗಂಟು(fdt, noff, imsic, mmode);
		ಆದರೆ (!rc)
			ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ rc;
}

ಪೂರ್ಣಾಂಕ fdt_parse_aplic_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic)
{
	bool ಮಗು_ಸಿಕ್ಕಿತು;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಸ್ಥಿರಾಂಕ fdt32_t *del;
	ರಚನೆ imsic_ದತ್ತಾಂಶ imsic = { 0 };
	ಪೂರ್ಣಾಂಕ i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_ಗಾತ್ರ;
	ರಚನೆ aplic_delegate_ದತ್ತಾಂಶ *deleg;

	ಆದರೆ (nodeoff < 0 || !aplic || !fdt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->ಗಾತ್ರ_2 = reg_ಗಾತ್ರ;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ಆದರೆ (len > 0)
		aplic->ಅಂಕೆ_ಮೂಲ = fdt32_ಗಮ್ಯ_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ಆದರೆ (val && len > ಗಾತ್ರ(fdt32_t)) {
		len = len / ಗಾತ್ರ(fdt32_t);
		ಗಾಗಿ (i = 0; i < len; i += 2) {
			ಆದರೆ (fdt32_ಗಮ್ಯ_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				ನಿಲ್ಲಿಸು;
			}
		}
		aplic->ಅಂಕೆ_idc = len / 2;
	}

	rc = fdt_aplic_ಕಂಡುಹಿಡಿ_imsic_ಗಂಟು(fdt, nodeoff, &imsic, true);
	ಆದರೆ (!rc) {
		aplic->ಪೋಷಕ_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
		aplic->msicfg_mmode.lhxw = imsic.hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
		aplic->msicfg_mmode.hhxw = imsic.ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
		aplic->msicfg_mmode.hhxs = imsic.ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಸರಿಸು;
		ಆದರೆ (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_ಪುಟ_ಸರಿಸು))
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.ತಳ_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_ಕಂಡುಹಿಡಿ_imsic_ಗಂಟು(fdt, nodeoff, &imsic, false);
	ಆದರೆ (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
		aplic->msicfg_smode.lhxw = imsic.hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
		aplic->msicfg_smode.hhxw = imsic.ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
		aplic->msicfg_smode.hhxs = imsic.ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಸರಿಸು;
		ಆದರೆ (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_ಪುಟ_ಸರಿಸು))
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.ತಳ_addr = imsic.regs[0].addr;
	}

	ಗಾಗಿ (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->ಮೊದಲ_irq = 0;
		deleg->ಕೊನೆಯ_irq = 0;
		deleg->ಮಗು_ಸೂಚ್ಯಂಕ = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ಆದರೆ (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ಆದರೆ (!del || len < (3 * ಗಾತ್ರ(fdt32_t)))
		ಹೋಗು skip_delegate_parse;
	d = 0;
	dcnt = len / ಗಾತ್ರ(fdt32_t);
	ಗಾಗಿ (i = 0; i < dcnt; i += 3) {
		ಆದರೆ (d >= APLIC_MAX_DELEGATE)
			ನಿಲ್ಲಿಸು;
		deleg = &aplic->delegate[d];

		deleg->ಮೊದಲ_irq = fdt32_ಗಮ್ಯ_cpu(del[i + 1]);
		deleg->ಕೊನೆಯ_irq = fdt32_ಗಮ್ಯ_cpu(del[i + 2]);
		deleg->ಮಗು_ಸೂಚ್ಯಂಕ = 0;

		ಮಗು_ಸಿಕ್ಕಿತು = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ಆದರೆ (!val || len < ಗಾತ್ರ(fdt32_t)) {
			deleg->ಮೊದಲ_irq = 0;
			deleg->ಕೊನೆಯ_irq = 0;
			deleg->ಮಗು_ಸೂಚ್ಯಂಕ = 0;
			ಮುಂದುವರಿ;
		}
		len = len / ಗಾತ್ರ(fdt32_t);
		ಗಾಗಿ (j = 0; j < len; j++) {
			ಆದರೆ (del[i] != val[j])
				ಮುಂದುವರಿ;
			deleg->ಮಗು_ಸೂಚ್ಯಂಕ = j;
			ಮಗು_ಸಿಕ್ಕಿತು = true;
			ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ (ಮಗು_ಸಿಕ್ಕಿತು) {
			d++;
		} ಇಲ್ಲದಿದ್ದರೆ {
			deleg->ಮೊದಲ_irq = 0;
			deleg->ಕೊನೆಯ_irq = 0;
			deleg->ಮಗು_ಸೂಚ್ಯಂಕ = 0;
		}
	}
skip_delegate_parse:

	ಹಿಂತಿರುಗಿ 0;
}

bool fdt_check_imsic_mlevel(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ i, len, noff = 0;

	ಆದರೆ (!fdt)
		ಹಿಂತಿರುಗಿ false;

	ಆಗ ((noff = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ಆದರೆ (val && len > ಗಾತ್ರ(fdt32_t)) {
			len = len / ಗಾತ್ರ(fdt32_t);
			ಗಾಗಿ (i = 0; i < len; i += 2) {
				ಆದರೆ (fdt32_ಗಮ್ಯ_cpu(val[i + 1]) == IRQ_M_EXT)
					ಹಿಂತಿರುಗಿ true;
			}
		}
	}

	ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ fdt_parse_imsic_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ರಚನೆ imsic_regs *regs;
	uint64_t reg_addr, reg_ಗಾತ್ರ;
	ಪೂರ್ಣಾಂಕ i, rc, len, nr_ಪೋಷಕ_irqs;

	ಆದರೆ (nodeoff < 0 || !imsic || !fdt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ಆದರೆ (val && len > ಗಾತ್ರ(fdt32_t)) {
		len = len / ಗಾತ್ರ(fdt32_t);
		nr_ಪೋಷಕ_irqs = len / 2;
		ಗಾಗಿ (i = 0; i < len; i += 2) {
			ಆದರೆ (fdt32_ಗಮ್ಯ_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				ನಿಲ್ಲಿಸು;
			}
		}
	} ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ಆದರೆ (val && len > 0)
		imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ಆದರೆ (val && len > 0) {
		imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = fdt32_ಗಮ್ಯ_cpu(*val);
	} ಇಲ್ಲದಿದ್ದರೆ {
		imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = sbi_fls(nr_ಪೋಷಕ_irqs);
		ಆದರೆ ((1UL << imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) < nr_ಪೋಷಕ_irqs)
			imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ಆದರೆ (val && len > 0)
		imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ಆದರೆ (val && len > 0)
		imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಸರಿಸು = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಸರಿಸು = 2 * IMSIC_MMIO_ಪುಟ_ಸರಿಸು;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ಆದರೆ (val && len > 0)
		imsic->ಅಂಕೆ_ids = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಗಾಗಿ (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->ಗಾತ್ರ_2 = 0;
	}

	ಗಾಗಿ (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, i,
					    &reg_addr, &reg_ಗಾತ್ರ);
		ಆದರೆ (rc < 0 || !reg_ಗಾತ್ರ)
			ನಿಲ್ಲಿಸು;
		regs->addr = reg_addr;
		regs->ಗಾತ್ರ_2 = reg_ಗಾತ್ರ;
	}
	ಆದರೆ (!imsic->regs[0].ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_plic_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ರಚನೆ plic_ದತ್ತಾಂಶ *plic)
{
	ಪೂರ್ಣಾಂಕ len, rc;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	uint64_t reg_addr, reg_ಗಾತ್ರ;

	ಆದರೆ (nodeoffset < 0 || !plic || !fdt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->ಗಾತ್ರ_2 = reg_ಗಾತ್ರ;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ಆದರೆ (len > 0)
		plic->ಅಂಕೆ_src = fdt32_ಗಮ್ಯ_cpu(*val);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_plic(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ರಚನೆ plic_ದತ್ತಾಂಶ *plic, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compat)
{
	ಪೂರ್ಣಾಂಕ nodeoffset;

	ಆದರೆ (!compat || !plic || !fdt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	nodeoffset = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, -1, compat);
	ಆದರೆ (nodeoffset < 0)
		ಹಿಂತಿರುಗಿ nodeoffset;

	ಹಿಂತಿರುಗಿ fdt_parse_plic_ಗಂಟು(fdt, nodeoffset, plic);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_get_aclint_addr_ಗಾತ್ರ_by_name(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
					    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_addr1,
					    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_size1,
					    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_addr2,
					    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_size2)
{
	ಪೂರ್ಣಾಂಕ rc;
	uint64_t reg_addr, reg_ಗಾತ್ರ;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_ಗಾತ್ರ)
		reg_addr = reg_ಗಾತ್ರ = 0;
	*ಹೊರಹೋಗು_addr1 = reg_addr;
	*ಹೊರಹೋಗು_size1 = reg_ಗಾತ್ರ;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	*ಹೊರಹೋಗು_addr2 = reg_addr;
	*ಹೊರಹೋಗು_size2 = reg_ಗಾತ್ರ;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_get_aclint_addr_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_addr1,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_size1,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_addr2,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_size2)
{
	ಪೂರ್ಣಾಂಕ rc;
	uint64_t reg_addr, reg_ಗಾತ್ರ;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	*ಹೊರಹೋಗು_addr1 = reg_addr;
	*ಹೊರಹೋಗು_size1 = reg_ಗಾತ್ರ;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoffset, 1,
				    &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_ಗಾತ್ರ)
		reg_addr = reg_ಗಾತ್ರ = 0;
	ಆದರೆ (ಹೊರಹೋಗು_addr2)
		*ಹೊರಹೋಗು_addr2 = reg_addr;
	ಆದರೆ (ಹೊರಹೋಗು_size2)
		*ಹೊರಹೋಗು_size2 = reg_ಗಾತ್ರ;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_aclint_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
			  bool for_timer, bool allow_regname,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_addr1, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_size1,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_addr2, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_size2,
			  u32 *ಹೊರಹೋಗು_ಮೊದಲ_hartid, u32 *ಹೊರಹೋಗು_hart_ಎಣಿಕೆ)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ i, rc, ಎಣಿಕೆ_2, cpu_ಸ್ಥಾನಾಂತರ, cpu_intc_ಸ್ಥಾನಾಂತರ;
	u32 phandle, hwirq, hartid, ಮೊದಲ_hartid, ಕೊನೆಯ_hartid;
	u32 ಹೊಂದಿಕೆ_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ಆದರೆ (nodeoffset < 0 || !fdt ||
	    !ಹೊರಹೋಗು_addr1 || !ಹೊರಹೋಗು_size1 ||
	    !ಹೊರಹೋಗು_ಮೊದಲ_hartid || !ಹೊರಹೋಗು_hart_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (for_timer && allow_regname && ಹೊರಹೋಗು_addr2 && ಹೊರಹೋಗು_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", ಖಾಲಿ))
		rc = fdt_get_aclint_addr_ಗಾತ್ರ_by_name(fdt, nodeoffset,
						      ಹೊರಹೋಗು_addr1, ಹೊರಹೋಗು_size1,
						      ಹೊರಹೋಗು_addr2, ಹೊರಹೋಗು_size2);
	ಇಲ್ಲದಿದ್ದರೆ
		rc = fdt_get_aclint_addr_ಗಾತ್ರ(fdt, nodeoffset,
					      ಹೊರಹೋಗು_addr1, ಹೊರಹೋಗು_size1,
					      ಹೊರಹೋಗು_addr2, ಹೊರಹೋಗು_size2);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	*ಹೊರಹೋಗು_ಮೊದಲ_hartid = 0;
	*ಹೊರಹೋಗು_hart_ಎಣಿಕೆ = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ಎಣಿಕೆ_2);
	ಆದರೆ (!val || ಎಣಿಕೆ_2 < ಗಾತ್ರ(fdt32_t))
		ಹಿಂತಿರುಗಿ 0;
	ಎಣಿಕೆ_2 = ಎಣಿಕೆ_2 / ಗಾತ್ರ(fdt32_t);

	ಮೊದಲ_hartid = -1U;
	ಕೊನೆಯ_hartid = 0;
	ಗಾಗಿ (i = 0; i < (ಎಣಿಕೆ_2 / 2); i++) {
		phandle = fdt32_ಗಮ್ಯ_cpu(val[2 * i]);
		hwirq = fdt32_ಗಮ್ಯ_cpu(val[(2 * i) + 1]);

		cpu_intc_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, phandle);
		ಆದರೆ (cpu_intc_ಸ್ಥಾನಾಂತರ < 0)
			ಮುಂದುವರಿ;

		cpu_ಸ್ಥಾನಾಂತರ = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, cpu_intc_ಸ್ಥಾನಾಂತರ);
		ಆದರೆ (cpu_ಸ್ಥಾನಾಂತರ < 0)
			ಮುಂದುವರಿ;

		rc = fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &hartid);
		ಆದರೆ (rc)
			ಮುಂದುವರಿ;

		ಆದರೆ (ಹೊಂದಿಕೆ_hwirq == hwirq) {
			ಆದರೆ (hartid < ಮೊದಲ_hartid)
				ಮೊದಲ_hartid = hartid;
			ಆದರೆ (hartid > ಕೊನೆಯ_hartid)
				ಕೊನೆಯ_hartid = hartid;
		}
	}

	ಆದರೆ ((ಕೊನೆಯ_hartid >= ಮೊದಲ_hartid) && ಮೊದಲ_hartid != -1U) {
		*ಹೊರಹೋಗು_ಮೊದಲ_hartid = ಮೊದಲ_hartid;
		*ಹೊರಹೋಗು_hart_ಎಣಿಕೆ = ಕೊನೆಯ_hartid - ಮೊದಲ_hartid + 1;
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_plmt_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *plmt_ತಳ,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *plmt_ಗಾತ್ರ, u32 *hart_ಎಣಿಕೆ)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ rc, i, ಎಣಿಕೆ_2;
	uint64_t reg_addr, reg_ಗಾತ್ರ;
	u32 phandle, hwirq, hartid, hcount;

	ಆದರೆ (nodeoffset < 0 || !fdt || !plmt_ತಳ ||
	    !hart_ಎಣಿಕೆ || !plmt_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	*plmt_ತಳ = reg_addr;
	*plmt_ಗಾತ್ರ = reg_ಗಾತ್ರ;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ಎಣಿಕೆ_2);
	ಆದರೆ (!val || ಎಣಿಕೆ_2 < ಗಾತ್ರ(fdt32_t))
		ಹಿಂತಿರುಗಿ 0;
	ಎಣಿಕೆ_2 = ಎಣಿಕೆ_2 / ಗಾತ್ರ(fdt32_t);

	hcount = 0;
	ಗಾಗಿ (i = 0; i < (ಎಣಿಕೆ_2 / 2); i++) {
		ಪೂರ್ಣಾಂಕ cpu_ಸ್ಥಾನಾಂತರ, cpu_intc_ಸ್ಥಾನಾಂತರ;

		phandle = fdt32_ಗಮ್ಯ_cpu(val[2 * i]);
		hwirq = fdt32_ಗಮ್ಯ_cpu(val[2 * i + 1]);

		cpu_intc_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, phandle);
		ಆದರೆ (cpu_intc_ಸ್ಥಾನಾಂತರ < 0)
			ಮುಂದುವರಿ;

		cpu_ಸ್ಥಾನಾಂತರ = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, cpu_intc_ಸ್ಥಾನಾಂತರ);
		ಆದರೆ (cpu_ಸ್ಥಾನಾಂತರ < 0)
			ಮುಂದುವರಿ;

		rc = fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &hartid);

		ಆದರೆ (rc)
			ಮುಂದುವರಿ;

		ಆದರೆ (SBI_HARTMASK_MAX_ಬಿಟ್ಟುಗಳು <= sbi_hartid_ಗಮ್ಯ_hartindex(hartid))
			ಮುಂದುವರಿ;

		ಆದರೆ (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_ಎಣಿಕೆ = hcount;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_plicsw_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *plicsw_ತಳ,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಗಾತ್ರ_2, u32 *hart_ಎಣಿಕೆ)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ rc, i, ಎಣಿಕೆ_2;
	uint64_t reg_addr, reg_ಗಾತ್ರ;
	u32 phandle, hwirq, hartid, hcount;

	ಆದರೆ (nodeoffset < 0 || !fdt || !plicsw_ತಳ ||
	    !hart_ಎಣಿಕೆ || !ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	*plicsw_ತಳ = reg_addr;
	*ಗಾತ್ರ_2 = reg_ಗಾತ್ರ;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ಎಣಿಕೆ_2);
	ಆದರೆ (!val || ಎಣಿಕೆ_2 < ಗಾತ್ರ(fdt32_t))
		ಹಿಂತಿರುಗಿ 0;
	ಎಣಿಕೆ_2 = ಎಣಿಕೆ_2 / ಗಾತ್ರ(fdt32_t);

	hcount = 0;
	ಗಾಗಿ (i = 0; i < (ಎಣಿಕೆ_2 / 2); i++) {
		ಪೂರ್ಣಾಂಕ cpu_ಸ್ಥಾನಾಂತರ, cpu_intc_ಸ್ಥಾನಾಂತರ;

		phandle = fdt32_ಗಮ್ಯ_cpu(val[2 * i]);
		hwirq = fdt32_ಗಮ್ಯ_cpu(val[2 * i + 1]);

		cpu_intc_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, phandle);
		ಆದರೆ (cpu_intc_ಸ್ಥಾನಾಂತರ < 0)
			ಮುಂದುವರಿ;

		cpu_ಸ್ಥಾನಾಂತರ = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, cpu_intc_ಸ್ಥಾನಾಂತರ);
		ಆದರೆ (cpu_ಸ್ಥಾನಾಂತರ < 0)
			ಮುಂದುವರಿ;

		rc = fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &hartid);

		ಆದರೆ (rc)
			ಮುಂದುವರಿ;

		ಆದರೆ (SBI_HARTMASK_MAX_ಬಿಟ್ಟುಗಳು <= sbi_hartid_ಗಮ್ಯ_hartindex(hartid))
			ಮುಂದುವರಿ;

		ಆದರೆ (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_ಎಣಿಕೆ = hcount;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_parse_compat_addr(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, uint64_t *addr,
			  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compatible)
{
	ಪೂರ್ಣಾಂಕ nodeoffset, rc;

	nodeoffset = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, -1, compatible);
	ಆದರೆ (nodeoffset < 0)
		ಹಿಂತಿರುಗಿ nodeoffset;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoffset, 0, addr, ಖಾಲಿ);
	ಆದರೆ (rc < 0 || !addr)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಹಿಂತಿರುಗಿ 0;
}
