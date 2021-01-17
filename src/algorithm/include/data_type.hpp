/*
 * @Author: your name
 * @Date: 2021-01-17 16:07:14
 * @LastEditTime: 2021-01-17 19:39:19
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \cl\src\algorithm\include\data_type.h
 */
#pragma once


#include <string>
#include <vector>
#include <list>
#include <fstream>

#include "json/json.h"

struct drive_t;

//  链接状态枚举
enum e_link_status
{
    els_unconnection = 0,
    els_connection
};

//  邻居


//  sptn设备
struct drive_t
{
    std::string node_id;    //  所属节点ID
    std::string mac;        //  设备mac

    std::list<struct neighbor_t> neighbors;    //  连接的所有邻居

    Json::Value save(bool save_neighbors = true);

    void load(const Json::Value& v);
};

struct neighbor_t
{
    //struct drive local;
    drive_t remote;          //  远端
    e_link_status link_status;      //  链接状态

    Json::Value save()
    {
        Json::Value v;
        v["remote"] = remote.save(false);
        v["link_status"] = link_status;

        return v;
    }

    void load(const Json::Value& v)
    {
        link_status = (e_link_status)v["node_id"].asInt();
        remote.load(v["remote"]);
    }
};

Json::Value drive_t::save(bool save_neighbors)
{
    Json::Value v;
    v["node_id"] = node_id;
    v["mac"] = mac;

    if (save_neighbors == true)
    {
        for (auto nei : neighbors)
        {
            v["neighbors"].append(nei.save());
        }
    }

    return v;
}

void drive_t::load(const Json::Value& v)
{
    node_id = v["node_id"].asString();
    mac = v["mac"].asString();

    for (auto i : v["neighbors"])
    {
        neighbor_t nei;
        nei.load(i);
        neighbors.push_back(nei);
    }
}

//  节点
struct node_t
{
    std::string id;             //  节点ID
    std::list<drive_t> drives;  //  节点的所有sptn设备

    Json::Value save()
    {
        Json::Value v(Json::objectValue);
        v["id"] = id;

        Json::Value drives_j;
        for (auto dev : drives)
        {
            v["drives"].append(dev.save());
        }

        return v;
    }

    void load(const Json::Value& v)
    {
        id = v["id"].asString();

        for (auto i : v["drives"])
        {
            drive_t dev;
            dev.load(i);
            drives.push_back(dev);
        }
    }

    bool is_null() const { return id.empty(); }

    //  链路合并操作，一般用于预设拓扑与实际拓扑合并，形成通断关系。
    void merge(const node_t &n)
    {
        //  遍历所有设备
        for (auto i_drive : n.drives)
        {
            auto dev_pos_l = std::find_if(drives.begin(), drives.end(), [&i_drive](const drive_t& dev) { return dev.node_id == i_drive.node_id; });
            if (dev_pos_l == drives.end())
            {
                drives.push_back(i_drive);
            }
            else
            {
                //  遍历所有链路
                for (auto i_nei : i_drive.neighbors)
                {
                    //  查找是否存在对应连接
                    auto i_nei_l = std::find_if(dev_pos_l->neighbors.begin(), dev_pos_l->neighbors.end(), [&i_nei](const neighbor_t &n){
                        return n.remote.node_id == i_nei.remote.node_id;
                    });

                    if (i_nei_l == dev_pos_l->neighbors.end())
                    {
                        dev_pos_l->neighbors.push_back(i_nei);
                    }
                    else
                    {
                        //  合并连接状态
                        if (i_nei.link_status == els_connection)
                        {
                            i_nei_l->link_status = els_connection;
                        }
                    } 
                }
                
            }
            
            
        }
    }

    //  获取所有邻居节点{节点ID, 链接状态}
    std::list<std::pair<std::string, e_link_status>> get_all_neighbor_node() const 
    {
        std::list<std::pair<std::string, e_link_status>> ann;

        //  遍历所有设备
        for (auto dev : drives)
        {
            //  遍历设备的邻居
            for (auto nei : dev.neighbors)
            {
                //  查找是否已经记录
                auto _tmp_nei = std::find_if(ann.begin(), ann.end(), [&nei](const std::pair<std::string, e_link_status> &item)
                {
                    return item.first == nei.remote.node_id;
                });

                if (_tmp_nei == ann.end())
                {
                    ann.push_back({nei.remote.node_id, nei.link_status});
                }
                else
                {
                    if (nei.link_status == els_connection)
                    {
                        _tmp_nei->second = els_connection;
                    }
                }

            }
            
        }
        
        return ann;
    }

    //  获取与某个邻居节点连接的设备
    std::list<std::pair<drive_t, neighbor_t>> get_neighbor_drive(const std::string &id) const
    {
        std::list<std::pair<drive_t, neighbor_t>> re_nei;

        //  遍历所有设备
        for (auto dev : drives)
        {
            //  搜索是否与目标节点连接
            auto _roi_mei_pos = std::find_if(dev.neighbors.begin(), dev.neighbors.end(), [&id](const neighbor_t &item){
                return item.remote.node_id == id;
            });

            //  存在连接
            if (_roi_mei_pos != dev.neighbors.end())
            {
                re_nei.push_back({dev, *_roi_mei_pos});
            }
            
        }

        return re_nei;
    }
    
};


class network_topology
{
private:
    
    std::list<node_t> nodes;
    
public:

    network_topology::network_topology(/* args */)
    {
    }

    network_topology::~network_topology()
    {
    }

    //   存储到json
    Json::Value save()
    {
        Json::Value root(Json::objectValue);

        for (auto node : nodes)
        {
            root["nodes"].append(node.save());
        }

        return root;
    }

