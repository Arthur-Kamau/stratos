import {
	createConnection,
	TextDocuments,
	Diagnostic,
	DiagnosticSeverity,
	ProposedFeatures,
	InitializeParams,
	DidChangeConfigurationNotification,
	CompletionItem,
	CompletionItemKind,
	TextDocumentPositionParams,
	TextDocumentSyncKind,
	InitializeResult,
	MarkupKind,
	InsertTextFormat,
	Definition,
	Location,
	Range,
	Position
} from 'vscode-languageserver/node';

import {
	TextDocument
} from 'vscode-languageserver-textdocument';

import { execFile } from 'child_process';
import * as path from 'path';
import * as fs from 'fs';
import * as os from 'os';
import * as hocon from 'hocon-parser';

// Create a connection for the server, using Node's IPC as a transport.
const connection = createConnection(ProposedFeatures.all);

// Create a simple text document manager.
const documents: TextDocuments<TextDocument> = new TextDocuments(TextDocument);

let hasConfigurationCapability = false;
let hasWorkspaceFolderCapability = false;
let hasDiagnosticRelatedInformationCapability = false;

// Standard library completion items
const stdlibCompletions: CompletionItem[] = [];

// Function signatures for validation
interface FunctionSignature {
	name: string;
	params: string[];
	returnType: string;
	module?: string;
}

const stdlibFunctions: Map<string, FunctionSignature> = new Map();

// Function definitions cache for go-to-definition
interface FunctionDefinition {
	name: string;
	uri: string;
	range: Range;
	params: Array<{name: string, type: string}>;
	returnType: string;
}

const functionDefinitions: Map<string, FunctionDefinition> = new Map();

// Workspace symbols cache
const workspaceSymbols: Map<string, CompletionItem[]> = new Map();

// HOCON specific completion items
const hoconCompletionItems: { [key: string]: CompletionItem[] } = {
    // Top-level completions
    "__root__": [
        { label: "project", kind: CompletionItemKind.Module, detail: "Defines project metadata", insertText: "project {\n\t$0\n}", insertTextFormat: InsertTextFormat.Snippet },
        { label: "build", kind: CompletionItemKind.Module, detail: "Defines build configuration", insertText: "build {\n\t$0\n}", insertTextFormat: InsertTextFormat.Snippet },
        { label: "dependencies", kind: CompletionItemKind.Module, detail: "Defines project dependencies", insertText: "dependencies {\n\t$0\n}", insertTextFormat: InsertTextFormat.Snippet }
    ],
    // 'project' section completions
    "project": [
        { label: "name", kind: CompletionItemKind.Property, detail: "Project name", insertText: "name = \"${1:my-project}\"", insertTextFormat: InsertTextFormat.Snippet },
        { label: "version", kind: CompletionItemKind.Property, detail: "Project version", insertText: "version = \"${1:1.0.0}\"", insertTextFormat: InsertTextFormat.Snippet },
        { label: "description", kind: CompletionItemKind.Property, detail: "Project description", insertText: "description = \"${1:A new Stratos project}\"", insertTextFormat: InsertTextFormat.Snippet },
        { label: "authors", kind: CompletionItemKind.Property, detail: "List of authors", insertText: "authors = [\"${1:Your Name}\"]", insertTextFormat: InsertTextFormat.Snippet },
        { label: "keywords", kind: CompletionItemKind.Property, detail: "Keywords for dependency search", insertText: "keywords = [\"${1:keyword1}\", \"${2:keyword2}\"]", insertTextFormat: InsertTextFormat.Snippet },
        { label: "categories", kind: CompletionItemKind.Property, detail: "Categories for crates.io", insertText: "categories = [\"${1:category1}\"]", insertTextFormat: InsertTextFormat.Snippet },
        { label: "license", kind: CompletionItemKind.Property, detail: "Project license", insertText: "license = \"${1:MIT}\"", insertTextFormat: InsertTextFormat.Snippet },
        { label: "repository", kind: CompletionItemKind.Property, detail: "Repository URL", insertText: "repository = \"${1:https://github.com/yourname/your-repo}\"", insertTextFormat: InsertTextFormat.Snippet },
        { label: "documentation", kind: CompletionItemKind.Property, detail: "Documentation URL", insertText: "documentation = \"${1:https://github.com/yourname/your-repo}\"", insertTextFormat: InsertTextFormat.Snippet }
    ],
    // 'build' section completions
    "build": [
        { label: "entry", kind: CompletionItemKind.Property, detail: "Entry point file (e.g., src/main.st)", insertText: "entry = \"${1:src/main.st}\"", insertTextFormat: InsertTextFormat.Snippet },
        { label: "output", kind: CompletionItemKind.Property, detail: "Output path for build artifact", insertText: "output = \"${1:build/my-project}\"", insertTextFormat: InsertTextFormat.Snippet }
    ],
    // 'dependencies' section completions - generic example
    "dependencies": [
        { label: "dependency-name", kind: CompletionItemKind.Property, detail: "Add a project dependency", insertText: "${1:dependency-name} = \"${2:github.com/user/repo}\"", insertTextFormat: InsertTextFormat.Snippet }
    ]
};


