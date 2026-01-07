<script lang="ts">
	import TutorialLayout from '$lib/components/TutorialLayout.svelte';

	let code = `print("Hello from Stratos!")`;
	let output = '';
	let error = '';
	let exitCode: number | null = null;
	let isLoading = false;

	async function runCode() {
		isLoading = true;
		output = '';
		error = '';
		exitCode = null;

		try {
			const response = await fetch('/api/run', {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({ code })
			});

			const result = await response.json();

			if (response.ok) {
				output = result.stdout;
				error = result.stderr;
				exitCode = result.exitCode;
			} else {
				error = result.error || 'Unknown API error';
				if (result.details) {
					error += `: ${result.details}`;
				}
				exitCode = result.exitCode || 1;
			}
		} catch (e: any) {
			error = `Failed to connect to server: ${e.message}`;
			exitCode = 1;
		} finally {
			isLoading = false;
		}
	}
</script>

<TutorialLayout>
	<svelte:fragment slot="guide">
		<h3 class="text-lg font-semibold mb-2">Welcome to Stratos!</h3>
		<p class="mb-4">
			This interactive tutorial will guide you through the basics of the Stratos programming language.
		</p>
		<h4 class="font-medium mb-1">Step 1: Your First Program</h4>
		<p class="mb-4">
			The editor to your right contains a simple "Hello World" program. Click the "Run Code" button
			to see its output in the preview panel.
		</p>
		<p class="text-sm text-gray-400">
			You can modify the code and run it again to experiment!
		</p>
	</svelte:fragment>

	<svelte:fragment slot="editor">
		<textarea
			class="w-full flex-grow bg-gray-800 text-white p-2 rounded font-mono text-sm resize-none"
			bind:value={code}
		></textarea>
		<button
			class="mt-4 px-4 py-2 bg-blue-600 hover:bg-blue-700 rounded text-white font-semibold disabled:opacity-50"
			onclick={runCode}
			disabled={isLoading}
		>
			{isLoading ? 'Running...' : 'Run Code'}
		</button>
	</svelte:fragment>

	<svelte:fragment slot="preview">
		{#if isLoading}
			<p class="text-yellow-400">Running Stratos code...</p>
		{:else if output || error}
			{#if output}
				<pre class="whitespace-pre-wrap text-green-300 mb-2">{output}</pre>
			{/if}
			{#if error}
				<pre class="whitespace-pre-wrap text-red-500 mb-2">Error: {error}</pre>
			{/if}
			<p class="text-gray-400">Exit Code: {exitCode}</p>
		{:else}
			<p class="text-gray-500">Run the code to see output here.</p>
		{/if}
	</svelte:fragment>
</TutorialLayout>