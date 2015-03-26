#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
size_t status();
const size_t N=1<<20;


main(int argc, char**argv)
{
	bool tail=0;
	int opt;
	while((opt=getopt(argc,argv,"01t")) != -1)
	switch(opt) {
		case '0': return 0;
		case '1': return 1;
		case 't': tail=1; break;
		default : return 1;
	}
	
	
	if(optind >= argc) { fprintf(stderr, "argument missing\n"); return 1; }	
	size_t l=atol(argv[optind]);
	
	size_t L=N/l; if(!L) L=1;
	char** p=new char*[L];
		
	for(size_t i=0; i < L; ++i)
		p[i]=new char[l];
	auto s1=status();
	if(tail)
		auto r=new char;

	for(size_t i=0; i < L; ++i)
		delete[] p[i];
	auto s2=status();
	delete p;
		
	return s2 < s1;
}



size_t status()
{
	char stat[256];
	sprintf(stat, "/proc/%u/stat", getpid());
	FILE *f=fopen(stat, "r");
	if(!f) { perror(stat); exit(1); }

	for(int i=0; i < 22; ++i) fscanf(f, "%s", stat);
	size_t vss;
	fscanf(f, "%lu", &vss);
	fclose(f);
	return vss;
}
	
	


	
