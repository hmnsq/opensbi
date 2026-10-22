/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#यदि_परिभाषितम् UBSAN_ENABLED

#अन्तर्भाव <sbi/sbi_types.h>
#अन्तर्भाव <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#परिभाषय विधा_INTEGER 0
#परिभाषय विधा_FLOAT 1
#परिभाषय विधा_UNKNOWN UINT16_MAX

चञ्चल अचिह्नित दीर्घ sbi_ubsan_report_गणनम्;

संरचना CSourceLocation {
	वर्ण *mFilename;
	uint32_t m_पङ्क्तिः;
	uint32_t mColumn;
};

संरचना CTypeDescriptor {
	uint16_t m_Type_विधा;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

संरचना COverflow_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CUnreachable_दत्त {
	संरचना CSourceLocation mLocation;
};

संरचना CCFICheck_Fail_दत्त {
	uint8_t m_Check_विधा;
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CDynamic_Type_Cache_अलब्धिः_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
	शून्य *mTypeInfo;
	uint8_t m_Type_Check_विधा;
};

संरचना CFunction_Type_Mismatch_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CInvalid_Builtin_दत्त {
	संरचना CSourceLocation mLocation;
	uint8_t m_विधा;
};

संरचना CInvalid_मानम्_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CNon_Null_Arg_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CSourceLocation mAttributeLocation;
	पूर्णाङ्क m_Arg_सूचकांक;
};

संरचना CNon_Null_Return_दत्त {
	संरचना CSourceLocation mAttributeLocation;
};

संरचना COut_सम्बन्धि_Bounds_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mArrayType;
	संरचना CTypeDescriptor *m_सूचकांक_Type;
};

संरचना CPointer_अतिप्रवाहः_दत्त {
	संरचना CSourceLocation mLocation;
};

संरचना CShift_बहिर्गमनम्_सम्बन्धि_Bounds_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mLHSType;
	संरचना CTypeDescriptor *mRHSType;
};

संरचना CType_Mismatch_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
	अचिह्नित दीर्घ mLogAlignment;
	uint8_t m_Type_Check_विधा;
};

संरचना CType_Mismatch_दत्त_v1 {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_विधा;
};

संरचना CVLABound_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CFloat_Cast_अतिप्रवाहः_दत्त {
	संरचना CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	संरचना CTypeDescriptor *mFromType;
	संरचना CTypeDescriptor *m_गन्तव्यम्_Type;
};

संरचना CImplicit_Conversion_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mFromType;
	संरचना CTypeDescriptor *m_गन्तव्यम्_Type;
	uint8_t m_विधा;
};

