#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

/*全局结构层*/

//全局储存文件名称
char SAVE_FILE_NAME[20] = "Test.save";

//定义书籍信息为结构体
//采用Key-Valve方式确定结构体储存方式
typedef struct LNode {
    double price;
    char date[9];
    char ISBN[14];//ISBN为节点唯一标识即Key
    char author[20];
    char press[20];
    char name[30];
    struct LNode *next;
} LNode, *LinkList;

//全局临时变量定义(用于计数或转移)
char cTemp;
char sTemp[50];
char saveTemp[20];
int iTemp;

//全局结构体链表头指针
LinkList HEAD;

/*文件IO层*/
//文件读入
void FileIn(char *savePath) {
    FILE *fp;
    LNode *p1, *p2;
    if ((fp = fopen(savePath, "rb")) == NULL)
        printf("##########################  存档文件不存在!  ##########################\n");
    HEAD = p1 = p2 = (LinkList) malloc(sizeof(LNode));
    fread(p1, sizeof(LNode), 1, fp);
    while (!feof(fp)) {
        p1 = p2;
        p2 = (LinkList) malloc(sizeof(LNode));
        p1->next = p2;
        fread(p2, sizeof(LNode), 1, fp);
    }
    if (p1 == p2)
        HEAD = NULL;
    else
        p1->next = NULL;
    free(p2);
    fclose(fp);
    printf("##########################  图书信息读取成功  ##########################\n");
}

//文件输出
void FileOut() {
    FILE *fp;
    LNode *p1, *p2;
    fp = fopen(SAVE_FILE_NAME, "wb");
    if (HEAD != NULL) {
        p2 = HEAD;
        while (p2 != NULL) {
            p1 = p2;
            fwrite(p1, sizeof(LNode), 1, fp);
            p2 = p1->next;
        }
    }
    fclose(fp);
    printf("##########################  图书信息存储成功  ##########################\n");
}

//检测存档是否为空
int EmptyFile(char *savePath) {
    FILE *fp;
    fp = fopen(savePath, "r+");
    if(fp== NULL) return 0;
    cTemp = fgetc(fp);
    fclose(fp);
    if (cTemp == EOF) return 0;
    return 1;
}



/*针对链表操作层*/
//初始化全局头指针
void CreateLink_L() {
    HEAD = (LinkList) malloc(sizeof(LNode));
    HEAD->next = NULL;
}

/*   (核心)ISBN遍历器
 * 1.链表为空则返回全局头节点HEAD
 * 2.链表不为空且无ISBN码不相等则返回尾节点
 * 3.链表不为空且无ISBN码相等则返回相等节点
 */
LNode *LoginBook(char *ISBN) {
    LNode *i;
    if (HEAD->next == NULL)return HEAD;
    i = HEAD->next;
    while (i->next != NULL && strcmp(i->ISBN, ISBN) != 0) {
        i = i->next;
    }
    return i;
}

//链表操作之插入节点
void InsertBook(LNode *i, char *date, double price, char *ISBN, char *name, char *author, char *press) {
    LinkList p = i;
    while (p->next != NULL) {
        p = p->next;
    }
    auto s = (LinkList) malloc(sizeof(LNode));
    s->price = price;
    strcpy(s->date, date);
    strcpy(s->author, author);
    strcpy(s->ISBN, ISBN);
    strcpy(s->name, name);
    strcpy(s->press, press);
    s->next = p->next;
    p->next = s;
    FileOut();//储存
}

//重置链表
void DeleteAll() {
    HEAD->next = NULL;
    FileOut();
}

//链表操作之删除特定节点
void DeleteBook(LNode *i) {
    LinkList p = HEAD, q = NULL;
    //遍历至i的前驱节点
    while (p->next != i) {
        p = p->next;
    }
    q = p->next;
    p->next = q->next;
    free(q);
    FileOut();//储存
}

