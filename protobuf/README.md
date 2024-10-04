## Install protobuf and sqlite3
```
sudo apt install -y protobuf-compiler libprotobuf-dev sqlite3
```
## Make and compile a .proto file
```
protoc --cpp_out=. person.proto
```
## Using the libraries
```
g++ -std=c++17 -o savePerson save_person.cpp person.pb.cc -lprotobuf -lsqlite3
g++ -std=c++17 -o loadPerson load_person.cpp person.pb.cc -lprotobuf -lsqlite3
```
## Using the application
```
savePerson <fname> <lname> <lang> <bday> <bmonth> <byear>
loadPerson <#ID>
```
