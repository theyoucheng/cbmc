/*******************************************************************\

Module: Generates string constraints to link results from string functions
        with their arguments. This is inspired by the PASS paper at HVC'13:
        "PASS: String Solving with Parameterized Array and Interval Automaton"
        by Guodong Li and Indradeep Ghosh, which gives examples of constraints
        for several functions.

Author: Romain Brenguier, romain.brenguier@diffblue.com

\*******************************************************************/

/// \file
/// Generates string constraints to link results from string functions with
///   their arguments. This is inspired by the PASS paper at HVC'13: "PASS:
///   String Solving with Parameterized Array and Interval Automaton" by Guodong
///   Li and Indradeep Ghosh, which gives examples of constraints for several
///   functions.

#ifndef CPROVER_SOLVERS_REFINEMENT_STRING_CONSTRAINT_GENERATOR_H
#define CPROVER_SOLVERS_REFINEMENT_STRING_CONSTRAINT_GENERATOR_H

#include <limits>
#include <util/string_expr.h>
#include <util/replace_expr.h>
#include <util/refined_string_type.h>
#include <util/constexpr.def>
#include <solvers/refinement/string_constraint.h>

class string_constraint_generatort final
{
public:
  // This module keeps a list of axioms. It has methods which generate
  // string constraints for different string functions and add them
  // to the axiom list.

  // Used by format function
  class format_specifiert;

  /// Arguments pack for the string_constraint_generator constructor
  struct infot
  {
    const namespacet *ns=nullptr;
    /// Max length of non-deterministic strings
    size_t string_max_length=std::numeric_limits<size_t>::max();
    /// Prefer printable characters in non-deterministic strings
    bool string_printable=false;
  };

  explicit string_constraint_generatort(const infot& info);

  /// Axioms are of three kinds: universally quantified string constraint,
  /// not contains string constraints and simple formulas.
  const std::vector<exprt> &get_axioms() const;

  /// Boolean symbols for the results of some string functions
  const std::vector<symbol_exprt> &get_boolean_symbols() const;

  /// Symbols used in existential quantifications
  const std::vector<symbol_exprt> &get_index_symbols() const;

  /// Set of strings that have been created by the generator
  const std::set<string_exprt> &get_created_strings() const;

  exprt get_witness_of(
    const string_not_contains_constraintt &c,
    const exprt &univ_val) const
  {
    return index_exprt(witness.at(c), univ_val);
  }

  symbol_exprt fresh_symbol(
    const irep_idt &prefix, const typet &type=bool_typet());
  symbol_exprt fresh_univ_index(const irep_idt &prefix, const typet &type);

  /// Maps unresolved symbols to the string_exprt that was created for them
  string_exprt add_axioms_for_refined_string(const exprt &expr);

  exprt add_axioms_for_function_application(
    const function_application_exprt &expr);

  symbol_exprt fresh_exist_index(const irep_idt &prefix, const typet &type);

private:
  symbol_exprt fresh_boolean(const irep_idt &prefix);
  string_exprt fresh_string(const refined_string_typet &type);
  string_exprt get_string_expr(const exprt &expr);
  plus_exprt plus_exprt_with_overflow_check(const exprt &op1, const exprt &op2);

  string_exprt find_or_add_string_of_symbol(
    const symbol_exprt &sym,
    const refined_string_typet &ref_type);

  static constant_exprt constant_char(int i, const typet &char_type);

  static irep_idt extract_java_string(const symbol_exprt &s);

  void add_default_axioms(const string_exprt &s);
  exprt axiom_for_is_positive_index(const exprt &x);

  // The following functions add axioms for the returned value
  // to be equal to the result of the function given as argument.
  // They are not accessed directly from other classes: they call
  // `add_axioms_for_function_application` which determines which of
  // these methods should be called.

  exprt add_axioms_for_char_at(const function_application_exprt &f);
  exprt add_axioms_for_code_point_at(const function_application_exprt &f);
  exprt add_axioms_for_code_point_before(const function_application_exprt &f);
  exprt add_axioms_for_contains(const function_application_exprt &f);
  exprt add_axioms_for_equals(const function_application_exprt &f);
  exprt add_axioms_for_equals_ignore_case(const function_application_exprt &f);

