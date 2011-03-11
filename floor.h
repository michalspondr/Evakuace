#ifndef FLOOR_H
#define FLOOR_H

#include "constants.h"
#include <stdexcept>

/**
 * Bunka pole
 */
struct Cell {
	bool forbidden;		//!< je true, pokud je bunka nedostupna (je to napr. zed)
	bool occupation;	//!< je true, pokud je bunka jiz nekym obsazena
	bool door;		//!< je true, pokud jsou v miste bunky dvere
};

class Floor {
	int people_count;	//!< pocet lidi v mistnosti
	void set_doors(const int x, const int y);	//!< nastavi dvere na dane souradnici

	protected:
	struct Cell **lattice;	//!< zakladni pole, ve kterem se budou lide pohybovat
	static unsigned int width, height;					//!< rozmery pole

	void set_size(unsigned int w, unsigned int h) {width=w; height=h;}	//!< nastavi rozmery pole
	
	public:
	Floor(unsigned int w, unsigned int h, unsigned int apeoplecount);
	virtual ~Floor();
	void init(int shape) throw(std::domain_error);	//!< vytvori mistnost
	int people() const { return people_count; }
	void decrease_people();	//!< snizi pocet lidi v mistnosti
	unsigned int get_width() {return width;}		//!< vrati sirku pole
	unsigned int get_height() {return height;}		//!< vrati vysku pole

	void printout(size_t current_time) const;	// debugovaci vypis
};

#endif
