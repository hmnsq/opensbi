/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#جيڪڏهن_وصف_ٿيل UBSAN_ENABLED

#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#وصف_ڪريو قسم_INTEGER 0
#وصف_ڪريو قسم_FLOAT 1
#وصف_ڪريو قسم_UNKNOWN UINT16_MAX

متطاير بنانشان ڊگهو sbi_ubsan_report_ڳڻپ;

بنية CSourceLocation {
	حرف *mFilename;
	uint32_t m_سٽ;
	uint32_t m_ڪالم;
};

بنية CTypeDescriptor {
	uint16_t m_Type_قسم;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

بنية COverflow_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CUnreachable_معلومات {
	بنية CSourceLocation mLocation;
};

بنية CCFICheck_Fail_معلومات {
	uint8_t m_Check_قسم;
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CDynamic_Type_Cache_ناڪامي_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	فراغ *mTypeInfo;
	uint8_t m_Type_Check_قسم;
};

بنية CFunction_Type_Mismatch_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CInvalid_Builtin_معلومات {
	بنية CSourceLocation mLocation;
	uint8_t m_قسم;
};

بنية CInvalid_قدر_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CNon_Null_Arg_معلومات {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAttributeLocation;
	صحيح m_Arg_اشاريو;
};

بنية CNon_Null_Return_معلومات {
	بنية CSourceLocation mAttributeLocation;
};

بنية COut_جو_Bounds_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mArrayType;
	بنية CTypeDescriptor *m_اشاريو_Type;
};

بنية CPointer_حد_کان_مٿي_معلومات {
	بنية CSourceLocation mLocation;
};

بنية CShift_ٻاهريون_جو_Bounds_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mLHSType;
	بنية CTypeDescriptor *mRHSType;
};

بنية CType_Mismatch_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	بنانشان ڊگهو mLogAlignment;
	uint8_t m_Type_Check_قسم;
};

بنية CType_Mismatch_معلومات_v1 {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_قسم;
};

بنية CVLABound_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CFloat_Cast_حد_کان_مٿي_معلومات {
	بنية CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *m_منزل_Type;
};

بنية CImplicit_Conversion_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *m_منزل_Type;
	uint8_t m_قسم;
};

بنية CAlignment_Assumption_معلومات {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAssumptionLocation;
	بنية CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
فراغ __ubsan_handle_add_overflow(بنية COverflow_معلومات *p_معلومات,
				 بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_add_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بنانشان ڊگهو ulLHS,
				       بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_alignment_assumption(بنية CAlignment_Assumption_معلومات *p_معلومات,
					 بنانشان ڊگهو ul_اشارو,
					 بنانشان ڊگهو ulAlignment,
					 بنانشان ڊگهو ul_وٿي);
فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignment_Assumption_معلومات *p_معلومات, بنانشان ڊگهو ul_اشارو,
	بنانشان ڊگهو ulAlignment, بنانشان ڊگهو ul_وٿي);
فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachable_معلومات *p_معلومات);
فراغ __ubsan_handle_divrem_overflow(بنية COverflow_معلومات *p_معلومات,
				    بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflow_معلومات *p_معلومات,
					  بنانشان ڊگهو ulLHS,
					  بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بنانشان ڊگهو ulFunction);
فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بنانشان ڊگهو ulFunction);
فراغ __ubsan_handle_function_type_mismatch_v1(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بنانشان ڊگهو ulFunction,
	بنانشان ڊگهو ulCalleeRTTI, بنانشان ڊگهو ulFnRTTI);
فراغ __ubsan_handle_function_type_mismatch_v1_abort(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بنانشان ڊگهو ulFunction,
	بنانشان ڊگهو ulCalleeRTTI, بنانشان ڊگهو ulFnRTTI);
فراغ __ubsan_handle_invalid_builtin(بنية CInvalid_Builtin_معلومات *p_معلومات);
فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalid_Builtin_معلومات *p_معلومات);
فراغ __ubsan_handle_load_invalid_value(بنية CInvalid_قدر_معلومات *p_معلومات,
				       بنانشان ڊگهو ulVal);
فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalid_قدر_معلومات *p_معلومات,
					     بنانشان ڊگهو ulVal);
