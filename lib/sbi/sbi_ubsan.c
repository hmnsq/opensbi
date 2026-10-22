/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#నిర్వచితమైతే UBSAN_ENABLED

#అన్తర్భావ <sbi/sbi_types.h>
#అన్తర్భావ <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#నిర్వచించు రకం_INTEGER 0
#నిర్వచించు రకం_FLOAT 1
#నిర్వచించు రకం_UNKNOWN UINT16_MAX

అస్థిర గుర్తులేని దీర్ఘ sbi_ubsan_report_లెక్క;

నిర్మాణం CSourceLocation {
	అక్షరం *mFilename;
	uint32_t m_పంక్తి;
	uint32_t m_స్తంభం;
};

నిర్మాణం CTypeDescriptor {
	uint16_t m_Type_రకం;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

నిర్మాణం COverflow_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mType;
};

నిర్మాణం CUnreachable_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
};

నిర్మాణం CCFICheck_Fail_దత్తాంశం {
	uint8_t m_Check_రకం;
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mType;
};

నిర్మాణం CDynamic_Type_Cache_తప్పిపోవు_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mType;
	శూన్య *mTypeInfo;
	uint8_t m_Type_Check_రకం;
};

నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mType;
};

నిర్మాణం CInvalid_Builtin_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	uint8_t m_రకం;
};

నిర్మాణం CInvalid_విలువ_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mType;
};

నిర్మాణం CNon_Null_Arg_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CSourceLocation mAttributeLocation;
	పూర్ణాంకం m_Arg_సూచీ;
};

నిర్మాణం CNon_Null_Return_దత్తాంశం {
	నిర్మాణం CSourceLocation mAttributeLocation;
};

నిర్మాణం COut_యొక్క_Bounds_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mArrayType;
	నిర్మాణం CTypeDescriptor *m_సూచీ_Type;
};

నిర్మాణం CPointer_అధికప్రవాహం_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
};

నిర్మాణం CShift_బహిర్గతం_యొక్క_Bounds_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mLHSType;
	నిర్మాణం CTypeDescriptor *mRHSType;
};

నిర్మాణం CType_Mismatch_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mType;
	గుర్తులేని దీర్ఘ mLogAlignment;
	uint8_t m_Type_Check_రకం;
};

నిర్మాణం CType_Mismatch_దత్తాంశం_v1 {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_రకం;
};

నిర్మాణం CVLABound_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mType;
};

నిర్మాణం CFloat_Cast_అధికప్రవాహం_దత్తాంశం {
	నిర్మాణం CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	నిర్మాణం CTypeDescriptor *mFromType;
	నిర్మాణం CTypeDescriptor *m_గమ్యం_Type;
};

నిర్మాణం CImplicit_Conversion_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CTypeDescriptor *mFromType;
	నిర్మాణం CTypeDescriptor *m_గమ్యం_Type;
	uint8_t m_రకం;
};

