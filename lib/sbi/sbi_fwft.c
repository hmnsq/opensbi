/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hfence.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_tlb.h>
#ضمن <sbi/sbi_types.h>

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
ساكن unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), فراغ *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

بنية fwft_config;

بنية fwft_feature {
	تعداد sbi_fwft_feature_t id;
	صحيح (*supported)(بنية fwft_config *conf);
	صحيح (*set)(بنية fwft_config *conf, unsigned long درج);
	صحيح (*get)(بنية fwft_config *conf, unsigned long *درج);
};

بنية fwft_config {
	ثابت بنية fwft_feature *feature;
	unsigned long flags;
};

بنية fwft_hart_state {
	unsigned صحيح config_كدي;
	بنية fwft_config configs[];
};

ساكن ثابت unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_تر,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

ساكن bool fwft_is_defined_feature(تعداد sbi_fwft_feature_t feature)
{
	صحيح i;

	لكل (i = 0; i < array_size(fwft_defined_features); i++) {
		إذا (fwft_defined_features[i] == feature)
			أرجع true;
	}

	أرجع false;
}

ساكن صحيح fwft_menvcfg_set_bit(unsigned long درج, unsigned long bit)
{
	إذا (درج == 1) {
		إذا (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		وإلا
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} وإلا إذا (درج == 0) {
		إذا (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		وإلا
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} وإلا {
		أرجع SBI_EINVAL;
	}

	أرجع SBI_OK;
}

ساكن صحيح fwft_menvcfg_read_bit(unsigned long *درج, unsigned long bit)
{
	unsigned long cfg;

	إذا (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	وإلا
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*درج = cfg != 0;

	أرجع SBI_OK;
}

ساكن صحيح fwft_misaligned_delegation_supported(بنية fwft_config *conf)
{
	إذا (!misa_extension('S'))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_set_misaligned_delegation(بنية fwft_config *conf,
					 unsigned long درج)
{
	إذا (درج == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	وإلا إذا (درج == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	وإلا
		أرجع SBI_EINVAL;

	أرجع SBI_OK;
}

ساكن صحيح fwft_get_misaligned_delegation(بنية fwft_config *conf,
					 unsigned long *درج)
{
	*درج = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	أرجع SBI_OK;
}

ساكن صحيح fwft_double_trap_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_set_double_trap(بنية fwft_config *conf, unsigned long درج)
{
	أرجع fwft_menvcfg_set_bit(درج, ENVCFG_DTE_SHIFT);
}

ساكن صحيح fwft_get_double_trap(بنية fwft_config *conf, unsigned long *درج)
{
	أرجع fwft_menvcfg_read_bit(درج, ENVCFG_DTE_SHIFT);
}

ساكن صحيح fwft_adue_supported(بنية fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_set_adue(بنية fwft_config *conf, unsigned long درج)
{
	صحيح res = fwft_menvcfg_set_bit(درج, ENVCFG_ADUE_SHIFT);

	إذا (res == SBI_OK) {
		__sbi_sfence_vma_all();

		إذا (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	أرجع res;
}

ساكن صحيح fwft_get_adue(بنية fwft_config *conf, unsigned long *درج)
{
	أرجع fwft_menvcfg_read_bit(درج, ENVCFG_ADUE_SHIFT);
}

ساكن صحيح fwft_lpad_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_enable_lpad(بنية fwft_config *conf, unsigned long درج)
{
	أرجع fwft_menvcfg_set_bit(درج, ENVCFG_LPE_SHIFT);
}

ساكن صحيح fwft_get_lpad(بنية fwft_config *conf, unsigned long *درج)
{
	أرجع fwft_menvcfg_read_bit(درج, ENVCFG_LPE_SHIFT);
}

ساكن صحيح fwft_sstack_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_enable_sstack(بنية fwft_config *conf, unsigned long درج)
{
	أرجع fwft_menvcfg_set_bit(درج, ENVCFG_SSE_SHIFT);
}

ساكن صحيح fwft_get_sstack(بنية fwft_config *conf, unsigned long *درج)
{
	أرجع fwft_menvcfg_read_bit(درج, ENVCFG_SSE_SHIFT);
}

#إذا __riscv_xlen > 32
ساكن صحيح fwft_pmlen_supported(بنية fwft_config *conf)
{
	إذا (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		أرجع SBI_ENOTSUPP;

	أرجع SBI_OK;
}

ساكن صحيح fwft_set_pmlen(بنية fwft_config *conf, unsigned long درج)
{
	unsigned long pmm, prev;

	بدل (درج) {
	حالة 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		اكسر;
	حالة 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		اكسر;
	حالة 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		اكسر;
	افتراضي:
		أرجع SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	إذا ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		أرجع SBI_EINVAL;
	}

	أرجع SBI_OK;
}

ساكن صحيح fwft_get_pmlen(بنية fwft_config *conf, unsigned long *درج)
{
	بدل (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	حالة ENVCFG_PMM_PMLEN_0:
		*درج = 0;
		اكسر;
	حالة ENVCFG_PMM_PMLEN_7:
		*درج = 7;
		اكسر;
	حالة ENVCFG_PMM_PMLEN_16:
		*درج = 16;
		اكسر;
	افتراضي:
		أرجع SBI_EFAIL;
	}

	أرجع SBI_OK;
}
#endif

ساكن بنية fwft_config* get_feature_config(تعداد sbi_fwft_feature_t feature)
{
	صحيح i;
	بنية fwft_hart_state *fhs = fwft_thishart_state_ptr();

	إذا (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		أرجع عدم;

	لكل (i = 0; i < fhs->config_كدي; i++){
		إذا (feature == fhs->configs[i].feature->id)
			أرجع &fhs->configs[i];
	}

	أرجع عدم;
}

ساكن صحيح fwft_get_feature(تعداد sbi_fwft_feature_t feature,
			    بنية fwft_config **conf)
{
	صحيح ret;
	بنية fwft_config *tconf;

	tconf = get_feature_config(feature);
	إذا (!tconf) {
		إذا (fwft_is_defined_feature(feature))
			أرجع SBI_ENOTSUPP;

		أرجع SBI_EDENIED;
	}

	إذا (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		إذا (ret)
			أرجع ret;
	}
	*conf = tconf;

	أرجع SBI_SUCCESS;
}

ساكن فراغ fwft_clear_config_lock(تعداد sbi_fwft_feature_t feature)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

صحيح sbi_fwft_set(تعداد sbi_fwft_feature_t feature, unsigned long درج,
		 unsigned long flags)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع ret;

	إذا ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		أرجع SBI_EINVAL;

	إذا (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		أرجع SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, درج);
	إذا (ret)
		أرجع ret;

	conf->flags = flags;

	أرجع SBI_OK;
}

صحيح sbi_fwft_get(تعداد sbi_fwft_feature_t feature, unsigned long *out_val)
{
	صحيح ret;
	بنية fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	إذا (ret)
		أرجع ret;

	أرجع conf->feature->get(conf, out_val);
}

ساكن ثابت بنية fwft_feature features[] =
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
		.id = SBI_FWFT_SHADOW_تر,
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
#إذا __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

صحيح sbi_fwft_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح i;
	بنية fwft_hart_state *fhs;

	إذا (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(فراغ *);
		إذا (!fwft_ptr_offset)
			أرجع SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	إذا (!fhs) {
		fhs = sbi_zalloc(حجم(*fhs) + array_size(features) * حجم(بنية fwft_config));
		إذا (!fhs)
			أرجع SBI_ENOMEM;

		fhs->config_كدي = array_size(features);
		لكل (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	لكل (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	أرجع 0;
}
