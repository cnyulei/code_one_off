#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>	
#include <string.h>		// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#include <fstream>
#include <conio.h>
#include <cmath>
//#include <vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// The one and only application object

#include <string.h>
#define NOLENCON true
using namespace std;

ofstream outFile;//创建了一个ofstream 对象


#define N 200000
#define M 1000
int maxp_c=-1;
char p[M];
/*********定义模式串的相关数据***********/
struct sub_ptn_struct //模式串
{
	char start,end;
	int min,max;
};
sub_ptn_struct sub_ptn[M];//定义模式串 
int ptn_len=0;  //the length of pattern
/*******************定义网树结点的相关信息*********************/
struct node;
struct point
{
	node *p;
};
struct node
{
	char data;
	int position;
	int *downpath;//树叶路径数
	int *uppath;//树根路径数
	int *up_downpath;
	node *next;
	node *pre;
	point *child,*parent;
	int ch_count;
	int p_count;
	int *localpath;
	bool dis;//当dis=ture时精确匹配，否则近似匹配
	
	int deltadistance; //记录每个结点的delta距离
};
/****************定义网树层的相关信息******************/
struct level_node
{
	int count;
	char ch;
	node *head;
	node *start;
	node *tail;
	bool needrep;
};
level_node *head_level;

/***************定义其它变量********************/
int d;  //伽马
int delta; //delta距离
int sumocc=0;
int seq_sumlen=0;
int seq_len;
char t[N];

void dearlrange(char *p,int len)
{
	sub_ptn[ptn_len].start =p[0];
	sub_ptn[ptn_len].end =p[len];
	if (len==1)
	{
		sub_ptn[ptn_len].max =sub_ptn[ptn_len].min=0;
	}
	else
	{
		int value=0;
		int i;
		int flag=0;
		for ( i=2;p[i]!=',';i++)
			if(p[i]=='-')
				flag=1;
			else
				value = value*10+p[i]-48;
			if(flag==1)
				sub_ptn[ptn_len].min = -value;
			else
				sub_ptn[ptn_len].min = value;		
			value=0;
			flag=0;
			for (int j=i+1;p[j]!=']';j++)
				if(p[j]=='-')
					flag = 1;
				else
					value = value*10+p[j]-48;
				if(flag==1)
					sub_ptn[ptn_len].max = -value;
				else
					sub_ptn[ptn_len].max = value;
	}
	if(sub_ptn[ptn_len].max-sub_ptn[ptn_len].min+1>maxp_c)
		maxp_c=sub_ptn[ptn_len].max-sub_ptn[ptn_len].min+1;
	ptn_len++;
	cout<<ptn_len<<endl;
}
void convert_p_to_ruler(char *p)
{
	char st,en;
	int l,r;
	int len=strlen(p);
	st=p[0];
	en=p[len-1];
	if(isalpha(st)&&isalpha(en))
	{
		l = 0;
		for(int i=1;i<len;i++)
		{
			if(isalpha(p[i]))
			{
				r=i;
				dearlrange(p+l,r-l);
				l=r;
			}
		}	
	}
	else
	{
		cout<<"irrengular pattern.\n";
		exit(-1);
	}
}
void disp_pattern()
{
	for(int i=0;i<ptn_len;i++)
	{
        cout<<sub_ptn[i].start<<"\t"<<sub_ptn[i].min<<"\t"<<sub_ptn[i].max<<"\t"<<sub_ptn[i].end<<endl;
	}
}
/**********************建立深度数组*********************/
void createlevel()
{
	head_level=new level_node[ptn_len+1];
	int i;
	for(i=0;i<ptn_len+1;i++)
	{
		head_level[i].count =-1;
		head_level[i].head  =NULL;
		head_level[i].start =NULL;
		head_level[i].tail  =NULL;
		head_level[i].needrep=false;
	}
	for(i=0;i<ptn_len;i++)
		head_level[i].ch=sub_ptn[i].start;
	head_level[ptn_len].ch=sub_ptn[ptn_len-1].end;
}
/********************检查内部是否产生重复*********************/
void cal_needrep()
{
	int i,j;
	int flag=0;
	char h;
	for(i=0;i<ptn_len;i++)
	{
		if(sub_ptn[i].min<0)
		{
			flag=1;
			break;
		}
	}
	if(flag==0)
	{
		return;
	}
	else
	{
		if(d==0)
		{
			for(i=ptn_len;i>=0;i--)
			{
				if(head_level[i].needrep==true)
					continue;
				else
				{
					h=head_level[i].ch;
					for(j=i-1;j>=0;j--)
					{
						if(head_level[j].ch==h)
						{
							int mini=0;
							for(int l=j;l<i;l++)
							{
								if(sub_ptn[l].min<0)
									mini+=sub_ptn[l].min;
								else
									mini+=sub_ptn[l].min+1;
							}
							if(mini<=0)
							{
								head_level[j].needrep=true;
							}
						}
					}
				}
			}
		}
		else
		{
			for(i=ptn_len;i>=0;i--)
			{
				if(head_level[i].needrep==true)
					continue;
				else
				{
					for(j=i-1;j>=0;j--)
					{
						int mini1=0;
						for(int l=j;l<i;l++)
						{
							if(sub_ptn[l].min<0)
								mini1+=sub_ptn[l].min;
							else
								mini1+=sub_ptn[l].min+1;
						}
						if(mini1<=0)
							head_level[j].needrep=true;
					}
				}
			}
		}
	}
	/***************验证输出*****************/
	cout<<"内部重复检查机制结果："<<endl;
	for(i=0;i<ptn_len+1;i++)
	{
		cout<<head_level[i].needrep<<endl;
	}
}

void display_nettree()
{
	for(int i=0;i<ptn_len+1;i++)
	{
		node *tmp=head_level[i].head;
		cout<<"Levle["<<i<<"]:\t"<<endl;
		while(tmp!=NULL)
		{
			cout<<tmp->position+1<<":";
			for(int m=0;m<d+1;m++)
				cout<<(tmp->uppath)[m]<<" ";
			    //cout<<(tmp->up_downpath)[m]<<" ";
			//cout<<"delta="<<tmp->dis;
			
			/*
			cout<<" child is "<<tmp->ch_count;
			cout<<"->";
			for(int n=0;n<=tmp->ch_count;n++)
				cout<<(tmp->child)[n].p->position;
			cout<<" ";
			*/
			tmp=tmp->next;
		}
		cout<<endl;
	}	
}
/************************能否插入精确匹配结点**************/
bool can_insert_exactnode(int position,int depth,node *start,node *ptail)
{
	node *k=start;
	while(k!=ptail)
	{
		if(sub_ptn[depth-1].min>=0)//间隙为正正的情况
		{
			if((position-k->position-1)<=sub_ptn[depth-1].max&&(position-k->position-1)>=sub_ptn[depth-1].min)
				return true;
			else
				k=k->next;
		}
		else 
		{
			if(sub_ptn[depth-1].max<0)//都为负间隙
			{
				if((position-k->position)<=sub_ptn[depth-1].max&&(position-k->position)>=sub_ptn[depth-1].min)
					return true;
				else
					k=k->next;
			}
			else//一正一负
			{
				if((position-k->position)<=sub_ptn[depth-1].max+1&&(position-k->position)>=sub_ptn[depth-1].min&&position!=k->position)
					return true;
				else
					k=k->next;
			}
		}
	}
	if(sub_ptn[depth-1].min>=0)
	{
		if((position-k->position-1)<=sub_ptn[depth-1].max&&(position-k->position-1)>=sub_ptn[depth-1].min)
			return true;
		return false;
	}
	else if(sub_ptn[depth-1].max<0)
	{
		if((position-k->position)<=sub_ptn[depth-1].max&&(position-k->position)>=sub_ptn[depth-1].min)
			return true;
		return false;
	}
	else if(sub_ptn[depth-1].min<0&&sub_ptn[depth-1].max>=0)
	{
		if((position-k->position)<=sub_ptn[depth-1].max+1&&(position-k->position)>=sub_ptn[depth-1].min&&position!=k->position)
			return true;
		return false;
	}
	return false;
}

