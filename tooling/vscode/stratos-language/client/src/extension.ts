/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */
import * as dotnet from './dotnet';
import * as executables from './executables';

import * as path from 'path';
const fs = require('fs');

import { workspace, ExtensionContext, commands } from 'vscode';
import * as vscode from 'vscode';

import {
	LanguageClient,
	LanguageClientOptions,
	ServerOptions,
	TransportKind
} from 'vscode-languageclient';
import { Script } from 'vm';

let client: LanguageClient;

export async function activate(context: ExtensionContext) {

	// The server is implemented in node
	let serverModule = context.asAbsolutePath(
		path.join('server', 'out', 'server.js')
	);
	// The debug options for the server
	// --inspect=6009: runs the server in Node's Inspector mode so VS Code can attach to the server for debugging
	let debugOptions = { execArgv: ['--nolazy', '--inspect=6009'] };

	// If the extension is launched in debug mode then the debug server options are used
	// Otherwise the run options are used
	const dotNetExecutable = await executables.find('dotnet');
	const serverAssembly = context.asAbsolutePath('csharp/bin/Debug/netcoreapp3.1/StratosServer.dll');

	if (dotNetExecutable === null) {
		vscode.window.showErrorMessage("You don't have .Net Core Support!");
	}
	else {
		const terminal = vscode.window.createTerminal('SL Diagnostic Terminal');
		terminal.sendText(`dotnet ${serverAssembly}`);
	}

	let serverOptions: ServerOptions = {
		run: { module: serverModule, transport: TransportKind.ipc },
		debug: {
			module: serverModule,
			transport: TransportKind.ipc,
			options: debugOptions
		}
	};

	// Options to control the language client
	let clientOptions: LanguageClientOptions = {
		// Register the server for stratos text documents
		documentSelector: [{ scheme: 'file', language: 'stratos' }],
		synchronize: {
			// Notify the server about file changes to '.clientrc files contained in the workspace
			fileEvents: workspace.createFileSystemWatcher('**/.clientrc')
		}
	};


	// Create the language client and start the client.
	client = new LanguageClient(
		'languageServerExample',
		'Stratos Language Server ',
		serverOptions,
		clientOptions
	);

	client.start();

	const fetchDeps = vscode.commands.registerCommand('extension.fetchDependancies', () => {
		vscode.window.showInformationMessage('Fetch  project  Dependancies ...');
	});
	
	const createProject = vscode.commands.registerCommand('extension.createProject',
		() => {



			const options: vscode.OpenDialogOptions = {
				canSelectMany: false,
				canSelectFolders: true,
				canSelectFiles: false,
				openLabel: 'Choose project folder',
				defaultUri: vscode.workspace.workspaceFolders[0].uri
			};

			vscode.window.showOpenDialog(options).then(fileUri => {
				if (fileUri && fileUri[0]) {
					let dir: string = fileUri[0].fsPath;
					let configFile: string = path.join(dir, "app.conf");
					let srcFolder = path.join(dir, "src");
					let mainFile = path.join(srcFolder, "main.st");



					if (!fs.existsSync(configFile)) {


						fs.writeFile(configFile, "project = { \n" +
							"\t# project name\n" +
							"\tname = \"application\"\n" +
							"\t# project version\n" +
							"\tversion = \"0.0.1\"\n" +
							"\t# project id \n" +
							"\tid = \"com.stratos.application\"\n" +
							"\t#keyword for \n" +
							"\tkeywords = [\"application\"]\n" +
							"\tdescription=\"\"\"\n" +
							"\tproject description\n" +
							"\t\"\"\"\n" +

							"\tdependancies = [\n" +

							"\n " +
							"\t]\n" +
							"}\n", "utf-8",
							(err) => {
								if (err) throw err; console.log("Config file was created");
							});

						if (!fs.existsSync(srcFolder)) {
							fs.mkdirSync(srcFolder)
						}

						fs.writeFile(mainFile, "package main;\n" +
							"\n" +
							"function main(){ \n" +
							"\t print(\"Hello world\");\n" +
							" }", "utf-8",
							(err) => {
								if (err) throw err; console.log("Main file was created");
							});

						vscode.workspace.openTextDocument(configFile).then((a: vscode.TextDocument) => {
							vscode.window.showTextDocument(a, 1, false);
						});
						vscode.workspace.openTextDocument(mainFile).then((a: vscode.TextDocument) => {
							vscode.window.showTextDocument(a, 1, false);
						});

						vscode.window.showInformationMessage(`Stratos Project Created`);
					}
					else {
						vscode.window.showErrorMessage('app.conf file exists, probably the directory contains a Stratos project');
					}
				}
			});

		});

	const runApplication = vscode.commands.registerCommand('extension.runApplication', () => {
		// The code you place here will be executed every time your command is executed

		// Display a message box to the user
		vscode.window.showInformationMessage('Running Application ...');
	});

	context.subscriptions.push(createProject, runApplication, fetchDeps);
	// context.subscriptions.push(createProject);
}

export function deactivate(): Thenable<void> | undefined {
	if (!client) {
		return undefined;
	}
	return client.stop();
}
