package com.araizen.com.model;

public enum NodeType {
    // keywords
    ValNode,
    VarNode,
    LetNode,
    ListNode,
    ClassNode,
    FunctionNode,
    StructNode,
    EnumNode,
    PackageNode,
    ImportNode,
    PrivateNode,
    ReturnNode,
    SomeNode,
    NoneNode,
NotNodeType,
    AndNode,
    AliasNode,
    ThenNode,
    OrNode,
    // types
    IntegerNode,
    DoubleNode,
    StringNode,
    StringValue,
    CharacterNode,
    StringVerbatimNode,
    IfNode,
    ElseNode,
    TryNode,
    CatchNode,
    WhenNode,
    ImplimentsNode,
    ExtendsNode,
    // operators
    DivideNode,
    MultiplyNode,
    ModulasNode,
    AddNode,
    AssignNode,
    SubtractNode,
    NotEqualToNode,
    EquateNode,
    //    symbols
    SquareBracketOpenNode,
    SquareBracketCloseNode,
    CurvedBracketOpenNode,
    CurvedBracketCloseNode,
    CurlyBracketOpenNode,
    CurlyBracketCloseNode,
    LineCommentNode,
    SemiColonNode,
    NewLineNode,
    SpaceCharacterNode,
NumericNode,
NegativeNumericNode,
AlphaNumericNode,
    EndOfFileNode,
    UnknownNode,

}