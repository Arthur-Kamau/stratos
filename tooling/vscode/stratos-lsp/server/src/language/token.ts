import { LanguageNode } from "../model/Node";
export class LanguageTokens {
	getTokenList(text: string): LanguageNode[] {
		var documentNode: LanguageNode[] = []
		let lines = text.split(/\r?\n/g);
		for (let index = 0; index < lines.length; index++) {

			var lineCharArray = lines[index].split('');

			for (let index = 0; index < lineCharArray.length; index++) {
				var n: LanguageNode = {
					start: index,
					end: index + lineCharArray[index].length,
					line: index,
					value: lineCharArray[index]
				};
				documentNode.push(n)
			}

		}
		return documentNode;
	}
}