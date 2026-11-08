/* ============================================================
 * main.c —— 通讯录 V3 菜单【默写练习版】
 *
 * 本文件基本给全了：菜单、输入校验、switch 分发。
 * 你只有 1 个 TODO（退出分支的三件事），其余读懂即可。
 * ============================================================ */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "contact.h"

/* ---------------- 菜单 ---------------- */
static void showMenu(void)
{
    printf("===== 通讯录 V3（链表版） =====\n");
    printf("1. 添加联系人\n");
    printf("2. 显示全部\n");
    printf("3. 查找联系人\n");
    printf("4. 删除联系人\n");
    printf("0. 保存并退出\n");
    /* count 在 contact.c 里是 static，本文件看不见，只能问 get_count() */
    printf("当前人数: %d\n", get_count());
    printf("请选择: ");
}

/* ---------------- 主函数 ---------------- */
int main(void)
{
    load();                          /* 启动第一件事：把存档读进链表 */

    int choice;
    while (1) {
        showMenu();
        if (scanf_s("%d", &choice) != 1) {   /* 输入非数字时清掉坏输入 */
            char buf[100];
            scanf_s("%99s", buf, 100);
            printf("输入无效，请重新选择。\n");
            continue;
        }
        switch (choice) {
        case 1: add();  break;
        case 2: show(); break;
        case 3: find(); break;
        case 4: del();  break;
        case 0:
            /* --------------------------------------------------
             * TODO 9：退出分支要做三件事，顺序不能反
             *
             *   ① 先把数据存进文件（调用哪个函数？）
             *   ② 再释放整条链表（调用哪个函数？）
             *   ③ return 0
             *
             * 想一想：如果第 ① ② 步写反了，先释放再保存，
             *        程序会发生什么？（提示：save 里要遍历 head……）
             * -------------------------------------------------- */
            save();
            free_all();
            printf("再见！\n");
            return 0;
        default:
            printf("没有这个功能，请重新选择。\n");
            break;
        }
    }
}
