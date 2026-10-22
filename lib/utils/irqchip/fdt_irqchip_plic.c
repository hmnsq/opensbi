/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/irqchip/fdt_irqchip.h>
#ضمن <sbi_utils/irqchip/plic.h>

ساكن صحيح irqchip_plic_update_context_map(ثابت فراغ *fdt, صحيح nodeoff,
					   بنية plic_معلومات *pd)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	صحيح i, err, شمېر, cpu_دننه_واټن, cpu_intc_دننه_واټن;

	لكل (i = 0; i < sbi_hart_شمېر(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &شمېر);
	إذا (!val || شمېر < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	شمېر = شمېر / حجم(fdt32_t);

	لكل (i = 0; i < شمېر; i += 2) {
		phandle = fdt32_منزل_cpu(val[i]);
		hwirq = fdt32_منزل_cpu(val[i + 1]);

		cpu_intc_دننه_واټن = fdt_غوټه_دننه_واټن_by_phandle(fdt, phandle);
		إذا (cpu_intc_دننه_واټن < 0)
			استمر;

		cpu_دننه_واټن = fdt_پلار_دننه_واټن(fdt, cpu_intc_دننه_واټن);
		إذا (cpu_دننه_واټن < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);
		إذا (err)
			استمر;

		hartindex = sbi_hartid_منزل_hartindex(hartid);
		إذا (hartindex == -1U)
			استمر;

		بدل (hwirq) {
		حالة IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			اكسر;
		حالة IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			اكسر;
		}
	}

	أرجع 0;
}

ساكن صحيح irqchip_plic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				  ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بنية plic_معلومات *pd;

	pd = sbi_zalloc(PLIC_معلومات_اندازه(sbi_hart_شمېر()));
	إذا (!pd)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_plic_غوټه(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_معلومات;

	pd->flags = (بینښه اوږد)سمون->معلومات;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_معلومات;

	rc = plic_cold_irqchip_init(pd);
	إذا (rc)
		اذهب fail_free_معلومات;

	أرجع 0;

fail_free_معلومات:
	sbi_free(pd);
	أرجع rc;
}

ساكن ثابت بنية fdt_سمون irqchip_plic_سمون[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .معلومات = (فراغ *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_irqchip_plic = {
	.سمون_جدول = irqchip_plic_سمون,
	.init = irqchip_plic_cold_init,
};
