#include "tree.h"
#include "common.h"
using namespace std;
string node_type[]={"stmt","type","const","bool","var","op","expr","prog","para","stmts"};
string stmt_type[]={"declare","if","while","printf","scanf","for","return","func","decl_char"};
string op_type[]={"==","!","+","-","*","/","%",">","<",">=","<=","!=","&&","||","+=","-=","++","--"};
string var_type[]={"int","void","char","string","float","notype"};
//�����ӽڵ㣨ע�⣬��Ϊ��֪���м������ӣ�����ÿ���ڵ��childֻ��һ����
//������Ķ��Ӽ���child->sibling�γ�һ������

void TreeNode::addChild(TreeNode *node){
    if(this->child!=NULL){
        this->child->addSibling(node);
    }
    else this->child=node;//�趨��һ������Ϊchild 
}

void TreeNode::addSibling(TreeNode *node){
    TreeNode *p=this;
    while (p->sibling){
        p=p->sibling;
    }
    p->sibling=node;
    
}

void TreeNode::genNodeId(){//�Ӹ��ڵ㿪ʼ�����Id
    int cnt=0;
    queue<TreeNode*>q;
    q.push(this);
    while(!q.empty()){
        TreeNode *p=q.front();
        q.pop();
        p->nodeID=cnt++;
        if(p->child){
            p=p->child;
            q.push(p);
            while(p->sibling){
                q.push(p->sibling);
                p=p->sibling;
            }
        }
    }
}

void TreeNode::printAST(){//��ӡ�﷨����㣬�������
    cerr<<"no/type/value/children\n";
    queue<TreeNode*>q;
    q.push(this);
    while(!q.empty()){
        TreeNode *p=q.front();
        q.pop();
        printNodeInfo(p);
        if(p->child){
            p=p->child;
            q.push(p);
            while(p->sibling){
                q.push(p->sibling);
                p=p->sibling;
            }
        }
    }
}
void TreeNode::printNodeInfo(TreeNode *node){
    cerr<<setw(8)<<node->nodeID;
    printNodeTypeInfo(node);
    printNodeConnection(node);
    cerr<<"\n";
}
void TreeNode::printNodeTypeInfo(TreeNode *node){
    cerr<<setw(16);
    const int size=8;
    switch(node->nodeType){
        case NODE_CONST:
            if(node->char_val)cerr<<"NODE_CHAR "<<setw(size)<<node->char_val;
            else if(node->float_val)cerr<<"NODE_FLOAT "<<setw(size)<<node->float_val;
            else if(node->string_val!="")cerr<<"NODE_STRING "<<setw(size)<<node->string_val;
            else cerr<<"NODE_INT "<<setw(size)<<node->int_val;
            break;
        case NODE_BOOL:
            cerr<<"NODE_BOOL "<<setw(size)<<op_type[node->opType];break;
        case NODE_VAR:
            cerr<<"NODE_VAR "<<setw(size)<<node->var_name;break;
        case NODE_TYPE:
            cerr<<"NODE_TYPE "<<setw(size)<<var_type[node->varType];break;
        case NODE_STMT:
            cerr<<"NODE_STMT "<<setw(size)<<stmt_type[node->stmtType];break;
        case NODE_PROG:
            cerr<<"NODE_PROG "<<setw(size)<<".";break;
        case NODE_EXPR:
            cerr<<"NODE_EXPR "<<setw(size)<<op_type[node->opType];break;
        case NODE_PARA:
            cerr<<"NODE_PARA "<<setw(size)<<var_type[node->varType];break;
        case NODE_STMTS:
            cerr<<"NODE_STMTS "<<setw(size)<<".";break;
    }
}
//����ӽڵ���
void TreeNode::printNodeConnection(TreeNode *p){
    if(p->child){
        cerr<<setw(12)<<'[';
        p=p->child;
        cerr<<p->nodeID;
        while(p->sibling){
            cerr<<','<<p->sibling->nodeID;
            p=p->sibling;
        }
        cerr<<']';
    }
}
//#######################################################################################
//################################ ���ͼ�� ###########################################
//#######################################################################################
extern action_zone zone[20];

//������������ÿ���м�ڵ�
void Tree::type_check(TreeNode *root){
    if(root->child==NULL){
        return;
    }
    TreeNode *p=root->child;
    type_check(p);
    while(p->sibling){
        type_check(p->sibling);
        p=p->sibling;
    }
    stmt_check(root);//�������������
    get_temp_var(root);//˳��������ʱ����
}


