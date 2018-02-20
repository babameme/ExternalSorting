/* Filename:  main.cpp

   Programmer:  Bao Quy Bui

   Date:  Jan 25, 2018
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <string>
#include <vector>
#include <queue>
using namespace std;
int run_index = 0;
long long sizeOfData, sizeSmallFile = 0, sizeOfRam, numberOfLine = 0;
string inputFileName, outputFileName, line, fileName;
vector<string> smallFileName, data;
ofstream outputSmallFile;

// Report Error and exit to OS
void Error(const string & MsgA, const string & MsgB){
   cerr << "Error: " << MsgA << MsgB << endl;
   // If error, return error code 1 to Operating System
   exit(1);
}

// Class compare which is used in minimum Heap
class Compare
{
public:
    //Ascending order sort
    bool operator() (pair<string, int> pair1, pair<string, int> pair2)
    {
        return pair1.first > pair2.first;
    }
};

// Input file and size of RAM
void inputData(){
	// Input input file and output file
	cout<<"Enter input file"<<endl;
	getline(cin, inputFileName);

	cout<<"Enter output file"<<endl;
	getline(cin, outputFileName);

	// Input RAM size is available to use
	cout<<"Enter RAM size (In bytes)"<<endl;
	cout<<"Please enter the maximum size of RAM which is available"<<endl;
	cin>>sizeOfRam;	
}

// Convert int to String
string intToString(int k){
	std::stringstream ss;
	ss<<k;
	return ss.str();
}

// Merge small file to only one output file
void mergeFiles(){
	//cout<<"Number of runs "<<run_index<<endl;
	ifstream smallFile[run_index];
	priority_queue<pair<string, int>, vector<pair<string, int>>, Compare> minHeap;

	// Push first line of each small file into min Heap
	for (int i = 0; i < run_index; i++){
		fileName = smallFileName[i];
		//cout<<"File name : "<<fileName<<endl;
		smallFile[i].open(fileName.c_str(), ifstream::in | ifstream::binary);
		if (getline(smallFile[i], line)){
			minHeap.push(pair<string, int> (line, i));
			//cout<<"Get : "<<line<<endl;
		}		
	}

	// While heap is not empty, get top and push next line in the same file into min Heap
	ofstream outputFile(outputFileName);
	while (minHeap.size()){
		pair<string, int> top = minHeap.top();
		minHeap.pop();
		outputFile<<top.first;
		numberOfLine--;
		if (numberOfLine){
			outputFile<<endl;
		}
		//cout<<"Out : "<<top.first<<endl;
		flush(outputFile);
		if (getline(smallFile[top.second],line)){
			minHeap.push(pair<string,int> (line, top.second));
		}
	}
	for (int i = 0; i < run_index; i++){
		smallFile[i].close();
		remove(smallFileName[i].c_str());
	}
	outputFile.close();
	ifstream outF(outputFileName.c_str(), ifstream::in | ifstream::binary);
	outF.seekg(0, outF.end);
	cout<<"Size of output file is : "<<(long long) outF.tellg()<<endl;
	outF.seekg(0, outF.beg);
	outF.close();
}

int main(){
	inputData();
	ifstream inputFile(inputFileName.c_str(),  ifstream::in | ifstream::binary);
	if (!inputFile.is_open()){
		Error("Could not open the file named ", inputFileName);
	}

	// Get size of Data
	inputFile.seekg(0, inputFile.end);
	sizeOfData = (long long)inputFile.tellg();
	inputFile.seekg(0, inputFile.beg);
	//numberOfChunks = sizeOfData / sizeOfRam + (sizeOfData % sizeOfRam == 0 ? 0 : 1);
	cout<<"--------------------------------------"<<endl;
	cout<<"Data size : "<<sizeOfData<<" bytes"<<endl;
	cout<<"RAM size  : "<<sizeOfRam<<" bytes"<<endl;
	//cout<<"Number of chunks : "<<numberOfChunks<<endl;

	// Begin Time:	
	const clock_t begin_time = clock();

	// Begin reading input file
	cout<<"--------------------------------------"<<endl;
	cout<<"Reading input file and divide into smaller file"<<endl;
	cout<<"File name "<<inputFileName<<" is being readed"<<endl;
	int start = 1;
	while (getline(inputFile, line)){
		numberOfLine++;
		//cout<<zz<<" :: size "<<line.size()<<" --"<<line<<endl;
		if (sizeSmallFile + line.size() + 1 - start <= sizeOfRam){
			data.push_back(line);
			// Plus one '\n'
			sizeSmallFile += line.size() + 1 - start;
			start = 0;
		}
		else{
			sort(data.begin(), data.end());
			fileName = "smallFile_" + intToString(run_index) + ".txt";
			cout<<"Create file "<<fileName<<" with size "<<sizeSmallFile<<" bytes"<<endl;
			smallFileName.push_back(fileName);
			outputSmallFile.open(fileName.c_str());
			// Print data to small file
			for (int i = 0; i < (data.size() - 1); i++){
				outputSmallFile<<data[i]<<endl;
			}
			outputSmallFile<<data[data.size() - 1];
			outputSmallFile.close();
			data.clear();
			run_index++;
			data.push_back(line);
			sizeSmallFile = line.size();
		}
	}
	if (data.size() > 0){
		sort(data.begin(), data.end());
		fileName = "smallFile_" + intToString(run_index) + ".txt";
		cout<<"Create file "<<fileName<<" with size "<<sizeSmallFile<<" bytes"<<endl;
		smallFileName.push_back(fileName);
		outputSmallFile.open(fileName.c_str());
		// Print data to small file
		for (int i = 0; i < (data.size() - 1); i++){
			outputSmallFile<<data[i]<<endl;
		}
		outputSmallFile<<data[data.size() - 1];
		outputSmallFile.close();
		data.clear();
		run_index++;				
		sizeSmallFile = 0;
	}
	// Close input file
	inputFile.close();
	cout<<"Finish divide and sort small files"<<endl;
	cout<<"--------------------------------------"<<endl;
	cout<<"Merge small files using k-way merge"<<endl;
	mergeFiles();
	cout<<"Finish sort, result is saved in "<<outputFileName<<endl;
	cout<<"--------------------------------------"<<endl;
	cout << "Entire process took a total of: " << float(clock() - begin_time) / CLOCKS_PER_SEC * 1000 << " msec." << endl;
}
