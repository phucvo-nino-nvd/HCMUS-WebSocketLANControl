#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <iostream>
#include <algorithm>  // ✅ Thêm dòng này cho transform

using namespace std;

// 📜 Liệt kê tiến trình đang chạy
string list_processes() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return "Failed to take process snapshot.\n";
    }

    PROCESSENTRY32W entry;  // ✅ Đổi sang PROCESSENTRY32W
    entry.dwSize = sizeof(PROCESSENTRY32W);

    stringstream ss;
    if (Process32FirstW(snapshot, &entry)) {  // ✅ Đổi sang Process32FirstW
        do {
            // ✅ Chuyển WCHAR sang string
            wstring wexeName(entry.szExeFile);
            string exeName(wexeName.begin(), wexeName.end());
            
            ss << exeName << " (PID: " << entry.th32ProcessID << ")\n";
        } while (Process32NextW(snapshot, &entry));  // ✅ Đổi sang Process32NextW
    }
    CloseHandle(snapshot);

    string result = ss.str();
    if (result.empty()) result = "No running processes found.\n";
    return result;
}

// ▶️ Khởi động ứng dụng
string start_process(const string& exeName) {
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi{};
    string cmd = exeName;

    if (CreateProcessA(NULL, cmd.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return "Started " + exeName + " successfully.\n";
    } else {
        return "Failed to start " + exeName + ". Error code: " + to_string(GetLastError()) + "\n";
    }
}

// ⏹️ Dừng tiến trình theo PID
string stop_process_by_pid(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        return "Failed to open process PID " + to_string(pid) + ". Error: " + to_string(GetLastError()) + "\n";
    }

    if (TerminateProcess(hProcess, 0)) {
        CloseHandle(hProcess);
        return "Successfully terminated process PID " + to_string(pid) + ".\n";
    } else {
        DWORD err = GetLastError();
        CloseHandle(hProcess);
        return "Failed to terminate PID " + to_string(pid) + ". Error: " + to_string(err) + "\n";
    }
}

// ⏹️ Dừng tiến trình theo tên
string stop_process_by_name(const string& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return "Failed to take process snapshot.\n";
    }

    PROCESSENTRY32W entry;  // ✅ Đổi sang PROCESSENTRY32W
    entry.dwSize = sizeof(PROCESSENTRY32W);

    stringstream ss;
    bool found = false;

    if (Process32FirstW(snapshot, &entry)) {  // ✅ Đổi sang Process32FirstW
        do {
            // ✅ Chuyển WCHAR sang string
            wstring wexeName(entry.szExeFile);
            string exeName(wexeName.begin(), wexeName.end());
            
            string targetName = processName;
            
            // Chuyển về lowercase để so sánh
            transform(exeName.begin(), exeName.end(), exeName.begin(), ::tolower);
            transform(targetName.begin(), targetName.end(), targetName.begin(), ::tolower);

            if (exeName == targetName) {
                found = true;
                string result = stop_process_by_pid(entry.th32ProcessID);
                ss << result;
            }
        } while (Process32NextW(snapshot, &entry));  // ✅ Đổi sang Process32NextW
    }

    CloseHandle(snapshot);

    if (!found) {
        return "Process '" + processName + "' not found.\n";
    }

    return ss.str();
}

// ⏹️ Hàm stop thông minh
string stop_process(const string& target) {
    bool isNumber = !target.empty() && all_of(target.begin(), target.end(), ::isdigit);

    if (isNumber) {
        DWORD pid = stoul(target);
        return stop_process_by_pid(pid);
    } else {
        return stop_process_by_name(target);
    }
}