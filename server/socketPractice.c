#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


struct addrinfo{
    int ai_flags;
    int ai_family;//force IPv4 or IPV6 or AF_UNSPEC to choose whatever
    int ai_socktype;//SOCK_STREAM, SOCK_DGRAM
    int ai_protocol;
    size_t *ai_addrlen;
    struct sockaddr *ai_addr;//struct sockaddr_in or _in6
    char *ai_canonname;//full canonical hostname
    struct addrinfo *ai_next; // linked list, next node
};

//the funtion returns pointer to linked list/next node
int getaddrinfo( const char *node, //wwww.google.com or ip
                const char *service, //http or port number ..typically in /etc/services
                const struct addrinfo *hints,
                struct addrinfo **res);


int main(){
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof hints); //make sure it is empty
    hints.ai_family=AF_UNSPEC; //doesn't matter if it is ipv4 or ipv6
    hints.ai_socktype=SOCK_STREAM;//tcp
    hints.ai_flags=AI_PASSIVE;//fill ip automatically

    if((status=getaddrinfo("127.0.0.1","9000",&hints,&servinfo))!=0){
        //syslog
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }


    freeaddrinfo(servinfo);
}


struct addrinfo{
    int ai_flags;
    int ai_family;//force IPv4 or IPV6 or AF_UNSPEC to choose whatever
    int ai_socktype;//SOCK_STREAM, SOCK_DGRAM
    int ai_protocol;
    size_t *ai_addrlen;
    struct sockaddr *ai_addr;//struct sockaddr_in or _in6
    char *ai_canonname;//full canonical hostname
    struct addrinfo *ai_next; // linked list, next node
};

struct sockaddr{
    unsigned short sa_family; //address family, AF_NET for IPV4 or AF_NET6
    char sa_data[14]; //14 byte protocol address...contains destination addr and port no.
};

// (IPv4 only--see struct sockaddr_in6 for IPv6)
//alternative to sock_addr
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};

struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};
// (IPv6 only--see struct sockaddr_in and struct in_addr for IPv4)

struct sockaddr_in6 {
    u_int16_t       sin6_family;   // address family, AF_INET6
    u_int16_t       sin6_port;     // port number, Network Byte Order
    u_int32_t       sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // IPv6 address
    u_int32_t       sin6_scope_id; // Scope ID
};

struct in6_addr {
    unsigned char   s6_addr[16];   // IPv6 address
};

//this is a struct large enough to hold both IPV6/4
struct sockaddr_storage {
    sa_family_t  ss_family;     // address family

    // all this is padding, implementation specific, ignore it:
    char      __ss_pad1[_SS_PAD1SIZE];
    int64_t   __ss_align;
    char      __ss_pad2[_SS_PAD2SIZE];
};

//inet_pton() "presentation to network"
//converts ips from dots to ipv4/6 addresses

//struct sockaddr_in sa; // IPv4
//struct sockaddr_in6 sa6; // IPv6

//inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); // IPv4
//inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr)); // IPv6

//returns -1 incase address is wrong

//to convert from binary address to dotted notation use ntop() "network to presentation"
// IPv4:

//char ip4[INET_ADDRSTRLEN];  // space to hold the IPv4 string
//struct sockaddr_in sa;      // pretend this is loaded with something

//inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);

//printf("The IPv4 address is: %s\n", ip4);

// IPv6:

//char ip6[INET6_ADDRSTRLEN]; // space to hold the IPv6 string
//struct sockaddr_in6 sa6;    // pretend this is loaded with something

//inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);

//printf("The address is: %s\n", ip6);

//for private Networks the NAT "Network aaddress translation" is used to hide the ip to the real world