فراغ __ubsan_handle_missing_return(بنية CUnreachable_معلومات *p_معلومات);
فراغ __ubsan_handle_mul_overflow(بنية COverflow_معلومات *p_معلومات,
				 بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_mul_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بنانشان ڊگهو ulLHS,
				       بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_negate_overflow(بنية COverflow_معلومات *p_معلومات,
				    بنانشان ڊگهو ulOldVal);
فراغ __ubsan_handle_negate_overflow_abort(بنية COverflow_معلومات *p_معلومات,
					  بنانشان ڊگهو ulOldVal);
فراغ __ubsan_handle_nullability_arg(بنية CNon_Null_Arg_معلومات *p_معلومات);
فراغ __ubsan_handle_nullability_arg_abort(بنية CNon_Null_Arg_معلومات *p_معلومات);
فراغ __ubsan_handle_nullability_return_v1(
	بنية CNon_Null_Return_معلومات *p_معلومات,
	بنية CSourceLocation *p_Location_اشارو);
فراغ __ubsan_handle_nullability_return_v1_abort(
	بنية CNon_Null_Return_معلومات *p_معلومات,
	بنية CSourceLocation *p_Location_اشارو);
فراغ __ubsan_handle_out_of_bounds(بنية COut_جو_Bounds_معلومات *p_معلومات,
				  بنانشان ڊگهو ul_اشاريو);
فراغ __ubsan_handle_out_of_bounds_abort(بنية COut_جو_Bounds_معلومات *p_معلومات,
					بنانشان ڊگهو ul_اشاريو);
فراغ __ubsan_handle_pointer_overflow(بنية CPointer_حد_کان_مٿي_معلومات *p_معلومات,
				     بنانشان ڊگهو ul_بنياد,
				     بنانشان ڊگهو ul_نتيجو);
فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointer_حد_کان_مٿي_معلومات *p_معلومات,
					   بنانشان ڊگهو ul_بنياد,
					   بنانشان ڊگهو ul_نتيجو);
فراغ __ubsan_handle_shift_out_of_bounds(بنية CShift_ٻاهريون_جو_Bounds_معلومات *p_معلومات,
					بنانشان ڊگهو ulLHS,
					بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShift_ٻاهريون_جو_Bounds_معلومات *p_معلومات, بنانشان ڊگهو ulLHS,
	بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_sub_overflow(بنية COverflow_معلومات *p_معلومات,
				 بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_sub_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بنانشان ڊگهو ulLHS,
				       بنانشان ڊگهو ulRHS);
فراغ __ubsan_handle_type_mismatch(بنية CType_Mismatch_معلومات *p_معلومات,
				  بنانشان ڊگهو ul_اشارو);
فراغ __ubsan_handle_type_mismatch_abort(بنية CType_Mismatch_معلومات *p_معلومات,
					بنانشان ڊگهو ul_اشارو);
فراغ __ubsan_handle_type_mismatch_v1(بنية CType_Mismatch_معلومات_v1 *p_معلومات,
				     بنانشان ڊگهو ul_اشارو);
فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CType_Mismatch_معلومات_v1 *p_معلومات,
					   بنانشان ڊگهو ul_اشارو);
فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABound_معلومات *p_معلومات,
					   بنانشان ڊگهو ulBound);
فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABound_معلومات *p_معلومات,
						 بنانشان ڊگهو ulBound);
فراغ __ubsan_get_current_report_data(ثابت حرف **pp_ٻاهريون_Issue_قسم,
				     ثابت حرف **pp_ٻاهريون_پيغام,
				     ثابت حرف **pp_ٻاهريون_Filename,
				     uint32_t *p_ٻاهريون_سٽ, uint32_t *p_ٻاهريون_کانو,
				     حرف **pp_ٻاهريون_يادگيري_Addr);
ساكن فراغ Handle_حد_کان_مٿي(bool isFatal, بنية COverflow_معلومات *p_معلومات,
			   بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS,
			   ثابت حرف *szOperation);
