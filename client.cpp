//client

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>

#include <sstream>

#define MAX_BUFFER_SIZE 64

int main()
{
    std::string pipe_name;
    std::cout<<"Enter pipe name :";
    std::cin >> pipe_name;
    std::string pipe = "\\\\.\\pipe\\" + pipe_name;
    HANDLE cpipe = CreateFile(pipe.c_str(),
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              nullptr,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (cpipe == INVALID_HANDLE_VALUE)
        {
            std::cout << "Error:"<< GetLastError()<<std::endl;
        }


    std::string command (64,'\0');
    DWORD r;
    DWORD w;


    while (true)
        {
            std::cout << "> ";
            std::getline(std::cin >> std::ws, command);
            WriteFile(cpipe, command.c_str(), command.size(), &w, nullptr);


            if (command == "quit")
                {
                    CloseHandle(cpipe);
                    break;
                }
            else
                {
                    std::string buf (MAX_BUFFER_SIZE, '\0');
                    ReadFile(cpipe, &buf[0], buf.size(), &r, nullptr);
                    std::cout << buf <<'\n';
                }
            command.replace(0,command.size(),command.size(),'\0');
        }
}
