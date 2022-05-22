#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <bitset>

using namespace std;

struct Entry
{
	int his = 0b000;
	int misprediction = 0;
	vector<int> history;
	vector<string> state;
};

struct Inst
{
	int myEntry = 0;
	string inst = "";
};

int main()
{
	cout << "Please input entry(entry > 0):\n";
	int entry_num = 0;
	cin >> entry_num;
	vector<Entry> predictor(entry_num);

	for (int i = 0; i < entry_num; i++)
	{
		for (int j = 0; j < 3; j++) predictor[i].history.push_back(0);
		for (int j = 0; j < 8; j++) predictor[i].state.push_back("SN");
	}

	ifstream ifs;
	ifs.open("test.txt");

	int PC = 0x0000;
	int theEntry = 0;
	string buffer = "";
	map<int, Inst> instructions;

	while (getline(ifs, buffer))
	{
		if (buffer.find(':') == string::npos)
		{
			buffer.erase(0, buffer.find_first_not_of(" "));
			instructions[PC].inst = buffer;
			instructions[PC].myEntry = theEntry;
			PC += 0x0004;
			theEntry = (theEntry + 1) % entry_num;
		}
	}

	ifs.close();

	PC = 0x0000;

	int last3 = 0;
	int last2 = 0;
	int last1 = 0;

	int R0 = 0;
	int R1 = 0;
	int R2 = 0;

	int Prediction = 0;
	int Outcome = 0;

	int temp = 0;
	int End = 0;

	while (true)
	{
		last3 = predictor[instructions[PC].myEntry].history[predictor[instructions[PC].myEntry].history.size() - 3];
		last2 = predictor[instructions[PC].myEntry].history[predictor[instructions[PC].myEntry].history.size() - 2];
		last1 = predictor[instructions[PC].myEntry].history[predictor[instructions[PC].myEntry].history.size() - 1];
		predictor[instructions[PC].myEntry].his = last3 * 4 + last2 * 2 + last1;

		cout << "Entry: " << instructions[PC].myEntry << "                    " << instructions[PC].inst << "\n";
		cout << "(" << bitset<3>(predictor[instructions[PC].myEntry].his);
		for (int i = 0; i < 8; i++) cout << "," << predictor[instructions[PC].myEntry].state[i];
		cout << ") ";

		if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "SN")
		{
			Prediction = 0;
			cout << "N ";
		}
		else if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "WN")
		{
			Prediction = 0;
			cout << "N ";
		}
		else if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "WT")
		{
			Prediction = 1;
			cout << "T ";
		}
		else if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "ST")
		{
			Prediction = 1;
			cout << "T ";
		}

		switch (PC)
		{
		case 0x0000:
			R1 = R0 + 0;
			Outcome = 0;
			cout << "N   ";
			break;
		case 0x0004:
			R2 = R0 + 4;
			Outcome = 0;
			cout << "N   ";
			break;
		case 0x0008:
			if (R1 == R2)
			{
				End = 1;
				Outcome = 1;
				cout << "T   ";
			}
			else
			{
				Outcome = 0;
				cout << "N   ";
			}
			break;
		case 0x000C:
			R2 = R2 - 1;
			Outcome = 0;
			cout << "N   ";
			break;
		case 0x0010:
			if (R0 == R0)
			{
				temp = PC - 0x000C;
				Outcome = 1;
				cout << "T   ";
			}
			break;
		default:
			break;
		}

		if (Prediction != Outcome)
		{
			if (Outcome == 0)
			{
				if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "WN")
				{
					predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] = "SN";
				}
				else if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "WT")
				{
					predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] = "WN";
				}
				else if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "ST")
				{
					predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] = "WT";
				}
			}
			else
			{
				if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "SN")
				{
					predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] = "WN";
				}
				else if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "WN")
				{
					predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] = "WT";
				}
				else if (predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] == "WT")
				{
					predictor[instructions[PC].myEntry].state[predictor[instructions[PC].myEntry].his] = "ST";
				}
			}

			predictor[instructions[PC].myEntry].misprediction++;
		}

		cout << "misprediction: " << predictor[instructions[PC].myEntry].misprediction << "\n";

		predictor[instructions[PC].myEntry].history.push_back(Outcome);

		if (PC == 0x0010)
		{
			PC = temp;
		}

		if (End)
		{
			break;
		}
		else
		{
			cout << "\n";
		}

		PC += 0x0004;
	}

	return 0;
}