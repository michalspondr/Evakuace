#include "automaton.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

#include <curses.h>

char *application_name = NULL;
// inicializace statickych hodnot
unsigned int Floor::height = FIELD_HEIGHT;
unsigned int Floor::width = FIELD_WIDTH;

void usage() {
	std::cout << "Usage: " << application_name << std::endl;
	std::cout << "Parameters:" << std::endl
		<< "\t-w <N> -- field width, by default: " << FIELD_WIDTH << std::endl
		<< "\t-h <N> -- field height, by default: " << FIELD_HEIGHT << std::endl
		<< "\t-p <N> -- people count, by default: " << PEOPLE_COUNT << std::endl
		<< "\t-d <F> -- diffusion, by default: " << DIFFUSE << std::endl
		<< "\t-e <F> -- decay, by default: " << DECAY << std::endl
		<< "\t-s <F> -- static floor constant, by default: " << KS << std::endl
		<< "\t-k <F> -- dynamic floor constant, by default: " << KD << std::endl
		<< "\t-r <N> -- random seed (if not set, time will be used)" << std::endl
		<< "\t-a <N> -- shape of the room, should be between 0 and 4, by default: 0" << std::endl
		<< "\t-t <N> -- time in microseconds, update between each step, by default: 10000" << std::endl
		<< "\t-u     -- you can do simulation steps by pressing any key" << std::endl
		<< std::endl
		<< "<N> = natural number\t<F> = real number" << std::endl;
	std::exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	application_name = argv[0];	// zaloha nazvu programu pro ostatni funkce

	// inicializujeme generator nahodnych cisel (bude-li zadan, mozna se zmeni
	std::srand(std::time((time_t *)NULL));   // inicializace generatoru nahodnych cisel

	unsigned int width=FIELD_WIDTH, height=FIELD_HEIGHT, people_count=PEOPLE_COUNT;
	int shape=0;
	double diffuse=DIFFUSE, decay=DECAY;
	float ks=KS, kd=KD;
	bool step=false;
	useconds_t utime=10000;

	// zpracujeme vstupni parametry
	int opt;
	while ((opt = getopt(argc, argv, "w:h:p:d:e:s:k:r:a:t:u")) != -1) {
		switch (opt) {
			case 'w':
				width = (unsigned int)atol(optarg);
				break;
			case 'h':
				height = (unsigned int)atol(optarg);
				break;
			case 'p':
				people_count = (unsigned int)atol(optarg);
				break;
			case 'd':
				diffuse = atof(optarg);
				if (diffuse > 1.0) usage();
				break;
			case 'e':
				decay = atof(optarg);
				if (decay > 1.0) usage();
				break;
			case 's':
				ks = (float)atof(optarg);
				break;
			case 'k':
				kd = (float)atof(optarg);
				break;
			case 'r':
				std::srand((unsigned int)atoi(optarg));
				break;
			case 'a':
				shape = atoi(optarg);
				if (shape > 4 || shape < 0) usage();
				break;
			case 't':
				utime = (useconds_t)atol(optarg);
				break;
			case 'u':
				step = true;
				break;
			default:
				usage();
				break;
		}
	}

	// inicializace ncurses
	initscr();

	// vytvorime automat
	Automaton automaton(width, height, people_count, shape, diffuse, decay, ks, kd);
	
	while (automaton.with_pedestrians()) {
		automaton.printout();
		if (step) getch();	// cekani na stisk klavesy
		automaton.update();
		usleep(utime);
	}
	automaton.printout();

	printw("\nPress any key to quit");
	getch();	// cekame na stisk klavesy
	endwin();

	exit(EXIT_SUCCESS);
}
