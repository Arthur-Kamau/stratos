import { LanguageNode } from './language_node';
import { SyntaxKindType } from "./syntax_kind";
import { ParameterTypes } from "./syntax_kind"


export class LanguageTokenSyntaxGroup {

	children: LanguageTokenSyntaxGroup[];
	tokenGroup: LanguageTokenSyntax;
	constructor(children: LanguageTokenSyntaxGroup[], tokenGroup: LanguageTokenSyntax) {
		this.children = children;
		this.tokenGroup = tokenGroup;
	}
}
export class LanguageTokenSyntax {
	type: SyntaxKindType;
	nodes: LanguageNode[];
	constructor(type: SyntaxKindType, nodes: LanguageNode[]) {
		this.type = type;
		this.nodes = nodes;
	}

}

export class LanguageTokenSyntaxClass extends LanguageTokenSyntax {
	name: string;
	memberFunctions: LanguageTokenSyntaxFunctionDeclaration[];
	constructos: LanguageTokenSyntaxFunctionDeclaration[];
	nodes: LanguageNode[];
	constructor(name: string, memberFunctions: LanguageTokenSyntaxFunctionDeclaration[], constructos: LanguageTokenSyntaxFunctionDeclaration[], 	nodes: LanguageNode[]) {
		super(SyntaxKindType.ClassDeclaration,nodes );
		this.name = name;
		this.nodes = nodes;
		this.memberFunctions = memberFunctions;
		this.constructos = constructos;
	}

}

export class LanguageTokenSyntaxFunctionDeclaration extends LanguageTokenSyntax {

	name: string;
	isPrivate: boolean;
	returnType: string;
	parameters: ParameterTypes<string>[];
	nodes: LanguageNode[];
	constructor(
		name: string,
		isPrivate: boolean,
		returnType: string,
		parameters: ParameterTypes<string>[],
		nodes: LanguageNode[]
	) {
		super(SyntaxKindType.FunctionDeclaration, nodes);
		this.nodes = nodes;
		this.name = name;
		this.isPrivate = isPrivate;
		this.returnType = returnType;
		this.parameters = parameters
	}
}

export class LanguageTokenSyntaxFunctionInvoking extends LanguageTokenSyntax {

}

export class LanguageTokenSyntaxPackage extends LanguageTokenSyntax {
	nodes: LanguageNode[];
	name: string;
	constructor(name : string ,nodes: LanguageNode[]){
		super(SyntaxKindType.PackageDeclaration, nodes);
		this.name = name;
		this.nodes = nodes;
	}
}

export class LanguageTokenSyntaxImport extends LanguageTokenSyntax {

}


export class LanguageTokenSyntaxVariableDeclaration extends LanguageTokenSyntax {

}


export class LanguageTokenSyntaxVariableDeclarationAndAssignment extends LanguageTokenSyntax {

}


export class LanguageTokenSyntaxVariableFunctionDeclarationAndAssignment extends LanguageTokenSyntax {

}





