#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{

	//Date Version Types
	static const char DATE[] = "25";
	static const char MONTH[] = "2";
	static const char YEAR[] = "2013";
	static const char UBUNTU_VERSION_STYLE[] = "13.2";

	//Software Status
	static const char STATUS[] = "Release";
	static const char STATUS_SHORT[] = "r";

	//Standard Version Type
	static const long MAJOR = 4;
	static const long MINOR = 2;
	static const long BUILD = 0;
	static const long REVISION = 0;

	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 0;
	#define RC_FILEVERSION 4,2,0,0
	#define RC_FILEVERSION_STRING "4, 2, 0, 0\0"
	static const char FULLVERSION_STRING[] = "4.2.0.0";

	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;


}
#endif //VERSION_H