నిర్మాణం CAlignment_Assumption_దత్తాంశం {
	నిర్మాణం CSourceLocation mLocation;
	నిర్మాణం CSourceLocation mAssumptionLocation;
	నిర్మాణం CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
శూన్య __ubsan_handle_add_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				 గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_add_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulLHS,
				       గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_alignment_assumption(నిర్మాణం CAlignment_Assumption_దత్తాంశం *p_దత్తాంశం,
					 గుర్తులేని దీర్ఘ ul_సూచిక,
					 గుర్తులేని దీర్ఘ ulAlignment,
					 గుర్తులేని దీర్ఘ ul_స్థానభ్రంశం);
శూన్య __ubsan_handle_alignment_assumption_abort(
	నిర్మాణం CAlignment_Assumption_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ul_సూచిక,
	గుర్తులేని దీర్ఘ ulAlignment, గుర్తులేని దీర్ఘ ul_స్థానభ్రంశం);
శూన్య __ubsan_handle_builtin_unreachable(నిర్మాణం CUnreachable_దత్తాంశం *p_దత్తాంశం);
శూన్య __ubsan_handle_divrem_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				    గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_divrem_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
					  గుర్తులేని దీర్ఘ ulLHS,
					  గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_function_type_mismatch(
	నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ulFunction);
శూన్య __ubsan_handle_function_type_mismatch_abort(
	నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ulFunction);
శూన్య __ubsan_handle_function_type_mismatch_v1(
	నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ulFunction,
	గుర్తులేని దీర్ఘ ulCalleeRTTI, గుర్తులేని దీర్ఘ ulFnRTTI);
శూన్య __ubsan_handle_function_type_mismatch_v1_abort(
	నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ulFunction,
	గుర్తులేని దీర్ఘ ulCalleeRTTI, గుర్తులేని దీర్ఘ ulFnRTTI);
శూన్య __ubsan_handle_invalid_builtin(నిర్మాణం CInvalid_Builtin_దత్తాంశం *p_దత్తాంశం);
శూన్య __ubsan_handle_invalid_builtin_abort(నిర్మాణం CInvalid_Builtin_దత్తాంశం *p_దత్తాంశం);
శూన్య __ubsan_handle_load_invalid_value(నిర్మాణం CInvalid_విలువ_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulVal);
శూన్య __ubsan_handle_load_invalid_value_abort(నిర్మాణం CInvalid_విలువ_దత్తాంశం *p_దత్తాంశం,
					     గుర్తులేని దీర్ఘ ulVal);
శూన్య __ubsan_handle_missing_return(నిర్మాణం CUnreachable_దత్తాంశం *p_దత్తాంశం);
శూన్య __ubsan_handle_mul_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				 గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_mul_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulLHS,
				       గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_negate_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				    గుర్తులేని దీర్ఘ ulOldVal);
శూన్య __ubsan_handle_negate_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
					  గుర్తులేని దీర్ఘ ulOldVal);
శూన్య __ubsan_handle_nullability_arg(నిర్మాణం CNon_Null_Arg_దత్తాంశం *p_దత్తాంశం);
శూన్య __ubsan_handle_nullability_arg_abort(నిర్మాణం CNon_Null_Arg_దత్తాంశం *p_దత్తాంశం);
శూన్య __ubsan_handle_nullability_return_v1(
	నిర్మాణం CNon_Null_Return_దత్తాంశం *p_దత్తాంశం,
	నిర్మాణం CSourceLocation *p_Location_సూచిక);
శూన్య __ubsan_handle_nullability_return_v1_abort(
	నిర్మాణం CNon_Null_Return_దత్తాంశం *p_దత్తాంశం,
	నిర్మాణం CSourceLocation *p_Location_సూచిక);
శూన్య __ubsan_handle_out_of_bounds(నిర్మాణం COut_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
				  గుర్తులేని దీర్ఘ ul_సూచీ);
శూన్య __ubsan_handle_out_of_bounds_abort(నిర్మాణం COut_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
					గుర్తులేని దీర్ఘ ul_సూచీ);
శూన్య __ubsan_handle_pointer_overflow(నిర్మాణం CPointer_అధికప్రవాహం_దత్తాంశం *p_దత్తాంశం,
				     గుర్తులేని దీర్ఘ ul_పునాది,
				     గుర్తులేని దీర్ఘ ul_ఫలితం);
శూన్య __ubsan_handle_pointer_overflow_abort(నిర్మాణం CPointer_అధికప్రవాహం_దత్తాంశం *p_దత్తాంశం,
					   గుర్తులేని దీర్ఘ ul_పునాది,
					   గుర్తులేని దీర్ఘ ul_ఫలితం);
