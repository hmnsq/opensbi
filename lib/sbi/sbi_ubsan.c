/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UBSAN_ENABLED

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕਿਸਮ_INTEGER 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕਿਸਮ_FLOAT 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕਿਸਮ_UNKNOWN UINT16_MAX

ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_ubsan_report_ਗਿਣਤੀ;

ਬਣਤਰ CSourceLocation {
	ਅੱਖਰ *mFilename;
	uint32_t m_ਸਤਰ;
	uint32_t m_ਕਾਲਮ;
};

ਬਣਤਰ CTypeDescriptor {
	uint16_t m_Type_ਕਿਸਮ;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

ਬਣਤਰ COverflow_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
};

ਬਣਤਰ CUnreachable_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
};

ਬਣਤਰ CCFICheck_Fail_ਅੰਕੜੇ {
	uint8_t m_Check_ਕਿਸਮ;
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
};

ਬਣਤਰ CDynamic_Type_Cache_ਖੁੰਝ_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
	ਖਾਲੀ *mTypeInfo;
	uint8_t m_Type_Check_ਕਿਸਮ;
};

ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
};

ਬਣਤਰ CInvalid_Builtin_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	uint8_t m_ਕਿਸਮ;
};

ਬਣਤਰ CInvalid_ਮੁੱਲ_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
};

ਬਣਤਰ CNon_Null_Arg_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CSourceLocation mAttributeLocation;
	ਪੂਰਨ m_Arg_ਸੂਚਕਾਂਕ;
};

ਬਣਤਰ CNon_Null_Return_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mAttributeLocation;
};

ਬਣਤਰ COut_ਦਾ_Bounds_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mArrayType;
	ਬਣਤਰ CTypeDescriptor *m_ਸੂਚਕਾਂਕ_Type;
};

ਬਣਤਰ CPointer_ਉਛਾਲ_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
};

ਬਣਤਰ CShift_ਬਾਹਰਲਾ_ਦਾ_Bounds_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mLHSType;
	ਬਣਤਰ CTypeDescriptor *mRHSType;
};

ਬਣਤਰ CType_Mismatch_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mLogAlignment;
	uint8_t m_Type_Check_ਕਿਸਮ;
};

ਬਣਤਰ CType_Mismatch_ਅੰਕੜੇ_v1 {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_ਕਿਸਮ;
};

ਬਣਤਰ CVLABound_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
};

ਬਣਤਰ CFloat_Cast_ਉਛਾਲ_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	ਬਣਤਰ CTypeDescriptor *mFromType;
	ਬਣਤਰ CTypeDescriptor *m_ਮੰਜ਼ਿਲ_Type;
};

ਬਣਤਰ CImplicit_Conversion_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CTypeDescriptor *mFromType;
	ਬਣਤਰ CTypeDescriptor *m_ਮੰਜ਼ਿਲ_Type;
	uint8_t m_ਕਿਸਮ;
};

