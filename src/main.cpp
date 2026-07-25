#include<iostream>
#include<filesystem>
#include<fstream>
#include<chrono>
#include<ctime>
#include<iomanip>
#include<vector>
using namespace std;
//commit1
void init(){
    filesystem::path repoPath = ".mygit/commits";
                if(filesystem::exists(".mygit")){
                    cout<<"Repository already exists\n";
                } else {
                    filesystem::create_directories(repoPath);
                    cout<<"Repository initialized\n";
                }
}

void commit(){
    if(!filesystem::exists(".mygit")){
        cout<<"Repository not initialized. Please run init first.\n";
        return;
    }
    int commitcount=0;
    for(auto&entry:filesystem::directory_iterator(".mygit/commits")){
        commitcount++;
    }
    commitcount++;
    string commitName = "commit" + to_string(commitcount) ;

    filesystem::path commitPath=filesystem::path (".mygit/commits")/commitName;
    filesystem::create_directory(commitPath);

    filesystem::recursive_directory_iterator it(".");
    filesystem::recursive_directory_iterator end;

    while(it!=end){
        //copying files and directories to the commit directory
        if(it->path().filename()==".mygit"){
            it.disable_recursion_pending();
            ++it;
            continue;
        }
        filesystem::path relativePath=filesystem::relative(it->path(), ".");
        filesystem::path destinationPath=commitPath/relativePath;
        if(filesystem::is_directory(it->path())){
            filesystem::create_directories(destinationPath);
        }else if(filesystem::is_regular_file(it->path())){
            filesystem::copy_file(it->path(), destinationPath);
        }
        ++it;
    }
    //Creating a log file for the commit
    string message;
    cout<<"Enter commit message: ";
    cin.ignore();
    getline(cin, message);

    //getting current date and time
    auto now = std::chrono::system_clock::now();

    time_t currentTime=chrono::system_clock::to_time_t(now);
    tm* localTime=localtime(&currentTime);

    //opening log in append mode, file will be created if it doesn't exist
    ofstream LogFile(".mygit/log.txt", ios::app);

    LogFile
        << commitName << "|"
        << message << "|"
        << put_time(localTime, "%d-%m-%Y %H:%M:%S")
        <<'\n';

        LogFile.close();

        cout<<"Commit created succesfully:"
            <<commitName<< '\n';
}

void Log(){
    //checking if repo is initialized
    if(!filesystem::exists(".mygit")){
        cout<<"Repository not initialized, please run the init first\n";
        return;
    }
    //Repo exists but no commits found
    if(!filesystem::exists(".mygit/log.txt")){
        cout<<"No commits found, please run the init first\n";
        return;
    }
    //opening the log file
    ifstream LogFile(".mygit/log.txt");
    vector<string>logs;
    string line;

    //reading the log file line by line
    while(getline(LogFile,line)){
        logs.push_back(line);
    }
    LogFile.close();
    for(int i = static_cast<int>(logs.size()) - 1; i >= 0; i--){
        cout<<logs[i]<<'\n';
    }
}

void revert(){
    if(!filesystem::exists(".mygit")){
    cout << "Repository not initialized.\n";
    return;
    }
    int commitNumber;
    cout<<"Enter the commit number: ";
    cin>>commitNumber;

    string commitName="commit"+to_string(commitNumber);

    filesystem::path commitPath=filesystem::path(".mygit/commits")/commitName;

    if(!filesystem::exists(commitPath)){
    cout << "Commit does not exist.\n";
    return;
    }
    for(const auto&entry: filesystem::recursive_directory_iterator(commitPath)){
        //get path relative to commit folder
        filesystem::path relativePath =
            filesystem::relative(entry.path(), commitPath);
        //where this file/folder belongs in the working directory
        filesystem::path destinationPath =
            filesystem::path(".") / relativePath;
        //recreating directories
        if(entry.is_directory()){
            filesystem::create_directories(destinationPath);
        }
        //restoring files
        else if(entry.is_regular_file()){
            filesystem::copy_file(
                entry.path(),
                destinationPath,
                filesystem::copy_options::overwrite_existing
            );
    }
    }
    cout<<"Successfully reverted to commit: "<<commitName<<'\n';
}

int main() {
    while(true){
        int choice;

        cout<<"1. Init\n";
        cout<<"2. Commit\n";
        cout<<"3. Log\n";
        cout<<"4. Revert\n";
        cout<<"5. Exit\n";

        cin>>choice;

        switch(choice){
            case 1:
                // Init functionality
                init();
                break;
            case 2:
                // Commit functionality
                commit();
                break;
            case 3:
                // Log functionality
                Log();
                break;
            case 4:
                // Revert functionality
                revert();
                break;
            case 5:
                cout<<"Exiting...\n";
                return 0;
            default:
                cout<<"Invalid choice\n";
        }
    }
}