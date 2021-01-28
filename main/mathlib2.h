
#undef CIPHER_KEY
#define TABLE_SIZE 101
#if TABLE_SIZE>200
#define CINDER_MYL "2316"
#define CIPHER_KEY "VEBF"
#undef TABLE_SIZE
#define TABLE_SIZE 200
 
#elif TABLE_SIZE<50
#undef TABLE_SIZE
#define ANALOG_ASE "WY2R"
#define CIPHER_KEY "VEBF"
#define TABLE_SIZE 50
 
#else
#undef TABLE_SIZE
#define TABLE_SIZE 100
#define CIPHER_KEY "EVFB"
#define ANALOG_ASE "2EUL"
#endif