VarType Tree::find_id_type(TreeNode *id){//���ݷ��ű��id������
    if(id->nodeType==NODE_CONST || id->varType!=Notype){
        return id->varType;
    }
    int zone_no=id->zone;
    int len=zone[zone_no].id_list.size();
    for(int i=0;i<len;++i){
        if(zone[zone_no].id_list[i].name==id->var_name){
            string type = zone[zone_no].id_list[i].type;
            if(type=="int"){
                return VAR_INTEGER;
            }
            else if(type=="char"){
                return VAR_CHAR;
            }
            else if(type=="void"){
                return VAR_VOID;
            }
            else if(type=="float"){
                return VAR_FLOAT;
            }
        }
    }
    return Notype;
}
//����������ΪNODE_STMT�������м��
void Tree::stmt_check(TreeNode *node)
{
    //���ʽ��9+3.4��������������Ƿ���ͬ
    if(node->nodeType==NODE_EXPR){
        TreeNode *e1,*e2;
        e1=node->child;
        e2=e1->sibling;
        //˫Ŀ����
        if(e2){
            VarType type1=find_id_type(e1),type2=find_id_type(e2);
            if(type1!=type2)
            {
                cerr<<"line "<<e1->lineno<<": EXPR TypeError : "<<var_type[type1]<<" and "<<var_type[type2]<<endl;
            }
            node->varType=type1;
        }
        //��Ŀ���㣬-/+
        else{//���ø��ڵ�����
            node->varType=e1->varType;
        }
    }
    //������䣺a=2�����ʽǰ������ˣ�����ֻ���Ⱥ��ұ���һ����ֵ����id�����
    //����ұ���id��a=b����ô��������ű�ȷ����������
    else if(node->stmtType==STMT_DECL){
        TreeNode *id,*expr;
        id=node->child;
        expr=id->sibling;
        if(id && expr){
            VarType type1=find_id_type(id),type2=find_id_type(expr);
            if(type1!=type2)
            {
                cerr<<"line "<<id->lineno<<": DECL TypeError: "<<var_type[type1]<<" and "<<var_type[type2]<<endl;
            }
        }
    }

    else if(node->stmtType==STMT_SCANF){//scanf("%c",&a);
        TreeNode *type,*real;
        type=node->child;
        real=type->sibling;
        if(real==nullptr)return;
        if(type->string_val=="%c" && real->varType!=VAR_CHAR||
        type->string_val=="%d" && real->varType!=VAR_INTEGER||
        type->string_val=="%f" && real->varType!=VAR_FLOAT){
            cerr<<"line "<<real->lineno<<": SCANF TypeError: "<<type->string_val<<" and "<<var_type[real->varType]<<endl;
        }
    }

    else if(node->stmtType==STMT_PRINTF){
        TreeNode *type,*real;
        type=node->child;
        real=type->sibling;
        if(real==nullptr)return;
        if(type->string_val=="%c" && real->varType!=VAR_CHAR||
        type->string_val=="%d" && real->varType!=VAR_INTEGER||
        type->string_val=="%f" && real->varType!=VAR_FLOAT){
            cerr<<"line "<<real->lineno<<": PRINTF TypeError: "<<type->string_val<<" and "<<var_type[real->varType]<<endl;
        }
    }
    else if(node->stmtType==STMT_WHILE){//while��֤������ǲ�����a<b,a+b,������a++����equation
        TreeNode *judge;
        judge=node->child;
        VarType judge_type=find_id_type(judge);
        if(judge->nodeType!=NODE_BOOL){
            cerr<<"line "<<judge->lineno<<": WHILE TypeError: "<<var_type[judge_type]<<" is not boolean type"<<endl;
        }
    }
    else if(node->stmtType==STMT_IF){
        TreeNode *judge;
        judge=node->child;
        VarType judge_type=find_id_type(judge);
        if(judge->nodeType!=NODE_BOOL){
            cerr<<"line "<<judge->lineno<<": IF TypeError: "<<var_type[judge_type]<<" is not boolean type"<<endl;
        }
    }
    else if(node->stmtType==STMT_FOR){
        TreeNode *judge;
        judge=node->child->sibling;
        VarType judge_type=find_id_type(judge);
        if(judge->nodeType!=NODE_BOOL){
            cerr<<"line "<<judge->lineno<<": FOR TypeError: "<<var_type[judge_type]<<" is not boolean type"<<endl;
        }
    }
}

//#######################################################################################
//################################ ��ǩ���� ###########################################
//#######################################################################################


