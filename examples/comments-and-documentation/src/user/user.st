package user;

class User {
    pub var name: string;
    pub var email: string;
    var isLoggedIn: bool;

    constructor(name: string, email: string) {
        this.name = name;
        this.email = email;
        this.isLoggedIn = false;
    }

    pub fn login(password: string) bool {
        if (password == "password123") {
            this.isLoggedIn = true;
            return true;
        }
        return false;
    }

    pub fn logout() void {
        this.isLoggedIn = false;
    }
}
