"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.isStratosRepoAsync = exports.hasConfigFileAsync = exports.hasConfigFile = exports.hasPackagesFile = exports.FileUtils = void 0;
const fs = require("fs");
const path = require("path");
class FileUtils {
    getFileDirectory(filePath) {
        if (filePath.indexOf("/") == -1) { // windows
            return filePath.substring(0, filePath.lastIndexOf('\\'));
        }
        else { // unix
            return filePath.substring(0, filePath.lastIndexOf('/'));
        }
    }
    getFileDirectoryName(filePath) {
        let path = this.getFileDirectory(filePath);
        if (path.indexOf("/") == -1) { // windows
            return path.split("\\").reverse()[0];
        }
        else { // unix
            return path.split("/").reverse()[0];
        }
    }
}
exports.FileUtils = FileUtils;
function hasPackagesFile(folder) {
    return fs.existsSync(path.join(folder, ".packages"));
}
exports.hasPackagesFile = hasPackagesFile;
function hasConfigFile(folder) {
    return fs.existsSync(path.join(folder, "app.config"));
}
exports.hasConfigFile = hasConfigFile;
async function hasConfigFileAsync(folder) {
    return await fileExists(path.join(folder, "app.config"));
}
exports.hasConfigFileAsync = hasConfigFileAsync;
async function isStratosRepoAsync(folder) {
    return await fileExists(path.join(folder, "bin/stratos")) && await fileExists(path.join(folder, "bin/cache/dart-sdk"));
}
exports.isStratosRepoAsync = isStratosRepoAsync;
async function fileExists(p) {
    try {
        await fs.promises.access(p);
        return true;
    }
    catch {
        return false;
    }
}
//# sourceMappingURL=file_util.js.map