/*��ǩ����ģ��*/
void Tree::get_label(){
    TreeNode *p=root;
    p->label.begin_label = "_start";
    //��ÿ��stmt�ڵ���ǰ���úñ�ǩ���Ա��������ʱֱ��ʹ��
    for(p=root->child;p;p=p->sibling){
        recursive_get_label(p);
    }
}
//�����µ�label
string Tree::new_label(){
    char tmp[20];
    sprintf(tmp, ".L%d", label_seq);
    label_seq++;
    return tmp;
}
void Tree::recursive_get_label(TreeNode *node){
    if(node->nodeType==NODE_STMTS){
        TreeNode *stmts=node,*stmt;
        for(stmt=stmts->child;stmt;stmt=stmt->sibling){
            recursive_get_label(stmt);
        }
    }
    else if(node->nodeType==NODE_STMT){
        stmt_get_label(node);
    }
    else if(node->nodeType==NODE_EXPR||node->nodeType==NODE_BOOL){
        expr_get_label(node);
    }
}
//��stmt�ĸ����ӽڵ㸳��ǰ���ǩ(while,if,for)
//�趨judge��true��false��stmts��begin��next��t��begin��next�����һ������ǰ���б�ǩ������Ҫ�趨��
//ע�������stmts����һ����䣬�������ļ���
void Tree::stmt_get_label(TreeNode *t){
    switch(t->stmtType){
        case STMT_DECL:{
            TreeNode *id=t->child;
            int i;
            for(i=0;i<varq_len;++i){
                if(varq[i]->var_name==id->var_name){
                    break;
                }
            }
            if(i==varq_len){
                varq[varq_len++]=id;//���б�������ȫ�ֱ���
            }

            break;
        }
        case STMT_DECL_CHAR:{
            TreeNode *id=t->child;
            int i;
            for(i=0;i<varq_len;++i){
                if(varq[i]->var_name==id->var_name){
                    break;
                }
            }
            if(i==varq_len){
                varq[varq_len++]=id;//���б�������ȫ�ֱ���
            }
            break;
        }
        case STMT_IF:{//if�����֣�һ��û��else��һ����else
            /*
            û��else
            L0: judge
                jne L2
            L1: stmts
            L2:

            ��else
            L0: judge
                jne L2
            L1: stmts
                jmp L3
            L2: else_stmts
            L3:
            */
            TreeNode *judge,*stmts,*else_stmts;
            judge=t->child;
            stmts=judge->sibling;
            else_stmts=stmts->sibling;
            //û��else
            if(else_stmts==NULL){
                if (t->label.next_label == "")
                {
                    t->label.next_label = new_label();//L2
                }
                judge->label.true_label=stmts->label.begin_label= new_label();//L1
                judge->label.false_label=stmts->label.next_label=t->label.next_label;
                //judge�������ϵ���һ����룬next��ǩ�������תָ��ʱ������Ҫ
                judge->label.next_label=stmts->label.begin_label;
                if (t->sibling){
                    t->sibling->label.begin_label = t->label.next_label;
                }
                recursive_get_label(judge);
                recursive_get_label(stmts);
            }
            //��else
            else{
                if (t->label.next_label == "")
                {
                    t->label.next_label = new_label();//L3
                }
                judge->label.true_label=stmts->label.begin_label= new_label();//L1
                judge->label.false_label=else_stmts->label.begin_label= new_label();//L2

                judge->label.next_label=stmts->label.begin_label;
                stmts->label.next_label=else_stmts->label.begin_label;
                else_stmts->label.next_label=t->label.next_label;
                if (t->sibling){
                    t->sibling->label.begin_label = t->label.next_label;
                }
                recursive_get_label(judge);
                recursive_get_label(stmts);
                recursive_get_label(else_stmts);
            }
            break;
        }
        
        case STMT_WHILE:{
            /*node������while���
            
            L1: stmts
            L0: judge 
                je L1
            L2:

            */
            TreeNode *judge,*stmts;//�ж�����ѭ����
            judge=t->child;
            stmts=judge->sibling;
            //��������ѭ����ʼ�ı�ǩ
            if(t->label.begin_label==""){
                t->label.begin_label=new_label();//L0
            }
            //ѭ�������һ�������ѭ���Ŀ�ʼ
            stmts->label.next_label=t->label.begin_label;
            //����ж���ȷ����ʼѭ����
            stmts->label.begin_label=judge->label.true_label=new_label();//L1

            //��������ѭ�������ı��
            if (t->label.next_label == ""){
                t->label.next_label = new_label();//L2
            }
            //ѭ�������ļ�ֵ��ż�Ϊ����ѭ������һ�������
            judge->label.false_label = t->label.next_label;
            //�ֵܽڵ�Ŀ�ʼ��ż�Ϊ��ǰ�ڵ����һ�����ı��
            if (t->sibling){
                t->sibling->label.begin_label = t->label.next_label;
            }

            judge->label.next_label=t->label.next_label;
            //�ݹ�����
            recursive_get_label(judge);
            recursive_get_label(stmts);
            break;
        }

        case STMT_FOR:{
            //for ������ĸ����֣���ʼ����䣬judge���������trans��stmts
            /*for(init;judge;trans)stmts;
            ��ȥ�ж�judge�������ȷȥstmts��Ȼ�����trans
            L0: init
                jmp L2
            L1: stmts
                trans
            L2: judge
                ��ȷ�� L1
            L3:
            */
            TreeNode *init,*judge,*trans,*stmts;
            init=t->child;
            judge=init->sibling;
            trans=judge->sibling;
            stmts=trans->sibling;

            judge->label.true_label=new_label();//L1
            init->label.next_label=new_label();//L2

            if (t->label.next_label == ""){
                t->label.next_label = new_label();//L3
            }
            judge->label.false_label=t->label.next_label;


            if (t->sibling){
                t->sibling->label.begin_label = t->label.next_label;
            }
            judge->label.next_label=t->label.next_label;
            recursive_get_label(init);
            recursive_get_label(judge);
            recursive_get_label(trans);
            recursive_get_label(stmts);
            
            break;
        }
	    
        case STMT_FUNC:{
            TreeNode *stmts=t->child->sibling;
            recursive_get_label(stmts);
            break;
        }
    
        case STMT_SCANF:{
            TreeNode *str=t->child;
            strq.push(str_node(str,0));
            break;
        }
        case STMT_PRINTF:{
            TreeNode *str=t->child;
            strq.push(str_node(str,0));
            break;
        }
    }
}
//����bool���ʽ��ǩ(&&,||,!)
void Tree::expr_get_label(TreeNode *judge){
    if(judge->nodeType!=NODE_BOOL){
        return;
    }
    TreeNode *e1,*e2;
    e1=judge->child;
    e2=e1->sibling;
    //judge��true��false�̳и��ӽڵ�
    switch (judge->opType){
        //�����㣬�����һ��������������ԣ�����true��ǩ
        /*and�����ʽ����
        begin:
            e1
            jne false
            e2
            jne false
        true:######
        false:######
        */
        case OP_AND:{
            e1->label.begin_label=judge->label.begin_label;//�����Ŀ�ʼ��ǩ���������ã�Ϊ�˶��������ϵ��������31��

            e1->label.true_label=e2->label.begin_label=new_label();
            e2->label.true_label=judge->label.true_label;
            e1->label.false_label=e2->label.false_label=judge->label.false_label;

            e1->label.next_label=e2->label.begin_label;
            e2->label.next_label=judge->label.next_label;
            expr_get_label(e1);
            expr_get_label(e2);
            break;
        }
        /*or�����ʽ����
        begin:
            e1
            je ture
            e2
            jne false
        true:######
        false:######
        */
        case OP_OR:{
            e1->label.begin_label=judge->label.begin_label;

            e1->label.true_label=e2->label.true_label=judge->label.true_label;
            e1->label.false_label=e2->label.begin_label=new_label();
            e2->label.false_label=judge->label.false_label;
            //��������next��ǩ���������תָ��ʱ������Ҫ��
            e1->label.next_label=e2->label.begin_label;
            e2->label.next_label=judge->label.next_label;
            expr_get_label(e1);
            expr_get_label(e2);
            break;
        }
        /*
        begin:
            if e1 is true goto false
        true:#####
        false:#####
        */
        case OP_NOT:{
            e1->label.begin_label=judge->label.begin_label;

            e1->label.true_label=judge->label.false_label;
            e1->label.false_label=judge->label.true_label;
            e1->label.next_label=judge->label.next_label;
            expr_get_label(e1);
            break;
        }
    }
}

