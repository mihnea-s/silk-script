#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <silk/syntax/token.h>

namespace silk {

namespace st {

struct Node;

/// Typing information
using Typing = std::nullptr_t;

/// Vector of identifier, typing pairs used by objects for
/// their fields or functions for their parameters.
using TypedFields = std::vector<std::pair<std::string_view, Typing>>;

/// A comment and the tree node attached to it
///
///   (BEFORE? $child) # $text (AFTER? \n$child)
///
struct Comment {
  enum Placement { BEFORE, AFTER } placement;
  std::string_view      text;
  std::unique_ptr<Node> child;
};

/// Declare the module as being the entrypoint
///
///   main;
///
struct ModuleMain {};

/// Declare the package path of the module
///
///   pkg $path;
///
struct ModuleDeclaration {
  std::string_view path;
};

/// Import of symbols from an external package
///
///   use $name (/ $imports)?;
///
struct ModuleImport {
  std::string_view              name;
  std::vector<std::string_view> imports;
};

/// Declare a free function, the child is of the ExpressionLambda
/// type and contains the actual function information
///
///   fun $name $child.parameters $child.child
///
struct DeclarationFunction {
  std::string_view      name;
  std::unique_ptr<Node> child;
};

// TODO: incomplete
// Enums are fp-like variants carrying one type
//
//    enum IPAddress {
//       v4 :: nat,
//       v6 :: bytes[16],
//    }
//
struct DeclarationEnum {
  std::string_view name;
};

// TODO: incomplete
// The idea is to have strongly typed struct semantics with
// the following syntax:
//
//    obj Vector {
//      x :: real;
//      y :: real;
//      z :: real;
//
//      fun length :: real => sqrt(x**2 + y**2 + z**2);
//    }
//
//  Additional ideas: inheritence, vtable
//
struct DeclarationObject {
  std::string_view name;
};

/// Declares functions available at run time thorugh the FFI.
/// Similar to `extern` blocks in other languages.
///
///   dll $name { $children }
///
struct DeclarationDynamicLibrary {
  std::string_view  name;
  std::vector<Node> children;
};

// TODO: incomplete
/// No clue where to start on this one
struct DeclarationMacro {
  std::string_view name;
};

/// Empty statement (lone semicolon)
struct StatementEmpty {};

/// An expression statement is an expression that is calculated,
/// pushed on the stack and then immidiately discarded.
///
///   $child;
///
struct StatementExpression {
  std::unique_ptr<Node> child;
};

/// A block contains multiple statements, delimited by a scope
///
///   { $children }
///
struct StatementBlock {
  std::vector<Node> children;
};

/// A circuit block contains multiple labels each corresponding to
/// a statement, on first execution the nameless (or _) path is
/// executed, the path is changed by a `switch label;` statement.
///
///   ${ $children }
///
struct StatementCircuit {
  std::vector<std::pair<std::string_view, Node>> children;
};

/// Define scoped variable at runtime, def for mutable variables,
/// let for immutable variables.
///
///   $kind $name = $child;
///
struct StatementVariable {
  std::string_view      name;
  std::unique_ptr<Node> child;
  enum Kind {
    LET = (int)TokenKind::KW_LET,
    DEF = (int)TokenKind::KW_DEF,
  } kind;
};

/// Define a compile time constant
///
/// const $name = $child;
///
struct StatementConstant {
  std::string_view      name;
  std::unique_ptr<Node> child;
};

/// A return in a function, returns have optional continuation
/// arguments given in parenthesis, continuation may be a nullptr
/// if no continuation is given.
///
///   return[$continuation] $child;
///
struct StatementReturn {
  std::unique_ptr<Node> continuation;
  std::unique_ptr<Node> child;
};

/// Switch to a different label in a circuit block
///
///   switch $label;
///
struct StatementSwitch {
  std::string_view label;
};

/// Control the flow of a (for, while, infinite) loop.
///
///   $kind;
///
struct StatementIterationControl {
  enum Kind {
    CONT  = (int)TokenKind::KW_CONTINUE,
    BREAK = (int)TokenKind::KW_BREAK,
  } kind;
};

/// An if statement. The alternative may be a nullptr.
///
///   if ($condition) $consequence (else $alternative)?
///
struct StatementIf {
  std::unique_ptr<Node> condition;
  std::unique_ptr<Node> consequence;
  std::unique_ptr<Node> alternative;
};

/// A while loop.
///
///   while ($condition) $child
///
struct StatementWhile {
  std::unique_ptr<Node> condition;
  std::unique_ptr<Node> child;
};

/// An infinite loop.
///
///   loop $child
///
struct StatementLoop {
  std::unique_ptr<Node> child;
};

/// A C style for loop.
///
///   for ($initial; $condition; $increment) $child
///
struct StatementFor {
  std::unique_ptr<Node> initial;
  std::unique_ptr<Node> condition;
  std::unique_ptr<Node> increment;
  std::unique_ptr<Node> child;
};

/// A foreach loop.
///
///   foreach ($iterator_kind $iterator <- $collection) $child
///
struct StatementForeach {
  enum IterKind {
    LET = (int)TokenKind::KW_LET,
    DEF = (int)TokenKind::KW_DEF,
  } iterator_kind;
  std::string_view      iterator;
  std::unique_ptr<Node> collection;
  std::unique_ptr<Node> child;
};

// TODO: incomplete
/// The idea is to have an fp-like match statement for
/// ranges, tuples etc. Initial syntax:
///
///   match (n % 3, n % 5) {
///     (0, 0) -> "fizzbuzz",
///     (0, _) -> "fizz",
///     (_, 0) -> "buzz",
///     _      -> n,
///   }
///
struct StatementMatch {};

struct ExpressionIdentifier {
  std::string_view name;
};

struct ExpressionVoid {};

struct ExpressionContinuation {};

struct ExpressionBool {
  bool value;
};

struct ExpressionNat {
  std::uint64_t value;
};

struct ExpressionInt {
  std::int64_t value;
};

struct ExpressionReal {
  double value;
};

struct ExpressionRealKeyword {
  enum Kind {
    PI    = (int)TokenKind::KEY_PI,
    TAU   = (int)TokenKind::KEY_TAU,
    EULER = (int)TokenKind::KEY_EUL
  } kind;
};

///
struct ExpressionChar {
  char32_t value;
};

///
struct ExpressionString {
  std::string_view raw_value;
  std::string      value;
};

///
struct ExpressionTuple {
  std::vector<Node> children;
};

///
struct ExpressionUnaryOp {
  std::unique_ptr<Node> child;
  enum Kind {
    NOT = (int)TokenKind::KW_NOT,
    NEG = (int)TokenKind::SYM_MINUS,
  } kind;
};

///
struct ExpressionBinaryOp {
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  enum Kind {
    // Booleans
    OR  = (int)TokenKind::KW_OR,
    AND = (int)TokenKind::KW_AND,

    // Comparison
    EQUAL     = (int)TokenKind::SYM_EQUAL_EQUAL,
    NOTEQ     = (int)TokenKind::SYM_BANG_EQUAL,
    LESS      = (int)TokenKind::SYM_LT,
    LESSEQ    = (int)TokenKind::SYM_LT_EQUAL,
    GREATER   = (int)TokenKind::SYM_GT,
    GREATEREQ = (int)TokenKind::SYM_GT_EQUAL,

    // Numberic
    ADD  = (int)TokenKind::SYM_PLUS,
    SUB  = (int)TokenKind::SYM_MINUS,
    DIV  = (int)TokenKind::SYM_SLASH,
    RDIV = (int)TokenKind::SYM_SLASH_SLASH,
    MUL  = (int)TokenKind::SYM_STAR,
    POW  = (int)TokenKind::SYM_STAR_STAR,
    MOD  = (int)TokenKind::SYM_PERC,

    // Collections
    INDEX  = (int)TokenKind::SYM_DOT,
    APPEND = (int)TokenKind::SYM_TILDE,
    MERGE  = (int)TokenKind::SYM_AMP,
  } kind;
};

///
struct ExpressionRange {
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
};

///
struct ExpressionVector {
  std::vector<Node> children;
};

///
struct ExpressionArray {
  std::vector<Node> children;
};

///
struct ExpressionDictionary {
  std::vector<std::pair<Node, Node>> children;
};

///
struct ExpressionAssignment {
  std::unique_ptr<Node> assignee;
  std::unique_ptr<Node> child;
  enum Kind {
    ASSIGN = (int)TokenKind::SYM_EQUAL,
    ADD    = (int)TokenKind::SYM_PLUS_EQUAL,
    SUB    = (int)TokenKind::SYM_MINUS_EQUAL,
    DIV    = (int)TokenKind::SYM_SLASH_EQUAL,
    RDIV   = (int)TokenKind::SYM_SLASH_SLASH_EQUAL,
    MUL    = (int)TokenKind::SYM_STAR_EQUAL,
    POW    = (int)TokenKind::SYM_STAR_STAR_EQUAL,
  } kind;
};

///
struct ExpressionCall {
  std::unique_ptr<Node> callee;
  std::vector<Node>     children;
};

///
struct ExpressionLambda {
  TypedFields           parameters;
  std::unique_ptr<Node> child;
};

/// Node in the syntax tree. Contains positional information
/// and construct specific data.
struct Node {
  Location location;
  std::variant<
    Comment,
    ModuleMain,
    ModuleDeclaration,
    ModuleImport,
    DeclarationFunction,
    DeclarationEnum,
    DeclarationObject,
    DeclarationDynamicLibrary,
    DeclarationMacro,
    StatementEmpty,
    StatementExpression,
    StatementBlock,
    StatementCircuit,
    StatementVariable,
    StatementConstant,
    StatementReturn,
    StatementSwitch,
    StatementIterationControl,
    StatementIf,
    StatementWhile,
    StatementLoop,
    StatementFor,
    StatementForeach,
    StatementMatch,
    ExpressionIdentifier,
    ExpressionVoid,
    ExpressionContinuation,
    ExpressionBool,
    ExpressionNat,
    ExpressionInt,
    ExpressionReal,
    ExpressionRealKeyword,
    ExpressionChar,
    ExpressionString,
    ExpressionTuple,
    ExpressionUnaryOp,
    ExpressionBinaryOp,
    ExpressionRange,
    ExpressionVector,
    ExpressionArray,
    ExpressionDictionary,
    ExpressionAssignment,
    ExpressionCall,
    ExpressionLambda>
    data;
};

} // namespace st

} // namespace silk