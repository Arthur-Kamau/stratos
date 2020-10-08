"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.isErrnoException = void 0;
function isErrnoException(error) {
    return typeof error === 'object' && typeof error['code'] !== 'undefined';
}
exports.isErrnoException = isErrnoException;
//# sourceMappingURL=errors.js.map