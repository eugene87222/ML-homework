#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <queue>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

class Ecoli{
public:
	int ID=-1;
	string Ecoli_name="",Ecoli_class="";
	double Attr[9]={0};
	Ecoli(){}
	Ecoli(int id,string ecoli_name,double *attr,string ecoli_class){
		this->ID=id;
		this->Ecoli_name=ecoli_name;
		for(int i=0;i<9;i++) this->Attr[i]=attr[i];
		this->Ecoli_class=ecoli_class;
	}
	Ecoli(const Ecoli& a){
		this->ID=a.ID;
		this->Ecoli_name=a.Ecoli_name;
		this->Ecoli_class=a.Ecoli_class;
		for(int i=0;i<9;i++) this->Attr[i]=a.Attr[i];
	}
	friend ostream& operator<<(ostream& os,const Ecoli& a){
		os<<setw(4)<<a.ID<<" "<<setw(10)<<a.Ecoli_name<<" ";
		for(int i=0;i<9;i++) os<<setw(4)<<a.Attr[i]<<" ";
		os<<a.Ecoli_class;
		return os;
	}
};

int attr_num;
bool cmp(const Ecoli& a,const Ecoli& b){return a.Attr[attr_num]<b.Attr[attr_num];}

class Node{
public:
	Ecoli Ecoli_threshold;
	int Attr_num;
	double dis=0.0f;
	Node *parent=NULL,*left=NULL,*right=NULL;
	bool operator<(const Node& b) const{
		return (*this).dis<b.dis;
	}
};

Node *build_kd_tree(vector<Ecoli> Data_Set,int attr){
	Node *node=new Node;
	if(Data_Set.size()==1){
		node->Ecoli_threshold=Data_Set[0];
		node->Attr_num=-1;
		return node;
	}
	else{
		attr_num=attr;
		sort(Data_Set.begin(),Data_Set.end(),cmp);
		int mid=Data_Set.size()>>1;
		vector<Ecoli> LeftSub,RightSub;
		for(int i=0;i<Data_Set.size();i++){
			if(i==mid) continue;
			else if(Data_Set[i].Attr[attr]>=Data_Set[mid].Attr[attr]) RightSub.push_back(Data_Set[i]);
			else LeftSub.push_back(Data_Set[i]);
		}
		node->Ecoli_threshold=Data_Set[mid];
		node->Attr_num=attr;
		attr=(attr+1)%9;
		if(LeftSub.size()){
			Node *left=new Node;
			left=build_kd_tree(LeftSub,attr);
			node->left=left;
			node->left->parent=node;
		}
		if(RightSub.size()){
			Node *right=new Node;
			right=build_kd_tree(RightSub,attr);
			node->right=right;
			node->right->parent=node;
		}
		return node;
	}
}

void traverse(Node *node){
	if(node!=NULL){
		cout<<node->Ecoli_threshold<<" Attr_num:"<<node->Attr_num<<endl;
		traverse(node->left);
		traverse(node->right);
	}
	return;
}

double eucli_dis(const Ecoli& a,const Ecoli& b){
	double dis=0.0f;
	for(int i=0;i<9;i++) dis+=((a.Attr[i]-b.Attr[i])*(a.Attr[i]-b.Attr[i]));
	return sqrt(dis);
}

bool *is_prune;
Node *searchLeaf(Node *node,Ecoli query){
	if(node->left==NULL&&node->right==NULL) return node;
	else{
		if(query.Attr[node->Attr_num]>=node->Ecoli_threshold.Attr[node->Attr_num]){
			if(node->right!=NULL&&!is_prune[node->right->Ecoli_threshold.ID])
				return searchLeaf(node->right,query);
			else{
				if(node->left!=NULL&&!is_prune[node->left->Ecoli_threshold.ID])
					return searchLeaf(node->left,query);
				else return node;
			}
		}
		else{
			if(node->left!=NULL&&!is_prune[node->left->Ecoli_threshold.ID])
				return searchLeaf(node->left,query);
			else{
				if(node->right!=NULL&&!is_prune[node->right->Ecoli_threshold.ID])
					return searchLeaf(node->right,query);
				else return node;
			}
		}
	}
}

