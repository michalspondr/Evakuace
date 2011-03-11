#ifndef STATIC_FLOOR_H
#define STATIC_FLOOR_H
#include "floor.h"
#include <cmath>
#include <vector>
#include <map>

/**
 * Souradnice
 */
struct Coord {
	int x;		//!< X souradnice
	int y;		//!< Y souradnice
};

/**
 * Jednoducha metoda pro porovnani 2 souradnic, ktere je treba pri vytvoreni mapy new_coords
 */
struct compare_coords {
	bool operator() (const struct Coord& c1, const struct Coord& c2) const {
		if (c1.x < c2.x) return true;
		else if (c1.x == c2.x) return c1.y < c2.y;
		else return false;
	}
};

class StaticFloor : public Floor{
	double **strengthField;

	/**
	 * Mapa cilovych souradnic, ktere jsou svazany s vektory zdrojovych souradnic
	 * Chodci se pohybuji v jednom okamziku zaroven, proto je treba overit pro danou (cilovou) souradnici,
	 * z kolika ruznych zdrojovych souradnic se na ne bude kdo chtit presunout.
	 * Pro tyto zdrojove souradnice vytvorime vektor, cilova souradnice je jen jedna (= klic).
	 * Z teto mapy pak probehne nahodny vyber zdrojove souradnice, z ktere se skutecne na cilovou souradnici
	 * chodec presune. Ostatni chodci zustanou stat.
	 */
	std::map<struct Coord, std::vector<struct Coord>, compare_coords > new_coords;

	/**
	 * Nejkratsi vzdalenost bunky k nejblizsimu vychodu
	 * Vypocita se tzv. sila v dane bunce
	 */
	Coord min_dist_to_exit(std::vector<int>);

	/**
	 * Nejvetsi vzdalenost bunky od daneho vychodu
	 */
	double max_dist_to_exit(const Coord& T, const Coord& l) const;

	/**
	 * Zjisti silu stopy v dane souradnici
	 */
	double get_strength(const Coord& c) const;

	/**
	 * Seznam dveri
	 */
	std::vector<Coord> doors;

	/**
	 * Odstrani chodce na dane souradnici
	 */
	void remove_pedestrian(const int i, const int j);

	public:
	StaticFloor(unsigned int w, unsigned int h, unsigned int apeoplecount, int shape=1);
	~StaticFloor();

	/**
	 * Nastavi velikost pole
	 */
	void set_size(unsigned int w, unsigned int h) { Floor::set_size(w,h); }

	/**
	 * Vrati priznak, zda je dana bunka jiz obsazena
	 */
	bool isOccupied(const int i, const int j) const { return lattice[i][j].occupation; }

	/**
	 * Vrati priznak, zda je dana bunka prekazkou (napr. zed)
	 */
	bool isForbidden(const int i, const int j) const { return lattice[i][j].forbidden; }

	/**
	 * Vrati priznak, zda jsou na dane bunce dvere
	 */
	bool isDoor(const int i, const int j) const { return lattice[i][j].door; }

	double value(const int i, const int j) const;	//!< vrati silu pole v dane souradnici

	void move_pedestrian(const int i1, const int j1, const int i2, const int j2);	//!< presune chodce na jinou bunku

	void remove_pedestrians_near_doors();	//!< odstrani chodce, co jsou v sousedni blizkosti se dvermi

	std::vector<struct Coord> sync();	//!< synchronizace se skutecnym stavem mistnosti

	void printout() const;	// debugovaci vypis
};

#endif
