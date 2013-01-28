#include "buffer.hpp"
#include <vector>

template <class T>
filer<T>::filer( const std::string& file_name,
		 size_t nb,
		 bool append):
  filename_(file_name),
  chunk_size_(nb),
  chunk_byte_size_(chunk_size_ * sizeof(T))
{
  using namespace std;
  if (append){
    ffile_.open(filename().c_str(), 
	       ios_base::binary | ios_base::in | ios_base::out);
    // go to end of file
    ffile_.seekg(0,ios_base::end);
    // get file size
    size_=ffile_.tellg() / this->chunk_byte_size_;
    if (ffile_.tellg() % this->chunk_byte_size_) 
	throw ios_base::failure(
	   "Filer::filer: File contains non-integer multiple of chunk size");   
  }else{
    // ignore file if it exists (ios_base::trunc) 
    ffile_.open(filename().c_str(), ios_base::binary | ios_base::in | 
	       ios_base::out | ios_base::trunc );
    size_=0;
  }
}

template <class T>
void filer<T>::read_chunk( size_t pos, T* d){

  if (pos >= size_)
    throw 
      std::ios_base::failure(
	    "Filer:read_chunk: pos exceeds file size in reading.");

  size_t p( pos * chunk_byte_size_);

  std::lock_guard<std::mutex> lg(filemutex_);

  ffile_.seekg( p );

  ffile_.read((char*)d, chunk_byte_size_ );
  if (ffile_.bad())
    throw std::ios_base::failure(
      "filer:read_chunk: IO-Error");   
}

template <class T>
void filer<T>::write_chunk( size_t pos,
			    const T* d){
  std::lock_guard<std::mutex> lg(filemutex_);
  if (size_ <= pos){
    size_t missing(pos - size_); 
    // if size=pos I do not need to write intermediates
    ffile_.seekp( size_ * chunk_byte_size_ ); // current end of file
    std::vector<T> empty(chunk_size_,0);
    for (size_t i=0;i < missing; ++i)
      ffile_.write((char*)&empty[0], chunk_byte_size_ );
    size_ = pos+1;
  }else{
    ffile_.seekp( pos * chunk_byte_size_ );
  }

  ffile_.write((char*)d, chunk_byte_size_ );
  if (ffile_.bad())
    throw std::ios_base::failure(
      "filer:write_chunk: IO-Error");   

  ffile_.flush();
}

