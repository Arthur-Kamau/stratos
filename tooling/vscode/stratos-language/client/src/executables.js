"use strict";
exports.__esModule = true;
exports.find = void 0;
var which = require("which");
var errors_1 = require("./errors");
/**
 * Find the specified executable (if it exists) on the current environment's `PATH`.
 *
 * @param executableName The executable name (with or without extension).
 * @returns {Promise<string | null>} A promise that resolves to the full path, or `null` if the executable was not found.
 */
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