संरचना CAlignment_Assumption_दत्त {
	संरचना CSourceLocation mLocation;
	संरचना CSourceLocation mAssumptionLocation;
	संरचना CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
शून्य __ubsan_handle_add_overflow(संरचना COverflow_दत्त *p_दत्त,
				 अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_add_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulLHS,
				       अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_alignment_assumption(संरचना CAlignment_Assumption_दत्त *p_दत्त,
					 अचिह्नित दीर्घ ulPointer,
					 अचिह्नित दीर्घ ulAlignment,
					 अचिह्नित दीर्घ ul_विस्थापनम्);
शून्य __ubsan_handle_alignment_assumption_abort(
	संरचना CAlignment_Assumption_दत्त *p_दत्त, अचिह्नित दीर्घ ulPointer,
	अचिह्नित दीर्घ ulAlignment, अचिह्नित दीर्घ ul_विस्थापनम्);
शून्य __ubsan_handle_builtin_unreachable(संरचना CUnreachable_दत्त *p_दत्त);
शून्य __ubsan_handle_divrem_overflow(संरचना COverflow_दत्त *p_दत्त,
				    अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_divrem_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
					  अचिह्नित दीर्घ ulLHS,
					  अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_function_type_mismatch(
	संरचना CFunction_Type_Mismatch_दत्त *p_दत्त, अचिह्नित दीर्घ ulFunction);
शून्य __ubsan_handle_function_type_mismatch_abort(
	संरचना CFunction_Type_Mismatch_दत्त *p_दत्त, अचिह्नित दीर्घ ulFunction);
शून्य __ubsan_handle_function_type_mismatch_v1(
	संरचना CFunction_Type_Mismatch_दत्त *p_दत्त, अचिह्नित दीर्घ ulFunction,
	अचिह्नित दीर्घ ulCalleeRTTI, अचिह्नित दीर्घ ulFnRTTI);
शून्य __ubsan_handle_function_type_mismatch_v1_abort(
	संरचना CFunction_Type_Mismatch_दत्त *p_दत्त, अचिह्नित दीर्घ ulFunction,
	अचिह्नित दीर्घ ulCalleeRTTI, अचिह्नित दीर्घ ulFnRTTI);
शून्य __ubsan_handle_invalid_builtin(संरचना CInvalid_Builtin_दत्त *p_दत्त);
शून्य __ubsan_handle_invalid_builtin_abort(संरचना CInvalid_Builtin_दत्त *p_दत्त);
शून्य __ubsan_handle_load_invalid_value(संरचना CInvalid_मानम्_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulVal);
शून्य __ubsan_handle_load_invalid_value_abort(संरचना CInvalid_मानम्_दत्त *p_दत्त,
					     अचिह्नित दीर्घ ulVal);
शून्य __ubsan_handle_missing_return(संरचना CUnreachable_दत्त *p_दत्त);
शून्य __ubsan_handle_mul_overflow(संरचना COverflow_दत्त *p_दत्त,
				 अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_mul_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulLHS,
				       अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_negate_overflow(संरचना COverflow_दत्त *p_दत्त,
				    अचिह्नित दीर्घ ulOldVal);
शून्य __ubsan_handle_negate_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
					  अचिह्नित दीर्घ ulOldVal);
शून्य __ubsan_handle_nullability_arg(संरचना CNon_Null_Arg_दत्त *p_दत्त);
शून्य __ubsan_handle_nullability_arg_abort(संरचना CNon_Null_Arg_दत्त *p_दत्त);
शून्य __ubsan_handle_nullability_return_v1(
	संरचना CNon_Null_Return_दत्त *p_दत्त,
	संरचना CSourceLocation *pLocationPointer);
शून्य __ubsan_handle_nullability_return_v1_abort(
	संरचना CNon_Null_Return_दत्त *p_दत्त,
	संरचना CSourceLocation *pLocationPointer);
शून्य __ubsan_handle_out_of_bounds(संरचना COut_सम्बन्धि_Bounds_दत्त *p_दत्त,
				  अचिह्नित दीर्घ ul_सूचकांक);
शून्य __ubsan_handle_out_of_bounds_abort(संरचना COut_सम्बन्धि_Bounds_दत्त *p_दत्त,
					अचिह्नित दीर्घ ul_सूचकांक);
शून्य __ubsan_handle_pointer_overflow(संरचना CPointer_अतिप्रवाहः_दत्त *p_दत्त,
				     अचिह्नित दीर्घ ul_आधारः,
				     अचिह्नित दीर्घ ul_परिणाम);
शून्य __ubsan_handle_pointer_overflow_abort(संरचना CPointer_अतिप्रवाहः_दत्त *p_दत्त,
					   अचिह्नित दीर्घ ul_आधारः,
					   अचिह्नित दीर्घ ul_परिणाम);
शून्य __ubsan_handle_shift_out_of_bounds(संरचना CShift_बहिर्गमनम्_सम्बन्धि_Bounds_दत्त *p_दत्त,
					अचिह्नित दीर्घ ulLHS,
					अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_shift_out_of_bounds_abort(
	संरचना CShift_बहिर्गमनम्_सम्बन्धि_Bounds_दत्त *p_दत्त, अचिह्नित दीर्घ ulLHS,
	अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_sub_overflow(संरचना COverflow_दत्त *p_दत्त,
				 अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_sub_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulLHS,
				       अचिह्नित दीर्घ ulRHS);
शून्य __ubsan_handle_type_mismatch(संरचना CType_Mismatch_दत्त *p_दत्त,
				  अचिह्नित दीर्घ ulPointer);
शून्य __ubsan_handle_type_mismatch_abort(संरचना CType_Mismatch_दत्त *p_दत्त,
					अचिह्नित दीर्घ ulPointer);
शून्य __ubsan_handle_type_mismatch_v1(संरचना CType_Mismatch_दत्त_v1 *p_दत्त,
				     अचिह्नित दीर्घ ulPointer);
शून्य __ubsan_handle_type_mismatch_v1_abort(संरचना CType_Mismatch_दत्त_v1 *p_दत्त,
					   अचिह्नित दीर्घ ulPointer);
शून्य __ubsan_handle_vla_bound_not_positive(संरचना CVLABound_दत्त *p_दत्त,
					   अचिह्नित दीर्घ ulBound);
शून्य __ubsan_handle_vla_bound_not_positive_abort(संरचना CVLABound_दत्त *p_दत्त,
						 अचिह्नित दीर्घ ulBound);
शून्य __ubsan_get_current_report_data(ध्रुव वर्ण **pp_बहिर्गमनम्_Issue_विधा,
				     ध्रुव वर्ण **pp_बहिर्गमनम्_सन्देशः,
				     ध्रुव वर्ण **pp_बहिर्गमनम्_Filename,
				     uint32_t *p_बहिर्गमनम्_पङ्क्तिः, uint32_t *p_बहिर्गमनम्_Col,
				     वर्ण **pp_बहिर्गमनम्_स्मृतिः_Addr);
स्थिर शून्य Handle_अतिप्रवाहः(bool isFatal, संरचना COverflow_दत्त *p_दत्त,
			   अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS,
			   ध्रुव वर्ण *szOperation);
स्थिर शून्य Handle_Negate_अतिप्रवाहः(bool isFatal, संरचना COverflow_दत्त *p_दत्त,
				 अचिह्नित दीर्घ ul_Old_मानम्);
स्थिर शून्य HandleBuiltinUnreachable(bool isFatal,
				     संरचना CUnreachable_दत्त *p_दत्त);
स्थिर शून्य HandleTypeMismatch(bool isFatal, संरचना CSourceLocation *mLocation,
			       संरचना CTypeDescriptor *mType,
			       अचिह्नित दीर्घ mLogAlignment,
			       uint8_t m_Type_Check_विधा, अचिह्नित दीर्घ ulPointer);
स्थिर शून्य Handle_Vla_Bound_Not_धनात्मकम्(bool isFatal, संरचना CVLABound_दत्त *p_दत्त,
				      अचिह्नित दीर्घ ulBound);
स्थिर शून्य Handle_बहिर्गमनम्_सम्बन्धि_Bounds(bool isFatal, संरचना COut_सम्बन्धि_Bounds_दत्त *p_दत्त,
			      अचिह्नित दीर्घ ul_सूचकांक);
स्थिर शून्य Handle_सरणम्_बहिर्गमनम्_सम्बन्धि_Bounds(bool isFatal,
				   संरचना CShift_बहिर्गमनम्_सम्बन्धि_Bounds_दत्त *p_दत्त,
				   अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS);
स्थिर शून्य Handle_आरोपय_Invalid_मानम्(bool isFatal,
				   संरचना CInvalid_मानम्_दत्त *p_दत्त,
				   अचिह्नित दीर्घ ul_मानम्);
स्थिर शून्य HandleInvalidBuiltin(bool isFatal,
				 संरचना CInvalid_Builtin_दत्त *p_दत्त);
स्थिर शून्य HandleFunctionTypeMismatch(bool isFatal,
				       संरचना CFunction_Type_Mismatch_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulFunction);
स्थिर शून्य HandleMissingReturn(bool isFatal, संरचना CUnreachable_दत्त *p_दत्त);
स्थिर शून्य Handle_Pointer_अतिप्रवाहः(bool isFatal,
				  संरचना CPointer_अतिप्रवाहः_दत्त *p_दत्त,
				  अचिह्नित दीर्घ ul_आधारः, अचिह्नित दीर्घ ul_परिणाम);
स्थिर शून्य HandleAlignmentAssumption(bool isFatal,
				      संरचना CAlignment_Assumption_दत्त *p_दत्त,
				      अचिह्नित दीर्घ ulPointer,
				      अचिह्नित दीर्घ ulAlignment,
				      अचिह्नित दीर्घ ul_विस्थापनम्);

#परिभाषय सङ्ख्या_SIGNED_BIT 1
#परिभाषय सङ्ख्या_MAXLEN 128
#परिभाषय __arraycount(__a) (परिमाण(__a) / परिमाण(__a[0]))
#परिभाषय __BIT(__n) (1UL << (__n))
#परिभाषय SEPARATOR sbi_printf("===========================================\n")
#परिभाषय ACK_REPORTED (1U << 31)

स्थिर bool isAlreadyReported(संरचना CSourceLocation *pLocation)
{
	uint32_t si_Old_मानम्;
	चञ्चल uint32_t *p_पङ्क्तिः;

	यदि (!pLocation)
		प्रत्यागम false;

	p_पङ्क्तिः = &pLocation->m_पङ्क्तिः;

	कुरु {
		si_Old_मानम् = *p_पङ्क्तिः;
	} यावत् (__sync_val_compare_and_swap(p_पङ्क्तिः, si_Old_मानम्,
					     si_Old_मानम् | ACK_REPORTED) !=
		 si_Old_मानम्);

	यदि (!(si_Old_मानम् & ACK_REPORTED)) {

		sbi_ubsan_report_गणनम्++;

		प्रत्यागम false;
	}

	प्रत्यागम true;
}

स्थिर शून्य Handle_अतिप्रवाहः(bool isFatal, संरचना COverflow_दत्त *p_दत्त,
			   अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS,
			   ध्रुव वर्ण *szOperation)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	bool is_signed = p_दत्त->mType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	यदि (is_signed) {
		sbi_printf("%ld %s %ld ", (दीर्घ)ulLHS, szOperation,
			   (दीर्घ)ulRHS);
	} अन्यथा {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_दत्त->mType->mTypeName);

	SEPARATOR;
}

स्थिर शून्य Handle_Negate_अतिप्रवाहः(bool isFatal, संरचना COverflow_दत्त *p_दत्त,
				 अचिह्नित दीर्घ ul_Old_मानम्)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}
	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	bool is_signed = p_दत्त->mType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	यदि (is_signed) {
		sbi_printf("%ld", (दीर्घ)ul_Old_मानम्);
	} अन्यथा {
		sbi_printf("%lu", ul_Old_मानम्);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_दत्त->mType->mTypeName);

	SEPARATOR;
}

स्थिर शून्य HandleBuiltinUnreachable(bool isFatal,
				     संरचना CUnreachable_दत्त *p_दत्त)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}
	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	SEPARATOR;
}

