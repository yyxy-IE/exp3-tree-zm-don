/**
 * 实验：目录树查看器（仿 Linux tree 命令）
 * 学号：2504020112 姓名：张素菲
 * 说明：请补全所有标记为 TODO 的函数体，不要修改其他代码。
 * 目录树查看器（仿 Linux tree 命令）
 * 完整实现版本（C语言，左孩子右兄弟二叉树）
 * 编译：gcc -o tree tree.c -std=c99
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// ================== 二叉树结点定义 ==================
typedef struct FileNode {
    char *name;                  // 文件/目录名
    int isDir;                   // 1:目录 0:文件
    struct FileNode *firstChild; // 左孩子：第一个子项
    struct FileNode *nextSibling;// 右兄弟：下一个同层项
} FileNode;

// ================== 函数声明 ==================
FileNode* createNode(const char *name, int isDir); 
int cmpNode(const void *a, const void *b);
FileNode* buildTree(const char *path);
void printTree(FileNode *node, const char *prefix, int isLast);
int countNodes(FileNode *root);
int countLeaves(FileNode *root);
int treeHeight(FileNode *root);
void countDirFile(FileNode *root, int *dirs, int *files);
void freeTree(FileNode *root);
char* getBaseName(void);

// ================== 需要补全的函数 ==================

// 创建新结点（分配内存、复制字符串、初始化指针）
FileNode* createNode(const char *name, int isDir) {
    FileNode *node = (FileNode*)malloc(sizeof(FileNode)); // 分配内存
    node->name = strdup(name); // 复制字符串
    node->isDir = isDir; // 设置类型
    node->firstChild = NULL; // 初始化孩子指针
    node->nextSibling = NULL; // 初始化兄弟指针
    return node; // 返回新结点
}
     // TODO: 实现

// 比较函数，用于 qsort 对子项按名称排序
int cmpNode(const void *a, const void *b) {
    FileNode *na = *(FileNode**)a; 
    FileNode *nb = *(FileNode**)b;
    return strcmp(na->name, nb->name);
}
    // TODO: 实现

// 递归构建目录树（核心难点）
FileNode* buildTree(const char *path) {
    // 1. opendir 打开目录，失败返回 NULL
    DIR *dir = opendir(path);  
    if (!dir) {
        perror("opendir");
        return NULL;
    }
    // 2. 从 path 中提取最后的目录名作为当前结点名（注意处理根目录"/"）
    const char *base = strrchr(path, '/'); // 查找最后一个 '/'
    base = base ? base + 1 : path; // 如果找到，指向下一个字符；否则指向整个路径
    // 3. 创建当前目录结点
    FileNode *curDir = createNode(base, 1); // 创建目录结点
    // 4. 循环 readdir，跳过 "." 和 ".."
    struct dirent *entry;
    FileNode **children = NULL; // 临时数组存储子结点指针
    int childCount = 0; // 子结点计数 
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue; // 跳过 "." 和 ".."
        // 5. 拼接完整路径，用 stat 判断类型
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name); // 拼接路径
        struct stat st;
        if (stat(fullPath, &st) != 0) continue; // stat 失败跳过
        // 6. 若是目录，递归调用 buildTree；若是普通文件，调用 createNode
        FileNode *child = NULL;
        if (S_ISDIR(st.st_mode)) {
            child = buildTree(fullPath); // 递归构建子目录树
        } else if (S_ISREG(st.st_mode)) {
            child = createNode(entry->d_name, 0); // 创建文件结点
        }
        if (child) {
            children = (FileNode**)realloc(children, (childCount+1) * sizeof(FileNode*)); // 扩展数组
            children[childCount++] = child; // 添加子结点指针到数组
        }
    }
    closedir(dir); // 8. 关闭目录
    // 9. 对子结点数组排序（调用 qsort 和 cmpNode）
    qsort(children, childCount, sizeof(FileNode*), cmpNode); // 排序
    // 10. 将排序后的子结点链接成兄弟链表（firstChild 指向第一个，后续 nextSibling）
    if (childCount > 0) {
        curDir->firstChild = children[0]; // 第一个子结点
        for (int i = 0; i < childCount-1; i++) {
            children[i]->nextSibling = children[i+1]; // 链接兄弟结点
        }
    }
    free(children); // 11. 释放临时数组
    return curDir; // 返回当前目录结点
    // TODO: 实现
    // 步骤提示：
}

// 树形输出（仿 tree 命令）
void printTree(FileNode *node, const char *prefix, int isLast) {
    // TODO: 实现
    // 步骤提示：
    // 1. 如果 node 为空，返回
    if (!node) return;
    // 2. 输出前缀、分支符号（isLast ? "`-- " : "|-- "）、结点名
    printf("%s", prefix); // 输出前缀
    printf("%s", isLast ? "`-- " : "|-- "); // 输出分支符号
    printf("%s", node->name); // 输出结点名 
    // 3. 如果是目录，输出 "/"
    if (node->isDir) printf("/"); // 目录后加 '/'   
        printf("\n"); // 换行
    // 5. 如果没有孩子，返回
    if (!node->firstChild) return;
    // 6. 遍历孩子链表，对每个孩子：
    FileNode *child = node->firstChild;
    //     计算孩子总数
    int childCount = 0;
    FileNode *tmp = child;
    while (tmp) { childCount++; tmp = tmp->nextSibling; }
    int idx = 0;
    while (child) {
        //     判断是否为最后一个孩子
        int isLastChild = (++idx == childCount);
         //     计算新前缀 = prefix + (isLast ? "    " : "|   ")
        char newPrefix[1024];
        snprintf(newPrefix, sizeof(newPrefix), "%s%s", prefix, isLast ? "    " : "|   ");
        //     递归调用 printTree
        printTree(child, newPrefix, isLastChild);
        child = child->nextSibling; // 处理下一个孩子
    }
}

// 统计二叉树结点总数
int countNodes(FileNode *root) {
    if (!root) return 0; // 空树结点数为0
    // 结点总数 = 1（当前结点） + 左子树结点数 + 右子树结点数
    return 1 + countNodes(root->firstChild) + countNodes(root->nextSibling);
    // TODO: 实现（递归）
}

// 统计叶子结点数（firstChild == NULL 的结点）
int countLeaves(FileNode *root) {
    // TODO: 实现（递归）
    if (!root) return 0; // 空树叶子数为0
    int leaf = (root->firstChild == NULL) ? 1 : 0; // 当前结点是否为叶子
    return leaf + countLeaves(root->firstChild) + countLeaves(root->nextSibling); // 叶子数 = 当前结点是否为叶
}

// 计算二叉树高度（根深度为1，空树高度为0）
int treeHeight(FileNode *root) {
    // TODO: 实现（递归）
    if (!root) return 0; // 空树高度为0
    int childH = treeHeight(root->firstChild); // 左子树高度
    int siblingH = treeHeight(root->nextSibling); // 右子树高度
    int h = childH + 1; // 当前结点高度 = 左子树高度 + 1
    return (h > siblingH) ? h : siblingH; //
}

// 统计目录数和文件数（遍历整棵树）
void countDirFile(FileNode *root, int *dirs, int *files) {
    // TODO: 实现（递归）
    if (!root) return; // 空树不统计
    if (root->isDir) (*dirs)++; // 目录数加1
    else (*files)++; // 文件数加1
    countDirFile(root->firstChild, dirs, files); // 统计左子树  
}

// 释放整棵树的内存
void freeTree(FileNode *root) {
    // TODO: 实现（递归释放左右子树，最后释放当前结点）
    if (!root) return; // 空树不释放
    freeTree(root->firstChild); // 释放左子树
    freeTree(root->nextSibling); // 释放右子树
    free(root->name); // 释放结点名字符串
    free(root); // 释放结点结构体
}

// 获取当前工作目录的“基本名称”（用于显示根结点名）
char* getBaseName(void) {
    char *cwd = getcwd(NULL, 0); // 获取当前工作目录绝对路径
    if (!cwd) return strdup("."); // 获取失败返回当前目录名 "."
    char *base = strrchr(cwd, '/'); // 查找最后一个 '/'
    char *res = base ? strdup(base+1) : strdup(cwd); // 提取最后一个 '/' 之后的部分作为基本名称
    free(cwd); // 释放 getcwd 分配的内存
    return res; // 返回基本名称
    // TODO: 实现
    // 提示：调用 getcwd(NULL,0) 获取绝对路径，提取最后一个 '/' 之后的部分
    // 注意释放 getcwd 分配的内存
}

int main(int argc, char *argv[]) {
    char targetPath[1024];
    if (argc >= 2) {
        strncpy(targetPath, argv[1], sizeof(targetPath)-1);
        targetPath[sizeof(targetPath)-1] = '\0';
    } else {
        if (getcwd(targetPath, sizeof(targetPath)) == NULL) {
            perror("getcwd");
            return 1;
        }
    }

    int len = strlen(targetPath);
    if (len > 0 && targetPath[len-1] == '/')
        targetPath[len-1] = '\0';

    struct stat st;
    if (stat(targetPath, &st) != 0) {
        perror("stat");
        return 1;
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "错误: %s 不是目录\n", targetPath);
        return 1;
    }

    FileNode *root = buildTree(targetPath);
    if (!root) {
        fprintf(stderr, "无法构建目录树\n");
        return 1;
    }

    // 输出根目录名
    char *displayName = NULL;
    if (argc >= 2) {
        displayName = root->name;
    } else {
        displayName = getBaseName();
    }
    printf("%s/\n", displayName);
    if (argc < 2) free(displayName);

    FileNode *child = root->firstChild;
    int childCount = 0;
    FileNode *tmp = child;
    while (tmp) { childCount++; tmp = tmp->nextSibling; }
    int idx = 0;
    while (child) {
        int isLast = (++idx == childCount);
        printTree(child, "", isLast);
        child = child->nextSibling;
    }

    int dirs = 0, files = 0;
    countDirFile(root, &dirs, &files);
    printf("\n%d 个目录, %d 个文件\n", dirs, files);
    printf("二叉树结点总数: %d\n", countNodes(root));
    printf("叶子结点数: %d\n", countLeaves(root));
    printf("树的高度: %d\n", treeHeight(root));

    freeTree(root);
    return 0;
}