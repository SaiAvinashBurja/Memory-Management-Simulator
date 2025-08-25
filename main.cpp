#include <bits/stdc++.h>
#include <fstream>
#include <iomanip> 
#include <ctime>
#include <cctype>  
#include <sstream>
using namespace std;
string mode;
vector<vector<int>> Mainmem,SwapMem;//values in memory
vector<int> Mainid,Swapid;//pid of the page
list<int> fifo;
vector<bool> MainmemAvl,SwapMemAvl;//page is free(true) or not(false).
int M,V,P;
int pnum=1;//to assign pid for each process
void swappages(int,int,int);//to swap 2 pages in main and swap memory
bool loadfail=false,runfail=false;//no use 
int avlPagesMain,avlPagesSwap;//no of pages available in mainmemory and swapmemory respectively
map<int,int> indexes;//fot lru. map the page number in main memory to its usage time 
int ind=0;
ofstream outfile;
//for each entry of page table
class pagetableentry {
    int pid;
    int logadd;
    int phyadd;
    bool isinmain;
    public:
    pagetableentry(){

    }
    pagetableentry(int p,int la,int pa,bool m){
        pid=p;
        logadd=la;
        phyadd=pa;
        isinmain=m;
    }
    int getphyadd(){
        return phyadd;
    }
    int getpid(){
        return pid;
    }
    bool chkinmain(){
        return isinmain;
    }
    void setpa(int pa){
        phyadd=pa;
    }
    void setbool (bool b){
        isinmain=b;
    }
};
void swappages2(int x, int pid, vector<pagetableentry> &pagetable){
    if (mode=="fifo"){ //algorithm for fifo 
        if (avlPagesMain==0){//main mem is full, then swap pages 
            int t=fifo.front();//take care of queue 
            fifo.pop_front();
            fifo.push_back(t);
            swappages(x,t,pid);//to swap the contents 
        }
        else {//main mem has some pages free
        for (int i=0;i<(M*1024)/P;i++){
            if (MainmemAvl[i]==true){
                Mainmem[i]=SwapMem[pagetable[x/P].getphyadd()];
                MainmemAvl[i]=false;
                avlPagesMain--;
                avlPagesSwap++;
                SwapMemAvl[i]=true;
                Mainid[i]=pid;
                Swapid[pagetable[x/P].getphyadd()]=-1;
                pagetable[x/P].setbool(true);
                pagetable[x/P].setpa(i);
                fifo.push_back(i);
                break;
            }
        }
        }
    }
    else if (mode=="lru"){//algo for lru 
        if (avlPagesMain==0){//main mem is full
            int maxi=1e9;
            int vall;
            for (int it1=0;it1<(M*1024)/P;it1++){
                if (indexes[it1]<maxi){
                    maxi=indexes[it1];
                    vall=it1;
                }
            }         //update the access map 
            swappages(x,vall,pid);//swap the contents of page 
        }
    else {//main mem has free pages 
        for (int i=0;i<(M*1024)/P;i++){
            if (MainmemAvl[i]==true){
                Mainmem[i]=SwapMem[pagetable[x/P].getphyadd()];
                MainmemAvl[i]=false;
                avlPagesMain--;
                avlPagesSwap++;
                SwapMemAvl[i]=true;
                Mainid[i]=pid;
                Swapid[pagetable[x/P].getphyadd()]=-1;
                pagetable[x/P].setbool(true);
                pagetable[x/P].setpa(i);
                break;
            }
        }
    }
    }
}


//for each process
class process{
    int pid;
    int size;
    bool isactive ;