శూన్య __ubsan_handle_shift_out_of_bounds(నిర్మాణం CShift_బహిర్గతం_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
					గుర్తులేని దీర్ఘ ulLHS,
					గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_shift_out_of_bounds_abort(
	నిర్మాణం CShift_బహిర్గతం_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ulLHS,
	గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_sub_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				 గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_sub_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulLHS,
				       గుర్తులేని దీర్ఘ ulRHS);
శూన్య __ubsan_handle_type_mismatch(నిర్మాణం CType_Mismatch_దత్తాంశం *p_దత్తాంశం,
				  గుర్తులేని దీర్ఘ ul_సూచిక);
శూన్య __ubsan_handle_type_mismatch_abort(నిర్మాణం CType_Mismatch_దత్తాంశం *p_దత్తాంశం,
					గుర్తులేని దీర్ఘ ul_సూచిక);
శూన్య __ubsan_handle_type_mismatch_v1(నిర్మాణం CType_Mismatch_దత్తాంశం_v1 *p_దత్తాంశం,
				     గుర్తులేని దీర్ఘ ul_సూచిక);
శూన్య __ubsan_handle_type_mismatch_v1_abort(నిర్మాణం CType_Mismatch_దత్తాంశం_v1 *p_దత్తాంశం,
					   గుర్తులేని దీర్ఘ ul_సూచిక);
శూన్య __ubsan_handle_vla_bound_not_positive(నిర్మాణం CVLABound_దత్తాంశం *p_దత్తాంశం,
					   గుర్తులేని దీర్ఘ ulBound);
శూన్య __ubsan_handle_vla_bound_not_positive_abort(నిర్మాణం CVLABound_దత్తాంశం *p_దత్తాంశం,
						 గుర్తులేని దీర్ఘ ulBound);
శూన్య __ubsan_get_current_report_data(స్థిరరాశి అక్షరం **pp_బహిర్గతం_Issue_రకం,
				     స్థిరరాశి అక్షరం **pp_బహిర్గతం_సందేశం,
				     స్థిరరాశి అక్షరం **pp_బహిర్గతం_Filename,
				     uint32_t *p_బహిర్గతం_పంక్తి, uint32_t *p_బహిర్గతం_నిలువు,
				     అక్షరం **pp_బహిర్గతం_జ్ఞాపకం_Addr);
స్థిర శూన్య Handle_అధికప్రవాహం(bool isFatal, నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
			   గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS,
			   స్థిరరాశి అక్షరం *szOperation);
స్థిర శూన్య Handle_Negate_అధికప్రవాహం(bool isFatal, నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				 గుర్తులేని దీర్ఘ ul_Old_విలువ);
స్థిర శూన్య HandleBuiltinUnreachable(bool isFatal,
				     నిర్మాణం CUnreachable_దత్తాంశం *p_దత్తాంశం);
స్థిర శూన్య HandleTypeMismatch(bool isFatal, నిర్మాణం CSourceLocation *mLocation,
			       నిర్మాణం CTypeDescriptor *mType,
			       గుర్తులేని దీర్ఘ mLogAlignment,
			       uint8_t m_Type_Check_రకం, గుర్తులేని దీర్ఘ ul_సూచిక);
స్థిర శూన్య Handle_Vla_Bound_Not_ధనాత్మక(bool isFatal, నిర్మాణం CVLABound_దత్తాంశం *p_దత్తాంశం,
				      గుర్తులేని దీర్ఘ ulBound);
స్థిర శూన్య Handle_బహిర్గతం_యొక్క_Bounds(bool isFatal, నిర్మాణం COut_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
			      గుర్తులేని దీర్ఘ ul_సూచీ);
స్థిర శూన్య Handle_జరుపు_బహిర్గతం_యొక్క_Bounds(bool isFatal,
				   నిర్మాణం CShift_బహిర్గతం_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
				   గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS);
స్థిర శూన్య Handle_ఎక్కించు_Invalid_విలువ(bool isFatal,
				   నిర్మాణం CInvalid_విలువ_దత్తాంశం *p_దత్తాంశం,
				   గుర్తులేని దీర్ఘ ul_విలువ);
స్థిర శూన్య HandleInvalidBuiltin(bool isFatal,
				 నిర్మాణం CInvalid_Builtin_దత్తాంశం *p_దత్తాంశం);
స్థిర శూన్య HandleFunctionTypeMismatch(bool isFatal,
				       నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulFunction);
స్థిర శూన్య HandleMissingReturn(bool isFatal, నిర్మాణం CUnreachable_దత్తాంశం *p_దత్తాంశం);
స్థిర శూన్య Handle_సూచిక_అధికప్రవాహం(bool isFatal,
				  నిర్మాణం CPointer_అధికప్రవాహం_దత్తాంశం *p_దత్తాంశం,
				  గుర్తులేని దీర్ఘ ul_పునాది, గుర్తులేని దీర్ఘ ul_ఫలితం);
స్థిర శూన్య HandleAlignmentAssumption(bool isFatal,
				      నిర్మాణం CAlignment_Assumption_దత్తాంశం *p_దత్తాంశం,
				      గుర్తులేని దీర్ఘ ul_సూచిక,
				      గుర్తులేని దీర్ఘ ulAlignment,
				      గుర్తులేని దీర్ఘ ul_స్థానభ్రంశం);

#నిర్వచించు సంఖ్య_SIGNED_బిట్టు 1
#నిర్వచించు సంఖ్య_MAXLEN 128
#నిర్వచించు __arraycount(__a) (పరిమాణం(__a) / పరిమాణం(__a[0]))
#నిర్వచించు __BIT(__n) (1UL << (__n))
#నిర్వచించు SEPARATOR sbi_printf("===========================================\n")
#నిర్వచించు ACK_REPORTED (1U << 31)

స్థిర bool isAlreadyReported(నిర్మాణం CSourceLocation *pLocation)
{
	uint32_t si_Old_విలువ;
	అస్థిర uint32_t *p_పంక్తి;

	ఒకవేళ (!pLocation)
		తిరిగి false;

	p_పంక్తి = &pLocation->m_పంక్తి;

	చేయి {
		si_Old_విలువ = *p_పంక్తి;
	} అయితే (__sync_val_compare_and_swap(p_పంక్తి, si_Old_విలువ,
					     si_Old_విలువ | ACK_REPORTED) !=
		 si_Old_విలువ);

	ఒకవేళ (!(si_Old_విలువ & ACK_REPORTED)) {

		sbi_ubsan_report_లెక్క++;

		తిరిగి false;
	}

	తిరిగి true;
}

స్థిర శూన్య Handle_అధికప్రవాహం(bool isFatal, నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
			   గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS,
			   స్థిరరాశి అక్షరం *szOperation)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	bool is_signed = p_దత్తాంశం->mType->mTypeInfo & సంఖ్య_SIGNED_బిట్టు;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	ఒకవేళ (is_signed) {
		sbi_printf("%ld %s %ld ", (దీర్ఘ)ulLHS, szOperation,
			   (దీర్ఘ)ulRHS);
	} లేకపోతే {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_దత్తాంశం->mType->mTypeName);

	SEPARATOR;
}