ਬਣਤਰ CAlignment_Assumption_ਅੰਕੜੇ {
	ਬਣਤਰ CSourceLocation mLocation;
	ਬਣਤਰ CSourceLocation mAssumptionLocation;
	ਬਣਤਰ CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
ਖਾਲੀ __ubsan_handle_add_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_add_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_alignment_assumption(ਬਣਤਰ CAlignment_Assumption_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulAlignment,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਵਿੱਥ);
ਖਾਲੀ __ubsan_handle_alignment_assumption_abort(
	ਬਣਤਰ CAlignment_Assumption_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ,
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulAlignment, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਵਿੱਥ);
ਖਾਲੀ __ubsan_handle_builtin_unreachable(ਬਣਤਰ CUnreachable_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਖਾਲੀ __ubsan_handle_divrem_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_divrem_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
					  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_function_type_mismatch(
	ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFunction);
ਖਾਲੀ __ubsan_handle_function_type_mismatch_abort(
	ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFunction);
ਖਾਲੀ __ubsan_handle_function_type_mismatch_v1(
	ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFunction,
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulCalleeRTTI, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFnRTTI);
ਖਾਲੀ __ubsan_handle_function_type_mismatch_v1_abort(
	ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFunction,
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulCalleeRTTI, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFnRTTI);
ਖਾਲੀ __ubsan_handle_invalid_builtin(ਬਣਤਰ CInvalid_Builtin_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਖਾਲੀ __ubsan_handle_invalid_builtin_abort(ਬਣਤਰ CInvalid_Builtin_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਖਾਲੀ __ubsan_handle_load_invalid_value(ਬਣਤਰ CInvalid_ਮੁੱਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulVal);
ਖਾਲੀ __ubsan_handle_load_invalid_value_abort(ਬਣਤਰ CInvalid_ਮੁੱਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulVal);
ਖਾਲੀ __ubsan_handle_missing_return(ਬਣਤਰ CUnreachable_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਖਾਲੀ __ubsan_handle_mul_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_mul_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_negate_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulOldVal);
ਖਾਲੀ __ubsan_handle_negate_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulOldVal);
ਖਾਲੀ __ubsan_handle_nullability_arg(ਬਣਤਰ CNon_Null_Arg_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਖਾਲੀ __ubsan_handle_nullability_arg_abort(ਬਣਤਰ CNon_Null_Arg_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਖਾਲੀ __ubsan_handle_nullability_return_v1(
	ਬਣਤਰ CNon_Null_Return_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
	ਬਣਤਰ CSourceLocation *p_Location_ਸੂਚਕ);
ਖਾਲੀ __ubsan_handle_nullability_return_v1_abort(
	ਬਣਤਰ CNon_Null_Return_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
	ਬਣਤਰ CSourceLocation *p_Location_ਸੂਚਕ);
ਖਾਲੀ __ubsan_handle_out_of_bounds(ਬਣਤਰ COut_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕਾਂਕ);
ਖਾਲੀ __ubsan_handle_out_of_bounds_abort(ਬਣਤਰ COut_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕਾਂਕ);
ਖਾਲੀ __ubsan_handle_pointer_overflow(ਬਣਤਰ CPointer_ਉਛਾਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਅਧਾਰ,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਨਤੀਜਾ);
ਖਾਲੀ __ubsan_handle_pointer_overflow_abort(ਬਣਤਰ CPointer_ਉਛਾਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਅਧਾਰ,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਨਤੀਜਾ);
ਖਾਲੀ __ubsan_handle_shift_out_of_bounds(ਬਣਤਰ CShift_ਬਾਹਰਲਾ_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_shift_out_of_bounds_abort(
	ਬਣਤਰ CShift_ਬਾਹਰਲਾ_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_sub_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_sub_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਖਾਲੀ __ubsan_handle_type_mismatch(ਬਣਤਰ CType_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ);
ਖਾਲੀ __ubsan_handle_type_mismatch_abort(ਬਣਤਰ CType_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ);
ਖਾਲੀ __ubsan_handle_type_mismatch_v1(ਬਣਤਰ CType_Mismatch_ਅੰਕੜੇ_v1 *p_ਅੰਕੜੇ,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ);
ਖਾਲੀ __ubsan_handle_type_mismatch_v1_abort(ਬਣਤਰ CType_Mismatch_ਅੰਕੜੇ_v1 *p_ਅੰਕੜੇ,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ);
ਖਾਲੀ __ubsan_handle_vla_bound_not_positive(ਬਣਤਰ CVLABound_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulBound);
ਖਾਲੀ __ubsan_handle_vla_bound_not_positive_abort(ਬਣਤਰ CVLABound_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
						 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulBound);
ਖਾਲੀ __ubsan_get_current_report_data(ਅਚਲ ਅੱਖਰ **pp_ਬਾਹਰਲਾ_Issue_ਕਿਸਮ,
				     ਅਚਲ ਅੱਖਰ **pp_ਬਾਹਰਲਾ_ਸੁਨੇਹਾ,
				     ਅਚਲ ਅੱਖਰ **pp_ਬਾਹਰਲਾ_Filename,
				     uint32_t *p_ਬਾਹਰਲਾ_ਸਤਰ, uint32_t *p_ਬਾਹਰਲਾ_ਖਾਨਾ,
				     ਅੱਖਰ **pp_ਬਾਹਰਲਾ_ਸਮ੍ਰਿਤੀ_Addr);
ਸਥਿਰ ਖਾਲੀ Handle_ਉਛਾਲ(bool isFatal, ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS,
			   ਅਚਲ ਅੱਖਰ *szOperation);
ਸਥਿਰ ਖਾਲੀ Handle_Negate_ਉਛਾਲ(bool isFatal, ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_Old_ਮੁੱਲ);
ਸਥਿਰ ਖਾਲੀ HandleBuiltinUnreachable(bool isFatal,
				     ਬਣਤਰ CUnreachable_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ HandleTypeMismatch(bool isFatal, ਬਣਤਰ CSourceLocation *mLocation,
			       ਬਣਤਰ CTypeDescriptor *mType,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mLogAlignment,
			       uint8_t m_Type_Check_ਕਿਸਮ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ);
ਸਥਿਰ ਖਾਲੀ Handle_Vla_Bound_Not_ਸਕਾਰਾਤਮਕ(bool isFatal, ਬਣਤਰ CVLABound_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulBound);
ਸਥਿਰ ਖਾਲੀ Handle_ਬਾਹਰਲਾ_ਦਾ_Bounds(bool isFatal, ਬਣਤਰ COut_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕਾਂਕ);
ਸਥਿਰ ਖਾਲੀ Handle_ਖਿਸਕਾਓ_ਬਾਹਰਲਾ_ਦਾ_Bounds(bool isFatal,
				   ਬਣਤਰ CShift_ਬਾਹਰਲਾ_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS);
ਸਥਿਰ ਖਾਲੀ Handle_ਲੱਦੋ_Invalid_ਮੁੱਲ(bool isFatal,
				   ਬਣਤਰ CInvalid_ਮੁੱਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਮੁੱਲ);
ਸਥਿਰ ਖਾਲੀ HandleInvalidBuiltin(bool isFatal,
				 ਬਣਤਰ CInvalid_Builtin_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ HandleFunctionTypeMismatch(bool isFatal,
				       ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFunction);
ਸਥਿਰ ਖਾਲੀ HandleMissingReturn(bool isFatal, ਬਣਤਰ CUnreachable_ਅੰਕੜੇ *p_ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ Handle_ਸੂਚਕ_ਉਛਾਲ(bool isFatal,
				  ਬਣਤਰ CPointer_ਉਛਾਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਨਤੀਜਾ);
ਸਥਿਰ ਖਾਲੀ HandleAlignmentAssumption(bool isFatal,
				      ਬਣਤਰ CAlignment_Assumption_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulAlignment,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਵਿੱਥ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਖਿਆ_SIGNED_ਦੋਆਧਾਰੀ_ਅੰਕ 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਖਿਆ_MAXLEN 128
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __arraycount(__a) (ਆਕਾਰ(__a) / ਆਕਾਰ(__a[0]))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __BIT(__n) (1UL << (__n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEPARATOR sbi_printf("===========================================\n")
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACK_REPORTED (1U << 31)

ਸਥਿਰ bool isAlreadyReported(ਬਣਤਰ CSourceLocation *pLocation)
{
	uint32_t si_Old_ਮੁੱਲ;
	ਅਸਥਿਰ uint32_t *p_ਸਤਰ;

	ਜੇ (!pLocation)
		ਵਾਪਸ false;

	p_ਸਤਰ = &pLocation->m_ਸਤਰ;

	ਕਰੋ {
		si_Old_ਮੁੱਲ = *p_ਸਤਰ;
	} ਜਦੋਂ (__sync_val_compare_and_swap(p_ਸਤਰ, si_Old_ਮੁੱਲ,
					     si_Old_ਮੁੱਲ | ACK_REPORTED) !=
		 si_Old_ਮੁੱਲ);

	ਜੇ (!(si_Old_ਮੁੱਲ & ACK_REPORTED)) {

		sbi_ubsan_report_ਗਿਣਤੀ++;

		ਵਾਪਸ false;
	}

	ਵਾਪਸ true;
}

ਸਥਿਰ ਖਾਲੀ Handle_ਉਛਾਲ(bool isFatal, ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS,
			   ਅਚਲ ਅੱਖਰ *szOperation)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	bool is_signed = p_ਅੰਕੜੇ->mType->mTypeInfo & ਸੰਖਿਆ_SIGNED_ਦੋਆਧਾਰੀ_ਅੰਕ;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	ਜੇ (is_signed) {
		sbi_printf("%ld %s %ld ", (ਲੰਮਾ)ulLHS, szOperation,
			   (ਲੰਮਾ)ulRHS);
	} ਨਹੀਂਤਾਂ {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_ਅੰਕੜੇ->mType->mTypeName);

	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ Handle_Negate_ਉਛਾਲ(bool isFatal, ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_Old_ਮੁੱਲ)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}
	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	bool is_signed = p_ਅੰਕੜੇ->mType->mTypeInfo & ਸੰਖਿਆ_SIGNED_ਦੋਆਧਾਰੀ_ਅੰਕ;

	sbi_printf("UBSan: Negation of ");

	ਜੇ (is_signed) {
		sbi_printf("%ld", (ਲੰਮਾ)ul_Old_ਮੁੱਲ);
	} ਨਹੀਂਤਾਂ {
		sbi_printf("%lu", ul_Old_ਮੁੱਲ);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_ਅੰਕੜੇ->mType->mTypeName);

	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ HandleBuiltinUnreachable(bool isFatal,
				     ਬਣਤਰ CUnreachable_ਅੰਕੜੇ *p_ਅੰਕੜੇ)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}
	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	SEPARATOR;
}

ਅਚਲ ਅੱਖਰ *rgczTypeCheckKinds[] = { "load of",
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

ਸਥਿਰ ਖਾਲੀ HandleTypeMismatch(bool isFatal, ਬਣਤਰ CSourceLocation *mLocation,
			       ਬਣਤਰ CTypeDescriptor *mType,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mLogAlignment,
			       uint8_t m_Type_Check_ਕਿਸਮ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ)
{

	ਜੇ ((!mLocation) || (!mType)) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_ਸਤਰ & ~ACK_REPORTED,
		   mLocation->m_ਕਾਲਮ);

	ਅਚਲ ਅੱਖਰ *ਕਿਸਮ_2 = (m_Type_Check_ਕਿਸਮ < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_ਕਿਸਮ]
				   : "access to";

	ਜੇ (ul_ਸੂਚਕ == 0) {
		sbi_printf("%s null pointer of type %s\n", ਕਿਸਮ_2,
			   mType->mTypeName);
	} ਨਹੀਂਤਾਂ ਜੇ (
		(mLogAlignment - 1) &
		ul_ਸੂਚਕ) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			ਕਿਸਮ_2, (ਖਾਲੀ *)ul_ਸੂਚਕ, mType->mTypeName,
			mLogAlignment);
	} ਨਹੀਂਤਾਂ {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			ਕਿਸਮ_2, (ਖਾਲੀ *)ul_ਸੂਚਕ, mType->mTypeName);
	}
	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ Handle_Vla_Bound_Not_ਸਕਾਰਾਤਮਕ(bool isFatal, ਬਣਤਰ CVLABound_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulBound)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}
	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_ਅੰਕੜੇ->mType->mTypeInfo & ਸੰਖਿਆ_SIGNED_ਦੋਆਧਾਰੀ_ਅੰਕ;

	ਜੇ (is_signed) {
		sbi_printf("%ld", (ਲੰਮਾ)ulBound);
	} ਨਹੀਂਤਾਂ {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ Handle_ਬਾਹਰਲਾ_ਦਾ_Bounds(bool isFatal, ਬਣਤਰ COut_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕਾਂਕ)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}
	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	bool is_signed = p_ਅੰਕੜੇ->m_ਸੂਚਕਾਂਕ_Type->mTypeInfo & ਸੰਖਿਆ_SIGNED_ਦੋਆਧਾਰੀ_ਅੰਕ;

	ਜੇ (is_signed) {
		sbi_printf("index %ld", (ਲੰਮਾ)ul_ਸੂਚਕਾਂਕ);
	} ਨਹੀਂਤਾਂ {
		sbi_printf("index %lu", ul_ਸੂਚਕਾਂਕ);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_ਅੰਕੜੇ->mArrayType->mTypeName);

	SEPARATOR;
}

ਸਥਿਰ bool is_Negative_ਸੰਖਿਆ(ਬਣਤਰ CTypeDescriptor *pType, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulVal)
{
	ਜੇ (!(pType->mTypeInfo & ਸੰਖਿਆ_SIGNED_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
		ਵਾਪਸ false;
	}

	ਵਾਪਸ (ਲੰਮਾ)ulVal < 0;
}

ਸਥਿਰ size_t type_ਚੌੜਾਈ(ਬਣਤਰ CTypeDescriptor *pType)
{
	ਵਾਪਸ 1UL << (pType->mTypeInfo >> 1);
}

ਸਥਿਰ ਖਾਲੀ Handle_ਖਿਸਕਾਓ_ਬਾਹਰਲਾ_ਦਾ_Bounds(bool isFatal,
				   ਬਣਤਰ CShift_ਬਾਹਰਲਾ_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	ਜੇ (is_Negative_ਸੰਖਿਆ(p_ਅੰਕੜੇ->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (ਲੰਮਾ)ulRHS);
	} ਨਹੀਂਤਾਂ ਜੇ (ulRHS >= type_ਚੌੜਾਈ(p_ਅੰਕੜੇ->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)type_ਚੌੜਾਈ(p_ਅੰਕੜੇ->mLHSType),
			p_ਅੰਕੜੇ->mLHSType->mTypeName);

	} ਨਹੀਂਤਾਂ ਜੇ (is_Negative_ਸੰਖਿਆ(p_ਅੰਕੜੇ->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (ਲੰਮਾ)ulLHS);
	} ਨਹੀਂਤਾਂ {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_ਅੰਕੜੇ->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ Handle_ਲੱਦੋ_Invalid_ਮੁੱਲ(bool isFatal,
				   ਬਣਤਰ CInvalid_ਮੁੱਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਮੁੱਲ)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	bool is_signed = p_ਅੰਕੜੇ->mType->mTypeInfo & ਸੰਖਿਆ_SIGNED_ਦੋਆਧਾਰੀ_ਅੰਕ;

	sbi_printf("load of value ");

	ਜੇ (is_signed) {
		sbi_printf("%ld ", (ਲੰਮਾ)ul_ਮੁੱਲ);
	} ਨਹੀਂਤਾਂ {
		sbi_printf("%lu ", ul_ਮੁੱਲ);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_ਅੰਕੜੇ->mType->mTypeName);

	SEPARATOR;
}

ਅਚਲ ਅੱਖਰ *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ਸਥਿਰ ਖਾਲੀ HandleInvalidBuiltin(bool isFatal,
				 ਬਣਤਰ CInvalid_Builtin_ਅੰਕੜੇ *p_ਅੰਕੜੇ)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	ਅਚਲ ਅੱਖਰ *builtin =
		(p_ਅੰਕੜੇ->m_ਕਿਸਮ < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_ਅੰਕੜੇ->m_ਕਿਸਮ]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ HandleFunctionTypeMismatch(bool isFatal,
				       ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFunction)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_ਅੰਕੜੇ->mType->mTypeName);

	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ HandleMissingReturn(bool isFatal, ਬਣਤਰ CUnreachable_ਅੰਕੜੇ *p_ਅੰਕੜੇ)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ Handle_ਸੂਚਕ_ਉਛਾਲ(bool isFatal,
				  ਬਣਤਰ CPointer_ਉਛਾਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਨਤੀਜਾ)
{
	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_ਅਧਾਰ, ul_ਨਤੀਜਾ);

	SEPARATOR;
}

ਸਥਿਰ ਖਾਲੀ HandleAlignmentAssumption(bool isFatal,
				      ਬਣਤਰ CAlignment_Assumption_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulAlignment,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਵਿੱਥ)
{

	ਜੇ (!p_ਅੰਕੜੇ) {
		ਵਾਪਸ;
	}

	ਜੇ (isAlreadyReported(&p_ਅੰਕੜੇ->mLocation)) {
		ਵਾਪਸ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ਅੰਕੜੇ->mLocation.mFilename,
		   p_ਅੰਕੜੇ->mLocation.m_ਸਤਰ & ~ACK_REPORTED,
		   p_ਅੰਕੜੇ->mLocation.m_ਕਾਲਮ);

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_Real_ਸੂਚਕ = ul_ਸੂਚਕ - ul_ਵਿੱਥ;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (ਖਾਲੀ *)ul_Real_ਸੂਚਕ, (ਖਾਲੀ *)ul_ਵਿੱਥ);

	ਜੇ (p_ਅੰਕੜੇ->mAssumptionLocation.mFilename != ਨਿਰਾ) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_ਅੰਕੜੇ->mAssumptionLocation.mFilename,
			   p_ਅੰਕੜੇ->mAssumptionLocation.m_ਸਤਰ,
			   p_ਅੰਕੜੇ->mAssumptionLocation.m_ਕਾਲਮ);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
ਖਾਲੀ __ubsan_handle_add_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਉਛਾਲ(false, p_ਅੰਕੜੇ, ulLHS, ulRHS, "+");
}

ਖਾਲੀ __ubsan_handle_add_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਉਛਾਲ(true, p_ਅੰਕੜੇ, ulLHS, ulRHS, "+");
}

ਖਾਲੀ __ubsan_handle_sub_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਉਛਾਲ(false, p_ਅੰਕੜੇ, ulLHS, ulRHS, "-");
}

ਖਾਲੀ __ubsan_handle_sub_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਉਛਾਲ(true, p_ਅੰਕੜੇ, ulLHS, ulRHS, "-");
}

