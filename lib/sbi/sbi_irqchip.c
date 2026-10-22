/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_irqchip.h>
#include <sbi/sbi_list.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
sa_nracanaa sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	පූර්ණ_සංඛ්‍යා (*raw_handler)(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	void *priv;
};

/** Internal irqchip interrupt handler */
sa_nracanaa sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	sa_nracanaa sbi_dlist node;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 num_hwirq;

	/** Write MSI function of this handler */
	void (*write_msi)(u32 hwirq, const sa_nracanaa sbi_irqchip_msi_msg *msg, void *priv);

	/** Callback function of this handler */
	පූර්ණ_සංඛ්‍යා (*callback)(u32 hwirq, void *priv);

	/** Callback private data */
	void *priv;
};

sa_nracanaa sbi_irqchip_hart_data {
	sa_nracanaa sbi_irqchip_device *chip;
};

static unsigned long irqchip_hart_data_off;
static SBI_LIST_HEAD(irqchip_list);

පූර්ණ_සංඛ්‍යා sbi_irqchip_process(void)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	නම් (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ආපසු SBI_ENODEV;

	ආපසු hd->chip->process_hwirqs(hd->chip);
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_process_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	නම් (!chip || chip->num_hwirq <= hwirq)
		ආපසු SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	නම් (!data->raw_handler)
		ආපසු SBI_ENOENT;

	ආපසු data->raw_handler(chip, hwirq);
}

