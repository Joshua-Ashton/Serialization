#A lite serialization solution
there are several lib out there support serialization,like protobuffer, boost:serialization , too heavy for me.  so I wrote this lite version.

#support 
* vector
* map
* set
* string
* primitives(int,double,long,...)
* nesting container support
```cpp
vector<map<int, Obj*> >  
vector<vector<string> >  
vector<list<string> >  
vector<string>  
```
* endian auto conversion  
  no need to worry about endian problem.all data write as little endian.


#not support 
```cpp
vector<Obj>   //not support non-pointer class in container except std::string
```
use char instead bool in STL
[why](http://stackoverflow.com/questions/15809157/why-is-the-size-of-stdvectorbool-16-byte)


#build
you can build this project by CMake. or  just import **serialization.h** into your project.

camke build process:

```sh
	mkdir prj
	cd prj
	cmake ..
```
#usage
define CHECK_ENDIAN=1  if you wanna check endian

#demo
check testSerialization.cpp 
 
 