ध्रुव वर्ण *rgczTypeCheckKinds[] = { "load of",
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

स्थिर शून्य HandleTypeMismatch(bool isFatal, संरचना CSourceLocation *mLocation,
			       संरचना CTypeDescriptor *mType,
			       अचिह्नित दीर्घ mLogAlignment,
			       uint8_t m_Type_Check_विधा, अचिह्नित दीर्घ ulPointer)
{

	यदि ((!mLocation) || (!mType)) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_पङ्क्तिः & ~ACK_REPORTED,
		   mLocation->mColumn);

	ध्रुव वर्ण *विधा = (m_Type_Check_विधा < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_विधा]
				   : "access to";

	यदि (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", विधा,
			   mType->mTypeName);
	} अन्यथा यदि (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			विधा, (शून्य *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} अन्यथा {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			विधा, (शून्य *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

स्थिर शून्य Handle_Vla_Bound_Not_धनात्मकम्(bool isFatal, संरचना CVLABound_दत्त *p_दत्त,
				      अचिह्नित दीर्घ ulBound)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}
	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_दत्त->mType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	यदि (is_signed) {
		sbi_printf("%ld", (दीर्घ)ulBound);
	} अन्यथा {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

स्थिर शून्य Handle_बहिर्गमनम्_सम्बन्धि_Bounds(bool isFatal, संरचना COut_सम्बन्धि_Bounds_दत्त *p_दत्त,
			      अचिह्नित दीर्घ ul_सूचकांक)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}
	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	bool is_signed = p_दत्त->m_सूचकांक_Type->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	यदि (is_signed) {
		sbi_printf("index %ld", (दीर्घ)ul_सूचकांक);
	} अन्यथा {
		sbi_printf("index %lu", ul_सूचकांक);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_दत्त->mArrayType->mTypeName);

	SEPARATOR;
}

स्थिर bool is_Negative_सङ्ख्या(संरचना CTypeDescriptor *pType, अचिह्नित दीर्घ ulVal)
{
	यदि (!(pType->mTypeInfo & सङ्ख्या_SIGNED_BIT)) {
		प्रत्यागम false;
	}

	प्रत्यागम (दीर्घ)ulVal < 0;
}

स्थिर size_t type_विस्तारः(संरचना CTypeDescriptor *pType)
{
	प्रत्यागम 1UL << (pType->mTypeInfo >> 1);
}

स्थिर शून्य Handle_सरणम्_बहिर्गमनम्_सम्बन्धि_Bounds(bool isFatal,
				   संरचना CShift_बहिर्गमनम्_सम्बन्धि_Bounds_दत्त *p_दत्त,
				   अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	यदि (is_Negative_सङ्ख्या(p_दत्त->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (दीर्घ)ulRHS);
	} अन्यथा यदि (ulRHS >= type_विस्तारः(p_दत्त->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (अचिह्नित दीर्घ)type_विस्तारः(p_दत्त->mLHSType),
			p_दत्त->mLHSType->mTypeName);

	} अन्यथा यदि (is_Negative_सङ्ख्या(p_दत्त->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (दीर्घ)ulLHS);
	} अन्यथा {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_दत्त->mLHSType->mTypeName);
	}

	SEPARATOR;
}

