#include <stdlib.h>
#include <stdio.h>
#include <string.h>


///////////////////////////////////////////////////////////////////////////////
//宏定义区域

#define MEMSIZE 80
#define COMMAND_LEN 256
#define FILE_NAME_LEN 20
#define SPLIT_LINE_1 "--------------------------------------------------------------------------------"
#define SPLIT_LINE_2 "****************************MemoryHeader****************************************"
#define SPLIT_LINE_3 "       id                *             next             pack      size      used"
#define SPLIT_LINE_4 "********************************************************************************"

///////////////////////////////////////////////////////////////////////////////
//结构体区域

/**
 * @brief 内存首部结构体
 *
 */
typedef struct header
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

typedef struct
{
    char *command_name;
    char *process_name;
    char *algo_name;
    int size;
    char *file_name;
} command_content;

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
/**
 * @brief 紧凑内存时将未使用的内存空间移动到后面
 *
 */
void mem_move(char *empty_ptr, char *full_ptr);

/**
 * @brief 紧凑内存时更改表头链表，将未使用的内存放到后面
 *
 */
memory_header *mem_compact_header_update(memory_header *empty_header, memory_header *prev_header, int size);

/**
 * @brief 找到header链表中末尾元素
 *
 */
memory_header *find_the_last_header();

/**
 * @brief 将未使用的内存整合成一个洞
 *
 */
void combine_empty_mem();

int parse_cmd(FILE *fp);

int select_cmd();

void reset();

void handler();
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
char mem_space[MEMSIZE];
command_content read_command;

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
    first_header = (memory_header *)malloc(sizeof(memory_header));
    if(first_header == NULL)
    {
        return -1;
    }

    first_header->pack = mem_space;
    first_header->size = MEMSIZE;
    first_header->next = NULL;
    first_header->used = 0;

    for(int i = 0; i < MEMSIZE; i++)
    {
        mem_space[i] = '.';
    }

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
    memory_header *cur_header = first_header;
    char *new_pack;
    while(cur_header != NULL)
    {
        if(cur_header->used == 0 && cur_header->size >= size)
        {
            new_pack = cur_header->pack;
            char *allocated_mem = new_pack;
            for(int i = 0; i < size; i++)
            {
                allocated_mem[i] = name;
            }

            return new_pack;
        } else
        {
            cur_header = cur_header->next;
        }
    }

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
    memory_header *cur_header = first_header;
    memory_header *best_header = NULL;
    char *new_pack = NULL;

    //先寻找第一个符合条件的洞
    while(cur_header != NULL)
    {
        if(cur_header->used == 0 && cur_header->size >= size)
        {
            best_header = cur_header;
            break;
        }

        cur_header = cur_header->next;
    }

    //再找出符合条件的最小的洞
    cur_header = best_header;
    if(best_header != NULL)
    {
        while(cur_header != NULL)
        {
            if(cur_header->used == 0 && cur_header->size >= size && cur_header->size < best_header->size)
            {
                best_header = cur_header;
            }
            cur_header = cur_header->next;
        }
        new_pack = best_header->pack;

        char *allocated_mem = new_pack;
        for(int i = 0; i < size; i++)
        {
            allocated_mem[i] = name;
        }

        return new_pack;
    } else
    {
        return NULL;
    }

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
    memory_header *cur_header = first_header;
    memory_header *best_header = NULL;
    char *new_pack = NULL;

    //先寻找第一个符合条件的洞
    while(cur_header != NULL)
    {
        if(cur_header->used == 0 && cur_header->size >= size)
        {
            best_header = cur_header;
            break;
        }

        cur_header = cur_header->next;
    }

    //再找出符合条件的最大的洞
    cur_header = best_header;
    if(best_header != NULL)
    {
        while(cur_header != NULL)
        {
            if(cur_header->used == 0 && cur_header->size >= size && cur_header->size > best_header->size)
            {
                best_header = cur_header;
            }
            cur_header = cur_header->next;
        }
        new_pack = best_header->pack;

        char *allocated_mem = new_pack;
        for(int i = 0; i < size; i++)
        {
            allocated_mem[i] = name;
        }

        return new_pack;
    } else
    {
        return NULL;
    }
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
    memory_header *cur_header = first_header;
    int id = 1;
    printf("%s\n", SPLIT_LINE_1);
    printf("%s\n", SPLIT_LINE_2);
    printf("%s\n", SPLIT_LINE_3);
    while(cur_header != NULL)
    {
        printf("%9d%17p%17p%17p%10d%10d\n", id, cur_header, cur_header->next, cur_header->pack, cur_header->size, cur_header->used);
        id++;
        cur_header = cur_header->next;
    }
    printf("%s\n", SPLIT_LINE_4);

    return 1;
}

