import random

def guess_game():
    print("Welcome to the Guessing Game!")
    print("I'm thinking of a number between 1 and 100.")
    secret_number = random.randint(1, 100)
    guess_count = 0
    max_guesses = 7

    while guess_count < max_guesses:
        guess = int(input("Take a guess: "))
        guess_count += 1

        if guess < secret_number:
            print("Too low! Try again.")
        elif guess > secret_number:
            print("Too high! Try again.")
        else:
            print(f"Congratulations! You guessed the number {secret_number} in {guess_count} guesses!")
            return

    print(f"Sorry, you ran out of guesses. The number was {secret_number}.")

guess_game()
