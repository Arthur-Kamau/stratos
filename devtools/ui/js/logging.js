/**
 * Stratos DevTools - Logging View
 * Real-time log display with filtering, search, and export
 */
(() => {
    const entries = [];
    let selectedEntry = null;
    let autoScroll = true;

    const logEntriesEl = document.getElementById('log-entries');
    const logDetailEl = document.getElementById('log-detail');
    const logDetailBody = document.getElementById('log-detail-body');
    const levelFilter = document.getElementById('log-level-filter');
    const searchInput = document.getElementById('log-search');

    // --- Event Handlers ---
    DevTools.on('Log.entryAdded', (params) => {
        if (!params || !params.entry) return;
        const entry = params.entry;
        entries.push(entry);
        if (entries.length > 10000) entries.shift();
        if (matchesFilter(entry)) {
            appendLogEntry(entry);
        }
    });

    DevTools.on('connected', () => {
        DevTools.call('Log.enable').catch(() => {});
    });

    // --- Filtering ---
    function matchesFilter(entry) {
        const level = levelFilter.value;
        if (level && entry.level !== level) return false;
        const search = searchInput.value.toLowerCase();
        if (search && !entry.message.toLowerCase().includes(search)) return false;
        return true;
    }

    function refilter() {
        logEntriesEl.innerHTML = '';
        entries.forEach(e => {
            if (matchesFilter(e)) appendLogEntry(e);
        });
    }

    levelFilter.addEventListener('change', refilter);
    searchInput.addEventListener('input', refilter);

    // --- Rendering ---
    function appendLogEntry(entry) {
        const row = document.createElement('div');
        row.className = 'log-entry';
        row.innerHTML = `
            <span class="log-time">${DevTools.formatTimestamp(entry.timestamp)}</span>
            <span class="log-level ${entry.level}">${entry.level}</span>
            <span class="log-message">${DevTools.escapeHtml(entry.message)}</span>
            ${entry.source && entry.source.file ?
                `<span class="log-source">${entry.source.file}:${entry.source.line}</span>` : ''}
        `;
        row.addEventListener('click', () => showDetail(entry, row));
        logEntriesEl.appendChild(row);

        if (autoScroll) {
            logEntriesEl.scrollTop = logEntriesEl.scrollHeight;
        }
    }

    function showDetail(entry, row) {
        document.querySelectorAll('.log-entry.selected').forEach(el => el.classList.remove('selected'));
        row.classList.add('selected');
        selectedEntry = entry;

        const detail = {
            timestamp: new Date(entry.timestamp).toISOString(),
            level: entry.level,
            message: entry.message,
        };
        if (entry.source) detail.source = entry.source;
        if (entry.data) detail.data = entry.data;

        logDetailBody.textContent = JSON.stringify(detail, null, 2);
        logDetailEl.style.display = '';
    }

    // --- Controls ---
    document.getElementById('log-clear-btn').addEventListener('click', () => {
        entries.length = 0;
        logEntriesEl.innerHTML = '';
        logDetailEl.style.display = 'none';
        DevTools.call('Log.clear').catch(() => {});
    });

    document.getElementById('log-detail-close').addEventListener('click', () => {
        logDetailEl.style.display = 'none';
        document.querySelectorAll('.log-entry.selected').forEach(el => el.classList.remove('selected'));
    });

    document.getElementById('log-autoscroll').addEventListener('change', (e) => {
        autoScroll = e.target.checked;
    });

    document.getElementById('log-export-btn').addEventListener('click', () => {
        const data = entries.map(e =>
            `[${DevTools.formatTimestamp(e.timestamp)}] [${e.level}] ${e.message}` +
            (e.source && e.source.file ? ` (${e.source.file}:${e.source.line})` : '')
        ).join('\n');
        const blob = new Blob([data], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = `stratos-logs-${Date.now()}.txt`;
        a.click();
        URL.revokeObjectURL(url);
    });
})();