/************************建立网树中的一个结点信息***************/
void create_node_table(char t,int position,int depth,int &nodecount)
{
	node *tmp;
	if(depth==0)//建立0层结点
	{
		tmp=new node();
		tmp->data=t;
		tmp->position=position;
		tmp->uppath=new int[1];
		(tmp->uppath)[0]=0;
		
		tmp->deltadistance=0; //精确匹配
		
		tmp->downpath=new int[1];
		tmp->up_downpath=new int[1];
		tmp->localpath=new int[1];
		tmp->next=NULL;
		tmp->child=new point[maxp_c];
		for(int i=0;i<maxp_c;i++)
			(tmp->child)[i].p=NULL;
		tmp->ch_count=-1;
		tmp->p_count=-1;
		tmp->dis=true;
		(head_level[0].count)++;
		if(head_level[0].count==0)//第一层的第一个结点
		{
			head_level[0].head=tmp;
			head_level[0].tail=tmp;
			head_level[0].start=tmp;
			tmp->pre=NULL;
		}
		else//不是第一个结点
		{
			tmp->pre=head_level[0].tail;
			(head_level[0].tail)->next=tmp;
			head_level[0].tail=tmp;
		}
	}
	else
	{
		bool flag=false;
		node *k=head_level[depth-1].start;//判断是否能够进入表；
		node *ptail=head_level[depth-1].tail;
		bool c_i_n;
		if(k!=NULL)
		{
			c_i_n= can_insert_exactnode(position,depth,k,ptail);
		}
		if(k!=NULL&&c_i_n)
		{
			int i;
			tmp=new node();
			tmp->data=t;
			tmp->position=position;
			tmp->uppath=new int[1];
			(tmp->uppath)[0]=0;
			
			tmp->deltadistance=0; //精确匹配
			
			tmp->downpath=new int[1];
			tmp->up_downpath=new int[1];
			tmp->localpath=new int[1];
			tmp->localpath[0]=0;
			tmp->next=NULL;
			tmp->child=new point[maxp_c];//对孩子结点进行处理
			for(i=0;i<maxp_c;i++)
				(tmp->child)[i].p=NULL;
			tmp->parent=new point[maxp_c];//对父亲结点进行处理
			for(i=0;i<maxp_c;i++)
				(tmp->parent)[i].p=NULL;
			tmp->ch_count=-1;
			tmp->p_count=-1;
			tmp->dis=true;
			(head_level[depth].count)++;
			if(head_level[depth].count==0)//本层第一个结点
			{
				head_level[depth].head=tmp;
				head_level[depth].tail=tmp;
				head_level[depth].start=tmp;
				tmp->pre=NULL;
			}
			else
			{
				tmp->pre=head_level[depth].tail;
				(head_level[depth].tail)->next=tmp;
				head_level[depth].tail=tmp;
			}
			while(k!=NULL)
			{
				int l;
				if(sub_ptn[depth-1].max<0)//最大间隙和最小间隙都为负值时
				{
					l=position-k->position;
					if(l<sub_ptn[depth-1].min)
						break;
					if(l>sub_ptn[depth-1].max)
					{
						if(k->next!=NULL)
							head_level[depth-1].start=k->next;//该结点与上层结点距离太近，起点后移
					}
					else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max)//该结点与k结点满足间隙约束，连接孩子双亲结点
					{
						(k->ch_count)++;
						((k->child)[k->ch_count]).p=tmp;
						(tmp->p_count)++;
						((tmp->parent)[tmp->p_count]).p=k;
					}
				}
				else if(sub_ptn[depth-1].min>=0) //都为正值
				{
					l=position-k->position-1;
					if(l<sub_ptn[depth-1].min)
						break;
					else if(l>sub_ptn[depth-1].max)
					{
						if(k->next!=NULL)  //确定好上一层满足间隙的开始位置
							head_level[depth-1].start=k->next;
					}
					else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max)
					{
						(k->ch_count)++;
						((k->child)[k->ch_count]).p=tmp;
						(tmp->p_count)++;
						((tmp->parent)[tmp->p_count]).p=k;
					}
				}
				else if(sub_ptn[depth-1].min<0&&sub_ptn[depth-1].max>=0)//一正一负
				{
					l=position-k->position;
					if(l<sub_ptn[depth-1].min)
						break;
					else if(l>sub_ptn[depth-1].max+1)
					{
						if(k->next!=NULL)
							head_level[depth-1].start=k->next;
					}
					else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max+1&&position!=k->position)
					{
						(k->ch_count)++;
						((k->child)[k->ch_count]).p=tmp;
						(tmp->p_count)++;
						((tmp->parent)[tmp->p_count]).p=k;
					}
				}
				k=k->next;//k继续往后移动
			}
		}
	}
}

/**********************判断能否插入近似结点*******************/
bool can_insert_appnode(int position,int depth,node *start,node *ptail)
{
	node *k=start;
	if(sub_ptn[depth-1].min>=0)//间隙为正正
	{
		while(k!=ptail)
		{
			if((position-k->position-1)<sub_ptn[depth-1].min)
				return false;
			else 
			{
				if((position-k->position-1)<=sub_ptn[depth-1].max)
				{
					for(int m=0;m<d;m++)
					{
						if((k->uppath)[m]!=0)
							return true;
					}
					k=k->next;
				}
				else
					k=k->next;
			}
		}
		if((position-k->position-1)<=sub_ptn[depth-1].max&&(position-k->position-1)>=sub_ptn[depth-1].min)
		{
			for(int m=0;m<d;m++)
			{
				if((k->uppath)[m]!=0)
					return true;
			}
			return false;
		}
		return false;
	}
	else
	{
		if(sub_ptn[depth-1].max<0)//间隙为负负情况
		{
			while(k!=ptail)
			{
				if((position-k->position)<sub_ptn[depth-1].min)
					return false;
				else 
				{
					if((position-k->position)<=sub_ptn[depth-1].max)
					{
						for(int m=0;m<d;m++)
						{
							if((k->uppath)[m]!=0)
								return true;
						}
						k=k->next;
					}
					else
						k=k->next;
				}
			}
			if((position-k->position)>=sub_ptn[depth-1].min&&(position-k->position)<=sub_ptn[depth-1].max)
			{
				for(int m=0;m<d;m++)
				{
					if((k->uppath)[m]!=0)
						return true;
				}
				return false;
			}
			return false;
		}
		else //间隙为正负
		{
			while(k!=ptail)
			{
				if((position-k->position)<sub_ptn[depth-1].min)
					return false;
				else
				{
					if((position-k->position-1)<=sub_ptn[depth-1].max&&position!=k->position)
					{
						for(int m=0;m<d;m++)
						{
							if((k->uppath)[m]!=0)
								return true;
						}
						k=k->next;
					}
					else
						k=k->next;
				}
			}
			if((position-k->position)>=sub_ptn[depth-1].min&&(position-k->position-1)<=sub_ptn[depth-1].max&&position!=k->position)
			{
				for(int m=0;m<d;m++)
				{
					if((k->uppath)[m]!=0)
						return true;
				}
				return false;
			}
			return false;
		}
	}
	return false;
}