//链表操作之节点的替换
void UpdateBook(LNode *i, char *date, double price, char *ISBN, char *name, char *author, char *press) {
    LinkList p = HEAD, q = NULL;
    auto s = (LinkList) malloc(sizeof(LNode));
    s->price = price;
    strcpy(s->date, date);
    strcpy(s->author, author);
    strcpy(s->ISBN, ISBN);
    strcpy(s->name, name);
    strcpy(s->press, press);
    while (p->next != i) {
        p = p->next;
    }
    q = p->next;
    p->next = s;
    s->next = q->next;
    free(q);
    FileOut();//储存
}


/* UI中间层
 * UI中间层作为用户交互界面与链表操作层的中间组件
 * 其用途为检验数据的合法性及获取或输出交互内容
 * 主要结构为 Main -> UI(用户功能请求) -> UI中间层(判断数据) -> 链表操作(内存数据CRUD) ->文件IO(内存数据IO)
 */


//书籍添加中间层
void Add(char *ISBN) {
    LNode *i = LoginBook(ISBN);
    if (i == HEAD || strcmp(i->ISBN, ISBN) != 0) {
        double price;
        char date[9], name[30], author[20], press[20];
        printf("################## 您添加的书籍 ISBN 码为 %s ###################\n", ISBN);
        printf("*                      请输入该书籍 >>名称<<                             *\n");
        scanf("%s", name);
        printf("*                      请输入该书籍 >>作者<<                             *\n");
        scanf("%s", author);
        printf("*                      请输入该书籍 >>出版社<<                            *\n");
        scanf("%s", press);
        printf("*                      请输入该书籍 >>出版日期<<                          *\n");
        printf("*                      出版日期 格式 如20180101                          *\n");
        scanf("%s", date);
        printf("*                      请输入该书籍 >>价格<<                             *\n");
        printf("*                       书籍价格 格式 如 120                             *\n");
        scanf("%lf", &price);
        InsertBook(i, date, price, ISBN, name, author, press);
        printf("########################   书籍 %s 添加成功   #######################\n",name);
    } else {
        printf("#################### 您添加的书籍已存在 请勿重复添加书籍 ###################\n");
    }
}

//书籍删除中间层
void Delete(char *ISBN) {
    LNode *i = LoginBook(ISBN);
    if (strcmp(i->ISBN, ISBN) == 0){
        DeleteBook(i);
        printf("########################   书籍 %s 删除成功   #######################\n",i->name);
    }
    else {
        printf("*              您要删除的书籍不存在 请重新输入 ISBN 码              *\n");
    }
}

//书籍修改中间层
void Update(char *ISBN) {
    LNode *i = LoginBook(ISBN);
    if (strcmp(i->ISBN, ISBN) == 0) {
        double price;
        char date[9], name[30], author[20], press[20];
        printf("################## 您所修改书籍的 ISBN 码为 %s ###################\n", ISBN);
        printf("*                      请输入修改后书籍 >>名称<<                             *\n");
        scanf("%s", name);
        printf("*                      请输入修改后书籍 >>作者<<                             *\n");
        scanf("%s", author);
        printf("*                      请输入修改后书籍 >>出版社<<                            *\n");
        scanf("%s", press);
        printf("*                      请输入修改后书籍 >>出版日期<<                        *\n");
        printf("######################  出版日期 格式 如20180101  ##########################\n");
        scanf("%s", date);
        printf("*                      请输入修改后书籍 >>价格<<                            *\n");
        printf("######################   书籍价格 格式 如 120  #############################\n");
        scanf("%lf", &price);
        UpdateBook(i, date, price, ISBN, name, author, press);
        printf("###################   ISBN : %s 修改成功   ##################\n",ISBN);
    }
}

