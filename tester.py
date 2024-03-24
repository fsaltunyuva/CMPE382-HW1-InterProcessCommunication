def is_prime(num):
    if num <= 1:
        return False
    for i in range(2, int(num ** 0.5) + 1):
        if num % i == 0:
            return False
    return True


def count_digits(num):
    return len(str(num))


def count_primes_and_digits_in_file(file_path):
    try:
        with open(file_path, 'r') as file:
            numbers = [int(num) for num in file.read().split()]
            digit_count = {}  # Initialize count for 1-digit numbers
            prime_numbers = []
            nonprime_count = 0
            for num in numbers:
                if is_prime(num):
                    prime_numbers.append(num)
                else:
                    nonprime_count += 1
                digit = count_digits(num)
                if digit in digit_count:
                    digit_count[digit] += 1
                else:
                    digit_count[digit] = 1

            print("Prime numbers in the file:", len(prime_numbers))
            print("Non-prime count in the file:", nonprime_count)
            for digit, count in digit_count.items():
                print(f"{digit} digit numbers: {count}")

    except FileNotFoundError:
        print("File not found.")
    except ValueError:
        print("File contains non-integer values.")
    except Exception as e:
        print("An error occurred:", e)


file_path = 'numbers.txt'  # Change this to name of the .txt file
count_primes_and_digits_in_file(file_path)