/*************************建立d>0时的网树****************/
void create_appnode_table(int position,int depth)
{
	node *tmp;
	int deltadis=abs(t[position] - head_level[depth].ch); //delta距离
	
	if(depth==0)//网树第一层直接建立结点
	{
		tmp=new node();
		tmp->data=t[position];
		tmp->position=position;
		
		tmp->deltadistance=deltadis; //记录结点的delta距离
		
		tmp->uppath=new int[d+1];
		tmp->downpath=new int[d+1];
		tmp->up_downpath=new int[d+1];
		tmp->localpath=new int[d+1];
		for(int i=0;i<d+1;i++)
			(tmp->localpath)[i]=0;
		tmp->next=NULL;
		tmp->child=new point[maxp_c];
		for(i=0;i<maxp_c;i++)
			(tmp->child)[i].p=NULL;
		tmp->ch_count=-1;
		tmp->p_count=-1;
		//if(tmp->data==head_level[depth].ch)//精确匹配
		
		if(tmp->deltadistance==0)
		{
			tmp->dis=true;
			(tmp->uppath)[0]=1;
			for(int k=1;k<d+1;k++)
				(tmp->uppath)[k]=0;
		}
		else//近似匹配
		{
			
			tmp->dis=false;
			
			for(int k=0;k<d+1;k++)
				(tmp->uppath)[k]=0;
			
			(tmp->uppath)[tmp->deltadistance]=1;  //第delta距离处的树根路径数为1
		}
		(head_level[0].count)++;
		if(head_level[0].count==0)//第一层第一个结点
		{
			head_level[0].head=tmp;
			head_level[0].tail=tmp;
			head_level[0].start=tmp;
			tmp->pre=NULL;
		}
		else
		{
			tmp->pre=head_level[0].tail;
			(head_level[0].tail)->next=tmp;
			head_level[0].tail=tmp;
		}
	}
	else//不是网树的第一层
	{
		node *k=head_level[depth-1].start;
		node *ptail=head_level[depth-1].tail;
		bool c_i_n;
		//if(t[position]==head_level[depth].ch)//精确匹配
		if(deltadis==0) //精确匹配
		{
			if(k!=NULL)
			{
				c_i_n=can_insert_exactnode(position,depth,k,ptail);
			}
			if(k!=NULL&&c_i_n)//建立结点
			{
				int i;
				tmp=new node();
				tmp->data=t[position];
				tmp->position=position;
				
				tmp->deltadistance=deltadis;  //赋值delta距离
				tmp->uppath=new int[d+1];
				for(int m=0;m<d+1;m++)
					(tmp->uppath)[m]=0;
				tmp->downpath=new int[d+1];
				tmp->up_downpath=new int[d+1];
				tmp->localpath=new int[d+1];
				for(i=0;i<d+1;i++)
					(tmp->localpath)[i]=0;
				tmp->next=NULL;
				tmp->child=new point[maxp_c];
				for(i=0;i<maxp_c;i++)
					(tmp->child)[i].p=NULL;
				tmp->parent=new point[maxp_c];
				for(i=0;i<maxp_c;i++)
					(tmp->parent )[i].p=NULL;
				tmp->ch_count=-1;
				tmp->p_count=-1;
				tmp->dis=true;
				(head_level[depth].count)++;
				
				//本层建立关系
				if(head_level[depth].count==0)
				{
					head_level[depth].head=tmp;
					head_level[depth].tail=tmp;
					head_level[depth].start=tmp;
					tmp->pre=NULL;
				}
				else
				{
					tmp->pre=head_level[depth].tail;
					(head_level[depth].tail)->next=tmp;
					head_level[depth].tail=tmp;
				}
				while(k!=NULL)  //建立层间关系
				{
					int l;
					if(sub_ptn[depth-1].max<0)//间隙为负负
					{
						l=position-k->position;
						if(l<sub_ptn[depth-1].min)
							break;
						if(l>sub_ptn[depth-1].max)
						{
							if(k->next!=NULL)
								head_level[depth-1].start=k->next;//该结点与上层结点距离太近，起点后移
						}
						else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max)//该结点与k结点满足间隙约束，连接孩子双亲结点
						{
							(k->ch_count)++;
							((k->child)[k->ch_count]).p=tmp;
							(tmp->p_count)++;
							((tmp->parent)[tmp->p_count]).p=k;
							for(int m=0;m<d+1;m++)
							{
								(tmp->uppath)[m]+=(((tmp->parent)[tmp->p_count]).p->uppath)[m];
							}
						}
					}
					else if(sub_ptn[depth-1].min>=0) //都为正值
					{
						l=position-k->position-1;
						if(l<sub_ptn[depth-1].min)
							break;
						else if(l>sub_ptn[depth-1].max)
						{
							if(k->next!=NULL)
								head_level[depth-1].start=k->next;
						}
						else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max)
						{
							(k->ch_count)++;
							((k->child)[k->ch_count]).p=tmp;
							(tmp->p_count)++;
							((tmp->parent)[tmp->p_count]).p=k;
							for(int m=0;m<d+1;m++)         //赋值树根路径数数组
							{
								(tmp->uppath)[m]+=(((tmp->parent)[tmp->p_count]).p->uppath)[m];
							}
						}
					}
					else if(sub_ptn[depth-1].min<0&&sub_ptn[depth-1].max>=0)//一正一负
					{
						l=position-k->position;
						if(l<sub_ptn[depth-1].min)
							break;
						else if(l>sub_ptn[depth-1].max+1)
						{
							if(k->next!=NULL)
								head_level[depth-1].start=k->next;
						}
						else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max+1&&position!=k->position)
						{
							(k->ch_count)++;
							((k->child)[k->ch_count]).p=tmp;
							(tmp->p_count)++;
							((tmp->parent)[tmp->p_count]).p=k;
							for(int m=0;m<d+1;m++)
							{
								(tmp->uppath)[m]+=(((tmp->parent)[tmp->p_count]).p->uppath)[m];
							}
						}
					}
					k=k->next;
				}
			}
		}
		else //近似匹配时
		{
			if(k!=NULL)
			{
				c_i_n=can_insert_appnode(position,depth,k,ptail);
			}
			if(k!=NULL&&c_i_n)
			{
				int i;
				tmp=new node();
				tmp->data=t[position];
				tmp->position=position;
				
				tmp->deltadistance=deltadis;  //赋值delta距离
				
				tmp->uppath=new int[d+1];
				for(int m=0;m<d+1;m++)
					(tmp->uppath)[m]=0;
				tmp->downpath=new int[d+1];
				tmp->up_downpath=new int[d+1];
				tmp->localpath=new int[d+1];
				for(i=0;i<d+1;i++)
					(tmp->localpath)[i]=0;
				tmp->next=NULL;
				tmp->child=new point[maxp_c];
				for(i=0;i<maxp_c;i++)
					(tmp->child)[i].p=NULL;
				tmp->parent=new point[maxp_c];
				for(i=0;i<maxp_c;i++)
					(tmp->parent )[i].p=NULL;
				tmp->ch_count=-1;
				tmp->p_count=-1;
				tmp->dis=false;
				(head_level[depth].count)++;
				if(head_level[depth].count==0)//本层第一个结点
				{
					head_level[depth].head=tmp;
					head_level[depth].tail=tmp;
					head_level[depth].start=tmp;
					tmp->pre=NULL;
				}
				else
				{
					tmp->pre=head_level[depth].tail;
					(head_level[depth].tail)->next=tmp;
					head_level[depth].tail=tmp;
				}
				while(k!=NULL)
				{
					int l,m;
					if(sub_ptn[depth-1].max<0)//间隙为负负
					{
						l=position-k->position;
						if(l<sub_ptn[depth-1].min)
							break;
						if(l>sub_ptn[depth-1].max)
						{
							if(k->next!=NULL)
								head_level[depth-1].start=k->next;
						}
						else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max)
						{
							for(m=0;m<d;m++)
							{
								if((k->uppath)[m]!=0)
									break;
							}
							if(m<d)//建立关系
							{
								(k->ch_count)++;
								((k->child)[k->ch_count]).p=tmp;
								(tmp->p_count)++;
								((tmp->parent)[tmp->p_count]).p=k;
								(tmp->uppath)[0]=0;
								for(int m=1;m<d+1;m++)
								{
									(tmp->uppath)[m]+=(((tmp->parent)[tmp->p_count]).p->uppath)[m-1];
								}
							}
						}
					}
					else if(sub_ptn[depth-1].min>=0)//间隙为正正
					{
						l=position-k->position-1;
						if(l<sub_ptn[depth-1].min)
							break;
						else if(l>sub_ptn[depth-1].max)
						{
							if(k->next!=NULL)
								head_level[depth-1].start=k->next;
						}
						else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max)
						{
							for(m=0;m<d;m++)
							{
								if((k->uppath)[m]!=0)
									break;
							}
							if(m<d)  //说明存在小于伽马的地方可以插进去结点
							{
								(k->ch_count)++;
								((k->child)[k->ch_count]).p=tmp;
								(tmp->p_count)++;
								((tmp->parent)[tmp->p_count]).p=k;
								for(m=0;m<tmp->deltadistance;m++)
								{
									(tmp->uppath)[m]+=0;
								}
								
								for(int m=tmp->deltadistance;m<d+1;m++)
								{
									(tmp->uppath)[m]+=(((tmp->parent)[tmp->p_count]).p->uppath)[m-tmp->deltadistance];
								}
							}
						}
					}
					else if(sub_ptn[depth-1].min<0&&sub_ptn[depth-1].max>=0)
					{
						l=position-k->position;
						if(l<sub_ptn[depth-1].min)
							break;
						else if(l>sub_ptn[depth-1].max+1)
						{
							if(k->next!=NULL)
								head_level[depth-1].start=k->next;
						}
						else if(l>=sub_ptn[depth-1].min&&l<=sub_ptn[depth-1].max+1&&position!=k->position)
						{
							for(m=0;m<d;m++)
							{
								if((k->uppath)[m]!=0)
									break;
							}
							if(m<d)
							{
								(k->ch_count)++;
								((k->child)[k->ch_count]).p=tmp;
								(tmp->p_count)++;
								((tmp->parent)[tmp->p_count]).p=k;
								(tmp->uppath)[0]=0;
								for(int m=1;m<d+1;m++)
								{
									(tmp->uppath)[m]+=(((tmp->parent)[tmp->p_count]).p->uppath)[m-1];
								}
							}
						}
					}
					k=k->next;
				}
			}
		}
	}
}