ਖਾਲੀ __ubsan_handle_mul_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਉਛਾਲ(false, p_ਅੰਕੜੇ, ulLHS, ulRHS, "*");
}

ਖਾਲੀ __ubsan_handle_mul_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਉਛਾਲ(true, p_ਅੰਕੜੇ, ulLHS, ulRHS, "*");
}

ਖਾਲੀ __ubsan_handle_negate_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_Old_ਮੁੱਲ)
{
	Handle_Negate_ਉਛਾਲ(false, p_ਅੰਕੜੇ, ul_Old_ਮੁੱਲ);
}

ਖਾਲੀ __ubsan_handle_negate_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_Old_ਮੁੱਲ)
{
	Handle_Negate_ਉਛਾਲ(true, p_ਅੰਕੜੇ, ul_Old_ਮੁੱਲ);
}

ਖਾਲੀ __ubsan_handle_divrem_overflow(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਉਛਾਲ(false, p_ਅੰਕੜੇ, ulLHS, ulRHS, "divrem");
}

ਖਾਲੀ __ubsan_handle_divrem_overflow_abort(ਬਣਤਰ COverflow_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
					  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਉਛਾਲ(true, p_ਅੰਕੜੇ, ulLHS, ulRHS, "divrem");
}

ਖਾਲੀ __ubsan_handle_type_mismatch_v1(ਬਣਤਰ CType_Mismatch_ਅੰਕੜੇ_v1 *p_ਅੰਕੜੇ,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ)
{
	HandleTypeMismatch(false, &p_ਅੰਕੜੇ->mLocation, p_ਅੰਕੜੇ->mType,
			   __BIT(p_ਅੰਕੜੇ->mLogAlignment), p_ਅੰਕੜੇ->m_Type_Check_ਕਿਸਮ,
			   ul_ਸੂਚਕ);
}