स्थिर शून्य Handle_आरोपय_Invalid_मानम्(bool isFatal,
				   संरचना CInvalid_मानम्_दत्त *p_दत्त,
				   अचिह्नित दीर्घ ul_मानम्)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	bool is_signed = p_दत्त->mType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	sbi_printf("load of value ");

	यदि (is_signed) {
		sbi_printf("%ld ", (दीर्घ)ul_मानम्);
	} अन्यथा {
		sbi_printf("%lu ", ul_मानम्);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_दत्त->mType->mTypeName);

	SEPARATOR;
}

ध्रुव वर्ण *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

स्थिर शून्य HandleInvalidBuiltin(bool isFatal,
				 संरचना CInvalid_Builtin_दत्त *p_दत्त)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	ध्रुव वर्ण *builtin =
		(p_दत्त->m_विधा < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_दत्त->m_विधा]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

स्थिर शून्य HandleFunctionTypeMismatch(bool isFatal,
				       संरचना CFunction_Type_Mismatch_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulFunction)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_दत्त->mType->mTypeName);

	SEPARATOR;
}

स्थिर शून्य HandleMissingReturn(bool isFatal, संरचना CUnreachable_दत्त *p_दत्त)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

स्थिर शून्य Handle_Pointer_अतिप्रवाहः(bool isFatal,
				  संरचना CPointer_अतिप्रवाहः_दत्त *p_दत्त,
				  अचिह्नित दीर्घ ul_आधारः, अचिह्नित दीर्घ ul_परिणाम)
{
	यदि (!p_दत्त) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_आधारः, ul_परिणाम);

	SEPARATOR;
}

