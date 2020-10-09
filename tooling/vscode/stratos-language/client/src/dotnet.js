"use strict";
// Простая проверка наличия dotnet в системе
// created by https://github.com/tintoy/ 
exports.__esModule = true;
exports.find = void 0;
var which = require("which");
var errors_1 = require("./errors");
function find(executableName) {
    return new Promise(function (resolve, reject) {
        which(executableName, function (error, resolvedPath) {
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