స్థిర శూన్య Handle_Negate_అధికప్రవాహం(bool isFatal, నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				 గుర్తులేని దీర్ఘ ul_Old_విలువ)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}
	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	bool is_signed = p_దత్తాంశం->mType->mTypeInfo & సంఖ్య_SIGNED_బిట్టు;

	sbi_printf("UBSan: Negation of ");

	ఒకవేళ (is_signed) {
		sbi_printf("%ld", (దీర్ఘ)ul_Old_విలువ);
	} లేకపోతే {
		sbi_printf("%lu", ul_Old_విలువ);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_దత్తాంశం->mType->mTypeName);

	SEPARATOR;
}

స్థిర శూన్య HandleBuiltinUnreachable(bool isFatal,
				     నిర్మాణం CUnreachable_దత్తాంశం *p_దత్తాంశం)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}
	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	SEPARATOR;
}

స్థిరరాశి అక్షరం *rgczTypeCheckKinds[] = { "load of",
				     "store to",
				     "reference binding to",
				     "member access within",
				     "member call on",
				     "constructor call on",
				     "downcast of",
				     "downcast of",
				     "upcast of",
				     "cast to virtual base of",
				     "_Nonnull binding to",
				     "dynamic operation on" };

స్థిర శూన్య HandleTypeMismatch(bool isFatal, నిర్మాణం CSourceLocation *mLocation,
			       నిర్మాణం CTypeDescriptor *mType,
			       గుర్తులేని దీర్ఘ mLogAlignment,
			       uint8_t m_Type_Check_రకం, గుర్తులేని దీర్ఘ ul_సూచిక)
{

	ఒకవేళ ((!mLocation) || (!mType)) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_పంక్తి & ~ACK_REPORTED,
		   mLocation->m_స్తంభం);

	స్థిరరాశి అక్షరం *రకం_2 = (m_Type_Check_రకం < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_రకం]
				   : "access to";

	ఒకవేళ (ul_సూచిక == 0) {
		sbi_printf("%s null pointer of type %s\n", రకం_2,
			   mType->mTypeName);
	} లేకపోతే ఒకవేళ (
		(mLogAlignment - 1) &
		ul_సూచిక) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			రకం_2, (శూన్య *)ul_సూచిక, mType->mTypeName,
			mLogAlignment);
	} లేకపోతే {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			రకం_2, (శూన్య *)ul_సూచిక, mType->mTypeName);
	}
	SEPARATOR;
}

