/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_version.h>
#అన్తర్భావ <sbi/riscv_asm.h>

స్థిర పూర్ణాంకం sbi_ecall_పునాది_probe(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ *బహిర్గతం_val)
{
	నిర్మాణం sbi_ecall_extension *ext;

	ext = sbi_ecall_కనుగొను_extension(extid);
	ఒకవేళ (!ext) {
		*బహిర్గతం_val = 0;
		తిరిగి 0;
	}

	ఒకవేళ (ext->probe)
		తిరిగి ext->probe(extid, బహిర్గతం_val);

	*బహిర్గతం_val = 1;
	తిరిగి 0;
}

స్థిర పూర్ణాంకం sbi_ecall_పునాది_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_పునాది_GET_SPEC_సంస్కరణ:
		బహిర్గతం->విలువ = (SBI_ECALL_సంస్కరణ_MAJOR <<
			      SBI_SPEC_సంస్కరణ_MAJOR_స్థానభ్రంశం) &
			     (SBI_SPEC_సంస్కరణ_MAJOR_ముసుగు <<
			      SBI_SPEC_సంస్కరణ_MAJOR_స్థానభ్రంశం);
		బహిర్గతం->విలువ = బహిర్గతం->విలువ | SBI_ECALL_సంస్కరణ_MINOR;
		ఆపు;
	స్థితి SBI_EXT_పునాది_GET_IMP_ID:
		బహిర్గతం->విలువ = sbi_ecall_get_impid();
		ఆపు;
	స్థితి SBI_EXT_పునాది_GET_IMP_సంస్కరణ:
		బహిర్గతం->విలువ = OPENSBI_సంస్కరణ;
		ఆపు;
	స్థితి SBI_EXT_పునాది_GET_MVENDORID:
		బహిర్గతం->విలువ = csr_read(CSR_MVENDORID);
		ఆపు;
	స్థితి SBI_EXT_పునాది_GET_MARCHID:
		బహిర్గతం->విలువ = csr_read(CSR_MARCHID);
		ఆపు;
	స్థితి SBI_EXT_పునాది_GET_MIMPID:
		బహిర్గతం->విలువ = csr_read(CSR_MIMPID);
		ఆపు;
	స్థితి SBI_EXT_పునాది_PROBE_EXT:
		ret = sbi_ecall_పునాది_probe(regs->a0, &బహిర్గతం->విలువ);
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	}

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_పునాది;

స్థిర పూర్ణాంకం sbi_ecall_పునాది_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_పునాది);
}

నిర్మాణం sbi_ecall_extension ecall_పునాది = {
	.name			= "base",
	.extid_ప్రారంభం		= SBI_EXT_పునాది,
	.extid_చివర		= SBI_EXT_పునాది,
	.రిజిస్టరు_extensions	= sbi_ecall_పునాది_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_పునాది_handler,
};
