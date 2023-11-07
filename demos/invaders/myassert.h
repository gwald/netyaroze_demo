//		assertion: use printout to siocons console

// use #define switch ASSERTIONS_ON;
// whether defined or not ...





#ifndef ASSERTIONS_ON

	#define assert(ignore)

#else

	#define assert(ex) \
        ((ex) ? 1 : \
              (printf("Assertion failure! " #ex " At line %d of file `%s'.\n", \
               __LINE__, __FILE__), exit(1), 0))

#endif	   
	 