ਖਾਲੀ __ubsan_handle_type_mismatch_v1_abort(ਬਣਤਰ CType_Mismatch_ਅੰਕੜੇ_v1 *p_ਅੰਕੜੇ,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ)
{
	HandleTypeMismatch(true, &p_ਅੰਕੜੇ->mLocation, p_ਅੰਕੜੇ->mType,
			   __BIT(p_ਅੰਕੜੇ->mLogAlignment), p_ਅੰਕੜੇ->m_Type_Check_ਕਿਸਮ,
			   ul_ਸੂਚਕ);
}

ਖਾਲੀ __ubsan_handle_out_of_bounds(ਬਣਤਰ COut_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕਾਂਕ)
{
	Handle_ਬਾਹਰਲਾ_ਦਾ_Bounds(false, p_ਅੰਕੜੇ, ul_ਸੂਚਕਾਂਕ);
}

ਖਾਲੀ __ubsan_handle_out_of_bounds_abort(ਬਣਤਰ COut_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕਾਂਕ)
{
	Handle_ਬਾਹਰਲਾ_ਦਾ_Bounds(true, p_ਅੰਕੜੇ, ul_ਸੂਚਕਾਂਕ);
}

ਖਾਲੀ __ubsan_handle_shift_out_of_bounds(ਬਣਤਰ CShift_ਬਾਹਰਲਾ_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਖਿਸਕਾਓ_ਬਾਹਰਲਾ_ਦਾ_Bounds(false, p_ਅੰਕੜੇ, ulLHS, ulRHS);
}

