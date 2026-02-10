/**
 * Stratos DevTools - Debugger View
 * Source display, breakpoints, call stack, variable inspection, stepping
 */
(() => {
    let enabled = false;
    let paused = false;
    let currentFile = '';
    let currentLine = 0;
    let sourceCache = {};
    let breakpoints = [];
    let watchExpressions = [];
    let pollTimer = null;

    const filenameEl = document.getElementById('debug-filename');
    const gutterEl = document.getElementById('source-gutter');
    const codeEl = document.getElementById('source-code');
    const callstackEl = document.getElementById('callstack-list');
    const variablesEl = document.getElementById('variables-list');
    const watchItemsEl = document.getElementById('watch-items');
    const breakpointsEl = document.getElementById('breakpoints-list');

    // --- Keywords for syntax highlighting ---
    const KEYWORDS = new Set([
        'fn', 'val', 'var', 'if', 'else', 'for', 'while', 'return', 'break', 'continue',
        'class', 'struct', 'enum', 'package', 'use', 'import', 'pub', 'mut',
        'true', 'false', 'null', 'void', 'defer', 'async', 'await', 'select',
        'when', 'in', 'is', 'as', 'new', 'this', 'super', 'match'
    ]);
    const TYPES = new Set(['int', 'double', 'string', 'bool', 'char', 'float', 'any']);

    // --- Event Handlers ---
    DevTools.on('Debugger.paused', (data) => {
        paused = true;
        if (data.file) loadSource(data.file, data.line);
        if (data.callFrames) renderCallStack(data.callFrames);
        refreshVariables();
        refreshWatches();
    });

    DevTools.on('Debugger.resumed', () => {
        paused = false;
        clearCurrentLine();
    });

    DevTools.on('Debugger.scriptParsed', (data) => {
        if (data.url && !currentFile) {
            loadSource(data.url);
        }
    });

    DevTools.on('connected', () => {
        // Start polling debug state
        startStatePoll();
    });

    DevTools.on('disconnected', () => {
        stopStatePoll();
    });

    DevTools.on('tabChanged', (tab) => {
        if (tab === 'debugger') {
            if (!enabled) {
                DevTools.call('Debugger.enable').then(() => {
                    enabled = true;
                    loadSourceFiles();
                }).catch(() => {});
            }
        }
    });

    function startStatePoll() {
        stopStatePoll();
        pollTimer = setInterval(async () => {
            if (!enabled) return;
            try {
                const state = await DevTools.call('Debugger.getState');
                if (state.state === 'paused' && !paused) {
                    paused = true;
                    if (state.file) loadSource(state.file, state.line);
                    refreshCallStack();
                    refreshVariables();
                } else if (state.state === 'running' && paused) {
                    paused = false;
                    clearCurrentLine();
                }
            } catch (e) {}
        }, 500);
    }

    function stopStatePoll() {
        if (pollTimer) { clearInterval(pollTimer); pollTimer = null; }
    }

    // --- Source Loading ---
    async function loadSourceFiles() {
        try {
            const result = await DevTools.call('Debugger.getSourceFiles');
            if (result.files && result.files.length > 0 && !currentFile) {
                loadSource(result.files[0]);
            }
        } catch (e) {}
    }

    async function loadSource(file, highlightLine) {
        currentFile = file;
        filenameEl.textContent = file;

        if (!sourceCache[file]) {
            try {
                const result = await DevTools.call('Debugger.getSource', { file });
                sourceCache[file] = result.source || '';
            } catch (e) {
                sourceCache[file] = '// Could not load source: ' + file;
            }
        }

        renderSource(sourceCache[file], highlightLine);
    }

    function renderSource(source, highlightLine) {
        const lines = source.split('\n');
        gutterEl.innerHTML = '';
        let codeHtml = '';

        lines.forEach((line, i) => {
            const lineNum = i + 1;

            // Gutter
            const gutterLine = document.createElement('div');
            gutterLine.className = 'gutter-line';
            gutterLine.dataset.line = lineNum;

            const hasBp = breakpoints.some(bp => bp.file === currentFile && bp.line === lineNum);
            if (hasBp) gutterLine.classList.add('has-breakpoint');

            gutterLine.innerHTML = `<span class="bp-marker"></span><span>${lineNum}</span>`;
            gutterLine.addEventListener('click', () => toggleBreakpoint(currentFile, lineNum));
            gutterEl.appendChild(gutterLine);

            // Code line
            const classes = ['code-line'];
            if (highlightLine && lineNum === highlightLine) {
                classes.push('current-line');
                currentLine = lineNum;
            }
            codeHtml += `<div class="${classes.join(' ')}">${highlightSyntax(line)}</div>`;
        });

        codeEl.innerHTML = `<code>${codeHtml}</code>`;

        // Scroll to highlighted line
        if (highlightLine) {
            const lineEl = codeEl.querySelectorAll('.code-line')[highlightLine - 1];
            if (lineEl) lineEl.scrollIntoView({ block: 'center', behavior: 'smooth' });
        }
    }

    function highlightSyntax(line) {
        // Simple regex-based syntax highlighting
        let escaped = DevTools.escapeHtml(line);

        // Comments (// ...)
        escaped = escaped.replace(/(\/\/.*)$/, '<span class="cmt">$1</span>');

        // Strings
        escaped = escaped.replace(/(&quot;[^&]*&quot;|"[^"]*")/g, '<span class="str">$1</span>');

        // Numbers
        escaped = escaped.replace(/\b(\d+\.?\d*)\b/g, '<span class="num">$1</span>');

        // Keywords and types
        escaped = escaped.replace(/\b(\w+)\b/g, (match) => {
            if (KEYWORDS.has(match)) return `<span class="kw">${match}</span>`;
            if (TYPES.has(match)) return `<span class="type">${match}</span>`;
            return match;
        });

        return escaped || ' '; // empty lines need a space for height
    }

    function clearCurrentLine() {
        codeEl.querySelectorAll('.current-line').forEach(el => el.classList.remove('current-line'));
    }

    // --- Breakpoints ---
    async function toggleBreakpoint(file, line) {
        const existing = breakpoints.find(bp => bp.file === file && bp.line === line);
        if (existing) {
            await DevTools.call('Debugger.removeBreakpoint', { breakpointId: existing.id });
            breakpoints = breakpoints.filter(bp => bp.id !== existing.id);
        } else {
            const result = await DevTools.call('Debugger.setBreakpoint', { file, line });
            if (result.breakpointId) {
                breakpoints.push({ id: result.breakpointId, file, line, enabled: true });
            }
        }
        renderSource(sourceCache[currentFile] || '', paused ? currentLine : undefined);
        renderBreakpointsList();
    }

    function renderBreakpointsList() {
        breakpointsEl.innerHTML = '';
        breakpoints.forEach(bp => {
            const div = document.createElement('div');
            div.className = 'bp-item';
            div.innerHTML = `
                <input type="checkbox" ${bp.enabled ? 'checked' : ''}>
                <span class="bp-location">${bp.file}:${bp.line}</span>
            `;
            div.querySelector('input').addEventListener('change', (e) => {
                bp.enabled = e.target.checked;
            });
            breakpointsEl.appendChild(div);
        });
    }

    // --- Call Stack ---
    async function refreshCallStack() {
        try {
            const result = await DevTools.call('Debugger.getCallStack');
            if (result.callFrames) renderCallStack(result.callFrames);
        } catch (e) {}
    }

    function renderCallStack(frames) {
        callstackEl.innerHTML = '';
        frames.forEach((frame, i) => {
            const div = document.createElement('div');
            div.className = 'callstack-item' + (i === 0 ? ' active' : '');
            div.innerHTML = `
                <span class="callstack-fn">${DevTools.escapeHtml(frame.functionName)}</span>
                <span class="callstack-loc">${frame.file ? frame.file + ':' + frame.line : ''}</span>
            `;
            div.addEventListener('click', () => {
                callstackEl.querySelectorAll('.active').forEach(el => el.classList.remove('active'));
                div.classList.add('active');
                if (frame.file) loadSource(frame.file, frame.line);
                refreshVariablesForFrame(frame.frameId);
            });
            callstackEl.appendChild(div);
        });
    }

    // --- Variables ---
    async function refreshVariables(frameId = 0) {
        try {
            const result = await DevTools.call('Debugger.getVariables', { frameId });
            renderVariables(result.variables || []);
        } catch (e) {
            variablesEl.innerHTML = '<div class="empty-state">No variables</div>';
        }
    }

    async function refreshVariablesForFrame(frameId) {
        await refreshVariables(frameId);
    }

    function renderVariables(vars) {
        variablesEl.innerHTML = '';
        if (vars.length === 0) {
            variablesEl.innerHTML = '<div class="empty-state" style="padding:8px;font-size:12px;">No variables in scope</div>';
            return;
        }
        vars.forEach(v => {
            const div = document.createElement('div');
            div.className = 'var-item';
            div.innerHTML = `
                <span class="var-name">${DevTools.escapeHtml(v.name)}</span>
                <span class="var-value">${DevTools.escapeHtml(v.value)}</span>
                <span class="var-type">${v.type}</span>
            `;
            variablesEl.appendChild(div);
        });
    }

    // --- Watch ---
    document.getElementById('watch-add-btn').addEventListener('click', addWatch);
    document.getElementById('watch-input').addEventListener('keydown', (e) => {
        if (e.key === 'Enter') addWatch();
    });

    function addWatch() {
        const input = document.getElementById('watch-input');
        const expr = input.value.trim();
        if (!expr) return;
        watchExpressions.push(expr);
        input.value = '';
        refreshWatches();
    }

    async function refreshWatches() {
        watchItemsEl.innerHTML = '';
        for (const expr of watchExpressions) {
            let value = '<pending>';
            if (paused) {
                try {
                    const result = await DevTools.call('Debugger.evaluateOnFrame', { expression: expr, frameId: 0 });
                    value = result.result || '<undefined>';
                } catch (e) {
                    value = '<error>';
                }
            }
            const div = document.createElement('div');
            div.className = 'watch-item';
            div.innerHTML = `
                <span class="var-name">${DevTools.escapeHtml(expr)}</span>
                <span class="var-value">${DevTools.escapeHtml(String(value))}</span>
            `;
            watchItemsEl.appendChild(div);
        }
    }

    // --- Debug Controls ---
    document.getElementById('btn-resume').addEventListener('click', () => DevTools.call('Debugger.resume'));
    document.getElementById('btn-pause').addEventListener('click', () => DevTools.call('Debugger.pause'));
    document.getElementById('btn-step-over').addEventListener('click', () => DevTools.call('Debugger.stepOver'));
    document.getElementById('btn-step-into').addEventListener('click', () => DevTools.call('Debugger.stepInto'));
    document.getElementById('btn-step-out').addEventListener('click', () => DevTools.call('Debugger.stepOut'));

    // Keyboard shortcuts
    document.addEventListener('keydown', (e) => {
        if (e.key === 'F5') { e.preventDefault(); DevTools.call('Debugger.resume'); }
        if (e.key === 'F6') { e.preventDefault(); DevTools.call('Debugger.pause'); }
        if (e.key === 'F9') { e.preventDefault(); if (currentFile && currentLine) toggleBreakpoint(currentFile, currentLine); }
        if (e.key === 'F10') { e.preventDefault(); DevTools.call('Debugger.stepOver'); }
        if (e.key === 'F11') {
            e.preventDefault();
            if (e.shiftKey) DevTools.call('Debugger.stepOut');
            else DevTools.call('Debugger.stepInto');
        }
    });
})();
