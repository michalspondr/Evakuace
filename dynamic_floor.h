#ifndef DYNAMIC_FLOOR_H
#define DYNAMIC_FLOOR_H
#include "floor.h"

/**
 * Dynamicka vrstva, obsahujici tzv. bosony.
 * Bosony jsou castice, ktere "zanechavaji" chodci behem cestovani v prostoru.
 * Jde o jakousi vlastnost, ktera se da pripodobnit k chemotaxi, tedy pohybu organismu ve smeru chemickeho gradientu.
 * Namisto chemie se zde mysli socialni vlivy (napr. typy chovani, nachylnost k panice, stadni chovani atd.)
 */
class DynamicFloor {
	unsigned int **bossonTrace;

	/**
	 * Rozptyl. Ukazuje, jak moc se kazdy boson siri v case
	 * Rozptyl se pohybuje v rozmezi [0,1]
	 */
	double diffuse;

	/**
	 * Slabnuti. Ukazuje, jak moc bosony slabnou v case
	 * Slabnuti se pohybuje v rozmezi [0,1]
	 */
	double decay;

	void makeDiffuse(const int i, const int j, const int v);	//!< provede 1 krok rozptylu bosonu
	void makeDecay(const int i, const int j);	//!< provede 1 krok slabnuti bosonu

	void updateBossonTrace(const int i, const int j, const int x);	//!< provede upravu daneho prvku

	unsigned int aheight;	//!< promenna pro pamatovani poctu radku pole kvuli uvolneni pameti
	Floor* floor;


	public:
	DynamicFloor(unsigned int w, unsigned int h, double diffuse, double decay, Floor* afloor);
	~DynamicFloor();
	unsigned int value(const int i, const int j) const;	//!< vrati silu bosonu v dane bunce
	void update();		//!< provede pripadne zmeny v dynamicke vrstve
	void increase(const int i, const int j);	//!< zvysi hodnotu daneho bosonu
	void decrease(const int i, const int j);	//!< zvysi hodnotu daneho bosonu

	void printout() const;
};

#endif
