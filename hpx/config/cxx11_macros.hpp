//  Copyright (c) 2013 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  boost-no-inspect : do not report deprecated macros in this file

#ifndef HPX_CONFIG_CXX11_MACROS_HPP
#define HPX_CONFIG_CXX11_MACROS_HPP

#include <boost/config.hpp>
#include <boost/version.hpp>

//  -------------------- Macros introduced at 1.44 ---------------------------
#if BOOST_VERSION < 104400

//  Define BOOST_NO_0X_HDR_TYPEINDEX fallback
#if !defined(BOOST_NO_0X_HDR_TYPEINDEX)
#  define BOOST_NO_0X_HDR_TYPEINDEX
#endif

#endif
//  ------------------ End of macros introduced at 1.44 ----------------------

//  -------------------- Macros introduced at 1.45 ---------------------------
#if BOOST_VERSION < 104500

//  Define BOOST_NO_VARIADIC_MACROS fallback
#if !defined(BOOST_NO_VARIADIC_MACROS)
#  define BOOST_NO_VARIADIC_MACROS
#endif

#endif
//  ------------------ End of macros introduced at 1.45 ----------------------

//  -------------------- Macros introduced at 1.47 ---------------------------
#if BOOST_VERSION < 104700

//  Define BOOST_NO_NOEXCEPT fallback
#if !defined(BOOST_NO_NOEXCEPT)
#  define BOOST_NO_NOEXCEPT
#endif
//  Define BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX fallback
#if !defined(BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX)
#  define BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX
#endif

#endif
//  ------------------ End of macros introduced at 1.47 ----------------------

//  -------------------- Macros introduced at 1.48 ---------------------------
#if BOOST_VERSION < 104800

//  Define BOOST_NO_DECLTYPE_N3276 fallback
#if !defined(BOOST_NO_DECLTYPE_N3276)
#  define BOOST_NO_DECLTYPE_N3276
#endif

#endif
//  ------------------ End of macros introduced at 1.48 ----------------------

//  -------------------- Macros introduced at 1.50 ---------------------------
#if BOOST_VERSION < 105000

//  Define BOOST_NO_CXX11_ALLOCATOR fallback
#if !defined(BOOST_NO_CXX11_ALLOCATOR)
#  define BOOST_NO_CXX11_ALLOCATOR
#endif
//  Define BOOST_NO_CXX11_ATOMIC_SP fallback
#if !defined(BOOST_NO_CXX11_ATOMIC_SP)
#  define BOOST_NO_CXX11_ATOMIC_SP
#endif
//  Define BOOST_NO_CXX11_HDR_FUNCTIONAL fallback
#if !defined(BOOST_NO_CXX11_HDR_FUNCTIONAL)
#  define BOOST_NO_CXX11_HDR_FUNCTIONAL
#endif
//  Define BOOST_NO_CXX11_SMART_PTR fallback
#if !defined(BOOST_NO_CXX11_SMART_PTR)
#  define BOOST_NO_CXX11_SMART_PTR
#endif
//  Define BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS fallback
#if !defined(BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS)
#  define BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
#endif

