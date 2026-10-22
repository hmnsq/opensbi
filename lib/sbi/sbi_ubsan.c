/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#اگر_تعریف_شدہ UBSAN_ENABLED

#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#تعریف_کرو قسم_INTEGER 0
#تعریف_کرو قسم_FLOAT 1
#تعریف_کرو قسم_UNKNOWN UINT16_MAX

متلون غیرعلامتی طویل sbi_ubsan_report_گنتی;

ساخت CSourceLocation {
	حرف *mFilename;
	uint32_t m_سطر;
	uint32_t m_عمود;
};

ساخت CTypeDescriptor {
	uint16_t m_Type_قسم;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

ساخت COverflow_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mType;
};

ساخت CUnreachable_ڈیٹا {
	ساخت CSourceLocation mLocation;
};

ساخت CCFICheck_Fail_ڈیٹا {
	uint8_t m_Check_قسم;
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mType;
};

ساخت CDynamic_Type_Cache_ناکامی_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mType;
	خالی *mTypeInfo;
	uint8_t m_Type_Check_قسم;
};

ساخت CFunction_Type_Mismatch_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mType;
};

ساخت CInvalid_Builtin_ڈیٹا {
	ساخت CSourceLocation mLocation;
	uint8_t m_قسم;
};

ساخت CInvalid_قدر_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mType;
};

ساخت CNon_Null_Arg_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CSourceLocation mAttributeLocation;
	صحیح m_Arg_اشاریہ;
};

ساخت CNon_Null_Return_ڈیٹا {
	ساخت CSourceLocation mAttributeLocation;
};

ساخت COut_کا_Bounds_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mArrayType;
	ساخت CTypeDescriptor *m_اشاریہ_Type;
};

ساخت CPointer_حد_سے_تجاوز_ڈیٹا {
	ساخت CSourceLocation mLocation;
};

ساخت CShift_برآمد_کا_Bounds_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mLHSType;
	ساخت CTypeDescriptor *mRHSType;
};

ساخت CType_Mismatch_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mType;
	غیرعلامتی طویل mLogAlignment;
	uint8_t m_Type_Check_قسم;
};

ساخت CType_Mismatch_ڈیٹا_v1 {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_قسم;
};

ساخت CVLABound_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mType;
};

ساخت CFloat_Cast_حد_سے_تجاوز_ڈیٹا {
	ساخت CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	ساخت CTypeDescriptor *mFromType;
	ساخت CTypeDescriptor *m_مقصد_Type;
};

ساخت CImplicit_Conversion_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CTypeDescriptor *mFromType;
	ساخت CTypeDescriptor *m_مقصد_Type;
	uint8_t m_قسم;
};

ساخت CAlignment_Assumption_ڈیٹا {
	ساخت CSourceLocation mLocation;
	ساخت CSourceLocation mAssumptionLocation;
	ساخت CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
خالی __ubsan_handle_add_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				 غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_add_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulLHS,
				       غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_alignment_assumption(ساخت CAlignment_Assumption_ڈیٹا *p_ڈیٹا,
					 غیرعلامتی طویل ul_اشارہ_گر,
					 غیرعلامتی طویل ulAlignment,
					 غیرعلامتی طویل ul_انحراف);
خالی __ubsan_handle_alignment_assumption_abort(
	ساخت CAlignment_Assumption_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ul_اشارہ_گر,
	غیرعلامتی طویل ulAlignment, غیرعلامتی طویل ul_انحراف);
خالی __ubsan_handle_builtin_unreachable(ساخت CUnreachable_ڈیٹا *p_ڈیٹا);
خالی __ubsan_handle_divrem_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				    غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_divrem_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
					  غیرعلامتی طویل ulLHS,
					  غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_function_type_mismatch(
	ساخت CFunction_Type_Mismatch_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ulFunction);
خالی __ubsan_handle_function_type_mismatch_abort(
	ساخت CFunction_Type_Mismatch_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ulFunction);
خالی __ubsan_handle_function_type_mismatch_v1(
	ساخت CFunction_Type_Mismatch_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ulFunction,
	غیرعلامتی طویل ulCalleeRTTI, غیرعلامتی طویل ulFnRTTI);
خالی __ubsan_handle_function_type_mismatch_v1_abort(
	ساخت CFunction_Type_Mismatch_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ulFunction,
	غیرعلامتی طویل ulCalleeRTTI, غیرعلامتی طویل ulFnRTTI);
خالی __ubsan_handle_invalid_builtin(ساخت CInvalid_Builtin_ڈیٹا *p_ڈیٹا);
خالی __ubsan_handle_invalid_builtin_abort(ساخت CInvalid_Builtin_ڈیٹا *p_ڈیٹا);
خالی __ubsan_handle_load_invalid_value(ساخت CInvalid_قدر_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulVal);
خالی __ubsan_handle_load_invalid_value_abort(ساخت CInvalid_قدر_ڈیٹا *p_ڈیٹا,
					     غیرعلامتی طویل ulVal);
خالی __ubsan_handle_missing_return(ساخت CUnreachable_ڈیٹا *p_ڈیٹا);
خالی __ubsan_handle_mul_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				 غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_mul_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulLHS,
				       غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_negate_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				    غیرعلامتی طویل ulOldVal);
