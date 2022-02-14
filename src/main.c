#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define WORD_LEN 5
#define GUESSES 6
#define ALLOWED_WORDS 2315
#define ALLOWED_GUESSES 12972

enum {
    GRAY, YELLOW, GREEN
};

struct WordleLetter {
    char letter;
    int color;
};

struct WordleLetter wordle[WORD_LEN][GUESSES];
char real_word[WORD_LEN];
char allowed_guesses[ALLOWED_GUESSES][WORD_LEN + 1];
char current_guess[WORD_LEN];
int guess = 0;

const char* decode_color(int color) {
    switch (color) {
    case GRAY: return "\033[0;37m";
    case YELLOW: return "\033[0;33m";
    case GREEN: return "\033[0;32m";
    default: " ";
    }
} 

void print_wordle() {
    for (int word = 0; word < GUESSES; word++) {
        for(int l = 0; l < WORD_LEN; l++) {
            printf("%s", decode_color(wordle[l][word].color));
            if (wordle[l][word].letter == ' ')
                printf("_");
            else {
                printf("%c", wordle[l][word].letter);
            }
        }
        printf("\033[0m");
        printf("\n");
    }
}

void get_word() {
    FILE* fp = fopen("data/possible_words.txt", "r");
    if (!fp) {
        fprintf(stderr, "unable to open 'data/possible_words.txt'.\n");
        return;
    }
    int w_index = rand() % ALLOWED_WORDS;

    char line[256]; 
    int count = 0;
    while (fgets(line, sizeof line, fp) != NULL) {
        if (w_index == count) {
            for (int i = 0; i < WORD_LEN; i++)
                real_word[i] = line[i];
            break;
        }
        count++;
    }
    fclose(fp);
}

void load_allowed_words() {
    FILE *fp = fopen("data/allowed_words.txt", "r");
    if (!fp) {
        fprintf(stderr, "unable to open 'data/allowed_words.txt'.\n");
        return;
    }

    for (int i = 0; i < ALLOWED_GUESSES; i++) {
        char line[256];
        fgets(line, sizeof line, fp);
        for (int j = 0; j < WORD_LEN; j++)
            allowed_guesses[i][j] = line[j];
    }

    fclose(fp);
}

void init_wordle() {
    for (int word = 0; word < GUESSES; word++) {
        for(int l = 0; l < WORD_LEN; l++) {
            wordle[l][word].letter = ' ';
            wordle[l][word].color = GRAY;
        }
    }
}

bool is_guess_valid() {
    return (strlen(current_guess) == WORD_LEN);
}

int main(int argc, char **argv) {
    srand(time(NULL));

    get_word();
    load_allowed_words();
    init_wordle();

    bool quit = false;
    while (!quit) {
        system("cls");
        print_wordle();

        memset(current_guess, 0, WORD_LEN);
        scanf("%s", current_guess);

        if (!is_guess_valid()) {
            continue;
        }

        bool allowed = false;
        for (int i = 0; i < ALLOWED_GUESSES; i++) {
            if (strcmp(allowed_guesses[i], current_guess) == 0) {
                allowed = true;
                break;
            }
        }

        if (!allowed)
            continue;

        bool used[WORD_LEN] =  { false };
        for (int i = 0; i < WORD_LEN; i++) {
            wordle[i][guess].letter = current_guess[i];
            if (real_word[i] == current_guess[i]) {
                wordle[i][guess].color = GREEN;
                used[i] = true;
            }
            else {
                for (int j = 0; j < WORD_LEN; j++) {
                    if (current_guess[i] == real_word[j] && used[j] == false) {
                        wordle[i][guess].color = YELLOW;
                        used[j] = true;
                    }
                }
            }
        }

        guess++;

        if (strcmp(current_guess, real_word) == 0) {
            system("cls");
            print_wordle();
            printf("well done! the correct word was %s\n", real_word);
            break;
        }

        if (guess == GUESSES) {
            system("cls");
            print_wordle();
            printf("you lost, the correct word was %s\n", real_word);
            break;
        }
    }

    free(real_word);

    return 0;
}