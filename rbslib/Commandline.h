#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <list>
#include <functional>

namespace RbsLib::Command
{
	class CommandLine
	{
	private:
		std::vector<std::string> args;
	public:
		bool Parse(const std::string& command);
		CommandLine() = default;
		CommandLine(int argc, const char** argv);
		bool IsFlagExist(const std::string& flag);
		std::string GetNextToken(const std::string& flag) const;
		std::string operator[](int index) const;
		std::string operator[](const std::string& flag) const;
		int GetSize() const;
		void Clear();
		void Push(const std::string& arg);
	};

	class CommandExecuter
	{
	public:
		class CommandNode
		{
		private:
			std::map<std::string, std::pair<bool, CommandNode*>> children;
			std::function<void(const std::map<std::string, std::string>&)> func;
			std::list<CommandNode*>* nodes;
		public:
			auto operator[](const std::string& str)->CommandNode&;
			void operator()(const std::map<std::string, std::string>& args);
			void SetFunction(const std::function<void(const std::map<std::string, std::string>&)>& func);
			auto CreateOption(const std::string& option, bool have_extra_arg) -> CommandNode&;
			CommandNode(std::list<CommandNode*>* nodes);
			bool HaveOption(const std::string& option);
		};
		auto operator[](const std::string& str)->CommandNode&;
		CommandExecuter(const std::string& root_name, bool have_extra_option);
		CommandExecuter(const CommandExecuter&) = delete;
		~CommandExecuter();
		void Execute(int argc,const char**argv);
	private:
		std::map<std::string, std::string> args;
		std::list<CommandNode*> nodes;
		CommandNode root;
	};
}