خالی __ubsan_handle_negate_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
					  غیرعلامتی طویل ulOldVal);
خالی __ubsan_handle_nullability_arg(ساخت CNon_Null_Arg_ڈیٹا *p_ڈیٹا);
خالی __ubsan_handle_nullability_arg_abort(ساخت CNon_Null_Arg_ڈیٹا *p_ڈیٹا);
خالی __ubsan_handle_nullability_return_v1(
	ساخت CNon_Null_Return_ڈیٹا *p_ڈیٹا,
	ساخت CSourceLocation *p_Location_اشارہ_گر);
خالی __ubsan_handle_nullability_return_v1_abort(
	ساخت CNon_Null_Return_ڈیٹا *p_ڈیٹا,
	ساخت CSourceLocation *p_Location_اشارہ_گر);
خالی __ubsan_handle_out_of_bounds(ساخت COut_کا_Bounds_ڈیٹا *p_ڈیٹا,
				  غیرعلامتی طویل ul_اشاریہ);
خالی __ubsan_handle_out_of_bounds_abort(ساخت COut_کا_Bounds_ڈیٹا *p_ڈیٹا,
					غیرعلامتی طویل ul_اشاریہ);
خالی __ubsan_handle_pointer_overflow(ساخت CPointer_حد_سے_تجاوز_ڈیٹا *p_ڈیٹا,
				     غیرعلامتی طویل ul_بنیاد,
				     غیرعلامتی طویل ul_نتیجہ);
خالی __ubsan_handle_pointer_overflow_abort(ساخت CPointer_حد_سے_تجاوز_ڈیٹا *p_ڈیٹا,
					   غیرعلامتی طویل ul_بنیاد,
					   غیرعلامتی طویل ul_نتیجہ);