//  Define BOOST_NO_CXX11_HDR_ARRAY in terms of BOOST_NO_0X_HDR_ARRAY
#if defined(BOOST_NO_0X_HDR_ARRAY) && !defined(BOOST_NO_CXX11_HDR_ARRAY)
#  define BOOST_NO_CXX11_HDR_ARRAY
#endif
//  Define BOOST_NO_CXX11_HDR_CHRONO in terms of BOOST_NO_0X_HDR_CHRONO
#if defined(BOOST_NO_0X_HDR_CHRONO) && !defined(BOOST_NO_CXX11_HDR_CHRONO)
#  define BOOST_NO_CXX11_HDR_CHRONO
#endif
//  Define BOOST_NO_CXX11_HDR_CODECVT in terms of BOOST_NO_0X_HDR_CODECVT
#if defined(BOOST_NO_0X_HDR_CODECVT) && !defined(BOOST_NO_CXX11_HDR_CODECVT)
#  define BOOST_NO_CXX11_HDR_CODECVT
#endif
//  Define BOOST_NO_CXX11_HDR_CONDITION_VARIABLE in terms of BOOST_NO_0X_HDR_CONDITION_VARIABLE
#if defined(BOOST_NO_0X_HDR_CONDITION_VARIABLE) && !defined(BOOST_NO_CXX11_HDR_CONDITION_VARIABLE)
#  define BOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#endif
//  Define BOOST_NO_CXX11_HDR_FORWARD_LIST in terms of BOOST_NO_0X_HDR_FORWARD_LIST
#if defined(BOOST_NO_0X_HDR_FORWARD_LIST) && !defined(BOOST_NO_CXX11_HDR_FORWARD_LIST)
#  define BOOST_NO_CXX11_HDR_FORWARD_LIST
#endif
//  Define BOOST_NO_CXX11_HDR_FUTURE in terms of BOOST_NO_0X_HDR_FUTURE
#if defined(BOOST_NO_0X_HDR_FUTURE) && !defined(BOOST_NO_CXX11_HDR_FUTURE)
#  define BOOST_NO_CXX11_HDR_FUTURE
#endif
//  Define BOOST_NO_CXX11_HDR_INITIALIZER_LIST
//  in terms of BOOST_NO_0X_HDR_INITIALIZER_LIST or BOOST_NO_INITIALIZER_LISTS
#if defined(BOOST_NO_0X_HDR_INITIALIZER_LIST) || defined(BOOST_NO_INITIALIZER_LISTS)
#  if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
#    define BOOST_NO_CXX11_HDR_INITIALIZER_LIST
#  endif
#endif
//  Define BOOST_NO_CXX11_HDR_MUTEX in terms of BOOST_NO_0X_HDR_MUTEX
#if defined(BOOST_NO_0X_HDR_MUTEX) && !defined(BOOST_NO_CXX11_HDR_MUTEX)
#  define BOOST_NO_CXX11_HDR_MUTEX
#endif
//  Define BOOST_NO_CXX11_HDR_RANDOM in terms of BOOST_NO_0X_HDR_RANDOM
#if defined(BOOST_NO_0X_HDR_RANDOM) && !defined(BOOST_NO_CXX11_HDR_RANDOM)
#  define BOOST_NO_CXX11_HDR_RANDOM
#endif
//  Define BOOST_NO_CXX11_HDR_RATIO in terms of BOOST_NO_0X_HDR_RATIO
#if defined(BOOST_NO_0X_HDR_RATIO) && !defined(BOOST_NO_CXX11_HDR_RATIO)
#  define BOOST_NO_CXX11_HDR_RATIO
#endif
//  Define BOOST_NO_CXX11_HDR_REGEX in terms of BOOST_NO_0X_HDR_REGEX
#if defined(BOOST_NO_0X_HDR_REGEX) && !defined(BOOST_NO_CXX11_HDR_REGEX)
#  define BOOST_NO_CXX11_HDR_REGEX
#endif
//  Define BOOST_NO_CXX11_HDR_SYSTEM_ERROR in terms of BOOST_NO_0X_HDR_SYSTEM_ERROR
#if defined(BOOST_NO_0X_HDR_SYSTEM_ERROR) && !defined(BOOST_NO_CXX11_HDR_SYSTEM_ERROR)
#  define BOOST_NO_CXX11_HDR_SYSTEM_ERROR
#endif
//  Define BOOST_NO_CXX11_HDR_THREAD in terms of BOOST_NO_0X_HDR_THREAD
#if defined(BOOST_NO_0X_HDR_THREAD) && !defined(BOOST_NO_CXX11_HDR_THREAD)
#  define BOOST_NO_CXX11_HDR_THREAD
#endif
//  Define BOOST_NO_CXX11_HDR_TUPLE in terms of BOOST_NO_0X_HDR_TUPLE
#if defined(BOOST_NO_0X_HDR_TUPLE) && !defined(BOOST_NO_CXX11_HDR_TUPLE)
#  define BOOST_NO_CXX11_HDR_TUPLE
#endif
//  Define BOOST_NO_CXX11_HDR_TYPE_TRAITS in terms of BOOST_NO_0X_HDR_TYPE_TRAITS
#if defined(BOOST_NO_0X_HDR_TYPE_TRAITS) && !defined(BOOST_NO_CXX11_HDR_TYPE_TRAITS)
#  define BOOST_NO_CXX11_HDR_TYPE_TRAITS
#endif
//  Define BOOST_NO_CXX11_HDR_TYPEINDEX in terms of BOOST_NO_0X_HDR_TYPEINDEX
#if defined(BOOST_NO_0X_HDR_TYPEINDEX) && !defined(BOOST_NO_CXX11_HDR_TYPEINDEX)
#  define BOOST_NO_CXX11_HDR_TYPEINDEX
#endif
//  Define BOOST_NO_CXX11_HDR_UNORDERED_SET in terms of BOOST_NO_0X_HDR_UNORDERED_SET
#if defined(BOOST_NO_0X_HDR_UNORDERED_SET) && !defined(BOOST_NO_CXX11_HDR_UNORDERED_SET)
#  define BOOST_NO_CXX11_HDR_UNORDERED_SET
#endif
//  Define BOOST_NO_CXX11_HDR_UNORDERED_MAP in terms of BOOST_NO_0X_HDR_UNORDERED_MAP
#if defined(BOOST_NO_0X_HDR_UNORDERED_MAP) && !defined(BOOST_NO_CXX11_HDR_UNORDERED_MAP)
#  define BOOST_NO_CXX11_HDR_UNORDERED_MAP
#endif
//  Define BOOST_NO_CXX11_STD_UNORDERED in terms of BOOST_NO_STD_UNORDERED
#if defined(BOOST_NO_STD_UNORDERED) && !defined(BOOST_NO_CXX11_STD_UNORDERED)
#  define BOOST_NO_CXX11_STD_UNORDERED
#endif

