import { SyntaxKindType } from "./syntax_kind";
import { ParameterTypes } from "./syntax_kind"


export class LanguageTokenSyntaxGroup {

	children: LanguageTokenSyntaxGroup[];
	tokenGroup: LanguageTokenSyntax[];
	constructor(children: LanguageTokenSyntaxGroup[], tokenGroup: LanguageTokenSyntax[]) {
		this.children = children;
		this.tokenGroup = tokenGroup;
	}
}
export class LanguageTokenSyntax {
	type: SyntaxKindType;

	constructor(type: SyntaxKindType) {
		this.type = type;
	}

}

export class LanguageTokenSyntaxClass extends LanguageTokenSyntax {
	name: string;
	memberFunctions: LanguageTokenSyntaxFunctionDeclaration[];
	constructos: LanguageTokenSyntaxFunctionDeclaration[];

	constructor(name: string, memberFunctions: LanguageTokenSyntaxFunctionDeclaration[], constructos: LanguageTokenSyntaxFunctionDeclaration[]) {
		super(SyntaxKindType.ClassDeclaration);
		this.name = name;
		this.memberFunctions = memberFunctions;
		this.constructos = constructos;
	}

}

export class LanguageTokenSyntaxFunctionDeclaration extends LanguageTokenSyntax {

	name: string;
	isPrivate: boolean;
	returnType: string;
	parameters: ParameterTypes<string>[];

	constructor(
		name: string,
		isPrivate: boolean,
		returnType: string,
		parameters: ParameterTypes<string>[]
	) {
		super(SyntaxKindType.FunctionDeclaration);
		this.name = name;
		this.isPrivate = isPrivate;
		this.returnType = returnType;
		this.parameters = parameters
	}
}

export class LanguageTokenSyntaxFunctionInvoking extends LanguageTokenSyntax {

}

export class LanguageTokenSyntaxPackage extends LanguageTokenSyntax {

}

export class LanguageTokenSyntaxImport extends LanguageTokenSyntax {

}


export class LanguageTokenSyntaxVariableDeclaration extends LanguageTokenSyntax {

}


export class LanguageTokenSyntaxVariableDeclarationAndAssignment extends LanguageTokenSyntax {

}


export class LanguageTokenSyntaxVariableFunctionDeclarationAndAssignment extends LanguageTokenSyntax {

}





