/*
 * CoreCheck - System Information Utility
 * @author: Aymen Brahim Djelloul
 * Modified: May 21, 2025
 * Version: 1.1
 * License: MIT
 *
 * Description:
 *   CoreCheck is a lightweight C++ application that displays
 *   essential system information including CPU details and OS version.
 */

// INCLUDES
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <intrin.h>
#include <winreg.h>
#include <cstdlib>


using namespace std;

// CONSTANTS
const string VERSION = "1.1";
const string AUTHOR = "Aymen Brahim Djelloul";
const int CPU_MULTIPLIER = 38;

// PROCESSOR CLASS
class Processor {
public:
    // Get CPU information
    string CPUName();
    string Architecture();
    int Model();
    int Family();
    int Stepping();
    int Revision();
    int MaxClockSpeed();
    double BaseClockSpeed(int maxFreq);
    int CoresCount(bool logical = false);
};

// FUNCTION PROTOTYPES
string GetOSName();
string GetOSVersion();
string GetHostname();
double MhzToGhz(int value);
void PrintHeader();
void PrintSeparator();
void PrintCPUInfo(Processor& cpu);
void PrintOSInfo();

// WINDOWS IMPLEMENTATION
#if defined(_WIN32) || defined(_WIN64)

// Get CPU Name
string Processor::CPUName() {
    int cpuInfo[4] = { -1 };
    char cpuBrandString[64] = { 0 };

    __cpuid(cpuInfo, 0x80000000);
    unsigned int nExIds = cpuInfo[0];

    if (nExIds >= 0x80000004) {
        __cpuid(cpuInfo, 0x80000002);
        memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));

        __cpuid(cpuInfo, 0x80000003);
        memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));

        __cpuid(cpuInfo, 0x80000004);
        memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));

        return string(cpuBrandString);
    }
    return "Unknown Processor";
}

// Get CPU Stepping
int Processor::Stepping() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    return cpuInfo[0] & 0xF;
}

// Get CPU Model
int Processor::Model() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    return (cpuInfo[0] >> 4) & 0xF;
}

// Get CPU Family
int Processor::Family() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    return (cpuInfo[0] >> 8) & 0xF;
}

// Get CPU Revision
int Processor::Revision() {
    // In a real application, this would extract the revision
    return 0;
}

// Get CPU Architecture
string Processor::Architecture() {
    #if defined(_M_X64) || defined(__x86_64__)
        return "x86_64";
    #elif defined(_M_IX86) || defined(__i386__)
        return "x86";
    #elif defined(_M_ARM64) || defined(__aarch64__)
        return "ARM64";
    #elif defined(_M_ARM) || defined(__arm__)
        return "ARM";
    #else
        return "Unknown Architecture";
    #endif
}

// Get CPU Max Clock Speed
int Processor::MaxClockSpeed() {
    HKEY hKey;
    DWORD data;
    DWORD dataSize = sizeof(data);

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                    "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                    0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        if (RegQueryValueEx(hKey, "~MHz", NULL, NULL, 
                           (LPBYTE)&data, &dataSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return data;
        }
        RegCloseKey(hKey);
    }
    return -1;
}

// Get CPU Base Clock Speed
double Processor::BaseClockSpeed(int maxFreq) {
    if (maxFreq <= 0 || CPU_MULTIPLIER <= 0) return 0;
    return static_cast<double>(maxFreq) / CPU_MULTIPLIER;
}

// Get CPU Core Count
int Processor::CoresCount(bool logical) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    if (logical) {
        return sysInfo.dwNumberOfProcessors;
    }
    
    // This is a simplification - in real code, you'd use GetLogicalProcessorInformation
    // to get the actual physical core count
    int cores = sysInfo.dwNumberOfProcessors / 2; // Assuming 2 threads per core
    return cores > 0 ? cores : sysInfo.dwNumberOfProcessors;
}

// Get OS Name
string GetOSName() {
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    #pragma warning(suppress : 4996) // Suppress GetVersionEx deprecation warning
    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        string osName = "Windows";
        
        if (osvi.dwMajorVersion == 10) {
            if (osvi.dwBuildNumber >= 22000) {
                osName += " 11";
            } else {
                osName += " 10";
            }
        } else if (osvi.dwMajorVersion == 6) {
            switch (osvi.dwMinorVersion) {
                case 0: osName += " Vista"; break;
                case 1: osName += " 7"; break;
                case 2: osName += " 8"; break;
                case 3: osName += " 8.1"; break;
                default: osName += " Unknown"; break;
            }
        } else {
            osName += " " + to_string(osvi.dwMajorVersion) + "." 
                   + to_string(osvi.dwMinorVersion);
        }
        
        return osName;
    }
    
    return "Unknown Windows Version";
}