  // Add axioms corresponding to the String.hashCode java function
  // The specification is partial: the actual value is not actually computed
  // but we ensure that hash codes of equal strings are equal.
  exprt add_axioms_for_hash_code(const function_application_exprt &f);

  exprt add_axioms_for_is_empty(const function_application_exprt &f);
  exprt add_axioms_for_is_prefix(
    const string_exprt &prefix, const string_exprt &str, const exprt &offset);
  exprt add_axioms_for_is_prefix(
    const function_application_exprt &f, bool swap_arguments=false);
  exprt add_axioms_for_is_suffix(
    const function_application_exprt &f, bool swap_arguments=false);
  exprt add_axioms_for_length(const function_application_exprt &f);
  string_exprt add_axioms_for_empty_string(const function_application_exprt &f);
  string_exprt add_axioms_for_char_set(const function_application_exprt &expr);
  string_exprt add_axioms_for_copy(const function_application_exprt &f);
  string_exprt add_axioms_for_concat(
    const string_exprt &s1, const string_exprt &s2);
  string_exprt add_axioms_for_concat_substr(
    const string_exprt &s1,
    const string_exprt &s2,
    const exprt &start_index,
    const exprt &end_index);
  string_exprt add_axioms_for_concat(const function_application_exprt &f);
  string_exprt add_axioms_for_concat_int(const function_application_exprt &f);
  string_exprt add_axioms_for_concat_long(const function_application_exprt &f);
  string_exprt add_axioms_for_concat_bool(const function_application_exprt &f);
  string_exprt add_axioms_for_concat_char(const function_application_exprt &f);
  string_exprt add_axioms_for_concat_char(
    const string_exprt &string_expr, const exprt &char_expr);
  string_exprt add_axioms_for_concat_double(
    const function_application_exprt &f);
  string_exprt add_axioms_for_concat_float(const function_application_exprt &f);
  string_exprt add_axioms_for_concat_code_point(
    const function_application_exprt &f);
  string_exprt add_axioms_for_constant(
    irep_idt sval, const refined_string_typet &ref_type);
  string_exprt add_axioms_for_delete(
    const string_exprt &str, const exprt &start, const exprt &end);
  string_exprt add_axioms_for_delete(const function_application_exprt &expr);
  string_exprt add_axioms_for_delete_char_at(
    const function_application_exprt &expr);
  string_exprt add_axioms_for_format(const function_application_exprt &f);
  string_exprt add_axioms_for_format(
    const std::string &s,
    const exprt::operandst &args,
    const refined_string_typet &ref_type);
  exprt add_axioms_for_format_specifier_is_correct(
    const function_application_exprt &expr);
  bool add_axioms_for_format_specifier_is_correct(
    const std::string &s);

  string_exprt add_axioms_for_format_specifier(
    const format_specifiert &fs,
    const struct_exprt &arg,
    const refined_string_typet &ref_type);

  string_exprt add_axioms_for_insert(
    const string_exprt &s1, const string_exprt &s2, const exprt &offset);
  string_exprt add_axioms_for_insert(const function_application_exprt &f);
  string_exprt add_axioms_for_insert_int(const function_application_exprt &f);
  string_exprt add_axioms_for_insert_long(const function_application_exprt &f);
  string_exprt add_axioms_for_insert_bool(const function_application_exprt &f);
  string_exprt add_axioms_for_insert_char(const function_application_exprt &f);
  string_exprt add_axioms_for_insert_double(
    const function_application_exprt &f);
  string_exprt add_axioms_for_insert_float(const function_application_exprt &f);
  string_exprt add_axioms_from_literal(const function_application_exprt &f);
  string_exprt add_axioms_from_int(const function_application_exprt &f);
  string_exprt add_axioms_from_int(
    const exprt &input_int,
    const refined_string_typet &ref_type,
    size_t max_size=0);
  string_exprt add_axioms_from_int_with_radix(
    const exprt &input_int,
    const exprt &radix,
    const refined_string_typet &ref_type,
    size_t max_size=0);
  string_exprt add_axioms_from_int_hex(
    const exprt &i, const refined_string_typet &ref_type);
  string_exprt add_axioms_from_int_hex(const function_application_exprt &f);
  string_exprt add_axioms_from_long(const function_application_exprt &f);
  string_exprt add_axioms_from_long(const exprt &i, size_t max_size);
  string_exprt add_axioms_from_bool(const function_application_exprt &f);
  string_exprt add_axioms_from_bool(
    const exprt &i, const refined_string_typet &ref_type);
  string_exprt add_axioms_from_char(const function_application_exprt &f);
  string_exprt add_axioms_from_char(
    const exprt &i, const refined_string_typet &ref_type);
  exprt add_axioms_for_index_of(
    const string_exprt &str,
    const exprt &c,
    const exprt &from_index);

