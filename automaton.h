#ifndef AUTOMATON_H
#define AUTOMATON_H

#include "static_floor.h"
#include "dynamic_floor.h"

/**
 * Trida pro celularni automat
 * Vytvori staticke a dynamicke pole a pocita pravdepodobnosti prechodu na okolni bunky
 */
class Automaton {
	StaticFloor* staticFloor;	//!< staticke pole
	DynamicFloor* dynamicFloor;	//! dynamicke pole

	double **probability_grid;	//!< pravdepodobnosti presunu pro dane bunky

	size_t time;	//<! cas automatu
	float kS;	//<! citlivost staticke vrstvy
	float kD;	//<! citlivost dynamicke vrstvy
	unsigned int aheight;		//!< promenna pro uchovani velikosti vysky pole

	void transition_probability(const int i, const int j);	//!< pravdepodobnost presunu z bunky (i,j)
	void move_pedestrian(const int i1, const int j1, const int i2, const int j2);	// presun chodce na novou souradnici
	bool can_move(const int i, const int j) const;	// zjisti, zda se da presunou na danou bunku
	void sync();	//!< synchronizuje staticke pole
	void remove_pedestrians_near_doors();	//!< odstrani z mistnosti chodce, kteri jsou u dveri

	public:
	Automaton(unsigned int width=FIELD_WIDTH, unsigned int height=FIELD_HEIGHT, unsigned int people_count=PEOPLE_COUNT, int shape=0, double diffuse=DIFFUSE, double decay=DECAY, float ks=KS, float kd=KD);
	~Automaton();
	void update();	//!< dalsi krok automatu
	bool with_pedestrians() const;	//!< vrati true, pokud jsou v CA jeste nejake osoby
	size_t get_time() const { return time; }

	void printout() const;	// debugovaci vypis
};
#endif
