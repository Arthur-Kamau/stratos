import {NodeType} from "../model/node_type";
export interface LanguageNode {
	char_start?: number;
	char_end?: number;
	line_start?: number;
	line_end?: number;
	type?: NodeType;
	value?: string;
}
