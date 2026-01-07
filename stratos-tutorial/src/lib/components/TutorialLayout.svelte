<script lang="ts">
	// No explicit slot props are declared here as we'll use <slot name="..."> directly.
</script>

<div class="flex h-screen bg-gray-900 text-white">
	<!-- Guide Panel -->
	<div class="w-1/3 p-4 border-r border-gray-700 flex flex-col">
		<h2 class="text-xl font-bold mb-4">Tutorial Guide</h2>
		<div class="flex-grow overflow-y-auto pr-2"> <!-- pr-2 to prevent scrollbar from hiding content -->
			<slot name="guide-content" />
		</div>
		<div class="mt-4 pt-4 border-t border-gray-700">
			<slot name="guide-navigation" />
		</div>
	</div>

	<!-- Editor Panel -->
	<div class="w-1/3 p-4 border-r border-gray-700 flex flex-col">
		<h2 class="text-xl font-bold mb-4">Code Editor</h2>
		<div class="flex-grow editor-container">
			<slot name="editor" />
		</div>
	</div>

	<!-- Preview Panel -->
	<div class="w-1/3 p-4 flex flex-col">
		<h2 class="text-xl font-bold mb-4">Live Preview / Output</h2>
		<div class="flex-grow preview-container bg-gray-800 p-2 rounded overflow-y-auto">
			<slot name="preview" />
		</div>
	</div>
</div>