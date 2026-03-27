#!/usr/bin/env python3

# Usage     : python3 merge_words.py words.h words_to_add.txt
# Returns   : updated_word_list.h
#
# worde.h should look like:
#
# #pragma once
# #define WORD_COUNT 2315
# #define WORD_LENGTH 5
#
# static const char wordList[WORD_COUNT][WORD_LENGTH + 1] = {
#     "aback",
#     "abase"
# };
#
# words_to_add.txt should be:
# aback
# abase
# etc

import sys
import os
import re


WORD_REGEX = re.compile(r'"([A-Za-z]+)"|^([A-Za-z]+)$')
WORD_LENGTH_REGEX = re.compile(r"#define\s+WORD_LENGTH\s+(\d+)")


def read_existing_header(path):
    if not os.path.isfile(path):
        raise FileNotFoundError(f"File not found: {path}")

    words = []
    word_length = None

    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            # Capture WORD_LENGTH
            length_match = WORD_LENGTH_REGEX.search(line)
            if length_match:
                word_length = int(length_match.group(1))

            # Capture quoted words
            for match in re.findall(r'"([A-Za-z]+)"', line):
                words.append(match.lower())

    if word_length is None:
        raise ValueError("WORD_LENGTH not found in header file")

    return words, word_length


def read_new_words(path, word_length):
    if not os.path.isfile(path):
        raise FileNotFoundError(f"File not found: {path}")

    words = []

    with open(path, "r", encoding="utf-8") as f:
        for line_num, line in enumerate(f, start=1):
            word = line.strip().lower()

            if not word:
                continue

            if not word.isalpha():
                raise ValueError(
                    f"Invalid word '{word}' in {path} on line {line_num}"
                )

            if len(word) > word_length:
                continue  # silently ignore too-long words

            words.append(word)

    return words


def main():
    if len(sys.argv) != 3:
        print(
            "Usage: python3 merge_words.py <words.h> <words_to_add.txt>",
            file=sys.stderr
        )
        sys.exit(1)

    existing_file = sys.argv[1]
    new_file = sys.argv[2]

    existing_words, word_length = read_existing_header(existing_file)
    new_words = read_new_words(new_file, word_length)

    # Pre flight: existing list must be unique
    if len(existing_words) != len(set(existing_words)):
        raise ValueError("Duplicate words found in existing word list")

    merged_set = set(existing_words)
    added_count = 0

    for word in new_words:
        if word not in merged_set:
            merged_set.add(word)
            added_count += 1

    merged_words = sorted(merged_set)
    word_count = len(merged_words)

    output_file = "updated_word_list.h" # Filename to export too

    with open(output_file, "w", encoding="utf-8") as f:
        f.write("#pragma once\n\n")
        f.write(f"#define WORD_COUNT {word_count}\n")
        f.write(f"#define WORD_LENGTH {word_length}\n\n")
        f.write(
            "static const char wordList[WORD_COUNT][WORD_LENGTH + 1] = {\n"
        )

        for i, word in enumerate(merged_words):
            comma = "," if i < word_count - 1 else "" # Removes extra comma at the end of list
            f.write(f'    "{word}"{comma}\n')

        f.write("};\n")

    print("Done :)")
    print(f"WORD_LENGTH    : {word_length}")
    print(f"Existing words : {len(existing_words)}")
    print(f"New words read : {len(new_words)}")
    print(f"Words added    : {added_count}")
    print(f"Output written : {output_file}")
    print(f"New WORD_COUNT : {word_count}")


if __name__ == "__main__":
    main()
