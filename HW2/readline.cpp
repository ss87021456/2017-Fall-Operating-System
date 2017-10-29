#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <sstream>

using namespace std;

int main(){
	
	ifstream file("Q1.txt");
	string str;
	while(getline(file,str)){
		stringstream ss(str);
		string temp;
		cout << str << "\n";
		while(ss >> temp){
			cout << temp << "\n";
		}
	}

	return 0;

}
