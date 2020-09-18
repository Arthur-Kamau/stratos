// This is a generated file. Not intended for manual editing.
package lang.stratos.parser;

import com.intellij.lang.PsiBuilder;
import com.intellij.lang.PsiBuilder.Marker;
import static lang.stratos.psi.StratosTypes.*;
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
    r = consumeToken(b, IDENTIFIER);
    r = r && consumeToken(b, "=");
    r = r && FunctionInvocation(b, l + 1);
    exit_section_(b, m, ASSIGNMENT_STATEMENT, r);
    return r;
  }

  /* ********************************************************** */
  // Type IDENTIFIER (';' | ('='  IDENTIFIER* )?)
  public static boolean BasicVariableDefinition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "BasicVariableDefinition")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, BASIC_VARIABLE_DEFINITION, "<basic variable definition>");
    r = Type(b, l + 1);
    r = r && consumeToken(b, IDENTIFIER);
    r = r && BasicVariableDefinition_2(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  // ';' | ('='  IDENTIFIER* )?
  private static boolean BasicVariableDefinition_2(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "BasicVariableDefinition_2")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, ";");
    if (!r) r = BasicVariableDefinition_2_1(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // ('='  IDENTIFIER* )?
  private static boolean BasicVariableDefinition_2_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "BasicVariableDefinition_2_1")) return false;
    BasicVariableDefinition_2_1_0(b, l + 1);
    return true;
  }

  // '='  IDENTIFIER*
  private static boolean BasicVariableDefinition_2_1_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "BasicVariableDefinition_2_1_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, "=");
    r = r && BasicVariableDefinition_2_1_0_1(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // IDENTIFIER*
  private static boolean BasicVariableDefinition_2_1_0_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "BasicVariableDefinition_2_1_0_1")) return false;
    while (true) {
      int c = current_position_(b);
      if (!consumeToken(b, IDENTIFIER)) break;
      if (!empty_element_parsed_guard_(b, "BasicVariableDefinition_2_1_0_1", c)) break;
    }
    return true;
  }

  /* ********************************************************** */
  // PackageDefinition | (EnumType)+ | (StructType)+ | (FunctionDefinition)+
  public static boolean Definition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Definition")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, DEFINITION, "<definition>");
    r = PackageDefinition(b, l + 1);
    if (!r) r = Definition_1(b, l + 1);
    if (!r) r = Definition_2(b, l + 1);
    if (!r) r = Definition_3(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  // (EnumType)+
  private static boolean Definition_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Definition_1")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = Definition_1_0(b, l + 1);
    while (r) {
      int c = current_position_(b);
      if (!Definition_1_0(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "Definition_1", c)) break;
    }
    exit_section_(b, m, null, r);
    return r;
  }

  // (EnumType)
  private static boolean Definition_1_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Definition_1_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = EnumType(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // (StructType)+
  private static boolean Definition_2(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Definition_2")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = Definition_2_0(b, l + 1);
    while (r) {
      int c = current_position_(b);
      if (!Definition_2_0(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "Definition_2", c)) break;
    }
    exit_section_(b, m, null, r);
    return r;
  }

  // (StructType)
  private static boolean Definition_2_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Definition_2_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = StructType(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // (FunctionDefinition)+
  private static boolean Definition_3(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Definition_3")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = Definition_3_0(b, l + 1);
    while (r) {
      int c = current_position_(b);
      if (!Definition_3_0(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "Definition_3", c)) break;
    }
    exit_section_(b, m, null, r);
    return r;
  }

  // (FunctionDefinition)
  private static boolean Definition_3_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Definition_3_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = FunctionDefinition(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  /* ********************************************************** */
  // TypeWrapper IDENTIFIER '{'  (IDENTIFIER ','? )* '}'
  public static boolean EnumType(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "EnumType")) return false;
    if (!nextTokenIs(b, "<enum type>", ENUM, STRUCT)) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, ENUM_TYPE, "<enum type>");
    r = TypeWrapper(b, l + 1);
    r = r && consumeToken(b, IDENTIFIER);
    r = r && consumeToken(b, "{");
    r = r && EnumType_3(b, l + 1);
    r = r && consumeToken(b, "}");
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  // (IDENTIFIER ','? )*
  private static boolean EnumType_3(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "EnumType_3")) return false;
    while (true) {
      int c = current_position_(b);
      if (!EnumType_3_0(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "EnumType_3", c)) break;
    }
    return true;
  }

  // IDENTIFIER ','?
  private static boolean EnumType_3_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "EnumType_3_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, IDENTIFIER);
    r = r && EnumType_3_0_1(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // ','?
  private static boolean EnumType_3_0_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "EnumType_3_0_1")) return false;
    consumeToken(b, ",");
    return true;
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
    r = consumeTokens(b, 0, FUNCTION, IDENTIFIER);
    r = r && consumeToken(b, "(");
    r = r && consumeToken(b, ")");
    r = r && FunctionDefinition_4(b, l + 1);
    r = r && consumeToken(b, "{");
    r = r && FunctionBody(b, l + 1);
    r = r && consumeToken(b, "}");
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
  // IDENTIFIER '(' ')' ';'?
  public static boolean FunctionInvocation(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "FunctionInvocation")) return false;
    if (!nextTokenIs(b, IDENTIFIER)) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, IDENTIFIER);
    r = r && consumeToken(b, "(");
    r = r && consumeToken(b, ")");
    r = r && FunctionInvocation_3(b, l + 1);
    exit_section_(b, m, FUNCTION_INVOCATION, r);
    return r;
  }

  // ';'?
  private static boolean FunctionInvocation_3(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "FunctionInvocation_3")) return false;
    consumeToken(b, ";");
    return true;
  }

  /* ********************************************************** */
  // TypePackage IDENTIFIER* ';'
  public static boolean PackageDefinition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "PackageDefinition")) return false;
    if (!nextTokenIs(b, PACKAGE)) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = TypePackage(b, l + 1);
    r = r && PackageDefinition_1(b, l + 1);
    r = r && consumeToken(b, ";");
    exit_section_(b, m, PACKAGE_DEFINITION, r);
    return r;
  }

  // IDENTIFIER*
  private static boolean PackageDefinition_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "PackageDefinition_1")) return false;
    while (true) {
      int c = current_position_(b);
      if (!consumeToken(b, IDENTIFIER)) break;
      if (!empty_element_parsed_guard_(b, "PackageDefinition_1", c)) break;
    }
    return true;
  }

  /* ********************************************************** */
  // VariableDefinition | AssignmentStatement | FunctionInvocation | EnumType | StructType
  public static boolean Statement(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "Statement")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, STATEMENT, "<statement>");
    r = VariableDefinition(b, l + 1);
    if (!r) r = AssignmentStatement(b, l + 1);
    if (!r) r = FunctionInvocation(b, l + 1);
    if (!r) r = EnumType(b, l + 1);
    if (!r) r = StructType(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  /* ********************************************************** */
  // Definition*
  static boolean StratosFile(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "StratosFile")) return false;
    while (true) {
      int c = current_position_(b);
      if (!Definition(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "StratosFile", c)) break;
    }
    return true;
  }

  /* ********************************************************** */
  // TypeWrapper IDENTIFIER '{' BasicVariableDefinition* '}'
  public static boolean StructType(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "StructType")) return false;
    if (!nextTokenIs(b, "<struct type>", ENUM, STRUCT)) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, STRUCT_TYPE, "<struct type>");
    r = TypeWrapper(b, l + 1);
    r = r && consumeToken(b, IDENTIFIER);
    r = r && consumeToken(b, "{");
    r = r && StructType_3(b, l + 1);
    r = r && consumeToken(b, "}");
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  // BasicVariableDefinition*
  private static boolean StructType_3(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "StructType_3")) return false;
    while (true) {
      int c = current_position_(b);
      if (!BasicVariableDefinition(b, l + 1)) break;
      if (!empty_element_parsed_guard_(b, "StructType_3", c)) break;
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
  // package
  public static boolean TypePackage(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "TypePackage")) return false;
    if (!nextTokenIs(b, PACKAGE)) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, PACKAGE);
    exit_section_(b, m, TYPE_PACKAGE, r);
    return r;
  }

  /* ********************************************************** */
  // enum | struct
  public static boolean TypeWrapper(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "TypeWrapper")) return false;
    if (!nextTokenIs(b, "<type wrapper>", ENUM, STRUCT)) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, TYPE_WRAPPER, "<type wrapper>");
    r = consumeToken(b, ENUM);
    if (!r) r = consumeToken(b, STRUCT);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  /* ********************************************************** */
  // Type IDENTIFIER (';' | ('=' (FunctionInvocation | IDENTIFIER* ))?)
  public static boolean VariableDefinition(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "VariableDefinition")) return false;
    boolean r;
    Marker m = enter_section_(b, l, _NONE_, VARIABLE_DEFINITION, "<variable definition>");
    r = Type(b, l + 1);
    r = r && consumeToken(b, IDENTIFIER);
    r = r && VariableDefinition_2(b, l + 1);
    exit_section_(b, l, m, r, false, null);
    return r;
  }

  // ';' | ('=' (FunctionInvocation | IDENTIFIER* ))?
  private static boolean VariableDefinition_2(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "VariableDefinition_2")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, ";");
    if (!r) r = VariableDefinition_2_1(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // ('=' (FunctionInvocation | IDENTIFIER* ))?
  private static boolean VariableDefinition_2_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "VariableDefinition_2_1")) return false;
    VariableDefinition_2_1_0(b, l + 1);
    return true;
  }

  // '=' (FunctionInvocation | IDENTIFIER* )
  private static boolean VariableDefinition_2_1_0(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "VariableDefinition_2_1_0")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = consumeToken(b, "=");
    r = r && VariableDefinition_2_1_0_1(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // FunctionInvocation | IDENTIFIER*
  private static boolean VariableDefinition_2_1_0_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "VariableDefinition_2_1_0_1")) return false;
    boolean r;
    Marker m = enter_section_(b);
    r = FunctionInvocation(b, l + 1);
    if (!r) r = VariableDefinition_2_1_0_1_1(b, l + 1);
    exit_section_(b, m, null, r);
    return r;
  }

  // IDENTIFIER*
  private static boolean VariableDefinition_2_1_0_1_1(PsiBuilder b, int l) {
    if (!recursion_guard_(b, l, "VariableDefinition_2_1_0_1_1")) return false;
    while (true) {
      int c = current_position_(b);
      if (!consumeToken(b, IDENTIFIER)) break;
      if (!empty_element_parsed_guard_(b, "VariableDefinition_2_1_0_1_1", c)) break;
    }
    return true;
  }

}