/***************************建立网树******************/
void create_store(char *c)
{
	if(d==0)//当d=0时建立网树
	{
		int count=0;
		for(int i=0;i<ptn_len+1;i++)
		{
			for(int j=0;j<seq_len;j++)
			{
				if(head_level[i].ch==t[j])
				{
					create_node_table(c[i],j,i,count);
				}
			}
		}
	}
	else//当d>0时
	{
		for(int i=0;i<ptn_len+1;i++)
		{
			for(int j=0;j<seq_len;j++)
			{
				if(abs(head_level[i].ch-t[j])<=delta)
				{
					create_appnode_table(j,i);
				}
			}
		}
	}
}



/**********************计算结点的树叶路径数********************/
void update_node_downpath(int **nodeinpath,bool *used)
{
	int i;
	for(i=ptn_len;i>=0;i--)
	{
		node *tmp=head_level[i].head;  //每次执行，先将网树的链表头结点，弄过来
		int j;
		if(i==ptn_len)//最后一层结点的树叶路径数都置为1
		{
			for(j=0;j<=head_level[i].count;j++)
			{
				for(int m=0;m<d+1;m++)
					(tmp->downpath)[m]=0;
				if(used[tmp->position]==false)
				{
					for(m=0;m<d+1;m++)
					{
						if(m==tmp->deltadistance)
							(tmp->downpath)[m]=1;
						else
							(tmp->downpath)[m]=0;
					}
				}
				tmp=tmp->next;
			}
		}
		else
		{
			if(d==0)
			{
				for(j=0;j<=head_level[i].count;j++)
				{
					int downpath=0;
					(tmp->downpath)[0]=0;
					if(used[tmp->position]==false)
					{
						for(int k=0;k<=tmp->ch_count;k++)  //遍历孩子结点，每个值相加
						{
							node *ppp=(tmp->child[k]).p;
							downpath+=ppp->downpath[0];
						}
						(tmp->downpath)[0]=downpath;  //赋值
					}
					tmp=tmp->next;
				}
			}
			else
			{
				for(j=0;j<=head_level[i].count;j++)
				{
					for(int m=0;m<d+1;m++)
						(tmp->downpath)[m]=0;
					if(used[tmp->position]==false)
					{
						if(tmp->dis==true)//精确匹配，孩子直接相加
						{
							for(m=0;m<d+1;m++)
							{
								for(int k=0;k<=tmp->ch_count;k++)
								{
									node *ppp=(tmp->child[k]).p;
									(tmp->downpath)[m]+=(ppp->downpath)[m]; //直接相加
								}
							}
						}
						else
						{
							//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
							for(int m=0;m<tmp->deltadistance;m++)
							{
								(tmp->downpath)[m]=0;
							}
							for(m=tmp->deltadistance;m<d+1;m++)
							{
								for(int k=0;k<=tmp->ch_count;k++)
								{
									node *ppp=(tmp->child[k]).p;
									(tmp->downpath)[m]+=(ppp->downpath)[m-tmp->deltadistance];  //右移delta距离位
								}
							}
						}
					}
					tmp=tmp->next;
				}
			}
		}
	}
}