ਖਾਲੀ __ubsan_handle_shift_out_of_bounds_abort(
	ਬਣਤਰ CShift_ਬਾਹਰਲਾ_ਦਾ_Bounds_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulLHS,
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulRHS)
{
	Handle_ਖਿਸਕਾਓ_ਬਾਹਰਲਾ_ਦਾ_Bounds(true, p_ਅੰਕੜੇ, ulLHS, ulRHS);
}

ਖਾਲੀ __ubsan_handle_pointer_overflow(ਬਣਤਰ CPointer_ਉਛਾਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਅਧਾਰ,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਨਤੀਜਾ)
{
	Handle_ਸੂਚਕ_ਉਛਾਲ(false, p_ਅੰਕੜੇ, ul_ਅਧਾਰ, ul_ਨਤੀਜਾ);
}

ਖਾਲੀ __ubsan_handle_pointer_overflow_abort(ਬਣਤਰ CPointer_ਉਛਾਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਅਧਾਰ,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਨਤੀਜਾ)
{
	Handle_ਸੂਚਕ_ਉਛਾਲ(true, p_ਅੰਕੜੇ, ul_ਅਧਾਰ, ul_ਨਤੀਜਾ);
}

ਖਾਲੀ __ubsan_handle_alignment_assumption(ਬਣਤਰ CAlignment_Assumption_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulAlignment,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਵਿੱਥ)
{
	HandleAlignmentAssumption(false, p_ਅੰਕੜੇ, ul_ਸੂਚਕ, ulAlignment,
				  ul_ਵਿੱਥ);
}

