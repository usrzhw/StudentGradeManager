#include <stdio.h>
#include <cJSON.h>
#include <iostream>
using namespace std;

void CreatStudentTable()
{
	cJSON* TCP = cJSON_CreateObject();//创建对象
	cJSON_AddnumberToObject(TCP, "student_id", student_id);//学生id
	cJSON_AddStringToObject(TCP, "name", name);//学生姓名
	cJSON* SUB = cJSON_CreateArray();//创建一个数组专门存储课程
	for (int i = 0; i < 20; i++)
	{
		if (subject[i] != 0)
		{
			cJSON_AddStringToObject(SUB, "", subject[i]);
		}
	}
	cJSON_AddItemToObject(TCP, "subject", SUB);
}