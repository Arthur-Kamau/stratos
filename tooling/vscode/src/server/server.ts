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
	InitializeResult
} from 'vscode-languageserver/node';

import {
	TextDocument
} from 'vscode-languageserver-textdocument';

import { exec } from 'child_process';
import * as path from 'path';

// Create a connection for the server, using Node's IPC as a transport.
// Also include all preview / proposed LSP features.
const connection = createConnection(ProposedFeatures.all);

// Create a simple text document manager.
const documents: TextDocuments<TextDocument> = new TextDocuments(TextDocument);

let hasConfigurationCapability = false;
let hasWorkspaceFolderCapability = false;
let hasDiagnosticRelatedInformationCapability = false;

connection.onInitialize((params: InitializeParams) => {
	const capabilities = params.capabilities;

	// Does the client support the `workspace/configuration` request?
	// If not, we fall back using global settings.
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
			// Tell the client that this server supports code completion.
			completionProvider: {
				resolveProvider: true
			}
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
		// Register for all configuration changes.
		connection.client.register(DidChangeConfigurationNotification.type, undefined);
	}
	if (hasWorkspaceFolderCapability) {
		connection.workspace.onDidChangeWorkspaceFolders(_event => {
			connection.console.log('Workspace folder change event received.');
		});
	}
});

// The content of a text document has changed. This event is emitted
// when the text document first opened or when its content has changed.
documents.onDidChangeContent(change => {
	validateTextDocument(change.document);
});

import * as fs from 'fs';
import * as os from 'os';
import { execFile } from 'child_process';

async function validateTextDocument(textDocument: TextDocument): Promise<void> {
    const text = textDocument.getText();
    const diagnostics: Diagnostic[] = [];

    // 1. Write content to temp file
    const tempFile = path.join(os.tmpdir(), 'stratos_temp_' + Math.random().toString(36).substring(7) + '.st');
    fs.writeFileSync(tempFile, text);

    // 2. Path to compiler (Hardcoded for this environment, in a real extension this would be a setting)
    const compilerPath = 'C:\\Users\\ADMIN\\Desktop\\Development\\Projects\\stratos\\compiler\\C++\\build\\stratos.exe';

    // 3. Run compiler
    execFile(compilerPath, [tempFile], (error, stdout, stderr) => {
        // Cleanup
        if (fs.existsSync(tempFile)) fs.unlinkSync(tempFile);

        // 4. Parse output
        // Output format: [Error] Line:Col: Message
        const output = stderr.toString();
        const errorRegex = /\[Error\] (\d+):(\d+): (.*)/g;
        let match;

        while ((match = errorRegex.exec(output)) !== null) {
            const line = parseInt(match[1]) - 1; // LSP is 0-based, Compiler is 1-based usually? 
                                                 // My compiler: token.line is 1-based (from Lexer init line=1).
            const col = parseInt(match[2]) - 1;  // Compiler col is 1-based? Lexer init col=0, but advance increments. So likely 1-based.
            const msg = match[3];

            const diagnostic: Diagnostic = {
                severity: DiagnosticSeverity.Error,
                range: {
                    start: { line: line, character: col },
                    end: { line: line, character: col + 100 } // Heuristic end, or until end of line
                },
                message: msg,
                source: 'stratos-compiler'
            };
            diagnostics.push(diagnostic);
        }

        // Send diagnostics
        connection.sendDiagnostics({ uri: textDocument.uri, diagnostics });
    });
}

// The settings interface
interface StratosSettings {
	maxNumberOfProblems: number;
}

// The global settings, used when the `workspace/configuration` request is not supported by the client.
const defaultSettings: StratosSettings = { maxNumberOfProblems: 1000 };
let globalSettings: StratosSettings = defaultSettings;

// Cache the settings of all open documents
const documentSettings: Map<string, Thenable<StratosSettings>> = new Map();

connection.onDidChangeConfiguration(change => {
	if (hasConfigurationCapability) {
		// Reset all cached document settings
		documentSettings.clear();
	} else {
		globalSettings = <StratosSettings>(
			(change.settings.stratosLanguageServer || defaultSettings)
		);
	}

	// Revalidate all open text documents
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

// Only keep settings for open documents
documents.onDidClose(e => {
	documentSettings.delete(e.document.uri);
});


// This handler provides the initial list of the completion items.
connection.onCompletion(
	(_textDocumentPosition: TextDocumentPositionParams): CompletionItem[] => {
		// The pass parameter contains the position of the text document in
		// which code complete got requested. For the example we ignore this
		// info and always provide the same completion items.
		return [
			{
				label: 'val',
				kind: CompletionItemKind.Keyword,
				data: 1
			},
			{
				label: 'var',
				kind: CompletionItemKind.Keyword,
				data: 2
			},
            {
				label: 'fn',
				kind: CompletionItemKind.Keyword,
				data: 3
			},
            {
				label: 'class',
				kind: CompletionItemKind.Keyword,
				data: 4
			},
            {
				label: 'package',
				kind: CompletionItemKind.Keyword,
				data: 6
			},
            {
				label: 'Optional',
				kind: CompletionItemKind.Class,
				data: 5
			}
		];
	}
);

// This handler resolves additional information for the item selected in
// the completion list.
connection.onCompletionResolve(
	(item: CompletionItem): CompletionItem => {
		if (item.data === 1) {
			item.detail = 'Immutable variable';
			item.documentation = 'val name = value';
		} else if (item.data === 3) {
			item.detail = 'Function definition';
			item.documentation = 'fn name() { ... }';
		}
		return item;
	}
);

// Make the text document manager listen on the connection
// for open, change and close text document events
documents.listen(connection);

// Listen on the connection
connection.listen();
