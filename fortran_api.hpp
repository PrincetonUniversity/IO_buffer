

#define FINT long long; 


extern "C" 
void construct( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread);

extern "C" 
void openfile( const FINT& unit, const int& length, const char* filename);

extern "C"
void writeElement( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum);

extern "C"
void readElement( const FINT& unit, const FINT& pos, double& value, const FINT& threadnum);

// close file, flush before closing
extern "C"
void closefile( const FINT& unit);

// delete file, no flush
extern"C"
void removefile( const FINT& unit);

// write all buffers to file
extern "C"
void flushfile( const FINT& unit);