connection.onInitialize((params: InitializeParams) => {
	const capabilities = params.capabilities;

	hasConfigurationCapability = !!(
		capabilities.workspace && !!capabilities.workspace.configuration
	);
	hasWorkspaceFolderCapability = !!(
		capabilities.workspace && !!capabilities.workspace.workspaceFolders
	);
	hasDiagnosticRelatedInformationCapability = !!(
		capabilities.textDocument &&
		capabilities.textDocument.publishDiagnostics &&
		capabilities.textDocument.publishDiagnostics.relatedInformation
	);

	const result: InitializeResult = {
		capabilities: {
			textDocumentSync: TextDocumentSyncKind.Incremental,
			completionProvider: {
				resolveProvider: true,
				triggerCharacters: ['.', ':']
			},
			definitionProvider: true
		}
	};
	if (hasWorkspaceFolderCapability) {
		result.capabilities.workspace = {
			workspaceFolders: {
				supported: true
			}
		};
	}
	return result;
});

connection.onInitialized(() => {
	if (hasConfigurationCapability) {
		connection.client.register(DidChangeConfigurationNotification.type, undefined);
	}
	if (hasWorkspaceFolderCapability) {
		connection.workspace.onDidChangeWorkspaceFolders(_event => {
			connection.console.log('Workspace folder change event received.');
		});
	}

	// Load standard library completions
	loadStandardLibraryCompletions();
});

