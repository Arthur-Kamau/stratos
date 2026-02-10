/**
 * Stratos DevTools - Memory View
 * Stats dashboard, GC history, memory usage visualization
 */
(() => {
    let enabled = false;
    let refreshTimer = null;

    const enableBtn = document.getElementById('memory-enable-btn');
    const refreshBtn = document.getElementById('memory-refresh-btn');
    const statusEl = document.getElementById('memory-status');

    // --- Enable / Disable ---
    enableBtn.addEventListener('click', async () => {
        if (!enabled) {
            await DevTools.call('Memory.enable');
            enabled = true;
            enableBtn.textContent = 'Disable Profiling';
            statusEl.textContent = 'Enabled';
            statusEl.style.color = 'var(--success)';
            startAutoRefresh();
            refreshStats();
        } else {
            await DevTools.call('Memory.disable');
            enabled = false;
            enableBtn.textContent = 'Enable Profiling';
            statusEl.textContent = 'Disabled';
            statusEl.style.color = '';
            stopAutoRefresh();
        }
    });

    refreshBtn.addEventListener('click', refreshStats);

    DevTools.on('tabChanged', (tab) => {
        if (tab === 'memory' && enabled) {
            refreshStats();
        }
    });

    // --- Auto Refresh ---
    function startAutoRefresh() {
        stopAutoRefresh();
        refreshTimer = setInterval(refreshStats, 2000);
    }

    function stopAutoRefresh() {
        if (refreshTimer) {
            clearInterval(refreshTimer);
            refreshTimer = null;
        }
    }

    // --- Refresh Stats ---
    async function refreshStats() {
        if (!DevTools.connected) return;
        try {
            const [stats, history] = await Promise.all([
                DevTools.call('Memory.getStats'),
                DevTools.call('Memory.getGCHistory')
            ]);
            updateDashboard(stats);
            updateGCHistory(history);
        } catch (e) {
            // Silently ignore if server not ready
        }
    }

    function updateDashboard(stats) {
        if (!stats) return;
        setStatValue('stat-current-usage', DevTools.formatBytes(stats.currentUsage || 0));
        setStatValue('stat-object-count', (stats.objectCount || 0).toLocaleString());

        const gc = stats.gcStats || {};
        setStatValue('stat-gc-collections', (gc.collectionsTotal || 0).toLocaleString());
        setStatValue('stat-avg-pause', DevTools.formatTime(gc.avgPauseTime || 0));

        // Update memory bar
        const total = stats.totalAllocated || 1;
        const current = stats.currentUsage || 0;
        const pct = Math.min(100, (current / total) * 100);
        const fill = document.getElementById('memory-usage-fill');
        fill.style.width = pct + '%';
        fill.className = 'memory-usage-fill' + (pct > 80 ? ' high' : '');
        document.getElementById('memory-usage-text').textContent =
            DevTools.formatBytes(current) + ' / ' + DevTools.formatBytes(total);
    }

    function setStatValue(cardId, value) {
        const card = document.getElementById(cardId);
        if (card) card.querySelector('.stat-value').textContent = value;
    }

    function updateGCHistory(data) {
        if (!data || !data.events) return;
        const tbody = document.getElementById('gc-history-body');
        tbody.innerHTML = '';

        data.events.slice().reverse().forEach(evt => {
            const tr = document.createElement('tr');
            const pauseClass = evt.pauseTime < 5 ? 'pause-low' :
                               evt.pauseTime < 50 ? 'pause-med' : 'pause-high';
            tr.innerHTML = `
                <td>${DevTools.formatTimestamp(evt.timestamp)}</td>
                <td class="${pauseClass}">${evt.pauseTime.toFixed(2)}</td>
                <td>${evt.freedObjects}</td>
                <td>${DevTools.formatBytes(evt.freedBytes)}</td>
                <td>${evt.cyclesBroken}</td>
            `;
            tbody.appendChild(tr);
        });
    }
})();
