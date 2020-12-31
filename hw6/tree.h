#ifndef TREE_H
#define TREE_H
#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <vector>
#include <queue>
using namespace std;

using std::cerr;
using std::cout;
using std::endl;
using std::string;
enum NodeType{
    NODE_STMT,//�������
    NODE_TYPE,//������
    NODE_CONST,//����������int_value,char_value��
    NODE_BOOL,
    NODE_VAR,//������id
    NODE_OP,//�����
    NODE_EXPR,
    NODE_PROG,//���������ɸ���stmt����
    NODE_PARA,//��������int a
    NODE_STMTS//һ����������stmt�ļ���
};

enum StmtType{
    STMT_DECL,//decl��a/a=4����������Ԫ��element��
    STMT_IF,
    STMT_WHILE,
    STMT_PRINTF,
    STMT_SCANF,
    STMT_FOR,
    STMT_RETURN,
    STMT_FUNC
};


enum OpType{
    OP_EQUAL,
    OP_NOT,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_LARGER,
    OP_SMALLER,
    OP_LARGER_EQUAL,
    OP_SMALLER_EQUAL,
    OP_NOT_EQUAL,
    OP_AND,
    OP_OR,
    OP_ADD_SELF,
    OP_SUB_SELF,
    OP_ADD_ONE,
    OP_SUB_ONE
};


enum VarType{
    VAR_INTEGER,
    VAR_VOID,
    VAR_CHAR,
    VAR_STRING,
    VAR_FLOAT,
    Notype
};

union NodeAttr {
	int op;
	int vali;
	char valc;
	int symtbl_seq;
	
	NodeAttr(void) { op = 0; }
	NodeAttr(int i)	{ op = i; }
	NodeAttr(char c) { valc = c; }
};

struct Label {
    string true_label="";
	string false_label="";
	string begin_label="";
	string next_label="";
};

struct TreeNode {
    int nodeID;//�﷨���еı��
    int lineno;//�к�
    int zone;//������
    NodeType nodeType;//�ܽڵ�����

    //�������Ͷ�Ӧ��ֵ
    int int_val;
    float float_val;
    bool bool_val;
    char char_val;
    string string_val;

    StmtType stmtType;
    OpType opType;
    VarType varType;
    string var_name;


    TreeNode *child = nullptr;
    TreeNode *sibling = nullptr;//���������ӽڵ���

    Label label;
    NodeAttr attr;
    int temp_var;//�������

    TreeNode(NodeType type){
        nodeType=type;//��ȷ��������
        //��ȷ���������������
        int_val=float_val=char_val=0;
        string_val="";
        varType=Notype;
    }

    /*����ӽڵ�ģ��*/
    void addChild(TreeNode *);
    void addSibling(TreeNode *);

    /*��ӡ�﷨��ģ��*/
    void genNodeId();//�Ӹ��ڵ㿪ʼ�����Id
    void printAST();//��ӡ�﷨��s���
    void printNodeInfo(TreeNode *node);
    void printNodeConnection(TreeNode *node);
    void printNodeTypeInfo(TreeNode *node);

};
struct str_node{
    TreeNode *str=nullptr;
    int no;
    str_node(){}
    str_node(TreeNode *a,int b){
        str=a;
        no=b;
    }
};

//���ṹ
struct Tree{
    TreeNode *root=nullptr;
    int label_seq=0;
    int node_seq = 0;
	int temp_var_seq = 0;
    /*���ͼ��ģ��*/
    VarType find_id_type(TreeNode *id);
    void type_check(TreeNode *root);
    void stmt_check(TreeNode *root);

    void get_temp_var(TreeNode *root);

    /*��ǩ����ģ��*/
    void get_label();
    string new_label();
    void get_label(TreeNode *node);
    void recursive_get_label(TreeNode *root);
    void stmt_get_label(TreeNode *node);
    void expr_get_label(TreeNode *root);

    /*��������ģ��*/
    void gen_code(TreeNode *root);
    void recursive_gen_code(TreeNode *node);
    void stmt_gen_code(TreeNode *root);
    void expr_gen_code(TreeNode *root);

    void gen_header();
    void gen_decl();

    //С���ߺ���
    void print_value(TreeNode *node);
    void deal_with_judge(TreeNode *judge);
    void jump_true(TreeNode *judge);
    void jump_false(TreeNode *judge);

    //ר�Ŵ��ַ����Ķ��У�����printf��scanf
    queue<str_node>strq;
    //������Ķ��У�����ȫ�ֱ���
    string varq[20];
    int varq_len=0;
};

#endif