स्थिर शून्य HandleAlignmentAssumption(bool isFatal,
				      संरचना CAlignment_Assumption_दत्त *p_दत्त,
				      अचिह्नित दीर्घ ulPointer,
				      अचिह्नित दीर्घ ulAlignment,
				      अचिह्नित दीर्घ ul_विस्थापनम्)
{

	यदि (!p_दत्त) {
		प्रत्यागम;
	}

	यदि (isAlreadyReported(&p_दत्त->mLocation)) {
		प्रत्यागम;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_दत्त->mLocation.mFilename,
		   p_दत्त->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   p_दत्त->mLocation.mColumn);

	अचिह्नित दीर्घ ulRealPointer = ulPointer - ul_विस्थापनम्;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (शून्य *)ulRealPointer, (शून्य *)ul_विस्थापनम्);

	यदि (p_दत्त->mAssumptionLocation.mFilename != रिक्त) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_दत्त->mAssumptionLocation.mFilename,
			   p_दत्त->mAssumptionLocation.m_पङ्क्तिः,
			   p_दत्त->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
शून्य __ubsan_handle_add_overflow(संरचना COverflow_दत्त *p_दत्त,
				 अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS)
{
	Handle_अतिप्रवाहः(false, p_दत्त, ulLHS, ulRHS, "+");
}

शून्य __ubsan_handle_add_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS)
{
	Handle_अतिप्रवाहः(true, p_दत्त, ulLHS, ulRHS, "+");
}