int mem_allocate()
{
    char *new_pack;
    memory_header *cur_header = first_header;
    memory_header *prev_header;

    char name = read_command.process_name[0];
    char algo = read_command.algo_name[0];
    int size = read_command.size;
    new_pack = mem_alloc(name, size, algo);
    if(new_pack != NULL)
    {
        //找到分配的洞的header
        while(cur_header != NULL && cur_header->pack != new_pack)
        {
            prev_header = cur_header;
            cur_header = cur_header->next;
        }

        //如果新分配内存大小等于对应的洞的大小，则直接修改对应header
        if(cur_header->size == size)
        {
            cur_header->used = 1;
        } else
        {
            //如果新分配的内存大小是小于洞的大小的，则插入新的header，对应的洞的大小减去新分配的大小，修改数据开始地址
            memory_header *new_header = (memory_header *)malloc(sizeof(memory_header));
            new_header->size = size;
            new_header->pack = new_pack;
            new_header->used = 1;

            cur_header->pack += size;
            cur_header->size -= size;

            if(cur_header == first_header)
            {
                new_header->next = first_header;
                first_header = new_header;
            } else
            {
                new_header->next = cur_header;
                prev_header->next = new_header;
            }

        }
    } else
    {
        printf("Failed to allocate memory.\n");
    }

    return 1;
}

/**
 * @brief 释放内存后更新memory header链表的内容
 *
 */
void mem_free_header_update(char *pack)
{
    memory_header *cur_header = first_header;
    memory_header *prev_header = NULL;
    memory_header *tmp_header;
    
    while(cur_header->pack != pack)
    {
        prev_header = cur_header;
        cur_header = cur_header->next;
    }

    //如果是第一个，如果后边是正在用的，或下一个是NULL，则直接改used=0;否则更改当前size和used，将后边的header删掉
    if(cur_header == first_header)
    {
        if(cur_header->next != NULL)
        {
            if(((memory_header *)cur_header->next)->used == 0)
            {
                cur_header->used = 0;
                cur_header->size += ((memory_header *)cur_header->next)->size;
                tmp_header = cur_header->next;
                cur_header->next = ((memory_header *)cur_header->next)->next;
                free(tmp_header);
            } else
            {
                cur_header->used = 0;
            }
        } else
        {
            cur_header->used = 0;
        }
    } else if(cur_header->next == NULL)
    {
        //如果是最后一个，如果前边是正在用的，或前一个是NULL，则直接改used=0；否则更改前一个的size，并删掉当前header
        if(prev_header != NULL)
        {
            if(prev_header->used == 0)
            {
                prev_header->size += cur_header->size;
                prev_header->next = cur_header->next;
                free(cur_header);
            } else
            {
                cur_header->used = 0;
            }
        } else
        {
            cur_header->used = 0;
        }
    }else
    {
        //否则（既不是第一个也不是最后一个的情况下）
        //如果前一个是空的，后一个不是，则修改前一个的size，删掉当前header
        //如果前一个是在用的，后一个是空的，则修改当前的size和used，删掉下一个header
        //如果前后都是在用的，则直接修改used=0；
        if(prev_header->used == 0)
        {
            if(((memory_header *)cur_header->next)->used == 0)
            {
                prev_header->size += (cur_header->size + ((memory_header *)cur_header->next)->size);
                prev_header->next = ((memory_header *)cur_header->next)->next;
                free(cur_header->next);
                free(cur_header);
            } else
            {
                prev_header->size += cur_header->size;
                prev_header->next = cur_header->next;
                free(cur_header);
            }
        } else if(((memory_header *)cur_header->next)->used == 0)
        {
            cur_header->used = 0;
            cur_header->size += ((memory_header *)cur_header->next)->size;
            tmp_header = cur_header->next;
            cur_header = ((memory_header *)cur_header->next)->next;
            free(tmp_header);
        } else
        {
            cur_header->used = 0;
        }
    }
}

