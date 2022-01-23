#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <map>
#define MAX_BUFFER_SIZE 64

int main()
    {
        std::string pipe_name;
        std::cout << "Pipe name: ";
        std::cin >> pipe_name;
        std::string path = "\\\\.\\pipe\\" + pipe_name;
        HANDLE pipe = CreateNamedPipe(path.c_str(),
                                    PIPE_ACCESS_DUPLEX,
                                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
                                    PIPE_UNLIMITED_INSTANCES,
                                    MAX_BUFFER_SIZE,MAX_BUFFER_SIZE,0,
                                    nullptr);
        if (pipe == INVALID_HANDLE_VALUE)
            {
                std::cout << "Error:"<< GetLastError()<<'\n';
            }
        std::string com (MAX_BUFFER_SIZE, '\0');
        std::string kw,key,v,rsp {};
        DWORD bytes;
        std::map<std::string, std::string> data {};
        while (true)
            {
                std::cout << "Wait for client \n";
                if (!ConnectNamedPipe(pipe, nullptr))
                    {
                        std::cout << "Error:"<< GetLastError()<<"\n";;
                        CloseHandle(pipe);
                    }
                else
                    {
                        std::cout << "Successful \n";;
                    }
                while(true)
                    {
                        std::cout << "Waiting for client comm \n";
                        auto rf = ReadFile(pipe, &com[0], com.size(), &bytes, nullptr);
                        if (!rf)
                            {
                                auto err = GetLastError();
                                std::cout<<"Error:"<<err;
                            }
                        com.resize(com.find('\0'));
                        std::cout << com;
                        std::istringstream parser {com};
                        parser >> std::ws >> kw;
                        if (kw == "set")
                            {
                                parser >> key >> v;
                                data[key] = v;
                                rsp = "acknowledged";
                            }
                        else if (kw == "list")
                            {
                                for (auto i = data.begin(); i != data.end(); ++ i)
                                    rsp += i->first + " ";
                            }
                        else if (kw == "get")
                            {
                                parser >> key;
                                if (data.find(key) != data.end())
                                    rsp = "found " + data[key];
                                else
                                    rsp = "missing";
                            }
                        else if (kw == "quit")
                            {
                                DisconnectNamedPipe(pipe);
                                com.replace(0, com.size(), com.size(), '\0');
                                com.resize(MAX_BUFFER_SIZE, '\0');
                                break;
                            }
                        else if (kw == "delete")
                            {
                                parser >> key;
                                auto del = data.find(key);
                                if (del != data.end())
                                    {
                                        data.erase(del);
                                        rsp = "deleted";
                                    }
                                else
                                    rsp = "missing";
                            }
                        else
                            {
                                std::cerr << "Wrong command! Commands: get, set, list, delete or quit. \n";
                                rsp = "incorrect command";
                            }
                        auto wf = WriteFile(pipe, rsp.c_str(), rsp.size(), &bytes, nullptr);
                        if (!wf)
                            {
                                auto err = GetLastError();
                                std::cout<<"Error:"<<err;
                            }
                        com.replace(0, com.size(), com.size(), '\0');
                        com.resize(MAX_BUFFER_SIZE, '\0');
                        rsp.clear();
                        kw.clear();
                        key.clear();
                        v.clear();
                    }
                char answer;
                bool exit = false;
                std::cout << "Ddestroy the pipe \"" << pipe_name << "\" (y or n)?: ";
                while (std::cin >> answer)
                    {
                        if (answer == 'y')
                            {
                                (CloseHandle(pipe), "occurred while closing pipe", pipe_name);
                                exit = true;
                                break;
                            }
                        else if (answer == 'n')
                            break;
                        else
                            {
                                std::cout << "(y or n): ";
                                continue;
                            }
                    }
                if (exit)
                    break;
            }
    }