//书籍查询中间层
void Login(char *ISBN) {
    LNode *i;
    i = LoginBook(ISBN);
    if (strcmp(i->ISBN, ISBN) != 0) {
        printf("#################### 您查找的书籍不存在 请核对后重新输入 ###################\n\n");
    } else {
        printf("########################## 您查找的书籍信息如下 ##########################\n");
        printf(" >>ISBN码   : %s                                                    \n", i->ISBN);
        printf(" >>书名     : %s                                                     \n", i->name);
        printf(" >>作者     : %s                                                     \n", i->author);
        printf(" >>出版社    : %s                                                     \n", i->press);
        printf(" >>出版日期  : %s                                                     \n", i->date);
        printf(" >>销售价格  : %.2lf                                                     \n", i->price);
        printf("##########################   书籍信息输出完毕   ##########################\n");
    }
}

//书籍ISBN码输入中间层
char *ScanfISBN() {
    printf("*                 请输入您的 ISBN 码（忽略ISBN码的连字符）                 *\n");
    scanf("%s", sTemp);
    if (strlen(sTemp) != 13) {
        printf("#################### 请输入正确的 13 位ISBN码！ #####################\n");
        ScanfISBN();
    }
    return sTemp;
}

//存档储存中间层
void Save() {
    FileOut();
}

//存档读取中间层
void Read() {
    printf("*              输入 1 去载入默认存档  输入 2 去载入自定义存档                *\n");
    scanf(" %c", &cTemp);
    switch (cTemp) {
        case '1':
            FileIn(SAVE_FILE_NAME);
            break;
        case '2':
            printf("*                         输入存档完整名称(带后缀名)                      *\n");
            scanf("%s",saveTemp);
            if(EmptyFile(saveTemp)==1) {
                strcpy(SAVE_FILE_NAME,saveTemp);
                FileIn(SAVE_FILE_NAME);
            }
            else printf("##########################  存档文件不存在!  ##########################\n");
            break;
        default:
            printf("######################  操作识别失败 请重新键入操作 #######################\n");
            break;
    }
}

//存档遍历输出中间层
void List() {
    if (HEAD->next == NULL) {
        printf("########################## 存档中没有储存图书 ##########################\n\n");
    } else {
        LinkList l = HEAD->next;
        printf("##########################  所有图书信息如下  ##########################\n");
        printf("     ISBN     |      名称     |   出版社  |   作者   | 出版日期  | 价格\n");
        while (l->next != NULL) {
            printf("%s | 《%s》 | %s | %s | %s | %.2lf\n", l->ISBN, l->name, l->press, l->author, l->date, l->price);
            l = l->next;
        }
        printf("%s | 《%s》 | %s | %s | %s | %.2lf\n", l->ISBN, l->name, l->press, l->author, l->date, l->price);
        printf("#######################  所有图书的信息已显示结束 ########################\n");
    }
}



/*高级操作层*/
//书名查询输出
void NameFinder(char *str) {
    LNode *i;
    iTemp = 0;
    if (HEAD->next == NULL) {
        printf("########################## 存档中没有储存图书 ##########################\n\n");
    }
    i = HEAD;
    printf("############################# 搜索到的信息如下 ###########################\n");
    while (i->next != NULL) {
        i = i->next;
        if (strstr(i->name, str)) {
            iTemp++;
            printf(" >>书名 : %s                                                    \n", i->name);
            printf(" >>ISBN码 : %s                                                    \n", i->ISBN);
            printf(" >>作者   : %s                                                     \n", i->author);
            printf(" >>出版社   : %s                                                     \n", i->press);
            printf(" >>出版日期 : %s                                                     \n", i->date);
            printf(" >>销售价格 : %.2lf                                                     \n", i->price);
        }
    }
    if (!iTemp) {
        printf("######################### 没有找到%s相关的的信息 ######################\n", str);
    }
    printf("##########################   书籍信息输出完毕   #########################\n\n");
}