#endif
//  ------------------ End of macros introduced at 1.50 ----------------------

//  -------------------- Macros introduced at 1.51 ---------------------------
#if BOOST_VERSION < 105100

//  Define BOOST_NO_CXX11_AUTO_DECLARATIONS in terms of BOOST_NO_AUTO_DECLARATIONS
#if defined(BOOST_NO_AUTO_DECLARATIONS) && !defined(BOOST_NO_CXX11_AUTO_DECLARATIONS)
#  define BOOST_NO_CXX11_AUTO_DECLARATIONS
#endif
//  Define BOOST_NO_CXX11_AUTO_MULTIDECLARATIONS in terms of BOOST_NO_AUTO_MULTIDECLARATIONS
#if defined(BOOST_NO_AUTO_MULTIDECLARATIONS) && !defined(BOOST_NO_CXX11_AUTO_MULTIDECLARATIONS)
#  define BOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#endif
//  Define BOOST_NO_CXX11_CHAR16_T in terms of BOOST_NO_CHAR16_T
#if defined(BOOST_NO_CHAR16_T) && !defined(BOOST_NO_CXX11_CHAR16_T)
#  define BOOST_NO_CXX11_CHAR16_T
#endif
//  Define BOOST_NO_CXX11_CHAR32_T in terms of BOOST_NO_CHAR32_T
#if defined(BOOST_NO_CHAR32_T) && !defined(BOOST_NO_CXX11_CHAR32_T)
#  define BOOST_NO_CXX11_CHAR32_T
#endif
//  Define BOOST_NO_CXX11_TEMPLATE_ALIASES in terms of BOOST_NO_TEMPLATE_ALIASES
#if defined(BOOST_NO_TEMPLATE_ALIASES) && !defined(BOOST_NO_CXX11_TEMPLATE_ALIASES)
#  define BOOST_NO_CXX11_TEMPLATE_ALIASES
#endif
//  Define BOOST_NO_CXX11_CONSTEXPR in terms of BOOST_NO_CONSTEXPR
#if defined(BOOST_NO_CONSTEXPR) && !defined(BOOST_NO_CXX11_CONSTEXPR)
#  define BOOST_NO_CXX11_CONSTEXPR
#endif
//  Define BOOST_NO_CXX11_DECLTYPE in terms of BOOST_NO_DECLTYPE
#if defined(BOOST_NO_DECLTYPE) && !defined(BOOST_NO_CXX11_DECLTYPE)
#  define BOOST_NO_CXX11_DECLTYPE
#endif
//  Define BOOST_NO_CXX11_DECLTYPE_N3276 in terms of BOOST_NO_DECLTYPE_N3276
#if defined(BOOST_NO_DECLTYPE_N3276) && !defined(BOOST_NO_CXX11_DECLTYPE_N3276)
#  define BOOST_NO_CXX11_DECLTYPE_N3276
#endif
//  Define BOOST_NO_CXX11_DEFAULTED_FUNCTIONS in terms of BOOST_NO_DEFAULTED_FUNCTIONS
#if defined(BOOST_NO_DEFAULTED_FUNCTIONS) && !defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
#  define BOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#endif
//  Define BOOST_NO_CXX11_DELETED_FUNCTIONS in terms of BOOST_NO_DELETED_FUNCTIONS
#if defined(BOOST_NO_DELETED_FUNCTIONS) && !defined(BOOST_NO_CXX11_DELETED_FUNCTIONS)
#  define BOOST_NO_CXX11_DELETED_FUNCTIONS
#endif
//  Define BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS in terms of BOOST_NO_EXPLICIT_CONVERSION_OPERATORS
#if defined(BOOST_NO_EXPLICIT_CONVERSION_OPERATORS) && !defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
#  define BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#endif
//  Define BOOST_NO_CXX11_EXTERN_TEMPLATE in terms of BOOST_NO_EXTERN_TEMPLATE
#if defined(BOOST_NO_EXTERN_TEMPLATE) && !defined(BOOST_NO_CXX11_EXTERN_TEMPLATE)
#  define BOOST_NO_CXX11_EXTERN_TEMPLATE
#endif
//  Define BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS in terms of BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#if defined(BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS) && !defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS)
#  define BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#endif
//  Define BOOST_NO_CXX11_LAMBDAS in terms of BOOST_NO_LAMBDAS
#if defined(BOOST_NO_LAMBDAS) && !defined(BOOST_NO_CXX11_LAMBDAS)
#  define BOOST_NO_CXX11_LAMBDAS
#endif
//  Define BOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS in terms of BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
#if defined(BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS) && !defined(BOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS)
#  define BOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#endif
//  Define BOOST_NO_CXX11_NOEXCEPT in terms of BOOST_NO_NOEXCEPT
#if defined(BOOST_NO_NOEXCEPT) && !defined(BOOST_NO_CXX11_NOEXCEPT)
#  define BOOST_NO_CXX11_NOEXCEPT
#endif
//  Define BOOST_NO_CXX11_NULLPTR in terms of BOOST_NO_NULLPTR
#if defined(BOOST_NO_NULLPTR) && !defined(BOOST_NO_CXX11_NULLPTR)
#  define BOOST_NO_CXX11_NULLPTR
#endif
//  Define BOOST_NO_CXX11_RAW_LITERALS in terms of BOOST_NO_RAW_LITERALS
#if defined(BOOST_NO_RAW_LITERALS) && !defined(BOOST_NO_CXX11_RAW_LITERALS)
#  define BOOST_NO_CXX11_RAW_LITERALS
#endif
//  Define BOOST_NO_CXX11_RVALUE_REFERENCES in terms of BOOST_NO_RVALUE_REFERENCES
#if defined(BOOST_NO_RVALUE_REFERENCES) && !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#  define BOOST_NO_CXX11_RVALUE_REFERENCES
#endif
//  Define BOOST_NO_CXX11_SCOPED_ENUMS in terms of BOOST_NO_SCOPED_ENUMS
#if defined(BOOST_NO_SCOPED_ENUMS) && !defined(BOOST_NO_CXX11_SCOPED_ENUMS)
#  define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
//  Define BOOST_NO_CXX11_STATIC_ASSERT in terms of BOOST_NO_STATIC_ASSERT
#if defined(BOOST_NO_STATIC_ASSERT) && !defined(BOOST_NO_CXX11_STATIC_ASSERT)
#  define BOOST_NO_CXX11_STATIC_ASSERT
#endif
//  Define BOOST_NO_CXX11_STD_UNORDERED in terms of BOOST_NO_STD_UNORDERED
#if defined(BOOST_NO_STD_UNORDERED) && !defined(BOOST_NO_CXX11_STD_UNORDERED)
#  define BOOST_NO_CXX11_STD_UNORDERED
#endif
//  Define BOOST_NO_CXX11_UNICODE_LITERALS in terms of BOOST_NO_UNICODE_LITERALS
#if defined(BOOST_NO_UNICODE_LITERALS) && !defined(BOOST_NO_CXX11_UNICODE_LITERALS)
#  define BOOST_NO_CXX11_UNICODE_LITERALS
#endif
//  Define BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX in terms of BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX
#if defined(BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX) && !defined(BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX)
#  define BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#endif
//  Define BOOST_NO_CXX11_VARIADIC_TEMPLATES in terms of BOOST_NO_VARIADIC_TEMPLATES
#if defined(BOOST_NO_VARIADIC_TEMPLATES) && !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
#  define BOOST_NO_CXX11_VARIADIC_TEMPLATES
#endif
//  Define BOOST_NO_CXX11_VARIADIC_MACROS in terms of BOOST_NO_VARIADIC_MACROS
#if defined(BOOST_NO_VARIADIC_MACROS) && !defined(BOOST_NO_CXX11_VARIADIC_MACROS)
#  define BOOST_NO_CXX11_VARIADIC_MACROS
#endif
//  Define BOOST_NO_CXX11_NUMERIC_LIMITS in terms of BOOST_NO_NUMERIC_LIMITS_LOWEST
#if defined(BOOST_NO_NUMERIC_LIMITS_LOWEST) && !defined(BOOST_NO_CXX11_NUMERIC_LIMITS)
#  define BOOST_NO_CXX11_NUMERIC_LIMITS
#endif

