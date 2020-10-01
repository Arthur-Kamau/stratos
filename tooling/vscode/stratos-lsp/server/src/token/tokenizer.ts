import {getCharNodeType , isUniqueSign , getkeySignsNodeType } from "../util/lookup_node";

export class LanguageTokens {



	getTokenList(text: string): LanguageNode[] {
		var documentNode: LanguageNode[] = []
		let lines = text.split(/\r?\n/g);
		let inMultiLineComment: boolean = false
		let multiLineCommet: string = "";
		let linemultiLineCommetStart: number = 0;
		for (let lineIndex = 0; lineIndex < lines.length; lineIndex++) {

			if (lines[lineIndex].length > 0) {
				var lineCharArray = lines[lineIndex].split('');

				var charaArray: string = "";

				for (let charIndex = 0; charIndex < lineCharArray.length; charIndex++) {
					var tempcharIndex = charIndex;

					if (lineCharArray[charIndex] == "/" && lineCharArray[charIndex + 1] == "/") {

						var n: LanguageNode = {
							line_start: lineIndex,
							line_end: lineIndex,
							char_start: charIndex,
							char_end: lines[lineIndex].length,
							type: NodeType.LineComment,
							value: lines[lineIndex].substr(charIndex, lines[lineIndex].length),
						};
						documentNode.push(n);
						break;
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[tempcharIndex + 1] == "*") {

						inMultiLineComment = true;
						linemultiLineCommetStart = lineIndex;

						multiLineCommet += lineCharArray[charIndex];
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[tempcharIndex - 1] == "*") {

						inMultiLineComment = false;

						multiLineCommet += lineCharArray[charIndex];

						var n: LanguageNode = {
							line_start: linemultiLineCommetStart,
							line_end: lineIndex,
							char_start: charIndex,
							char_end: charIndex + lineCharArray[charIndex].length,  // ,
							type: NodeType.MultiLineComment,
							value: multiLineCommet
						};
						documentNode.push(n);

						multiLineCommet = "";
						linemultiLineCommetStart = 0;

					}

					if (!inMultiLineComment) {
						if (isUniqueSign(lineCharArray[charIndex])) {

							console.log("add  " + lineCharArray[charIndex])
							

							if (charaArray.length > 0) {
								
								var x: LanguageNode = {
									line_start: lineIndex,
									line_end: lineIndex,
									char_start: charIndex != 0 ? charIndex - charaArray.length : 0,
									char_end: charIndex,
									type: getCharNodeType(charaArray),
									value: charaArray
								};
								documentNode.push(x);
							} else {
								console.log("character array empty " + lineCharArray[charIndex] + " line " + lineIndex)
							}

							var n: LanguageNode = {
								line_start: lineIndex,
								line_end: lineIndex,
								char_start: charIndex,
								char_end: charIndex + lineCharArray[charIndex].length,
								type: getkeySignsNodeType(lineCharArray[charIndex]),
								value: lineCharArray[charIndex],
							};
							documentNode.push(n);

							charaArray = "";

						} else if (lineCharArray[charIndex] == " " || lineCharArray[charIndex] == "\t") {


							if (charaArray.length > 0) {

								var x: LanguageNode = {
									line_start: lineIndex,
									line_end: lineIndex,
									char_start: charIndex - charaArray.length,
									char_end: charaArray.length,
									type: getCharNodeType(charaArray),
									value: charaArray
								};
								documentNode.push(x);

								charaArray = "";
							} else {
								console.log(" line " + lineIndex + " character array empty  item space " + lineCharArray[charIndex])
							}


							var x: LanguageNode = {
								line_start: lineIndex,
								line_end: lineIndex,
								char_start: charIndex,
								char_end: charIndex + lineCharArray[charIndex].length,
								type: NodeType.SpaceNode,
								value: " "
							};
							documentNode.push(x);


						} else {

							charaArray += lineCharArray[charIndex]
						}

					} else {
						multiLineCommet += lineCharArray[charIndex];
					}

				}


			} else {
				//console.log("Empty line number "+lineIndex);
			}

			var n: LanguageNode = {
				line_start: lineIndex,
				line_end: lineIndex,
				char_start: lines[lineIndex].length-1,
				char_end: lines[lineIndex].length,
				type: NodeType.NewLine,
				value: "\n",
			};
			documentNode.push(n);

		}
		return documentNode;
	}


}