//出版社含有书籍
void PressFinder(char *str) {
    LNode *i;
    iTemp = 0;
    if (HEAD->next == NULL) {
        printf("########################## 存档中没有储存图书 ##########################\n\n");
    }
    i = HEAD;
    printf("######################## %s的全部书籍如下 ########################\n", str);
    while (i->next != NULL) {
        i = i->next;
        if (!strcmp(i->press, str)) {
            iTemp++;
            printf(" >>ISBN码 : %s                                                    \n", i->ISBN);
            printf(" >>书名   : %s                                                     \n", i->name);
            printf(" >>作者   : %s                                                     \n", i->author);
            printf(" >>出版日期 : %s                                                     \n", i->date);
            printf(" >>销售价格 : %.2lf                                                     \n", i->price);
        }
    }
    if (!iTemp) {
        printf("####################### 没有找到%s的书籍信息 #######################\n", str);
    }
    printf("##########################   书籍信息输出完毕   #########################\n\n");
}

//书名含有关键词统计
void NameStatistics(char *str) {
    iTemp=0;
    LNode *i;
    if (HEAD->next == NULL) {
        printf("########################## 存档中没有储存图书 ##########################\n\n");
    }
    i = HEAD;
    while (i->next != NULL) {
        i = i->next;
        if (strstr(i->name, str)) iTemp++;
    }
    printf(" >>包含关键词 \"%s\" 的书籍有 %d 本。\n", str ,iTemp);
}

//作者拥有书籍数量查询
void AuthorStatistics(char *str) {
    iTemp=0;
    LNode *i;
    if (HEAD->next == NULL) {
        printf("########################## 存档中没有储存图书 ##########################\n\n");
    }
    i = HEAD;
    while (i->next != NULL) {
        i = i->next;
        if (strcmp(i->author, str)==0) iTemp++;
    }
    printf(" >>作者 %s 的书籍有 %d 本。\n", str ,iTemp);
}

//出版社拥有书籍数量查询
void PressStatistics(char *str) {
    iTemp=0;
    LNode *i;
    if (HEAD->next == NULL) {
        printf("########################## 存档中没有储存图书 ##########################\n\n");
    }
    i = HEAD;
    while (i->next != NULL) {
        i = i->next;
        if (strcmp(i->press, str)==0) iTemp++;
    }
    printf(" >>出版社 %s 的书籍有 %d 本。\n",str,iTemp);
}

//初始化存档导入控制
void InitMain() {
    if (EmptyFile(const_cast<char *>(SAVE_FILE_NAME)) == 1) {
        printf("################ 检测到存档 %s 不为空 是否载入存档?(Y/N) #################\n", SAVE_FILE_NAME);
        scanf("%c", &cTemp);
        if (cTemp == 'Y') {
            FileIn(const_cast<char *>(SAVE_FILE_NAME));
        }
    }
}

/*UI显示层*/
//启动界面 接受操作值
char StartUI() {
    printf("########################## 欢迎来到图书管理系统 ##########################\n");
    printf(">>                                                                   <<\n");
    printf("*                        输入 1 或 A 去添加该图书                        *\n");
    printf("*                        输入 2 或 D 去删除该图书                        *\n");
    printf("*                        输入 3 或 R 去显示该图书                        *\n");
    printf("*                        输入 4 或 U 去修改该图书                        *\n");
    printf("*                        输入 5 或 V 手动存档操作                        *\n");
    printf("*                        输入 6 或 F 进入搜索功能                        *\n");
    printf("*                        输入 7 或 S 进入统计功能                        *\n");
    printf("*                        输入 0 或 E 退出管理系统                        *\n");
    printf(">>                                                                   <<\n");
    printf("######################  接受输入信息 按回车键结束输入 #####################\n");
    scanf(" %c", &cTemp);
    return cTemp;
}

