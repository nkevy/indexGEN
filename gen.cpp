#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <fstream>

#include <sys/stat.h>
#include <sys/types.h>

#include <plog/Log.h>
#define MAX_NAME 256
using namespace std;

int moveAll(ofstream &fhtml);
int setHTML(ofstream &fhtml);
int matchType(string fn);
int fmove(string fn);
int genDTAG(string fname, ofstream &fhtml);
int genITAG(string fname, ofstream &fhtml);
int getServer();

int main(){
	//create log
	plog::init(plog::debug,"log.csv",10000000,3);
	PLOGI<<"main start";
	//create public folder
	if (mkdir("public",ACCESSPERMS)){
		PLOGF<<"cannot create public dir";
		exit(0);
	}
	//create index.html
	ofstream index;
	index.open("index.html");
	if (!(index.is_open())){
		PLOGF<<"cannot create index.html";
		exit(0);
	}
	//setup html
	setHTML(index);
	//move all .jpg .png .docx .txt files into public folder
	//add all files to index.html
	if (moveAll(index)){
		PLOGF<<"cannot move files";
		exit(0);
	}
	//close index.html and end
	index<<endl<<"</html>"<<endl;
	index.close();
	//download and install appachie server??
	if(0){
		getServer();
	}
	PLOGI<<"main end";
	return 0; 
}
int setHTML(ofstream &fhtml){
	string add = "<!DOCTYPE html>\n<html>\n<style>\nimg{ width:128px;height:128px }\n</style>\n";
	fhtml<<add;
	return 0;
}
string toLower(string ss){
	string ret;
	for (unsigned i=0;i<ss.length();i++){
		string add(1,tolower(ss.at(i)));
		ret=ret+add;
	}
	return ret;
}
int matchType(string fn){
	string type[] = {".jpg",".png",".docx",".txt",".pdf"};
	fn = toLower(fn);
	for(unsigned i=0;i<5;i++){
		string chk(type[i]);
		if (fn.find(chk)!=string::npos) return i;
	}
	return -1;
}
int fmove(string fn){
	if (matchType(fn)!=-1){
		string cmd = "mv "+fn+" public/"+fn;
		system(cmd.c_str());
		return 1;
	}
	//fail
	return 0;
}
int moveAll(ofstream &fhtml){
	char path[MAX_NAME] = {0};
	struct stat statBuf;
	DIR *d;
	struct dirent *dir;
	try{
		d = opendir("./");
		while( (dir=readdir(d)) != NULL){
			memset(path,0,MAX_NAME);
			snprintf(path,MAX_NAME,"%s%s","./",dir->d_name);
			if (stat(path,&statBuf)<0){
				PLOGE<<"file error";
				continue;
			}
			// handle regular files
			if (S_ISREG(statBuf.st_mode)){
				//move files
				fmove(dir->d_name);
				//add files to index.html
				int flag = matchType(dir->d_name);
				PLOGI<<flag<<" "<<dir->d_name;
				if (-1!=flag&&2>flag){
					//img
					if(genITAG(dir->d_name,fhtml)){
						PLOGF<<"generate "<<dir->d_name;
					}//err
				}else if (2<=flag){
					//doc
					if(genDTAG(dir->d_name,fhtml)){
						PLOGF<<"generate "<<dir->d_name;
					}//err
				}//fi
			}//fi
		}//while has next file name
		//close directory
		closedir(d);
	}catch (const exception& e){
		PLOGF<<e.what();
		return 1;
	}catch (...){
		return 1;
	}
	return 0;
}
int genITAG(string fname, ofstream &fhtml){
	ifstream f(fname.c_str());
	if(f.bad()){ //error
		PLOGE<<"file "<<fname<<" not found";
		return 1;
	}// else print html code for img download
	fhtml<<"<a href=\"public/"<<fname<<"\" download> <img src=\"public/"<<fname<<"\"></a>"<<endl;
	return 0;
}
int genDTAG(string fname, ofstream &fhtml){
	ifstream f(fname.c_str());
	if(f.bad()){//error
		PLOGE<<"file"<<fname<<" not found";
		return 1;
	}//else print html code for download
	fhtml<<"<a href=\"public/"<<fname<<"\"download>click here for "<<fname<<"</a>"<<endl;
	return 0;
}
int getServer(){
	string cmd;
	cmd = "sudo apt-get update && sudo apt-get install apache2 php5 libapache2-mod-php5";
	system(cmd.c_str());
	//now edit /etc/apache2/sites-enabled/000-default
	//change AllowOveride None to AllowOveride ALL
	cmd = "sudo service apache2 restart";
	system(cmd.c_str());
	//move to /var/ and change /www
	cmd = "sudo chmod 777 /www";
	system(cmd.c_str());
	//go to router and enable port forward
	//get domain name 
	//note use www.no-ip.com/ to update a changing ip
	return 0;
}
