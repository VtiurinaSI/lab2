#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#define MAX_BUFFER_SIZE 64

int main()
    {
        std::string pipe_name;
        std::cout << "Enter pipe name: ";
        std::getline(std::cin, pipe_name);
        auto pipe_path = "\\\\.\\pipe\\" + pipe_name;
        auto pipe = CreateFile(pipe_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (pipe == INVALID_HANDLE_VALUE)
            {
                std::cout << "Error. Error code: " << GetLastError()<<'\n';
                return EXIT_FAILURE;
            }
        if (pipe != INVALID_HANDLE_VALUE)
            {
                DWORD dmode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
                auto fsuccess = SetNamedPipeHandleState(pipe, &dmode, nullptr, nullptr);
                if (!fsuccess)
                    {
                        CloseHandle(pipe);
                        pipe = INVALID_HANDLE_VALUE;
                    }
            }
        std::string com;
        DWORD r;
        DWORD w;
        while (true)
            {
                std::cout << "> ";
                WriteFile(pipe, com.c_str(), com.size(), &w, nullptr);
                if (com == "quit")
                    {
                        CloseHandle(pipe);
                        break;
                    }
                else
                    {
                        std::string message (MAX_BUFFER_SIZE, '\0');
                        ReadFile(pipe, &message[0], message.size(), &r, nullptr);
                        std::cout << message <<'\n';
                    }
                com.replace(0,com.size(),com.size(),'\0');
            }
    }