خالی __ubsan_handle_shift_out_of_bounds(ساخت CShift_برآمد_کا_Bounds_ڈیٹا *p_ڈیٹا,
					غیرعلامتی طویل ulLHS,
					غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_shift_out_of_bounds_abort(
	ساخت CShift_برآمد_کا_Bounds_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ulLHS,
	غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_sub_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				 غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_sub_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulLHS,
				       غیرعلامتی طویل ulRHS);
خالی __ubsan_handle_type_mismatch(ساخت CType_Mismatch_ڈیٹا *p_ڈیٹا,
				  غیرعلامتی طویل ul_اشارہ_گر);
خالی __ubsan_handle_type_mismatch_abort(ساخت CType_Mismatch_ڈیٹا *p_ڈیٹا,
					غیرعلامتی طویل ul_اشارہ_گر);
خالی __ubsan_handle_type_mismatch_v1(ساخت CType_Mismatch_ڈیٹا_v1 *p_ڈیٹا,
				     غیرعلامتی طویل ul_اشارہ_گر);
خالی __ubsan_handle_type_mismatch_v1_abort(ساخت CType_Mismatch_ڈیٹا_v1 *p_ڈیٹا,
					   غیرعلامتی طویل ul_اشارہ_گر);
خالی __ubsan_handle_vla_bound_not_positive(ساخت CVLABound_ڈیٹا *p_ڈیٹا,
					   غیرعلامتی طویل ulBound);
خالی __ubsan_handle_vla_bound_not_positive_abort(ساخت CVLABound_ڈیٹا *p_ڈیٹا,
						 غیرعلامتی طویل ulBound);
خالی __ubsan_get_current_report_data(ثابت حرف **pp_برآمد_Issue_قسم,
				     ثابت حرف **pp_برآمد_پیغام,
				     ثابت حرف **pp_برآمد_Filename,
				     uint32_t *p_برآمد_سطر, uint32_t *p_برآمد_ستون,
				     حرف **pp_برآمد_حافظہ_Addr);
ساکن خالی Handle_حد_سے_تجاوز(bool isFatal, ساخت COverflow_ڈیٹا *p_ڈیٹا,
			   غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS,
			   ثابت حرف *szOperation);
ساکن خالی Handle_Negate_حد_سے_تجاوز(bool isFatal, ساخت COverflow_ڈیٹا *p_ڈیٹا,
				 غیرعلامتی طویل ul_Old_قدر);
ساکن خالی HandleBuiltinUnreachable(bool isFatal,
				     ساخت CUnreachable_ڈیٹا *p_ڈیٹا);
ساکن خالی HandleTypeMismatch(bool isFatal, ساخت CSourceLocation *mLocation,
			       ساخت CTypeDescriptor *mType,
			       غیرعلامتی طویل mLogAlignment,
			       uint8_t m_Type_Check_قسم, غیرعلامتی طویل ul_اشارہ_گر);
ساکن خالی Handle_Vla_Bound_Not_مثبت(bool isFatal, ساخت CVLABound_ڈیٹا *p_ڈیٹا,
				      غیرعلامتی طویل ulBound);
ساکن خالی Handle_برآمد_کا_Bounds(bool isFatal, ساخت COut_کا_Bounds_ڈیٹا *p_ڈیٹا,
			      غیرعلامتی طویل ul_اشاریہ);
ساکن خالی Handle_سرکاؤ_برآمد_کا_Bounds(bool isFatal,
				   ساخت CShift_برآمد_کا_Bounds_ڈیٹا *p_ڈیٹا,
				   غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS);
ساکن خالی Handle_بھرو_Invalid_قدر(bool isFatal,
				   ساخت CInvalid_قدر_ڈیٹا *p_ڈیٹا,
				   غیرعلامتی طویل ul_قدر);
ساکن خالی HandleInvalidBuiltin(bool isFatal,
				 ساخت CInvalid_Builtin_ڈیٹا *p_ڈیٹا);
ساکن خالی HandleFunctionTypeMismatch(bool isFatal,
				       ساخت CFunction_Type_Mismatch_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulFunction);
ساکن خالی HandleMissingReturn(bool isFatal, ساخت CUnreachable_ڈیٹا *p_ڈیٹا);
ساکن خالی Handle_اشارہ_گر_حد_سے_تجاوز(bool isFatal,
				  ساخت CPointer_حد_سے_تجاوز_ڈیٹا *p_ڈیٹا,
				  غیرعلامتی طویل ul_بنیاد, غیرعلامتی طویل ul_نتیجہ);
ساکن خالی HandleAlignmentAssumption(bool isFatal,
				      ساخت CAlignment_Assumption_ڈیٹا *p_ڈیٹا,
				      غیرعلامتی طویل ul_اشارہ_گر,
				      غیرعلامتی طویل ulAlignment,
				      غیرعلامتی طویل ul_انحراف);

#تعریف_کرو نمبر_SIGNED_ثنائی_ہندسہ 1
#تعریف_کرو نمبر_MAXLEN 128
#تعریف_کرو __arraycount(__a) (حجم(__a) / حجم(__a[0]))
#تعریف_کرو __BIT(__n) (1UL << (__n))
#تعریف_کرو SEPARATOR sbi_printf("===========================================\n")
#تعریف_کرو ACK_REPORTED (1U << 31)

ساکن bool isAlreadyReported(ساخت CSourceLocation *pLocation)
{
	uint32_t si_Old_قدر;
	متلون uint32_t *p_سطر;

	اگر (!pLocation)
		واپس false;

	p_سطر = &pLocation->m_سطر;

	کرو {
		si_Old_قدر = *p_سطر;
	} جب (__sync_val_compare_and_swap(p_سطر, si_Old_قدر,
					     si_Old_قدر | ACK_REPORTED) !=
		 si_Old_قدر);

	اگر (!(si_Old_قدر & ACK_REPORTED)) {

		sbi_ubsan_report_گنتی++;

		واپس false;
	}

	واپس true;
}

ساکن خالی Handle_حد_سے_تجاوز(bool isFatal, ساخت COverflow_ڈیٹا *p_ڈیٹا,
			   غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS,
			   ثابت حرف *szOperation)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}

	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	bool is_signed = p_ڈیٹا->mType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	اگر (is_signed) {
		sbi_printf("%ld %s %ld ", (طویل)ulLHS, szOperation,
			   (طویل)ulRHS);
	} ورنہ {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_ڈیٹا->mType->mTypeName);

	SEPARATOR;
}

