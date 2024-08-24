/*
@author : Aymen Brahim Djelloul
date : 21.08.2024
version : 1.0
License : MIT

    
    // What is CoreCheck ?

        CoreCheck is a lightweight and efficient C++ application designed to swiftly retrieve
        and display detailed information about your operating system and machine configuration.
        With a CLI interface, CoreCheck provides essential data such as OS version,
        system architecture, processor details, and more. Ideal for system administrators,
        developers, and power users, CoreCheck simplifies the process of gathering critical system
        information with minimal overhead.

    // What CoreCheck can do ?

        it can get this following informations.
        Processor :

        - CPU name
        - CPU revision
        - CPU Archeticture
        - CPU Model
        - CPU Family
        - CPU Stepping
        - CPU Cores & Threads
        - CPU max clock speed
        - CPU base clock speed

        Operating System :
        
        - OS name

*/

// INCLUDES
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <typeinfo>
#include <cstddef>
#include <windows.h>
#include <intrin.h>
#include <winreg.h> // For registry functions

using namespace std;

// DECLARE GLOBAL VARIABLES
const string AUTHOR = "Aymen Brahim Djelloul";
const string VERSION = "1.0";

// DECLARE CPU INFO VARIABLE TO USE IT WITH INTRIN
const int CPUINFO[4];
// Define a CPU default Multiplier value
const int CPU_MULTIPLIER = 38;

// DECLARE SYSTEM INFO VARIABLES TO GET SYS INFO
const SYSTEM_INFO sysInfo;

// Create Processor Class that contains methods to get cpu info
class Processor {
public:
    string CPUName();
    string Architecture();
    int Model();
    int Family();
    int Stepping();
    int Revision();
    int MaxClockSpeed();
    double BaseClockSpeed(int maxFreq, int Multiplier);
    int CoresCount(bool logical);
};


// Use preprocessing directives to run CoreCheck on Cross-platform
// This section is for Win32 and Win64 systems
#if defined(_WIN32) || defined(_WIN64)

    // This method will return the CPU name
    string Processor::CPUName() {
        int cpuInfo[4] = { -1 };
        char cpuBrandString[0x40] = { 0 };

        // Get the information associated with the processor
        __cpuid(cpuInfo, 0x80000002);
        memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));

        __cpuid(cpuInfo, 0x80000003);
        memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));

        __cpuid(cpuInfo, 0x80000004);
        memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));

        return string(cpuBrandString);
    }

    // This method will get the CPU Stepping
    int Processor::Stepping() {
        __cpuid(CPUINFO, 1);
        int stepping = CPUINFO[0] & 0xF;
        return stepping;
    }

    // This method will get the CPU Model 
    int Processor::Model() {
        __cpuid(CPUINFO, 1);
        int model = (CPUINFO[0] >> 4) & 0xF;
        return model;
    }

    // This method will get the CPU family
    int Processor::Family() {
        __cpuid(CPUINFO, 1);
        int family = (CPUINFO[0] >> 8) & 0xF;
        return family;
    }

    // This method will return the CPU architecture string
    string Processor::Architecture() {
        #if defined(__x86_64__) || defined(_M_X64)
            return "x86_64";
        #elif defined(__i386) || defined(_M_IX86)
            return "x86";
        #elif defined(__aarch64__) || defined(_M_ARM64)
            return "ARM64";
        #elif defined(__arm__) || defined(_M_ARM)
            return "ARM";
        #elif defined(__powerpc64__) || defined(__ppc64__)
            return "PowerPC 64-bit";
        #elif defined(__powerpc__) || defined(__ppc__)
            return "PowerPC";
        #elif defined(__mips__) || defined(__mips64)
            return "MIPS";
        #else
            return "Unknown Architecture";
        #endif
    }

    // This method will get the base clock speed
    double Processor::BaseClockSpeed(int MaxFreq, int Multiplier = CPU_MULTIPLIER) {
        return MaxFreq / Multiplier;
    }

    // This method will get the Maximum clock speed
    int Processor::MaxClockSpeed() {
        HKEY hKey;
        DWORD data;
        DWORD dataSize = sizeof(data);

        // Open the registry key where CPU frequency info is stored
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                        0,
                        KEY_READ,
                        &hKey) == ERROR_SUCCESS) {
            
            // Query the Max Clock Speed from the registry
            if (RegQueryValueEx(hKey, "~Mhz", NULL, NULL, (LPBYTE)&data, &dataSize) == ERROR_SUCCESS) {
                return data;
            } else {
                return -1;
            }
            RegCloseKey(hKey);
        } else {
            return -1;
        }
    };

    // This method will get the number of CPU Cores
    int Processor::CoresCount(bool logical = false) {
        GetSystemInfo(&sysInfo);
        if (logical) {
            return sysInfo.dwNumberOfProcessors;  // Return logical core count
        } else {
            return sysInfo.dwNumberOfProcessors;  // Returns physical core count
        }
    }

    // This method will get the OS name with version
    std::string GetOSName() {
        OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO) };
        if (GetVersionEx(&osvi)) {
            return "Windows " + std::to_string(osvi.dwMajorVersion) + "." + std::to_string(osvi.dwMinorVersion);
        } else {
            return "Unknown Windows version";
        }
    }