/****************计算结点的树根路径数*****************/
void update_node_uppath(int **nodeinpath,bool *used)
{
	int i;
	if(d==0)  
	{
		for(i=0;i<=ptn_len;i++)
		{
			node *tmp=head_level[i].head;
			int j;
			if(i==0)  //0层
			{
				for(j=0;j<=head_level[i].count;j++)
				{
					(tmp->uppath)[0]=0; 
					if(used[tmp->position]==false)
						(tmp->uppath)[0]=1;
					tmp=tmp->next;
				}
			}
			else
			{
				for(j=0;j<=head_level[i].count;j++)
				{
					(tmp->uppath)[0]=0;
					if(used[tmp->position]==false)
					{
						for(int k=0;k<=tmp->p_count;k++)
						{
							node *ppp=(tmp->parent[k]).p;
							(tmp->uppath)[0]+=(ppp->uppath)[0];
						}
					}
					tmp=tmp->next;
				}
			}
		}
	}
	else   //d>0
	{
		for(i=0;i<ptn_len+1;i++)
		{
			node *tmp=head_level[i].head;
			int j;
			if(i==0)  //第一层
			{
				for(j=0;j<=head_level[i].count;j++)
				{
					
					for(int m=0;m<d+1;m++)
					{
						(tmp->uppath)[m]=0;
					}
					
					if(used[tmp->position]==false)
					{
						if(tmp->dis==true)  //精确匹配
						{
							(tmp->uppath)[0]=1;
							for(int m=1;m<d+1;m++)
								(tmp->uppath)[m]=0;
						}
						else
						{							
							for(int m=0;m<d+1;m++)
								(tmp->uppath)[m]=0;
							(tmp->uppath)[tmp->deltadistance]=1;
						}
					}
					tmp=tmp->next;
				}
			}
			else
			{
				for(j=0;j<=head_level[i].count;j++)
				{
					for(int m=0;m<d+1;m++)
						(tmp->uppath)[m]=0;
					if(used[tmp->position]==false)
					{
						if(tmp->dis==true)//精确匹配，双亲直接相加
						{
							for(m=0;m<d+1;m++)
							{
								for(int k=0;k<=tmp->p_count;k++)
								{
									node *ppp=(tmp->parent[k]).p;
									(tmp->uppath)[m]+=(ppp->uppath)[m]; //直接相加
								}
							}
						}
						else
						{
							//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
							for(int m=0;m<tmp->deltadistance;m++)
							{
								(tmp->uppath)[m]=0;
							}
							for(m=tmp->deltadistance;m<d+1;m++)
							{
								for(int k=0;k<=tmp->p_count;k++)
								{
									node *ppp=(tmp->parent[k]).p;
									(tmp->uppath)[m]+=(ppp->uppath)[m-tmp->deltadistance];  //右移delta距离位
								}
							}
						}
					}
					tmp=tmp->next;
				}
			}
		}
	}
}


/*******************************计算结点的树根树叶路径数********************/
void update_node_updownpath(int **nodeinpath,bool *used)
{
	for(int i=0;i<ptn_len+1;i++)
	{
		node *tmp=head_level[i].head;
		int j;
		for(j=0;j<head_level[i].count+1;j++)
		{
			for(int k=0;k<d+1;k++)
			{
				//赋初值
				if(k<tmp->deltadistance)
				{
					(tmp->up_downpath)[k]=0;
					continue;
				}
				(tmp->up_downpath)[k]=0;

				//做乘法
				for(int m=tmp->deltadistance;m<d+1+tmp->deltadistance-k;m++)
					(tmp->up_downpath)[k]+=((tmp->uppath)[k])*((tmp->downpath)[m]);
			}
			tmp=tmp->next;
		}
	}
}


/********************计算序列串结点的树根树叶路径数******************/
void cal_nodeinpath(int **nodeinpath,bool *used)
{
	update_node_downpath(nodeinpath,used);//计算结点的树叶路径数
	update_node_uppath(nodeinpath,used);//计算结点的树根路径数
	
	if(d>0)
	{
		update_node_updownpath(nodeinpath,used);//计算结点的树根-树叶路径数
	}
	int i,j;
	for(i=0;i<seq_len;i++)
	{
		for(j=0;j<d+1;j++)
			nodeinpath[i][j]=0;
	}
	if(d==0)
	{
		//遍历一遍
		for(i=0;i<ptn_len;i++)
		{
			node *tmp=head_level[i].head;
			for(j=0;j<=head_level[i].count;j++)
			{
				int nodename=tmp->position;
				nodeinpath[nodename][0]+=tmp->uppath[0]*tmp->downpath[0];
				tmp=tmp->next;
			}
		}
	}
	else
	{
		for(i=0;i<ptn_len+1;i++)
		{
			node *tmp=head_level[i].head;
			for(j=0;j<head_level[i].count+1;j++)
			{
				for(int m=0;m<d+1;m++)
				{
					nodeinpath[tmp->position][m]+=(tmp->up_downpath)[m];  //每个结点的树根树叶路径数之和，此处应该为满足伽马距离的树根树叶路径数
				}
				tmp=tmp->next;
			}
		}
	}
	
	//输出位置相关数
	/*
	int *aa=new int [seq_len];
	for(i=0;i<seq_len;i++)
	{
		aa[i]=0;
		for(int m=0;m<d+1;m++)
		{
			aa[i]+=nodeinpath[i][m];  //每个结点的树根树叶路径数之和，此处应该为满足伽马距离的树根树叶路径数
		}
		cout<<aa[i]<<" - ";
	}
	cout<<endl;
	*/
	
}
/*************************找出现***********************/

