# Real-time Application Example

A real-time dashboard and chat application demonstrating Server-Sent Events (SSE) and Redis pub/sub.

## Features

- Live metrics dashboard with auto-updating stats
- Real-time chat room
- Server-Sent Events (SSE) for push notifications
- Redis pub/sub for message distribution
- Traffic simulation for testing
- Active user tracking

## Requirements

- Redis server
- Stratos runtime

## Configuration

Edit the configuration constants in `src/main.st`:

```stratos
val REDIS_HOST = "localhost";
val REDIS_PORT = 6379;
val SERVER_PORT = 8080;
```

## Running

```bash
stratos run src/main.st
```

Then visit: http://localhost:8080

## Pages

### Dashboard (/)

A live metrics dashboard showing:
- Page views counter
- API calls counter
- Error counter
- Average response time
- Active users count
- Real-time event stream

The dashboard updates automatically via Server-Sent Events.

### Chat Room (/chat)

A real-time chat room where multiple users can communicate. Messages are:
- Stored in Redis for persistence
- Broadcast via SSE for real-time delivery
- Limited to last 100 messages

### Traffic Simulator (/simulate)

A tool to generate test traffic:
- Simulate page views
- Simulate API calls
- Simulate errors
- Burst mode (10 events at once)
- Reset all metrics

## Technical Details

### Server-Sent Events

The application uses SSE for real-time updates:

```javascript
const evtSource = new EventSource('/events');
evtSource.onmessage = (e) => {
    const data = JSON.parse(e.data);
    // Update UI
};
```

### Redis Data Structures

**Metrics:**
- `metrics:pageviews` - Counter (String)
- `metrics:apicalls` - Counter (String)
- `metrics:errors` - Counter (String)
- `metrics:response_times` - List of response times
- `metrics:active_users` - Set of user IDs (with TTL)

**Chat:**
- `chat:general:messages` - List of messages (last 100)
- `chat:general` - Pub/sub channel for real-time delivery

### API Endpoints

| Method | Path | Description |
|--------|------|-------------|
| GET | / | Dashboard page |
| GET | /events | SSE stream for dashboard |
| GET | /chat | Chat room page |
| GET | /chat/events | SSE stream for chat |
| POST | /chat/send | Send chat message |
| GET | /simulate | Traffic simulator page |
| POST | /api/simulate/pageview | Simulate page view |
| POST | /api/simulate/api | Simulate API call |
| POST | /api/simulate/error | Simulate error |
| POST | /api/simulate/burst | Simulate 10 events |
| POST | /api/reset | Reset all metrics |

## Architecture

```
Browser <--SSE--> HTTP Server <--Pub/Sub--> Redis
                      |
                      v
                  Metrics Store
```

1. Browser connects to SSE endpoint
2. Server pushes updates when metrics change
3. Redis stores metrics and handles pub/sub
4. Multiple browser tabs stay in sync

## Screenshots

The dashboard features a dark theme with:
- Grid of metric cards with large numbers
- Real-time event log with color-coded entries
- Connection status indicator
- Navigation between pages