static anta_hstha u32 sbi_irqchip_get_irq_state(sa_nracanaa sbi_irqchip_device *chip,
					     u32 hwirq)
{
	නම් (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		ආපසු 0;

	ආපසු chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(sa_nracanaa sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ආපසු !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_unmask_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	නම් (!chip || chip->num_hwirq <= hwirq)
		ආපසු SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	නම් (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ආපසු SBI_EALREADY;

	නම් (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_mask_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	නම් (!chip || chip->num_hwirq <= hwirq)
		ආපසු SBI_EINVAL;

	නම් (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ආපසු SBI_EALREADY;

	නම් (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	ආපසු 0;
}

static sa_nracanaa sbi_irqchip_handler *sbi_irqchip_find_handler(sa_nracanaa sbi_irqchip_device *chip,
							    u32 hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;

	නම් (!chip || chip->num_hwirq <= hwirq)
		ආපසු NULL;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		නම් (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			ආපසු h;
	}

	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_set_hwirq_priv(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq, void *priv)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	නම් (!chip || chip->num_hwirq <= hwirq)
	     ආපසු SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	ආපසු 0;
}

void *sbi_irqchip_get_hwirq_priv(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;
	නම් (!chip || chip->num_hwirq <= hwirq)
	    ආපසු NULL;

	data = &chip->hwirqs[hwirq];

	ආපසු data->priv;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_raw_handler_default(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;
	පූර්ණ_සංඛ්‍යා rc = SBI_OK;

	නම් (!chip || chip->num_hwirq <= hwirq)
		ආපසු SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	නම් (!h)
		ආපසු SBI_EINVAL;

	නම් (h->callback)
		rc = h->callback(hwirq, h->priv);

	නම් (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ආපසු rc;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_set_raw_handler(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
				පූර්ණ_සංඛ්‍යා (*raw_hndl)(sa_nracanaa sbi_irqchip_device *, u32))
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	නම් (!chip || chip->num_hwirq <= hwirq)
		ආපසු SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_write_msi(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			  const sa_nracanaa sbi_irqchip_msi_msg *msg)
{
	sa_nracanaa sbi_irqchip_handler *h;

	නම් (!chip || chip->num_hwirq <= hwirq || !msg)
		ආපසු SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	නම් (!h)
		ආපසු SBI_EFAIL;
	නම් (!h->write_msi)
		ආපසු SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_get_affinity(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	නම් (!chip || chip->num_hwirq <= hwirq)
		ආපසු SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	නම් (!sbi_irqchip_find_handler(chip, hwirq))
		ආපසු SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_set_affinity(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;
	පූර්ණ_සංඛ්‍යා rc;

	නම් (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		ආපසු SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	නම් (!sbi_irqchip_find_handler(chip, hwirq))
		ආපසු SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	නම් (data->hart_index != hart_index) {
		නම් (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			නම් (rc)
				ආපසු rc;
		}
		data->hart_index = hart_index;
	}

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා __sbi_irqchip_handler_set_affinity(sa_nracanaa sbi_irqchip_device *chip,
					      sa_nracanaa sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	පූර්ණ_සංඛ්‍යා rc;

	සඳහා (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		නම් (rc)
			ආපසු rc;

		නම් (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			continue;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		නම් (rc)
			ආපසු rc;
	}

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා __sbi_irqchip_register_handler(sa_nracanaa sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
					  void (*write_msi)(u32 hwirq,
							    const sa_nracanaa sbi_irqchip_msi_msg *msg,
							    void *priv),
					  පූර්ණ_සංඛ්‍යා (*callback)(u32 hwirq, void *priv), void *priv)
{
	sa_nracanaa sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	පූර්ණ_සංඛ්‍යා rc;

	සඳහා (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		නම් (h)
			ආපසු SBI_EALREADY;
	}

	h = sbi_zalloc(parimaa_na(*h));
	නම් (!h)
		ආපසු SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = NULL;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		නම් (h->first_hwirq < th->first_hwirq) {
			nh = th;
			bha_nga;
		}
	}
	නම් (nh)
		sbi_list_add(&h->node, &nh->node);
	නැතහොත්
		sbi_list_add_tail(&h->node, &chip->handler_list);

	නම් (chip->hwirq_setup) {
		සඳහා (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			නම් (rc) {
				නම් (chip->hwirq_cleanup) {
					සඳහා (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				ආපසු rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	නම් (rc) {
		නම් (chip->hwirq_cleanup) {
			සඳහා (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		ආපසු rc;
	}

	නම් (chip->hwirq_unmask) {
		සඳහා (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_register_msi(sa_nracanaa sbi_irqchip_device *chip, u32 num_hwirq,
			     void (*write_msi)(u32 hwirq,
					       const sa_nracanaa sbi_irqchip_msi_msg *msg,
					       void *priv),
			     පූර්ණ_සංඛ්‍යා (*callback)(u32 hwirq, void *priv), void *priv,
			     u32 *out_first_hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	නම් (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		ආපසු SBI_EINVAL;
	නම් (chip->num_hwirq < num_hwirq)
		ආපසු SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		නම් (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} නැතහොත් නම් (hwirq < h->first_hwirq) {
			නම් (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				bha_nga;
			} නැතහොත් {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	නම් (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	නම් (!found)
		ආපසු SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	ආපසු __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_register_handler(sa_nracanaa sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 පූර්ණ_සංඛ්‍යා (*callback)(u32 hwirq, void *priv), void *priv)
{
	නම් (!chip || !num_hwirq || !callback)
		ආපසු SBI_EINVAL;
	නම් (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ආපසු SBI_EBAD_RANGE;

	ආපසු __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      NULL, callback, priv);
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_register_reserved(sa_nracanaa sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	නම් (!chip || !num_hwirq)
		ආපසු SBI_EINVAL;
	නම් (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ආපසු SBI_EBAD_RANGE;

	ආපසු __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, NULL, NULL, NULL);
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_unregister_handler(sa_nracanaa sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	sa_nracanaa sbi_irqchip_handler *fh, *lh;
	u32 i;

	නම් (!chip || !num_hwirq)
		ආපසු SBI_EINVAL;
	නම් (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ආපසු SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	නම් (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		ආපසු SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	නම් (!lh || lh != fh)
		ආපසු SBI_ENODEV;

	නම් (chip->hwirq_mask) {
		සඳහා (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	නම් (chip->hwirq_cleanup) {
		සඳහා (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	ආපසු 0;
}

sa_nracanaa sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   sa_nracanaa sbi_irqchip_device *first)
{
	sa_nracanaa sbi_irqchip_device *chip;
	bool found = (first == NULL);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		නම් (!found) {
			නම් (first == chip)
				found = true;
			continue;
		}
		නම් ((chip->caps & caps) == caps)
			ආපසු chip;
	}

	ආපසු NULL;
}

sa_nracanaa sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	sa_nracanaa sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		නම් (chip->id == id)
			ආපසු chip;
	}

	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_add_device(sa_nracanaa sbi_irqchip_device *chip)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_scratch *scratch;
	u32 i, h;

	නම් (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		ආපසු SBI_EINVAL;

	නම් (sbi_irqchip_find_device(chip->id))
		ආපසු SBI_EALREADY;

	නම් (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			නම් (!scratch)
				continue;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			නම් (hd->chip && hd->chip != chip)
				ආපසු SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(parimaa_na(*chip->hwirqs) * chip->num_hwirq);
	නම් (!chip->hwirqs)
		ආපසු SBI_ENOMEM;
	සඳහා (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා sbi_irqchip_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_irqchip_device *chip;
	පූර්ණ_සංඛ්‍යා rc;

	නම් (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(parimaa_na(sa_nracanaa sbi_irqchip_hart_data));
		නම් (!irqchip_hart_data_off)
			ආපසු SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		නම් (rc)
			ආපසු rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		නම් (!chip->warm_init)
			continue;
		නම් (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			continue;
		rc = chip->warm_init(chip);
		නම් (rc)
			ආපසු rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	නම් (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	ආපසු 0;
}

void sbi_irqchip_exit(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_irqchip_device *chip;
	sa_nracanaa sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	පූර්ණ_සංඛ්‍යා rc;

	sbi_for_each_hartindex(i) {
		නම් (i == current_hartindex())
			continue;
		නම් (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			continue;
		migrate_hidx = i;
		migrate = true;
		bha_nga;
	}

	නම් (!migrate)
		goto skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			නම් (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	නම් (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