#endif
//  ------------------ End of macros introduced at 1.51 ----------------------

//  -------------------- Macros introduced at 1.54 ---------------------------
#if BOOST_VERSION < 105400

//  Define BOOST_NO_CXX11_USER_DEFINED_LITERALS fallback
#if !defined(BOOST_NO_CXX11_USER_DEFINED_LITERALS)
#  define BOOST_NO_CXX11_USER_DEFINED_LITERALS
#endif

#endif
//  ------------------ End of macros introduced at 1.54 ----------------------

//  -------------------- Macros introduced at 1.55 ---------------------------
#if BOOST_VERSION < 105500

//  Define BOOST_NO_CXX11_ALIGNAS fallback
#if !defined(BOOST_NO_CXX11_ALIGNAS)
#  define BOOST_NO_CXX11_ALIGNAS
#endif

//  Define BOOST_NO_CXX11_INLINE_NAMESPACES fallback
#if !defined(BOOST_NO_CXX11_INLINE_NAMESPACES)
#  define BOOST_NO_CXX11_INLINE_NAMESPACES
#endif

//  Define BOOST_NO_CXX11_TRAILING_RESULT_TYPES fallback
#if !defined(BOOST_NO_CXX11_TRAILING_RESULT_TYPES)
#  define BOOST_NO_CXX11_TRAILING_RESULT_TYPES
#endif

