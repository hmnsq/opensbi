/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UBSAN_ENABLED

#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಗೆ_INTEGER 0
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಗೆ_FLOAT 1
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಗೆ_UNKNOWN UINT16_MAX

ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_ubsan_report_ಎಣಿಕೆ;

ರಚನೆ CSourceLocation {
	ಅಕ್ಷರ *mFilename;
	uint32_t m_ಸಾಲು;
	uint32_t m_ಸ್ತಂಭ;
};

ರಚನೆ CTypeDescriptor {
	uint16_t m_Type_ಬಗೆ;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

ರಚನೆ COverflow_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mType;
};

ರಚನೆ CUnreachable_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
};

ರಚನೆ CCFICheck_Fail_ದತ್ತಾಂಶ {
	uint8_t m_Check_ಬಗೆ;
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mType;
};

ರಚನೆ CDynamic_Type_Cache_ತಪ್ಪಿದ_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mType;
	ಶೂನ್ಯ *mTypeInfo;
	uint8_t m_Type_Check_ಬಗೆ;
};

ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mType;
};

ರಚನೆ CInvalid_Builtin_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	uint8_t m_ಬಗೆ;
};

ರಚನೆ CInvalid_ಮೌಲ್ಯ_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mType;
};

ರಚನೆ CNon_Null_Arg_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CSourceLocation mAttributeLocation;
	ಪೂರ್ಣಾಂಕ m_Arg_ಸೂಚ್ಯಂಕ;
};

ರಚನೆ CNon_Null_Return_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mAttributeLocation;
};

ರಚನೆ COut_ದ_Bounds_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mArrayType;
	ರಚನೆ CTypeDescriptor *m_ಸೂಚ್ಯಂಕ_Type;
};

ರಚನೆ CPointer_ಅತಿಪ್ರವಾಹ_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
};

ರಚನೆ CShift_ಹೊರಹೋಗು_ದ_Bounds_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mLHSType;
	ರಚನೆ CTypeDescriptor *mRHSType;
};

ರಚನೆ CType_Mismatch_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mType;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mLogAlignment;
	uint8_t m_Type_Check_ಬಗೆ;
};

ರಚನೆ CType_Mismatch_ದತ್ತಾಂಶ_v1 {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_ಬಗೆ;
};

ರಚನೆ CVLABound_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mType;
};

ರಚನೆ CFloat_Cast_ಅತಿಪ್ರವಾಹ_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	ರಚನೆ CTypeDescriptor *mFromType;
	ರಚನೆ CTypeDescriptor *m_ಗಮ್ಯ_Type;
};

ರಚನೆ CImplicit_Conversion_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CTypeDescriptor *mFromType;
	ರಚನೆ CTypeDescriptor *m_ಗಮ್ಯ_Type;
	uint8_t m_ಬಗೆ;
};

