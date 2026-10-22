/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#अगर_घोषित UBSAN_ENABLED

#समावेश <sbi/sbi_types.h>
#समावेश <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#घोषणा KIND_INTEGER 0
#घोषणा KIND_FLOAT 1
#घोषणा KIND_UNKNOWN UINT16_MAX

अस्थिर अचिन्हित दीर्घ sbi_ubsan_report_गिनती;

काष्ठा CSourceLocation {
	अक्षर *mFilename;
	uint32_t m_पंक्ति;
	uint32_t mColumn;
};

काष्ठा CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

काष्ठा COverflow_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mType;
};

काष्ठा CUnreachable_तथ्य {
	काष्ठा CSourceLocation mLocation;
};

काष्ठा CCFICheck_Fail_तथ्य {
	uint8_t mCheckKind;
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mType;
};

काष्ठा CDynamic_Type_Cache_Miss_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mType;
	व्योम *mTypeInfo;
	uint8_t mTypeCheckKind;
};

काष्ठा CFunction_Type_Mismatch_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mType;
};

काष्ठा CInvalid_Builtin_तथ्य {
	काष्ठा CSourceLocation mLocation;
	uint8_t mKind;
};

काष्ठा CInvalid_मूल्य_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mType;
};

काष्ठा CNon_Null_Arg_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CSourceLocation mAttributeLocation;
	पूर्णांक m_Arg_सूचकांक;
};

काष्ठा CNon_Null_Return_तथ्य {
	काष्ठा CSourceLocation mAttributeLocation;
};

काष्ठा COut_Of_Bounds_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mArrayType;
	काष्ठा CTypeDescriptor *m_सूचकांक_Type;
};

काष्ठा CPointer_Overflow_तथ्य {
	काष्ठा CSourceLocation mLocation;
};

काष्ठा CShift_Out_Of_Bounds_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mLHSType;
	काष्ठा CTypeDescriptor *mRHSType;
};

काष्ठा CType_Mismatch_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mType;
	अचिन्हित दीर्घ mLogAlignment;
	uint8_t mTypeCheckKind;
};

काष्ठा CType_Mismatch_तथ्य_v1 {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

काष्ठा CVLABound_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mType;
};

काष्ठा CFloat_Cast_Overflow_तथ्य {
	काष्ठा CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	काष्ठा CTypeDescriptor *mFromType;
	काष्ठा CTypeDescriptor *mToType;
};

काष्ठा CImplicit_Conversion_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CTypeDescriptor *mFromType;
	काष्ठा CTypeDescriptor *mToType;
	uint8_t mKind;
};

काष्ठा CAlignment_Assumption_तथ्य {
	काष्ठा CSourceLocation mLocation;
	काष्ठा CSourceLocation mAssumptionLocation;
	काष्ठा CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
व्योम __ubsan_handle_add_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				 अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_add_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulLHS,
				       अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_alignment_assumption(काष्ठा CAlignment_Assumption_तथ्य *p_तथ्य,
					 अचिन्हित दीर्घ ulPointer,
					 अचिन्हित दीर्घ ulAlignment,
					 अचिन्हित दीर्घ ulOffset);
व्योम __ubsan_handle_alignment_assumption_abort(
	काष्ठा CAlignment_Assumption_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulPointer,
	अचिन्हित दीर्घ ulAlignment, अचिन्हित दीर्घ ulOffset);
व्योम __ubsan_handle_builtin_unreachable(काष्ठा CUnreachable_तथ्य *p_तथ्य);
व्योम __ubsan_handle_divrem_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				    अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_divrem_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
					  अचिन्हित दीर्घ ulLHS,
					  अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_function_type_mismatch(
	काष्ठा CFunction_Type_Mismatch_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulFunction);
व्योम __ubsan_handle_function_type_mismatch_abort(
	काष्ठा CFunction_Type_Mismatch_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulFunction);
व्योम __ubsan_handle_function_type_mismatch_v1(
	काष्ठा CFunction_Type_Mismatch_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulFunction,
	अचिन्हित दीर्घ ulCalleeRTTI, अचिन्हित दीर्घ ulFnRTTI);
व्योम __ubsan_handle_function_type_mismatch_v1_abort(
	काष्ठा CFunction_Type_Mismatch_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulFunction,
	अचिन्हित दीर्घ ulCalleeRTTI, अचिन्हित दीर्घ ulFnRTTI);
व्योम __ubsan_handle_invalid_builtin(काष्ठा CInvalid_Builtin_तथ्य *p_तथ्य);
व्योम __ubsan_handle_invalid_builtin_abort(काष्ठा CInvalid_Builtin_तथ्य *p_तथ्य);
व्योम __ubsan_handle_load_invalid_value(काष्ठा CInvalid_मूल्य_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulVal);
व्योम __ubsan_handle_load_invalid_value_abort(काष्ठा CInvalid_मूल्य_तथ्य *p_तथ्य,
					     अचिन्हित दीर्घ ulVal);
व्योम __ubsan_handle_missing_return(काष्ठा CUnreachable_तथ्य *p_तथ्य);
व्योम __ubsan_handle_mul_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				 अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_mul_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulLHS,
				       अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_negate_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				    अचिन्हित दीर्घ ulOldVal);
