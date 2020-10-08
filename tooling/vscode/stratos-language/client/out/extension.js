"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.deactivate = exports.activate = void 0;
const executables = require("./executables");
const path = require("path");
const fs = require('fs');
const vscode_1 = require("vscode");
const vscode = require("vscode");
const vscode_languageclient_1 = require("vscode-languageclient");
let client;
function activate(context) {
    return __awaiter(this, void 0, void 0, function* () {
        // The server is implemented in node
        let serverModule = context.asAbsolutePath(path.join('server', 'out', 'server.js'));
        // The debug options for the server
        // --inspect=6009: runs the server in Node's Inspector mode so VS Code can attach to the server for debugging
        let debugOptions = { execArgv: ['--nolazy', '--inspect=6009'] };
        // If the extension is launched in debug mode then the debug server options are used
        // Otherwise the run options are used
        const dotNetExecutable = yield executables.find('dotnet');
        const serverAssembly = context.asAbsolutePath('csharp/bin/Debug/netcoreapp2.1/AntlrServer.dll');
        if (dotNetExecutable === null) {
            vscode.window.showErrorMessage("You don't have .Net Core Support!");
        }
        else {
            const terminal = vscode.window.createTerminal('SL Diagnostic Terminal');
            terminal.sendText(`dotnet ${serverAssembly}`);
        }
        let serverOptions = {
            run: { module: serverModule, transport: vscode_languageclient_1.TransportKind.ipc },
            debug: {
                module: serverModule,
                transport: vscode_languageclient_1.TransportKind.ipc,
                options: debugOptions
            }
        };
        // Options to control the language client
        let clientOptions = {
            // Register the server for plain text documents
            documentSelector: [{ scheme: 'file', language: 'semanticlanguage' }],
            synchronize: {
                // Notify the server about file changes to '.clientrc files contained in the workspace
                fileEvents: vscode_1.workspace.createFileSystemWatcher('**/.clientrc')
            }
        };
        // Create the language client and start the client.
        client = new vscode_languageclient_1.LanguageClient('languageServerExample', 'Language Server Example', serverOptions, clientOptions);
        client.start();
        let v = vscode.commands.registerCommand('ext.InitProject', () => {
            const options = {
                canSelectMany: false,
                canSelectFolders: true,
                canSelectFiles: false,
                openLabel: 'Choose project folder',
                defaultUri: vscode.workspace.workspaceFolders[0].uri
            };
            vscode.window.showOpenDialog(options).then(fileUri => {
                if (fileUri && fileUri[0]) {
                    let dir = fileUri[0].fsPath;
                    let pfile = path.join(dir, "Main.sl");
                    fs.writeFile(pfile, 'module Main\nstart\n\toutput \"Hello World!\";\nend Main.', "utf-8", (err) => {
                        if (err)
                            throw err;
                        console.log("Project was created");
                    });
                    if (!fs.existsSync(pfile)) {
                        vscode.window.showInformationMessage(`SL Project Created, Main file = ${pfile}`);
                        vscode.workspace.openTextDocument(pfile).then((a) => {
                            vscode.window.showTextDocument(a, 1, false);
                        });
                    }
                    else {
                        vscode.window.showErrorMessage('Project Error');
                    }
                }
            });
        });
        context.subscriptions.push(v);
    });
}
exports.activate = activate;
function deactivate() {
    if (!client) {
        return undefined;
    }
    return client.stop();
}
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map