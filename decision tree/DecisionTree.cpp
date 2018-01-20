#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

const int TP=0,FP=1,FN=2,TN=3;
const int setosa=0,versicolor=1,virginica=2;

int statitc[3][4];

class iris{
public:
	double Feature[4];
   	string Iris_Class;
   	iris(double a,double b,double c,double d,string e){
		Feature[0]=a,Feature[1]=b,Feature[2]=c,Feature[3]=d;
		Iris_Class=e;
	}
   	friend ostream& operator<<(ostream& os,const iris& a){
   		os<<a.Feature[0]<<","<<a.Feature[1]<<","<<a.Feature[2]<<","<<a.Feature[3]<<","<<a.Iris_Class;
   		return os;
   	}
};

class Node{
public:
	double info_gain,Threshold;
	int Feature;
	string Iris_Class;
	Node *left,*right;
	Node(){
		info_gain=Threshold=0.0f;
		Feature=0;
		left=right=NULL;
		Iris_Class="";
	};
	Node(double a,double b,int c){
		info_gain=a,Threshold=b;
		Feature=c;
		left=right=NULL;
		Iris_Class="";
	}
	friend ostream& operator<<(ostream& os,const Node& a){
		os<<"info_gain:"<<a.info_gain<<" Threshold:"<<a.Threshold<<" feature:"<<a.Feature<<" Iris_Class:"<<a.Iris_Class;
		return os;
	}
};

int feature_num;
bool cmp(const iris& a,const iris& b){return a.Feature[feature_num]<b.Feature[feature_num];}

typedef vector<iris>::iterator Iter;

double getEntropy(vector<iris> Set){
	if(Set.size()==1) return 0.0f;
	int class_count[3]={0};
	double entropy=0.0f;
	for(int i=0;i<Set.size();i++){
		if(Set[i].Iris_Class=="Iris-setosa") class_count[0]++;
		else if(Set[i].Iris_Class=="Iris-versicolor") class_count[1]++;
		else if(Set[i].Iris_Class=="Iris-virginica") class_count[2]++;
	}
	for(int i=0;i<3;i++){
		if(class_count[i])
			entropy+=(double(class_count[i])/Set.size())*(log2(double(class_count[i])/Set.size()));
	}
	return (entropy==0?0:-1*entropy);
}

void divid_data(vector<iris> Set,vector<iris>& left,vector<iris>& right,int feature,double threshold){
	for(int i=0;i<Set.size();i++){
		if(Set[i].Feature[feature]<threshold) left.push_back(Set[i]);
		else right.push_back(Set[i]);
	}
}

Node findThreshold(vector<iris> Set){
	double threshold,max_info_gain=-10000.0f;
	int feature;
	double entropy=getEntropy(Set);
	for(int i=0;i<4;i++){
		feature_num=i;
		sort(Set.begin(),Set.end(),cmp);
		vector<double> candidate_threshold;
		for(int j=1;j<Set.size();j++){
			if(Set[j-1].Iris_Class!=Set[j].Iris_Class){
				candidate_threshold.push_back((Set[j-1].Feature[i]+Set[j].Feature[i])/2.0);
			}
		}
		for(int j=0;j<candidate_threshold.size();j++){
			vector<iris> leftsub,rightsub;
			divid_data(Set,leftsub,rightsub,i,candidate_threshold[j]);
			double info_gain=entropy-(getEntropy(leftsub)*leftsub.size()+getEntropy(rightsub)*rightsub.size())/Set.size();
			if(info_gain>max_info_gain){
				max_info_gain=info_gain;
				threshold=candidate_threshold[j];
				feature=i;
			}
		}
	}
	return Node(max_info_gain,threshold,feature);
}

Node* buildDecisionTree(vector<iris> Set){
	Node *node=new Node;
	*node=findThreshold(Set);
	vector <iris> leftsub,rightsub;
	divid_data(Set,leftsub,rightsub,node->Feature,node->Threshold);

	Node *left=new Node;
	if(!getEntropy(leftsub)) left->Iris_Class=leftsub[0].Iris_Class;
	else left=buildDecisionTree(leftsub);

	Node *right=new Node;
	if(!getEntropy(rightsub)) right->Iris_Class=rightsub[0].Iris_Class;
	else right=buildDecisionTree(rightsub);

	node->left=left;
	node->right=right;
	return node;
}

