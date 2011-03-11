#include "floor.h"
#include <cstdlib>
#include <curses.h>

#ifdef DEBUG
#include <iostream>
#endif

Floor::Floor(unsigned int w, unsigned int h, unsigned int apeoplecount) : people_count(apeoplecount) {
	width = w;
	height = h;

	lattice = (struct Cell **)std::malloc(height * sizeof(struct Cell *));
	for (int i=0; i<height; ++i) {
		lattice[i] = (struct Cell *)std::malloc(width * sizeof(double));
	}
}

Floor::~Floor() {
	for (int i=0; i < height; ++i) {
		free(lattice[i]);
	}

	free(lattice);
}


/**
 * Vytvori mistnost. To znamena - vytvori steny (= nedostupna policka) a obsadi ji lidmi
 */
void Floor::init(int shape) throw(std::domain_error) {
	// pri vytvareni musi byt splnena podminka, ze nelze vlozit vice osob, nez je obsah
	// mistnosti dany parametry width a height s odectenymi bunkami,
	// ktere jsou zakazane (zdi apod.)
	unsigned int free_cells = width * height;

	for (int i=0; i < height; ++i) {
		for (int j=0; j < width; ++j) {
			if (i == 0 || j == 0 || i+1 == height || j+1 == width) {
				lattice[i][j].forbidden = true;
				--free_cells;	// bunka obsazena
			}
			else
				lattice[i][j].forbidden = false;

			lattice[i][j].occupation = false;	// zatim nikde nikdo neni
			lattice[i][j].door = false;		// dvere take nikde nejsou
		}
	}

	// vybereme tvar mistnosti
	switch (shape) {
		case 0 :
			// jednoduche dvere v horni casti mistnosti presne uprostred
			set_doors(0, width/2);
			break;
		case 1 :
			// dvoje dvere v horni casti mistnosti
			set_doors(0, width/4); set_doors(0, width/4 * 3);
			break;
		case 2 :
			// ctyri dvere v horni a dolni casti mistnosti
			set_doors(0, width/4); set_doors(0, width/4 * 3);
			set_doors(height-1, width/4); set_doors(height-1, width/4 * 3);
			break;
		case 3 :
			// 2 dvere v levem hornim rohu mistnosti
			if (width <= 1 || height <= 1) throw std::domain_error("Mistnost je pro volbu 3 prilis mala.\n");
			set_doors(0,1);
			set_doors(1,0);
			break;
		case 4 :
			// 9 dveri v dolni casti mistnosti a jedny dvere v prave casti mistnosti
			if (width <= 10) throw std::domain_error("Mistnost je pro volbu 3 prilis mala.\n");
			for (int n=1; n < 10; ++n) {
				set_doors(0, width/10 * n);
			}
			set_doors(height/2, width-1);
			break;
		default:
			break;
	}

	// nyni overime, zda lze volne misto vyplnit danym poctem lidi
	if (people_count > free_cells) {
		throw std::domain_error("Lidi je vice nez volnych bunek.\n");
	}

	// nahodne obsadime mistnost lidmi
	for (int n=0; n < people_count; ++n) {
		generate_position:
		int i = std::rand()%height;
		int j = std::rand()%width;
		// pokud nelze na danem miste vlozit cloveka, zkousime znovu
		// tento algoritmus neni moc chytry, ale pro mistnosti v radech stovek bunek
		// funguje dostatecne rychle
		if (lattice[i][j].forbidden || lattice[i][j].occupation || lattice[i][j].door) goto generate_position;
		
		lattice[i][j].occupation = true;
	}

}

/**
 * Nastavi na dane souradnici dvere
 */
void Floor::set_doors(const int x, const int y) {
	if (x<0 || y<0 || x>=height || y>=width) throw std::domain_error("Pokus o vlozeni dveri mimo hranice pole.\n");

	lattice[x][y].door = true;
	lattice[x][y].occupation = false;
	lattice[x][y].forbidden = false;

}

/**
 * Snizi pocet lidi v mistnosti
 */
void Floor::decrease_people() {
	--people_count;
}

void Floor::printout(size_t current_time) const {
	clear();
	for (int i=0; i < height; ++i) {
		for (int j=0; j < width; ++j) {
			if (lattice[i][j].forbidden)
				printw("X");
			else if (lattice[i][j].door)
				printw("-");
			else if (lattice[i][j].occupation)
				printw("@");
			else
				printw(" ");
		}
		printw("\n");
	}
	printw("\nPedestrians in room: %d\tTime: %d", people(), current_time); 
	refresh();
}