ساکن خالی Handle_Negate_حد_سے_تجاوز(bool isFatal, ساخت COverflow_ڈیٹا *p_ڈیٹا,
				 غیرعلامتی طویل ul_Old_قدر)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}
	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	bool is_signed = p_ڈیٹا->mType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	sbi_printf("UBSan: Negation of ");

	اگر (is_signed) {
		sbi_printf("%ld", (طویل)ul_Old_قدر);
	} ورنہ {
		sbi_printf("%lu", ul_Old_قدر);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_ڈیٹا->mType->mTypeName);

	SEPARATOR;
}

ساکن خالی HandleBuiltinUnreachable(bool isFatal,
				     ساخت CUnreachable_ڈیٹا *p_ڈیٹا)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}
	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

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

ساکن خالی HandleTypeMismatch(bool isFatal, ساخت CSourceLocation *mLocation,
			       ساخت CTypeDescriptor *mType,
			       غیرعلامتی طویل mLogAlignment,
			       uint8_t m_Type_Check_قسم, غیرعلامتی طویل ul_اشارہ_گر)
{

	اگر ((!mLocation) || (!mType)) {
		واپس;
	}

	اگر (isAlreadyReported(mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_سطر & ~ACK_REPORTED,
		   mLocation->m_عمود);

	ثابت حرف *قسم_2 = (m_Type_Check_قسم < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_قسم]
				   : "access to";

	اگر (ul_اشارہ_گر == 0) {
		sbi_printf("%s null pointer of type %s\n", قسم_2,
			   mType->mTypeName);
	} ورنہ اگر (
		(mLogAlignment - 1) &
		ul_اشارہ_گر) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			قسم_2, (خالی *)ul_اشارہ_گر, mType->mTypeName,
			mLogAlignment);
	} ورنہ {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			قسم_2, (خالی *)ul_اشارہ_گر, mType->mTypeName);
	}
	SEPARATOR;
}

ساکن خالی Handle_Vla_Bound_Not_مثبت(bool isFatal, ساخت CVLABound_ڈیٹا *p_ڈیٹا,
				      غیرعلامتی طویل ulBound)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}
	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_ڈیٹا->mType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	اگر (is_signed) {
		sbi_printf("%ld", (طویل)ulBound);
	} ورنہ {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ساکن خالی Handle_برآمد_کا_Bounds(bool isFatal, ساخت COut_کا_Bounds_ڈیٹا *p_ڈیٹا,
			      غیرعلامتی طویل ul_اشاریہ)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}
	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	bool is_signed = p_ڈیٹا->m_اشاریہ_Type->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	اگر (is_signed) {
		sbi_printf("index %ld", (طویل)ul_اشاریہ);
	} ورنہ {
		sbi_printf("index %lu", ul_اشاریہ);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_ڈیٹا->mArrayType->mTypeName);

	SEPARATOR;
}

ساکن bool is_Negative_نمبر(ساخت CTypeDescriptor *pType, غیرعلامتی طویل ulVal)
{
	اگر (!(pType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ)) {
		واپس false;
	}

	واپس (طویل)ulVal < 0;
}

ساکن size_t type_چوڑائی(ساخت CTypeDescriptor *pType)
{
	واپس 1UL << (pType->mTypeInfo >> 1);
}

