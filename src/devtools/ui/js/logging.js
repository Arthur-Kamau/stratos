// Logging View JavaScript

class LoggingView {
    constructor() {
        this.logs = [];
        this.filteredLogs = [];
        this.selectedLog = null;
        this.searchTerm = '';
        this.currentLevelFilter = 'all';
        this.receivedEventIds = new Set();  // Track processed event IDs

        this.init();
    }

    init() {
        // Get DOM elements
        this.logContainer = document.getElementById('logContainer');
        this.logEntries = document.getElementById('logEntries');
        this.logDetails = document.getElementById('logDetails');
        this.detailsContent = document.getElementById('detailsContent');
        this.searchInput = document.getElementById('logSearch');
        this.levelFilterSelect = document.getElementById('levelFilter');
        this.clearBtn = document.getElementById('clearLogs');
        this.exportBtn = document.getElementById('exportLogs');
        this.closeDetailsBtn = document.getElementById('closeDetails');

        // Setup event listeners
        this.searchInput.addEventListener('input', (e) => {
            this.searchTerm = e.target.value.toLowerCase();
            this.filterLogs();
        });

        this.levelFilterSelect.addEventListener('change', (e) => {
            this.currentLevelFilter = e.target.value;
            this.filterLogs();
        });

        this.clearBtn.addEventListener('click', () => {
            this.clearLogs();
        });

        this.exportBtn.addEventListener('click', () => {
            this.exportLogs();
        });

        this.closeDetailsBtn.addEventListener('click', () => {
            this.hideDetails();
        });

        // Show empty state initially
        this.renderLogs();
    }

    handleEvent(event, params, eventId) {
        console.log('[LoggingView] Event received:', event, params, 'eventId:', eventId);

        // Skip duplicate events based on ID
        if (eventId !== undefined && this.receivedEventIds.has(eventId)) {
            console.log('[LoggingView] Skipping duplicate event ID:', eventId);
            return;
        }

        // Mark event as processed
        if (eventId !== undefined) {
            this.receivedEventIds.add(eventId);
        }

        switch (event) {
            case 'entryAdded':
                this.addLogEntry(params.entry);
                break;
            default:
                console.log('[LoggingView] Unknown log event:', event, params);
        }
    }

    addLogEntry(entry) {
        console.log('[LoggingView] Adding log entry:', entry);
        this.logs.push(entry);
        console.log('[LoggingView] Total logs:', this.logs.length);
        this.filterLogs();
        this.scrollToBottom();
    }

    filterLogs() {
        this.filteredLogs = this.logs.filter(log => {
            // Level filter
            if (this.currentLevelFilter !== 'all' && log.level !== this.currentLevelFilter) {
                return false;
            }

            // Search filter
            if (this.searchTerm) {
                const searchable = [
                    log.message,
                    log.source?.file || '',
                    log.source?.function || ''
                ].join(' ').toLowerCase();

                if (!searchable.includes(this.searchTerm)) {
                    return false;
                }
            }

            return true;
        });

        this.renderLogs();
    }

    renderLogs() {
        if (this.filteredLogs.length === 0) {
            this.logEntries.innerHTML = `
                <tr>
                    <td colspan="4" class="log-empty">
                        <p>No log entries to display</p>
                    </td>
                </tr>
            `;
            return;
        }

        const html = this.filteredLogs.map((log, index) => {
            const time = this.formatTime(log.timestamp);
            const source = this.formatSource(log.source);
            const message = this.escapeHtml(log.message);

            return `
                <tr class="log-entry" data-index="${index}" onclick="window.loggingView.selectLog(${index})">
                    <td class="log-time">${time}</td>
                    <td><span class="log-level ${log.level}">${log.level}</span></td>
                    <td class="log-source">${source}</td>
                    <td class="log-message">${message}</td>
                </tr>
            `;
        }).join('');

        this.logEntries.innerHTML = html;
    }

    selectLog(index) {
        const log = this.filteredLogs[index];
        if (!log) return;

        this.selectedLog = log;
        this.showDetails(log);

        // Highlight selected row
        document.querySelectorAll('.log-entry').forEach(row => {
            row.classList.remove('selected');
        });
        document.querySelector(`[data-index="${index}"]`).classList.add('selected');
    }

