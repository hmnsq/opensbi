// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hartmask.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/aplic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/imsic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/plic.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_UART_FREQ		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_UART_BAUD		115200
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_UART_REG_ਖਿਸਕਾਓ		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_UART_REG_IO_ਚੌੜਾਈ	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_UART_REG_ਵਿੱਥ		0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_RENESAS_SCIF_FREQ		100000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_RENESAS_SCIF_BAUD		115200

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_SIFIVE_UART_FREQ		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_SIFIVE_UART_BAUD		115200

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_SHAKTI_UART_FREQ		50000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_SHAKTI_UART_BAUD		115200

ਪੂਰਨ fdt_parse_phandle_with_ਦਲੀਲਾਂ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				ਅਚਲ ਅੱਖਰ *prop, ਅਚਲ ਅੱਖਰ *cells_prop,
				ਪੂਰਨ ਸੂਚਕਾਂਕ_2, ਬਣਤਰ fdt_phandle_ਦਲੀਲਾਂ *ਬਾਹਰਲਾ_ਦਲੀਲਾਂ)
{
	u32 i, pcells;
	ਪੂਰਨ len, pnodeoff;
	ਅਚਲ fdt32_t *list, *list_ਸਿਰਾ, *val;

	ਜੇ (!fdt || (nodeoff < 0) || !prop || !cells_prop || !ਬਾਹਰਲਾ_ਦਲੀਲਾਂ)
		ਵਾਪਸ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ਜੇ (!list)
		ਵਾਪਸ SBI_ENOENT;
	list_ਸਿਰਾ = list + (len / ਆਕਾਰ(*list));

	ਜਦੋਂ (list < list_ਸਿਰਾ) {
		pnodeoff = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt,
						fdt32_ਮੰਜ਼ਿਲ_cpu(*list));
		ਜੇ (pnodeoff < 0)
			ਵਾਪਸ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ਜੇ (!val)
			ਵਾਪਸ SBI_ENOENT;
		pcells = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
		ਜੇ (FDT_MAX_PHANDLE_ਦਲੀਲਾਂ < pcells)
			ਵਾਪਸ SBI_EINVAL;
		ਜੇ (list + pcells > list_ਸਿਰਾ)
			ਵਾਪਸ SBI_ENOENT;

		ਜੇ (ਸੂਚਕਾਂਕ_2 > 0) {
			list += pcells;
			ਸੂਚਕਾਂਕ_2--;
		} ਨਹੀਂਤਾਂ {
			ਬਾਹਰਲਾ_ਦਲੀਲਾਂ->ਗੰਢ_ਵਿੱਥ = pnodeoff;
			ਬਾਹਰਲਾ_ਦਲੀਲਾਂ->ਦਲੀਲਾਂ_ਗਿਣਤੀ = pcells;
			ਲਈ (i = 0; i < pcells; i++)
				ਬਾਹਰਲਾ_ਦਲੀਲਾਂ->ਦਲੀਲਾਂ[i] = fdt32_ਮੰਜ਼ਿਲ_cpu(list[i]);
			ਵਾਪਸ 0;
		}
	}

	ਵਾਪਸ SBI_ENOENT;
}

