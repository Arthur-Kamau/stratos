"use strict";
// Простая проверка наличия dotnet в системе
// created by https://github.com/tintoy/ 
Object.defineProperty(exports, "__esModule", { value: true });
exports.find = void 0;
const which = require("which");
const errors_1 = require("./errors");
function find(executableName) {
    return new Promise((resolve, reject) => {
        which(executableName, (error, resolvedPath) => {
            if (error) {
                if (errors_1.isErrnoException(error) && error.code === 'ENOENT')
                    resolve(null);
                else
                    reject(error);
            }
            else
                resolve(resolvedPath);
        });
    });
}
exports.find = find;
//# sourceMappingURL=dotnet.js.map