స్థిర శూన్య Handle_Vla_Bound_Not_ధనాత్మక(bool isFatal, నిర్మాణం CVLABound_దత్తాంశం *p_దత్తాంశం,
				      గుర్తులేని దీర్ఘ ulBound)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}
	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_దత్తాంశం->mType->mTypeInfo & సంఖ్య_SIGNED_బిట్టు;

	ఒకవేళ (is_signed) {
		sbi_printf("%ld", (దీర్ఘ)ulBound);
	} లేకపోతే {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

స్థిర శూన్య Handle_బహిర్గతం_యొక్క_Bounds(bool isFatal, నిర్మాణం COut_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
			      గుర్తులేని దీర్ఘ ul_సూచీ)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}
	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	bool is_signed = p_దత్తాంశం->m_సూచీ_Type->mTypeInfo & సంఖ్య_SIGNED_బిట్టు;

	ఒకవేళ (is_signed) {
		sbi_printf("index %ld", (దీర్ఘ)ul_సూచీ);
	} లేకపోతే {
		sbi_printf("index %lu", ul_సూచీ);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_దత్తాంశం->mArrayType->mTypeName);

	SEPARATOR;
}

స్థిర bool is_Negative_సంఖ్య(నిర్మాణం CTypeDescriptor *pType, గుర్తులేని దీర్ఘ ulVal)
{
	ఒకవేళ (!(pType->mTypeInfo & సంఖ్య_SIGNED_బిట్టు)) {
		తిరిగి false;
	}

	తిరిగి (దీర్ఘ)ulVal < 0;
}

స్థిర size_t type_వెడల్పు(నిర్మాణం CTypeDescriptor *pType)
{
	తిరిగి 1UL << (pType->mTypeInfo >> 1);
}

స్థిర శూన్య Handle_జరుపు_బహిర్గతం_యొక్క_Bounds(bool isFatal,
				   నిర్మాణం CShift_బహిర్గతం_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
				   గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	ఒకవేళ (is_Negative_సంఖ్య(p_దత్తాంశం->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (దీర్ఘ)ulRHS);
	} లేకపోతే ఒకవేళ (ulRHS >= type_వెడల్పు(p_దత్తాంశం->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (గుర్తులేని దీర్ఘ)type_వెడల్పు(p_దత్తాంశం->mLHSType),
			p_దత్తాంశం->mLHSType->mTypeName);

	} లేకపోతే ఒకవేళ (is_Negative_సంఖ్య(p_దత్తాంశం->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (దీర్ఘ)ulLHS);
	} లేకపోతే {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_దత్తాంశం->mLHSType->mTypeName);
	}

	SEPARATOR;
}

