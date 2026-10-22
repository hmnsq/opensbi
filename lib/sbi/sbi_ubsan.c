/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#اگر_تعریف_شده UBSAN_ENABLED

#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#تعریف_کن نوع_INTEGER 0
#تعریف_کن نوع_FLOAT 1
#تعریف_کن نوع_UNKNOWN UINT16_MAX

متطاير بیعلامت دراز sbi_ubsan_report_شمار;

بنية CSourceLocation {
	حرف *mFilename;
	uint32_t m_سطر;
	uint32_t m_ستونی;
};

بنية CTypeDescriptor {
	uint16_t m_Type_نوع;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

بنية COverflow_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CUnreachable_داده {
	بنية CSourceLocation mLocation;
};

بنية CCFICheck_Fail_داده {
	uint8_t m_Check_نوع;
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CDynamic_Type_Cache_نیافتن_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	فراغ *mTypeInfo;
	uint8_t m_Type_Check_نوع;
};

بنية CFunction_Type_Mismatch_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CInvalid_Builtin_داده {
	بنية CSourceLocation mLocation;
	uint8_t m_نوع;
};

بنية CInvalid_مقدار_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CNon_Null_Arg_داده {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAttributeLocation;
	صحيح m_Arg_شاخص;
};

بنية CNon_Null_Return_داده {
	بنية CSourceLocation mAttributeLocation;
};

بنية COut_از_Bounds_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mArrayType;
	بنية CTypeDescriptor *m_شاخص_Type;
};

بنية CPointer_سرریز_داده {
	بنية CSourceLocation mLocation;
};

بنية CShift_بیرون_از_Bounds_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mLHSType;
	بنية CTypeDescriptor *mRHSType;
};

بنية CType_Mismatch_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	بیعلامت دراز mLogAlignment;
	uint8_t m_Type_Check_نوع;
};

بنية CType_Mismatch_داده_v1 {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_نوع;
};

بنية CVLABound_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CFloat_Cast_سرریز_داده {
	بنية CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *m_مقصد_Type;
};

بنية CImplicit_Conversion_داده {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *m_مقصد_Type;
	uint8_t m_نوع;
};

بنية CAlignment_Assumption_داده {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAssumptionLocation;
	بنية CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
فراغ __ubsan_handle_add_overflow(بنية COverflow_داده *p_داده,
				 بیعلامت دراز ulLHS, بیعلامت دراز ulRHS);
فراغ __ubsan_handle_add_overflow_abort(بنية COverflow_داده *p_داده,
				       بیعلامت دراز ulLHS,
				       بیعلامت دراز ulRHS);
فراغ __ubsan_handle_alignment_assumption(بنية CAlignment_Assumption_داده *p_داده,
					 بیعلامت دراز ul_اشاره_گر,
					 بیعلامت دراز ulAlignment,
					 بیعلامت دراز ul_جابه_جایی);
فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignment_Assumption_داده *p_داده, بیعلامت دراز ul_اشاره_گر,
	بیعلامت دراز ulAlignment, بیعلامت دراز ul_جابه_جایی);
فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachable_داده *p_داده);
فراغ __ubsan_handle_divrem_overflow(بنية COverflow_داده *p_داده,
				    بیعلامت دراز ulLHS, بیعلامت دراز ulRHS);
فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflow_داده *p_داده,
					  بیعلامت دراز ulLHS,
					  بیعلامت دراز ulRHS);
فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunction_Type_Mismatch_داده *p_داده, بیعلامت دراز ulFunction);
فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunction_Type_Mismatch_داده *p_داده, بیعلامت دراز ulFunction);
فراغ __ubsan_handle_function_type_mismatch_v1(
	بنية CFunction_Type_Mismatch_داده *p_داده, بیعلامت دراز ulFunction,
	بیعلامت دراز ulCalleeRTTI, بیعلامت دراز ulFnRTTI);
فراغ __ubsan_handle_function_type_mismatch_v1_abort(
	بنية CFunction_Type_Mismatch_داده *p_داده, بیعلامت دراز ulFunction,
	بیعلامت دراز ulCalleeRTTI, بیعلامت دراز ulFnRTTI);
