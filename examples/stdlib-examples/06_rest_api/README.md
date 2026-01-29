# REST API Example

A comprehensive REST API demonstrating CRUD operations, validation, rate limiting, and API versioning.

## Features

- Full CRUD operations for products
- Request validation with detailed error messages
- Rate limiting with Redis
- CORS middleware
- API versioning (v1)
- Pagination support
- JSON responses with consistent format

## Requirements

- PostgreSQL database
- Redis server
- Stratos runtime

## Configuration

Edit the configuration constants in `src/main.st`:

```stratos
val DB_CONNECTION = "host=localhost port=5432 dbname=restapi user=postgres password=postgres";
val REDIS_HOST = "localhost";
val REDIS_PORT = 6379;
val SERVER_PORT = 8080;
val RATE_LIMIT_MAX = 100;
val RATE_LIMIT_WINDOW = 60;
```

## Running

```bash
stratos run src/main.st
```

## API Endpoints

### Health Check

```
GET /health
```

Response:
```json
{"status":"healthy","version":"1.0.0"}
```

### List Products

```
GET /api/v1/products?limit=20&offset=0&category=electronics
```

Response:
```json
{
  "success": true,
  "data": [...],
  "meta": {
    "total": 100,
    "limit": 20,
    "offset": 0,
    "totalPages": 5,
    "currentPage": 1
  }
}
```

### Get Product

```
GET /api/v1/products/:id
```

Response:
```json
{
  "success": true,
  "data": {
    "id": 1,
    "name": "Laptop",
    "description": "A powerful laptop",
    "price": 999.99,
    "stock": 10,
    "category": "electronics",
    "createdAt": "2024-01-01T00:00:00Z",
    "updatedAt": "2024-01-01T00:00:00Z"
  }
}
```

### Create Product

```
POST /api/v1/products
Content-Type: application/json

{
  "name": "Laptop",
  "description": "A powerful laptop",
  "price": 999.99,
  "stock": 10,
  "category": "electronics"
}
```

### Update Product

```
PUT /api/v1/products/:id
Content-Type: application/json

{
  "name": "Updated Name",
  "price": 899.99
}
```

### Delete Product

```
DELETE /api/v1/products/:id
```

## Error Responses

### Validation Error

```json
{
  "success": false,
  "error": {
    "message": "Validation failed",
    "status": 400,
    "details": [
      {"field": "name", "message": "Name is required"},
      {"field": "price", "message": "Price must be non-negative"}
    ]
  }
}
```

### Not Found

```json
{
  "success": false,
  "error": {
    "message": "Product not found",
    "status": 404
  }
}
```

### Rate Limited

```json
{
  "success": false,
  "error": {
    "message": "Rate limit exceeded. Please try again later.",
    "status": 429
  }
}
```

## Rate Limit Headers

All responses include rate limit headers:

```
X-RateLimit-Limit: 100
X-RateLimit-Remaining: 95
X-RateLimit-Reset: 45
```

## Testing with curl

```bash
# List products
curl http://localhost:8080/api/v1/products

# Create product
curl -X POST http://localhost:8080/api/v1/products \
  -H "Content-Type: application/json" \
  -d '{"name":"Test","price":9.99,"stock":5}'

# Get product
curl http://localhost:8080/api/v1/products/1

# Update product
curl -X PUT http://localhost:8080/api/v1/products/1 \
  -H "Content-Type: application/json" \
  -d '{"price":19.99}'

# Delete product
curl -X DELETE http://localhost:8080/api/v1/products/1
```
