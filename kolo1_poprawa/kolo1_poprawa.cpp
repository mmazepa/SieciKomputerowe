#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
using namespace std;
void createPC(int number)
{
	char pom[10];
	snprintf(pom, sizeof(pom), "%d", number);
	char command[100]="mkdir -p pc";
	strcat(command,pom);
	system(command);
	cout<<"Stworzono folder pc"<<number<<endl;
}

void createRouter(int number)
{
	char pom[10];
	snprintf(pom, sizeof(pom), "%d", number);
	char command[100]="mkdir -p r";
	strcat(command,pom);
	strcat(command,"/etc/zebra");
	system(command);
	cout<<"Stworzono drzewo folderow r"<<number<<endl;
}

void createPCStartup(int number, string ip, string routerIP, string broadcast, string mask, string address, string maskAll)
{
	ofstream pc;
	char name[100]="pc";
	char pom[10];
	snprintf(pom, sizeof(pom), "%d", number);
	strcat(name,pom);
	strcat(name,".startup");
	pc.open(name);
	pc<<"ifconfig eth0 "+ip+" netmask "+mask+" broadcast "+broadcast+" up"<<endl;
	pc<<"route add -net "<<address<<" netmask "<<maskAll<<" gw "<<routerIP<<" dev eth0"<<endl;

	cout<<"Stworzono startup dla PC "<<number<<endl;
	pc.close();
}

void createRouterStartup(int number, int eth, string ip, string broadcast, string mask)
{
	ofstream router;
	char name[100]="r";
	char pom[10];
	snprintf(pom, sizeof(pom), "%d", number);
	strcat(name,pom);
	strcat(name,".startup");
	router.open(name,fstream::app|fstream::out);
	router<<"ifconfig eth"<<eth<<" "+ip+" netmask "+mask+" broadcast "+broadcast+" up"<<endl;
	router.close();
}

void addRouterZebra(int number, string address)
{
	ofstream router;
	char name[100]="r";
	char pom[10];
	snprintf(pom, sizeof(pom), "%d", number);
	strcat(name,pom);
	strcat(name,".startup");
	router.open(name,fstream::app|fstream::out);
	router<<"/etc/init.d/zebra start"<<endl;
	router.close();
	
	router.open("ripd.conf");
	router<<"!"<<endl;
	router<<"hostname ripd"<<endl;
	router<<"password zebra"<<endl;
	router<<"enable password zebra"<<endl;
	router<<"!"<<endl;
	router<<"router rip"<<endl;
	router<<"redistribute connected"<<endl;
	router<<"network "<<address<<endl;
	router<<"!"<<endl;
	router<<"log file /var/log/zebra/ripd.log"<<endl;
	router.close();
	char cpRipd[]="cp ripd.conf ./r";
	strcat(cpRipd,pom);
	strcat(cpRipd,"/etc/zebra/");
	system(cpRipd);
	
	router.open("zebra.conf");
	router<<"! -*- zebra -*-"<<endl;
	router<<"!"<<endl;
	router<<"! zebra configuration file"<<endl;
	router<<"!"<<endl;
	router<<"hostname zebra"<<endl;
	router<<"password zebra"<<endl;
	router<<"enable password zebra"<<endl;
	router<<"!"<<endl;
	router<<"! Static default route sample."<<endl;
	router<<"!"<<endl;
	router<<"!ip route 0.0.0.0/0 203.181.89.241"<<endl;
	router<<"!"<<endl;
	router<<"log file /var/log/zebra/zebra.log"<<endl;
	router.close();
	
	char cpZebra[]="cp zebra.conf ./r";
	strcat(cpZebra,pom);
	strcat(cpZebra,"/etc/zebra/");
	system(cpZebra);
	router.open("daemons");
	router<<"# This file tells the zebra package "<<endl;
	router<<"# which daemons to start."<<endl;
	router<<"# Entries are in the format: <daemon>=(yes|no|priority)"<<endl;
	router<<"# where 'yes' is equivalent to infinitely low priority, and"<<endl;
	router<<"# lower numbers mean higher priority. Read"<<endl;
	router<<"# /usr/doc/zebra/README.Debian for details."<<endl;
	router<<"# Daemons are: bgpd zebra ospfd ospf6d ripd ripngd"<<endl;
	router<<"zebra=yes"<<endl;
	router<<"bgpd=no"<<endl;
	router<<"ospfd=no"<<endl;
	router<<"ospf6d=no"<<endl;
	router<<"ripd=yes"<<endl;
	router<<"ripngd=no"<<endl;
	router.close();
	char cpDaemons[]="cp daemons ./r";
	strcat(cpDaemons,pom);
	strcat(cpDaemons,"/etc/zebra/");
	system(cpDaemons);
	char delZebra[]="rm zebra.conf";
    system(delZebra);
    char delRip[]="rm ripd.conf";
    system(delRip);
    char delDaemons[]="rm daemons";
    system(delDaemons);
    cout<<"Dodano zebre"<<endl;
}
void createLab()
{
	char pom[10];
	ofstream lab;
	ofstream router;
	int number, eth, askPC;
	int numberOfPcs=1;
	string letter, address, address1, maskAll;
	string routerIP, broadcast, pcIP, maskRouter, maskPC;
	lab.open("lab.conf");
	cout<<"Podaj ogolny adres sieci w formacie ip/maska (np. 172.12.4.0/24)"<<endl;
	cin>>address;
	cout<<"Podaj tylko IP sieci (np. 172.12.4.0)"<<endl;
	cin>>address1;
	cout<<"Podaj ogolna maske dla sieci w formacie *.*.*.*"<<endl;
	cin>>maskAll;
	cout<<"Podaj maske dla PCtow"<<endl;
	cin>>maskPC;
	cout<<"Podaj maske dla routerow"<<endl;
	cin>>maskRouter;
	cout<<endl;
	cout<<"Tworzenie lab.conf i startupów"<<endl;
	cout<<"Ile chcesz dodac routerow?"<<endl;
	cin>>number;
	for(int i=1;i<=number;i++)
	{
		createRouter(i);
		cout<<"Ile router "<<i<<" bedzie mial eth?"<<endl;
		cin>>eth;
		for(int j=0;j<eth;j++)
		{
			cout<<"Podaj litere dla routera "<<i<<" dla eth"<<j<<endl;
			cin>>letter;
			lab<<"r"<<i<<"["<<j<<"]=\""<<letter<<"\""<<endl;
			cout<<"Podaj IP dla routera "<<i<<" dla eth "<<j<<endl;
			cin>>routerIP;
			cout<<"Podaj broadcast dla routera "<<i<<" dla eth "<<j<<endl;
			cin>>broadcast;
			cout<<"Czy chcesz dodac PC'ta do tej litery? (1-tak, 2-nie)"<<endl;
			cin>>askPC;
			if(askPC==1)
			{
				createPC(numberOfPcs);
				lab<<"pc"<<numberOfPcs<<"[0]=\""<<letter<<"\""<<endl;
				cout<<"Podaj IP dla PC "<<numberOfPcs<<endl;
				cin>>pcIP;
				createPCStartup(numberOfPcs, pcIP, routerIP, broadcast, maskPC, address1, maskAll);
				numberOfPcs++;
				createRouterStartup(i, j, routerIP, broadcast, maskPC);
			}
			else createRouterStartup(i, j, routerIP, broadcast, maskRouter);
		}
		addRouterZebra(i,address);
	}
	lab.close();	
	cout<<"Stworzono lab.conf"<<endl;
}
int main(int argc, char** argv) {
	createLab();
	return 0;
}