//#######################################################################################
//################################ �������� ###########################################
//#######################################################################################

//��һ��ɨ���﷨��ʱ�����ڱ��ʽ����a+b+c��Ҫ��ʱ�����洢a+b���ú���������Ϊ������
void Tree::get_temp_var(TreeNode *t){
    if (t->nodeType != NODE_EXPR)
		return;

	TreeNode *arg1 = t->child;
	TreeNode *arg2 = arg1->sibling;
    //��Ŀ���������������µ���ʱ����
	if (arg1->nodeType == NODE_OP){//-(a+b)
		temp_var_seq--;
    }
	if (arg2 && arg2->nodeType == NODE_OP){//a++
		temp_var_seq--;
    }
	t->temp_var = temp_var_seq;
	temp_var_seq++;
}

/*
�������ɵ����
��1������ͷ��
��2���ѱ�������ʱ�ͷ���ʱ�ģ���ǰ��
��3���������岿�֣�ͨ��recursive��stmt���д��ϵ��µĵݹ�
��4��ret
*/

void Tree::gen_code(TreeNode *root){
    get_label();
    gen_header();//����ͷ��

    //��һ��ʼ���뿪ʼ�����е�������䣬��Щ����ȫ�ֱ����������������ǰ��
    gen_decl();
    
    cout << endl << endl << "# your asm code here" << endl;
	cout << "\t.text" << endl;
    cout << "\t.globl _start" << endl;

    TreeNode *p;
    for(p=root->child;p;p=p->sibling){
        recursive_gen_code(p);//�������ɣ��Ӹ���ʼ�ݹ�
    }


    //printf�ر�
    cout<<"\tmovl\t$0, %eax\n";
	cout<<"\tmovl\t-4(%ebp), %ecx\n";
	cout<<"\tleave\n";
	cout<<"\tleal\t-4(%ecx), %esp\n";
	cout << "\tret" << endl;
    cout<<"\t.section\t.note.GNU-stack,\"\",@progbits\n";//����ʹ��ջ
}


