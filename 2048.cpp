#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#define SIZE 25

int mainOpt, opt1;

int insideGrid[7][7];

char tempID[6];
char tempName[16];
int tempScore;

bool added;
bool full;
bool gameOver;

int firstPossibility[10] = {2, 2, 2, 2, 2, 2, 2, 4, 4, 4};
int secondPossibility[10] = {2, 2, 2, 4, 4, 4, 4, 4, 4, 4};

struct Player {
    char id[6];
    char name[16];
    int score;

    Player *next;
    Player *prev;
} *head[SIZE], *tail[SIZE];

struct Rank {
	char id[6];
    char name[16];
    int score;

	Rank *next;
	Rank *prev;
} *start, *end;

Rank *createRank(char id[], char name[], int score) {
	Rank* newRank = (Rank*) malloc(sizeof(Rank));
	strcpy(newRank -> id, id);
	strcpy(newRank -> name, name);
	newRank -> score = score;
	newRank -> next = newRank -> prev = NULL;
	
	return newRank;
}

void pushHead(char id[], char name[], int score) {
	Rank *newRank = createRank(id, name, score);
	
	if (start == NULL) {
		start = end = newRank;
	} else {
		newRank -> next = start;
		start -> prev = newRank;
		start = newRank;
	}
	
	start -> prev = NULL;
}

void pushTail(char id[], char name[], int score) {
	Rank *newRank = createRank(id, name, score);
	
	if (start == NULL) {
		start = end = newRank;
	} else {
		newRank -> prev = end;
		end -> next = newRank;
		end = newRank;
	}
	
	end -> next = NULL;
}

// Descending dari score
void pushMid(char id[], char name[], int score) {
	Rank *newRank = createRank(id, name, score);
	
	if (start == NULL) {
		start = end = newRank;
	} else if (newRank -> score >= start -> score) {
		pushHead(id, name, score);
	} else if (newRank -> score <= end -> score) {
		pushTail(id, name, score);
	} else {
		Rank *curr = start;
		
		while (curr -> next != NULL && curr -> next -> score >= newRank -> score) {
			curr = curr -> next;
		}
		
		newRank -> next = curr -> next;
		newRank -> prev = curr;
		curr -> next -> prev = newRank;
		curr -> next = newRank;
	}
}

void popHead() {
	if (start != NULL) {
		Rank *temp = start;
		
		if (start == end) {
			start = end = NULL;
			free(temp);
		} else {
			start = start -> next;
			start -> prev = NULL;
			free(temp);
		}
	}
}

void popTail() {
	if (start != NULL) {
		Rank *temp = end;
		
		if (start == end) {
			start = end = NULL;
			free(temp);
		} else {
			end = end -> prev;
			end -> next = NULL;
			free(temp);
		}
	}
}

void popMid(char id[]) {
	if (start != NULL) {
		if (strcmp(id, start -> id) == 0) {
			popHead();
		} else if (strcmp(id, end -> id) == 0) {
			popTail();
		} else {
			Rank *curr = start;
			
			while (curr -> next != NULL && strcmp(id, curr -> next -> id) != 0) {
				curr = curr -> next;
			}
			
			if (curr -> next != NULL) {
				Rank *temp = curr -> next;
				curr -> next = temp -> next;
				temp -> next -> prev = curr;
				free(temp);
			}
		}
	}
}

char generateLetter(char name[], int letter) {
    return name[letter];
}

char randomNumber() {
    return '0' + (rand() % 10);
}

void generateID(char name[]) {
    tempID[0] = generateLetter(name, 0);
    tempID[1] = generateLetter(name, 1);
    
    for (int i = 0; i < 2; i++) {
	    if (tempID[i] <= 122 && tempID[i] >= 97) {
	        tempID[i] -= 32;
	    }
	}

    srand(time(0));

    tempID[2] = randomNumber();
    tempID[3] = randomNumber();
    tempID[4] = randomNumber();
    tempID[5] = '\0';
}

Player *createPlayer(char id[], char name[], int score) {
	Player* newPlayer = (Player*) malloc(sizeof(Player));
	strcpy(newPlayer -> id, id);
	strcpy(newPlayer -> name, name);
	newPlayer -> score = score;

	newPlayer -> next = newPlayer -> prev = NULL;
	
	return newPlayer;
}

int hash(char id[]) {
	int x = 0;
	
	for (int i = 0; i < 5; i++) {
		x += id[i];
	}
	
	return x % SIZE;
}

