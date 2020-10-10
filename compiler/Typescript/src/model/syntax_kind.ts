export enum SyntaxKindType {
	VariableDeclaration = 1,
	TypeAliasDeclaration = 2,
	ImportDeclaration = 3,
	ClassDeclaration = 4,
	ImportClause = 5,
	Parameter = 6,
	PropertyDeclaration = 7,
	BinaryExpression = 8,
	EnumDeclaration = 9,
	StructDeclaration = 10,
	FunctionDeclaration = 11,
	PackageDeclaration = 12,
	UnknownDeclaration = 13
}

export class ParameterTypes<T>{
	name : string ;
	type : T ;
	constructor(name : string, type : T){
		this.name=name;
		this.type = type;
	}

}