शून्य __ubsan_handle_sub_overflow(संरचना COverflow_दत्त *p_दत्त,
				 अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS)
{
	Handle_अतिप्रवाहः(false, p_दत्त, ulLHS, ulRHS, "-");
}

शून्य __ubsan_handle_sub_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS)
{
	Handle_अतिप्रवाहः(true, p_दत्त, ulLHS, ulRHS, "-");
}

शून्य __ubsan_handle_mul_overflow(संरचना COverflow_दत्त *p_दत्त,
				 अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS)
{
	Handle_अतिप्रवाहः(false, p_दत्त, ulLHS, ulRHS, "*");
}

शून्य __ubsan_handle_mul_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS)
{
	Handle_अतिप्रवाहः(true, p_दत्त, ulLHS, ulRHS, "*");
}

शून्य __ubsan_handle_negate_overflow(संरचना COverflow_दत्त *p_दत्त,
				    अचिह्नित दीर्घ ul_Old_मानम्)
{
	Handle_Negate_अतिप्रवाहः(false, p_दत्त, ul_Old_मानम्);
}

शून्य __ubsan_handle_negate_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
					  अचिह्नित दीर्घ ul_Old_मानम्)
{
	Handle_Negate_अतिप्रवाहः(true, p_दत्त, ul_Old_मानम्);
}

शून्य __ubsan_handle_divrem_overflow(संरचना COverflow_दत्त *p_दत्त,
				    अचिह्नित दीर्घ ulLHS, अचिह्नित दीर्घ ulRHS)
{
	Handle_अतिप्रवाहः(false, p_दत्त, ulLHS, ulRHS, "divrem");
}

शून्य __ubsan_handle_divrem_overflow_abort(संरचना COverflow_दत्त *p_दत्त,
					  अचिह्नित दीर्घ ulLHS,
					  अचिह्नित दीर्घ ulRHS)
{
	Handle_अतिप्रवाहः(true, p_दत्त, ulLHS, ulRHS, "divrem");
}

शून्य __ubsan_handle_type_mismatch_v1(संरचना CType_Mismatch_दत्त_v1 *p_दत्त,
				     अचिह्नित दीर्घ ulPointer)
{
	HandleTypeMismatch(false, &p_दत्त->mLocation, p_दत्त->mType,
			   __BIT(p_दत्त->mLogAlignment), p_दत्त->m_Type_Check_विधा,
			   ulPointer);
}

शून्य __ubsan_handle_type_mismatch_v1_abort(संरचना CType_Mismatch_दत्त_v1 *p_दत्त,
					   अचिह्नित दीर्घ ulPointer)
{
	HandleTypeMismatch(true, &p_दत्त->mLocation, p_दत्त->mType,
			   __BIT(p_दत्त->mLogAlignment), p_दत्त->m_Type_Check_विधा,
			   ulPointer);
}

शून्य __ubsan_handle_out_of_bounds(संरचना COut_सम्बन्धि_Bounds_दत्त *p_दत्त,
				  अचिह्नित दीर्घ ul_सूचकांक)
{
	Handle_बहिर्गमनम्_सम्बन्धि_Bounds(false, p_दत्त, ul_सूचकांक);
}

शून्य __ubsan_handle_out_of_bounds_abort(संरचना COut_सम्बन्धि_Bounds_दत्त *p_दत्त,
					अचिह्नित दीर्घ ul_सूचकांक)
{
	Handle_बहिर्गमनम्_सम्बन्धि_Bounds(true, p_दत्त, ul_सूचकांक);
}

शून्य __ubsan_handle_shift_out_of_bounds(संरचना CShift_बहिर्गमनम्_सम्बन्धि_Bounds_दत्त *p_दत्त,
					अचिह्नित दीर्घ ulLHS,
					अचिह्नित दीर्घ ulRHS)
{
	Handle_सरणम्_बहिर्गमनम्_सम्बन्धि_Bounds(false, p_दत्त, ulLHS, ulRHS);
}

शून्य __ubsan_handle_shift_out_of_bounds_abort(
	संरचना CShift_बहिर्गमनम्_सम्बन्धि_Bounds_दत्त *p_दत्त, अचिह्नित दीर्घ ulLHS,
	अचिह्नित दीर्घ ulRHS)
{
	Handle_सरणम्_बहिर्गमनम्_सम्बन्धि_Bounds(true, p_दत्त, ulLHS, ulRHS);
}

