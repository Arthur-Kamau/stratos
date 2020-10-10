// Простая проверка наличия dotnet в системе
// created by https://github.com/tintoy/ 

import * as which from 'which';
import { isErrnoException } from './errors';

export function find(executableName: string): Promise<string | null> {
    return new Promise<string | null>((resolve, reject) => {
        which(executableName, (error, resolvedPath) => {
            if (error) {
                if (isErrnoException(error) && error.code === 'ENOENT')
                    resolve(null);
                else
                    reject(error);
            }
            else
                resolve(resolvedPath);
        });
    });
}