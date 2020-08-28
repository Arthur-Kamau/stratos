// This is a generated file. Not intended for manual editing.
package lang.stratos.grammer.parser;

import com.intellij.lang.PsiBuilder;
import com.intellij.lang.PsiBuilder.Marker;
import static lang.stratos.grammer.types.StratosTypes.*;
import static com.intellij.lang.parser.GeneratedParserUtilBase.*;
import com.intellij.psi.tree.IElementType;
import com.intellij.lang.ASTNode;
import com.intellij.psi.tree.TokenSet;
import com.intellij.lang.PsiParser;
import com.intellij.lang.LightPsiParser;

@SuppressWarnings({"SimplifiableIfStatement", "UnusedAssignment"})
public class StratosParser implements PsiParser, LightPsiParser {

  public ASTNode parse(IElementType t, PsiBuilder b) {
    parseLight(t, b);
    return b.getTreeBuilt();
  }

  public void parseLight(IElementType t, PsiBuilder b) {
    boolean r;
    b = adapt_builder_(t, b, this, null);
    Marker m = enter_section_(b, 0, _COLLAPSE_, null);
    r = parse_root_(t, b);
    exit_section_(b, 0, m, t, r, true, TRUE_CONDITION);
  }

  protected boolean parse_root_(IElementType t, PsiBuilder b) {
    return parse_root_(t, b, 0);
  }

  static boolean parse_root_(IElementType t, PsiBuilder b, int l) {
    return StratosFile(b, l + 1);
  }