  // Add axioms corresponding to the String.indexOf:(String;I) java function
  exprt add_axioms_for_index_of_string(
    const string_exprt &haystack,
    const string_exprt &needle,
    const exprt &from_index);

  // Add axioms corresponding to the String.indexOf java functions
  exprt add_axioms_for_index_of(const function_application_exprt &f);

  // Add axioms corresponding to the String.lastIndexOf:(String;I) java function
  exprt add_axioms_for_last_index_of_string(
    const string_exprt &haystack,
    const string_exprt &needle,
    const exprt &from_index);

  // Add axioms corresponding to the String.lastIndexOf:(CI) java function
  exprt add_axioms_for_last_index_of(
    const string_exprt &str,
    const exprt &c,
    const exprt &from_index);

  // Add axioms corresponding to the String.lastIndexOf java functions
  exprt add_axioms_for_last_index_of(const function_application_exprt &f);

  // TODO: the specifications of these functions is only partial
  // We currently only specify that the string for NaN is "NaN", for infinity
  // and minus infinity the string are "Infinity" and "-Infinity respectively
  // otherwise the string contains only characters in [0123456789.] and '-' at
  // the start for negative number
  string_exprt add_axioms_for_string_of_float(
    const function_application_exprt &f);
  string_exprt add_axioms_for_string_of_float(
    const exprt &f, const refined_string_typet &ref_type);

  string_exprt add_axioms_for_fractional_part(
    const exprt &i, size_t max_size, const refined_string_typet &ref_type);
  string_exprt add_axioms_from_float_scientific_notation(
    const exprt &f, const refined_string_typet &ref_type);
  string_exprt add_axioms_from_float_scientific_notation(
    const function_application_exprt &f);

  // Add axioms corresponding to the String.valueOf(D) java function
  // TODO: the specifications is only partial
  string_exprt add_axioms_from_double(const function_application_exprt &f);

  string_exprt add_axioms_for_replace(const function_application_exprt &f);
  string_exprt add_axioms_for_set_length(const function_application_exprt &f);

  // TODO: the specification may not be correct for the case where the
  // string is shorter than end. An actual java program should throw an
  // exception in that case
  string_exprt add_axioms_for_substring(
    const string_exprt &str, const exprt &start, const exprt &end);
  string_exprt add_axioms_for_substring(const function_application_exprt &expr);

  string_exprt add_axioms_for_to_lower_case(
    const function_application_exprt &expr);
  string_exprt add_axioms_for_to_upper_case(
    const function_application_exprt &expr);
  string_exprt add_axioms_for_to_upper_case(
    const string_exprt &expr);
  string_exprt add_axioms_for_trim(const function_application_exprt &expr);

  string_exprt add_axioms_for_code_point(
    const exprt &code_point, const refined_string_typet &ref_type);
  exprt add_axioms_for_char_pointer(const function_application_exprt &fun);
  string_exprt add_axioms_for_if(const if_exprt &expr);
  exprt add_axioms_for_char_literal(const function_application_exprt &f);

  // Add axioms corresponding the String.codePointCount java function
  // TODO: this function is underspecified, we do not compute the exact value
  // but over approximate it.
  exprt add_axioms_for_code_point_count(const function_application_exprt &f);