స్థిర శూన్య Handle_ఎక్కించు_Invalid_విలువ(bool isFatal,
				   నిర్మాణం CInvalid_విలువ_దత్తాంశం *p_దత్తాంశం,
				   గుర్తులేని దీర్ఘ ul_విలువ)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	bool is_signed = p_దత్తాంశం->mType->mTypeInfo & సంఖ్య_SIGNED_బిట్టు;

	sbi_printf("load of value ");

	ఒకవేళ (is_signed) {
		sbi_printf("%ld ", (దీర్ఘ)ul_విలువ);
	} లేకపోతే {
		sbi_printf("%lu ", ul_విలువ);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_దత్తాంశం->mType->mTypeName);

	SEPARATOR;
}

స్థిరరాశి అక్షరం *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

స్థిర శూన్య HandleInvalidBuiltin(bool isFatal,
				 నిర్మాణం CInvalid_Builtin_దత్తాంశం *p_దత్తాంశం)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	స్థిరరాశి అక్షరం *builtin =
		(p_దత్తాంశం->m_రకం < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_దత్తాంశం->m_రకం]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

స్థిర శూన్య HandleFunctionTypeMismatch(bool isFatal,
				       నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulFunction)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_దత్తాంశం->mType->mTypeName);

	SEPARATOR;
}

స్థిర శూన్య HandleMissingReturn(bool isFatal, నిర్మాణం CUnreachable_దత్తాంశం *p_దత్తాంశం)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

స్థిర శూన్య Handle_సూచిక_అధికప్రవాహం(bool isFatal,
				  నిర్మాణం CPointer_అధికప్రవాహం_దత్తాంశం *p_దత్తాంశం,
				  గుర్తులేని దీర్ఘ ul_పునాది, గుర్తులేని దీర్ఘ ul_ఫలితం)
{
	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_పునాది, ul_ఫలితం);

	SEPARATOR;
}

స్థిర శూన్య HandleAlignmentAssumption(bool isFatal,
				      నిర్మాణం CAlignment_Assumption_దత్తాంశం *p_దత్తాంశం,
				      గుర్తులేని దీర్ఘ ul_సూచిక,
				      గుర్తులేని దీర్ఘ ulAlignment,
				      గుర్తులేని దీర్ఘ ul_స్థానభ్రంశం)
{

	ఒకవేళ (!p_దత్తాంశం) {
		తిరిగి;
	}

	ఒకవేళ (isAlreadyReported(&p_దత్తాంశం->mLocation)) {
		తిరిగి;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_దత్తాంశం->mLocation.mFilename,
		   p_దత్తాంశం->mLocation.m_పంక్తి & ~ACK_REPORTED,
		   p_దత్తాంశం->mLocation.m_స్తంభం);

	గుర్తులేని దీర్ఘ ul_Real_సూచిక = ul_సూచిక - ul_స్థానభ్రంశం;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (శూన్య *)ul_Real_సూచిక, (శూన్య *)ul_స్థానభ్రంశం);

	ఒకవేళ (p_దత్తాంశం->mAssumptionLocation.mFilename != ఖాళీ) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_దత్తాంశం->mAssumptionLocation.mFilename,
			   p_దత్తాంశం->mAssumptionLocation.m_పంక్తి,
			   p_దత్తాంశం->mAssumptionLocation.m_స్తంభం);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
శూన్య __ubsan_handle_add_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				 గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_అధికప్రవాహం(false, p_దత్తాంశం, ulLHS, ulRHS, "+");
}

శూన్య __ubsan_handle_add_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_అధికప్రవాహం(true, p_దత్తాంశం, ulLHS, ulRHS, "+");
}

శూన్య __ubsan_handle_sub_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				 గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_అధికప్రవాహం(false, p_దత్తాంశం, ulLHS, ulRHS, "-");
}

శూన్య __ubsan_handle_sub_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_అధికప్రవాహం(true, p_దత్తాంశం, ulLHS, ulRHS, "-");
}

శూన్య __ubsan_handle_mul_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				 గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_అధికప్రవాహం(false, p_దత్తాంశం, ulLHS, ulRHS, "*");
}

శూన్య __ubsan_handle_mul_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_అధికప్రవాహం(true, p_దత్తాంశం, ulLHS, ulRHS, "*");
}

