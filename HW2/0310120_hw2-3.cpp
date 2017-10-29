#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

class Process {
	public:
		int burst_time;
		int waiting_time;
		int arrival_time;
		int remain_time;
        int turnaround_time;
};

int main(){

	float avg_wt=0.0, avg_tat=0.0;

    ifstream file("Q3.txt");
    string str;
    int num, count=0, burst_time=0, min;
    file >> num;

    // create process array
    Process* process = new Process[num+1];

	// load arrival time and burst time
    while(!file.eof()){
		for(int i=0;i<num;++i)
    	{
    		file >> str;
    		process[i].arrival_time = atoi(str.c_str());
    	}
		for(int i=0;i<num;++i)
    		{
    			file >> str;
    			process[i].burst_time = atoi(str.c_str());
                process[i].remain_time = atoi(str.c_str());
    		}
    }

    process[num+1].remain_time = 9999;

    int remain=0,endTime=0,sum_wait=0,sum_turnaround=0;
    for(int time=0;remain!=num;time++)
    {
        int smallest = num+1;
        for(int i=0;i<num;++i)
        {
            /*choose the smallest remaining time target*/
            if ((process[i].arrival_time <= time)                              
                && (process[i].remain_time < process[smallest].remain_time) 
                && (process[i].remain_time > 0))
                smallest = i;
        }

        process[smallest].remain_time--;

        if(process[smallest].remain_time==0)
        {
            remain++;           //One process complete the total job
            endTime = time + 1; //Total competional time of process

            process[smallest].turnaround_time = endTime - process[smallest].arrival_time;                       //Calculate the turnaround time
            process[smallest].waiting_time = process[smallest].turnaround_time - process[smallest].burst_time;  //Calculate the Waiting Time

            sum_wait += process[smallest].waiting_time;
            sum_turnaround += process[smallest].turnaround_time;
        }
    }

    cout<<"Process\tWaiting Time\tTurnaround Time";

    for(int i=0;i<num;++i)
        cout<<"\nP["<<i+1<<"]"<<"\t"<<process[i].waiting_time<<"\t\t"<<process[i].turnaround_time;

    cout<<"\n\nAverage waiting time : "<<sum_wait*1.0/num;
    cout<<"\nAverage turnaround time : "<<sum_turnaround*1.0/num<<"\n";

    // delete process array
    delete[] process;

	return 0;
}