ರಚನೆ CAlignment_Assumption_ದತ್ತಾಂಶ {
	ರಚನೆ CSourceLocation mLocation;
	ರಚನೆ CSourceLocation mAssumptionLocation;
	ರಚನೆ CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
ಶೂನ್ಯ __ubsan_handle_add_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_add_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_alignment_assumption(ರಚನೆ CAlignment_Assumption_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulAlignment,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸ್ಥಾನಾಂತರ);
ಶೂನ್ಯ __ubsan_handle_alignment_assumption_abort(
	ರಚನೆ CAlignment_Assumption_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ,
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulAlignment, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸ್ಥಾನಾಂತರ);
ಶೂನ್ಯ __ubsan_handle_builtin_unreachable(ರಚನೆ CUnreachable_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಶೂನ್ಯ __ubsan_handle_divrem_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_divrem_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
					  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_function_type_mismatch(
	ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFunction);
ಶೂನ್ಯ __ubsan_handle_function_type_mismatch_abort(
	ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFunction);
ಶೂನ್ಯ __ubsan_handle_function_type_mismatch_v1(
	ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFunction,
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulCalleeRTTI, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFnRTTI);
ಶೂನ್ಯ __ubsan_handle_function_type_mismatch_v1_abort(
	ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFunction,
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulCalleeRTTI, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFnRTTI);
ಶೂನ್ಯ __ubsan_handle_invalid_builtin(ರಚನೆ CInvalid_Builtin_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಶೂನ್ಯ __ubsan_handle_invalid_builtin_abort(ರಚನೆ CInvalid_Builtin_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಶೂನ್ಯ __ubsan_handle_load_invalid_value(ರಚನೆ CInvalid_ಮೌಲ್ಯ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulVal);
ಶೂನ್ಯ __ubsan_handle_load_invalid_value_abort(ರಚನೆ CInvalid_ಮೌಲ್ಯ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulVal);
ಶೂನ್ಯ __ubsan_handle_missing_return(ರಚನೆ CUnreachable_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಶೂನ್ಯ __ubsan_handle_mul_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_mul_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_negate_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulOldVal);
ಶೂನ್ಯ __ubsan_handle_negate_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulOldVal);
ಶೂನ್ಯ __ubsan_handle_nullability_arg(ರಚನೆ CNon_Null_Arg_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಶೂನ್ಯ __ubsan_handle_nullability_arg_abort(ರಚನೆ CNon_Null_Arg_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಶೂನ್ಯ __ubsan_handle_nullability_return_v1(
	ರಚನೆ CNon_Null_Return_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
	ರಚನೆ CSourceLocation *p_Location_ಸೂಚಕ);
ಶೂನ್ಯ __ubsan_handle_nullability_return_v1_abort(
	ರಚನೆ CNon_Null_Return_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
	ರಚನೆ CSourceLocation *p_Location_ಸೂಚಕ);
ಶೂನ್ಯ __ubsan_handle_out_of_bounds(ರಚನೆ COut_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚ್ಯಂಕ);
ಶೂನ್ಯ __ubsan_handle_out_of_bounds_abort(ರಚನೆ COut_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚ್ಯಂಕ);
ಶೂನ್ಯ __ubsan_handle_pointer_overflow(ರಚನೆ CPointer_ಅತಿಪ್ರವಾಹ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ತಳ,
				     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಫಲಿತಾಂಶ);
ಶೂನ್ಯ __ubsan_handle_pointer_overflow_abort(ರಚನೆ CPointer_ಅತಿಪ್ರವಾಹ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ತಳ,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಫಲಿತಾಂಶ);
ಶೂನ್ಯ __ubsan_handle_shift_out_of_bounds(ರಚನೆ CShift_ಹೊರಹೋಗು_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
					ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_shift_out_of_bounds_abort(
	ರಚನೆ CShift_ಹೊರಹೋಗು_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_sub_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_sub_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಶೂನ್ಯ __ubsan_handle_type_mismatch(ರಚನೆ CType_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ);
ಶೂನ್ಯ __ubsan_handle_type_mismatch_abort(ರಚನೆ CType_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ);
ಶೂನ್ಯ __ubsan_handle_type_mismatch_v1(ರಚನೆ CType_Mismatch_ದತ್ತಾಂಶ_v1 *p_ದತ್ತಾಂಶ,
				     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ);
ಶೂನ್ಯ __ubsan_handle_type_mismatch_v1_abort(ರಚನೆ CType_Mismatch_ದತ್ತಾಂಶ_v1 *p_ದತ್ತಾಂಶ,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ);
ಶೂನ್ಯ __ubsan_handle_vla_bound_not_positive(ರಚನೆ CVLABound_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulBound);
ಶೂನ್ಯ __ubsan_handle_vla_bound_not_positive_abort(ರಚನೆ CVLABound_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
						 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulBound);
ಶೂನ್ಯ __ubsan_get_current_report_data(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **pp_ಹೊರಹೋಗು_Issue_ಬಗೆ,
				     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **pp_ಹೊರಹೋಗು_ಸಂದೇಶ,
				     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **pp_ಹೊರಹೋಗು_Filename,
				     uint32_t *p_ಹೊರಹೋಗು_ಸಾಲು, uint32_t *p_ಹೊರಹೋಗು_ಲಂಬಸಾಲು,
				     ಅಕ್ಷರ **pp_ಹೊರಹೋಗು_ಸ್ಮರಣೆ_Addr);
ಸ್ಥಿರ ಶೂನ್ಯ Handle_ಅತಿಪ್ರವಾಹ(bool isFatal, ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS,
			   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *szOperation);
ಸ್ಥಿರ ಶೂನ್ಯ Handle_Negate_ಅತಿಪ್ರವಾಹ(bool isFatal, ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_Old_ಮೌಲ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ HandleBuiltinUnreachable(bool isFatal,
				     ರಚನೆ CUnreachable_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ HandleTypeMismatch(bool isFatal, ರಚನೆ CSourceLocation *mLocation,
			       ರಚನೆ CTypeDescriptor *mType,
			       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mLogAlignment,
			       uint8_t m_Type_Check_ಬಗೆ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ);
ಸ್ಥಿರ ಶೂನ್ಯ Handle_Vla_Bound_Not_ಧನಾತ್ಮಕ(bool isFatal, ರಚನೆ CVLABound_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulBound);
ಸ್ಥಿರ ಶೂನ್ಯ Handle_ಹೊರಹೋಗು_ದ_Bounds(bool isFatal, ರಚನೆ COut_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
			      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚ್ಯಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ Handle_ಸರಿಸು_ಹೊರಹೋಗು_ದ_Bounds(bool isFatal,
				   ರಚನೆ CShift_ಹೊರಹೋಗು_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS);
ಸ್ಥಿರ ಶೂನ್ಯ Handle_ತುಂಬು_Invalid_ಮೌಲ್ಯ(bool isFatal,
				   ರಚನೆ CInvalid_ಮೌಲ್ಯ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಮೌಲ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ HandleInvalidBuiltin(bool isFatal,
				 ರಚನೆ CInvalid_Builtin_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ HandleFunctionTypeMismatch(bool isFatal,
				       ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFunction);
ಸ್ಥಿರ ಶೂನ್ಯ HandleMissingReturn(bool isFatal, ರಚನೆ CUnreachable_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ Handle_ಸೂಚಕ_ಅತಿಪ್ರವಾಹ(bool isFatal,
				  ರಚನೆ CPointer_ಅತಿಪ್ರವಾಹ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ತಳ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಫಲಿತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ HandleAlignmentAssumption(bool isFatal,
				      ರಚನೆ CAlignment_Assumption_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulAlignment,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸ್ಥಾನಾಂತರ);

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಖ್ಯೆ_SIGNED_ಬಿಟ್ 1
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಖ್ಯೆ_MAXLEN 128
#ವ್ಯಾಖ್ಯಾನಿಸು __arraycount(__a) (ಗಾತ್ರ(__a) / ಗಾತ್ರ(__a[0]))
#ವ್ಯಾಖ್ಯಾನಿಸು __BIT(__n) (1UL << (__n))
#ವ್ಯಾಖ್ಯಾನಿಸು SEPARATOR sbi_printf("===========================================\n")
#ವ್ಯಾಖ್ಯಾನಿಸು ACK_REPORTED (1U << 31)

ಸ್ಥಿರ bool isAlreadyReported(ರಚನೆ CSourceLocation *pLocation)
{
	uint32_t si_Old_ಮೌಲ್ಯ;
	ಅಸ್ಥಿರ uint32_t *p_ಸಾಲು;

	ಆದರೆ (!pLocation)
		ಹಿಂತಿರುಗಿ false;

	p_ಸಾಲು = &pLocation->m_ಸಾಲು;

	ಮಾಡು {
		si_Old_ಮೌಲ್ಯ = *p_ಸಾಲು;
	} ಆಗ (__sync_val_compare_and_swap(p_ಸಾಲು, si_Old_ಮೌಲ್ಯ,
					     si_Old_ಮೌಲ್ಯ | ACK_REPORTED) !=
		 si_Old_ಮೌಲ್ಯ);

	ಆದರೆ (!(si_Old_ಮೌಲ್ಯ & ACK_REPORTED)) {

		sbi_ubsan_report_ಎಣಿಕೆ++;

		ಹಿಂತಿರುಗಿ false;
	}

	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಶೂನ್ಯ Handle_ಅತಿಪ್ರವಾಹ(bool isFatal, ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS,
			   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *szOperation)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	bool is_signed = p_ದತ್ತಾಂಶ->mType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_ಬಿಟ್;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	ಆದರೆ (is_signed) {
		sbi_printf("%ld %s %ld ", (ದೀರ್ಘ)ulLHS, szOperation,
			   (ದೀರ್ಘ)ulRHS);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_ದತ್ತಾಂಶ->mType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ Handle_Negate_ಅತಿಪ್ರವಾಹ(bool isFatal, ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_Old_ಮೌಲ್ಯ)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	bool is_signed = p_ದತ್ತಾಂಶ->mType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_ಬಿಟ್;

	sbi_printf("UBSan: Negation of ");

	ಆದರೆ (is_signed) {
		sbi_printf("%ld", (ದೀರ್ಘ)ul_Old_ಮೌಲ್ಯ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_printf("%lu", ul_Old_ಮೌಲ್ಯ);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_ದತ್ತಾಂಶ->mType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleBuiltinUnreachable(bool isFatal,
				     ರಚನೆ CUnreachable_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	SEPARATOR;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rgczTypeCheckKinds[] = { "load of",
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

ಸ್ಥಿರ ಶೂನ್ಯ HandleTypeMismatch(bool isFatal, ರಚನೆ CSourceLocation *mLocation,
			       ರಚನೆ CTypeDescriptor *mType,
			       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mLogAlignment,
			       uint8_t m_Type_Check_ಬಗೆ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ)
{

	ಆದರೆ ((!mLocation) || (!mType)) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_ಸಾಲು & ~ACK_REPORTED,
		   mLocation->m_ಸ್ತಂಭ);

	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಗೆ = (m_Type_Check_ಬಗೆ < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_ಬಗೆ]
				   : "access to";

	ಆದರೆ (ul_ಸೂಚಕ == 0) {
		sbi_printf("%s null pointer of type %s\n", ಬಗೆ,
			   mType->mTypeName);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (
		(mLogAlignment - 1) &
		ul_ಸೂಚಕ) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			ಬಗೆ, (ಶೂನ್ಯ *)ul_ಸೂಚಕ, mType->mTypeName,
			mLogAlignment);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			ಬಗೆ, (ಶೂನ್ಯ *)ul_ಸೂಚಕ, mType->mTypeName);
	}
	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ Handle_Vla_Bound_Not_ಧನಾತ್ಮಕ(bool isFatal, ರಚನೆ CVLABound_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulBound)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_ದತ್ತಾಂಶ->mType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_ಬಿಟ್;

	ಆದರೆ (is_signed) {
		sbi_printf("%ld", (ದೀರ್ಘ)ulBound);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ Handle_ಹೊರಹೋಗು_ದ_Bounds(bool isFatal, ರಚನೆ COut_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
			      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚ್ಯಂಕ)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	bool is_signed = p_ದತ್ತಾಂಶ->m_ಸೂಚ್ಯಂಕ_Type->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_ಬಿಟ್;

	ಆದರೆ (is_signed) {
		sbi_printf("index %ld", (ದೀರ್ಘ)ul_ಸೂಚ್ಯಂಕ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_printf("index %lu", ul_ಸೂಚ್ಯಂಕ);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_ದತ್ತಾಂಶ->mArrayType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರ bool is_Negative_ಸಂಖ್ಯೆ(ರಚನೆ CTypeDescriptor *pType, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulVal)
{
	ಆದರೆ (!(pType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_ಬಿಟ್)) {
		ಹಿಂತಿರುಗಿ false;
	}

	ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)ulVal < 0;
}

ಸ್ಥಿರ size_t type_ಅಗಲ(ರಚನೆ CTypeDescriptor *pType)
{
	ಹಿಂತಿರುಗಿ 1UL << (pType->mTypeInfo >> 1);
}

ಸ್ಥಿರ ಶೂನ್ಯ Handle_ಸರಿಸು_ಹೊರಹೋಗು_ದ_Bounds(bool isFatal,
				   ರಚನೆ CShift_ಹೊರಹೋಗು_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	ಆದರೆ (is_Negative_ಸಂಖ್ಯೆ(p_ದತ್ತಾಂಶ->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (ದೀರ್ಘ)ulRHS);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ulRHS >= type_ಅಗಲ(p_ದತ್ತಾಂಶ->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)type_ಅಗಲ(p_ದತ್ತಾಂಶ->mLHSType),
			p_ದತ್ತಾಂಶ->mLHSType->mTypeName);

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_Negative_ಸಂಖ್ಯೆ(p_ದತ್ತಾಂಶ->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (ದೀರ್ಘ)ulLHS);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_ದತ್ತಾಂಶ->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ Handle_ತುಂಬು_Invalid_ಮೌಲ್ಯ(bool isFatal,
				   ರಚನೆ CInvalid_ಮೌಲ್ಯ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಮೌಲ್ಯ)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	bool is_signed = p_ದತ್ತಾಂಶ->mType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_ಬಿಟ್;

	sbi_printf("load of value ");

	ಆದರೆ (is_signed) {
		sbi_printf("%ld ", (ದೀರ್ಘ)ul_ಮೌಲ್ಯ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_printf("%lu ", ul_ಮೌಲ್ಯ);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_ದತ್ತಾಂಶ->mType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ಸ್ಥಿರ ಶೂನ್ಯ HandleInvalidBuiltin(bool isFatal,
				 ರಚನೆ CInvalid_Builtin_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *builtin =
		(p_ದತ್ತಾಂಶ->m_ಬಗೆ < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_ದತ್ತಾಂಶ->m_ಬಗೆ]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleFunctionTypeMismatch(bool isFatal,
				       ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFunction)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_ದತ್ತಾಂಶ->mType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleMissingReturn(bool isFatal, ರಚನೆ CUnreachable_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ Handle_ಸೂಚಕ_ಅತಿಪ್ರವಾಹ(bool isFatal,
				  ರಚನೆ CPointer_ಅತಿಪ್ರವಾಹ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ತಳ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಫಲಿತಾಂಶ)
{
	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_ತಳ, ul_ಫಲಿತಾಂಶ);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleAlignmentAssumption(bool isFatal,
				      ರಚನೆ CAlignment_Assumption_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulAlignment,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸ್ಥಾನಾಂತರ)
{

	ಆದರೆ (!p_ದತ್ತಾಂಶ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (isAlreadyReported(&p_ದತ್ತಾಂಶ->mLocation)) {
		ಹಿಂತಿರುಗಿ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_ದತ್ತಾಂಶ->mLocation.mFilename,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   p_ದತ್ತಾಂಶ->mLocation.m_ಸ್ತಂಭ);

	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_Real_ಸೂಚಕ = ul_ಸೂಚಕ - ul_ಸ್ಥಾನಾಂತರ;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (ಶೂನ್ಯ *)ul_Real_ಸೂಚಕ, (ಶೂನ್ಯ *)ul_ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (p_ದತ್ತಾಂಶ->mAssumptionLocation.mFilename != ಖಾಲಿ) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_ದತ್ತಾಂಶ->mAssumptionLocation.mFilename,
			   p_ದತ್ತಾಂಶ->mAssumptionLocation.m_ಸಾಲು,
			   p_ದತ್ತಾಂಶ->mAssumptionLocation.m_ಸ್ತಂಭ);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
ಶೂನ್ಯ __ubsan_handle_add_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಅತಿಪ್ರವಾಹ(false, p_ದತ್ತಾಂಶ, ulLHS, ulRHS, "+");
}

ಶೂನ್ಯ __ubsan_handle_add_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಅತಿಪ್ರವಾಹ(true, p_ದತ್ತಾಂಶ, ulLHS, ulRHS, "+");
}

ಶೂನ್ಯ __ubsan_handle_sub_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಅತಿಪ್ರವಾಹ(false, p_ದತ್ತಾಂಶ, ulLHS, ulRHS, "-");
}

ಶೂನ್ಯ __ubsan_handle_sub_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಅತಿಪ್ರವಾಹ(true, p_ದತ್ತಾಂಶ, ulLHS, ulRHS, "-");
}

ಶೂನ್ಯ __ubsan_handle_mul_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಅತಿಪ್ರವಾಹ(false, p_ದತ್ತಾಂಶ, ulLHS, ulRHS, "*");
}

ಶೂನ್ಯ __ubsan_handle_mul_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಅತಿಪ್ರವಾಹ(true, p_ದತ್ತಾಂಶ, ulLHS, ulRHS, "*");
}

ಶೂನ್ಯ __ubsan_handle_negate_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_Old_ಮೌಲ್ಯ)
{
	Handle_Negate_ಅತಿಪ್ರವಾಹ(false, p_ದತ್ತಾಂಶ, ul_Old_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ __ubsan_handle_negate_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_Old_ಮೌಲ್ಯ)
{
	Handle_Negate_ಅತಿಪ್ರವಾಹ(true, p_ದತ್ತಾಂಶ, ul_Old_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ __ubsan_handle_divrem_overflow(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಅತಿಪ್ರವಾಹ(false, p_ದತ್ತಾಂಶ, ulLHS, ulRHS, "divrem");
}

ಶೂನ್ಯ __ubsan_handle_divrem_overflow_abort(ರಚನೆ COverflow_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
					  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಅತಿಪ್ರವಾಹ(true, p_ದತ್ತಾಂಶ, ulLHS, ulRHS, "divrem");
}

ಶೂನ್ಯ __ubsan_handle_type_mismatch_v1(ರಚನೆ CType_Mismatch_ದತ್ತಾಂಶ_v1 *p_ದತ್ತಾಂಶ,
				     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ)
{
	HandleTypeMismatch(false, &p_ದತ್ತಾಂಶ->mLocation, p_ದತ್ತಾಂಶ->mType,
			   __BIT(p_ದತ್ತಾಂಶ->mLogAlignment), p_ದತ್ತಾಂಶ->m_Type_Check_ಬಗೆ,
			   ul_ಸೂಚಕ);
}

ಶೂನ್ಯ __ubsan_handle_type_mismatch_v1_abort(ರಚನೆ CType_Mismatch_ದತ್ತಾಂಶ_v1 *p_ದತ್ತಾಂಶ,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ)
{
	HandleTypeMismatch(true, &p_ದತ್ತಾಂಶ->mLocation, p_ದತ್ತಾಂಶ->mType,
			   __BIT(p_ದತ್ತಾಂಶ->mLogAlignment), p_ದತ್ತಾಂಶ->m_Type_Check_ಬಗೆ,
			   ul_ಸೂಚಕ);
}

ಶೂನ್ಯ __ubsan_handle_out_of_bounds(ರಚನೆ COut_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚ್ಯಂಕ)
{
	Handle_ಹೊರಹೋಗು_ದ_Bounds(false, p_ದತ್ತಾಂಶ, ul_ಸೂಚ್ಯಂಕ);
}

ಶೂನ್ಯ __ubsan_handle_out_of_bounds_abort(ರಚನೆ COut_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚ್ಯಂಕ)
{
	Handle_ಹೊರಹೋಗು_ದ_Bounds(true, p_ದತ್ತಾಂಶ, ul_ಸೂಚ್ಯಂಕ);
}

ಶೂನ್ಯ __ubsan_handle_shift_out_of_bounds(ರಚನೆ CShift_ಹೊರಹೋಗು_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
					ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಸರಿಸು_ಹೊರಹೋಗು_ದ_Bounds(false, p_ದತ್ತಾಂಶ, ulLHS, ulRHS);
}

ಶೂನ್ಯ __ubsan_handle_shift_out_of_bounds_abort(
	ರಚನೆ CShift_ಹೊರಹೋಗು_ದ_Bounds_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulLHS,
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulRHS)
{
	Handle_ಸರಿಸು_ಹೊರಹೋಗು_ದ_Bounds(true, p_ದತ್ತಾಂಶ, ulLHS, ulRHS);
}

ಶೂನ್ಯ __ubsan_handle_pointer_overflow(ರಚನೆ CPointer_ಅತಿಪ್ರವಾಹ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ತಳ,
				     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಫಲಿತಾಂಶ)
{
	Handle_ಸೂಚಕ_ಅತಿಪ್ರವಾಹ(false, p_ದತ್ತಾಂಶ, ul_ತಳ, ul_ಫಲಿತಾಂಶ);
}

ಶೂನ್ಯ __ubsan_handle_pointer_overflow_abort(ರಚನೆ CPointer_ಅತಿಪ್ರವಾಹ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ತಳ,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಫಲಿತಾಂಶ)
{
	Handle_ಸೂಚಕ_ಅತಿಪ್ರವಾಹ(true, p_ದತ್ತಾಂಶ, ul_ತಳ, ul_ಫಲಿತಾಂಶ);
}

ಶೂನ್ಯ __ubsan_handle_alignment_assumption(ರಚನೆ CAlignment_Assumption_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulAlignment,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸ್ಥಾನಾಂತರ)
{
	HandleAlignmentAssumption(false, p_ದತ್ತಾಂಶ, ul_ಸೂಚಕ, ulAlignment,
				  ul_ಸ್ಥಾನಾಂತರ);
}

ಶೂನ್ಯ __ubsan_handle_alignment_assumption_abort(
	ರಚನೆ CAlignment_Assumption_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸೂಚಕ,
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulAlignment, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಸ್ಥಾನಾಂತರ)
{
	HandleAlignmentAssumption(true, p_ದತ್ತಾಂಶ, ul_ಸೂಚಕ, ulAlignment,
				  ul_ಸ್ಥಾನಾಂತರ);
}

ಶೂನ್ಯ __ubsan_handle_builtin_unreachable(ರಚನೆ CUnreachable_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ)
{
	HandleBuiltinUnreachable(true, p_ದತ್ತಾಂಶ);
}

ಶೂನ್ಯ __ubsan_handle_invalid_builtin(ರಚನೆ CInvalid_Builtin_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ)
{
	HandleInvalidBuiltin(true, p_ದತ್ತಾಂಶ);
}

ಶೂನ್ಯ __ubsan_handle_invalid_builtin_abort(ರಚನೆ CInvalid_Builtin_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ)
{
	HandleInvalidBuiltin(true, p_ದತ್ತಾಂಶ);
}

ಶೂನ್ಯ __ubsan_handle_load_invalid_value(ರಚನೆ CInvalid_ಮೌಲ್ಯ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಮೌಲ್ಯ)
{
	Handle_ತುಂಬು_Invalid_ಮೌಲ್ಯ(false, p_ದತ್ತಾಂಶ, ul_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ __ubsan_handle_load_invalid_value_abort(ರಚನೆ CInvalid_ಮೌಲ್ಯ_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul_ಮೌಲ್ಯ)
{
	Handle_ತುಂಬು_Invalid_ಮೌಲ್ಯ(true, p_ದತ್ತಾಂಶ, ul_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ __ubsan_handle_missing_return(ರಚನೆ CUnreachable_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ)
{
	HandleMissingReturn(true, p_ದತ್ತಾಂಶ);
}

ಶೂನ್ಯ __ubsan_handle_vla_bound_not_positive(ರಚನೆ CVLABound_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulBound)
{
	Handle_Vla_Bound_Not_ಧನಾತ್ಮಕ(false, p_ದತ್ತಾಂಶ, ulBound);
}

ಶೂನ್ಯ __ubsan_handle_vla_bound_not_positive_abort(ರಚನೆ CVLABound_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ,
						 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulBound)
{
	Handle_Vla_Bound_Not_ಧನಾತ್ಮಕ(true, p_ದತ್ತಾಂಶ, ulBound);
}

ಶೂನ್ಯ __ubsan_handle_function_type_mismatch(
	ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFunction)
{
	HandleFunctionTypeMismatch(false, p_ದತ್ತಾಂಶ, ulFunction);
}

ಶೂನ್ಯ __ubsan_handle_function_type_mismatch_abort(
	ರಚನೆ CFunction_Type_Mismatch_ದತ್ತಾಂಶ *p_ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulFunction)
{
	HandleFunctionTypeMismatch(true, p_ದತ್ತಾಂಶ, ulFunction);
}
#ಆದರೆ_ಅಂತ್ಯ