void insertion(char id[], char name[], int score) {
	Player *newPlayer = createPlayer(id, name, score);
	
	int index = hash(id);
	
	if (!head[index]) {
		head[index] = tail[index] = newPlayer;
	} else {	
		if (strcmp(name, head[index] -> name) <= 0) {
			newPlayer -> next = head[index];
			head[index] -> prev = newPlayer;
			head[index] = newPlayer;
			
		} else if (strcmp(name, head[index] -> name) >= 0) {
			newPlayer -> prev = tail[index];
			tail[index] -> next = newPlayer;
			tail[index] = newPlayer;
			
		} else {
			Player *curr = head[index];
			
			while (curr && strcmp(name, curr -> name) > 0) {
				curr = curr -> next;
			}
			
			curr -> prev -> next = newPlayer;
			newPlayer -> prev = curr -> prev;
			newPlayer -> next = curr;
			curr -> prev = newPlayer;
		}
	}
}

void readFile() {
    FILE *fptr = fopen("highscore.txt", "r");
	
	if (fptr == NULL) {
		puts("File doesn't exist");
	}

	while (!feof(fptr)) {
		fscanf(fptr, "%[^#]#%[^#]#%d\n", tempID, tempName, &tempScore);
        insertion(tempID, tempName, tempScore);
        pushMid(tempID, tempName, tempScore);
	}
	
	fclose(fptr);
}

void writeFile() {
	FILE *fptr = fopen("highscore.txt", "w");
	
    for (int i = 0; i < SIZE; i++) {
        if (head[i]) {
            Player *curr = head[i];
            
            while (curr) {
                fprintf(fptr, "%s#%s#%d\n", curr -> id, curr -> name, curr -> score);
                curr = curr -> next;
            }
        }
    }
	
	fclose(fptr);
}

void display() {
    system("cls");

	if (head == NULL) {
		printf("Kosong\n");
	} else {
        puts("No.\t|ID\t|Username\t|Score");
        
        Rank *curr = start;
		
		int i = 1;
		while (curr != NULL && i < 11) {
            if (strlen(curr -> name) < 7) {
                printf("%d\t|%s\t|%s\t\t|%d\n", i, curr -> id, curr -> name, curr -> score);
            } else if (strlen(curr -> name) > 14){
                printf("%d\t|%s\t|%s|%d\n", i, curr -> id, curr -> name, curr -> score);
            } else {
                printf("%d\t|%s\t|%s\t|%d\n", i, curr -> id, curr -> name, curr -> score);
            }

			curr = curr -> next;
			i++;
		}
        
        printf("\nPress enter to continue..."); getchar();
	}
}

void mainMenu() {
    system("cls");

    puts("1. New Game");
    puts("2. High Score");
    puts("3. Exit");
    printf(">> ");

    scanf("%d", &mainOpt); getchar();

    if (mainOpt < 1 || mainOpt > 3) return mainMenu();
}

void inputGrid() {
    printf("Input Grid Size [4 or 6]: ");
    scanf("%d", &opt1); getchar();

    if (opt1 != 4 && opt1 != 6) return inputGrid();
}

void initializeGrid(int opt) {
    srand(time(0));

    for (int i = 0; i < opt; i++) {
        for (int j = 0; j < opt; j++) {
            insideGrid[i][j] = 0;
        }
    }

    for (int i = 0; i < 2; i++) {
        int x = rand() % opt;
        int y = rand() % opt;
        int temp = (rand() % 2 + 1) * 2;
        insideGrid[x][y] = temp;
    }
}

void displayGrid(int opt) {
    for (int i = 0; i < opt; i++) {
        for (int j = 0; j < opt; j++) {
            printf("%-4d", insideGrid[i][j]);
        }
        puts("");
    }
}

void generateNew(int opt) {
    int x, y;
    
	do {
		x = rand() % opt;
	    y = rand() % opt;
	    
	} while (insideGrid[x][y] != 0);
    
    int temp;

    if (tempScore < 1000) {
        temp = firstPossibility[rand() % 10];
    } else {
        temp = secondPossibility[rand() % 10];
    }

    insideGrid[x][y] = temp;
    
}

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void upward(int opt) {
    added = false;
    
    for (int j = 0; j < opt; j++) {
        for (int k = 0; k < opt - 1; k++) {
            for (int i = 0; i < opt - k - 1; i++) {
                if (insideGrid[i][j] == 0) {
                    swap(&insideGrid[i][j], &insideGrid[i + 1][j]);
                }
            }
        }
    }

    for (int j = 0; j < opt; j++) {
        for (int i = 0; i < opt - 1; i++) {
            if (insideGrid[i][j] == insideGrid[i + 1][j]) {
                insideGrid[i + 1][j] = 0;
                insideGrid[i][j] *= 2;
                tempScore += insideGrid[i][j];
                added = true;
            }
        }
    }
    
    if (added) {
        for (int j = 0; j < opt; j++) {
            for (int k = 0; k < opt - 1; k++) {
                for (int i = 0; i < opt - k - 1; i++) {
                    if (insideGrid[i][j] == 0) {
                        swap(&insideGrid[i][j], &insideGrid[i + 1][j]);
                    }
                }
            }
        } 
    }
}

