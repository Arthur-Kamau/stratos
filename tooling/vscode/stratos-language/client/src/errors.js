"use strict";
exports.__esModule = true;
exports.isErrnoException = void 0;
function isErrnoException(error) {
    return typeof error === 'object' && typeof error['code'] !== 'undefined';
}
exports.isErrnoException = isErrnoException;
