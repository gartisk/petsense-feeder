import { onMount } from 'svelte';
import { browser } from '$app/environment';

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