ਖਾਲੀ __ubsan_handle_alignment_assumption_abort(
	ਬਣਤਰ CAlignment_Assumption_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਸੂਚਕ,
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulAlignment, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਵਿੱਥ)
{
	HandleAlignmentAssumption(true, p_ਅੰਕੜੇ, ul_ਸੂਚਕ, ulAlignment,
				  ul_ਵਿੱਥ);
}

ਖਾਲੀ __ubsan_handle_builtin_unreachable(ਬਣਤਰ CUnreachable_ਅੰਕੜੇ *p_ਅੰਕੜੇ)
{
	HandleBuiltinUnreachable(true, p_ਅੰਕੜੇ);
}

ਖਾਲੀ __ubsan_handle_invalid_builtin(ਬਣਤਰ CInvalid_Builtin_ਅੰਕੜੇ *p_ਅੰਕੜੇ)
{
	HandleInvalidBuiltin(true, p_ਅੰਕੜੇ);
}

ਖਾਲੀ __ubsan_handle_invalid_builtin_abort(ਬਣਤਰ CInvalid_Builtin_ਅੰਕੜੇ *p_ਅੰਕੜੇ)
{
	HandleInvalidBuiltin(true, p_ਅੰਕੜੇ);
}

ਖਾਲੀ __ubsan_handle_load_invalid_value(ਬਣਤਰ CInvalid_ਮੁੱਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਮੁੱਲ)
{
	Handle_ਲੱਦੋ_Invalid_ਮੁੱਲ(false, p_ਅੰਕੜੇ, ul_ਮੁੱਲ);
}

