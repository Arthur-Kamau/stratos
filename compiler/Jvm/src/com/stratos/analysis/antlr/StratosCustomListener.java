package com.stratos.analysis.antlr;

import com.stratos.util.Print.Log;
import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ErrorNode;
import org.antlr.v4.runtime.tree.TerminalNode;

public class StratosCustomListener implements StratosParserListener {
    @Override
    public void enterSourceFile(StratosParser.SourceFileContext ctx) {
        Log.blue("Enter source file \n" +ctx.toString());
    }

    @Override
    public void exitSourceFile(StratosParser.SourceFileContext ctx) {
        Log.blue("exit source file");
    }

    @Override
    public void enterPackageClause(StratosParser.PackageClauseContext ctx) {
        Log.blue("Enter package file ");
    }

    @Override
    public void exitPackageClause(StratosParser.PackageClauseContext ctx) {
        Log.blue("exit package file ");
    }

    @Override
    public void enterImportDecl(StratosParser.ImportDeclContext ctx) {

    }

    @Override
    public void exitImportDecl(StratosParser.ImportDeclContext ctx) {

    }

    @Override
    public void enterImportSpec(StratosParser.ImportSpecContext ctx) {

    }

    @Override
    public void exitImportSpec(StratosParser.ImportSpecContext ctx) {

    }

    @Override
    public void enterImportPath(StratosParser.ImportPathContext ctx) {

    }

    @Override
    public void exitImportPath(StratosParser.ImportPathContext ctx) {

    }

    @Override
    public void enterDeclaration(StratosParser.DeclarationContext ctx) {

    }

    @Override
    public void exitDeclaration(StratosParser.DeclarationContext ctx) {

    }

    @Override
    public void enterConstDecl(StratosParser.ConstDeclContext ctx) {

    }

    @Override
    public void exitConstDecl(StratosParser.ConstDeclContext ctx) {

    }

    @Override
    public void enterConstSpec(StratosParser.ConstSpecContext ctx) {

    }

    @Override
    public void exitConstSpec(StratosParser.ConstSpecContext ctx) {

    }

    @Override
    public void enterIdentifierList(StratosParser.IdentifierListContext ctx) {

    }

    @Override
    public void exitIdentifierList(StratosParser.IdentifierListContext ctx) {

    }

    @Override
    public void enterExpressionList(StratosParser.ExpressionListContext ctx) {

    }

    @Override
    public void exitExpressionList(StratosParser.ExpressionListContext ctx) {

    }

    @Override
    public void enterTypeDecl(StratosParser.TypeDeclContext ctx) {

    }

    @Override
    public void exitTypeDecl(StratosParser.TypeDeclContext ctx) {

    }

    @Override
    public void enterTypeSpec(StratosParser.TypeSpecContext ctx) {

    }

    @Override
    public void exitTypeSpec(StratosParser.TypeSpecContext ctx) {

    }

    @Override
    public void enterFunctionDecl(StratosParser.FunctionDeclContext ctx) {

    }

    @Override
    public void exitFunctionDecl(StratosParser.FunctionDeclContext ctx) {

    }

    @Override
    public void enterMethodDecl(StratosParser.MethodDeclContext ctx) {

    }

    @Override
    public void exitMethodDecl(StratosParser.MethodDeclContext ctx) {

    }

    @Override
    public void enterReceiver(StratosParser.ReceiverContext ctx) {

    }

    @Override
    public void exitReceiver(StratosParser.ReceiverContext ctx) {

    }

    @Override
    public void enterVarDecl(StratosParser.VarDeclContext ctx) {

    }

    @Override
    public void exitVarDecl(StratosParser.VarDeclContext ctx) {

    }

    @Override
    public void enterVarSpec(StratosParser.VarSpecContext ctx) {

    }

    @Override
    public void exitVarSpec(StratosParser.VarSpecContext ctx) {

    }

    @Override
    public void enterBlock(StratosParser.BlockContext ctx) {

    }

    @Override
    public void exitBlock(StratosParser.BlockContext ctx) {

    }

