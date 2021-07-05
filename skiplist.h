/*
 * @Author: Hao Dong
 * @Date: 2021-07-04 11:12:48
 * @LastEditors: Hao Dong
 * @LastEditTime: 2021-07-05 18:36:19
 * @Description: 
 */
#ifndef SKIPLIST_H
#define SKIPLIST_H
#include <chrono>
#include <random>
#include <string.h>
#include <iostream>
using namespace std;
class node
{
    public:
        node* *forward_pointer;
        int cur_level;
        int value;

        node(int value,int level)
        {
            this->value = value;
            this->cur_level = level;
            forward_pointer = new node*[level + 1];
        }
        ~node()
        {
            delete[] forward_pointer;
        }
    private:
        int key;
        
        
        
};
class skiplist
{
    public:
        node* tail;
        node* head;
        int max_level;//上限
        int maxmum_level_of_the_list;//当前最大
        skiplist(int max_level)
        {
            this->max_level = max_level;
            maxmum_level_of_the_list = 1;
            tail = new node(0,max_level);
            head = new node(0,max_level);

            for(int i = 0;i <= max_level;++i)
            {
                head->forward_pointer[i] = tail;
            }
            for(int i = 0;i <= max_level;++i)
            {
                tail->forward_pointer[i] = nullptr;
            }
        }
        ~skiplist()
        {
            delete head;
            delete tail;
        }
        node* create_node(int);
        bool insert(int);
        bool search(int);
        bool remove(int);
        unsigned int get_random_level();
        void display();
    private:
        
};

/**
 * @@description: 以1/e为分界点，在[0.0,1.0)这个区间内产生随机数，大于1/e时跳出，返回产生符合条件的随机数的次数作为level(<= max_level)
 * @param {*}
 * @return {unsigned int}
 * @author: Hao Dong
 */
unsigned int skiplist::get_random_level()
{
    random_device rd;
    mt19937 rand(rd());
    uniform_real_distribution<> dis(0.0,1.0);
    int random_level = 1;
    while(dis(rand) < 0.368 && random_level <= max_level)
    {
        ++random_level;
    }
    return random_level;       
}

/**
 * @@description:根据传入的value产生一个新的具有随机level的node，不会改变skiplist的maxmum_level
 * @param {int} value
 * @return {node}
 * @author: Hao Dong
 */
node* skiplist::create_node(int value)
{
    
    return new node(value,get_random_level());
}

/**
 * @@description: 给元素找到一个合适的位置，并重建索引
 * @param {int} value
 * @return {bool}
 * @author: Hao Dong
 */
bool skiplist::insert(int value)
{
    //tmp node for traversing
    node* tmp_node = head;
    //create update array
    node* *update = new node*[max_level+1]; 
    memset(update,0,sizeof((max_level+1)*(sizeof(node*))));
    update[head->cur_level] = head;
    //update[i]代表待插入节点左侧，level为i的节点中，最右边的节点
    int tmp_level = tmp_node->cur_level;
    while(true)//找到待插入位置
    {
        while(tmp_node->forward_pointer[tmp_level] != tail && value > tmp_node->forward_pointer[tmp_level]->value)
        {
            /*
                更新update数组这里之前写错了，导致后面无法重建索引，现已修正
            */
            tmp_node = tmp_node->forward_pointer[tmp_level];
            update[tmp_node->cur_level] = tmp_node;
        }
        if(tmp_node->forward_pointer[tmp_level] != tail && value == tmp_node->forward_pointer[tmp_level]->value)
        {
            return false;
        }
        if(tmp_level == 1)
        {
            break;
        }
        --tmp_level;
    }
    //insert after tmp_node
    node* new_node = create_node(value);
    //cout << "new node level: " << new_node->cur_level << " " << new_node << "\n";
    if(new_node->cur_level > maxmum_level_of_the_list)
    {
        maxmum_level_of_the_list = new_node->cur_level;
    }
    //cout << "head:" << head << "\n";
    //cout << "tail:" << tail << "\n";
    // for(int i = 1;i <= max_level;++i)
    // {
    //     cout << "update[" << i << "]" << update[i] << "\n";
    // }
    //接下来重建被隔断的索引
    for(int i = 1;i <= max_level;++i)
    {
        if(update[i] != nullptr)
        {
            int level;
            if(new_node->cur_level >= update[i]->cur_level)
            {
                level = update[i]->cur_level;
            }
            else 
            {
                level = new_node->cur_level;
            }
            for(int j = 1;j <= level;++j)
            {
                if(new_node->value < update[i]->forward_pointer[j]->value || update[i]->forward_pointer[j] == tail)
                {
                    new_node->forward_pointer[j] = update[i]->forward_pointer[j];
                    update[i]->forward_pointer[j] = new_node;
                }
            }
        }
    }
    return true;
}

bool skiplist::remove(int value)
{
    node* tmp_node = head;
    int tmp_level = head->cur_level;
    node* *update = new node*[max_level+1];
    memset(update,0,sizeof((max_level+1)*sizeof(node*)));
    update[head->cur_level] = head;
    while(true)
    {
        while(value > tmp_node->forward_pointer[tmp_level]->value && tmp_node->forward_pointer[tmp_level] != tail)
        {
            tmp_node = tmp_node->forward_pointer[tmp_level];
            update[tmp_node->cur_level] = tmp_node;
        }
        if(value == tmp_node->forward_pointer[tmp_level]->value && tmp_level == 1)
        {
            tmp_node = tmp_node->forward_pointer[tmp_level];
            break;
        }
        if(tmp_level == 1)
        {
            return false;
        }
        --tmp_level;
    }
    for(int i = 1;i <= max_level;++i)
    {
        if(update[i] != nullptr)
        {
            int level = update[i]->cur_level > tmp_node->cur_level ? tmp_node->cur_level : update[i]->cur_level;
            for(int j = 1;j <= level;++j)
            {
                if(update[i]->forward_pointer[j] == tmp_node)
                {
                    update[i]->forward_pointer[j] = tmp_node->forward_pointer[j];
                }
            }
        }
    }
    delete tmp_node;
    return true;
}


bool skiplist::search(int value)
{
    node* tmp_node = head;
    int tmp_level = head->cur_level;
    while(true)
    {
        while(value > tmp_node->forward_pointer[tmp_level]->value && tmp_node->forward_pointer[tmp_level] != tail)
        {
            tmp_node = tmp_node->forward_pointer[tmp_level];
        }
        if(value == tmp_node->forward_pointer[tmp_level]->value && tmp_node->forward_pointer[tmp_level] != tail)
        {
            return true;
        }
        if(tmp_level == 1)
        {
            return false;
        }
        --tmp_level;
    }
}

void skiplist::display()
{
    for(int i = maxmum_level_of_the_list;i >= 1;--i)
    {
        cout << "level" << i << ": ";
        node* tmp_node = head->forward_pointer[i];
        while(tmp_node != tail)
        {
            cout << tmp_node->value << " ";
            tmp_node = tmp_node->forward_pointer[i];
        }
        cout << "\n";
    }
}

#endif // !SKIPLIST_H