ਸਥਿਰ ਪੂਰਨ fdt_translate_ਪਤਾ(ਅਚਲ ਖਾਲੀ *fdt, uint64_t reg, ਪੂਰਨ ਮਾਪਾ,
				 uint64_t *addr)
{
	ਪੂਰਨ i, rlen;
	ਪੂਰਨ cell_ਮਾਪਾ_addr, cell_ਬੱਚਾ_addr, cell_ਮਾਪ;
	ਅਚਲ fdt32_t *ranges;
	uint64_t ਵਿੱਥ, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, ਮਾਪਾ, "ranges", &rlen);
	ਜੇ (ranges && rlen > 0) {
		cell_ਬੱਚਾ_addr = fdt_ਪਤਾ_cells(fdt, ਮਾਪਾ);
		ਜੇ (cell_ਬੱਚਾ_addr < 1)
			ਵਾਪਸ SBI_ENODEV;

		cell_ਮਾਪਾ_addr = fdt_ਪਤਾ_cells(fdt, fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, ਮਾਪਾ));
		ਜੇ (cell_ਮਾਪਾ_addr < 1)
			ਵਾਪਸ SBI_ENODEV;

		cell_ਮਾਪ = fdt_ਮਾਪ_cells(fdt, ਮਾਪਾ);
		ਜੇ (cell_ਮਾਪ < 0)
			ਵਾਪਸ SBI_ENODEV;

		ਲਈ (i = 0; i < cell_ਬੱਚਾ_addr; i++)
			caddr = (caddr << 32) | fdt32_ਮੰਜ਼ਿਲ_cpu(*ranges++);
		ਲਈ (i = 0; i < cell_ਮਾਪਾ_addr; i++)
			paddr = (paddr << 32) | fdt32_ਮੰਜ਼ਿਲ_cpu(*ranges++);
		ਲਈ (i = 0; i < cell_ਮਾਪ; i++)
			rsize = (rsize << 32) | fdt32_ਮੰਜ਼ਿਲ_cpu(*ranges++);
		ਜੇ (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ਵਾਪਸ SBI_ENODEV;
		}
		ਵਿੱਥ = reg - caddr;
		*addr = paddr + ਵਿੱਥ;
	} ਨਹੀਂਤਾਂ {
		/* No translation required */
		*addr = reg;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_get_ਗੰਢ_addr_ਮਾਪ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਗੰਢ, ਪੂਰਨ ਸੂਚਕਾਂਕ_2,
			   uint64_t *addr, uint64_t *ਮਾਪ)
{
	ਪੂਰਨ ਮਾਪਾ, len, i, rc;
	ਪੂਰਨ cell_addr, cell_ਮਾਪ;
	ਅਚਲ fdt32_t *prop_addr, *prop_ਮਾਪ;
	uint64_t ਆਰਜ਼ੀ = 0;

	ਜੇ (!fdt || ਗੰਢ < 0 || ਸੂਚਕਾਂਕ_2 < 0)
		ਵਾਪਸ SBI_EINVAL;

	ਮਾਪਾ = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, ਗੰਢ);
	ਜੇ (ਮਾਪਾ < 0)
		ਵਾਪਸ ਮਾਪਾ;
	cell_addr = fdt_ਪਤਾ_cells(fdt, ਮਾਪਾ);
	ਜੇ (cell_addr < 1)
		ਵਾਪਸ SBI_ENODEV;

	cell_ਮਾਪ = fdt_ਮਾਪ_cells(fdt, ਮਾਪਾ);
	ਜੇ (cell_ਮਾਪ < 0)
		ਵਾਪਸ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, ਗੰਢ, "reg", &len);
	ਜੇ (!prop_addr)
		ਵਾਪਸ SBI_ENODEV;

	ਜੇ ((len / ਆਕਾਰ(u32)) <= (ਸੂਚਕਾਂਕ_2 * (cell_addr + cell_ਮਾਪ)))
		ਵਾਪਸ SBI_EINVAL;

	prop_addr = prop_addr + (ਸੂਚਕਾਂਕ_2 * (cell_addr + cell_ਮਾਪ));
	prop_ਮਾਪ = prop_addr + cell_addr;

	ਜੇ (addr) {
		ਲਈ (i = 0; i < cell_addr; i++)
			ਆਰਜ਼ੀ = (ਆਰਜ਼ੀ << 32) | fdt32_ਮੰਜ਼ਿਲ_cpu(*prop_addr++);
		ਕਰੋ {
			ਜੇ (ਮਾਪਾ < 0)
				ਰੋਕੋ;
			rc  = fdt_translate_ਪਤਾ(fdt, ਆਰਜ਼ੀ, ਮਾਪਾ, addr);
			ਜੇ (rc)
				ਰੋਕੋ;
			ਮਾਪਾ = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, ਮਾਪਾ);
			ਆਰਜ਼ੀ = *addr;
		} ਜਦੋਂ (1);
	}
	ਆਰਜ਼ੀ = 0;

	ਜੇ (ਮਾਪ) {
		ਲਈ (i = 0; i < cell_ਮਾਪ; i++)
			ਆਰਜ਼ੀ = (ਆਰਜ਼ੀ << 32) | fdt32_ਮੰਜ਼ਿਲ_cpu(*prop_ਮਾਪ++);
		*ਮਾਪ = ਆਰਜ਼ੀ;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_get_ਗੰਢ_addr_ਮਾਪ_by_name(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਗੰਢ, ਅਚਲ ਅੱਖਰ *name,
				   uint64_t *addr, uint64_t *ਮਾਪ)
{
	ਪੂਰਨ i, j, ਗਿਣਤੀ_2;
	ਅਚਲ ਅੱਖਰ *val;
	ਅਚਲ ਅੱਖਰ *regname;

	ਜੇ (!fdt || ਗੰਢ < 0 || !name)
		ਵਾਪਸ SBI_EINVAL;

	val = fdt_getprop(fdt, ਗੰਢ, "reg-names", &ਗਿਣਤੀ_2);
	ਜੇ (!val)
		ਵਾਪਸ SBI_ENODEV;

	ਲਈ (i = 0, j = 0; i < ਗਿਣਤੀ_2; i++, j++) {
		regname = val + i;

		ਜੇ (strcmp(name, regname) == 0)
			ਵਾਪਸ fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, ਗੰਢ, j, addr, ਮਾਪ);

		i += ਲੜੀ_ਲੰਬਾਈ(regname);
	}

	ਵਾਪਸ SBI_ENODEV;
}

bool fdt_ਗੰਢ_is_enabled(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff)
{
	ਪੂਰਨ len;
	ਅਚਲ ਖਾਲੀ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ਜੇ (!prop)
		ਵਾਪਸ true;

	ਜੇ (!ਲੜੀ_ਤੁਲਨਾ(prop, "okay", ਲੜੀ_ਲੰਬਾਈ("okay")))
		ਵਾਪਸ true;

	ਜੇ (!ਲੜੀ_ਤੁਲਨਾ(prop, "ok", ਲੜੀ_ਲੰਬਾਈ("ok")))
		ਵਾਪਸ true;

	ਵਾਪਸ false;
}

