#ifdef __cplusplus
extern "C"
{
#endif
#include <erl_nif.h>
#include "total_nif.h"
#include "publisher_nif.h"
#include <stdio.h>
#include <string.h>

#include "rcl/publisher.h"
#include <rosidl_runtime_c/message_type_support_struct.h>

#include <std_msgs/msg/int16.h>
#include <std_msgs/msg/string.h>

ERL_NIF_TERM nif_rcl_get_zero_initialized_publisher(ErlNifEnv* env,int argc,const ERL_NIF_TERM argv[])
{
  rcl_publisher_t* res;
  ERL_NIF_TERM ret;
  res = enif_alloc_resource(rt_pub,sizeof(rcl_publisher_t));
  if(res == NULL) {
    return enif_make_badarg(env);
  }
  ret = enif_make_resource(env,res);
  enif_release_resource(res);
  *res = rcl_get_zero_initialized_publisher();

  return ret;
}

ERL_NIF_TERM nif_rcl_publisher_get_default_options(ErlNifEnv* env,int argc,const ERL_NIF_TERM argv[])
{
  rcl_publisher_options_t* res;
  ERL_NIF_TERM ret;
  res = enif_alloc_resource(rt_pub_options,sizeof(rcl_publisher_options_t));
  if(res == NULL) {
    return enif_make_badarg(env);
  }
  ret = enif_make_resource(env,res);
  enif_release_resource(res);
  *res = rcl_publisher_get_default_options();

  return ret;
}


ERL_NIF_TERM nif_rcl_publisher_get_topic_name(ErlNifEnv* env,int argc,const ERL_NIF_TERM argv[])
{
  rcl_publisher_t* res;
  if(argc != 1) {
    return enif_make_badarg(env);
  }

  if(!enif_get_resource(env,argv[0],rt_pub,(void**) &res)) {
    return enif_make_badarg(env);
  }
  const char* result;
  result = rcl_publisher_get_topic_name(res);
  return enif_make_string(env,result,ERL_NIF_LATIN1);
}


ERL_NIF_TERM nif_rcl_publisher_fini(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  rcl_ret_t* res;
  ERL_NIF_TERM ret;

  rcl_publisher_t* res_arg_pub;
  rcl_node_t* res_arg_node;

  if(argc != 2) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env, argv[0], rt_pub, (void**) &res_arg_pub)) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env, argv[1], rt_node, (void**) &res_arg_node)) {
    return enif_make_badarg(env);
  }

  res = enif_alloc_resource(rt_ret,sizeof(rcl_ret_t));
  if(res == NULL) {
    return enif_make_badarg(env);
  }
  ret = enif_make_resource(env,res);
  enif_release_resource(res);

  *res = rcl_publisher_fini(res_arg_pub,res_arg_node);

  return ret;
}
/*
  rcl_publisher_init(
    rcl_publisher_t * publisher,
    const rcl_node_t * node,
    const rosidl_message_type_support_t * type_support,
    const char * topic_name,
    const rcl_publisher_options_t * options);
*/
ERL_NIF_TERM nif_rcl_publisher_init(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  if(argc != 5) {
    return enif_make_badarg(env);
  }
  int return_value = 0;
  ERL_NIF_TERM ret;
  rcl_publisher_t*  res_pub;
  rcl_node_t* res_node;
  void* res_ts_tmp;
  rosidl_message_type_support_t** res_ts;
  rcl_publisher_options_t* res_options;

  if(!enif_get_resource(env, argv[0], rt_pub, (void**) &res_pub)) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env, argv[1], rt_node, (void**) &res_node)) {
    return enif_make_badarg(env);
  }

  char buf[128]; //トピック名を格納するためのバッファ
  (void)memset(&buf,'\0',sizeof(buf));
  if(!enif_get_string(env,argv[2],buf,sizeof(buf),ERL_NIF_LATIN1)) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env, argv[3], rt_void, (void**) &res_ts_tmp)) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env, argv[4], rt_pub_options, (void**) &res_options)) {
    return enif_make_badarg(env);
  }

  res_ts = (rosidl_message_type_support_t**) res_ts_tmp;
  return_value = rcl_publisher_init(res_pub,res_node,*res_ts,buf,res_options); //segmentation fault
  ret = enif_make_resource(env,res_pub);

  return ret;
}


ERL_NIF_TERM nif_rcl_publisher_is_valid(ErlNifEnv* env,int argc,const ERL_NIF_TERM argv[])
{
  rcl_publisher_t* res;
  if(argc != 1) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env, argv[0], rt_pub, (void**) &res)) {
    return enif_make_badarg(env);
  }

  bool result;
  result = rcl_publisher_is_valid(res);
  if(result == true) {
    return atom_true;
  } else {
    return atom_false;
  }
}


ERL_NIF_TERM nif_rcl_publish(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{

  int return_value;
  rcl_publisher_t* res_pub;
  rmw_publisher_allocation_t* res_pub_alloc;
  const void * ros_message;
  ERL_NIF_TERM ret_pub,ret_pub_alloc;
  if(argc != 3) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env, argv[0], rt_pub, (void**) &res_pub)) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env,argv[1],rt_void,(void**) &ros_message)) {
    return enif_make_badarg(env);
  }
  if(!enif_get_resource(env, argv[2], rt_pub_alloc, (void**) &res_pub_alloc)) {
    return enif_make_badarg(env);
  }

  return_value = rcl_publish(res_pub,ros_message,res_pub_alloc);

  ret_pub = enif_make_resource(env,res_pub);
  ret_pub_alloc = enif_make_resource(env,res_pub_alloc);

  return enif_make_tuple3(env,enif_make_int(env,return_value),ret_pub,ret_pub_alloc);
}


ERL_NIF_TERM nif_create_pub_alloc(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  if(argc != 0) {
    return enif_make_badarg(env);
  }
  rmw_publisher_allocation_t* res;
  ERL_NIF_TERM ret;
  res = enif_alloc_resource(rt_pub_alloc,sizeof(rmw_publisher_allocation_t));
  if(res == NULL) {
    return enif_make_badarg(env);
  }
  ret = enif_make_resource(env,res);
  enif_release_resource(res);

  return ret;
}

#ifdef __cplusplus
}
#endif