/************************贪婪双亲策略*****************/
bool Gready_Search_Occ(node *start_off,int *apath,bool *used,int **nodeinpath)
{
	int start=start_off->position;
	apath[ptn_len]=start;
	node *parentnode;
	node *childnode=start_off;
	int *pos,i;
	pos=new int[ptn_len+1];
	point *array;
	array=new point[ptn_len+1];
	array[ptn_len].p=start_off;
	int d1=d;
	for(i=0;i<ptn_len+1;i++)
	{
		pos[i]=-1;
	}
	pos[ptn_len]=0;
	if(childnode->dis==true)
		d1=d1;
	else
		//d1=d1-1;
		d1=d1-childnode->deltadistance;  //减去delta距离
	for(i=ptn_len-1;i>=0;)
	{
		int nodename,j;
		if(pos[i]==-1)
			j=childnode->p_count;
		else
			j=pos[i]-1;
		int mincost=0;
		int pos_j=-1;
		bool firstfind=false;
		for(;j>=0;j--)
		{
			bool flag=false;
			bool flag1=false;
			parentnode=childnode->parent[j].p;
			nodename=parentnode->position;
			for(int m=0;m<=d1;m++)
			{
				if(parentnode->uppath[m]!=0)
				{
					flag=true;
					break;
				}
			}
			if(flag==true)
			{
				if(used[nodename]==false)
				{
					//此处应为一般间隙代码
					for(int t=ptn_len;t>i;t--)
					{
						if(nodename==apath[t])
						{
							flag1=true;
							break;
						}
					}
					if(flag1==true)
						continue;
					if(firstfind==false)
					{
						firstfind=true;
						mincost=0;
						for(m=0;m<=d1;m++)
							mincost+=nodeinpath[nodename][m];
						pos_j=j;
					}
					else//如果找到比较位置相关数。选择位置相关数小的
					{
						int cost_j=0;
						for(m=0;m<=d1;m++)
						{
							cost_j+=nodeinpath[nodename][m];
						}
						if(mincost>cost_j)
						{
							mincost=cost_j;
							pos_j=j;
							//cout<<mincost<<" > "<<cost_j<<endl;
						}
						
						else if(mincost==cost_j)//位置相关数相同时，选择路径分支数大的
						{
							//cout<<mincost<<" = "<<cost_j<<endl;
							//cout<<"计算一次路径分支数"<<endl;
							
							int nodenamea=0,nodenameb=0;
							int k;
							int level=i;
							point *rangestart,*rangeend;
							rangestart=new point[level+1];
							rangeend=new point[level+1];
							node *p,*q;
							p=childnode->parent[0].p;
							q=childnode->parent[childnode->p_count].p;
							for(k=level;k>0;k--)
							{
								rangestart[k].p=p;
								rangeend[k].p=q;
								p=p->parent[0].p;
								q=q->parent[q->p_count].p;
							}
							rangestart[k].p=p;
							rangeend[k].p=q;
							//cout<<p->position<<" and "<<q->position<<endl; //输出祖先集的第一层范围
							
							for(k=0;k<=level;k++)   //更新祖先集中的树根路径数
							{
								node *tmp1=rangestart[k].p;
								if(k==0)  //第一层
								{
									while(1)
									{
										if(tmp1==rangeend[k].p)  //如果只有一个结点
										{
											if(tmp1->dis==true)
											{
												tmp1->localpath[0]=1;
												for(int n=1;n<d+1;n++)
													tmp1->localpath[n]=0;
											}
											if(d>0&&tmp1->dis==false)
											{												
												for(int n=0;n<d+1;n++)
													tmp1->localpath[n]=0;
												tmp1->localpath[tmp1->deltadistance]=1;
											}

											//输出
											//cout<<"祖先:";
											//for(int n=0;n<d+1;n++)
											//		cout<<tmp1->localpath[n]<<" ";
											break;
										}
										else  //按照从左往右的顺序开始
										{
											if(tmp1->dis==true)
											{
												tmp1->localpath[0]=1;
												for(int n=1;n<d+1;n++)
													tmp1->localpath[n]=0;
											}
											if(d>0&&tmp1->dis==false)
											{
												for(int n=0;n<d+1;n++)
													tmp1->localpath[n]=0;
												tmp1->localpath[tmp1->deltadistance]=1;
											}
											//输出
											//cout<<"祖先:";
											//for(int n=0;n<d+1;n++)
											//		cout<<tmp1->localpath[n]<<" ";
											tmp1=tmp1->next;
										}
									}
								}
								else  //其他层
								{
									while(1)
									{
										int h;
										if(tmp1==rangeend[k].p)
										{
											if(tmp1->dis==true)
											{
												for(int n=0;n<d+1;n++)
												{
													for(h=0;h<=tmp1->p_count;h++)
														tmp1->localpath[n]+=(tmp1->parent[h]).p->localpath[n];
												}
											}
											if(tmp1->dis==false&&d>0)
											{
												
												for(int n=0;n<d+1;n++)
												{
													for(h=0;h<=tmp1->p_count;h++)
													{
														if(n<tmp1->deltadistance)
															tmp1->localpath[n]=0;
														else
															tmp1->localpath[n]+=(tmp1->parent[h]).p->localpath[n-tmp1->deltadistance];
													}
												}
											}

											//输出
											//cout<<"祖先:";
											//for(int n=0;n<d+1;n++)
											//		cout<<tmp1->localpath[n]<<" ";
											break;
										}
										else
										{
											if(tmp1->dis==true)
											{
												for(int n=0;n<d+1;n++)
												{
													for(h=0;h<=tmp1->p_count;h++)
														tmp1->localpath[n]+=(tmp1->parent[h]).p->localpath[n];
												}
											}
											if(tmp1->dis==false&&d>0)
											{
												for(int n=0;n<d+1;n++)
												{
													for(h=0;h<=tmp1->p_count;h++)
													{
														if(n<tmp1->deltadistance)
															tmp1->localpath[n]=0;
														else
															tmp1->localpath[n]+=(tmp1->parent[h]).p->localpath[n-tmp1->deltadistance];
													}
												}
											}
											//输出
											//cout<<"祖先:";
											//for(int n=0;n<d+1;n++)
											//		cout<<tmp1->localpath[n]<<" ";
											tmp1=tmp1->next;
										}
									}
								}
							} //end for

							//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
							int **local;
							local=new int*[seq_len];
							for(int x=0;x<seq_len;x++)
								local[x]=new int[d+1];
							for(m=0;m<seq_len;m++)
							{
								for(int n=0;n<d+1;n++)
									local[m][n]=0;
							}
							for(k=0;k<=level;k++)  //计算出每个名称下的路径分支数（除了想要对比的两个，多计算了很多没用的结点，有待提高）
							{
								node *tmp1;
								tmp1=rangestart[k].p;
								int tmp1name;
								while(1)
								{
									if(tmp1==rangeend[k].p)
									{
										tmp1name=tmp1->position;
										for(int n=0;n<d+1;n++)
											local[tmp1name][n]+=tmp1->localpath[n];
										break;
									}
									else
									{
										tmp1name=tmp1->position;
										for(int n=0;n<d+1;n++)
											local[tmp1name][n]+=tmp1->localpath[n];
										tmp1=tmp1->next;
									}
								}
							}
							for(m=0;m<=d1;m++)
							{
								nodenamea+=local[((childnode->parent[j]).p->position)][m];
								nodenameb+=local[((childnode->parent[pos_j]).p->position)][m];
							}
							//输出两个路径分支数，比较其大小，选择较大的，一样的话，不变！
							//cout<<"较小名称结点路径分支数为 ： "<<nodenamea<<" 较大结点路径分支数为 ： "<<nodenameb<<endl;
							if(nodenamea>nodenameb)
							{
								mincost=cost_j;
								pos_j=j;
							}
							delete[]local;
							delete[]rangeend;
							delete[]rangestart;							
						}
					}//end		
				}
			}
		}
		j=pos_j;
		if(j>=0)
		{
			parentnode=childnode->parent[j].p;
			nodename=parentnode->position;
			array[i].p=parentnode;
			apath[i]=nodename;
			if(parentnode->dis==false)
				//d1--;
				d1=d1-parentnode->deltadistance;
			childnode=parentnode;
			pos[i]=j;
			i--;
		}
		else
		{
			for(int j=0;j<=i;j++)
				pos[j]=-1;
			i++;
			if(i>=ptn_len)
			{
				delete[]array;
				delete[]pos;
				return false;
			}
			if((array[i].p)->dis==false)
				//d1++;
				d1=d1+(array[i].p)->deltadistance;
			childnode=array[i+1].p;
			//	cal_partinpath(childnode,i-1,used,apath,nodeinpath);
		}
	}
	delete[]array;
	delete[]pos;
	return true;
}