    public:
    vector<pagetableentry> pagetable;
    vector<string> instructions;
    process(){
        size=0;
        pid=-1;
        isactive=false;
    }
    process(int s,int p,vector<pagetableentry> pe ){
        size=s;
        pid=p;
        pagetable=pe;
        isactive= true;
    }
    void runprocess (){
        for (auto it: instructions){
            string ins;
            istringstream iss(it);
            iss>>ins;
            if (ins=="add"){
                int x,y,z;
                char comma;
                iss>>x>>comma>>y>>comma>>z;
                int valx,valy,valz;
                //check for logical address out of bounds
                if (x<0 || x>=size*1024){
                    runfail=true;
                    outfile<<"Invalid Memory Address "<<x<< " specified for process id " <<pid<<endl;
                    return;
                }
                if (y<0 ||y>=size*1024){
                    runfail=true;
                    outfile<<"Invalid Memory Address "<<y<< " specified for process id " <<pid<<endl;
                    return;
                }
                if (z<0 || z>=size*1024){
                    runfail=true;
                    outfile<<"Invalid Memory Address "<<z<< " specified for process id " <<pid<<endl;
                    return;
                }  outfile<<"Result: ";
                if (pagetable[x/P].chkinmain()){ // if page is in main mem 
                    valx=Mainmem[pagetable[x/P].getphyadd()][x%P];
                    ind+=1;
                    indexes[pagetable[x/P].getphyadd()]=ind;
                }  else {// in swap mem 
                    swappages2(x,pid,pagetable);
                    ind+=1;
                    indexes[pagetable[x/P].getphyadd()]=ind;
                    valx=Mainmem[pagetable[x/P].getphyadd()][x%P];
                }
                if (pagetable[y/P].chkinmain()){ // if page is in main mem 
                    valy=Mainmem[pagetable[y/P].getphyadd()][y%P];
                    ind+=1;
                    indexes[pagetable[y/P].getphyadd()]=ind;
                }  else {// in swap mem 
                    swappages2(y,pid,pagetable);
                    ind+=1;
                    indexes[pagetable[y/P].getphyadd()]=ind;
                    valy=Mainmem[pagetable[y/P].getphyadd()][y%P];
                   } 
                if (pagetable[z/P].chkinmain()){ // if page is in main mem 
                    valz=Mainmem[pagetable[z/P].getphyadd()][z%P];
                    ind+=1;
                    indexes[pagetable[z/P].getphyadd()]=ind;
                } else {// in swap mem 
                    swappages2(z,pid,pagetable);
                    ind+=1;
                    indexes[pagetable[z/P].getphyadd()]=ind;
                  } 
                valz=valx+valy;       
                Mainmem[pagetable[z/P].getphyadd()][z%P]=valz;
                outfile<<"Value in ";
                outfile<< "addr" << x << " = "<< valx<<" , "<<"addr"<<y<<" = "<< valy<<" , "<< "addr"<<z<<" = "<< Mainmem[pagetable[z/P].getphyadd()][z%P]<<endl;
            }
            else if (ins=="sub"){
                int x,y,z;
                char comma;
                iss>>x>>comma>>y>>comma>>z;
                if (x<0 || x>=size*1024){
                    runfail=true;
                    outfile<<"Invalid Memory Address "<<x<< " specified for process id " <<pid<<endl;
                    return;
                }
                if (y<0 ||y>=size*1024){
                    runfail=true;
                    outfile<<"Invalid Memory Address "<<y<< " specified for process id " <<pid<<endl;
                    return;
                }
                if (z<0 || z>=size*1024){
                    runfail=true;
                    outfile<<"Invalid Memory Address "<<z<< " specified for process id " <<pid<<endl;
                    return;
                }  outfile<<"Result: ";
                int valx,valy,valz;
                if (pagetable[x/P].chkinmain()){
                    valx=Mainmem[pagetable[x/P].getphyadd()][x%P];
                    ind+=1;
                    indexes[pagetable[x/P].getphyadd()]=ind;
                }  else {
                    swappages2(x,pid,pagetable);
                    ind+=1;
                    indexes[pagetable[x/P].getphyadd()]=ind;
                    valx=Mainmem[pagetable[x/P].getphyadd()][x%P];
                }
                if (pagetable[y/P].chkinmain()){
                    valy=Mainmem[pagetable[y/P].getphyadd()][y%P];
                    ind+=1;
                    indexes[pagetable[y/P].getphyadd()]=ind;
                }  else {
                    swappages2(y,pid,pagetable);
                    ind+=1;
                    indexes[pagetable[y/P].getphyadd()]=ind;
                    valy=Mainmem[pagetable[y/P].getphyadd()][y%P];
                   } 
                if (pagetable[z/P].chkinmain()){
                    valz=Mainmem[pagetable[z/P].getphyadd()][z%P];
                    ind+=1;
                    indexes[pagetable[z/P].getphyadd()]=ind;
                } else {
                    swappages2(z,pid,pagetable);
                    ind+=1;
                    indexes[pagetable[z/P].getphyadd()]=ind;
                  } 
                valz=valx-valy;
                Mainmem[pagetable[z/P].getphyadd()][z%P]=valz;
                outfile<<"Value in ";
                outfile<< "addr" << x << " = "<< valx<<" , "<<"addr"<<y<<" = "<< valy<<" , "<< "addr"<<z<<" = "<< valz<<endl;
            }
            else if (ins=="print"){
                int x;
                iss>>x;
                if (x<0 || x>=size*1024){
                    runfail=true;
                    outfile<<"Invalid Memory Address "<<x<< " specified for process id " <<pid<<endl;
                    return;
                }
                int valx;
                outfile<<"Result: ";
                if (pagetable[x/P].chkinmain()){
                    valx=Mainmem[pagetable[x/P].getphyadd()][x%P];
                    ind+=1;
                    indexes[pagetable[x/P].getphyadd()]=ind;
                } else { 
                    swappages2(x,pid,pagetable);
                    ind+=1;
                    indexes[pagetable[x/P].getphyadd()]=ind;
                    valx=Mainmem[pagetable[x/P].getphyadd()][x%P];
                  }
                outfile<<"Value in addr "<<x<<" = "<<valx<<endl;
            }
            else if (ins=="load"){
                int val,x;
                char comma;
                iss>>val>>comma>>x;
                
                if (x<0 || x>=size*1024){
                    
                    runfail=true;
                    outfile<<"Invalid Memory Address "<<x<< " specified for process id " <<pid<<endl;
                    return;
                } outfile<<"Result: ";
                if (pagetable[x/P].chkinmain()){
                    
                    Mainmem[pagetable[x/P].getphyadd()][x%P]=val;
                    ind+=1;
                    indexes[pagetable[x/P].getphyadd()]=ind;
                } else { 
                        swappages2(x,pid,pagetable);
                        Mainmem[pagetable[x/P].getphyadd()][x%P]=val;              
                    ind+=1;
                    indexes[pagetable[x/P].getphyadd()]=ind;
                    
                  } 
                outfile<<"Value of "<<Mainmem[pagetable[x/P].getphyadd()][x%P]<<" is now stored in addr "<<x<<endl; 
            }
        }
    }

