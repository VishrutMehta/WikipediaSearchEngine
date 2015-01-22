
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
#include<time.h>
#include<utility>
#include "porterStemmer.cpp"
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
#define directory 	"/media/Entertainment/study/sem6/ir/mini_project/"
typedef pair<int,int> PI;
typedef vector<int> VI;
typedef vector<VI> VVI;
typedef long long LL;
map<string,int> stop_words;
map<int,string> TITLE;
char query[10000];
char path[10000],word[20000000],line[20000000],temp[20000000];
int no_files = 12961996,id,tag,tag_present;
vector<pair<string,int> > Q;
map<string,int> tags;
map<string,LL> index_sec_index;
map<string,pair<int,double> > docs;
vector<string> index_sec;
vector<LL> title_sec;
map<LL,LL> title_sec_index;
FILE *F,*T;
int tag_value[]={10,20,30,40,50};
void map_tags()
{
	tags["T"]=4;
	tags["I"]=3;
	tags["O"]=2;
	tags["C"]=1;
	tags["B"]=0;
}
void read_stop_words()
{
	FILE *f;
	strcpy(path,directory);
	strcat(path,"stop_words");
	f=fopen(path,"r");
	while(fscanf(f,"%s",word)!=EOF)
		stop_words[word]=1;
	fclose(f);
}
LL convert(string x)
{
	LL f=1;
	LL s=0;
	int i;
	for(i=sz(x)-1;i>=0;i--)
	{
		s+=(x[i]-'!')*f;
		f*=93;
	}	
	return s;
}
void read_title_sec_index()
{
	FILE *f;
	strcpy(path,directory);
	strcat(path,"title_id_secondary_index");
	f = fopen(path,"r");
	LL offset,id;
	while(fscanf(f,"%s %lld",word,&offset)!=EOF)
	{
		id = convert(word);
		title_sec_index[id]=offset;
		title_sec.pb(id);
	}
	fclose(f);
}
void read_index_sec_index()
{
	FILE *f;
	strcpy(path,directory);
	strcat(path,"secondary_index");
	f = fopen(path,"r");
	LL offset;
	while(fscanf(f,"%s %lld",word,&offset)!=EOF)
	{
		index_sec_index[word]=offset;
		index_sec.pb(word);
	}
	fclose(f);
}

void parse_query()
{
	char *p;
	p = strtok(query,": ");
	vector<string> words;
	while(p!=NULL)
	{
		words.pb(p);
		p=strtok(NULL,": ");
	}
	Q.clear();
	if(sz(words[0])==1 && (words[0]=="T" || words[0]=="I" || words[0]=="B" || words[0]=="C" || words[0]=="O"))
		tag_present = 1;
	if(tag_present)
	{
		rep(i,sz(words))
		{
			rep(j,sz(words[i+1]))
				words[i+1][j] = tolower(words[i+1][j]);
			if(stop_words[words[i+1]])
			{
				i++;
				continue;
			}
			strcpy(word,words[i+1].c_str());
			word[stem(word,0,strlen(word)-1)+1] = 0;
			Q.pb(mp(word,1<<(tags[words[i]])));
			i++;
		}
	}
	else
	{
		rep(i,sz(words))
		{
			rep(j,sz(words[i]))
				words[i][j] = tolower(words[i][j]);
			if(stop_words[words[i]])
				continue;
			strcpy(word,words[i].c_str());
			word[stem(word,0,strlen(word)-1)+1] = 0;
			Q.pb(mp(word,1));
		}
	}
}
void title_search(string title)
{
	vector<LL>::iterator it;
	LL id = convert(title);
	it = lower_bound(ALL(title_sec),id);
	if(it!=title_sec.begin() && (*it)!=id)
		it--;
	LL offset = title_sec_index[*it];
	fseek(T,offset,SEEK_SET);
	while(fscanf(T,"%s %[^\n]",temp,line)!=EOF)
	{
		if(strcmp(temp,title.c_str())==0)
		{
			printf("%s\n",line);
			return;
		}
		else if(strcmp(temp,title.c_str())>0)
		{
			printf("%s\n",line);
			return;
		}
	}
}
string index_search()
{
	vector<string>::iterator it;
	it = lower_bound(ALL(index_sec),word);
	if(it!=index_sec.begin() && strcmp((*it).c_str(),word)!=0)
		it--;
	LL offset = index_sec_index[*it];
	fseek(F,offset,SEEK_SET);
	while(fscanf(F,"%s %[^\n]",temp,line)!=EOF)
	{
		if(strcmp(temp,word)==0)
			return line;
		else if(strcmp(temp,word)>0)
			break;
	}
	return "-1";
}
pair<string,pair<int,int> > parse_element(char *x)
{
	int l = strlen(x);
	int fr,c,k=0,j=0,z=0;
	char tr[3][10000];
	char *p;
	while(j<l)
	{
		while(j<l && x[j]!='~')
			tr[k][z++]=x[j++];
		tr[k][z]='\0';
		k++;
		j++;
		z=0;
	}
	fr = atoi(tr[1]);
	c = atoi(tr[2]);
	return mp(tr[0],mp(fr,c));
}
vector<pair<string,double> > parse_docs(string s)
{
	strcpy(line,s.c_str());
	vector<pair<string,double> > q;
	char *p = strtok(line," ");
	int fr,c;
	double no_doc=0.0;
	while(p!=NULL)
	{
		strcpy(word,p);
		no_doc++;
		pair<string,pair<int,int> > P = parse_element(word);
		fr = P.second.first;
		c = P.second.second;
		if(tag_present)
		{
			if(c&tag)
				q.pb(mp(P.first,fr*1.0/*tag_value[int(log2(tag))]*/));
		}
		else
			q.pb(mp(P.first,fr*1.0));
		p=strtok(NULL," ");
	}
	no_doc*=1.0;
	rep(i,sz(q))
		q[i].second*=log(no_files/no_doc);
	return q;
}
void result()
{
	docs.clear();
	vector<pair<string,double> > q;
	rep(i,sz(Q))
	{
		strcpy(word,Q[i].first.c_str());
		tag = Q[i].second;
		string s = index_search();
		if(s=="-1")
			break;//continue;
		q = parse_docs(s);
		rep(j,sz(q))
		{
			docs[q[j].first].first++;
			docs[q[j].first].second+= q[j].second;
		}
	}
	priority_queue<pair<double,string> > P;
	int no_queries = sz(Q);
	EACH(it,docs)
		if(it->second.first==no_queries)
			P.push(mp(it->second.second,it->first));
	if(sz(P)==0)
	{
		printf("No Result\n");
		return ;
	}
	int c=0;
	while(!P.empty() && c<15)
	{
		pi(c+1);
		title_search(P.top().second);
		c++;
		P.pop();
	}
}
int main()
{
	map_tags();
	read_stop_words();
	read_title_sec_index();
	read_index_sec_index();
	strcpy(path,directory);
	strcat(path,"INDEX");
	F = fopen(path,"r");
	strcpy(path,directory);
	strcat(path,"TITLE-ID");
	T = fopen(path,"r");
	char ch[] = "Y";
	while(strcmp(ch,"Y")==0)
	{
		printf("Query: ");
		scanf(" %[^\n]",query);
		tag_present = 0;
		parse_query();
		result();
		printf("Enter more(Y or N): ");
		scanf("%s",ch);
		printf("\n");
	}
	fclose(F);
	fclose(T);
	return 0;
}