void Tree::gen_header(){
    /*�����ַ���
    .LC0:
	.string	"%d"
	.text
	.globl	main
	.type	main, @function
    */
    int cnt=0;
    while(!strq.empty()){
        string s=strq.front().str->string_val;
        cout<<".LC"<<cnt<<":\n";
        cout<<"\t.string\t"<<s<<"\n";
        strq.front().str->int_val=cnt++;//int_val��Ϊ���
        strq.pop();
    }
    cout<<"\t.text\n\t.globl\tmain\n\t.type	main, @function\n";
    
}

//ȫ�ֱ�������ʱ���������������ǰ��
void Tree::gen_decl(){
    cout << endl << "# define your veriables and temp veriables here" << endl;
	cout << "\t.bss" << endl;

    //�������.comm	a,4,4
    for(int i=0;i<varq_len;++i){
        if(varq[i]->varType==VAR_INTEGER){
            cout<<"\t.comm "<<varq[i]->var_name<<",4,4"<<endl;
        }
        else if(varq[i]->varType==VAR_CHAR){
            cout<<"\t.comm "<<varq[i]->var_name<<",1,1"<<endl;
        }
    }
	//��ӡ��ʱ����
	for (int i = 0; i < temp_var_seq; i++)
	{
		cout<<"\t.comm "<< "t" <<  i <<",4,4"<<endl;
	}
}


//��һ���ڵ����ɴ���
void Tree::recursive_gen_code(TreeNode *node){
    if(node->nodeType==NODE_STMTS){
        TreeNode *stmts=node,*stmt;
        for(stmt=stmts->child;stmt;stmt=stmt->sibling){
            recursive_gen_code(stmt);
        }
    }
    else if(node->nodeType==NODE_STMT){
        stmt_gen_code(node);
    }
    else if(node->nodeType==NODE_EXPR||node->nodeType==NODE_BOOL){
        expr_gen_code(node);
    }
}


