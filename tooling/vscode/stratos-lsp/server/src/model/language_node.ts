interface LanguageNode {
	char_start?: number;
	char_end?: number;
	line_start?: number;
	line_end?: number;
	type?: NodeType;
	value?: string;
}
