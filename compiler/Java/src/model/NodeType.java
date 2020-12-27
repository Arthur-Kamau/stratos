package model;

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

    // types
    IntegerNode,
    DoubleNode,
    StringNode,
    StringValue,
    CharacterNode,
    IfNode,
    ElseNode,
    WhenNode,
    ImplementsNode,
    ExtendsNode,

    // operation node

    AndAndNode,
    OrNode,
    AndOrNode,


    // binary node

    BinaryAndNode ,
    BinaryOrNode ,




    // operators
    DivideNode,
    MultiplyNode,
    ModulasNode,
    AddNode,
    AssignNode,
    SubtractNode,
    NotEqualToNode,
    EquateNode,
    LessThan,
    LessThanOrEqualTo,
    GreaterThan,
    GreaterThanOrEqualTo,


    //    symbols
    SquareBracketOpenNode,
    SquareBracketCloseNode,
    CurvedBracketOpenNode,
    CurvedBracketCloseNode,
    CurlyBracketOpenNode,
    CurlyBracketCloseNode,
    LineCommentNode,
    MultiLineCommentNode,
    SemiColonNode,
    ColonNode,
    NewLineNode,
    SpaceCharacterNode,
    NumericNode,
    NegativeNumericNode,
    AlphaNumericNode,
    EndOfFileNode,
    UnknownNode,




}
