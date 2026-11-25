#include <stdio.h>
#include <string.h>

#define MAX_CANDIDATES 10
#define MAX_VOTERS     100
#define ADMIN_PASSWORD 1234

/* ---------- Structures ---------- */
struct Candidate {
    int  id;
    char name[50];
    char party[50];
    int  votes;
};

struct Voter {
    int  id;
    char name[50];
    int  age;
    int  hasVoted;      /* 0 = not voted, 1 = voted */
};

/* ---------- Global arrays of structures ---------- */
struct Candidate candidates[MAX_CANDIDATES];
struct Voter     voters[MAX_VOTERS];
int candidateCount = 0;
int voterCount     = 0;

/* ---------- Function prototypes ---------- */
void addCandidate(void);
void registerVoter(void);
void castVote(void);
void showCandidates(void);
void showResults(void);
void declareWinner(void);
int  findVoter(int id);
int  adminLogin(void);

/* ---------- Main ---------- */
int main(void) {
    int choice;

    do {
        printf("\n==============================\n");
        printf("   DIGITAL VOTING SYSTEM\n");
        printf("==============================\n");
        printf("1. Add Candidate   (Admin)\n");
        printf("2. Register Voter\n");
        printf("3. Cast Vote\n");
        printf("4. Show Candidates\n");
        printf("5. Show Results    (Admin)\n");
        printf("6. Declare Winner  (Admin)\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {      /* handle non-number input */
            printf("Invalid input!\n");
            while (getchar() != '\n');        /* clear input buffer */
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:
                if (adminLogin()) addCandidate();
                break;
            case 2:
                registerVoter();
                break;
            case 3:
                castVote();
                break;
            case 4:
                showCandidates();
                break;
            case 5:
                if (adminLogin()) showResults();
                break;
            case 6:
                if (adminLogin()) declareWinner();
                break;
            case 0:
                printf("Thank you for using the system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }
    } while (choice != 0);

    return 0;
}

/* ---------- Admin password check ---------- */
int adminLogin(void) {
    int pass;
    printf("Enter admin password: ");
    scanf("%d", &pass);

    if (pass == ADMIN_PASSWORD) {
        return 1;
    }
    printf("Wrong password! Access denied.\n");
    return 0;
}

/* ---------- Add a candidate ---------- */
void addCandidate(void) {
    if (candidateCount >= MAX_CANDIDATES) {
        printf("Candidate limit reached!\n");
        return;
    }

    struct Candidate *c = &candidates[candidateCount];
    c->id    = candidateCount + 1;
    c->votes = 0;

    printf("Enter candidate name: ");
    scanf(" %49[^\n]", c->name);
    printf("Enter party name: ");
    scanf(" %49[^\n]", c->party);

    candidateCount++;
    printf("Candidate added successfully! (ID: %d)\n", c->id);
}

/* ---------- Register a voter ---------- */
void registerVoter(void) {
    int id, age;

    if (voterCount >= MAX_VOTERS) {
        printf("Voter limit reached!\n");
        return;
    }

    printf("Enter voter ID (number): ");
    scanf("%d", &id);

    if (findVoter(id) != -1) {
        printf("This voter ID is already registered!\n");
        return;
    }

    printf("Enter age: ");
    scanf("%d", &age);

    if (age < 18) {
        printf("Sorry, you must be 18 or older to vote.\n");
        return;
    }

    voters[voterCount].id       = id;
    voters[voterCount].age      = age;
    voters[voterCount].hasVoted = 0;

    printf("Enter name: ");
    scanf(" %49[^\n]", voters[voterCount].name);

    voterCount++;
    printf("Registration successful!\n");
}

/* ---------- Cast a vote ---------- */
void castVote(void) {
    int id, pos, choice;

    if (candidateCount == 0) {
        printf("No candidates available yet.\n");
        return;
    }

    printf("Enter your voter ID: ");
    scanf("%d", &id);

    pos = findVoter(id);
    if (pos == -1) {
        printf("Voter not registered!\n");
        return;
    }
    if (voters[pos].hasVoted == 1) {
        printf("You have already voted! Double voting is not allowed.\n");
        return;
    }

    showCandidates();
    printf("Enter candidate ID to vote for: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > candidateCount) {
        printf("Invalid candidate! Vote not counted.\n");
        return;
    }

    candidates[choice - 1].votes++;
    voters[pos].hasVoted = 1;
    printf("Thank you, %s! Your vote has been recorded.\n", voters[pos].name);
}

/* ---------- Display candidates ---------- */
void showCandidates(void) {
    int i;

    if (candidateCount == 0) {
        printf("No candidates added yet.\n");
        return;
    }

    printf("\n%-5s %-25s %-20s\n", "ID", "Name", "Party");
    printf("---------------------------------------------------\n");
    for (i = 0; i < candidateCount; i++) {
        printf("%-5d %-25s %-20s\n",
               candidates[i].id, candidates[i].name, candidates[i].party);
    }
}

/* ---------- Display results ---------- */
void showResults(void) {
    int i, totalVotes = 0;

    if (candidateCount == 0) {
        printf("No candidates available.\n");
        return;
    }

    for (i = 0; i < candidateCount; i++)
        totalVotes += candidates[i].votes;

    printf("\n%-5s %-25s %-8s %-10s\n", "ID", "Name", "Votes", "Percent");
    printf("-------------------------------------------------------\n");
    for (i = 0; i < candidateCount; i++) {
        float percent = (totalVotes > 0)
                        ? (candidates[i].votes * 100.0f / totalVotes) : 0;
        printf("%-5d %-25s %-8d %.2f%%\n",
               candidates[i].id, candidates[i].name,
               candidates[i].votes, percent);
    }
    printf("Total votes cast: %d\n", totalVotes);
}

/* ---------- Declare winner (handles ties) ---------- */
void declareWinner(void) {
    int i, max = 0, winners = 0;

    if (candidateCount == 0) {
        printf("No candidates available.\n");
        return;
    }

    for (i = 0; i < candidateCount; i++)
        if (candidates[i].votes > max)
            max = candidates[i].votes;

    if (max == 0) {
        printf("No votes have been cast yet.\n");
        return;
    }

    for (i = 0; i < candidateCount; i++)
        if (candidates[i].votes == max)
            winners++;

    if (winners > 1) {
        printf("\nIt's a TIE between:\n");
        for (i = 0; i < candidateCount; i++)
            if (candidates[i].votes == max)
                printf(" - %s (%s) with %d votes\n",
                       candidates[i].name, candidates[i].party, max);
    } else {
        for (i = 0; i < candidateCount; i++)
            if (candidates[i].votes == max)
                printf("\nWINNER: %s (%s) with %d votes!\n",
                       candidates[i].name, candidates[i].party, max);
    }
}

/* ---------- Find voter by ID: returns index or -1 ---------- */
int findVoter(int id) {
    int i;
    for (i = 0; i < voterCount; i++) {
        if (voters[i].id == id)
            return i;
    }
    return -1;
}