#endif
//  ------------------ End of macros introduced at 1.55 ----------------------

//  -------------------- Macros introduced at 1.56 ---------------------------
#if BOOST_VERSION < 105600

//  Define BOOST_NO_CXX11_ADDRESSOF fallback
#if !defined(BOOST_NO_CXX11_ADDRESSOF)
#  define BOOST_NO_CXX11_ADDRESSOF
#endif

//  Define BOOST_NO_CXX11_ATOMIC_SMART_PTR fallback
#if !defined(BOOST_NO_CXX11_ATOMIC_SMART_PTR)
#  define BOOST_NO_CXX11_ATOMIC_SMART_PTR
#endif

//  Define BOOST_NO_CXX11_HDR_ATOMIC fallback
#if !defined(BOOST_NO_CXX11_HDR_ATOMIC)
#  define BOOST_NO_CXX11_HDR_ATOMIC
#endif

//  Define BOOST_NO_CXX11_STD_ALIGN fallback
#if !defined(BOOST_NO_CXX11_STD_ALIGN)
#  define BOOST_NO_CXX11_STD_ALIGN
#endif

//  Define BOOST_NO_CXX11_REF_QUALIFIERS fallback
#if !defined(BOOST_NO_CXX11_REF_QUALIFIERS)
#  define BOOST_NO_CXX11_REF_QUALIFIERS
#endif

// Boost versions before 1.56 may define this unconditionally
#if defined(BOOST_MSVC)
#undef BOOST_NO_CXX11_VARIADIC_TEMPLATES
#endif

#endif
//  ------------------ End of macros introduced at 1.56 ----------------------

#endif