int mem_free()
{
    //将对应内存修改为'.'
    int first_mem_flag = 1; //是否是要释放的内存的首地址
    char *free_pack;
    char process_name = read_command.process_name[0];
    for(int i = 0; i < MEMSIZE; i++)
    {
        if(mem_space[i] == process_name)
        {
            mem_space[i] = '.';
            if(first_mem_flag)
            {
                free_pack = &mem_space[i];
                mem_free_header_update(free_pack);
                first_mem_flag = 0;
            }
        } else
        {
            first_mem_flag = 1;
        }
    }

    return 1;
}

int mem_show()
{
    for(int i = 0; i < MEMSIZE; i++)
    {
        printf("%c", mem_space[i]);
    }
    printf("\n");

    return 1;
}

int mem_read()
{
    //打开文件
    FILE *fp_in;
    if((fp_in = fopen(read_command.file_name, "r")) == NULL)
    {
        fprintf(stderr, "Failed to open %s.\n", read_command.file_name);
    }

    while(parse_cmd(fp_in))
    {
        handler();
    }

    return should_run;    
}

void mem_move(char *empty_ptr, char *full_ptr)
{
    while(full_ptr  < (mem_space + MEMSIZE))
    {
        *empty_ptr = *full_ptr;
        ++empty_ptr;
        ++full_ptr;
    }

    while(empty_ptr < (mem_space + MEMSIZE))
    {
        *empty_ptr = '.';
        ++empty_ptr;
    }
}

memory_header *mem_compact_header_update(memory_header *empty_header, memory_header *prev_header, int size)
{
    memory_header *next_header = empty_header->next;
    memory_header *last_header;
    memory_header *tmp_header;
    
    //若空的洞是第一个表头，则下一个表头改为第一个，当前表头变为最后一个
    if(empty_header == first_header)
    {
        first_header = empty_header->next;
        empty_header->next = NULL;
        last_header = find_the_last_header();
        last_header->next = empty_header;
        tmp_header = first_header;
    } else
    {
        //若空的洞不是第一个表头，则前一个表头指向当前的表头的下一个，当前表头改为最后一个
        prev_header->next = empty_header->next;
        empty_header->next = NULL;
        last_header = find_the_last_header();
        last_header->next = empty_header;
        tmp_header = prev_header->next;
    }

    //更新数据开始位置
    while(tmp_header->next != NULL)
    {
        tmp_header->pack -= size;
        tmp_header = tmp_header->next;
    }
    tmp_header->pack = &mem_space[MEMSIZE - size];

    combine_empty_mem();

    return next_header;
}

memory_header *find_the_last_header()
{
    memory_header *cur_header = first_header;
    memory_header *prev_header = NULL;

    while(cur_header != NULL)
    {
        prev_header = cur_header;
        cur_header = cur_header->next;
    }

    return prev_header;
}

