// SDCC verwendet je nach Version unterschiedliche Namen fuer denn Startup (doppelter
// Unterstrich zu Beginn des Namens
#if __SDCC_REVISION >= 13762
#define STARTUP_FUNCTION __sdcc_external_startup
#else
#define STARTUP_FUNCTION _sdcc_external_startup
#endif