فراغ __ubsan_handle_invalid_builtin(بنية CInvalid_Builtin_داده *p_داده);
فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalid_Builtin_داده *p_داده);
فراغ __ubsan_handle_load_invalid_value(بنية CInvalid_مقدار_داده *p_داده,
				       بیعلامت دراز ulVal);
فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalid_مقدار_داده *p_داده,
					     بیعلامت دراز ulVal);
فراغ __ubsan_handle_missing_return(بنية CUnreachable_داده *p_داده);
فراغ __ubsan_handle_mul_overflow(بنية COverflow_داده *p_داده,
				 بیعلامت دراز ulLHS, بیعلامت دراز ulRHS);
فراغ __ubsan_handle_mul_overflow_abort(بنية COverflow_داده *p_داده,
				       بیعلامت دراز ulLHS,
				       بیعلامت دراز ulRHS);
فراغ __ubsan_handle_negate_overflow(بنية COverflow_داده *p_داده,
				    بیعلامت دراز ulOldVal);
فراغ __ubsan_handle_negate_overflow_abort(بنية COverflow_داده *p_داده,
					  بیعلامت دراز ulOldVal);
فراغ __ubsan_handle_nullability_arg(بنية CNon_Null_Arg_داده *p_داده);
فراغ __ubsan_handle_nullability_arg_abort(بنية CNon_Null_Arg_داده *p_داده);
فراغ __ubsan_handle_nullability_return_v1(
	بنية CNon_Null_Return_داده *p_داده,
	بنية CSourceLocation *p_Location_اشاره_گر);
فراغ __ubsan_handle_nullability_return_v1_abort(
	بنية CNon_Null_Return_داده *p_داده,
	بنية CSourceLocation *p_Location_اشاره_گر);
فراغ __ubsan_handle_out_of_bounds(بنية COut_از_Bounds_داده *p_داده,
				  بیعلامت دراز ul_شاخص);
فراغ __ubsan_handle_out_of_bounds_abort(بنية COut_از_Bounds_داده *p_داده,
					بیعلامت دراز ul_شاخص);
فراغ __ubsan_handle_pointer_overflow(بنية CPointer_سرریز_داده *p_داده,
				     بیعلامت دراز ul_پایه,
				     بیعلامت دراز ul_نتیجه);
فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointer_سرریز_داده *p_داده,
					   بیعلامت دراز ul_پایه,
					   بیعلامت دراز ul_نتیجه);
فراغ __ubsan_handle_shift_out_of_bounds(بنية CShift_بیرون_از_Bounds_داده *p_داده,
					بیعلامت دراز ulLHS,
					بیعلامت دراز ulRHS);
فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShift_بیرون_از_Bounds_داده *p_داده, بیعلامت دراز ulLHS,
	بیعلامت دراز ulRHS);
فراغ __ubsan_handle_sub_overflow(بنية COverflow_داده *p_داده,
				 بیعلامت دراز ulLHS, بیعلامت دراز ulRHS);
فراغ __ubsan_handle_sub_overflow_abort(بنية COverflow_داده *p_داده,
				       بیعلامت دراز ulLHS,
				       بیعلامت دراز ulRHS);
فراغ __ubsan_handle_type_mismatch(بنية CType_Mismatch_داده *p_داده,
				  بیعلامت دراز ul_اشاره_گر);
فراغ __ubsan_handle_type_mismatch_abort(بنية CType_Mismatch_داده *p_داده,
					بیعلامت دراز ul_اشاره_گر);
فراغ __ubsan_handle_type_mismatch_v1(بنية CType_Mismatch_داده_v1 *p_داده,
				     بیعلامت دراز ul_اشاره_گر);
فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CType_Mismatch_داده_v1 *p_داده,
					   بیعلامت دراز ul_اشاره_گر);
فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABound_داده *p_داده,
					   بیعلامت دراز ulBound);
فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABound_داده *p_داده,
						 بیعلامت دراز ulBound);
فراغ __ubsan_get_current_report_data(ثابت حرف **pp_بیرون_Issue_نوع,
				     ثابت حرف **pp_بیرون_پیام,
				     ثابت حرف **pp_بیرون_Filename,
				     uint32_t *p_بیرون_سطر, uint32_t *p_بیرون_ستون,
				     حرف **pp_بیرون_حافظه_Addr);
