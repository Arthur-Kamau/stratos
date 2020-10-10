import { LanguageNode } from './language_node';

export class LanguageToken {

	children: LanguageToken[];
	tokenGroup: LanguageNode[];
	constructor(children: LanguageToken[], tokenGroup: LanguageNode[]) {
		this.children = children;
		this.tokenGroup = tokenGroup;

	}
}
