/* ============================================================
 * contact.c —— 通讯录 V3 链表实现【默写练习版】
 *
 * 你的任务：补全下面 9 个函数。
 *
 * 【练习方法】
 *   补一个 → 按 Ctrl+F5 编译一次 → 通过再补下一个。
 *   千万别一口气写完 9 个再编译，那样一处笔误会挡住你看别的。
 *
 * 【进度自检】写完一个打个勾
 *   [ ] 1. tail_insert —— 所有添加动作的地基，先写它
 *   [ ] 2. add
 *   [ ] 3. show
 *   [ ] 4. find
 *   [ ] 5. del        —— 最难，卡住回看详解第 9 节
 *   [ ] 6. save
 *   [ ] 7. load       —— 卡住回看详解第 12 节
 *   [ ] 8. free_all   —— 卡住回看详解第 11 节
 *   [ ] 9. （可选）把 get_count 也自己重写一遍
 *
 * 【提示】练习期间如果看到 C4100（参数未使用）、C4505（static 函数没人调用）
 *   这类警告，是正常的——说明那里还没写，补全后自然消失。
 * ============================================================ */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"     /* 只包含 .h，绝不包含 contact.c */

/* ---------------- 链表的两个家当（已给） ----------------
 * head == NULL 表示空表；最后一个节点的 next 也必须是 NULL。
 * 为什么加 static？—— 让 main.c 看不到、改不了它们，
 * main.c 只能通过下面的函数间接操作链表。
 * -------------------------------------------------------- */
static Node *head = NULL;
static int count = 0;    /* 当前人数；只在"添加成功"的地方 +1 */

/* 存档文件名（已给）。相对路径 = 相对"当前工作目录"：
   VS 里 F5/Ctrl+F5 运行时，文件生成在项目文件夹（.vcxproj 旁边）。 */
static const char *FILENAME = "contacts.txt";

/* ============================================================
 * TODO 1：tail_insert —— 尾插（static，只给本文件内部用）
 *
 * 为什么要单独抽成一个函数？
 *   add()（用户输入）和 load()（文件读入）都要"接到链尾"，
 *   抽出来的话 count++ 只写一处，不会出现"加载 3 行却数成 6"。
 *
 * 三步，顺序不能乱：
 *   ① 新节点的 next 立刻置 NULL —— 它是最后一个，不留随机值
 *   ② 分两种情况接上：
 *        空表（head == NULL）→ 谁变成新的 head？
 *        非空表 → 先用一个临时指针走到尾节点
 *                 （循环条件是 next != NULL 还是 != NULL 的 next？想清楚）
 *                 再让【旧尾节点的 next】指向新节点
 *   ③ count++
 *
 * 自检：连续添加 3 人，显示顺序应与输入顺序一致，并且 count == 3。
 * ============================================================ */
static void tail_insert(Node *p)
{
    p->next = NULL;
    /* TODO 1：三步走 */
    if (head == NULL) {
         head=p;
    }
    else {
        Node* tail = head;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = p;
    }
    count++;
}

/* ============================================================
 * TODO 2：add —— 读输入 → 申请节点 → 装数据 → 交给 tail_insert
 *
 *   ① 先把输入读到【局部变量】里：
 *        char name[NAME_LEN];  int age;  char phone[PHONE_LEN];
 *      为什么不直接读进节点？—— 因为下一步 malloc 可能失败，
 *      先读进局部变量，失败了用户的输入不会白丢。
 *
 *   ② scanf_s 写法与 V2 完全一致：
 *        scanf_s("%19s %d %11s", name, NAME_LEN, &age, phone, PHONE_LEN)
 *      字符串要带【字段宽度】和【数组大小】，%d 只传 &age。
 *      返回值 != 3 时：清掉这行坏输入，提示"输入无效"，return。
 *
 *   ③ malloc(sizeof *p) 申请节点，【立刻检查 p == NULL】。
 *
 *   ④ 装数据：strcpy 拷 name / phone，age 直接赋值。
 *      （p->next 不用管，tail_insert 里会初始化）
 *
 *   ⑤ 调用 tail_insert(p)，打印"添加成功"。
 *
 *   注意：链表没有容量上限，V2 里的 count >= MAX 检查【不要】照抄。
 * ============================================================ */
