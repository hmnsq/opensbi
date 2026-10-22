/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_bitmap.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_hfence.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_tlb.h>
#અન્તર્ભાવ <sbi/sbi_types.h>

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
સ્થિર unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), શૂન્ય *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), શૂન્ય *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

સંરચના fwft_config;

સંરચના fwft_feature {
	ગણના sbi_fwft_feature_t id;
	પૂર્ણાંક (*supported)(સંરચના fwft_config *conf);
	પૂર્ણાંક (*set)(સંરચના fwft_config *conf, unsigned long કિંમત);
	પૂર્ણાંક (*get)(સંરચના fwft_config *conf, unsigned long *કિંમત);
};

સંરચના fwft_config {
	અચલ સંરચના fwft_feature *feature;
	unsigned long flags;
};

સંરચના fwft_hart_state {
	unsigned પૂર્ણાંક config_ગણતરી;
	સંરચના fwft_config configs[];
};

સ્થિર અચલ unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_ઢગલો,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

સ્થિર bool fwft_is_defined_feature(ગણના sbi_fwft_feature_t feature)
{
	પૂર્ણાંક i;

	માટે (i = 0; i < array_size(fwft_defined_features); i++) {
		જો (fwft_defined_features[i] == feature)
			પાછા true;
	}

	પાછા false;
}

