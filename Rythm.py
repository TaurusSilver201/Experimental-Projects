import nltk
from nltk.corpus import cmudict

def get_rhyming_words(word):
    # Load CMU Pronouncing Dictionary
    nltk.download('cmudict')
    cmu_dict = cmudict.dict()

    # Get phonetic representation of the word
    try:
        phonemes = cmu_dict[word.lower()][0]
    except KeyError:
        return []

    # Find words that rhyme with the given word
    rhyming_words = []
    for w, pronunciations in cmu_dict.items():
        for pron in pronunciations:
            if pron[-2:] == phonemes[-2:]:
                rhyming_words.append(w)

    return rhyming_words

def main():
    user_input = input("Enter a word: ")
    rhyming_words = get_rhyming_words(user_input)
    if rhyming_words:
        print("Rhyming words:")
        print(rhyming_words)
    else:
        print("No rhyming words found.")

if __name__ == "__main__":
    main()
