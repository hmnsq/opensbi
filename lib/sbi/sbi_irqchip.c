/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_irqchip.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_list.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
ସଂରଚନା sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*raw_handler)(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	ଶୂନ୍ୟ *priv;
};

/** Internal irqchip interrupt handler */
ସଂରଚନା sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	ସଂରଚନା sbi_dlist ଗଣ୍ଠି;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 ଅଙ୍କ_hwirq;

	/** Write MSI function of this handler */
	ଶୂନ୍ୟ (*write_msi)(u32 hwirq, ଅଚଳ ସଂରଚନା sbi_irqchip_msi_ସନ୍ଦେଶ *ସନ୍ଦେଶ, ଶୂନ୍ୟ *priv);

	/** Callback function of this handler */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*callback)(u32 hwirq, ଶୂନ୍ୟ *priv);

	/** Callback private data */
	ଶୂନ୍ୟ *priv;
};

ସଂରଚନା sbi_irqchip_hart_data {
	ସଂରଚନା sbi_irqchip_device *chip;
};

ସ୍ଥିର unsigned long irqchip_hart_data_off;
ସ୍ଥିର SBI_LIST_HEAD(irqchip_list);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_process(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ଯଦି (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଫେରନ୍ତୁ hd->chip->process_hwirqs(hd->chip);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_process_hwirq(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା sbi_irqchip_hwirq_data *data;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ଯଦି (!data->raw_handler)
		ଫେରନ୍ତୁ SBI_ENOENT;

	ଫେରନ୍ତୁ data->raw_handler(chip, hwirq);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32 sbi_irqchip_get_irq_state(ସଂରଚନା sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ଯଦି (!chip || !chip->hwirqs || hwirq >= chip->ଅଙ୍କ_hwirq)
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(ସଂରଚନା sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ଫେରନ୍ତୁ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_unmask_hwirq(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା sbi_irqchip_hwirq_data *data;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ଯଦି (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ଫେରନ୍ତୁ SBI_EALREADY;

	ଯଦି (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_mask_hwirq(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା sbi_irqchip_hwirq_data *data;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ଫେରନ୍ତୁ SBI_EALREADY;

	ଯଦି (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା sbi_irqchip_handler *sbi_irqchip_find_handler(ସଂରଚନା sbi_irqchip_device *chip,
							    u32 hwirq)
{
	ସଂରଚନା sbi_irqchip_handler *h;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
		ଫେରନ୍ତୁ ଖାଲି;

	sbi_list_for_each_entry(h, &chip->handler_list, ଗଣ୍ଠି) {
		ଯଦି (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->ଅଙ୍କ_hwirq))
			ଫେରନ୍ତୁ h;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_set_hwirq_priv(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq, ଶୂନ୍ୟ *priv)
{
	ସଂରଚନା sbi_irqchip_hwirq_data *data;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
	     ଫେରନ୍ତୁ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ *sbi_irqchip_get_hwirq_priv(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା sbi_irqchip_hwirq_data *data;
	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
	    ଫେରନ୍ତୁ ଖାଲି;

	data = &chip->hwirqs[hwirq];

	ଫେରନ୍ତୁ data->priv;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_raw_handler_default(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା sbi_irqchip_handler *h;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = SBI_OK;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ଯଦି (!h)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (h->callback)
		rc = h->callback(hwirq, h->priv);

	ଯଦି (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ଫେରନ୍ତୁ rc;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_set_raw_handler(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq,
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*raw_hndl)(ସଂରଚନା sbi_irqchip_device *, u32))
{
	ସଂରଚନା sbi_irqchip_hwirq_data *data;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_write_msi(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq,
			  ଅଚଳ ସଂରଚନା sbi_irqchip_msi_ସନ୍ଦେଶ *ସନ୍ଦେଶ)
{
	ସଂରଚନା sbi_irqchip_handler *h;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq || !ସନ୍ଦେଶ)
		ଫେରନ୍ତୁ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ଯଦି (!h)
		ଫେରନ୍ତୁ SBI_EFAIL;
	ଯଦି (!h->write_msi)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	h->write_msi(hwirq, ସନ୍ଦେଶ, h->priv);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_get_affinity(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ଯଦି (!sbi_irqchip_find_handler(chip, hwirq))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_set_affinity(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	ସଂରଚନା sbi_irqchip_hwirq_data *data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (!chip || chip->ଅଙ୍କ_hwirq <= hwirq || sbi_hart_ଗଣନା() <= hart_index)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ଯଦି (!sbi_irqchip_find_handler(chip, hwirq))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	ଯଦି (data->hart_index != hart_index) {
		ଯଦି (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			ଯଦି (rc)
				ଫେରନ୍ତୁ rc;
		}
		data->hart_index = hart_index;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __sbi_irqchip_handler_set_affinity(ସଂରଚନା sbi_irqchip_device *chip,
					      ସଂରଚନା sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ପାଇଁ (i = 0; i < h->ଅଙ୍କ_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		ଯଦି (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			ଜାରି;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __sbi_irqchip_register_handler(ସଂରଚନା sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 ଅଙ୍କ_hwirq, u32 hwirq_flags,
					  ଶୂନ୍ୟ (*write_msi)(u32 hwirq,
							    ଅଚଳ ସଂରଚନା sbi_irqchip_msi_ସନ୍ଦେଶ *ସନ୍ଦେଶ,
							    ଶୂନ୍ୟ *priv),
					  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*callback)(u32 hwirq, ଶୂନ୍ୟ *priv), ଶୂନ୍ୟ *priv)
{
	ସଂରଚନା sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ପାଇଁ (i = first_hwirq; i < (first_hwirq + ଅଙ୍କ_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		ଯଦି (h)
			ଫେରନ୍ତୁ SBI_EALREADY;
	}

	h = sbi_zalloc(ଆକାର(*h));
	ଯଦି (!h)
		ଫେରନ୍ତୁ SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->ଅଙ୍କ_hwirq = ଅଙ୍କ_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = ଖାଲି;
	sbi_list_for_each_entry(th, &chip->handler_list, ଗଣ୍ଠି) {
		ଯଦି (h->first_hwirq < th->first_hwirq) {
			nh = th;
			ଭାଙ୍ଗ;
		}
	}
	ଯଦି (nh)
		sbi_list_add(&h->ଗଣ୍ଠି, &nh->ଗଣ୍ଠି);
	ଅନ୍ୟଥା
		sbi_list_add_tail(&h->ଗଣ୍ଠି, &chip->handler_list);

	ଯଦି (chip->hwirq_setup) {
		ପାଇଁ (i = 0; i < h->ଅଙ୍କ_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			ଯଦି (rc) {
				ଯଦି (chip->hwirq_cleanup) {
					ପାଇଁ (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->ଗଣ୍ଠି);
				sbi_free(h);
				ଫେରନ୍ତୁ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ଯଦି (rc) {
		ଯଦି (chip->hwirq_cleanup) {
			ପାଇଁ (i = 0; i < h->ଅଙ୍କ_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->ଗଣ୍ଠି);
		sbi_free(h);
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି (chip->hwirq_unmask) {
		ପାଇଁ (i = 0; i < h->ଅଙ୍କ_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_register_msi(ସଂରଚନା sbi_irqchip_device *chip, u32 ଅଙ୍କ_hwirq,
			     ଶୂନ୍ୟ (*write_msi)(u32 hwirq,
					       ଅଚଳ ସଂରଚନା sbi_irqchip_msi_ସନ୍ଦେଶ *ସନ୍ଦେଶ,
					       ଶୂନ୍ୟ *priv),
			     ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*callback)(u32 hwirq, ଶୂନ୍ୟ *priv), ଶୂନ୍ୟ *priv,
			     u32 *out_first_hwirq)
{
	ସଂରଚନା sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	ଯଦି (!chip || !chip->hwirq_set_affinity || !ଅଙ୍କ_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (chip->ଅଙ୍କ_hwirq < ଅଙ୍କ_hwirq)
		ଫେରନ୍ତୁ SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, ଗଣ୍ଠି) {
		ଯଦି (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->ଅଙ୍କ_hwirq)) {
			hwirq = h->first_hwirq + h->ଅଙ୍କ_hwirq;
		} ଅନ୍ୟଥା ଯଦି (hwirq < h->first_hwirq) {
			ଯଦି (h->first_hwirq - hwirq >= ଅଙ୍କ_hwirq) {
				found = true;
				ଭାଙ୍ଗ;
			} ଅନ୍ୟଥା {
				hwirq = h->first_hwirq + h->ଅଙ୍କ_hwirq;
			}
		}
	}
	ଯଦି (!found && (chip->ଅଙ୍କ_hwirq - hwirq) >= ଅଙ୍କ_hwirq)
		found = true;
	ଯଦି (!found)
		ଫେରନ୍ତୁ SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	ଫେରନ୍ତୁ __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      ଅଙ୍କ_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_register_handler(ସଂରଚନା sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 ଅଙ୍କ_hwirq, u32 hwirq_flags,
				 ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*callback)(u32 hwirq, ଶୂନ୍ୟ *priv), ଶୂନ୍ୟ *priv)
{
	ଯଦି (!chip || !ଅଙ୍କ_hwirq || !callback)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (chip->ଅଙ୍କ_hwirq <= first_hwirq ||
	    chip->ଅଙ୍କ_hwirq <= (first_hwirq + ଅଙ୍କ_hwirq - 1))
		ଫେରନ୍ତୁ SBI_EBAD_RANGE;

	ଫେରନ୍ତୁ __sbi_irqchip_register_handler(chip, first_hwirq, ଅଙ୍କ_hwirq, hwirq_flags,
					      ଖାଲି, callback, priv);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_register_reserved(ସଂରଚନା sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 ଅଙ୍କ_hwirq)
{
	ଯଦି (!chip || !ଅଙ୍କ_hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (chip->ଅଙ୍କ_hwirq <= first_hwirq ||
	    chip->ଅଙ୍କ_hwirq <= (first_hwirq + ଅଙ୍କ_hwirq - 1))
		ଫେରନ୍ତୁ SBI_EBAD_RANGE;

	ଫେରନ୍ତୁ __sbi_irqchip_register_handler(chip, first_hwirq, ଅଙ୍କ_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, ଖାଲି, ଖାଲି, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_unregister_handler(ସଂରଚନା sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 ଅଙ୍କ_hwirq)
{
	ସଂରଚନା sbi_irqchip_handler *fh, *lh;
	u32 i;

	ଯଦି (!chip || !ଅଙ୍କ_hwirq)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (chip->ଅଙ୍କ_hwirq <= first_hwirq ||
	    chip->ଅଙ୍କ_hwirq <= (first_hwirq + ଅଙ୍କ_hwirq - 1))
		ଫେରନ୍ତୁ SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	ଯଦି (!fh || fh->first_hwirq != first_hwirq || fh->ଅଙ୍କ_hwirq != ଅଙ୍କ_hwirq)
		ଫେରନ୍ତୁ SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + ଅଙ୍କ_hwirq - 1);
	ଯଦି (!lh || lh != fh)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯଦି (chip->hwirq_mask) {
		ପାଇଁ (i = 0; i < fh->ଅଙ୍କ_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	ଯଦି (chip->hwirq_cleanup) {
		ପାଇଁ (i = 0; i < fh->ଅଙ୍କ_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->ଗଣ୍ଠି);
	ଫେରନ୍ତୁ 0;
}

ସଂରଚନା sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   ସଂରଚନା sbi_irqchip_device *first)
{
	ସଂରଚନା sbi_irqchip_device *chip;
	bool found = (first == ଖାଲି);

	sbi_list_for_each_entry(chip, &irqchip_list, ଗଣ୍ଠି) {
		ଯଦି (!found) {
			ଯଦି (first == chip)
				found = true;
			ଜାରି;
		}
		ଯଦି ((chip->caps & caps) == caps)
			ଫେରନ୍ତୁ chip;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ସଂରଚନା sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	ସଂରଚନା sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, ଗଣ୍ଠି) {
		ଯଦି (chip->id == id)
			ଫେରନ୍ତୁ chip;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_add_device(ସଂରଚନା sbi_irqchip_device *chip)
{
	ସଂରଚନା sbi_irqchip_hart_data *hd;
	ସଂରଚନା sbi_scratch *scratch;
	u32 i, h;

	ଯଦି (!chip || !chip->ଅଙ୍କ_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (sbi_irqchip_find_device(chip->id))
		ଫେରନ୍ତୁ SBI_EALREADY;

	ଯଦି (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			ଯଦି (!scratch)
				ଜାରି;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			ଯଦି (hd->chip && hd->chip != chip)
				ଫେରନ୍ତୁ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(ଆକାର(*chip->hwirqs) * chip->ଅଙ୍କ_hwirq);
	ଯଦି (!chip->hwirqs)
		ଫେରନ୍ତୁ SBI_ENOMEM;
	ପାଇଁ (i = 0; i < chip->ଅଙ୍କ_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->ଗଣ୍ଠି, &irqchip_list);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_irqchip_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_ptr(scratch);
	ସଂରଚନା sbi_irqchip_hart_data *hd;
	ସଂରଚନା sbi_irqchip_device *chip;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(ଆକାର(ସଂରଚନା sbi_irqchip_hart_data));
		ଯଦି (!irqchip_hart_data_off)
			ଫେରନ୍ତୁ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, ଗଣ୍ଠି) {
		ଯଦି (!chip->warm_init)
			ଜାରି;
		ଯଦି (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			ଜାରି;
		rc = chip->warm_init(chip);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ଯଦି (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_irqchip_exit(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_irqchip_hart_data *hd;
	ସଂରଚନା sbi_irqchip_device *chip;
	ସଂରଚନା sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	sbi_for_each_hartindex(i) {
		ଯଦି (i == current_hartindex())
			ଜାରି;
		ଯଦି (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			ଜାରି;
		migrate_hidx = i;
		migrate = true;
		ଭାଙ୍ଗ;
	}

	ଯଦି (!migrate)
		ଯାଅ skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, ଗଣ୍ଠି) {
		sbi_list_for_each_entry(h, &chip->handler_list, ଗଣ୍ଠି) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ଯଦି (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ଯଦି (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