void add(void)
{
    /* TODO 2 */
    char name[NAME_LEN];
    int age;
    char phone[PHONE_LEN];

    printf("请输入 姓名 年龄 电话\n");
    if (scanf_s("%19s %d %11s", name, NAME_LEN, &age, phone, PHONE_LEN) != 3) {
        char buf[100];
        scanf_s("%99s", buf, 100);
        printf("输入无效\n");
        return;
    }
    Node* p = malloc(sizeof * p);
    if (p == NULL) {
        printf("内存不足，申请失败\n");
        return;
    }
    strcpy(p->name, name);
    p->age = age;
    strcpy(p->phone, phone);

    tail_insert(p);
    printf("添加成功\n");
}

/* ============================================================
 * TODO 3：show —— 空表判断 + 从头遍历
 *
 *   ① head == NULL → 打印"通讯录为空。"并 return（不能进入遍历）
 *   ② for 循环遍历，三个格子：
 *        初始化：p 从哪开始？
 *        条件：p != NULL 还是 p->next != NULL？—— 想想区别
 *        步进：怎么走到下一个节点？
 *   ③ 遍历指针加 const（只读遍历，不允许通过它改节点）
 *   ④ 序号用局部变量 i 从 1 开始自己加（链表没有下标）
 *   ⑤ 输出格式照 V2：
 *        printf("%-4d  %-18s  %-4d  %s\n", i, p->name, p->age, p->phone);
 * ============================================================ */
void show(void)
{
    /* TODO 3 */
    if (head == NULL) {
        printf("通讯录为空\n");
        return;
    }
    printf("序号  姓名                年龄  电话\n");
    int i = 0;
    for (const Node* p = head;p!= NULL;p = p->next) {
        i++;
        printf("%-4d %-18s %-4d %s\n", i, p->name, p->age, p->phone);
    }
}

/* ============================================================
 * TODO 4：find —— 按姓名巡逻
 *
 *   ① 读入要查的姓名：char key[NAME_LEN]; scanf_s 带宽度和大小
 *   ② 遍历，用 strcmp(p->name, key) == 0 判断（不能用 == 比较字符串）
 *   ③ 找到：打印 姓名/年龄/电话，然后【立即 return】
 *   ④ 循环自然跑完 = 全表都没有 → 打印"没有找到 %s。"
 * ============================================================ */
void find(void)
{
    /* TODO 4 */
    char key[NAME_LEN];
    scanf_s("%19s", key, NAME_LEN);
    for (const Node* p = head;p!= NULL;p = p->next) {
        if (strcmp(p->name, key) == 0) {
            printf("找到%s %d %s\n", p->name, p->age, p->phone);
            return;
        }
            
    }
        printf("没有找到%s\n", key);
}

/* ============================================================
 * TODO 5：del —— 双指针删除（本节最容易写错，慢慢想）
 *
 * 开场：
 *    Node *prev = NULL;   ← 为什么是 NULL？因为 cur 一开始在第一个节点，
 *                            它前面没有任何节点
 *    Node *cur  = head;
 *
 * 循环里三件事，顺序是铁律：
 *   ① 先比较（strcmp == 0）
 *   ② 匹配后接链，分两种情况：
 *        prev == NULL → 删的是【头节点】：该改谁？（改 head，绝不能碰 prev->next）
 *        否则         → 删中间/尾巴：让 prev 的 next 跳过 cur
 *      接好链之后才 free(cur)，再 count--，最后 return
 *   ③ 不匹配才移动，而且必须是：
 *        prev = cur;
 *        cur  = cur->next;
 *      —— 这两行顺序反了会丢失前驱，想想为什么？
 *
 * 循环结束仍未找到：打印"没有找到 %s，无法删除。"，链表和 count 保持不变
 * ============================================================ */
void del(void)
{
    /* TODO 5 */
    char key[NAME_LEN];
    printf("请输入要删除的名字\n");
    scanf_s("%19s", key, NAME_LEN);

    Node* prev = NULL;
    Node* cur = head;
    while (cur != NULL) {
        if(strcmp(cur->name,key)==0){
            if (prev == NULL)
                head = head->next;
            else
                prev->next = cur->next;
            free(cur);
            count--;
            printf("已删除%s\n", key);
            return;
        }
        prev = cur;
        cur = cur->next;
      }
    printf("没有找到%s\n", key);
    }