/**********************************最右策略********************************/
bool RightMost_Occ(node *start_off,int *apath,bool *used)
{
	point *array;
	array=new point[ptn_len+1];
	array[ptn_len].p=start_off;
	int start=start_off->position;
	apath[ptn_len]=start;
	node *parentnode,*childnode;
	childnode=start_off;
	int *pos=new int[ptn_len+1];
	int i;
	int n;
	for(i=0;i<ptn_len+1;i++)
		pos[i]=-1;
	pos[ptn_len]=0;
	int d1=d;
	if(start_off->dis==false)
		//d1--;
		d1=d1-start_off->deltadistance;  //----------------------
	for(i=ptn_len-1;i>=0;)
	{
		n=childnode->p_count;
		int nodename,j;
		if(pos[i]==-1)
			j=n;
		else
			j=pos[i]-1;
		int pos_j=-1;
		for(;j>=0;j--)
		{
			bool flag=false,flag1=false;//用来记录是否满足近似条件
			parentnode=childnode->parent[j].p;
			nodename=parentnode->position;
			for(int m=0;m<=d1;m++)
			{
				if(parentnode->uppath[m]!=0)
				{
					flag1=true;
					break;
				}
			}
			if(flag1==true)
			{
				if(used[nodename]==false)
				{
					for(int h=ptn_len;h>i;h--)
					{
						if(nodename==apath[h])
						{
							flag=true;
							break;
						}
					}
					if(flag==true)
						continue;
					pos_j=j;
					break;
				}
			}
			
		}
		j=pos_j;
		if(j>=0)
		{
			parentnode=childnode->parent[j].p;
			nodename=parentnode->position;
			array[i].p=parentnode;
			apath[i]=nodename;
			childnode=parentnode;
			pos[i]=j;
			if(parentnode->dis==false)
				//d1--;
				d1=d1-parentnode->deltadistance;
			i--;
		}
		else
		{
			for(int j=0;j<=i;j++)
				pos[j]=-1;
			i++;
			if(i>=ptn_len)
			{
				delete[]array;
				delete[]pos;
				return false;
			}
			if((array[i].p)->dis==false)
				//d1++;
				d1=d1+(array[i].p)->deltadistance;
			childnode=array[i+1].p;
		}
	}
	delete[]array;
	delete[]pos;
	return true;
}

/***********************找最右出现***********************/
void disp_most_right_occ()
{
	int i;
	int count=0;
	int m1=head_level[ptn_len].count+1;
	int **nodeinpath;
	nodeinpath=new int*[seq_len];
	for(i=0;i<seq_len;i++)
		nodeinpath[i]=new int[d+1];
	bool *used;
	used=new bool[seq_len];
	for(i=0;i<seq_len;i++)
		used[i]=false;
	//	int *apath;
	//	apath=new int[ptn_len+1];
	int *apath2;
	apath2=new int[ptn_len+1];
	node *start_off=head_level[ptn_len].tail;
	while(start_off!=NULL)
	{
		if(used[start_off->position]==false)//如果序列串中这个字符没有被使用
		{
			cal_nodeinpath(nodeinpath,used);//计算树根树叶路径数
			//	bool retval=Gready_Search_Occ(start_off,apath,used,nodeinpath);
			bool retval2=RightMost_Occ(start_off,apath2,used);
			if(retval2==true)
			{
				
				for(int j=0;j<ptn_len+1;j++)
					used[(apath2[j])]=true;
				/*
				printf("<");
				for(int k=0;k<ptn_len;k++)
					printf("%d,  ",apath2[k]);
				printf("%d>\n",apath2[ptn_len]);
				*/
				count++;
			}
			start_off=start_off->pre;
		}
		else
			start_off=start_off->pre;
	}
	//cout<<"出现个数："<<count<<endl;
	sumocc+=count;
}

/***********************找最优出现***********************/
void disp_Gready_Search_Occ()
{
	int i;
	int count=0;
	int m1=head_level[ptn_len].count+1;
	int **nodeinpath;
	nodeinpath=new int*[seq_len];
	for(i=0;i<seq_len;i++)
		nodeinpath[i]=new int[d+1];
	bool *used;
	used=new bool[seq_len];
	for(i=0;i<seq_len;i++)
		used[i]=false;
	//	int *apath;
	//	apath=new int[ptn_len+1];
	int *apath2;
	apath2=new int[ptn_len+1];
	node *start_off=head_level[ptn_len].tail;
	while(start_off!=NULL)
	{
		if(used[start_off->position]==false)//如果序列串中这个字符没有被使用
		{
			cal_nodeinpath(nodeinpath,used);//计算树根树叶路径数
			bool retval2=Gready_Search_Occ(start_off,apath2,used,nodeinpath);
			//	bool retval2=RightMost_Occ(start_off,apath2,used);
			if(retval2==true)
			{
				
				for(int j=0;j<ptn_len+1;j++)
					used[(apath2[j])]=true;
				/*
				printf("<");
				for(int k=0;k<ptn_len;k++)
					printf("%d,  ",apath2[k]);
				printf("%d>\n",apath2[ptn_len]);
				*/

				count++;
			}
			start_off=start_off->pre;
		}
		else
			start_off=start_off->pre;
	}
	//cout<<"出现个数："<<count<<endl;
	//cout<<"------------"<<count<<endl;
	sumocc+=count;  //统计总的出现数
}

/***********************找出现***********************/
void disp_occ()
{
	int i;
	int count=0;
	int m1=head_level[ptn_len].count+1;
	int **nodeinpath;
	nodeinpath=new int*[seq_len];
	for(i=0;i<seq_len;i++)
		nodeinpath[i]=new int[d+1];
	bool *used;
	used=new bool[seq_len];
	for(i=0;i<seq_len;i++)
		used[i]=false;
	int *apath;
	apath=new int[ptn_len+1];
	int *apath2;
	apath2=new int[ptn_len+1];
	
	node *start_off=head_level[ptn_len].tail;
	while(start_off!=NULL)
	{
		//cout<<"count is "<<count<<"\n";
		if(used[start_off->position]==false)//如果序列串中这个字符没有被使用
		{
			cal_nodeinpath(nodeinpath,used);//计算树根树叶路径数
			//输出网树
			//display_nettree();
			
			bool retval=Gready_Search_Occ(start_off,apath,used,nodeinpath);
			bool retval2=RightMost_Occ(start_off,apath2,used);
			if(retval==true && retval2==true)
			{
				int j;
				for(j=0;j<=ptn_len;j++)
				{
					used[(apath[j])]=true;
				}
				update_node_uppath(nodeinpath,used);
				int sum=0;
				{
					node *s=head_level[ptn_len].head;
					int l=0;
					for(;l<m1;l++)
					{
						for(int m=0;m<d+1;m++)
						{
							if(used[s->position]==false)
								sum+=s->uppath[m];
						}
						s=s->next;
					}
				}
				for(j=0;j<ptn_len+1;j++)
					used[(apath[j])]=false;
				for(j=0;j<ptn_len+1;j++)
					used[(apath2[j])]=true;
				update_node_uppath(nodeinpath,used);
				int sum2=0;
				{
					int l=0;
					node *s=head_level[ptn_len].head;
					for(;l<m1;l++)
					{
						for(int m=0;m<d+1;m++)
						{
							if(used[s->position]==false)
								sum2+=s->uppath[m];
						}
						s=s->next;
					}
				}
				//cout<<"sum is "<<sum<<" &sum2 is "<<sum2<<endl;
				if(sum<=sum2)
				{
					/*
					printf("(3) RightMost_Occ :\t<");
					for(int k=0;k<ptn_len;k++)
						printf("%d%c,  ",apath2[k],t[apath2[k]]);
					printf("%d%c>\n",apath2[ptn_len],t[apath2[ptn_len]]);
					*/

					count++;

					/*
					outFile.open("C:\\Users\\LeiLei\\Desktop\\result.txt",iostream::app);//outFile 与一个文本文件关联
					outFile<<count<<endl;
					outFile.close();
					*/
				}
				else
				{
					for(int k=0;k<=ptn_len;k++)
					{
						used[(apath2[k])]=false;
					}
					for(k=0;k<=ptn_len;k++)
					{
						used[(apath[k])]=true;
					}
					
					/*
					printf("(3) Gready_Search_Occ :\t<");
					for(k=0;k<ptn_len;k++)
						printf("%d%c,  ",apath[k],t[apath[k]]);
					printf("%d%c>\n",apath[ptn_len],t[apath[ptn_len]]);
					*/

					count++;

					/*
					outFile.open("C:\\Users\\LeiLei\\Desktop\\result.txt",iostream::app);//outFile 与一个文本文件关联
					outFile<<count<<endl;
					outFile.close();
					*/
				}
			}
			start_off=start_off->pre;
		}
		else
			start_off=start_off->pre;
		
	}
	//cout<<"The number of occ is "<<count<<endl;
	//cout<<"------------"<<count<<endl;
	sumocc+=count;  //统计总的出现数
}