ساكن فراغ Handle_سرریز(bool isFatal, بنية COverflow_داده *p_داده,
			   بیعلامت دراز ulLHS, بیعلامت دراز ulRHS,
			   ثابت حرف *szOperation);
ساكن فراغ Handle_Negate_سرریز(bool isFatal, بنية COverflow_داده *p_داده,
				 بیعلامت دراز ul_Old_مقدار);
ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachable_داده *p_داده);
ساكن فراغ HandleTypeMismatch(bool isFatal, بنية CSourceLocation *mLocation,
			       بنية CTypeDescriptor *mType,
			       بیعلامت دراز mLogAlignment,
			       uint8_t m_Type_Check_نوع, بیعلامت دراز ul_اشاره_گر);
ساكن فراغ Handle_Vla_Bound_Not_مثبت(bool isFatal, بنية CVLABound_داده *p_داده,
				      بیعلامت دراز ulBound);
ساكن فراغ Handle_بیرون_از_Bounds(bool isFatal, بنية COut_از_Bounds_داده *p_داده,
			      بیعلامت دراز ul_شاخص);
ساكن فراغ Handle_لغزش_بیرون_از_Bounds(bool isFatal,
				   بنية CShift_بیرون_از_Bounds_داده *p_داده,
				   بیعلامت دراز ulLHS, بیعلامت دراز ulRHS);
ساكن فراغ Handle_بارگیری_Invalid_مقدار(bool isFatal,
				   بنية CInvalid_مقدار_داده *p_داده,
				   بیعلامت دراز ul_مقدار);
ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalid_Builtin_داده *p_داده);
ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunction_Type_Mismatch_داده *p_داده,
				       بیعلامت دراز ulFunction);
ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachable_داده *p_داده);
ساكن فراغ Handle_اشاره_گر_سرریز(bool isFatal,
				  بنية CPointer_سرریز_داده *p_داده,
				  بیعلامت دراز ul_پایه, بیعلامت دراز ul_نتیجه);
ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignment_Assumption_داده *p_داده,
				      بیعلامت دراز ul_اشاره_گر,
				      بیعلامت دراز ulAlignment,
				      بیعلامت دراز ul_جابه_جایی);

#تعریف_کن عدد_SIGNED_بیت 1
#تعریف_کن عدد_MAXLEN 128
#تعریف_کن __arraycount(__a) (حجم(__a) / حجم(__a[0]))
#تعریف_کن __BIT(__n) (1UL << (__n))
#تعریف_کن SEPARATOR sbi_printf("===========================================\n")
#تعریف_کن ACK_REPORTED (1U << 31)

ساكن bool isAlreadyReported(بنية CSourceLocation *pLocation)
{
	uint32_t si_Old_مقدار;
	متطاير uint32_t *p_سطر;

	إذا (!pLocation)
		أرجع false;

	p_سطر = &pLocation->m_سطر;

	افعل {
		si_Old_مقدار = *p_سطر;
	} طالما (__sync_val_compare_and_swap(p_سطر, si_Old_مقدار,
					     si_Old_مقدار | ACK_REPORTED) !=
		 si_Old_مقدار);

	إذا (!(si_Old_مقدار & ACK_REPORTED)) {

		sbi_ubsan_report_شمار++;

		أرجع false;
	}

	أرجع true;
}

