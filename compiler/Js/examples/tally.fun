proc main() [
    text = "What is this thing called love?"
    tally = {}
    for char in split(text, "") [
        if tally[char] [
            tally[char] = tally[char] + 1
        ] else [
            tally[char] = 1
        ]
    ]
    print(tally)
]