    @Override
    public void enterStatementList(StratosParser.StatementListContext ctx) {

    }

    @Override
    public void exitStatementList(StratosParser.StatementListContext ctx) {

    }

    @Override
    public void enterStatement(StratosParser.StatementContext ctx) {

    }

    @Override
    public void exitStatement(StratosParser.StatementContext ctx) {

    }

    @Override
    public void enterSimpleStmt(StratosParser.SimpleStmtContext ctx) {

    }

    @Override
    public void exitSimpleStmt(StratosParser.SimpleStmtContext ctx) {

    }

    @Override
    public void enterExpressionStmt(StratosParser.ExpressionStmtContext ctx) {

    }

    @Override
    public void exitExpressionStmt(StratosParser.ExpressionStmtContext ctx) {

    }

    @Override
    public void enterSendStmt(StratosParser.SendStmtContext ctx) {

    }

    @Override
    public void exitSendStmt(StratosParser.SendStmtContext ctx) {

    }

    @Override
    public void enterIncDecStmt(StratosParser.IncDecStmtContext ctx) {

    }

    @Override
    public void exitIncDecStmt(StratosParser.IncDecStmtContext ctx) {

    }

    @Override
    public void enterAssignment(StratosParser.AssignmentContext ctx) {

    }

    @Override
    public void exitAssignment(StratosParser.AssignmentContext ctx) {

    }

    @Override
    public void enterAssign_op(StratosParser.Assign_opContext ctx) {

    }

    @Override
    public void exitAssign_op(StratosParser.Assign_opContext ctx) {

    }

    @Override
    public void enterShortVarDecl(StratosParser.ShortVarDeclContext ctx) {

    }

    @Override
    public void exitShortVarDecl(StratosParser.ShortVarDeclContext ctx) {

    }

    @Override
    public void enterEmptyStmt(StratosParser.EmptyStmtContext ctx) {

    }

    @Override
    public void exitEmptyStmt(StratosParser.EmptyStmtContext ctx) {

    }

    @Override
    public void enterLabeledStmt(StratosParser.LabeledStmtContext ctx) {

    }

    @Override
    public void exitLabeledStmt(StratosParser.LabeledStmtContext ctx) {

    }

    @Override
    public void enterReturnStmt(StratosParser.ReturnStmtContext ctx) {

    }

    @Override
    public void exitReturnStmt(StratosParser.ReturnStmtContext ctx) {

    }

    @Override
    public void enterBreakStmt(StratosParser.BreakStmtContext ctx) {

    }

    @Override
    public void exitBreakStmt(StratosParser.BreakStmtContext ctx) {

    }

    @Override
    public void enterContinueStmt(StratosParser.ContinueStmtContext ctx) {

    }

    @Override
    public void exitContinueStmt(StratosParser.ContinueStmtContext ctx) {

    }

    @Override
    public void enterGotoStmt(StratosParser.GotoStmtContext ctx) {

    }

    @Override
    public void exitGotoStmt(StratosParser.GotoStmtContext ctx) {

    }

    @Override
    public void enterFallthroughStmt(StratosParser.FallthroughStmtContext ctx) {

    }

    @Override
    public void exitFallthroughStmt(StratosParser.FallthroughStmtContext ctx) {

    }

    @Override
    public void enterDeferStmt(StratosParser.DeferStmtContext ctx) {

    }

    @Override
    public void exitDeferStmt(StratosParser.DeferStmtContext ctx) {

    }

    @Override
    public void enterIfStmt(StratosParser.IfStmtContext ctx) {

    }

    @Override
    public void exitIfStmt(StratosParser.IfStmtContext ctx) {

    }

    @Override
    public void enterSwitchStmt(StratosParser.SwitchStmtContext ctx) {

    }

    @Override
    public void exitSwitchStmt(StratosParser.SwitchStmtContext ctx) {

    }

    @Override
    public void enterExprSwitchStmt(StratosParser.ExprSwitchStmtContext ctx) {

    }

    @Override
    public void exitExprSwitchStmt(StratosParser.ExprSwitchStmtContext ctx) {

    }