व्योम __ubsan_handle_negate_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
					  अचिन्हित दीर्घ ulOldVal);
व्योम __ubsan_handle_nullability_arg(काष्ठा CNon_Null_Arg_तथ्य *p_तथ्य);
व्योम __ubsan_handle_nullability_arg_abort(काष्ठा CNon_Null_Arg_तथ्य *p_तथ्य);
व्योम __ubsan_handle_nullability_return_v1(
	काष्ठा CNon_Null_Return_तथ्य *p_तथ्य,
	काष्ठा CSourceLocation *pLocationPointer);
व्योम __ubsan_handle_nullability_return_v1_abort(
	काष्ठा CNon_Null_Return_तथ्य *p_तथ्य,
	काष्ठा CSourceLocation *pLocationPointer);
व्योम __ubsan_handle_out_of_bounds(काष्ठा COut_Of_Bounds_तथ्य *p_तथ्य,
				  अचिन्हित दीर्घ ul_सूचकांक);
व्योम __ubsan_handle_out_of_bounds_abort(काष्ठा COut_Of_Bounds_तथ्य *p_तथ्य,
					अचिन्हित दीर्घ ul_सूचकांक);
व्योम __ubsan_handle_pointer_overflow(काष्ठा CPointer_Overflow_तथ्य *p_तथ्य,
				     अचिन्हित दीर्घ ulBase,
				     अचिन्हित दीर्घ ul_परिणाम);
व्योम __ubsan_handle_pointer_overflow_abort(काष्ठा CPointer_Overflow_तथ्य *p_तथ्य,
					   अचिन्हित दीर्घ ulBase,
					   अचिन्हित दीर्घ ul_परिणाम);
