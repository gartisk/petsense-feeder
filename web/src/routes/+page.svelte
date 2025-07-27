<script>
	import { onMount } from 'svelte';
	import { browser } from '$app/environment';

	import { t, locale } from 'svelte-i18n';

	// Reactive state variables
	let isLoading = false;
	let showSaveWarning = false;
	let selectedLanguage = 'pt';
	let doorState = 'closed';
	let doorStatusMessage = 'The door is currently closed.';
	let showLogs = false;
	let logContent = '';

	// Settings data
	let settings = {
		DOOR_OPEN_ANGLE: '',
		DOOR_OPEN_SPEED: '',
		DOOR_CLOSE_ANGLE: '',
		DOOR_CLOSE_SPEED: '',
		DOOR_CLOSE_WAIT: '',
		RFID_LIST: []
	};

	// RFID data
	let scannedRfids = [];
	let newRfidId = '';
	let newOwnerName = '';
	let newRfidAllowed = true;

	// Modals
	let restoreModal;
	let saveModal;

	// API Functions
	async function loadSettings() {
		try {
			isLoading = true;
			const response = await fetch('/api/settings');
			if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
			const data = await response.json();
			
			settings = {
				DOOR_OPEN_ANGLE: data.DOOR_OPEN_ANGLE ?? '',
				DOOR_OPEN_SPEED: data.DOOR_OPEN_SPEED ?? '',
				DOOR_CLOSE_ANGLE: data.DOOR_CLOSE_ANGLE ?? '',
				DOOR_CLOSE_SPEED: data.DOOR_CLOSE_SPEED ?? '',
				DOOR_CLOSE_WAIT: data.DOOR_CLOSE_WAIT ?? '',
				RFID_LIST: data.RFID_LIST || []
			};
			
			showSaveWarning = false;
		} catch (error) {
			console.error('Error loading settings:', error);
		} finally {
			isLoading = false;
		}
	}

	async function loadDoorStatus() {
		try {
			const response = await fetch('/api/door_state');
			if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
			const data = await response.json();
			
			doorState = data.door_state;
			doorStatusMessage = doorState === 'open' 
				? 'The door is currently open.' 
				: 'The door is currently closed.';
		} catch (error) {
			console.error('Error loading door status:', error);
		}
	}

	async function toggleDoor() {
		try {
			isLoading = true;
			const response = await fetch('/api/toggle_door', { method: 'POST' });
			if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
			
			// Refresh door status after toggle
			await loadDoorStatus();
		} catch (error) {
			console.error('Error toggling door:', error);
			alert('Failed to toggle door on server.');
		} finally {
			isLoading = false;
		}
	}

	async function fetchScannedRfids() {
		try {
			const response = await fetch('/api/last_rfids');
			if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
			const data = await response.json();
			scannedRfids = data.rfids || [];
		} catch (error) {
			console.error('Error fetching last RFIDs:', error);
		}
	}

	async function fetchSystemLog() {
		try {
			const response = await fetch('/api/logs');
			if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
			const data = await response.text();
			logContent = data;
		} catch (error) {
			console.error('Error fetching system log:', error);
		}
	}

	async function saveSettings() {
		try {
			isLoading = true;
			const response = await fetch('/api/settings', {
				method: 'POST',
				headers: { 'Content-Type': 'application/json' },
				body: JSON.stringify(settings)
			});
			
			if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
			showSaveWarning = false;
			saveModal.close();
		} catch (error) {
			console.error('Error saving settings:', error);
			alert('Failed to save settings.');
		} finally {
			isLoading = false;
		}
	}

	async function restoreSettings() {
		await loadSettings();
		restoreModal.close();
	}

	function addNewRfid() {
		if (!newRfidId.trim() || !newOwnerName.trim()) {
			alert('Please fill in both RFID ID and Owner Name.');
			return;
		}

		const newRfid = {
			ID: newRfidId.trim(),
			NAME: newOwnerName.trim(),
			ALLOWED: newRfidAllowed
		};

		settings.RFID_LIST = [...settings.RFID_LIST, newRfid];
		showSaveWarning = true;
		
		// Clear form
		newRfidId = '';
		newOwnerName = '';
		newRfidAllowed = true;
	}

	function removeRfid(index) {
		settings.RFID_LIST = settings.RFID_LIST.filter((_, i) => i !== index);
		showSaveWarning = true;
	}

	function importScannedRfid(rfidId) {
		const newRfid = {
			ID: rfidId,
			NAME: 'Unknown',
			ALLOWED: true
		};
		
		settings.RFID_LIST = [...settings.RFID_LIST, newRfid];
		showSaveWarning = true;
	}

	function isRfidAlreadyImported(rfidId) {
		return settings.RFID_LIST.some(rfid => rfid.ID === rfidId);
	}

	function onSettingsChange() {
		showSaveWarning = true;
	}

	function toggleLogs() {
		showLogs = !showLogs;
		if (showLogs) {
			fetchSystemLog();
		}
	}

	// Reactive statements for auto-save warning
	$: if (browser && (settings.DOOR_OPEN_ANGLE || settings.DOOR_OPEN_SPEED || 
		settings.DOOR_CLOSE_ANGLE || settings.DOOR_CLOSE_SPEED || settings.DOOR_CLOSE_WAIT)) {
		// Settings changed, but don't show warning on initial load
	}

	onMount(() => {
		loadSettings();
		loadDoorStatus();
		fetchScannedRfids();
		
		// Poll for scanned RFIDs every 60 seconds
		const interval = setInterval(fetchScannedRfids, 60000);
		
		return () => clearInterval(interval);
	});
