/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_bitmap.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_hfence.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_tlb.h>
#அந்தர்பாவ <sbi/sbi_types.h>

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
நிலையான unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), வெறுமை *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), வெறுமை *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

கட்டமைப்பு fwft_config;

கட்டமைப்பு fwft_feature {
	எண்ணிக்கை sbi_fwft_feature_t id;
	முழு (*supported)(கட்டமைப்பு fwft_config *conf);
	முழு (*set)(கட்டமைப்பு fwft_config *conf, unsigned long மதிப்பு);
	முழு (*get)(கட்டமைப்பு fwft_config *conf, unsigned long *மதிப்பு);
};

கட்டமைப்பு fwft_config {
	மாறிலி கட்டமைப்பு fwft_feature *feature;
	unsigned long flags;
};

கட்டமைப்பு fwft_hart_state {
	unsigned முழு config_மொத்தம்;
	கட்டமைப்பு fwft_config configs[];
};

நிலையான மாறிலி unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_அடுக்கு,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

நிலையான bool fwft_is_defined_feature(எண்ணிக்கை sbi_fwft_feature_t feature)
{
	முழு i;

	க்கு (i = 0; i < array_size(fwft_defined_features); i++) {
		எனில் (fwft_defined_features[i] == feature)
			திரும்பு true;
	}

	திரும்பு false;
}