    void killprocess(){
        for (auto it: pagetable){
            int pa = it.getphyadd();
            if (it.chkinmain()){//if page is in main 
                MainmemAvl[pa]=true;//free main page
                Mainid[pa]=-1;//no process 
                avlPagesMain++;
                fifo.remove(pa);
                indexes[pa]=-1;
            }
            else {
                SwapMemAvl[pa]=true;
                Swapid[pa]=-1;
                avlPagesSwap++;
            }
        }
        isactive=false;
        outfile<<"killed "<<pid<<endl;

    }

    bool isPractive(){
        return isactive;
    }

};
map<int,process> allProc;//maps each process wih its pid
void swappages (int x,int t,int id){//exchanges data and sets phyiscal address for both pages 
    //x-> logical address of the page in swap mem
    //t-> page number of tha page that is to be removed from main mem
    //id-> pid of the page in swap mem 
    process &Pr=allProc[id];
    int swapadd = Pr.pagetable[x/P].getphyadd();
    vector<int> swapPage = SwapMem[swapadd];
    vector<int> phypage = Mainmem[t];
    Mainmem[t]=swapPage;
    SwapMem[swapadd]=phypage;
    int m= Mainid[t];
    process &Pr1 = allProc[m];
    int logadd;
    int n=Pr1.pagetable.size();
    for (int i=0;i<n;i++){
        if (Pr1.pagetable[i].getphyadd()==t && Pr1.pagetable[i].chkinmain()){
            logadd=i;
            break;
        }
    }
    Pr1.pagetable[logadd].setpa(swapadd);
    Pr1.pagetable[logadd].setbool(false);
    Pr.pagetable[x/P].setbool(true);
    Pr.pagetable[x/P].setpa(t);
    Mainid[t]=id;
    Swapid[swapadd]=m;
}