// Get OS Version
string GetOSVersion() {
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    #pragma warning(suppress : 4996)
    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        return to_string(osvi.dwMajorVersion) + "." 
             + to_string(osvi.dwMinorVersion) + " (Build " 
             + to_string(osvi.dwBuildNumber) + ")";
    }
    
    return "Unknown";
}

// Get Hostname
string GetHostname() {
    char buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(buffer);
    
    if (GetComputerNameA(buffer, &size)) {
        return string(buffer);
    }
    
    return "Unknown";
}

#elif defined(__linux__)
// Linux Implementation (stubs)

string Processor::CPUName() { return "Not implemented for Linux"; }
int Processor::Stepping() { return 0; }
int Processor::Model() { return 0; }
int Processor::Family() { return 0; }
int Processor::Revision() { return 0; }
string Processor::Architecture() { return "Not implemented for Linux"; }
int Processor::MaxClockSpeed() { return 0; }
double Processor::BaseClockSpeed(int maxFreq) { return 0; }
int Processor::CoresCount(bool logical) { return 0; }

string GetOSName() { return "Linux"; }
string GetOSVersion() { return "Not implemented for Linux"; }
string GetHostname() { return "Not implemented for Linux"; }

#elif defined(__APPLE__)
// macOS Implementation (stubs)

string Processor::CPUName() { return "Not implemented for macOS"; }
int Processor::Stepping() { return 0; }
int Processor::Model() { return 0; }
int Processor::Family() { return 0; }
int Processor::Revision() { return 0; }
string Processor::Architecture() { return "Not implemented for macOS"; }
int Processor::MaxClockSpeed() { return 0; }
double Processor::BaseClockSpeed(int maxFreq) { return 0; }
int Processor::CoresCount(bool logical) { return 0; }

string GetOSName() { return "macOS"; }
string GetOSVersion() { return "Not implemented for macOS"; }
string GetHostname() { return "Not implemented for macOS"; }

#else
#error "Unsupported platform"
#endif

// Convert MHz to GHz
double MhzToGhz(int value) {
    return value / 1000.0;
}

// Print application header
void PrintHeader() {
    cout << "\n=============================================\n";
    cout << "  CoreCheck v" << VERSION << "  |  by " << AUTHOR << "\n";
    cout << "=============================================\n";
}

// Print section separator
void PrintSeparator() {
    cout << "\n---------------------------------------------\n";
}

// Print CPU information
void PrintCPUInfo(Processor& cpu) {
    int maxClockSpeed = cpu.MaxClockSpeed();
    double baseClockSpeed = cpu.BaseClockSpeed(maxClockSpeed);
    
    cout << "\n[ CPU INFORMATION ]\n";
    cout << "\n" << cpu.CPUName() << "\n";
    
    cout << "\n" << left << setw(20) << "Architecture" << ": " << cpu.Architecture();
    cout << "\n" << left << setw(20) << "Family" << ": " << cpu.Family();
    cout << "\n" << left << setw(20) << "Model" << ": " << cpu.Model();
    cout << "\n" << left << setw(20) << "Stepping" << ": " << cpu.Stepping();
    
    cout << fixed << setprecision(2);
    if (maxClockSpeed > 0) {
        cout << "\n" << left << setw(20) << "Max Clock Speed" << ": " << MhzToGhz(maxClockSpeed) << " GHz";
    }
    
    if (baseClockSpeed > 0) {
        cout << "\n" << left << setw(20) << "Base Clock Speed" << ": " << baseClockSpeed << " MHz";
    }
    
    cout << "\n" << left << setw(20) << "Physical Cores" << ": " << cpu.CoresCount();
    cout << "\n" << left << setw(20) << "Logical Cores" << ": " << cpu.CoresCount(true);
}

// Print OS information
void PrintOSInfo() {
    cout << "\n[ OPERATING SYSTEM ]\n";
    cout << "\n" << left << setw(20) << "Name" << ": " << GetOSName();
    cout << "\n" << left << setw(20) << "Version" << ": " << GetOSVersion();
    cout << "\n" << left << setw(20) << "Hostname" << ": " << GetHostname();
}

// Main function
int main() {
    try {

        // Set terminal title
        system("title CoreCheck - v1.1");


        Processor cpu;
        
        PrintHeader();
        PrintCPUInfo(cpu);
        PrintSeparator();
        PrintOSInfo();
        PrintSeparator();
        
        cout << "\nPress Enter to exit...";
        cin.get();
        
        return 0;
    }
    catch (const exception& e) {
        cerr << "\nError: " << e.what() << endl;
        return 1;
    }
}