ساکن خالی Handle_سرکاؤ_برآمد_کا_Bounds(bool isFatal,
				   ساخت CShift_برآمد_کا_Bounds_ڈیٹا *p_ڈیٹا,
				   غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}

	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	اگر (is_Negative_نمبر(p_ڈیٹا->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (طویل)ulRHS);
	} ورنہ اگر (ulRHS >= type_چوڑائی(p_ڈیٹا->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (غیرعلامتی طویل)type_چوڑائی(p_ڈیٹا->mLHSType),
			p_ڈیٹا->mLHSType->mTypeName);

	} ورنہ اگر (is_Negative_نمبر(p_ڈیٹا->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (طویل)ulLHS);
	} ورنہ {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_ڈیٹا->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ساکن خالی Handle_بھرو_Invalid_قدر(bool isFatal,
				   ساخت CInvalid_قدر_ڈیٹا *p_ڈیٹا,
				   غیرعلامتی طویل ul_قدر)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}

	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	bool is_signed = p_ڈیٹا->mType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	sbi_printf("load of value ");

	اگر (is_signed) {
		sbi_printf("%ld ", (طویل)ul_قدر);
	} ورنہ {
		sbi_printf("%lu ", ul_قدر);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_ڈیٹا->mType->mTypeName);

	SEPARATOR;
}

ثابت حرف *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ساکن خالی HandleInvalidBuiltin(bool isFatal,
				 ساخت CInvalid_Builtin_ڈیٹا *p_ڈیٹا)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}

	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	ثابت حرف *builtin =
		(p_ڈیٹا->m_قسم < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_ڈیٹا->m_قسم]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ساکن خالی HandleFunctionTypeMismatch(bool isFatal,
				       ساخت CFunction_Type_Mismatch_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulFunction)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}

	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_ڈیٹا->mType->mTypeName);

	SEPARATOR;
}

ساکن خالی HandleMissingReturn(bool isFatal, ساخت CUnreachable_ڈیٹا *p_ڈیٹا)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}

	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ساکن خالی Handle_اشارہ_گر_حد_سے_تجاوز(bool isFatal,
				  ساخت CPointer_حد_سے_تجاوز_ڈیٹا *p_ڈیٹا,
				  غیرعلامتی طویل ul_بنیاد, غیرعلامتی طویل ul_نتیجہ)
{
	اگر (!p_ڈیٹا) {
		واپس;
	}

	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_بنیاد, ul_نتیجہ);

	SEPARATOR;
}

ساکن خالی HandleAlignmentAssumption(bool isFatal,
				      ساخت CAlignment_Assumption_ڈیٹا *p_ڈیٹا,
				      غیرعلامتی طویل ul_اشارہ_گر,
				      غیرعلامتی طویل ulAlignment,
				      غیرعلامتی طویل ul_انحراف)
{

	اگر (!p_ڈیٹا) {
		واپس;
	}

	اگر (isAlreadyReported(&p_ڈیٹا->mLocation)) {
		واپس;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ڈیٹا->mLocation.mFilename,
		   p_ڈیٹا->mLocation.m_سطر & ~ACK_REPORTED,
		   p_ڈیٹا->mLocation.m_عمود);

	غیرعلامتی طویل ul_Real_اشارہ_گر = ul_اشارہ_گر - ul_انحراف;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (خالی *)ul_Real_اشارہ_گر, (خالی *)ul_انحراف);

	اگر (p_ڈیٹا->mAssumptionLocation.mFilename != تہی) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_ڈیٹا->mAssumptionLocation.mFilename,
			   p_ڈیٹا->mAssumptionLocation.m_سطر,
			   p_ڈیٹا->mAssumptionLocation.m_عمود);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
خالی __ubsan_handle_add_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				 غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS)
{
	Handle_حد_سے_تجاوز(false, p_ڈیٹا, ulLHS, ulRHS, "+");
}

خالی __ubsan_handle_add_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS)
{
	Handle_حد_سے_تجاوز(true, p_ڈیٹا, ulLHS, ulRHS, "+");
}

