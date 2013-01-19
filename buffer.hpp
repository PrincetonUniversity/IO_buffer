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

  filer( const std::string& file_name, 
	 size_t nb,
	 bool append = false );
  ~filer(){};

  // read a chunk at pos, exception if pos > filesize
  void read_chunk( size_t pos, T* );
  // write a chunk at pos, file is enlarged if pos > filesize
  void write_chunk( size_t pos, const T* );

  // size (in units of chunks)
  size_t filesize() const { return size_; };

  const std::string& filename() const { return filename_;}

private:

  filer();

  std::string filename_;

  size_t nblock_;

  size_t chunk_size_;

  std::fstream ffile_;

  size_t size_;

};

#endif
