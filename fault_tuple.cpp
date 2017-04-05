#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <time.h>

using namespace std;

void wire_arr_expand(int*, vector<int>*);

int main(int argc, char **argv){

	//check the command
	if(argc != 3){
		cout << "Usage: ./[exe] [circuit file(.v)] [output fault-tuple file]" << endl;
		exit(1);
	}

	//check input file
	fstream fin_cir;
	fin_cir.open(argv[1],fstream::in);
	if(!fin_cir.is_open()){
		cout << "Error: the input file: Circuit file is not opened!!" << endl;
		exit(1);
	}

	//check output file
	fstream fout_ft;
	fout_ft.open(argv[2],fstream::out);
	if(!fout_ft.is_open()){
		cout << "Error: the output file: Fault tuple file is not opened!!" << endl;
		exit(1);
	}
	
	//parse the input file
	cout << "====================" << endl;
	cout << "       Start!       " << endl;
	cout << "====================" << endl;

	string buffer;	//fin buffer

	int wire_count = 0;
	vector<int> wire_arr;

	int n_extract = 30;	// extracr %
	srand(time(NULL));
	double n_ac_extract = rand() % n_extract + 1;	//actally extract %
	double n_ac_extract_buffer;

	while(!fin_cir.eof()){
		cout << "fin_cir.eof() before " << fin_cir.eof() << endl;
		//read next row
		getline(fin_cir, buffer);
		string::iterator it = buffer.begin();
		
		cout << "read next row finish " << endl;		

		//front erase space
		size_t found = buffer.find_first_not_of(' ');
		buffer.erase(0,found);

		cout << "front erase space finish " << endl;

		//pick out repeat wire
		if(*it == '/')		// //
			continue;
		else if(*it == 'm')	// module
			continue;
		else if(*it == 'd')	// dff
			continue;
		else if(*it == 'n')	// not nand nor
			continue;
		else if(*it == 'a')	// and
			continue;
		else if(*it == 'o'){	// or output
			string::iterator it_next = it + 1;	//check next char or or output
			if(*it_next == 'r')	// or
				continue;
		}
		else if(*it == 'e'){	// end
			cout << "end " << endl;
			break;	
		}	

		cout << "pick out repeat wire finish " << endl;

		//erase back space
		found = buffer.find_last_not_of(' ');
		cout << "buffer " << buffer << endl;
		buffer.erase(found,buffer.size());
		cout << "buffer " << buffer << endl;

		cout << "erase back space finish" << endl;

		// input output wire
		//erease "input output wire"
		if(*it == 'w' || *it == 'i' || *it == 'o'){
			found = buffer.find_first_of(' ');
			char buffer_show[10] = {0};	//fout buffer
			buffer.copy(buffer_show,found,0);
			buffer.erase(0,found);
			found = buffer.find_first_not_of(' ');
			buffer.erase(0,found);
		}

		cout << "erease input output wire " << endl;
		
		while(!buffer.empty()){
			//distinguish gate from buffer
			found = buffer.find_first_of(",;");
			char buffer_show2[found];
			buffer.copy(buffer_show2,found,0);
			string buffer_show3 = buffer_show2;
			if(buffer_show2[0] == 'G'|| buffer_show2[0] == 'g'){
				if(buffer_show2[1] != 'N' && buffer_show2[1] != 'n'){	//not gnd GND
					cout << "buffer_show3 " << buffer_show3 << endl;
					buffer_show3.erase(0,1);
					cout << "buffer_show3 " << buffer_show3 << endl;
					char *c_buffer_show3 = new char [buffer_show3.length()+1];	//change char to int
					strcpy (c_buffer_show3, buffer_show3.c_str());
					cout << "buffer_show3 " << buffer_show3 << endl;
					cout << "c_buffer_show3 "<< c_buffer_show3 << endl;
					int n = atoi(c_buffer_show3);
					cout << "n "<< n << endl;
					cout << "wire_arr.size()"<< wire_arr.size();
					cout << endl;
					if(n >= wire_arr.size())	//if wire_arr size too small 
						wire_arr_expand(&n,&wire_arr);	//expand wire_arr
	
					if(wire_arr[n] == 0)	//count number of gate
						wire_count = wire_count + 1;
					wire_arr[n] = wire_arr[n] + 1 ;	// >0 means the gate exist
				}
			};
			buffer.erase(0,found+1);
			n_ac_extract_buffer = rand() % n_extract + 1;	//actally extract %
			if(n_ac_extract_buffer > n_ac_extract )
				n_ac_extract = n_ac_extract_buffer;  
			cout << "n_ac_extract: " << n_ac_extract << endl;
		}

		// extract gate
	}

		cout << "wire_count: " << wire_count << endl;
		int n_g_extract = wire_count * (n_ac_extract/100);
		cout << "n_g_extract: " << n_g_extract << endl;
		cout << "fin_cir.eof() after " << fin_cir.eof() << endl; 					
		cout << "fin_cir.eof() " << fin_cir.eof() << endl;
/*
	for(int i=0; i< wire_arr.size(); i++){
		cout << i << " " << wire_arr[i] <<endl;
	}
*/
	int counter = 0;
	for(int i=n_g_extract; i>0; i--){
		int g1 = rand()%wire_arr.size() + 1 ;
		if(wire_arr[g1] != 0 && wire_arr[g1] < 4){
			int g2 = rand()%wire_arr.size() + 1;
				if(wire_arr[g2] != 0 && wire_arr[g2] <4){
					cout << "G"<< g1 << " G" << g2 << endl;
					fout_ft << "G"<< g1 << " G" << g2 << endl;
					wire_arr[g1]++;
					wire_arr[g2]++;
					counter++;
				}
		}
	}
	
	cout << "#wire count: " << wire_count << endl;
	fout_ft << "#wire count: " << wire_count << endl;
	cout << "#coupling wire: " << counter << endl;
	fout_ft << "#coupling wire: " << counter << endl;
	double percent = (double)counter/(double)wire_count;
	cout << "percentage: " << percent*100 << endl;
	fout_ft << "percentage: " << percent*100 << endl;

}

//expand wire_arr
void wire_arr_expand(int* i, vector<int>* wire_arr){
	while((*wire_arr).size() != ((*i)+1))
		(*wire_arr).push_back(0);
}
