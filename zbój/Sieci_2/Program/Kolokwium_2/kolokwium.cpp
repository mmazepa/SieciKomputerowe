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
	strcat(command,"/etc");
	system(command);
	cout<<"Stworzono drzewo folderow pc"<<number<<endl;
}

void createRouter()
{
	char command[100]="mkdir -p r1/etc/dhcp3";
	system(command);
	cout<<"Stworzono drzewo folderow r1"<<endl;
}

void createServer()
{
	char command[100]="mkdir -p s1/etc/bind";
	system(command);
	cout<<"Stworzono drzewo folderow s1"<<endl;
}

void createPCStartup(int number, string routerIP)
{
	ofstream pc;
	char name[100]="pc";
	char pom[10];
	snprintf(pom, sizeof(pom), "%d", number);
	strcat(name,pom);
	strcat(name,".startup");
	pc.open(name);
	pc<<"dhclient"<<endl;
	pc<<"route add default gw "+routerIP+" dev eth0";
	pc.close();
	cout<<"Stworzono startup dla pc"<<number<<endl;
}

void createRouterStartup(string ip0, string ip1, string broadcast1, string broadcast2, string mask)
{
	ofstream router;
	router.open("r1.startup");
	router<<"ifconfig eth0 "+ip0+" netmask "+mask+" broadcast "+broadcast1+" up"<<endl;
	router<<"ifconfig eth1 "+ip1+" netmask "+mask+" broadcast "+broadcast2+" up"<<endl;
	router<<"/etc/init.d/dhcp3-server start";
	router.close();
	cout<<"Stworzono startup dla r1"<<endl;
}

void createServerStartup(string serverIP, string routerIP, string mask, string broadcast)
{
	ofstream server;
	server.open("s1.startup");
	server<<"ifconfig eth0 "+serverIP+" netmask "+mask+" broadcast "+broadcast+" up"<<endl;
	server<<"route add default gw "+routerIP+" dev eth0"<<endl;
	server<<"/etc/init.d/bind start";
	server.close();
	cout<<"Stworzono startup dla s1"<<endl;
}

void createLab(int number)
{
	char pom[10];
	ofstream lab;
	ofstream lab2;
	lab.open("lab.conf");
	lab2.open("lab.dep");
	lab2<<"s1:r1"<<endl;
	for(int i=1;i<=number;i++)
	{
		snprintf(pom, sizeof(pom), "%d", i);
		lab<<"pc"<<pom<<"[0]=\"A\""<<endl;
		lab2<<"pc"<<pom<<":s1"<<endl;
	}
	lab<<endl<<"r1[0]=\"A\""<<endl;
	lab<<"r1[1]=\"B\""<<endl;
	lab<<"r1[mem]=24"<<endl;
	lab<<endl<<"s1[0]=\"B\""<<endl;
	lab<<"s1[mem]=32"<<endl;
	lab.close();	
	lab2.close();
	cout<<"Stworzono lab.conf i lab.dep"<<endl;
}

void delDHCP()
{
	char cp[]="cp dhcpd.conf ./r1/etc/dhcp3";
    system(cp);
    char del[]="rm dhcpd.conf";
    system(del);
    cout<<"Postawiono server DHCP"<<endl;
}

void createDHCP(string subnet, string rangeA, string rangeB, string mask)
{
	ofstream dhcp;
	dhcp.open("dhcpd.conf",fstream::app|fstream::out);
	dhcp<<"subnet "<<subnet<<" netmask "<<mask<<endl;
	dhcp<<"{"<<endl<<"	range "<<rangeA<<" "<<rangeB<<";"<<endl;
    dhcp<<"	default-lease-time 600;"<<endl<<"	max-lease-time 7200;"<<endl<<"}";
    dhcp.close();
}

void createResolv(int number, string serverIP, string zoneName)
{
	ofstream resolv;
	char pom[10];
	resolv.open("resolv.conf");
	resolv<<"nameserver "<<serverIP<<endl;
	resolv<<"search "<<zoneName;
	resolv.close();
	for(int i=1;i<=number;i++)
	{
		snprintf(pom, sizeof(pom), "%d", i);
		char commandPC[100]="cp resolv.conf ./pc";
		strcat(commandPC,pom);
		strcat(commandPC,"/etc");
		system(commandPC);
	}
	char commandRouter[100]="cp resolv.conf ./r1/etc/";
	system(commandRouter);
	char commandServer[100]="cp resolv.conf ./s1/etc/";
	system(commandServer);
	char commandDel[]="rm resolv.conf";
	system(commandDel);
	cout<<"Stworzono pliki resolv.conf"<<endl;
	
}

