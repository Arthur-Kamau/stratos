/**
 * Stratos DevTools - Main Controller
 * Connection manager, tab switching, event dispatcher
 */

const DevTools = (() => {
    const DEVTOOLS_PORT = 9222;
    const POLL_INTERVAL = 100;

    let baseUrl = `http://localhost:${DEVTOOLS_PORT}`;
    let pollTimer = null;
    let connected = false;
    let rpcId = 1;
    let lastSeenEventId = 0;
    const eventHandlers = {};

    // --- Event System ---
    function on(event, handler) {
        if (!eventHandlers[event]) eventHandlers[event] = [];
        eventHandlers[event].push(handler);
    }

    function emit(event, data) {
        if (eventHandlers[event]) {
            eventHandlers[event].forEach(h => {
                try { h(data); } catch (e) { console.error(`Handler error for ${event}:`, e); }
            });
        }
    }

    // --- JSON-RPC ---
    async function call(method, params = {}) {
        const id = rpcId++;
        try {
            const resp = await fetch(baseUrl, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ jsonrpc: '2.0', id, method, params })
            });
            const json = await resp.json();
            if (json.error) throw new Error(json.error.message || 'RPC error');
            return json.result;
        } catch (e) {
            if (connected) console.warn(`RPC call ${method} failed:`, e.message);
            throw e;
        }
    }

    // --- Event Polling ---
    async function pollEvents() {
        try {
            const resp = await fetch(`${baseUrl}/events`);
            const events = await resp.json();

            if (!connected) {
                connected = true;
                updateConnectionStatus('connected');
                emit('connected');
            }

            if (Array.isArray(events)) {
                events.forEach(evt => {
                    if (evt.id > lastSeenEventId) {
                        lastSeenEventId = evt.id;
                        emit(evt.method, evt.params);
                    }
                });
            }
        } catch (e) {
            if (connected) {
                connected = false;
                updateConnectionStatus('disconnected');
                emit('disconnected');
            }
        }
    }

    function startPolling() {
        if (pollTimer) return;
        updateConnectionStatus('connecting');
        pollTimer = setInterval(pollEvents, POLL_INTERVAL);
        pollEvents();
    }

    function stopPolling() {
        if (pollTimer) {
            clearInterval(pollTimer);
            pollTimer = null;
        }
    }

    // --- Connection Status ---
    function updateConnectionStatus(status) {
        const el = document.getElementById('connection-status');
        el.className = `status ${status}`;
        el.textContent = status.charAt(0).toUpperCase() + status.slice(1);
    }

    // --- Tab Switching ---
    function initTabs() {
        const tabs = document.querySelectorAll('.tab-btn');
        tabs.forEach(tab => {
            tab.addEventListener('click', () => {
                const target = tab.dataset.tab;
                tabs.forEach(t => t.classList.remove('active'));
                tab.classList.add('active');
                document.querySelectorAll('.panel').forEach(p => p.style.display = 'none');
                const panel = document.getElementById(`panel-${target}`);
                if (panel) panel.style.display = '';
                emit('tabChanged', target);
            });
        });
    }

    // --- Theme Toggle ---
    function initTheme() {
        const saved = localStorage.getItem('stratos-devtools-theme');
        if (saved === 'light') document.body.setAttribute('data-theme', 'light');

        document.getElementById('theme-toggle').addEventListener('click', () => {
            const isLight = document.body.getAttribute('data-theme') === 'light';
            if (isLight) {
                document.body.removeAttribute('data-theme');
                localStorage.setItem('stratos-devtools-theme', 'dark');
            } else {
                document.body.setAttribute('data-theme', 'light');
                localStorage.setItem('stratos-devtools-theme', 'light');
            }
        });
    }

    // --- Utility ---
    function formatBytes(bytes) {
        if (bytes === 0) return '0 B';
        const k = 1024;
        const sizes = ['B', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        return parseFloat((bytes / Math.pow(k, i)).toFixed(1)) + ' ' + sizes[i];
    }

    function formatTime(ms) {
        if (ms < 1) return '<1 ms';
        if (ms < 1000) return ms.toFixed(1) + ' ms';
        return (ms / 1000).toFixed(2) + ' s';
    }

    function formatTimestamp(epochMs) {
        const d = new Date(epochMs);
        return d.toLocaleTimeString('en-US', { hour12: false }) + '.' +
               String(d.getMilliseconds()).padStart(3, '0');
    }

    function escapeHtml(str) {
        const div = document.createElement('div');
        div.textContent = str;
        return div.innerHTML;
    }

    // --- Init ---
    function init() {
        initTabs();
        initTheme();
        startPolling();
    }

    document.addEventListener('DOMContentLoaded', init);

    return {
        call,
        on,
        emit,
        formatBytes,
        formatTime,
        formatTimestamp,
        escapeHtml,
        get connected() { return connected; }
    };
})();
