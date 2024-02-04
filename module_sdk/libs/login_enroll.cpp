#include <stdio.h>
#include <iostream>
#include <string.h>
#include "login_enroll.h"
#include "../../rbslib/FileIO.h"
#include "../../rbslib/Buffer.h"
#include "../../rbslib/Storage.h"
using namespace std;

void Login_Enroll::Login()
{
	cout << "请输入账号与密码" << endl;
	std::string user_account, user_password;
	cin >> user_account;
	cout << endl;
	cin >> user_password;
}

void Login_Enroll::Change_Password()
{
	std::string user_password1, user_passerword2;
	while (1)
	{
		cout << "请输入新密码" << endl;
		cin >> user_password1;
		cout << endl;
		cout << "请再次确认你的密码" << endl;
		cin >> user_password2;
		if (!strcmp(user_password1, user_passerword2))  break;
		else cout << "请重新输入密码";
	}
}