void createDNS(string zoneName)
{
	ofstream dns;
	dns.open("tmp",fstream::app|fstream::out);
	int n=-1;
	string name, ip;
	cout<<"Ile chcesz dodac nazw do DNS?"<<endl;
	while(n<0)
		cin>>n;
	dns<<"$TTL   60000"<<endl;
	dns<<"@               IN      SOA     ns."<<zoneName<<"    root.ns."<<zoneName<<" ("<<endl;
	dns<<"                        1111111101 ; serial"<<endl;
	dns<<"                        28800 ; refresh"<<endl;
	dns<<"                        14400 ; retry"<<endl;
	dns<<"                        3600000 ; expire"<<endl;
	dns<<"                        0 ; negative cache ttl"<<endl;
	dns<<"                        )"<<endl;
	dns<<"@               IN      NS      ns."<<zoneName<<"."<<endl;
	for(int i=0;i<n;i++)
	{
		cout<<"Podaj nazwe"<<endl;
		cin>>name;
		cout<<"Podaj IP"<<endl;
		cin>>ip;
		dns<<name<<"		IN	A	"<<ip<<endl;
	}
	dns.close();
	dns.open("named.conf",fstream::app|fstream::out);
	
	dns<<"options {"<<endl;
    dns<<"    directory \"/var/cache/bind\";"<<endl;
	dns<<"	min-roots 1;"<<endl;
	dns<<"};"<<endl;
	dns<<"logging {"<<endl;
	dns<<"        category lame-servers { null; };"<<endl;
	dns<<"	};"<<endl;
	dns<<"zone \".\" {"<<endl;
	dns<<"        type hint;"<<endl;
	dns<<"        file \"/etc/bind/db.root\";"<<endl;
	dns<<"	};"<<endl;
	dns<<"zone \"localhost\" {"<<endl;
	dns<<"	type master;"<<endl;
	dns<<"	file \"/etc/bind/db.local\";"<<endl;
	dns<<"	};"<<endl;
	dns<<"zone \"127.in-addr.arpa\" {"<<endl;
	dns<<"       type master;"<<endl;
	dns<<"        file \"/etc/bind/db.127\";"<<endl;
	dns<<"};"<<endl;
	dns<<"zone \"0.in-addr.arpa\" {"<<endl;
	dns<<"	type master;"<<endl;
	dns<<"	file \"/etc/bind/db.0\";"<<endl;
	dns<<"};"<<endl;
	dns<<"zone \"255.in-addr.arpa\" {"<<endl;
	dns<<"        type master;"<<endl;
	dns<<"        file \"/etc/bind/db.255\";"<<endl;
	dns<<"};"<<endl;
	dns<<"// Przypisanie nazwy strefy do pliku z ustawieniami"<<endl;
	dns<<"zone \""<<zoneName<<"\" {"<<endl;
	dns<<"        type master;"<<endl;
	dns<<"        file \"/etc/bind/db.<<zoneName<<\";"<<endl;
	dns<<"};"<<endl;
	dns.close();
	char cpNamed[]="cp named.conf ./s1/etc/bind";
	system(cpNamed);
	char cpZone[100]="cp tmp ./s1/etc/bind/db.";
	strcat(cpZone,zoneName.c_str());
	system(cpZone);
	char delNamed[]="rm named.conf";
	char delZone[]="rm tmp";
    system(delNamed);
    system(delZone);
    cout<<"Serwer DNS postawiony"<<endl;
}

int main(int argc, char** argv) 
{
	int number;
	int subnets=0;
	string serverIP, routerIP1, routerIP2, mask, broadcast1, broadcast2, zoneName;
	string subnet, rangeA, rangeB;
	cout<<"Podaj liczbe PC'tow"<<endl;
	cin>>number;
	cout<<"Podaj pierwsze IP routera"<<endl;
	cin>>routerIP1;
	cout<<"Podaj pierwszy broadcast"<<endl;
	cin>>broadcast1;
	cout<<"Podaj drugie IP routera"<<endl;
	cin>>routerIP2;
	cout<<"Podaj drugi broadcast"<<endl;
	cin>>broadcast2;
	cout<<"Podaj IP servera"<<endl;
	cin>>serverIP;
	cout<<"Podaj maske"<<endl;
	cin>>mask;
	createLab(number);
	createServer();
	createRouter();
	createServerStartup(serverIP, routerIP2, mask, broadcast2);
	createRouterStartup(routerIP1, routerIP2, broadcast1, broadcast2, mask);
	for(int i=1;i<=number;i++)
	{
		createPCStartup(i,routerIP1);
		createPC(i);
	}
	cout<<"Ile chcesz stworzyc podsieci?"<<endl;
	while(subnets<1)
		cin>>subnets;
	for(int i=1;i<=subnets;i++)
	{
		cout<<"Podaj adres podsieci nr "<<i<<endl;
		cin>>subnet;
		cout<<"Podaj poczatek zakresu dla podsieci nr "<<i<<endl;
		cin>>rangeA;
		cout<<"Podaj koniec zakresu dla podsieci nr "<<i<<endl;
		cin>>rangeB;
		createDHCP(subnet, rangeA, rangeB, mask);
	}
	delDHCP();
	cout<<"Podaj nazwe strefy"<<endl;
	cin>>zoneName;
	createDNS(zoneName);
	createResolv(number,serverIP,zoneName);
	getchar();
	system("clear");
	cout<<"Super, zdales/zdalas"<<endl;
	return 0;
}