శూన్య __ubsan_handle_negate_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				    గుర్తులేని దీర్ఘ ul_Old_విలువ)
{
	Handle_Negate_అధికప్రవాహం(false, p_దత్తాంశం, ul_Old_విలువ);
}

శూన్య __ubsan_handle_negate_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
					  గుర్తులేని దీర్ఘ ul_Old_విలువ)
{
	Handle_Negate_అధికప్రవాహం(true, p_దత్తాంశం, ul_Old_విలువ);
}

శూన్య __ubsan_handle_divrem_overflow(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
				    గుర్తులేని దీర్ఘ ulLHS, గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_అధికప్రవాహం(false, p_దత్తాంశం, ulLHS, ulRHS, "divrem");
}

శూన్య __ubsan_handle_divrem_overflow_abort(నిర్మాణం COverflow_దత్తాంశం *p_దత్తాంశం,
					  గుర్తులేని దీర్ఘ ulLHS,
					  గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_అధికప్రవాహం(true, p_దత్తాంశం, ulLHS, ulRHS, "divrem");
}

శూన్య __ubsan_handle_type_mismatch_v1(నిర్మాణం CType_Mismatch_దత్తాంశం_v1 *p_దత్తాంశం,
				     గుర్తులేని దీర్ఘ ul_సూచిక)
{
	HandleTypeMismatch(false, &p_దత్తాంశం->mLocation, p_దత్తాంశం->mType,
			   __BIT(p_దత్తాంశం->mLogAlignment), p_దత్తాంశం->m_Type_Check_రకం,
			   ul_సూచిక);
}

శూన్య __ubsan_handle_type_mismatch_v1_abort(నిర్మాణం CType_Mismatch_దత్తాంశం_v1 *p_దత్తాంశం,
					   గుర్తులేని దీర్ఘ ul_సూచిక)
{
	HandleTypeMismatch(true, &p_దత్తాంశం->mLocation, p_దత్తాంశం->mType,
			   __BIT(p_దత్తాంశం->mLogAlignment), p_దత్తాంశం->m_Type_Check_రకం,
			   ul_సూచిక);
}

శూన్య __ubsan_handle_out_of_bounds(నిర్మాణం COut_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
				  గుర్తులేని దీర్ఘ ul_సూచీ)
{
	Handle_బహిర్గతం_యొక్క_Bounds(false, p_దత్తాంశం, ul_సూచీ);
}

శూన్య __ubsan_handle_out_of_bounds_abort(నిర్మాణం COut_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
					గుర్తులేని దీర్ఘ ul_సూచీ)
{
	Handle_బహిర్గతం_యొక్క_Bounds(true, p_దత్తాంశం, ul_సూచీ);
}

శూన్య __ubsan_handle_shift_out_of_bounds(నిర్మాణం CShift_బహిర్గతం_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం,
					గుర్తులేని దీర్ఘ ulLHS,
					గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_జరుపు_బహిర్గతం_యొక్క_Bounds(false, p_దత్తాంశం, ulLHS, ulRHS);
}

శూన్య __ubsan_handle_shift_out_of_bounds_abort(
	నిర్మాణం CShift_బహిర్గతం_యొక్క_Bounds_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ulLHS,
	గుర్తులేని దీర్ఘ ulRHS)
{
	Handle_జరుపు_బహిర్గతం_యొక్క_Bounds(true, p_దత్తాంశం, ulLHS, ulRHS);
}

శూన్య __ubsan_handle_pointer_overflow(నిర్మాణం CPointer_అధికప్రవాహం_దత్తాంశం *p_దత్తాంశం,
				     గుర్తులేని దీర్ఘ ul_పునాది,
				     గుర్తులేని దీర్ఘ ul_ఫలితం)
{
	Handle_సూచిక_అధికప్రవాహం(false, p_దత్తాంశం, ul_పునాది, ul_ఫలితం);
}

శూన్య __ubsan_handle_pointer_overflow_abort(నిర్మాణం CPointer_అధికప్రవాహం_దత్తాంశం *p_దత్తాంశం,
					   గుర్తులేని దీర్ఘ ul_పునాది,
					   గుర్తులేని దీర్ఘ ul_ఫలితం)
{
	Handle_సూచిక_అధికప్రవాహం(true, p_దత్తాంశం, ul_పునాది, ul_ఫలితం);
}