//node_stmt���ɴ���
/*jne:���cmpl������������������ת��je�����cmpl�����������������ת��jmp��ֱ����ת
��������testl�ǲ��Բ������Ƿ�Ϊ0��Ҳ����˵ϣ��Ϊ0
����jne:���testlΪ1����ת��je�����testlΪ0����ת
*/
void Tree::stmt_gen_code(TreeNode *t){
    if(t->stmtType==STMT_FUNC){
        TreeNode *id=t->child;
        TreeNode *stmts=id->sibling;
        cout<<id->var_name<<":\n";
        cout<<"\tendbr32\n";

        //printf��Ҫ���
        cout<<"\tleal\t4(%esp), %ecx\n";
        cout<<"\tandl\t$-16, %esp\n";
        cout<<"\tpushl\t-4(%ecx)\n";


        cout<<"\tpushl\t%ebp"<<endl;
        cout<<"\tmovl\t%esp, %ebp"<<endl;
        cout<<"\tpushl\t%ecx"<<endl;
        for(TreeNode *stmt=stmts->child;stmt;stmt=stmt->sibling){
            recursive_gen_code(stmt);
        }
    }
    else if(t->stmtType==STMT_DECL){
        /*��ֵ��䣬�����������
        a
        a=expr
        a+=expr��opType==OP_ADD_SELF,OP_SUB_SELF��
        a++��opType==OP_ADD_ONE,OP_SUB_ONE��
        */
        TreeNode *expr=t->child->sibling;
        if(t->opType==OP_ADD_SELF){
            recursive_gen_code(expr);
            if(expr->child){//�Ǳ��ʽ
                cout<<"\tmovl\tt"<<expr->temp_var<<", %eax\n";
                cout<<"\taddl\t%eax, "<<t->child->var_name<<endl;
            }
            else{//��id���߳���
                cout<<"\tmovl\t";
                print_value(expr);
                cout<<", %eax\n";
                cout<<"\taddl\t%eax, "<<t->child->var_name<<endl;
            }
        }
        else if(t->opType==OP_SUB_SELF){
            recursive_gen_code(expr);
            if(expr->child){//�Ǳ��ʽ
                cout<<"\tmovl\tt"<<expr->temp_var<<", %eax\n";
                cout<<"\tsubl\t%eax, "<<t->child->var_name<<endl;
            }
            else{//��id���߳���
                cout<<"\tmovl\t";
                print_value(expr);
                cout<<", %eax\n";
                cout<<"\tsubl\t%eax, "<<t->child->var_name<<endl;
            }
        }
        else if(t->opType==OP_ADD_ONE){
            cout<<"\taddl\t$1, "<<t->child->var_name<<endl;
        }
        else if(t->opType==OP_SUB_ONE){
            cout<<"\tsubl\t$1, "<<t->child->var_name<<endl;
        }
        else if(expr){//a=expr
            TreeNode *id=t->child;
            //���ȣ����ҵ���ĩ�ı��ʽ
            while(expr->child && expr->nodeType==NODE_STMT){
                expr=expr->child->sibling;
            }
            recursive_gen_code(expr);
            //�ѱ��ʽ��Ӧ����ʱ����������ߵ�ÿ��id��һ��С�Ŀ�ָ��
            while(id){
                if(expr->child){//�����Ǳ��ʽ
                    cout<<"\tmovl\tt"<<expr->temp_var<<", %eax\n";
                    cout<<"\tmovl\t%eax, "<<id->var_name<<endl;
                }
                else{//��id���߳���
                    cout<<"\tmovl\t";
                    print_value(expr);
                    cout<<", %eax\n";
                    cout<<"\tmovl\t%eax, "<<id->var_name<<endl;
                }
                if(id->sibling->nodeType==NODE_STMT){
                    id=id->sibling->child;
                }
                else break;
            }
        }
    }

    else if(t->stmtType==STMT_DECL_CHAR){//�ַ���ֵ
        /*
        movb	$57, s
        movb	s, %al
        movsbl	%al, %eax
        */
        TreeNode *id,*value;
        id=t->child;
        value=id->sibling;
        cout<<"\tmovb\t";
        print_value(value);
        cout<<", "<<id->var_name<<endl;

    }
    else if(t->stmtType==STMT_IF){
        TreeNode *judge,*stmts,*else_stmts;
        judge=t->child;
        stmts=judge->sibling;
        else_stmts=stmts->sibling;

        deal_with_judge(judge);//�ж����
        cout<<stmts->label.begin_label<<":\n";//��ȷ��ǩ
        recursive_gen_code(stmts);
        if(else_stmts){//�����else
            cout<<"\tjmp\t"<<else_stmts->label.next_label<<endl;
            cout<<stmts->label.next_label<<":\n";
            recursive_gen_code(else_stmts);
        }
        cout<<t->label.next_label<<":\n";
    } 
    //while���������judge��Ȼ��jump��judge��next
    else if(t->stmtType==STMT_WHILE){
        TreeNode *judge=t->child;
        TreeNode *stmts=judge->sibling;

        cout<<"\tjmp "<<t->label.begin_label<<endl;
        cout<<judge->label.true_label<<":\n";
        recursive_gen_code(stmts);

        cout<<t->label.begin_label<<":\n";    

        //��������Ҫ��judge�������Ƿ��Ž��л��֣��������﷨����������judge�Ĵ���
        deal_with_judge(judge);
        cout<<t->label.next_label<<":\n";
    }

    else if(t->stmtType==STMT_FOR){
        TreeNode *init,*judge,*trans,*stmts;
        init=t->child;
        judge=init->sibling;
        trans=judge->sibling;
        stmts=trans->sibling;

        recursive_gen_code(init);
        cout<<"\tjmp\t"<<init->label.next_label<<endl;
        cout<<judge->label.true_label<<":\n";
        recursive_gen_code(stmts);
        recursive_gen_code(trans);
        cout<<init->label.next_label<<":\n";

        deal_with_judge(judge);
        cout<<t->label.next_label<<":\n";
    }
    else if(t->stmtType==STMT_RETURN){
        TreeNode *expr=t->child;
        cout<<"\tmovl\t";
        print_value(expr);
        cout<<", %eax\n";
    }

    else if(t->stmtType==STMT_SCANF){
        /*
        subl	$12, %esp
        pushl	$b
        pushl	$.LC1
        call	scanf
        addl	$16, %esp
        */
        TreeNode *str,*id;
        str=t->child;
        id=str->sibling;
        cout<<"\tsubl\t$12, %esp\n";
        cout<<"\tpushl\t$";//ȡ��ַ
        print_value(id);
        cout<<endl;
        cout<<"\tpushl\t$.LC"<<str->int_val<<endl;//�ַ����ı�ǩ
        cout<<"\tcall\tscanf\n";
        cout<<"\taddl\t$16, %esp\n";
    }

    else if(t->stmtType==STMT_PRINTF){
        /*
        subl	$8, %esp
        pushl	a
        pushl	$.LC0
        call	printf
        addl	$16, %esp

	    movb	t, %al
        movsbl	%al, %eax
        subl	$12, %esp
        pushl	%eax
        call	putchar
        addl	$16, %esp
        */
        TreeNode *str,*id,*id2=NULL;
        str=t->child;
        id=str->sibling;
        VarType id_type=Notype;
        if(id){
            id_type=find_id_type(id);
            id2=id->sibling;
        }
        if(id==NULL ||id_type==VAR_INTEGER){
            cout<<"\tsubl\t$8, %esp\n";//����ջ��-8�����������ռ䣬����ѹջ
            if(id2){
                cout<<"\tpushl\t";
                print_value(id2);
                cout<<endl;
            }
            if(id){
                cout<<"\tpushl\t";
                print_value(id);//printf����������
                cout<<endl;
            }
            else{
                cout<<"\tpushl\t";
                cout<<"%eax";
                cout<<endl;
            }
            cout<<"\tpushl\t$.LC"<<str->int_val<<endl;//�ַ����ı�ǩ
            cout<<"\tcall\tprintf\n";
            cout<<"\taddl\t$16, %esp\n";
        }
        else if(id_type==VAR_CHAR){

            cout<<"\tmovb\t"<<id->var_name<<", %al\n";
            cout<<"\tmovsbl\t%al, %eax\n";
            cout<<"\tsubl\t$12, %esp\n";
            cout<<"\tpushl\t%eax"<<endl;
            cout<<"\tcall\tputchar\n";
            cout<<"\taddl\t$16, %esp\n";
        }
    }
}

