/**
 * @file memory.c
 * @author 智识之道 (binary@techbootcamp)
 * @brief 连续内存分配
 * @version 0.1
 * @date 2022-01-05
 *
 * @copyright Copyright (c) 2022
 *
 */

///////////////////////////////////////////////////////////////////////////////
//头文件区域
#include <stdlib.h>
#include <stdio.h>
//有需要增加其他头文件在这里添加

///////////////////////////////////////////////////////////////////////////////
//宏定义区域

#define MEMSIZE 80
//有需要增加其他头宏定义在这里添加

///////////////////////////////////////////////////////////////////////////////
//结构体区域

/**
 * @brief 内存首部结构体
 *
 */
typedef struct
{
    //下一个
    void *next;
    //数据起始地址
    void *pack;
    //大小
    int size;
    //是否正在使用
    int used;
} memory_header;

/**
 * @brief 命令结构体
 *
 */
typedef struct
{
    //命令
    char *command;
    //命令函数
    int (*command_func)();
} command_info;

///////////////////////////////////////////////////////////////////////////////
//函数声明区域

/**
 * @brief 调试：打印当前内存分配的详细情况，方便调试。
 *
 * @return int
 */
int mem_dump();

/**
 * @brief 分配内存：使用上述 3 种分配算法之一为进程分配 N 个字节。
 *
 * @return int
 */
int mem_allocate();

/**
 * @brief 释放内存：释放给定进程的所有分配。
 *
 * @return int
 */
int mem_free();

/**
 * @brief 显示内存：显示内存池的状态(已分配和空闲块)。
 *
 * @return int
 */
int mem_show();

/**
 * @brief 读取脚本：读取脚本，执行文件中的每个命令。
 *
 * @return int
 */
int mem_read();

/**
 * @brief 紧凑内存：紧凑分配，成为一个连续的块。
 *
 * @return int
 */
int mem_compact();

/**
 * @brief 退出程序：退出当前的程序。
 *
 * @return int
 */
int mem_exit();

//有需要增加其他函数声明在这里添加

///////////////////////////////////////////////////////////////////////////////
//变量区域

//内存首部地址
memory_header *first_header;

//是否继续运行
int should_run = 1;

command_info commands[] = {
    {"D", &mem_dump},
    {"A", &mem_allocate},
    {"F", &mem_free},
    {"S", &mem_show},
    {"R", &mem_read},
    {"C", &mem_compact},
    {"E", &mem_exit}};

//有需要增加其他变量在这里添加

///////////////////////////////////////////////////////////////////////////////
//函数区域

//有需要增加其他函数在这里添加

/**
 * @brief 内存池初始化
 *
 * @return int 正确返回0,错误返回负数
 */
int mem_init()
{
    printf("TODO: mem_init");
    return 0;
}

/**
 * @brief First-Fit
 *
 * @param name 名字
 * @param size 大小
 * @return void* 分配的首地址
 */
void *mem_alloc_for_first_fit(char name, int size)
{
    printf("TODO: mem_alloc_for_first_fit");
    return NULL;
}

/**
 * @brief Best-Fit
 *
 * @param name 名字
 * @param size 大小
 * @return void* 分配的首地址
 */
void *mem_alloc_for_best_fit(char name, int size)
{
    printf("TODO: mem_alloc_for_best_fit");
    return NULL;
}

/**
 * @brief Worst-Fit
 *
 * @param name 名字
 * @param size 大小
 * @return void* 分配的首地址
 */
void *mem_alloc_for_worst_fit(char name, int size)
{
    printf("TODO: mem_alloc_for_worst_fit");
    return NULL;
}

/**
 * @brief 根据指定的算法分配内存
 * 注：此函数无须增加内容。
 *
 * @param name 名字
 * @param size 大小
 * @param algo 指定算法
 * @return void* 分配的首地址
 */
void *mem_alloc(char name, int size, char algo)
{
    switch (algo)
    {
    case 'F':
        mem_alloc_for_first_fit(name, size);
        break;

    case 'B':
        mem_alloc_for_best_fit(name, size);
        break;

    case 'W':
        mem_alloc_for_worst_fit(name, size);
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////

int mem_dump()
{
    printf("TODO: mem_dump");
    return 0;
}

int mem_allocate()
{
    printf("TODO: mem_allocate");
    return 0;
}

int mem_free()
{
    printf("TODO: mem_free");
    return 0;
}

int mem_show()
{
    printf("TODO: mem_show");
    return 0;
}

int mem_read()
{
    printf("TODO: mem_read");
    return 0;
}

int mem_compact()
{
    printf("TODO: mem_compact");
    return 0;
}

int mem_exit()
{
    printf("TODO: mem_exit");
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 命令个数
 *
 * @return int
 */
int cmd_num()
{
    printf("TODO: cmd_num");
    return 0;
}

/**
 * @brief 显示标记
 *
 */
void show_mem()
{
    printf("TODO: show_mem");
}

/**
 * @brief 显示用法
 *
 */
void show_usage()
{
    printf("TODO: show_usage");
}

/**
 * @brief 解析命令
 *
 * @param fp
 * @return int
 */
int parse_cmd(FILE *fp)
{
    printf("TODO: parse_cmd");
    return 0;
}

/**
 * @brief 选择命令
 *
 * @return int 未找到返回-1,找到了返回数组下标
 */
int select_cmd()
{
    printf("TODO: select_cmd");
    return -1;
}

/**
 * @brief 重置数据（如有必要）
 *
 */
void reset()
{
    printf("TODO: reset");
}

/**
 * @brief 处理命令
 * 注：此函数无须增加内容。
 *
 */
void handler()
{
    int pos = select_cmd();

    if (pos >= 0)
    {
        should_run = commands[pos].command_func();
    }

    reset();
}
////////////////////////////////////////////

/**
 * @brief 主函数入口
 * 注：此函数无须增加内容。
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 返回值
 */
int main(int argc, char *argv[])
{
    if (mem_init())
    {
        exit(1);
    }

    show_usage();

    while (should_run)
    {
        show_mem();

        if (!parse_cmd(stdin))
        {
            continue;
        }

        handler();
    }

    return 0;
}
