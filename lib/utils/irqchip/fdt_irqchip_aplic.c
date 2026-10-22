/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/fdt_irqchip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/aplic.h>

ਸਥਿਰ ਪੂਰਨ irqchip_aplic_update_idc_map(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
					ਬਣਤਰ aplic_ਅੰਕੜੇ *pd)
{
	ਪੂਰਨ i, err, ਗਿਣਤੀ_2, cpu_ਵਿੱਥ, cpu_intc_ਵਿੱਥ;
	u32 phandle, hartid, hartindex;
	ਅਚਲ fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ਗਿਣਤੀ_2);
	ਜੇ (!val || ਗਿਣਤੀ_2 < ਆਕਾਰ(fdt32_t))
		ਵਾਪਸ SBI_EINVAL;
	ਗਿਣਤੀ_2 = ਗਿਣਤੀ_2 / ਆਕਾਰ(fdt32_t);

	ਲਈ (i = 0; i < ਗਿਣਤੀ_2; i += 2) {
		phandle = fdt32_ਮੰਜ਼ਿਲ_cpu(val[i]);

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

		pd->idc_map[i / 2] = hartindex;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ irqchip_aplic_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				   ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ aplic_ਅੰਕੜੇ *pd;

	pd = sbi_zalloc(ਆਕਾਰ(*pd));
	ਜੇ (!pd)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_parse_aplic_ਗੰਢ(fdt, nodeoff, pd);
	ਜੇ (rc)
		ਜਾਓ fail_free_ਅੰਕੜੇ;

	ਜੇ (pd->ਅੰਕ_idc) {
		pd->idc_map = sbi_zalloc(ਆਕਾਰ(*pd->idc_map) * pd->ਅੰਕ_idc);
		ਜੇ (!pd->idc_map) {
			rc = SBI_ENOMEM;
			ਜਾਓ fail_free_ਅੰਕੜੇ;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		ਜੇ (rc)
			ਜਾਓ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	ਜੇ (rc)
		ਜਾਓ fail_free_idc_map;

	ਵਾਪਸ 0;

fail_free_idc_map:
	ਜੇ (pd->ਅੰਕ_idc)
		sbi_free(pd->idc_map);
fail_free_ਅੰਕੜੇ:
	sbi_free(pd);
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ irqchip_aplic_ਮੇਲ[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_irqchip_aplic = {
	.ਮੇਲ_ਸਾਰਣੀ = irqchip_aplic_ਮੇਲ,
	.init = irqchip_aplic_cold_init,
};