  /* ********************************************************** */
  // IDENTIFIER '=' FunctionInvocation
  public static boolean AssignmentStatement(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "AssignmentStatement")) return false;
    if (!nextTokenIs(b, IDENTIFIER)) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeTokens(b, 0, IDENTIFIER, EQ);
    r = r && FunctionInvocation(b, l + 1);
    exit_section_(b, m, ASSIGNMENT_STATEMENT, r);
    return r;
  }

  /* ********************************************************** */
  // class IDENTIFIER  '{' FunctionBody | Statement*  '}'
  public static boolean ClassDefinition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "ClassDefinition")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, CLASS_DEFINITION, "<class definition>");
    r = ClassDefinition_0(b, l + 1);
    if (!r) r = ClassDefinition_1(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  // class IDENTIFIER  '{' FunctionBody
  private static boolean ClassDefinition_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "ClassDefinition_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeTokens(b, 0, CLASS, IDENTIFIER, LBRACE);
    r = r && FunctionBody(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // Statement*  '}'
  private static boolean ClassDefinition_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "ClassDefinition_1")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = ClassDefinition_1_0(b, l + 1);
    r = r && consumeToken(b, RBRACE);
    exit_section_(b, m, null, r);
    return r;
  }

  // Statement*
  private static boolean ClassDefinition_1_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "ClassDefinition_1_0")) return false;
    while (true) {
      int c = current_position_(b);
      if (!Statement(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "ClassDefinition_1_0", c)) break;
    }
    return true;
  }

  /* ********************************************************** */
  // ClassDefinition | NameSpaceDefinition  | FunctionDefinition
  public static boolean Definition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Definition")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, DEFINITION, "<definition>");
    r = ClassDefinition(b, l + 1);
    if (!r) r = NameSpaceDefinition(b, l + 1);
    if (!r) r = FunctionDefinition(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  /* ********************************************************** */
  // Statement*
  public static boolean FunctionBody(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "FunctionBody")) return false;
    Marker m = enter_section_(b, l, _NONE_, FUNCTION_BODY, "<function body>");
    while (true) {
      int c = current_position_(b);
      if (!Statement(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "FunctionBody", c)) break;
    }
    exit_section_(b, l, m, true, false, null);
    return true;
  }

  /* ********************************************************** */
  // function IDENTIFIER '(' ')' (returns Type)? '{' FunctionBody '}'
  public static boolean FunctionDefinition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "FunctionDefinition")) return false;
    if (!nextTokenIs(b, FUNCTION)) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeTokens(b, 0, FUNCTION, IDENTIFIER, LPAREN, RPAREN);
    r = r && FunctionDefinition_4(b, l + 1);
    r = r && consumeToken(b, LBRACE);
    r = r && FunctionBody(b, l + 1);
    r = r && consumeToken(b, RBRACE);
    exit_section_(b, m, FUNCTION_DEFINITION, r);
    return r;
  }

  // (returns Type)?
  private static boolean FunctionDefinition_4(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "FunctionDefinition_4")) return false;
    FunctionDefinition_4_0(b, l + 1);
    return true;
  }

  // returns Type
  private static boolean FunctionDefinition_4_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "FunctionDefinition_4_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, RETURNS);
    r = r && Type(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  /* ********************************************************** */
  // IDENTIFIER '(' ')' ';'
  public static boolean FunctionInvocation(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "FunctionInvocation")) return false;
    if (!nextTokenIs(b, IDENTIFIER)) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeTokens(b, 0, IDENTIFIER, LPAREN, RPAREN, SEMICOLON);
    exit_section_(b, m, FUNCTION_INVOCATION, r);
    return r;
  }

  /* ********************************************************** */
  // namespace IDENTIFIER  '{' FunctionBody | Statement*  '}'
  public static boolean NameSpaceDefinition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "NameSpaceDefinition")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, NAME_SPACE_DEFINITION, "<name space definition>");
    r = NameSpaceDefinition_0(b, l + 1);
    if (!r) r = NameSpaceDefinition_1(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  // namespace IDENTIFIER  '{' FunctionBody
  private static boolean NameSpaceDefinition_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "NameSpaceDefinition_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeTokens(b, 0, NAMESPACE, IDENTIFIER, LBRACE);
    r = r && FunctionBody(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // Statement*  '}'
  private static boolean NameSpaceDefinition_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "NameSpaceDefinition_1")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = NameSpaceDefinition_1_0(b, l + 1);
    r = r && consumeToken(b, RBRACE);
    exit_section_(b, m, null, r);
    return r;
  }

  // Statement*
  private static boolean NameSpaceDefinition_1_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "NameSpaceDefinition_1_0")) return false;
    while (true) {
      int c = current_position_(b);
      if (!Statement(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "NameSpaceDefinition_1_0", c)) break;
    }
    return true;
  }

  /* ********************************************************** */
  // VariableDefinition | AssignmentStatement | FunctionInvocation
  public static boolean Statement(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Statement")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, STATEMENT, "<statement>");
    r = VariableDefinition(b, l + 1);
    if (!r) r = AssignmentStatement(b, l + 1);
    if (!r) r = FunctionInvocation(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  /* ********************************************************** */
  // Definition*
  public static boolean StratosFile(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "StratosFile")) return false;
    while (true) {
      int c = current_position_(b);
      if (!Definition(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "StratosFile", c)) break;
    }
    return true;
  }

  /* ********************************************************** */
  // int | string | number | double
  public static boolean Type(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Type")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, TYPE, "<type>");
    r = consumeToken(b, INT);
    if (!r) r = consumeToken(b, STRING);
    if (!r) r = consumeToken(b, NUMBER);
    if (!r) r = consumeToken(b, DOUBLE);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  /* ********************************************************** */
  // variableDeclarationWithTypes | variableDeclarationWithoutTypes
  public static boolean VariableDefinition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "VariableDefinition")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, VARIABLE_DEFINITION, "<variable definition>");
    r = variableDeclarationWithTypes(b, l + 1);
    if (!r) r = variableDeclarationWithoutTypes(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  /* ********************************************************** */
  // var | val | let
  public static boolean variableDeclaration(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "variableDeclaration")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, VARIABLE_DECLARATION, "<variable declaration>");
    r = consumeToken(b, VAR);
    if (!r) r = consumeToken(b, VAL);
    if (!r) r = consumeToken(b, LET);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  /* ********************************************************** */
  // variableDeclaration IDENTIFIER ':' Type (';' | ('=' FunctionInvocation)?)
  public static boolean variableDeclarationWithTypes(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "variableDeclarationWithTypes")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, VARIABLE_DECLARATION_WITH_TYPES, "<variable declaration with types>");
    r = variableDeclaration(b, l + 1);
    r = r && consumeTokens(b, 0, IDENTIFIER, COLON);
    r = r && Type(b, l + 1);
    r = r && variableDeclarationWithTypes_4(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  // ';' | ('=' FunctionInvocation)?
  private static boolean variableDeclarationWithTypes_4(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "variableDeclarationWithTypes_4")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, SEMICOLON);
    if (!r) r = variableDeclarationWithTypes_4_1(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // ('=' FunctionInvocation)?
  private static boolean variableDeclarationWithTypes_4_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "variableDeclarationWithTypes_4_1")) return false;
    variableDeclarationWithTypes_4_1_0(b, l + 1);
    return true;
  }

  // '=' FunctionInvocation
  private static boolean variableDeclarationWithTypes_4_1_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "variableDeclarationWithTypes_4_1_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, EQ);
    r = r && FunctionInvocation(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  /* ********************************************************** */
  // variableDeclaration IDENTIFIER '=' (ArrayOrSliceType ) ';'
  public static boolean variableDeclarationWithoutTypes(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "variableDeclarationWithoutTypes")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, VARIABLE_DECLARATION_WITHOUT_TYPES, "<variable declaration without types>");
    r = variableDeclaration(b, l + 1);
    r = r && consumeTokens(b, 0, IDENTIFIER, EQ);
    r = r && consumeToken(b, ARRAYORSLICETYPE);
    r = r && consumeToken(b, SEMICOLON);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

}
