#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

//structure to read a csv file
struct csvreader
{
	vector <vector <string> > data;							//data entry
	
	vector <string> heading;								//heading for data

	void read(const string& filename, bool header = true)	//automaticall assume heading is there
	{
		heading.clear();							//empty initial header
		data.clear();								//empty the initial data

		vector <string> line_data;					//data per line entry
		string line;								//unparsed row
		string cell;								//data inside each cell
		ifstream file;								//file to extract data from
		file.open(filename);						

		if (file.is_open())
		{
			int l_index = 0;						//line index

			while (getline(file, line))
			{
				line_data.clear();							//empty the data found in the line
															//after each iteration

				line = line + ",";							//in the case that the last cell in the line
															//is empty, I append a comma to the line. 

				istringstream iss(line);					//string stream for line data

				if (header == true && l_index == 0)			//gathering heading data if there
				{
					int cell_num = 0;
					while (getline(iss, cell, ','))
					{
						if (cell.empty()) 							//if the cell is empty,
							heading.push_back(to_string(cell_num));	//append a space as a header.

						else
						{
							int cell_index = 0;
							while (cell[cell_index] == ' ')			//trim leading whitespace 
							{
								cell_index++;
								cell = cell.substr(cell_index, cell.length());
							}
							cell_index = cell.size()-1;
							while (cell[cell_index] == ' ')			//trim ending whitespace
							{
								cell = cell.substr(0, cell_index);
								cell_index--;
							}
							heading.push_back(cell);				//append the cell name
						}
						cell_num++;									//increase cell number with each iteration
					}
					data.resize(heading.size());			//number of columns is same 
															//as number of columns of header
				}
				else
				{
					while (getline(iss, cell, ','))			//parsing data from each row
					{
						if (cell.empty())					//check if the cell data is empty
							line_data.push_back("NA");		//if it is, consider it NULL
						else
						{
							int cell_index = 0;					//trim leading whitespace
							while (cell[cell_index] == ' ')
							{
								cell_index++;
								cell = cell.substr(cell_index, cell.length());
							}
							cell_index = cell.size() - 1;
							while (cell[cell_index] == ' ')		//trim ending whitespace
							{
								cell = cell.substr(0, cell_index);
								cell_index--;
							}

							line_data.push_back(cell);		//if it is not, append the cell
						}
					}

					if (l_index == 0 && header == false)			//resize columns appropriately
						data.resize(line_data.size());				//from first row

					for (int column = 0; column < data.size(); column++)
					{
						if (line_data[column] != "NA")					//if the cell was empty
							data[column].push_back(line_data[column]);	//append line data to
																		//rest of data
					}
				}
				l_index++;		//moving to the next line
			}
			file.close();
		}
	}
};