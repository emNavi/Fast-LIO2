#include <string>
#include <ros/ros.h>
template <class T>
class msg_base
{
private:
    /* data */
    std::string _msg_topic;
    virtual int deserializeMsg(T &msg) = 0;
    virtual int serializeMsg(T &msg) = 0;
    char *msg_buf;
    int _buf_max_len;
    // pvalue  = new char[20]; // 为变量请求内存

public:
    msg_base(std::string msg_topic, size_t buf_max_len);
    ~msg_base();
    int push_msg_once(char *msg, size_t len);
    void push_msg(char *msg, size_t len);
    int get_msg(char *msg);
};


template <class T>
msg_base<T>::msg_base(std::string msg_topic,size_t buf_max_len)
{
    _msg_topic = msg_topic;
    _buf_max_len = buf_max_len;
}
template <class T>
msg_base<T>::~msg_base()
{

}

template <class T>
int msg_base<T>::push_msg_once(char *msg,size_t len)
{
    if(len < _buf_max_len)
    {
        deserializeMsg(msg);
        return 0;
    }
    else
    {
        ROS_ERROR("msg to long,maybe you should use push_msgs");
        return -1;
    }
}
template <class T>
void msg_base<T>::push_msg(char *msg,size_t len)
{

}
