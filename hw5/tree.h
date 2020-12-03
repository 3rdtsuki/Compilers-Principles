#ifndef TREE_H
#define TREE_H
#include<iostream>
#include <string>
#include <cstring>
#include <iomanip>
using namespace std;

using std::cerr;
using std::cout;
using std::endl;
using std::string;

enum NodeType{
    NODE_CONST,//����
    NODE_BOOL,
    NODE_VAR,
    NODE_EXPR,
    NODE_TYPE,
    NODE_STMT,
    NODE_PROG,//��������
    NODE_OP,
    NODE_PARA,//��������int a
    NODE_FUNC,
    NODE_DECL_LIST,//����list
    NODE_CHAR,
    NODE_STRING
};

enum StmtType{
    STMT_IF,
    STMT_WHILE,
    STMT_DECL,
    STMT_ASSIGN,
    STMT_PRINTF,
    STMT_SCANF,
    STMT_FOR,
    STMT_RETURN,
    STMTS
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
    VAR_STRING
};

struct TreeNode {
    int nodeID;
    NodeType nodeType;
    //��������
    int int_val;
    bool bool_val;
    char char_val;
    char *string_val;
    StmtType stmtType;
    OpType opType;
    VarType varType;
    string var_name;

    int zone;//������

    TreeNode *child = nullptr;
    TreeNode *sibling = nullptr;//���������ӽڵ���


    TreeNode(NodeType type){
        nodeType=type;//��ȷ��������
        //��ȷ���������������

    }
    void addChild(TreeNode *);
    void addSibling(TreeNode *);

    void genNodeId();//�Ӹ��ڵ㿪ʼ�����Id ʵ�ַ�ʽͬѧ�ǿ��������޸�

    void printAST();//��ӡ�﷨��s���
    /***
     * ���µļ���������Ϊ��printAST�����и������ʹ��
     * ͬѧ�ǿ��Ը�����Ҫ�Լ�ʹ����������
    ***/
    void printNodeInfo(TreeNode *node);
    void printNodeConnection(TreeNode *node);
    void printNodeTypeInfo(TreeNode *node);
};
#endif