व्योम __ubsan_handle_shift_out_of_bounds(काष्ठा CShift_Out_Of_Bounds_तथ्य *p_तथ्य,
					अचिन्हित दीर्घ ulLHS,
					अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_shift_out_of_bounds_abort(
	काष्ठा CShift_Out_Of_Bounds_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulLHS,
	अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_sub_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				 अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_sub_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulLHS,
				       अचिन्हित दीर्घ ulRHS);
व्योम __ubsan_handle_type_mismatch(काष्ठा CType_Mismatch_तथ्य *p_तथ्य,
				  अचिन्हित दीर्घ ulPointer);
व्योम __ubsan_handle_type_mismatch_abort(काष्ठा CType_Mismatch_तथ्य *p_तथ्य,
					अचिन्हित दीर्घ ulPointer);
व्योम __ubsan_handle_type_mismatch_v1(काष्ठा CType_Mismatch_तथ्य_v1 *p_तथ्य,
				     अचिन्हित दीर्घ ulPointer);
व्योम __ubsan_handle_type_mismatch_v1_abort(काष्ठा CType_Mismatch_तथ्य_v1 *p_तथ्य,
					   अचिन्हित दीर्घ ulPointer);
व्योम __ubsan_handle_vla_bound_not_positive(काष्ठा CVLABound_तथ्य *p_तथ्य,
					   अचिन्हित दीर्घ ulBound);
व्योम __ubsan_handle_vla_bound_not_positive_abort(काष्ठा CVLABound_तथ्य *p_तथ्य,
						 अचिन्हित दीर्घ ulBound);
व्योम __ubsan_get_current_report_data(स्थिर अक्षर **ppOutIssueKind,
				     स्थिर अक्षर **ppOutMessage,
				     स्थिर अक्षर **ppOutFilename,
				     uint32_t *p_Out_पंक्ति, uint32_t *pOutCol,
				     अक्षर **ppOutMemoryAddr);
अटल व्योम HandleOverflow(bool isFatal, काष्ठा COverflow_तथ्य *p_तथ्य,
			   अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS,
			   स्थिर अक्षर *szOperation);
अटल व्योम HandleNegateOverflow(bool isFatal, काष्ठा COverflow_तथ्य *p_तथ्य,
				 अचिन्हित दीर्घ ul_Old_मूल्य);
अटल व्योम HandleBuiltinUnreachable(bool isFatal,
				     काष्ठा CUnreachable_तथ्य *p_तथ्य);
अटल व्योम HandleTypeMismatch(bool isFatal, काष्ठा CSourceLocation *mLocation,
			       काष्ठा CTypeDescriptor *mType,
			       अचिन्हित दीर्घ mLogAlignment,
			       uint8_t mTypeCheckKind, अचिन्हित दीर्घ ulPointer);
अटल व्योम HandleVlaBoundNotPositive(bool isFatal, काष्ठा CVLABound_तथ्य *p_तथ्य,
				      अचिन्हित दीर्घ ulBound);
अटल व्योम HandleOutOfBounds(bool isFatal, काष्ठा COut_Of_Bounds_तथ्य *p_तथ्य,
			      अचिन्हित दीर्घ ul_सूचकांक);
अटल व्योम HandleShiftOutOfBounds(bool isFatal,
				   काष्ठा CShift_Out_Of_Bounds_तथ्य *p_तथ्य,
				   अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS);
अटल व्योम Handle_Load_Invalid_मूल्य(bool isFatal,
				   काष्ठा CInvalid_मूल्य_तथ्य *p_तथ्य,
				   अचिन्हित दीर्घ ul_मूल्य);
अटल व्योम HandleInvalidBuiltin(bool isFatal,
				 काष्ठा CInvalid_Builtin_तथ्य *p_तथ्य);
अटल व्योम HandleFunctionTypeMismatch(bool isFatal,
				       काष्ठा CFunction_Type_Mismatch_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulFunction);
अटल व्योम HandleMissingReturn(bool isFatal, काष्ठा CUnreachable_तथ्य *p_तथ्य);
अटल व्योम HandlePointerOverflow(bool isFatal,
				  काष्ठा CPointer_Overflow_तथ्य *p_तथ्य,
				  अचिन्हित दीर्घ ulBase, अचिन्हित दीर्घ ul_परिणाम);
अटल व्योम HandleAlignmentAssumption(bool isFatal,
				      काष्ठा CAlignment_Assumption_तथ्य *p_तथ्य,
				      अचिन्हित दीर्घ ulPointer,
				      अचिन्हित दीर्घ ulAlignment,
				      अचिन्हित दीर्घ ulOffset);

#घोषणा संख्या_SIGNED_BIT 1
#घोषणा संख्या_MAXLEN 128
#घोषणा __arraycount(__a) (माप(__a) / माप(__a[0]))
#घोषणा __BIT(__n) (1UL << (__n))
#घोषणा SEPARATOR sbi_printf("===========================================\n")
#घोषणा ACK_REPORTED (1U << 31)

अटल bool isAlreadyReported(काष्ठा CSourceLocation *pLocation)
{
	uint32_t si_Old_मूल्य;
	अस्थिर uint32_t *p_पंक्ति;

	अगर (!pLocation)
		वापस false;

	p_पंक्ति = &pLocation->m_पंक्ति;

	करो {
		si_Old_मूल्य = *p_पंक्ति;
	} जबतक (__sync_val_compare_and_swap(p_पंक्ति, si_Old_मूल्य,
					     si_Old_मूल्य | ACK_REPORTED) !=
		 si_Old_मूल्य);

	अगर (!(si_Old_मूल्य & ACK_REPORTED)) {

		sbi_ubsan_report_गिनती++;

		वापस false;
	}

	वापस true;
}

अटल व्योम HandleOverflow(bool isFatal, काष्ठा COverflow_तथ्य *p_तथ्य,
			   अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS,
			   स्थिर अक्षर *szOperation)
{
	अगर (!p_तथ्य) {
		वापस;
	}

	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	bool is_signed = p_तथ्य->mType->mTypeInfo & संख्या_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	अगर (is_signed) {
		sbi_printf("%ld %s %ld ", (दीर्घ)ulLHS, szOperation,
			   (दीर्घ)ulRHS);
	} अन्यथा {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_तथ्य->mType->mTypeName);

	SEPARATOR;
}

अटल व्योम HandleNegateOverflow(bool isFatal, काष्ठा COverflow_तथ्य *p_तथ्य,
				 अचिन्हित दीर्घ ul_Old_मूल्य)
{
	अगर (!p_तथ्य) {
		वापस;
	}
	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	bool is_signed = p_तथ्य->mType->mTypeInfo & संख्या_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	अगर (is_signed) {
		sbi_printf("%ld", (दीर्घ)ul_Old_मूल्य);
	} अन्यथा {
		sbi_printf("%lu", ul_Old_मूल्य);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_तथ्य->mType->mTypeName);

	SEPARATOR;
}

अटल व्योम HandleBuiltinUnreachable(bool isFatal,
				     काष्ठा CUnreachable_तथ्य *p_तथ्य)
{
	अगर (!p_तथ्य) {
		वापस;
	}
	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	SEPARATOR;
}

स्थिर अक्षर *rgczTypeCheckKinds[] = { "load of",
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

अटल व्योम HandleTypeMismatch(bool isFatal, काष्ठा CSourceLocation *mLocation,
			       काष्ठा CTypeDescriptor *mType,
			       अचिन्हित दीर्घ mLogAlignment,
			       uint8_t mTypeCheckKind, अचिन्हित दीर्घ ulPointer)
{

	अगर ((!mLocation) || (!mType)) {
		वापस;
	}

	अगर (isAlreadyReported(mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_पंक्ति & ~ACK_REPORTED,
		   mLocation->mColumn);

	स्थिर अक्षर *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	अगर (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} अन्यथा अगर (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (व्योम *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} अन्यथा {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (व्योम *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

अटल व्योम HandleVlaBoundNotPositive(bool isFatal, काष्ठा CVLABound_तथ्य *p_तथ्य,
				      अचिन्हित दीर्घ ulBound)
{
	अगर (!p_तथ्य) {
		वापस;
	}
	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_तथ्य->mType->mTypeInfo & संख्या_SIGNED_BIT;

	अगर (is_signed) {
		sbi_printf("%ld", (दीर्घ)ulBound);
	} अन्यथा {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

अटल व्योम HandleOutOfBounds(bool isFatal, काष्ठा COut_Of_Bounds_तथ्य *p_तथ्य,
			      अचिन्हित दीर्घ ul_सूचकांक)
{
	अगर (!p_तथ्य) {
		वापस;
	}
	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	bool is_signed = p_तथ्य->m_सूचकांक_Type->mTypeInfo & संख्या_SIGNED_BIT;

	अगर (is_signed) {
		sbi_printf("index %ld", (दीर्घ)ul_सूचकांक);
	} अन्यथा {
		sbi_printf("index %lu", ul_सूचकांक);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_तथ्य->mArrayType->mTypeName);

	SEPARATOR;
}

अटल bool is_Negative_संख्या(काष्ठा CTypeDescriptor *pType, अचिन्हित दीर्घ ulVal)
{
	अगर (!(pType->mTypeInfo & संख्या_SIGNED_BIT)) {
		वापस false;
	}

	वापस (दीर्घ)ulVal < 0;
}

अटल माप_प्रकार type_width(काष्ठा CTypeDescriptor *pType)
{
	वापस 1UL << (pType->mTypeInfo >> 1);
}

अटल व्योम HandleShiftOutOfBounds(bool isFatal,
				   काष्ठा CShift_Out_Of_Bounds_तथ्य *p_तथ्य,
				   अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS)
{
	अगर (!p_तथ्य) {
		वापस;
	}

	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	अगर (is_Negative_संख्या(p_तथ्य->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (दीर्घ)ulRHS);
	} अन्यथा अगर (ulRHS >= type_width(p_तथ्य->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (अचिन्हित दीर्घ)type_width(p_तथ्य->mLHSType),
			p_तथ्य->mLHSType->mTypeName);

	} अन्यथा अगर (is_Negative_संख्या(p_तथ्य->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (दीर्घ)ulLHS);
	} अन्यथा {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_तथ्य->mLHSType->mTypeName);
	}

	SEPARATOR;
}

अटल व्योम Handle_Load_Invalid_मूल्य(bool isFatal,
				   काष्ठा CInvalid_मूल्य_तथ्य *p_तथ्य,
				   अचिन्हित दीर्घ ul_मूल्य)
{
	अगर (!p_तथ्य) {
		वापस;
	}

	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	bool is_signed = p_तथ्य->mType->mTypeInfo & संख्या_SIGNED_BIT;

	sbi_printf("load of value ");

	अगर (is_signed) {
		sbi_printf("%ld ", (दीर्घ)ul_मूल्य);
	} अन्यथा {
		sbi_printf("%lu ", ul_मूल्य);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_तथ्य->mType->mTypeName);

	SEPARATOR;
}

स्थिर अक्षर *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

अटल व्योम HandleInvalidBuiltin(bool isFatal,
				 काष्ठा CInvalid_Builtin_तथ्य *p_तथ्य)
{
	अगर (!p_तथ्य) {
		वापस;
	}

	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	स्थिर अक्षर *builtin =
		(p_तथ्य->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_तथ्य->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

अटल व्योम HandleFunctionTypeMismatch(bool isFatal,
				       काष्ठा CFunction_Type_Mismatch_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulFunction)
{
	अगर (!p_तथ्य) {
		वापस;
	}

	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_तथ्य->mType->mTypeName);

	SEPARATOR;
}

अटल व्योम HandleMissingReturn(bool isFatal, काष्ठा CUnreachable_तथ्य *p_तथ्य)
{
	अगर (!p_तथ्य) {
		वापस;
	}

	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

अटल व्योम HandlePointerOverflow(bool isFatal,
				  काष्ठा CPointer_Overflow_तथ्य *p_तथ्य,
				  अचिन्हित दीर्घ ulBase, अचिन्हित दीर्घ ul_परिणाम)
{
	अगर (!p_तथ्य) {
		वापस;
	}

	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ul_परिणाम);

	SEPARATOR;
}

अटल व्योम HandleAlignmentAssumption(bool isFatal,
				      काष्ठा CAlignment_Assumption_तथ्य *p_तथ्य,
				      अचिन्हित दीर्घ ulPointer,
				      अचिन्हित दीर्घ ulAlignment,
				      अचिन्हित दीर्घ ulOffset)
{

	अगर (!p_तथ्य) {
		वापस;
	}

	अगर (isAlreadyReported(&p_तथ्य->mLocation)) {
		वापस;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_तथ्य->mLocation.mFilename,
		   p_तथ्य->mLocation.m_पंक्ति & ~ACK_REPORTED,
		   p_तथ्य->mLocation.mColumn);

	अचिन्हित दीर्घ ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (व्योम *)ulRealPointer, (व्योम *)ulOffset);

	अगर (p_तथ्य->mAssumptionLocation.mFilename != शून्य) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_तथ्य->mAssumptionLocation.mFilename,
			   p_तथ्य->mAssumptionLocation.m_पंक्ति,
			   p_तथ्य->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
व्योम __ubsan_handle_add_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				 अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS)
{
	HandleOverflow(false, p_तथ्य, ulLHS, ulRHS, "+");
}

व्योम __ubsan_handle_add_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS)
{
	HandleOverflow(true, p_तथ्य, ulLHS, ulRHS, "+");
}

व्योम __ubsan_handle_sub_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				 अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS)
{
	HandleOverflow(false, p_तथ्य, ulLHS, ulRHS, "-");
}

व्योम __ubsan_handle_sub_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS)
{
	HandleOverflow(true, p_तथ्य, ulLHS, ulRHS, "-");
}

व्योम __ubsan_handle_mul_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				 अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS)
{
	HandleOverflow(false, p_तथ्य, ulLHS, ulRHS, "*");
}

व्योम __ubsan_handle_mul_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS)
{
	HandleOverflow(true, p_तथ्य, ulLHS, ulRHS, "*");
}

व्योम __ubsan_handle_negate_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				    अचिन्हित दीर्घ ul_Old_मूल्य)
{
	HandleNegateOverflow(false, p_तथ्य, ul_Old_मूल्य);
}

व्योम __ubsan_handle_negate_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
					  अचिन्हित दीर्घ ul_Old_मूल्य)
{
	HandleNegateOverflow(true, p_तथ्य, ul_Old_मूल्य);
}

व्योम __ubsan_handle_divrem_overflow(काष्ठा COverflow_तथ्य *p_तथ्य,
				    अचिन्हित दीर्घ ulLHS, अचिन्हित दीर्घ ulRHS)
{
	HandleOverflow(false, p_तथ्य, ulLHS, ulRHS, "divrem");
}

व्योम __ubsan_handle_divrem_overflow_abort(काष्ठा COverflow_तथ्य *p_तथ्य,
					  अचिन्हित दीर्घ ulLHS,
					  अचिन्हित दीर्घ ulRHS)
{
	HandleOverflow(true, p_तथ्य, ulLHS, ulRHS, "divrem");
}

व्योम __ubsan_handle_type_mismatch_v1(काष्ठा CType_Mismatch_तथ्य_v1 *p_तथ्य,
				     अचिन्हित दीर्घ ulPointer)
{
	HandleTypeMismatch(false, &p_तथ्य->mLocation, p_तथ्य->mType,
			   __BIT(p_तथ्य->mLogAlignment), p_तथ्य->mTypeCheckKind,
			   ulPointer);
}

व्योम __ubsan_handle_type_mismatch_v1_abort(काष्ठा CType_Mismatch_तथ्य_v1 *p_तथ्य,
					   अचिन्हित दीर्घ ulPointer)
{
	HandleTypeMismatch(true, &p_तथ्य->mLocation, p_तथ्य->mType,
			   __BIT(p_तथ्य->mLogAlignment), p_तथ्य->mTypeCheckKind,
			   ulPointer);
}

व्योम __ubsan_handle_out_of_bounds(काष्ठा COut_Of_Bounds_तथ्य *p_तथ्य,
				  अचिन्हित दीर्घ ul_सूचकांक)
{
	HandleOutOfBounds(false, p_तथ्य, ul_सूचकांक);
}

व्योम __ubsan_handle_out_of_bounds_abort(काष्ठा COut_Of_Bounds_तथ्य *p_तथ्य,
					अचिन्हित दीर्घ ul_सूचकांक)
{
	HandleOutOfBounds(true, p_तथ्य, ul_सूचकांक);
}

व्योम __ubsan_handle_shift_out_of_bounds(काष्ठा CShift_Out_Of_Bounds_तथ्य *p_तथ्य,
					अचिन्हित दीर्घ ulLHS,
					अचिन्हित दीर्घ ulRHS)
{
	HandleShiftOutOfBounds(false, p_तथ्य, ulLHS, ulRHS);
}

व्योम __ubsan_handle_shift_out_of_bounds_abort(
	काष्ठा CShift_Out_Of_Bounds_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulLHS,
	अचिन्हित दीर्घ ulRHS)
{
	HandleShiftOutOfBounds(true, p_तथ्य, ulLHS, ulRHS);
}

व्योम __ubsan_handle_pointer_overflow(काष्ठा CPointer_Overflow_तथ्य *p_तथ्य,
				     अचिन्हित दीर्घ ulBase,
				     अचिन्हित दीर्घ ul_परिणाम)
{
	HandlePointerOverflow(false, p_तथ्य, ulBase, ul_परिणाम);
}

व्योम __ubsan_handle_pointer_overflow_abort(काष्ठा CPointer_Overflow_तथ्य *p_तथ्य,
					   अचिन्हित दीर्घ ulBase,
					   अचिन्हित दीर्घ ul_परिणाम)
{
	HandlePointerOverflow(true, p_तथ्य, ulBase, ul_परिणाम);
}

व्योम __ubsan_handle_alignment_assumption(काष्ठा CAlignment_Assumption_तथ्य *p_तथ्य,
					 अचिन्हित दीर्घ ulPointer,
					 अचिन्हित दीर्घ ulAlignment,
					 अचिन्हित दीर्घ ulOffset)
{
	HandleAlignmentAssumption(false, p_तथ्य, ulPointer, ulAlignment,
				  ulOffset);
}

व्योम __ubsan_handle_alignment_assumption_abort(
	काष्ठा CAlignment_Assumption_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulPointer,
	अचिन्हित दीर्घ ulAlignment, अचिन्हित दीर्घ ulOffset)
{
	HandleAlignmentAssumption(true, p_तथ्य, ulPointer, ulAlignment,
				  ulOffset);
}

व्योम __ubsan_handle_builtin_unreachable(काष्ठा CUnreachable_तथ्य *p_तथ्य)
{
	HandleBuiltinUnreachable(true, p_तथ्य);
}

व्योम __ubsan_handle_invalid_builtin(काष्ठा CInvalid_Builtin_तथ्य *p_तथ्य)
{
	HandleInvalidBuiltin(true, p_तथ्य);
}

व्योम __ubsan_handle_invalid_builtin_abort(काष्ठा CInvalid_Builtin_तथ्य *p_तथ्य)
{
	HandleInvalidBuiltin(true, p_तथ्य);
}

व्योम __ubsan_handle_load_invalid_value(काष्ठा CInvalid_मूल्य_तथ्य *p_तथ्य,
				       अचिन्हित दीर्घ ul_मूल्य)
{
	Handle_Load_Invalid_मूल्य(false, p_तथ्य, ul_मूल्य);
}

व्योम __ubsan_handle_load_invalid_value_abort(काष्ठा CInvalid_मूल्य_तथ्य *p_तथ्य,
					     अचिन्हित दीर्घ ul_मूल्य)
{
	Handle_Load_Invalid_मूल्य(true, p_तथ्य, ul_मूल्य);
}

व्योम __ubsan_handle_missing_return(काष्ठा CUnreachable_तथ्य *p_तथ्य)
{
	HandleMissingReturn(true, p_तथ्य);
}

व्योम __ubsan_handle_vla_bound_not_positive(काष्ठा CVLABound_तथ्य *p_तथ्य,
					   अचिन्हित दीर्घ ulBound)
{
	HandleVlaBoundNotPositive(false, p_तथ्य, ulBound);
}

व्योम __ubsan_handle_vla_bound_not_positive_abort(काष्ठा CVLABound_तथ्य *p_तथ्य,
						 अचिन्हित दीर्घ ulBound)
{
	HandleVlaBoundNotPositive(true, p_तथ्य, ulBound);
}

व्योम __ubsan_handle_function_type_mismatch(
	काष्ठा CFunction_Type_Mismatch_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulFunction)
{
	HandleFunctionTypeMismatch(false, p_तथ्य, ulFunction);
}

व्योम __ubsan_handle_function_type_mismatch_abort(
	काष्ठा CFunction_Type_Mismatch_तथ्य *p_तथ्य, अचिन्हित दीर्घ ulFunction)
{
	HandleFunctionTypeMismatch(true, p_तथ्य, ulFunction);
}
#पूर्ण_अगर
