#include "worldgen.h"
#include <iostream>
#include <string>
using namespace std;

int main(){
    int rows, cols, room_density, room_w, room_h, room_stdev, spacy, pillars;
    string input;
    while(true){
    	cout << "ROWS(21): ";
    	cin >> rows;
    	rows = (rows/2)*2+1;
    	cout << "COLUMNS(79): ";
    	cin >> cols;
    	cols = (cols/2)*2+1;
    	cout << "ROOM DENSITY(750): ";
    	cin >> room_density;
    	cout << "ROOM WIDTH(11): ";
    	cin >> room_w;
    	cout << "ROOM HEIGHT(6): ";
    	cin >> room_h;
    	cout << "ROOM DEVIATION(2): ";
    	cin >> room_stdev;
    	cout << "SPACINESS(10): ";
    	cin >> spacy;
    	cout << "PILLARS(0): ";
    	cin >> pillars;
    	while(true){
    		cout << "CREATE? ";
    		cin.ignore();
    		getline(cin,input);
    		if (input == "yes"){
    			WorldGen maker{rows, cols};
    			while (true){
					maker.generate(room_density, room_w, room_h, room_stdev, spacy, pillars);
					cout << "AGAIN? ";
					getline(cin,input);
					cout << input << endl;
					if (input == "yes") continue;
					else break;
				}
    		}
    		while(true){
    			cout << "RESTART? ";
    			getline(cin,input);
    			if (input == "yes") break;
    			else return 0;
    		}
    		break;
    	}

    }
}
