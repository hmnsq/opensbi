/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_irqchip.h>
#ضمن <sbi/sbi_list.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
بنية sbi_irqchip_hwirq_داده {
	/** raw hardware interrupt handler */
	صحيح (*raw_handler)(بنية sbi_irqchip_device *chip, u32 hwirq);

#تعریف_کن IRQ_ENABLED	بیت(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_شاخص;

	/** chip's private data */
	فراغ *priv;
};

/** Internal irqchip interrupt handler */
بنية sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	بنية sbi_dlist گره;

	/** First hardware IRQ handled by this handler */
	u32 اولی_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 رقم_hwirq;

	/** Write MSI function of this handler */
	فراغ (*write_msi)(u32 hwirq, ثابت بنية sbi_irqchip_msi_پیام *پیام, فراغ *priv);

	/** Callback function of this handler */
	صحيح (*callback)(u32 hwirq, فراغ *priv);

	/** Callback private data */
	فراغ *priv;
};

بنية sbi_irqchip_hart_داده {
	بنية sbi_irqchip_device *chip;
};

ساكن بیعلامت دراز irqchip_hart_داده_off;
ساكن SBI_LIST_سر(irqchip_list);

صحيح sbi_irqchip_process(فراغ)
{
	بنية sbi_irqchip_hart_داده *hd;

	hd = sbi_scratch_thishart_جابه_جایی_ptr(irqchip_hart_داده_off);
	إذا (!hd || !hd->chip || !hd->chip->process_hwirqs)
		أرجع SBI_ENODEV;

	أرجع hd->chip->process_hwirqs(hd->chip);
}

صحيح sbi_irqchip_process_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_داده *داده;

	إذا (!chip || chip->رقم_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	داده = &chip->hwirqs[hwirq];
	إذا (!داده->raw_handler)
		أرجع SBI_ENOENT;

	أرجع داده->raw_handler(chip, hwirq);
}

