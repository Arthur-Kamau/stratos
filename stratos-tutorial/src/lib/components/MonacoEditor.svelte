<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import type * as Monaco from 'monaco-editor/esm/vs/editor/editor.api';

	export let value: string;
	export let language = 'stratos';
	export let readOnly = false;

	let editorElement: HTMLDivElement;
	let editor: Monaco.editor.IStandaloneCodeEditor;
	let monaco: typeof Monaco;

	onMount(async () => {
		// Import monaco dynamically to avoid SSR issues
		monaco = await import('monaco-editor');

		// Define Stratos language
		monaco.languages.register({ id: 'stratos' });
		monaco.languages.setMonarchTokensProvider('stratos', {
			tokenizer: {
				root: [
					[/\b(fun|return|if|else|while|for|break|continue|var|const|true|false|null|import|extern)\b/, 'keyword'],
					[/[A-Z][a-zA-Z0-9_]*/, 'type.identifier'],
					[/[a-z][a-zA-Z0-9_]*/, 'identifier'],
					[/\d+/, 'number'],
					[/"([^"\\]|\\.)*"/, 'string'],
					[/\/\/.*$/, 'comment'],
				]
			}
		});

        // Define a simple dark theme
        monaco.editor.defineTheme('stratos-dark', {
            base: 'vs-dark',
            inherit: true,
            rules: [
                { token: 'keyword', foreground: 'C586C0' },
                { token: 'type.identifier', foreground: '4EC9B0' },
                { token: 'identifier', foreground: '9CDCFE' },
                { token: 'string', foreground: 'CE9178' },
                { token: 'number', foreground: 'B5CEA8' },
                { token: 'comment', foreground: '6A9955' },
            ],
            colors: {
                'editor.background': '#1f2937' // Match Tailwind gray-800
            }
        });

		editor = monaco.editor.create(editorElement, {
			value,
			language,
            theme: 'stratos-dark',
			readOnly,
			minimap: { enabled: false },
			automaticLayout: true,
            fontSize: 14,
            fontFamily: 'ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace',
            scrollBeyondLastLine: false,
            padding: { top: 16, bottom: 16 }
		});

		editor.onDidChangeModelContent(() => {
			value = editor.getValue();
		});
	});

	onDestroy(() => {
		if (editor) {
			editor.dispose();
		}
	});

    // React to external value changes
    $: if (editor && value !== editor.getValue()) {
        editor.setValue(value);
    }
</script>

<div bind:this={editorElement} class="w-full h-full min-h-[400px]"></div>