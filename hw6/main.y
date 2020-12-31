%{  //ע��%left�ĵط�Խ��������������ȼ�Խ�ߣ�
    #include"common.h"
    extern TreeNode * root;
    int yylex();
    int yyerror( char const * );
    extern action_zone zone[20];
%}
%defines

%start program

%token ID INT_VAL STRING_VAL CHAR_VAL FLOAT_VAL
%token IF ELSE WHILE FOR RETURN
%token INT VOID CHAR FLOAT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA QUOTA 
%token GET_ADDR
%token TRUE FALSE
%token NOT
%token ADD_ONE SUB_ONE
%token ADD_SELF SUB_SELF
%token MUL DIV MOD
%token ADD SUB
%token LARGER SMALLER LARGER_EQUAL SMALLER_EQUAL NOT_EQUAL
%token EQUAL
%token AND OR
%token ASSIGN
%token PRINTF SCANF

%right NOT
%right ADD_ONE SUB_ONE
%right ADD_SELF SUB_SELF
%left ADD SUB
%left MUL DIV MOD
%left LARGER SMALLER LARGER_EQUAL SMALLER_EQUAL NOT_EQUAL
%left EQUAL
%right ASSIGN
%left OR 
%left AND
%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE 
%nonassoc UMINUS
%%
program
    : statements {
        root=new TreeNode(NODE_PROG);
        root->addChild($1);
    }
    ;
statements
    : statement {$$=$1;}
    | statements statement{$$=$1;$$->addSibling($2);}
    ;
statement
    : LBRACE statements RBRACE {//�����ţ���һ��������Ҫ��������������
        TreeNode *node=new TreeNode(NODE_STMTS);
        node->addChild($2);
        $$=node;
    }
    | function {$$=$1;}
    | type instruction SEMICOLON{//int a=2;
        $$=$2;
    }
    | instruction SEMICOLON{//a=2;
        $$=$1;
    }
    | scanf_statement {$$=$1;}
    | printf_statement {$$=$1;}
    | if_else_statement {$$=$1;}
    | while_statement {$$=$1;}
    | for_statement {$$=$1;}
    | return_statement {$$=$1;}
    | self_add SEMICOLON {$$=$1;}
    ;

instruction
    : instruction_element{
        $$=$1;  
    }
    | instruction COMMA instruction_element{
        $$=$1;
        $$->addSibling($3);
    }
    ;
instruction_element
    : ID {//a=2;
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_DECL;
        node->addChild($1);
        $$=node; 
    }
    | ID ASSIGN expr {
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_DECL;
        node->addChild($1);
        node->addChild($3);
        $$=node;  
    }
    ;

if_else_statement
    : IF LPAREN bool_expr RPAREN statement %prec LOWER_THEN_ELSE {
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_IF;
        node->addChild($3);
        node->addChild($5);
        $$=node;
    }
    | IF LPAREN bool_expr RPAREN statement ELSE statement {
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_IF;
        node->addChild($3);
        node->addChild($5);
        node->addChild($7);
        $$=node;
    }
    ;

while_statement
    : WHILE LPAREN bool_expr RPAREN statement {  //while(a>b)
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_WHILE;
        node->addChild($3);
        node->addChild($5);
        $$=node;
    }
    ;

for_statement
    : FOR LPAREN instruction SEMICOLON bool_expr SEMICOLON instruction RPAREN statement {
        //for(i=0; i<b; i++)
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_FOR;
        node->addChild($3);
        node->addChild($5);
        node->addChild($7);
        node->addChild($9);
        $$=node;
    }
    | FOR LPAREN type instruction SEMICOLON bool_expr SEMICOLON instruction RPAREN statement {
        //for(int i=0; i<b; i++)
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_FOR;
        node->addChild($4);
        node->addChild($6);
        node->addChild($8);
        node->addChild($10);
        $$=node;
    }
    ;

return_statement
    : RETURN expr SEMICOLON{
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_RETURN;
        node->addChild($2);
        $$=node;
    }
    ;


scanf_statement
    : SCANF LPAREN STRING_VAL COMMA GET_ADDR ID RPAREN SEMICOLON {//scanf("%c", &ch);
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_SCANF;
        node->addChild($3);
        node->addChild($6);
        $$=node;    
    }
    ;

printf_statement
    : PRINTF LPAREN STRING_VAL COMMA ID RPAREN SEMICOLON {//printf("hello, %d\n", i);
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_PRINTF;
        node->addChild($3);
        node->addChild($5);
        $$=node;   
    } 
    | PRINTF LPAREN STRING_VAL RPAREN SEMICOLON {//printf("hello, %d\n");
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_PRINTF;
        node->addChild($3);
        $$=node;   
    } 
    ;
