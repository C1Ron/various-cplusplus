SRC1 = timer1.cpp
SRC2 = timer2.cpp
SRC3 = timer3.cpp
SRC4 = timer4.cpp
SRC5 = timer5.cpp
TARGET1 = timer1
TARGET2 = timer2
TARGET3 = timer3
TARGET4 = timer4
TARGET5 = timer5

CXXFLAGS = -std=c++17 -I/home/c1ron/downloads/boost_1_85_0 -L/home/c1ron/downloads/boost_1_85_0/stage/lib

$(TARGET1): $(SRC1)
	g++ $(CXXFLAGS) $(SRC1) -o $(TARGET1)
$(TARGET2): $(SRC2)
	g++ $(CXXFLAGS) $(SRC2) -o $(TARGET2)
$(TARGET3): $(SRC3)
	g++ $(CXXFLAGS) $(SRC3) -o $(TARGET3)
$(TARGET4): $(SRC4)
	g++ $(CXXFLAGS) $(SRC4) -o $(TARGET4)
$(TARGET5): $(SRC5)
	g++ $(CXXFLAGS) $(SRC5) -o $(TARGET5)

clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) *.o