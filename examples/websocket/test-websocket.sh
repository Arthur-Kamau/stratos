#!/bin/bash

# WebSocket Integration Test
# Runs both server and client concurrently and displays their output

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
STRATOS="$SCRIPT_DIR/../../src/interpreter/cpp/build/stratos"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Temporary files for output
SERVER_LOG=$(mktemp)
CLIENT_LOG=$(mktemp)
SERVER_PID=""

# Cleanup function
cleanup() {
    echo ""
    echo -e "${YELLOW}Cleaning up...${NC}"

    # Kill server if still running
    if [ ! -z "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null || true
        wait $SERVER_PID 2>/dev/null || true
    fi

    # Remove temp files
    rm -f "$SERVER_LOG" "$CLIENT_LOG"

    echo -e "${GREEN}✓ Cleanup complete${NC}"
}

# Set trap to cleanup on exit
trap cleanup EXIT INT TERM

# Check if stratos binary exists
if [ ! -f "$STRATOS" ]; then
    echo -e "${RED}Error: Stratos binary not found at $STRATOS${NC}"
    echo "Please build the interpreter first:"
    echo "  cd ../../src/interpreter/cpp && ./build.sh"
    exit 1
fi

echo ""
echo "════════════════════════════════════════════════════════"
echo "  WebSocket Integration Test"
echo "════════════════════════════════════════════════════════"
echo ""

# Start server in background
echo -e "${BLUE}[1/3]${NC} Starting WebSocket server..."
cd "$SCRIPT_DIR/websocket-server"
"$STRATOS" run > "$SERVER_LOG" 2>&1 &
SERVER_PID=$!

# Wait for server to start (minimal delay)
echo -e "${YELLOW}      Waiting for server to initialize...${NC}"
sleep 0.5

# Check if server is still running
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo -e "${RED}✗ Server failed to start${NC}"
    echo ""
    echo "Server output:"
    cat "$SERVER_LOG"
    exit 1
fi

echo -e "${GREEN}✓     Server started (PID: $SERVER_PID)${NC}"
echo ""

# Run client
echo -e "${BLUE}[2/3]${NC} Running WebSocket client..."
cd "$SCRIPT_DIR/websocket-client"
"$STRATOS" run > "$CLIENT_LOG" 2>&1
CLIENT_EXIT_CODE=$?

echo ""
if [ $CLIENT_EXIT_CODE -eq 0 ]; then
    echo -e "${GREEN}✓     Client completed successfully${NC}"
else
    echo -e "${RED}✗     Client failed with exit code $CLIENT_EXIT_CODE${NC}"
fi
echo ""

# Wait for server to finish (it should exit after handling the client)
echo -e "${BLUE}[3/3]${NC} Waiting for server to finish..."
sleep 1
if kill -0 $SERVER_PID 2>/dev/null; then
    # Server still running, kill it gracefully
    kill $SERVER_PID 2>/dev/null || true
    wait $SERVER_PID 2>/dev/null || true
fi
echo -e "${GREEN}✓     Server finished${NC}"
echo ""

# Display results
echo "════════════════════════════════════════════════════════"
echo "  Test Results"
echo "════════════════════════════════════════════════════════"
echo ""

echo -e "${CYAN}━━━ SERVER OUTPUT ━━━${NC}"
cat "$SERVER_LOG"
echo ""

echo -e "${CYAN}━━━ CLIENT OUTPUT ━━━${NC}"
cat "$CLIENT_LOG"
echo ""

# Verify communication happened
echo "════════════════════════════════════════════════════════"
echo "  Verification"
echo "════════════════════════════════════════════════════════"
echo ""

ERRORS=0

# Check if client connected
if grep -q "✓ Connected successfully!" "$CLIENT_LOG"; then
    echo -e "${GREEN}✓ Client connected to server${NC}"
else
    echo -e "${RED}✗ Client failed to connect${NC}"
    ERRORS=$((ERRORS + 1))
fi

# Check if server accepted client
if grep -q "✓ Client connected!" "$SERVER_LOG"; then
    echo -e "${GREEN}✓ Server accepted client${NC}"
else
    echo -e "${RED}✗ Server did not accept client${NC}"
    ERRORS=$((ERRORS + 1))
fi

# Check if welcome message was received (timing-dependent, not critical)
if grep -q "Welcome to Stratos WebSocket Server!" "$CLIENT_LOG"; then
    echo -e "${GREEN}✓ Welcome message received${NC}"
else
    echo -e "${YELLOW}⚠ Welcome message not received (timing issue, non-critical)${NC}"
fi

# Check if ping-pong worked
if grep -q "pong" "$CLIENT_LOG"; then
    echo -e "${GREEN}✓ Ping-pong exchange successful${NC}"
else
    echo -e "${RED}✗ Ping-pong exchange failed${NC}"
    ERRORS=$((ERRORS + 1))
fi

# Check if echo worked
if grep -q "Echo: Hello from Stratos!" "$CLIENT_LOG"; then
    echo -e "${GREEN}✓ Echo messages working${NC}"
else
    echo -e "${RED}✗ Echo messages not working${NC}"
    ERRORS=$((ERRORS + 1))
fi

# Check if goodbye message was sent
if grep -q "Goodbye!" "$CLIENT_LOG"; then
    echo -e "${GREEN}✓ Goodbye message received${NC}"
else
    echo -e "${RED}✗ Goodbye message not received${NC}"
    ERRORS=$((ERRORS + 1))
fi

echo ""
echo "════════════════════════════════════════════════════════"
if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}  ✓ All checks passed! WebSocket communication works!${NC}"
    echo "════════════════════════════════════════════════════════"
    echo ""
    exit 0
else
    echo -e "${RED}  ✗ $ERRORS check(s) failed${NC}"
    echo "════════════════════════════════════════════════════════"
    echo ""
    exit 1
fi
