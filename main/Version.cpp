/*
 * Version.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: iltis 
 */

#include "Version.h"
#include <stdio.h>
#include <string.h>
#include <logdef.h>

char Version::_version[32];
char * program_version;

Version::Version() {
	const char data[]=__DATE__;
	const char tempo[]=__TIME__;
	const char nomes[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char omes[4];
	int ano, mes, dia, hora, min, seg;
	sscanf(data, "%s %d %d", omes, &dia, &ano);
	sscanf(tempo, "%d:%d:%d", &hora, &min, &seg);
	mes=(strstr(nomes, omes)-nomes)/3+1;

<<<<<<< HEAD
	sprintf(_version,"%02d.%02d%02d-%02d%02d", ano%100, mes, dia, hora, min );
=======
	sprintf(_version,"%02d.%02d%02d-%02d", ano%100, mes, dia, hora );
	program_version = _version;
>>>>>>> 6d4fdd1baa8561b1de0632823deccb549c2dcd9b
}

Version::~Version() {
}