    @Override
    public void enterExprCaseClause(StratosParser.ExprCaseClauseContext ctx) {

    }

    @Override
    public void exitExprCaseClause(StratosParser.ExprCaseClauseContext ctx) {

    }

    @Override
    public void enterExprSwitchCase(StratosParser.ExprSwitchCaseContext ctx) {

    }

    @Override
    public void exitExprSwitchCase(StratosParser.ExprSwitchCaseContext ctx) {

    }

    @Override
    public void enterTypeSwitchStmt(StratosParser.TypeSwitchStmtContext ctx) {

    }

    @Override
    public void exitTypeSwitchStmt(StratosParser.TypeSwitchStmtContext ctx) {

    }

    @Override
    public void enterTypeSwitchGuard(StratosParser.TypeSwitchGuardContext ctx) {

    }

    @Override
    public void exitTypeSwitchGuard(StratosParser.TypeSwitchGuardContext ctx) {

    }

    @Override
    public void enterTypeCaseClause(StratosParser.TypeCaseClauseContext ctx) {

    }

    @Override
    public void exitTypeCaseClause(StratosParser.TypeCaseClauseContext ctx) {

    }

    @Override
    public void enterTypeSwitchCase(StratosParser.TypeSwitchCaseContext ctx) {

    }

    @Override
    public void exitTypeSwitchCase(StratosParser.TypeSwitchCaseContext ctx) {

    }

    @Override
    public void enterTypeList(StratosParser.TypeListContext ctx) {

    }

    @Override
    public void exitTypeList(StratosParser.TypeListContext ctx) {

    }

    @Override
    public void enterSelectStmt(StratosParser.SelectStmtContext ctx) {

    }

    @Override
    public void exitSelectStmt(StratosParser.SelectStmtContext ctx) {

    }

    @Override
    public void enterCommClause(StratosParser.CommClauseContext ctx) {

    }

    @Override
    public void exitCommClause(StratosParser.CommClauseContext ctx) {

    }

    @Override
    public void enterCommCase(StratosParser.CommCaseContext ctx) {

    }

    @Override
    public void exitCommCase(StratosParser.CommCaseContext ctx) {

    }

    @Override
    public void enterRecvStmt(StratosParser.RecvStmtContext ctx) {

    }

    @Override
    public void exitRecvStmt(StratosParser.RecvStmtContext ctx) {

    }

    @Override
    public void enterForStmt(StratosParser.ForStmtContext ctx) {

    }

    @Override
    public void exitForStmt(StratosParser.ForStmtContext ctx) {

    }

    @Override
    public void enterForClause(StratosParser.ForClauseContext ctx) {

    }

    @Override
    public void exitForClause(StratosParser.ForClauseContext ctx) {

    }

    @Override
    public void enterRangeClause(StratosParser.RangeClauseContext ctx) {

    }

    @Override
    public void exitRangeClause(StratosParser.RangeClauseContext ctx) {

    }

    @Override
    public void enterGoStmt(StratosParser.GoStmtContext ctx) {

    }

    @Override
    public void exitGoStmt(StratosParser.GoStmtContext ctx) {

    }

    @Override
    public void enterType_(StratosParser.Type_Context ctx) {

    }

    @Override
    public void exitType_(StratosParser.Type_Context ctx) {

    }

    @Override
    public void enterTypeName(StratosParser.TypeNameContext ctx) {

    }

    @Override
    public void exitTypeName(StratosParser.TypeNameContext ctx) {

    }

    @Override
    public void enterTypeLit(StratosParser.TypeLitContext ctx) {

    }

    @Override
    public void exitTypeLit(StratosParser.TypeLitContext ctx) {

    }

    @Override
    public void enterArrayType(StratosParser.ArrayTypeContext ctx) {

    }

    @Override
    public void exitArrayType(StratosParser.ArrayTypeContext ctx) {

    }

    @Override
    public void enterArrayLength(StratosParser.ArrayLengthContext ctx) {

    }

    @Override
    public void exitArrayLength(StratosParser.ArrayLengthContext ctx) {

    }

