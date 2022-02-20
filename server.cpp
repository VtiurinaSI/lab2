//server

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
        std::cout << "Enter  pipe name: ";
        std::cin >> pipe_name;
        std::string pipe = "\\\\.\\pipe\\" + pipe_name;
        HANDLE cpipe = CreateNamedPipe(pipe.c_str(),
                                    PIPE_ACCESS_DUPLEX,
                                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
                                    PIPE_UNLIMITED_INSTANCES,
                                    MAX_BUFFER_SIZE,MAX_BUFFER_SIZE,0,
                                    nullptr);
        if (cpipe == INVALID_HANDLE_VALUE)
            {
                std::cout << "Error:"<< GetLastError()<<'\n';
            }

        std::string command (MAX_BUFFER_SIZE, '\0');
        std::string kw,k,v,resp {};
        DWORD bytes;
        std::map<std::string, std::string> data {};
        while (true)
            {
                std::cout << "Waiting for client connects...\n";
                if (!ConnectNamedPipe(cpipe, nullptr))
                    {
                        std::cout << "Error:"<< GetLastError()<<'\n';;
                        CloseHandle(cpipe);
                    }
                else
                    {
                        std::cout << "successful"<<'\n';;
                    }


                while(true)
                    {

                        std::cout << "Waiting for client's command...\n";
                        auto fr = ReadFile(cpipe, &command[0], command.size(), &bytes, nullptr);
                        if (!fr)
                            {
                                auto err = GetLastError();
                                std::cout<<"Error:"<<err;
                            }
                        command.resize(command.find('\0'));
                        std::cout << command;
                        std::istringstream parser {command};
                        parser >> std::ws >> kw;
                        if (kw == "set")
                            {
                                parser >> k >> v;
                                data[k] = v;
                                resp = "acknowledged";
                            }
                        else if (kw == "list")
                            {
                                for (auto i = data.begin(); i != data.end(); ++ i)
                                resp += i->first + " ";
                            }
                        else if (kw == "get")
                            {
                                parser >> k;
                                if (data.find(k) != data.end())
                                    resp = "found " + data[k];
                                else
                                    resp = "missing";
                            }

                        else if (kw == "delete")
                            {
                                parser >> k;
                                auto del = data.find(k);
                                if (del != data.end())
                                    {
                                        data.erase(del);
                                        resp = "deleted";
                                    }
                                else
                                    resp = "missing";
                            }
                        else if (kw == "quit")
                            {
                                DisconnectNamedPipe(cpipe);
                                command.replace(0, command.size(), command.size(), '\0');
                                command.resize(MAX_BUFFER_SIZE, '\0');
                                break;
                            }
                        else
                            {
                                std::cerr << "Incorrect command! (command: \"" << command << "\")\n";
                                resp = "incorrect command";
                            }


                        auto fr2 = WriteFile(cpipe, resp.c_str(), resp.size(), &bytes, nullptr);
                        if (!fr2)
                            {
                                auto err = GetLastError();
                                std::cout<<"Error:"<<err;
                            }


                        command.replace(0, command.size(), command.size(), '\0');
                        command.resize(MAX_BUFFER_SIZE, '\0');
                        resp.clear();
                        kw.clear();
                        k.clear();
                        v.clear();
                    }


                char answer;
                bool exit = false;
                std::cout << "Do you want to destroy pipe \"" << pipe_name << "\" (y/n)?: ";
                while (std::cin >> answer)
                    {
                        if (answer == 'y')
                            {
                                (CloseHandle(cpipe),
                                "occurred while closing pipe",
                                pipe_name);
                                exit = true;
                                break;
                            }
                        else if (answer == 'n')
                            break;
                        else
                            {
                                std::cout << "(y/n): ";
                                continue;
                            }
                    }
                if (exit)
                    break;
            }
    }