// Load standard library function signatures for autocomplete
function loadStandardLibraryCompletions() {
	// Standard library modules and their functions
	const stdlibModules = {
		'math': [
			{ name: 'PI', type: 'const', returnType: 'double', doc: 'Pi (π) = 3.14159...' },
			{ name: 'E', type: 'const', returnType: 'double', doc: 'Euler\'s number (e) = 2.71828...' },
			{ name: 'PHI', type: 'const', returnType: 'double', doc: 'Golden ratio (φ) = 1.61803...' },
			{ name: 'sin', type: 'function', params: 'x: double', returnType: 'double', doc: 'Sine function' },
			{ name: 'cos', type: 'function', params: 'x: double', returnType: 'double', doc: 'Cosine function' },
			{ name: 'tan', type: 'function', params: 'x: double', returnType: 'double', doc: 'Tangent function' },
			{ name: 'sqrt', type: 'function', params: 'x: double', returnType: 'double', doc: 'Square root' },
			{ name: 'pow', type: 'function', params: 'x: double, y: double', returnType: 'double', doc: 'Power function x^y' },
			{ name: 'abs', type: 'function', params: 'x: double', returnType: 'double', doc: 'Absolute value' },
			{ name: 'ceil', type: 'function', params: 'x: double', returnType: 'double', doc: 'Round up' },
			{ name: 'floor', type: 'function', params: 'x: double', returnType: 'double', doc: 'Round down' },
			{ name: 'round', type: 'function', params: 'x: double', returnType: 'double', doc: 'Round to nearest' },
			{ name: 'max', type: 'function', params: 'a: double, b: double', returnType: 'double', doc: 'Maximum value' },
			{ name: 'min', type: 'function', params: 'a: double, b: double', returnType: 'double', doc: 'Minimum value' },
			{ name: 'toRadians', type: 'function', params: 'degrees: double', returnType: 'double', doc: 'Convert degrees to radians' },
			{ name: 'toDegrees', type: 'function', params: 'radians: double', returnType: 'double', doc: 'Convert radians to degrees' },
		],
		'strings': [
			{ name: 'toUpper', type: 'function', params: 's: string', returnType: 'string', doc: 'Convert to uppercase' },
			{ name: 'toLower', type: 'function', params: 's: string', returnType: 'string', doc: 'Convert to lowercase' },
			{ name: 'trim', type: 'function', params: 's: string', returnType: 'string', doc: 'Remove leading/trailing whitespace' },
			{ name: 'split', type: 'function', params: 's: string, separator: string', returnType: 'Array<string>', doc: 'Split string by separator' },
			{ name: 'join', type: 'function', params: 'parts: Array<string>, separator: string', returnType: 'string', doc: 'Join array with separator' },
			{ name: 'contains', type: 'function', params: 's: string, substr: string', returnType: 'bool', doc: 'Check if string contains substring' },
			{ name: 'indexOf', type: 'function', params: 's: string, substr: string', returnType: 'int', doc: 'Find index of substring' },
			{ name: 'length', type: 'function', params: 's: string', returnType: 'int', doc: 'Get string length' },
			{ name: 'replace', type: 'function', params: 's: string, old: string, new: string', returnType: 'string', doc: 'Replace first occurrence' },
			{ name: 'replaceAll', type: 'function', params: 's: string, old: string, new: string', returnType: 'string', doc: 'Replace all occurrences' },
		],
		'log': [
			{ name: 'debug', type: 'function', params: 'message: string', returnType: 'void', doc: 'Log debug message' },
			{ name: 'info', type: 'function', params: 'message: string', returnType: 'void', doc: 'Log info message' },
			{ name: 'warn', type: 'function', params: 'message: string', returnType: 'void', doc: 'Log warning message' },
			{ name: 'error', type: 'function', params: 'message: string', returnType: 'void', doc: 'Log error message' },
			{ name: 'fatal', type: 'function', params: 'message: string', returnType: 'void', doc: 'Log fatal message' },
		],
		'io': [
			{ name: 'readFile', type: 'function', params: 'path: string', returnType: 'string', doc: 'Read file contents' },
			{ name: 'writeFile', type: 'function', params: 'path: string, content: string', returnType: 'bool', doc: 'Write to file' },
			{ name: 'appendFile', type: 'function', params: 'path: string, content: string', returnType: 'bool', doc: 'Append to file' },
			{ name: 'exists', type: 'function', params: 'path: string', returnType: 'bool', doc: 'Check if file exists' },
			{ name: 'delete', type: 'function', params: 'path: string', returnType: 'bool', doc: 'Delete file' },
		],
		'time': [
			{ name: 'now', type: 'function', params: '', returnType: 'Time', doc: 'Get current time' },
			{ name: 'sleep', type: 'function', params: 'ms: int', returnType: 'void', doc: 'Sleep for milliseconds' },
		],
		'os': [
			{ name: 'getenv', type: 'function', params: 'name: string', returnType: 'string?', doc: 'Get environment variable' },
			{ name: 'setenv', type: 'function', params: 'name: string, value: string', returnType: 'bool', doc: 'Set environment variable' },
			{ name: 'exec', type: 'function', params: 'command: string, args: Array<string>', returnType: 'ProcessResult', doc: 'Execute command' },
			{ name: 'exit', type: 'function', params: 'code: int', returnType: 'void', doc: 'Exit program' },
			{ name: 'platform', type: 'function', params: '', returnType: 'string', doc: 'Get platform name' },
		],
		'net': [
			{ name: 'get', type: 'function', params: 'url: string', returnType: 'Response', doc: 'HTTP GET request' },
			{ name: 'post', type: 'function', params: 'url: string, body: string', returnType: 'Response', doc: 'HTTP POST request' },
		],
		'crypto': [
			{ name: 'sha256', type: 'function', params: 'data: string', returnType: 'string', doc: 'SHA-256 hash' },
			{ name: 'md5', type: 'function', params: 'data: string', returnType: 'string', doc: 'MD5 hash' },
			{ name: 'uuid', type: 'function', params: '', returnType: 'string', doc: 'Generate UUID v4' },
		],
		'async': [
			{ name: 'sleep', type: 'function', params: 'ms: int', returnType: 'Promise<void>', doc: 'Async sleep' },
			{ name: 'all', type: 'function', params: 'promises: Array<Promise<T>>', returnType: 'Promise<Array<T>>', doc: 'Wait for all promises' },
			{ name: 'race', type: 'function', params: 'promises: Array<Promise<T>>', returnType: 'Promise<T>', doc: 'First promise wins' },
		],
		'collections': [
			{ name: 'Map', type: 'class', doc: 'Hash map data structure' },
			{ name: 'Set', type: 'class', doc: 'Set data structure' },
			{ name: 'List', type: 'class', doc: 'Linked list data structure' },
			{ name: 'Queue', type: 'class', doc: 'FIFO queue data structure' },
		],
	};

	// Create completion items for module names
	Object.keys(stdlibModules).forEach(moduleName => {
		stdlibCompletions.push({
			label: moduleName,
			kind: CompletionItemKind.Module,
			detail: `Standard library module`,
			documentation: {
				kind: MarkupKind.Markdown,
				value: `Import with: 	use ${moduleName};	`
			},
			insertText: moduleName
		});
	});

	// Create completion items for module.function patterns
	Object.entries(stdlibModules).forEach(([moduleName, items]) => {
		items.forEach((item: any) => {
			const label = `${moduleName}.${item.name}`;
			let kind = CompletionItemKind.Function;
			let detail = '';
			let insertText = item.name;

			if (item.type === 'const') {
				kind = CompletionItemKind.Constant as any;
				detail = `${item.returnType}`;
				insertText = item.name; // Constants don't need snippets
			} else if (item.type === 'function') {
				kind = CompletionItemKind.Function as any;
				detail = `fn ${item.name}(${item.params}) ${item.returnType}`;

				// Store function signature for validation
				const params = item.params ? item.params.split(',').map((p: string) => p.trim()) : [];
				stdlibFunctions.set(label, {
					name: item.name,
					params: params,
					returnType: item.returnType,
					module: moduleName
				});

				if (item.params) {
					// Create snippet with parameter placeholders
					const paramCount = params.length;
					const placeholders = Array.from({ length: paramCount }, (_, i) => `	${i + 1}}`).join(', ');
					insertText = `${item.name}(${placeholders})`;
				} else {
					insertText = `${item.name}()`;
				}
			} else if (item.type === 'class') {
				kind = CompletionItemKind.Class as any;
				detail = `class ${item.name}`;
				insertText = item.name;
			}

			stdlibCompletions.push({
				label: label,
				kind: kind,
				detail: detail,
				documentation: {
					kind: MarkupKind.Markdown,
					value: item.doc
				},
				insertText: insertText,
				insertTextFormat: InsertTextFormat.Snippet,
				filterText: label,
				sortText: `1_${label}` // Prioritize stdlib
			});
		});
	});
}

// Validate document on change
documents.onDidChangeContent(change => {
	validateTextDocument(change.document);
	indexFunctionDefinitions(change.document);
});

