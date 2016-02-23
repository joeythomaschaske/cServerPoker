#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
typedef struct card {
	char suit;
	int val;
} card;
void makeDeck(card** deck);
void ctrlc();



int main(int args, char* argv[])
{

	unlink("Server");
	int s0, s1;
	card** deck = malloc(sizeof(card) * 52);
	makeDeck(deck);
	struct sigaction news, olds;
	void ctrlc() {
		unlink("Server");
		unlink("Client");
		int i;
		for(i=0; i<52; ++i){
			free(deck[i]);
		}
		free(deck);
		close(s1);
		sigaction(SIGINT, &olds, NULL);
		printf("Quit\n");
		exit(0);
	}
	news.sa_handler = ctrlc;
	news.sa_flags = 0;
	sigaction(SIGINT, &news, &olds);
	struct sockaddr name1 = { AF_UNIX, "Server" };
	socklen_t serverlen = sizeof(struct sockaddr) + 7;
	s1 = socket(AF_UNIX, SOCK_STREAM, 0);
	bind(s1, &name1, serverlen);
	while(1){
		if (listen(s1, 1) == -1) {
			printf("\nListen error");
			exit(1);
		}
		s0 = accept(s1, NULL, NULL);
		if (s0 == -1) {
			printf("\nAccept Error");
			exit(1);
		}
		else
			printf("\n Successfully Connected to client");
		printf("attempting to write\n");
		int i;
		for(i=0; i<52; ++i){
			write(s0, deck[i], sizeof(card));
		}
		close(s0);
	}
	return 0;
}
void makeDeck(card** deck) {
	int i, j;
	time_t t;
	for (i = 1; i < 53; ++i) {
		if (i <= 13) {
			card *newCard = malloc(sizeof(card));
			newCard->suit = 'S';
			newCard->val = i % 13 + 1;
			deck[i - 1] = newCard;
		} else if (i > 13 && i <= 26) {
			card *newCard = malloc(sizeof(card));
			newCard->suit = 'H';
			newCard->val = i % 13 + 1;
			deck[i - 1] = newCard;
		} else if (i > 26 && i <= 39) {
			card *newCard = malloc(sizeof(card));
			newCard->suit = 'C';
			newCard->val = i % 13 + 1;
			deck[i - 1] = newCard;
		} else if (i > 39) {
			card *newCard = malloc(sizeof(card));
			newCard->suit = 'D';
			newCard->val = i % 13 + 1;
			deck[i - 1] = newCard;
		}
	}
	card *temp;
	srand((unsigned) time(&t));
	for (i = 0; i < 52; i++) {
		j = rand() % 52;
		temp = deck[i];
		deck[i] = deck[j];
		deck[j] = temp;
	}
}
