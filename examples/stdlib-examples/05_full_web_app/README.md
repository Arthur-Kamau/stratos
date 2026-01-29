# Full-Stack Web Application

A complete web application demonstrating Stratos' database, session, and templating capabilities.

## Features

- User registration and authentication
- Session management with Redis
- Blog posts CRUD with PostgreSQL
- HTML templating with the `template` module
- Middleware for logging and authentication
- Form handling
- JSON API endpoints

## Requirements

- PostgreSQL database
- Redis server
- Stratos runtime

## Configuration

Edit the configuration constants in `src/main.st`:

```stratos
val DB_CONNECTION = "host=localhost port=5432 dbname=webapp user=postgres password=postgres";
val REDIS_HOST = "localhost";
val REDIS_PORT = 6379;
val SESSION_TTL = 3600;
val SERVER_PORT = 8080;
```

## Database Setup

The application automatically creates the required tables on startup:
- `users` - User accounts
- `posts` - Blog posts

## Running

```bash
stratos run src/main.st
```

Then visit: http://localhost:8080

## Routes

| Method | Path | Description |
|--------|------|-------------|
| GET | / | Home page with latest posts |
| GET/POST | /login | User login |
| GET/POST | /register | User registration |
| GET | /logout | Logout |
| GET/POST | /posts/new | Create new post |
| GET | /posts/:id | View post |
| GET | /api/posts | JSON API for posts |

## Architecture

```
src/
  main.st           # Main application code
    - Models        # User, Post classes
    - Database      # PostgreSQL repository
    - Sessions      # Redis session management
    - Templates     # HTML rendering functions
    - Middleware    # Auth, logging
    - Handlers      # Route handlers
```
