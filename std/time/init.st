// Time module - Date and time operations
// Inspired by Go's time package

package time;

// ============================================================================
// Types
// ============================================================================

class Time {
    var timestamp: int;  // Unix timestamp in milliseconds

    fn year() int;
    fn month() int;
    fn day() int;
    fn hour() int;
    fn minute() int;
    fn second() int;
    fn millisecond() int;

    fn format(layout: string) string;
    fn add(duration: Duration) Time;
    fn sub(other: Time) Duration;
    fn before(other: Time) bool;
    fn after(other: Time) bool;
    fn equals(other: Time) bool;
    fn unix() int;
}

class Duration {
    var milliseconds: int;

    fn hours() double;
    fn minutes() double;
    fn seconds() double;
    fn milliseconds() int;

    fn add(other: Duration) Duration;
    fn sub(other: Duration) Duration;
}

class Ticker {
    var channel: Channel<Time>;
    var interval: Duration;

    fn stop() void;
}

class Timer {
    var channel: Channel<Time>;
    var duration: Duration;

    fn stop() void;
    fn reset(duration: Duration) void;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Current time
fn now() Time;
fn unix(seconds: int) Time;
fn fromMillis(millis: int) Time;

// Parsing and formatting
fn parse(value: string, layout: string) Result<Time, Error>;

// Duration creation
fn milliseconds(ms: int) Duration;
fn seconds(s: int) Duration;
fn minutes(m: int) Duration;
fn hours(h: int) Duration;
fn days(d: int) Duration;

// Sleep and timing
fn sleep(duration: Duration) void;
fn sleepUntil(time: Time) void;

// Timers and tickers
fn after(duration: Duration) Channel<Time>;
fn tick(interval: Duration) Ticker;
fn newTimer(duration: Duration) Timer;
fn newTicker(interval: Duration) Ticker;

// Date operations
fn date(year: int, month: int, day: int) Time;
fn dateTime(year: int, month: int, day: int, hour: int, minute: int, second: int) Time;

// ============================================================================
// Constants - Common layouts
// ============================================================================

// Layout formats (Go-style)
val ANSIC = "Mon Jan _2 15:04:05 2006";
val UnixDate = "Mon Jan _2 15:04:05 MST 2006";
val RFC3339 = "2006-01-02T15:04:05Z07:00";
val Kitchen = "3:04PM";
val DateTime = "2006-01-02 15:04:05";
val DateOnly = "2006-01-02";
val TimeOnly = "15:04:05";

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Check if year is leap year
fn isLeapYear(year: int) bool {
    if year % 400 == 0 {
        return true;
    }
    if year % 100 == 0 {
        return false;
    }
    return year % 4 == 0;
}

// Days in month
fn daysInMonth(year: int, month: int) int {
    if month == 2 {
        if isLeapYear(year) {
            return 29;
        }
        return 28;
    }

    val days30 = [4, 6, 9, 11];
    for val m in days30 {
        if month == m {
            return 30;
        }
    }

    return 31;
}

// Add days to time
fn addDays(t: Time, numDays: int) Time {
    return t.add(days(numDays));
}

// Add weeks to time
fn addWeeks(t: Time, weeks: int) Time {
    return t.add(days(weeks * 7));
}

// Add months to time (approximate)
fn addMonths(t: Time, months: int) Time {
    return t.add(days(months * 30));
}

// Add years to time (approximate)
fn addYears(t: Time, years: int) Time {
    return t.add(days(years * 365));
}

// Start of day
fn startOfDay(t: Time) Time {
    return dateTime(t.year(), t.month(), t.day(), 0, 0, 0);
}

// End of day
fn endOfDay(t: Time) Time {
    return dateTime(t.year(), t.month(), t.day(), 23, 59, 59);
}

// Start of month
fn startOfMonth(t: Time) Time {
    return dateTime(t.year(), t.month(), 1, 0, 0, 0);
}

// End of month
fn endOfMonth(t: Time) Time {
    val lastDay = daysInMonth(t.year(), t.month());
    return dateTime(t.year(), t.month(), lastDay, 23, 59, 59);
}

// Age in years
fn ageInYears(birthDate: Time) int {
    val now = now();
    var age = now.year() - birthDate.year();

    if now.month() < birthDate.month() {
        age = age - 1;
    } else if now.month() == birthDate.month() && now.day() < birthDate.day() {
        age = age - 1;
    }

    return age;
}

// Duration between two times in days
fn daysBetween(start: Time, end: Time) int {
    val diff = end.sub(start);
    return diff.milliseconds() / (1000 * 60 * 60 * 24);
}

// Format duration as human-readable
fn humanizeDuration(d: Duration) string {
    val totalSeconds = d.seconds();

    if totalSeconds < 60 {
        return totalSeconds + "s";
    }

    val totalMinutes = d.minutes();
    if totalMinutes < 60 {
        return totalMinutes + "m";
    }

    val totalHours = d.hours();
    if totalHours < 24 {
        return totalHours + "h";
    }

    val totalDays = totalHours / 24;
    return totalDays + "d";
}

// Measure function execution time
fn measure(fn: Function) Duration {
    val start = now();
    fn();
    val end = now();
    return end.sub(start);
}

// Retry with timeout
fn retry(fn: Function<Result>, timeout: Duration, interval: Duration) Result {
    val deadline = now().add(timeout);

    while now().before(deadline) {
        val result = fn();
        if result.ok() {
            return result;
        }
        sleep(interval);
    }

    return Result { isOk: false, error: Error("Timeout") };
}
