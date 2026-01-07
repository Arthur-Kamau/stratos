<script lang="ts">
	import TutorialLayout from '$lib/components/TutorialLayout.svelte';
	import MonacoEditor from '$lib/components/MonacoEditor.svelte';
	import { goto } from '$app/navigation';

	export let data;
	
	// Reactive declarations to update when data (step) changes
	$: projectId = data.projectId;
	$: step = data.step;
	$: totalSteps = data.totalSteps;
	$: code = step.initialCode; 

	// Reset output when step changes
	$: {
		if (step) {
			output = '';
			error = '';
			exitCode = null;
		}
	}

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
				body: JSON.stringify({ code, projectId })
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

	function goNext() {
		if (step.id < totalSteps) {
			goto(`/step/${step.id + 1}`);
		}
	}

	function goPrev() {
		if (step.id > 1) {
			goto(`/step/${step.id - 1}`);
		}
	}
</script>

<TutorialLayout>
	<svelte:fragment slot="guide-content">
		<!-- Render HTML content safely -->
		{@html step.content}
	</svelte:fragment>
	
	<svelte:fragment slot="guide-navigation">
		<div class="flex justify-between items-center">
			<button 
				class="px-3 py-1 bg-gray-700 hover:bg-gray-600 rounded disabled:opacity-50 text-sm"
				onclick={goPrev}
				disabled={step.id === 1}
			>
				&larr; Previous
			</button>
			<span class="text-sm text-gray-400">Step {step.id} of {totalSteps}</span>
			<button 
				class="px-3 py-1 bg-blue-600 hover:bg-blue-500 rounded disabled:opacity-50 text-sm"
				onclick={goNext}
				disabled={step.id === totalSteps}
			>
				Next &rarr;
			</button>
		</div>
	</svelte:fragment>

	<svelte:fragment slot="editor">
		<div class="flex-grow relative border border-gray-700 rounded overflow-hidden">
			<!-- Key block forces editor re-creation when step ID changes to reset content properly -->
			{#key step.id}
				<MonacoEditor bind:value={code} />
			{/key}
		</div>
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
			<p class="text-yellow-400">Compiling Stratos code...</p>
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
