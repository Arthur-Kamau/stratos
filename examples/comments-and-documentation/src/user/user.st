package user;

/**
 * Represents a registered user in the system.
 *
 * The User class handles:
 * - Profile storage
 * - Authentication status
 * - Session management
 *
 * @example
 * val user = User("Bob", "bob@email.com");
 * user.login("secret");
 */
class User {
    /** The display name of the user. */
    var name: string;

    /** 
     * The unique email address.
     * Used for login and notifications.
     */
    var email: string;

    /** Internal state tracking login status. */
    var isLoggedIn: bool;

    /**
     * Creates a new User instance.
     *
     * @param name Full display name
     * @param email Valid email address
     */
    constructor(name: string, email: string) {
        this.name = name;
        this.email = email;
        this.isLoggedIn = false;
    }

    /**
     * Attempts to log the user in.
     *
     * @param password The raw password string
     * @return `true` if credentials are correct
     */
    fn login(password: string) bool {
        // Mock authentication
        if (password == "password123") {
            this.isLoggedIn = true;
            return true;
        }
        return false;
    }

    /**
     * Logs the user out of the current session.
     */
    fn logout() void {
        this.isLoggedIn = false;
    }
}