</script>

<svelte:head>
	<title>Pet Feeder Control</title>
</svelte:head>

<!-- Loader -->
{#if isLoading}
	<div class="mt-4 flex items-center justify-center">
		<span class="loading loading-spinner loading-lg text-primary"></span>
		<p class="text-sm text-base-content ml-2">Loading...</p>
	</div>
{/if}

<!-- Navigation Bar -->
<div class="navbar bg-base-100 shadow-lg" data-theme="cupcake">
	<div class="navbar-start">
		<h1 class="text-2xl font-bold text-primary">Pet Feeder Control</h1>
	</div>
	<div class="navbar-end">
		<select bind:value={$locale} class="select select-bordered select-sm">
			<option value="pt">🇧🇷 PT</option>
			<option value="en">🇬🇧 EN</option>
			<option value="es">🇪🇸 ES</option>
		</select>
	</div>
</div>

<div class="container mx-auto p-4 max-w-4xl">
	<!-- Main Container -->
	<div class="bg-base-100 p-8 rounded-lg shadow-xl">
		
		<!-- Save Warning Message -->
		{#if showSaveWarning}
			<div class="mb-6 alert alert-warning">
				<svg xmlns="http://www.w3.org/2000/svg" class="stroke-current shrink-0 h-6 w-6" fill="none" viewBox="0 0 24 24">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-2.5L13.732 4c-.77-.833-1.964-.833-2.732 0L3.732 16c-.77.833.192 2.5 1.732 2.5z" />
				</svg>
				<span>Settings have been changed. Click "Save All Settings" to persist them.</span>
			</div>
		{/if}
		
		<!-- Door Control Panel -->
		<div class="mb-8 card bg-base-100 shadow-md relative">
			{#if isLoading}
				<!-- Overlay Loader -->
				<div class="absolute inset-0 bg-base-100 bg-opacity-70 flex items-center justify-center z-10">
					<span class="loading loading-spinner loading-lg text-primary"></span>
				</div>
			{/if}
			<div class="card-body text-center">
				<h2 class="card-title justify-center text-primary mb-4">Door Control</h2>
				<button 
					class="btn btn-circle btn-lg mx-auto mb-4 {doorState === 'open' ? 'btn-success' : 'btn-neutral'}"
					on:click={toggleDoor}
					disabled={isLoading}
				>
					{#if doorState === 'open'}
						<!-- Open Door Icon -->
						<svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" viewBox="0 0 24 24" width="40" height="40">
							<path d="M17 8h-1V7a5 5 0 0 0-10 0v1h2V7a3 3 0 0 1 6 0v2H6a2 2 0 0 0-2 2v9a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2v-9a2 2 0 0 0-2-2zm0 11H6v-9h11v9zm-5-4a1.5 1.5 0 0 0 1.5-1.5V14a1.5 1.5 0 0 0-3 0v1.5A1.5 1.5 0 0 0 12 15z"/>
						</svg>
					{:else}
						<!-- Closed Door Icon -->
						<svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" viewBox="0 0 24 24" width="40" height="40">
							<path d="M12 17a1.5 1.5 0 0 0 1.5-1.5V14a1.5 1.5 0 0 0-3 0v1.5A1.5 1.5 0 0 0 12 17zm6-8h-1V7a5 5 0 0 0-10 0v2H6a2 2 0 0 0-2 2v9a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2v-9a2 2 0 0 0-2-2zm-8-2a3 3 0 0 1 6 0v2h-6V7zm8 13H6v-9h12v9z"/>
						</svg>
					{/if}
				</button>

				<p class="text-base-content/70 mb-4">{doorStatusMessage}</p>
				<div>
					<button class="btn btn-outline btn-sm" on:click={loadDoorStatus}>Refresh</button>
				</div>
			</div>
		</div>

		<!-- Door Configuration -->
		<div class="mb-8 card bg-warning/10 border border-warning/20">
			<div class="card-body">
				<h2 class="card-title text-warning mb-4">Door Configuration</h2>
				<div class="grid grid-cols-1 md:grid-cols-2 gap-6">
					<div class="form-control">
						<label class="label" for="door-max-opening">
							<span class="label-text">Door Max Opening (mm)</span>
						</label>
						<input 
							type="number" 
							bind:value={settings.DOOR_OPEN_ANGLE}
							on:input={onSettingsChange}
							class="input input-bordered input-warning w-full"
						/>
					</div>
					<div class="form-control">
						<label class="label" for="door-opening-speed">
							<span class="label-text">Door Opening Speed (%)</span>
						</label>
						<input 
							type="number" 
							min="1" 
							max="100"
							bind:value={settings.DOOR_OPEN_SPEED}
							on:input={onSettingsChange}
							class="input input-bordered input-warning w-full"
						/>
					</div>
					<div class="form-control">
						<label class="label" for="door-max-closing">
							<span class="label-text">Door Max Closing (mm)</span>
						</label>
						<input 
							type="number" 
							bind:value={settings.DOOR_CLOSE_ANGLE}
							on:input={onSettingsChange}
							class="input input-bordered input-warning w-full"
						/>
					</div>
					<div class="form-control">
						<label class="label" for="door-closing-speed">
							<span class="label-text">Door Closing Speed (%)</span>
						</label>
						<input 
							type="number" 
							min="1" 
							max="100"
							bind:value={settings.DOOR_CLOSE_SPEED}
							on:input={onSettingsChange}
							class="input input-bordered input-warning w-full"
						/>
					</div>
					<div class="form-control">
						<label class="label" for="delay-to-close">
							<span class="label-text">Delay to Close (seconds)</span>
						</label>
						<input 
							type="number" 
							min="0"
							bind:value={settings.DOOR_CLOSE_WAIT}
							on:input={onSettingsChange}
							class="input input-bordered input-warning w-full"
						/>
					</div>
				</div>

				<!-- Collapsible Helper -->
				<div class="collapse collapse-arrow bg-warning/5 border border-warning/10 mt-4">
					<input type="checkbox" class="peer" />
					<div class="collapse-title text-sm font-medium text-warning">
						How Door Configuration Works
					</div>
					<div class="collapse-content text-xs text-base-content/70">
						<p class="mb-1"><strong>Door Max Opening (mm):</strong> Sets the maximum distance the door can open from its closed position.</p>
						<p class="mb-1"><strong>Door Opening Speed (%):</strong> Controls how fast the door opens (1-100%, higher is faster).</p>
						<p class="mb-1"><strong>Door Max Closing (mm):</strong> Sets the maximum distance the door can close from its fully open position.</p>
						<p class="mb-1"><strong>Door Closing Speed (%):</strong> Controls how fast the door closes (1-100%, higher is faster).</p>
						<p><strong>Delay to Close (seconds):</strong> Specifies the time, in seconds, before the door automatically begins to close after being opened. Set to 0 for no auto-close.</p>
					</div>
				</div>
			</div>
		</div>

		<!-- Allowed RFID List -->
		<div class="mb-8 card bg-success/10 border border-success/20">
			<div class="card-body">
				<h2 class="card-title text-success mb-4">RFID Tags</h2>
				<div class="space-y-4">
					{#each settings.RFID_LIST as rfid, index}
						<div class="flex items-center justify-between bg-white px-3 py-2 rounded shadow border">
							<div class="flex-1">
								<span class="font-semibold text-base-content">ID: {rfid.ID}</span>
								<span class="text-sm text-base-content/70 ml-2">({rfid.NAME})</span>
								<span class="badge {rfid.ALLOWED ? 'badge-success' : 'badge-error'} badge-sm ml-2">
									{rfid.ALLOWED ? 'Allowed' : 'Blocked'}
								</span>
							</div>
							<button 
								class="btn btn-error btn-sm"
								on:click={() => removeRfid(index)}
							>
								Remove
							</button>
						</div>
					{/each}
				</div>
				
				<div class="mt-6 p-4 bg-base-200 rounded-lg">
					<h3 class="text-lg font-semibold text-base-content mb-4">Add New RFID</h3>
					<div class="grid grid-cols-1 md:grid-cols-3 gap-4 mb-4">
						<div class="form-control">
							<label class="label">
								<span class="label-text">RFID ID</span>
							</label>
							<input 
								type="text" 
								bind:value={newRfidId}
								placeholder="Enter RFID ID"
								class="input input-bordered input-success w-full"
							/>
						</div>
						<div class="form-control">
							<label class="label">
								<span class="label-text">Owner Name</span>
							</label>
							<input 
								type="text" 
								bind:value={newOwnerName}
								placeholder="Enter owner name"
								class="input input-bordered input-success w-full"
							/>
						</div>
						<div class="form-control flex flex-row items-center mt-6 md:mt-0">
							<label class="label cursor-pointer">
								<span class="label-text mr-2">Allowed</span>
								<input type="checkbox" bind:checked={newRfidAllowed} class="toggle toggle-success" />
							</label>
						</div>
					</div>
					<button class="btn btn-success w-full" on:click={addNewRfid}>Add RFID</button>
				</div>
			</div>
		</div>

		<!-- Real-time RFID Log -->
		<div class="mb-8 card bg-primary/10 border border-primary/20">
			<div class="card-body">
				<h2 class="card-title text-primary mb-4">Scanned RFIDs</h2>
				<div class="space-y-4">
					{#each scannedRfids as rfid}
						<div class="flex items-center justify-between bg-white px-3 py-2 rounded shadow border border-gray-200 text-xs">
							<span class="truncate">RFID ID: <span class="font-bold text-base-content">{rfid.id}</span></span>
							{#if isRfidAlreadyImported(rfid.id)}
								<span class="text-success">Already imported</span>
							{:else}
								<button 
									class="btn btn-ghost btn-sm ml-2 flex items-center gap-1"
									on:click={() => importScannedRfid(rfid.id)}
								>
									<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-4 h-4">
										<path stroke-linecap="round" stroke-linejoin="round" d="M7.5 7.5h-.75A2.25 2.25 0 0 0 4.5 9.75v7.5a2.25 2.25 0 0 0 2.25 2.25h7.5a2.25 2.25 0 0 0 2.25-2.25v-7.5a2.25 2.25 0 0 0-2.25-2.25h-.75m0-3-3-3m0 0-3 3m3-3v11.25m6-2.25h.75a2.25 2.25 0 0 1 2.25 2.25v7.5a2.25 2.25 0 0 1-2.25 2.25h-7.5a2.25 2.25 0 0 1-2.25-2.25v-.75" />
									</svg>
									Import
								</button>
							{/if}
						</div>
					{/each}
				</div>
				<div>
					<button class="btn btn-ghost btn-sm mt-2" on:click={fetchScannedRfids}>Refresh</button>
				</div>
			</div>
		</div>

		<!-- System Log -->
		<div class="mb-8 card bg-base-200 border border-base-300">
			<div class="card-body">
				<div class="flex justify-between items-center mb-4">
					<h2 class="card-title text-base-content">System Log</h2>
					<button class="btn btn-outline btn-sm" on:click={toggleLogs}>
						{showLogs ? 'Hide Logs' : 'View Logs'}
					</button>
				</div>
				{#if showLogs}
					<div>
						<textarea 
							readonly
							bind:value={logContent}
							class="textarea textarea-bordered w-full h-48 bg-neutral text-neutral-content font-mono text-sm resize-none"
						></textarea>
						<button class="btn btn-ghost btn-sm mt-2" on:click={fetchSystemLog}>Refresh</button>
					</div>
				{/if}
			</div>
		</div>

		<!-- Save Settings Buttons -->
		<div class="mt-8 flex flex-col md:flex-row gap-4">
			<button 
				class="md:flex-none md:w-1/6 btn-soft hover:bg-base-600 font-bold py-3 px-6 rounded-md shadow-lg transition duration-300 ease-in-out transform hover:scale-105"
				on:click={() => restoreModal.showModal()}
			>
				Restore
			</button>
			<div class="flex-1 flex flex-row justify-end gap-4">
				<button class="md:w-32 btn btn-outline btn-info font-semibold">
					Test Changes
				</button>
				<button 
					class="md:w-40 py-3 px-6 shadow-lg transition duration-300 transform hover:scale-105 btn btn-primary bg-indigo-600 hover:bg-indigo-700 text-white ease-in-out font-bold"
					on:click={() => saveModal.showModal()}
				>
					Save Settings
				</button>
			</div>
		</div>

		<!-- Restore Confirmation Modal -->
		<dialog bind:this={restoreModal} class="modal">
			<div class="modal-box">
				<h3 class="font-bold text-lg text-warning mb-2">Restore Settings?</h3>
				<p class="mb-4">Are you sure you want to restore the previous settings? All unsaved changes will be lost.</p>
				<div class="flex justify-end gap-2">
					<button class="btn btn-ghost" on:click={() => restoreModal.close()}>Cancel</button>
					<button class="btn btn-warning" on:click={restoreSettings}>Yes, Restore</button>
				</div>
			</div>
		</dialog>

		<!-- Save Confirmation Modal -->
		<dialog bind:this={saveModal} class="modal">
			<div class="modal-box">
				<h3 class="font-bold text-lg text-primary mb-2">Save Settings?</h3>
				<p class="mb-4">Are you sure you want to save permanently the current settings? This will overwrite the previous configuration.</p>
				<div class="flex justify-end gap-2">
					<button class="btn btn-ghost" on:click={() => saveModal.close()}>Cancel</button>
					<button class="btn btn-primary" on:click={saveSettings}>Yes, Save</button>
				</div>
			</div>
		</dialog>
	</div>
</div>

<style>
	/* Custom scrollbar for log areas */
	:global(textarea::-webkit-scrollbar) {
		width: 8px;
	}

	:global(textarea::-webkit-scrollbar-track) {
		background: #f1f1f1;
		border-radius: 10px;
	}

	:global(textarea::-webkit-scrollbar-thumb) {
		background: #888;
		border-radius: 10px;
	}

	:global(textarea::-webkit-scrollbar-thumb:hover) {
		background: #555;
	}

	/* Make the main buttons less tall and more compact */
	:global(#restore-settings-btn),
	:global(#test-settings-btn),
	:global(#save-settings-btn) {
		padding-top: 0.5rem;
		padding-bottom: 0.5rem;
		font-size: 1rem;
	}

	/* Make Restore button more subtle */
	.btn-soft {
		background-color: transparent;
		color: #f59e42;
		border: 1px solid #fbbf24;
		box-shadow: none;
	}

	.btn-soft:hover {
		background-color: #fbbf24;
		color: #fff;
		border-color: #f59e42;
	}
</style>