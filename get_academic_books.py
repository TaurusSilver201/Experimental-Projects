import requests
from bs4 import BeautifulSoup

def get_academic_books(subject):
    # Perform a Google search
    search_query = f"academic books on {subject}"
    search_url = f"https://www.google.com/search?q={search_query}"
    headers = {'User-Agent': 'Mozilla/5.0'}
    response = requests.get(search_url, headers=headers)

    # Parse the search results using BeautifulSoup
    soup = BeautifulSoup(response.text, 'html.parser')
    book_results = soup.find_all('h3', class_='LC20lb DKV0Md')

    # Extract book titles and return
    books = [book.text for book in book_results]
    return books

def main():
    subject = input("Enter a subject: ")
    academic_books = get_academic_books(subject)

    if academic_books:
        print(f"Suggested academic books on {subject}:")
        for book in academic_books:
            print(book)
    else:
        print(f"No academic books found on {subject}.")

if __name__ == "__main__":
    main()
