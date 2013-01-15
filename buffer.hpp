#ifndef BUFFER_F_FLO
#define BUFFER_F_FLO

#include <iostream>
#include <fstream>
#
template <class T, size_t nblock> 
class filer{  
public:
 
  static const size_t chunk_size = nblock * sizeof(T);

  // create filer object associated with file "filename"
  // rewrite file if append=false, otherwise reopen file
  filer( const std::string& filename, bool append = false );
  ~filer(){};

  // read a chunk at pos, exception if pos > filesize
  void read_chunk( size_t pos, T* );
  // write a chunk at pos, file is enlarged if pos > filesize
  void write_chunk( size_t pos, const T* );

  // size (in units of chunks)
  size_t filesize(){ return size; };

private:

  filer();

  std::fstream datei;

  size_t size;

};

#endif