void loadprocess(const string &tobeloaded) {
    ifstream file(tobeloaded);
    if (!file.is_open()) {
        outfile << tobeloaded << " could not be loaded - file does not exist" << endl;
        return ;
    }
    int sizeoffile;
    file>>sizeoffile;
    int PagesReq = (sizeoffile*1024)/P;
    if (PagesReq> avlPagesMain+avlPagesSwap) {  // when memory is not sufficient 
        outfile<<tobeloaded<<" could not be loaded - memory is full"<<endl;
        return;
    } 
    vector<pagetableentry> pt;
    process Pr(sizeoffile,pnum,pt);
    
    int i=0;
    for (int j=0;j<(M*1024)/P;j++){//first search for empty pages in main memory
        if ( i<PagesReq && avlPagesMain>0){// i>=PagesReq -> already all pages for process are allocated so break 
            if (MainmemAvl[j]){//if avlPagesMain=0 , main memory is full, break and search in swap memory
            pagetableentry pe(pnum,i,j,true);
            Pr.pagetable.push_back(pe);
            avlPagesMain--;
            MainmemAvl[j]=false;
            fifo.push_back(j);
            Mainid[j]=pnum;
            i++;
            ind+=1;
            indexes[j]=ind;
            }
            else continue;
        }
        else break;
    } 
    for (int j=0;j<(V*1024)/P;j++){
        if (  i<PagesReq){// i>=PagesReq -> already all pages for process are allocated so break 
            if (SwapMemAvl[j]){ 
            pagetableentry pe(pnum,i,j,false);
            Pr.pagetable.push_back(pe);
            avlPagesSwap--;
            SwapMemAvl[j]=false;
            Swapid[j]=pnum;
            i++;}
            else continue;
        }
        else break;
    }
    
    string ins;
    while(getline(file,ins)){//store all instructions 
        bool chk=false;
        for (char c:ins){
            if (isspace(static_cast<unsigned char>(c))) continue;
            else {//if getline gives empty line, just ignore it. do not store in instructions 
                chk=true;
                break;
            }
        }
        if (chk){
            Pr.instructions.push_back(ins); 
        }
    }
    
    allProc[pnum]=Pr;
    outfile<<tobeloaded<<" is loaded and is assigned process id "<<pnum<<endl;
    pnum++;
    file.close();
}


