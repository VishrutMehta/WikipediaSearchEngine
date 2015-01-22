
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
#include<utility>
#include<sstream>
#include<time.h>
#include "rapidxml-1.13/rapidxml.hpp"
#include "porterStemmer.cpp"
using namespace std;
using namespace rapidxml;

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
map<string,int> stop_words;
map<string,PI> word_count;
map<string,string> TITLE;
map<string,vector<pair<string,pair<int,int> > > > Index;
char file_name[100],path[1000],title[500],text[200000000],doc[200000000],word[100],temp[200000000];
char sorted_chunks[1000],hash[100];
string ID;
int id;
clock_t CUR,PREV;

void get_stop_words()
{
	FILE *f;
	strcpy(path,directory);
	strcat(path,"stop_words");
	f=fopen(path,"rb");
	while(fscanf(f,"%s",word)!=EOF)
		stop_words[word]=1;
	fclose(f);
}

string convert(int x)
{
	string s;
	while(x)
	{
		stringstream q;
		q << hash[x%93];
		s.insert(0,q.str());
		x/=93;
	}
	return s;
}
int check(char *w)
{
	if(stop_words[w])
		return 1;
	int l=strlen(w),c=0;
	// if ant 3 consective letters are same then also stop word 
	rep(i,l-3)
		if(w[i]==w[i+1] && w[i+1]==w[i+2] && w[i+2]==w[i+3])
			return 1;
	return 0;
}
void parse_text()
{
	// Parse data in the text field 
	int i=0,k,l=strlen(text),j,c;
	int br_count=0;
	int a[4]={0};			//0 - body,	1-category,	2-outlink,	3-infobox,	4-title
	while(i<l)
	{
		// checking <!-- --> 
		if(i+3<l && text[i]=='<' && text[i+1]=='!' && text[i+2]=='-' && text[i+3]=='-')
		{
			i+=4;
			while(i+2<l && !(text[i]=='-' && text[i+1]=='-' && text[i+2]=='>'))
				i++;
			i+=3;
			continue;
		}

		// come in the Infobox 
		if(i+8<l && text[i]=='{' && text[i+1]=='{' && text[i+2]=='I' && text[i+3]=='n' && text[i+4]=='f' && text[i+5]=='o' && text[i+6]=='b' && text[i+7]=='o' && text[i+8]=='x')
		{
			i+=9;
			br_count=2;
			a[3]=1;
			while(br_count>0)
			{
				if(text[i]=='}')
				{
					br_count--;
					i++;
					continue;
				}
				if(text[i]=='{')
				{
					i++;
					br_count++;
					continue;
				}
				// check the outlink 
				if(i+4<l && text[i]=='[' && text[i+1]=='h' && text[i+2]=='t' && text[i+3]=='t' && text[i+4]=='p')
				{
					i+=5;
					while(!(text[i]=='/' && text[i+1]=='/'))
						i++;
					i+=2;
					a[2]=1;
					while(text[i]!=']')
					{
						if(!isalpha(text[i]))
						{
							i++;
							continue;
						}
						c=0;
						while(isalpha(text[i]))
							word[c++]=tolower(text[i++]);
						word[c]='\0';
						if(check(word))
							continue;
						word[stem(word,0,strlen(word)-1)+1] = 0;
						rep(c,4)
							if(a[c])
								word_count[word].second |= 1<<c; // second shows set bits shows title or .... 
						word_count[word].first++; // total count of a word 
					}
					a[2]=0; // outlink is zero now 
					i++;
					continue;
				}

				// Check the Category 
				if(i+1<l && text[i]=='[' && text[i+1]=='[')
				{
					i+=2;
					if(text[i]=='C' && text[i+1]=='a' && text[i+2]=='t' && text[i+3]=='e' && text[i+4]=='g' && text[i+5]=='o' && text[i+6]=='r' && text[i+7]=='y')
					{
						a[1]=1; // standard category 
						i+=8;
					}
					else
						a[2]=1; // may also a http in the category 
					while(!(text[i]==']' && text[i+1]==']'))
					{
						if(!isalpha(text[i]))
						{
							i++;
							continue;
						}
						c=0;
						while(isalpha(text[i]))
							word[c++]=tolower(text[i++]);
						word[c]='\0';
						if(check(word))
							continue;
						word[stem(word,0,strlen(word)-1)+1] = 0;
						rep(c,4)
							if(a[c])
								word_count[word].second |= 1<<c;
						word_count[word].first++;
					}
					a[1]=a[2]=0;
					i+=2;
					continue;
				}
				// if not http or category in infobox , put in the infobox category 
				if(!isalpha(text[i]))
				{
					i++;
					continue;
				}
				c=0;
				while(isalpha(text[i]))
					word[c++]=tolower(text[i++]);
				word[c]='\0';
				if(check(word))
					continue;
				word[stem(word,0,strlen(word)-1)+1] = 0;
				rep(c,4)
					if(a[c])
						word_count[word].second |= 1<<c;
				word_count[word].first++;
			}
			a[3]=0;
			continue;
		}

		// only http without infobox 
		if(i+4<l && text[i]=='[' && text[i+1]=='h' && text[i+2]=='t' && text[i+3]=='t' && text[i+4]=='p')
		{
			i+=5;
			while(!(text[i]=='/' && text[i+1]=='/'))
				i++;
			i+=2;
			a[2]=1;
			while(text[i]!=']')
			{
				if(!isalpha(text[i]))
				{
					i++;
					continue;
				}
				c=0;
				while(isalpha(text[i]))
					word[c++]=tolower(text[i++]);
				word[c]='\0';
				if(check(word))
					continue;
				word[stem(word,0,strlen(word)-1)+1] = 0;
				rep(c,4)
					if(a[c])
						word_count[word].second |= 1<<c;
				word_count[word].first++;
			}
			a[2]=0;
			i++;
			continue;
		}

		// only the category without the infobox 
		if(i+1<l && text[i]=='[' && text[i+1]=='[')
		{
			i+=2;
			if(text[i]=='C' && text[i+1]=='a' && text[i+2]=='t' && text[i+3]=='e' && text[i+4]=='g' && text[i+5]=='o' && text[i+6]=='r' && text[i+7]=='y')
			{
				a[1]=1;
				i+=8;
			}
			else
				a[2]=1;
			while(!(text[i]==']' && text[i+1]==']'))
			{
				if(!isalpha(text[i]))
				{
					i++;
					continue;
				}
				c=0;
				while(isalpha(text[i]))
					word[c++]=tolower(text[i++]);
				word[c]='\0';
				if(check(word))
					continue;
				word[stem(word,0,strlen(word)-1)+1] = 0;
				rep(c,4)
					if(a[c])
						word_count[word].second |= 1<<c;
				word_count[word].first++;
			}
			a[1]=a[2]=0;
			i+=2;
			continue;
		}
		// those which are not in any of the tags comes under the body tag 
		if(!isalpha(text[i]))
		{
			i++;
			continue;
		}
		c=0;
		a[0]=1;
		while(isalpha(text[i]))
			word[c++]=tolower(text[i++]);
		word[c]='\0';
		if(check(word))
			continue;
		word[stem(word,0,strlen(word)-1)+1] = 0;
		rep(c,4)
			if(a[c])
				word_count[word].second |= 1<<c;
		word_count[word].first++;
		a[0]=0;
	}
}
void create_index()
{
	int i,j,n,file_length,doc_length,k,l;
	PREV = clock();
	xml_document<> DOC;
	printf("parsing %s\n",file_name);
	strcpy(path,directory);
	strcat(path,"Wiki_Split_Files/");
	strcat(path,file_name);
	FILE *Read = fopen(path,"rb");
	Index.clear();
	file_length=fread(doc,sizeof(char),200000000,Read); // take a chunk and put in doc 
	doc[file_length]='\0';
	DOC.parse<0>(doc);
	xml_node<>* doc_node = DOC.first_node("file");
	xml_node<> *title_node, *id_node, *text_node, *revision_node;
	for( xml_node<>* page_node = doc_node->first_node("page");page_node; page_node = page_node->next_sibling())
	{
		title_node = page_node->first_node("title");
		id_node = page_node->first_node("id");
		revision_node = page_node->first_node("revision");
		text_node = revision_node->first_node("text");
		sscanf((char *)id_node->value(),"%d",&id);
		ID = convert(id);  // int to string 
		strcpy(text,text_node->value());
		word_count.clear();
		strcpy(title,title_node->value());
		TITLE[ID] = title;
		l=strlen(title); 
		j=0;
		// for the title only 
		while(j<l)
		{
			if(!isalpha(title[j]))
			{
				j++;
				continue;
			}
			k=0;
			while(isalpha(title[j]))
				word[k++]=tolower(title[j++]);
			word[k]='\0';
			if(check(word))
				continue; // check for the stop word , if there continue 
			// Else not stop word , then stem it 
			word[stem(word,0,strlen(word)-1)+1] = 0;
			word_count[word].first++;
			word_count[word].second |= 1<<4;
		}
		parse_text();
		// Putting all the words in Index as string -> string -> (int , int) // sachin -> ID -> (count , 11010) 
		EACH(it,word_count)
			Index[it->first].pb(mp(ID,it->second));
	}
	fclose(Read);
	strcpy(path,directory);
	strcat(path,"sorted_chunks/");
	strcat(path,file_name);
	FILE *Write = fopen(path,"wb");
	EACH(it,Index)
	{
		fprintf(Write,"%s ",it->first.c_str());
		EACH(jt,it->second)
			fprintf(Write,"%s~%d~%d ",jt->first.c_str(),jt->second.first,jt->second.second);
		fprintf(Write,"\n");
	}
	fclose(Write);
	strcpy(path,directory);
	strcat(path,"Id-Title/");
	strcat(path,file_name);
	FILE *f = fopen(path,"wb");
	EACH(it,TITLE)
		fprintf(f,"%s %s\n",it->first.c_str(),it->second.c_str());
	printf("time: %f\n",(float(clock()-PREV))/CLOCKS_PER_SEC);
	PREV = CUR;
	fclose(f);
	TITLE.clear();
}
int main(int argc,char *argv[])
{
	get_stop_words();
	FOR(i,33,126)
		hash[i-33] = (char)i;	
	strcpy(file_name,argv[1]);
	create_index();
	return 0;
}
