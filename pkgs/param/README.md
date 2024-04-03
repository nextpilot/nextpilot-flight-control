

每一种param api接口需要提供以下函数

```c++
// 获取参数的总个数，成功返回总个数，错误返回PARAM_INVALID
uint16_t get_count();

// 根据名称查找参数，成功返回索引，错误返回PARAM_INVALID
param_t find(const char* name, bool mark_used = true);

// 获取参数名称
const char* get_name(param_t p);

// 获取变量类型
param_type_t get_type(param_t p);

// 获取默认值
param_value_t get_value(param_t p);

// 获取变量标志
param_flag_t get_flag(param_t p);

// 获取和设置参数值，成功返回0，错误返回-1
// 每当调用set_value的时候，需要将unsaved和unsaved_ulog设置为1
int get_value(param_t p, param_value_t* val);
int set_value(param_t p, param_value_t* val, bool notify = true);

// 获取和设置参数状态，成功返回0，错误返回-1
int get_status(param_t p, param_status_t* status)
int set_status(param_t p, param_status_t* status);

// 重置参数为默认值，成功返回0，错误返回-1
int reset(param_t p, bool mark, bool notify = true);
```
