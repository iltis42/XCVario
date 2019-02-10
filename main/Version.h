/*
 * Version.h
 *
 *  Created on: Feb 10, 2019
 *      Author: iltis
 */

#ifndef MAIN_VERSION_H_
#define MAIN_VERSION_H_

class Version {
public:
	Version();
	virtual ~Version();
	static char _version[12];
	char *version()  { return _version; };
};

#endif /* MAIN_VERSION_H_ */