ਖਾਲੀ __ubsan_handle_load_invalid_value_abort(ਬਣਤਰ CInvalid_ਮੁੱਲ_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul_ਮੁੱਲ)
{
	Handle_ਲੱਦੋ_Invalid_ਮੁੱਲ(true, p_ਅੰਕੜੇ, ul_ਮੁੱਲ);
}

ਖਾਲੀ __ubsan_handle_missing_return(ਬਣਤਰ CUnreachable_ਅੰਕੜੇ *p_ਅੰਕੜੇ)
{
	HandleMissingReturn(true, p_ਅੰਕੜੇ);
}

ਖਾਲੀ __ubsan_handle_vla_bound_not_positive(ਬਣਤਰ CVLABound_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulBound)
{
	Handle_Vla_Bound_Not_ਸਕਾਰਾਤਮਕ(false, p_ਅੰਕੜੇ, ulBound);
}

ਖਾਲੀ __ubsan_handle_vla_bound_not_positive_abort(ਬਣਤਰ CVLABound_ਅੰਕੜੇ *p_ਅੰਕੜੇ,
						 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulBound)
{
	Handle_Vla_Bound_Not_ਸਕਾਰਾਤਮਕ(true, p_ਅੰਕੜੇ, ulBound);
}

ਖਾਲੀ __ubsan_handle_function_type_mismatch(
	ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFunction)
{
	HandleFunctionTypeMismatch(false, p_ਅੰਕੜੇ, ulFunction);
}

ਖਾਲੀ __ubsan_handle_function_type_mismatch_abort(
	ਬਣਤਰ CFunction_Type_Mismatch_ਅੰਕੜੇ *p_ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulFunction)
{
	HandleFunctionTypeMismatch(true, p_ਅੰਕੜੇ, ulFunction);
}
#ਜੇ_ਅੰਤ