int main(int argc, char* argv[]){
    string modes = argv[0];
    M = atoi(argv[2]);
    V = atoi(argv[4]);
    P = atoi(argv[6]);
    string infilename=argv[8];
    string outfilename=argv[10];

    if (modes=="./fifo") mode="fifo";
    else if (modes=="./lru") mode="lru";
    Mainmem.assign((M*1024)/P,vector<int>(P,0));
    SwapMem.assign((V*1024)/P,vector<int>(P,0));
    MainmemAvl.assign((M*1024)/P,true);
    SwapMemAvl.assign((V*1024)/P,true);
    avlPagesMain=(M*1024)/P;
    avlPagesSwap=(V*1024)/P;
    Mainid.assign((M*1024)/P,-1);
    Swapid.assign((V*1024)/P,-1);
    for (int i=0;i<avlPagesMain;i++){
        indexes[i]=0;
    } 
    ifstream infile(infilename);  
    outfile.open(outfilename, ios::app);
    string command;
    while (getline(infile, command)) {
        istringstream iss(command);
        string cmd;
        iss >> cmd;

        if (cmd == "load") {
            string tobeloaded;
            while (iss >> tobeloaded) {
                loadprocess(tobeloaded);
            }
        } 
        else if (cmd == "run") {
            int givenpid;
            iss>>givenpid;
            if (allProc.find(givenpid)==allProc.end()){
                outfile<<"invalid PID "<<givenpid<<endl;
                continue;
            }
            process &Pr=allProc[givenpid];
            if (!Pr.isPractive()) {
                outfile<<"invalid PID "<<givenpid<<endl;//processes that are killed
                continue;
            }
            Pr.runprocess();
        } 
        else if (cmd=="kill"){
            int givenpid;
            iss>>givenpid;
            if (allProc.find(givenpid)==allProc.end()){
                outfile<<"invalid PID "<<givenpid<<endl;
                continue;
            }
            process &Pr=allProc[givenpid];
            if (!Pr.isPractive()) {
                outfile<<"invalid PID "<<givenpid<<endl;//processes that are already killed
                continue;
            }
            Pr.killprocess();
        }
        else if (cmd=="listpr"){
            for (int i=1;i<pnum;i++){
                process &Pr=allProc[i];
                if (Pr.isPractive()){
                    outfile<<i<<" ";
                }
                outfile<<endl;
            }
        }
        else if (cmd == "pte"){
            int givenpid;
            iss>>givenpid;
            if (givenpid<=0 || givenpid>=pnum ){
                outfile<<"Invalid PID "<<givenpid<<endl;
                continue;
            }
            process &Pr=allProc[givenpid];
            if (!Pr.isPractive()){
                outfile<<"invalid PID "<<givenpid<<endl;//killed processes 
                continue;
            }
            string outputfile;
            iss>>outputfile;
            ofstream outfile1(outputfile,ios::app);
            time_t now = time(0);
            tm *ltm = localtime(&now);
            outfile1 << 1900 + ltm->tm_year << "-"
            << 1 + ltm->tm_mon << "-"
            << ltm->tm_mday << " "
            << ltm->tm_hour << ":"
            << ltm->tm_min << ":"
            << ltm->tm_sec << endl;
            int n=Pr.pagetable.size();
            for (int i=0;i<n;i++){
                outfile1<<i<<" "<<Pr.pagetable[i].getphyadd()<<" "<<Pr.pagetable[i].chkinmain()<<endl;
            }
            outfile1.close();
        }
        else if (cmd=="pteall"){
            string outputfile;
            iss>>outputfile;
            ofstream outfile1(outputfile,ios::app);
            time_t now = time(0);
            tm *ltm = localtime(&now);
            outfile1 << 1900 + ltm->tm_year << "-"
            << 1 + ltm->tm_mon << "-"
            << ltm->tm_mday << " "
            << ltm->tm_hour << ":"
            << ltm->tm_min << ":"
            << ltm->tm_sec << endl;
            for (int i=1;i<pnum;i++){
                process &Pr=allProc[i];
                if (!Pr.isPractive()) continue;
                int n=Pr.pagetable.size();
                for (int j=0;j<n;j++){
                    outfile1<<i<<" "<<j<<" "<<Pr.pagetable[j].getphyadd()<<" "<<Pr.pagetable[j].chkinmain()<<endl;
                }
            }
            outfile1.close();

        }
        else if (cmd == "print"){
            int memloc,length;
            iss>>memloc>>length;
            for (int i=memloc;i<memloc+length;i++){
                outfile<<"value of"<<i<<": "<<Mainmem[i/P][i%P]<<endl;
            }
        }
        else if (cmd=="exit"){
            infile.close();
            outfile.close();
            exit(0);
            break;
        }
    }


    return 0;
}