સ્થિર પૂર્ણાંક fwft_menvcfg_set_bit(unsigned long કિંમત, unsigned long bit)
{
	જો (કિંમત == 1) {
		જો (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		નહીંતર
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} નહીંતર જો (કિંમત == 0) {
		જો (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		નહીંતર
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} નહીંતર {
		પાછા SBI_EINVAL;
	}

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_menvcfg_read_bit(unsigned long *કિંમત, unsigned long bit)
{
	unsigned long cfg;

	જો (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	નહીંતર
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*કિંમત = cfg != 0;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_misaligned_delegation_supported(સંરચના fwft_config *conf)
{
	જો (!misa_extension('S'))
		પાછા SBI_ENOTSUPP;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_set_misaligned_delegation(સંરચના fwft_config *conf,
					 unsigned long કિંમત)
{
	જો (કિંમત == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	નહીંતર જો (કિંમત == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	નહીંતર
		પાછા SBI_EINVAL;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_get_misaligned_delegation(સંરચના fwft_config *conf,
					 unsigned long *કિંમત)
{
	*કિંમત = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_double_trap_supported(સંરચના fwft_config *conf)
{
	જો (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		પાછા SBI_ENOTSUPP;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_set_double_trap(સંરચના fwft_config *conf, unsigned long કિંમત)
{
	પાછા fwft_menvcfg_set_bit(કિંમત, ENVCFG_DTE_SHIFT);
}

સ્થિર પૂર્ણાંક fwft_get_double_trap(સંરચના fwft_config *conf, unsigned long *કિંમત)
{
	પાછા fwft_menvcfg_read_bit(કિંમત, ENVCFG_DTE_SHIFT);
}

સ્થિર પૂર્ણાંક fwft_adue_supported(સંરચના fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	જો (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		પાછા SBI_ENOTSUPP;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_set_adue(સંરચના fwft_config *conf, unsigned long કિંમત)
{
	પૂર્ણાંક res = fwft_menvcfg_set_bit(કિંમત, ENVCFG_ADUE_SHIFT);

	જો (res == SBI_OK) {
		__sbi_sfence_vma_all();

		જો (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	પાછા res;
}

સ્થિર પૂર્ણાંક fwft_get_adue(સંરચના fwft_config *conf, unsigned long *કિંમત)
{
	પાછા fwft_menvcfg_read_bit(કિંમત, ENVCFG_ADUE_SHIFT);
}

સ્થિર પૂર્ણાંક fwft_lpad_supported(સંરચના fwft_config *conf)
{
	જો (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		પાછા SBI_ENOTSUPP;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_enable_lpad(સંરચના fwft_config *conf, unsigned long કિંમત)
{
	પાછા fwft_menvcfg_set_bit(કિંમત, ENVCFG_LPE_SHIFT);
}

સ્થિર પૂર્ણાંક fwft_get_lpad(સંરચના fwft_config *conf, unsigned long *કિંમત)
{
	પાછા fwft_menvcfg_read_bit(કિંમત, ENVCFG_LPE_SHIFT);
}

સ્થિર પૂર્ણાંક fwft_sstack_supported(સંરચના fwft_config *conf)
{
	જો (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		પાછા SBI_ENOTSUPP;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_enable_sstack(સંરચના fwft_config *conf, unsigned long કિંમત)
{
	પાછા fwft_menvcfg_set_bit(કિંમત, ENVCFG_SSE_SHIFT);
}

સ્થિર પૂર્ણાંક fwft_get_sstack(સંરચના fwft_config *conf, unsigned long *કિંમત)
{
	પાછા fwft_menvcfg_read_bit(કિંમત, ENVCFG_SSE_SHIFT);
}

#જો __riscv_xlen > 32
સ્થિર પૂર્ણાંક fwft_pmlen_supported(સંરચના fwft_config *conf)
{
	જો (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		પાછા SBI_ENOTSUPP;

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_set_pmlen(સંરચના fwft_config *conf, unsigned long કિંમત)
{
	unsigned long pmm, prev;

	પસંદગી (કિંમત) {
	સ્થિતિ 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		તોડો;
	સ્થિતિ 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		તોડો;
	સ્થિતિ 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		તોડો;
	મૂળભૂત:
		પાછા SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	જો ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		પાછા SBI_EINVAL;
	}

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક fwft_get_pmlen(સંરચના fwft_config *conf, unsigned long *કિંમત)
{
	પસંદગી (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	સ્થિતિ ENVCFG_PMM_PMLEN_0:
		*કિંમત = 0;
		તોડો;
	સ્થિતિ ENVCFG_PMM_PMLEN_7:
		*કિંમત = 7;
		તોડો;
	સ્થિતિ ENVCFG_PMM_PMLEN_16:
		*કિંમત = 16;
		તોડો;
	મૂળભૂત:
		પાછા SBI_EFAIL;
	}

	પાછા SBI_OK;
}
#endif

સ્થિર સંરચના fwft_config* get_feature_config(ગણના sbi_fwft_feature_t feature)
{
	પૂર્ણાંક i;
	સંરચના fwft_hart_state *fhs = fwft_thishart_state_ptr();

	જો (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		પાછા ખાલી;

	માટે (i = 0; i < fhs->config_ગણતરી; i++){
		જો (feature == fhs->configs[i].feature->id)
			પાછા &fhs->configs[i];
	}

	પાછા ખાલી;
}

સ્થિર પૂર્ણાંક fwft_get_feature(ગણના sbi_fwft_feature_t feature,
			    સંરચના fwft_config **conf)
{
	પૂર્ણાંક ret;
	સંરચના fwft_config *tconf;

	tconf = get_feature_config(feature);
	જો (!tconf) {
		જો (fwft_is_defined_feature(feature))
			પાછા SBI_ENOTSUPP;

		પાછા SBI_EDENIED;
	}

	જો (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		જો (ret)
			પાછા ret;
	}
	*conf = tconf;

	પાછા SBI_SUCCESS;
}

સ્થિર શૂન્ય fwft_clear_config_lock(ગણના sbi_fwft_feature_t feature)
{
	પૂર્ણાંક ret;
	સંરચના fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	જો (ret)
		પાછા;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

પૂર્ણાંક sbi_fwft_set(ગણના sbi_fwft_feature_t feature, unsigned long કિંમત,
		 unsigned long flags)
{
	પૂર્ણાંક ret;
	સંરચના fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	જો (ret)
		પાછા ret;

	જો ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		પાછા SBI_EINVAL;

	જો (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		પાછા SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, કિંમત);
	જો (ret)
		પાછા ret;

	conf->flags = flags;

	પાછા SBI_OK;
}

પૂર્ણાંક sbi_fwft_get(ગણના sbi_fwft_feature_t feature, unsigned long *out_val)
{
	પૂર્ણાંક ret;
	સંરચના fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	જો (ret)
		પાછા ret;

	પાછા conf->feature->get(conf, out_val);
}

સ્થિર અચલ સંરચના fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.set = fwft_set_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.set = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_ઢગલો,
		.supported = fwft_sstack_supported,
		.set = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.set = fwft_set_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.set = fwft_set_adue,
		.get = fwft_get_adue,
	},
#જો __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

પૂર્ણાંક sbi_fwft_init(સંરચના sbi_scratch *scratch, bool cold_boot)
{
	પૂર્ણાંક i;
	સંરચના fwft_hart_state *fhs;

	જો (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(શૂન્ય *);
		જો (!fwft_ptr_offset)
			પાછા SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	જો (!fhs) {
		fhs = sbi_zalloc(કદ(*fhs) + array_size(features) * કદ(સંરચના fwft_config));
		જો (!fhs)
			પાછા SBI_ENOMEM;

		fhs->config_ગણતરી = array_size(features);
		માટે (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	માટે (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	પાછા 0;
}
