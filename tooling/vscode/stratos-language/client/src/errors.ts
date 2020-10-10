export function isErrnoException(error: Error): error is NodeJS.ErrnoException {
    return typeof error === 'object' && typeof error['code'] !== 'undefined';
}