/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/fdt_irqchip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/plic.h>

ਸਥਿਰ ਪੂਰਨ irqchip_plic_update_context_map(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
					   ਬਣਤਰ plic_ਅੰਕੜੇ *pd)
{
	ਅਚਲ fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	ਪੂਰਨ i, err, ਗਿਣਤੀ_2, cpu_ਵਿੱਥ, cpu_intc_ਵਿੱਥ;

	ਲਈ (i = 0; i < sbi_hart_ਗਿਣਤੀ(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ਗਿਣਤੀ_2);
	ਜੇ (!val || ਗਿਣਤੀ_2 < ਆਕਾਰ(fdt32_t))
		ਵਾਪਸ SBI_EINVAL;
	ਗਿਣਤੀ_2 = ਗਿਣਤੀ_2 / ਆਕਾਰ(fdt32_t);

	ਲਈ (i = 0; i < ਗਿਣਤੀ_2; i += 2) {
		phandle = fdt32_ਮੰਜ਼ਿਲ_cpu(val[i]);
		hwirq = fdt32_ਮੰਜ਼ਿਲ_cpu(val[i + 1]);

		cpu_intc_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, phandle);
		ਜੇ (cpu_intc_ਵਿੱਥ < 0)
			ਜਾਰੀ;

		cpu_ਵਿੱਥ = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, cpu_intc_ਵਿੱਥ);
		ਜੇ (cpu_ਵਿੱਥ < 0)
			ਜਾਰੀ;

		err = fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &hartid);
		ਜੇ (err)
			ਜਾਰੀ;

		hartindex = sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid);
		ਜੇ (hartindex == -1U)
			ਜਾਰੀ;

		ਚੋਣ (hwirq) {
		ਹਾਲਤ IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			ਰੋਕੋ;
		ਹਾਲਤ IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			ਰੋਕੋ;
		}
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ irqchip_plic_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				  ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ plic_ਅੰਕੜੇ *pd;

	pd = sbi_zalloc(PLIC_ਅੰਕੜੇ_ਮਾਪ(sbi_hart_ਗਿਣਤੀ()));
	ਜੇ (!pd)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_parse_plic_ਗੰਢ(fdt, nodeoff, pd);
	ਜੇ (rc)
		ਜਾਓ fail_free_ਅੰਕੜੇ;

	pd->flags = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਮੇਲ->ਅੰਕੜੇ;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	ਜੇ (rc)
		ਜਾਓ fail_free_ਅੰਕੜੇ;

	rc = plic_cold_irqchip_init(pd);
	ਜੇ (rc)
		ਜਾਓ fail_free_ਅੰਕੜੇ;

	ਵਾਪਸ 0;

fail_free_ਅੰਕੜੇ:
	sbi_free(pd);
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ irqchip_plic_ਮੇਲ[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .ਅੰਕੜੇ = (ਖਾਲੀ *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_irqchip_plic = {
	.ਮੇਲ_ਸਾਰਣੀ = irqchip_plic_ਮੇਲ,
	.init = irqchip_plic_cold_init,
};