void downward(int opt) {
    added = false;
    
    for (int j = 0; j < opt; j++) {
        for (int k = 0; k < opt - 1; k++) {
            for (int i = 0; i < opt - k - 1; i++) {
                if (insideGrid[i + 1][j] == 0) {
                    swap(&insideGrid[i][j], &insideGrid[i + 1][j]);
                }
            }
        }
    }

    for (int j = opt - 1; j >= 0; j--) {
        for (int i = opt - 1; i >= 0; i--) {
            if (insideGrid[i][j] == insideGrid[i + 1][j]) {
                insideGrid[i][j] = 0;
                insideGrid[i + 1][j] *= 2;
                tempScore += insideGrid[i + 1][j];
                added = true;
            }
        }
    }
    
    if (added) {
        for (int j = 0; j < opt; j++) {
            for (int k = 0; k < opt - 1; k++) {
                for (int i = 0; i < opt - k - 1; i++) {
                    if (insideGrid[i + 1][j] == 0) {
                        swap(&insideGrid[i][j], &insideGrid[i + 1][j]);
                    }
                }
            }
        }
    }
}

void left(int opt) {
    added = false;
    
    for (int i = 0; i < opt; i++) {
        for (int k = 0; k < opt - 1; k++) {
            for (int j = 0; j < opt - k - 1; j++) {
                if (insideGrid[i][j] == 0) {
                    swap(&insideGrid[i][j], &insideGrid[i][j + 1]);
                }
            }
        }
    }

    for (int i = 0; i < opt; i++) {
        for (int j = 0; j < opt - 1; j++) {
            if (insideGrid[i][j] == insideGrid[i][j + 1]) {
                insideGrid[i][j + 1] = 0;
                insideGrid[i][j] *= 2;
                tempScore += insideGrid[i][j];
                added = true;
            }
        }
    }
    
    if (added) {
        for (int i = 0; i < opt; i++) {
            for (int k = 0; k < opt - 1; k++) {
                for (int j = 0; j < opt - k - 1; j++) {
                    if (insideGrid[i][j] == 0) {
                        swap(&insideGrid[i][j], &insideGrid[i][j + 1]);
                    }
                }
            }
        }
    }
}

void right(int opt) {
    added = false;

    for (int i = 0; i < opt; i++) {
        for (int k = 0; k < opt - 1; k++) {
            for (int j = 0; j < opt - k - 1; j++) {
                if (insideGrid[i][j + 1] == 0) {
                    swap(&insideGrid[i][j], &insideGrid[i][j + 1]);
                }
            }
        }
    }

    for (int i = opt - 1; i >= 0; i--) {
        for (int j = opt - 1; j >= 0; j--) {
            if (insideGrid[i][j] == insideGrid[i][j + 1]) {
                insideGrid[i][j] = 0;
                insideGrid[i][j + 1] *= 2;
                tempScore += insideGrid[i][j + 1];
                added = true;
            }
        }
    }
    
    if (added) {
        for (int i = 0; i < opt; i++) {
            for (int k = 0; k < opt - 1; k++) {
                for (int j = 0; j < opt - k - 1; j++) {
                    if (insideGrid[i][j + 1] == 0) {
                        swap(&insideGrid[i][j], &insideGrid[i][j + 1]);
                    }
                }
            }
        }
    }
}

void checkFull(int opt) {
    for (int i = 0; i < opt; i++) {
        for (int j = 0; j < opt; j++) {
            if (insideGrid[i][j] == 0) {
                full = false;
            }
        }
    }
}

void check(int opt) {
    checkFull(opt);

    for (int i = 0; i < opt; i++) {
        for (int j = 0; j < opt - 1; j++) {
            if (insideGrid[i][j] == insideGrid[i][j + 1] || !full) {
                gameOver = false;
            }
        }
    }

    for (int j = opt - 1; j >= 0; j--) {
        for (int i = opt - 1; i >= 0; i--) {
            if (insideGrid[i][j] == insideGrid[i + 1][j] || !full) {
                gameOver = false;
            }
        }
    }
}

void game(int opt) {
    system("cls");
    printf("Score : %d\n\n", tempScore);
    puts("[Press enter to exit]\n\n");
    displayGrid(opt);

    full = true;
    gameOver = true;

    char direction = getche();

    switch (direction) {
        case 'W':
        case 'w':
            upward(opt);
            break;
        case 'S':
        case 's':
            downward(opt);
            break;
        case 'A':
        case 'a':
            left(opt);
            break;
        case 'D':
        case 'd':
            right(opt);
            break;
        case 13:
            return;
        default:
            return game(opt);
    }

    check(opt);
    if (gameOver) {
        printf("\bPress enter to continue..."); getchar();
        return;
    }
        
    if (full) return game(opt);
    
    generateNew(opt);

    return game(opt);
}

