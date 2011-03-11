#include "static_floor.h"
#include <algorithm>
#include <cstdlib>

#ifdef DEBUG
#include <iostream>
#endif

StaticFloor::StaticFloor(unsigned int w, unsigned int h, unsigned int apeoplecount, int shape) : Floor(w,h,apeoplecount) {
	strengthField =  (double **)std::malloc(height * sizeof(double *));
	for (int i=0; i<height; ++i) {
		strengthField[i] = (double *)std::malloc(width * sizeof(double));
	}

	// inicializace bazove tridy poli
	init(shape);

	// nejprve vytvorime seznam dveri
	for (int i=0; i < height; ++i) {
		for (int j=0; j < width; ++j) {
			if (lattice[i][j].door) {
				// mame dvere, pridame je do seznamu
				Coord door = {i,j};
				doors.push_back(door);
			}
		}
	}

	for (int i=0; i < height; ++i) {
		for (int j=0; j < width; ++j) {
			Coord coord = {i,j};
			strengthField[i][j] = get_strength(coord);
		}
	}
}

StaticFloor::~StaticFloor() {
	for (int i=0; i < height; ++i) {
		free(strengthField[i]);
	}
	free(strengthField);
}

double StaticFloor::max_dist_to_exit(const Coord& T, const Coord& l) const {
	double max_coord = 0.0;	// maximalni
	for (unsigned int x = 0; x < height; ++x) {
		for (unsigned int y = 0; y < width; ++y) {
			double tmp_coord = std::sqrt((T.x-x)*(T.x-x) + (T.y-y)*(T.y-y));
			if (tmp_coord > max_coord) max_coord = tmp_coord;
		}
	}

	double tmp_coord = std::sqrt((T.x-l.x)*(T.x-l.x) + (T.y-l.y)*(T.y-l.y));
	return max_coord - tmp_coord;
}

/**
 * Vypocte silu v dane bunce vuci dostupnym vychodum
 */
double StaticFloor::get_strength(const Coord& c) const {
	// projdeme vsechny souradnice vychodu a pro kazdou z nich vypocteme silu
	// hledame nejmensi vzdalenost od souradnice c

	// vektor vzdalenosti souradnice c od vsech moznych dveri
	std::vector<double> min_distance;
	for (std::vector<Coord>::const_iterator it = doors.begin(); it != doors.end(); ++it) {
		min_distance.push_back(max_dist_to_exit(*it, c));
	}
	return *std::max_element(min_distance.begin(), min_distance.end());

	/* Podle clanku by byla treba vracet min_element. Podle me je to vsak spatne, navratova hodnota ovlivnuje
	 * silu statickeho pole v danem miste. Je treba vybrat tu nejvetsi, ktera nejvic ovlivnuje posun chodcu
	 * ke dverim.
	 * Vysledne grafy navic odpovidaji situaci, kdy je proveden vypocet pres max_element
	 */
}

/**
 * Vrati silu v dane bunce
 */
double StaticFloor::value(const int i, const int j) const {
	return strengthField[i][j]; 
}

/**
 * Presune chodce na novou bunku
 * 
 * Vlozi do mapy novych cilovych souradnic ty souradnice, ze kterych se na tuto cilovou souradnici pohybuji chodci
 * Muze se stat, ze se na jednu souradnici bude chtit presunout vice chodcu, proto je treba nahodne vybrat, ktery
 * z nich se na tuto novou souradnici muze presunout
 *
 * (i1, j1) - zdrojova souradnice
 * (i2, j2) - cilova souradnice
 */
void StaticFloor::move_pedestrian(const int i1, const int j1, const int i2, const int j2) {
	struct Coord src = {i1, j1}, dst = {i2, j2};

	// do mapy s klicem cilove souradnice vkladame zdrojovou souradnici
	// zdrojovych souradnic muze byt vice, proto pouzivam vektor
	if (new_coords.count(dst) == 0)	{
		// musime nejprve vektor vytvorit
		std::vector<struct Coord> vec;
		vec.push_back(src);
		new_coords[dst] = vec;
	}
	else new_coords[dst].push_back(src);

}

/**
 * Odstrani chodce, kteri jsou na dverich
 */
void StaticFloor::remove_pedestrians_near_doors() {
	for (std::vector<Coord>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if (lattice[door->x][door->y].occupation) {
			remove_pedestrian(door->x, door->y);
		}
	}
}

/**
 * Odstrani chodce z dane souradnice
 */
void StaticFloor::remove_pedestrian(const int i, const int j) {
	lattice[i][j].occupation = false;
	decrease_people();
}

/**
 * Synchronizace se skutecnym stavem v mistnosti
 *
 * Projdeme mapu souradnic new_coords a pro kazdou cilovou souradnici vybereme nahodne prave jednu zdrojovou souradnici,
 * z ktere se chodec presune. Ostatnic chodci pro tuto vybranou cilovou souradnici zustavaji na miste.
 *
 * \return Vektor souradnic, z kterych doslo skutecne k presunu
 */

std::vector<struct Coord> StaticFloor::sync() {
	std::vector<struct Coord> left_cells;	// bunky, ktere byly opusteny
	// projdeme celou mapu
	std::map<struct Coord, std::vector<struct Coord>, compare_coords >::const_iterator it = new_coords.begin();
	for (; it != new_coords.end(); ++it) {
		std::vector<struct Coord> src = it->second;

		// obsadime cilovou souradnici
		struct Coord c = it->first;	// cilova souradnice
		lattice[c.x][c.y].occupation = true;	// nova cilova souradnice je nyni obsazena

		// a uvolnime nahodne zvolenou zdrojovou souradnici
		// src.size() nemuze byt 0, protoze byl urcite vlozen s 1 prvkem
		int i=0;
		if (src.size() > 1) i = (std::rand() % src.size());	// nahodne vybrana souradnice dana velikosti vektoru
		c = src[i];
		lattice[c.x][c.y].occupation = false;
		left_cells.push_back(c);
	}

	// po synchronizaci vycistime mapu
	new_coords.clear();

	return left_cells;
}

void StaticFloor::printout() const {
#ifdef DEBUG
	for (int i=0; i < height; ++i) {
		for (int j=0; j < width; ++j) {
			std::cout << strengthField[i][j] << " ";
		}
		std::cout << std::endl;
	}
#endif
}