bool_expr
    : TRUE {
        $$=$1;//true��false�ڵ����Ϣ��lex�������涨����
    }
    | FALSE {$$=$1;}
    | expr {//����һ�����ʽҲ���ԣ���0�Ƚ�
        $$=$1;
    }
    | LPAREN bool_expr RPAREN {//������ܻ�warning: 1 shift/reduce conflict
        $$=$2;
    }
    | expr EQUAL expr { //a+b==c���������ʽ������Ӧ���ǲ���
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_EQUAL;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;//�õ�expr���к�
        $$=node;
    }
    | expr LARGER expr { //a+b>c
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_LARGER;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;
    }
    | expr SMALLER expr { //a+b<c
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_SMALLER;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;
    }
    | expr LARGER_EQUAL expr { //a+b>=c
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_LARGER_EQUAL;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;
    }
    | expr SMALLER_EQUAL expr { //a+b<=c
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_SMALLER_EQUAL;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;
    }
    | expr NOT_EQUAL expr { //a+b!=c
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_NOT_EQUAL;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;
    }
    | bool_expr AND bool_expr{  //a>0 && a<=10 
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_AND;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;   
    }
    | bool_expr OR bool_expr{   //a<=10 || a%100==10 
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_OR;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;   
    }
    | NOT bool_expr{    //!a==0
        TreeNode *node=new TreeNode(NODE_BOOL);
        node->opType=OP_NOT;
        node->addChild($2);
        node->lineno=$2->lineno;
        $$=node;  
    }

    ;
expr
    : ID {$$=$1;}
    | INT_VAL {$$=$1;}
    | FLOAT_VAL {$$=$1;}
    | CHAR_VAL {$$=$1;}
    | STRING_VAL {$$=$1;}
    | LPAREN expr RPAREN {//������ܻ�warning: 1 shift/reduce conflict
        $$=$2;
    }
    | expr ADD expr {
        TreeNode *node=new TreeNode(NODE_EXPR);
        node->opType=OP_ADD;
        node->varType=Notype;//���ڻ���֪�����ͣ������ͼ��ʱ����
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;//����expr�ڵ���к����ӱ��ʽ���кž���
        $$=node;   
    }
    | expr SUB expr {
        TreeNode *node=new TreeNode(NODE_EXPR);
        node->opType=OP_SUB;
        node->varType=Notype;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;   
    }
    | expr MUL expr {
        TreeNode *node=new TreeNode(NODE_EXPR);
        node->opType=OP_MUL;
        node->varType=Notype;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;   
    }
    | expr DIV expr {
        TreeNode *node=new TreeNode(NODE_EXPR);
        node->opType=OP_DIV;
        node->varType=Notype;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;   
    }
    | expr MOD expr {
        TreeNode *node=new TreeNode(NODE_EXPR);
        node->opType=OP_MOD;
        node->varType=Notype;
        node->addChild($1);
        node->addChild($3);
        node->lineno=$1->lineno;
        $$=node;   
    }
   
    | SUB expr %prec UMINUS {//ǰ���Ǹ��ŵ����
        TreeNode *node=new TreeNode(NODE_EXPR);
        node->opType=OP_SUB;
        node->varType=Notype;
        node->addChild($2);
        node->lineno=$2->lineno;
        $$=node;  
    }
    | ADD expr %prec UMINUS {//ǰ�������ŵ����
        TreeNode *node=new TreeNode(NODE_EXPR);
        node->opType=OP_ADD;
        node->varType=Notype;
        node->addChild($2);
        node->lineno=$2->lineno;
        $$=node;  
    }
    ;

self_add
    : ID ADD_SELF expr { //a+=b����һ��stmt��
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_DECL;
        node->opType=OP_ADD_SELF;
        node->addChild($1);
        node->addChild($3);
        $$=node;   
    }
    | ID SUB_SELF expr{ //a-=b
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_DECL;
        node->opType=OP_SUB_SELF;
        node->addChild($1);
        node->addChild($3);
        $$=node;   
    }
    | ID ADD_ONE {//a++
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_DECL;
        node->opType=OP_ADD_ONE;
        node->addChild($1);
        $$=node;  
    }
    | ID SUB_ONE {//a--
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_DECL;
        node->opType=OP_SUB_ONE;
        node->addChild($1);
        $$=node;  
    }
type
    : INT {//�����"int"��������ĸ
        TreeNode *node=new TreeNode(NODE_TYPE);
        node->varType=VAR_INTEGER;
        $$=node; 
    }
    | VOID {
        TreeNode *node=new TreeNode(NODE_TYPE);
        node->varType=VAR_VOID;
        $$=node;         
    }
    | CHAR {
        TreeNode *node=new TreeNode(NODE_TYPE);
        node->varType=VAR_CHAR;
        $$=node;    
    }
    | FLOAT {
        TreeNode *node=new TreeNode(NODE_TYPE);
        node->varType=VAR_FLOAT;
        $$=node;    
    }
    ;

parameter_list
    : parameter {$$=$1;}
    | parameter_list COMMA parameter {  //int a,char b
        $$=$1;
        $$->addSibling($3);
    }
    ;
parameter
    : type ID { //int a
        TreeNode *node=new TreeNode(NODE_PARA);//��������
        node->varType=$1->varType;
        node->addChild($1);
        node->addChild($2);
        $$=node;
    }
    ;
function
    : type ID LPAREN parameter_list RPAREN statement{        //int f(int a,int b){}
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_FUNC;//��������
        node->varType=$1->varType;
        node->addChild($2);
        node->addChild($4);
        node->addChild($6);
        $$=node;
    }
    | type ID LPAREN RPAREN statement{   //int f()������paralist->�գ�����ʹ��epsilon
        TreeNode *node=new TreeNode(NODE_STMT);
        node->stmtType=STMT_FUNC;//��������
        node->varType=$1->varType;
        node->addChild($2);
        node->addChild($5);
        $$=node;
    }
    
    ;

%%
