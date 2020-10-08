/**
 * Find the specified executable (if it exists) on the current environment's `PATH`.
 *
 * @param executableName The executable name (with or without extension).
 * @returns {Promise<string | null>} A promise that resolves to the full path, or `null` if the executable was not found.
 */
export declare function find(executableName: string): Promise<string | null>;
