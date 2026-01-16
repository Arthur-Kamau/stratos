package main;

class BankAccount {
    var balance: int;
    var owner: string;

    constructor(owner: string, initialBalance: int) {
        this.owner = owner;
        this.balance = initialBalance;
    }

      pub  fn getOwner() string {
        return this.owner;
    }

  pub  fn deposit(amount: int) void {
        if amount > 0 {
            this.balance = this.balance + amount;
            println(this.owner + " deposited " + amount);
        }
    }

  pub  fn withdraw(amount: int) bool {
        if amount <= this.balance {
            this.balance = this.balance - amount;
            println(this.owner + " withdrew " + amount);
            return true;
        } else {
            println("Insufficient funds for " + this.owner);
            return false;
        }
    }

 pub   fn getBalance() int {
        return this.balance;
    }
}

fn main() {
    println("--- OOP Classes Demo ---");
    
    val account = BankAccount("Alice", 100);
    println("Created account for " + account.getOwner());
    
    account.deposit(50);
    println("Balance: " + account.getBalance());
    
    account.withdraw(30);
    println("Balance: " + account.getBalance());
    
    account.withdraw(200); // Should fail
}