void combine_empty_mem()
{
    memory_header *cur_header = first_header;
    memory_header *tmp_header;

    while(cur_header->next != NULL)
    {
        if(cur_header->used == 0 && ((memory_header *)(cur_header->next))->used ==0)
        {
            cur_header->size += ((memory_header *)(cur_header->next))->size;
            tmp_header = cur_header->next;
            cur_header->next = ((memory_header *)(cur_header->next))->next;
            free(tmp_header);
        }

        if(cur_header->next != NULL)
        {
            cur_header = cur_header->next;
        }
    }
}

int mem_compact()
{
    char *empty_ptr;
    char *full_ptr;
    memory_header *cur_header = first_header;
    memory_header *prev_header = NULL;
    int size;
    //根据表头链表找到未使用的洞，且不是最后一个表头
    while(cur_header->next != NULL)
    {
        if(cur_header->used == 0)
        {
            empty_ptr = cur_header->pack;
            size = cur_header->size;
            full_ptr = ((memory_header *)(cur_header->next))->pack;
            mem_move(empty_ptr, full_ptr);
            cur_header = (memory_header *)mem_compact_header_update(cur_header, prev_header, size);
        }else
        {
            prev_header = cur_header;
            cur_header = cur_header->next;
        }
    }

    return 1;
}

int mem_exit()
{
    memory_header *cur_header = first_header;
    memory_header *tmp_header;
    while(cur_header != NULL)
    {
        tmp_header = cur_header->next;
        free(cur_header);
        cur_header = tmp_header;
    }

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
    printf("mem>");
}

/**
 * @brief 显示用法
 *
 */
void show_usage()
{
    printf("Usage:\n");
    printf("D\nA <name> <size> <algo>\nF <name>\nS\nR <file>\nC\nE\n");
    printf("Type program names and arguments, and hit enter.\n");
}

/**
 * @brief 解析命令，命令正确返回1，不正确返回0
 *
 * @param fp
 * @return int
 */
int parse_cmd(FILE *fp)
{
    char *str_tok;
    char read_line[COMMAND_LEN];

    read_command.command_name = (char *)calloc(2, sizeof(char));
    read_command.process_name = (char *)calloc(2, sizeof(char));
    read_command.algo_name = (char *)calloc(2, sizeof(char));
    read_command.file_name = (char *)calloc(FILE_NAME_LEN, sizeof(char));

    if(fgets(read_line, COMMAND_LEN, fp) == NULL)
    {
        return 0;
    }
    str_tok = strtok(read_line, " \n");
    switch(str_tok[0])
    {
        case 'D':
            strcpy(read_command.command_name, "D");
            break;
        case 'A':
            strcpy(read_command.command_name, "A");

            str_tok = strtok(NULL, " \n");
            strcpy(read_command.process_name, str_tok);

            str_tok = strtok(NULL, " \n");
            read_command.size = atoi(str_tok);

            str_tok = strtok(NULL, " \n");
            strcpy(read_command.algo_name, str_tok);

            break;
        case 'F':
            strcpy(read_command.command_name, "F");

            str_tok = strtok(NULL, " \n");
            strcpy(read_command.process_name, str_tok);

            break;
        case 'S':
            strcpy(read_command.command_name, "S");

            break;
        case 'R':
            strcpy(read_command.command_name, "R");

            str_tok = strtok(NULL, " \n");
            strcpy(read_command.file_name, str_tok);

            break;
        case 'C':
            strcpy(read_command.command_name, "C");

            break;
        case 'E':
            strcpy(read_command.command_name, "E");

            break;
        default:
            return 0;
    }

    return 1;
}

/**
 * @brief 选择命令
 *
 * @return int 未找到返回-1,找到了返回数组下标
 */
int select_cmd()
{
    for(int i = 0; i < 7; i++)
    {
        if(!strcmp(commands[i].command, read_command.command_name))
        {
            return i;
        }
    }

    return -1;
}

/**
 * @brief 重置数据（如有必要）
 *
 */
void reset()
{
    free(read_command.command_name);
    free(read_command.process_name);
    free(read_command.algo_name);
    free(read_command.file_name);
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