ਪੂਰਨ fdt_parse_hart_id(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ cpu_ਵਿੱਥ, u32 *hartid)
{
	ਪੂਰਨ len;
	ਅਚਲ ਖਾਲੀ *prop;
	ਅਚਲ fdt32_t *val;

	ਜੇ (!fdt || cpu_ਵਿੱਥ < 0)
		ਵਾਪਸ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_ਵਿੱਥ, "device_type", &len);
	ਜੇ (!prop || !len)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ (prop, "cpu", ਲੜੀ_ਲੰਬਾਈ ("cpu")))
		ਵਾਪਸ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_ਵਿੱਥ, "reg", &len);
	ਜੇ (!val || len < ਆਕਾਰ(fdt32_t))
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (len > ਆਕਾਰ(fdt32_t))
		val++;

	ਜੇ (hartid)
		*hartid = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_cbom_ਖੰਡ_ਮਾਪ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ cpu_ਵਿੱਥ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *cbom_ਖੰਡ_ਮਾਪ)
{
	ਪੂਰਨ len;
	ਅਚਲ ਖਾਲੀ *prop;
	ਅਚਲ fdt32_t *val;

	ਜੇ (!fdt || cpu_ਵਿੱਥ < 0)
		ਵਾਪਸ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_ਵਿੱਥ, "device_type", &len);
	ਜੇ (!prop || !len)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ (prop, "cpu", ਲੜੀ_ਲੰਬਾਈ ("cpu")))
		ਵਾਪਸ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_ਵਿੱਥ, "riscv,cbom-block-size", &len);
	ਜੇ (!val || len < ਆਕਾਰ(fdt32_t))
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (cbom_ਖੰਡ_ਮਾਪ)
		*cbom_ਖੰਡ_ਮਾਪ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_max_enabled_hart_id(ਅਚਲ ਖਾਲੀ *fdt, u32 *max_hartid)
{
	u32 hartid;
	ਪੂਰਨ err, cpu_ਵਿੱਥ, cpus_ਵਿੱਥ;

	ਜੇ (!fdt)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!max_hartid)
		ਵਾਪਸ 0;

	*max_hartid = 0;

	cpus_ਵਿੱਥ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/cpus");
	ਜੇ (cpus_ਵਿੱਥ < 0)
		ਵਾਪਸ cpus_ਵਿੱਥ;

	fdt_for_each_subnode(cpu_ਵਿੱਥ, fdt, cpus_ਵਿੱਥ) {
		err = fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &hartid);
		ਜੇ (err)
			ਜਾਰੀ;

		ਜੇ (!fdt_ਗੰਢ_is_enabled(fdt, cpu_ਵਿੱਥ))
			ਜਾਰੀ;

		ਜੇ (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_timebase_frequency(ਅਚਲ ਖਾਲੀ *fdt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *freq)
{
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ len, cpus_ਵਿੱਥ;

	ਜੇ (!fdt || !freq)
		ਵਾਪਸ SBI_EINVAL;

	cpus_ਵਿੱਥ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/cpus");
	ਜੇ (cpus_ਵਿੱਥ < 0)
		ਵਾਪਸ cpus_ਵਿੱਥ;

	val = fdt_getprop(fdt, cpus_ਵਿੱਥ, "timebase-frequency", &len);
	ਜੇ (len > 0 && val)
		*freq = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_ENOENT;

	ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_ISA_EXT_NAME_LEN_MAX	32

ਸਥਿਰ ਪੂਰਨ fdt_parse_isa_one_hart(ਅਚਲ ਅੱਖਰ *isa, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *extensions)
{
	size_t i, j, isa_len;
	ਅੱਖਰ mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = ਲੜੀ_ਲੰਬਾਈ(isa);

	ਜੇ (isa[i] == 'r' || isa[i] == 'R')
		i++;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (isa[i] == 'v' || isa[i] == 'V')
		i++;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	ਨਹੀਂਤਾਂ ਜੇ (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EINVAL;

	/* Skip base ISA extensions */
	ਲਈ (; i < isa_len; i++) {
		ਜੇ (isa[i] == '_')
			ਰੋਕੋ;
	}

	ਜਦੋਂ (i < isa_len) {
		ਜੇ (isa[i] != '_') {
			i++;
			ਜਾਰੀ;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ਜਦੋਂ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (ਆਕਾਰ(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ਜੇ (!j)
			ਜਾਰੀ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_multi_letter_ext(name, ਦੋਆਧਾਰੀ_ਅੰਕ_2)				\
			ਜੇ (!strcmp(mstr, name)) {		\
				__set_bit(ਦੋਆਧਾਰੀ_ਅੰਕ_2, extensions);	\
				ਜਾਰੀ;			\
			}

		ਲਈ (j = 0; j < SBI_HART_EXT_MAX; j++) {
			ਸੈੱਟ_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਸੈੱਟ_multi_letter_ext
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ fdt_parse_isa_extensions_one_hart(ਅਚਲ ਅੱਖਰ *isa,
					      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *extensions,
					      ਪੂਰਨ len)
{
	size_t i;

	ਲਈ (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ਜੇ (fdt_stringlist_ਸ਼ਾਮਲ_ਹੈ(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

ਪੂਰਨ fdt_parse_isa_extensions_all_harts(ਅਚਲ ਖਾਲੀ *fdt)
{
	u32 hartid;
	ਅਚਲ fdt32_t *val;
	ਬਣਤਰ sbi_scratch *scratch;
	ਬਣਤਰ sbi_hart_features *hfeatures;
	ਪੂਰਨ err, cpu_ਵਿੱਥ, cpus_ਵਿੱਥ, len;

	ਜੇ (!fdt)
		ਵਾਪਸ SBI_EINVAL;

	cpus_ਵਿੱਥ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/cpus");
	ਜੇ (cpus_ਵਿੱਥ < 0)
		ਵਾਪਸ cpus_ਵਿੱਥ;

	fdt_for_each_subnode(cpu_ਵਿੱਥ, fdt, cpus_ਵਿੱਥ) {
		err = fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &hartid);
		ਜੇ (err)
			ਜਾਰੀ;

		ਜੇ (!fdt_ਗੰਢ_is_enabled(fdt, cpu_ਵਿੱਥ))
			ਜਾਰੀ;

		scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
		ਜੇ (!scratch)
			ਵਾਪਸ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ਜੇ (!hfeatures)
			ਵਾਪਸ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_ਵਿੱਥ, "riscv,isa-extensions", &len);
		ਜੇ (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ਅਚਲ ਅੱਖਰ *)val,
							  hfeatures->extensions, len);
			ਜਾਰੀ;
		}

		val = fdt_getprop(fdt, cpu_ਵਿੱਥ, "riscv,isa", &len);
		ਜੇ (!val || len <= 0)
			ਵਾਪਸ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ਅਚਲ ਅੱਖਰ *)val, hfeatures->extensions);
		ਜੇ (err)
			ਵਾਪਸ err;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_parse_uart_ਗੰਢ_common(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
				      ਬਣਤਰ platform_uart_ਅੰਕੜੇ *uart,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ default_freq,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ default_baud)
{
	ਪੂਰਨ len, rc;
	ਅਚਲ fdt32_t *val;
	uint64_t reg_addr, reg_ਮਾਪ;

	ਜੇ (nodeoffset < 0 || !uart || !fdt)
		ਵਾਪਸ SBI_ENODEV;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_ਮਾਪ)
		ਵਾਪਸ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ਜੇ (len > 0 && val)
		uart->freq = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ਜੇ (len > 0 && val)
		uart->baud = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		uart->baud = default_baud;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_gaisler_uart_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
				ਬਣਤਰ platform_uart_ਅੰਕੜੇ *uart)
{
	ਵਾਪਸ fdt_parse_uart_ਗੰਢ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

ਪੂਰਨ fdt_parse_renesas_scif_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
				ਬਣਤਰ platform_uart_ਅੰਕੜੇ *uart)
{
	ਵਾਪਸ fdt_parse_uart_ਗੰਢ_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

ਪੂਰਨ fdt_parse_shakti_uart_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
			       ਬਣਤਰ platform_uart_ਅੰਕੜੇ *uart)
{
	ਵਾਪਸ fdt_parse_uart_ਗੰਢ_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

ਪੂਰਨ fdt_parse_sifive_uart_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
			       ਬਣਤਰ platform_uart_ਅੰਕੜੇ *uart)
{
	ਵਾਪਸ fdt_parse_uart_ਗੰਢ_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

ਪੂਰਨ fdt_parse_uart_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
			ਬਣਤਰ platform_uart_ਅੰਕੜੇ *uart)
{
	ਪੂਰਨ len, rc;
	ਅਚਲ fdt32_t *val;

	rc = fdt_parse_uart_ਗੰਢ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ਜੇ (len > 0 && val)
		uart->reg_ਖਿਸਕਾਓ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		uart->reg_ਖਿਸਕਾਓ = DEFAULT_UART_REG_ਖਿਸਕਾਓ;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ਜੇ (len > 0 && val)
		uart->reg_io_ਚੌੜਾਈ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		uart->reg_io_ਚੌੜਾਈ = DEFAULT_UART_REG_IO_ਚੌੜਾਈ;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ਜੇ (len > 0 && val)
		uart->reg_ਵਿੱਥ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		uart->reg_ਵਿੱਥ = DEFAULT_UART_REG_ਵਿੱਥ;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_uart8250(ਅਚਲ ਖਾਲੀ *fdt, ਬਣਤਰ platform_uart_ਅੰਕੜੇ *uart,
		   ਅਚਲ ਅੱਖਰ *compatible)
{
	ਪੂਰਨ nodeoffset;

	ਜੇ (!compatible || !uart || !fdt)
		ਵਾਪਸ SBI_ENODEV;

	nodeoffset = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, -1, compatible);
	ਜੇ (nodeoffset < 0)
		ਵਾਪਸ nodeoffset;

	ਵਾਪਸ fdt_parse_uart_ਗੰਢ(fdt, nodeoffset, uart);
}

ਪੂਰਨ fdt_parse_xlnx_uartlite_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
			       ਬਣਤਰ platform_uart_ਅੰਕੜੇ *uart)
{
	ਵਾਪਸ fdt_parse_uart_ਗੰਢ_common(fdt, nodeoffset, uart, 0, 0);
}

ਸਥਿਰ ਪੂਰਨ fdt_aplic_ਲੱਭੋ_imsic_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				     ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic, bool mmode)
{
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ਜੇ (val && len >= ਆਕਾਰ(fdt32_t)) {
		noff = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, fdt32_ਮੰਜ਼ਿਲ_cpu(*val));
		ਜੇ (noff < 0)
			ਵਾਪਸ noff;

		rc = fdt_parse_imsic_ਗੰਢ(fdt, noff, imsic);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		rc = imsic_ਅੰਕੜੇ_check(imsic);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		ਜੇ (imsic->targets_mmode == mmode) {
			ਵਾਪਸ 0;
		}
	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ਜੇ (!val || len < ਆਕਾਰ(fdt32_t))
		ਵਾਪਸ SBI_ENODEV;

	len /= ਆਕਾਰ(fdt32_t);

	ਲਈ (i = 0; i < len; i++) {
		noff = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, fdt32_ਮੰਜ਼ਿਲ_cpu(val[i]));
		ਜੇ (noff < 0)
			ਵਾਪਸ noff;

		rc = fdt_aplic_ਲੱਭੋ_imsic_ਗੰਢ(fdt, noff, imsic, mmode);
		ਜੇ (!rc)
			ਰੋਕੋ;
	}

	ਵਾਪਸ rc;
}

ਪੂਰਨ fdt_parse_aplic_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic)
{
	bool ਬੱਚਾ_ਮਿਲਿਆ;
	ਅਚਲ fdt32_t *val;
	ਅਚਲ fdt32_t *del;
	ਬਣਤਰ imsic_ਅੰਕੜੇ imsic = { 0 };
	ਪੂਰਨ i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_ਮਾਪ;
	ਬਣਤਰ aplic_delegate_ਅੰਕੜੇ *deleg;

	ਜੇ (nodeoff < 0 || !aplic || !fdt)
		ਵਾਪਸ SBI_ENODEV;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_ਮਾਪ)
		ਵਾਪਸ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->ਮਾਪ = reg_ਮਾਪ;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ਜੇ (len > 0)
		aplic->ਅੰਕ_ਸਰੋਤ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ਜੇ (val && len > ਆਕਾਰ(fdt32_t)) {
		len = len / ਆਕਾਰ(fdt32_t);
		ਲਈ (i = 0; i < len; i += 2) {
			ਜੇ (fdt32_ਮੰਜ਼ਿਲ_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				ਰੋਕੋ;
			}
		}
		aplic->ਅੰਕ_idc = len / 2;
	}

	rc = fdt_aplic_ਲੱਭੋ_imsic_ਗੰਢ(fdt, nodeoff, &imsic, true);
	ਜੇ (!rc) {
		aplic->ਮਾਪਾ_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
		aplic->msicfg_mmode.lhxw = imsic.hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
		aplic->msicfg_mmode.hhxw = imsic.ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
		aplic->msicfg_mmode.hhxs = imsic.ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
		ਜੇ (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ))
			ਵਾਪਸ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.ਅਧਾਰ_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_ਲੱਭੋ_imsic_ਗੰਢ(fdt, nodeoff, &imsic, false);
	ਜੇ (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
		aplic->msicfg_smode.lhxw = imsic.hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
		aplic->msicfg_smode.hhxw = imsic.ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
		aplic->msicfg_smode.hhxs = imsic.ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
		ਜੇ (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ))
			ਵਾਪਸ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.ਅਧਾਰ_addr = imsic.regs[0].addr;
	}

	ਲਈ (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->ਪਹਿਲਾ_irq = 0;
		deleg->ਆਖਰੀ_irq = 0;
		deleg->ਬੱਚਾ_ਸੂਚਕਾਂਕ = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ਜੇ (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ਜੇ (!del || len < (3 * ਆਕਾਰ(fdt32_t)))
		ਜਾਓ skip_delegate_parse;
	d = 0;
	dcnt = len / ਆਕਾਰ(fdt32_t);
	ਲਈ (i = 0; i < dcnt; i += 3) {
		ਜੇ (d >= APLIC_MAX_DELEGATE)
			ਰੋਕੋ;
		deleg = &aplic->delegate[d];

		deleg->ਪਹਿਲਾ_irq = fdt32_ਮੰਜ਼ਿਲ_cpu(del[i + 1]);
		deleg->ਆਖਰੀ_irq = fdt32_ਮੰਜ਼ਿਲ_cpu(del[i + 2]);
		deleg->ਬੱਚਾ_ਸੂਚਕਾਂਕ = 0;

		ਬੱਚਾ_ਮਿਲਿਆ = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ਜੇ (!val || len < ਆਕਾਰ(fdt32_t)) {
			deleg->ਪਹਿਲਾ_irq = 0;
			deleg->ਆਖਰੀ_irq = 0;
			deleg->ਬੱਚਾ_ਸੂਚਕਾਂਕ = 0;
			ਜਾਰੀ;
		}
		len = len / ਆਕਾਰ(fdt32_t);
		ਲਈ (j = 0; j < len; j++) {
			ਜੇ (del[i] != val[j])
				ਜਾਰੀ;
			deleg->ਬੱਚਾ_ਸੂਚਕਾਂਕ = j;
			ਬੱਚਾ_ਮਿਲਿਆ = true;
			ਰੋਕੋ;
		}

		ਜੇ (ਬੱਚਾ_ਮਿਲਿਆ) {
			d++;
		} ਨਹੀਂਤਾਂ {
			deleg->ਪਹਿਲਾ_irq = 0;
			deleg->ਆਖਰੀ_irq = 0;
			deleg->ਬੱਚਾ_ਸੂਚਕਾਂਕ = 0;
		}
	}
skip_delegate_parse:

	ਵਾਪਸ 0;
}

bool fdt_check_imsic_mlevel(ਅਚਲ ਖਾਲੀ *fdt)
{
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ i, len, noff = 0;

	ਜੇ (!fdt)
		ਵਾਪਸ false;

	ਜਦੋਂ ((noff = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ਜੇ (val && len > ਆਕਾਰ(fdt32_t)) {
			len = len / ਆਕਾਰ(fdt32_t);
			ਲਈ (i = 0; i < len; i += 2) {
				ਜੇ (fdt32_ਮੰਜ਼ਿਲ_cpu(val[i + 1]) == IRQ_M_EXT)
					ਵਾਪਸ true;
			}
		}
	}

	ਵਾਪਸ false;
}

ਪੂਰਨ fdt_parse_imsic_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic)
{
	ਅਚਲ fdt32_t *val;
	ਬਣਤਰ imsic_regs *regs;
	uint64_t reg_addr, reg_ਮਾਪ;
	ਪੂਰਨ i, rc, len, nr_ਮਾਪਾ_irqs;

	ਜੇ (nodeoff < 0 || !imsic || !fdt)
		ਵਾਪਸ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ਜੇ (val && len > ਆਕਾਰ(fdt32_t)) {
		len = len / ਆਕਾਰ(fdt32_t);
		nr_ਮਾਪਾ_irqs = len / 2;
		ਲਈ (i = 0; i < len; i += 2) {
			ਜੇ (fdt32_ਮੰਜ਼ਿਲ_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				ਰੋਕੋ;
			}
		}
	} ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ਜੇ (val && len > 0)
		imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ਜੇ (val && len > 0) {
		imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	} ਨਹੀਂਤਾਂ {
		imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ = sbi_fls(nr_ਮਾਪਾ_irqs);
		ਜੇ ((1UL << imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ) < nr_ਮਾਪਾ_irqs)
			imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ਜੇ (val && len > 0)
		imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ਜੇ (val && len > 0)
		imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ = 2 * IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ਜੇ (val && len > 0)
		imsic->ਅੰਕ_ids = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EINVAL;

	ਲਈ (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->ਮਾਪ = 0;
	}

	ਲਈ (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, i,
					    &reg_addr, &reg_ਮਾਪ);
		ਜੇ (rc < 0 || !reg_ਮਾਪ)
			ਰੋਕੋ;
		regs->addr = reg_addr;
		regs->ਮਾਪ = reg_ਮਾਪ;
	}
	ਜੇ (!imsic->regs[0].ਮਾਪ)
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_plic_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਬਣਤਰ plic_ਅੰਕੜੇ *plic)
{
	ਪੂਰਨ len, rc;
	ਅਚਲ fdt32_t *val;
	uint64_t reg_addr, reg_ਮਾਪ;

	ਜੇ (nodeoffset < 0 || !plic || !fdt)
		ਵਾਪਸ SBI_ENODEV;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_ਮਾਪ)
		ਵਾਪਸ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->ਮਾਪ = reg_ਮਾਪ;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ਜੇ (len > 0)
		plic->ਅੰਕ_src = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_plic(ਅਚਲ ਖਾਲੀ *fdt, ਬਣਤਰ plic_ਅੰਕੜੇ *plic, ਅਚਲ ਅੱਖਰ *compat)
{
	ਪੂਰਨ nodeoffset;

	ਜੇ (!compat || !plic || !fdt)
		ਵਾਪਸ SBI_ENODEV;

	nodeoffset = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, -1, compat);
	ਜੇ (nodeoffset < 0)
		ਵਾਪਸ nodeoffset;

	ਵਾਪਸ fdt_parse_plic_ਗੰਢ(fdt, nodeoffset, plic);
}

ਸਥਿਰ ਪੂਰਨ fdt_get_aclint_addr_ਮਾਪ_by_name(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
					    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_addr1,
					    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_size1,
					    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_addr2,
					    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_size2)
{
	ਪੂਰਨ rc;
	uint64_t reg_addr, reg_ਮਾਪ;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_ਮਾਪ)
		reg_addr = reg_ਮਾਪ = 0;
	*ਬਾਹਰਲਾ_addr1 = reg_addr;
	*ਬਾਹਰਲਾ_size1 = reg_ਮਾਪ;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_ਮਾਪ)
		ਵਾਪਸ SBI_ENODEV;
	*ਬਾਹਰਲਾ_addr2 = reg_addr;
	*ਬਾਹਰਲਾ_size2 = reg_ਮਾਪ;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_get_aclint_addr_ਮਾਪ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_addr1,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_size1,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_addr2,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_size2)
{
	ਪੂਰਨ rc;
	uint64_t reg_addr, reg_ਮਾਪ;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_ਮਾਪ)
		ਵਾਪਸ SBI_ENODEV;
	*ਬਾਹਰਲਾ_addr1 = reg_addr;
	*ਬਾਹਰਲਾ_size1 = reg_ਮਾਪ;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoffset, 1,
				    &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_ਮਾਪ)
		reg_addr = reg_ਮਾਪ = 0;
	ਜੇ (ਬਾਹਰਲਾ_addr2)
		*ਬਾਹਰਲਾ_addr2 = reg_addr;
	ਜੇ (ਬਾਹਰਲਾ_size2)
		*ਬਾਹਰਲਾ_size2 = reg_ਮਾਪ;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_aclint_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset,
			  bool for_timer, bool allow_regname,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_addr1, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_size1,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_addr2, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_size2,
			  u32 *ਬਾਹਰਲਾ_ਪਹਿਲਾ_hartid, u32 *ਬਾਹਰਲਾ_hart_ਗਿਣਤੀ)
{
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ i, rc, ਗਿਣਤੀ_2, cpu_ਵਿੱਥ, cpu_intc_ਵਿੱਥ;
	u32 phandle, hwirq, hartid, ਪਹਿਲਾ_hartid, ਆਖਰੀ_hartid;
	u32 ਮੇਲ_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ਜੇ (nodeoffset < 0 || !fdt ||
	    !ਬਾਹਰਲਾ_addr1 || !ਬਾਹਰਲਾ_size1 ||
	    !ਬਾਹਰਲਾ_ਪਹਿਲਾ_hartid || !ਬਾਹਰਲਾ_hart_ਗਿਣਤੀ)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (for_timer && allow_regname && ਬਾਹਰਲਾ_addr2 && ਬਾਹਰਲਾ_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", ਨਿਰਾ))
		rc = fdt_get_aclint_addr_ਮਾਪ_by_name(fdt, nodeoffset,
						      ਬਾਹਰਲਾ_addr1, ਬਾਹਰਲਾ_size1,
						      ਬਾਹਰਲਾ_addr2, ਬਾਹਰਲਾ_size2);
	ਨਹੀਂਤਾਂ
		rc = fdt_get_aclint_addr_ਮਾਪ(fdt, nodeoffset,
					      ਬਾਹਰਲਾ_addr1, ਬਾਹਰਲਾ_size1,
					      ਬਾਹਰਲਾ_addr2, ਬਾਹਰਲਾ_size2);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	*ਬਾਹਰਲਾ_ਪਹਿਲਾ_hartid = 0;
	*ਬਾਹਰਲਾ_hart_ਗਿਣਤੀ = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ਗਿਣਤੀ_2);
	ਜੇ (!val || ਗਿਣਤੀ_2 < ਆਕਾਰ(fdt32_t))
		ਵਾਪਸ 0;
	ਗਿਣਤੀ_2 = ਗਿਣਤੀ_2 / ਆਕਾਰ(fdt32_t);

	ਪਹਿਲਾ_hartid = -1U;
	ਆਖਰੀ_hartid = 0;
	ਲਈ (i = 0; i < (ਗਿਣਤੀ_2 / 2); i++) {
		phandle = fdt32_ਮੰਜ਼ਿਲ_cpu(val[2 * i]);
		hwirq = fdt32_ਮੰਜ਼ਿਲ_cpu(val[(2 * i) + 1]);

		cpu_intc_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, phandle);
		ਜੇ (cpu_intc_ਵਿੱਥ < 0)
			ਜਾਰੀ;

		cpu_ਵਿੱਥ = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, cpu_intc_ਵਿੱਥ);
		ਜੇ (cpu_ਵਿੱਥ < 0)
			ਜਾਰੀ;

		rc = fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &hartid);
		ਜੇ (rc)
			ਜਾਰੀ;

		ਜੇ (ਮੇਲ_hwirq == hwirq) {
			ਜੇ (hartid < ਪਹਿਲਾ_hartid)
				ਪਹਿਲਾ_hartid = hartid;
			ਜੇ (hartid > ਆਖਰੀ_hartid)
				ਆਖਰੀ_hartid = hartid;
		}
	}

	ਜੇ ((ਆਖਰੀ_hartid >= ਪਹਿਲਾ_hartid) && ਪਹਿਲਾ_hartid != -1U) {
		*ਬਾਹਰਲਾ_ਪਹਿਲਾ_hartid = ਪਹਿਲਾ_hartid;
		*ਬਾਹਰਲਾ_hart_ਗਿਣਤੀ = ਆਖਰੀ_hartid - ਪਹਿਲਾ_hartid + 1;
	}
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_plmt_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *plmt_ਅਧਾਰ,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *plmt_ਮਾਪ, u32 *hart_ਗਿਣਤੀ)
{
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ rc, i, ਗਿਣਤੀ_2;
	uint64_t reg_addr, reg_ਮਾਪ;
	u32 phandle, hwirq, hartid, hcount;

	ਜੇ (nodeoffset < 0 || !fdt || !plmt_ਅਧਾਰ ||
	    !hart_ਗਿਣਤੀ || !plmt_ਮਾਪ)
		ਵਾਪਸ SBI_EINVAL;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0)
		ਵਾਪਸ SBI_ENODEV;
	*plmt_ਅਧਾਰ = reg_addr;
	*plmt_ਮਾਪ = reg_ਮਾਪ;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ਗਿਣਤੀ_2);
	ਜੇ (!val || ਗਿਣਤੀ_2 < ਆਕਾਰ(fdt32_t))
		ਵਾਪਸ 0;
	ਗਿਣਤੀ_2 = ਗਿਣਤੀ_2 / ਆਕਾਰ(fdt32_t);

	hcount = 0;
	ਲਈ (i = 0; i < (ਗਿਣਤੀ_2 / 2); i++) {
		ਪੂਰਨ cpu_ਵਿੱਥ, cpu_intc_ਵਿੱਥ;

		phandle = fdt32_ਮੰਜ਼ਿਲ_cpu(val[2 * i]);
		hwirq = fdt32_ਮੰਜ਼ਿਲ_cpu(val[2 * i + 1]);

		cpu_intc_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, phandle);
		ਜੇ (cpu_intc_ਵਿੱਥ < 0)
			ਜਾਰੀ;

		cpu_ਵਿੱਥ = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, cpu_intc_ਵਿੱਥ);
		ਜੇ (cpu_ਵਿੱਥ < 0)
			ਜਾਰੀ;

		rc = fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &hartid);

		ਜੇ (rc)
			ਜਾਰੀ;

		ਜੇ (SBI_HARTMASK_MAX_ਬਿੱਟ <= sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid))
			ਜਾਰੀ;

		ਜੇ (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_ਗਿਣਤੀ = hcount;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_plicsw_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *plicsw_ਅਧਾਰ,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮਾਪ, u32 *hart_ਗਿਣਤੀ)
{
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ rc, i, ਗਿਣਤੀ_2;
	uint64_t reg_addr, reg_ਮਾਪ;
	u32 phandle, hwirq, hartid, hcount;

	ਜੇ (nodeoffset < 0 || !fdt || !plicsw_ਅਧਾਰ ||
	    !hart_ਗਿਣਤੀ || !ਮਾਪ)
		ਵਾਪਸ SBI_EINVAL;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0)
		ਵਾਪਸ SBI_ENODEV;
	*plicsw_ਅਧਾਰ = reg_addr;
	*ਮਾਪ = reg_ਮਾਪ;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ਗਿਣਤੀ_2);
	ਜੇ (!val || ਗਿਣਤੀ_2 < ਆਕਾਰ(fdt32_t))
		ਵਾਪਸ 0;
	ਗਿਣਤੀ_2 = ਗਿਣਤੀ_2 / ਆਕਾਰ(fdt32_t);

	hcount = 0;
	ਲਈ (i = 0; i < (ਗਿਣਤੀ_2 / 2); i++) {
		ਪੂਰਨ cpu_ਵਿੱਥ, cpu_intc_ਵਿੱਥ;

		phandle = fdt32_ਮੰਜ਼ਿਲ_cpu(val[2 * i]);
		hwirq = fdt32_ਮੰਜ਼ਿਲ_cpu(val[2 * i + 1]);

		cpu_intc_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, phandle);
		ਜੇ (cpu_intc_ਵਿੱਥ < 0)
			ਜਾਰੀ;

		cpu_ਵਿੱਥ = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, cpu_intc_ਵਿੱਥ);
		ਜੇ (cpu_ਵਿੱਥ < 0)
			ਜਾਰੀ;

		rc = fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &hartid);

		ਜੇ (rc)
			ਜਾਰੀ;

		ਜੇ (SBI_HARTMASK_MAX_ਬਿੱਟ <= sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid))
			ਜਾਰੀ;

		ਜੇ (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_ਗਿਣਤੀ = hcount;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_parse_compat_addr(ਅਚਲ ਖਾਲੀ *fdt, uint64_t *addr,
			  ਅਚਲ ਅੱਖਰ *compatible)
{
	ਪੂਰਨ nodeoffset, rc;

	nodeoffset = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, -1, compatible);
	ਜੇ (nodeoffset < 0)
		ਵਾਪਸ nodeoffset;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoffset, 0, addr, ਨਿਰਾ);
	ਜੇ (rc < 0 || !addr)
		ਵਾਪਸ SBI_ENODEV;

	ਵਾਪਸ 0;
}
