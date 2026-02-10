/**
 * Stratos DevTools - Performance View
 * Flame graph, call tree, function table, recording controls
 */
(() => {
    let recording = false;
    let profileData = null;
    let recordingStart = 0;
    let durationTimer = null;

    const recordBtn = document.getElementById('perf-record-btn');
    const stopBtn = document.getElementById('perf-stop-btn');
    const clearBtn = document.getElementById('perf-clear-btn');
    const statusEl = document.getElementById('perf-status');
    const durationEl = document.getElementById('perf-duration');
    const flamegraphContainer = document.getElementById('flamegraph-container');
    const calltreeBody = document.getElementById('calltree-body');
    const functionsBody = document.getElementById('functions-body');

    // --- Tab switching ---
    document.querySelectorAll('.perf-tab').forEach(tab => {
        tab.addEventListener('click', () => {
            document.querySelectorAll('.perf-tab').forEach(t => t.classList.remove('active'));
            tab.classList.add('active');
            document.querySelectorAll('.perf-view').forEach(v => v.style.display = 'none');
            document.getElementById('perf-' + tab.dataset.perf).style.display = '';
        });
    });

    // --- Events ---
    DevTools.on('connected', () => {
        DevTools.call('Profiler.enable').catch(() => {});
    });

    DevTools.on('Profiler.started', () => {
        recording = true;
        updateUI();
    });

    DevTools.on('Profiler.stopped', () => {
        recording = false;
        updateUI();
        fetchProfile();
    });

    // --- Controls ---
    recordBtn.addEventListener('click', async () => {
        if (!recording) {
            await DevTools.call('Profiler.start');
            recording = true;
            recordingStart = Date.now();
            startDurationTimer();
            updateUI();
        }
    });

    stopBtn.addEventListener('click', async () => {
        if (recording) {
            await DevTools.call('Profiler.stop');
            recording = false;
            stopDurationTimer();
            updateUI();
            fetchProfile();
        }
    });

    clearBtn.addEventListener('click', () => {
        profileData = null;
        flamegraphContainer.innerHTML = '<p class="empty-state">Record a profile to see the flame graph.</p>';
        calltreeBody.innerHTML = '';
        functionsBody.innerHTML = '';
        durationEl.textContent = '';
    });

    function startDurationTimer() {
        stopDurationTimer();
        durationTimer = setInterval(() => {
            const elapsed = ((Date.now() - recordingStart) / 1000).toFixed(1);
            durationEl.textContent = elapsed + 's';
        }, 100);
    }

    function stopDurationTimer() {
        if (durationTimer) { clearInterval(durationTimer); durationTimer = null; }
    }

    function updateUI() {
        if (recording) {
            recordBtn.classList.add('recording');
            recordBtn.textContent = 'Recording...';
            recordBtn.disabled = true;
            stopBtn.disabled = false;
            statusEl.textContent = 'Recording';
            statusEl.style.color = 'var(--error)';
        } else {
            recordBtn.classList.remove('recording');
            recordBtn.textContent = 'Record';
            recordBtn.disabled = false;
            stopBtn.disabled = true;
            statusEl.textContent = profileData ? 'Profile ready' : 'Idle';
            statusEl.style.color = profileData ? 'var(--success)' : '';
        }
    }

    // --- Fetch Profile ---
    async function fetchProfile() {
        try {
            profileData = await DevTools.call('Profiler.getProfile');
            durationEl.textContent = DevTools.formatTime(profileData.duration);
            renderFlameGraph(profileData);
            renderFunctionsTable(profileData);
            renderCallTree(profileData);
        } catch (e) {
            flamegraphContainer.innerHTML = '<p class="empty-state">Failed to load profile data.</p>';
        }
    }

    // --- Flame Graph ---
    function renderFlameGraph(data) {
        if (!data.flameGraph || data.flameGraph.length === 0) {
            flamegraphContainer.innerHTML = '<p class="empty-state">No flame graph data recorded.</p>';
            return;
        }

        const totalDuration = data.duration || 1;
        const barHeight = 22;
        const maxDepth = Math.max(...data.flameGraph.map(n => n.depth), 0);
        const chartHeight = (maxDepth + 1) * barHeight + 20;

        flamegraphContainer.innerHTML = '';
        const chart = document.createElement('div');
        chart.className = 'flame-chart';
        chart.style.height = chartHeight + 'px';

        data.flameGraph.forEach((node, i) => {
            if (node.value <= 0) return;
            const widthPct = (node.value / totalDuration) * 100;
            if (widthPct < 0.5) return; // Skip tiny bars

            const bar = document.createElement('div');
            bar.className = `flame-bar flame-d${node.depth % 8}`;
            bar.style.width = widthPct + '%';
            bar.style.top = ((maxDepth - node.depth) * barHeight) + 'px';
            bar.style.left = '0%'; // Simplified - real flame graphs offset by start position
            bar.textContent = node.name;
            bar.title = `${node.name}: ${DevTools.formatTime(node.value)}`;
            chart.appendChild(bar);
        });

        flamegraphContainer.appendChild(chart);
    }

    // --- Functions Table ---
    function renderFunctionsTable(data) {
        functionsBody.innerHTML = '';
        if (!data.functions || data.functions.length === 0) return;

        data.functions.forEach(fn => {
            const tr = document.createElement('tr');
            tr.innerHTML = `
                <td class="fn-name">${DevTools.escapeHtml(fn.name)}</td>
                <td class="time-val">${fn.selfTime.toFixed(2)}</td>
                <td class="time-val">${fn.totalTime.toFixed(2)}</td>
                <td class="time-val">${fn.selfPercent.toFixed(1)}%</td>
                <td class="time-val">${fn.calls}</td>
            `;
            functionsBody.appendChild(tr);
        });
    }

    // --- Call Tree ---
    function renderCallTree(data) {
        calltreeBody.innerHTML = '';
        if (!data.functions || data.functions.length === 0) return;

        // Use functions as flat tree (simplified)
        data.functions.forEach(fn => {
            const tr = document.createElement('tr');
            tr.innerHTML = `
                <td class="fn-name">${DevTools.escapeHtml(fn.name)}</td>
                <td class="time-val">${DevTools.formatTime(fn.selfTime)}</td>
                <td class="time-val">${DevTools.formatTime(fn.totalTime)}</td>
                <td class="time-val">${fn.calls}</td>
            `;
            calltreeBody.appendChild(tr);
        });
    }

    // --- Sortable columns ---
    document.querySelectorAll('#functions-table th').forEach((th, idx) => {
        th.addEventListener('click', () => {
            if (!profileData || !profileData.functions) return;
            const fields = ['name', 'selfTime', 'totalTime', 'selfPercent', 'calls'];
            const field = fields[idx];
            if (!field) return;
            profileData.functions.sort((a, b) => {
                if (typeof a[field] === 'string') return a[field].localeCompare(b[field]);
                return b[field] - a[field]; // descending
            });
            renderFunctionsTable(profileData);
        });
    });
})();
