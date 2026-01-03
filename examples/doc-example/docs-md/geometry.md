# Package: geometry

Geometry module - Geometric shapes and calculations 

This module provides classes and functions for working with basic geometric shapes including circles, rectangles, and triangles. 



## Dependencies

- math

## Classes

### Class: `Circle`

**Properties:**

- `x: double` - Represents a circle in 2D space.
- `y: double`
- `radius: double`

**Methods:**

#### `constructor(centerX: double, centerY: double, r: double) void`

#### `area() double`

#### `circumference() double`

#### `contains(px: double, py: double) bool`

**Source:** [src/geometry.st:29](src/geometry.st#L29)

---

### Class: `Rectangle`

**Properties:**

- `x: double` - Represents a rectangle in 2D space.
- `y: double`
- `width: double`
- `height: double`

**Methods:**

#### `constructor(topLeftX: double, topLeftY: double, w: double, h: double) void`

#### `area() double`

#### `perimeter() double`

#### `contains(px: double, py: double) bool`

#### `diagonal() double`

**Source:** [src/geometry.st:117](src/geometry.st#L117)

---

### Class: `Triangle`

**Properties:**

- `x1: double` - Represents a triangle in 2D space.
- `y1: double`
- `x2: double`
- `y2: double`
- `x3: double`
- `y3: double`

**Methods:**

#### `constructor(ax: double, ay: double, bx: double, by: double, cx: double, cy: double) void`

#### `area() double`

**Source:** [src/geometry.st:216](src/geometry.st#L216)

---