    @Override
    public void enterElementType(StratosParser.ElementTypeContext ctx) {

    }

    @Override
    public void exitElementType(StratosParser.ElementTypeContext ctx) {

    }

    @Override
    public void enterPointerType(StratosParser.PointerTypeContext ctx) {

    }

    @Override
    public void exitPointerType(StratosParser.PointerTypeContext ctx) {

    }

    @Override
    public void enterInterfaceType(StratosParser.InterfaceTypeContext ctx) {

    }

    @Override
    public void exitInterfaceType(StratosParser.InterfaceTypeContext ctx) {

    }

    @Override
    public void enterSliceType(StratosParser.SliceTypeContext ctx) {

    }

    @Override
    public void exitSliceType(StratosParser.SliceTypeContext ctx) {

    }

    @Override
    public void enterMapType(StratosParser.MapTypeContext ctx) {

    }

    @Override
    public void exitMapType(StratosParser.MapTypeContext ctx) {

    }

    @Override
    public void enterChannelType(StratosParser.ChannelTypeContext ctx) {

    }

    @Override
    public void exitChannelType(StratosParser.ChannelTypeContext ctx) {

    }

    @Override
    public void enterMethodSpec(StratosParser.MethodSpecContext ctx) {

    }

    @Override
    public void exitMethodSpec(StratosParser.MethodSpecContext ctx) {

    }

    @Override
    public void enterFunctionType(StratosParser.FunctionTypeContext ctx) {

    }

    @Override
    public void exitFunctionType(StratosParser.FunctionTypeContext ctx) {

    }

    @Override
    public void enterSignature(StratosParser.SignatureContext ctx) {

    }

    @Override
    public void exitSignature(StratosParser.SignatureContext ctx) {

    }

    @Override
    public void enterResult(StratosParser.ResultContext ctx) {

    }

    @Override
    public void exitResult(StratosParser.ResultContext ctx) {

    }

    @Override
    public void enterParameters(StratosParser.ParametersContext ctx) {

    }

    @Override
    public void exitParameters(StratosParser.ParametersContext ctx) {

    }

    @Override
    public void enterParameterDecl(StratosParser.ParameterDeclContext ctx) {

    }

    @Override
    public void exitParameterDecl(StratosParser.ParameterDeclContext ctx) {

    }

    @Override
    public void enterExpression(StratosParser.ExpressionContext ctx) {

    }

    @Override
    public void exitExpression(StratosParser.ExpressionContext ctx) {

    }

    @Override
    public void enterPrimaryExpr(StratosParser.PrimaryExprContext ctx) {

    }

    @Override
    public void exitPrimaryExpr(StratosParser.PrimaryExprContext ctx) {

    }

    @Override
    public void enterUnaryExpr(StratosParser.UnaryExprContext ctx) {

    }

    @Override
    public void exitUnaryExpr(StratosParser.UnaryExprContext ctx) {

    }

    @Override
    public void enterConversion(StratosParser.ConversionContext ctx) {

    }

    @Override
    public void exitConversion(StratosParser.ConversionContext ctx) {

    }

    @Override
    public void enterOperand(StratosParser.OperandContext ctx) {

    }

    @Override
    public void exitOperand(StratosParser.OperandContext ctx) {

    }

    @Override
    public void enterLiteral(StratosParser.LiteralContext ctx) {

    }

    @Override
    public void exitLiteral(StratosParser.LiteralContext ctx) {

    }

    @Override
    public void enterBasicLit(StratosParser.BasicLitContext ctx) {

    }

    @Override
    public void exitBasicLit(StratosParser.BasicLitContext ctx) {

    }

    @Override
    public void enterInteger(StratosParser.IntegerContext ctx) {

    }

    @Override
    public void exitInteger(StratosParser.IntegerContext ctx) {

    }

    @Override
    public void enterOperandName(StratosParser.OperandNameContext ctx) {

    }

    @Override
    public void exitOperandName(StratosParser.OperandNameContext ctx) {

    }

    @Override
    public void enterQualifiedIdent(StratosParser.QualifiedIdentContext ctx) {

    }