void inputPlayer() {
    printf("Input Name [name length must be between 3-15]: ");
    scanf("%s", tempName);

    if (strlen(tempName) < 3 || strlen(tempName) > 15) return inputPlayer();

    generateID(tempName);

    insertion(tempID, tempName, tempScore);
    pushMid(tempID, tempName, tempScore);
}

void exitLogo() {
    system("cls");
    puts("                            .:^:.. ..^^:..");
    puts("                          ...   ...:.     .......    .");
    puts("                            .   . ^.   :..:~~77~~~::..::.. ..");
    puts("                          ::::..~..:..:~7~.: :^ ~^:~..:....:^~^:^~^");
    puts("                        ..      .. .^!777^~~!?~^~7:^::.  .5BGB##&#7.");
    puts("                             .:~!!. .:~^.77?P57P7!^::~::?5B&&#GGG5.");
    puts("                          .:^~~^^: YJJGBGP5YY7J7!^^^?7YG##P7^7#&B~:");
    puts("                        .^~~:  ~J55PJ~:.       ..~JPBBBY^. .:.JG!:!!^..");
    puts("                     ..^7~  .::???.         .~!JG&&#J:       .7!:.^77?!^..");
    puts("                   .:~7!:. ^Y^.  ..      .!JPB&&#Y:          .  :7!: ~G5?!:.");
    puts("                 .:~7?7..~!!       .:..^?YP&&&G7         .        ~J: .BP7J!:.");
    puts("               .:^777~^::7^          :JB&&#BG^ .                   .J! !YYYJ?^.");
    puts("              .:!JJJJ: :7G.         ?#&&&&B~                         ?5..^!~~!!:.");
    puts("             .:!YYY5~ .J&Y      . .5BBBBBG^                           YP  :?J!~!^.");
    puts("             .:!J?YJ^..P&!        P&&&&&#!             ..              #7 .^55?^~:");
    puts("              .^7!??^..P#? .  .   ~#&&&&&J:..   ...  .   ..  ...     . 5#..:J57~!:.");
    puts("              .:!J~~^.?G#5      .:.^JGBBBP5Y!^75GBGP5PJYPGPP5YYJ?!^    !&:.:??~?~:.");
    puts("               .^?5^:.JPBP!       .:::.. .!YYY&&&###&&&&YG##&&&&#B5:   !&:.^~!5J~.");
    puts("                .^??:.^^?77..                 ^:.  ..P&G:^&&&&&&&&7    PG ~7:JP7:.");
    puts("                .:^7? .~~J?7!                 .      7#^ P&&&&&&G^    :&^ 7?5?!:.");
    puts("                  .^J5.:!JPBBY            .:.       ^5^ 7###&#P^      G7..7G5!:.");
    puts("                   .^?5~^~:~PBP~.?!~: ^^7~^!^     .7: ~#&&#5~        ~~:.:PY~:.");
    puts("                    .^!5J7..~!B&PJY?J^J?YGY?7!:..^^:?#@&G7.        .~.::^Y!^.");
    puts("                     .:^??J!77JY57 .: :?#BYGG!7~:7G&#5~.           .~^!J5!:.");
    puts("                       .:~?JY~..... :.^!J^^!7~7PGPJ~.          ... ^?Y57:.");
    puts("                          :~75P~:.  ^^!!^~!JYJ57:..^^....:^!:  ...5GP7^.");
    puts("                          ::.:~^7.JY^?77Y5Y7^..::~??5J???5J: :?Y!J5!:.");
    puts("                           .  .^.7G&###B5!^:.:.::......~?J?YB#BY7^:.");
    puts("                              .^^YJ5###BG5J7!7JYPPGYJ??YPPPY?7~:.");
    puts("                              ..:^~^^........^~!~!!!!~^^::...");
    puts("\n\n");
    puts("                        o---------------------------------------------o");
    puts("                        |      Breaking and Overcoming Challenges     |");
    puts("                        |    Through Courage Hardwork and Persistence |");
    puts("                        |           ~~ Bluejackets 23-1 ~~            |");
    puts("                        o---------------------------------------------o");
    getchar();
}

int main() {
    readFile();

    bool done = false;
    while (!done) {
        mainMenu();
        writeFile();

        switch (mainOpt) {
            case 1:
                tempScore = 0;
                inputGrid();
                initializeGrid(opt1);
                game(opt1);

                system("cls");
                inputPlayer();
                break;

            case 2:
                display();
                break;

            case 3:
                exitLogo();
                done = true;
                break;

            default:
                break;
        }
    }


    return 0;
}
