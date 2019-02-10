/*
 * Version.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: iltis
 */

#include "Version.h"
#include <stdio.h>
#include <string.h>

char Version::_version[12];

Version::Version() {
	const char data[]=__DATE__;
	const char tempo[]=__TIME__;
	const char nomes[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char omes[4];
	int ano, mes, dia, hora, min, seg;
	sscanf(data, "%s %d %d", omes, &dia, &ano);
	sscanf(tempo, "%d:%d:%d", &hora, &min, &seg);
	mes=(strstr(nomes, omes)-nomes)/3+1;

	sprintf(_version,"%02d.%02d%02d", ano%100, mes, dia );
}

Version::~Version() {
}