  // Add axioms corresponding the String.offsetByCodePointCount java function
  // TODO: this function is underspecified, it should return the index within
  // this String that is offset from the given first argument by second argument
  // code points and we approximate this by saying the result is
  // between index + offset and index + 2 * offset
  exprt add_axioms_for_offset_by_code_point(
    const function_application_exprt &f);

  void add_axioms_for_characters_in_integer_string(
    const exprt &input_int,
    const typet &type,
    const bool strict_formatting,
    const string_exprt &str,
    const std::size_t max_string_length,
    const exprt &radix,
    const unsigned long radix_ul);
  void add_axioms_for_correct_number_format(
    const exprt &input_int,
    const string_exprt &str,
    const exprt &radix_as_char,
    const unsigned long radix_ul,
    const std::size_t max_size,
    const bool strict_formatting);
  exprt add_axioms_for_parse_int(const function_application_exprt &f);
  exprt add_axioms_for_to_char_array(const function_application_exprt &f);
  exprt add_axioms_for_compare_to(const function_application_exprt &f);

  // Add axioms corresponding to the String.intern java function
  // TODO: this does not work at the moment because of the way we treat
  // string pointers
  symbol_exprt add_axioms_for_intern(const function_application_exprt &f);

  // assert that the number of argument is equal to nb and extract them
  static const function_application_exprt::argumentst &args(
    const function_application_exprt &expr, size_t nb)
  {
    const function_application_exprt::argumentst &args=expr.arguments();
    PRECONDITION(args.size()==nb);
    return args;
  }

  // Helper functions
  static exprt int_of_hex_char(const exprt &chr);
  static exprt is_high_surrogate(const exprt &chr);
  static exprt is_low_surrogate(const exprt &chr);
  static exprt character_equals_ignore_case(
    exprt char1, exprt char2, exprt char_a, exprt char_A, exprt char_Z);
  static bool is_constant_string(const string_exprt &expr);
  static string_exprt empty_string(const refined_string_typet &ref_type);
  unsigned long to_integer_or_default(const exprt &expr, unsigned long def);

  // MEMBERS
public:
  const size_t max_string_length;
  // Used to store information about witnesses for not_contains constraints
  std::map<string_not_contains_constraintt, symbol_exprt> witness;
private:
  // The integer with the longest string is Integer.MIN_VALUE which is -2^31,
  // that is -2147483648 so takes 11 characters to write.
  // The long with the longest string is Long.MIN_VALUE which is -2^63,
  // approximately -9.223372037*10^18 so takes 20 characters to write.
  CBMC_CONSTEXPR static const std::size_t MAX_INTEGER_LENGTH=11;
  CBMC_CONSTEXPR static const std::size_t MAX_LONG_LENGTH=20;
  CBMC_CONSTEXPR static const std::size_t MAX_FLOAT_LENGTH=15;
  CBMC_CONSTEXPR static const std::size_t MAX_DOUBLE_LENGTH=30;
  std::set<string_exprt> m_created_strings;
  unsigned m_symbol_count=0;
  const messaget m_message;
  const bool m_force_printable_characters;

  std::vector<exprt> m_axioms;
  std::map<irep_idt, string_exprt> m_unresolved_symbols;
  std::vector<symbol_exprt> m_boolean_symbols;
  std::vector<symbol_exprt> m_index_symbols;
  std::map<function_application_exprt, exprt> m_function_application_cache;
  const namespacet m_ns;
  // To each string on which hash_code was called we associate a symbol
  // representing the return value of the hash_code function.
  std::map<string_exprt, exprt> m_hash_code_of_string;

  // Pool used for the intern method
  std::map<string_exprt, symbol_exprt> m_intern_of_string;
};

exprt is_digit_with_radix(
  const exprt &chr,
  const bool strict_formatting,
  const exprt &radix_as_char,
  const unsigned long radix_ul);

exprt get_numeric_value_from_character(
  const exprt &chr,
  const typet &char_type,
  const typet &type,
  const bool strict_formatting,
  unsigned long radix_ul);

size_t max_printed_string_length(const typet &type, unsigned long ul_radix);

std::string utf16_constant_array_to_java(
  const array_exprt &arr, unsigned length);

#endif
