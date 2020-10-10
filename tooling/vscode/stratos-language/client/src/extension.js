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
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
exports.__esModule = true;
exports.deactivate = exports.activate = void 0;
var executables = require("./executables");
var path = require("path");
var fs = require('fs');
var vscode_1 = require("vscode");
var vscode = require("vscode");
var vscode_languageclient_1 = require("vscode-languageclient");
var client;
function activate(context) {
    return __awaiter(this, void 0, void 0, function () {
        var serverModule, debugOptions, dotNetExecutable, serverAssembly, terminal, serverOptions, clientOptions, fetchDeps, createProject, runApplication;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    serverModule = context.asAbsolutePath(path.join('server', 'out', 'server.js'));
                    debugOptions = { execArgv: ['--nolazy', '--inspect=6009'] };
                    return [4 /*yield*/, executables.find('dotnet')];
                case 1:
                    dotNetExecutable = _a.sent();
                    serverAssembly = context.asAbsolutePath('csharp/bin/Debug/netcoreapp3.1/StratosServer.dll');
                    if (dotNetExecutable === null) {
                        vscode.window.showErrorMessage("You don't have .Net Core Support!");
                    }
                    else {
                        terminal = vscode.window.createTerminal('SL Diagnostic Terminal');
                        terminal.sendText("dotnet " + serverAssembly);
                    }
                    serverOptions = {
                        run: { module: serverModule, transport: vscode_languageclient_1.TransportKind.ipc },
                        debug: {
                            module: serverModule,
                            transport: vscode_languageclient_1.TransportKind.ipc,
                            options: debugOptions
                        }
                    };
                    clientOptions = {
                        // Register the server for stratos text documents
                        documentSelector: [{ scheme: 'file', language: 'stratos' }],
                        synchronize: {
                            // Notify the server about file changes to '.clientrc files contained in the workspace
                            fileEvents: vscode_1.workspace.createFileSystemWatcher('**/.clientrc')
                        }
                    };
                    // 👍 formatter implemented using API
                    vscode.languages.registerDocumentFormattingEditProvider('stratos', {
                        provideDocumentFormattingEdits: function (document) {
                            var firstLine = document.lineAt(0);
                            if (firstLine.text !== '42') {
                                return [vscode.TextEdit.insert(firstLine.range.start, '42\n')];
                            }
                        }
                    });
                    
                    // Create the language client and start the client.
                    client = new vscode_languageclient_1.LanguageClient('languageServerExample', 'Stratos Language Server ', serverOptions, clientOptions);
                    client.start();
                    fetchDeps = vscode.commands.registerCommand('extension.fetchDependancies', function () {
                        vscode.window.showInformationMessage('Fetch  project  Dependancies ...');
                    });
                    createProject = vscode.commands.registerCommand('extension.createProject', function () {
                        var options = {
                            canSelectMany: false,
                            canSelectFolders: true,
                            canSelectFiles: false,
                            openLabel: 'Choose project folder',
                            defaultUri: vscode.workspace.workspaceFolders[0].uri
                        };
                        vscode.window.showOpenDialog(options).then(function (fileUri) {
                            if (fileUri && fileUri[0]) {
                                var dir = fileUri[0].fsPath;
                                var configFile = path.join(dir, "app.conf");
                                var srcFolder = path.join(dir, "src");
                                var mainFile = path.join(srcFolder, "main.st");
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
                                        "}\n", "utf-8", function (err) {
                                        if (err)
                                            throw err;
                                        console.log("Config file was created");
                                    });
                                    if (!fs.existsSync(srcFolder)) {
                                        fs.mkdirSync(srcFolder);
                                    }
                                    fs.writeFile(mainFile, "package main;\n" +
                                        "\n" +
                                        "function main(){ \n" +
                                        "\t print(\"Hello world\");\n" +
                                        " }", "utf-8", function (err) {
                                        if (err)
                                            throw err;
                                        console.log("Main file was created");
                                    });
                                    vscode.workspace.openTextDocument(configFile).then(function (a) {
                                        vscode.window.showTextDocument(a, 1, false);
                                    });
                                    vscode.workspace.openTextDocument(mainFile).then(function (a) {
                                        vscode.window.showTextDocument(a, 1, false);
                                    });
                                    vscode.window.showInformationMessage("Stratos Project Created");
                                }
                                else {
                                    vscode.window.showErrorMessage('app.conf file exists, probably the directory contains a Stratos project');
                                }
                            }
                        });
                    });
                    runApplication = vscode.commands.registerCommand('extension.runApplication', function () {
                        // The code you place here will be executed every time your command is executed
                        // Display a message box to the user
                        vscode.window.showInformationMessage('Running Application ...');
                    });
                    context.subscriptions.push(createProject, runApplication, fetchDeps);
                    return [2 /*return*/];
            }
        });
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