خالی __ubsan_handle_sub_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				 غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS)
{
	Handle_حد_سے_تجاوز(false, p_ڈیٹا, ulLHS, ulRHS, "-");
}

خالی __ubsan_handle_sub_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS)
{
	Handle_حد_سے_تجاوز(true, p_ڈیٹا, ulLHS, ulRHS, "-");
}

خالی __ubsan_handle_mul_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				 غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS)
{
	Handle_حد_سے_تجاوز(false, p_ڈیٹا, ulLHS, ulRHS, "*");
}

خالی __ubsan_handle_mul_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS)
{
	Handle_حد_سے_تجاوز(true, p_ڈیٹا, ulLHS, ulRHS, "*");
}

خالی __ubsan_handle_negate_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				    غیرعلامتی طویل ul_Old_قدر)
{
	Handle_Negate_حد_سے_تجاوز(false, p_ڈیٹا, ul_Old_قدر);
}

خالی __ubsan_handle_negate_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
					  غیرعلامتی طویل ul_Old_قدر)
{
	Handle_Negate_حد_سے_تجاوز(true, p_ڈیٹا, ul_Old_قدر);
}

خالی __ubsan_handle_divrem_overflow(ساخت COverflow_ڈیٹا *p_ڈیٹا,
				    غیرعلامتی طویل ulLHS, غیرعلامتی طویل ulRHS)
{
	Handle_حد_سے_تجاوز(false, p_ڈیٹا, ulLHS, ulRHS, "divrem");
}

خالی __ubsan_handle_divrem_overflow_abort(ساخت COverflow_ڈیٹا *p_ڈیٹا,
					  غیرعلامتی طویل ulLHS,
					  غیرعلامتی طویل ulRHS)
{
	Handle_حد_سے_تجاوز(true, p_ڈیٹا, ulLHS, ulRHS, "divrem");
}

خالی __ubsan_handle_type_mismatch_v1(ساخت CType_Mismatch_ڈیٹا_v1 *p_ڈیٹا,
				     غیرعلامتی طویل ul_اشارہ_گر)
{
	HandleTypeMismatch(false, &p_ڈیٹا->mLocation, p_ڈیٹا->mType,
			   __BIT(p_ڈیٹا->mLogAlignment), p_ڈیٹا->m_Type_Check_قسم,
			   ul_اشارہ_گر);
}

خالی __ubsan_handle_type_mismatch_v1_abort(ساخت CType_Mismatch_ڈیٹا_v1 *p_ڈیٹا,
					   غیرعلامتی طویل ul_اشارہ_گر)
{
	HandleTypeMismatch(true, &p_ڈیٹا->mLocation, p_ڈیٹا->mType,
			   __BIT(p_ڈیٹا->mLogAlignment), p_ڈیٹا->m_Type_Check_قسم,
			   ul_اشارہ_گر);
}

خالی __ubsan_handle_out_of_bounds(ساخت COut_کا_Bounds_ڈیٹا *p_ڈیٹا,
				  غیرعلامتی طویل ul_اشاریہ)
{
	Handle_برآمد_کا_Bounds(false, p_ڈیٹا, ul_اشاریہ);
}

خالی __ubsan_handle_out_of_bounds_abort(ساخت COut_کا_Bounds_ڈیٹا *p_ڈیٹا,
					غیرعلامتی طویل ul_اشاریہ)
{
	Handle_برآمد_کا_Bounds(true, p_ڈیٹا, ul_اشاریہ);
}

خالی __ubsan_handle_shift_out_of_bounds(ساخت CShift_برآمد_کا_Bounds_ڈیٹا *p_ڈیٹا,
					غیرعلامتی طویل ulLHS,
					غیرعلامتی طویل ulRHS)
{
	Handle_سرکاؤ_برآمد_کا_Bounds(false, p_ڈیٹا, ulLHS, ulRHS);
}

خالی __ubsan_handle_shift_out_of_bounds_abort(
	ساخت CShift_برآمد_کا_Bounds_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ulLHS,
	غیرعلامتی طویل ulRHS)
{
	Handle_سرکاؤ_برآمد_کا_Bounds(true, p_ڈیٹا, ulLHS, ulRHS);
}