/*************************主函数***********************/
int main()
{
	/*********输入模式串并转化**************/
	int k=0;
	//cout<<"please input pattern at first:"<<endl;
	//cin>>p;
	//char p0[100]="b[0,2]d[0,2]d[0,2]d";
	char p0[100]="b[0,2]c[0,2]d[0,2]d";


	//char p0[100]="f[0,4]d[0,4]d[0,4]d";
	
	/*char p1[100]="g[0,5]e[0,5]c[0,5]d[0,5]g[0,5]e[0,5]c[0,5]d";			
	char p2[100]="h[0,10]f[0,8]d[0,6]e[0,10]c";			
	char p3[100]="b[0,2]f[0,4]e[0,6]f[0,8]d[0,10]e";			
	char p4[100]="b[1,5]d[0,6]c[2,7]e[3,9]f[2,5]g[4,9]d[1,8]e[2,9]b";			
	char p5[100]="b[0,1]e[0,3]g[0,5]b[0,7]e[0,9]g";			
	char p6[100]="a[0,5]f[0,7]e[0,6]f[0,8]d[0,7]e[0,9]d";
	char p7[100]="c[0,5]g[0,5]f[0,5]a[0,5]e";			
	char p8[100]="a[1,5]d[2,5]e[3,5]c[2,5]h";			
	char p9[100]="b[2,8]f[2,8]e[2,8]j[2,8]c[2,8]b[2,8]d";			
	char p10[100]="e[1,6]f[0,5]b[3,8]d[2,7]h[1,5]e[3,7]b[1,9]d";
	*/

	/*
	char p1[100]="k[0,8]d[0,8]g[0,8]p[0,8]k[0,8]d[0,8]g[0,8]p";			
	char p2[100]="t[0,10]q[0,8]k[0,6]g[0,10]c";			
	char p3[100]="c[0,2]g[0,4]k[0,6]q[0,8]t[0,10]d";			
	char p4[100]="d[1,5]g[0,6]j[2,7]p[3,9]d[2,5]g[4,9]j[1,8]p[2,9]d";		
	char p5[100]="f[0,5]q[0,7]c[0,6]t[0,8]g[0,7]n[0,9]f";
	char p6[100]="q[0,5]t[0,5]g[0,5]r[0,5]f";					
	char p7[100]="j[2,8]h[2,8]d[2,8]k[2,8]j[2,8]h[2,8]d";			
	char p8[100]="i[1,6]l[0,5]s[3,8]d[2,7]a[1,5]e[3,7]q[1,9]f";			
	*/
	
	//char p1[200]="c[0,1]b[0,1]c";
	//char p1[100]="d[1,3]q[3,5]k[5,7]h";	
	//char p2[100]="c[0,2]g[2,4]q[4,6]k[6,8]h[8,10]t";	
	//char p3[100]="d[1,7]g[0,6]j[2,8]p[3,10]d[2,5]g[4,11]j[1,9]p";	

	
	char p1[100]="l[0,10]r[0,10]e[0,10]q[0,10]d";
	char p2[100]="r[1,9]g[1,11]d[1,13]e[1,9]c[1,11]k";		
	char p3[100]="t[0,9]q[0,9]k[0,9]g[0,9]c";			
	char p4[100]="d[1,8]g[1,8]l[1,8]d[1,8]g[1,8]l";			
	char p5[100]="d[1,8]g[1,8]l[1,8]d[1,8]g[1,8]l[1,8]d[1,8]g[1,8]l";		
	char p6[100]="k[2,5]q[2,7]s[2,9]f[2,11]i";
	char p7[100]="k[2,7]q[2,9]s[2,11]f[2,13]i";					
	char p8[100]="k[2,9]q[2,11]s[2,13]f[2,15]i";


	//char p9[100]="r[0,5]e[0,5]b[0,5]d[0,5]r[0,5]q";
	//char p9[100]="b[0,6]f[0,6]k[0,6]r[0,6]r[0,6]q[0,6]b";

	//char p10[100]="c[0,5]e[0,5]q[0,5]r[0,5]e[0,5]m[0,5]o";
	//char p10[100]="r[0,8]m[0,8]e[0,8]b[0,8]d[0,8]m[0,8]q";
	//char p10[100]="r[0,6]m[0,6]e[0,6]b[0,6]d[0,6]m[0,6]q";
	char p10[100]="o[0,6]m[0,6]d[0,6]e[0,6]r[0,6]s[0,6]s";

	strcat(p,p8);
	convert_p_to_ruler(p);
	disp_pattern();
	
	/***************输入近似度********************/
	cout<<"Input the delta:";
	cin>>delta;
	cout<<"Input the gama:";
	cin>>d;
	/********************读取序列串并存储******************/
	FILE *fp;
	if((fp=fopen("C:\\Users\\LeiLei\\Desktop\\DataSet\\SDB.txt","r"))==NULL)
		//if((fp=fopen("C:\\Users\\LeiLei\\Desktop\\fuhao.txt","r"))==NULL)
	{
		cout<<"\nconnot open file strike any key exit!"<<endl;
		exit(1);
	}
	DWORD   dwBeginTime=GetTickCount();
	while(fscanf(fp,"%s",t)==1)
	{
		createlevel();
		seq_len=strlen(t);
		seq_sumlen+=seq_len;
		//cout<<"S is "<<t<<endl;
		//cout<<"/////"<<seq_len<<endl;
		//cal_needrep();
		/******************建立网树******************/
		//for(int k=0;k<10;++k)
		//{
			create_store(t);
			
			//cout<<"-------------------------------"<<endl;
			//找出现
			
			//disp_most_right_occ();       
			//disp_Gready_Search_Occ();
			disp_occ();

		//}
		
	}
	DWORD   dwEndTime0=GetTickCount();
	int time0=dwEndTime0-dwBeginTime;
	cout<<"-------------------------------"<<endl;
	
	fclose(fp);
	//cout<<"length of the seq is "<<seq_sumlen<<endl;
	cout<<"sum of occ is "<<sumocc<<endl;
	cout<<"The time cost is: "<<time0 / 1.0<<endl;	
	return 0;
}
