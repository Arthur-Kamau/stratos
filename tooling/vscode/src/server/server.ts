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
    	// Standard library modules and their functions
	const stdlibModules = {
		'async': [
			{ name: 'Promise', type: 'class', doc: 'Class Promise' },
			{ name: 'then', type: 'function', params: 'onFulfilled: Function<T, R>', returnType: 'Promise<R>', doc: 'Function then' },
			{ name: 'catch', type: 'function', params: 'onRejected: Function<Error, T>', returnType: 'Promise<T>', doc: 'Function catch' },
			{ name: 'finally', type: 'function', params: 'onFinally: Function', returnType: 'Promise<T>', doc: 'Function finally' },
			{ name: 'Task', type: 'class', doc: 'Class Task' },
			{ name: 'await', type: 'function', params: '', returnType: 'T', doc: 'Function await' },
			{ name: 'cancel', type: 'function', params: '', returnType: 'void', doc: 'Function cancel' },
			{ name: 'isRunning', type: 'function', params: '', returnType: 'bool', doc: 'Function isRunning' },
			{ name: 'PromiseResult', type: 'class', doc: 'Class PromiseResult' },
			{ name: 'delay', type: 'function', params: 'ms: int', returnType: 'Promise<void>', doc: 'Delay and timeout' },
			{ name: 'rejectWith', type: 'function', params: 'error: Error', returnType: 'Promise<void>', doc: 'Create rejected promise' },
			{ name: 'next', type: 'const', returnType: 'any', doc: 'Constant next' },
			{ name: 'attemptFn', type: 'function', params: '', returnType: 'void', doc: 'Function attemptFn' },
			{ name: 'promises', type: 'const', returnType: 'any', doc: 'Constant promises' },
			{ name: 'errors', type: 'const', returnType: 'any', doc: 'Constant errors' },
			{ name: 'AsyncThrottle', type: 'class', doc: 'Throttle async function calls' },
			{ name: 'execute', type: 'function', params: 'arg: T', returnType: 'Promise<R>', doc: 'Function execute' },
			{ name: 'promise', type: 'const', returnType: 'any', doc: 'Constant promise' },
			{ name: 'processQueue', type: 'function', params: '', returnType: 'void', doc: 'Function processQueue' },
			{ name: 'item', type: 'const', returnType: 'int', doc: 'Constant item' },
			{ name: 'AsyncDebounce', type: 'class', doc: 'Debounce async function' },
			{ name: 'sleep', type: 'function', params: 'ms: int', returnType: 'Promise<void>', doc: 'Sleep utility' },
			{ name: 'waitUntil', type: 'function', params: 'predicate: Function<bool>, checkInterval: int, maxWait: int', returnType: 'Promise<bool>', doc: 'Wait until condition is true' },
			{ name: 'startTime', type: 'const', returnType: 'any', doc: 'Constant startTime' },
			{ name: 'check', type: 'function', params: '', returnType: 'void', doc: 'Function check' },
		],
		'package': [
			{ name: 'List', type: 'class', doc: 'Class List' },
			{ name: 'add', type: 'function', params: 'item: T', returnType: 'void', doc: 'Function add' },
			{ name: 'get', type: 'function', params: 'index: int', returnType: 'T', doc: 'Function get' },
			{ name: 'set', type: 'function', params: 'index: int, item: T', returnType: 'void', doc: 'Function set' },
			{ name: 'remove', type: 'function', params: 'index: int', returnType: 'T', doc: 'Function remove' },
			{ name: 'clear', type: 'function', params: '', returnType: 'void', doc: 'Function clear' },
			{ name: 'isEmpty', type: 'function', params: '', returnType: 'bool', doc: 'Function isEmpty' },
			{ name: 'length', type: 'function', params: '', returnType: 'int', doc: 'Function length' },
			{ name: 'map', type: 'function', params: 'fn: Function<T, R>', returnType: 'List<R>', doc: 'Higher-order functions' },
			{ name: 'filter', type: 'function', params: 'predicate: Function<T, bool>', returnType: 'List<T>', doc: 'Function filter' },
			{ name: 'reduce', type: 'function', params: 'initial: R, fn: Function<R, T, R>', returnType: 'R', doc: 'Function reduce' },
			{ name: 'forEach', type: 'function', params: 'fn: Function<T, void>', returnType: 'void', doc: 'Function forEach' },
			{ name: 'find', type: 'function', params: 'predicate: Function<T, bool>', returnType: 'T?', doc: 'Function find' },
			{ name: 'any', type: 'function', params: 'predicate: Function<T, bool>', returnType: 'bool', doc: 'Function any' },
			{ name: 'all', type: 'function', params: 'predicate: Function<T, bool>', returnType: 'bool', doc: 'Function all' },
			{ name: 'sort', type: 'function', params: 'comparator: Function<T, T, int>', returnType: 'List<T>', doc: 'Function sort' },
			{ name: 'Map', type: 'class', doc: 'Class Map' },
			{ name: 'put', type: 'function', params: 'key: K, value: V', returnType: 'void', doc: 'Function put' },
			{ name: 'containsKey', type: 'function', params: 'key: K', returnType: 'bool', doc: 'Function containsKey' },
			{ name: 'keys', type: 'function', params: '', returnType: 'List<K>', doc: 'Function keys' },
			{ name: 'values', type: 'function', params: '', returnType: 'List<V>', doc: 'Function values' },
			{ name: 'entries', type: 'function', params: '', returnType: 'List<Pair<K, V>>', doc: 'Function entries' },
			{ name: 'Set', type: 'class', doc: 'Class Set' },
			{ name: 'contains', type: 'function', params: 'item: T', returnType: 'bool', doc: 'Function contains' },
			{ name: 'union', type: 'function', params: 'other: Set<T>', returnType: 'Set<T>', doc: 'Function union' },
			{ name: 'intersection', type: 'function', params: 'other: Set<T>', returnType: 'Set<T>', doc: 'Function intersection' },
			{ name: 'difference', type: 'function', params: 'other: Set<T>', returnType: 'Set<T>', doc: 'Function difference' },
			{ name: 'Queue', type: 'class', doc: 'Class Queue' },
			{ name: 'enqueue', type: 'function', params: 'item: T', returnType: 'void', doc: 'Function enqueue' },
			{ name: 'dequeue', type: 'function', params: '', returnType: 'T?', doc: 'Function dequeue' },
			{ name: 'peek', type: 'function', params: '', returnType: 'T?', doc: 'Function peek' },
			{ name: 'Stack', type: 'class', doc: 'Class Stack' },
			{ name: 'push', type: 'function', params: 'item: T', returnType: 'void', doc: 'Function push' },
			{ name: 'pop', type: 'function', params: '', returnType: 'T?', doc: 'Function pop' },
			{ name: 'Pair', type: 'class', doc: 'Class Pair' },
			{ name: 'range', type: 'function', params: 'start: int, end: int', returnType: 'List<int>', doc: 'Range generation' },
			{ name: 'result', type: 'const', returnType: 'any', doc: 'Constant result' },
			{ name: 'minLen', type: 'const', returnType: 'any', doc: 'Constant minLen' },
			{ name: 'key', type: 'const', returnType: 'any', doc: 'Constant key' },
			{ name: 'trueList', type: 'const', returnType: 'any', doc: 'Constant trueList' },
			{ name: 'falseList', type: 'const', returnType: 'any', doc: 'Constant falseList' },
			{ name: 'count', type: 'const', returnType: 'any', doc: 'Constant count' },
			{ name: 'seen', type: 'const', returnType: 'any', doc: 'Constant seen' },
			{ name: 'PI', type: 'const', returnType: 'int', doc: 'Constants' },
			{ name: 'E', type: 'const', returnType: 'int', doc: 'Constant E' },
			{ name: 'PHI', type: 'const', returnType: 'int', doc: 'Constant PHI' },
			{ name: 'sin', type: 'function', params: 'x: double', returnType: 'double', doc: 'Trigonometric functions' },
			{ name: 'cos', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function cos' },
			{ name: 'tan', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function tan' },
			{ name: 'asin', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function asin' },
			{ name: 'acos', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function acos' },
			{ name: 'atan', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function atan' },
			{ name: 'atan2', type: 'function', params: 'y: double, x: double', returnType: 'double', doc: 'Function atan2' },
			{ name: 'sinh', type: 'function', params: 'x: double', returnType: 'double', doc: 'Hyperbolic functions' },
			{ name: 'cosh', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function cosh' },
			{ name: 'tanh', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function tanh' },
			{ name: 'exp', type: 'function', params: 'x: double', returnType: 'double', doc: 'Exponential and logarithmic' },
			{ name: 'log', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function log' },
			{ name: 'log10', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function log10' },
			{ name: 'log2', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function log2' },
			{ name: 'pow', type: 'function', params: 'x: double, y: double', returnType: 'double', doc: 'Function pow' },
			{ name: 'sqrt', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function sqrt' },
			{ name: 'cbrt', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function cbrt' },
			{ name: 'ceil', type: 'function', params: 'x: double', returnType: 'double', doc: 'Rounding' },
			{ name: 'floor', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function floor' },
			{ name: 'round', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function round' },
			{ name: 'trunc', type: 'function', params: 'x: double', returnType: 'double', doc: 'Function trunc' },
			{ name: 'abs', type: 'function', params: 'x: double', returnType: 'double', doc: 'Absolute and sign' },
			{ name: 'sign', type: 'function', params: 'x: double', returnType: 'int', doc: 'Function sign' },
			{ name: 'min', type: 'function', params: 'a: double, b: double', returnType: 'double', doc: 'Min/Max' },
			{ name: 'max', type: 'function', params: 'a: double, b: double', returnType: 'double', doc: 'Function max' },
			{ name: 'random', type: 'function', params: '', returnType: 'double', doc: 'Random (seed-based)' },
			{ name: 'randomInt', type: 'function', params: 'min: int, max: int', returnType: 'int', doc: 'Function randomInt' },
			{ name: 'randomSeed', type: 'function', params: 'seed: int', returnType: 'void', doc: 'Function randomSeed' },
			{ name: 'toRadians', type: 'function', params: 'degrees: double', returnType: 'double', doc: 'Degrees/Radians conversion' },
			{ name: 'toDegrees', type: 'function', params: 'radians: double', returnType: 'double', doc: 'Function toDegrees' },
			{ name: 'clamp', type: 'function', params: 'value: double, min_val: double, max_val: double', returnType: 'double', doc: 'Clamp value between min and max' },
			{ name: 'lerp', type: 'function', params: 'a: double, b: double, t: double', returnType: 'double', doc: 'Linear interpolation' },
			{ name: 'isEven', type: 'function', params: 'n: int', returnType: 'bool', doc: 'Check if number is even' },
			{ name: 'isOdd', type: 'function', params: 'n: int', returnType: 'bool', doc: 'Check if number is odd' },
			{ name: 'factorial', type: 'function', params: 'n: int', returnType: 'int', doc: 'Factorial (recursive)' },
			{ name: 'gcd', type: 'function', params: 'a: int, b: int', returnType: 'int', doc: 'Greatest common divisor' },
			{ name: 'temp', type: 'const', returnType: 'any', doc: 'Constant temp' },
			{ name: 'lcm', type: 'function', params: 'a: int, b: int', returnType: 'int', doc: 'Least common multiple' },
			{ name: 'isPrime', type: 'function', params: 'n: int', returnType: 'bool', doc: 'Check if number is prime' },
			{ name: 'sum', type: 'function', params: 'numbers: Array<double>', returnType: 'double', doc: 'Sum of array' },
			{ name: 'average', type: 'function', params: 'numbers: Array<double>', returnType: 'double', doc: 'Average of array' },
			{ name: 'median', type: 'function', params: 'numbers: Array<double>', returnType: 'double', doc: 'Median of array' },
			{ name: 'sorted', type: 'const', returnType: 'any', doc: 'Constant sorted' },
			{ name: 'len', type: 'const', returnType: 'any', doc: 'Constant len' },
			{ name: 'Conn', type: 'class', doc: 'Class Conn' },
			{ name: 'read', type: 'function', params: 'buffer: Array<byte>', returnType: 'int', doc: 'Function read' },
			{ name: 'write', type: 'function', params: 'data: Array<byte>', returnType: 'int', doc: 'Function write' },
			{ name: 'readString', type: 'function', params: 'maxBytes: int', returnType: 'string', doc: 'Function readString' },
			{ name: 'writeString', type: 'function', params: 'data: string', returnType: 'int', doc: 'Function writeString' },
			{ name: 'close', type: 'function', params: '', returnType: 'void', doc: 'Function close' },
			{ name: 'setTimeout', type: 'function', params: 'ms: int', returnType: 'void', doc: 'Function setTimeout' },
			{ name: 'Listener', type: 'class', doc: 'Class Listener' },
			{ name: 'accept', type: 'function', params: '', returnType: 'Result<Conn, Error>', doc: 'Function accept' },
			{ name: 'UDPConn', type: 'class', doc: 'Class UDPConn' },
			{ name: 'readFrom', type: 'function', params: 'buffer: Array<byte>', returnType: 'Pair<int, string>', doc: 'Function readFrom' },
			{ name: 'writeTo', type: 'function', params: 'data: Array<byte>, addr: string, port: int', returnType: 'int', doc: 'Function writeTo' },
			{ name: 'IPAddr', type: 'class', doc: 'Class IPAddr' },
			{ name: 'toString', type: 'function', params: '', returnType: 'string', doc: 'Function toString' },
			{ name: 'isIPv4', type: 'function', params: '', returnType: 'bool', doc: 'Function isIPv4' },
			{ name: 'isIPv6', type: 'function', params: '', returnType: 'bool', doc: 'Function isIPv6' },
			{ name: 'dial', type: 'function', params: 'network: string, address: string', returnType: 'Result<Conn, Error>', doc: 'TCP' },
			{ name: 'dialTCP', type: 'function', params: 'addr: string, port: int', returnType: 'Result<Conn, Error>', doc: 'Function dialTCP' },
			{ name: 'listen', type: 'function', params: 'network: string, address: string', returnType: 'Result<Listener, Error>', doc: 'Function listen' },
			{ name: 'listenTCP', type: 'function', params: 'addr: string, port: int', returnType: 'Result<Listener, Error>', doc: 'Function listenTCP' },
			{ name: 'dialUDP', type: 'function', params: 'addr: string, port: int', returnType: 'Result<UDPConn, Error>', doc: 'UDP' },
			{ name: 'listenUDP', type: 'function', params: 'addr: string, port: int', returnType: 'Result<UDPConn, Error>', doc: 'Function listenUDP' },
			{ name: 'parseIP', type: 'function', params: 's: string', returnType: 'Result<IPAddr, Error>', doc: 'Address parsing' },
			{ name: 'resolveTCPAddr', type: 'function', params: 'addr: string', returnType: 'Result<IPAddr, Error>', doc: 'Function resolveTCPAddr' },
			{ name: 'resolveUDPAddr', type: 'function', params: 'addr: string', returnType: 'Result<IPAddr, Error>', doc: 'Function resolveUDPAddr' },
			{ name: 'lookupHost', type: 'function', params: 'host: string', returnType: 'Result<Array<string>, Error>', doc: 'DNS' },
			{ name: 'lookupIP', type: 'function', params: 'host: string', returnType: 'Result<Array<IPAddr>, Error>', doc: 'Function lookupIP' },
			{ name: 'TCPServer', type: 'class', doc: 'Simple TCP server' },
			{ name: 'listenerResult', type: 'const', returnType: 'any', doc: 'Constant listenerResult' },
			{ name: 'start', type: 'function', params: '', returnType: 'void', doc: 'Function start' },
			{ name: 'connResult', type: 'const', returnType: 'any', doc: 'Constant connResult' },
			{ name: 'conn', type: 'const', returnType: 'any', doc: 'Constant conn' },
			{ name: 'stop', type: 'function', params: '', returnType: 'void', doc: 'Function stop' },
			{ name: 'newTCPServer', type: 'function', params: 'addr: string, port: int, handler: Function<Conn, void>', returnType: 'TCPServer', doc: 'Function newTCPServer' },
			{ name: 'connectTCP', type: 'function', params: 'addr: string, port: int', returnType: 'Result<Conn, Error>', doc: 'Simple TCP client' },
			{ name: 'readLine', type: 'function', params: 'conn: Conn', returnType: 'string', doc: 'Read line from connection' },
			{ name: 'n', type: 'const', returnType: 'any', doc: 'Constant n' },
			{ name: 'char', type: 'const', returnType: 'int', doc: 'Constant char' },
			{ name: 'writeLine', type: 'function', params: 'conn: Conn, line: string', returnType: 'int', doc: 'Write line to connection' },
			{ name: 'copy', type: 'function', params: 'dst: Conn, src: Conn', returnType: 'int', doc: 'Copy data from one connection to another' },
			{ name: 'pipe', type: 'function', params: 'conn1: Conn, conn2: Conn', returnType: 'void', doc: 'Pipe two connections (bidirectional)' },
			{ name: 'wg', type: 'const', returnType: 'any', doc: 'Constant wg' },
			{ name: 'UDPServer', type: 'class', doc: 'UDP echo server' },
			{ name: 'bytesRead', type: 'const', returnType: 'any', doc: 'Constant bytesRead' },
			{ name: 'clientAddr', type: 'const', returnType: 'any', doc: 'Constant clientAddr' },
			{ name: 'response', type: 'const', returnType: 'any', doc: 'Constant response' },
			{ name: 'newUDPServer', type: 'function', params: 'addr: string, port: int, handler: Function<Array<byte>, string, int, Array<byte>>', returnType: 'UDPServer', doc: 'Function newUDPServer' },
			{ name: 'isReachable', type: 'function', params: 'host: string, port: int, timeoutMs: int', returnType: 'bool', doc: 'Check if host is reachable' },
			{ name: 'getLocalIPs', type: 'function', params: '', returnType: 'Array<string>', doc: 'Get local IP addresses' },
			{ name: 'hostname', type: 'const', returnType: 'any', doc: 'Constant hostname' },
			{ name: 'ipsResult', type: 'const', returnType: 'any', doc: 'Constant ipsResult' },
			{ name: 'Time', type: 'class', doc: 'Class Time' },
			{ name: 'year', type: 'function', params: '', returnType: 'int', doc: 'Function year' },
			{ name: 'month', type: 'function', params: '', returnType: 'int', doc: 'Function month' },
			{ name: 'day', type: 'function', params: '', returnType: 'int', doc: 'Function day' },
			{ name: 'hour', type: 'function', params: '', returnType: 'int', doc: 'Function hour' },
			{ name: 'minute', type: 'function', params: '', returnType: 'int', doc: 'Function minute' },
			{ name: 'second', type: 'function', params: '', returnType: 'int', doc: 'Function second' },
			{ name: 'millisecond', type: 'function', params: '', returnType: 'int', doc: 'Function millisecond' },
			{ name: 'format', type: 'function', params: 'layout: string', returnType: 'string', doc: 'Function format' },
			{ name: 'sub', type: 'function', params: 'other: Time', returnType: 'Duration', doc: 'Function sub' },
			{ name: 'before', type: 'function', params: 'other: Time', returnType: 'bool', doc: 'Function before' },
			{ name: 'after', type: 'function', params: 'other: Time', returnType: 'bool', doc: 'Function after' },
			{ name: 'equals', type: 'function', params: 'other: Time', returnType: 'bool', doc: 'Function equals' },
			{ name: 'unix', type: 'function', params: '', returnType: 'int', doc: 'Function unix' },
			{ name: 'Duration', type: 'class', doc: 'Class Duration' },
			{ name: 'getHours', type: 'function', params: '', returnType: 'double', doc: 'Function getHours' },
			{ name: 'getMinutes', type: 'function', params: '', returnType: 'double', doc: 'Function getMinutes' },
			{ name: 'getSeconds', type: 'function', params: '', returnType: 'double', doc: 'Function getSeconds' },
			{ name: 'getMilliseconds', type: 'function', params: '', returnType: 'int', doc: 'Function getMilliseconds' },
			{ name: 'Ticker', type: 'class', doc: 'Class Ticker' },
			{ name: 'Timer', type: 'class', doc: 'Class Timer' },
			{ name: 'reset', type: 'function', params: 'duration: Duration', returnType: 'void', doc: 'Function reset' },
			{ name: 'now', type: 'function', params: '', returnType: 'Time', doc: 'Current time' },
			{ name: 'fromMillis', type: 'function', params: 'millis: int', returnType: 'Time', doc: 'Function fromMillis' },
			{ name: 'parse', type: 'function', params: 'value: string, layout: string', returnType: 'Result<Time, Error>', doc: 'Parsing and formatting' },
			{ name: 'milliseconds', type: 'function', params: 'ms: int', returnType: 'Duration', doc: 'Duration creation' },
			{ name: 'seconds', type: 'function', params: 's: int', returnType: 'Duration', doc: 'Function seconds' },
			{ name: 'minutes', type: 'function', params: 'm: int', returnType: 'Duration', doc: 'Function minutes' },
			{ name: 'hours', type: 'function', params: 'h: int', returnType: 'Duration', doc: 'Function hours' },
			{ name: 'days', type: 'function', params: 'd: int', returnType: 'Duration', doc: 'Function days' },
			{ name: 'sleep', type: 'function', params: 'duration: Duration', returnType: 'void', doc: 'Sleep and timing' },
			{ name: 'sleepUntil', type: 'function', params: 'time: Time', returnType: 'void', doc: 'Function sleepUntil' },
			{ name: 'tick', type: 'function', params: 'interval: Duration', returnType: 'Ticker', doc: 'Function tick' },
			{ name: 'newTimer', type: 'function', params: 'duration: Duration', returnType: 'Timer', doc: 'Function newTimer' },
			{ name: 'newTicker', type: 'function', params: 'interval: Duration', returnType: 'Ticker', doc: 'Function newTicker' },
			{ name: 'date', type: 'function', params: 'year: int, month: int, day: int', returnType: 'Time', doc: 'Date operations' },
			{ name: 'dateTime', type: 'function', params: 'year: int, month: int, day: int, hour: int, minute: int, second: int', returnType: 'Time', doc: 'Function dateTime' },
			{ name: 'ANSIC', type: 'const', returnType: 'string', doc: 'Layout formats (Go-style)' },
			{ name: 'UnixDate', type: 'const', returnType: 'string', doc: 'Constant UnixDate' },
			{ name: 'RFC3339', type: 'const', returnType: 'string', doc: 'Constant RFC3339' },
			{ name: 'Kitchen', type: 'const', returnType: 'string', doc: 'Constant Kitchen' },
			{ name: 'DateTime', type: 'const', returnType: 'string', doc: 'Constant DateTime' },
			{ name: 'DateOnly', type: 'const', returnType: 'string', doc: 'Constant DateOnly' },
			{ name: 'TimeOnly', type: 'const', returnType: 'string', doc: 'Constant TimeOnly' },
			{ name: 'isLeapYear', type: 'function', params: 'year: int', returnType: 'bool', doc: 'Check if year is leap year' },
			{ name: 'daysInMonth', type: 'function', params: 'year: int, month: int', returnType: 'int', doc: 'Days in month' },
			{ name: 'days30', type: 'const', returnType: 'int', doc: 'Constant days30' },
			{ name: 'addDays', type: 'function', params: 't: Time, numDays: int', returnType: 'Time', doc: 'Add days to time' },
			{ name: 'addWeeks', type: 'function', params: 't: Time, weeks: int', returnType: 'Time', doc: 'Add weeks to time' },
			{ name: 'addMonths', type: 'function', params: 't: Time, months: int', returnType: 'Time', doc: 'Add months to time (approximate)' },
			{ name: 'addYears', type: 'function', params: 't: Time, years: int', returnType: 'Time', doc: 'Add years to time (approximate)' },
			{ name: 'startOfDay', type: 'function', params: 't: Time', returnType: 'Time', doc: 'Start of day' },
			{ name: 'endOfDay', type: 'function', params: 't: Time', returnType: 'Time', doc: 'End of day' },
			{ name: 'startOfMonth', type: 'function', params: 't: Time', returnType: 'Time', doc: 'Start of month' },
			{ name: 'endOfMonth', type: 'function', params: 't: Time', returnType: 'Time', doc: 'End of month' },
			{ name: 'lastDay', type: 'const', returnType: 'any', doc: 'Constant lastDay' },
			{ name: 'ageInYears', type: 'function', params: 'birthDate: Time', returnType: 'int', doc: 'Age in years' },
			{ name: 'daysBetween', type: 'function', params: 'start: Time, end: Time', returnType: 'int', doc: 'Duration between two times in days' },
			{ name: 'diff', type: 'const', returnType: 'any', doc: 'Constant diff' },
			{ name: 'humanizeDuration', type: 'function', params: 'd: Duration', returnType: 'string', doc: 'Format duration as human-readable' },
			{ name: 'totalSeconds', type: 'const', returnType: 'any', doc: 'Constant totalSeconds' },
			{ name: 'totalMinutes', type: 'const', returnType: 'any', doc: 'Constant totalMinutes' },
			{ name: 'totalHours', type: 'const', returnType: 'any', doc: 'Constant totalHours' },
			{ name: 'totalDays', type: 'const', returnType: 'int', doc: 'Constant totalDays' },
			{ name: 'measure', type: 'function', params: 'callback: Function', returnType: 'Duration', doc: 'Measure function execution time' },
			{ name: 'end', type: 'const', returnType: 'any', doc: 'Constant end' },
			{ name: 'retry', type: 'function', params: 'callback: Function<Result>, timeout: Duration, interval: Duration', returnType: 'Result', doc: 'Retry with timeout' },
			{ name: 'deadline', type: 'const', returnType: 'any', doc: 'Constant deadline' },
			{ name: 'compress', type: 'function', params: 'data: string', returnType: 'Array<byte>', doc: 'Compression' },
			{ name: 'decompress', type: 'function', params: 'compressed: Array<byte>', returnType: 'string', doc: 'Function decompress' },
			{ name: 'compressFile', type: 'function', params: 'inputPath: string, outputPath: string', returnType: 'Result<void, Error>', doc: 'File compression' },
			{ name: 'decompressFile', type: 'function', params: 'inputPath: string, outputPath: string', returnType: 'Result<void, Error>', doc: 'Function decompressFile' },
			{ name: 'createArchive', type: 'function', params: 'archivePath: string, files: Array<string>', returnType: 'Result<void, Error>', doc: 'Archive operations' },
			{ name: 'extractArchive', type: 'function', params: 'archivePath: string, destPath: string', returnType: 'Result<void, Error>', doc: 'Function extractArchive' },
			{ name: 'listArchive', type: 'function', params: 'archivePath: string', returnType: 'Result<Array<string>, Error>', doc: 'Function listArchive' },
			{ name: 'addToArchive', type: 'function', params: 'archivePath: string, filePath: string', returnType: 'Result<void, Error>', doc: 'Add/remove files from archive' },
			{ name: 'removeFromArchive', type: 'function', params: 'archivePath: string, fileName: string', returnType: 'Result<void, Error>', doc: 'Function removeFromArchive' },
			{ name: 'compressString', type: 'function', params: 'data: string, outputPath: string', returnType: 'Result<void, Error>', doc: 'Compress string to file' },
			{ name: 'compressed', type: 'const', returnType: 'any', doc: 'Constant compressed' },
			{ name: 'decompressToString', type: 'function', params: 'inputPath: string', returnType: 'Result<string, Error>', doc: 'Decompress file to string' },
			{ name: 'bytesResult', type: 'const', returnType: 'any', doc: 'Constant bytesResult' },
			{ name: 'decompressed', type: 'const', returnType: 'any', doc: 'Constant decompressed' },
			{ name: 'compressDirectory', type: 'function', params: 'dirPath: string, archivePath: string', returnType: 'Result<void, Error>', doc: 'Compress directory to archive' },
			{ name: 'filesResult', type: 'const', returnType: 'any', doc: 'Constant filesResult' },
			{ name: 'extractToDirectory', type: 'function', params: 'archivePath: string, destPath: string', returnType: 'Result<void, Error>', doc: 'Extract archive to directory' },
			{ name: 'ensureResult', type: 'const', returnType: 'any', doc: 'Ensure destination exists' },
			{ name: 'isCompressed', type: 'function', params: 'filePath: string', returnType: 'bool', doc: 'Check if file is compressed' },
			{ name: 'ext', type: 'const', returnType: 'any', doc: 'Constant ext' },
			{ name: 'getCompressionRatio', type: 'function', params: 'originalSize: int, compressedSize: int', returnType: 'double', doc: 'Get compression ratio' },
		],
		'concurrent': [
			{ name: 'Channel', type: 'class', doc: 'Class Channel' },
			{ name: 'send', type: 'function', params: 'value: T', returnType: 'bool', doc: 'Function send' },
			{ name: 'receive', type: 'function', params: '', returnType: 'T?', doc: 'Function receive' },
			{ name: 'tryReceive', type: 'function', params: '', returnType: 'Pair<T?, bool>', doc: 'Function tryReceive' },
			{ name: 'close', type: 'function', params: '', returnType: 'void', doc: 'Function close' },
			{ name: 'isClosed', type: 'function', params: '', returnType: 'bool', doc: 'Function isClosed' },
			{ name: 'Mutex', type: 'class', doc: 'Class Mutex' },
			{ name: 'lock', type: 'function', params: '', returnType: 'void', doc: 'Function lock' },
			{ name: 'unlock', type: 'function', params: '', returnType: 'void', doc: 'Function unlock' },
			{ name: 'tryLock', type: 'function', params: '', returnType: 'bool', doc: 'Function tryLock' },
			{ name: 'RWMutex', type: 'class', doc: 'Class RWMutex' },
			{ name: 'rLock', type: 'function', params: '', returnType: 'void', doc: 'Function rLock' },
			{ name: 'rUnlock', type: 'function', params: '', returnType: 'void', doc: 'Function rUnlock' },
			{ name: 'WaitGroup', type: 'class', doc: 'Class WaitGroup' },
			{ name: 'add', type: 'function', params: 'delta: int', returnType: 'void', doc: 'Function add' },
			{ name: 'done', type: 'function', params: '', returnType: 'void', doc: 'Function done' },
			{ name: 'wait', type: 'function', params: '', returnType: 'void', doc: 'Function wait' },
			{ name: 'Once', type: 'class', doc: 'Class Once' },
			{ name: 'do', type: 'function', params: 'fn: Function', returnType: 'void', doc: 'Function do' },
			{ name: 'Semaphore', type: 'class', doc: 'Class Semaphore' },
			{ name: 'acquire', type: 'function', params: '', returnType: 'void', doc: 'Function acquire' },
			{ name: 'release', type: 'function', params: '', returnType: 'void', doc: 'Function release' },
			{ name: 'tryAcquire', type: 'function', params: '', returnType: 'bool', doc: 'Function tryAcquire' },
			{ name: 'Future', type: 'class', doc: 'Class Future' },
			{ name: 'get', type: 'function', params: '', returnType: 'T', doc: 'Function get' },
			{ name: 'getWithTimeout', type: 'function', params: 'timeout: Duration', returnType: 'Result<T, Error>', doc: 'Function getWithTimeout' },
			{ name: 'isDone', type: 'function', params: '', returnType: 'bool', doc: 'Function isDone' },
			{ name: 'cancel', type: 'function', params: '', returnType: 'void', doc: 'Function cancel' },
			{ name: 'go', type: 'function', params: 'fn: Function', returnType: 'void', doc: 'Goroutine spawning' },
			{ name: 'goWithArgs', type: 'function', params: 'fn: Function, args: Array<any>', returnType: 'void', doc: 'Function goWithArgs' },
			{ name: 'select', type: 'function', params: 'cases: Array<SelectCase>', returnType: 'int', doc: 'Select statement (for channel operations)' },
			{ name: 'SelectCase', type: 'class', doc: 'Class SelectCase' },
			{ name: 'newMutex', type: 'function', params: '', returnType: 'Mutex', doc: 'Mutex creation' },
			{ name: 'newRWMutex', type: 'function', params: '', returnType: 'RWMutex', doc: 'Function newRWMutex' },
			{ name: 'newWaitGroup', type: 'function', params: '', returnType: 'WaitGroup', doc: 'Synchronization primitives' },
			{ name: 'newOnce', type: 'function', params: '', returnType: 'Once', doc: 'Function newOnce' },
			{ name: 'newSemaphore', type: 'function', params: 'capacity: int', returnType: 'Semaphore', doc: 'Function newSemaphore' },
			{ name: 'getThreadId', type: 'function', params: '', returnType: 'int', doc: 'Thread info' },
			{ name: 'getThreadCount', type: 'function', params: '', returnType: 'int', doc: 'Function getThreadCount' },
			{ name: 'setMaxThreads', type: 'function', params: 'count: int', returnType: 'void', doc: 'Function setMaxThreads' },
			{ name: 'atomicAdd', type: 'function', params: 'ptr: int*, value: int', returnType: 'int', doc: 'Atomic operations' },
			{ name: 'atomicCompareAndSwap', type: 'function', params: 'ptr: int*, old: int, new: int', returnType: 'bool', doc: 'Function atomicCompareAndSwap' },
			{ name: 'atomicLoad', type: 'function', params: 'ptr: int*', returnType: 'int', doc: 'Function atomicLoad' },
			{ name: 'atomicStore', type: 'function', params: 'ptr: int*, value: int', returnType: 'void', doc: 'Function atomicStore' },
			{ name: 'future', type: 'const', returnType: 'any', doc: 'Constant future' },
			{ name: 'result', type: 'const', returnType: 'any', doc: 'Constant result' },
			{ name: 'results', type: 'const', returnType: 'any', doc: 'Constant results' },
			{ name: 'wg', type: 'const', returnType: 'any', doc: 'Constant wg' },
			{ name: 'index', type: 'const', returnType: 'any', doc: 'Constant index' },
			{ name: 'output', type: 'const', returnType: 'int', doc: 'Constant output' },
			{ name: 'stageInput', type: 'const', returnType: 'any', doc: 'Constant stageInput' },
			{ name: 'value', type: 'const', returnType: 'any', doc: 'Constant value' },
			{ name: 'WorkerPool', type: 'class', doc: 'Worker pool pattern' },
			{ name: 'job', type: 'const', returnType: 'any', doc: 'Constant job' },
			{ name: 'submit', type: 'function', params: 'job: T', returnType: 'void', doc: 'Function submit' },
			{ name: 'RateLimiter', type: 'class', doc: 'Rate limiter' },
			{ name: 'newRateLimiter', type: 'function', params: 'requestsPerSecond: int', returnType: 'RateLimiter', doc: 'Function newRateLimiter' },
			{ name: 'timedOut', type: 'const', returnType: 'any', doc: 'Constant timedOut' },
			{ name: 'timer', type: 'const', returnType: 'any', doc: 'Constant timer' },
		],
		'convert': [
			{ name: 'toInt', type: 'function', params: 'value: any', returnType: 'int', doc: 'Works with: string, double, bool' },
			{ name: 'toDouble', type: 'function', params: 'value: any', returnType: 'double', doc: 'Works with: string, int, bool' },
			{ name: 'toString', type: 'function', params: 'value: any', returnType: 'string', doc: 'Works with: int, double, bool, char' },
			{ name: 'toBool', type: 'function', params: 'value: any', returnType: 'bool', doc: 'Works with: int (0=false, other=true), double, string ("true"/"1"/"yes"=true)' },
			{ name: 'toFixed', type: 'function', params: 'value: double, decimals: int', returnType: 'string', doc: 'Example: toFixed(3.14159, 2) => "3.14"' },
			{ name: 'toPrecision', type: 'function', params: 'value: double, precision: int', returnType: 'string', doc: 'Example: toPrecision(123.456, 4) => "123.5"' },
			{ name: 'toExponential', type: 'function', params: 'value: double, decimals: int', returnType: 'string', doc: 'Example: toExponential(1234.5, 2) => "1.23e+03"' },
			{ name: 'toHex', type: 'function', params: 'value: int', returnType: 'string', doc: 'Example: toHex(255) => "0xff"' },
			{ name: 'toOct', type: 'function', params: 'value: int', returnType: 'string', doc: 'Example: toOct(8) => "010"' },
			{ name: 'toBinary', type: 'function', params: 'value: int', returnType: 'string', doc: 'Example: toBinary(5) => "0b101"' },
			{ name: 'parseHex', type: 'function', params: 'value: string', returnType: 'int', doc: 'Example: parseHex("FF") => 255' },
			{ name: 'parseOct', type: 'function', params: 'value: string', returnType: 'int', doc: 'Example: parseOct("10") => 8' },
			{ name: 'parseBinary', type: 'function', params: 'value: string', returnType: 'int', doc: 'Example: parseBinary("101") => 5' },
		],
		'and': [
			{ name: 'Hash', type: 'class', doc: 'Class Hash' },
			{ name: 'update', type: 'function', params: 'data: Array<byte>', returnType: 'Hash', doc: 'Function update' },
			{ name: 'updateString', type: 'function', params: 's: string', returnType: 'Hash', doc: 'Function updateString' },
			{ name: 'digest', type: 'function', params: '', returnType: 'Array<byte>', doc: 'Function digest' },
			{ name: 'digestHex', type: 'function', params: '', returnType: 'string', doc: 'Function digestHex' },
			{ name: 'digestBase64', type: 'function', params: '', returnType: 'string', doc: 'Function digestBase64' },
			{ name: 'Cipher', type: 'class', doc: 'Class Cipher' },
			{ name: 'encrypt', type: 'function', params: 'plaintext: Array<byte>', returnType: 'Array<byte>', doc: 'Function encrypt' },
			{ name: 'decrypt', type: 'function', params: 'ciphertext: Array<byte>', returnType: 'Array<byte>', doc: 'Function decrypt' },
			{ name: 'encryptString', type: 'function', params: 'plaintext: string', returnType: 'string', doc: 'Function encryptString' },
			{ name: 'decryptString', type: 'function', params: 'ciphertext: string', returnType: 'string', doc: 'Function decryptString' },
			{ name: 'HMAC', type: 'class', doc: 'Class HMAC' },
			{ name: 'KeyPair', type: 'class', doc: 'Class KeyPair' },
			{ name: 'md5', type: 'function', params: 'data: Array<byte>', returnType: 'Array<byte>', doc: 'Hashing' },
			{ name: 'sha1', type: 'function', params: 'data: Array<byte>', returnType: 'Array<byte>', doc: 'Function sha1' },
			{ name: 'sha256', type: 'function', params: 'data: Array<byte>', returnType: 'Array<byte>', doc: 'Function sha256' },
			{ name: 'sha512', type: 'function', params: 'data: Array<byte>', returnType: 'Array<byte>', doc: 'Function sha512' },
			{ name: 'md5String', type: 'function', params: 's: string', returnType: 'string', doc: 'String hashing (returns hex)' },
			{ name: 'sha1String', type: 'function', params: 's: string', returnType: 'string', doc: 'Function sha1String' },
			{ name: 'sha256String', type: 'function', params: 's: string', returnType: 'string', doc: 'Function sha256String' },
			{ name: 'sha512String', type: 'function', params: 's: string', returnType: 'string', doc: 'Function sha512String' },
			{ name: 'newHash', type: 'function', params: 'algorithm: string', returnType: 'Hash', doc: 'Hash creation' },
			{ name: 'newHMAC', type: 'function', params: 'algorithm: string, key: Array<byte>', returnType: 'HMAC', doc: 'Function newHMAC' },
			{ name: 'aesEncrypt', type: 'function', params: 'plaintext: Array<byte>, key: Array<byte>, iv: Array<byte>', returnType: 'Array<byte>', doc: 'Encryption/Decryption' },
			{ name: 'aesDecrypt', type: 'function', params: 'ciphertext: Array<byte>, key: Array<byte>, iv: Array<byte>', returnType: 'Array<byte>', doc: 'Function aesDecrypt' },
			{ name: 'newCipher', type: 'function', params: 'algorithm: string, key: Array<byte>, iv: Array<byte>', returnType: 'Cipher', doc: 'Cipher creation' },
			{ name: 'randomBytes', type: 'function', params: 'length: int', returnType: 'Array<byte>', doc: 'Random generation' },
			{ name: 'randomHex', type: 'function', params: 'length: int', returnType: 'string', doc: 'Function randomHex' },
			{ name: 'randomBase64', type: 'function', params: 'length: int', returnType: 'string', doc: 'Function randomBase64' },
			{ name: 'pbkdf2', type: 'function', params: 'password: string, salt: Array<byte>, iterations: int, keyLength: int, algorithm: string', returnType: 'Array<byte>', doc: 'Key derivation' },
			{ name: 'bcrypt', type: 'function', params: 'password: string, rounds: int', returnType: 'string', doc: 'Function bcrypt' },
			{ name: 'bcryptVerify', type: 'function', params: 'password: string, hash: string', returnType: 'bool', doc: 'Function bcryptVerify' },
			{ name: 'generateRSAKeyPair', type: 'function', params: 'bits: int', returnType: 'KeyPair', doc: 'RSA' },
			{ name: 'rsaEncrypt', type: 'function', params: 'plaintext: Array<byte>, publicKey: Array<byte>', returnType: 'Array<byte>', doc: 'Function rsaEncrypt' },
			{ name: 'rsaDecrypt', type: 'function', params: 'ciphertext: Array<byte>, privateKey: Array<byte>', returnType: 'Array<byte>', doc: 'Function rsaDecrypt' },
			{ name: 'rsaSign', type: 'function', params: 'data: Array<byte>, privateKey: Array<byte>', returnType: 'Array<byte>', doc: 'Function rsaSign' },
			{ name: 'rsaVerify', type: 'function', params: 'data: Array<byte>, signature: Array<byte>, publicKey: Array<byte>', returnType: 'bool', doc: 'Function rsaVerify' },
			{ name: 'quickMD5', type: 'function', params: 's: string', returnType: 'string', doc: 'Quick hash functions' },
			{ name: 'quickSHA256', type: 'function', params: 's: string', returnType: 'string', doc: 'Function quickSHA256' },
			{ name: 'hashFile', type: 'function', params: 'path: string, algorithm: string', returnType: 'Result<string, Error>', doc: 'Hash file' },
			{ name: 'content', type: 'const', returnType: 'any', doc: 'Constant content' },
			{ name: 'hash', type: 'const', returnType: 'any', doc: 'Constant hash' },
			{ name: 'verifyChecksum', type: 'function', params: 'path: string, expectedHash: string, algorithm: string', returnType: 'Result<bool, Error>', doc: 'Checksum verification' },
			{ name: 'computed', type: 'const', returnType: 'any', doc: 'Constant computed' },
			{ name: 'match', type: 'const', returnType: 'any', doc: 'Constant match' },
			{ name: 'hashPassword', type: 'function', params: 'password: string', returnType: 'string', doc: 'Password hashing' },
			{ name: 'verifyPassword', type: 'function', params: 'password: string, hash: string', returnType: 'bool', doc: 'Function verifyPassword' },
			{ name: 'generateToken', type: 'function', params: 'length: int', returnType: 'string', doc: 'Generate secure token' },
			{ name: 'byteToHex', type: 'function', params: 'b: int', returnType: 'string', doc: 'Helper function to convert byte to hex' },
			{ name: 'hexChars', type: 'const', returnType: 'string', doc: 'Constant hexChars' },
			{ name: 'high', type: 'const', returnType: 'int', doc: 'Constant high' },
			{ name: 'low', type: 'const', returnType: 'int', doc: 'Constant low' },
			{ name: 'uuid', type: 'function', params: '', returnType: 'string', doc: 'Generate UUID v4' },
			{ name: 'bytes', type: 'const', returnType: 'int', doc: 'Constant bytes' },
			{ name: 'secureCompare', type: 'function', params: 'a: string, b: string', returnType: 'bool', doc: 'Constant-time comparison' },
			{ name: 'AuthToken', type: 'class', doc: 'HMAC-based authentication' },
			{ name: 'sign', type: 'function', params: 'message: string', returnType: 'string', doc: 'Function sign' },
			{ name: 'hmac', type: 'const', returnType: 'any', doc: 'Constant hmac' },
			{ name: 'verify', type: 'function', params: 'message: string, signature: string', returnType: 'bool', doc: 'Function verify' },
			{ name: 'expected', type: 'const', returnType: 'any', doc: 'Constant expected' },
			{ name: 'newAuthToken', type: 'function', params: 'secret: string', returnType: 'AuthToken', doc: 'Function newAuthToken' },
			{ name: 'JWTToken', type: 'class', doc: 'JWT-like token generation (simplified)' },
			{ name: 'encode', type: 'function', params: 'payload: json.JsonValue', returnType: 'string', doc: 'Function encode' },
			{ name: 'header', type: 'const', returnType: 'any', doc: 'Header' },
			{ name: 'headerJson', type: 'const', returnType: 'any', doc: 'Constant headerJson' },
			{ name: 'payloadJson', type: 'const', returnType: 'any', doc: 'Constant payloadJson' },
			{ name: 'headerB64', type: 'const', returnType: 'int', doc: 'Base64 encode' },
			{ name: 'payloadB64', type: 'const', returnType: 'int', doc: 'Constant payloadB64' },
			{ name: 'message', type: 'const', returnType: 'string', doc: 'Create signature' },
			{ name: 'signature', type: 'const', returnType: 'int', doc: 'Constant signature' },
			{ name: 'parts', type: 'const', returnType: 'string', doc: 'Constant parts' },
			{ name: 'expectedSig', type: 'const', returnType: 'int', doc: 'Constant expectedSig' },
			{ name: 'newJWT', type: 'function', params: 'secret: string', returnType: 'JWTToken', doc: 'Function newJWT' },
			{ name: 'salt', type: 'const', returnType: 'int', doc: 'Generate salt and IV' },
			{ name: 'iv', type: 'const', returnType: 'int', doc: 'Constant iv' },
			{ name: 'key', type: 'const', returnType: 'string', doc: 'Derive key from password' },
			{ name: 'cipher', type: 'const', returnType: 'string', doc: 'Encrypt' },
			{ name: 'ciphertext', type: 'const', returnType: 'any', doc: 'Constant ciphertext' },
			{ name: 'saltResult', type: 'const', returnType: 'int', doc: 'Extract salt, IV, and ciphertext' },
			{ name: 'ivResult', type: 'const', returnType: 'int', doc: 'Constant ivResult' },
			{ name: 'plaintext', type: 'const', returnType: 'int', doc: 'Constant plaintext' },
			{ name: 'encryptFile', type: 'function', params: 'inputPath: string, outputPath: string, password: string', returnType: 'Result<void, Error>', doc: 'File encryption' },
			{ name: 'encrypted', type: 'const', returnType: 'any', doc: 'Constant encrypted' },
			{ name: 'decryptFile', type: 'function', params: 'inputPath: string, outputPath: string, password: string', returnType: 'Result<void, Error>', doc: 'Function decryptFile' },
			{ name: 'decrypted', type: 'const', returnType: 'any', doc: 'Constant decrypted' },
			{ name: 'Process', type: 'class', doc: 'Class Process' },
			{ name: 'kill', type: 'function', params: 'signal: int', returnType: 'Result<void, Error>', doc: 'Function kill' },
			{ name: 'wait', type: 'function', params: '', returnType: 'int', doc: 'Function wait' },
			{ name: 'EnvVars', type: 'class', doc: 'Class EnvVars' },
			{ name: 'get', type: 'function', params: 'key: string', returnType: 'string', doc: 'Function get' },
			{ name: 'set', type: 'function', params: 'key: string, value: string', returnType: 'void', doc: 'Function set' },
			{ name: 'unset', type: 'function', params: 'key: string', returnType: 'void', doc: 'Function unset' },
			{ name: 'has', type: 'function', params: 'key: string', returnType: 'bool', doc: 'Function has' },
			{ name: 'all', type: 'function', params: '', returnType: 'Map<string, string>', doc: 'Function all' },
			{ name: 'getenv', type: 'function', params: 'key: string', returnType: 'string', doc: 'Environment' },
			{ name: 'setenv', type: 'function', params: 'key: string, value: string', returnType: 'void', doc: 'Function setenv' },
			{ name: 'unsetenv', type: 'function', params: 'key: string', returnType: 'void', doc: 'Function unsetenv' },
			{ name: 'environ', type: 'function', params: '', returnType: 'Map<string, string>', doc: 'Function environ' },
			{ name: 'getpid', type: 'function', params: '', returnType: 'int', doc: 'Process info' },
			{ name: 'getppid', type: 'function', params: '', returnType: 'int', doc: 'Function getppid' },
			{ name: 'getuid', type: 'function', params: '', returnType: 'int', doc: 'Function getuid' },
			{ name: 'getgid', type: 'function', params: '', returnType: 'int', doc: 'Function getgid' },
			{ name: 'platform', type: 'function', params: '', returnType: 'Platform', doc: 'System info' },
			{ name: 'arch', type: 'function', params: '', returnType: 'Arch', doc: 'Function arch' },
			{ name: 'hostname', type: 'function', params: '', returnType: 'string', doc: 'Function hostname' },
			{ name: 'homedir', type: 'function', params: '', returnType: 'string', doc: 'Function homedir' },
			{ name: 'tmpdir', type: 'function', params: '', returnType: 'string', doc: 'Function tmpdir' },
			{ name: 'cpuCount', type: 'function', params: '', returnType: 'int', doc: 'Function cpuCount' },
			{ name: 'totalMemory', type: 'function', params: '', returnType: 'int', doc: 'Function totalMemory' },
			{ name: 'freeMemory', type: 'function', params: '', returnType: 'int', doc: 'Function freeMemory' },
			{ name: 'exec', type: 'function', params: 'command: string, args: Array<string>', returnType: 'Result<int, Error>', doc: 'Process execution' },
			{ name: 'execWithOutput', type: 'function', params: 'command: string, args: Array<string>', returnType: 'Result<string, Error>', doc: 'Function execWithOutput' },
			{ name: 'spawn', type: 'function', params: 'command: string, args: Array<string>', returnType: 'Result<Process, Error>', doc: 'Function spawn' },
			{ name: 'system', type: 'function', params: 'command: string', returnType: 'int', doc: 'Function system' },
			{ name: 'exit', type: 'function', params: 'code: int', returnType: 'void', doc: 'Process control' },
			{ name: 'abort', type: 'function', params: '', returnType: 'void', doc: 'Function abort' },
			{ name: 'getcwd', type: 'function', params: '', returnType: 'string', doc: 'Working directory' },
			{ name: 'chdir', type: 'function', params: 'path: string', returnType: 'Result<void, Error>', doc: 'Function chdir' },
			{ name: 'getUsername', type: 'function', params: '', returnType: 'string', doc: 'User info' },
			{ name: 'getUserHome', type: 'function', params: '', returnType: 'string', doc: 'Function getUserHome' },
			{ name: 'signal', type: 'function', params: 'sig: int, handler: Function', returnType: 'void', doc: 'Signals' },
			{ name: 'getEnvOr', type: 'function', params: 'key: string, defaultValue: string', returnType: 'string', doc: 'Get environment variable with default' },
			{ name: 'value', type: 'const', returnType: 'any', doc: 'Constant value' },
			{ name: 'hasEnv', type: 'function', params: 'key: string', returnType: 'bool', doc: 'Check if environment variable exists' },
			{ name: 'run', type: 'function', params: 'command: string', returnType: 'Result<string, Error>', doc: 'Execute command and get output' },
			{ name: 'runWithArgs', type: 'function', params: 'command: string, args: Array<string>', returnType: 'Result<string, Error>', doc: 'Execute command with arguments' },
			{ name: 'runSuccess', type: 'function', params: 'command: string', returnType: 'bool', doc: 'Execute command and check success' },
			{ name: 'result', type: 'const', returnType: 'any', doc: 'Constant result' },
			{ name: 'SystemInfo', type: 'class', doc: 'Get system info' },
			{ name: 'getSystemInfo', type: 'function', params: '', returnType: 'SystemInfo', doc: 'Function getSystemInfo' },
			{ name: 'isLinux', type: 'function', params: '', returnType: 'bool', doc: 'Check if running on specific platform' },
			{ name: 'isDarwin', type: 'function', params: '', returnType: 'bool', doc: 'Function isDarwin' },
			{ name: 'isWindows', type: 'function', params: '', returnType: 'bool', doc: 'Function isWindows' },
			{ name: 'joinPath', type: 'function', params: 'parts: Array<string>', returnType: 'string', doc: 'Path helpers' },
			{ name: 'separator', type: 'const', returnType: 'string', doc: 'Constant separator' },
			{ name: 'expandPath', type: 'function', params: 'path: string', returnType: 'string', doc: 'Function expandPath' },
			{ name: 'which', type: 'function', params: 'command: string', returnType: 'string?', doc: 'Which - find executable in PATH' },
			{ name: 'pathEnv', type: 'const', returnType: 'string', doc: 'Constant pathEnv' },
			{ name: 'paths', type: 'const', returnType: 'any', doc: 'Constant paths' },
			{ name: 'fullPath', type: 'const', returnType: 'any', doc: 'Constant fullPath' },
			{ name: 'commandExists', type: 'function', params: 'command: string', returnType: 'bool', doc: 'Check if command exists' },
			{ name: 'shell', type: 'function', params: 'command: string', returnType: 'Result<string, Error>', doc: 'Execute shell command' },
			{ name: 'shellCmd', type: 'const', returnType: 'string', doc: 'Constant shellCmd' },
			{ name: 'shellArg', type: 'const', returnType: 'string', doc: 'Constant shellArg' },
			{ name: 'pipe', type: 'function', params: 'commands: Array<string>', returnType: 'Result<string, Error>', doc: 'Pipe commands' },
			{ name: 'Environment', type: 'class', doc: 'Environment manager' },
			{ name: 'restore', type: 'function', params: '', returnType: 'void', doc: 'Function restore' },
			{ name: 'snapshot', type: 'function', params: '', returnType: 'Map<string, string>', doc: 'Function snapshot' },
			{ name: 'newEnvironment', type: 'function', params: '', returnType: 'Environment', doc: 'Function newEnvironment' },
			{ name: 'withEnv', type: 'function', params: 'vars: Map<string, string>, fn: Function', returnType: 'void', doc: 'Temporary environment for execution' },
			{ name: 'env', type: 'const', returnType: 'any', doc: 'Constant env' },
			{ name: 'getCurrentProcess', type: 'function', params: '', returnType: 'Process', doc: 'Current process info' },
			{ name: 'uptime', type: 'function', params: '', returnType: 'int', doc: 'Uptime (in seconds) - requires proc filesystem on Linux' },
			{ name: 'loadAverage', type: 'function', params: '', returnType: 'Array<double>', doc: 'Load average (Linux/Unix)' },
			{ name: 'pathSeparator', type: 'function', params: '', returnType: 'string', doc: 'Platform-specific path separator' },
			{ name: 'lineEnding', type: 'function', params: '', returnType: 'string', doc: 'Platform-specific line ending' },
			{ name: 'SIGINT', type: 'const', returnType: 'int', doc: 'Signals' },
			{ name: 'SIGTERM', type: 'const', returnType: 'int', doc: 'Constant SIGTERM' },
			{ name: 'SIGKILL', type: 'const', returnType: 'int', doc: 'Constant SIGKILL' },
			{ name: 'SIGHUP', type: 'const', returnType: 'int', doc: 'Constant SIGHUP' },
			{ name: 'SIGUSR1', type: 'const', returnType: 'int', doc: 'Constant SIGUSR1' },
			{ name: 'SIGUSR2', type: 'const', returnType: 'int', doc: 'Constant SIGUSR2' },
			{ name: 'toUpper', type: 'function', params: 's: string', returnType: 'string', doc: 'Case conversion' },
			{ name: 'toLower', type: 'function', params: 's: string', returnType: 'string', doc: 'Function toLower' },
			{ name: 'toTitle', type: 'function', params: 's: string', returnType: 'string', doc: 'Function toTitle' },
			{ name: 'trim', type: 'function', params: 's: string', returnType: 'string', doc: 'Trimming' },
			{ name: 'trimLeft', type: 'function', params: 's: string', returnType: 'string', doc: 'Function trimLeft' },
			{ name: 'trimRight', type: 'function', params: 's: string', returnType: 'string', doc: 'Function trimRight' },
			{ name: 'trimPrefix', type: 'function', params: 's: string, prefix: string', returnType: 'string', doc: 'Function trimPrefix' },
			{ name: 'trimSuffix', type: 'function', params: 's: string, suffix: string', returnType: 'string', doc: 'Function trimSuffix' },
			{ name: 'split', type: 'function', params: 's: string, separator: string', returnType: 'Array<string>', doc: 'Splitting and joining' },
			{ name: 'join', type: 'function', params: 'parts: Array<string>, separator: string', returnType: 'string', doc: 'Function join' },
			{ name: 'contains', type: 'function', params: 's: string, substr: string', returnType: 'bool', doc: 'Searching' },
			{ name: 'indexOf', type: 'function', params: 's: string, substr: string', returnType: 'int', doc: 'Function indexOf' },
			{ name: 'lastIndexOf', type: 'function', params: 's: string, substr: string', returnType: 'int', doc: 'Function lastIndexOf' },
			{ name: 'startsWith', type: 'function', params: 's: string, prefix: string', returnType: 'bool', doc: 'Function startsWith' },
			{ name: 'endsWith', type: 'function', params: 's: string, suffix: string', returnType: 'bool', doc: 'Function endsWith' },
			{ name: 'count', type: 'function', params: 's: string, substr: string', returnType: 'int', doc: 'Function count' },
			{ name: 'replace', type: 'function', params: 's: string, old: string, new: string', returnType: 'string', doc: 'Replacement' },
			{ name: 'replaceAll', type: 'function', params: 's: string, old: string, new: string', returnType: 'string', doc: 'Function replaceAll' },
			{ name: 'substring', type: 'function', params: 's: string, start: int, end: int', returnType: 'string', doc: 'Extraction' },
			{ name: 'charAt', type: 'function', params: 's: string, index: int', returnType: 'string', doc: 'Function charAt' },
			{ name: 'repeat', type: 'function', params: 's: string, count: int', returnType: 'string', doc: 'Repetition and padding' },
			{ name: 'padLeft', type: 'function', params: 's: string, length: int, pad: string', returnType: 'string', doc: 'Function padLeft' },
			{ name: 'padRight', type: 'function', params: 's: string, length: int, pad: string', returnType: 'string', doc: 'Function padRight' },
			{ name: 'compare', type: 'function', params: 'a: string, b: string', returnType: 'int', doc: 'Comparison' },
			{ name: 'equals', type: 'function', params: 'a: string, b: string', returnType: 'bool', doc: 'Function equals' },
			{ name: 'equalsIgnoreCase', type: 'function', params: 'a: string, b: string', returnType: 'bool', doc: 'Function equalsIgnoreCase' },
			{ name: 'length', type: 'function', params: 's: string', returnType: 'int', doc: 'Utility' },
			{ name: 'isEmpty', type: 'function', params: 's: string', returnType: 'bool', doc: 'Function isEmpty' },
			{ name: 'isBlank', type: 'function', params: 's: string', returnType: 'bool', doc: 'Function isBlank' },
			{ name: 'reverse', type: 'function', params: 's: string', returnType: 'string', doc: 'Function reverse' },
			{ name: 'isDigit', type: 'function', params: 's: string', returnType: 'bool', doc: 'Check if string contains only digits' },
			{ name: 'digits', type: 'const', returnType: 'string', doc: 'Constant digits' },
			{ name: 'isAlpha', type: 'function', params: 's: string', returnType: 'bool', doc: 'Check if string contains only letters' },
			{ name: 'lower', type: 'const', returnType: 'any', doc: 'Constant lower' },
			{ name: 'letters', type: 'const', returnType: 'string', doc: 'Constant letters' },
			{ name: 'isAlphaNumeric', type: 'function', params: 's: string', returnType: 'bool', doc: 'Check if string contains only alphanumeric characters' },
			{ name: 'capitalize', type: 'function', params: 's: string', returnType: 'string', doc: 'Capitalize first letter' },
			{ name: 'first', type: 'const', returnType: 'int', doc: 'Constant first' },
			{ name: 'rest', type: 'const', returnType: 'int', doc: 'Constant rest' },
			{ name: 'uncapitalize', type: 'function', params: 's: string', returnType: 'string', doc: 'Uncapitalize first letter' },
			{ name: 'toCamelCase', type: 'function', params: 's: string', returnType: 'string', doc: 'Convert to camelCase' },
			{ name: 'toPascalCase', type: 'function', params: 's: string', returnType: 'string', doc: 'Convert to PascalCase' },
			{ name: 'toSnakeCase', type: 'function', params: 's: string', returnType: 'string', doc: 'Convert to snake_case' },
			{ name: 'toKebabCase', type: 'function', params: 's: string', returnType: 'string', doc: 'Convert to kebab-case' },
			{ name: 'truncate', type: 'function', params: 's: string, maxLength: int, suffix: string', returnType: 'string', doc: 'Truncate string to max length' },
			{ name: 'truncated', type: 'const', returnType: 'int', doc: 'Constant truncated' },
			{ name: 'removeWhitespace', type: 'function', params: 's: string', returnType: 'string', doc: 'Remove all whitespace' },
			{ name: 'wordCount', type: 'function', params: 's: string', returnType: 'int', doc: 'Word count' },
			{ name: 'trimmed', type: 'const', returnType: 'any', doc: 'Constant trimmed' },
			{ name: 'words', type: 'const', returnType: 'string', doc: 'Constant words' },
			{ name: 'lines', type: 'function', params: 's: string', returnType: 'Array<string>', doc: 'Lines' },
			{ name: 'indent', type: 'function', params: 's: string, spaces: int', returnType: 'string', doc: 'Indent each line' },
			{ name: 'padding', type: 'const', returnType: 'string', doc: 'Constant padding' },
			{ name: 'lineArray', type: 'const', returnType: 'any', doc: 'Constant lineArray' },
			{ name: 'center', type: 'function', params: 's: string, width: int, fill: string', returnType: 'string', doc: 'Center string within width' },
			{ name: 'len', type: 'const', returnType: 'any', doc: 'Constant len' },
			{ name: 'leftPad', type: 'const', returnType: 'int', doc: 'Constant leftPad' },
			{ name: 'rightPad', type: 'const', returnType: 'any', doc: 'Constant rightPad' },
			{ name: 'commonPrefix', type: 'function', params: 'strings: Array<string>', returnType: 'string', doc: 'Common prefix of strings' },
			{ name: 'char', type: 'const', returnType: 'any', doc: 'Constant char' },
			{ name: 'sprintf', type: 'function', params: 'format: string, args: Array<any>', returnType: 'string', doc: 'Example: sprintf("Hello {}, you are {} years old", ["Alice", "25"])' },
			{ name: 'argStr', type: 'const', returnType: 'string', doc: 'Convert arg to string' },
			{ name: 'placeholder', type: 'const', returnType: 'string', doc: 'Find the first occurrence of {}' },
			{ name: 'index', type: 'const', returnType: 'any', doc: 'Constant index' },
			{ name: 'before', type: 'const', returnType: 'int', doc: 'Replace the first {} with the argument' },
			{ name: 'after', type: 'const', returnType: 'int', doc: 'Constant after' },
			{ name: 'TestCase', type: 'class', doc: 'Class TestCase' },
			{ name: 'TestSuite', type: 'class', doc: 'Class TestSuite' },
			{ name: 'TestResult', type: 'class', doc: 'Class TestResult' },
			{ name: 'TestRunner', type: 'class', doc: 'Class TestRunner' },
			{ name: 'runSuite', type: 'function', params: 'suite: TestSuite', returnType: 'Array<TestResult>', doc: 'Function runSuite' },
			{ name: 'TestReport', type: 'class', doc: 'Class TestReport' },
			{ name: 'print', type: 'function', params: '', returnType: 'void', doc: 'Function print' },
			{ name: 'Assertion', type: 'class', doc: 'Class Assertion' },
			{ name: 'notEquals', type: 'function', params: 'actual: any, expected: any', returnType: 'void', doc: 'Function notEquals' },
			{ name: 'isTrue', type: 'function', params: 'value: bool', returnType: 'void', doc: 'Function isTrue' },
			{ name: 'isFalse', type: 'function', params: 'value: bool', returnType: 'void', doc: 'Function isFalse' },
			{ name: 'isNull', type: 'function', params: 'value: any?', returnType: 'void', doc: 'Function isNull' },
			{ name: 'isNotNull', type: 'function', params: 'value: any?', returnType: 'void', doc: 'Function isNotNull' },
			{ name: 'throws', type: 'function', params: 'fn: Function', returnType: 'void', doc: 'Function throws' },
			{ name: 'assertEqual', type: 'function', params: 'actual: any, expected: any, message: string', returnType: 'void', doc: 'Assertion helpers' },
			{ name: 'assertNotEqual', type: 'function', params: 'actual: any, expected: any, message: string', returnType: 'void', doc: 'Function assertNotEqual' },
			{ name: 'assertTrue', type: 'function', params: 'value: bool, message: string', returnType: 'void', doc: 'Function assertTrue' },
			{ name: 'assertFalse', type: 'function', params: 'value: bool, message: string', returnType: 'void', doc: 'Function assertFalse' },
			{ name: 'runTests', type: 'function', params: 'suites: Array<TestSuite>', returnType: 'TestReport', doc: 'Test execution' },
			{ name: 'describe', type: 'function', params: 'name: string, fn: Function', returnType: 'void', doc: 'Define test suite' },
			{ name: 'suite', type: 'const', returnType: 'any', doc: 'Constant suite' },
			{ name: 'it', type: 'function', params: 'name: string, fn: Function', returnType: 'void', doc: 'Define test case' },
			{ name: 'test', type: 'const', returnType: 'any', doc: 'Constant test' },
			{ name: 'skip', type: 'function', params: 'name: string, fn: Function', returnType: 'void', doc: 'Skip test' },
			{ name: 'beforeEach', type: 'function', params: 'fn: Function', returnType: 'void', doc: 'Setup/teardown hooks' },
			{ name: 'afterEach', type: 'function', params: 'fn: Function', returnType: 'void', doc: 'Function afterEach' },
			{ name: 'beforeAll', type: 'function', params: 'fn: Function', returnType: 'void', doc: 'Function beforeAll' },
			{ name: 'afterAll', type: 'function', params: 'fn: Function', returnType: 'void', doc: 'Function afterAll' },
			{ name: 'expect', type: 'function', params: 'value: any', returnType: 'Assertion', doc: 'Assertion builder' },
			{ name: 'toBe', type: 'function', params: 'assertion: Assertion, expected: any', returnType: 'void', doc: 'Matchers' },
			{ name: 'toEqual', type: 'function', params: 'assertion: Assertion, expected: any', returnType: 'void', doc: 'Function toEqual' },
			{ name: 'toBeTrue', type: 'function', params: 'assertion: Assertion', returnType: 'void', doc: 'Function toBeTrue' },
			{ name: 'toBeFalse', type: 'function', params: 'assertion: Assertion', returnType: 'void', doc: 'Function toBeFalse' },
			{ name: 'toBeNull', type: 'function', params: 'assertion: Assertion', returnType: 'void', doc: 'Function toBeNull' },
			{ name: 'toContain', type: 'function', params: 'assertion: Assertion, substring: string', returnType: 'void', doc: 'Function toContain' },
			{ name: 'runAllTests', type: 'function', params: '', returnType: 'TestReport', doc: 'Run all tests' },
			{ name: 'runner', type: 'const', returnType: 'any', doc: 'Constant runner' },
			{ name: 'Mock', type: 'class', doc: 'Mock function builder' },
			{ name: 'mockReturnValue', type: 'function', params: 'value: T', returnType: 'Mock<T>', doc: 'Function mockReturnValue' },
			{ name: 'mockImplementation', type: 'function', params: 'fn: Function', returnType: 'Mock<T>', doc: 'Function mockImplementation' },
			{ name: 'call', type: 'function', params: 'args: Array<any>', returnType: 'T', doc: 'Function call' },
			{ name: 'getCalls', type: 'function', params: '', returnType: 'Array<Array<any>>', doc: 'Function getCalls' },
			{ name: 'getCallCount', type: 'function', params: '', returnType: 'int', doc: 'Function getCallCount' },
			{ name: 'wasCalledWith', type: 'function', params: 'args: Array<any>', returnType: 'bool', doc: 'Function wasCalledWith' },
			{ name: 'reset', type: 'function', params: '', returnType: 'void', doc: 'Function reset' },
			{ name: 'Spy', type: 'class', doc: 'Spy function wrapper' },
			{ name: 'Benchmark', type: 'class', doc: 'Benchmark helper' },
			{ name: 'start', type: 'const', returnType: 'any', doc: 'Constant start' },
			{ name: 'end', type: 'const', returnType: 'any', doc: 'Constant end' },
			{ name: 'report', type: 'function', params: '', returnType: 'void', doc: 'Function report' },
			{ name: 'total', type: 'const', returnType: 'int', doc: 'Constant total' },
			{ name: 'avg', type: 'const', returnType: 'any', doc: 'Constant avg' },
			{ name: 'benchmark', type: 'function', params: 'name: string, iterations: int, fn: Function', returnType: 'void', doc: 'Function benchmark' },
			{ name: 'bench', type: 'const', returnType: 'any', doc: 'Constant bench' },
			{ name: 'Snapshot', type: 'class', doc: 'Snapshot testing' },
			{ name: 'load', type: 'function', params: '', returnType: 'void', doc: 'Function load' },
			{ name: 'parsed', type: 'const', returnType: 'any', doc: 'Constant parsed' },
			{ name: 'save', type: 'function', params: '', returnType: 'void', doc: 'Function save' },
			{ name: 'data', type: 'const', returnType: 'any', doc: 'Constant data' },
			{ name: 'Coverage', type: 'class', doc: 'Test coverage helper' },
			{ name: 'add', type: 'function', params: 'file: string, line: int', returnType: 'void', doc: 'Function add' },
			{ name: 'FileCoverage', type: 'class', doc: 'Class FileCoverage' },
			{ name: 'percentage', type: 'function', params: '', returnType: 'double', doc: 'Function percentage' },
		],
		'db': [
			{ name: '__sqlite_open', type: 'function', params: 'path: string', returnType: 'any', doc: 'Function __sqlite_open' },
			{ name: '__sqlite_close', type: 'function', params: 'handle: any', returnType: 'void', doc: 'Function __sqlite_close' },
			{ name: '__sqlite_exec', type: 'function', params: 'handle: any, sql: string', returnType: 'int', doc: 'Function __sqlite_exec' },
			{ name: '__sqlite_query', type: 'function', params: 'handle: any, sql: string', returnType: 'any', doc: 'Function __sqlite_query' },
			{ name: '__sqlite_prepare', type: 'function', params: 'handle: any, sql: string', returnType: 'any', doc: 'Function __sqlite_prepare' },
			{ name: '__sqlite_bind', type: 'function', params: 'handle: any, index: int, value: any', returnType: 'void', doc: 'Function __sqlite_bind' },
			{ name: '__sqlite_bind_int', type: 'function', params: 'handle: any, index: int, value: int', returnType: 'void', doc: 'Function __sqlite_bind_int' },
			{ name: '__sqlite_bind_double', type: 'function', params: 'handle: any, index: int, value: double', returnType: 'void', doc: 'Function __sqlite_bind_double' },
			{ name: '__sqlite_bind_string', type: 'function', params: 'handle: any, index: int, value: string', returnType: 'void', doc: 'Function __sqlite_bind_string' },
			{ name: '__sqlite_bind_null', type: 'function', params: 'handle: any, index: int', returnType: 'void', doc: 'Function __sqlite_bind_null' },
			{ name: '__sqlite_stmt_exec', type: 'function', params: 'handle: any', returnType: 'int', doc: 'Function __sqlite_stmt_exec' },
			{ name: '__sqlite_stmt_reset', type: 'function', params: 'handle: any', returnType: 'void', doc: 'Function __sqlite_stmt_reset' },
			{ name: '__sqlite_stmt_close', type: 'function', params: 'handle: any', returnType: 'void', doc: 'Function __sqlite_stmt_close' },
			{ name: '__sqlite_step', type: 'function', params: 'handle: any', returnType: 'bool', doc: 'Function __sqlite_step' },
			{ name: '__sqlite_column_int', type: 'function', params: 'handle: any, index: int', returnType: 'int', doc: 'Function __sqlite_column_int' },
			{ name: '__sqlite_column_double', type: 'function', params: 'handle: any, index: int', returnType: 'double', doc: 'Function __sqlite_column_double' },
			{ name: '__sqlite_column_string', type: 'function', params: 'handle: any, index: int', returnType: 'string', doc: 'Function __sqlite_column_string' },
			{ name: '__sqlite_column_index', type: 'function', params: 'handle: any, name: string', returnType: 'int', doc: 'Function __sqlite_column_index' },
			{ name: '__sqlite_column_is_null', type: 'function', params: 'handle: any, index: int', returnType: 'bool', doc: 'Function __sqlite_column_is_null' },
			{ name: '__sqlite_column_count', type: 'function', params: 'handle: any', returnType: 'int', doc: 'Function __sqlite_column_count' },
			{ name: '__sqlite_column_name', type: 'function', params: 'handle: any, index: int', returnType: 'string', doc: 'Function __sqlite_column_name' },
			{ name: '__sqlite_rows_close', type: 'function', params: 'handle: any', returnType: 'void', doc: 'Function __sqlite_rows_close' },
			{ name: '__sqlite_last_insert_id', type: 'function', params: 'handle: any', returnType: 'int', doc: 'Function __sqlite_last_insert_id' },
			{ name: '__sqlite_changes', type: 'function', params: 'handle: any', returnType: 'int', doc: 'Function __sqlite_changes' },
			{ name: 'Rows', type: 'class', doc: 'Rows represents the result set from a query' },
			{ name: 'next', type: 'function', params: '', returnType: 'bool', doc: 'Returns true if there is a row, false if no more rows' },
			{ name: 'getInt', type: 'function', params: 'index: int', returnType: 'int', doc: 'Get an integer column by index (0-based)' },
			{ name: 'getDouble', type: 'function', params: 'index: int', returnType: 'double', doc: 'Get a double column by index' },
			{ name: 'getString', type: 'function', params: 'index: int', returnType: 'string', doc: 'Get a string column by index' },
			{ name: 'get', type: 'function', params: 'columnName: string', returnType: 'string', doc: 'Returns the value as a string (convert as needed)' },
			{ name: 'index', type: 'const', returnType: 'any', doc: 'Constant index' },
			{ name: 'isNull', type: 'function', params: 'index: int', returnType: 'bool', doc: 'Check if a column is NULL' },
			{ name: 'columnCount', type: 'function', params: '', returnType: 'int', doc: 'Get the number of columns in the result' },
			{ name: 'columnName', type: 'function', params: 'index: int', returnType: 'string', doc: 'Get the name of a column by index' },
			{ name: 'close', type: 'function', params: '', returnType: 'void', doc: 'Close the rows' },
			{ name: 'Statement', type: 'class', doc: 'Statement represents a prepared SQL statement' },
			{ name: 'bind', type: 'function', params: 'index: int, value: any', returnType: 'void', doc: 'stmt.bind(2, 25);' },
			{ name: 'bindInt', type: 'function', params: 'index: int, value: int', returnType: 'void', doc: 'Bind an integer parameter' },
			{ name: 'bindDouble', type: 'function', params: 'index: int, value: double', returnType: 'void', doc: 'Bind a double parameter' },
			{ name: 'bindString', type: 'function', params: 'index: int, value: string', returnType: 'void', doc: 'Bind a string parameter' },
			{ name: 'bindNull', type: 'function', params: 'index: int', returnType: 'void', doc: 'Bind NULL parameter' },
			{ name: 'execute', type: 'function', params: '', returnType: 'int', doc: 'Returns the number of rows affected' },
			{ name: 'query', type: 'function', params: '', returnType: 'Rows', doc: 'Query executes the prepared statement and returns rows' },
			{ name: 'reset', type: 'function', params: '', returnType: 'void', doc: 'Reset the statement to be executed again' },
			{ name: 'Transaction', type: 'class', doc: 'Transaction represents a database transaction' },
			{ name: 'commit', type: 'function', params: '', returnType: 'void', doc: 'Commit the transaction' },
			{ name: 'rollback', type: 'function', params: '', returnType: 'void', doc: 'Rollback the transaction' },
			{ name: 'Database', type: 'class', doc: 'Database represents a connection to a SQLite database' },
			{ name: 'exec', type: 'function', params: 'sql: string', returnType: 'int', doc: 'val affected = db.exec("DELETE FROM users WHERE id = 5");' },
			{ name: 'stmtHandle', type: 'const', returnType: 'any', doc: 'Constant stmtHandle' },
			{ name: 'prepare', type: 'function', params: 'sql: string', returnType: 'Statement', doc: 'stmt.execute();' },
			{ name: 'begin', type: 'function', params: '', returnType: 'Transaction', doc: 'Returns a Transaction object' },
			{ name: 'lastInsertId', type: 'function', params: '', returnType: 'int', doc: 'Get the last inserted row ID' },
			{ name: 'changes', type: 'function', params: '', returnType: 'int', doc: 'Get the number of changes made by the last statement' },
			{ name: 'open', type: 'function', params: 'path: string', returnType: 'Database', doc: 'val db = db.open(":memory:");              // In-memory database' },
			{ name: 'handle', type: 'const', returnType: 'any', doc: 'Constant handle' },
		],
		'ffi': [
			{ name: 'load', type: 'function', params: 'path: string', returnType: 'int', doc: 'Example: val lib = ffi.load("./libmylib.so");' },
			{ name: 'unload', type: 'function', params: 'libraryId: int', returnType: 'void', doc: 'Example: ffi.unload(lib);' },
			{ name: 'call', type: 'function', params: 'libraryId: int, functionName: string, returnType: string, paramTypes: array, args: array', returnType: 'int', doc: 'Example: val result = ffi.call(lib, "add", "int", ["int", "int"], [5, 3]);' },
			{ name: 'callVoid', type: 'function', params: 'libraryId: int, functionName: string, paramTypes: array, args: array', returnType: 'void', doc: 'Example: ffi.callVoid(lib, "greet", ["string"], ["World"]);' },
			{ name: 'callInt', type: 'function', params: 'libraryId: int, functionName: string, paramTypes: array, args: array', returnType: 'int', doc: 'Example: val result = ffi.callInt(lib, "add", ["int", "int"], [5, 3]);' },
			{ name: 'callDouble', type: 'function', params: 'libraryId: int, functionName: string, paramTypes: array, args: array', returnType: 'double', doc: 'Example: val result = ffi.callDouble(lib, "sqrt", ["double"], [9.0]);' },
		],
		'std.fs': [
			{ name: 'File', type: 'class', doc: 'Class File' },
			{ name: 'open', type: 'function', params: 'mode: string', returnType: 'void', doc: 'Function open' },
			{ name: 'close', type: 'function', params: '', returnType: 'void', doc: 'Function close' },
			{ name: 'write', type: 'function', params: 'text: string', returnType: 'void', doc: 'Function write' },
		],
		'greeting': [
			{ name: 'hello', type: 'function', params: 'name: string', returnType: 'string', doc: 'Function hello' },
			{ name: 'goodbye', type: 'function', params: 'name: string', returnType: 'string', doc: 'Function goodbye' },
			{ name: 'greet', type: 'function', params: '', returnType: 'string', doc: 'Function greet' },
		],
		'io': [
			{ name: 'print', type: 'function', params: 'message: string', returnType: 'void', doc: 'Writes the string to the standard output.' },
			{ name: 'println', type: 'function', params: 'message: string', returnType: 'void', doc: 'Writes the string to the standard output, followed by a newline.' },
			{ name: 'File', type: 'class', doc: 'Class File' },
			{ name: 'readAll', type: 'function', params: '', returnType: 'string', doc: 'Function readAll' },
			{ name: 'readLine', type: 'function', params: '', returnType: 'string', doc: 'Function readLine' },
			{ name: 'write', type: 'function', params: 'data: string', returnType: 'Result<int, Error>', doc: 'Function write' },
			{ name: 'close', type: 'function', params: '', returnType: 'void', doc: 'Function close' },
			{ name: 'Result', type: 'class', doc: 'Class Result' },
			{ name: 'ok', type: 'function', params: '', returnType: 'bool', doc: 'Function ok' },
			{ name: 'err', type: 'function', params: '', returnType: 'E', doc: 'Function err' },
			{ name: 'unwrap', type: 'function', params: '', returnType: 'T', doc: 'Function unwrap' },
			{ name: 'Error', type: 'class', doc: 'Class Error' },
			{ name: 'FileInfo', type: 'class', doc: 'Class FileInfo' },
			{ name: 'readFile', type: 'function', params: 'path: string', returnType: 'Result<string, Error>', doc: 'File reading' },
			{ name: 'readBytes', type: 'function', params: 'path: string', returnType: 'Result<Array<byte>, Error>', doc: 'Function readBytes' },
			{ name: 'writeFile', type: 'function', params: 'path: string, content: string', returnType: 'Result<void, Error>', doc: 'File writing' },
			{ name: 'appendFile', type: 'function', params: 'path: string, content: string', returnType: 'Result<void, Error>', doc: 'Function appendFile' },
			{ name: 'writeBytes', type: 'function', params: 'path: string, data: Array<byte>', returnType: 'Result<void, Error>', doc: 'Function writeBytes' },
			{ name: 'open', type: 'function', params: 'path: string, mode: string', returnType: 'Result<File, Error>', doc: 'File operations' },
			{ name: 'create', type: 'function', params: 'path: string', returnType: 'Result<File, Error>', doc: 'Function create' },
			{ name: 'remove', type: 'function', params: 'path: string', returnType: 'Result<void, Error>', doc: 'Function remove' },
			{ name: 'rename', type: 'function', params: 'oldPath: string, newPath: string', returnType: 'Result<void, Error>', doc: 'Function rename' },
			{ name: 'copy', type: 'function', params: 'src: string, dst: string', returnType: 'Result<void, Error>', doc: 'Function copy' },
			{ name: 'mkdir', type: 'function', params: 'path: string', returnType: 'Result<void, Error>', doc: 'Directory operations' },
			{ name: 'mkdirAll', type: 'function', params: 'path: string', returnType: 'Result<void, Error>', doc: 'Function mkdirAll' },
			{ name: 'removeDir', type: 'function', params: 'path: string', returnType: 'Result<void, Error>', doc: 'Function removeDir' },
			{ name: 'removeDirAll', type: 'function', params: 'path: string', returnType: 'Result<void, Error>', doc: 'Function removeDirAll' },
			{ name: 'readDir', type: 'function', params: 'path: string', returnType: 'Result<Array<FileInfo>, Error>', doc: 'Function readDir' },
			{ name: 'exists', type: 'function', params: 'path: string', returnType: 'bool', doc: 'File info' },
			{ name: 'isFile', type: 'function', params: 'path: string', returnType: 'bool', doc: 'Function isFile' },
			{ name: 'isDirectory', type: 'function', params: 'path: string', returnType: 'bool', doc: 'Function isDirectory' },
			{ name: 'fileSize', type: 'function', params: 'path: string', returnType: 'Result<int, Error>', doc: 'Function fileSize' },
			{ name: 'stat', type: 'function', params: 'path: string', returnType: 'Result<FileInfo, Error>', doc: 'Function stat' },
			{ name: 'pathJoin', type: 'function', params: 'parts: Array<string>', returnType: 'string', doc: 'Path operations' },
			{ name: 'basename', type: 'function', params: 'path: string', returnType: 'string', doc: 'Function basename' },
			{ name: 'dirname', type: 'function', params: 'path: string', returnType: 'string', doc: 'Function dirname' },
			{ name: 'extension', type: 'function', params: 'path: string', returnType: 'string', doc: 'Function extension' },
			{ name: 'absolute', type: 'function', params: 'path: string', returnType: 'string', doc: 'Function absolute' },
			{ name: 'tempFile', type: 'function', params: 'pattern: string', returnType: 'Result<File, Error>', doc: 'Temporary files' },
			{ name: 'tempDir', type: 'function', params: 'pattern: string', returnType: 'Result<string, Error>', doc: 'Function tempDir' },
			{ name: 'readLines', type: 'function', params: 'path: string', returnType: 'Result<Array<string>, Error>', doc: 'Read lines from file' },
			{ name: 'content', type: 'const', returnType: 'any', doc: 'Constant content' },
			{ name: 'lines', type: 'const', returnType: 'string', doc: 'Constant lines' },
			{ name: 'writeLines', type: 'function', params: 'path: string, lines: Array<string>', returnType: 'Result<void, Error>', doc: 'Write lines to file' },
			{ name: 'appendLine', type: 'function', params: 'path: string, line: string', returnType: 'Result<void, Error>', doc: 'Append line to file' },
			{ name: 'hasExtension', type: 'function', params: 'path: string, ext: string', returnType: 'bool', doc: 'Check if path has extension' },
			{ name: 'listFiles', type: 'function', params: 'path: string, filter: Function<FileInfo, bool>', returnType: 'Result<Array<FileInfo>, Error>', doc: 'List files in directory with filter' },
			{ name: 'filesResult', type: 'const', returnType: 'any', doc: 'Constant filesResult' },
			{ name: 'files', type: 'const', returnType: 'any', doc: 'Constant files' },
			{ name: 'listFilesOnly', type: 'function', params: 'path: string', returnType: 'Result<Array<FileInfo>, Error>', doc: 'List only files (not directories)' },
			{ name: 'listDirectoriesOnly', type: 'function', params: 'path: string', returnType: 'Result<Array<FileInfo>, Error>', doc: 'List only directories' },
			{ name: 'ensureDir', type: 'function', params: 'path: string', returnType: 'Result<void, Error>', doc: 'Ensure directory exists (create if not)' },
			{ name: 'walk', type: 'function', params: 'root: string, visitor: Function<string, FileInfo, void>', returnType: 'Result<void, Error>', doc: 'Walk directory tree' },
			{ name: 'fullPath', type: 'const', returnType: 'any', doc: 'Constant fullPath' },
		],
		'log': [
			{ name: 'Logger', type: 'class', doc: 'Class Logger' },
			{ name: 'debug', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function debug' },
			{ name: 'info', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function info' },
			{ name: 'warn', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function warn' },
			{ name: 'error', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function error' },
			{ name: 'fatal', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function fatal' },
			{ name: 'with', type: 'function', params: 'fields: Map<string, any>', returnType: 'Logger', doc: 'Function with' },
			{ name: 'setLevel', type: 'function', params: 'level: LogLevel', returnType: 'void', doc: 'Function setLevel' },
			{ name: 'setFormat', type: 'function', params: 'format: LogFormat', returnType: 'void', doc: 'Function setFormat' },
			{ name: 'setFile', type: 'function', params: 'path: string', returnType: 'Result<void, Error>', doc: 'fn setOutput(writer: Writer) void; // Writer not defined' },
			{ name: 'newLogger', type: 'function', params: 'level: LogLevel, format: LogFormat', returnType: 'Logger', doc: 'Logger creation' },
			{ name: 'debugWith', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Structured logging wrappers' },
			{ name: 'infoWith', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function infoWith' },
			{ name: 'warnWith', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function warnWith' },
			{ name: 'errorWith', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function errorWith' },
			{ name: 'fatalWith', type: 'function', params: 'message: string, fields: Map<string, any>', returnType: 'void', doc: 'Function fatalWith' },
			{ name: 'formatFields', type: 'function', params: 'fields: Map<string, any>', returnType: 'string', doc: 'Function formatFields' },
			{ name: 'infof', type: 'function', params: 'format: string, args: Array<any>', returnType: 'void', doc: 'Convenient logging with format' },
			{ name: 'message', type: 'const', returnType: 'any', doc: 'Constant message' },
			{ name: 'debugf', type: 'function', params: 'format: string, args: Array<any>', returnType: 'void', doc: 'Function debugf' },
			{ name: 'warnf', type: 'function', params: 'format: string, args: Array<any>', returnType: 'void', doc: 'Function warnf' },
			{ name: 'errorf', type: 'function', params: 'format: string, args: Array<any>', returnType: 'void', doc: 'Function errorf' },
			{ name: 'withContext', type: 'function', params: 'fields: Map<string, any>', returnType: 'Logger', doc: 'Create logger with context' },
			{ name: 'LogTimer', type: 'class', doc: 'LogTimer for performance logging' },
			{ name: 'elapsed', type: 'function', params: '', returnType: 'int', doc: 'Function elapsed' },
			{ name: 'log', type: 'function', params: '', returnType: 'void', doc: 'Function log' },
			{ name: 'startTimer', type: 'function', params: 'name: string', returnType: 'LogTimer', doc: 'Function startTimer' },
			{ name: 'logFunction', type: 'function', params: 'name: string, callback: Function', returnType: 'any', doc: 'Log middleware for functions' },
			{ name: 'timer', type: 'const', returnType: 'any', doc: 'Constant timer' },
			{ name: 'result', type: 'const', returnType: 'any', doc: 'Constant result' },
		],
		'regex': [
			{ name: 'Match', type: 'class', doc: 'Class Match' },
			{ name: 'Regex', type: 'class', doc: 'Class Regex' },
			{ name: 'CASE_INSENSITIVE', type: 'const', returnType: 'int', doc: 'Regex flags' },
			{ name: 'MULTILINE', type: 'const', returnType: 'int', doc: 'Constant MULTILINE' },
			{ name: 'DOTALL', type: 'const', returnType: 'int', doc: 'Constant DOTALL' },
			{ name: 'EXTENDED', type: 'const', returnType: 'int', doc: 'Constant EXTENDED' },
			{ name: 'compile', type: 'function', params: 'pattern: string', returnType: 'int', doc: 'Returns 1 if valid, 0 if invalid' },
			{ name: 'compileWithFlags', type: 'function', params: 'pattern: string, flags: int', returnType: 'int', doc: 'Compile with flags' },
			{ name: 'matches', type: 'function', params: 'pattern: string, text: string', returnType: 'bool', doc: 'Test if entire string matches pattern' },
			{ name: 'contains', type: 'function', params: 'pattern: string, text: string', returnType: 'bool', doc: 'Test if pattern is found anywhere in string' },
			{ name: 'find', type: 'function', params: 'pattern: string, text: string', returnType: 'int', doc: 'Returns match position or -1 if not found' },
			{ name: 'findAll', type: 'function', params: 'pattern: string, text: string', returnType: 'Array<int>', doc: 'Returns array of start positions' },
			{ name: 'replace', type: 'function', params: 'pattern: string, text: string, replacement: string', returnType: 'string', doc: 'Replace first occurrence' },
			{ name: 'replaceAll', type: 'function', params: 'pattern: string, text: string, replacement: string', returnType: 'string', doc: 'Replace all occurrences' },
			{ name: 'split', type: 'function', params: 'pattern: string, text: string', returnType: 'Array<string>', doc: 'Split string by regex pattern' },
			{ name: 'groups', type: 'function', params: 'pattern: string, text: string', returnType: 'Array<string>', doc: 'Returns array where index 0 is full match, 1+ are groups' },
			{ name: 'matchAll', type: 'function', params: 'pattern: string, text: string', returnType: 'Array<Array<string>>', doc: 'Get all matches with capture groups' },
			{ name: 'escape', type: 'function', params: 'text: string', returnType: 'string', doc: 'Escape special regex characters' },
			{ name: 'startsWith', type: 'function', params: 'pattern: string, text: string', returnType: 'bool', doc: 'Test if string starts with pattern' },
			{ name: 'anchoredPattern', type: 'const', returnType: 'string', doc: 'Constant anchoredPattern' },
			{ name: 'endsWith', type: 'function', params: 'pattern: string, text: string', returnType: 'bool', doc: 'Test if string ends with pattern' },
			{ name: 'count', type: 'function', params: 'pattern: string, text: string', returnType: 'int', doc: 'Count occurrences of pattern' },
			{ name: 'positions', type: 'const', returnType: 'any', doc: 'Constant positions' },
			{ name: 'extract', type: 'function', params: 'pattern: string, text: string', returnType: 'Array<string>', doc: 'Extract all matches (without groups)' },
			{ name: 'allMatches', type: 'const', returnType: 'any', doc: 'Constant allMatches' },
			{ name: 'replaceFunc', type: 'function', params: 'pattern: string, text: string, replacer: Function<string, string>', returnType: 'string', doc: 'Replace with callback function' },
			{ name: 'replacement', type: 'const', returnType: 'any', doc: 'Constant replacement' },
			{ name: 'matchesAny', type: 'function', params: 'patterns: Array<string>, text: string', returnType: 'bool', doc: 'Test multiple patterns (OR)' },
			{ name: 'matchesAll', type: 'function', params: 'patterns: Array<string>, text: string', returnType: 'bool', doc: 'Test all patterns (AND)' },
			{ name: 'EMAIL_PATTERN', type: 'const', returnType: 'string', doc: 'Email validation (simplified)' },
			{ name: 'URL_PATTERN', type: 'const', returnType: 'string', doc: 'URL pattern (simplified)' },
			{ name: 'IPV4_PATTERN', type: 'const', returnType: 'string', doc: 'IPv4 address' },
			{ name: 'PHONE_PATTERN', type: 'const', returnType: 'string', doc: 'Phone number (US format)' },
			{ name: 'DATE_ISO', type: 'const', returnType: 'string', doc: 'Date patterns' },
			{ name: 'DATE_US', type: 'const', returnType: 'string', doc: 'Constant DATE_US' },
			{ name: 'DATE_EU', type: 'const', returnType: 'string', doc: 'Constant DATE_EU' },
			{ name: 'TIME_24H', type: 'const', returnType: 'string', doc: 'Time pattern (24-hour)' },
			{ name: 'HEX_COLOR', type: 'const', returnType: 'string', doc: 'Hex color code' },
			{ name: 'INTEGER', type: 'const', returnType: 'string', doc: 'Integer number' },
			{ name: 'DECIMAL', type: 'const', returnType: 'string', doc: 'Decimal number' },
			{ name: 'WHITESPACE', type: 'const', returnType: 'string', doc: 'Whitespace' },
			{ name: 'WORD', type: 'const', returnType: 'string', doc: 'Word boundary' },
			{ name: 'isEmail', type: 'function', params: 'text: string', returnType: 'bool', doc: 'Function isEmail' },
			{ name: 'isURL', type: 'function', params: 'text: string', returnType: 'bool', doc: 'Function isURL' },
			{ name: 'isIPv4', type: 'function', params: 'text: string', returnType: 'bool', doc: 'Function isIPv4' },
			{ name: 'isPhoneNumber', type: 'function', params: 'text: string', returnType: 'bool', doc: 'Function isPhoneNumber' },
			{ name: 'isDateISO', type: 'function', params: 'text: string', returnType: 'bool', doc: 'Function isDateISO' },
			{ name: 'isInteger', type: 'function', params: 'text: string', returnType: 'bool', doc: 'Function isInteger' },
			{ name: 'isDecimal', type: 'function', params: 'text: string', returnType: 'bool', doc: 'Function isDecimal' },
			{ name: 'isHexColor', type: 'function', params: 'text: string', returnType: 'bool', doc: 'Function isHexColor' },
			{ name: 'extractEmails', type: 'function', params: 'text: string', returnType: 'Array<string>', doc: 'Extract all emails from text' },
			{ name: 'extractURLs', type: 'function', params: 'text: string', returnType: 'Array<string>', doc: 'Extract all URLs from text' },
			{ name: 'extractNumbers', type: 'function', params: 'text: string', returnType: 'Array<string>', doc: 'Extract all numbers from text' },
			{ name: 'extractWords', type: 'function', params: 'text: string', returnType: 'Array<string>', doc: 'Extract all words from text' },
			{ name: 'removeWhitespace', type: 'function', params: 'text: string', returnType: 'string', doc: 'Remove all whitespace' },
			{ name: 'normalizeWhitespace', type: 'function', params: 'text: string', returnType: 'string', doc: 'Normalize whitespace (collapse multiple spaces to one)' },
			{ name: 'stripHTMLTags', type: 'function', params: 'text: string', returnType: 'string', doc: 'Remove HTML tags' },
			{ name: 'extractBetween', type: 'function', params: 'text: string, start: string, end: string', returnType: 'Array<string>', doc: 'Extract text between delimiters' },
			{ name: 'pattern', type: 'const', returnType: 'string', doc: 'Constant pattern' },
			{ name: 'maskSensitive', type: 'function', params: 'text: string, pattern: string, maskChar: string', returnType: 'string', doc: 'Mask sensitive data (like credit card numbers)' },
			{ name: 'camelToSnake', type: 'function', params: 'text: string', returnType: 'string', doc: 'Convert camelCase to snake_case' },
			{ name: 'snakeToCamel', type: 'function', params: 'text: string', returnType: 'string', doc: 'Convert snake_case to camelCase' },
			{ name: 'findOverlapping', type: 'function', params: 'pattern: string, text: string', returnType: 'Array<string>', doc: 'Find overlapping matches' },
			{ name: 'substring', type: 'const', returnType: 'any', doc: 'Constant substring' },
			{ name: 'pos', type: 'const', returnType: 'any', doc: 'Constant pos' },
			{ name: 'match', type: 'const', returnType: 'any', doc: 'Extract the match' },
			{ name: 'splitLimit', type: 'function', params: 'pattern: string, text: string, limit: int', returnType: 'Array<string>', doc: 'Split with limit' },
			{ name: 'parts', type: 'const', returnType: 'any', doc: 'Constant parts' },
			{ name: 'MatchInfo', type: 'class', doc: 'Match with position information' },
			{ name: 'findWithInfo', type: 'function', params: 'pattern: string, text: string', returnType: 'MatchInfo?', doc: 'Get detailed match information' },
			{ name: 'matchText', type: 'const', returnType: 'int', doc: 'Constant matchText' },
			{ name: 'isValid', type: 'function', params: 'pattern: string', returnType: 'bool', doc: 'Check if pattern is valid' },
			{ name: 'quote', type: 'function', params: 'text: string', returnType: 'string', doc: 'Quote/escape a string to be used as literal text in regex' },
			{ name: 'alternate', type: 'function', params: 'patterns: Array<string>', returnType: 'string', doc: 'Create alternation pattern (a|b|c)' },
			{ name: 'charClass', type: 'function', params: 'chars: string', returnType: 'string', doc: 'Create character class [abc]' },
			{ name: 'negatedCharClass', type: 'function', params: 'chars: string', returnType: 'string', doc: 'Create negated character class [^abc]' },
			{ name: 'optional', type: 'function', params: 'pattern: string', returnType: 'string', doc: 'Make pattern optional (pattern)?' },
			{ name: 'oneOrMore', type: 'function', params: 'pattern: string', returnType: 'string', doc: 'Make pattern repeat one or more times (pattern)+' },
			{ name: 'zeroOrMore', type: 'function', params: 'pattern: string', returnType: 'string', doc: 'Make pattern repeat zero or more times (pattern)*' },
			{ name: 'exactly', type: 'function', params: 'pattern: string, n: int', returnType: 'string', doc: 'Make pattern repeat exactly n times' },
			{ name: 'between', type: 'function', params: 'pattern: string, min: int, max: int', returnType: 'string', doc: 'Make pattern repeat between min and max times' },
		],
		'terminal': [
			{ name: 'TerminalSize', type: 'class', doc: 'Class TerminalSize' },
			{ name: 'Key', type: 'class', doc: 'Class Key' },
			{ name: 'KEY_UP', type: 'const', returnType: 'int', doc: 'Key codes for special keys' },
			{ name: 'KEY_DOWN', type: 'const', returnType: 'int', doc: 'Constant KEY_DOWN' },
			{ name: 'KEY_LEFT', type: 'const', returnType: 'int', doc: 'Constant KEY_LEFT' },
			{ name: 'KEY_RIGHT', type: 'const', returnType: 'int', doc: 'Constant KEY_RIGHT' },
			{ name: 'KEY_ENTER', type: 'const', returnType: 'int', doc: 'Constant KEY_ENTER' },
			{ name: 'KEY_ESC', type: 'const', returnType: 'int', doc: 'Constant KEY_ESC' },
			{ name: 'KEY_BACKSPACE', type: 'const', returnType: 'int', doc: 'Constant KEY_BACKSPACE' },
			{ name: 'KEY_TAB', type: 'const', returnType: 'int', doc: 'Constant KEY_TAB' },
			{ name: 'KEY_DELETE', type: 'const', returnType: 'int', doc: 'Constant KEY_DELETE' },
			{ name: 'KEY_HOME', type: 'const', returnType: 'int', doc: 'Constant KEY_HOME' },
			{ name: 'KEY_END', type: 'const', returnType: 'int', doc: 'Constant KEY_END' },
			{ name: 'KEY_PAGE_UP', type: 'const', returnType: 'int', doc: 'Constant KEY_PAGE_UP' },
			{ name: 'KEY_PAGE_DOWN', type: 'const', returnType: 'int', doc: 'Constant KEY_PAGE_DOWN' },
			{ name: 'KEY_F1', type: 'const', returnType: 'int', doc: 'Constant KEY_F1' },
			{ name: 'KEY_F2', type: 'const', returnType: 'int', doc: 'Constant KEY_F2' },
			{ name: 'KEY_F3', type: 'const', returnType: 'int', doc: 'Constant KEY_F3' },
			{ name: 'KEY_F4', type: 'const', returnType: 'int', doc: 'Constant KEY_F4' },
			{ name: 'KEY_F5', type: 'const', returnType: 'int', doc: 'Constant KEY_F5' },
			{ name: 'KEY_F6', type: 'const', returnType: 'int', doc: 'Constant KEY_F6' },
			{ name: 'KEY_F7', type: 'const', returnType: 'int', doc: 'Constant KEY_F7' },
			{ name: 'KEY_F8', type: 'const', returnType: 'int', doc: 'Constant KEY_F8' },
			{ name: 'KEY_F9', type: 'const', returnType: 'int', doc: 'Constant KEY_F9' },
			{ name: 'KEY_F10', type: 'const', returnType: 'int', doc: 'Constant KEY_F10' },
			{ name: 'KEY_F11', type: 'const', returnType: 'int', doc: 'Constant KEY_F11' },
			{ name: 'KEY_F12', type: 'const', returnType: 'int', doc: 'Constant KEY_F12' },
			{ name: 'clear', type: 'function', params: '', returnType: 'void', doc: 'Clear the entire screen' },
			{ name: 'clearLine', type: 'function', params: '', returnType: 'void', doc: 'Clear from cursor to end of line' },
			{ name: 'clearToBottom', type: 'function', params: '', returnType: 'void', doc: 'Clear from cursor to end of screen' },
			{ name: 'clearToTop', type: 'function', params: '', returnType: 'void', doc: 'Clear from cursor to start of screen' },
			{ name: 'moveCursor', type: 'function', params: 'row: int, col: int', returnType: 'void', doc: 'Move cursor to specific position (1-based)' },
			{ name: 'cursorUp', type: 'function', params: 'n: int', returnType: 'void', doc: 'Move cursor up by n lines' },
			{ name: 'cursorDown', type: 'function', params: 'n: int', returnType: 'void', doc: 'Move cursor down by n lines' },
			{ name: 'cursorLeft', type: 'function', params: 'n: int', returnType: 'void', doc: 'Move cursor left by n columns' },
			{ name: 'cursorRight', type: 'function', params: 'n: int', returnType: 'void', doc: 'Move cursor right by n columns' },
			{ name: 'saveCursor', type: 'function', params: '', returnType: 'void', doc: 'Save cursor position' },
			{ name: 'restoreCursor', type: 'function', params: '', returnType: 'void', doc: 'Restore cursor position' },
			{ name: 'hideCursor', type: 'function', params: '', returnType: 'void', doc: 'Hide cursor' },
			{ name: 'showCursor', type: 'function', params: '', returnType: 'void', doc: 'Show cursor' },
			{ name: 'getCursorPosition', type: 'function', params: '', returnType: 'Array<int>', doc: 'Get cursor position' },
			{ name: 'setForeground', type: 'function', params: 'color: Color', returnType: 'void', doc: 'Set foreground color' },
			{ name: 'setBackground', type: 'function', params: 'color: Color', returnType: 'void', doc: 'Set background color' },
			{ name: 'setForegroundRGB', type: 'function', params: 'r: int, g: int, b: int', returnType: 'void', doc: 'Set foreground color using RGB (0-255)' },
			{ name: 'setBackgroundRGB', type: 'function', params: 'r: int, g: int, b: int', returnType: 'void', doc: 'Set background color using RGB (0-255)' },
			{ name: 'setStyle', type: 'function', params: 'style: Style', returnType: 'void', doc: 'Set text style' },
			{ name: 'reset', type: 'function', params: '', returnType: 'void', doc: 'Reset all styles and colors' },
			{ name: 'getSize', type: 'function', params: '', returnType: 'TerminalSize', doc: 'Get terminal size' },
			{ name: 'getWidth', type: 'function', params: '', returnType: 'int', doc: 'Get terminal width' },
			{ name: 'getHeight', type: 'function', params: '', returnType: 'int', doc: 'Get terminal height' },
			{ name: 'supportsColor', type: 'function', params: '', returnType: 'bool', doc: 'Check if terminal supports colors' },
			{ name: 'isTTY', type: 'function', params: '', returnType: 'bool', doc: 'Check if running in a TTY' },
			{ name: 'enableRawMode', type: 'function', params: '', returnType: 'void', doc: 'Enable raw mode (no buffering, no echo)' },
			{ name: 'disableRawMode', type: 'function', params: '', returnType: 'void', doc: 'Disable raw mode (restore normal mode)' },
			{ name: 'readChar', type: 'function', params: '', returnType: 'string', doc: 'Read a single character (non-blocking if in raw mode)' },
			{ name: 'readKey', type: 'function', params: '', returnType: 'Key', doc: 'Read a single key press (handles escape sequences)' },
			{ name: 'readLine', type: 'function', params: '', returnType: 'string', doc: 'Read a line of input' },
			{ name: 'input', type: 'function', params: 'prompt: string', returnType: 'string', doc: 'Read input with a prompt' },
			{ name: 'hasInput', type: 'function', params: '', returnType: 'bool', doc: 'Check if input is available (non-blocking)' },
			{ name: 'useAlternateScreen', type: 'function', params: '', returnType: 'void', doc: 'Switch to alternate screen buffer' },
			{ name: 'useMainScreen', type: 'function', params: '', returnType: 'void', doc: 'Switch back to main screen buffer' },
			{ name: 'printColor', type: 'function', params: 'text: string, fg: Color', returnType: 'void', doc: 'Print with color' },
			{ name: 'printColorBg', type: 'function', params: 'text: string, fg: Color, bg: Color', returnType: 'void', doc: 'Print with color and background' },
			{ name: 'printStyled', type: 'function', params: 'text: string, style: Style', returnType: 'void', doc: 'Print styled text' },
			{ name: 'printBold', type: 'function', params: 'text: string', returnType: 'void', doc: 'Print bold text' },
			{ name: 'printItalic', type: 'function', params: 'text: string', returnType: 'void', doc: 'Print italic text' },
			{ name: 'printUnderline', type: 'function', params: 'text: string', returnType: 'void', doc: 'Print underlined text' },
			{ name: 'printError', type: 'function', params: 'text: string', returnType: 'void', doc: 'Print error message in red' },
			{ name: 'printSuccess', type: 'function', params: 'text: string', returnType: 'void', doc: 'Print success message in green' },
			{ name: 'printWarning', type: 'function', params: 'text: string', returnType: 'void', doc: 'Print warning message in yellow' },
			{ name: 'printInfo', type: 'function', params: 'text: string', returnType: 'void', doc: 'Print info message in blue' },
			{ name: 'colored', type: 'function', params: 'text: string, fg: Color', returnType: 'string', doc: 'Create a colored string (returns ANSI escape codes)' },
			{ name: 'code', type: 'const', returnType: 'any', doc: 'Constant code' },
			{ name: 'coloredBg', type: 'function', params: 'text: string, fg: Color, bg: Color', returnType: 'string', doc: 'Create a colored string with background' },
			{ name: 'fgCode', type: 'const', returnType: 'any', doc: 'Constant fgCode' },
			{ name: 'bgCode', type: 'const', returnType: 'any', doc: 'Constant bgCode' },
			{ name: 'getColorCode', type: 'function', params: 'color: Color, background: bool', returnType: 'string', doc: 'Helper to get ANSI color code' },
			{ name: 'base', type: 'const', returnType: 'int', doc: 'Constant base' },
			{ name: 'brightBase', type: 'const', returnType: 'int', doc: 'Constant brightBase' },
			{ name: 'drawLine', type: 'function', params: 'width: int, char: string', returnType: 'void', doc: 'Draw a horizontal line' },
			{ name: 'i', type: 'const', returnType: 'int', doc: 'Constant i' },
			{ name: 'drawBox', type: 'function', params: 'x: int, y: int, width: int, height: int, title: string', returnType: 'void', doc: 'Draw a box' },
			{ name: 'printCentered', type: 'function', params: 'text: string, row: int', returnType: 'void', doc: 'Center text on screen' },
			{ name: 'width', type: 'const', returnType: 'any', doc: 'Constant width' },
			{ name: 'col', type: 'const', returnType: 'int', doc: 'Constant col' },
			{ name: 'printAt', type: 'function', params: 'row: int, col: int, text: string', returnType: 'void', doc: 'Print at specific position' },
			{ name: 'drawProgressBar', type: 'function', params: 'current: int, total: int, width: int', returnType: 'void', doc: 'Progress bar' },
			{ name: 'percentage', type: 'const', returnType: 'int', doc: 'Constant percentage' },
			{ name: 'filled', type: 'const', returnType: 'any', doc: 'Constant filled' },
			{ name: 'MenuItem', type: 'class', doc: 'Menu system' },
			{ name: 'showMenu', type: 'function', params: 'title: string, items: Array<MenuItem>', returnType: 'int', doc: 'Function showMenu' },
			{ name: 'selected', type: 'const', returnType: 'int', doc: 'Constant selected' },
			{ name: 'item', type: 'const', returnType: 'any', doc: 'Constant item' },
			{ name: 'key', type: 'const', returnType: 'any', doc: 'Constant key' },
			{ name: 'confirm', type: 'function', params: 'message: string', returnType: 'bool', doc: 'Confirm dialog' },
			{ name: 'response', type: 'const', returnType: 'any', doc: 'Constant response' },
			{ name: 'inputWithValidation', type: 'function', params: 'prompt: string, validator: Function<string, bool>', returnType: 'string', doc: 'Text input with validation' },
			{ name: 'value', type: 'const', returnType: 'any', doc: 'Constant value' },
			{ name: 'Spinner', type: 'class', doc: 'Spinner animation' },
			{ name: 'next', type: 'function', params: '', returnType: 'string', doc: 'Function next' },
			{ name: 'frame', type: 'const', returnType: 'any', doc: 'Constant frame' },
			{ name: 'newSpinner', type: 'function', params: '', returnType: 'Spinner', doc: 'Function newSpinner' },
			{ name: 'printTable', type: 'function', params: 'headers: Array<string>, rows: Array<Array<string>>', returnType: 'void', doc: 'Table printing' },
			{ name: 'widths', type: 'const', returnType: 'any', doc: 'Calculate column widths' },
			{ name: 'len', type: 'const', returnType: 'any', doc: 'Constant len' },
			{ name: 'j', type: 'const', returnType: 'int', doc: 'Constant j' },
			{ name: 'k', type: 'const', returnType: 'int', doc: 'Constant k' },
			{ name: 'l', type: 'const', returnType: 'int', doc: 'Constant l' },
			{ name: 'm', type: 'const', returnType: 'int', doc: 'Constant m' },
		],
		'websocket': [
			{ name: 'connect', type: 'function', params: 'url: string', returnType: 'int', doc: 'Returns: WebSocket connection ID (int), -1 on error' },
			{ name: 'send', type: 'function', params: 'wsId: int, message: string', returnType: 'void', doc: 'Usage: websocket.send(ws, "Hello, World!")' },
			{ name: 'receive', type: 'function', params: 'wsId: int', returnType: 'string', doc: 'Returns: Message string (empty if timeout/error)' },
			{ name: 'close', type: 'function', params: 'wsId: int', returnType: 'void', doc: 'Usage: websocket.close(ws)' },
			{ name: 'isConnected', type: 'function', params: 'wsId: int', returnType: 'bool', doc: 'Returns: true if connected, false otherwise' },
			{ name: 'createServer', type: 'function', params: 'port: int', returnType: 'int', doc: 'Returns: Server ID (int), -1 on error' },
			{ name: 'acceptClient', type: 'function', params: 'serverId: int', returnType: 'int', doc: 'Returns: Client ID (int), -1 if no connection or timeout' },
			{ name: 'sendToClient', type: 'function', params: 'clientId: int, message: string', returnType: 'void', doc: 'Usage: websocket.sendToClient(client, "Hello, Client!")' },
			{ name: 'receiveFromClient', type: 'function', params: 'clientId: int', returnType: 'string', doc: 'Returns: Message string (empty if timeout/error)' },
			{ name: 'closeClient', type: 'function', params: 'clientId: int', returnType: 'void', doc: 'Usage: websocket.closeClient(client)' },
			{ name: 'closeServer', type: 'function', params: 'serverId: int', returnType: 'void', doc: 'Usage: websocket.closeServer(server)' },
			{ name: 'isClientConnected', type: 'function', params: 'clientId: int', returnType: 'bool', doc: 'Returns: true if connected, false otherwise' },
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
            // Development build path (Windows)
            path.resolve(__dirname, '../../../../src/interpreter/cpp/build/stratos.exe'),
            path.join(process.cwd(), 'src/interpreter/cpp/build/stratos.exe'),
            // Development build path (Unix)
            path.resolve(__dirname, '../../../../src/interpreter/cpp/build/stratos'),
            path.join(process.cwd(), 'src/interpreter/cpp/build/stratos'),
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
		const errorRegex = /\[Error\]\s+(?:Line\s+)?(\d+):(\d+):\s+(.+)/gi;
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