நிலையான முழு fwft_menvcfg_set_bit(unsigned long மதிப்பு, unsigned long bit)
{
	எனில் (மதிப்பு == 1) {
		எனில் (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		இல்லையெனில்
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} இல்லையெனில் எனில் (மதிப்பு == 0) {
		எனில் (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		இல்லையெனில்
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} இல்லையெனில் {
		திரும்பு SBI_EINVAL;
	}

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_menvcfg_read_bit(unsigned long *மதிப்பு, unsigned long bit)
{
	unsigned long cfg;

	எனில் (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	இல்லையெனில்
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*மதிப்பு = cfg != 0;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_misaligned_delegation_supported(கட்டமைப்பு fwft_config *conf)
{
	எனில் (!misa_extension('S'))
		திரும்பு SBI_ENOTSUPP;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_set_misaligned_delegation(கட்டமைப்பு fwft_config *conf,
					 unsigned long மதிப்பு)
{
	எனில் (மதிப்பு == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	இல்லையெனில் எனில் (மதிப்பு == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	இல்லையெனில்
		திரும்பு SBI_EINVAL;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_get_misaligned_delegation(கட்டமைப்பு fwft_config *conf,
					 unsigned long *மதிப்பு)
{
	*மதிப்பு = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_double_trap_supported(கட்டமைப்பு fwft_config *conf)
{
	எனில் (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		திரும்பு SBI_ENOTSUPP;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_set_double_trap(கட்டமைப்பு fwft_config *conf, unsigned long மதிப்பு)
{
	திரும்பு fwft_menvcfg_set_bit(மதிப்பு, ENVCFG_DTE_SHIFT);
}

நிலையான முழு fwft_get_double_trap(கட்டமைப்பு fwft_config *conf, unsigned long *மதிப்பு)
{
	திரும்பு fwft_menvcfg_read_bit(மதிப்பு, ENVCFG_DTE_SHIFT);
}

நிலையான முழு fwft_adue_supported(கட்டமைப்பு fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	எனில் (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		திரும்பு SBI_ENOTSUPP;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_set_adue(கட்டமைப்பு fwft_config *conf, unsigned long மதிப்பு)
{
	முழு res = fwft_menvcfg_set_bit(மதிப்பு, ENVCFG_ADUE_SHIFT);

	எனில் (res == SBI_OK) {
		__sbi_sfence_vma_all();

		எனில் (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	திரும்பு res;
}

நிலையான முழு fwft_get_adue(கட்டமைப்பு fwft_config *conf, unsigned long *மதிப்பு)
{
	திரும்பு fwft_menvcfg_read_bit(மதிப்பு, ENVCFG_ADUE_SHIFT);
}

நிலையான முழு fwft_lpad_supported(கட்டமைப்பு fwft_config *conf)
{
	எனில் (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		திரும்பு SBI_ENOTSUPP;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_enable_lpad(கட்டமைப்பு fwft_config *conf, unsigned long மதிப்பு)
{
	திரும்பு fwft_menvcfg_set_bit(மதிப்பு, ENVCFG_LPE_SHIFT);
}

நிலையான முழு fwft_get_lpad(கட்டமைப்பு fwft_config *conf, unsigned long *மதிப்பு)
{
	திரும்பு fwft_menvcfg_read_bit(மதிப்பு, ENVCFG_LPE_SHIFT);
}

நிலையான முழு fwft_sstack_supported(கட்டமைப்பு fwft_config *conf)
{
	எனில் (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		திரும்பு SBI_ENOTSUPP;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_enable_sstack(கட்டமைப்பு fwft_config *conf, unsigned long மதிப்பு)
{
	திரும்பு fwft_menvcfg_set_bit(மதிப்பு, ENVCFG_SSE_SHIFT);
}

நிலையான முழு fwft_get_sstack(கட்டமைப்பு fwft_config *conf, unsigned long *மதிப்பு)
{
	திரும்பு fwft_menvcfg_read_bit(மதிப்பு, ENVCFG_SSE_SHIFT);
}

#எனில் __riscv_xlen > 32
நிலையான முழு fwft_pmlen_supported(கட்டமைப்பு fwft_config *conf)
{
	எனில் (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		திரும்பு SBI_ENOTSUPP;

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_set_pmlen(கட்டமைப்பு fwft_config *conf, unsigned long மதிப்பு)
{
	unsigned long pmm, prev;

	தேர்வு (மதிப்பு) {
	நிலை 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		நிறுத்து;
	நிலை 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		நிறுத்து;
	நிலை 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		நிறுத்து;
	இயல்பு:
		திரும்பு SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	எனில் ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		திரும்பு SBI_EINVAL;
	}

	திரும்பு SBI_OK;
}

நிலையான முழு fwft_get_pmlen(கட்டமைப்பு fwft_config *conf, unsigned long *மதிப்பு)
{
	தேர்வு (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	நிலை ENVCFG_PMM_PMLEN_0:
		*மதிப்பு = 0;
		நிறுத்து;
	நிலை ENVCFG_PMM_PMLEN_7:
		*மதிப்பு = 7;
		நிறுத்து;
	நிலை ENVCFG_PMM_PMLEN_16:
		*மதிப்பு = 16;
		நிறுத்து;
	இயல்பு:
		திரும்பு SBI_EFAIL;
	}

	திரும்பு SBI_OK;
}
#endif

நிலையான கட்டமைப்பு fwft_config* get_feature_config(எண்ணிக்கை sbi_fwft_feature_t feature)
{
	முழு i;
	கட்டமைப்பு fwft_hart_state *fhs = fwft_thishart_state_ptr();

	எனில் (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		திரும்பு வெற்று;

	க்கு (i = 0; i < fhs->config_மொத்தம்; i++){
		எனில் (feature == fhs->configs[i].feature->id)
			திரும்பு &fhs->configs[i];
	}

	திரும்பு வெற்று;
}

நிலையான முழு fwft_get_feature(எண்ணிக்கை sbi_fwft_feature_t feature,
			    கட்டமைப்பு fwft_config **conf)
{
	முழு ret;
	கட்டமைப்பு fwft_config *tconf;

	tconf = get_feature_config(feature);
	எனில் (!tconf) {
		எனில் (fwft_is_defined_feature(feature))
			திரும்பு SBI_ENOTSUPP;

		திரும்பு SBI_EDENIED;
	}

	எனில் (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		எனில் (ret)
			திரும்பு ret;
	}
	*conf = tconf;

	திரும்பு SBI_SUCCESS;
}

நிலையான வெறுமை fwft_clear_config_lock(எண்ணிக்கை sbi_fwft_feature_t feature)
{
	முழு ret;
	கட்டமைப்பு fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	எனில் (ret)
		திரும்பு;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

முழு sbi_fwft_set(எண்ணிக்கை sbi_fwft_feature_t feature, unsigned long மதிப்பு,
		 unsigned long flags)
{
	முழு ret;
	கட்டமைப்பு fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	எனில் (ret)
		திரும்பு ret;

	எனில் ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		திரும்பு SBI_EINVAL;

	எனில் (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		திரும்பு SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, மதிப்பு);
	எனில் (ret)
		திரும்பு ret;

	conf->flags = flags;

	திரும்பு SBI_OK;
}

முழு sbi_fwft_get(எண்ணிக்கை sbi_fwft_feature_t feature, unsigned long *out_val)
{
	முழு ret;
	கட்டமைப்பு fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	எனில் (ret)
		திரும்பு ret;

	திரும்பு conf->feature->get(conf, out_val);
}

நிலையான மாறிலி கட்டமைப்பு fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_அடுக்கு,
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
#எனில் __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

முழு sbi_fwft_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	முழு i;
	கட்டமைப்பு fwft_hart_state *fhs;

	எனில் (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(வெறுமை *);
		எனில் (!fwft_ptr_offset)
			திரும்பு SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	எனில் (!fhs) {
		fhs = sbi_zalloc(அளவு(*fhs) + array_size(features) * அளவு(கட்டமைப்பு fwft_config));
		எனில் (!fhs)
			திரும்பு SBI_ENOMEM;

		fhs->config_மொத்தம் = array_size(features);
		க்கு (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	க்கு (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	திரும்பு 0;
}
