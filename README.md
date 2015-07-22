#A lite serialization solution(Only Head file needed)
there are several lib out there support serialization,like protobuffer,flatbuffers,capnproto.
Need schema file for generating source files, which is cool between peers. 
but that is too heavy for me. so I wrote this lite version.only support c++. 
If your just want to serialize & deserialize data on the same machine. I think it is a  good choice. simple as it is , hack whatever u want.

#support 
* vector
* list
* map
* set
* string
* primitives(int,double,long,...)
* nesting container support
```cpp
Obj : implements iSerializable interface
vector<map<int, Obj*> >  
vector<vector<string> >  
vector<list<string> >  
vector<customer_struct>
vector<string>  
```
* endian auto conversion  
  no need to worry about endian problem.all data is writed in little endian.


#not support 
```cpp
vector<Obj>   //not support non-pointer Obj,and I don`t think that`s a good idea though :)
```
use char instead bool in STL
[why](http://stackoverflow.com/questions/15809157/why-is-the-size-of-stdvectorbool-16-byte)


#build
you can build this project by CMake. or  just import **serialization.h** into your project.

cmake build process:

```sh
	mkdir prj
	cd prj
	cmake ..
```
if it doesn`t work. set compile flags to c++11


#demo
check testSerialization.cpp 
 
 

