// dkuzmy3 proj3
// 11/26//2020
// Simple assembly interpreter

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>

using namespace std;

class Ali{
private:
    vector<string> memory;          // 32bit addressable memory of 255 words
    unordered_map<string, int> map; // 32bit addressable memory for every declared variable
    bool debug = false;             // debug boolean used to print data for debug

public:
    // debug stuff
    void setDebug(){
        debug = true;
    }

    void disableDebug(){
        debug = false;
    }

    bool isDebug(){
        return debug;
    }
    // memory stuff
    void clearMemory(){                         // clear memory
        memory.clear();
    }

    string getMemory(int a){                    // get memory at position a
        return memory[a];
    }

    void setMemory(string &s, int c){           // set memory s at position c
       memory[c] = s;
    }

    void printMemory(){                         // print memory words
        cout << endl << "Memory:" << endl;
        for(auto & i : memory){
            cout << i << endl;
        }
        cout << endl;
    }

    int getMemorySize(){                        // used to spot memory exceding 255 words
        return memory.size();
    }
    // map stuff
    void add_map(string &a, int b){             // add new mapped value from symbol a to int b
        map[a] = b;
    }

    int get_map(string &a){                     // get map at symbol a
        return map[a];
    }

    void printMap(){                            // print all mapped items
        for(auto &s : map)
            cout << s.first << " : " << s.second << endl;
    }

    bool read_file(const string &fname){        // read lines from file and insert them into memory
        ifstream file;
        string line;

        file.open(fname, ios::in);
        if(!file.is_open()) return false;

        while(getline(file, line)){
            if(line.length() > 0){              // skip empty lines
                memory.push_back(line);
            }
        }
        file.close();
        return true;
    }

    void dec(string &comm, int &count) {        // DEC function
        string tmp;
        tmp.push_back(comm[4]);              // extract symbol
        setMemory(tmp, count);              // replace instruction with symbol
        add_map(tmp, 0);                 // map symbol with initial value 0
        if(debug) cout << "Declared " << tmp << endl;
    }

    int lda(string &comm, int &count) {         // LDA function
        string tmp;
        tmp.push_back(comm[4]);
        if(debug) cout << "A : " << get_map(tmp) << endl;
        return get_map(tmp);                 // return stored value at symbol in the map
    }

    int ldb(string &comm, int &count) {
        string tmp;
        tmp.push_back(comm[4]);
        if(debug) cout << "B : " << get_map(tmp) << endl;
        return get_map(tmp);
    }

    int ldi(string &comm, int &count){          // LDI function
        string tmp;
        int j;
        int value;
        j = comm.find(" ");                  // find space in the command string
        tmp = comm.substr(j+1);            // extract the whole integer from command after space
        stringstream converter(tmp);
        converter >> value;                    // transform string to int
        if(debug) cout << "Accumulator : " << value << endl;
        return value;                          // return int
    }

    void str(string &comm, int &acc){          // STR function
        string tmp;
        tmp.push_back(comm[4]);
        add_map(tmp, acc);                  // store accumulator value into symbol
        if(debug) cout << "Added " << acc << " to " << tmp << endl;
    }

    int jmp(string &comm, int &count){          // JMP function
        string tmp;
        int j;
        int value;
        j = comm.find(" ");
        tmp = comm.substr(j+1);
        stringstream converter(tmp);
        converter >> value;
        if(debug) cout << "Jump to " << value << endl;
        return value-1;                         // return value-1 because it will be auto increased in the while loop
    }

    int jzs(string &comm, int &count){
        string tmp;
        int j;
        int value;
        j = comm.find(" ");
        tmp = comm.substr(j+1);
        stringstream converter(tmp);
        converter >> value;
        if(debug) cout << "Jump to " << value << ", zero result is true." << endl;
        return value-1;
    }

    int jvs(string &comm, int &count){
        string tmp;
        int j;
        int value;
        j = comm.find(" ");
        tmp = comm.substr(j+1);
        stringstream converter(tmp);
        converter >> value;
        if(debug) cout << "Jump to " << value << ", overflow is true." << endl;
        return value-1;
    }

    int add(int a, int b){
        if(debug) cout << "A = A + B = " << a+b << endl;
        return a+b;
    }
};

class Implementation : public Ali{
private:
    bool done = false;
    int accumulator = 0;
    int add_reg = 0;
    int counter = 0;
    bool zero_result = false;
    bool overflow = false;
    int max = 2147483647;
    int min = -2147483647;

public:

    void printData(){
        cout << "Accumulator (A): " << accumulator << endl;
        cout << "Additional registry (B): " << add_reg << endl;
        cout << "Overflow bit: " << overflow << endl;
        cout << "Zero-result bit: " << zero_result << endl;
    }

    bool isDone(){
        return done;
    }

    void setDone(){
        done = true;
    }

    void execute(bool automatic = false){           // main execution function, takes (true/false, no param) for automatic execution
        setDebug();                                 // debug mode on
        do {                                        // use do while so that command 'l' and 'a' can use the same func
            string currentCommand = getMemory(counter);
            if (currentCommand == "HLT") {
                done = true;
                automatic = false;
                break;
            }
            // take command from memory & scan for result, commands are idx 0, length 3 in the memory[counter]
            string check = currentCommand.substr(0,3);

            if(check == "DEC"){
                dec(currentCommand, counter);       // use of inherited functions from Ali
            }

            else if(check == "LDA"){
                accumulator = lda(currentCommand, counter);
            }

            else if(check == "LDB"){
                add_reg = ldb(currentCommand, counter);
            }

            else if(check == "LDI"){
                accumulator = ldi(currentCommand, counter);
            }

            else if(check == "STR"){
                str(currentCommand, accumulator);
            }

            else if(check == "XCH"){
                int tmp1 = accumulator;
                accumulator = add_reg;
                accumulator = tmp1;
                if(isDebug()) cout << "A <-> B" << endl;
            }

            else if(check == "JMP"){
                counter = jmp(currentCommand, counter);
            }

            else if(check == "JZS"){
                if(zero_result)
                    counter = jzs(currentCommand, counter);
                else
                    if(isDebug()) cout << "Skipping JZS since zero-result: " << zero_result << endl;
            }

            else if(check == "JVS"){
                if(overflow)
                    counter = jvs(currentCommand, counter);
                else
                if(isDebug()) cout << "Skipping JVS since overflow: " << overflow << endl;
            }
            else if(check == "ADD"){
                accumulator = add(accumulator, add_reg);
                if(accumulator > max || accumulator < min){
                    overflow = true;
                    if(isDebug()) cout << "Attention! accumulator exceeded memory. Flushing.." << endl;
                    accumulator = accumulator % max;
                }
                else
                    overflow = false;
                if(accumulator == 0) {
                    zero_result = true;
                    if(isDebug()) cout << "Zero result true." << endl;
                }
                else zero_result = false;
            }
            else {
                cout << "Error entry: " << check << " is not an operation instruction." << endl;
                cout << "Program will close to prevent divine punishment." << endl;
                disableDebug();
                setDone();
                break;
            }

            counter++;
            // extra checkers
            if(accumulator > max || accumulator < min){
                if(isDebug()) cout << "Attention! accumulator exceeded memory. Flushing.." << endl;
                accumulator = accumulator % max;
            }
            if(add_reg > max || add_reg < min){
                if(isDebug()) cout << "Attention! add_reg exceeded memory. Flushing.." << endl;
                add_reg = add_reg % max;
            }
        }while(automatic);                          // automatic true/false based on call from main

    }

};

int main() {
    bool done = false;
    bool validFile = false;
    char command;
    string fileName;
    Implementation imp;

    imp.setDebug();   // used to print debug data, comment out to disable

    while(!done){
        while(!validFile){
            // ask for file
            cout << "Enter file name (eg. text.txt): ";
            cin >> fileName;
            if(!imp.read_file(fileName)) cout << "Invalid file." << endl;
            else if(imp.getMemorySize()>255) {
                cout << "Memory overflow!" << endl;
                imp.clearMemory();
            }
            else {
                validFile = true;
                if(imp.isDebug()) imp.printMemory();
            }
        }

        cout << "Enter command: " << endl;
        cin >> command;                     // takes in multiple commands at a time for faster debug eg: llll

        switch (command) {
            case 'l':
                // move by 1
                imp.execute();
                break;

            case 'a':
                // automatic move till end
                imp.execute(true);
                break;

            case 'q':
                cout << "Quitting.." << endl;
                imp.setDone();
                break;

            default:
                cout << "No such command.." << endl;
                break;
        }

        done = imp.isDone();

    }

    if(imp.isDebug()) {
        imp.printMemory();
        imp.printData();
        imp.printMap();
    }

    return 0;
}