    @Override
    public void exitQualifiedIdent(StratosParser.QualifiedIdentContext ctx) {

    }

    @Override
    public void enterCompositeLit(StratosParser.CompositeLitContext ctx) {

    }

    @Override
    public void exitCompositeLit(StratosParser.CompositeLitContext ctx) {

    }

    @Override
    public void enterLiteralType(StratosParser.LiteralTypeContext ctx) {

    }

    @Override
    public void exitLiteralType(StratosParser.LiteralTypeContext ctx) {

    }

    @Override
    public void enterLiteralValue(StratosParser.LiteralValueContext ctx) {

    }

    @Override
    public void exitLiteralValue(StratosParser.LiteralValueContext ctx) {

    }

    @Override
    public void enterElementList(StratosParser.ElementListContext ctx) {

    }

    @Override
    public void exitElementList(StratosParser.ElementListContext ctx) {

    }

    @Override
    public void enterKeyedElement(StratosParser.KeyedElementContext ctx) {

    }

    @Override
    public void exitKeyedElement(StratosParser.KeyedElementContext ctx) {

    }

    @Override
    public void enterKey(StratosParser.KeyContext ctx) {

    }

    @Override
    public void exitKey(StratosParser.KeyContext ctx) {

    }

    @Override
    public void enterElement(StratosParser.ElementContext ctx) {

    }

    @Override
    public void exitElement(StratosParser.ElementContext ctx) {

    }

    @Override
    public void enterStructType(StratosParser.StructTypeContext ctx) {

    }

    @Override
    public void exitStructType(StratosParser.StructTypeContext ctx) {

    }

    @Override
    public void enterFieldDecl(StratosParser.FieldDeclContext ctx) {

    }

    @Override
    public void exitFieldDecl(StratosParser.FieldDeclContext ctx) {

    }

    @Override
    public void enterString_(StratosParser.String_Context ctx) {

    }

    @Override
    public void exitString_(StratosParser.String_Context ctx) {

    }

    @Override
    public void enterAnonymousField(StratosParser.AnonymousFieldContext ctx) {

    }

    @Override
    public void exitAnonymousField(StratosParser.AnonymousFieldContext ctx) {

    }

    @Override
    public void enterFunctionLit(StratosParser.FunctionLitContext ctx) {

    }

    @Override
    public void exitFunctionLit(StratosParser.FunctionLitContext ctx) {

    }

    @Override
    public void enterIndex(StratosParser.IndexContext ctx) {

    }

    @Override
    public void exitIndex(StratosParser.IndexContext ctx) {

    }

    @Override
    public void enterSlice(StratosParser.SliceContext ctx) {

    }

    @Override
    public void exitSlice(StratosParser.SliceContext ctx) {

    }

    @Override
    public void enterTypeAssertion(StratosParser.TypeAssertionContext ctx) {

    }

    @Override
    public void exitTypeAssertion(StratosParser.TypeAssertionContext ctx) {

    }

    @Override
    public void enterArguments(StratosParser.ArgumentsContext ctx) {

    }

    @Override
    public void exitArguments(StratosParser.ArgumentsContext ctx) {

    }

    @Override
    public void enterMethodExpr(StratosParser.MethodExprContext ctx) {

    }

    @Override
    public void exitMethodExpr(StratosParser.MethodExprContext ctx) {

    }

    @Override
    public void enterReceiverType(StratosParser.ReceiverTypeContext ctx) {

    }

    @Override
    public void exitReceiverType(StratosParser.ReceiverTypeContext ctx) {

    }

    @Override
    public void enterEos(StratosParser.EosContext ctx) {

    }

    @Override
    public void exitEos(StratosParser.EosContext ctx) {

    }

    @Override
    public void visitTerminal(TerminalNode terminalNode) {

    }

    @Override
    public void visitErrorNode(ErrorNode errorNode) {

    }

    @Override
    public void enterEveryRule(ParserRuleContext parserRuleContext) {

    }

    @Override
    public void exitEveryRule(ParserRuleContext parserRuleContext) {

    }
}
