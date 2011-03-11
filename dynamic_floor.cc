#include "dynamic_floor.h"
#include "constants.h"
#include <cstdlib>
#include <cstring>
#ifdef DEBUG
#include <iostream>
#endif

DynamicFloor::DynamicFloor(unsigned int w, unsigned int h, double adiffuse, double adecay, Floor* afloor) : diffuse(adiffuse), decay(adecay) {
	aheight = h;
	bossonTrace = (unsigned int **)std::calloc(h, sizeof(unsigned int *));
	for (int i=0; i<h; ++i) {
		bossonTrace[i] = (unsigned int *)std::calloc(w, sizeof(unsigned int));
	}

	floor = afloor;
}

DynamicFloor::~DynamicFloor() {
	for (int i=0; i < aheight; ++i) {
		free(bossonTrace[i]);
	}
	free(bossonTrace);
}

/**
 * Aktualizace dynamicke vrstvy - rozptylu a slabnuti
 */
void DynamicFloor::update() {
	const unsigned int height = floor->get_height();
	const unsigned int width = floor->get_width();

	for (int i=0; i < height; ++i) {
		for (int j=0; j < width; ++j) {
			// bunce se snizi hodnota s pravdepodobnosti 'decay'
			if ( ((double)(std::rand()%100)/100.0) <= decay) {
				makeDecay(i,j);
			}

			// bunka rozptyli svou hodnotu do jedne ze sve okolnich bunek s pravdepodobnosti 'diffuse'
			// -> zvysi hodnotu okolni bunky o 1
			if ( ((double)(std::rand()%100)/100.0) <= diffuse) {
				const short tmp = std::rand()%4;
				switch (tmp) {
					case 0 : // vlevo
						makeDiffuse(i,j-1, 1);
						break;
					case 1 : // vpravo
						makeDiffuse(i,j+1, 1);
						break;
					case 2 : // nahoru
						makeDiffuse(i-1,j, 1);
						break;
					case 3 : // dolu
						makeDiffuse(i+1,j, 1);
						break;
					default:
						break;
				}
			}
		}
	}
}

/**
 * Provede 1 krok rozptylu bosonu
 */
void DynamicFloor::makeDiffuse(const int i, const int j, const int v) {
	const unsigned int height = floor->get_height();
	const unsigned int width = floor->get_width();

	if (i<0 || j<0 || i>=height || j>=width) return;
	updateBossonTrace(i, j, v);
}

/**
 * Provede 1 krok slabnuti bosonu
 */
void DynamicFloor::makeDecay(const int i, const int j) {
	const unsigned int height = floor->get_height();
	const unsigned int width = floor->get_width();

	// osetreni hranic
	if (i<0 || j<0 || i>=height || j>=width) return;
	updateBossonTrace(i, j, -1);
}

/**
 * Vrati hodnotu dynamicke vrstvy v dane bunce
 */
unsigned int DynamicFloor::value(const int i, const int j) const {
	return bossonTrace[i][j];
}

/**
 * Zvysi hodnotu dynamicke vrstvy v dane bunce
 * To odpovida zvysenemu zajmu o pruchod touto bunkou
 */
void DynamicFloor::increase(const int i, const int j) {
	++bossonTrace[i][j];
}

/**
 * Snizi hodnotu dynamicke vrstvy v dane bunkce
 */
void DynamicFloor::decrease(const int i, const int j) {
	--bossonTrace[i][j];
}

/**
 * Provede upravu hodnoty daneho prvku pole
 */
void DynamicFloor::updateBossonTrace(const int i, const int j, const int x) {
	const unsigned int height = floor->get_height();
	const unsigned int width = floor->get_width();

	// osetreni hranic
	if (i<0 || j<0 || i>=height || j>=width) return;
	// kontrola preteceni
	if (x < 0 && -x > bossonTrace[i][j]) bossonTrace[i][j]=0;
	else bossonTrace[i][j] += x;
}

void DynamicFloor::printout() const {
#ifdef DEBUG
	const unsigned int height = floor->get_height();
	const unsigned int width = floor->get_width();

	for (int i=0; i < height; ++i) {
		for (int j=0; j < width; ++j) {
			std::cout << bossonTrace[i][j] << " ";
		}
		std::cout << std::endl;
	}
#endif
}
