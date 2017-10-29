#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

int main() 
{ 
	int *P_Bt, *P_Wt, *P_Tat;
	float avg_wt=0.0, avg_tat=0.0;

    ifstream file("Q1.txt");
    string str;
    int num, count=0;
    file >> num;

    P_Bt = new int[num];
	P_Wt = new int[num];
	P_Tat = new int[num];


    while(!file.eof()){
    	for(int i=0;i<num;++i)
    	{
    		file >> str;
    		P_Bt[i] = atoi(str.c_str());
    		//cout << "Burst " << i << ":" << P_Bt[i] << "\n";
    	}
    	/*cout << file.peek() << "\n";
    	if (file.peek() == '\r'){
    		file.ignore(2);
    	}*/
    }

    P_Wt[0] = 0;

	// calculate waiting time
	for (int i = 1 ; i < num ; ++i)
	{
		P_Wt[i] = 0;
		for (int j = 0 ; j < i ; ++j)
		{
			P_Wt[i] += P_Bt[j];
		}
	}


	// calculate turnaround time
	for (int i = 0 ; i < num ; ++i)
	{
		P_Tat[i] = P_Wt[i] + P_Bt[i];
	}

	cout<<"Process\tWaiting Time\tTurnaround Time";

	for (int i = 0; i < num ; i++)
    {
        avg_wt+=P_Wt[i];
        avg_tat+=P_Tat[i];
        cout<<"\nP["<<i+1<<"]"<<"\t"<<P_Wt[i]<<"\t\t"<<P_Tat[i];
    }
 
    avg_wt /= (float)num;
    avg_tat/= (float)num;
    cout<<"\n\nAverage Waiting Time:"<<avg_wt;
    cout<<"\nAverage Turnaround Time:"<<avg_tat<<"\n";




}
