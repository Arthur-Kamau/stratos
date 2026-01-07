import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vite';
import pkg from 'vite-plugin-monaco-editor';
const monacoEditorPlugin = pkg.default;

export default defineConfig({
	plugins: [sveltekit(), monacoEditorPlugin({})],
	server: {
		port: 8002
	}
});