ساكن مضمن u32 sbi_irqchip_get_irq_state(بنية sbi_irqchip_device *chip,
					     u32 hwirq)
{
	إذا (!chip || !chip->hwirqs || hwirq >= chip->رقم_hwirq)
		أرجع 0;

	أرجع chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(بنية sbi_irqchip_device *chip,
				   u32 hwirq)
{
	أرجع !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

صحيح sbi_irqchip_unmask_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_داده *داده;

	إذا (!chip || chip->رقم_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	داده = &chip->hwirqs[hwirq];
	إذا (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	إذا (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	داده->irq_state |= IRQ_ENABLED;
	أرجع 0;
}

صحيح sbi_irqchip_ماسک_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_داده *داده;

	إذا (!chip || chip->رقم_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	إذا (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	إذا (chip->hwirq_ماسک)
		chip->hwirq_ماسک(chip, hwirq);

	داده = &chip->hwirqs[hwirq];
	داده->irq_state &= ~IRQ_ENABLED;
	أرجع 0;
}

ساكن بنية sbi_irqchip_handler *sbi_irqchip_پیدا_کن_handler(بنية sbi_irqchip_device *chip,
							    u32 hwirq)
{
	بنية sbi_irqchip_handler *h;

	إذا (!chip || chip->رقم_hwirq <= hwirq)
		أرجع عدم;

	sbi_list_for_each_مدخل(h, &chip->handler_list, گره) {
		إذا (h->اولی_hwirq <= hwirq && hwirq < (h->اولی_hwirq + h->رقم_hwirq))
			أرجع h;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_مجموعه_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq, فراغ *priv)
{
	بنية sbi_irqchip_hwirq_داده *داده;

	إذا (!chip || chip->رقم_hwirq <= hwirq)
	     أرجع SBI_EINVAL;

	داده = &chip->hwirqs[hwirq];
	داده->priv = priv;

	أرجع 0;
}

فراغ *sbi_irqchip_get_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_داده *داده;
	إذا (!chip || chip->رقم_hwirq <= hwirq)
	    أرجع عدم;

	داده = &chip->hwirqs[hwirq];

	أرجع داده->priv;
}

صحيح sbi_irqchip_raw_handler_default(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_handler *h;
	صحيح rc = SBI_OK;

	إذا (!chip || chip->رقم_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_پیدا_کن_handler(chip, hwirq);
	إذا (!h)
		أرجع SBI_EINVAL;

	إذا (h->callback)
		rc = h->callback(hwirq, h->priv);

	إذا (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	أرجع rc;
}

صحيح sbi_irqchip_مجموعه_raw_handler(بنية sbi_irqchip_device *chip, u32 hwirq,
				صحيح (*raw_hndl)(بنية sbi_irqchip_device *, u32))
{
	بنية sbi_irqchip_hwirq_داده *داده;

	إذا (!chip || chip->رقم_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	داده = &chip->hwirqs[hwirq];
	داده->raw_handler = raw_hndl;
	أرجع 0;
}

صحيح sbi_irqchip_write_msi(بنية sbi_irqchip_device *chip, u32 hwirq,
			  ثابت بنية sbi_irqchip_msi_پیام *پیام)
{
	بنية sbi_irqchip_handler *h;

	إذا (!chip || chip->رقم_hwirq <= hwirq || !پیام)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_پیدا_کن_handler(chip, hwirq);
	إذا (!h)
		أرجع SBI_EFAIL;
	إذا (!h->write_msi)
		أرجع SBI_ENOTSUPP;

	h->write_msi(hwirq, پیام, h->priv);
	أرجع 0;
}

صحيح sbi_irqchip_get_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 *بیرون_hart_شاخص)
{
	إذا (!chip || chip->رقم_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	إذا (!sbi_irqchip_پیدا_کن_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	*بیرون_hart_شاخص = chip->hwirqs[hwirq].hart_شاخص;
	أرجع 0;
}

صحيح sbi_irqchip_مجموعه_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_شاخص)
{
	بنية sbi_irqchip_hwirq_داده *داده;
	صحيح rc;

	إذا (!chip || chip->رقم_hwirq <= hwirq || sbi_hart_شمار() <= hart_شاخص)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	إذا (!sbi_irqchip_پیدا_کن_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	داده = &chip->hwirqs[hwirq];
	إذا (داده->hart_شاخص != hart_شاخص) {
		إذا (chip->hwirq_مجموعه_affinity) {
			rc = chip->hwirq_مجموعه_affinity(chip, hwirq, hart_شاخص);
			إذا (rc)
				أرجع rc;
		}
		داده->hart_شاخص = hart_شاخص;
	}

	أرجع 0;
}

ساكن صحيح __sbi_irqchip_handler_set_affinity(بنية sbi_irqchip_device *chip,
					      بنية sbi_irqchip_handler *h,
					      u32 compare_hart_شاخص,
					      u32 hart_شاخص)
{
	u32 i, current_hart_شاخص;
	صحيح rc;

	لكل (i = 0; i < h->رقم_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->اولی_hwirq + i,
					      &current_hart_شاخص);
		إذا (rc)
			أرجع rc;

		إذا (compare_hart_شاخص != -1U &&
		    current_hart_شاخص != compare_hart_شاخص)
			استمر;

		rc = sbi_irqchip_مجموعه_affinity(chip, h->اولی_hwirq + i, hart_شاخص);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح __sbi_irqchip_register_handler(بنية sbi_irqchip_device *chip,
					  u32 اولی_hwirq, u32 رقم_hwirq, u32 hwirq_flags,
					  فراغ (*write_msi)(u32 hwirq,
							    ثابت بنية sbi_irqchip_msi_پیام *پیام,
							    فراغ *priv),
					  صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	بنية sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	صحيح rc;

	لكل (i = اولی_hwirq; i < (اولی_hwirq + رقم_hwirq); i++) {
		h = sbi_irqchip_پیدا_کن_handler(chip, i);
		إذا (h)
			أرجع SBI_EALREADY;
	}

	h = sbi_zalloc(حجم(*h));
	إذا (!h)
		أرجع SBI_ENOMEM;
	h->اولی_hwirq = اولی_hwirq;
	h->رقم_hwirq = رقم_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = عدم;
	sbi_list_for_each_مدخل(th, &chip->handler_list, گره) {
		إذا (h->اولی_hwirq < th->اولی_hwirq) {
			nh = th;
			اكسر;
		}
	}
	إذا (nh)
		sbi_list_جمع_کن(&h->گره, &nh->گره);
	وإلا
		sbi_list_جمع_کن_دم(&h->گره, &chip->handler_list);

	إذا (chip->hwirq_راهاندازی) {
		لكل (i = 0; i < h->رقم_hwirq; i++) {
			rc = chip->hwirq_راهاندازی(chip, h->اولی_hwirq + i, hwirq_flags);
			إذا (rc) {
				إذا (chip->hwirq_cleanup) {
					لكل (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->اولی_hwirq + j);
				}
				sbi_list_del(&h->گره);
				sbi_free(h);
				أرجع rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	إذا (rc) {
		إذا (chip->hwirq_cleanup) {
			لكل (i = 0; i < h->رقم_hwirq; i++)
				chip->hwirq_cleanup(chip, h->اولی_hwirq + i);
		}
		sbi_list_del(&h->گره);
		sbi_free(h);
		أرجع rc;
	}

	إذا (chip->hwirq_unmask) {
		لكل (i = 0; i < h->رقم_hwirq; i++)
			chip->hwirq_unmask(chip, h->اولی_hwirq + i);
	}

	أرجع 0;
}

صحيح sbi_irqchip_ثبات_msi(بنية sbi_irqchip_device *chip, u32 رقم_hwirq,
			     فراغ (*write_msi)(u32 hwirq,
					       ثابت بنية sbi_irqchip_msi_پیام *پیام,
					       فراغ *priv),
			     صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv,
			     u32 *بیرون_اولی_hwirq)
{
	بنية sbi_irqchip_handler *h;
	bool یافت;
	u32 hwirq;

	إذا (!chip || !chip->hwirq_مجموعه_affinity || !رقم_hwirq ||
	    !write_msi || !callback || !بیرون_اولی_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->رقم_hwirq < رقم_hwirq)
		أرجع SBI_EBAD_RANGE;

	hwirq = 0;
	یافت = false;
	sbi_list_for_each_مدخل(h, &chip->handler_list, گره) {
		إذا (h->اولی_hwirq <= hwirq && hwirq < (h->اولی_hwirq + h->رقم_hwirq)) {
			hwirq = h->اولی_hwirq + h->رقم_hwirq;
		} وإلا إذا (hwirq < h->اولی_hwirq) {
			إذا (h->اولی_hwirq - hwirq >= رقم_hwirq) {
				یافت = true;
				اكسر;
			} وإلا {
				hwirq = h->اولی_hwirq + h->رقم_hwirq;
			}
		}
	}
	إذا (!یافت && (chip->رقم_hwirq - hwirq) >= رقم_hwirq)
		یافت = true;
	إذا (!یافت)
		أرجع SBI_ENOSPC;
	*بیرون_اولی_hwirq = hwirq;

	أرجع __sbi_irqchip_register_handler(chip, *بیرون_اولی_hwirq,
					      رقم_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

صحيح sbi_irqchip_ثبات_handler(بنية sbi_irqchip_device *chip,
				 u32 اولی_hwirq, u32 رقم_hwirq, u32 hwirq_flags,
				 صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	إذا (!chip || !رقم_hwirq || !callback)
		أرجع SBI_EINVAL;
	إذا (chip->رقم_hwirq <= اولی_hwirq ||
	    chip->رقم_hwirq <= (اولی_hwirq + رقم_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, اولی_hwirq, رقم_hwirq, hwirq_flags,
					      عدم, callback, priv);
}

صحيح sbi_irqchip_ثبات_reserved(بنية sbi_irqchip_device *chip,
				  u32 اولی_hwirq, u32 رقم_hwirq)
{
	إذا (!chip || !رقم_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->رقم_hwirq <= اولی_hwirq ||
	    chip->رقم_hwirq <= (اولی_hwirq + رقم_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, اولی_hwirq, رقم_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, عدم, عدم, عدم);
}

صحيح sbi_irqchip_unregister_handler(بنية sbi_irqchip_device *chip,
				   u32 اولی_hwirq, u32 رقم_hwirq)
{
	بنية sbi_irqchip_handler *fh, *lh;
	u32 i;

	إذا (!chip || !رقم_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->رقم_hwirq <= اولی_hwirq ||
	    chip->رقم_hwirq <= (اولی_hwirq + رقم_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	fh = sbi_irqchip_پیدا_کن_handler(chip, اولی_hwirq);
	إذا (!fh || fh->اولی_hwirq != اولی_hwirq || fh->رقم_hwirq != رقم_hwirq)
		أرجع SBI_ENODEV;
	lh = sbi_irqchip_پیدا_کن_handler(chip, اولی_hwirq + رقم_hwirq - 1);
	إذا (!lh || lh != fh)
		أرجع SBI_ENODEV;

	إذا (chip->hwirq_ماسک) {
		لكل (i = 0; i < fh->رقم_hwirq; i++)
			chip->hwirq_ماسک(chip, fh->اولی_hwirq + i);
	}

	إذا (chip->hwirq_cleanup) {
		لكل (i = 0; i < fh->رقم_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->اولی_hwirq + i);
	}

	sbi_list_del(&fh->گره);
	أرجع 0;
}

بنية sbi_irqchip_device *sbi_irqchip_پیدا_کن_device_by_caps(بیعلامت دراز caps,
							   بنية sbi_irqchip_device *اولی)
{
	بنية sbi_irqchip_device *chip;
	bool یافت = (اولی == عدم);

	sbi_list_for_each_مدخل(chip, &irqchip_list, گره) {
		إذا (!یافت) {
			إذا (اولی == chip)
				یافت = true;
			استمر;
		}
		إذا ((chip->caps & caps) == caps)
			أرجع chip;
	}

	أرجع عدم;
}

بنية sbi_irqchip_device *sbi_irqchip_پیدا_کن_device(u32 id)
{
	بنية sbi_irqchip_device *chip;

	sbi_list_for_each_مدخل(chip, &irqchip_list, گره) {
		إذا (chip->id == id)
			أرجع chip;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_جمع_کن_device(بنية sbi_irqchip_device *chip)
{
	بنية sbi_irqchip_hart_داده *hd;
	بنية sbi_scratch *scratch;
	u32 i, h;

	إذا (!chip || !chip->رقم_hwirq || !sbi_hartmask_وزن(&chip->هدف_harts))
		أرجع SBI_EINVAL;

	إذا (sbi_irqchip_پیدا_کن_device(chip->id))
		أرجع SBI_EALREADY;

	إذا (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->هدف_harts) {
			scratch = sbi_hartindex_مقصد_scratch(h);
			إذا (!scratch)
				استمر;

			hd = sbi_scratch_جابه_جایی_ptr(scratch, irqchip_hart_داده_off);
			إذا (hd->chip && hd->chip != chip)
				أرجع SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(حجم(*chip->hwirqs) * chip->رقم_hwirq);
	إذا (!chip->hwirqs)
		أرجع SBI_ENOMEM;
	لكل (i = 0; i < chip->رقم_hwirq; i++) {
		sbi_irqchip_مجموعه_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_شاخص = -1U;
	}

	SBI_INIT_LIST_سر(&chip->handler_list);

	sbi_list_جمع_کن_دم(&chip->گره, &irqchip_list);
	أرجع 0;
}

صحيح sbi_irqchip_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	بنية sbi_irqchip_hart_داده *hd;
	بنية sbi_irqchip_device *chip;
	صحيح rc;

	إذا (cold_boot) {
		irqchip_hart_داده_off =
			sbi_scratch_alloc_جابه_جایی(حجم(بنية sbi_irqchip_hart_داده));
		إذا (!irqchip_hart_داده_off)
			أرجع SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		إذا (rc)
			أرجع rc;
	}

	sbi_list_for_each_مدخل(chip, &irqchip_list, گره) {
		إذا (!chip->warm_init)
			استمر;
		إذا (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->هدف_harts))
			استمر;
		rc = chip->warm_init(chip);
		إذا (rc)
			أرجع rc;
	}

	hd = sbi_scratch_thishart_جابه_جایی_ptr(irqchip_hart_داده_off);
	إذا (hd && hd->chip && hd->chip->process_hwirqs)
		csr_مجموعه(CSR_MIE, MIP_MEIP);

	أرجع 0;
}

فراغ sbi_irqchip_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_irqchip_hart_داده *hd;
	بنية sbi_irqchip_device *chip;
	بنية sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	صحيح rc;

	sbi_for_each_hartindex(i) {
		إذا (i == current_hartindex())
			استمر;
		إذا (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			استمر;
		migrate_hidx = i;
		migrate = true;
		اكسر;
	}

	إذا (!migrate)
		اذهب skip_migrate;
	sbi_list_for_each_مدخل(chip, &irqchip_list, گره) {
		sbi_list_for_each_مدخل(h, &chip->handler_list, گره) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			إذا (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->اولی_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_جابه_جایی_ptr(irqchip_hart_داده_off);
	إذا (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
