#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>

#ifndef __IOS__
#include <net/if_arp.h>
#endif
#include <netinet/tcp.h>

#include <net/if.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "parson.h"
#include "utils.h"

void noly_hexdump(unsigned char *start, int len)
{
    int i = 0;
    for(i = 0; i < len ; i++) {
        printf("%02X ", start[i]);
        if((i+1) % 8 == 0) printf("\n");
    }
    printf("\n");
}
void noly_set_tcp_nodelay(int sk)
{
    int enable = 1;
    setsockopt(sk, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}

int noly_socket_set_reuseaddr(int sk)
{
    int on = 1;
    return setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));
}

int noly_socket_set_nonblock(int sk)
{
    unsigned long on = 1;
    return ioctl(sk, FIONBIO, &on);
}

int noly_udp_rand_socket(int *port)
{
    int sock;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(struct sockaddr_in));
    unsigned sin_len = sizeof(struct sockaddr_in);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(0);
    serv.sin_family = AF_INET;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(bind(sock, (struct sockaddr *) &serv, sin_len)<0){
        fprintf(stderr, "bind socket port error\n");
    }
    if(getsockname(sock, (struct sockaddr *)&serv, &sin_len) < 0){
        fprintf(stderr, "get socket name error\n");
    }
    int sport = htons(serv.sin_port);
    fprintf(stdout, "create udp random port %d\n", sport);
    *port = sport;
    return sock;
}

int noly_udp_socket(int port)
{
    int sock;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(struct sockaddr_in));
    unsigned sin_len = sizeof(struct sockaddr_in);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(port);
    serv.sin_family = AF_INET;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(bind(sock, (struct sockaddr *) &serv, sin_len)<0){
        fprintf(stderr, "bind socket port error\n");
    }
    if(getsockname(sock, (struct sockaddr *)&serv, &sin_len) < 0){
        fprintf(stderr, "get socket name eerror\n");
    }
    fprintf(stdout, "create udp random port %d\n", port);
    return sock;
}

int noly_udp_sender(char *addr, int port, char *payload, int len)
{
    int sock;
    struct sockaddr_in serv_addr;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0) { return -1; }
    if(payload == NULL) { return -1; }
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(addr);
    serv_addr.sin_port = htons(port);
    ssize_t n = sendto(sock, payload, len, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    close(sock);
    return n;
}

int noly_tcp_socket(int port, int max_cli)
{
    int max;
    if(port < 1 || port > 65535) return -1;
    if(max_cli < 1 || max_cli > 65535)
        max = 10;
    else
        max = max_cli;
    int sock = -1;
    struct sockaddr_in srv_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock > 0){
        memset(&srv_addr, 0, sizeof(srv_addr));
        srv_addr.sin_family = AF_INET;
        srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        srv_addr.sin_port = htons(port);
        noly_socket_set_reuseaddr(sock);
        noly_socket_set_nonblock(sock);
        if(bind(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0 || listen(sock, max) != 0){
            printf("bind error (%d) %s\n", errno, strerror(errno));
            close(sock);
            sock = -1;
        }
        printf("tcp socket %d port %d have being created\n", sock, port);
    }
    return sock;
}

int noly_tcp_connect(char *ip, int port)
{
    if(!ip || port < 0 || port > 65535) return -1;
    int sk;
    struct sockaddr_in dest;
    sk = socket(AF_INET, SOCK_STREAM, 0);
    if(sk < 0) return INVALID_SOCKET;
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr.s_addr = inet_addr(ip);
    noly_set_tcp_nodelay(sk);
    int ret = connect(sk , (struct sockaddr *)&dest, sizeof(dest));
    if(ret == 0) return sk;
    return INVALID_SOCKET;
}



int noly_udp_socket_from(int start, int *port)
{
    int i = start;
    int sk = INVALID_SOCKET;
    for(i = start ; i < 65535 ; i++){
        if((sk = noly_udp_socket(i)) > 0 ){
            *port = i;
            return sk;
        }
    }
    return INVALID_SOCKET;
}