    void load(const Json::Value& v)
    {

        for (auto i : v["nodes"])
        {
            node_t node;
            node.load(i);
            nodes.push_back(node);
        }
    }

    bool save(const std::string& path)
    {
        Json::StreamWriterBuilder writerBuilder;
        std::ostringstream ostr;

        std::ofstream os(path.c_str(), std::ios::binary);
        if (os.is_open() == false)
        {
            return false;
        }

        std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
        jsonWriter->write(save(), &ostr);
        os << ostr.str();
        os.close();

        return true;
    }

    bool load(const std::string& path)
    {
        std::ifstream in(path.c_str(), std::ios::binary);
        if (in.is_open() == false)
        {
            return false;
        }

        Json::CharReaderBuilder builder;
        Json::String errs;

        Json::Value root;
        if (parseFromStream(builder, in, &root, &errs))
        {
            return false;
        }

        load(root);

        return true;
    }

    //  增加一个节点
    void add_node(const node_t &n)
    {
        nodes.push_back(n);
    }

    //  merge一个节点
    void merge_node(const node_t &n)
    {
        auto pos = std::find_if(nodes.begin(), nodes.end(), [&n](const node_t &item){
            return item.id == n.id;
        });

        if (pos == nodes.end())
        {
            add_node(n);
        }
        else
        {
            pos->merge(n);
        }
    }

    node_t get_node(const std::string id)
    {
        auto pos = std::find_if(nodes.begin(), nodes.end(), [&id](const node_t &item){
            return item.id == id;
        });

        if (pos == nodes.end())
        {
            return node_t();
        }
        else
        {
            return *pos;
        }
    }
    

    //  判断一个节点是否处于黑暗区（不可达）
    static bool is_isolated_island(const node_t &node)
    {
        auto neis = node.get_all_neighbor_node();
        
        bool b_isolated_island = true;

        for (auto nei : neis)
        {
            if (nei.second == els_connection)
            {
                b_isolated_island = false;
                break;
            }
            
        }
        
        return b_isolated_island;
    }

    //  传入一个中心节点为参考，获取边缘
    std::list<std::string> get_edge(const std::string &central_node_id)
    {
        /*
            是否存在边：搜索是否存在彻底断开的节点。
            暗边：搜索
            首先获取

            1. 判断中心节点是否
        */

       std::list<std::string> re;
       
       auto central_node = get_node(central_node_id);

       if (central_node.is_null() == true)
       {
           return re;
       }
       
       //   判断是孤岛外还是孤岛内
       bool b_isolated_island = is_isolated_island(central_node);

        //  孤岛内算边，计算明边即可
        if (b_isolated_island == false)
        {
            /* code */
        } 
    }

    static node_t _find(std::list<node_t> &ns, const std::string &id)
    {
        auto pos = std::find_if(ns.begin(), ns.end(), [&id](const node_t &n){
            return n.id == id;
        });

        if (pos == ns.end())
        {
            return node_t();
        }

        return *pos;
        
    }

    node_t find(const std::string &id) { return _find(nodes, id); }

    //  找到所有相邻的区
    //  tobe:剩余搜索区
    //  ns:区域内节点列表， 当前计算节点， 
    void _get_area(std::list<node_t> &tobe, std::list<node_t> &ns, e_link_status s)
    {
        if (tobe.empty() == true)
        {
            return ;
        }
        
        const node_t &n = tobe.front();
        tobe.pop_front();
        
        auto neis = n.get_all_neighbor_node();

        for (auto nei : neis)
        {
            if (nei.second == s)
            {
                //  判断是否在结果区
                if (_find(ns, nei.first).is_null() == true)
                {
                    //  不在则加入待处理区和结果区
                    tobe.push_back(find(nei.first));
                    ns.push_back(find(nei.first));
                }
                
            }
        }

        //tobe.remove_if([&n](const node_t &i){ return n.id == i.id; });
    }

    //  所有孤岛分区
    std::list<std::pair<std::list<node_t>, e_link_status>> get_area()
    {
        std::list<std::pair<std::list<node_t>, e_link_status>> re;
        
        std::list<node_t> nodes_tmp = nodes;

        //  增加第一个区间
        while (true)
        {
            auto pos = std::find_if(nodes_tmp.begin(), nodes_tmp.end(), [this](const node_t &n){ return is_isolated_island(n); });

            if (pos == nodes.end())
            {
                break;
            }

            std::list<node_t> tobe;
            std::list<node_t> ns;

            tobe.push_back(*pos);
            while (tobe.empty() == false)
            {
                _get_area(tobe, ns, els_unconnection);
            }

            re.push_back({ns, els_unconnection});

            //  排除区域
            nodes_tmp.remove_if([&ns](const node_t &n){
                return std::find_if(ns.begin(), ns.end(), [n](const node_t &nn){ return nn.id == n.id; }) != ns.end();
            });
        
        }

        re.push_back({nodes_tmp, els_connection});

        return re;
    }
    
    //  孤岛边缘计算
    std::list<node_t> get_edge(std::list<node_t> area)
    {
        std::list<node_t> re;
        
        for (auto n : area)
        {
            //  计算每个节点的边是否在区域内
            for (auto nei : n.get_all_neighbor_node())
            {
                if (_find(area, nei.first).is_null() == true)
                {
                    //  在区域外部
                    re.push_back(find(nei.first));
                }
                
            }
        }

        return re;
    }

    //  获取所有边
    std::list<std::list<node_t>> get_edge()
    {
        std::list<std::list<node_t>> re;

        //  获取所有分区
        auto areas = get_area();
        for (auto area : areas)
        {
            re.push_back(get_edge(area.first));
        }
        
        return re;
    }

};


