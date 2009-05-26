#ifndef CALCDEFS_H
#define CALCDEFS_H

#include <SupportDefs.h>

class CCalcThread ;
class CCalcStack ;
class CCalculateJob ;
const uint32 msg_CalculationDone = 'DonC';

// calculation cell states
enum {
	kUndefined,
	kOnStack,
	kCalculated,
	kCircle
};

#endif