خالی __ubsan_handle_pointer_overflow(ساخت CPointer_حد_سے_تجاوز_ڈیٹا *p_ڈیٹا,
				     غیرعلامتی طویل ul_بنیاد,
				     غیرعلامتی طویل ul_نتیجہ)
{
	Handle_اشارہ_گر_حد_سے_تجاوز(false, p_ڈیٹا, ul_بنیاد, ul_نتیجہ);
}

خالی __ubsan_handle_pointer_overflow_abort(ساخت CPointer_حد_سے_تجاوز_ڈیٹا *p_ڈیٹا,
					   غیرعلامتی طویل ul_بنیاد,
					   غیرعلامتی طویل ul_نتیجہ)
{
	Handle_اشارہ_گر_حد_سے_تجاوز(true, p_ڈیٹا, ul_بنیاد, ul_نتیجہ);
}

خالی __ubsan_handle_alignment_assumption(ساخت CAlignment_Assumption_ڈیٹا *p_ڈیٹا,
					 غیرعلامتی طویل ul_اشارہ_گر,
					 غیرعلامتی طویل ulAlignment,
					 غیرعلامتی طویل ul_انحراف)
{
	HandleAlignmentAssumption(false, p_ڈیٹا, ul_اشارہ_گر, ulAlignment,
				  ul_انحراف);
}

خالی __ubsan_handle_alignment_assumption_abort(
	ساخت CAlignment_Assumption_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ul_اشارہ_گر,
	غیرعلامتی طویل ulAlignment, غیرعلامتی طویل ul_انحراف)
{
	HandleAlignmentAssumption(true, p_ڈیٹا, ul_اشارہ_گر, ulAlignment,
				  ul_انحراف);
}

خالی __ubsan_handle_builtin_unreachable(ساخت CUnreachable_ڈیٹا *p_ڈیٹا)
{
	HandleBuiltinUnreachable(true, p_ڈیٹا);
}

خالی __ubsan_handle_invalid_builtin(ساخت CInvalid_Builtin_ڈیٹا *p_ڈیٹا)
{
	HandleInvalidBuiltin(true, p_ڈیٹا);
}

خالی __ubsan_handle_invalid_builtin_abort(ساخت CInvalid_Builtin_ڈیٹا *p_ڈیٹا)
{
	HandleInvalidBuiltin(true, p_ڈیٹا);
}

خالی __ubsan_handle_load_invalid_value(ساخت CInvalid_قدر_ڈیٹا *p_ڈیٹا,
				       غیرعلامتی طویل ul_قدر)
{
	Handle_بھرو_Invalid_قدر(false, p_ڈیٹا, ul_قدر);
}

خالی __ubsan_handle_load_invalid_value_abort(ساخت CInvalid_قدر_ڈیٹا *p_ڈیٹا,
					     غیرعلامتی طویل ul_قدر)
{
	Handle_بھرو_Invalid_قدر(true, p_ڈیٹا, ul_قدر);
}

خالی __ubsan_handle_missing_return(ساخت CUnreachable_ڈیٹا *p_ڈیٹا)
{
	HandleMissingReturn(true, p_ڈیٹا);
}

خالی __ubsan_handle_vla_bound_not_positive(ساخت CVLABound_ڈیٹا *p_ڈیٹا,
					   غیرعلامتی طویل ulBound)
{
	Handle_Vla_Bound_Not_مثبت(false, p_ڈیٹا, ulBound);
}

خالی __ubsan_handle_vla_bound_not_positive_abort(ساخت CVLABound_ڈیٹا *p_ڈیٹا,
						 غیرعلامتی طویل ulBound)
{
	Handle_Vla_Bound_Not_مثبت(true, p_ڈیٹا, ulBound);
}

خالی __ubsan_handle_function_type_mismatch(
	ساخت CFunction_Type_Mismatch_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ulFunction)
{
	HandleFunctionTypeMismatch(false, p_ڈیٹا, ulFunction);
}

خالی __ubsan_handle_function_type_mismatch_abort(
	ساخت CFunction_Type_Mismatch_ڈیٹا *p_ڈیٹا, غیرعلامتی طویل ulFunction)
{
	HandleFunctionTypeMismatch(true, p_ڈیٹا, ulFunction);
}
#اگر_ختم
