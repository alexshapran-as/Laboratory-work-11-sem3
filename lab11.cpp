#include "solver.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <boost/filesystem.hpp>
#include <sys\stat.h>
#include <cstdio>
#include <map>
#include <future>

using namespace std;
vector<map<int, int>> for_results;
const string basepath = "C:\\Users\\й\\Desktop\\Образование\\МГТУ_Баумана_обучение\\Алгоритмические_языки\\3_Семестр\\Лабы\\lab11\\lab11\\F1";
const string destpath = "C:\\Users\\й\\Desktop\\Образование\\МГТУ_Баумана_обучение\\Алгоритмические_языки\\3_Семестр\\Лабы\\lab11\\lab11\\F3";

int main(void)
{
	initialise();
	std::vector<std::pair<int, int>> numbers;
	ifstream fin;
	fin.open("F2\\F2.txt");
	// Получение задач из файла
	if (!fin.is_open())
	{
		cout << endl << "File wasn't opened!" << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	while (!fin.eof())
	{
		int temp1 = { 0 };
		int temp2 = { 0 };
		fin >> temp1;
		fin.get();
		fin >> temp2;
		numbers.push_back(std::make_pair(temp1, temp2));
	}
	fin.close();
	cout << endl << "Count of tasks: " << numbers.size() << endl;
	// Копирование файла
	for (const boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(basepath))
	{
		try
		{
			boost::filesystem::copy(x.path(), destpath / x.path().filename()/*, boost::filesystem::copy_options::recursive*/);
		}
		catch (boost::filesystem::filesystem_error& e)
		{
			std::cout << e.what() << std::endl << "There is a problem with the file: "
				<< destpath / x.path().filename() << std::endl;
		}
	}
	rename("F3\\F1.txt", "F3\\F3.txt");
	// Вывод задач на экран
	cout << endl << "Tasks from file F2: " << endl;
	for (auto it = numbers.begin(); it != numbers.end(); ++it)
	{
		cout << it->first << " + " << it->second << endl;
	}

	shared_future<int> id;
	cout << endl << "Results of tasks in file F2:" << endl;
	unsigned int step = { 0 };
	unsigned int step2 = { 0 };
	int rest = { 0 };
	bool is_prime = { false };
	bool step2_1 = { false };
	// Выбор шага
	if (numbers.size() % 4 == 0)
	{
		step = { 4 };
	}
	else if (numbers.size() % 3 == 0)
	{
		step = { 3 };
	}
	else if (numbers.size() % 2 == 0)
	{
		step = { 2 };
	}
	else if (numbers.size() > 1)
	{
		step = { 4 };
		step2 = numbers.size() % step;
		rest = numbers.size() % step2;
		is_prime = { true };
		if (step2 == 1)
			step2_1 = { true };
	}
	else
	{
		step = { 1 };
		ofstream fout;
		fout.open("F3\\F3.txt", ios_base::in);
		shared_future<int> result = async(launch::async, solveOne, (numbers.begin())->first, (numbers.begin())->second);
		cout << "result " << result.get() << endl;
		fout << result.get();
		fout << "\r";
		fout.close();
		system("pause");
		exit(EXIT_SUCCESS);
	}

	auto it = numbers.begin();
	int size = numbers.size();
	bool checked = { false };
	for (; it != numbers.end() && size > 1; it += step, size -= step)
	{
		static unsigned int i = { 0 };
		try
		{
			unsigned int j = { 0 };
			std::cout << endl << "Task Bunch " << ++i << endl;
			std::vector<std::pair<int, int>> v(step);
			if (checked)
				copy(it + 1, it + step + 1, v.begin());
			else
				copy(it, it + step, v.begin());
			id = async(launch::async, solveBunch, v);
			if (rest == i && is_prime)
			{
				step = step2;
				checked = { true };
			}
			for (;;)
			{
				shared_future<map<int, int>> res = async(launch::async, getBunchResult, id.get());
				for_results.push_back(res.get());
				if ((res.get()).size())
					std::cout << "task " << ++j << " was done;" << endl;
			}
		}
		catch (...) 
		{
		};
	}
	cout << endl;
	ofstream fout;
	fout.open("F3\\F3.txt", ios_base::in);
	if (!fout.is_open())
	{
		cout << endl << "Error. File wasn't opened!" << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	for (auto it = for_results.begin(); it != for_results.end(); ++it)
	{
		for (auto it2 = (*it).begin(); it2 != (*it).end(); ++it2)
		{
			cout << "id " << it2->first << ", result " << it2->second << endl;		
			fout << it2->second << "\r\n";
		}
	}
	if (step2_1 == true)
	{
		shared_future<int> result = async(launch::async, solveOne, (--numbers.end())->first, (--numbers.end())->second);
		cout << "result " << result.get() << endl;
		fout << result.get();
		fout << "\r\n";
		fout << "\r\n";
		fout.close();
		system("pause");
		exit(EXIT_SUCCESS);
	}
	fout.close();

	system("pause");
	return 0;
}
