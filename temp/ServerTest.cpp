	#include <iostream>
	#include "Server.hpp"
	using namespace std;
	int main()
	{
	    Server s1('A'), s2('B'), s3('C');
	    int number, count;
	    do {
	        cout << "How many in group ?";
	        cin >> number;
	        cout << "Your turns are: ";
	        for (count = 0; count < number; count++) 
	        {
	            cout << Server::getTurn() << " ";
	        }
	        cout << endl;
	        s1.serveOne();
	        s2.serveOne();
	        s3.serveOne();
	    } while (Server::stillOpen());
	    
	    return 0;
}