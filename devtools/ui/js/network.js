/**
 * Stratos DevTools - Network View
 * HTTP request monitoring, filtering, detail inspection
 */
(() => {
    let requests = [];
    let selectedRequest = null;
    let pollTimer = null;

    const networkBody = document.getElementById('network-body');
    const detailPanel = document.getElementById('network-detail');
    const detailBody = document.getElementById('network-detail-body');
    const filterInput = document.getElementById('network-filter');
    const methodFilter = document.getElementById('network-method-filter');

    // --- Events ---
    DevTools.on('Network.requestStarted', (data) => {
        const req = {
            id: data.requestId,
            url: data.url,
            method: data.method,
            status: 0,
            type: '',
            size: 0,
            time: 0,
            startTime: data.startTime,
            completed: false
        };
        requests.push(req);
        if (matchesFilter(req)) appendRequestRow(req);
    });

    DevTools.on('Network.requestCompleted', (data) => {
        const req = requests.find(r => r.id === data.requestId);
        if (req) {
            req.status = data.status;
            req.size = data.size;
            req.time = data.duration;
            req.completed = true;
            updateRequestRow(req);
        }
    });

    DevTools.on('connected', () => {
        DevTools.call('Network.enable').catch(() => {});
        startPolling();
    });

    DevTools.on('disconnected', () => stopPolling());

    DevTools.on('tabChanged', (tab) => {
        if (tab === 'network') refreshRequests();
    });

    // --- Polling for initial data ---
    function startPolling() {
        stopPolling();
        pollTimer = setInterval(refreshRequests, 3000);
    }

    function stopPolling() {
        if (pollTimer) { clearInterval(pollTimer); pollTimer = null; }
    }

    async function refreshRequests() {
        try {
            const result = await DevTools.call('Network.getRequests');
            if (result.requests) {
                requests = result.requests;
                rerender();
            }
        } catch (e) {}
    }

    // --- Filtering ---
    function matchesFilter(req) {
        const urlFilter = filterInput.value.toLowerCase();
        if (urlFilter && !req.url.toLowerCase().includes(urlFilter)) return false;
        const method = methodFilter.value;
        if (method && req.method !== method) return false;
        return true;
    }

    function rerender() {
        networkBody.innerHTML = '';
        requests.filter(matchesFilter).forEach(appendRequestRow);
    }

    filterInput.addEventListener('input', rerender);
    methodFilter.addEventListener('change', rerender);

    // --- Rendering ---
    function appendRequestRow(req) {
        const tr = document.createElement('tr');
        tr.dataset.id = req.id;
        updateRowContent(tr, req);
        tr.addEventListener('click', () => showDetail(req, tr));
        networkBody.appendChild(tr);
    }

    function updateRequestRow(req) {
        const tr = networkBody.querySelector(`tr[data-id="${req.id}"]`);
        if (tr) updateRowContent(tr, req);
    }

    function updateRowContent(tr, req) {
        // Extract short name from URL
        let name = req.url;
        try { name = new URL(req.url).pathname; } catch (e) {}

        const statusClass = req.status >= 400 ? 'status-error' :
                           req.status >= 300 ? 'status-redirect' :
                           req.status >= 200 ? 'status-ok' : '';

        const timelineWidth = Math.min(200, Math.max(2, req.time / 10));
        const timelineClass = req.time < 100 ? 'fast' : req.time < 1000 ? 'medium' : 'slow';

        tr.innerHTML = `
            <td title="${DevTools.escapeHtml(req.url)}">${DevTools.escapeHtml(name)}</td>
            <td>${req.method}</td>
            <td class="${statusClass}">${req.completed ? req.status : '...'}</td>
            <td>${req.type || '-'}</td>
            <td>${req.completed ? DevTools.formatBytes(req.size) : '-'}</td>
            <td>${req.completed ? DevTools.formatTime(req.time) : '...'}</td>
            <td><span class="timeline-bar ${timelineClass}" style="width:${timelineWidth}px"></span></td>
        `;
    }

    // --- Detail Panel ---
    async function showDetail(req, tr) {
        networkBody.querySelectorAll('.selected').forEach(el => el.classList.remove('selected'));
        tr.classList.add('selected');
        selectedRequest = req;
        detailPanel.style.display = '';

        try {
            const detail = await DevTools.call('Network.getRequestDetail', { id: req.id });
            renderDetail(detail, 'headers');
        } catch (e) {
            detailBody.textContent = 'Could not load request detail';
        }
    }

    // Detail tabs
    document.querySelectorAll('.net-detail-tab').forEach(tab => {
        tab.addEventListener('click', () => {
            document.querySelectorAll('.net-detail-tab').forEach(t => t.classList.remove('active'));
            tab.classList.add('active');
            if (selectedRequest) {
                DevTools.call('Network.getRequestDetail', { id: selectedRequest.id })
                    .then(detail => renderDetail(detail, tab.dataset.detail))
                    .catch(() => {});
            }
        });
    });

    function renderDetail(detail, view) {
        if (!detail) return;

        if (view === 'headers') {
            let html = '<div class="header-group"><h4>General</h4>';
            html += `<div class="header-row"><span class="header-key">URL:</span> <span class="header-val">${DevTools.escapeHtml(detail.url)}</span></div>`;
            html += `<div class="header-row"><span class="header-key">Method:</span> <span class="header-val">${detail.method}</span></div>`;
            html += `<div class="header-row"><span class="header-key">Status:</span> <span class="header-val">${detail.status}</span></div>`;
            html += '</div>';

            if (detail.requestHeaders && Object.keys(detail.requestHeaders).length > 0) {
                html += '<div class="header-group"><h4>Request Headers</h4>';
                for (const [k, v] of Object.entries(detail.requestHeaders)) {
                    html += `<div class="header-row"><span class="header-key">${DevTools.escapeHtml(k)}:</span> <span class="header-val">${DevTools.escapeHtml(v)}</span></div>`;
                }
                html += '</div>';
            }

            if (detail.responseHeaders && Object.keys(detail.responseHeaders).length > 0) {
                html += '<div class="header-group"><h4>Response Headers</h4>';
                for (const [k, v] of Object.entries(detail.responseHeaders)) {
                    html += `<div class="header-row"><span class="header-key">${DevTools.escapeHtml(k)}:</span> <span class="header-val">${DevTools.escapeHtml(v)}</span></div>`;
                }
                html += '</div>';
            }

            // Copy as cURL
            html += `<button class="toolbar-btn copy-curl-btn" onclick="navigator.clipboard.writeText('curl ${detail.url}')">Copy as cURL</button>`;
            detailBody.innerHTML = html;

        } else if (view === 'response') {
            const body = detail.responseBody || '<empty>';
            // Try to pretty-print JSON
            try {
                const parsed = JSON.parse(body);
                detailBody.textContent = JSON.stringify(parsed, null, 2);
            } catch (e) {
                detailBody.textContent = body;
            }

        } else if (view === 'timing') {
            const total = detail.duration || 0;
            let html = '<div class="timing-bar-container">';
            html += renderTimingRow('Total', total, total);
            html += '</div>';
            html += `<p style="color:var(--text-secondary);font-size:12px;">Total time: ${DevTools.formatTime(total)}</p>`;
            html += `<p style="color:var(--text-secondary);font-size:12px;">Response size: ${DevTools.formatBytes(detail.size || 0)}</p>`;
            detailBody.innerHTML = html;
        }
    }

    function renderTimingRow(label, value, total) {
        const pct = total > 0 ? (value / total * 100) : 0;
        return `<div class="timing-row">
            <span class="timing-label">${label}</span>
            <div class="timing-bar-bg"><div class="timing-bar-fill" style="width:${pct}%"></div></div>
            <span class="timing-value">${DevTools.formatTime(value)}</span>
        </div>`;
    }

    // --- Controls ---
    document.getElementById('network-clear-btn').addEventListener('click', () => {
        requests = [];
        networkBody.innerHTML = '';
        detailPanel.style.display = 'none';
        DevTools.call('Network.clear').catch(() => {});
    });
})();
