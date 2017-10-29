#include<stdio.h> 
#include<vector>
#include<iostream>
#include<deque>  

using namespace std;

int main() 
{ 
 
  int count,j,n,time,remain,time_quantum; 
  int wait_time=0,turnaround_time=0,at[10],bt[10],rt[10],wt[10],tt[10]; 
  //vector<int> box;
  deque<int> box;
  printf("Enter Total Process:\t "); 
  scanf("%d",&n); 
  remain=n; 
  for(count=0;count<n;count++) 
  { 
    printf("Enter Arrival Time and Burst Time for Process Process Number %d :",count+1); 
    scanf("%d",&at[count]); 
    scanf("%d",&bt[count]); 
    rt[count]=bt[count]; 
  } 
  printf("Enter Time Quantum:\t"); 
  scanf("%d",&time_quantum); 
cout<<"Process\tWaiting Time\tTurnaround Time";



  int current=0,flag=0,last_time=-1;
  box.push_back(0);
  for(time=0;remain!=0;) 
  { 
  	for(int i=current+1;i<n;++i)
  	{
  		if(at[i]<=time && at[i]>last_time)
  		{
  			//cout << "current time: " << time << "last time:"<< last_time << "\n";
  			box.push_back(i);
  			//cout << "time: " << time << " insert: "<< i << "\n";
  		}
  	}
  	if(flag==1)
  		{
  			box.push_back(current);
  			//cout << "time: " << time << " not yet finished: "<< current << "\n";
  		}
  	last_time = time;
  	if (!box.empty())
  	{
  		int index = box.front();
  		box.pop_front();
  		//cout << "index: " << index << "\n" ;
  		if(rt[index]<=time_quantum && rt[index]>0) 
  		{ 
      		time+=rt[index]; 
      		rt[index]=0;  
      		flag = 0;
    	} 
    	else if(rt[index]>0)
    	{
    		rt[index]-=time_quantum; 
      		time+=time_quantum;
      		flag = 1;
      		current = index;
    	}

    	if (rt[index]==0)
    	{
    		remain--;
    		wt[index] = time-at[index]-bt[index];
    		tt[index] = time-at[index];
    		//printf("P[%d]\t|\t%d\t|\t%d\n",index+1,time-at[index],time-at[index]-bt[index]); 
    		wait_time+=time-at[index]-bt[index];
    		turnaround_time+=time-at[index]; 
    	}
  	}
  } 
	for (int i=0;i<n;i++)
    {
    	cout<<"\nP["<<i+1<<"]"<<"\t"<<wt[i]<<"\t\t"<<tt[i];
       // cout<<"\n"<<P_id[i]<<"\t"<<P_Wt[i]<<"\t\t"<<P_Tat[i];
    }
 
    cout<<"\n\nAverage Waiting Time:"<<wait_time*(1.0)/n;
    cout<<"\nAverage Turnaround Time:"<<turnaround_time*(1.0)/n<<"\n";
  
  return 0; 
}