शून्य __ubsan_handle_pointer_overflow(संरचना CPointer_अतिप्रवाहः_दत्त *p_दत्त,
				     अचिह्नित दीर्घ ul_आधारः,
				     अचिह्नित दीर्घ ul_परिणाम)
{
	Handle_Pointer_अतिप्रवाहः(false, p_दत्त, ul_आधारः, ul_परिणाम);
}

शून्य __ubsan_handle_pointer_overflow_abort(संरचना CPointer_अतिप्रवाहः_दत्त *p_दत्त,
					   अचिह्नित दीर्घ ul_आधारः,
					   अचिह्नित दीर्घ ul_परिणाम)
{
	Handle_Pointer_अतिप्रवाहः(true, p_दत्त, ul_आधारः, ul_परिणाम);
}

शून्य __ubsan_handle_alignment_assumption(संरचना CAlignment_Assumption_दत्त *p_दत्त,
					 अचिह्नित दीर्घ ulPointer,
					 अचिह्नित दीर्घ ulAlignment,
					 अचिह्नित दीर्घ ul_विस्थापनम्)
{
	HandleAlignmentAssumption(false, p_दत्त, ulPointer, ulAlignment,
				  ul_विस्थापनम्);
}

शून्य __ubsan_handle_alignment_assumption_abort(
	संरचना CAlignment_Assumption_दत्त *p_दत्त, अचिह्नित दीर्घ ulPointer,
	अचिह्नित दीर्घ ulAlignment, अचिह्नित दीर्घ ul_विस्थापनम्)
{
	HandleAlignmentAssumption(true, p_दत्त, ulPointer, ulAlignment,
				  ul_विस्थापनम्);
}

शून्य __ubsan_handle_builtin_unreachable(संरचना CUnreachable_दत्त *p_दत्त)
{
	HandleBuiltinUnreachable(true, p_दत्त);
}

शून्य __ubsan_handle_invalid_builtin(संरचना CInvalid_Builtin_दत्त *p_दत्त)
{
	HandleInvalidBuiltin(true, p_दत्त);
}

शून्य __ubsan_handle_invalid_builtin_abort(संरचना CInvalid_Builtin_दत्त *p_दत्त)
{
	HandleInvalidBuiltin(true, p_दत्त);
}

शून्य __ubsan_handle_load_invalid_value(संरचना CInvalid_मानम्_दत्त *p_दत्त,
				       अचिह्नित दीर्घ ul_मानम्)
{
	Handle_आरोपय_Invalid_मानम्(false, p_दत्त, ul_मानम्);
}

शून्य __ubsan_handle_load_invalid_value_abort(संरचना CInvalid_मानम्_दत्त *p_दत्त,
					     अचिह्नित दीर्घ ul_मानम्)
{
	Handle_आरोपय_Invalid_मानम्(true, p_दत्त, ul_मानम्);
}

शून्य __ubsan_handle_missing_return(संरचना CUnreachable_दत्त *p_दत्त)
{
	HandleMissingReturn(true, p_दत्त);
}

शून्य __ubsan_handle_vla_bound_not_positive(संरचना CVLABound_दत्त *p_दत्त,
					   अचिह्नित दीर्घ ulBound)
{
	Handle_Vla_Bound_Not_धनात्मकम्(false, p_दत्त, ulBound);
}

शून्य __ubsan_handle_vla_bound_not_positive_abort(संरचना CVLABound_दत्त *p_दत्त,
						 अचिह्नित दीर्घ ulBound)
{
	Handle_Vla_Bound_Not_धनात्मकम्(true, p_दत्त, ulBound);
}

शून्य __ubsan_handle_function_type_mismatch(
	संरचना CFunction_Type_Mismatch_दत्त *p_दत्त, अचिह्नित दीर्घ ulFunction)
{
	HandleFunctionTypeMismatch(false, p_दत्त, ulFunction);
}

शून्य __ubsan_handle_function_type_mismatch_abort(
	संरचना CFunction_Type_Mismatch_दत्त *p_दत्त, अचिह्नित दीर्घ ulFunction)
{
	HandleFunctionTypeMismatch(true, p_दत्त, ulFunction);
}
#यदि_अन्तः
