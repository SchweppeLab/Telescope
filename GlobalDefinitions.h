#ifndef _GLOBALDEFINITIONS_H
#define _GLOBALDEFINITIONS_H

#define MAXMC 2
#define MINPEPMASS 600
#define MAXPEPMASS 5000
#define MINPEPLEN 5
#define MAXPEPLEN 50
#define MAXMZ 2000              //Convenient ceiling to fragment ion size. Also means not necessary to keep spectrum peaks past this point.
#define SPECTRUMCOUNT 500000
#define NOISE 00
#define PPM 20
#define MINPEAK 2 //1e-6
#define XCORR 1
#define MAXMODS 2
#define SEMIENZYME false
#define PSMCOUNT 5

#define HISTOSZ 152

#define PROTON 1.00727646688  //matches comet

#endif