/* ============================================================
 * TODO 6：save —— 遍历写文件
 *
 *   V2 的 fprintf 一字不改，只把"用下标遍历"换成"沿 next 遍历"。
 *
 *   ① fopen(FILENAME, "w")，"w" 表示不存在则创建、存在则清空重写
 *   ② 打开失败（NULL）→ 提示并 return -1
 *   ③ for 遍历（const），fprintf(fp, "%s %d %s\n", p->name, p->age, p->phone)
 *      空表时循环一次都不执行 → 得到空文件，这是合法的，不是错误
 *   ④ fclose
 *   ⑤ 打印"已保存 %d 条记录到 %s。"，return 0
 * ============================================================ */
int save(void)
{
    FILE* fp = fopen(FILENAME, "w");
    if (fp == NULL) {
        printf("保存失败，无法打开文件\n");
        return -1;   /* TODO 6：这里先占位，保证文件能编译 */
    }
    for (const Node* p = head;p != NULL;p = p->next) {
        fprintf(fp, "%s %d %s\n", p->name, p->age, p->phone);
    }
    fclose(fp);
    printf("已保存%d条记录到%s\n", count, FILENAME);
    return 0;
 }
/* ============================================================
 * TODO 7：load —— 打开文件 → 逐行读 → 校验 → 建节点 → 尾插
 *
 *   ① 第一件事：free_all();
 *      —— 先清空旧表，保证重复加载不会攒出两份联系人
 *         （启动时旧表本来就是空的，调用它没有副作用）
 *   ② fopen(FILENAME, "r")，NULL → 打印"没有找到存档"，return（从空表开始）
 *   ③ char line[128]; int lineno = 0;
 *      while (fgets(line, sizeof line, fp) != NULL) { lineno++; ... }
 *      —— fgets 一次只搬一行，坏行在本行内就能识破
 *   ④ 用 sscanf(line, "%19s %d %11s", name, &age, phone) 拆包：
 *        返回 3 → 三个字段全部成功：
 *                  malloc(sizeof *p) → 查 NULL（失败就提示并 break）
 *                  → 装数据 → tail_insert(p)
 *                  ? count++ 已经在 tail_insert 里，这里【绝不再加】
 *        返回 != 3 → 坏行/半条记录：
 *                  打印"第 %d 行损坏，已停止加载（已保留前 %d 条）。" → break
 *   ⑤ fclose(fp)，打印"已加载 %d 条记录。"
 *
 *   自检：添加 3 条并保存 → 退出重开 → 3 条全部恢复、顺序不变、count == 3
 * ============================================================ */
void load(void)
{    /* TODO 7 */
    free_all();
    FILE* fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("没有找到存档%s，从空通讯录开始\n", FILENAME);
        return;
    }
    char line[128];
    int lineno = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        lineno++;
        char name[NAME_LEN];
        int age;
        char phone[PHONE_LEN];
        if (sscanf(line, "%19s %d %11s", name, &age, phone)==3) {
            Node* p = malloc(sizeof * p);
            if (p == NULL) {
                printf("内存不足，停止加载(已保留%d条记录)\n", count);
                break;
            }
            strcpy(p->name, name);
            p->age = age;
            strcpy(p->phone, phone);
            tail_insert(p);
        }
        else {
            printf("第%d条损坏，已停止加载（已保留%d条记录）\n", lineno, count);
            break;
        }     
    }
        fclose(fp);
        printf("已加载%d条记录\n", count); 
}

/* ============================================================
 * TODO 8：free_all —— 退出前逐个拆节点
 *
 *   循环条件：head != NULL
 *   每一轮三行，顺序是铁律：
 *     ① 先把 head->next 存进一个局部指针变量
 *        （为什么必须最先做？因为下一节 free 之后，那个 next 就再也读不到了）
 *     ② free(head)
 *     ③ head = 刚才存下的那个指针
 *   循环结束后：count = 0
 *
 *   自检：函数结束时 head == NULL、count == 0，没有节点残留。
 * ============================================================ */
void free_all(void)
{
    /* TODO 8 */
    while (head != NULL) {
        Node* next = head->next;
        free(head);
        head = next;
    }
    count = 0;
}
/* ============================================================
 * get_count（已给）：main.c 的只读窗口
 * count 是 static，main.c 看不见它，只能通过这个函数问人数。
 * 有余力可以自己删掉重写：就一条 return count;
 * ============================================================ */
int get_count(void)
{
    return count;
}
