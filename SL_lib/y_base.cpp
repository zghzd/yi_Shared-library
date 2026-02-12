#include "pch.h"
#include "y_base.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <ranges>
#define NOMINMAX
#include <windows.h>

namespace base {
	namespace fio {
		int file_write_new(std::string fpath, std::string str) {
			std::ofstream outputFile;
			outputFile.open(fpath);
			if (outputFile.is_open()) {
				outputFile << str;
				outputFile.close();
				return 0;
			}
			else {
				return -1;
			}
		}
		int file_write_c(std::string fpath, std::string str) {

			std::ofstream outputFile;
			outputFile.open(fpath, std::ios_base::app);
			if (outputFile.is_open()) {

				outputFile << str;
				outputFile.close();
				return 0;
			}
			else {
				return -1;
			}

		}
		std::string file_read_all(std::string fpath) {
			std::ifstream f(fpath.c_str());
			std::ifstream fin(fpath);
			if (f.good()) {
				goto file_good;
			}
			else {
				return "";
			}
		file_good:
			if (fin.is_open()) {
				std::stringstream buffer;
				buffer << fin.rdbuf();
				std::string str(buffer.str());
				fin.close();
				return str;
			}
			else
			{
				fin.close();
				return "";
			}

		}
		std::string file_read_line(std::string fpath, int line_num) {
			std::ifstream file(fpath);
			if (!file.is_open()) {
				return "";
			}
			std::vector<std::string> lines;
			std::string line;
			while (std::getline(file, line)) {
				lines.push_back(line);
			}
			file.close();
			int total_lines = static_cast<int>(lines.size());
			int actual_index;
			if (line_num >= 0) {
				actual_index = line_num;
			}
			else {
				actual_index = total_lines + line_num;
			}
			if (actual_index >= 0 && actual_index < total_lines) {
				return lines[actual_index];
			}
			else {
				return "";
			}
		}
		std::vector<std::string>file_read_lines(std::string fpath) {
			std::vector<std::string> lines;
			std::ifstream file(fpath);
			if (!file.is_open()) {
				return lines;
			}
			std::string line;
			while (std::getline(file, line)) {
				lines.push_back(line);
			}
			file.close();
			return lines;
		}
	}
	namespace data_process {
		std::vector<std::string>getAllFiles(std::string root_path) {
			std::vector<std::string> files;
			for (const auto& entry : std::filesystem::recursive_directory_iterator(root_path)) {
				if (entry.is_regular_file()) {
					files.push_back(entry.path().string());
				}
			}
			return files;
		}
		bool is_dir_has_file(std::string root_path) {
			try {
				if (!std::filesystem::exists(root_path)) {
					std::cout << "路径不存在: " << root_path << std::endl;
					return false;
				}
				if (!std::filesystem::is_directory(root_path)) {
					std::cout << "路径不是目录: " << root_path << std::endl;
					return false;
				}
				if (std::filesystem::is_empty(root_path)) {
					return false;
				}
				return true;
			}
			catch (const std::filesystem::filesystem_error& ex) {
				std::cout << "文件系统错误: " << ex.what() << std::endl;
				return false;
			}
			catch (const std::exception& ex) {
				std::cout << "错误: " << ex.what() << std::endl;
				return false;
			}
		}
		std::vector<std::string>part_str(std::string str, std::string part_by) {
			std::vector<std::string> result;
			size_t start = 0;
			size_t end = str.find(part_by);
			while (end != std::string::npos) {
				result.push_back(str.substr(start, end - start));
				start = end + part_by.length();
				end = str.find(part_by, start);
			}
			result.push_back(str.substr(start));

			return result;
		}
		std::vector<std::string>part_str_once(std::string str, std::string part_by) {
			std::vector<std::string> result;
			size_t pos = str.find(part_by);
			if (pos != std::string::npos) {
				result.push_back(str.substr(0, pos));
				result.push_back(str.substr(pos + part_by.length()));
			}
			else {
				result.push_back(str);
				result.push_back("");
			}
			return result;
		}
		void addIfNotExists_invector(std::vector<std::string>& vec, const std::string& str) {
			if (std::ranges::find(vec, str) == vec.end()) {
				vec.emplace_back(str);
			}
		}
	}
	namespace time {
		std::string utc_p0800() {
			auto now = std::chrono::system_clock::now();
			auto shanghai_time = std::chrono::zoned_time{ "Asia/Shanghai", now };
			return std::format("{:%Y-%m-%dT%H:%M:%S%z}", shanghai_time);
		}
		std::string date() {
			SYSTEMTIME time_now;
			GetLocalTime(&time_now);
			std::string now = (std::to_string(time_now.wYear)
				+ "-" + std::to_string(time_now.wMonth)
				+ "-" + std::to_string(time_now.wDay)
				+ "T"
				+ std::to_string(time_now.wHour)
				+ ":" + std::to_string(time_now.wMinute)
				+ ":" + std::to_string(time_now.wSecond)
				+ "." + std::to_string(time_now.wMilliseconds));
			std::string short_now = (std::to_string(time_now.wYear)
				+ "-" + std::to_string(time_now.wMonth)
				+ "-" + std::to_string(time_now.wDay));
			return short_now;
		}
	}
	namespace logger {
		std::string E(std::string msg, std::string logpath) {
			std::string log = "[" + base::time::utc_p0800() + "]" + "E" + ":" + msg + "\n";
			std::filesystem::path log_file_path = std::filesystem::current_path() / logpath / std::filesystem::path(base::time::date() + ".log");
			base::fio::file_write_c(log_file_path.string(), log);
			std::cout << log << std::endl;
			return log;
		}
		std::string W(std::string msg, std::string logpath) {
			std::string log = "[" + base::time::utc_p0800() + "]" + "W" + ":" + msg + "\n";
			std::filesystem::path log_file_path = std::filesystem::current_path() / logpath / std::filesystem::path(base::time::date() + ".log");
			base::fio::file_write_c(log_file_path.string(), log);
			std::cout << log << std::endl;
			return log;
		}
		std::string D(std::string msg, std::string logpath) {
			std::string log = "[" + base::time::utc_p0800() + "]" + "D" + ":" + msg + "\n";
			std::filesystem::path log_file_path = std::filesystem::current_path() / logpath / std::filesystem::path(base::time::date() + ".log");
			base::fio::file_write_c(log_file_path.string(), log);
			std::cout << log << std::endl;
			return log;
		}
		std::string I(std::string msg, std::string logpath) {
			std::string log = "[" + base::time::utc_p0800() + "]" + "I" + ":" + msg + "\n";
			std::filesystem::path log_file_path = std::filesystem::current_path() / logpath / std::filesystem::path(base::time::date() + ".log");
			base::fio::file_write_c(log_file_path.string(), log);
			std::cout << log << std::endl;
			return log;
		}
	}
	namespace fio {
		std::vector<std::string> filterFilesByExtension(
			const std::vector<std::string>& files,
			const std::string& fileextname) {

			std::vector<std::string> result;

			for (const auto& file : files) {
				std::filesystem::path filePath(file);
				std::string extension = filePath.extension().string();
				if (!extension.empty() && extension[0] == '.') {
					extension = extension.substr(1);
				}
				if (extension == fileextname) {
					result.push_back(file);
				}
			}

			return result;
		}

		std::string getFileExtension(const std::string& file) {
			std::filesystem::path filePath(file);
			std::string extension = filePath.extension().string();
			if (extension.empty() && filePath.filename().string()[0] == '.') {
				std::string filename = filePath.filename().string();
				if (filename.length() > 1) {
					return filename.substr(1);
				}
			}

			if (!extension.empty() && extension[0] == '.') {
				return extension.substr(1);
			}

			return extension;
		}

		std::string transPathToDot(const std::string path) {
			std::string result = path;
			for (size_t i = 0; i < result.length(); ++i) {
				if (result[i] == '\\') {
					result[i] = '/';
				}
			}
			size_t lastSlash = result.find_last_of('/');
			for (size_t i = 0; i < result.length(); ++i) {
				if (result[i] == '/' && i != lastSlash) {
					result[i] = '.';
				}
			}
			if (lastSlash != std::string::npos) {
				result[lastSlash] = '_';
			}
			return result;
		}

		std::string removePrefix(const std::string& str, const std::string& prefix) {
			if (prefix.empty()) return str;
			if (str.length() < prefix.length()) return str;
			if (str.compare(0, prefix.length(), prefix) == 0) {
				return str.substr(prefix.length());
			}
			return str;
		}
	}
}
