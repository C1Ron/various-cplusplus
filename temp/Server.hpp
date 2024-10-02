	#include <iostream>
	using namespace std;
	class Server
	{
	public:
	    Server(char letterName);
	    static int getTurn();
	    void serveOne();
	    static bool stillOpen();
	private:
	    static int turn;
	    static int lastServed;
	    static bool nowOpen;
	    char name;
	};
	
	// Constructor of the Server class
	Server::Server(char letterName) : name(letterName)
	{/*Intentionally empty*/}
	
	// Members of the Server class
	int Server::getTurn() // Since getTurn() is static, only static members can be referenced
	{
	    turn++;
	    return turn;
	}
	
	bool Server::stillOpen()
	{
	    return nowOpen;
	}
	
	void Server::serveOne()
	{
	    if (nowOpen && lastServed < turn) {
	        lastServed++;
	        cout << "Server " << name << " now serving " << lastServed << endl;
	    }
	    if (lastServed >= turn) { // Everyone is served
	        nowOpen = false;
	    }
	}
	
	// Static variables of the Server class
	int Server::turn = 0;
	int Server::lastServed = 0;
bool Server::nowOpen = true;