శూన్య __ubsan_handle_alignment_assumption(నిర్మాణం CAlignment_Assumption_దత్తాంశం *p_దత్తాంశం,
					 గుర్తులేని దీర్ఘ ul_సూచిక,
					 గుర్తులేని దీర్ఘ ulAlignment,
					 గుర్తులేని దీర్ఘ ul_స్థానభ్రంశం)
{
	HandleAlignmentAssumption(false, p_దత్తాంశం, ul_సూచిక, ulAlignment,
				  ul_స్థానభ్రంశం);
}

శూన్య __ubsan_handle_alignment_assumption_abort(
	నిర్మాణం CAlignment_Assumption_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ul_సూచిక,
	గుర్తులేని దీర్ఘ ulAlignment, గుర్తులేని దీర్ఘ ul_స్థానభ్రంశం)
{
	HandleAlignmentAssumption(true, p_దత్తాంశం, ul_సూచిక, ulAlignment,
				  ul_స్థానభ్రంశం);
}

శూన్య __ubsan_handle_builtin_unreachable(నిర్మాణం CUnreachable_దత్తాంశం *p_దత్తాంశం)
{
	HandleBuiltinUnreachable(true, p_దత్తాంశం);
}

శూన్య __ubsan_handle_invalid_builtin(నిర్మాణం CInvalid_Builtin_దత్తాంశం *p_దత్తాంశం)
{
	HandleInvalidBuiltin(true, p_దత్తాంశం);
}

శూన్య __ubsan_handle_invalid_builtin_abort(నిర్మాణం CInvalid_Builtin_దత్తాంశం *p_దత్తాంశం)
{
	HandleInvalidBuiltin(true, p_దత్తాంశం);
}

శూన్య __ubsan_handle_load_invalid_value(నిర్మాణం CInvalid_విలువ_దత్తాంశం *p_దత్తాంశం,
				       గుర్తులేని దీర్ఘ ul_విలువ)
{
	Handle_ఎక్కించు_Invalid_విలువ(false, p_దత్తాంశం, ul_విలువ);
}

శూన్య __ubsan_handle_load_invalid_value_abort(నిర్మాణం CInvalid_విలువ_దత్తాంశం *p_దత్తాంశం,
					     గుర్తులేని దీర్ఘ ul_విలువ)
{
	Handle_ఎక్కించు_Invalid_విలువ(true, p_దత్తాంశం, ul_విలువ);
}

శూన్య __ubsan_handle_missing_return(నిర్మాణం CUnreachable_దత్తాంశం *p_దత్తాంశం)
{
	HandleMissingReturn(true, p_దత్తాంశం);
}

శూన్య __ubsan_handle_vla_bound_not_positive(నిర్మాణం CVLABound_దత్తాంశం *p_దత్తాంశం,
					   గుర్తులేని దీర్ఘ ulBound)
{
	Handle_Vla_Bound_Not_ధనాత్మక(false, p_దత్తాంశం, ulBound);
}

శూన్య __ubsan_handle_vla_bound_not_positive_abort(నిర్మాణం CVLABound_దత్తాంశం *p_దత్తాంశం,
						 గుర్తులేని దీర్ఘ ulBound)
{
	Handle_Vla_Bound_Not_ధనాత్మక(true, p_దత్తాంశం, ulBound);
}

శూన్య __ubsan_handle_function_type_mismatch(
	నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ulFunction)
{
	HandleFunctionTypeMismatch(false, p_దత్తాంశం, ulFunction);
}

శూన్య __ubsan_handle_function_type_mismatch_abort(
	నిర్మాణం CFunction_Type_Mismatch_దత్తాంశం *p_దత్తాంశం, గుర్తులేని దీర్ఘ ulFunction)
{
	HandleFunctionTypeMismatch(true, p_దత్తాంశం, ulFunction);
}
#అయితే_ముగింపు
