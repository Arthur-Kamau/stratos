export class CommentsCheck {

	removeSpaceCharacter(docTokens: string[]): string[] {
		var sanitizedArray: string[] = [];
		for (let index = 0; index < docTokens.length; index++) {
			if (docTokens[index] == " ") {
// ignore the item
			} else {
				sanitizedArray.push(docTokens[index])
			}
		}
		return sanitizedArray;
	}

	// comments can be of two types
	// single line comments
	// multi line comment
	removecomments(docTokens: string[]): string[] {
		var sanitizedArray: string[] = [];
		var areWeInLineComment: boolean = false;
		var areWeInMuliLineComment: boolean = false;

		for (let index = 0; index < docTokens.length; index++) {
			const item = docTokens[index];
			const peekItem = docTokens[index + 1];
			const prevItem = docTokens[index - 1];

			if (areWeInLineComment) {
				if (item == "\n" || item == "\r") {
					areWeInLineComment = false;
				}
			} else if (areWeInMuliLineComment) {
				if (item == "/" && prevItem == "*") {
					areWeInMuliLineComment = false
				}
			} else {
				if (item == "/" && peekItem == "/") {
					areWeInLineComment = true
				} else if (item == "/" && peekItem == "*") {
					areWeInMuliLineComment = true
				} else {
					sanitizedArray.push(item)
				}
			}

		}
		return sanitizedArray;
	}

}