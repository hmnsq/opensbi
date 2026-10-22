/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_bitmap.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hfence.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_tlb.h>
#شامل <sbi/sbi_types.h>

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
ایستا unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), تهی *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), تهی *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

ساختار fwft_config;

ساختار fwft_feature {
	شمارش sbi_fwft_feature_t id;
	صحیح (*supported)(ساختار fwft_config *conf);
	صحیح (*set)(ساختار fwft_config *conf, unsigned long مقدار);
	صحیح (*get)(ساختار fwft_config *conf, unsigned long *مقدار);
};

ساختار fwft_config {
	ثابت ساختار fwft_feature *feature;
	unsigned long flags;
};

ساختار fwft_hart_state {
	unsigned صحیح config_شمار;
	ساختار fwft_config configs[];
};

ایستا ثابت unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_پشته,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

ایستا bool fwft_is_defined_feature(شمارش sbi_fwft_feature_t feature)
{
	صحیح i;

	برای (i = 0; i < array_size(fwft_defined_features); i++) {
		اگر (fwft_defined_features[i] == feature)
			بازگشت true;
	}

	بازگشت false;
}

ایستا صحیح fwft_menvcfg_set_bit(unsigned long مقدار, unsigned long bit)
{
	اگر (مقدار == 1) {
		اگر (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		وگرنه
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} وگرنه اگر (مقدار == 0) {
		اگر (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		وگرنه
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} وگرنه {
		بازگشت SBI_EINVAL;
	}

	بازگشت SBI_OK;
}

ایستا صحیح fwft_menvcfg_read_bit(unsigned long *مقدار, unsigned long bit)
{
	unsigned long cfg;

	اگر (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	وگرنه
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*مقدار = cfg != 0;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_misaligned_delegation_supported(ساختار fwft_config *conf)
{
	اگر (!misa_extension('S'))
		بازگشت SBI_ENOTSUPP;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_set_misaligned_delegation(ساختار fwft_config *conf,
					 unsigned long مقدار)
{
	اگر (مقدار == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	وگرنه اگر (مقدار == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	وگرنه
		بازگشت SBI_EINVAL;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_get_misaligned_delegation(ساختار fwft_config *conf,
					 unsigned long *مقدار)
{
	*مقدار = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_double_trap_supported(ساختار fwft_config *conf)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		بازگشت SBI_ENOTSUPP;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_set_double_trap(ساختار fwft_config *conf, unsigned long مقدار)
{
	بازگشت fwft_menvcfg_set_bit(مقدار, ENVCFG_DTE_SHIFT);
}

ایستا صحیح fwft_get_double_trap(ساختار fwft_config *conf, unsigned long *مقدار)
{
	بازگشت fwft_menvcfg_read_bit(مقدار, ENVCFG_DTE_SHIFT);
}

ایستا صحیح fwft_adue_supported(ساختار fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		بازگشت SBI_ENOTSUPP;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_set_adue(ساختار fwft_config *conf, unsigned long مقدار)
{
	صحیح res = fwft_menvcfg_set_bit(مقدار, ENVCFG_ADUE_SHIFT);

	اگر (res == SBI_OK) {
		__sbi_sfence_vma_all();

		اگر (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	بازگشت res;
}

ایستا صحیح fwft_get_adue(ساختار fwft_config *conf, unsigned long *مقدار)
{
	بازگشت fwft_menvcfg_read_bit(مقدار, ENVCFG_ADUE_SHIFT);
}

ایستا صحیح fwft_lpad_supported(ساختار fwft_config *conf)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		بازگشت SBI_ENOTSUPP;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_enable_lpad(ساختار fwft_config *conf, unsigned long مقدار)
{
	بازگشت fwft_menvcfg_set_bit(مقدار, ENVCFG_LPE_SHIFT);
}

ایستا صحیح fwft_get_lpad(ساختار fwft_config *conf, unsigned long *مقدار)
{
	بازگشت fwft_menvcfg_read_bit(مقدار, ENVCFG_LPE_SHIFT);
}

ایستا صحیح fwft_sstack_supported(ساختار fwft_config *conf)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		بازگشت SBI_ENOTSUPP;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_enable_sstack(ساختار fwft_config *conf, unsigned long مقدار)
{
	بازگشت fwft_menvcfg_set_bit(مقدار, ENVCFG_SSE_SHIFT);
}

ایستا صحیح fwft_get_sstack(ساختار fwft_config *conf, unsigned long *مقدار)
{
	بازگشت fwft_menvcfg_read_bit(مقدار, ENVCFG_SSE_SHIFT);
}

#اگر __riscv_xlen > 32
ایستا صحیح fwft_pmlen_supported(ساختار fwft_config *conf)
{
	اگر (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		بازگشت SBI_ENOTSUPP;

	بازگشت SBI_OK;
}

ایستا صحیح fwft_set_pmlen(ساختار fwft_config *conf, unsigned long مقدار)
{
	unsigned long pmm, prev;

	انتخاب (مقدار) {
	حالت 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		بشکن;
	حالت 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		بشکن;
	حالت 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		بشکن;
	پیشفرض:
		بازگشت SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	اگر ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		بازگشت SBI_EINVAL;
	}

	بازگشت SBI_OK;
}

ایستا صحیح fwft_get_pmlen(ساختار fwft_config *conf, unsigned long *مقدار)
{
	انتخاب (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	حالت ENVCFG_PMM_PMLEN_0:
		*مقدار = 0;
		بشکن;
	حالت ENVCFG_PMM_PMLEN_7:
		*مقدار = 7;
		بشکن;
	حالت ENVCFG_PMM_PMLEN_16:
		*مقدار = 16;
		بشکن;
	پیشفرض:
		بازگشت SBI_EFAIL;
	}

	بازگشت SBI_OK;
}
#endif

ایستا ساختار fwft_config* get_feature_config(شمارش sbi_fwft_feature_t feature)
{
	صحیح i;
	ساختار fwft_hart_state *fhs = fwft_thishart_state_ptr();

	اگر (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		بازگشت پوچ;

	برای (i = 0; i < fhs->config_شمار; i++){
		اگر (feature == fhs->configs[i].feature->id)
			بازگشت &fhs->configs[i];
	}

	بازگشت پوچ;
}

ایستا صحیح fwft_get_feature(شمارش sbi_fwft_feature_t feature,
			    ساختار fwft_config **conf)
{
	صحیح ret;
	ساختار fwft_config *tconf;

	tconf = get_feature_config(feature);
	اگر (!tconf) {
		اگر (fwft_is_defined_feature(feature))
			بازگشت SBI_ENOTSUPP;

		بازگشت SBI_EDENIED;
	}

	اگر (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		اگر (ret)
			بازگشت ret;
	}
	*conf = tconf;

	بازگشت SBI_SUCCESS;
}

ایستا تهی fwft_clear_config_lock(شمارش sbi_fwft_feature_t feature)
{
	صحیح ret;
	ساختار fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	اگر (ret)
		بازگشت;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

صحیح sbi_fwft_set(شمارش sbi_fwft_feature_t feature, unsigned long مقدار,
		 unsigned long flags)
{
	صحیح ret;
	ساختار fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	اگر (ret)
		بازگشت ret;

	اگر ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		بازگشت SBI_EINVAL;

	اگر (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		بازگشت SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, مقدار);
	اگر (ret)
		بازگشت ret;

	conf->flags = flags;

	بازگشت SBI_OK;
}

صحیح sbi_fwft_get(شمارش sbi_fwft_feature_t feature, unsigned long *out_val)
{
	صحیح ret;
	ساختار fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	اگر (ret)
		بازگشت ret;

	بازگشت conf->feature->get(conf, out_val);
}

ایستا ثابت ساختار fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_پشته,
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
#اگر __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

صحیح sbi_fwft_init(ساختار sbi_scratch *scratch, bool cold_boot)
{
	صحیح i;
	ساختار fwft_hart_state *fhs;

	اگر (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(تهی *);
		اگر (!fwft_ptr_offset)
			بازگشت SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	اگر (!fhs) {
		fhs = sbi_zalloc(اندازه(*fhs) + array_size(features) * اندازه(ساختار fwft_config));
		اگر (!fhs)
			بازگشت SBI_ENOMEM;

		fhs->config_شمار = array_size(features);
		برای (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	برای (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	بازگشت 0;
}
