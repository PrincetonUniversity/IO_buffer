#ifndef BUFFER_F_FLO
#define BUFFER_F_FLO

#include <iostream>
#include <fstream>


template <class T> 

class filer{  
public:

  // create filer object associated with file "filename"
  // and blocksize nb
  // rewrite file if append=false, otherwise reopen file

  filer( const std::string& filename, 
	 size_t nb,
	 bool append = false );
  ~filer(){};

  // read a chunk at pos, exception if pos > filesize
  void read_chunk( size_t pos, T* );
  // write a chunk at pos, file is enlarged if pos > filesize
  void write_chunk( size_t pos, const T* );

  // size (in units of chunks)
  size_t filesize(){ return size; };

private:

  size_t nblock;

  size_t chunk_size;

  filer();

  size_t nblock;

  size_t chunk_size;

  std::fstream ffile;

  size_t size;

};

#endif