ساكن فراغ Handle_Negate_حد_کان_مٿي(bool isFatal, بنية COverflow_معلومات *p_معلومات,
				 بنانشان ڊگهو ul_Old_قدر);
ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachable_معلومات *p_معلومات);
ساكن فراغ HandleTypeMismatch(bool isFatal, بنية CSourceLocation *mLocation,
			       بنية CTypeDescriptor *mType,
			       بنانشان ڊگهو mLogAlignment,
			       uint8_t m_Type_Check_قسم, بنانشان ڊگهو ul_اشارو);
ساكن فراغ Handle_Vla_Bound_Not_مثبت(bool isFatal, بنية CVLABound_معلومات *p_معلومات,
				      بنانشان ڊگهو ulBound);
ساكن فراغ Handle_ٻاهريون_جو_Bounds(bool isFatal, بنية COut_جو_Bounds_معلومات *p_معلومات,
			      بنانشان ڊگهو ul_اشاريو);
ساكن فراغ Handle_سرڪايو_ٻاهريون_جو_Bounds(bool isFatal,
				   بنية CShift_ٻاهريون_جو_Bounds_معلومات *p_معلومات,
				   بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS);
ساكن فراغ Handle_ڀريو_Invalid_قدر(bool isFatal,
				   بنية CInvalid_قدر_معلومات *p_معلومات,
				   بنانشان ڊگهو ul_قدر);
ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalid_Builtin_معلومات *p_معلومات);
ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunction_Type_Mismatch_معلومات *p_معلومات,
				       بنانشان ڊگهو ulFunction);
ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachable_معلومات *p_معلومات);
ساكن فراغ Handle_اشارو_حد_کان_مٿي(bool isFatal,
				  بنية CPointer_حد_کان_مٿي_معلومات *p_معلومات,
				  بنانشان ڊگهو ul_بنياد, بنانشان ڊگهو ul_نتيجو);
ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignment_Assumption_معلومات *p_معلومات,
				      بنانشان ڊگهو ul_اشارو,
				      بنانشان ڊگهو ulAlignment,
				      بنانشان ڊگهو ul_وٿي);

#وصف_ڪريو عدد_SIGNED_ٻٽو_انگ 1
#وصف_ڪريو عدد_MAXLEN 128
#وصف_ڪريو __arraycount(__a) (حجم(__a) / حجم(__a[0]))
#وصف_ڪريو __BIT(__n) (1UL << (__n))
#وصف_ڪريو SEPARATOR sbi_printf("===========================================\n")
#وصف_ڪريو ACK_REPORTED (1U << 31)

ساكن bool isAlreadyReported(بنية CSourceLocation *pLocation)
{
	uint32_t si_Old_قدر;
	متطاير uint32_t *p_سٽ;

	إذا (!pLocation)
		أرجع false;

	p_سٽ = &pLocation->m_سٽ;

	افعل {
		si_Old_قدر = *p_سٽ;
	} طالما (__sync_val_compare_and_swap(p_سٽ, si_Old_قدر,
					     si_Old_قدر | ACK_REPORTED) !=
		 si_Old_قدر);

	إذا (!(si_Old_قدر & ACK_REPORTED)) {

		sbi_ubsan_report_ڳڻپ++;

		أرجع false;
	}

	أرجع true;
}