int noly_tcp_socket_from(int start, int *port, int max_cli)
{
    int i = start;
    int sk = INVALID_SOCKET;
    for(i = start ; i < 65535 ; i++){
        if((sk = noly_tcp_socket(i, max_cli)) > 0 ){
            *port = i;
            return sk;
        }
    }
    return INVALID_SOCKET;
}

int noly_json_obj_get_obj(JSON_Object *obj, char *key, char *val, int len)
{
    if(!obj) return -1;
    size_t obj_count = json_object_get_count(obj);
    int i = 0;
    int ret = -1;
    for(i = 0 ; i < obj_count ; i++) {
        const char *name = json_object_get_name(obj, i);
        if(name){
            JSON_Object *sub_obj = json_object_get_object(obj, name);
            if(sub_obj){//recursive
                ret = noly_json_obj_get_obj(sub_obj, key, val, len);
                if(ret == 0) break;
            }else{
                if(strcmp(key, name) == 0){
                    const char *str = json_object_get_string(obj, name);
                    if(!str){
                        double num = json_object_get_number(obj, name);
                        snprintf(val, len ,"%0.0lf\n", num);
                    }else{
                        snprintf(val, len ,"%s", str);
                    }
                    return 0;
                }
            }
        }else{
            printf("json object no name???\n");
        }
    }
    return ret;
}

JSON_Array *noly_json_obj_get_array(JSON_Object *obj, char *key)
{
    if(!obj) return NULL;
    size_t obj_count = json_object_get_count(obj);
    int i = 0;
    JSON_Array *res = NULL;
    for(i = 0 ; i < obj_count ; i++) {
        const char *name = json_object_get_name(obj, i);
        if(name){
            JSON_Object *sub_obj = json_object_get_object(obj, name);
            if(sub_obj){//recursive
                res = noly_json_obj_get_array(sub_obj, key);
                if(res != NULL) {
                    break;
                }
            }else{
                if(strncmp(name, key, strlen(key)) == 0){
                    res =  json_object_get_array(obj, name);
                    return res;
                }
            }
        }else{
            printf("json object no name???\n");
        }
    }
    return res;
}

struct noly_json_array *noly_json_str_get_array(char *str, char *key)
{
    JSON_Value *js_val = NULL;
    JSON_Object *js_obj;
    js_val = json_parse_string(str);
    if(json_value_get_type(js_val) == JSONObject){
        js_obj = json_value_get_object(js_val);
        JSON_Array *ary = noly_json_obj_get_array(js_obj, key);
        if(ary){
            struct noly_json_array *res = malloc(sizeof(struct noly_json_array));
            if(res){
                res->js_val = js_val;
                res->array = ary;
                res->count = 0;
                res->size = json_array_get_count(ary);
                return res;
            }
        }
    }
    return NULL;
}

struct noly_json_obj *noly_json_array_get_obj(struct noly_json_array *array, int index)
{
    struct noly_json_obj *obj = malloc(sizeof(struct noly_json_obj));
    if(obj){
        obj->obj = json_array_get_object(array->array, index);
        if(obj->obj){
            obj->size = json_object_get_count(obj->obj);
        }else{
            free(obj);
            obj = NULL;
        }
    }
    return obj;   
}

int noly_json_array_get_str(struct noly_json_array *array, int index, char **str, int *len)
{
    const char *val = json_array_get_string(array->array, index);
    if(val){
        *str = malloc(strlen(val) + 1);
        strcpy(*str, val);
        *len = strlen(val);
    }
    return 0;
}

int json_array_release(struct noly_json_array *array)
{
    if(array){
        json_value_free(array->js_val);
        free(array);
    }
    return 0;
}


int noly_json_str_get_str(const char *str, char *key, char *val, int len)
{
    int ret = -1;
    if(!key || !val) return -1;
    JSON_Value *js_val = NULL;
    JSON_Object *js_obj;
    js_val = json_parse_string(str);
    if(json_value_get_type(js_val) == JSONObject){
        js_obj = json_value_get_object(js_val);
        ret = noly_json_obj_get_obj(js_obj, key, val, len);
    }
    json_value_free(js_val);
    return ret;
}

