#include "buffer.hpp"
#include <vector>

template <class T, size_t nblock>
filer<T, nblock>::filer( const std::string& filename,
			     bool append)
{
  using namespace std;
  if (append){
    file.open(filename.c_str(), 
	       ios_base::binary | ios_base::in | ios_base::out);
    // go to end of file
    file.seekg(0,ios_base::end);
    // get file size
    size=file.tellg() / this->chunk_size;
    if (file.tellg() % this->chunk_size) 
	throw ios_base::failure(
	   "Filer::filer: File contains non-integer multiple of chunk size");   
  }else{
    // ignore file if it exists (ios_base::trunc) 
    file.open(filename.c_str(), ios_base::binary | ios_base::in | 
	       ios_base::out | ios_base::trunc );
    size=0;
  }
}

template <class T, size_t nblock>
void filer<T, nblock>::read_chunk( size_t pos, T* d){

  if (pos >= size)
    throw 
      std::ios_base::failure(
	    "Filer:read_chunk: pos exceeds file size in reading.");

  size_t p( pos * chunk_size);
  file.seekg( p );

  file.read((char*)d, chunk_size );
  if (file.bad())
    throw std::ios_base::failure(
      "filer:read_chunk: IO-Error");   
}

template <class T, size_t nblock>
void filer<T, nblock>::write_chunk( size_t pos,
				    const T* d){

  if (size <= pos){
    size_t missing(pos - size); 
    // if size=pos I do not need to write intermediates
    file.seekp( size * chunk_size ); // current end of file
    std::vector<T> empty(nblock,0);
    for (size_t i=0;i < missing; ++i)
      file.write((char*)&empty[0], chunk_size );
    size = pos+1;
  }else{
    file.seekp( pos * chunk_size );
  }

  file.write((char*)d, chunk_size );
  if (file.bad())
    throw std::ios_base::failure(
      "filer:write_chunk: IO-Error");   

  file.flush();
}

