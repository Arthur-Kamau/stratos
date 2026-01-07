import * as mod from 'vite-plugin-monaco-editor';
import defaultImport from 'vite-plugin-monaco-editor';

console.log('Namespace import keys:', Object.keys(mod));
console.log('Default import type:', typeof defaultImport);
console.log('Default import:', defaultImport);