ساكن فراغ Handle_حد_کان_مٿي(bool isFatal, بنية COverflow_معلومات *p_معلومات,
			   بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS,
			   ثابت حرف *szOperation)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	bool is_signed = p_معلومات->mType->mTypeInfo & عدد_SIGNED_ٻٽو_انگ;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	إذا (is_signed) {
		sbi_printf("%ld %s %ld ", (ڊگهو)ulLHS, szOperation,
			   (ڊگهو)ulRHS);
	} وإلا {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_معلومات->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ Handle_Negate_حد_کان_مٿي(bool isFatal, بنية COverflow_معلومات *p_معلومات,
				 بنانشان ڊگهو ul_Old_قدر)
{
	إذا (!p_معلومات) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	bool is_signed = p_معلومات->mType->mTypeInfo & عدد_SIGNED_ٻٽو_انگ;

	sbi_printf("UBSan: Negation of ");

	إذا (is_signed) {
		sbi_printf("%ld", (ڊگهو)ul_Old_قدر);
	} وإلا {
		sbi_printf("%lu", ul_Old_قدر);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_معلومات->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachable_معلومات *p_معلومات)
{
	إذا (!p_معلومات) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	SEPARATOR;
}

ثابت حرف *rgczTypeCheckKinds[] = { "load of",
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

ساكن فراغ HandleTypeMismatch(bool isFatal, بنية CSourceLocation *mLocation,
			       بنية CTypeDescriptor *mType,
			       بنانشان ڊگهو mLogAlignment,
			       uint8_t m_Type_Check_قسم, بنانشان ڊگهو ul_اشارو)
{

	إذا ((!mLocation) || (!mType)) {
		أرجع;
	}

	إذا (isAlreadyReported(mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_سٽ & ~ACK_REPORTED,
		   mLocation->m_ڪالم);

	ثابت حرف *قسم = (m_Type_Check_قسم < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_قسم]
				   : "access to";

	إذا (ul_اشارو == 0) {
		sbi_printf("%s null pointer of type %s\n", قسم,
			   mType->mTypeName);
	} وإلا إذا (
		(mLogAlignment - 1) &
		ul_اشارو) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			قسم, (فراغ *)ul_اشارو, mType->mTypeName,
			mLogAlignment);
	} وإلا {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			قسم, (فراغ *)ul_اشارو, mType->mTypeName);
	}
	SEPARATOR;
}

ساكن فراغ Handle_Vla_Bound_Not_مثبت(bool isFatal, بنية CVLABound_معلومات *p_معلومات,
				      بنانشان ڊگهو ulBound)
{
	إذا (!p_معلومات) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_معلومات->mType->mTypeInfo & عدد_SIGNED_ٻٽو_انگ;

	إذا (is_signed) {
		sbi_printf("%ld", (ڊگهو)ulBound);
	} وإلا {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ساكن فراغ Handle_ٻاهريون_جو_Bounds(bool isFatal, بنية COut_جو_Bounds_معلومات *p_معلومات,
			      بنانشان ڊگهو ul_اشاريو)
{
	إذا (!p_معلومات) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	bool is_signed = p_معلومات->m_اشاريو_Type->mTypeInfo & عدد_SIGNED_ٻٽو_انگ;

	إذا (is_signed) {
		sbi_printf("index %ld", (ڊگهو)ul_اشاريو);
	} وإلا {
		sbi_printf("index %lu", ul_اشاريو);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_معلومات->mArrayType->mTypeName);

	SEPARATOR;
}

ساكن bool is_Negative_عدد(بنية CTypeDescriptor *pType, بنانشان ڊگهو ulVal)
{
	إذا (!(pType->mTypeInfo & عدد_SIGNED_ٻٽو_انگ)) {
		أرجع false;
	}

	أرجع (ڊگهو)ulVal < 0;
}

ساكن size_t type_ويڪر(بنية CTypeDescriptor *pType)
{
	أرجع 1UL << (pType->mTypeInfo >> 1);
}

ساكن فراغ Handle_سرڪايو_ٻاهريون_جو_Bounds(bool isFatal,
				   بنية CShift_ٻاهريون_جو_Bounds_معلومات *p_معلومات,
				   بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	إذا (is_Negative_عدد(p_معلومات->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (ڊگهو)ulRHS);
	} وإلا إذا (ulRHS >= type_ويڪر(p_معلومات->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (بنانشان ڊگهو)type_ويڪر(p_معلومات->mLHSType),
			p_معلومات->mLHSType->mTypeName);

	} وإلا إذا (is_Negative_عدد(p_معلومات->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (ڊگهو)ulLHS);
	} وإلا {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_معلومات->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ساكن فراغ Handle_ڀريو_Invalid_قدر(bool isFatal,
				   بنية CInvalid_قدر_معلومات *p_معلومات,
				   بنانشان ڊگهو ul_قدر)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	bool is_signed = p_معلومات->mType->mTypeInfo & عدد_SIGNED_ٻٽو_انگ;

	sbi_printf("load of value ");

	إذا (is_signed) {
		sbi_printf("%ld ", (ڊگهو)ul_قدر);
	} وإلا {
		sbi_printf("%lu ", ul_قدر);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_معلومات->mType->mTypeName);

	SEPARATOR;
}

ثابت حرف *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalid_Builtin_معلومات *p_معلومات)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	ثابت حرف *builtin =
		(p_معلومات->m_قسم < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_معلومات->m_قسم]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunction_Type_Mismatch_معلومات *p_معلومات,
				       بنانشان ڊگهو ulFunction)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_معلومات->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachable_معلومات *p_معلومات)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ساكن فراغ Handle_اشارو_حد_کان_مٿي(bool isFatal,
				  بنية CPointer_حد_کان_مٿي_معلومات *p_معلومات,
				  بنانشان ڊگهو ul_بنياد, بنانشان ڊگهو ul_نتيجو)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_بنياد, ul_نتيجو);

	SEPARATOR;
}

ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignment_Assumption_معلومات *p_معلومات,
				      بنانشان ڊگهو ul_اشارو,
				      بنانشان ڊگهو ulAlignment,
				      بنانشان ڊگهو ul_وٿي)
{

	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_سٽ & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ڪالم);

	بنانشان ڊگهو ul_Real_اشارو = ul_اشارو - ul_وٿي;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (فراغ *)ul_Real_اشارو, (فراغ *)ul_وٿي);

	إذا (p_معلومات->mAssumptionLocation.mFilename != عدم) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_معلومات->mAssumptionLocation.mFilename,
			   p_معلومات->mAssumptionLocation.m_سٽ,
			   p_معلومات->mAssumptionLocation.m_ڪالم);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