void test()
{
    char val[128];
    char tmp[] = "{\"serial\":450024072,\"src\":\"700000165\",\"type\":5,\"version\":\"1.0\"}";
    noly_json_str_get_str(tmp, "serial", val, 128);
    printf("result : %s\n", val);
}
/*
 * @name    bee_tlv_parser
 * @brief   tlv parser
 * @param   input   input tlv data
 * @param   type    type return address
 * @param   value   value return address
 * @param   len     length return address
 * @retval  >0 TLV offset
 * @retval  =0 TLV end
 * @retval  <0 error
 */
int bee_tlv_parser(void *input, unsigned long *type, void **value, unsigned long *len)
{
    unsigned long data_len = 0;
    unsigned char *start = input;
    if(!start || !type ||!value || !len) return -1;
    *type = (start[0]<<8) + start[1];
    data_len = (start[2]<<24) + (start[3]<<16) + (start[4]<<8) + start[5];
    *value = start + BEE_TLV_OFFSET;
    *len = data_len;
    return BEE_TLV_OFFSET + data_len;
}

/*
 * @name    bee_tlv_creator
 * @brief   tlv creator
 * @param   type    tlv type
 * @param   len     tlv length
 * @param   value   tlv value
 * @param   output  output buffer address
 * @retval  <0  error
 * @retval  >0  output buffer length
 */
int bee_tlv_creator(unsigned long type, unsigned long len, void *value, void **output)
{
    if(!value || !output) return -1;
    *output = malloc(len + BEE_TLV_OFFSET);
    unsigned char *ptr = *output;
    if(*output){
        ptr[0] = type>>8;
        ptr[1] = type & 0xff;
        ptr[2] = (len >> 24)& 0xff;
        ptr[3] = (len >> 16)& 0xff;
        ptr[4] = (len >> 8)& 0xff;
        ptr[5] = len & 0xff;
        ptr = *output + BEE_TLV_OFFSET;
        memcpy(ptr, value, len);
        return BEE_TLV_OFFSET + len;
    }
    return -1;
}


int bee_tlv_appender(unsigned long type, unsigned long len, void *value, void **output, int rlen)
{
    if(!value || !output) return -1;
    if(*output) {
        *output = realloc(*output ,rlen + len + BEE_TLV_OFFSET);
        unsigned char *ptr = *output;
        if(*output){
            ptr[rlen] = type>>8;
            ptr[rlen + 1] = type & 0xff;
            ptr[rlen + 2] = (len >> 24)& 0xff;
            ptr[rlen + 3] = (len >> 16)& 0xff;
            ptr[rlen + 4] = (len >> 8)& 0xff;
            ptr[rlen + 5] = len & 0xff;
            ptr = *output + BEE_TLV_OFFSET + rlen;
            memcpy(ptr, value, len);
            return BEE_TLV_OFFSET + len + rlen;
        }
    }else{
        *output = malloc(len + BEE_TLV_OFFSET);
        unsigned char *ptr = *output;
        if(*output){
            ptr[0] = type>>8;
            ptr[1] = type & 0xff;
            ptr[2] = (len >> 24)& 0xff;
            ptr[3] = (len >> 16)& 0xff;
            ptr[4] = (len >> 8)& 0xff;
            ptr[5] = len & 0xff;
            ptr = *output + BEE_TLV_OFFSET;
            memcpy(ptr, value, len);
            return BEE_TLV_OFFSET + len;
        }
    }
    return -1;
}

uint16_t bee_pkt_csum(const void *buff, size_t len)
{
    const uint16_t *buf=buff;
    uint32_t sum;

    // Calculate the sum
    sum = 0;
    while (len > 1)
    {
        sum += *buf++;
        if (sum & 0x80000000)
            sum = (sum & 0xFFFF) + (sum >> 16);
            len -= 2;
    }
    // Add the padding if the packet lenght is odd
    if ( len & 1 ) sum += *((uint8_t *)buf);
    // Add the carries
    while (sum >> 16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    // Return the one's complement of sum
    return ( (uint16_t)(~sum)  );
}
