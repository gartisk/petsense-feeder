document.addEventListener('DOMContentLoaded', () => {
    // --- Element Selectors ---



    const dispenserScheduleList = document.getElementById('dispenser-schedule-list');

    const timeIntervalsContainer = document.getElementById('time-intervals-container');
    const rfidRequiredCheckbox = document.getElementById('rfid-required-checkbox');



    const newRfidIdInput = document.getElementById('new-rfid-id');
    const newOwnerNameInput = document.getElementById('new-owner-name');


    // --- LOAD APP SETTINGS ---
    const saveWarningDiv = document.getElementById('save-warning');
    const doorMaxOpeningInput = document.getElementById('door-max-opening');
    const doorOpeningSpeedInput = document.getElementById('door-opening-speed');
    const doorMaxClosingInput = document.getElementById('door-max-closing');
    const doorClosingSpeedInput = document.getElementById('door-closing-speed');
    const delayToCloseInput = document.getElementById('delay-to-close');

    const rfidListContainer = document.getElementById('rfid-list-container');

    const fnLoadSuccess = function (settings) {
        doorMaxOpeningInput.value = settings.DOOR_OPEN_ANGLE ?? '';
        doorOpeningSpeedInput.value = settings.DOOR_OPEN_SPEED ?? '';
        doorMaxClosingInput.value = settings.DOOR_CLOSE_ANGLE ?? '';
        doorClosingSpeedInput.value = settings.DOOR_CLOSE_SPEED ?? '';
        delayToCloseInput.value = settings.DOOR_CLOSE_WAIT ?? '';

        // Allowed RFID tags
        if (settings.RFID_LIST) {
            // Remove all except the add-new form
            rfidListContainer.querySelectorAll('.flex').forEach(e => e.remove());
            settings.RFID_LIST.forEach(rfid => {
                rfidListContainer.appendChild(createRfidListItem(rfid.ID, rfid.NAME, rfid.ALLOWED));
            });
        }

        hideSaveWarning();
    };

    // Action load settings from the server
    async function loadSettings(fnSuccess) {
        try {
            const response = await fetch('/api/settings');

            if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
            const settings = await response.json();

            fnSuccess(settings);

        } catch (error) {
            console.error('Error loading settings:', error);
        }
    }


    // --- Load Door Status ---
    const doorToggleButton = document.getElementById('doorToggleButton');
    const doorStatusMessage = document.getElementById('doorStatusMessage');
    const refreshDoorBtn = document.getElementById('refresh-door-btn');


    /**
     * Updates the UI based on the door's current state.
     */
    function closeDoorUI() {
        const closedDoorIcon = `
                    <svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" viewBox="0 0 24 24" width="40" height="40">
                        <path d="M12 17a1.5 1.5 0 0 0 1.5-1.5V14a1.5 1.5 0 0 0-3 0v1.5A1.5 1.5 0 0 0 12 17zm6-8h-1V7a5 5 0 0 0-10 0v2H6a2 2 0 0 0-2 2v9a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2v-9a2 2 0 0 0-2-2zm-8-2a3 3 0 0 1 6 0v2h-6V7zm8 13H6v-9h12v9z"/>
                    </svg>
                `;

        doorToggleButton.innerHTML = closedDoorIcon; // Set closed door icon
        doorToggleButton.classList.remove('btn-success');
        doorToggleButton.classList.add('btn-neutral');
        doorStatusMessage.textContent = 'The door is currently closed.';
    }

    function openDoorUI() {
        const openDoorIcon = `
                    <svg xmlns="http://www.w3.org/2000/svg" fill="currentColor" viewBox="0 0 24 24" width="40" height="40">
                        <path d="M17 8h-1V7a5 5 0 0 0-10 0v1h2V7a3 3 0 0 1 6 0v2H6a2 2 0 0 0-2 2v9a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2v-9a2 2 0 0 0-2-2zm0 11H6v-9h11v9zm-5-4a1.5 1.5 0 0 0 1.5-1.5V14a1.5 1.5 0 0 0-3 0v1.5A1.5 1.5 0 0 0 12 15z"/>
                    </svg>
                `;

        doorToggleButton.innerHTML = openDoorIcon; // Set open door icon
        doorToggleButton.classList.remove('btn-neutral');
        doorToggleButton.classList.add('btn-success');
        doorStatusMessage.textContent = 'The door is currently open.';
    }

    const fnDoorStatusSuccess = function (data) {
        const doorToggleButton = document.getElementById('doorToggleButton');
        const doorStatusMessage = document.getElementById('doorStatusMessage');

        if (data.door_state == "open") {
            openDoorUI();
            return;
        }

        closeDoorUI();

    };

    // Action Door Status
    async function loadDoorStatus(fnSuccess) {
        try {
            const response = await fetch('/api/door_state');
            if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);

            const data = await response.json();
            fnSuccess(data);

        } catch (error) {
            console.error('Error loading door status:', error);
        }
    }

    // Action Door Toggle
    async function toggleDoorOnServer(fnSuccess) {
        try {
            const response = await fetch('/api/toggle_door', { method: 'POST' });
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            const data = await response.json();
            console.log('Server response:', data);
            fnSuccess(data);
        } catch (error) {
            console.error('Error toggling door on server:', error);
            alert('Failed to toggle door on server.');
        }
    }

    // --- RFIDS Scanneds ---
    let lastRfidEntries = [];
    const scannedRfidsContainer = document.getElementById('last-rfid-list-container');

    // const fnLastRFidSuccess = function (settings) {
    //     settings.rfids.forEach(
    //         rfid => {
    //             const rfidDiv = createRFIDScanned(rfid.ID);
    //             scannedRfidsContainer.appendChild(rfidDiv);
    //         }
    //     );
    // };

    const rfidImportBtn = `<button class="import-rfid-btn btn btn-ghost btn-md ml-2 flex items-center gap-1">
                            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-4 h">
                                <path stroke-linecap="round" stroke-linejoin="round" d="M7.5 7.5h-.75A2.25 2.25 0 0 0 4.5 9.75v7.5a2.25 2.25 0 0 0 2.25 2.25h7.5a2.25 2.25 0 0 0 2.25-2.25v-7.5a2.25 2.25 0 0 0-2.25-2.25h-.75m0-3-3-3m0 0-3 3m3-3v11.25m6-2.25h.75a2.25 2.25 0 0 1 2.25 2.25v7.5a2.25 2.25 0 0 1-2.25 2.25h-7.5a2.25 2.25 0 0 1-2.25-2.25v-.75" />
                            </svg>
                            Import
                        </button>`;
    const rfidAlreadyImported = `<span class="text-success">Already imported</span>`;

    function createScannedRfid(id) {
        const rfidDiv = document.createElement('div');
        const rfidOption = lastRfidEntries.find(r => r.id === id) ? rfidAlreadyImported : rfidImportBtn;

        rfidDiv.className = 'flex items-center justify-between bg-white px-3 py-2 rounded shadow border border-gray-200 text-xs';
        rfidDiv.innerHTML = `
                        <span class="truncate">RFID ID: <span class="font-bold text-base-content">${id}</span></span>
                        ${rfidOption}
                `;
        // rfidDiv.querySelector('.import-rfid-btn').addEventListener('click', () => {
        //      rfidDiv.remove();
        // });

        return rfidDiv;
    }

    const fnScannedRfidSuccess = function (data) {

        // Allowed RFID tags
        if (data.rfids) {
            // Remove all except the add-new form
            scannedRfidsContainer.querySelectorAll('.flex').forEach(e => e.remove());

            data.rfids.forEach(rfid => {
                scannedRfidsContainer.appendChild(createScannedRfid(rfid.id));
            });
        }
    };



    async function fetchLastRfids(fnSuccess) {
        try {
            const response = await fetch('/api/last_rfids');
            if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);

            const data = await response.json();
            fnSuccess(data);
        } catch (error) {
            console.error('Error fetching last RFIDs:', error);
        }
    }

    // Poll every 5 seconds
    setInterval(() => fetchLastRfids(fnScannedRfidSuccess), 60000);
    fetchLastRfids(fnScannedRfidSuccess);


    // --- System Log Management ---
    const toggleLogBtn = document.getElementById('toggle-log-btn');
    const logContainer = document.getElementById('log-container');
    const logContent = document.getElementById('log-content');
    const refreshLogBtn = document.getElementById('refresh-log-btn');

    async function fetchLogs() {
        try {
            const response = await fetch('/api/logs');
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            const logText = await response.text();
            logContent.value = logText;

            // scrolldown to the bottom of the log content
            logContent.scrollTop = logContent.scrollHeight;
        } catch (error) {
            console.error('Error fetching logs:', error);
            logContent.value = 'Error fetching logs.';
        }
    }


    // INIT HERE
    // All Settings begin here:
    loadSettings(fnLoadSuccess);

    // Verify Door Status
    loadDoorStatus(fnDoorStatusSuccess);


    // EVENT LISTENERS

    // DOOR STATUS
    refreshDoorBtn.addEventListener('click', () => {
        loadDoorStatus(fnDoorStatusSuccess);
    });

    // DOOR TOGGLE
    doorToggleButton.addEventListener('click', function () {
        toggleDoorOnServer(fnDoorStatusSuccess);
    });

    // LOGS UI
    toggleLogBtn.addEventListener('click', () => {
        const isHidden = logContainer.classList.toggle('hidden');
        if (!isHidden) {
            fetchLogs();
        }
    });

    // LOG REFRESH
    refreshLogBtn.addEventListener('click', fetchLogs);












    // --- Save Warning ---
    function showSaveWarning() {
        saveWarningDiv.classList.remove('hidden');
    }

    function hideSaveWarning() {
        saveWarningDiv.classList.add('hidden');
    }

    // Attach listeners to all inputs to show warning on change
    document.querySelectorAll('input, select, textarea').forEach(input => {
        input.addEventListener('input', showSaveWarning);
    });


    // --- RFID List Management ---
    function createRfidListItem(rfidId, ownerName, isAllowed) {
        const rfidDiv = document.createElement('div');
        rfidDiv.className = 'flex flex-col md:flex-row items-center bg-white p-4 rounded-md shadow-sm border border-gray-200';
        rfidDiv.innerHTML = `
                    <div class="flex-grow w-full md:w-auto mb-2 md:mb-0">
                        <p class="text-base-content/70 text-sm font-medium">RFID ID: <span class="font-bold text-base-content">${rfidId}</span></p>
                        <p class="text-base-content/70 text-sm font-medium">Owner: <span class="text-base-content">${ownerName}</span></p>
                        <p class="text-base-content/70 text-sm font-medium">Allowed: <span class="text-success">${isAllowed ? 'Yes' : 'No'}</span></p>
                    </div>
                    <button class="remove-rfid-btn btn-ghost-secondary w-full md:w-auto">
                        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-6">
                            <path stroke-linecap="round" stroke-linejoin="round" d="m14.74 9-.346 9m-4.788 0L9.26 9m9.968-3.21c.342.052.682.107 1.022.166m-1.022-.165L18.16 19.673a2.25 2.25 0 0 1-2.244 2.077H8.084a2.25 2.25 0 0 1-2.244-2.077L4.772 5.79m14.456 0a48.108 48.108 0 0 0-3.478-.397m-12 .562c.34-.059.68-.114 1.022-.165m0 0a48.11 48.11 0 0 1 3.478-.397m7.5 0v-.916c0-1.18-.91-2.164-2.09-2.201a51.964 51.964 0 0 0-3.32 0c-1.18.037-2.09 1.022-2.09 2.201v.916m7.5 0a48.667 48.667 0 0 0-7.5 0" />
                        </svg>
                    </button>
                `;
        rfidDiv.querySelector('.remove-rfid-btn').addEventListener('click', () => {
            rfidDiv.remove();
            showSaveWarning();
        });
        return rfidDiv;
    }

    const addRfidBtn = document.getElementById('add-rfid-btn');
    addRfidBtn.addEventListener('click', () => {
        const rfidId = newRfidIdInput.value.trim();
        const ownerName = newOwnerNameInput.value.trim();
        if (rfidId && ownerName) {
            rfidListContainer.appendChild(createRfidListItem(rfidId, ownerName));
            newRfidIdInput.value = '';
            newOwnerNameInput.value = '';
            showSaveWarning();
        } else {
            alert('Please enter both RFID ID and Owner Name.');
        }
    });

    document.querySelectorAll('.remove-rfid-btn').forEach(button => {
        button.addEventListener('click', (event) => {
            event.target.closest('.flex').remove();
            showSaveWarning();
        });
    });

    function mountSettings(){
        const rfidEntries = [];
        rfidListContainer.querySelectorAll('.flex').forEach(item => {
            const rfidId = item.querySelector('p:nth-child(1) span').textContent;
            const ownerName = item.querySelector('p:nth-child(2) span').textContent;
            const isAllowed = item.querySelector('p:nth-child(3) span').textContent === 'Yes' ? "true" : "false";
            rfidEntries.push({ ID: rfidId, NAME: ownerName, ALLOWED: isAllowed });
        });

        const settings = {
            DOOR_OPEN_ANGLE: doorMaxOpeningInput.value,
            DOOR_CLOSE_ANGLE: doorMaxClosingInput.value,
            DOOR_OPEN_SPEED: doorOpeningSpeedInput.value,
            DOOR_CLOSE_SPEED: doorClosingSpeedInput.value,
            DOOR_CLOSE_WAIT: delayToCloseInput.value,
            RFID_LIST: rfidEntries
        };

        return settings;
    }

    // --- Restore Settings ---
    const restoreSettingsBtn = document.getElementById('restore-settings-btn');
    // Show save confirm modal when save button is clicked
    restoreSettingsBtn.addEventListener('click', () => {
        document.getElementById('restore-confirm-modal').showModal();
    });

    document.getElementById('confirm-restore-btn').addEventListener('click', () => {
       loadSettings(function(settings){
        fnLoadSuccess(settings);
        document.getElementById('restore-confirm-modal').close();
      });
    });

    // --- Test Settings ---
    const testSettingsBtn = document.getElementById('test-settings-btn');
    testSettingsBtn.addEventListener('click', () => {
        const settings = mountSettings();

        console.log('Testing settings:', settings);
        fetch('/api/settings', {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(settings)

        }).then(response => {
            if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
            return response.json();
        })
            .then(data => {
                console.log('Server response:', data);
                hideSaveWarning();
            })
            .catch(error => {
                console.error('Error testing settings:', error);
                alert(`Failed to test settings: ${error.message}.`);
            });
    });

    
    

    const saveSettingsBtn = document.getElementById('save-settings-btn');
    // Show save confirm modal when save button is clicked
    saveSettingsBtn.addEventListener('click', () => {
        document.getElementById('save-confirm-modal').showModal();
    });

    // --- Save Settings ---
    // Handle confirm save in modal
    const confirmSaveBtn = document.getElementById('confirm-save-btn');
    confirmSaveBtn.addEventListener('click', () => {

        const settings = mountSettings();

        console.log('Settings to save:', settings);

        fetch('/api/settings', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(settings)
        })
        .then(response => {
            if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
            return response.json();
        })
        .then(data => {
            console.log('Server response:', data);
            
            document.getElementById('save-confirm-modal').close();
            hideSaveWarning();
            
        })
        .catch(error => {
            console.error('Error saving settings:', error);
            alert(`Failed to save settings: ${error.message}.`);
        });
    });
    

});