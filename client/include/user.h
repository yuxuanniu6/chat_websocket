#ifndef USER_H
#define USER_H

#include <string>

// id/name/sex/age/desc
class User
{
public:
    void setId(int id){
        m_id = id;
    }
    void setName(std::string name){
        m_name = name;
    }
    void setSex(std::string sex){
        m_sex = sex;
    }
    void setAge(int age){
        m_age = age;
    }
    void setDesc(std::string desc){
        m_desc= desc;
    }
    int getId(){
        return m_id;
    }
    int getAge(){
        return m_age;
    }
    std::string getName(){
        return m_name;
    }
    std::string getSex(){
        return m_sex;
    }
    std::string getDesc(){
        return m_desc;
    }
private:
    int m_id;
    std::string m_name;
    std::string m_sex;
    int m_age;
    std::string m_desc;
};

#endif