ساكن فراغ Handle_سرریز(bool isFatal, بنية COverflow_داده *p_داده,
			   بیعلامت دراز ulLHS, بیعلامت دراز ulRHS,
			   ثابت حرف *szOperation)
{
	إذا (!p_داده) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	bool is_signed = p_داده->mType->mTypeInfo & عدد_SIGNED_بیت;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	إذا (is_signed) {
		sbi_printf("%ld %s %ld ", (دراز)ulLHS, szOperation,
			   (دراز)ulRHS);
	} وإلا {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_داده->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ Handle_Negate_سرریز(bool isFatal, بنية COverflow_داده *p_داده,
				 بیعلامت دراز ul_Old_مقدار)
{
	إذا (!p_داده) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	bool is_signed = p_داده->mType->mTypeInfo & عدد_SIGNED_بیت;

	sbi_printf("UBSan: Negation of ");

	إذا (is_signed) {
		sbi_printf("%ld", (دراز)ul_Old_مقدار);
	} وإلا {
		sbi_printf("%lu", ul_Old_مقدار);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_داده->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachable_داده *p_داده)
{
	إذا (!p_داده) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

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
			       بیعلامت دراز mLogAlignment,
			       uint8_t m_Type_Check_نوع, بیعلامت دراز ul_اشاره_گر)
{

	إذا ((!mLocation) || (!mType)) {
		أرجع;
	}

	إذا (isAlreadyReported(mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_سطر & ~ACK_REPORTED,
		   mLocation->m_ستونی);

	ثابت حرف *نوع_2 = (m_Type_Check_نوع < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_نوع]
				   : "access to";

	إذا (ul_اشاره_گر == 0) {
		sbi_printf("%s null pointer of type %s\n", نوع_2,
			   mType->mTypeName);
	} وإلا إذا (
		(mLogAlignment - 1) &
		ul_اشاره_گر) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			نوع_2, (فراغ *)ul_اشاره_گر, mType->mTypeName,
			mLogAlignment);
	} وإلا {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			نوع_2, (فراغ *)ul_اشاره_گر, mType->mTypeName);
	}
	SEPARATOR;
}

ساكن فراغ Handle_Vla_Bound_Not_مثبت(bool isFatal, بنية CVLABound_داده *p_داده,
				      بیعلامت دراز ulBound)
{
	إذا (!p_داده) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_داده->mType->mTypeInfo & عدد_SIGNED_بیت;

	إذا (is_signed) {
		sbi_printf("%ld", (دراز)ulBound);
	} وإلا {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ساكن فراغ Handle_بیرون_از_Bounds(bool isFatal, بنية COut_از_Bounds_داده *p_داده,
			      بیعلامت دراز ul_شاخص)
{
	إذا (!p_داده) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	bool is_signed = p_داده->m_شاخص_Type->mTypeInfo & عدد_SIGNED_بیت;

	إذا (is_signed) {
		sbi_printf("index %ld", (دراز)ul_شاخص);
	} وإلا {
		sbi_printf("index %lu", ul_شاخص);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_داده->mArrayType->mTypeName);

	SEPARATOR;
}

ساكن bool is_Negative_عدد(بنية CTypeDescriptor *pType, بیعلامت دراز ulVal)
{
	إذا (!(pType->mTypeInfo & عدد_SIGNED_بیت)) {
		أرجع false;
	}

	أرجع (دراز)ulVal < 0;
}

ساكن size_t type_عرض(بنية CTypeDescriptor *pType)
{
	أرجع 1UL << (pType->mTypeInfo >> 1);
}

ساكن فراغ Handle_لغزش_بیرون_از_Bounds(bool isFatal,
				   بنية CShift_بیرون_از_Bounds_داده *p_داده,
				   بیعلامت دراز ulLHS, بیعلامت دراز ulRHS)
{
	إذا (!p_داده) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	إذا (is_Negative_عدد(p_داده->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (دراز)ulRHS);
	} وإلا إذا (ulRHS >= type_عرض(p_داده->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (بیعلامت دراز)type_عرض(p_داده->mLHSType),
			p_داده->mLHSType->mTypeName);

	} وإلا إذا (is_Negative_عدد(p_داده->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (دراز)ulLHS);
	} وإلا {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_داده->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ساكن فراغ Handle_بارگیری_Invalid_مقدار(bool isFatal,
				   بنية CInvalid_مقدار_داده *p_داده,
				   بیعلامت دراز ul_مقدار)
{
	إذا (!p_داده) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	bool is_signed = p_داده->mType->mTypeInfo & عدد_SIGNED_بیت;

	sbi_printf("load of value ");

	إذا (is_signed) {
		sbi_printf("%ld ", (دراز)ul_مقدار);
	} وإلا {
		sbi_printf("%lu ", ul_مقدار);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_داده->mType->mTypeName);

	SEPARATOR;
}

ثابت حرف *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalid_Builtin_داده *p_داده)
{
	إذا (!p_داده) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	ثابت حرف *builtin =
		(p_داده->m_نوع < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_داده->m_نوع]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunction_Type_Mismatch_داده *p_داده,
				       بیعلامت دراز ulFunction)
{
	إذا (!p_داده) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_داده->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachable_داده *p_داده)
{
	إذا (!p_داده) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ساكن فراغ Handle_اشاره_گر_سرریز(bool isFatal,
				  بنية CPointer_سرریز_داده *p_داده,
				  بیعلامت دراز ul_پایه, بیعلامت دراز ul_نتیجه)
{
	إذا (!p_داده) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_پایه, ul_نتیجه);

	SEPARATOR;
}

ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignment_Assumption_داده *p_داده,
				      بیعلامت دراز ul_اشاره_گر,
				      بیعلامت دراز ulAlignment,
				      بیعلامت دراز ul_جابه_جایی)
{

	إذا (!p_داده) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_داده->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_داده->mLocation.mFilename,
		   p_داده->mLocation.m_سطر & ~ACK_REPORTED,
		   p_داده->mLocation.m_ستونی);

	بیعلامت دراز ul_Real_اشاره_گر = ul_اشاره_گر - ul_جابه_جایی;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (فراغ *)ul_Real_اشاره_گر, (فراغ *)ul_جابه_جایی);

	إذا (p_داده->mAssumptionLocation.mFilename != عدم) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_داده->mAssumptionLocation.mFilename,
			   p_داده->mAssumptionLocation.m_سطر,
			   p_داده->mAssumptionLocation.m_ستونی);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
فراغ __ubsan_handle_add_overflow(بنية COverflow_داده *p_داده,
				 بیعلامت دراز ulLHS, بیعلامت دراز ulRHS)
{
	Handle_سرریز(false, p_داده, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_add_overflow_abort(بنية COverflow_داده *p_داده,
				       بیعلامت دراز ulLHS, بیعلامت دراز ulRHS)
{
	Handle_سرریز(true, p_داده, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_sub_overflow(بنية COverflow_داده *p_داده,
				 بیعلامت دراز ulLHS, بیعلامت دراز ulRHS)
{
	Handle_سرریز(false, p_داده, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_sub_overflow_abort(بنية COverflow_داده *p_داده,
				       بیعلامت دراز ulLHS, بیعلامت دراز ulRHS)
{
	Handle_سرریز(true, p_داده, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_mul_overflow(بنية COverflow_داده *p_داده,
				 بیعلامت دراز ulLHS, بیعلامت دراز ulRHS)
{
	Handle_سرریز(false, p_داده, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_mul_overflow_abort(بنية COverflow_داده *p_داده,
				       بیعلامت دراز ulLHS, بیعلامت دراز ulRHS)
{
	Handle_سرریز(true, p_داده, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_negate_overflow(بنية COverflow_داده *p_داده,
				    بیعلامت دراز ul_Old_مقدار)
{
	Handle_Negate_سرریز(false, p_داده, ul_Old_مقدار);
}

فراغ __ubsan_handle_negate_overflow_abort(بنية COverflow_داده *p_داده,
					  بیعلامت دراز ul_Old_مقدار)
{
	Handle_Negate_سرریز(true, p_داده, ul_Old_مقدار);
}

فراغ __ubsan_handle_divrem_overflow(بنية COverflow_داده *p_داده,
				    بیعلامت دراز ulLHS, بیعلامت دراز ulRHS)
{
	Handle_سرریز(false, p_داده, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflow_داده *p_داده,
					  بیعلامت دراز ulLHS,
					  بیعلامت دراز ulRHS)
{
	Handle_سرریز(true, p_داده, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_type_mismatch_v1(بنية CType_Mismatch_داده_v1 *p_داده,
				     بیعلامت دراز ul_اشاره_گر)
{
	HandleTypeMismatch(false, &p_داده->mLocation, p_داده->mType,
			   __BIT(p_داده->mLogAlignment), p_داده->m_Type_Check_نوع,
			   ul_اشاره_گر);
}

فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CType_Mismatch_داده_v1 *p_داده,
					   بیعلامت دراز ul_اشاره_گر)
{
	HandleTypeMismatch(true, &p_داده->mLocation, p_داده->mType,
			   __BIT(p_داده->mLogAlignment), p_داده->m_Type_Check_نوع,
			   ul_اشاره_گر);
}

فراغ __ubsan_handle_out_of_bounds(بنية COut_از_Bounds_داده *p_داده,
				  بیعلامت دراز ul_شاخص)
{
	Handle_بیرون_از_Bounds(false, p_داده, ul_شاخص);
}

فراغ __ubsan_handle_out_of_bounds_abort(بنية COut_از_Bounds_داده *p_داده,
					بیعلامت دراز ul_شاخص)
{
	Handle_بیرون_از_Bounds(true, p_داده, ul_شاخص);
}

فراغ __ubsan_handle_shift_out_of_bounds(بنية CShift_بیرون_از_Bounds_داده *p_داده,
					بیعلامت دراز ulLHS,
					بیعلامت دراز ulRHS)
{
	Handle_لغزش_بیرون_از_Bounds(false, p_داده, ulLHS, ulRHS);
}

فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShift_بیرون_از_Bounds_داده *p_داده, بیعلامت دراز ulLHS,
	بیعلامت دراز ulRHS)
{
	Handle_لغزش_بیرون_از_Bounds(true, p_داده, ulLHS, ulRHS);
}

فراغ __ubsan_handle_pointer_overflow(بنية CPointer_سرریز_داده *p_داده,
				     بیعلامت دراز ul_پایه,
				     بیعلامت دراز ul_نتیجه)
{
	Handle_اشاره_گر_سرریز(false, p_داده, ul_پایه, ul_نتیجه);
}

فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointer_سرریز_داده *p_داده,
					   بیعلامت دراز ul_پایه,
					   بیعلامت دراز ul_نتیجه)
{
	Handle_اشاره_گر_سرریز(true, p_داده, ul_پایه, ul_نتیجه);
}

فراغ __ubsan_handle_alignment_assumption(بنية CAlignment_Assumption_داده *p_داده,
					 بیعلامت دراز ul_اشاره_گر,
					 بیعلامت دراز ulAlignment,
					 بیعلامت دراز ul_جابه_جایی)
{
	HandleAlignmentAssumption(false, p_داده, ul_اشاره_گر, ulAlignment,
				  ul_جابه_جایی);
}

فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignment_Assumption_داده *p_داده, بیعلامت دراز ul_اشاره_گر,
	بیعلامت دراز ulAlignment, بیعلامت دراز ul_جابه_جایی)
{
	HandleAlignmentAssumption(true, p_داده, ul_اشاره_گر, ulAlignment,
				  ul_جابه_جایی);
}

فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachable_داده *p_داده)
{
	HandleBuiltinUnreachable(true, p_داده);
}

فراغ __ubsan_handle_invalid_builtin(بنية CInvalid_Builtin_داده *p_داده)
{
	HandleInvalidBuiltin(true, p_داده);
}

فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalid_Builtin_داده *p_داده)
{
	HandleInvalidBuiltin(true, p_داده);
}

فراغ __ubsan_handle_load_invalid_value(بنية CInvalid_مقدار_داده *p_داده,
				       بیعلامت دراز ul_مقدار)
{
	Handle_بارگیری_Invalid_مقدار(false, p_داده, ul_مقدار);
}

فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalid_مقدار_داده *p_داده,
					     بیعلامت دراز ul_مقدار)
{
	Handle_بارگیری_Invalid_مقدار(true, p_داده, ul_مقدار);
}

فراغ __ubsan_handle_missing_return(بنية CUnreachable_داده *p_داده)
{
	HandleMissingReturn(true, p_داده);
}

فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABound_داده *p_داده,
					   بیعلامت دراز ulBound)
{
	Handle_Vla_Bound_Not_مثبت(false, p_داده, ulBound);
}

فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABound_داده *p_داده,
						 بیعلامت دراز ulBound)
{
	Handle_Vla_Bound_Not_مثبت(true, p_داده, ulBound);
}

فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunction_Type_Mismatch_داده *p_داده, بیعلامت دراز ulFunction)
{
	HandleFunctionTypeMismatch(false, p_داده, ulFunction);
}

فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunction_Type_Mismatch_داده *p_داده, بیعلامت دراز ulFunction)
{
	HandleFunctionTypeMismatch(true, p_داده, ulFunction);
}
#پایان_اگر
