#ifndef SINGLETON_H_F
#define SINGLETON_H_F

#include <memory>
#include <iostream>

template <typename T>
class Singleton{
    Singleton(const Singleton<T>&){};

    static std::shared_ptr<T>& get_p()
    {        
        static std::shared_ptr<T> s;

	if (!s) s.reset(new T);

        return s;
    }
protected:
  Singleton(){};
public:

    static T& get(){
	return *get_p();
    }

    template <typename S>
    static T& reset(const S& ns){ get_p().reset(new T(ns));}

    static void destroy(){ get_p().reset();};    
};

#endif