    showDetails(log) {
        const time = new Date(log.timestamp).toLocaleString();
        const source = log.source || {};

        let html = `
            <div class="detail-section">
                <h4>General</h4>
                <div class="detail-field">
                    <div class="detail-label">Timestamp:</div>
                    <div class="detail-value">${time}</div>
                </div>
                <div class="detail-field">
                    <div class="detail-label">Level:</div>
                    <div class="detail-value"><span class="log-level ${log.level}">${log.level}</span></div>
                </div>
                <div class="detail-field">
                    <div class="detail-label">Message:</div>
                    <div class="detail-value">${this.escapeHtml(log.message)}</div>
                </div>
            </div>
        `;

        if (source.file) {
            html += `
                <div class="detail-section">
                    <h4>Source</h4>
                    <div class="detail-field">
                        <div class="detail-label">File:</div>
                        <div class="detail-value">${this.escapeHtml(source.file)}</div>
                    </div>
                    <div class="detail-field">
                        <div class="detail-label">Line:</div>
                        <div class="detail-value">${source.line || 'N/A'}</div>
                    </div>
                    <div class="detail-field">
                        <div class="detail-label">Function:</div>
                        <div class="detail-value">${this.escapeHtml(source.function || 'N/A')}</div>
                    </div>
                </div>
            `;
        }

        if (log.data && Object.keys(log.data).length > 0) {
            html += `
                <div class="detail-section">
                    <h4>Data</h4>
                    <div class="detail-json">${this.escapeHtml(JSON.stringify(log.data, null, 2))}</div>
                </div>
            `;
        }

        this.detailsContent.innerHTML = html;
        this.logDetails.style.display = 'flex';
    }

    hideDetails() {
        this.logDetails.style.display = 'none';
        this.selectedLog = null;

        // Remove selection
        document.querySelectorAll('.log-entry').forEach(row => {
            row.classList.remove('selected');
        });
    }

    clearLogs() {
        if (confirm('Clear all log entries?')) {
            this.logs = [];
            this.filteredLogs = [];
            this.receivedEventIds.clear();  // Clear event ID tracking
            this.renderLogs();
            this.hideDetails();

            // Send clear request to backend
            if (window.devtools && window.devtools.connected) {
                window.devtools.sendRequest('Log.clear');
            }
        }
    }

    exportLogs() {
        const data = JSON.stringify(this.logs, null, 2);
        const blob = new Blob([data], { type: 'application/json' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = `stratos-logs-${Date.now()}.json`;
        a.click();
        URL.revokeObjectURL(url);
    }

    formatTime(timestamp) {
        const date = new Date(timestamp);
        const hours = String(date.getHours()).padStart(2, '0');
        const minutes = String(date.getMinutes()).padStart(2, '0');
        const seconds = String(date.getSeconds()).padStart(2, '0');
        const ms = String(date.getMilliseconds()).padStart(3, '0');
        return `${hours}:${minutes}:${seconds}.${ms}`;
    }

    formatSource(source) {
        if (!source || !source.file) {
            return '<span class="log-source">-</span>';
        }

        let result = `<span class="file">${this.escapeHtml(source.file)}:${source.line || '?'}</span>`;
        if (source.function) {
            result += ` (${this.escapeHtml(source.function)})`;
        }
        return result;
    }

    escapeHtml(text) {
        if (typeof text !== 'string') {
            text = String(text);
        }

        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }

    scrollToBottom() {
        // Auto-scroll to bottom if user is already at bottom
        const container = this.logContainer;
        const isAtBottom = container.scrollHeight - container.clientHeight <= container.scrollTop + 50;

        if (isAtBottom) {
            setTimeout(() => {
                container.scrollTop = container.scrollHeight;
            }, 0);
        }
    }

    // Add demo logs for testing without backend
    addDemoLogs() {
        const demoLogs = [
            {
                timestamp: Date.now() - 5000,
                level: 'INFO',
                message: 'Application starting',
                source: { file: 'main.st', line: 42, function: 'main' }
            },
            {
                timestamp: Date.now() - 4000,
                level: 'DEBUG',
                message: 'Initializing system components',
                source: { file: 'main.st', line: 43, function: 'main' }
            },
            {
                timestamp: Date.now() - 3000,
                level: 'INFO',
                message: 'Processing order',
                source: { file: 'order.st', line: 15, function: 'processOrder' }
            },
            {
                timestamp: Date.now() - 2000,
                level: 'WARN',
                message: 'Large order amount detected',
                source: { file: 'order.st', line: 22, function: 'processOrder' },
                data: { orderId: 1002, amount: 15000 }
            },
            {
                timestamp: Date.now() - 1000,
                level: 'ERROR',
                message: 'Invalid order amount: negative value',
                source: { file: 'order.st', line: 18, function: 'processOrder' },
                data: { orderId: 1003, amount: -50 }
            },
            {
                timestamp: Date.now(),
                level: 'INFO',
                message: 'Application completed',
                source: { file: 'main.st', line: 50, function: 'main' }
            }
        ];

        demoLogs.forEach(log => this.addLogEntry(log));
    }
}

// Initialize logging view
window.loggingView = new LoggingView();

// Add demo logs after 1 second (for testing without backend)
setTimeout(() => {
    if (!window.devtools || !window.devtools.connected) {
        console.log('Adding demo logs (not connected to backend)');
        window.loggingView.addDemoLogs();
    }
}, 1000);
