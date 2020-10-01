enum NodeType {
	IntTypeNode,
	DoubleTypeNode,
	StringTypeNode,
	CharTypeNode,
	WhenNode,
	IfNode,
	ElseNode,
	PackageNode,
	ImportNode,
	EnumNode,
	StructNode,
	ClassNode,
	FunctionNode,
	PrivateNode,
	ValNode, VarNode, LetNode,
	CharArrayNode,
	UnknownKeyWordNode,
	UnknownSignNodeNode,

	// math symobols
	ModulusNode,
	AddNode,
	MinusNode,
	DivideNode,
	SubtractNode,
	MultiplyNode,

	// space Node
	SpaceNode,

	// unif signs
	SquareBracketOpenNode,
	SquareBracketCloseNode,
	CurvedBracketOpenNode,
	CurvedBracketCloseNode,
	CurlyBracketOpenNode,
	CurlyBracketCloseNode,

	SemiColonNode,
	ColonNode,

	// comment
	LineComment,
	MultiLineComment,

	// stirn 
	QuotationNode,

	//new 
	NewLine

}