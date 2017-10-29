#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

int main() 
{ 
	int *P_Bt, *P_Wt, *P_Tat, *P_At, *P_id;
	float avg_wt=0.0, avg_tat=0.0;

    ifstream file("Q2.txt");
    string str;
    int num, count=0, burst_time=0, min;
    file >> num;

    P_Bt = new int[num];
    P_At = new int[num];
	P_Wt = new int[num];
	P_Tat = new int[num];
	P_id = new int[num];

	// initial P_id
	for (int i=0;i<num;++i)
	{
		P_id[i] = i+1;
	}

	// load arrival time and burst time
    while(!file.eof()){
		for(int i=0;i<num;++i)
    	{
    		file >> str;
    		P_At[i] = atoi(str.c_str());
    		//cout << "Arrival " << i << ":" << P_At[i] << "\n";
    	}
		for(int i=0;i<num;++i)
    		{
    			file >> str;
    			P_Bt[i] = atoi(str.c_str());
    			//cout << "Burst " << i << ":" << P_Bt[i] << "\n";
    		}
    }

    // Sorting According to Arrival Time
    for(int i=0;i<num;i++)
    {
    	for(int j=0;j<num;j++)
    	{
    		if (P_At[i] < P_At[j])
    		{
    			swap(P_id[i],P_id[j]);
    			swap(P_At[i],P_At[j]);
    			swap(P_Bt[i],P_Bt[j]);
    		}
    		else if (P_At[i] == P_At[j])		// deal with same arrival time problem
    			{
    				if (P_Bt[i] < P_Bt[j])
    				{
    					swap(P_id[i],P_id[j]);
    					swap(P_At[i],P_At[j]);
    					swap(P_Bt[i],P_Bt[j]);
    				}
    			}
    	}
    }

    //for (int i=0;i<num;++i)
    	//cout << P_id[i] << "\n";

    /*Arranging the table according to Burst time,
	Execution time and Arrival Time
	Arrival time <= Execution time
	*/

	int k = 1;

	for(int i=0;i<num;i++)
	{
		burst_time = burst_time + P_Bt[i];
		min = P_Bt[k];
		for (int j=k;j<num;++j)
		{
			if ((P_At[j] <= burst_time) && (P_Bt[j] < min))
			{
				swap(P_id[k],P_id[j]);
				swap(P_At[k],P_At[j]);
				swap(P_Bt[k],P_Bt[j]);
			}
		}
		++k;
	}


	// Calculate for waiting time
	P_Wt[0]=0;
	
	int sum = 0;
	int sum_wt = 0;
	for(int i=1;i<num;i++)
	{
		sum=sum+P_Bt[i-1];
		P_Wt[i]=sum-P_At[i];
		sum_wt=sum_wt+P_Wt[i];
	}
	 
	avg_wt = ((float)sum_wt/(float)num);

	// Calculate for turnaround time
	int sum_tat = 0;
	for(int i=0;i<num;i++)
	{
		P_Tat[i] = P_Wt[i] + P_Bt[i];
		sum_tat += P_Tat[i];
	}
	 
	avg_tat = ((float)sum_tat/(float)num);

	cout<<"Process\tWaiting Time\tTurnaround Time";

	// sort by index
   for(int i=0;i<num;i++)
    {
    	for(int j=0;j<num;j++)
    	{
    		if (P_id[i] < P_id[j])
    		{
    			swap(P_id[i],P_id[j]);
    			swap(P_At[i],P_At[j]);
    			swap(P_Bt[i],P_Bt[j]);
    			swap(P_Wt[i],P_Wt[j]);
    			swap(P_Tat[i],P_Tat[j]);
    		}
    	}
    }


	for (int i=0;i<num;i++)
    {
    	cout<<"\nP["<<i+1<<"]"<<"\t"<<P_Wt[i]<<"\t\t"<<P_Tat[i];
       // cout<<"\n"<<P_id[i]<<"\t"<<P_Wt[i]<<"\t\t"<<P_Tat[i];
    }
 
    cout<<"\n\nAverage Waiting Time:"<<avg_wt;
    cout<<"\nAverage Turnaround Time:"<<avg_tat<<"\n";


    return 0;
}