#include "automaton.h"
#include <cmath>
#include <cstdlib>

#ifdef DEBUG
#include <iostream>
#endif

Automaton::Automaton(unsigned int width, unsigned int height, unsigned int people_count, int shape, double diffuse, double decay, float ks, float kd)
	: kS(ks), kD(kd), time(0)
{
	// alokace pameti pole
	probability_grid = (double **)std::malloc(height * sizeof(double *));
	aheight=height;
	for (int i=0; i<height; ++i) {
		probability_grid[i] = (double *)std::malloc(width * sizeof(double));
	}

	staticFloor = new StaticFloor(width, height, people_count, shape);
	dynamicFloor = new DynamicFloor(width, height, diffuse, decay, (Floor *)staticFloor);

	// inicializace generatoru nahodnych cisel
//	std::srand(124);

}

Automaton::~Automaton() {
	delete dynamicFloor;
	delete staticFloor;

	for (int i=0; i < aheight; ++i) {
		free(probability_grid[i]);
	}

	free(probability_grid);
}

/**
 * Aktualizace celularniho automatu
 */
void Automaton::update() {
	// Modifikace dynamickeho pole dle rozptylu a slabnuti
	dynamicFloor->update();
	
	// Urceni pravdepodobnosti presunu chodce na vedlejsi prazdnou bunku
	// Pravdepodobnost je urcena obemi poli (statickym a dynamickym) a mistni dynamikou
	// Hodnoty poli D a S jsou vazeny konstantami citlivosti kS a kD
	//
	// Vypoctu pro kazdeho chodce

	const unsigned int height = staticFloor->get_height(), width = staticFloor->get_width();

	for (int i=0; i < height; ++i) {
		for (int j=0; j < width; ++j) {
			// resime pouze chodce
			if (staticFloor->isOccupied(i,j)) {
				transition_probability(i,j);
			}
		}
	}

	// synchronizace novych pozic s hodnotami na mrizce
	sync();

	// nyni zkontrolujeme, zda se nektery chodec nenachazi u dveri, pokud ano, pak odejde
	remove_pedestrians_near_doors();
	
	// posuneme cas
	++time;
}

/**
 * Vypocet pravdepodobnosti presunu z bunky (i,j)
 */
