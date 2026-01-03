// Memory View Logic

class MemoryView {
    constructor() {
        this.stats = null;
        this.gcHistory = [];
        this.enabled = false;
        this.updateInterval = null;
        this.receivedEventIds = new Set();  // Track processed event IDs
    }

    init() {
        console.log('[MemoryView] Initializing');

        // Setup UI
        this.setupMemoryUI();

        // Listen for connection events
        window.addEventListener('devtools-connected', () => {
            this.onConnected();
        });

        window.addEventListener('devtools-disconnected', () => {
            this.onDisconnected();
        });

        // Listen for Memory events
        window.addEventListener('devtools-event', (e) => {
            // Skip duplicate events based on ID
            if (e.detail.eventId !== undefined && this.receivedEventIds.has(e.detail.eventId)) {
                console.log('[MemoryView] Skipping duplicate event ID:', e.detail.eventId);
                return;
            }

            // Mark event as processed
            if (e.detail.eventId !== undefined) {
                this.receivedEventIds.add(e.detail.eventId);
            }

            if (e.detail.method === 'Memory.statsUpdated') {
                this.handleStatsUpdate(e.detail.params);
            } else if (e.detail.method === 'Memory.gcPerformed') {
                this.handleGCEvent(e.detail.params);
            }
        });
    }

    setupMemoryUI() {
        const memoryPane = document.getElementById('memory');
        if (!memoryPane) return;

        memoryPane.innerHTML = `
            <div class="memory-view">
                <!-- Toolbar -->
                <div class="memory-toolbar">
                    <button id="enableMemory">Enable Memory Profiling</button>
                    <button id="refreshStats">Refresh Stats</button>
                    <button id="takeSnapshot" disabled>Take Snapshot</button>
                    <div class="spacer"></div>
                    <button id="clearHistory">Clear History</button>
                </div>

                <!-- Stats Overview -->
                <div class="memory-stats" id="memoryStats">
                    <div class="stat-card">
                        <h3>Current Usage</h3>
                        <div class="value" id="currentUsage">0</div>
                        <div class="unit">bytes</div>
                        <div class="memory-bar">
                            <div class="memory-bar-fill" id="usageBar" style="width: 0%"></div>
                        </div>
                    </div>

                    <div class="stat-card">
                        <h3>Object Count</h3>
                        <div class="value" id="objectCount">0</div>
                        <div class="unit">objects</div>
                    </div>

                    <div class="stat-card">
                        <h3>GC Collections</h3>
                        <div class="value" id="gcCollections">0</div>
                        <div class="unit">total</div>
                    </div>

                    <div class="stat-card">
                        <h3>Avg GC Pause</h3>
                        <div class="value" id="avgPause">0.0</div>
                        <div class="unit">ms</div>
                    </div>

                    <div class="stat-card">
                        <h3>Cycles Broken</h3>
                        <div class="value" id="cyclesBroken">0</div>
                        <div class="unit">total</div>
                    </div>

                    <div class="stat-card">
                        <h3>Last Collection</h3>
                        <div class="value" id="lastGC">Never</div>
                        <div class="unit"></div>
                    </div>
                </div>

                <!-- GC History -->
                <div class="gc-history">
                    <h2>GC Collection History</h2>
                    <div class="gc-history-table">
                        <table class="gc-table">
                            <thead>
                                <tr>
                                    <th>Time</th>
                                    <th>Pause (ms)</th>
                                    <th>Freed Objects</th>
                                    <th>Freed Bytes</th>
                                    <th>Cycles Broken</th>
                                </tr>
                            </thead>
                            <tbody id="gcHistoryBody">
                                <tr>
                                    <td colspan="5" style="text-align: center; padding: 2rem; color: var(--text-secondary);">
                                        No GC events recorded
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>
            </div>
        `;

        // Setup event listeners
        document.getElementById('enableMemory')?.addEventListener('click', () => {
            this.toggleMemoryProfiling();
        });

        document.getElementById('refreshStats')?.addEventListener('click', () => {
            this.refreshStats();
        });

        document.getElementById('clearHistory')?.addEventListener('click', () => {
            this.clearHistory();
        });
    }

    async toggleMemoryProfiling() {
        const btn = document.getElementById('enableMemory');
        if (!btn) return;

        if (!this.enabled) {
            // Enable
            await window.devtools.sendRequest('Memory.enable', {});
            this.enabled = true;
            btn.textContent = 'Disable Memory Profiling';
            btn.style.background = 'var(--error-color, #ef4444)';

            // Start periodic refresh
            this.updateInterval = setInterval(() => {
                this.refreshStats();
            }, 2000);

            // Initial refresh
            this.refreshStats();
        } else {
            // Disable
            await window.devtools.sendRequest('Memory.disable', {});
            this.enabled = false;
            btn.textContent = 'Enable Memory Profiling';
            btn.style.background = 'var(--primary-color)';

            // Stop periodic refresh
            if (this.updateInterval) {
                clearInterval(this.updateInterval);
                this.updateInterval = null;
            }
        }
    }

