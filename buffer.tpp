#include "buffer.hpp"
#include <vector>

template <class T>
filer<T>::filer( const std::string& filename,
		 size_t nb,
		 bool append):
  nblock(nb),
  chunk_size(nblock * sizeof(T))
{
  using namespace std;
  if (append){
    ffile.open(filename.c_str(), 
	       ios_base::binary | ios_base::in | ios_base::out);
    // go to end of file
    ffile.seekg(0,ios_base::end);
    // get file size
    size=ffile.tellg() / this->chunk_size;
    if (ffile.tellg() % this->chunk_size) 
	throw ios_base::failure(
	   "Filer::filer: File contains non-integer multiple of chunk size");   
  }else{
    // ignore file if it exists (ios_base::trunc) 
    ffile.open(filename.c_str(), ios_base::binary | ios_base::in | 
	       ios_base::out | ios_base::trunc );
    size=0;
  }
}

template <class T>
void filer<T>::read_chunk( size_t pos, T* d){

  if (pos >= size)
    throw 
      std::ios_base::failure(
	    "Filer:read_chunk: pos exceeds file size in reading.");

  size_t p( pos * chunk_size);
  ffile.seekg( p );

  ffile.read((char*)d, chunk_size );
  if (ffile.bad())
    throw std::ios_base::failure(
      "filer:read_chunk: IO-Error");   
}

template <class T>
void filer<T>::write_chunk( size_t pos,
			    const T* d){

  if (size <= pos){
    size_t missing(pos - size); 
    // if size=pos I do not need to write intermediates
    ffile.seekp( size * chunk_size ); // current end of file
    std::vector<T> empty(nblock,0);
    for (size_t i=0;i < missing; ++i)
      ffile.write((char*)&empty[0], chunk_size );
    size = pos+1;
  }else{
    ffile.seekp( pos * chunk_size );
  }

  ffile.write((char*)d, chunk_size );
  if (ffile.bad())
    throw std::ios_base::failure(
      "filer:write_chunk: IO-Error");   

  ffile.flush();
}

