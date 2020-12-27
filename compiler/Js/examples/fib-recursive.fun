fun fib(n) [
    if n == 0 [
        return 0
    ]
    if n == 1 [
        return 1
    ]
    fib_1 = fib(n - 1)
    fib_2 = fib(n - 2)
    return fib_1 + fib_2
]

proc main() [
    print(fib(5))
]