void Automaton::transition_probability(const int i, const int j) {
	// vypocteme hodnoty z okolnich bunek, na ktere lze prejit
	double norm=0.0, left=0.0, right=0.0, up=0.0, down=0.0;
	if (can_move(i-1, j)) {
		int anoccupation = staticFloor->isOccupied(i-1,j) ? 1 : 0;	// 1 = obsazeno, 0 = volne
		int anobstacle = staticFloor->isForbidden(i-1,j) ? 0 : 1;	// 0 = prekazka, 1 = jinak
		norm = up = std::exp(kD*dynamicFloor->value(i-1,j)) * std::exp(kS*staticFloor->value(i-1,j)) * (1-anoccupation) * anobstacle;
	}
	if (can_move(i+1, j)) {
		int anoccupation = staticFloor->isOccupied(i+1,j) ? 1 : 0;	// 1 = obsazeno, 0 = volne
		int anobstacle = staticFloor->isForbidden(i+1,j) ? 0 : 1;	// 0 = prekazka, 1 = jinak
		down = std::exp(kD*dynamicFloor->value(i+1,j)) * std::exp(kS*staticFloor->value(i+1,j)) * (1-anoccupation) * anobstacle;
		norm += down;
	}
	if (can_move(i, j-1)) {
		int anoccupation = staticFloor->isOccupied(i,j-1) ? 1 : 0;	// 1 = obsazeno, 0 = volne
		int anobstacle = staticFloor->isForbidden(i,j-1) ? 0 : 1;	// 0 = prekazka, 1 = jinak
		left = std::exp(kD*dynamicFloor->value(i,j-1)) * std::exp(kS*staticFloor->value(i,j-1)) * (1-anoccupation) * anobstacle;
		norm += left;
	}
	if (can_move(i, j+1)) {
		int anoccupation = staticFloor->isOccupied(i,j+1) ? 1 : 0;	// 1 = obsazeno, 0 = volne
		int anobstacle = staticFloor->isForbidden(i,j+1) ? 0 : 1;	// 0 = prekazka, 1 = jinak
		right = std::exp(kD*dynamicFloor->value(i,j+1)) * std::exp(kS*staticFloor->value(i,j+1)) * (1-anoccupation) * anobstacle;
		norm += right;
	}

	// normalizace
	up = up/norm;
	down = down/norm;
	left = left/norm;
	right = right/norm;

	// zjistim pravdepodobnosti prechodu pro pole vlevo, vpravo, nahore a dole od soucasne bunky
	// vyuziji vypoctu podle nasledujici tabulky
	//    
	//    l       r     u     d
	// -------------------------------------
	// | Left | Right | Up | Down | stojim |
	// -------------------------------------
	// pr.   0.3     0.5  0.6    0.8      1.0
	//
	const unsigned int height = staticFloor->get_height(), width = staticFloor->get_width();

	double l = (j==0 || (j==1 && !staticFloor->isDoor(i,j-1)))? 0 : left;
	double r = l + ((j==width-1 || (j==width-2 && !staticFloor->isDoor(i,j+1))) ? 0 : right);
	double u = r + ((i==0 || (i==1 && !staticFloor->isDoor(i-1,j))) ? 0 : up);
	double d = u + ((i==height-1 || (i==height-2 && !staticFloor->isDoor(i+1,j))) ? 0 : down);
	// zbytek do 1.0 je pravdepodobnost pro zustani na miste


	// vybereme nahodne smer, kterym se budeme pohybovat a presuneme chodce
	const double rnd = (double)(std::rand()%100)/100.0; // vyber nahodneho cisla
	if (rnd < 1.0 && rnd > d) {
		// zustava na miste nebo nahodny pohyb
		const short tmp = std::rand()%5;
		switch (tmp) {
			case 0: // zustava
				break;
			case 1: // vlevo
				if (can_move(i,j-1)) {
					move_pedestrian(i,j, i,j-1);
				}
				break;
			case 2: // vpravo
				if (can_move(i,j+1)) {
					move_pedestrian(i,j, i,j+1);
				}
				break;
			case 3: // nahoru
				if (can_move(i-1,j)) {
					move_pedestrian(i,j, i-1,j);
				}
				break;
			case 4: // dolu
				if (can_move(i+1,j)) {
					move_pedestrian(i,j, i+1,j);
				}
				break;
			default:
				break;
		}
		return;
	}

	if (rnd <= d && rnd > u) {
		// down
		if (can_move(i+1,j)) {
			move_pedestrian(i,j, i+1,j);
		}
		return;
	}
	if (rnd <= u && rnd > r) {
		// up
		if (can_move(i-1,j)) {
			move_pedestrian(i,j, i-1,j);
		}
		return;
	}
	if (rnd <= r && rnd > l) {
		// right
		if (can_move(i,j+1)) {
			move_pedestrian(i,j, i,j+1);
		}
		return;
	}
	if (rnd <= l) {
		// left
		if (can_move(i,j-1)) {
			move_pedestrian(i,j, i,j-1);
		}
		return;
	}


}

/**
 * Presun chodce na novou souradnici
 *
 * (i1, j1) - zdrojova souradnice
 * (i2, j2) - cilova souradnice
 */
void Automaton::move_pedestrian(const int i1, const int j1, const int i2, const int j2) {
	staticFloor->move_pedestrian(i1,j1, i2,j2);
}

/**
 * Da se presunou na danou bunku?
 * Bunka nesmi byt obsazena jinym chodcem ani nesmi obsahovat prekazku
 */
bool Automaton::can_move(const int i, const int j) const {
	const unsigned int height = staticFloor->get_height();
	const unsigned int width = staticFloor->get_width();

	// osetreni hranic
	if (i<0 || j<0 || i>=height || j>=width) return false;

	return !(staticFloor->isOccupied(i,j) || staticFloor->isForbidden(i,j));
}

/**
 * Rekne, zda jsou v mistnosti jeste nejake osoby
 * \return true, pokud se v mistnosti jeste nekdo nachazi
 */
bool Automaton::with_pedestrians() const {
	return (staticFloor->people() > 0);
}

/**
 * Odstrani z mistnosti chodce, kteri jsou u dveri
 */
void Automaton::remove_pedestrians_near_doors() {
	// projdu seznam dveri a overi, zda se v jejich sousedstvi nevyskytuje chodec
	staticFloor->remove_pedestrians_near_doors();	
}


/**
 * Synchronizuje staticke pole
 */
void Automaton::sync() {
	std::vector<struct Coord> left_cells = staticFloor->sync();

	// pro kazdou souradnici, u ktere doslo k presunu, zvysime hodnotu dynamicke vrstvy o 1
	for (std::vector<struct Coord>::const_iterator it = left_cells.begin(); it != left_cells.end(); ++it) {
		dynamicFloor->increase(it->x, it->y);

	}
}

void Automaton::printout() const {
	staticFloor->Floor::printout(get_time());
}
