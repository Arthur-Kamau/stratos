
export enum NodeType {
	IntTypeNode = 1,
	DoubleTypeNode = 2,
	StringTypeNode = 3,
	CharTypeNode = 4,
	WhenNode = 5,
	IfNode = 6,
	ElseNode = 7,
	PackageNode = 8,
	ImportNode = 9,
	EnumNode = 10,
	StructNode = 11,
	ClassNode = 12,
	FunctionNode = 13,
	PrivateNode = 14,
	ValNode = 15,
	VarNode = 16,
	LetNode = 17,
	CharArrayNode = 18,
	UnknownKeyWordNode = 19,
	UnknownSignNodeNode = 20,

	// math symobols
	ModulusNode = 21,
	AddNode = 22,
	MinusNode = 23,
	DivideNode = 24,
	SubtractNode = 25,
	MultiplyNode = 26,

	// space Node
	SpaceNode = 27,

	// unif signs
	SquareBracketOpenNode = 28,
	SquareBracketCloseNode = 29,
	CurvedBracketOpenNode = 30,
	CurvedBracketCloseNode = 31,
	CurlyBracketOpenNode = 32,
	CurlyBracketCloseNode = 33,

	SemiColonNode = 34,
	ColonNode = 35,

	// comment
	LineComment = 36,
	MultiLineComment = 37,

	// stirn 
	QuotationNode = 38,

	//new 
	NewLine = 39

}