// Function to find duplicate keys in HOCON files
function findHoconDuplicateKeys(textDocument: TextDocument): Diagnostic[] {
    const diagnostics: Diagnostic[] = [];
    const lines = textDocument.getText().split('\n');

    interface Scope {
        keys: Map<string, Range>;
        braceLine: number; // Line number where this scope's opening brace was found
    }

    const scopeStack: Scope[] = [{ keys: new Map<string, Range>(), braceLine: -1 }]; // Root scope

    lines.forEach((lineText, i) => {
        const lineNumber = i;

        // Detect scope opening
        const openBraceMatch = lineText.match(/(\w+)\s*\{/);
        if (openBraceMatch) {
            // Push the new scope onto the stack
            scopeStack.push({ keys: new Map<string, Range>(), braceLine: lineNumber });
            // The key that opens the scope is handled by the regular key detection below
        }

        // Detect key declarations
        // Matches: key = value, key: value, key { (object-like key)
        const keyMatch = lineText.match(/^\s*([a-zA-Z_][a-zA-Z0-9_\\-\\.]*|\"[^\"]+\")\s*(?::|=|\{|$)/);
        if (keyMatch) {
            let key = keyMatch[1];
            // Remove quotes if it's a quoted key
            if (key.startsWith('"') && key.endsWith('"')) {
                key = key.substring(1, key.length - 1);
            }

            const currentScope = scopeStack[scopeStack.length - 1];
            if (currentScope.keys.has(key)) {
                // Duplicate key found
                const existingRange = currentScope.keys.get(key)!;
                const duplicateRange: Range = {
                    start: { line: lineNumber, character: lineText.indexOf(keyMatch[1]) },
                    end: { line: lineNumber, character: lineText.indexOf(keyMatch[1]) + keyMatch[1].length }
                };

                diagnostics.push({
                    severity: DiagnosticSeverity.Error,
                    range: duplicateRange,
                    message: `Duplicate key '${key}' in this scope. First declared on line ${existingRange.start.line + 1}.`,
                    source: 'hocon-linter'
                });
            } else {
                currentScope.keys.set(key, {
                    start: { line: lineNumber, character: lineText.indexOf(keyMatch[1]) },
                    end: { line: lineNumber, character: lineText.indexOf(keyMatch[1]) + keyMatch[1].length }
                });
            }
        }

        // Detect scope closing
        const closeBraceMatch = lineText.match(/\}/);
        if (closeBraceMatch) {
            if (scopeStack.length > 1) { // Never pop the root scope
                scopeStack.pop();
            }
        }
    });

    return diagnostics;
}


async function validateTextDocument(textDocument: TextDocument): Promise<void> {
	const text = textDocument.getText();
	let diagnostics: Diagnostic[] = []; // Change to let for adding HOCON diagnostics

	if (textDocument.languageId === 'hocon') {
		diagnostics = findHoconDuplicateKeys(textDocument);
		connection.sendDiagnostics({ uri: textDocument.uri, diagnostics });
		return; // Exit early for HOCON files as no compiler validation is needed
	}

	// Get compiler path from settings or use default
	const settings = await getDocumentSettings(textDocument.uri);
	let compilerPath = settings.compilerPath;

	// Try to find compiler in common locations
	if (!compilerPath || !fs.existsSync(compilerPath)) {
		const possiblePaths = [
			'/usr/local/bin/stratos',
			'/usr/bin/stratos',
			path.join(os.homedir(), 'bin', 'stratos'),
			'stratos' // Try PATH
		];

		for (const tryPath of possiblePaths) {
			try {
				if (fs.existsSync(tryPath)) {
					compilerPath = tryPath;
					break;
				}
			} catch {}
		}

		if (!compilerPath) {
			compilerPath = 'stratos'; // Fallback to PATH
		}
	}

	// Write content to temp file
	const tempFile = path.join(os.tmpdir(), 'stratos_temp_' + Math.random().toString(36).substring(7) + '.st');
	fs.writeFileSync(tempFile, text);

	// Run compiler for validation
	execFile(compilerPath, [tempFile, '-v'], { timeout: 5000 }, (error, stdout, stderr) => {
		// Cleanup temp file
		try {
			if (fs.existsSync(tempFile)) {
				fs.unlinkSync(tempFile);
			}
		} catch {}

		// Parse compiler output for errors
		const output = (stderr + stdout).toString();

		// Pattern: [Error] Line:Col: Message
		const errorRegex = /\ \[Error\]\s+(?:Line\s+)?(\d+):(\d+):\s+(.+)/gi;
		// Pattern: Error at line X, column Y: Message
		const altErrorRegex = /Error\s+at\s+line\s+(\d+),\s+column\s+(\d+):\s+(.+)/gi;
		// Pattern: Line X:Y - Error: Message
		const simpleErrorRegex = /(\d+):(\d+)\s*-\s*(?:Error|Warning):\s*(.+)/gi;

		let match;
		const patterns = [errorRegex, altErrorRegex, simpleErrorRegex];

		for (const regex of patterns) {
			while ((match = regex.exec(output)) !== null) {
				const line = parseInt(match[1]) - 1; // LSP is 0-based
				const col = parseInt(match[2]) - 1;
				const msg = match[3].trim();

				// Determine severity
				let severity: DiagnosticSeverity = DiagnosticSeverity.Error as any;
				if (msg.toLowerCase().includes('warning')) {
					severity = DiagnosticSeverity.Warning as any;
				}

				// Get end position (try to highlight the whole token)
				const textLine = textDocument.getText({
					start: { line, character: 0 },
					end: { line, character: 1000 }
				});

				let endCol = col + 1;
				// Try to find the end of the current word/token
				if (textLine && col < textLine.length) {
					const remainingLine = textLine.substring(col);
					const wordMatch = remainingLine.match(/^[a-zA-Z_][a-zA-Z0-9_]*/);
					if (wordMatch) {
						endCol = col + wordMatch[0].length;
					} else {
						endCol = Math.min(col + 20, textLine.length);
					}
				}

				const diagnostic: Diagnostic = {
					severity: severity,
					range: {
						start: { line: line, character: col },
						end: { line: line, character: endCol }
					},
					message: msg,
					source: 'stratos-compiler'
				};
				diagnostics.push(diagnostic);
			}
		}

		// Add runtime error detection (simple pattern matching)
		detectRuntimeErrors(textDocument, diagnostics);

		// Send diagnostics to client
		connection.sendDiagnostics({ uri: textDocument.uri, diagnostics });
	});
}

// Detect common runtime errors
function detectRuntimeErrors(textDocument: TextDocument, diagnostics: Diagnostic[]) {
	const text = textDocument.getText();
	const lines = text.split('\n');

	// Extract imported modules from document
	const importedModules = new Set<string>();
	const moduleRegex = /use\s+([a-zA-Z_][a-zA-Z0-9_]*);/g;
	let moduleMatch;
	while ((moduleMatch = moduleRegex.exec(text)) !== null) {
		importedModules.add(moduleMatch[1]);
	}

	lines.forEach((line, lineIndex) => {
		// Division by zero
		if (/\/\s*0(?!\d)/g.test(line)) {
			const match = line.match(/\/\s*0(?!\d)/);
			if (match) {
				diagnostics.push({
					severity: DiagnosticSeverity.Warning,
					range: {
						start: { line: lineIndex, character: match.index || 0 },
						end: { line: lineIndex, character: (match.index || 0) + match[0].length }
				},
				message: 'Potential division by zero',
				source: 'stratos-linter'
			});
			}
		}

		// Null pointer access
		if (/	\?\s*\./.test(line)) {
			// This is safe navigation, so it's fine
		} else if (/([a-zA-Z_][a-zA-Z0-9_]*)\s*\./.test(line) && line.includes('= null')) {
			const match = line.match(/([a-zA-Z_][a-zA-Z0-9_]*)\s*\./);
			if (match) {
				diagnostics.push({
					severity: DiagnosticSeverity.Warning,
					range: {
						start: { line: lineIndex, character: match.index || 0 },
						end: { line: lineIndex, character: (match.index || 0) + match[0].length }
				},
				message: 'Potential null pointer access. Consider using safe navigation (?.) or null check',
				source: 'stratos-linter'
			});
			}
		}

		// Array out of bounds (simple heuristic)
		const arrayAccessMatch = line.match(/\ \[(-?\d+)\]/);
		if (arrayAccessMatch) {
			const index = parseInt(arrayAccessMatch[1]);
			if (index < 0) {
				diagnostics.push({
					severity: DiagnosticSeverity.Warning,
					range: {
						start: { line: lineIndex, character: arrayAccessMatch.index || 0 },
						end: { line: lineIndex, character: (arrayAccessMatch.index || 0) + arrayAccessMatch[0].length }
				},
				message: 'Negative array index',
				source: 'stratos-linter'
			});
			}
		}

		// Check for function calls - module.function(args)
		const functionCallRegex = /([a-zA-Z_][a-zA-Z0-9_]*)\.([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)/g;
		let callMatch;
		while ((callMatch = functionCallRegex.exec(line)) !== null) {
			const moduleName = callMatch[1];
			const functionName = callMatch[2];
			const argsString = callMatch[3];
			const fullName = `${moduleName}.${functionName}`;

			// Check if function exists in stdlib
			const funcSignature = stdlibFunctions.get(fullName);

			if (importedModules.has(moduleName)) {
				if (!funcSignature) {
					// Function doesn't exist in this module
					const startChar = callMatch.index || 0;
					const endChar = startChar + callMatch[0].length;
					diagnostics.push({
						severity: DiagnosticSeverity.Error,
						range: {
							start: { line: lineIndex, character: startChar },
							end: { line: lineIndex, character: endChar }
						},
					message: `Function '${functionName}' does not exist in module '${moduleName}'`,
					source: 'stratos-linter'
					});
				} else {
					// Check argument count
					const providedArgs = argsString.trim() ? argsString.split(',').map(a => a.trim()).filter(a => a) : [];
					const expectedArgCount = funcSignature.params.length;

					if (providedArgs.length !== expectedArgCount) {
						const startChar = callMatch.index || 0;
						const endChar = startChar + callMatch[0].length;
						diagnostics.push({
							severity: DiagnosticSeverity.Error,
							range: {
								start: { line: lineIndex, character: startChar },
								end: { line: lineIndex, character: endChar }
							},
						message: `Function '${fullName}' expects ${expectedArgCount} argument(s) but got ${providedArgs.length}`,
						source: 'stratos-linter'
						});
					}
				}
			}
		}

		// Check for user-defined function calls - standalone function(args)
		const standaloneCallRegex = /(?<![a-zA-Z0-9_\.])([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)/g;
		let standaloneMatch;
		while ((standaloneMatch = standaloneCallRegex.exec(line)) !== null) {
			const functionName = standaloneMatch[1];
			const argsString = standaloneMatch[2];

			// Skip known built-in functions and keywords
			const builtIns = ['print', 'Some', 'None', 'if', 'when', 'while', 'for', 'return'];
			if (builtIns.includes(functionName)) {
				continue;
			}

			// Skip if it looks like a type or class (starts with uppercase)
			if (functionName[0] === functionName[0].toUpperCase()) {
				continue;
			}

			// Check if it's a user-defined function
			const userFunc = functionDefinitions.get(functionName);
			if (userFunc) {
				// Parse provided arguments
				const providedArgs = argsString.trim() ? argsString.split(',').map(a => a.trim()).filter(a => a) : [];
				const expectedParams = userFunc.params;

				// Check argument count
				if (providedArgs.length !== expectedParams.length) {
					const startChar = standaloneMatch.index || 0;
					const endChar = startChar + standaloneMatch[0].length;
					diagnostics.push({
						severity: DiagnosticSeverity.Error,
						range: {
							start: { line: lineIndex, character: startChar },
							end: { line: lineIndex, character: endChar }
						},
						message: `Function '${functionName}' expects ${expectedParams.length} argument(s) but got ${providedArgs.length}`,
						source: 'stratos-linter'
						});
					continue;
				}

				// Check argument types (basic type inference)
				for (let i = 0; i < providedArgs.length; i++) {
					const arg = providedArgs[i];
					const expectedType = expectedParams[i].type;
					let inferredType: string | null = null;

					// Infer type from argument
					if (arg.startsWith('"') || arg.startsWith("'")) {
						inferredType = 'string';
					} else if (/^-?\d+$/.test(arg)) {
						inferredType = 'int';
					} else if (/^-?\d+\.\d+$/.test(arg)) {
						inferredType = 'double';
					} else if (arg === 'true' || arg === 'false') {
						inferredType = 'bool';
					} else if (arg === 'null') {
						inferredType = 'null';
					}

					// Check type mismatch
					if (inferredType && inferredType !== expectedType && !expectedType.includes('?')) {
						const startChar = standaloneMatch.index || 0;
						const endChar = startChar + standaloneMatch[0].length;
						diagnostics.push({
							severity: DiagnosticSeverity.Error,
							range: {
								start: { line: lineIndex, character: startChar },
								end: { line: lineIndex, character: endChar }
							},
							message: `Argument ${i + 1} of function '${functionName}' expects type '${expectedType}' but got '${inferredType}'`,
							source: 'stratos-linter'
							});
					}
				}
			}
		}
	});
}

// Index function definitions in a document
function indexFunctionDefinitions(textDocument: TextDocument) {
	const text = textDocument.getText();
	const lines = text.split('\n');
	const uri = textDocument.uri;

	// Remove old definitions from this document
	const keysToDelete: string[] = [];
	functionDefinitions.forEach((def, key) => {
		if (def.uri === uri) {
			keysToDelete.push(key);
		}
	});
	keysToDelete.forEach(key => functionDefinitions.delete(key));

	// Parse function definitions: fn functionName(params) returnType {
	// Pattern: fn name(param1: type1, param2: type2) returnType {
	const functionRegex = /fn\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*([a-zA-Z_][a-zA-Z0-9_?]*)/g;

	lines.forEach((line, lineIndex) => {
		let match;
		const regex = new RegExp(functionRegex);
		while ((match = regex.exec(line)) !== null) {
			const functionName = match[1];
			const paramsString = match[2].trim();
			const returnType = match[3].trim() || 'void';
			const startChar = match.index;
			const endChar = startChar + match[0].length;

			// Parse parameters: "a: int, b: int" -> [{name: "a", type: "int"}, {name: "b", type: "int"}]
			const params: Array<{name: string, type: string}> = [];
			if (paramsString) {
				const paramParts = paramsString.split(',');
				paramParts.forEach(part => {
					const trimmed = part.trim();
					// Match: paramName: paramType
					const paramMatch = trimmed.match(/([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*([a-zA-Z_][a-zA-Z0-9_?<>,	]*)/);
					if (paramMatch) {
						params.push({
							name: paramMatch[1],
							type: paramMatch[2].trim()
						});
					}
				});
			}

			const range: Range = {
				start: { line: lineIndex, character: startChar },
				end: { line: lineIndex, character: endChar }
			};

			functionDefinitions.set(functionName, {
				name: functionName,
				uri: uri,
			range: range,
				params: params,
				returnType: returnType
			});
		}
	});
}

// Settings interface
interface StratosSettings {
	maxNumberOfProblems: number;
	compilerPath: string;
}

const defaultSettings: StratosSettings = {
	maxNumberOfProblems: 1000,
	compilerPath: ''
};
let globalSettings: StratosSettings = defaultSettings;

const documentSettings: Map<string, Thenable<StratosSettings>> = new Map();

connection.onDidChangeConfiguration(change => {
	if (hasConfigurationCapability) {
		documentSettings.clear();
	} else {
		globalSettings = <StratosSettings>(
			(change.settings.stratosLanguageServer || defaultSettings)
		);
	}

	documents.all().forEach(validateTextDocument);
});

function getDocumentSettings(resource: string): Thenable<StratosSettings> {
	if (!hasConfigurationCapability) {
		return Promise.resolve(globalSettings);
	}
	let result = documentSettings.get(resource);
	if (!result) {
		result = connection.workspace.getConfiguration({
			scopeUri: resource,
			section: 'stratosLanguageServer'
		});
		documentSettings.set(resource, result);
	}
	return result;
}

documents.onDidClose(e => {
	documentSettings.delete(e.document.uri);
});



// Completion handler
connection.onCompletion(
	(textDocumentPosition: TextDocumentPositionParams): CompletionItem[] => {
		const document = documents.get(textDocumentPosition.textDocument.uri);
		if (!document) {
			return [];
		}

		const text = document.getText();
		const offset = document.offsetAt(textDocumentPosition.position);
		const linePrefix = text.substring(
			Math.max(0, offset - 100),
			offset
		);
        const languageId = document.languageId;

        if (languageId === 'hocon') {
            let completions: CompletionItem[] = [];
            
            try {
                // Determine the current scope key
                let currentScopeKey = "__root__";
                const currentPosition = textDocumentPosition.position;
                const lines = text.split('\n');
                let scopeStack: string[] = [];

                // Iterate through lines from the beginning up to the current line
                for (let i = 0; i <= currentPosition.line; i++) {
                    let line = lines[i];
                    // For the current line, only consider text up to the cursor position
                    if (i === currentPosition.line) {
                        line = line.substring(0, currentPosition.character);
                    }

                    // Match keys followed by an opening brace (and handle multiple on one line)
                    let openMatch;
                    const openBraceRegex = /(\w+)\s*\{/g;
                    while ((openMatch = openBraceRegex.exec(line)) !== null) {
                        scopeStack.push(openMatch[1]);
                    }

                    // Match closing braces (and handle multiple on one line)
                    let closeMatch;
                    const closeBraceRegex = /\}/g;
                    while ((closeMatch = closeBraceRegex.exec(line)) !== null) {
                        if (scopeStack.length > 0) {
                            scopeStack.pop();
                        }
                    }
                }
                
                // The top of the stack is the most immediate parent scope
                if (scopeStack.length > 0) {
                    currentScopeKey = scopeStack[scopeStack.length - 1];
                }
                
                // Filter out existing keys in the current block for more relevant suggestions
                const existingKeysInBlock = new Set<string>();
                try {
                    const parsedHocon = hocon.parse(text); // Parse full document
                    
                    let currentParsedScope: any = parsedHocon;
                    if (currentScopeKey !== "__root__") {
                        // Traverse the parsed HOCON object to find the relevant scope
                        const scopeKeys = currentScopeKey.split('.'); // Note: currentScopeKey will only be a single key here, not a path
                        for (const keyPart of scopeKeys) { // This loop will only run once with currentScopeKey
                            if (currentParsedScope && typeof currentParsedScope === 'object' && currentParsedScope[keyPart] !== undefined) {
                                currentParsedScope = currentParsedScope[keyPart];
                            } else {
                                currentParsedScope = null; // Path not found
                                break;
                            }
                        }
                    }

                    // Collect existing keys from the current scope
                    if (typeof currentParsedScope === 'object' && currentParsedScope !== null) {
                        for (const key in currentParsedScope) {
                            if (Object.prototype.hasOwnProperty.call(currentParsedScope, key)) {
                                existingKeysInBlock.add(key);
                            }
                        }
                    }
                } catch (e) {
                    // Ignore parse errors, we are just trying to get existing keys
                    // connection.console.warn(`Partial HOCON parse error for existing keys: ${e instanceof Error ? e.message : e}`);
                }
                
                // Get completions for the determined scope
                let completionsForScope = hoconCompletionItems[currentScopeKey] || [];
                
                // Filter completions: do not suggest keys already present in the current block
                completions = completionsForScope.filter(item => !existingKeysInBlock.has(item.label));

                // Filter completions based on what's already typed on the current line
                const lastWordMatch = linePrefix.match(/(\w+)$/);
                if (lastWordMatch) {
                    const lastWord = lastWordMatch[1];
                    completions = completions.filter(item => item.label.startsWith(lastWord));
                }
                
                return completions;
            } catch (e) {
                if (e instanceof Error) {
                    connection.console.warn(`HOCON parse error: ${e.message}`);
                } else {
                    connection.console.warn(`HOCON parse error: ${e}`);
                }
                // Fallback to root completions on parse error
                completions = hoconCompletionItems["__root__"];
            }
            return completions;
        } else {
		// Check if we're after "use "
		if (/use\s+[a-zA-Z_]*$/.test(linePrefix)) {
			// Return module names only
			return stdlibCompletions.filter(item =>
				item.kind === CompletionItemKind.Module
			);
		}

		// Check if we're typing module.something
		const moduleMatch = linePrefix.match(/([a-zA-Z_][a-zA-Z0-9_]*)\.[a-zA-Z_]*$/);
		if (moduleMatch) {
			const moduleName = moduleMatch[1];
			// Return functions/constants for this module
			return stdlibCompletions.filter(item =>
				item.label.startsWith(moduleName + '.')
			);
		}

		// Return all completions (keywords + stdlib)
		const keywordCompletions: CompletionItem[] = [
			{ label: 'val', kind: CompletionItemKind.Keyword, detail: 'Immutable variable', insertText: 'val ${1:name} = ${2:value};', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'var', kind: CompletionItemKind.Keyword, detail: 'Mutable variable', insertText: 'var ${1:name} = ${2:value};', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'fn', kind: CompletionItemKind.Keyword, detail: 'Function', insertText: 'fn ${1:name}($2) {\n\t$0\n}', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'class', kind: CompletionItemKind.Keyword, detail: 'Class definition', insertText: 'class ${1:Name} {\n\t$0\n}', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'interface', kind: CompletionItemKind.Keyword, detail: 'Interface definition' },
			{ label: 'package', kind: CompletionItemKind.Keyword, detail: 'Package declaration', insertText: 'package ${1:main};', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'use', kind: CompletionItemKind.Keyword, detail: 'Import module', insertText: 'use ${1:module};', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'if', kind: CompletionItemKind.Keyword, detail: 'If statement', insertText: 'if (${1:condition}) {\n\t$0\n}', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'else', kind: CompletionItemKind.Keyword, detail: 'Else clause' },
			{ label: 'while', kind: CompletionItemKind.Keyword, detail: 'While loop', insertText: 'while (${1:condition}) {\n\t$0\n}', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'for', kind: CompletionItemKind.Keyword, detail: 'For loop', insertText: 'for (${1:item} in ${2:items}) {\n\t$0\n}', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'when', kind: CompletionItemKind.Keyword, detail: 'Pattern matching', insertText: 'when (${1:value}) {\n\t${2:pattern} -> ${3:result}\n\telse -> ${4:default}\n}', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'return', kind: CompletionItemKind.Keyword, detail: 'Return statement' },
			{ label: 'break', kind: CompletionItemKind.Keyword, detail: 'Break from loop' },
			{ label: 'continue', kind: CompletionItemKind.Keyword, detail: 'Continue loop' },
			{ label: 'async', kind: CompletionItemKind.Keyword, detail: 'Async function' },
			{ label: 'await', kind: CompletionItemKind.Keyword, detail: 'Await expression' },
			{ label: 'try', kind: CompletionItemKind.Keyword, detail: 'Try block', insertText: 'try {\n\t${1}\n} catch (${2:error}) {\n\t${3}\n}', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'catch', kind: CompletionItemKind.Keyword, detail: 'Catch block' },
			{ label: 'throw', kind: CompletionItemKind.Keyword, detail: 'Throw error' },
			{ label: 'constructor', kind: CompletionItemKind.Keyword, detail: 'Class constructor' },
			{ label: 'this', kind: CompletionItemKind.Keyword, detail: 'Current instance' },
			{ label: 'super', kind: CompletionItemKind.Keyword, detail: 'Parent class' },
			{ label: 'true', kind: CompletionItemKind.Keyword, detail: 'Boolean true' },
			{ label: 'false', kind: CompletionItemKind.Keyword, detail: 'Boolean false' },
			{ label: 'null', kind: CompletionItemKind.Keyword, detail: 'Null value' },
			{ label: 'Optional', kind: CompletionItemKind.Class, detail: 'Optional type' },
			{ label: 'Some', kind: CompletionItemKind.Function, detail: 'Wrap value in Optional', insertText: 'Some(${1:value})', insertTextFormat: InsertTextFormat.Snippet },
			{ label: 'None', kind: CompletionItemKind.Constant, detail: 'Empty Optional' },
			{ label: 'print', kind: CompletionItemKind.Function, detail: 'Built-in print function', insertText: 'print(${1})', insertTextFormat: InsertTextFormat.Snippet },
		];

		return [...keywordCompletions, ...stdlibCompletions];
        }
	}
);

// Completion resolve
connection.onCompletionResolve(
	(item: CompletionItem): CompletionItem => {
		return item;
	}
);

// Definition provider (Ctrl+Click / Go to Definition)
connection.onDefinition(
	(params: TextDocumentPositionParams): Definition | null => {
		const document = documents.get(params.textDocument.uri);
		if (!document) {
			return null;
		}

		const text = document.getText();
		const offset = document.offsetAt(params.position);
		const line = params.position.line;
		const lineText = text.split('\n')[line];

		// Get the word at the cursor position
		const wordRegex = /[a-zA-Z_][a-zA-Z0-9_]*/g;
		let match;
		let targetWord: string | null = null;

		while ((match = wordRegex.exec(lineText)) !== null) {
			const startChar = match.index;
			const endChar = startChar + match[0].length;

			if (startChar <= params.position.character && params.position.character <= endChar) {
				targetWord = match[0];
				break;
			}
		}

		if (!targetWord) {
			return null;
		}

		// Check if it's a stdlib function call (module.function)
		const stdlibCallRegex = new RegExp(`([a-zA-Z_][a-zA-Z0-9_]*)\.${targetWord}\s*\(`);
		const stdlibMatch = lineText.match(stdlibCallRegex);

		if (stdlibMatch) {
			const moduleName = stdlibMatch[1];
			const fullName = `${moduleName}.${targetWord}`;

			// Check if it exists in stdlib
			if (stdlibFunctions.has(fullName)) {
				// For stdlib functions, we could point to documentation or show a message
				// For now, return null (could be enhanced to generate virtual docs)
				return null;
			}
		}

		// Check for user-defined function
		const funcDef = functionDefinitions.get(targetWord);
		if (funcDef) {
			return Location.create(funcDef.uri, funcDef.range);
		}

		return null;
	}
);

// Listen for document changes
documents.listen(connection);

// Listen on the connection
connection.listen();