فراغ __ubsan_handle_add_overflow(بنية COverflow_معلومات *p_معلومات,
				 بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS)
{
	Handle_حد_کان_مٿي(false, p_معلومات, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_add_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS)
{
	Handle_حد_کان_مٿي(true, p_معلومات, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_sub_overflow(بنية COverflow_معلومات *p_معلومات,
				 بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS)
{
	Handle_حد_کان_مٿي(false, p_معلومات, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_sub_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS)
{
	Handle_حد_کان_مٿي(true, p_معلومات, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_mul_overflow(بنية COverflow_معلومات *p_معلومات,
				 بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS)
{
	Handle_حد_کان_مٿي(false, p_معلومات, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_mul_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS)
{
	Handle_حد_کان_مٿي(true, p_معلومات, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_negate_overflow(بنية COverflow_معلومات *p_معلومات,
				    بنانشان ڊگهو ul_Old_قدر)
{
	Handle_Negate_حد_کان_مٿي(false, p_معلومات, ul_Old_قدر);
}

فراغ __ubsan_handle_negate_overflow_abort(بنية COverflow_معلومات *p_معلومات,
					  بنانشان ڊگهو ul_Old_قدر)
{
	Handle_Negate_حد_کان_مٿي(true, p_معلومات, ul_Old_قدر);
}

فراغ __ubsan_handle_divrem_overflow(بنية COverflow_معلومات *p_معلومات,
				    بنانشان ڊگهو ulLHS, بنانشان ڊگهو ulRHS)
{
	Handle_حد_کان_مٿي(false, p_معلومات, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflow_معلومات *p_معلومات,
					  بنانشان ڊگهو ulLHS,
					  بنانشان ڊگهو ulRHS)
{
	Handle_حد_کان_مٿي(true, p_معلومات, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_type_mismatch_v1(بنية CType_Mismatch_معلومات_v1 *p_معلومات,
				     بنانشان ڊگهو ul_اشارو)
{
	HandleTypeMismatch(false, &p_معلومات->mLocation, p_معلومات->mType,
			   __BIT(p_معلومات->mLogAlignment), p_معلومات->m_Type_Check_قسم,
			   ul_اشارو);
}

فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CType_Mismatch_معلومات_v1 *p_معلومات,
					   بنانشان ڊگهو ul_اشارو)
{
	HandleTypeMismatch(true, &p_معلومات->mLocation, p_معلومات->mType,
			   __BIT(p_معلومات->mLogAlignment), p_معلومات->m_Type_Check_قسم,
			   ul_اشارو);
}

فراغ __ubsan_handle_out_of_bounds(بنية COut_جو_Bounds_معلومات *p_معلومات,
				  بنانشان ڊگهو ul_اشاريو)
{
	Handle_ٻاهريون_جو_Bounds(false, p_معلومات, ul_اشاريو);
}

فراغ __ubsan_handle_out_of_bounds_abort(بنية COut_جو_Bounds_معلومات *p_معلومات,
					بنانشان ڊگهو ul_اشاريو)
{
	Handle_ٻاهريون_جو_Bounds(true, p_معلومات, ul_اشاريو);
}

فراغ __ubsan_handle_shift_out_of_bounds(بنية CShift_ٻاهريون_جو_Bounds_معلومات *p_معلومات,
					بنانشان ڊگهو ulLHS,
					بنانشان ڊگهو ulRHS)
{
	Handle_سرڪايو_ٻاهريون_جو_Bounds(false, p_معلومات, ulLHS, ulRHS);
}

فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShift_ٻاهريون_جو_Bounds_معلومات *p_معلومات, بنانشان ڊگهو ulLHS,
	بنانشان ڊگهو ulRHS)
{
	Handle_سرڪايو_ٻاهريون_جو_Bounds(true, p_معلومات, ulLHS, ulRHS);
}

فراغ __ubsan_handle_pointer_overflow(بنية CPointer_حد_کان_مٿي_معلومات *p_معلومات,
				     بنانشان ڊگهو ul_بنياد,
				     بنانشان ڊگهو ul_نتيجو)
{
	Handle_اشارو_حد_کان_مٿي(false, p_معلومات, ul_بنياد, ul_نتيجو);
}

فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointer_حد_کان_مٿي_معلومات *p_معلومات,
					   بنانشان ڊگهو ul_بنياد,
					   بنانشان ڊگهو ul_نتيجو)
{
	Handle_اشارو_حد_کان_مٿي(true, p_معلومات, ul_بنياد, ul_نتيجو);
}

فراغ __ubsan_handle_alignment_assumption(بنية CAlignment_Assumption_معلومات *p_معلومات,
					 بنانشان ڊگهو ul_اشارو,
					 بنانشان ڊگهو ulAlignment,
					 بنانشان ڊگهو ul_وٿي)
{
	HandleAlignmentAssumption(false, p_معلومات, ul_اشارو, ulAlignment,
				  ul_وٿي);
}

فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignment_Assumption_معلومات *p_معلومات, بنانشان ڊگهو ul_اشارو,
	بنانشان ڊگهو ulAlignment, بنانشان ڊگهو ul_وٿي)
{
	HandleAlignmentAssumption(true, p_معلومات, ul_اشارو, ulAlignment,
				  ul_وٿي);
}

فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachable_معلومات *p_معلومات)
{
	HandleBuiltinUnreachable(true, p_معلومات);
}

فراغ __ubsan_handle_invalid_builtin(بنية CInvalid_Builtin_معلومات *p_معلومات)
{
	HandleInvalidBuiltin(true, p_معلومات);
}

فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalid_Builtin_معلومات *p_معلومات)
{
	HandleInvalidBuiltin(true, p_معلومات);
}

فراغ __ubsan_handle_load_invalid_value(بنية CInvalid_قدر_معلومات *p_معلومات,
				       بنانشان ڊگهو ul_قدر)
{
	Handle_ڀريو_Invalid_قدر(false, p_معلومات, ul_قدر);
}

فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalid_قدر_معلومات *p_معلومات,
					     بنانشان ڊگهو ul_قدر)
{
	Handle_ڀريو_Invalid_قدر(true, p_معلومات, ul_قدر);
}

فراغ __ubsan_handle_missing_return(بنية CUnreachable_معلومات *p_معلومات)
{
	HandleMissingReturn(true, p_معلومات);
}

فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABound_معلومات *p_معلومات,
					   بنانشان ڊگهو ulBound)
{
	Handle_Vla_Bound_Not_مثبت(false, p_معلومات, ulBound);
}

فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABound_معلومات *p_معلومات,
						 بنانشان ڊگهو ulBound)
{
	Handle_Vla_Bound_Not_مثبت(true, p_معلومات, ulBound);
}

فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بنانشان ڊگهو ulFunction)
{
	HandleFunctionTypeMismatch(false, p_معلومات, ulFunction);
}

فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بنانشان ڊگهو ulFunction)
{
	HandleFunctionTypeMismatch(true, p_معلومات, ulFunction);
}
#جيڪڏهن_پورو