//选择界面 中间层入口
int choose() {
    cTemp = StartUI();
    switch (cTemp) {
        case '1':
        case 'A':
            Add(ScanfISBN());
            break;
        case '2':
        case 'D':
            printf("*              输入 1 去输入 ISBN 码 输入 2 去删除所有书籍                 *\n");
            scanf(" %c", &cTemp);
            switch (cTemp) {
                case '1':
                    Delete(ScanfISBN());
                    break;
                case '2':
                    printf("#################### 存档删除后不可恢复请确认操作(Y/N) ####################\n");
                    scanf(" %c", &cTemp);
                    switch (cTemp){
                        case 'Y':
                            DeleteAll();
                            printf("##########################   全部书籍删除成功   #########################\n");
                        case 'N':
                            break;
                        default:
                            printf("######################  操作识别失败 请重新键入操作 #######################\n");
                    }
                    break;
                default:
                    printf("######################  操作识别失败 请重新键入操作 #######################\n");
                    break;
            }
            break;
        case '3':
        case 'R':
            printf("*              输入 1 去输入 ISBN 码 输入 2 去显示所有书籍                *\n");
            scanf(" %c", &cTemp);
            switch (cTemp) {
                case '1':
                    Login(ScanfISBN());
                    break;
                case '2':
                    List();
                    break;
                default:
                    printf("######################  操作识别失败 请重新键入操作 #######################\n");
                    break;
            }
            break;
        case '4':
        case 'U':
            Update(ScanfISBN());
            break;
        case '5':
        case 'V':
            printf("*                输入 1 去储存存档     输入 2 去载入存档                   *\n");
            scanf(" %c", &cTemp);
            switch (cTemp) {
                case '1':
                    Save();
                    break;
                case '2':
                    Read();
                    break;
                default:
                    printf("######################  操作识别失败 请重新键入操作 #######################\n");
                    break;
            }
            break;
        case '6':
        case 'F':
            printf("*             输入 1 去搜索书名 输入 2 去搜索出版社的所有图书               *\n");
            scanf(" %c", &cTemp);
            switch (cTemp) {
                case '1':
                    printf("*                      请输入该书籍 >>名称<<                            *\n");
                    scanf("%s", sTemp);
                    NameFinder(sTemp);
                    break;
                case '2':
                    printf("*                      请输入该书籍 >>出版社<<                             *\n");
                    scanf("%s", sTemp);
                    PressFinder(sTemp);
                    break;
                default:
                    printf("######################  操作识别失败 请重新键入操作 #######################\n");
                    break;
            }
            break;
        case '7':
        case 'S':
            printf("*                      输入 1 去统计作者拥有图书数量                      *\n");
            printf("*                      输入 2 去统计出版社有图书数量                      *\n");
            printf("*                      输入 3 去统计书名含关键词图书数量                   *\n");
            scanf(" %c", &cTemp);
            switch (cTemp) {
                case '1':
                    printf("*                      请输入该作者 >>名称<<                            *\n");
                    scanf("%s", sTemp);
                    AuthorStatistics(sTemp);
                    break;
                case '2':
                    printf("*                      请输入该出版社 >>名称<<                          *\n");
                    scanf("%s", sTemp);
                    PressStatistics(sTemp);
                    break;
                case '3':
                    printf("*                      请输入该关键词 >>字符串<<                        *\n");
                    scanf("%s", sTemp);
                    NameStatistics(sTemp);
                    break;
                default:
                    printf("######################  操作识别失败 请重新键入操作 #######################\n");
                    break;
            }
            break;
        case '0':
        case 'E':
            printf("\n************************  欢迎您再次使用本管理系统 *************************\n\n");
            return 0;
        default:
            printf("\n***********************  请区分大小写 重新键入操作 ************************\n\n");
            break;
    }
    return 1;
}

//主函数 处理分流操作函数及简单的界面显示
int main() {
    int iChoose;
    CreateLink_L();
    InitMain();
    iChoose = 1;
    while (iChoose == 1) {
        iChoose = choose();
    }
}