#ifndef COMMON_H
#define COMMON_H

#include "tree.h"
#define YYSTYPE TreeNode *

//��ʶ�����������ƺͷ��ű��е�ָ��
/*
    type��id�����ͣ��������δ�����﷨�����в��ܻ�ã�ֻҪ�趨һ��ȫ�ֱ���last_type�����������type�ؼ��֣���
    �����ڵ�һ�ζ���ʱȷ������type������Ͳ����Ը��ˣ������ǲ�ͬ���������ͬ��������
*/
struct identifier{
    string name;
    int no; //������ı��
    string type;
    identifier(){}
    identifier(string n,int num,string t){
        name=n;
        no=num;
        type=t;
    }
};

//�ṹ��action_zon��������ÿ��zone����һ�����ű��һ�����ڵ�ֵ
struct action_zone{
    vector<identifier>id_list;//���ű������ؼ�
    int father;
    action_zone(){
        father=-1;
    }
};

#endif