Node *get_parent(Node *node){
	is_prune[node->Ecoli_threshold.ID]=true;
	return node->parent;
}

priority_queue<Node> max_heap;
int k=10;
void search_KNN(Node *root,Ecoli query){
	Node *NN=root;
	while(NN!=NULL){
		NN=searchLeaf(NN,query);
		double dis=eucli_dis(NN->Ecoli_threshold,query);
		NN->dis=dis;
		max_heap.push(*NN);
		while(max_heap.size()>k) max_heap.pop();
		if(NN->parent==NULL) NN=NULL;
		else if(abs(NN->parent->Ecoli_threshold.Attr[NN->parent->Attr_num]-query.Attr[NN->parent->Attr_num])
				<max_heap.top().dis){
			if((NN->left==NULL&&NN->right==NULL)||
			(NN->left==NULL&&is_prune[NN->right->Ecoli_threshold.ID])||
			(NN->right==NULL&&is_prune[NN->left->Ecoli_threshold.ID])||
			(is_prune[NN->left->Ecoli_threshold.ID]&&is_prune[NN->right->Ecoli_threshold.ID])) NN=get_parent(NN);
			NN=searchLeaf(NN,query);
		}
		else NN=get_parent(NN);
	}
	return;
}

void parse_input(string input,int& id,string& ecoli_name,double *attr,string& ecoli_class){
	string token;
	stringstream ss(input);
	getline(ss,token,',');//read the index
	id=stoi(token);

	getline(ss,token,',');//read ecoli name
	ecoli_name=token;
	
	for(int i=0;i<9;i++){//read 9 attr
		getline(ss,token,',');
		attr[i]=stod(token);
	}
	
	getline(ss,token,',');//read ecoli class
	ecoli_class=token;
	return;
}

int main(int argc, char const *argv[]){
	int id,count=0;
	string input,ecoli_name,ecoli_class;
	double attr[9];
	vector<Ecoli> Ecoli_Data;
	fstream fin;

	fin.open(argv[1],ios::in);
	fin>>input;//first line (column name)
	
	while(fin>>input){ //read training data
		parse_input(input,id,ecoli_name,attr,ecoli_class);
		Ecoli_Data.push_back(Ecoli(id,ecoli_name,attr,ecoli_class));
	}fin.close();

	Node *root;
	root=build_kd_tree(Ecoli_Data,0);

	vector<Ecoli> Test_Data;
	fin.open(argv[2],ios::in);
	fin>>input;
	while(fin>>input){ //read test data
		parse_input(input,id,ecoli_name,attr,ecoli_class);
		Test_Data.push_back(Ecoli(id,ecoli_name,attr,ecoli_class));
	}fin.close();

	is_prune=new bool[Ecoli_Data.size()];
	int K[]={1,5,10,100};
	for(int n=0;n<4;n++){
		k=K[n];
		count=0;
		vector<Ecoli> KNN_vec[Test_Data.size()];
		for(int i=0;i<Test_Data.size();i++){
			fill(is_prune,is_prune+Ecoli_Data.size(),false);
			search_KNN(root,Test_Data[i]);
			while(max_heap.size()){
				KNN_vec[i].push_back(max_heap.top().Ecoli_threshold);
				max_heap.pop();
			}

			map<string,int> class_count; //check the prediction
			int max_count=0;
			string predict;
			for(int j=KNN_vec[i].size()-1;j>=0;j--){
				if(class_count[KNN_vec[i][j].Ecoli_class]) class_count[KNN_vec[i][j].Ecoli_class]++;
				else class_count[KNN_vec[i][j].Ecoli_class]=1;
				if(class_count[KNN_vec[i][j].Ecoli_class]>max_count){
					predict=KNN_vec[i][j].Ecoli_class;
					max_count=class_count[KNN_vec[i][j].Ecoli_class];
				}
			}
			if(predict==Test_Data[i].Ecoli_class) count++;
		}
		cout<<"KNN accuracy: "<<(double)count/Test_Data.size()<<endl;
		for(int i=0;i<3;i++){ //output knn of first 3 data
			for(int j=KNN_vec[i].size()-1;j>=0;j--) cout<<KNN_vec[i][j].ID<<" ";
			cout<<endl;
		}
		cout<<endl;
	}
	return 0;
}