// This section is for Linux-debian based systems
#elif defined(__linux__)
    
    // This method will get CPU Name
    string Processor::CPUName() {
        return 0;
    }
    
    // This method will get the CPU Stepping
    int Processor::Stepping() {
        return 0;
    }

    // This method will get the CPU Model
    int Processor::Model() {
        return 0;
    }

    // This method will get the CPU Family
    int Processor::Family() {
        return 0;
    }

    // This method will get the CPU Archeticture string
    string Processor::Architecture() {
        return "";
    }

    // This method will get CPU base clock speed
    double Processor::BaseClockSpeed(int MaxFreq, int multiplier = CPU_MULTIPLIER) {
        return 0;
    }

    // This method will get the Maximum clock speed
    int Processor::MaxClockSpeed() {
        return 0;
    }

    // This method will get the number CPU Cores 
    int Processor::CoresCount(bool logical = false) {
        return 0;
    }

    // This method will get the OS name and version
    string GetOSName() {
        return 0;
    }

#elif defined(__APPLE__)
        // This method will get CPU Name
    string Processor::CPUName() {
        return 0;
    }
    
    // This method will get the CPU Stepping
    int Processor::Stepping() {
        return 0;
    }

    // This method will get the CPU Model
    int Processor::Model() {
        return 0;
    }

    // This method will get the CPU Family
    int Processor::Family() {
        return 0;
    }

    // This method will get the CPU Archeticture string
    string Processor::Architecture() {
        return "";
    }

    // This method will get CPU base clock speed
    double Processor::BaseClockSpeed(int MaxFreq, int multiplier = CPU_MULTIPLIER) {
        return 0;
    }

    // This method will get the Maximum clock speed
    int Processor::MaxClockSpeed() {
        return 0;
    }

    // This method will get the number CPU Cores 
    int Processor::CoresCount(bool logical = false) {
        return 0;
    }

    // This method will get the OS name and version
    string GetOSName() {
        return 0;
    }


#else 
    return "Sorry this is Unsupported platform";

#endif

// This Function will Convert Mhz value to Ghz 
double MhzToGhz(int value) {
    return value / 1000.0;
}

int main() {
    Processor cpu;

    string x;
    // Print the software Command-Line interface
    cout << "\n [ CoreCheck" << "   v" << VERSION << "]    Developd by " << AUTHOR << endl;

    // CPU SECTION
    cout << "\n [CPU INFO] ===========================================\n\n";
    // Get and Print CPU Informaions
    cout  << cpu.CPUName() << endl;
    cout << "\n Stepping : " << cpu.Stepping() << "\n Model : " << cpu.Model();
    cout << "\n Family : " << cpu.Family() << "\n Architecture : " << cpu.Architecture();

    // Set output precision for double values
    cout << fixed << setprecision(1);
    // Get the CPU max clock speed
    int maxClockSpeed = cpu.MaxClockSpeed();
    cout << "\n Base clock speed : " << cpu.BaseClockSpeed(maxClockSpeed) << "Mhz";
    cout << "\n Maximum clock speed : " << MhzToGhz(maxClockSpeed) << " Ghz";
    cout << "\n Cores : " << cpu.CoresCount() << "\n Threads : " << cpu.CoresCount(true); 
     
    // OS SECTION
    cout << "\n\n [OS INFO] ========================================\n\n";
    // Get the OS version
    cout << "Operating System : " << GetOSName();

    cin >> x;
    return 0;
}
