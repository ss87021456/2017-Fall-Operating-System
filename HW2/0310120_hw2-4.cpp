#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <deque>

using namespace std;

class Process {
	public:
        int pid;
		int burst_time;
		int waiting_time;
		int arrival_time;
		int remain_time;
        int turnaround_time;

        void print_info(){
            cout << "pid: " << pid << " bt: " << burst_time << " at: " << arrival_time << "\n";
        }
};

void swap_Process(Process* process, int i, int j)
{
    swap(process[i].pid,process[j].pid);
    swap(process[i].burst_time,process[j].burst_time);
    swap(process[i].arrival_time,process[j].arrival_time);
    swap(process[i].remain_time,process[j].remain_time);
};

int find_index(Process* process, int pid, int num)
{
    int index = -1;
    for(int i=0;i<num;++i)
    {
        if(process[i].pid == pid)
            index = i;
    }
    return index;
}


int main(){

	float avg_wt=0.0, avg_tat=0.0;

    ifstream file("Q4.txt");
    string str;
    int num, count=0, burst_time=0, min, TIME_Q1, TIME_Q2;
    file >> num;
    int remain = num;
    int wait_time=0,turnaround_time=0;

    // create process array
    Process* process = new Process[num];

    // load arrival, burst, time quantum
    while(!file.eof()){
		for(int i=0;i<num;++i)
    	{ 
            //cout << file.peek() << "\n";
    		file >> str;
            process[i].pid = i;
    		process[i].arrival_time = atoi(str.c_str());
    		//cout << "process " << i <<" arrival time: " << process[i].arrival_time << "\n";
    	}
		for(int i=0;i<num;++i)
    		{
                //cout << file.peek() << "\n";
    			file >> str;
    			process[i].burst_time = atoi(str.c_str());
                process[i].remain_time = atoi(str.c_str());
                //cout << "process " << i <<" burst time: " << process[i].burst_time << "\n";
    		}

    	file >> str;
        TIME_Q1 = atoi(str.c_str());
    	file >> str;
        TIME_Q2 = atoi(str.c_str());
    	//cout << "time quantum 1 : " << TIME_Q1 << "\n";
    	//cout << "time quantum 2 : " << TIME_Q2 << "\n";
    }

    // Sorting According to Arrival Time, and burst time
    for(int i=0;i<num;i++)
    {
        for(int j=0;j<num;j++)
        {
            if (process[i].arrival_time < process[j].arrival_time)
            {
                swap_Process(process,i,j);
            }
            else if (process[i].arrival_time == process[j].arrival_time)        // deal with same arrival time problem
            {
                if (process[i].burst_time < process[j].burst_time)
                {
                    swap_Process(process,i,j);
                }
            }
        }
    }

    deque<int> Q1, Q2, Q3;
    /*for (int i=0;i<num;++i) // put all process into Q1
    {
        Q1.push_back(process[i].pid);
    }*/

    // initial condition
    Q1.push_back(process[0].pid);
    
    //cout << "hello" << "\n";

    int last_time=-1;
    int current = 0;
    for (int time = 0 ; remain != 0 ;)
    {
        //cout << "time: " << time << "\n";
        for (int i=current+1;i<num;++i)
        {
            if((process[i].arrival_time <= time) && (process[i].arrival_time > last_time))
            {
                Q1.push_back(process[i].pid);
                //cout << "Q1 insert pid : " << process[i].pid << "\n";
            }

        }

        last_time = time;

        if (!Q1.empty())
        {
            int pid = Q1.front();
            Q1.pop_front();
            int index = find_index(process,pid,num);
            current = index;

            //cout <<"Q1 deal with process :" << pid << "\n" ;

            if(process[index].remain_time <= TIME_Q1 && process[index].remain_time>0) 
                { 
                    time+=process[index].remain_time; 
                    process[index].remain_time = 0;
                } 
            else if(process[index].remain_time > 0)
            {
                process[index].remain_time -= TIME_Q1; 
                time += TIME_Q1;
                Q2.push_back(process[index].pid);
                //cout << "Q2 insert pid : " << process[index].pid << "\n";
            }
    
            if (process[index].remain_time == 0)
            {
                remain--;
                // calculate turnaround time
                process[index].turnaround_time = time-process[index].arrival_time;
                // calculate waiting time
                process[index].waiting_time = process[index].turnaround_time-process[index].burst_time;
                //printf("P[%d]\t|\t%d\t|\t%d\n",index+1,time-at[index],time-at[index]-bt[index]); 
                wait_time += process[index].waiting_time;
                turnaround_time += process[index].turnaround_time; 
            }
        }

        else if (Q1.empty() && !Q2.empty())
        {
            int pid = Q2.front();
            Q2.pop_front();
            int index = find_index(process,pid,num);

            //cout <<"Q2 deal with process :" << pid << "\n" ;

            if(process[index].remain_time <= TIME_Q2 && process[index].remain_time>0) 
                { 
                    time+=process[index].remain_time; 
                    process[index].remain_time = 0;
                } 
            else if(process[index].remain_time > 0)
            {
                process[index].remain_time -= TIME_Q2; 
                time += TIME_Q2;
                Q3.push_back(process[index].pid);
                //cout << "Q3 insert pid : " << process[index].pid << "\n";
            }
    
            if (process[index].remain_time == 0)
            {
                remain--;
                // calculate turnaround time
                process[index].turnaround_time = time-process[index].arrival_time;
                // calculate waiting time
                process[index].waiting_time = process[index].turnaround_time-process[index].burst_time;
                //printf("P[%d]\t|\t%d\t|\t%d\n",index+1,time-at[index],time-at[index]-bt[index]); 
                wait_time += process[index].waiting_time;
                turnaround_time += process[index].turnaround_time; 
            }
        }

        else if (Q1.empty() && Q2.empty() && !Q3.empty())
        {
            int target = 0;
            int min = 9999;
            int position = 0;
            for (int i = 0; i < Q3.size(); ++i)
            {
                int idx = find_index(process,i,num);
                if (process[idx].burst_time < min)
                {
                    min = process[idx].burst_time;
                    target = idx;
                    position = i;
                }
            }

            Q3.erase(Q3.begin()+position);

            //cout <<"Q3 deal with process :" << process[target].pid << "\n" ;

            time += process[target].remain_time;
            process[target].remain_time = 0;

            remain--;
            // calculate turnaround time
            process[target].turnaround_time = time-process[target].arrival_time;
            // calculate waiting time
            process[target].waiting_time = process[target].turnaround_time-process[target].burst_time;
            //printf("P[%d]\t|\t%d\t|\t%d\n",index+1,time-at[index],time-at[index]-bt[index]); 
            wait_time += process[target].waiting_time;
            turnaround_time += process[target].turnaround_time;

        }

    }

    cout<<"Process\tWaiting Time\tTurnaround Time";

    for(int i=0;i<num;++i)
        cout<<"\nP["<<i+1<<"]"<<"\t"<<process[i].waiting_time<<"\t\t"<<process[i].turnaround_time;

    cout<<"\n\nAverage waiting time : "<<wait_time*1.0/num;
    cout<<"\nAverage turnaround time : "<<turnaround_time*1.0/num<<"\n";

    // delete process array
    delete[] process;

	return 0;
}

