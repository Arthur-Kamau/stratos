#!/bin/bash

# WebSocket Daemon Test
# Tests the daemon server with multiple sequential clients

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
STRATOS="$SCRIPT_DIR/../../src/interpreter/cpp/build/stratos"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

SERVER_LOG=$(mktemp)
SERVER_PID=""

cleanup() {
    echo ""
    echo -e "${YELLOW}Cleaning up...${NC}"

    if [ ! -z "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null || true
        wait $SERVER_PID 2>/dev/null || true
    fi

    rm -f "$SERVER_LOG"
    echo -e "${GREEN}✓ Cleanup complete${NC}"
}

trap cleanup EXIT INT TERM

if [ ! -f "$STRATOS" ]; then
    echo -e "${RED}Error: Stratos binary not found${NC}"
    exit 1
fi

echo ""
echo "════════════════════════════════════════════════════════"
echo "  WebSocket Daemon Test"
echo "════════════════════════════════════════════════════════"
echo ""

# Start daemon server
echo -e "${BLUE}[1/4]${NC} Starting daemon server..."
cd "$SCRIPT_DIR/websocket-server"
"$STRATOS" run src/server-daemon.st > "$SERVER_LOG" 2>&1 &
SERVER_PID=$!

sleep 1

if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo -e "${RED}✗ Server failed to start${NC}"
    cat "$SERVER_LOG"
    exit 1
fi

echo -e "${GREEN}✓     Server running (PID: $SERVER_PID)${NC}"
echo ""

# Run multiple clients
NUM_CLIENTS=3
echo -e "${BLUE}[2/4]${NC} Testing with $NUM_CLIENTS sequential clients..."
echo ""

for i in $(seq 1 $NUM_CLIENTS); do
    echo -e "${CYAN}  Client #$i${NC}"
    cd "$SCRIPT_DIR/websocket-client"
    "$STRATOS" run src/client-interactive.st 2>&1 | sed 's/^/    /'

    if [ $? -eq 0 ]; then
        echo -e "  ${GREEN}✓ Client #$i completed${NC}"
    else
        echo -e "  ${RED}✗ Client #$i failed${NC}"
    fi
    echo ""
    sleep 0.5
done

echo -e "${BLUE}[3/4]${NC} All clients completed"
echo ""

# Show server log
echo -e "${BLUE}[4/4]${NC} Server activity:"
echo ""
echo -e "${CYAN}━━━ SERVER LOG ━━━${NC}"
cat "$SERVER_LOG"
echo ""

# Verify
echo "════════════════════════════════════════════════════════"
echo "  Verification"
echo "════════════════════════════════════════════════════════"
echo ""

ERRORS=0

# Count client connections in server log
CLIENT_COUNT=$(grep -c "New client connected!" "$SERVER_LOG" || echo "0")
if [ "$CLIENT_COUNT" -eq "$NUM_CLIENTS" ]; then
    echo -e "${GREEN}✓ Server handled $CLIENT_COUNT clients${NC}"
else
    echo -e "${RED}✗ Expected $NUM_CLIENTS clients, got $CLIENT_COUNT${NC}"
    ERRORS=$((ERRORS + 1))
fi

# Check for ping-pong
if grep -q "pong" "$SERVER_LOG"; then
    echo -e "${GREEN}✓ Ping-pong working${NC}"
else
    echo -e "${RED}✗ Ping-pong not working${NC}"
    ERRORS=$((ERRORS + 1))
fi

# Check for echo
if grep -q "Echo:" "$SERVER_LOG"; then
    echo -e "${GREEN}✓ Echo functionality working${NC}"
else
    echo -e "${RED}✗ Echo not working${NC}"
    ERRORS=$((ERRORS + 1))
fi

echo ""
echo "════════════════════════════════════════════════════════"
if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}  ✓ Daemon server test passed!${NC}"
    echo "════════════════════════════════════════════════════════"
    echo ""
    exit 0
else
    echo -e "${RED}  ✗ $ERRORS check(s) failed${NC}"
    echo "════════════════════════════════════════════════════════"
    echo ""
    exit 1
fi