//����stmt�����ͣ�judge�ڵ�
void Tree::deal_with_judge(TreeNode *judge){
    TreeNode *e1=judge->child;
    TreeNode *e2=e1->sibling;
    if(judge->opType==OP_NOT){
        deal_with_judge(e1);
    }
    else if(judge->opType==OP_OR||judge->opType==OP_AND){
        deal_with_judge(e1);
        deal_with_judge(e2);
    }
    else if(judge->nodeType==NODE_BOOL){//�ǲ������ʽ��Ԫ�������ٷ���
        //�����һ��Ŀ�ʼ��ǩ
        if(judge->label.begin_label!="")cout<<judge->label.begin_label<<":\n";
        recursive_gen_code(judge);
        //�����ת��ǩ������һ��Ŀ�ʼ��ǩ��ͬ���Ͳ����
        if(judge->label.true_label!="" && judge->label.next_label!=judge->label.true_label)
        {
            jump_true(judge);
            cout<<judge->label.true_label<<endl;
        }
        if(judge->label.false_label!=""&& judge->label.next_label!=judge->label.false_label)
        {
            jump_false(judge);
            cout<<judge->label.false_label<<endl;
        }
    }
}
//����expr�Ļ��
void Tree::expr_gen_code(TreeNode *t){
    TreeNode *e1,*e2;
    e1=t->child;
    e2=e1->sibling;
    //���ȣ�����ӽڵ㻹�Ǳ��ʽ����Ҫ�������ӽڵ�Ĵ��롣��a+b+c+d
    if(e1 && (e1->nodeType==NODE_EXPR)){
        expr_gen_code(e1);
    }
    if(e2 && (e2->nodeType==NODE_EXPR)){
        expr_gen_code(e2);
    }
    //˫Ŀ�����
    if(t->opType==OP_ADD && t->child->sibling){//e1+e2
        //���ж�e1������
        cout << "\tmovl\t";
        print_value(e1);
        cout<< ", %eax" <<endl;//��e1�������eax
        
        //�ж�e2������
        cout << "\taddl\t";
        print_value(e2);
        cout << ", %eax" << endl;
        
        cout << "\tmovl\t%eax, t" << t->temp_var << endl;//ÿ����һ������eax����һ����ʱ����
    }
    else if(t->opType==OP_SUB && t->child->sibling){//e1-e2��SUB��Ϊ˫Ŀ�����
        //���ж�e1������
        cout << "\tmovl\t";
        print_value(e1);//������ǳ�����ֵ
        cout<< ", %eax" <<endl;//��e1�������eax

        //�ж�e2������
        cout << "\tsubl\t";
        print_value(e2);
        cout << ", %eax" << endl;
        
        cout << "\tmovl\t%eax, t" << t->temp_var << endl;//ÿ����һ������eax����һ����ʱ����
    }
    else if(t->opType==OP_MUL){//e1*e2
        //���ж�e1������
        cout << "\tmovl\t";
        print_value(e1);//������ǳ�����ֵ
        cout<< ", %eax" <<endl;//��e1�������eax
        
        //�ж�e2������
        cout << "\timull\t";
        print_value(e2);
        cout << ", %eax" << endl;
        
        cout << "\tmovl\t%eax, t" << t->temp_var << endl;//ÿ����һ������eax����һ����ʱ����
    }

    else if(t->opType==OP_DIV){//a/b
        //e1,e2������Ĵ���
        cout << "\tmovl\t";
        print_value(e1);
        cout<< ", %eax" <<endl;

        cout << "\tmovl\t";
        print_value(e2);
        cout << ", %ecx" << endl;
        
        cout<<"\tcltd\n";//32λ��չΪ64λ
        cout<<"\tidivl\t%ecx\n";
        cout << "\tmovl\t%eax, t" << t->temp_var << endl;////���������̱�����eax
    }
    else if(t->opType==OP_MOD){
        //e1,e2������Ĵ���
        cout << "\tmovl\t";
        print_value(e1);
        cout<< ", %eax" <<endl;

        cout << "\tmovl\t";
        print_value(e2);
        cout << ", %ecx" << endl;
        
        cout<<"\tcltd\n";//32λ��չΪ64λ
        cout<<"\tidivl\t%ecx\n";
        cout << "\tmovl\t%edx, %eax\n";//������������������edx
        cout << "\tmovl\t%eax, t" << t->temp_var << endl;
    }

    //��Ŀ�����
    else if(t->opType==OP_SUB && t->child->sibling==nullptr){
        cout << "\tmovl\t";
        print_value(e1);
        cout << ", %eax" << endl;
        cout<<"\tnegl\t%eax\n";
        cout << "\tmovl\t%eax, t" << t->temp_var << endl;
    }
    else if(t->opType==OP_ADD && t->child->sibling==nullptr){
        cout << "\tmovl\t";
        print_value(e1);
        cout << ", %eax" << endl;
        cout << "\tmovl\t%eax, t" << t->temp_var << endl;
    }

    //����bool���ʽ��Ԫ��������Ҫcmpl�����������߱Ƚ�
    else if(t->nodeType==NODE_BOOL){
        //��e1,e2�浽eax,edx
        cout<<"\tmovl\t";
        print_value(e1);
        cout<<", %eax\n";
        
        cout<<"\tmovl\t";
        print_value(e2);
        cout<<", %edx\n";
        //�����и���ӣ�cmpl a,b����jle����b<=a!
        cout<<"\tcmpl\t"<<"%edx, %eax\n";
    }
}
//bool���ʽ��ȷʱ����ת���
void Tree::jump_true(TreeNode *judge){
    if(judge->nodeType==NODE_EXPR){
        cout<<"\ttestl\t%eax, %eax\n";//testl �ǲ���eax�Ƿ�Ϊ0��
    }
    //���judge��bool_expr����cmpl��䣬����expr_gen_code���涨��
    else if(judge->nodeType==NODE_BOOL){
        switch(judge->opType){
            case OP_EQUAL:{cout<<"\tje\t";break;}
            case OP_LARGER:{cout<<"\tjg\t";break;}//j�������������ͬ
            case OP_LARGER_EQUAL:{cout<<"\tjge\t";break;}
            case OP_SMALLER:{cout<<"\tjl\t";break;}
            case OP_SMALLER_EQUAL:{cout<<"\tjle\t";break;}
            case OP_NOT_EQUAL:{cout<<"\tjne\t";break;}
        }
    }
}
//bool���ʽ����ʱ����ת���
void Tree::jump_false(TreeNode *judge){
    if(judge->nodeType==NODE_EXPR){
        cout<<"\ttestl\t%eax, %eax\n";
        //if��while����ת��䲻ͬ����Ϊ��ǩ��ͬ��for��while��ͬ
    }
    else if(judge->nodeType==NODE_BOOL){
        switch(judge->opType){
            case OP_EQUAL:{cout<<"\tjne\t";break;}
            case OP_LARGER:{cout<<"\tjle\t";break;}
            case OP_LARGER_EQUAL:{cout<<"\tjl\t";break;}
            case OP_SMALLER:{cout<<"\tjge\t";break;}
            case OP_SMALLER_EQUAL:{cout<<"\tjg\t";break;}
            case OP_NOT_EQUAL:{cout<<"\tje\t";break;}
        }
    }
}
//С���ߣ���ӡһ���ڵ��ֵ
void Tree::print_value(TreeNode *e1){
    if (e1->nodeType == NODE_VAR){//����
        cout << e1->var_name;
    }
    else if(e1->nodeType==NODE_EXPR){
        cout<<"t"<<e1->temp_var;
    }
    else if(e1->nodeType==NODE_CONST){
        cout<<"$";
        if(e1->varType==VAR_INTEGER)cout << e1->int_val;
        else if(e1->varType==VAR_CHAR)cout << (int)e1->char_val;
    }
}
