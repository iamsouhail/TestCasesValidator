#include<iostream>
#include <fstream>
#include <sys/stat.h>
#include<string>
#include<vector>
#include <sstream>
#include<stdio.h>
#include<stdlib.h>
#include<ctime>


enum ResultType {
        WRONG_ANSWER,
        TLE,
        COMPILATION_ERROR,
        RUNTIME_ERROR,
        ACCEPTED,
        SYSTEM_ERROR
        };
enum TypeFile {
    CPP,
    JAVA,
    FILE_ERROR
};


class TestValidator {
    

public:
    std::string File_inputInformations;
    std::string Folder_TT;
    std::string File_input,File_output;
    std::string File_Temp;
    int numberOfTestCases;
    double timeLimitInSec; 
    double executing_time;
public:
    TestValidator(){
        this->File_inputInformations = "numberofinput";
        this->Folder_TT = "TT";
        this->File_input = "input-";
        this->File_output = "output-";
        this->File_output = "output-";
        this->File_Temp = "temp-output-testcases-TestValidator";
        this->numberOfTestCases = 0;
        this->timeLimitInSec = 0;
    };

    /*
     *  This method check if the TT exist in the pathfile
     * 
     */
    bool checkIfTTFolderExist(){
        struct stat sb;

        if (stat(this->Folder_TT.c_str(), &sb) == 0)
            return true;

        return false;
    }
    /*
    *   This method check if a file exist 
    *
    */
    bool checkIfAfileExist(std::string &fileName){
        std::ifstream myfile(fileName.c_str());
        return (bool) myfile; 

    }


    /*
        Read the information from the file specified
        number of input
        time limit
    
    */
    bool readInputInformations(){
        std::pair<int,double> result = std::make_pair(0,0);
        std::string fileName = this->Folder_TT+"/"+this->File_inputInformations;
        std::ifstream myfile(fileName.c_str());
        myfile>>this->numberOfTestCases;
        myfile>>this->timeLimitInSec;
        return this->numberOfTestCases !=0;
    }
    /*
        read from the output file every thing and return a vector
        of it
    
    */
    std::vector<std::string> readOutput(std::string fileName){
        std::vector<std::string> result;
        std::ifstream myfile(fileName.c_str());
        std::string tmp;
        while(getline(myfile,tmp)){
            result.push_back(tmp);
        }
        return result;
    }

    /*
        check if output in the vectors are the same 
    */
    bool areTheSame(std::vector<std::string> vec1,std::vector<std::string> vec2){
        std::string tmp1,tmp2;

        if(vec1.size()!=vec2.size())
            return false;
        for(int i=0;i<vec1.size();i++){
            std::stringstream flux1(vec1[i]),flux2(vec2[i]);
            while(flux1>>tmp1 && flux2>>tmp2){
                if(tmp1!=tmp2)
                    return false;
            }
        }
        return true;
    }


    /*
        execute a Commande to compile a code
    
    */
    bool compileCommand(std::string Commande){
        char buffer[128];
        FILE *pPipe;

        // if the command cant be executed
        if((pPipe = _popen(Commande.c_str(),"rt")) == NULL)
            return false;

        if(_pclose(pPipe)!=0){
            return false;
        }
    return true;
    }

    /*
        execute a commande to run a code
    */
    std::vector<std::string> executeCommand(std::string Commande){
        FILE *pPipe;
        std::vector<std::string> result;
        clock_t t1,t2;

        Commande = "( "+Commande+" )"+" > "+this->Folder_TT+"/"+this->File_Temp;
        t1 = clock();
        // if the command cant be executed
        if((pPipe = _popen(Commande.c_str(),"rt")) == NULL)
            return result;
        //getting all the output from the command
        
        
        if(_pclose(pPipe)!=0){
            return result;
        }
        t2 = clock();
        this->executing_time = ((double)t2 - (double)t1)/(double)CLOCKS_PER_SEC;




        // getting the answer
        std::fstream out((this->Folder_TT+"/"+this->File_Temp).c_str());
        std::string tmp;
        while(getline(out,tmp)){
            result.push_back(tmp);
        }

        return result;
    }


    /*
        check the type of the input (file)
        return 
           
    */
    int typeOfFile(std::string file){
        for(int i=0;i<file.size();i++){
            if(file[i] == '.'){
                std::string tmp = file.substr(i+1,file.size() - i+1);
                if(tmp== "c" || tmp == "cpp" )
                    return CPP;
                if(tmp == "java")
                    return JAVA;
                return FILE_ERROR;

            }
        }

        return 0;
    }


    int validate(std::string file){
        bool ans;
        std::string commande;
        int type_of_file;

        type_of_file = this->typeOfFile(file);
        if(type_of_file == FILE_ERROR)
            return COMPILATION_ERROR;
        else if(type_of_file == CPP){
            commande = "g++ -o answer "+file;
        } else { // case of java
            commande = "javac "+file;
        }
        if(this->checkIfAfileExist(file) == false){
            return SYSTEM_ERROR;
        }
        ans = this->compileCommand(commande.c_str());
            if(!ans)
                return COMPILATION_ERROR;
        
        //executing the code 
        
        if(type_of_file == JAVA){
            std::string name = "";
            for(int i=0;i<file.size();i++){
                if(file[i]=='.')
                    break;
                name+=file[i];
            }
            commande = "java "+name+".class < ";
        } else { // cpp || c
            commande = "answer.exe < ";
        }
        if(this->checkIfTTFolderExist() == false)
            return SYSTEM_ERROR;
        if(this->readInputInformations() == false){
            return SYSTEM_ERROR;
        }

        std::string chemin1,chemin2;
        for(int i=1;i<=numberOfTestCases;i++){
            chemin1 =this->Folder_TT+"/"+ this->File_input+this->NumberToString<int>(i);
            chemin2 =this->Folder_TT+"/"+ this->File_output+this->NumberToString<int>(i);
            if(this->checkIfAfileExist(chemin1) == false)
                return SYSTEM_ERROR;
            if(this->checkIfAfileExist(chemin2) == false)
                return SYSTEM_ERROR;
            std::vector<std::string> test = this->executeCommand(commande+chemin1);
            if(test.size() == 0)
                
                return RUNTIME_ERROR;
            if(this->executing_time>this->timeLimitInSec) 
                return TLE;
            if(this->areTheSame(test,this->readOutput(chemin2)) == false){
                return WRONG_ANSWER;
            }
        }

        return ACCEPTED;            
        
    }

    template <typename T>
  std::string NumberToString ( T Number )
  {
     std::ostringstream ss;
     ss << Number;
     return ss.str();
  }
};//end of TestValidator class



int main(int argc,char **argv){
    TestValidator a;
    std::cout<<a.validate("sol.cpp");
    return 0;
}

