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
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/imsic.h>

ਸਥਿਰ ਪੂਰਨ irqchip_imsic_update_hartid_ਸਾਰਣੀ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
					     ਬਣਤਰ imsic_ਅੰਕੜੇ *id)
{
	ਅਚਲ fdt32_t *val;
	u32 phandle, hwirq, hartid;
	ਪੂਰਨ i, err, ਗਿਣਤੀ_2, cpu_ਵਿੱਥ, cpu_intc_ਵਿੱਥ;

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
			ਵਾਪਸ SBI_EINVAL;

		ਚੋਣ (hwirq) {
		ਹਾਲਤ IRQ_M_EXT:
			err = imsic_map_hartid_ਮੰਜ਼ਿਲ_ਅੰਕੜੇ(hartid, id, i / 2);
			ਜੇ (err)
				ਵਾਪਸ err;
			ਰੋਕੋ;
		ਮੂਲ:
			ਰੋਕੋ;
		}
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ irqchip_imsic_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				   ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ imsic_ਅੰਕੜੇ *id;

	id = sbi_zalloc(ਆਕਾਰ(*id));
	ਜੇ (!id)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_parse_imsic_ਗੰਢ(fdt, nodeoff, id);
	ਜੇ (rc || !id->targets_mmode)
		ਜਾਓ fail_free_ਅੰਕੜੇ;

	rc = imsic_cold_irqchip_init(id);
	ਜੇ (rc)
		ਜਾਓ fail_free_ਅੰਕੜੇ;

	rc = irqchip_imsic_update_hartid_ਸਾਰਣੀ(fdt, nodeoff, id);
	ਜੇ (rc)
		ਜਾਓ fail_free_ਅੰਕੜੇ;

	ਵਾਪਸ 0;

fail_free_ਅੰਕੜੇ:
	sbi_free(id);
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ irqchip_imsic_ਮੇਲ[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_irqchip_imsic = {
	.ਮੇਲ_ਸਾਰਣੀ = irqchip_imsic_ਮੇਲ,
	.init = irqchip_imsic_cold_init,
};