    async refreshStats() {
        if (!this.enabled) return;

        try {
            // Get current stats
            const statsResponse = await window.devtools.sendRequest('Memory.getStats', {});
            if (statsResponse.result) {
                this.stats = statsResponse.result;
                this.updateStatsDisplay();
            }

            // Get GC history
            const historyResponse = await window.devtools.sendRequest('Memory.getGCHistory', {});
            if (historyResponse.result && historyResponse.result.events) {
                this.gcHistory = historyResponse.result.events;
                this.updateGCHistoryDisplay();
            }
        } catch (error) {
            console.error('[MemoryView] Error refreshing stats:', error);
        }
    }

    updateStatsDisplay() {
        if (!this.stats) return;

        // Update stat cards
        const currentUsage = document.getElementById('currentUsage');
        if (currentUsage) {
            currentUsage.textContent = this.formatBytes(this.stats.currentUsage);
        }

        const objectCount = document.getElementById('objectCount');
        if (objectCount) {
            objectCount.textContent = this.stats.objectCount.toLocaleString();
        }

        // GC stats
        if (this.stats.gcStats) {
            const gcCollections = document.getElementById('gcCollections');
            if (gcCollections) {
                gcCollections.textContent = this.stats.gcStats.collectionsTotal.toLocaleString();
            }

            const avgPause = document.getElementById('avgPause');
            if (avgPause) {
                avgPause.textContent = this.stats.gcStats.avgPauseTime.toFixed(2);
            }

            const cyclesBroken = document.getElementById('cyclesBroken');
            if (cyclesBroken) {
                cyclesBroken.textContent = this.stats.gcStats.cyclesBroken.toLocaleString();
            }

            const lastGC = document.getElementById('lastGC');
            if (lastGC && this.stats.gcStats.lastCollectionTime) {
                const date = new Date(this.stats.gcStats.lastCollectionTime);
                lastGC.textContent = date.toLocaleTimeString();
            }
        }

        // Update memory bar (assume max of 10MB for visualization)
        const usageBar = document.getElementById('usageBar');
        if (usageBar) {
            const maxMemory = 10 * 1024 * 1024; // 10MB
            const percentage = Math.min(100, (this.stats.currentUsage / maxMemory) * 100);
            usageBar.style.width = percentage + '%';

            if (percentage > 80) {
                usageBar.classList.add('high');
            } else {
                usageBar.classList.remove('high');
            }
        }
    }

    updateGCHistoryDisplay() {
        const tbody = document.getElementById('gcHistoryBody');
        if (!tbody) return;

        if (this.gcHistory.length === 0) {
            tbody.innerHTML = `
                <tr>
                    <td colspan="5" style="text-align: center; padding: 2rem; color: var(--text-secondary);">
                        No GC events recorded
                    </td>
                </tr>
            `;
            return;
        }

        tbody.innerHTML = '';

        // Show most recent first
        const sorted = [...this.gcHistory].reverse();

        for (const event of sorted) {
            const row = document.createElement('tr');

            const date = new Date(event.timestamp);
            const timeStr = date.toLocaleTimeString() + '.' + String(date.getMilliseconds()).padStart(3, '0');

            row.innerHTML = `
                <td>${timeStr}</td>
                <td>${event.pauseTime.toFixed(2)}</td>
                <td>${event.freedObjects.toLocaleString()}</td>
                <td>${this.formatBytes(event.freedBytes)}</td>
                <td>${event.cyclesBroken}</td>
            `;

            tbody.appendChild(row);
        }
    }

    handleStatsUpdate(params) {
        // Real-time stats update from server
        if (params.currentUsage !== undefined) {
            const currentUsage = document.getElementById('currentUsage');
            if (currentUsage) {
                currentUsage.textContent = this.formatBytes(params.currentUsage);
            }
        }

        if (params.objectCount !== undefined) {
            const objectCount = document.getElementById('objectCount');
            if (objectCount) {
                objectCount.textContent = params.objectCount.toLocaleString();
            }
        }
    }

    handleGCEvent(params) {
        console.log('[MemoryView] GC event:', params);

        // Add to history
        this.gcHistory.push(params);

        // Limit history size
        if (this.gcHistory.length > 100) {
            this.gcHistory.shift();
        }

        // Update display
        this.updateGCHistoryDisplay();

        // Refresh full stats
        this.refreshStats();
    }

    clearHistory() {
        this.gcHistory = [];
        this.updateGCHistoryDisplay();
    }

    formatBytes(bytes) {
        if (bytes === 0) return '0 B';

        const k = 1024;
        const sizes = ['B', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));

        return (bytes / Math.pow(k, i)).toFixed(1) + ' ' + sizes[i];
    }

    onConnected() {
        console.log('[MemoryView] Connected');
        // Auto-enable if was previously enabled
        if (this.enabled) {
            this.refreshStats();
        }
    }

    onDisconnected() {
        console.log('[MemoryView] Disconnected');
        if (this.updateInterval) {
            clearInterval(this.updateInterval);
            this.updateInterval = null;
        }
    }
}

// Initialize when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', () => {
        window.memoryView = new MemoryView();
        window.memoryView.init();
    });
} else {
    window.memoryView = new MemoryView();
    window.memoryView.init();
}
