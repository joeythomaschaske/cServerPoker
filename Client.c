#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include<stdbool.h>
typedef struct card {
	char suit;
	int val;
} card;
void game(card** deck);
bool fourOfAKind(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet);
bool straightFlush(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet);
bool threeOfAKind(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet);
bool straight(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet);
bool twoPair(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet);
bool flush(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet);
bool pair(card* ccard1, card* ccard2, card* ccard3, card* ccard4, int *bankRoll,
		int *bet);
int main(int args, char* argv[]) {
	unlink("Client");
	int s0, s1;
	struct sockaddr name0 = { AF_UNIX, "Client" };
	s0 = socket(AF_UNIX, SOCK_STREAM, 0);
	if (s0 == -1)
		return -1;
	socklen_t clientlen = sizeof(struct sockaddr) + 7;
	bind(s0, &name0, clientlen);

	struct sockaddr name1 = { AF_UNIX, "Server" };
	s1 = socket(AF_UNIX, SOCK_STREAM, 0);
	if (s1 == -1)
		return -1;
	socklen_t serverlen = sizeof(struct sockaddr) + 7;
	bind(s1, &name1, clientlen);
	if (connect(s0, &name1, serverlen) == -1) {
		printf("connect() fail\n");
		return -1;
	}
	card** deck = malloc(sizeof(card) * 52);
	int i;
	for (i = 0; i < 52; ++i) {
		card* ccard = malloc(sizeof(card));
		read(s0, ccard, sizeof(card));
		deck[i] = ccard;
	}
	close(s0);
	game(deck);
	return 0;
}
void game(card** deck) {
	int rounds = 0;
	int wins = 0;
	int losses = 0;
	int bankRoll = 100;
	int numCards = 52;
	int bet = 0;
	int index = 0;
	bool play = true;
	while (bankRoll > 0 && play) {
		printf("\nBalance: %d", bankRoll);
		printf("\nPlease place a bet or enter -1 to quit, Ace are low: ");
		scanf("%d", &bet);
		if (bet == -1) {
			printf("\nThanks for playing!");
			printf("\nRounds Played: %d Wins: %d Losses: %d", rounds, wins, losses);
			int k;
			for(k=0; k<52; ++k){
				free(deck[k]);
			}
			free(deck);
			unlink("ClientDeck");
			unlink("Client");
			exit(0);
		}
		else if(bet >= bankRoll){
			printf("\nAll In!!!");
			bet = bankRoll;
		}
		rounds++;
		if (numCards == 0) {
			unlink("ClientDeck");
			int s0, s1;
			struct sockaddr name0 = { AF_UNIX, "ClientDeck" };
				s0 = socket(AF_UNIX, SOCK_STREAM, 0);
				if (s0 == -1)
					exit(1);
				socklen_t clientlen = sizeof(struct sockaddr) + 9;
				bind(s0, &name0, clientlen);
				struct sockaddr name1 = { AF_UNIX, "Server" };
				s1 = socket(AF_UNIX, SOCK_STREAM, 0);
				if (s1 == -1)
					exit(1);
				socklen_t serverlen = sizeof(struct sockaddr) + 7;
				bind(s1, &name1, clientlen);
				if (connect(s0, &name1, serverlen) == -1) {
					printf("connect() fail\n");
					exit(1);
				}
				int i;
				for(i=0; i<52; ++i){
					free(deck[i]);
				}
				free(deck);
				printf("\nfreeing deck");
				card** deck2 = malloc(sizeof(card) * 52);
				for (i = 0; i < 52; ++i) {
					card* ccard = malloc(sizeof(card));
					read(s0, ccard, sizeof(card));
					deck2[i] = ccard;
				}
				deck = deck2;
				close(s0);
				index = 0;
				numCards = 52;
		}
		card* ccard1 = deck[index];
		card* ccard2 = deck[index + 1];
		card* ccard3 = deck[index + 2];
		card* ccard4 = deck[index + 3];
		card* hashTable[4];
		hashTable[0] = ccard1;
		hashTable[1] = ccard2;
		hashTable[2] = ccard3;
		hashTable[3] = ccard4;
		int i;
		for(i = 0; i<4; ++i){
			if(hashTable[i]->val == 1){
				printf("%c%s ",hashTable[i]->suit, "Ace");
			}
			else if(hashTable[i]->val == 11){
				printf("%c%s ",hashTable[i]->suit, "Jack");
			}
			else if(hashTable[i]->val == 12){
				printf("%c%s ",hashTable[i]->suit, "Queen");
			}
			else if(hashTable[i]->val == 13){
				printf("%c%s ",hashTable[i]->suit, "King");
			}
			else{
				printf("%c%d ",hashTable[i]->suit, hashTable[i]->val);
			}
		}
		index += 4;
		numCards -= 4;
		bool points = false;
		if (points == false) {
			points = fourOfAKind(ccard1, ccard2, ccard3, ccard4, &bankRoll,
					&bet);
		}
		if (points == false) {
			points = straightFlush(ccard1, ccard2, ccard3, ccard4, &bankRoll,
					&bet);
		}
		if (points == false) {
			points = threeOfAKind(ccard1, ccard2, ccard3, ccard4, &bankRoll,
					&bet);
		}
		if (points == false) {
			points = straight(ccard1, ccard2, ccard3, ccard4, &bankRoll,
					&bet);
		}
		if (points == false) {
			points = twoPair(ccard1, ccard2, ccard3, ccard4, &bankRoll,
					&bet);
		}
		if (points == false) {
			points = flush(ccard1, ccard2, ccard3, ccard4, &bankRoll,
					&bet);
		}
		if (points == false) {
			points = pair(ccard1, ccard2, ccard3, ccard4, &bankRoll,
					&bet);
		}
		if(points == false){
			bankRoll -= bet;
			losses++;
			printf("\nYou lost this hand");
		}else{
			wins++;
		}
		printf("\n\n\n");
	}
	if(bankRoll <= 0 ){
		printf("\nOut of funds!");
		printf("\nRounds Played: %d Wins: %d Losses: %d", rounds, wins, losses);
		int k;
		for(k=0; k<52; ++k){
			free(deck[k]);
		}
		free(deck);
		unlink("ClientDeck");
		unlink("Client");
		exit(0);
	}
}
bool fourOfAKind(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet) {
	if (ccard1->val == ccard2->val && ccard2->val == ccard3->val
			&& ccard3->val == ccard4->val) {
		*bankRoll += 20825 * *bet;
		printf("\n4 of a kind!");
		return true;
	}
	return false;
}
bool straightFlush(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
	int *bankRoll, int *bet) {
	int hashTable[13];
	int sequence = 0;
	bool bSequence = false;
	int j = 0;
	for (j = 0; j < 13; ++j) {
		hashTable[j] = 0;
	}
	hashTable[ccard1->val-1] += 1;
	hashTable[ccard2->val-1] += 1;
	hashTable[ccard3->val-1] += 1;
	hashTable[ccard4->val-1] += 1;
	for (j = 0; j < 13; ++j) {
		if (hashTable[j] == 0) {
			sequence = 0;
		} else if (hashTable[j] > 0) {
			sequence += 1;
		}
		if (sequence == 4) {
			bSequence = true;
			break;
		}
	}
	if (bSequence == true) {
		if ((ccard1->suit == ccard2->suit) && (ccard2->suit == ccard3->suit) && (ccard3->suit == ccard4->suit)) {
			*bankRoll += 6153 * *bet;
			printf("\nStraight flush!");
			return true;
		}
	}
	return false;
}
bool threeOfAKind(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet) {
	int hashTable[13];
	int j = 0;
	for (j = 0; j < 13; ++j) {
		hashTable[j] = 0;
	}
	hashTable[ccard1->val-1] += 1;
	hashTable[ccard2->val-1] += 1;
	hashTable[ccard3->val-1] += 1;
	hashTable[ccard4->val-1] += 1;
	for (j = 0; j < 13; ++j) {
		if (hashTable[j] == 3) {
			*bankRoll += 108 * *bet;
			printf("\n3 of a kind!!");
			return true;
		}
	}
	return false;
}
bool straight(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet) {
	int hashTable[13];
	int sequence = 0;
	int j = 0;
	for (j = 0; j < 13; ++j) {
		hashTable[j] = 0;
	}
	hashTable[ccard1->val-1] += 1;
	hashTable[ccard2->val-1] += 1;
	hashTable[ccard3->val-1] += 1;
	hashTable[ccard4->val-1] += 1;
	for (j = 0; j < 13; ++j) {
		if (hashTable[j] == 0) {
			sequence = 0;
		} else if (hashTable[j] > 0) {
			sequence += 1;
		}
		if (sequence == 4) {
			*bankRoll += 98 * *bet;
			printf("\nStraight!");
			return true;
		}
	}
	return false;
}
bool twoPair(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet) {
	int hashTable[13];
	int pairs = 0;
	int j = 0;
	for (j = 0; j < 13; ++j) {
		hashTable[j] = 0;
	}
	hashTable[ccard1->val-1] += 1;
	hashTable[ccard2->val-1] += 1;
	hashTable[ccard3->val-1] += 1;
	hashTable[ccard4->val-1] += 1;
	for (j = 0; j < 13; ++j) {
		if (hashTable[j] == 2) {
			++pairs;
		}
	}
	if (pairs == 2) {
		*bankRoll += 96 * *bet;
		printf("\n2 Pair!");
		return true;
	}
	return false;
}
bool flush(card* ccard1, card* ccard2, card* ccard3, card* ccard4,
		int *bankRoll, int *bet) {
	if (ccard1->suit == ccard2->suit && ccard2->suit == ccard3->suit
			&& ccard3->suit == ccard4->suit) {
		*bankRoll += 96 * *bet;
		printf("\nFlush!");
		return true;
	}
	return false;
}
bool pair(card* ccard1, card* ccard2, card* ccard3, card* ccard4, int *bankRoll,
		int *bet) {
	int hashTable[13];
	int pairs = 0;
	int j = 0;
	for (j = 0; j < 13; ++j) {
		hashTable[j] = 0;
	}
	hashTable[ccard1->val-1] += 1;
	hashTable[ccard2->val-1] += 1;
	hashTable[ccard3->val-1] += 1;
	hashTable[ccard4->val-1] += 1;
	for (j = 0; j < 13; ++j) {
		if (hashTable[j] == 2) {
			++pairs;
		}
	}
	if (pairs > 0) {
		*bankRoll += 3 * *bet;
		printf("\nPair!");
		return true;
	}
	return false;
}

