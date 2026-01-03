// Stratos DevTools - Main JavaScript

class DevTools {
    constructor() {
        this.ws = null;
        this.connected = false;
        this.reconnectInterval = null;
        this.messageId = 0;
        this.pendingRequests = new Map();

        this.init();
    }

    init() {
        // Setup tab switching
        this.setupTabs();

        // Setup connection
        this.connect();

        // Setup event listeners
        window.addEventListener('beforeunload', () => {
            if (this.ws) {
                this.ws.close();
            }
        });
    }

    setupTabs() {
        const tabButtons = document.querySelectorAll('.tab-button');
        const tabPanes = document.querySelectorAll('.tab-pane');

        tabButtons.forEach(button => {
            button.addEventListener('click', () => {
                const tabName = button.dataset.tab;

                // Update buttons
                tabButtons.forEach(btn => btn.classList.remove('active'));
                button.classList.add('active');

                // Update panes
                tabPanes.forEach(pane => pane.classList.remove('active'));
                document.getElementById(tabName).classList.add('active');
            });
        });
    }

    connect() {
        const baseUrl = 'http://localhost:9222';

        // Test connection
        fetch(baseUrl)
            .then(response => response.json())
            .then(data => {
                console.log('Connected to Stratos DevTools server:', data);
                this.connected = true;
                this.updateConnectionStatus(true);

                // Stop reconnection attempts
                if (this.reconnectInterval) {
                    clearInterval(this.reconnectInterval);
                    this.reconnectInterval = null;
                }

                // Enable logging
                this.sendRequest('Log.enable', {});

                // Start polling for events
                this.startPolling();
            })
            .catch(error => {
                console.log('DevTools server not available, using demo mode');
                this.connected = false;
                this.updateConnectionStatus(false);

                // Attempt to reconnect
                if (!this.reconnectInterval) {
                    this.reconnectInterval = setInterval(() => {
                        console.log('Attempting to reconnect...');
                        this.connect();
                    }, 3000);
                }
            });
    }

    startPolling() {
        if (this.pollingInterval) {
            clearInterval(this.pollingInterval);
        }

        console.log('[DevTools] Starting event polling...');

        this.pollingInterval = setInterval(() => {
            if (!this.connected) {
                console.log('[DevTools] Not connected, stopping poll');
                clearInterval(this.pollingInterval);
                return;
            }

            // Poll for events
            fetch('http://localhost:9222/events')
                .then(response => {
                    console.log('[DevTools] Poll response status:', response.status);
                    return response.json();
                })
                .then(events => {
                    console.log('[DevTools] Poll received:', events);
                    if (Array.isArray(events) && events.length > 0) {
                        console.log(`[DevTools] Received ${events.length} events`, events);
                        events.forEach(event => {
                            if (event.method) {
                                this.handleEvent(event.method, event.params, event.id);
                            }
                        });
                    }
                })
                .catch(error => {
                    console.error('[DevTools] Error polling events:', error);
                });
        }, 100); // Poll every 100ms
    }

    updateConnectionStatus(connected) {
        const indicator = document.querySelector('.status-indicator');
        const text = document.querySelector('.status-text');

        if (connected) {
            indicator.classList.remove('disconnected');
            indicator.classList.add('connected');
            text.textContent = 'Connected • localhost:9222';

            // Dispatch connection event
            window.dispatchEvent(new CustomEvent('devtools-connected'));
        } else {
            indicator.classList.remove('connected');
            indicator.classList.add('disconnected');
            text.textContent = 'Disconnected';

            // Dispatch disconnection event
            window.dispatchEvent(new CustomEvent('devtools-disconnected'));
        }
    }

    sendRequest(method, params = {}) {
        if (!this.connected) {
            console.error('Cannot send request: not connected');
            return Promise.reject(new Error('Not connected'));
        }

        const id = ++this.messageId;
        const message = {
            jsonrpc: '2.0',
            id: id,
            method: method,
            params: params
        };

        return fetch('http://localhost:9222', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(message)
        })
        .then(response => response.json())
        .then(data => {
            if (data.error) {
                throw new Error(data.error.message || 'Request failed');
            }
            return data.result;
        });
    }

    handleMessage(message) {
        // Response to a request
        if (message.id !== undefined) {
            const pending = this.pendingRequests.get(message.id);
            if (pending) {
                this.pendingRequests.delete(message.id);

                if (message.error) {
                    pending.reject(new Error(message.error.message));
                } else {
                    pending.resolve(message.result);
                }
            }
        }
        // Event notification
        else if (message.method) {
            this.handleEvent(message.method, message.params);
        }
    }

    handleEvent(method, params, eventId) {
        // Dispatch window-level event for all views to listen
        window.dispatchEvent(new CustomEvent('devtools-event', {
            detail: { method, params, eventId }
        }));

        // Dispatch to appropriate handler
        const [domain, event] = method.split('.');

        switch (domain) {
            case 'Log':
                if (window.loggingView) {
                    window.loggingView.handleEvent(event, params, eventId);
                }
                break;
            case 'Memory':
                // Memory events handled via window events
                break;
            case 'Network':
                // Network events
                break;
            case 'Debugger':
                // Debugger events
                break;
            default:
                console.log('Unknown event:', method, params);
        }
    }
}

// Initialize DevTools when page loads
window.devtools = new DevTools();