void traversal(Node *root,int tier){
	if(root){
		cout<<"Tier:"<<tier<<endl<<*root<<endl;
		traversal(root->left,tier+1);
		traversal(root->right,tier+1);
	}
}

int check(Node *node,const iris& test){
	if(!node->info_gain&&!node->Threshold&&!node->Feature){
		cout<<node->Iris_Class<<" "<<test.Iris_Class<<endl;
		if(node->Iris_Class=="Iris-setosa"){
			if(test.Iris_Class=="Iris-setosa"){
				statitc[setosa][TP]++,statitc[versicolor][TN]++,statitc[virginica][TN]++;
				return 1;
			}
			else if(test.Iris_Class=="Iris-versicolor"){
				statitc[setosa][FP]++,statitc[versicolor][FN]++,statitc[virginica][TN]++;
				return 0;
			}
			else if(test.Iris_Class=="Iris-virginica"){
				statitc[setosa][FP]++,statitc[versicolor][TN]++,statitc[virginica][FN]++;
				return 0;
			}
		}
		else if(node->Iris_Class=="Iris-versicolor"){
			if(test.Iris_Class=="Iris-versicolor"){
				statitc[setosa][TN]++,statitc[versicolor][TP]++,statitc[virginica][TN]++;
				return 1;
			}
			else if(test.Iris_Class=="Iris-setosa"){
				statitc[setosa][FN]++,statitc[versicolor][FP]++,statitc[virginica][TN]++;
				return 0;
			}
			else if(test.Iris_Class=="Iris-virginica"){
				statitc[setosa][TN]++,statitc[versicolor][FP]++,statitc[virginica][FN]++;
				return 0;
			}
		}
		else if(node->Iris_Class=="Iris-virginica"){
			if(test.Iris_Class=="Iris-virginica"){
				statitc[setosa][TN]++,statitc[versicolor][TN]++,statitc[virginica][TP]++;
				return 1;
			}
			else if(test.Iris_Class=="Iris-setosa"){
				statitc[setosa][FN]++,statitc[versicolor][TN]++,statitc[virginica][FP]++;
				return 0;
			}
			else if(test.Iris_Class=="Iris-versicolor"){
				statitc[setosa][TN]++,statitc[versicolor][FN]++,statitc[virginica][FP]++;
				return 0;
			}
		}
	}
	else{
		if(test.Feature[node->Feature]<node->Threshold) check(node->left,test);
		else check(node->right,test);
	}
}

int main(int argc, char const *argv[]){
	double precision[3]={0.0f},recall[3]={0.0f};
	double accuracy=0.0f;
	for(int i=1;i<=5;i++){
		double a,b,c,d;
		char ch;
		string e;

		string train=to_string(i)+"-1",test=to_string(i)+"-2";
		cout<<train<<" "<<test<<endl;
		fstream fin;
		fin.open(train,ios::in);
		vector<iris> Iris;

		while(fin>>a>>ch>>b>>ch>>c>>ch>>d>>ch>>e) Iris.push_back(iris(a,b,c,d,e));
		fin.close();

		Node *root;
		root=buildDecisionTree(Iris);

		fin.open(test,ios::in);
		memset(statitc,0,sizeof(statitc));
		int total=0,correct=0;
		while(fin>>a>>ch>>b>>ch>>c>>ch>>d>>ch>>e){
			total++;
			iris testFeature(a,b,c,d,e);
			if(check(root,testFeature)) correct++;
		}
		cout<<(double)correct/total<<endl;
		accuracy+=(double)correct/total;
		for(int i=0;i<3;i++){
			precision[i]+=(double)statitc[i][TP]/(statitc[i][TP]+statitc[i][FP]);
			recall[i]+=(double)statitc[i][TP]/(statitc[i][TP]+statitc[i][FN]);
		}
		fin.close();
	}
	printf("%.3f\n",accuracy/5);
	for(int i=0;i<3;i++) printf("%.3f %.3f\n",precision[i]/5,recall[i]/5);
	return 0;
}