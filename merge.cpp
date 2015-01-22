
#include<cstdio>
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cmath>
#include<cstring>
#include<limits.h>
#include<string.h>
#include<algorithm>
#include<vector>
#include<string>
#include<set>
#include<map>
#include<queue>
#include<utility>
#include<sstream>
#include<time.h>
using namespace std;

#define FOR(i,a,b) 	for(int i= (int )a ; i < (int )b ; ++i)
#define rep(i,n) 	FOR(i,0,n)
#define ALL(x) 		x.begin(),x.end()
#define LET(x,a)	__typeof(a) x(a)
#define IFOR(i,a,b) 	for(LET(i,a);i!=(b);++i)
#define EACH(it,v) 	IFOR(it,v.begin(),v.end())
#define pb 		push_back
#define sz(x) 		int(x.size())
#define mp 		make_pair
#define fill(x,v)	memset(x,v,sizeof(x))
#define max(a,b)	((a)>(b)?(a):(b))
#define min(a,b)	((a)<(b)?(a):(b))
#define pi(n)		printf("%d ",n)
#define pil(n)		printf("%d\n",n)
#define directory "/home/himanshu/mini_project/"
typedef pair<int,int> PI;
typedef vector<int> VI;
typedef vector<VI> VVI;
typedef long long LL;
map<string,vector<pair<string,PI> > > Index;
map<string,string> TITLE;
char file_name[100],path[1000],word[20000000],temp[20000000],title[2000],id[20],loc[1000];
char sorted_chunks[1000];
int no_files,done[500];
clock_t PREV;
map<string,pair<string,VI> > INDEX;
map<string,LL> secondary_index;
void merge_index()
{
	PREV = clock();
	printf("Merging Index\n");
	FILE *f[no_files+1],*F,*W,*S;
	int files_done=0;
	F = fopen(loc,"rb");
	LL word_done = 0;
	int sparse=100;
	strcpy(path,directory);
	strcat(path,"INDEX");
	W = fopen(path,"wb");
	strcpy(path,directory);
	strcat(path,"secondary_index");
	S = fopen(path,"wb");
	VI Copy;
	map<string,pair<string,VI> >::iterator it;
	rep(i,no_files)
	{
		fscanf(F,"%s",file_name);
		strcpy(path,directory);
		strcat(path,"sorted_chunks/");
		strcat(path,file_name);
		f[i]=fopen(path,"rb");
		Copy.pb(i);
	}
	while(files_done<no_files)
	{
		rep(i,sz(Copy))
		{
			if(done[Copy[i]])
				continue;
			if(fscanf(f[Copy[i]],"%s %[^\n]",word,temp)==EOF)
			{
				done[Copy[i]]=1;
				files_done++;
				fclose(f[Copy[i]]);
				continue;
			}
			INDEX[word].first += temp;
			INDEX[word].first += ' ';
			INDEX[word].second.pb(Copy[i]);
		}
		if(files_done==no_files)
			break;
		Copy.clear();
		it = INDEX.begin(); // taking first element and inserting in INDEX file so as to make a single sorted file 
		if(word_done%sparse==0)
			fprintf(S,"%s %lld\n",it->first.c_str(),(LL)ftell(W)); // in Secondary file word with the tell pointer for primary 
		fprintf(W,"%s %s\n",it->first.c_str(),it->second.first.c_str());
		rep(i,sz(it->second.second))
			Copy.pb(it->second.second[i]);
		INDEX.erase(INDEX.begin());
		word_done++;
	}
	printf("%f\n",(float(clock()-PREV))/CLOCKS_PER_SEC);
	INDEX.clear();
	fclose(W);
	fclose(S);
	fclose(F);
}
void merge_title_id()
{
	PREV = clock();
	printf("Merging Title ID\n");
	FILE *f[no_files+1],*F,*W,*S;
	int files_done=0;
	F = fopen(loc,"rb");
	LL word_done = 0;
	int sparse=100;
	strcpy(path,directory);
	strcat(path,"TITLE-ID"); // for the primary index
	W = fopen(path,"wb");
	strcpy(path,directory);
	strcat(path,"title_id_secondary_index"); // for secondary title 
	S = fopen(path,"wb");
	rep(i,no_files)
	{
		fscanf(F,"%s",file_name);
		strcpy(path,directory);
		strcat(path,"Id-Title/");
		strcat(path,file_name);
		f[i]=fopen(path,"rb");
		while(fscanf(f[i],"%s %[^\n]",id,title)!=EOF)
		{
			word_done++;
			if(word_done%sparse==0)
				fprintf(S,"%s %lld\n",id,(LL)ftell(W));
			fprintf(W,"%s %s\n",id,title);
		}
		fclose(f[i]);
	}
	printf("%f\n",(float(clock()-PREV))/CLOCKS_PER_SEC);
	fclose(W);
	fclose(S);
	fclose(F);
}
int main(int argc, char *argv[])
{
	no_files = atoi(argv[1]);
	strcpy(loc,directory);
	strcat(loc,"file_names");
	merge_index();
	merge_title_id();
	return 0;
}
